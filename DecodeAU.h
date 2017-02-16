#ifndef DECODEAU_H
#define DECODEAU_H

#include <QDebug>
#include "COMDEF.h"
#include "FFMPEG_COMMON.h"

class DecodeAU
{
public:
    DecodeAU();
    ~DecodeAU();
    int decodeAUOpen();
    void decodeAUClose();
    int decodeAUDo(void *ibuf,int ilen);
    int audioResampling(AVCodecContext * audio_dec_ctx, AVFrame * pAudioDecodeFrame,int out_sample_fmt, int out_channels , int out_sample_rate , uint8_t * out_buf);

    AVFormatContext* pFormatCtx;
    AVOutputFormat* fmt;
    AVStream* audioSt;
    AVCodecContext* pCodecCtx;
    AVCodec* pCodec;
    AVAudioFifo* auoutFifo;
    int16_t* frameBuf;
    uint8_t* outBuf;
    AVFrame* pFrame;
    AVPacket pkt;
    FILE* pcm;
    int gotFrame,ret,size,outSize;
};

#endif // DECODEAU_H
