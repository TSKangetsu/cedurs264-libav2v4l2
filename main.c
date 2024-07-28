#include <libavcodec/avcodec.h>

int main(int argc, char const *argv[])
{
    AVPacket *data;
    av_init_packet(data);
    return 0;
}
