#!/bin/bash

#下边环境变量是互斥的关系
export GUI_MINIGUI=N
export GUI_DIRECTFB=N
export PATH=$PATH:$(pwd)/iot/build
export AWTK_ACCEL=M200_BRILLO

source iot/build/iotpatch.sh
