extern "C"
{
#include "AvEncoder.h"
}

#include <map>
#include <csignal>
#include <thread>
#include "FlowController.hpp"
#include "CameraDrive/Drive_V4L2Reader.hpp"

FILE *f;
FlowThread *threadss;

#define EMAP(Variable) (#Variable)
static const std::map<std::string, AVPixelFormat> FFMPEGFormat_s =
    {
        {EMAP(YUYV), AV_PIX_FMT_YUYV422},
        {EMAP(NV12), AV_PIX_FMT_NV12},
};

static const std::map<std::string, unsigned int> V4L2Format_s =
    {
        {EMAP(YUYV), V4L2_PIX_FMT_YUYV},
        {EMAP(YUV420), V4L2_PIX_FMT_YUV420},
        {EMAP(NV12), V4L2_PIX_FMT_NV12},
        {EMAP(BGR3), V4L2_PIX_FMT_BGR24},
        {EMAP(H264), V4L2_PIX_FMT_H264},
        {EMAP(H265), V4L2_PIX_FMT_HEVC},
        {EMAP(MJPEG), V4L2_PIX_FMT_MJPEG},
};

uint8_t *smdata;
uint8_t *smdataOut;
ssize_t smdataSize;
ssize_t smdataSizeOut;

int bytesperline;

int main(int argc, char const *argv[])
{
    std::cout << "v4l2-cedurs /dev/video1 640 480 60 30 NV12 460800 640\n";
    if (argc >= 9) // dev width height fps qp
    {
        AVPacket pkt;
        TAVCodecCtx *AVCTX = new TAVCodecCtx;
        TAVCodecInfo AVINF = {
            .qp = (char *)argv[5],
            .width = std::atoi(argv[2]),
            .height = std::atoi(argv[3]),
            .framerate = std::atoi(argv[4]),
            .inputPixfmt = FFMPEGFormat_s.at(argv[6]),
        };
        AVCodecInit(AVCTX, AVINF);

        smdataSize = std::atoi(argv[7]);
        bytesperline = std::atoi(argv[8]);

        threadss = new FlowThread(
            [&]
            {
                pkt = AVCodecPushFrame2(AVCTX, AVINF, smdata, bytesperline);
                std::copy(pkt.data, pkt.data + pkt.size, smdataOut);
                smdataSizeOut = pkt.size;

                std::cout << threadss->TimeDT << " " << smdataSizeOut << " " << smdataSize << "\n\n";
            },
            1,
            (float)std::atoi(argv[4]));

        sleep(-1);
    }
    return 0;
}
