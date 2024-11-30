extern "C"
{
#include "AvEncoder.h"
}

#include <csignal>
#include <thread>
#include "FlowController.hpp"
#include "CameraDrive/Drive_V4L2Reader.hpp"

using namespace V4L2Tools;
FILE *f;
FlowThread *threadss;

int main(int argc, char const *argv[])
{
    if (argc >= 6) // dev width height fps qp
    {
        std::signal(SIGINT, [](int sig) -> void
                    {
                        threadss->FlowStopAndWait();
                        // fclose(f);
                    });

        // f = fopen("stream.h264", "wb");

        V4L2Encoder v4leOut(argv[1], {
                                         std::atoi(argv[2]),
                                         std::atoi(argv[3]),
                                         std::atoi(argv[4]),
                                         1,
                                         false,
                                         V4L2_PIX_FMT_H264,
                                         V4L2_PIX_FMT_NV12,
                                     },
                            false);

        V4l2Data datain2 = v4leOut.V4l2DataGet();
        V4l2Data dataOut2 = v4leOut.V4l2DataGetOut();

        AVPacket pkt;
        TAVCodecCtx *AVCTX = new TAVCodecCtx;
        TAVCodecInfo AVINF = {
            .qp = (char *)argv[5],
            .width = std::atoi(argv[2]),
            .height = std::atoi(argv[3]),
            .framerate = std::atoi(argv[4]),
            .inputPixfmt = AV_PIX_FMT_NV12,
        };
        AVCodecInit(AVCTX, AVINF);

        threadss = new FlowThread(
            [&]
            {
                v4leOut.V4L2EncodeSet(datain2, dataOut2);
                if (dataOut2.size != 0)
                {
                    pkt = AVCodecPushFrame2(AVCTX, AVINF, dataOut2.data, dataOut2.bytesperline);
                    std::copy(pkt.data, pkt.data + pkt.size, datain2.data);
                    datain2.size = pkt.size;

                    // fwrite(datain2.data, 1, datain2.size, f);
                }

                // std::cout << threadss->TimeDT << " " << datain2.size << " " << dataOut2.size << "\n\n";
            },
            (float)std::atoi(argv[4]));

        sleep(-1);
    }
    return 0;
}
