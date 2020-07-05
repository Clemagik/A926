##########################################################
# File Name		: build.sh
# Author		: JKZhao
# Created Time	: 2020年06月04日 星期四 10时35分37秒
# Description	:
##########################################################
#!/bin/bash
mips-linux-gnu-g++ -march=xburst2 -mfp64 -I/home1/jkzhao/task/project/01_xxz/20200526-xxz_iotdev_customer-20200526/xxz_iotdev_customer-20200526/out/product/xxzA926/obj/buildroot-intermediate/host/usr/mipsel-buildroot-linux-gnu/sysroot/usr/include -I/home1/jkzhao/task/project/01_xxz/20200526-xxz_iotdev_customer-20200526/xxz_iotdev_customer-20200526/out/product/xxzA926/obj/buildroot-intermediate/host/usr/mipsel-buildroot-linux-gnu/sysroot/usr/include/media/openmax wrapperiotdevcamera.cpp -L /home1/jkzhao/task/project/01_xxz/20200526-xxz_iotdev_customer-20200526/xxz_iotdev_customer-20200526/out/product/xxzA926/system/usr/lib -o libwrapperiotdevcamera.so --shared -fPIC -lstdc++ -DLINUX -std=c++11 -liotdevcamera-h264
