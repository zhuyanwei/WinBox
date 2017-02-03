#ifndef FFMPEG_COMMON_H
#define FFMPEG_COMMON_H

extern "C"{
#include <libavutil/opt.h>
#include <libavutil/common.h>
#include <libavutil/avutil.h>
#include <libavutil/audio_fifo.h>
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libswscale/swscale.h>
#include <libswresample/swresample.h>
}

#endif // FFMPEG_COMMON_H
