#include <libavcodec/avcodec.h>
#include <libavutil/imgutils.h>
#include <libswscale/swscale.h>
#include <unistd.h>

#define CEDRUSLOG "[CEDRUS-V4L]"

typedef struct TAVCodecInfo_t
{
    // int codec_id;
    // int bitrate;
    char *qp;
    int width;
    int height;
    int framerate;
    // int gop_size;
    // int maxBFrame;
    AVPixelFormat inputPixfmt; // FIXME: seems cedurs only accept NV12
} TAVCodecInfo;

typedef struct TAVCodecCtx_t
{
    AVCodec *codec;
    AVCodecContext *c;
    int i, ret, x, y, got_output;
    AVFrame *frame;
    struct SwsContext *swsCtx;
    AVPacket pkt;
} TAVCodecCtx;

uint8_t endcode[4] = {0, 0, 1, 0xb7};

inline int AVCodecInit(TAVCodecCtx *codecCtx,
                       TAVCodecInfo codecInfo)
{
    int ret = -1;

    avcodec_register_all();
    codecCtx->codec = avcodec_find_encoder_by_name("cedrus264");
    if (!codecCtx->codec)
        exit(1);
    codecCtx->c = avcodec_alloc_context3(codecCtx->codec);
    codecCtx->c->width = codecInfo.width;
    codecCtx->c->height = codecInfo.height;
    codecCtx->c->pix_fmt = AV_PIX_FMT_NV12;
    AVDictionary *TmpCodecOptions = nullptr;
    av_dict_set(&TmpCodecOptions, "qp", codecInfo.qp, 0);
    avcodec_open2(codecCtx->c, codecCtx->codec, &TmpCodecOptions);

    codecCtx->swsCtx = sws_getContext(codecInfo.width,
                                      codecInfo.height,
                                      codecInfo.inputPixfmt,
                                      codecInfo.width,
                                      codecInfo.height,
                                      AV_PIX_FMT_NV12,
                                      SWS_FAST_BILINEAR, 0, 0, 0);

    av_init_packet(&codecCtx->pkt);
    codecCtx->pkt.data = NULL;
    codecCtx->pkt.size = 0;
    codecCtx->frame = av_frame_alloc();
    av_image_alloc(codecCtx->frame->data,
                   codecCtx->frame->linesize,
                   codecInfo.width,
                   codecInfo.height,
                   AV_PIX_FMT_NV12,
                   1);
    codecCtx->frame->format = AV_PIX_FMT_NV12;
    codecCtx->frame->width = codecInfo.width;
    codecCtx->frame->height = codecInfo.height;

    return 0;
};

inline AVPacket AVCodecPushFrame2(TAVCodecCtx *codecCtx, TAVCodecInfo codecInfo, uint8_t *frameData, int dataPreline)
{
    // convert frame to AVFrame
    if (codecInfo.inputPixfmt == AV_PIX_FMT_YUYV422)
    {
        int frame_size = codecInfo.width * codecInfo.height;
        uint8_t *datain[3];
        datain[0] = frameData;
        datain[1] = frameData + frame_size;
        datain[2] = frameData + frame_size * 5 / 4;
        // FIXME: YUYV very slow, seem the dvp rate or cpu lag?
        sws_scale(codecCtx->swsCtx, datain, &dataPreline, 0,
                  codecInfo.height, codecCtx->frame->data, codecCtx->frame->linesize);
    }
    else if (codecInfo.inputPixfmt = AV_PIX_FMT_NV12)
    {
        int frame_size = codecInfo.width * codecInfo.height;
        codecCtx->frame->data[0] = frameData;
        codecCtx->frame->data[1] = frameData + frame_size;
    }
    else
    {
        printf("[AVCODEC] unsupport fmt, only YUYV422 or NV12 support");
        exit(1);
    }

    codecCtx->pkt.data = NULL;
    codecCtx->pkt.size = 0;

    avcodec_encode_video2(codecCtx->c,
                          &codecCtx->pkt,
                          codecCtx->frame,
                          &codecCtx->got_output);
    while (!&codecCtx->got_output)
        usleep(1000);

    // printf("Write frame size=%d\n", codecCtx->pkt.size);

    return codecCtx->pkt;
};