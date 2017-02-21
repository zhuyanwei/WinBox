#ifndef THREADMIC_H
#define THREADMIC_H

#include <QThread>
#include <QDebug>
#include "MicGet.h"

namespace Ui {
class ThreadMic;
}

class ThreadMic : public QThread
{
    Q_OBJECT
public:
    explicit ThreadMic(MicGet *au);
    ~ThreadMic();
    void stop();

private:
    MicGet *mg;
    void *auBuf;
    int auSize;
    int ret;
    bool isStop;

signals:
    void captured();

protected:
    void run();
};

#endif // THREADMIC_H
