#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include "CameraGet.h"
#include "ThreadCamera.h"
#include <QImage>
#include <QTime>
#include <QDebug>

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
    void showLocalPic();

private:
    Ui::Widget *ui;
    ThreadCamera *TC;
    bool isStart;
    QTimer *timerCam;
    QImage *img;
    QImage imgLocal;
};

#endif // WIDGET_H
