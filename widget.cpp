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
    timer = new QTimer(this);
    mg = new MicGet();
    ea = new EncodeAU();
    da = new DecodeAU();
    ea->encodeAUOpen();
    da->decodeAUOpen();
    mg->initAudio(&ea->auinFifo,&da->auoutFifo);

    //connect sigmal and slot
    connect(ui->B_Initial,SIGNAL(clicked()),this,SLOT(on_B_Initial()));
    connect(ui->B_OpenCam,SIGNAL(clicked()),this,SLOT(on_B_OpenCam()));
    connect(ui->B_CloseCam,SIGNAL(clicked()),this,SLOT(on_B_CloseCam()));
    connect(ui->B_Connect,SIGNAL(clicked()),this,SLOT(on_B_Connect()));
    connect(ui->B_Invite,SIGNAL(clicked()),this,SLOT(on_B_Invite()));
    connect(ui->B_Add,SIGNAL(clicked()),this,SLOT(on_B_Add()));

    connect(ui->B_Pause,SIGNAL(clicked()),this,SLOT(on_B_Pause()));
    connect(ui->B_Test,SIGNAL(clicked()),this,SLOT(on_B_Test()));

    connect(timer, SIGNAL(timeout()), this, SLOT(proAudio()));
    timer->start(10);

    isStart = 0;
    isStart2 = 0;
    isServer = 1;
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

void Widget::on_B_OpenCam()
{
    qDebug()<<"B_OpenCam clicked";
    TC = new ThreadCamera(&session);
    connect(TC,SIGNAL(captured()),this,SLOT(showLocalPic()));
    isStart = true;
    TC->start();

    TM = new ThreadMic(mg);
    TM->start();
//    sleep(1);

    sh = new Show(this,&session,&da);
    sh->show();
    isStart2 = true;
}

