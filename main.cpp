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

#include <thread>
#include "CameraDrive/Drive_V4L2Reader.hpp"

using namespace V4L2Tools;

int main(int argc, char const *argv[])
{
    V4L2Encoder v4le("/dev/video2", {
                                        1280,
                                        720,
                                        30,
                                        1,
                                        false,
                                        V4L2_PIX_FMT_NV12,
                                        V4L2_PIX_FMT_H264,
                                    },
                     false);

    V4L2Encoder v4leOut("/dev/video2", {
                                           1280,
                                           720,
                                           30,
                                           1,
                                           false,
                                           V4L2_PIX_FMT_H264,
                                           V4L2_PIX_FMT_NV12,
                                       },
                        false);

    V4l2Data datain = v4le.V4l2DataGet();
    V4l2Data dataOut = v4le.V4l2DataGetOut();
    V4l2Data datain2 = v4leOut.V4l2DataGet();
    V4l2Data dataOut2 = v4leOut.V4l2DataGetOut();
    //================================================================//
    FILE *f;
    AVPacket pkt;

    V4l2Info info = {
        .ImgWidth = 1280,
        .ImgHeight = 720,
        .FrameRate = 30,
        .PixFormat = V4L2_PIX_FMT_NV12,
    };
    V4L2Drive v4l("/dev/video0", info);
    //================================================================//
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
        //
        v4le.V4L2EncodeSet(data, dataOut);
        usleep(10 * 1000);
        v4leOut.V4L2EncodeSet(datain2, dataOut2);
        pkt = AVCodecPushFrame2(AVCTX, AVINF, dataOut2.data, dataOut2.bytesperline);
        fwrite(pkt.data, 1, pkt.size, f);
    }

    fwrite(endcode, 1, sizeof(endcode), f);
    fclose(f);

    return 0;
}
