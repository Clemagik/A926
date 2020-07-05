# Copyright 2005 The Android Open Source Project
#
# Android.mk for adb
#

LOCAL_PATH:= $(call my-dir)

# adb host tool
# =========================================================
# Default to a virtual (sockets) usb interface
# =========================================================
include $(CLEAR_VARS)

LOCAL_SRC_FILES := \
	adb/adb.c \
	adb/backup_service.c \
	adb/fdevent.c \
	adb/transport.c \
	adb/transport_local.c \
	adb/transport_usb.c \
	adb/adb_auth_client.c \
	adb/sockets.c \
	adb/services.c \
	adb/file_sync_service.c \
	adb/jdwp_service.c \
	adb/framebuffer_service.c \
	adb/remount_service.c \
	adb/usb_linux_client.c \
	adb/log_service.c	\
	adb/utils.c

LOCAL_CFLAGS := -O2 -DADB_HOST=0 -Wall -Wno-unused-parameter
LOCAL_CFLAGS += -D_XOPEN_SOURCE -D_GNU_SOURCE -DADB_TRACING -DALLOW_ADBD_ROOT -D__LINUX__ -DALLOW_ADBD_ROOT=1
LOCAL_LDLIBS = -lpthread -lc
LOCAL_C_INCLUDES := adb/ include
LOCAL_MODULE := adbd
LOCAL_MODULE_TAGS := optional

LOCAL_STATIC_LIBRARIES := libcutils.a
LOCAL_SHARED_LIBRARIES :=libdlog
LOCAL_MODULE_PATH := $(TARGET_FS_BUILD)/sbin
include $(BUILD_EXECUTABLE)


include $(CLEAR_VARS)

LOCAL_MODULE := adb_device_model1
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_PATH := $(TARGET_FS_BUILD)
LOCAL_COPY_FILES := etc/init.d/S90adb:adbd/S90adb	\
		    etc/init.d/adb:adbd/adb	\
		    sbin/adbserver.sh:adbd/adbserver.sh
include $(BUILD_MULTI_COPY)


