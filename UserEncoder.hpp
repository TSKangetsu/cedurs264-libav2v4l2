#include "Drive_V4L2Reader.hpp"

using namespace V4L2Tools;

extern "C" void UserEncoderInit(V4l2Info info);
extern "C" void UserEncoderExChange(V4l2Data &input, V4l2Data &output);