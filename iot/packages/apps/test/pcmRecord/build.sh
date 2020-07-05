#!/bin/sh

#mips-linux-gnu-g++ -DHAVE_PTHREADS -DHAVE_ANDROID_OS -std=c++11 -fpermissive -lstdc++ -lrt -lpthread -lm -lc -ldlog
mips-linux-gnu-g++ -I/backup/home/jkzhao/work/task/project/brillo_x2000/iot/packages/apps/test/pcmRecord -I/backup/home/jkzhao/work/task/project/brillo_x2000/out/product/brillo/obj/buildroot-intermediate/host/mipsel-buildroot-linux-gnu/sysroot/usr/include ingenic_AudioRecordTest.cpp g711.cpp -o libingenic_AudioRecordTest.so --shared -fPIC -std=c++11 -fpermissive -march=xburst2 -mfp64
