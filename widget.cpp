#include "widget.h"
#include "ui_widget.h"

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);
    //widget UI part
    setFixedSize(600,650);
    setWindowTitle("Elevator audio-video system");
    //open socket
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
    connect(ui->B_Finish,SIGNAL(clicked()),this,SLOT(on_B_Finish()));
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
    end();
    ea->encodeAUClose();
    da->decodeAUClose();
    mg->closeAudio();
    delete ea;
    delete da;
    delete mg;
    ea = NULL;
    da = NULL;
    mg = NULL;
    delete ui;
    qDebug()<<"+++ all finished";
}

void Widget::showLocalPic()
{
    if(isStart == true)
    {
        imgLocal = QImage((const uchar*)TC->capFrame->imageData,TC->capFrame->width, TC->capFrame->height, QImage::Format_RGB888).rgbSwapped();
//        imgLocal = imgLocal.mirrored(true,false);
        ui->L_LocalWindow->setPixmap(QPixmap::fromImage(imgLocal));
    }
    else
    {
        ui->L_LocalWindow->setText("LocalWindow");
    }
}

void Widget::on_B_OpenCam()
{
    //add remote ip and port
    std::string ipStr = ui->I_RemoteIP->text().toStdString();
    uint32_t destIp = inet_addr(ipStr.c_str());
    uint16_t desPort = ui->I_RemotePort->text().toInt();

    addDest(destIp,desPort);

    begin();
}

void Widget::on_B_CloseCam()
{
    end();
}

void Widget::on_B_Finish()
{
    //finish local tasks
    end();
    //send message to remote
    //get working partnal's ip
    struct in_addr a1;
//    uint32_t b1 = ipList[1];
//    memcpy(&a1,&b1,4);
    a1.S_un.S_addr = ipList[1];
    char* destip = inet_ntoa(a1);
    qDebug()<<"Remote ip--"<<destip;
    sendMessage(End,destip);
}

void Widget::on_B_Initial()
{
    //***************************************************reading ini file way
//    QSettings *configIniRead = new QSettings("Settings", QSettings::IniFormat);
//    uint16_t localport = configIniRead->value("Local/port").toInt();
//    uint32_t localSSRC = configIniRead->value("Local/SSRC").toInt();
//    QString remoteIP = configIniRead->value("Remote/IP").toString();
//    uint16_t remoteport = configIniRead->value("Remote/port").toInt();
//    std::string ipss = remoteIP.toStdString();
//    delete configIniRead;
//    configIniRead = NULL;

//    sessionPara.SetOwnTimestampUnit(1.0/90000.0);
//    sessionPara.SetAcceptOwnPackets(true);
//    sessionPara.SetUsePredefinedSSRC(true);
////    uint32_t ssrc = ui->I_SSRC->text().toInt();
//    uint32_t ssrc = localSSRC;
//    sessionPara.SetPredefinedSSRC(ssrc);
//    ssrcList[0] = ssrc;
//    sessionPara.SetMaximumPacketSize(MAXDATASIZE);
//    //set baseport
////    uint16_t baseport = ui->I_LocalPort->text().toInt();
//    uint16_t baseport = localport;
//    transPara.SetPortbase(baseport);
//    portList[0] = baseport;
//    status = session.Create(sessionPara,&transPara);
//    session.SetDefaultPayloadType(H264);
//    session.SetDefaultMark(false);
//    session.SetDefaultTimestampIncrement(90000.0 /FRAMERATE);
//    //add des ip and port
////    std::string ipStr = ui->I_RemoteIP->text().toStdString();
//    std::string ipStr = ipss;
//    uint32_t destIp = inet_addr(ipStr.c_str());
//    destIp = ntohl(destIp);
////    uint16_t desPort = ui->I_RemotePort->text().toInt();
//    uint16_t desPort = remoteport;
//    RTPIPv4Address addr(destIp,desPort);
//    status = session.AddDestination(addr);

    //****************************************************the original way
    sessionPara.SetOwnTimestampUnit(1.0/90000.0);
    sessionPara.SetAcceptOwnPackets(true);
    sessionPara.SetUsePredefinedSSRC(true);
    uint32_t ssrc = ui->I_SSRC->text().toInt();
    sessionPara.SetPredefinedSSRC(ssrc);
    ssrcList[0] = ssrc;
    sessionPara.SetMaximumPacketSize(MAXDATASIZE);
    uint16_t baseport = ui->I_LocalPort->text().toInt();
    transPara.SetPortbase(baseport);
    portList[0] = baseport;
    status = session.Create(sessionPara,&transPara);
    checkError(status);
    qDebug()<<"after creat";
    session.SetDefaultPayloadType(H264);
    session.SetDefaultMark(false);
    session.SetDefaultTimestampIncrement(90000.0 /FRAMERATE);

    udpSocket = new QUdpSocket(this);
    port = 8010;
    udpSocket->bind(port,QUdpSocket::ShareAddress|QUdpSocket::ReuseAddressHint);

    //get local PC's name and ip
    QString localName = QHostInfo::localHostName();
    QHostInfo localInfo = QHostInfo::fromName(localName);
    foreach(QHostAddress address,localInfo.addresses())
    {
        if(address.protocol() == QAbstractSocket::IPv4Protocol)
        {
            QByteArray ba = address.toString().toLatin1();
            ipList[0] = inet_addr(ba.data());
            qDebug()<<"Local ip--"<<ba;
        }
    }
//    //multi play part
//    status = session.SupportsMulticasting();
//    m_ip = inet_addr("111.1.1.1");
//    m_port = 8000;
//    RTPIPv4Address m_addr(ntohl(m_ip), m_port);
//    status = session.JoinMulticastGroup(m_addr);
    connect(udpSocket,SIGNAL(readyRead()),this,SLOT(proRequest()));
    //UI part moving
    //before(0,0,600,500)
    ui->F_Welcome->setGeometry(600,0,600,500);
    //before(0,550,600,50)
    ui->Wig_Initial->setGeometry(600,550,600,50);
    //before(600,450,600,150)
    ui->Wig_Working->setGeometry(0,450,600,150);
}

