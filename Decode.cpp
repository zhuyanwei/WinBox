#include "Decode.h"

Decode::Decode()
{

}

Decode::~Decode()
{

}

int Decode::decodeOpen()
{
//    avcodec_init();
    avcodec_register_all();
    codec = avcodec_find_decoder(AV_CODEC_ID_H264);
    if (!codec)
    {
        qDebug()<<"codec not found";
    }
    ctx = avcodec_alloc_context3(codec);
    picture = av_frame_alloc();
    if(codec->capabilities&CODEC_CAP_TRUNCATED)
    {
        ctx->flags|= CODEC_FLAG_TRUNCATED;
    }
    ctx->width = WIDTH;
    ctx->height = HEIGHT;
    ctx->bit_rate = 400000;
    ctx->time_base.den = FRAMERATE;
    ctx->time_base.num = 1;
    ctx->gop_size =10;
    ctx->max_b_frames =1;
    ctx->pix_fmt = AV_PIX_FMT_YUV420P;
    av_opt_set(ctx->priv_data, "preset", "slow", 0);

    ctx->extradata = new uint8_t[35];
    ctx->extradata_size = 35;
    //00 00 00 01
    ctx->extradata[0] = 0x00;
    ctx->extradata[1] = 0x00;
    ctx->extradata[2] = 0x00;
    ctx->extradata[3] = 0x01;
    //67 42 80 1e
    ctx->extradata[4] = 0x67;
    ctx->extradata[5] = 0x64;
    ctx->extradata[6] = 0x00;
    ctx->extradata[7] = 0x0;
    //88 8b 40 50
    ctx->extradata[8] = 0xac;
    ctx->extradata[9] = 0xec;
    ctx->extradata[10] = 0x14;
    ctx->extradata[11] = 0x1f;
    //1e d0 80 00
    ctx->extradata[12] = 0xa1;
    ctx->extradata[13] = 0x00;
    ctx->extradata[14] = 0x00;
    ctx->extradata[15] = 0x03;
    //03 84 00 00
    ctx->extradata[16] = 0x00;
    ctx->extradata[17] = 0x01;
    ctx->extradata[18] = 0x00;
    ctx->extradata[19] = 0x00;
    //af c8 02 00
    ctx->extradata[20] = 0x03;
    ctx->extradata[21] = 0x00;
    ctx->extradata[22] = 0x1e;
    ctx->extradata[23] = 0x8f;
    //00 00 00 01
    ctx->extradata[24] = 0x14;
    ctx->extradata[25] = 0x29;
    ctx->extradata[26] = 0x38;
    ctx->extradata[27] = 0x00;
    //68 ce 38 80
    ctx->extradata[28] = 0x00;
    ctx->extradata[29] = 0x00;
    ctx->extradata[30] = 0x01;
    ctx->extradata[31] = 0x68;
    ctx->extradata[32] = 0xef;
    ctx->extradata[33] = 0xbc;
    ctx->extradata[34] = 0xb0;

    if(avcodec_open2(ctx,codec,NULL)<0)
    {
        qDebug()<<"could not open codec";
    }
    av_init_packet(&this->packet);
    this->packet.data = NULL;
    this->packet.size = 0;
    qDebug()<<"+++ Decode Opend";
    return 1;
}

int Decode::decodeDo(void *inBufPtr, int size, void **showBuf)
{
    av_free_packet(&this->packet);
    av_init_packet(&this->packet);
    this->packet.data = (uchar *)inBufPtr;
    this->packet.size = size;
    len = avcodec_decode_video2(ctx, picture,&gotPicture,&this->packet);
    if (len < 0)
    {
        qDebug()<<"Error while decoding frame ";
    }
    if (gotPicture)
    {
        sws_scale(swsContext, picture->data, picture->linesize, 0, ctx->height, dstFrame->data, dstFrame->linesize);
        *showBuf=dstBuffer;
        return 1;
   }
   return 0;
}

void Decode::decodeClose()
{
    av_free(dstBuffer);
    av_frame_free(&dstFrame);
    sws_freeContext(swsContext);
    av_free_packet(&this->packet);
    av_frame_free(&this->picture);
    avcodec_close(this->ctx);
    av_free(this->ctx);
    free(this);
    qDebug()<<"Decode Closed";
}

int Decode::convertOpen2()
{
    AVPixelFormat dstFmt = AV_PIX_FMT_BGR24;
    swsContext = sws_getContext(WIDTH, HEIGHT, AV_PIX_FMT_YUV420P, WIDTH, HEIGHT,dstFmt,SWS_BICUBIC, 0, 0, 0);
    if (swsContext == 0)
    {
        return 0;
    }
    dstFrame = av_frame_alloc();
    dstSize = avpicture_get_size(dstFmt,WIDTH, HEIGHT);
    dstBuffer = (uint8_t *) av_malloc(dstSize);
    avpicture_fill((AVPicture*)dstFrame, dstBuffer, dstFmt, WIDTH, HEIGHT);
    return 1;
}

void Decode::IplImage2AVFrame(IplImage* iplImage, AVFrame* avFrame, int frameWidth, int frameHeight)
{
    struct SwsContext * imgConvertCtx = 0;
    int linesize[4] = {0, 0, 0, 0};
    imgConvertCtx = sws_getContext(iplImage->width, iplImage->height,AV_PIX_FMT_BGR24,frameWidth,frameHeight,AV_PIX_FMT_YUV420P, SWS_BICUBIC, 0, 0, 0);
    if (imgConvertCtx != 0)
    {
        linesize[0] = 3 * iplImage->width;
        sws_scale(imgConvertCtx, (uint8_t**)iplImage->imageData, linesize, 0, iplImage->height, avFrame->data, avFrame->linesize);
        sws_freeContext(imgConvertCtx);
    }
}

IplImage Decode::*AVFrame2IplImage(AVFrame *src, IplImage *dst,int width,int height)
{

}
