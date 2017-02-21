#ifndef CAMERAGET_H
#define CAMERAGET_H

#include <QObject>
#include <QDebug>
#include <opencv2/highgui/highgui_c.h>
#include <opencv/cv.h>

namespace Ui {
class CameraGet;
}

class CameraGet : public QObject
{
public:
    CameraGet();
    ~CameraGet();
    int openCamera(int width,int height);
    int readFrame(IplImage **frame);
    void closeCamera();
private:
    CvCapture *cam; //parameter for video
    IplImage *frame; //to restore the picture frame
};

#endif // CAMERAGET_H
