#ifndef RTPSEND_H
#define RTPSEND_H

#include <QObject>

class RtpSend : public QObject
{
    Q_OBJECT
public:
    RtpSend();
    ~RtpSend();

    void *inBuf;
    int inBufSize;

signals:

public slots:
};

#endif // RTPSEND_H
