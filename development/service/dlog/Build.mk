LOCAL_PATH := $(my-dir)

#===================================================
# compile the dlog_logger
# #
include $(CLEAR_VARS)
LOCAL_MODULE := dlog_logger
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := \
	src/shared/logprint.c \
	src/logger/logger.c

LOCAL_LDLIBS := -lc
LOCAL_C_INCLUDES:= include
LOCAL_MODULE_PATH:=$(OUT_DEVICE_BINRARY_DIR)
include $(BUILD_EXECUTABLE)
#===================================================
# compile the libdlog.a
# #
include $(CLEAR_VARS)
LOCAL_MODULE=libdlog
LOCAL_MODULE_TAGS:=optional
LOCAL_SRC_FILES:= \
	src/libdlog/log.c \
	src/libdlog/logconfig.c \
	src/libdlog/loglimiter.c

LOCAL_C_INCLUDES := include
LOCAL_CFLAGS := -O2 -Wall
include $(BUILD_STATIC_LIBRARY)
#===================================================
# compile the libdlog.so
# #
include $(CLEAR_VARS)
LOCAL_MODULE=libdlog
LOCAL_MODULE_TAGS:=optional
LOCAL_SRC_FILES:= \
	src/libdlog/log.c \
	src/libdlog/logconfig.c \
	src/libdlog/loglimiter.c

LOCAL_C_INCLUDES := include
LOCAL_CFLAGS := -O2 -Wall -fPIC
include $(BUILD_SHARED_LIBRARY)
#===================================================
# copy the dlog.h , tizen_error.h
# #
include $(CLEAR_VARS)
LOCAL_MODULE := dlog
LOCAL_MODULE_TAGS :=optional
LOCAL_MODULE_PATH :=$(OUT_DEVICE_INCLUDE_DIR)
LOCAL_COPY_FILES := dlog.h:include/dlog.h tizen_error.h:include/tizen_error.h logger.h:include/logger.h
LOCAL_SHARED_LIBRARIES:= libdlog
LOCAL_DEPANNER_MODULES := dlog_cfg  dlogctrl dlog_logger dlogutil S90dlog
include $(BUILD_MULTI_COPY)
#===================================================
# compile the dlogutil
# #
include $(CLEAR_VARS)
LOCAL_MODULE := dlogutil
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := \
	src/shared/logprint.c \
	src/logutil/logutil.c

LOCAL_LDLIBS := -lc
LOCAL_C_INCLUDES:= include
LOCAL_MODULE_PATH:=$(OUT_DEVICE_BINRARY_DIR)
include $(BUILD_EXECUTABLE)
#===================================================
# copy the dlogctrl
# #
include $(CLEAR_VARS)
LOCAL_MODULE := dlogctrl
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_PATH := $(OUT_DEVICE_BINRARY_DIR)
LOCAL_COPY_FILES := dlogctrl:scripts/dlogctrl
include $(BUILD_PREBUILT)
#===================================================
# copy the dlog.conf , dlog_logger.conf
# #
include $(CLEAR_VARS)
LOCAL_MODULE := dlog_cfg
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_PATH := $(TARGET_FS_BUILD)/usr/etc
LOCAL_COPY_FILES := dlog.conf:config/dlog.conf.in dlog_logger.conf:config/dlog_logger.conf.in
include $(BUILD_MULTI_PREBUILT)
#===================================================
# copy the S90dlog with start
# #
include $(CLEAR_VARS)
LOCAL_MODULE := S90dlog
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_PATH := $(TARGET_FS_BUILD)/etc/init.d
LOCAL_COPY_FILES := S90dlog:config/S90dlog
include $(BUILD_PREBUILT)
