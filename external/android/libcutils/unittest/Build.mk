LOCAL_PATH := $(my-dir)
include $(CLEAR_VARS)

LOCAL_MODULE := libcutils_test
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_PATH:=$(TARGET_FS_BUILD)/$(TARGET_TESTSUIT_DIR)/$(LOCAL_MODULE)
LOCAL_SRC_FILES := MemsetTest.cpp \
				PropertiesTest.cpp
LOCAL_CFLAGS := -D__STDC_LIMIT_MACROS -D__STDC_CONSTANT_MACROS
LOCAL_DEPANNER_MODULES := dlog libcutils libgtest_target
LOCAL_STATIC_LIBRARIES := libcutils.a
LOCAL_SHARED_LIBRARIES := libdlog.so
LOCAL_LDLIBS := -lc -lstdc++ -ldlog

include $(BUILD_NATIVE_TEST)

