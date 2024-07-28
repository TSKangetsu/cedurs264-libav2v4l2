cp -p ffmpeg-h3/cedrus264.c ffmpeg-cedurs/libavcodec/
cp -R ffmpeg-h3/sunxi ffmpeg-cedurs/libavcodec/arm

cd ffmpeg-cedurs 
./configure \
             --cc="arm-openwrt-linux-gcc" \
             --enable-cross-compile \
             --arch=arm \
             --target-os=linux \
             --enable-gpl \
             --enable-version3 \
             --disable-ffplay \
             --disable-ffprobe \
             --disable-ffserver

make -j32
make DESTDIR=/home/user/build/cedurs-h3-encoder/build  install
