#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QImage>
#include <QTime>
#include <QDebug>
#include "ThreadCamera.h"
#include "Video.h"

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
    void showRemotePic();

private:
    Ui::Widget *ui;
    ThreadCamera *TC;
    Video *vid;
    bool isStart;
    QTimer *timerCam;
    QImage *img;
    QImage imgLocal;
    QImage imgRemote;
};

#endif // WIDGET_H
