extern "C"
{
#include "AvEncoder.h"
}

#include <map>
#include <csignal>
#include <thread>
#include "FlowController.hpp"
#include "CameraDrive/Drive_V4L2Reader.hpp"

using namespace V4L2Tools;
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

int main(int argc, char const *argv[])
{
    if (argc >= 7) // dev width height fps qp
    {
        // std::signal(SIGINT,
        //             [](int sig) -> void
        //             {
        //                 threadss->FlowStopAndWait();
        //                 // fclose(f);
        //                 exit(0);
        //             });

        // f = fopen("stream.h264", "wb");

        V4L2Encoder v4leOut(argv[1], {
                                         std::atoi(argv[2]),
                                         std::atoi(argv[3]),
                                         std::atoi(argv[4]),
                                         1,
                                         false,
                                         V4L2_PIX_FMT_H264,
                                         V4L2Format_s.at(argv[6]),
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
            .inputPixfmt = FFMPEGFormat_s.at(argv[6]),
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

// #include <stdio.h>
// #include <stdlib.h>
// #include <string.h>
// #include <fcntl.h>
// #include <unistd.h>
// #include <errno.h>
// #include <sys/ioctl.h>
// #include <sys/mman.h>
// #include <linux/videodev2.h>

// #define DEVICE "/dev/video1" // Replace with your video device
// #define WIDTH  640
// #define HEIGHT 480
// #define BUFFER_COUNT 4

// struct buffer {
//     void   *start;
//     size_t length;
// };

// int main() {
//     int fd = open(DEVICE, O_RDWR);
//     if (fd < 0) {
//         perror("Failed to open device");
//         return -1;
//     }

//     // Set the video format
//     struct v4l2_format fmt = {0};
//     fmt.type = V4L2_BUF_TYPE_VIDEO_OUTPUT; // Output queue type
//     fmt.fmt.pix.width = WIDTH;
//     fmt.fmt.pix.height = HEIGHT;
//     fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_NV12; // Change to the required format
//     fmt.fmt.pix.field = V4L2_FIELD_ANY;

//     if (ioctl(fd, VIDIOC_S_FMT, &fmt) < 0) {
//         perror("Failed to set format");
//         close(fd);
//         return -1;
//     }

//     // Request buffers
//     struct v4l2_requestbuffers req = {0};
//     req.count = BUFFER_COUNT;
//     req.type = V4L2_BUF_TYPE_VIDEO_OUTPUT;
//     req.memory = V4L2_MEMORY_USERPTR;

//     if (ioctl(fd, VIDIOC_REQBUFS, &req) < 0) {
//         perror("Failed to request buffers");
//         close(fd);
//         return -1;
//     }

//     // Allocate user-space buffers
//     struct buffer buffers[BUFFER_COUNT];
//     for (int i = 0; i < BUFFER_COUNT; ++i) {
//         buffers[i].length = WIDTH * HEIGHT * 2; // Adjust size for YUYV format
//         buffers[i].start = malloc(buffers[i].length);
//         if (!buffers[i].start) {
//             perror("Failed to allocate memory");
//             close(fd);
//             return -1;
//         }
//         memset(buffers[i].start, 0x80, buffers[i].length); // Initialize with dummy data
//     }

//     // Queue the buffers
//     for (int i = 0; i < BUFFER_COUNT; ++i) {
//         struct v4l2_buffer buf = {0};
//         buf.type = V4L2_BUF_TYPE_VIDEO_OUTPUT;
//         buf.memory = V4L2_MEMORY_USERPTR;
//         buf.index = i;
//         buf.m.userptr = (unsigned long)buffers[i].start;
//         buf.length = buffers[i].length;

//         if (ioctl(fd, VIDIOC_QBUF, &buf) < 0) {
//             perror("Failed to queue buffer");
//             close(fd);
//             return -1;
//         }
//     }

//     // Start streaming
//     enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_OUTPUT;
//     if (ioctl(fd, VIDIOC_STREAMON, &type) < 0) {
//         perror("Failed to start streaming");
//         close(fd);
//         return -1;
//     }

//     // Simulate frame submission
//     for (int i = 0; i < 10; ++i) {
//         printf("check\n");
//         struct v4l2_buffer buf = {0};
//         buf.type = V4L2_BUF_TYPE_VIDEO_OUTPUT;
//         buf.memory = V4L2_MEMORY_USERPTR;

//         if (ioctl(fd, VIDIOC_DQBUF, &buf) < 0) {
//             perror("Failed to dequeue buffer");
//             break;
//         }

//         // Process buffer and re-queue
//         // memset((void *)buf.m.userptr, 0xAA, buffers[buf.index].length); // Simulated new frame data

//         if (ioctl(fd, VIDIOC_QBUF, &buf) < 0) {
//             perror("Failed to queue buffer");
//             break;
//         }
//     }

//     // Stop streaming
//     if (ioctl(fd, VIDIOC_STREAMOFF, &type) < 0) {
//         perror("Failed to stop streaming");
//     }

//     // Free allocated buffers
//     for (int i = 0; i < BUFFER_COUNT; ++i) {
//         free(buffers[i].start);
//     }

//     close(fd);
//     return 0;
// }