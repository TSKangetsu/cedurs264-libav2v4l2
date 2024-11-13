cd thirdpart/

if [ -e check ]
then
    echo "check skiping ffmpeg"
    exit 1
fi

touch check

cp -p ffmpeg-h3/cedrus264.c ffmpeg-cedurs/libavcodec/
cp -R ffmpeg-h3/sunxi ffmpeg-cedurs/libavcodec/arm


cd ffmpeg-cedurs 

crs=$(find $1 -name "*gcc" | sed 's!.*/!!' | sed 's!gcc!!')

echo "check cross $crs"

./configure \
             --cross-prefix="$crs" \
             --enable-cross-compile \
             --arch=arm \
             --target-os=linux \
             --disable-runtime-cpudetect \
             --enable-pthreads \
             --enable-gpl \
             --disable-zlib \
             --enable-version3 \
             --disable-programs \
             --enable-ffmpeg \
             --enable-small

make -j32
mkdir -p `pwd`/../../build-install
make DESTDIR=`pwd`/../../build-install  install
