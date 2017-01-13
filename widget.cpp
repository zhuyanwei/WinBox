#include "widget.h"
#include "ui_widget.h"

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);

    //connect sigmal and slot
    connect(ui->B_OpenCam,SIGNAL(clicked()),this,SLOT(on_B_OpenCam()));
}

Widget::~Widget()
{
    delete ui;
}

void Widget::showLocalPic()
{
    QImage img1 = QImage((const uchar*)TC->capFrame->imageData,TC->capFrame->width, TC->capFrame->height, QImage::Format_RGB888).rgbSwapped();
    ui->L_LocalWindow->setPixmap(QPixmap::fromImage(img1));
}

void Widget::on_B_OpenCam()
{
    qDebug()<<"B_OpenCam clicked";
    TC = new ThreadCamera();
    connect(TC,SIGNAL(captured()),this,SLOT(showLocalPic()));
    TC->start();
}
