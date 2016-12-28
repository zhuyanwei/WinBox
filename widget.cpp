#include "widget.h"
#include "ui_widget.h"

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);
}

Widget::~Widget()
{
    delete ui;
}

void Widget::on_B_OpenCam_clicked()
{
    //show frames on the L_LocalWimdow
    CameraGet cg = new CameraGet();
    cg.openCamera(200,200);
    QImage image = QImage((const uchar*)CT->cap_frame->imageData,CT->cap_frame->width, CT->cap_frame->height, QImage::Format_RGB888).rgbSwapped();
}
