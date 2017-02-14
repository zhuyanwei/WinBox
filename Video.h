#ifndef VIDEO_H
#define VIDEO_H

#include <QObject>
#include <QDebug>
#include <QTimer>
#include "RtpReceive.h"
#include "Decode.h"

namespace Ui {
class Video;
}

class Video : public QObject
{
    Q_OBJECT
public:
    Video(RTPSession *session);
    ~Video();

    uint32_t ssrc[2];
    void *deBufR,*deBufT,*disBufR,*disBufT;

private:
    void checkError( int errorCode );

    int ret;
    bool updateR,updateT;
    QTimer    *timer;
    RtpReceive *rtpReR;
    RtpReceive *rtpReT;
    Decode *deR;
    Decode *deT;
//    au_decode *ad;
    int receiveBytes,deLen,totalSizeR,totalSizeT;
    char *recvBufR,*recvBufT,*recvBufa;
    bool marker;
    RTPSession *rtpSess;
    RTPPacket *rtpPack;
    int status, timeout;

signals:
    getFrame();
    decodeDone();

public slots:
    void readingFrame();

private slots:
    void showRemoteWindow();
};

#endif // VIDEO_H
