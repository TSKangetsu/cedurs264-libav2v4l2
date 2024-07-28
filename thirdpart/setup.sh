cd thirdpart/
cp -p ffmpeg-h3/cedrus264.c ffmpeg-cedurs/libavcodec/
cp -R ffmpeg-h3/sunxi ffmpeg-cedurs/libavcodec/arm

echo "check cross $1 $2"
cd ffmpeg-cedurs 
./configure \
             --cross-prefix="$1" \
             --enable-cross-compile \
             --arch=$2 \
             --target-os=linux \
             --enable-gpl \
             --enable-version3 \
             --disable-ffplay \
             --disable-ffprobe \
             --disable-ffserver

make -j32
mkdir -p `pwd`/../../build
make DESTDIR=`pwd`/../../build  install
