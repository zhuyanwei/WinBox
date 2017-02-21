#ifndef CONVERT_H
#define CONVERT_H

#include <QObject>
#include <assert.h>
#include <opencv2/highgui/highgui_c.h>
#include <opencv/cv.h>

#include "COMDEF.h"
#include "FFMPEG_COMMON.h"

class Convert : public QObject
{
    Q_OBJECT
public:
    Convert();
    ~Convert();

    struct SwsContext *swsCtx;
    uint8_t *srcBuffer;
    int srcBuffersize;
    AVFrame *srcFrame;
    uint8_t *dstBuffer;
    int dstBuffersize;
    AVFrame *dstFrame;
    enum AVPixelFormat inAvfmt;
    enum AVPixelFormat outAvfmt;

    int inWidth;
    int inHeight;
    AVPixelFormat inPixfmt;
    int outWidth;
    int outHeight;
    AVPixelFormat outPixfmt;

    int convertOpen(int inWidth, int inHeight, AVPixelFormat inPixfmt, int outWidth, int outHeight, AVPixelFormat outPixfmt);
    void convertClose();
    int convertDo(void *inbuf, int isize,void **poutbuf, int *posize);

private:
//    void yuv422Toyuv420(uint8_t *inbuf, uint8_t *outbuf, int width,int height);

signals:

public slots:
};

#endif // CONVERT_H
