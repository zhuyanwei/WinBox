#include "widget.h"
#include "ui_widget.h"

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);

#ifdef WIN32
    WSADATA dat;
    WSAStartup(MAKEWORD(2,2),&dat);
#endif

    //connect sigmal and slot
    connect(ui->B_Initial,SIGNAL(clicked()),this,SLOT(on_B_Initial()));
    connect(ui->B_OpenCam,SIGNAL(clicked()),this,SLOT(on_B_OpenCam()));
    connect(ui->B_CloseCam,SIGNAL(clicked()),this,SLOT(on_B_CloseCam()));
    connect(ui->B_Pause,SIGNAL(clicked()),this,SLOT(on_B_Pause()));
    connect(ui->B_Test,SIGNAL(clicked()),this,SLOT(on_B_Test()));
}

Widget::~Widget()
{
#ifdef WIN32
    WSACleanup();
#endif
    if(TC)
    {
        TC->stop();
        free(TC);
    }
    delete ui;
    qDebug()<<"+++ all finished";
}

void Widget::showLocalPic()
{
    if(isStart == true)
    {
        imgLocal = QImage((const uchar*)TC->capFrame->imageData,TC->capFrame->width, TC->capFrame->height, QImage::Format_RGB888).rgbSwapped();
        imgLocal = imgLocal.mirrored(true,false);
        ui->L_LocalWindow->setPixmap(QPixmap::fromImage(imgLocal));
    }
    else
    {
        ui->L_LocalWindow->setText("LocalWindow");
    }
}

void Widget::showRemotePic()
{
    qDebug()<<"showRemotePic start";
    imgRemote = QImage((const uchar*)vid->disBufR,WIDTH,HEIGHT, QImage::Format_RGB888).rgbSwapped();
    ui->L_RemoteWindow->setPixmap(QPixmap::fromImage(imgRemote));
}

void Widget::on_B_OpenCam()
{
    qDebug()<<"B_OpenCam clicked";
//    isStart = true;
//    TC = new ThreadCamera();
//    connect(TC,SIGNAL(captured()),this,SLOT(showLocalPic()));
//    TC->start();

//    vid = new Video();
//    connect(vid,SIGNAL(decodeDone()),this,SLOT(showRemotePic()));
//    connect(TC,SIGNAL(sendDone()),vid,SLOT(readingFrame(TC->encBuf,TC->encLen)));
////    vid->readingFrame(TC->encBuf,TC->encLen);

    isStart = true;
    TC = new ThreadCamera(&session);
    vid = new Video(&session);
    connect(TC,SIGNAL(captured()),this,SLOT(showLocalPic()));
    connect(vid,SIGNAL(getFrame()),this,SLOT(showRemotePic()));
//    connect(TC,SIGNAL(sendDone(void*,int)),vid,SLOT(readingFrame(void*,int)));
    TC->start();
}

void Widget::on_B_CloseCam()
{
    qDebug()<<"B_CloseCam clicked";
    isStart = false;
    TC->stop();
    TC->sleep(2);
    TC->~ThreadCamera();
    ui->L_LocalWindow->setText("LocalWindow");
}

void Widget::on_B_Pause()
{
    qDebug()<<"B_Pause clicked";
    TC->stop();
}

void Widget::on_B_Initial()
{
    qDebug()<<"B_Initial clicked";

    sessionPara.SetOwnTimestampUnit(1.0/90000.0);
    sessionPara.SetAcceptOwnPackets(true);
    sessionPara.SetUsePredefinedSSRC(true);
    //set ssrc
//    uint32_t ssrc = ui->ssrc->text().toInt();
    uint32_t ssrc = 10;
    sessionPara.SetPredefinedSSRC(ssrc);
    ssrcList[0] = ssrc;
    sessionPara.SetMaximumPacketSize(MAXDATASIZE);
    //set baseport
//    uint16_t baseport = ui->baseport->text().toInt();
    uint16_t baseport = 8090;
    transPara.SetPortbase(baseport);
    portList[0] = baseport;
//    sessionPara.SetMulticastTTL(255);
    status = session.Create(sessionPara,&transPara);
    session.SetDefaultPayloadType(H264);
    session.SetDefaultMark(false);
    session.SetDefaultTimestampIncrement(90000.0 /FRAMERATE);
    //add des ip and port
    std::string ipStr = "127.0.0.1";
    uint32_t destIp = inet_addr(ipStr.c_str());
    destIp = ntohl(destIp);
    uint16_t desPort = 8090;
    RTPIPv4Address addr(destIp,desPort);
    status = session.AddDestination(addr);
    checkError(status);

//    udpSocket = new QUdpSocket(this);
//    port = 8080;
//    udpSocket->bind(port,QUdpSocket::ShareAddress|QUdpSocket::ReuseAddressHint);
//    QList<QHostAddress> list = QNetworkInterface::allAddresses();
//    foreach (QHostAddress address, list)
//    {
//        if(address.protocol() == QAbstractSocket::IPv4Protocol && address.toString().contains("192.168."))
//        {
//            QByteArray ba = address.toString().toLatin1();
//            ipList[0] = inet_addr(ba.data());
//        }
//    }
//    status =session.SupportsMulticasting();
//    m_ip = inet_addr("224.1.1.1");
//    m_port = 8090;
//    RTPIPv4Address m_addr(ntohl(m_ip), m_port);
//    status = session.JoinMulticastGroup(m_addr);
//    connect(udpSocket,SIGNAL(readyRead()),this,SLOT(processRequest()));
}

void Widget::on_B_Test()
{
    qDebug()<<"B_Test clicked";
//    RTPSession sess;
//    int status;
//    ui->T_Test->setText("LocalWindow");
    //send part

    uint16_t basePort,desPort;
    uint32_t destIp;
    std::string ipStr;
    int status,num;
    bool done;

    num = 10;
    basePort = 80;
    desPort = 80;
    ipStr = "127.0.0.1";
    destIp = inet_addr(ipStr.c_str());
    destIp = ntohl(destIp);

    RTPSession se;
    RTPSessionParams sp;
    sp.SetOwnTimestampUnit(1.0/10.0);
    RTPUDPv4TransmissionParams tp;
    tp.SetPortbase(basePort);
    status = se.Create(sp,&tp);
    checkError(status);
    RTPIPv4Address addr(destIp,desPort);
    status = se.AddDestination(addr);
    checkError(status);
    for(int i = 1;i <= num;i++)
    {
        status = se.SendPacket((void *)"1234567890",10,0,false,10);
        checkError(status);
        //receive part
        done = false;
        se.BeginDataAccess();
        if (se.GotoFirstSourceWithData())
        {
            do
            {
                RTPPacket *pack;
                while ((pack = se.GetNextPacket()) != NULL)
                {
                    qDebug()<<"receive";
                    qDebug()<<pack->GetSSRC();
                    ui->T_Test->setText("receive");

                    se.DeletePacket(pack);
                }
            } while (se.GotoNextSourceWithData());
        }
        se.EndDataAccess();
    }
}

bool Widget::checkError(int rtpErr)
{
    if (rtpErr < 0)
    {
        qDebug()<<"!!!!!!This is rtp error---"<<rtpErr;
        QString yui = QString::fromStdString(RTPGetErrorString(rtpErr));
        qDebug()<<yui;
        return false;
    }
    else
    {
        qDebug()<<"no rtp error";
        return true;
    }
}
