LOCAL_PATH:= $(call my-dir)

commonSources:= BacktraceImpl.cpp \
	BacktraceMap.cpp \
	BacktraceThread.cpp \
	UnwindCurrent.cpp \
	UnwindMap.cpp \
	UnwindPtrace.cpp \
	thread_utils.c \

include $(CLEAR_VARS)

LOCAL_SRC_FILES:= $(commonSources)
LOCAL_MODULE:= libbacktrace
LOCAL_MODULE_TAGS := optional
LOCAL_CPPFLAGS := -std=gnu++11 \
	-D__STDC_FORMAT_MACROS
LOCAL_SHARED_LIBRARIES := libdlog.so libunwind.so
LOCAL_DEPANNER_MODULES := android-include dlog
LOCAL_CFLAGS += -Wa,-mips32r2 -O2 -G 0 -Wall  -D_GNU_SOURCE=1 -DHAVE_CONFIG_H
LOCAL_LDLIBS += -lc -lstdc++ -lpthread -lrt -lunwind
include $(BUILD_STATIC_LIBRARY)


include $(CLEAR_VARS)

LOCAL_SRC_FILES:= $(commonSources)
LOCAL_MODULE:= libbacktrace
LOCAL_MODULE_TAGS := optional
LOCAL_CPPFLAGS := -std=gnu++11 \
	-D__STDC_FORMAT_MACROS
LOCAL_SHARED_LIBRARIES := libdlog.so libunwind.so
LOCAL_DEPANNER_MODULES := android-include dlog
LOCAL_MODULE_GEN_SHARED_FILES:=libbacktrace.so
LOCAL_CFLAGS += -Wa,-mips32r2 -O2 -G 0 -Wall -shared -fPIC -D_GNU_SOURCE=1 -DHAVE_CONFIG_H
LOCAL_LDLIBS += -lc -lstdc++ -lpthread -lrt -lunwind
include $(BUILD_SHARED_LIBRARY)
