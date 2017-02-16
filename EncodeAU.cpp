#include "EncodeAU.h"

EncodeAU::EncodeAU()
{
    gotFrame=0;
    ret=0;
    size=0;
}
EncodeAU::~EncodeAU()
{

}

int EncodeAU::encodeAUOpen()
{
    avcodec_register_all();
    pCodec = avcodec_find_encoder(AV_CODEC_ID_AAC);
    if (!pCodec)
    {
        qDebug()<<"--- AAC codec not found\n";
        return -1;
    }
    pCodecCtx = avcodec_alloc_context3(pCodec);
    if (!pCodecCtx)
    {
        qDebug()<<"--- Could not allocate audio codec context\n";
        return -1;
    }
    pCodecCtx->codec_type = AVMEDIA_TYPE_AUDIO;
    pCodecCtx->sample_fmt = AV_SAMPLE_FMT_FLTP;
    pCodecCtx->sample_rate= SAMPLE_RATE;
    pCodecCtx->channel_layout=AV_CH_LAYOUT_STEREO;
    pCodecCtx->channels = av_get_channel_layout_nb_channels(pCodecCtx->channel_layout);
    pCodecCtx->bit_rate = 64000;
    int ret;
    ret = avcodec_open2(pCodecCtx, pCodec,NULL);
    if (ret < 0)
    {
        qDebug()<<"Failed to open encoder!---"<<ret;
        void *tty;
//        av_make_error_string((char *)tty,80,ret);
        qDebug("1");
        qDebug(av_make_error_string((char *)tty,80,ret));
        qDebug("2");
        return -1;
    }
    pFrame = av_frame_alloc();
    pFrame->nb_samples= pCodecCtx->frame_size;
    pFrame->format= pCodecCtx->sample_fmt;
    size = av_samples_get_buffer_size(NULL, pCodecCtx->channels,pCodecCtx->frame_size,pCodecCtx->sample_fmt, 1);
    frameBuf = (uint8_t *)av_malloc(size);
    avcodec_fill_audio_frame(pFrame, pCodecCtx->channels, pCodecCtx->sample_fmt,(const uint8_t*)frameBuf, size, 1);
    auinFifo = av_audio_fifo_alloc(pCodecCtx->sample_fmt,pCodecCtx->channels,pCodecCtx->frame_size*10);
    av_init_packet(&pkt);
    return 0;
}

void EncodeAU::encodeAUClose()
{
    av_free(pFrame);
    av_free(frameBuf);
}

int EncodeAU::encodeAUDo(void **poBuf,int *poLen)
{
    out = fopen("test.aac","a");
    *poLen = 0;
    av_free_packet(&pkt);
    av_init_packet(&pkt);
    av_audio_fifo_read(auinFifo,(void**)&frameBuf,pFrame->nb_samples);
    gotFrame=0;
    ret = avcodec_encode_audio2(pCodecCtx, &pkt,pFrame, &gotFrame);
    if(ret < 0)
    {
        qDebug()<<"Failed to encode!\n";
        return -1;
    }
    if (gotFrame==1)
    {
        fwrite(pkt.data,pkt.size,1,out);
        *poBuf = pkt.data;
        *poLen = pkt.size;
    }
    fclose(out);
    return 0;
}

int EncodeAU::flushEncoder(AVFormatContext *fmtCtx,unsigned int streamIndex)
{
    int ret;
    int got_frame;
    AVPacket enc_pkt;
    if (!(fmtCtx->streams[streamIndex]->codec->codec->capabilities &CODEC_CAP_DELAY)) return 0;
    while (1)
    {
        enc_pkt.data = NULL;
        enc_pkt.size = 0;
        av_init_packet(&enc_pkt);
        ret = avcodec_encode_audio2 (fmtCtx->streams[streamIndex]->codec, &enc_pkt,NULL, &got_frame);
        av_frame_free(NULL);
        if (ret < 0)
            break;
        if (!got_frame)
        {
            ret=0;
            break;
        }
        qDebug("Flush Encoder: Succeed to encode 1 frame!\tsize:%5d\n",enc_pkt.size);
        ret = av_write_frame(fmtCtx, &enc_pkt);
        if (ret < 0)
            break;
    }
    return ret;
}
