#ifndef ENCODEAU_H
#define ENCODEAU_H

#include <QObject>
#include <QDebug>
#include "COMDEF.h"
#include "FFMPEG_COMMON.h"

class EncodeAU : public QObject
{
    Q_OBJECT
public:
    EncodeAU();
    ~EncodeAU();
    int encodeAUOpen();
    void encodeAUClose();
    int encodeAUDo(void **poBuf,int *poLen);

    AVFormatContext* pFormatCtx;
    AVOutputFormat* fmt;
    AVStream* audioSt;
    AVCodecContext* pCodecCtx;
    AVCodec* pCodec;
    AVAudioFifo* auinFifo;
    uint8_t* frameBuf;
    AVFrame* pFrame;
    AVPacket pkt;
    int gotFrame,ret,size,i;
    FILE *in;
    FILE *out;

private:
    int flushEncoder(AVFormatContext *fmtCtx,unsigned int streamIndex);

signals:

public slots:
};

#endif // ENCODEAU_H
