#include "ThreadMic.h"

ThreadMic::ThreadMic(MicGet *au)
{
    mg = au;
    isStop = false;
}

ThreadMic::~ThreadMic()
{
    qDebug("threadmic destruct");
}

void ThreadMic::stop()
{
    isStop = true;
}

void ThreadMic::run()
{
    while(!isStop)
    {
        mg->recordAudio();
    }
}

