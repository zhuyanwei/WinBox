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
    uint16_t baseport = 102;
    transPara.SetPortbase(baseport);
    portList[0] = baseport;
//    sessionPara.SetMulticastTTL(255);
    status = session.Create(sessionPara,&transPara);
    checkError(status);
    session.SetDefaultPayloadType(H264);
    session.SetDefaultMark(false);
    session.SetDefaultTimestampIncrement(90000.0 /FRAMERATE);
    //add des ip and port
    std::string ipStr = "127.0.0.1";
    uint32_t destIp = inet_addr(ipStr.c_str());
    destIp = ntohl(destIp);
    uint16_t desPort = 102;
    RTPIPv4Address addr(destIp,desPort);
    status = session.AddDestination(addr);
    checkError(status);
    int i = 1;
    i = 2;

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

    RTPSession sess;
    uint16_t portbase,destport;
    uint32_t destip;
    std::string ipstr;
    int status,i,num;

    portbase = 98;
    ipstr = "127.0.0.1";
    destip = inet_addr(ipstr.c_str());
    if (destip == INADDR_NONE)
    {
        qDebug()<< "Bad IP address specified" ;
    }
    destip = ntohl(destip);
    destport = 98;
    num = 10;

    RTPUDPv4TransmissionParams transparams;
    RTPSessionParams sessparams;

    sessparams.SetOwnTimestampUnit(1.0/10.0);
    sessparams.SetAcceptOwnPackets(true);
    transparams.SetPortbase(portbase);
    status = sess.Create(sessparams,&transparams);
    checkError(status);

    RTPIPv4Address addr(destip,destport);

    status = sess.AddDestination(addr);
    checkError(status);

    for (i = 1 ; i <= num ; i++)
    {
        qDebug("\nSending packet %d/%d\n",i,num);
        status = sess.SendPacket((void *)"1234567890",10,0,false,10);
        checkError(status);

        sess.BeginDataAccess();

        if (sess.GotoFirstSourceWithData())
        {
            do
            {
                RTPPacket *pack;

                while ((pack = sess.GetNextPacket()) != NULL)
                {
                    // You can examine the data here
                    qDebug()<<"Got packet !";
                    sess.DeletePacket(pack);
                }
            } while (sess.GotoNextSourceWithData());
        }
        sess.EndDataAccess();
#ifndef RTP_SUPPORT_THREAD
        status = sess.Poll();
        checkError(status);
#endif // RTP_SUPPORT_THREAD
        RTPTime::Wait(RTPTime(1,0));
    }
    sess.BYEDestroy(RTPTime(10,0),0,0);

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
