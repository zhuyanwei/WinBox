#ifndef COMDEF_H
#define COMDEF_H

typedef unsigned int U32;

#define H264 96
#define AAC 97
#define SSRC 10

#define WIDTH 320
#define HEIGHT 240
#define FRAMERATE 15
#define MAXDATASIZE 1420
//-------------------------------ffmpeg part
#define AVSAMPLEFMT AV_SAMPLE_FMT_FLTP
#define AVCHLAYOUT AV_CH_LAYOUT_MONO
//-----------------------------portaudio part
#define SAMPLE_RATE  (44100)
#define FRAMES_PER_BUFFER (512)
#define DITHER_FLAG     (0)
#define NUM_CHANNELS    (1)
// Select sample format.
#if 1
#define PA_SAMPLE_TYPE  paFloat32
typedef float SAMPLE;
#define SAMPLE_SILENCE  (0.0f)
#define PRINTF_S_FORMAT "%.8f"
#elif 0
#define PA_SAMPLE_TYPE  paInt16
typedef short SAMPLE;
#define SAMPLE_SILENCE  (0)
#define PRINTF_S_FORMAT "%d"
#elif 0
#define PA_SAMPLE_TYPE  paInt8
typedef char SAMPLE;
#define SAMPLE_SILENCE  (0)
#define PRINTF_S_FORMAT "%d"
#else
#define PA_SAMPLE_TYPE  paUInt8
typedef unsigned char SAMPLE;
#define SAMPLE_SILENCE  (128)
#define PRINTF_S_FORMAT "%d"
#endif


#endif // COMDEF_H
