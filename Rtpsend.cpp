#include "RtpSend.h"

RtpSend::RtpSend()
{
    timestampIncrese=0;
    tsCurrent=0;
}

RtpSend::~RtpSend()
{
//    free(this);
    qDebug("RtpSend destruct");
}

int RtpSend::netOpen(RTPSession *sess)
{
    session = sess;
    qDebug()<<"Network Opened";
    return 0;
}

void RtpSend::netClose()
{
//    free(this);
    qDebug()<<"Network Closed";
}

int RtpSend::packOpen(int maxPktLen,int ssrc)
{
    this->FUIndex = 0;
    this->nalu.data = NULL;
    this->seqNum = 0;
    this->tsCurrentSample = 0;
    this->maxPktLen = maxPktLen;
    this->ssrc = ssrc;
    this->tsStartMillisec = getCurrentMillisec();
    qDebug()<<"Pack Opened";
    return 0;
}

void RtpSend::packPut(void *inBuf,int inSize)
{
    this->inBuf = inBuf;
    this->nextNaluPtr = (char*)this->inBuf;
    this->inBufSize = inSize;
    this->FUCounter = 0;
    this->lastFUSize = 0;
    this->FUIndex = 0;
    this->inBufComplete = 0;
    this->naluComplete = 1;
}

int RtpSend::packGet(void *outBuf, int bufSize,int *outSize)
{
    int ret;
    if (this->inBufComplete) return 0;
    memset(outBuf, 0, bufSize);
    if (this->naluComplete)    // current nalu complete, find the next nalu in inbuf
    {
        ret = getNextNalu();
        if (ret <= 0)    // no more nalus
        {
            this->inBufComplete = 1;
            return 0;
        }
        if (this->nalu.len <= this->maxPktLen)    // no need to fragment
        {
            nalu_header *nalu_hdr_;
            nalu_hdr_ =(nalu_header*)&sendBuf[0];
            nalu_hdr_->F = this->nalu.forbidden_bit;
            nalu_hdr_->NRI = this->nalu.nal_reference_idc;
            nalu_hdr_->TYPE = this->nalu.nal_unit_type;
            *outSize = this->nalu.len + 12;
            if (bufSize < *outSize)    // check size
            {
                qDebug() << bufSize << *outSize;
                qDebug()<<"calling abort";
                abort();
            }
            naluPayload=&sendBuf[1];
            memcpy(naluPayload, this->nalu.data + 1, this->nalu.len - 1);    // exclude the nalu header
            if(nalu_hdr_->TYPE==1 || nalu_hdr_->TYPE==5)
            {
                status = session->SendPacket((void *)sendBuf,this->nalu.len,H264,true,90000/FRAMERATE);
            }
            else
            {
                status = session->SendPacket((void *)sendBuf,this->nalu.len,H264,true,0);
            }
            if (status < 0)
            {
                qDebug()<<"error single NALU";
//                std::cerr << RTPGetErrorString(status) << std::endl;
//                exit(-1);
            }
            this->naluComplete = 1;
            return 1;
        }
        else    // fragment needed
        {
            if (this->nalu.len % this->maxPktLen == 0)    // in case divide exactly
            {
                this->FUCounter = this->nalu.len / this->maxPktLen - 1;
                this->lastFUSize = this->maxPktLen;
            }
            else
            {
                this->FUCounter = this->nalu.len / this->maxPktLen;
                this->lastFUSize = this->nalu.len % this->maxPktLen;
            }
            this->FUIndex = 0;
            fu_ind =(fu_indicator*)&sendBuf[0];
            fu_ind->F = this->nalu.forbidden_bit;
            fu_ind->NRI = this->nalu.nal_reference_idc;
            fu_ind->TYPE = 28;    // FU_A
            fu_hdr =(fu_header*)&sendBuf[1];
            fu_hdr->E = 0;
            fu_hdr->R = 0;
            fu_hdr->S = 1;    // start bit
            fu_hdr->TYPE = this->nalu.nal_unit_type;
            *outSize = this->maxPktLen + 14;    // RTP header + FU indicator + FU header
            if (bufSize < *outSize)
            {
                qDebug()<<"bufSize<outSize,calling abort";
                abort();
            }
            naluPayload=&sendBuf[2];
            memcpy(naluPayload, this->nalu.data + 1,this->maxPktLen);
            status = session->SendPacket((void *)sendBuf,this->maxPktLen + 2,H264,false,0);
            if (status < 0)
            {
                qDebug()<<"error first FUs";
//                exit(-1);
            }
            this->naluComplete = 0;    // not complete
            this->FUIndex++;
            return 1;
        }
    }
    else    // send remaining FUs
    {
        if (this->FUIndex == this->FUCounter)    // the last FU
        {
            fu_ind =(fu_indicator*)&sendBuf[0];
            fu_ind->F = this->nalu.forbidden_bit;
            fu_ind->NRI = this->nalu.nal_reference_idc;
            fu_ind->TYPE = 28;
            fu_hdr =(fu_header*)&sendBuf[1];
            fu_hdr->R = 0;
            fu_hdr->S = 0;
            fu_hdr->TYPE = this->nalu.nal_unit_type;
            fu_hdr->E = 1;    // the last EU
            *outSize = this->lastFUSize - 1 + 14;
            if (bufSize < *outSize)
            {
                qDebug()<<"bufSize<outSize,calling abort,,,155";
                abort();
            }
            naluPayload=&sendBuf[2];
            memcpy(naluPayload,this->nalu.data + 1 + this->FUIndex * this->maxPktLen,this->lastFUSize - 1);    // minus the nalu header
            status = session->SendPacket((void *)sendBuf,this->lastFUSize+1,H264,true,90000/FRAMERATE);
            if (status < 0)
            {
              qDebug()<<"error last FUs,,,163";
//              exit(-1);
            }
            this->naluComplete = 1;    // this nalu is complete
            this->FUIndex = 0;
            return 1;
        }
        else    // middle FUs
        {
            fu_ind =(fu_indicator*)&sendBuf[0];
            fu_ind->F = this->nalu.forbidden_bit;
            fu_ind->NRI = this->nalu.nal_reference_idc;
            fu_ind->TYPE = 28;
            fu_hdr =(fu_header*)&sendBuf[1];
            fu_hdr->R = 0;
            fu_hdr->S = 0;
            fu_hdr->TYPE = this->nalu.nal_unit_type;
            fu_hdr->E = 0;
            *outSize = this->maxPktLen + 14;
            if (bufSize < *outSize)
            {
                qDebug()<<"bufSize<outSize,calling abort,,,184";
                abort();
            }
            naluPayload=&sendBuf[2];
            memcpy(naluPayload, this->nalu.data + 1 + this->FUIndex * this->maxPktLen,this->maxPktLen);
            status = session->SendPacket((void *)sendBuf,this->maxPktLen+2,H264,false,0);
            if (status < 0)
            {
                qDebug()<<"error middle FUs,,,192";
//                exit(-1);
            }
            this->FUIndex++;
            return 1;
        }
    }
}