void Widget::on_B_CloseCam()
{
    qDebug()<<"B_CloseCam clicked";
    isStart = false;
    TC->stop();
    TC->sleep(2);
    TC->~ThreadCamera();
//    free(TC);

    TM->stop();
    TM->sleep(1);
    TM->~ThreadMic();
//    free(TM);

    isStart2 = false;
    sh->close();
//    free(sh);

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
    uint32_t ssrc = ui->I_SSRC->text().toInt();
//    uint32_t ssrc = 10;
    sessionPara.SetPredefinedSSRC(ssrc);
    ssrcList[0] = ssrc;
    sessionPara.SetMaximumPacketSize(MAXDATASIZE);
    //set baseport
    uint16_t baseport = ui->I_LocalPort->text().toInt();
//    uint16_t baseport = 100;
    transPara.SetPortbase(baseport);
    portList[0] = baseport;
//    sessionPara.SetMulticastTTL(255);
    status = session.Create(sessionPara,&transPara);
    session.SetDefaultPayloadType(H264);
    session.SetDefaultMark(false);
    session.SetDefaultTimestampIncrement(90000.0 /FRAMERATE);
    //add des ip and port
//    std::string ipStr = "127.0.0.1";
//    uint32_t destIp = inet_addr(ipStr.c_str());
//    destIp = ntohl(destIp);
//    uint16_t desPort = 100;
//    RTPIPv4Address addr(destIp,desPort);
//    status = session.AddDestination(addr);

    udpSocket = new QUdpSocket();
    port = 8080;
    udpSocket->bind(port,QUdpSocket::ShareAddress|QUdpSocket::ReuseAddressHint);
    QList<QHostAddress> list = QNetworkInterface::allAddresses();
    foreach (QHostAddress address, list)
    {
        if(address.protocol() == QAbstractSocket::IPv4Protocol && address.toString().contains("192.168."))
        {
            QByteArray ba = address.toString().toLatin1();
            ipList[0] = inet_addr(ba.data());
        }
    }
    status =session.SupportsMulticasting();
    m_ip = inet_addr("224.1.1.1");
    m_port = 8090;
    RTPIPv4Address m_addr(ntohl(m_ip), m_port);
    status = session.JoinMulticastGroup(m_addr);
    connect(udpSocket,SIGNAL(readyRead()),this,SLOT(proRequest()));
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

void Widget::addDest(uint32_t dest_ip,uint16_t dest_port)
{
    dest_ip = ntohl(dest_ip);
    RTPIPv4Address addr(dest_ip,dest_port);
    status = session.AddDestination(addr);
    checkError(status);
}

void Widget::sendMessage(MessageType type,char* destip)
{
    QByteArray data;
    QDataStream out(&data,QIODevice::WriteOnly);
    out<<type;
    switch(type)
    {
        case Request:
        {
            QString name = QHostInfo::localHostName();
            out<<name<<ipList[0]<<portList[0]<<ssrcList[0];
            port = 8080;
            break;
        }
        case Callback:
        {
            out<<isStart2<<ipList[0]<<portList[0]<<ssrcList[0];
            port = 8080;
            break;
        }
        case Callback2:
        {
            port = 8080;
            break;
        }
        case Invite:
        {
            out<<ipList[0]<<portList[0]<<ssrcList[0]<<ipList[1]<<portList[1]<<ssrcList[1];
            port = 8080;
            break;
        }
        case CutIn:
        {
            out<<QCIp<<QCPort;
            port = 8080;
            break;
        }
        case CutInCB:
        {
            out<<ssrcList[0]<<ssrcList[1]<<ssrcList[2];
            port = 8060;
            break;
        }
    }
    udpSocket->writeDatagram(data,data.length(),QHostAddress(destip),port);
}

int Widget::proAudio()
{
    if(isStart == 1)
    {
        while (av_audio_fifo_size(ea->auinFifo) >= FRAMES_PER_BUFFER)
        {
            ea->encodeAUDo(&aeBuf,&aeSize);
            session.SendPacket(aeBuf,aeSize,AAC,false,FRAMES_PER_BUFFER);
        }
    }
    return 0;
}

void Widget::proRequest()
{
    while(udpSocket->hasPendingDatagrams())
    {
        QByteArray datagram;
        datagram.resize(udpSocket->pendingDatagramSize());
        udpSocket->readDatagram(datagram.data(),datagram.size());
        QDataStream in(&datagram,QIODevice::ReadOnly);
        int messageType;
        in>>messageType;
        switch(messageType)
        {
            case Callback:
            {
                int YesorNo;
                uint16_t srcport;
                uint32_t srcip;
                uint32_t ssrc;
                in>>YesorNo>>srcip>>srcport>>ssrc;
                if (YesorNo == 1)
                {
                    if (isStart == 0)
                    {
                        addDest(m_ip,m_port);
                        TC = new ThreadCamera(&session);
                        connect(TC,SIGNAL(captured()),this,SLOT(showLocalPic()),Qt::BlockingQueuedConnection);
                        isStart = 1;
                        TC->start();
                        TM = new ThreadMic(mg);
                        TM->start();
                        ipList[1] = srcip;
                        portList[1] = srcport;
                        ssrcList[1] = ssrc;
//                        sleep(1);
                        sh = new Show(this,&session,&da);
                        sh->ssrc[0] = ssrc;
                        sh->show();
                        isStart2 =1;
                    }
                    else
                    {
                        ipList[2] =srcip;
                        portList[2] = srcport;
                        ssrcList[2] = ssrc;
                        sh->ssrc[1] = ssrc;
                    }
                    struct in_addr tempip;
                    tempip.s_addr = srcip;
                    char *temp_ip = inet_ntoa(tempip);
                    sendMessage(Callback2,temp_ip);
                }
                else if (YesorNo == 0)
                {
                    QMessageBox::information(this,"Refused","Refused!!");
                }
                break;
            }
            case Request:
            {
                QString name;
                uint16_t srcport;
                uint32_t srcip;
                uint32_t ssrc;
                in>>name>>srcip>>srcport>>ssrc;
                int btn = QMessageBox::information(this,"Request",tr("Request from%1").arg(name),QMessageBox::Yes,QMessageBox::No);
                if (btn == QMessageBox::Yes)
                {
                    addDest(m_ip,m_port);
                    ipList[1] = srcip;
                    portList[1] = srcport;
                    ssrcList[1] = ssrc;
                    sh = new Show(this,&session,&da);
                    sh->ssrc[0] = ssrc;
                    sh->show();
                    isStart2 =1;
                }
                else if (btn == QMessageBox::No)
                {
                    isStart2 = 0;
                }
                struct in_addr tempip;
                tempip.s_addr = srcip;
                char *temp_ip= inet_ntoa(tempip);
                sendMessage(Callback,temp_ip);
                break;
            }
            case Callback2:
            {
                if (isStart == 0)
                {
                    TC = new ThreadCamera(&session);
                    connect(TC,SIGNAL(captured()),this,SLOT(showLocalPic()),Qt::BlockingQueuedConnection);
                    isStart = 1;
                    TC->start();
                    TM = new ThreadMic(mg);
                    TM->start();
                }
                break;
            }
            case Invite:
            {
                in>>ipList[1]>>portList[1]>>ssrcList[1]>>ipList[2]>>portList[2]>>ssrcList[2];
                int btn = QMessageBox::information(this,"Invitation","invitation",QMessageBox::Yes,QMessageBox::No);
                if (btn == QMessageBox::Yes)
                {
                    addDest(m_ip,m_port);
                    sh = new Show(this,&session,&da);
                    sh->ssrc[0] = ssrcList[1];
                    sh->ssrc[1] = ssrcList[2];
                    sh->show();
                    isStart2 =1;
                }
                else if (btn == QMessageBox::No)
                {
                    isStart2 = 0;
                }
                struct in_addr tempip;
                tempip.s_addr = ipList[1];
                char *temp_ip= inet_ntoa(tempip);
                sendMessage(Callback,temp_ip);
                tempip.s_addr = ipList[2];
                temp_ip= inet_ntoa(tempip);
                sendMessage(Callback,temp_ip);
                break;
            }
            case CutIn:
            {
                in>>QCIp>>QCPort;
                if (isServer == 1)
                {
                    struct in_addr tempip;
                    tempip.s_addr = ipList[1];
                    char *temp_ip= inet_ntoa(tempip);
                    sendMessage(CutIn,temp_ip);
                    tempip.s_addr = ipList[2];
                    temp_ip= inet_ntoa(tempip);
                    sendMessage(CutIn,temp_ip);
                    tempip.s_addr = QCIp;
                    temp_ip= inet_ntoa(tempip);
                    sendMessage(CutInCB,temp_ip);
                }
                break;
            }
        }
    }
}

void Widget::on_B_Connect()
{
    QByteArray ba = ui->I_RemoteIP->text().toLatin1();
    char* destip=ba.data();
    sendMessage(Request,destip);
}
void Widget::on_B_Invite()
{
    QByteArray ba = ui->I_RemoteIP->text().toLatin1();
    char* destip=ba.data();
    sendMessage(Invite,destip);
}

void Widget::on_B_Add()
{
    QByteArray ba = ui->I_RemoteIP->text().toLatin1();
    char *serip = ba.data();
    int port = ui->I_RemotePort->text().toInt();
    uint16_t dest_port = port;
    uint32_t dest_ip = inet_addr(serip);
    dest_ip = ntohl(dest_ip);
    RTPIPv4Address addr(dest_ip,dest_port);
    status = session.AddDestination(addr);
}
