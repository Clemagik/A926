LOCAL_PATH := $(my-dir)
include $(CLEAR_VARS)
#WITH_DEBUG := true
LOCAL_MODULE = PcmRecord
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_PATH := $(OUT_DEVICE_BINRARY_DIR)
LOCAL_SRC_FILES := AudioRecordTest.cpp

LOCAL_CFLAGS += -DHAVE_PTHREADS -DHAVE_ANDROID_OS -std=c++11 \
	-fpermissive
LOCAL_LDFLAGS := -rpath-link=$(OUT_DEVICE_SHARED_DIR)
LOCAL_LDLIBS := -lstdc++ -lrt -lpthread -lm -lc -ldlog

LOCAL_DEPANNER_MODULES += \
	dlog \

LOCAL_SHARED_LIBRARIES += \
	libcutils \
	libutils \
	libmedia \
	libbinder

include $(BUILD_EXECUTABLE)
