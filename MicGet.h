#ifndef MICGET_H
#define MICGET_H

#include <QObject>
#include <QDebug>
#include <portaudio.h>
#include "COMDEF.h"
#include "FFMPEG_COMMON.h"

#define WRITE_TO_FILE   (0)
#define PA_SAMPLE_TYPE  paInt16
#define SAMPLE_SILENCE  (0)
#define PRINTF_S_FORMAT "%d"

typedef short SAMPLE;
typedef struct
{
    AVAudioFifo      *recordedSamples;
    AVAudioFifo      *receivedSamples;
}paData;

static int audioCallback(const void *inputBuffer,void *outputBuffer,unsigned long framesPerBuffer,const PaStreamCallbackTimeInfo* timeInfo,PaStreamCallbackFlags statusFlags,void *userData)
{
    paData *data = (paData*)userData;
    (void) timeInfo;
    (void) statusFlags;
    (void) userData;
    av_audio_fifo_write(data->recordedSamples,(void**)&inputBuffer,framesPerBuffer);
    if (av_audio_fifo_size(data->receivedSamples)>=framesPerBuffer)
    av_audio_fifo_read(data->receivedSamples,(void**)&outputBuffer,framesPerBuffer);
    return paContinue;
}



class MicGet : public QObject
{
    Q_OBJECT
public:
    explicit MicGet(QObject *parent = 0);
    ~MicGet();
    int initAudio(AVAudioFifo **auinFifo, AVAudioFifo **auoutFifo);
    int recordAudio();
    int closeAudio();

    int numBytes;

private:
    PaStreamParameters  inputPara,outputPara;
    PaStream*           stream;
    PaError             err = paNoError;
    paData              data;
    int                 i;
    int                 totalFrames;
    int                 numSamples;
    SAMPLE              max, val;
    double              average;

signals:
    getAudio();

public slots:
};

#endif // MICGET_H
