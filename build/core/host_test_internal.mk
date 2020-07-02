#####################################################
## Shared definitions for all host test compilations.
#####################################################

LOCAL_CFLAGS = -DGTEST_OS_LINUX
LOCAL_LDLIBS = -lpthread -lstdc++

LOCAL_STATIC_LIBRARIES = libgtest_host.a
