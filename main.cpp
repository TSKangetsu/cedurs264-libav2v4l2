extern "C"
{
#include "AvEncoder.h"
}

#include <thread>
#include "FlowController.hpp"
#include "CameraDrive/Drive_V4L2Reader.hpp"

using namespace V4L2Tools;

int main(int argc, char const *argv[])
{
    if (argc >= 5) // width height fps qp
    {
        V4L2Encoder v4leOut("/dev/video1", {
                                               std::atoi(argv[1]),
                                               std::atoi(argv[2]),
                                               std::atoi(argv[3]),
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
            .qp = (char *)argv[4],
            .width = std::atoi(argv[1]),
            .height = std::atoi(argv[2]),
            .framerate = std::atoi(argv[3]),
            .inputPixfmt = AV_PIX_FMT_NV12,
        };
        AVCodecInit(AVCTX, AVINF);

        FlowThread thread(
            [&]
            {
                v4leOut.V4L2EncodeSet(datain2, dataOut2);
                pkt = AVCodecPushFrame2(AVCTX, AVINF, dataOut2.data.get(), dataOut2.bytesperline);
                std::copy(pkt.data, pkt.data + pkt.size, datain2.data.get());
                datain2.size = pkt.size;

                std::cout << thread.TimeDT << " " << datain2.size << " " << dataOut2.size << "\n\n";
            },
            (float)std::atoi(argv[3]));

        thread.join();
    }
    return 0;
}
