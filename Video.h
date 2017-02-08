#ifndef VIDEO_H
#define VIDEO_H

#include <QObject>
#include <QDebug>
#include "Decode.h"

namespace Ui {
class Video;
}

class Video : public QObject
{
    Q_OBJECT
public:
    Video();
    ~Video();

    void *deBufR,*deBufT,*disBufR,*disBufT;

private:
    int ret;
    Decode *deR;
    int recvBytes,deLen,totalSizeR;
    char *recvBufR;
//    void *deBufR,*deBufT,*disBufR,*disBufT;
    bool marker;

signals:
    getFrame();
    decodeDone();

public slots:
    void readingFrame(void *buf,int len);

private slots:
    void showRemoteWindow();
};

#endif // VIDEO_H
