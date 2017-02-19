#ifndef SHOW_H
#define SHOW_H

#include <QObject>
#include <QDebug>
#include <QTimer>
#include <QDialog>
#include "RtpReceive.h"
#include "Decode.h"
#include "DecodeAU.h"

namespace Ui {
class Show;
}

class Show : public QDialog
{
    Q_OBJECT

public:
    explicit Show(QWidget *parent = 0,RTPSession *session = 0,DecodeAU **audecode = 0);
    ~Show();

    uint32_t ssrc[2];
    void *deBufR,*deBufT,*disBufR,*disBufT;

private:
    Ui::Show *ui;

    void checkError( int errorCode );

    int ret;
    bool updateR,updateT;
    QTimer    *timer;
    RtpReceive *rtpReR;
    RtpReceive *rtpReT;
    Decode *deR;
    Decode *deT;
    DecodeAU *da;
    int receiveBytes,deLen,totalSizeR,totalSizeT;
    char *recvBufR,*recvBufT,*recvBufa;
    bool marker;
    RTPSession *rtpSess;
    RTPPacket *rtpPack;
    int status, timeout;
    QImage imageR,imageT;

signals:
    getFrame();

public slots:
    void readingFrame();

private slots:
    void showAllWindow();
};

#endif // SHOW_H
