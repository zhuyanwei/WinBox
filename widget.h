#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QImage>
#include <QTime>
#include <QDebug>
#include <winsock2.h>
#include <QtNetwork/QUdpSocket>
#include <QtNetwork/QNetworkInterface>
#include "ThreadCamera.h"
#include "Video.h"

//#pragma comment(lib,"ws2_32.dll")

namespace Ui {
class Widget;
}

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

private:
    bool checkError(int rtpErr);

    Ui::Widget *ui;
    ThreadCamera *TC;
    Video *vid;

    bool isStart;
    QTimer *timerCam;
    QImage *img;
    QImage imgLocal;
    QImage imgRemote;

    RTPSession session;
    RTPSessionParams sessionPara;
    RTPUDPv4TransmissionParams transPara;
    int status;

    int port;
    QUdpSocket *udpSocket;
    uint32_t m_ip;
    uint16_t m_port;

    uint32_t ipList[3];
    uint16_t portList[3];
    uint32_t ssrcList[3];
};

#endif // WIDGET_H
