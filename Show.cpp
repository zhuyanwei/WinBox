#include "Show.h"
#include "ui_Show.h"

Show::Show(QWidget *parent ,RTPSession *session,DecodeAU **audecode ) :
    QDialog(parent),
    ui(new Ui::Show)
{
    ui->setupUi(this);

    rtpSess = session;
    timer   = new QTimer(this);
    rtpReR =new RtpReceive();
    rtpReT =new RtpReceive();
    deR =new Decode();
    deT =new Decode();
    da = *audecode;
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

    connect(this, SIGNAL(getFrame()), this, SLOT(showAllWindow()));
    connect(timer, SIGNAL(timeout()), this, SLOT(readingFrame()));
    timer->start(1);
    updateR = false;
    updateT = false;
}

Show::~Show()
{
    deR->decodeClose();
    rtpReR->netClose();
    deT->decodeClose();
    rtpReT->netClose();
    free(recvBufR);
    free(recvBufT);
    delete ui;
}

void Show::checkError( int errorCode )
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

void Show::readingFrame()
{
#ifndef RTP_SUPPORT_THREAD
    status = rtpSess->Poll();
#endif
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
                   da->decodeAUDo(recvBufa,receiveBytes);
                }
                else if (rtpPack->GetPayloadType() == H264)
                {
                    uint32_t packssrc = rtpPack->GetSSRC();
                    int number= 1;
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

void Show::showAllWindow()
{
    if (updateR)
    {
        imageR = QImage((const uchar*)disBufR,WIDTH,HEIGHT, QImage::Format_RGB888).rgbSwapped();
        imageR = imageR.mirrored(true,false);
        ui->L_RemoteWindow->setPixmap(QPixmap::fromImage(imageR));
        updateR = false;
    }
    if (updateT)
    {
        imageT = QImage((const uchar*)disBufT,WIDTH,HEIGHT, QImage::Format_RGB888).rgbSwapped();
        imageT = imageT.mirrored(true,false);
        ui->L_ThirdWindow->setPixmap(QPixmap::fromImage(imageT));
        updateT = false;
    }
}
