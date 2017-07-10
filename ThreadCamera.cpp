#include "ThreadCamera.h"

ThreadCamera::ThreadCamera(RTPSession *session)
{
    cg = new CameraGet();
    cv = new Convert();
    ec = new Encode();
    rs = new RtpSend();

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

    rs->maxPktLen = MAXDATASIZE - 20;
    rs->ssrc = SSRC;

    pacBuf = (char *) malloc(MAXDATASIZE);

    ret = cg->openCamera(WIDTH,HEIGHT);
    ret = cv->convertOpen(cv->inWidth,cv->inHeight, cv->inPixfmt, cv->outWidth, cv->outHeight, cv->outPixfmt);
    ret = ec->encodeOpen(ec->srcPicWidth,ec->srcPicHeight,ec->encPicWidth, ec->encPicHeight,ec->fps,ec->bitrate,ec->gop,ec->chromaInterleave);
    ret = rs->packOpen(rs->maxPktLen,rs->ssrc);
    ret = rs->netOpen(session);

    isStop = false;
}
ThreadCamera::~ThreadCamera()
{
    free(pacBuf);
    rs->netClose();
    ec->encodeClose();
    cv->convertClose();
    cg->closeCamera();
    delete rs;
    delete ec;
    delete cv;
    delete cg;
    rs = NULL;
    ec = NULL;
    cv = NULL;
    cg = NULL;
//    free(this);
    qDebug("ThreadCamera destruct");
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
        //send or restore the data.....encBuf,encLen
//        emit sendDone(encBuf,encLen);
        rs->packPut(encBuf, encLen);
        while(rs->packGet(pacBuf, MAXDATASIZE, &pacLen) == 1)
        {

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
