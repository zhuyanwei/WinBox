#include "Video.h"

Video::Video(RTPSession *session)
{
    rtpSess = session;
    timer   = new QTimer(this);
    rtpReR =new RtpReceive();
    rtpReT =new RtpReceive();
    deR =new Decode();
    deT =new Decode();
//    ad = *audecode;
    receiveBytes = 0;
    totalSizeR = 0;
    totalSizeT = 0;
    marker = false;

    ret=deR->decodeOpen();
    ret=deR->convertOpen2();
    ret=deT->decodeOpen();
    ret=deT->convertOpen2();
    recvBufR = (char *) malloc(MAXDATASIZE);
    recvBufT = (char *) malloc(MAXDATASIZE);
    recvBufa = (char *) malloc(MAXDATASIZE);

    connect(timer, SIGNAL(timeout()), this, SLOT(readingFrame()));
    timer->start(1);
    updateR = false;
    updateT = false;
}

Video::~Video()
{
    deR->decodeClose();
    rtpReR->netClose();
    deT->decodeClose();
    rtpReT->netClose();
    free(recvBufR);
    free(recvBufT);
}

void checkError( int errorCode )
{
    if (errorCode < 0)
    {
        qDebug()<<"!!!!!!This is rtp error---"<<errorCode;
        QString yui = QString::fromStdString(RTPGetErrorString(errorCode));
        qDebug()<<yui;
    }
    else
    {
        qDebug()<<"no rtp error";
    }
}

//void Video::readingFrame(void *buf,int len)
//{
//    qDebug()<<"readingFrame start";
//    deR->decodeDo(buf,len,&disBufR);
//    emit decodeDone();
//}

void Video::readingFrame()
{
    rtpSess->BeginDataAccess();
    if( rtpSess->GotoFirstSourceWithData() )
    {
        do
        {
            while( ( rtpPack = rtpSess->GetNextPacket() ) != NULL )
            {
                if (rtpPack->GetPayloadType() == AAC)
                {
                   receiveBytes = rtpPack->GetPayloadLength();
                   recvBufa = (char *)rtpPack->GetPayloadData();
//                   ad->decode_do(recv_bufa,receive_bytes);
                }
                else if (rtpPack->GetPayloadType() == H264)
                {
                    uint32_t packssrc = rtpPack->GetSSRC();
                    int number=0;
                    if (packssrc == ssrc[0]) number = 1;
                    else if(packssrc == ssrc[1]) number = 2;
                    switch (number)
                    {
                        case 1:
                        {
                            receiveBytes = rtpPack->GetPayloadLength();
                            totalSizeR += receiveBytes;
                            recvBufR = (char *)rtpPack->GetPayloadData();
                            marker = rtpPack->HasMarker();
                            if(rtpReR->rtpUnpackage(recvBufR,receiveBytes,marker,&deBufR,&deLen) == 1)
                            {
                                if (deR->decodeDo(deBufR,deLen,&disBufR) ==1)
                                {
                                    updateR = true;
                                    emit getFrame();
                                }
                            }
                            break;
                        }
                        case 2:
                        {
                            receiveBytes = rtpPack->GetPayloadLength();
                            totalSizeT += receiveBytes;
                            recvBufT = (char *)rtpPack->GetPayloadData();
                            marker = rtpPack->HasMarker();
                            if(rtpReT->rtpUnpackage(recvBufT,receiveBytes,marker,&deBufT,&deLen) == 1)
                            {
                                if (deT->decodeDo(deBufT,deLen,&disBufT) ==1)
                                {
                                    updateT = true;
                                    emit getFrame();
                                }
                            }
                            break;
                        }
                    }
                }
            rtpSess->DeletePacket( rtpPack );
            }
        }
        while( rtpSess->GotoNextSourceWithData() );
    }
    rtpSess->EndDataAccess();
}

void Video::showRemoteWindow()
{

}