void Widget::on_B_Test()
{
//    ui->w1->geometry().x()=0;
//    ui->w1->geometry().y()=0;
//    ui->w1->setGeometry(100,100,100,100);
    qDebug()<<"current applicationDirPath: "<<QCoreApplication::applicationDirPath();
//    qDebug()<<"current currentPath: "<<QDir::currentPath();
}

int Widget::slotTest()
{
//    while (udpR->hasPendingDatagrams())
//    {
//        qDebug()<<"received clicked";
//    }
//    return 0;
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
    qDebug()<<"after DDDEST";
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
            //local name
            QString name = QHostInfo::localHostName();
            //local ip
            out<<name<<ipList[0]<<portList[0]<<ssrcList[0];
            //local udp port
            port = 8010;
            break;
        }
        case Callback:
        {
            //local ip
            //the answer of yes or no
            out<<isStart2<<ipList[0]<<portList[0]<<ssrcList[0];
            port = 8010;
            break;
        }
        case End:
        {
            port = 8010;
            break;
        }
//        case Callback2:
//        {
//            port = 8010;
//            break;
//        }
//        case Invite:
//        {
//            out<<ipList[0]<<portList[0]<<ssrcList[0]<<ipList[1]<<portList[1]<<ssrcList[1];
//            port = 8010;
//            break;
//        }
//        case CutIn:
//        {
//            out<<QCIp<<QCPort;
//            port = 8010;
//            break;
//        }
//        case CutInCB:
//        {
//            out<<ssrcList[0]<<ssrcList[1]<<ssrcList[2];
//            port = 8010;
//            break;
//        }
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
    qDebug()<<"udp received";
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
            case Request:
            {
                QString name;
                uint16_t srcport;
                uint32_t srcip;
                uint32_t ssrc;
                in>>name>>srcip>>srcport>>ssrc;
                int btn = QMessageBox::information(this,"New request",tr("Request from %1").arg(name),QMessageBox::Yes,QMessageBox::No);
                if (btn == QMessageBox::Yes)
                {
    //                    addDest(m_ip,m_port);
                    addDest(srcip,srcport);
                    qDebug()<<"request goto adddest";
                    //the server self starts
                    begin();

                    sh->ssrc[0] = ssrc;
                    ipList[1] = srcip;
                    portList[1] = srcport;
                    ssrcList[1] = ssrc;
                }
                else if (btn == QMessageBox::No)
                {
                    isStart2 = false;
                }
                struct in_addr tempip;
                tempip.s_addr = srcip;
                char *temp_ip= inet_ntoa(tempip);
                sendMessage(Callback,temp_ip);
                break;
            }

            case Callback:
            {
                bool YesorNo;
                uint16_t srcport;
                uint32_t srcip;
                uint32_t ssrc;
                in>>YesorNo>>srcip>>srcport>>ssrc;
                if (YesorNo == true)
                {
                    if (isStart == false)
                    {
//                        addDest(m_ip,m_port);
                        addDest(srcip,srcport);
                        //the client self starts
                        begin();

                        sh->ssrc[0] = ssrc;
                        ipList[1] = srcip;
                        portList[1] = srcport;
                        ssrcList[1] = ssrc;
                    }
                }
                else if (YesorNo == false)
                {
                    QMessageBox::information(this,"Refuse","Request refused !");
                }
                break;
            }

            case End:
            {
                end();
                break;
            }

