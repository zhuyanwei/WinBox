#include "Encode.h"

Encode::Encode()
{

}

Encode::~Encode()
{

}

int Encode::encodeOpen(int srcPicWidth,int srcPicHeight,int encPicWidth, int encPicHeight,int fps,int bitrate,int gop, int chromaInterleave)
{
    this->codec = NULL;
    this->ctx = NULL;
    this->frame = NULL;
    this->inbuffer = NULL;
    this->inbufsize = 0;
    this->frameCounter = 0;

    this->srcPicWidth = srcPicWidth;
    this->srcPicHeight = srcPicHeight;
    this->encPicWidth = encPicWidth;
    this->encPicHeight = encPicHeight;
    this->fps = fps;
    this->bitrate = bitrate;
    this->gop = gop;
    this->chromaInterleave = chromaInterleave;

    avcodec_register_all();
    this->codec = avcodec_find_encoder(AV_CODEC_ID_H264);
    if (!this->codec)
    {
        qDebug()<<"H264 codec not found";
        goto err0;
    }

    this->ctx = avcodec_alloc_context3(this->codec);
    if (!this->ctx)
    {
        qDebug()<<"Could not allocate video codec context";
        goto err0;
    }
    this->ctx->bit_rate = this->bitrate * 1000;
    this->ctx->width = this->srcPicWidth;
    this->ctx->height = this->srcPicHeight;
    this->ctx->time_base = (AVRational
            )
            { 1, this->fps };
    this->ctx->gop_size = this->gop;
    this->ctx->max_b_frames = 1;
    this->ctx->pix_fmt = AV_PIX_FMT_YUV420P;
//	this->ctx->thread_count = 1;
    // eliminate frame delay!
    av_opt_set(this->ctx->priv_data, "preset", "slow", 0);
//    av_opt_set(this->ctx->priv_data, "tune", "zerolatency", 0);
//    av_opt_set(this->ctx->priv_data, "x264opts",
//            "no-mbtree:sliced-threads:sync-lookahead=0", 0);

    if (avcodec_open2(this->ctx, this->codec, NULL) < 0)
    {
        qDebug()<<"Could not open codec";
        goto err1;
    }

    this->frame = av_frame_alloc();
    if (!this->frame)
    {
        qDebug()<<"Could not allocate video frame";
        goto err2;
    }

    this->frame->format = this->ctx->pix_fmt;
    this->frame->width = this->ctx->width;
    this->frame->height = this->ctx->height;
    this->inbufsize = avpicture_get_size(AV_PIX_FMT_YUV420P,this->srcPicWidth, this->srcPicHeight);
    this->inbuffer = (uint8_t*)av_malloc(this->inbufsize);
    if (!this->inbuffer)
    {
        qDebug()<<"Could not allocate inbuffer";
        goto err3;
    }
    avpicture_fill((AVPicture *) this->frame, this->inbuffer,AV_PIX_FMT_YUV420P, this->srcPicWidth,this->srcPicHeight);

    av_init_packet(&this->packet);
    this->packet.data = NULL;
    this->packet.size = 0;

    qDebug()<<"Encode Opened";
    return 0;

    err3: av_frame_free(&this->frame);
    err2: avcodec_close(this->ctx);
    err1: av_free(this->ctx);
    err0: free(this);
    return -1;
}

void Encode::encodeClose()
{
    av_free_packet(&this->packet);
    av_free(this->inbuffer);
    av_frame_free(&this->frame);
    avcodec_close(this->ctx);
    av_free(this->ctx);
    free(this);
    printf("+++ Encode Closed\n");

}

int Encode::encodeDo(void *ibuf, int ilen, void **pobuf,int *polen, enum picType *type)
{

}

int Encode::encodeSetQp(int val)
{

}

int Encode::encodeSetGop(int val)
{

}

int Encode::encodeSetBitrate(int val)
{

}

int Encode::encodeSetFramerate( int val)
{

}

void Encode::encodeForceIpic()
{

}
