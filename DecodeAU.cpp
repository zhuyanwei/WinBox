#include "DecodeAU.h"

DecodeAU::DecodeAU()
{
    gotFrame=0;
    ret=0;
    size=0;
}
DecodeAU::~DecodeAU()
{

}

int DecodeAU::decodeAUOpen()
{
    avcodec_register_all();
    pCodec = avcodec_find_decoder(AV_CODEC_ID_AAC);
    if (!pCodec)
    {
        qDebug()<<"--- AAC codec not found\n";
        return -1;
    }
    pCodecCtx = avcodec_alloc_context3(pCodec);
    if (!pCodecCtx)
    {
        qDebug()<<"--- Could not allocate audio codec context\n";
        return -1;
    }
    pCodecCtx->codec_type = AVMEDIA_TYPE_AUDIO;
    pCodecCtx->sample_rate= SAMPLE_RATE;
    pCodecCtx->channel_layout= AVCHLAYOUT;
    pCodecCtx->channels = av_get_channel_layout_nb_channels(pCodecCtx->channel_layout);
    pCodecCtx->bit_rate = 64000;
    if (avcodec_open2(pCodecCtx, pCodec,NULL) < 0)
    {
        qDebug()<<"Failed to open encoder!\n";
        return -1;
    }
    pFrame = av_frame_alloc();
    pFrame->nb_samples= pCodecCtx->frame_size;
    pFrame->format= pCodecCtx->sample_fmt;
    size = av_samples_get_buffer_size(NULL, pCodecCtx->channels,pCodecCtx->frame_size,pCodecCtx->sample_fmt, 1);
    frameBuf = (int16_t*) av_malloc(size);
    outSize = av_samples_get_buffer_size(NULL, NUM_CHANNELS,FRAMES_PER_BUFFER,AVSAMPLEFMT, 1);
    outBuf =(uint8_t*) av_malloc(outSize);
    auoutFifo = av_audio_fifo_alloc(AVSAMPLEFMT,NUM_CHANNELS,FRAMES_PER_BUFFER * 10);
    return 0;
}

void DecodeAU::decodeAUClose()
{
    av_free(pFrame);
    av_free(frameBuf);
    av_free(outBuf);
}

int DecodeAU::decodeAUDo(void *ibuf,int ilen)
{
    pcm = fopen("raw.pcm","a");
    av_init_packet(&pkt);
    pkt.data = (uchar *)ibuf;
    pkt.size = ilen;
    while(pkt.size > 0)
    {
        ret = avcodec_decode_audio4(pCodecCtx,pFrame, &gotFrame,&pkt);
        if(ret < 0)
        {
            qDebug()<<"Failed to decode!0068\n";
            return -1;
        }
        if (gotFrame >0)
        {
            audioResampling(pCodecCtx,pFrame,AVSAMPLEFMT,NUM_CHANNELS,SAMPLE_RATE,outBuf);
            fwrite(outBuf,1,outSize,pcm);
            av_audio_fifo_write(auoutFifo,(void**)&outBuf,FRAMES_PER_BUFFER);
        }
        pkt.size -= ret;
        pkt.data += ret;
    }
    av_free_packet(&pkt);
    fclose(pcm);
    return 0;
}

