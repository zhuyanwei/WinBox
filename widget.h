#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QImage>
#include <QTime>
#include <QString>
#include <QSettings>
#include <QDebug>
#include <QMessageBox>
#include <winsock2.h>
#include <QHostInfo>
#include <QUdpSocket>
#include <QNetworkInterface>
#include <unistd.h>
#include "ThreadCamera.h"
#include "ThreadMic.h"
#include "EncodeAU.h"
#include "DecodeAU.h"
#include "Show.h"

namespace Ui {
class Widget;
}

enum MessageType{Request,Callback,Callback2,Invite,CutIn,CutInCB,End,CallbackEnd};

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
    void on_B_Connect();
    void on_B_Invite();
    void on_B_Add();
    void on_B_Test();
    int slotTest();
    void showLocalPic();
    int proAudio();
    void proRequest();

private:
    bool checkError(int rtpErr);
    void addDest(uint32_t dest_ip,uint16_t dest_port);
    void sendMessage(MessageType type,char* destip);
    void end();
    void begin();

    Ui::Widget *ui;
    ThreadCamera *TC;
    ThreadMic *TM;
    Show *sh;
    MicGet *mg;
    EncodeAU *ea;
    DecodeAU *da;
    void *aeBuf;
    int aeSize;
    QTimer *timer;

    bool isStart,isStart2,isServer;
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
    QUdpSocket *udpS;
    QUdpSocket *udpR;
    int portS;
    QHostAddress ipS;
    int portR;
    uint32_t m_ip;
    uint16_t m_port;
    uint32_t ipList[3];
    uint16_t portList[3];
    uint32_t ssrcList[3];
};

#endif // WIDGET_H