U32 RtpSend::getCurrentMillisec(void)
{
    struct timeb tb;
    ftime(&tb);
    return 1000 * tb.time + tb.millitm;
}

int RtpSend::isStartCode4(char *buf)
{
    if (buf[0] != 0 || buf[1] != 0 || buf[2] != 0 || buf[3] != 1)    // 0x00000001
        return 0;
    else
        return 1;
}

int RtpSend::isStartCode3(char *buf)
{
    if (buf[0] != 0 || buf[1] != 0 || buf[2] != 1)    // 0x000001
        return 0;
    else
        return 1;
}

int RtpSend::getNextNalu()
{
    if (!this->nextNaluPtr)    // reach data end, no next nalu
    {
        return 0;
    }
    if (isStartCode3(this->nextNaluPtr))    // check 0x000001 first
    {
        this->nalu.startcodeprefix_len = 3;
    }
    else
    {
        if (isStartCode4(this->nextNaluPtr))    // check 0x00000001
        {
            this->nalu.startcodeprefix_len = 4;
        }
        else
        {
            qDebug()<<"!!! No startcode found";
            return -1;
        }
    }
    // find the next start code
    int startcode_found = 0;
    char *cur_nalu_ptr = this->nextNaluPtr;    // rotate, save the next ptr
    char *next_ptr = cur_nalu_ptr + this->nalu.startcodeprefix_len;    // skip current start code
    while (!startcode_found)
    {
        next_ptr++;
        if ((next_ptr - (char *) this->inBuf) >= this->inBufSize)    // reach data end
        {
            this->nextNaluPtr = NULL;    // no more nalus
            break;
        }
        if (isStartCode3(next_ptr) || isStartCode4(next_ptr))    // found the next start code
        {
            this->nextNaluPtr = next_ptr;    // next ptr
            break;
        }
    }
    this->nalu.data = cur_nalu_ptr + this->nalu.startcodeprefix_len;    // exclude the start code
    this->nalu.len = next_ptr - cur_nalu_ptr - this->nalu.startcodeprefix_len;
    this->nalu.forbidden_bit = (this->nalu.data[0] & 0x80) >> 7;    // 1 bit, 0b1000 0000
    this->nalu.nal_reference_idc = (this->nalu.data[0] & 0x60) >> 5;    // 2 bit, 0b0110 0000
    this->nalu.nal_unit_type = (this->nalu.data[0] & 0x1f);    // 5 bit, 0b0001 1111
    return 1;
}

void RtpSend::dumpNalu(const nalu_t *nalu)
{
    if (!nalu) return;
    qDebug()<<"nalu len:  "<< nalu->len;
    qDebug()<<"nal_unit_type:  "<< nalu->nal_unit_type;
}
