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
    inputPara.channelCount = 1;
    inputPara.sampleFormat = PA_SAMPLE_TYPE;
    inputPara.suggestedLatency = Pa_GetDeviceInfo( inputPara.device )->defaultLowInputLatency;
    inputPara.hostApiSpecificStreamInfo = NULL;
    outputPara.device = Pa_GetDefaultOutputDevice();
    if(outputPara.device == paNoDevice)
    {
        qDebug("Error: No default output device.\n");
        goto done;
    }
    outputPara.channelCount = 1;
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
    err = Pa_StartStream( stream );
    if( err != paNoError ) goto done;
    qDebug("\n=== Now recording!! Please speak into the microphone. ===\n");
    fflush(stdout);
    while( ( err = Pa_IsStreamActive( stream ) ) == 1 )
    {
        qDebug("233");
    }
    if( err < 0 ) goto done;
    err = Pa_CloseStream( stream );
    if( err != paNoError ) goto done;
#if WRITE_TO_FILE
    {
        FILE  *fid;
        fid = fopen("recorded.raw", "wb");
        if( fid == NULL )
        {
            qDebug("Could not open file.");
        }
        else
        {
            fwrite( data.recordedSamples, NUM_CHANNELS * sizeof(SAMPLE), totalFrames, fid );
            fclose( fid );
            qDebug("Wrote data to 'recorded.raw'\n");
        }
    }
#endif
    return 0 ;
done:
    Pa_Terminate();
    if( data.recordedSamples )       /* Sure it is NULL or valid. */
        free( data.recordedSamples );
    if( data.receivedSamples )       /* Sure it is NULL or valid. */
        free( data.receivedSamples );
    if( err != paNoError )
    {
        qDebug("An error occured while using the portaudio stream\n");
        qDebug("Error number: %d\n", err);
        qDebug("Error message: %s\n", Pa_GetErrorText( err ));
        err = 1;
    }
    return err;
}

int MicGet::closeAudio()
{
    Pa_Terminate();
    if( data.recordedSamples )       /* Sure it is NULL or valid. */
        free( data.recordedSamples );
    if( data.receivedSamples )       /* Sure it is NULL or valid. */
        free( data.receivedSamples );
    if( err != paNoError )
    {
        qDebug("An error occured while using the portaudio stream\n" );
        qDebug("Error number: %d\n", err );
        qDebug("Error message: %s\n", Pa_GetErrorText( err ));
        err = 1;
    }
    return err;
}
