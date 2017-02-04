#include "ThreadCamera.h"

ThreadCamera::ThreadCamera()
{
    cg = new CameraGet();
    cv = new Convert();
    ec = new Encode();

    AVPixelFormat vfmt = AV_PIX_FMT_BGR24;
    AVPixelFormat ofmt = AV_PIX_FMT_YUV420P;

    cv->inWidth = WIDTH;
    cv->inHeight = HEIGHT;
    cv->inPixfmt = vfmt;
    cv->outWidth = WIDTH;
    cv->outHeight = HEIGHT;
    cv->outPixfmt = ofmt;

    ec->srcPicWidth = WIDTH;
    ec->srcPicHeight = HEIGHT;
    ec->encPicWidth = WIDTH;
    ec->encPicHeight = HEIGHT;
    ec->chromaInterleave = 0;
    ec->fps = FRAMERATE;
    ec->gop = 10;
    ec->bitrate = 400;

    pacBuf = (char *) malloc(MAXDATASIZE);

    ret = cg->openCamera(WIDTH,HEIGHT);
    ret = cv->convertOpen(cv->inWidth,cv->inHeight, cv->inPixfmt, cv->outWidth, cv->outHeight, cv->outPixfmt);
    ret = ec->encodeOpen(ec->srcPicWidth,ec->srcPicHeight,ec->encPicWidth, ec->encPicHeight,ec->fps,ec->bitrate,ec->gop,ec->chromaInterleave);

    isStop = false;
}
ThreadCamera::~ThreadCamera()
{
    free(pacBuf);

    ec->encodeClose();
    cv->convertClose();
    cg->closeCamera();
}
void ThreadCamera::run()
{
    frameCount = 0;
    while(!isStop)
    {
        ret = cg->readFrame(&capFrame);
        emit captured();

        capBuf = (void *)capFrame->imageData;
        capLen = capFrame->imageSize;

        ret = cv->convertDo(capBuf,capLen,&cvtBuf,&cvtLen);
        if (ret < 0)
        {
           qDebug()<<"--- convert_do failed";
        }
        if (cvtLen <= 0)
        {
            qDebug()<<"--- No convert data";
        }
        ret = ec->encodeDo(cvtBuf,cvtLen,&encBuf,&encLen,&pType);
        if (ret < 0)
        {
            qDebug()<<"--- encode_do failed";
        }
        if (encLen <= 0)
        {
            qDebug()<<"--- No encode data";
        }
        frameCount++;
//        qDebug()<<frameCount;
    }
    isStop = true;
    qDebug()<<"ThreadCamera out";
}
void ThreadCamera::stop()
{
    isStop = true;
}
