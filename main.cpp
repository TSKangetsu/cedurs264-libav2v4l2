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
    FILE *f;
    AVPacket pkt;

    // V4l2Info info = {
    //     .ImgWidth = 1280,
    //     .ImgHeight = 720,
    //     .FrameRate = 30,
    //     .PixFormat = V4L2_PIX_FMT_NV12,
    // };
    // V4L2Drive v4l("/dev/video0", info);
    //
    std::cout << "check here 3\n";
    V4L2Encoder v4le("/dev/video2", {
                                        1280,
                                        720,
                                        30,
                                        1,
                                        false,
                                        V4L2_PIX_FMT_BGR24,
                                        V4L2_PIX_FMT_YUYV,
                                    },
                     false);

    V4L2Encoder v4leOut("/dev/video2", {
                                           1280,
                                           720,
                                           30,
                                           1,
                                           false,
                                           V4L2_PIX_FMT_BGR24,
                                           V4L2_PIX_FMT_YUYV,
                                       },
                        false);

    V4l2Data datain = v4le.V4l2DataGet();
    V4l2Data dataOut = v4le.V4l2DataGet();
    V4l2Data datain2;
    V4l2Data dataOut2;
    datain.data[0] = 0xfe;

    // std::thread in = std::thread(
    //     [&]
    //     {
    while (true)
    {
        std::cout << "check out:" << (int)datain.data[0] << "\n";
        v4le.V4L2EncodeSet(datain, dataOut);
        datain.data[0]--;
        std::cout << "check in:" << (int)dataOut.data[0] << "\n";
        usleep(1000 * 1000);
    }
    // });

    // std::thread out = std::thread(
    //     [&]
    //     {
    //         while (true)
    //         {
    //             // dataOut2.data[0] = 0xf3;
    //             // v4leOut.V4L2EncodeSet(dataOut2, datain2);
    //             // std::cout << "check in 2:" << (int)datain2.data[0] << "\n";
    //             usleep(1000 * 1000);
    //         }
    //     });

    // in.join();
    // out.join();

    // V4l2Data data;
    // TAVCodecCtx *AVCTX = new TAVCodecCtx;
    // TAVCodecInfo AVINF = {
    //     .width = 1280,
    //     .height = 720,
    //     .framerate = 30,
    //     .inputPixfmt = AV_PIX_FMT_NV12,
    // };

    // AVCodecInit(AVCTX, AVINF);

    // // f = fopen("stream.h264", "wb");

    // for (size_t i = 0; i < 120; i++)
    // {
    //     v4l.V4L2Read(data);

    //     pkt = AVCodecPushFrame2(AVCTX, AVINF, data.data, data.bytesperline);
    //     // fwrite(pkt.data, 1, pkt.size, f);
    // }

    // // fwrite(endcode, 1, sizeof(endcode), f);
    // // fclose(f);

    return 0;
}
