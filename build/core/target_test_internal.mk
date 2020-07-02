#######################################################
## Shared definitions for all target test compilations.
#######################################################

#LOCAL_CFLAGS += -DGTEST_OS_LINUX_ANDROID -DGTEST_HAS_STD_STRING

#LOCAL_C_INCLUDES += external/gtest/include

LOCAL_STATIC_LIBRARIES += libgtest_target.a
LOCAL_LDLIBS = -lpthread -lstdc++ -lgcc -lc




