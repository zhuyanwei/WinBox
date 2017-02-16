#include "MicGet.h"

MicGet::MicGet(QObject *parent) : QObject(parent)
{

}

MicGet::~MicGet()
{

}

int MicGet::initAudio(AVAudioFifo **auinFifo, AVAudioFifo **auoutFifo)
{
    fflush(stdout);
    data.recordedSamples = *auinFifo;
    if( data.recordedSamples == NULL )
    {
        qDebug("Could not allocate record array.\n");
        goto done;
    }
    data.receivedSamples = *auoutFifo;
    if( data.receivedSamples == NULL )
    {
        qDebug("Could not allocate receive array.\n");
        goto done;
    }
    err = Pa_Initialize();
    if(err != paNoError ) goto done;
    inputPara.device = Pa_GetDefaultInputDevice();
    if(inputPara.device == paNoDevice)
    {
        qDebug("Error: No default input device.\n");
        goto done;
    }
    inputPara.channelCount = 2;
    inputPara.sampleFormat = PA_SAMPLE_TYPE;
    inputPara.suggestedLatency = Pa_GetDeviceInfo( inputPara.device )->defaultLowInputLatency;
    inputPara.hostApiSpecificStreamInfo = NULL;
    outputPara.device = Pa_GetDefaultOutputDevice();
    if(outputPara.device == paNoDevice)
    {
        qDebug("Error: No default output device.\n");
        goto done;
    }
    outputPara.channelCount = 2;
    outputPara.sampleFormat =  PA_SAMPLE_TYPE;
    outputPara.suggestedLatency = Pa_GetDeviceInfo( outputPara.device )->defaultLowOutputLatency;
    outputPara.hostApiSpecificStreamInfo = NULL;
    err = Pa_OpenStream(&stream,&inputPara, &outputPara,SAMPLE_RATE,FRAMES_PER_BUFFER,paClipOff, audioCallback,&data );
    if( err != paNoError ) goto done;
    return 0;
done:
    Pa_Terminate();
    if( data.recordedSamples ) free( data.recordedSamples );
    if( data.receivedSamples ) free( data.receivedSamples );
    if( err != paNoError )
    {
        qDebug( "An error occured while using the portaudio stream\n" );
        qDebug( "Error number: %d\n", err );
        qDebug( "Error message: %s\n", Pa_GetErrorText( err ) );
        err = 1;
    }
    return err;
}

int MicGet::recordAudio()
{

}

int MicGet::closeAudio()
{

}
