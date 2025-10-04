#include "UserEncoder.hpp"
#include <iostream>
#include <map>
extern "C"
{
#include "AvEncoder.h"
}

#define EMAP(Variable) (#Variable)

V4l2Data innerVdata;
TAVCodecCtx *AVCTX;
TAVCodecInfo AVINF;
AVPacket pkt;

inline static const std::map<unsigned int, int> V4L2_to_AV_Format =
    {
        {V4L2_PIX_FMT_YUYV, AV_PIX_FMT_YUYV422},
        {V4L2_PIX_FMT_YUV420, AV_PIX_FMT_YUV420P},
        {V4L2_PIX_FMT_BGR24, AV_PIX_FMT_BGR24},
        {V4L2_PIX_FMT_NV12, AV_PIX_FMT_NV12},
};

void UserEncoderInit(V4l2Info info)
{
    std::cout << "[USER ENCODER] Check enc init\n";

    AVPixelFormat targetOption = (AVPixelFormat)V4L2_to_AV_Format.at(info.PixFormat);
    AVCTX = new TAVCodecCtx;
    AVINF = {
        .qp = (char *)std::to_string(info.H264_Bitrate).c_str(),
        .width = info.ImgWidth,
        .height = info.ImgHeight,
        .framerate = info.FrameRate,
        .inputPixfmt = targetOption,
    };
    AVCodecInit(AVCTX, AVINF);

    innerVdata = V4l2Data(
        info.ImgWidth,
        info.ImgWidth,
        info.ImgHeight * info.ImgWidth * 3,
        info.ImgHeight * info.ImgWidth * 3,
        V4L2_PIX_FMT_H264,
        0,
        false,
        -1);
}

void UserEncoderExChange(V4l2Data &input, V4l2Data &output)
{
    innerVdata.size = 0;
    pkt = AVCodecPushFrame2(AVCTX, AVINF, input.data, input.bytesperline);
    std::copy(pkt.data, pkt.data + pkt.size, innerVdata.data);
    innerVdata.size = pkt.size;
    output.ismapping = true;
    innerVdata.ismapping = true;
    output = innerVdata;

    // std::cout << "[user encoder] check enc run: " << pkt.size << " " << input.size << " " << input.bytesperline << "\n ";
}