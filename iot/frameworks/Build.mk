LOCAL_PATH := $(my-dir)
#############################################
include $(CLEAR_VARS)
LOCAL_MODULE := iotdev-exe
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS:= DIR
LOCAL_MODULE_PATH := $(OUT_DEVICE_BINRARY_DIR)
LOCAL_MODULE_DIR  := exe
include $(BUILD_MULTI_COPY)

#############################################
include $(CLEAR_VARS)
LOCAL_MODULE := iotdev-lib
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS:= DIR
LOCAL_MODULE_PATH := $(OUT_DEVICE_SHARED_DIR)
LOCAL_MODULE_DIR  := lib
include $(BUILD_MULTI_COPY)

#############################################
include $(CLEAR_VARS)
LOCAL_MODULE := iotdev-include
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS:= DIR
LOCAL_MODULE_PATH := $(OUT_DEVICE_INCLUDE_DIR)
LOCAL_MODULE_DIR  := include
include $(BUILD_MULTI_COPY)

#############################################
include $(CLEAR_VARS)
LOCAL_MODULE := iotdev-etc
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS:= DIR
LOCAL_MODULE_PATH := $(TARGET_FS_BUILD)/etc
LOCAL_MODULE_DIR  := etc
include $(BUILD_MULTI_COPY)

#############################################
include $(CLEAR_VARS)
LOCAL_MODULE := iotdev-firmware
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS:= DIR
LOCAL_MODULE_PATH := $(TARGET_FS_BUILD)/firmware
LOCAL_MODULE_DIR  := firmware

LOCAL_DEPANNER_MODULES := jzrtspserver

include $(BUILD_MULTI_COPY)

#include $(call all-subdir-makefiles)
include $(call all-makefiles-under,$(LOCAL_PATH))
