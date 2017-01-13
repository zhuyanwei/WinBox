#ifndef THREADCAMERA_H
#define THREADCAMERA_H

#include <QThread>
#include "CameraGet.h"
#include <QDebug>

#include "COMDEF.h"

namespace Ui {
class ThreadCamera;
}

class ThreadCamera : public QThread
{
    Q_OBJECT
public:
    explicit ThreadCamera();
    ~ThreadCamera();
    void stop();

    IplImage *capFrame;
private:
    CameraGet *cg;
    int ret;
    bool isStop;

protected:
    void run();

signals:
    void captured();

public slots:
};

#endif // THREADCAMERA_H
