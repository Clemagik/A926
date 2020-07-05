LOCAL_PATH := $(my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE=libiotdevcamera-h264
LOCAL_MODULE_TAGS :=optional
LOCAL_SRC_FILES:= CameraRecorder.cpp \
	x264/x264_x2000.c
LOCAL_CFLAGS:= -O2 -Wall -march=xburst2 -mfp64 -fPIC \
	-std=gnu++11 -DLINUX -DHAVE_ANDROID_OS
LOCAL_EXPORT_C_INCLUDE_FILES:= CameraRecorder.h
LOCAL_MODULE_COMPILE_CLEAN=

LOCAL_DEPANNER_MODULES := \
	iotdev-include \
	iotdev-lib \

LOCAL_LDLIBS := \
	-lcutils -lutils -ldlog -lstagefright

LOCAL_C_INCLUDES := \
	x264 \
	../../../../../$(OUT_DEVICE_INCLUDE_DIR)/media/openmax \

include $(BUILD_SHARED_LIBRARY)
