#include "widget.h"
#include "ui_widget.h"

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);

    //connect sigmal and slot
    connect(ui->B_OpenCam,SIGNAL(clicked()),this,SLOT(on_B_OpenCam()));
    connect(ui->B_CloseCam,SIGNAL(clicked()),this,SLOT(on_B_CloseCam()));
    connect(ui->B_Pause,SIGNAL(clicked()),this,SLOT(on_B_Pause()));
}

Widget::~Widget()
{
    delete ui;
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
    TC = new ThreadCamera();
    vid = new Video();
    connect(TC,SIGNAL(captured()),this,SLOT(showLocalPic()));
    connect(vid,SIGNAL(decodeDone()),this,SLOT(showRemotePic()));
    connect(TC,SIGNAL(sendDone(void*,int)),vid,SLOT(readingFrame(void*,int)));
    TC->start();
}

void Widget::on_B_CloseCam()
{
    qDebug()<<"B_CloseCam clicked";
    isStart = false;
    TC->stop();
    TC->sleep(1);
    TC->~ThreadCamera();
    ui->L_LocalWindow->setText("LocalWindow");
}

void Widget::on_B_Pause()
{
    qDebug()<<"B_Pause clicked";
    TC->stop();
}
