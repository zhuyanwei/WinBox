#include "CameraGet.h"

CameraGet::CameraGet()
{
    cam = NULL;
}

CameraGet::~CameraGet()
{

}

int CameraGet::openCamera(int width,int height)
{
    cam = cvCreateCameraCapture(0);
    cvSetCaptureProperty(cam,CV_CAP_PROP_FRAME_WIDTH,width);
    cvSetCaptureProperty(cam,CV_CAP_PROP_FRAME_HEIGHT,height);
    qDebug()<<"+++ Camera Opened,all clear";
    return 0;
}

int CameraGet::readFrame(IplImage **frame)
{
    *frame = cvQueryFrame(cam);
    return 0;
}

void CameraGet::closeCamera()
{
    cvReleaseCapture(&cam);
    qDebug()<<"Camera Closed";
}
