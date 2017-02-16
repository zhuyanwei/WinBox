#ifndef DECODE_H
#define DECODE_H

#include <QObject>
#include <QDebug>
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <COMDEF.h>
#include <FFMPEG_COMMON.h>

namespace Ui {
class Decode;
}

class Decode : public QObject
{
    Q_OBJECT
public:
    Decode();
    ~Decode();

    int decodeOpen();
    int decodeDo(void *inBufPtr, int size, void **showBuf);
    void decodeClose();
    int convertOpen2();
    void IplImage2AVFrame(IplImage* iplImage, AVFrame* avFrame, int frameWidth, int frameHeight);
    IplImage *AVFrame2IplImage(AVFrame *src, IplImage *dst,int width,int height);

    AVCodec *codec;
    AVCodecContext *ctx ;
    int  gotPicture, len;
    AVFrame *picture;
    AVPacket packet;
    struct SwsContext * swsContext;
    int dstSize;
    uint8_t *dstBuffer;
    AVFrame *dstFrame;

signals:

public slots:
};

#endif // DECODE_H