//            case Callback2:
//            {
//                if (isStart == 0)
//                {
//                    TC = new ThreadCamera(&session);
//                    connect(TC,SIGNAL(captured()),this,SLOT(showLocalPic()),Qt::BlockingQueuedConnection);
//                    isStart = 1;
//                    TC->start();
//                    TM = new ThreadMic(mg);
//                    TM->start();
//                }
//                break;
//            }
//            case Invite:
//            {
//                in>>ipList[1]>>portList[1]>>ssrcList[1]>>ipList[2]>>portList[2]>>ssrcList[2];
//                int btn = QMessageBox::information(this,"Invitation","invitation",QMessageBox::Yes,QMessageBox::No);
//                if (btn == QMessageBox::Yes)
//                {
//                    addDest(m_ip,m_port);
//                    sh = new Show(this,&session,&da);
//                    sh->ssrc[0] = ssrcList[1];
//                    sh->ssrc[1] = ssrcList[2];
//                    sh->show();
//                    isStart2 =1;
//                }
//                else if (btn == QMessageBox::No)
//                {
//                    isStart2 = 0;
//                }
//                struct in_addr tempip;
//                tempip.s_addr = ipList[1];
//                char *temp_ip= inet_ntoa(tempip);
//                sendMessage(Callback,temp_ip);
//                tempip.s_addr = ipList[2];
//                temp_ip= inet_ntoa(tempip);
//                sendMessage(Callback,temp_ip);
//                break;
//            }
//            case CutIn:
//            {
//                in>>QCIp>>QCPort;
//                if (isServer == 1)
//                {
//                    struct in_addr tempip;
//                    tempip.s_addr = ipList[1];
//                    char *temp_ip= inet_ntoa(tempip);
//                    sendMessage(CutIn,temp_ip);
//                    tempip.s_addr = ipList[2];
//                    temp_ip= inet_ntoa(tempip);
//                    sendMessage(CutIn,temp_ip);
//                    tempip.s_addr = QCIp;
//                    temp_ip= inet_ntoa(tempip);
//                    sendMessage(CutInCB,temp_ip);
//                }
//                break;
//            }
        }
    }
}

void Widget::on_B_Connect()
{
    QByteArray ba = ui->I_RemoteIP->text().toLatin1();
    char* destip=ba.data();
    sendMessage(Request,destip);
    qDebug()<<"send connect to--"<<ba;
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

void Widget::end()
{
    isStart = false;
    //delete all threads
    if(TC)
    {
        TC->stop();
        while(TC->isRunning())
        {
        }
        delete TC;
        TC = NULL;
    }
    if(sh)
    {
//        sh->close();
        delete sh;
        sh = NULL;
    }
    if(TM)
    {
        TM->stop();
        sleep(1);
        delete TM;
        TM = NULL;
    }
    isStart2 = false;
    //clear rtp
    session.ClearDestinations();

    ui->L_LocalWindow->setText("LocalWindow");
    qDebug()<<"one time view ends";
}

void Widget::begin()
{
    TC = new ThreadCamera(&session);
    connect(TC,SIGNAL(captured()),this,SLOT(showLocalPic()));
    isStart = true;
    TC->start();

    TM = new ThreadMic(mg);
    TM->start();
    sleep(1);

    sh = new Show(this,&session,&da);
    sh->show();
    isStart2 = true;
}
