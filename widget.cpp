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

void Widget::on_B_OpenCam()
{
    qDebug()<<"B_OpenCam clicked";
    isStart = true;
    TC = new ThreadCamera();
    connect(TC,SIGNAL(captured()),this,SLOT(showLocalPic()));
    TC->start();
}

void Widget::on_B_CloseCam()
{
    qDebug()<<"B_CloseCam clicked";
    isStart = false;
    TC->stop();
    TC->~ThreadCamera();
    ui->L_LocalWindow->setText("LocalWindow");
}

void Widget::on_B_Pause()
{
    qDebug()<<"B_CloseCam clicked";
    TC->stop();
}
