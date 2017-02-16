#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QImage>
#include <QTime>
#include <QDebug>
#include <winsock2.h>
#include <QtNetwork/QHostInfo>
#include <QtNetwork/QUdpSocket>
#include <QtNetwork/QNetworkInterface>
#include "ThreadCamera.h"
#include "ThreadMic.h"
#include "EncodeAU.h"
#include "DecodeAU.h"
#include "Video.h"

namespace Ui {
class Widget;
}

enum MessageType{Request,Callback,Callback2,Invite,CutIn,CutInCB};

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = 0);
    ~Widget();

private slots:
    void on_B_OpenCam();
    void on_B_CloseCam();
    void on_B_Pause();
    void on_B_Initial();
    void on_B_Test();
    void showLocalPic();
    void showRemotePic();
    int proAudio();
    void procRequest();

private:
    bool checkError(int rtpErr);
    void addDest(uint32_t dest_ip,uint16_t dest_port);
    void sendMessage(MessageType type,char* destip);

    Ui::Widget *ui;
    ThreadCamera *TC;
    ThreadMic *TM;
    Video *vid;
    MicGet *mg;
    EncodeAU *ea;
    DecodeAU *da;
    void *aeBuf;
    int aeSize;
    QTimer *timer;

    bool isStart,isStart2;
    QTimer *timerCam;
    QImage *img;
    QImage imgLocal;
    QImage imgRemote;

    RTPSession session;
    RTPSessionParams sessionPara;
    RTPUDPv4TransmissionParams transPara;
    int status;

    uint32_t QCIp;
    uint16_t QCPort;
    int port;
    QUdpSocket *udpSocket;
    uint32_t m_ip;
    uint16_t m_port;
    uint32_t ipList[3];
    uint16_t portList[3];
    uint32_t ssrcList[3];
};

#endif // WIDGET_H
