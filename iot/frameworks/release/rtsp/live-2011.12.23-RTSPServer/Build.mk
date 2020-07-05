LOCAL_PATH := $(my-dir)
include $(CLEAR_VARS)

LOCAL_MODULE=jzrtspserver
LOCAL_MODULE_TAGS :=optional
LOCAL_MODULE_GEN_BINRARY_FILES:=jz_rtspServer/RTSPServer
LOCAL_MODULE_GEN_STATIC_FILES := groupsock/libgroupsock.a \
				liveMedia/libliveMedia.a \
				BasicUsageEnvironment/libBasicUsageEnvironment.a \
				UsageEnvironment/libUsageEnvironment.a
LOCAL_MODULE_COMPILE :=make -j$(MAKE_JLEVEL) \
		EXT_SHAREDS=$(ABS_DEVICE_SHARED_DIR) \
		EXT_ROOT=$(ROOT_PATH) \
		EXT_INCLUDE="$(ABS_DEVICE_INCLUDE_DIR) -I$(ABS_DEVICE_INCLUDE_DIR)/media/openmax" \
		EXT_LIB=$(ABS_DEVICE_STATIC_DIR);	\
		mkdir -p $(ABS_DEVICE_INCLUDE_DIR)/live555;\
		cp -rf liveMedia/include/* $(ABS_DEVICE_INCLUDE_DIR)/live555;\
		cp -rf BasicUsageEnvironment/include/* $(ABS_DEVICE_INCLUDE_DIR)/live555;\
		cp -rf UsageEnvironment/include/* $(ABS_DEVICE_INCLUDE_DIR)/live555;\
		cp -rf groupsock/include/* $(ABS_DEVICE_INCLUDE_DIR)/live555;

LOCAL_MODULE_COMPILE_CLEAN=make clean

LOCAL_SHARED_LIBRARIES := libiotdevcamera-h264

include $(BUILD_THIRDPART)
