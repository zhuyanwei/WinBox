#ifndef ENCODE_H
#define ENCODE_H

#include <QObject>
#include <QDebug>
#include "FFMPEG_COMMON.h"

namespace Ui {
class Encode;
}

class Encode : public QObject
{
    Q_OBJECT
public:
    Encode();
    ~Encode();

    enum picType
    {
       NONE = -1,SPS,PPS,I,P,B
    };

    int encodeOpen(int srcPicWidth,int srcPicHeight,int encPicWidth, int encPicHeight,int fps,int bitrate,int gop, int chromaInterleave);
    void encodeClose();
    int encodeDo(void *ibuf, int ilen, void **pobuf,int *polen, enum picType *type);

    int encodeGetHeaders(void **pbuf, int *plen, enum picType *type);
    int encodeSetQp(int val);
    int encodeSetGop(int val);
    int encodeSetBitrate(int val);
    int encodeSetFramerate( int val);
    void encodeForceIpic();

    AVCodec *codec;
    AVCodecContext *ctx;
    AVFrame *frame;
    uint8_t *inbuffer;
    int inbufsize;
    AVPacket packet;
    unsigned long frameCounter;

    //encodeOpen parametres
    int srcPicWidth;
    int srcPicHeight;
    int encPicWidth;
    int encPicHeight;
    int fps;
    int bitrate;
    int gop;
    int chromaInterleave;


signals:

public slots:
};

#endif // ENCODE_H
