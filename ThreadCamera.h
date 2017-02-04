#ifndef THREADCAMERA_H
#define THREADCAMERA_H

#include <QThread>
#include <QDebug>

#include "COMDEF.h"
#include "CameraGet.h"
#include "Convert.h"
#include "Encode.h"

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
    Convert *cv;
    Encode *ec;

    int ret;
    bool isStop;

    char *pacBuf ;
    void *capBuf,*cvtBuf,*hdBuf,*encBuf;
    int capLen, cvtLen, hdLen, encLen, pacLen;
    enum Encode::picType pType;
    unsigned long frameCount;

protected:
    void run();

signals:
    void captured();

public slots:
};

#endif // THREADCAMERA_H
