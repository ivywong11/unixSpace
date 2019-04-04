#!/bin/bash
#make clean
export TMPDIR=H:/ffmpeg-3.1.3/ffmpegtemp #设置编译中临时文件目录，不然会报错 unable to create temporary file
# NDK的路径，根据实际安装位置设置
export NDK=E:/android-ndk-r12b
# 编译针对的平台，这里选择最低支持android-14, arm架构，生成的so库是放在libs/armeabi文件夹下的，若针对x86架构，要选择arch-x86
export SYSROOT=$NDK/platforms/android-24/arch-arm
# 工具链的路径，arm-linux-androideabi-4.9与上面设置的PLATFORM对应，4.9为工具的版本号
export TOOLCHAIN=$NDK/toolchains/arm-linux-androideabi-4.9/prebuilt/windows-x86_64
export CPU=arm
export PREFIX=./android/$CPU
export ADDI_CFLAGS="-marm"
./configure --target-os=android \
--prefix=$PREFIX --arch=arm \
--disable-doc \
--enable-shared \
--disable-static \
--disable-yasm \
--disable-symver \
--disable-gpl \
--disable-ffmpeg \
--disable-ffplay \
--disable-ffprobe \
--disable-ffserver \
--cross-prefix=$TOOLCHAIN/bin/arm-linux-androideabi- \
--enable-cross-compile \
--sysroot=$SYSROOT \
--extra-cflags="-Os -fpic $ADDI_CFLAGS" \
$ADDITIONAL_CONFIGURE_FLAG
make clean
make
make install
