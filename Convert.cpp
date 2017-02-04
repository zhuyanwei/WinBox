#include "Convert.h"

Convert::Convert()
{

}

Convert::~Convert()
{

}

int Convert::convertOpen(int inWidth, int inHeight, AVPixelFormat inPixfmt, int outWidth, int outHeight, AVPixelFormat outPixfmt)
{
    //initial parametres
    this->swsCtx = NULL;
    this->srcBuffer = NULL;
    this->srcBuffersize = 0;
    this->srcFrame = NULL;
    this->dstBuffer = NULL;
    this->dstBuffersize = 0;
    this->dstFrame = NULL;
    this->inAvfmt = AV_PIX_FMT_NONE;
    this->outAvfmt = AV_PIX_FMT_NONE;
    //set parametres
    this->inWidth = inWidth;
    this->inHeight = inHeight;
//    this->inPixfmt = inPixfmt;
    this->inAvfmt = inPixfmt;
    this->outWidth = outWidth;
    this->outHeight = outHeight;
//    this->outPixfmt = outPixfmt;
    this->outAvfmt = outPixfmt;

    this->swsCtx = sws_getContext(this->inWidth,this->inHeight, this->inAvfmt, this->outWidth,this->outHeight, this->outAvfmt, SWS_BILINEAR, NULL,NULL, NULL);
    if (!this->swsCtx)
    {
        qDebug()<<"Create scale context failed";
        goto err0;
    }
    // alloc buffers
    this->srcFrame = av_frame_alloc();
    if (!this->srcFrame)
    {
        qDebug()<<"allocate src_frame failed";
        goto err1;
    }
    this->srcBuffersize = avpicture_get_size(this->inAvfmt,this->inWidth, this->inHeight);
    this->srcBuffer = (uint8_t *) av_malloc(this->srcBuffersize);
    if (!this->srcBuffer)
    {
        qDebug()<<"allocate src_buffer failed";
        goto err2;
    }
    avpicture_fill((AVPicture *) this->srcFrame, this->srcBuffer,this->inAvfmt, this->inWidth, this->inHeight);

    this->dstFrame = av_frame_alloc();
    if (!this->dstFrame)
    {
        qDebug()<<"allocate dst_frame failed";
        goto err3;
    }
    this->dstBuffersize = avpicture_get_size(this->outAvfmt,this->outWidth, this->outHeight);
    this->dstBuffer = (uint8_t *) av_malloc(this->dstBuffersize);
    if (!this->dstBuffer)
    {
        qDebug()<<"allocate dst_buffer failed";
        goto err4;
    }
    avpicture_fill((AVPicture *) this->dstFrame, this->dstBuffer,this->outAvfmt, this->outWidth,this->outHeight);

    qDebug()<<"+++ Convert Opened,all clear";
    return 0;

    err4: av_frame_free(&this->dstFrame);
    err3: av_free(this->srcBuffer);
    err2: av_frame_free(&this->srcFrame);
    err1: sws_freeContext(this->swsCtx);
    err0: free(this);
    return -1;
}

void Convert::convertClose()
{
    av_free(this->dstBuffer);
    av_frame_free(&this->dstFrame);
    av_free(this->srcBuffer);
    av_frame_free(&this->srcFrame);
    sws_freeContext(this->swsCtx);
    free(this);
    qDebug()<<"Convert Closed";
}

int Convert::convertDo(void *inbuf, int isize,void **poutbuf, int *posize)
{
    if(this->inAvfmt == this->outAvfmt)
    {
        *poutbuf = inbuf;
        *posize = isize;
        return 0;
    }
//    assert(isize == this->srcBuffersize);
    memcpy(this->srcBuffer, inbuf, isize);
    sws_scale(this->swsCtx,(const uint8_t * const *) this->srcFrame->data,this->srcFrame->linesize, 0, this->inHeight,this->dstFrame->data, this->dstFrame->linesize);
    *poutbuf = this->dstBuffer;
    *posize = this->dstBuffersize;
    return 0;
}

