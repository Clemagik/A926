LOCAL_PATH := $(my-dir)

#include $(call all-subdir-makefiles)
include $(call all-makefiles-under,$(LOCAL_PATH))
