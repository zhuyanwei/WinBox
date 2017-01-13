#include "ThreadCamera.h"

ThreadCamera::ThreadCamera()
{
    cg = new CameraGet();
    ret = cg->openCamera(WIDTH,HEIGHT);
    if(ret == -1)
    {
        qDebug()<<"open camera fail";
        cg->closeCamera();
    }
    isStop = false;
}
ThreadCamera::~ThreadCamera()
{
    cg->closeCamera();
}
void ThreadCamera::run()
{
    qDebug()<<"ThreadCamera is running";
    while(!isStop)
    {
        ret = cg->readFrame(&capFrame);
        emit captured();
    }
    isStop = true;
}
void ThreadCamera::stop()
{
    isStop = true;
}
