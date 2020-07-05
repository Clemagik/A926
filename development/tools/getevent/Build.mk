LOCAL_PATH := $(my-dir)
include $(CLEAR_VARS)

LOCAL_MODULE = getevent
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_PATH := $(TARGET_FS_BUILD)/sbin
LOCAL_SRC_FILES := getevent.c
LOCAL_LDLIBS :=  -lc

include $(BUILD_EXECUTABLE)