int DecodeAU::audioResampling(AVCodecContext * audio_dec_ctx, AVFrame * pAudioDecodeFrame,int out_sample_fmt, int out_channels , int out_sample_rate , uint8_t * out_buf)
{
    SwrContext * swr_ctx = NULL;
    int data_size = 0;
    int ret = 0;
    int64_t src_ch_layout = AVCHLAYOUT;
    int64_t dst_ch_layout = AVCHLAYOUT;
    int dst_nb_channels = 0;
    int dst_linesize = 0;
    int src_nb_samples = 0;
    int dst_nb_samples = 0;
    int max_dst_nb_samples = 0;
    uint8_t **dst_data = NULL;
    int resampled_data_size = 0;
    if (swr_ctx)
    {
        swr_free(&swr_ctx);
    }
    swr_ctx = swr_alloc();
    if (!swr_ctx)
    {
        qDebug("swr_alloc error \n");
        return -1;
    }
    src_ch_layout = (audio_dec_ctx->channel_layout && audio_dec_ctx->channels ==
        av_get_channel_layout_nb_channels(audio_dec_ctx->channel_layout)) ?
        audio_dec_ctx->channel_layout : av_get_default_channel_layout(audio_dec_ctx->channels);
    if (out_channels == 1)
    {
        dst_ch_layout = AVCHLAYOUT;
    }
    else if(out_channels == 2)
    {
        dst_ch_layout = AVCHLAYOUT;
    }
    else
    {
    }
    if (src_ch_layout <= 0)
    {
        qDebug("src_ch_layout error \n");
        return -1;
    }
    src_nb_samples = pAudioDecodeFrame->nb_samples;
    if (src_nb_samples <= 0)
    {
        qDebug("src_nb_samples error \n");
        return -1;
    }
    av_opt_set_int(swr_ctx, "in_channel_layout",    src_ch_layout, 0);
    av_opt_set_int(swr_ctx, "in_sample_rate",       audio_dec_ctx->sample_rate, 0);
    av_opt_set_sample_fmt(swr_ctx, "in_sample_fmt", audio_dec_ctx->sample_fmt, 0);
    av_opt_set_int(swr_ctx, "out_channel_layout",    dst_ch_layout, 0);
    av_opt_set_int(swr_ctx, "out_sample_rate",       out_sample_rate, 0);
    av_opt_set_sample_fmt(swr_ctx, "out_sample_fmt", (AVSampleFormat)out_sample_fmt, 0);
    swr_init(swr_ctx);
    max_dst_nb_samples = dst_nb_samples = av_rescale_rnd(src_nb_samples, out_sample_rate, audio_dec_ctx->sample_rate, AV_ROUND_UP);
    if (max_dst_nb_samples <= 0)
    {
        qDebug("av_rescale_rnd error \n");
        return -1;
    }
    dst_nb_channels = av_get_channel_layout_nb_channels(dst_ch_layout);
    ret = av_samples_alloc_array_and_samples(&dst_data, &dst_linesize, dst_nb_channels, dst_nb_samples, (AVSampleFormat)out_sample_fmt, 0);
    if (ret < 0)
    {
        qDebug("av_samples_alloc_array_and_samples error \n");
        return -1;
    }
    dst_nb_samples = av_rescale_rnd(swr_get_delay(swr_ctx, audio_dec_ctx->sample_rate) + src_nb_samples, out_sample_rate, audio_dec_ctx->sample_rate,AV_ROUND_UP);
    if (dst_nb_samples <= 0)
    {
        qDebug("av_rescale_rnd error \n");
        return -1;
    }
    if (dst_nb_samples > max_dst_nb_samples)
    {
        av_free(dst_data[0]);
        ret = av_samples_alloc(dst_data, &dst_linesize, dst_nb_channels,dst_nb_samples, (AVSampleFormat)out_sample_fmt, 1);
        max_dst_nb_samples = dst_nb_samples;
    }
    data_size = av_samples_get_buffer_size(NULL, audio_dec_ctx->channels,pAudioDecodeFrame->nb_samples,audio_dec_ctx->sample_fmt, 1);
    if (data_size <= 0)
    {
        qDebug("av_samples_get_buffer_size error \n");
        return -1;
    }
    resampled_data_size = data_size;
    if (swr_ctx)
    {
        ret = swr_convert(swr_ctx, dst_data, dst_nb_samples,(const uint8_t **)pAudioDecodeFrame->data, pAudioDecodeFrame->nb_samples);
        if (ret <= 0)
        {
            qDebug("swr_convert error \n");
            return -1;
        }
        resampled_data_size = av_samples_get_buffer_size(&dst_linesize, dst_nb_channels,ret, (AVSampleFormat)out_sample_fmt, 1);
        if (resampled_data_size <= 0)
        {
            qDebug("av_samples_get_buffer_size error \n");
            return -1;
        }
    }
    else
    {
        qDebug("swr_ctx null error \n");
        return -1;
    }
    memcpy(out_buf,dst_data[0],resampled_data_size);
    if (dst_data)
    {
        av_freep(&dst_data[0]);
    }
    av_freep(&dst_data);
    dst_data = NULL;
    if (swr_ctx)
    {
        swr_free(&swr_ctx);
    }
    return resampled_data_size;
}

