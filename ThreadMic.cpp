#include "ThreadMic.h"

ThreadMic::ThreadMic(MicGet *au)
{
    mg = au;
    isStop = false;
    mg->isStop = false;
}

ThreadMic::~ThreadMic()
{
    qDebug("threadmic destruct");
}

void ThreadMic::stop()
{
    isStop = true;
    mg->isStop = true;
}

void ThreadMic::run()
{
    while(!isStop)
    {
        mg->recordAudio();
    }
    qDebug("threadmic out");
}

