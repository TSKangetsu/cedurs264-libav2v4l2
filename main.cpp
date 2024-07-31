extern "C"
{
#include <libavcodec/avcodec.h>
#include <libavutil/opt.h>
#include <libavcodec/avcodec.h>
#include <libavutil/channel_layout.h>
#include <libavutil/common.h>
#include <libavutil/imgutils.h>
#include <libavutil/mathematics.h>
#include <libavutil/samplefmt.h>
#include "AvEncoder.h"
}
#include "CameraDrive/Drive_V4L2Reader.hpp"

using namespace V4L2Tools;

int main(int argc, char const *argv[])
{
    FILE *f;
    AVPacket pkt;

    V4l2Info info = {
        .ImgWidth = 1280,
        .ImgHeight = 720,
        .FrameRate = 30,
        .PixFormat = V4L2_PIX_FMT_NV12,
    };
    V4L2Drive v4l("/dev/video0", info);
    V4l2Data data;
    TAVCodecCtx *AVCTX = new TAVCodecCtx;
    TAVCodecInfo AVINF = {
        .width = 1280,
        .height = 720,
        .framerate = 30,
        .inputPixfmt = AV_PIX_FMT_NV12,
    };

    AVCodecInit(AVCTX, AVINF);

    f = fopen("stream.h264", "wb");

    for (size_t i = 0; i < 120; i++)
    {
        v4l.V4L2Read(data);
        pkt = AVCodecPushFrame2(AVCTX, AVINF, data.data, data.bytesperline);
        fwrite(pkt.data, 1, pkt.size, f);
    }

    fwrite(endcode, 1, sizeof(endcode), f);
    fclose(f);

    return 0;
}
