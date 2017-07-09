#include "RtpReceive.h"

RtpReceive::RtpReceive()
{

}

RtpReceive::~RtpReceive()
{
//    free(this);
    printf("RtpReceive destruct");
}

void RtpReceive::freeNALU(NALU_t *n)
{
    if(n)
    {
        free (n);
        n = NULL;
    }
}

int RtpReceive::initiateWinsock(RTPSession *sess)
{
    rtpSess = sess;
    return 1;
}

int RtpReceive::rtpUnpackage(char *bufIn, int len, bool marker, void **bufOut, int *olen)
{
    uchar recvbuf[MAXDATASIZE];
    NALU_HEADER  * nalu_hdr = NULL;
    FU_INDICATOR * fu_ind   = NULL;
    FU_HEADER	 * fu_hdr   = NULL;
    NALU_t * n  = NULL;
    static char encodebuf[WIDTH*HEIGHT*3];
    static int pencode_number = 0;
    unsigned char F_N;
    unsigned char NRI_N;
    unsigned char TYPE_N;
    int NALFinished = 0;
    *olen=0;
    memcpy(recvbuf,bufIn, len);
    if ((nalu_hdr = (NALU_HEADER *)malloc(sizeof(NALU_HEADER))) == NULL)
    {
        qDebug()<<"NALU_HEADER MEMORY ERROR";
    }
    //begin nal_hdr
    if ((n = (NALU_t*)calloc (1, sizeof(NALU_t))) == NULL)
    {
        qDebug()<<"AllocNALU Error: Allocate Meory To NALU_t Failed ";
    }
    memcpy(nalu_hdr, &recvbuf[0], sizeof(NALU_HEADER));
    n->forbidden_bit= nalu_hdr->F << 7;
    n->nal_reference_idc = nalu_hdr->NRI << 5;
    n->nal_unit_type = nalu_hdr->TYPE;
    //end nal_hdr
    if ( nalu_hdr->TYPE  == 0)
    {
        qDebug()<<"no definition";
    }
    else if ( nalu_hdr->TYPE >0 &&  nalu_hdr->TYPE < 24)
    {
        encodebuf[0] = 0x00;
        encodebuf[1] = 0x00;
        encodebuf[2] = 0x00;
        encodebuf[3] = 0x01;
        F_N = nalu_hdr->F << 7;
        NRI_N = nalu_hdr->NRI << 5;
        TYPE_N = nalu_hdr->TYPE;
        encodebuf[4] = F_N | NRI_N |TYPE_N;
        memcpy(encodebuf + 5 ,&recvbuf[1],len-1);
        *olen = len + 4;
        *bufOut = encodebuf;
        if (nalu_hdr->TYPE==7 || nalu_hdr->TYPE ==8)
        {
            NALFinished = 2;
        }
        else NALFinished = 1;
    }
    else if ( nalu_hdr->TYPE == 28)
    {
        if ((fu_ind = (FU_INDICATOR *)malloc(sizeof(FU_INDICATOR))) == NULL)
        {
            qDebug()<<"FU_INDICATOR MEMORY ERROR";
        }
        if ((fu_hdr = (FU_HEADER *)malloc(sizeof(FU_HEADER))) == NULL)
        {
            qDebug()<<"FU_HEADER MEMORY ERROR";
        }
        memcpy(fu_ind, &recvbuf[0], sizeof(FU_INDICATOR));
        n->forbidden_bit = fu_ind->F << 7;
        n->nal_reference_idc = fu_ind->NRI << 5;
        n->nal_unit_type = fu_ind->TYPE;
        memcpy(fu_hdr, &recvbuf[1], sizeof(FU_HEADER));
        n->nal_unit_type = fu_hdr->TYPE;
        if (marker == 1)
        {
            memcpy(encodebuf + pencode_number ,&recvbuf[2],len - 2);
            *olen = pencode_number + len - 2;
            *bufOut = encodebuf;
            pencode_number = 0;
            NALFinished = 1;
        }
        else if (marker == 0)
        {
            if (fu_hdr->S == 1)
            {
                unsigned char F;
                unsigned char NRI;
                unsigned char TYPE;
                unsigned char nh;
                F = fu_ind->F << 7;
                NRI = fu_ind->NRI << 5;
                TYPE = fu_hdr->TYPE;
                nh = F | NRI | TYPE;
                encodebuf[0] = 0x00;
                encodebuf[1] = 0x00;
                encodebuf[2] = 0x00;
                encodebuf[3] = 0x01;
                encodebuf[4] = nh;
                memcpy(encodebuf + 5 ,&recvbuf[2],len - 2);
                pencode_number +=  (5 + len - 2);
                *olen = pencode_number;
                *bufOut = encodebuf;
            }
            else
            {
                memcpy(encodebuf + pencode_number ,&recvbuf[2],len - 2);
                pencode_number += len - 2;
                *olen = pencode_number;
                *bufOut = encodebuf;
            }
            NALFinished = 0;
        }
    }
    else
    {
        qDebug()<<"this pack is wrong";
    }
    memset(recvbuf,0,MAXDATASIZE);
    if (nalu_hdr)
    {
        free(nalu_hdr);
        nalu_hdr = NULL;
    }
    if (fu_ind)
    {
        free(fu_ind);
        fu_ind = NULL;
    }
    if (fu_hdr)
    {
        free(fu_hdr);
        fu_hdr = NULL;
    }
    freeNALU(n);
    if (NALFinished == 0)
        return 0;
    else if (NALFinished == 1)
        return 1;
    else if (NALFinished == 2)
        return 1;
    else return -1;
}

void RtpReceive::netClose()
{
//    free(this);
    qDebug()<<"Network Closed";
}

void RtpReceive::checkError( int errCode )
{

}

