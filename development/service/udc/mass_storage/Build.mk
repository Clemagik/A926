# Copyright 2005 The Android Open Source Project

LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_SRC_FILES:= \
    udc_mass_storage.c

LOCAL_MODULE:= udc_mass_storage
LOCAL_CFLAGS :=  -Wall -O2
LOCAL_LDLIBS = -lc
LOCAL_MODULE_TAGS := eng
LOCAL_MODULE_PATH := $(TARGET_FS_BUILD)/usr/bin
include $(BUILD_EXECUTABLE)

include $(CLEAR_VARS)

LOCAL_MODULE := udc_mass_storage_sh
LOCAL_MODULE_TAGS := eng
LOCAL_MODULE_PATH := $(TARGET_FS_BUILD)/usr/bin
LOCAL_COPY_FILES := udc_mass_storage.sh:udc_mass_storage.sh

include $(BUILD_MULTI_PREBUILT)
