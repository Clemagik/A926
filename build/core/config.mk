
include $(BUILD_SYSTEM)/envsetup.mk
CLEAR_VARS:= $(BUILD_SYSTEM)/clear_vars.mk


BUILD_SHARED_LIBRARY:= $(BUILD_SYSTEM)/device_shared_library.mk
BUILD_STATIC_LIBRARY:= $(BUILD_SYSTEM)/device_static_library.mk
BUILD_EXECUTABLE:= $(BUILD_SYSTEM)/executable.mk
BUILD_STATIC_EXECUTABLE:= $(BUILD_SYSTEM)/static_executable.mk
BUILD_THIRDPART:=$(BUILD_SYSTEM)/device_thirdpart.mk
BUILD_KERNEL:=$(BUILD_SYSTEM)/build_kernel.mk
BUILD_UBOOT:=$(BUILD_SYSTEM)/build_uboot.mk
BUILD_CMAKE_DEVICE:=$(BUILD_SYSTEM)/build_cmake.mk
BUILD_BUSYBOX:=$(BUILD_SYSTEM)/build_busybox.mk
BUILD_KERNEL_MODULE:=$(BUILD_SYSTEM)/build_kernel_module.mk

ifeq ($(strip x$(SDK_BUILD)),x)
BUILD_PREBUILT:=$(BUILD_SYSTEM)/prebuilt.mk
BUILD_MULTI_PREBUILT := $(BUILD_SYSTEM)/multi_prebuilt.mk
BUILD_MULTI_COPY := $(BUILD_SYSTEM)/multi_copy.mk
endif

BUILD_HOST_STATIC_LIBRARY:=$(BUILD_SYSTEM)/host_static_library.mk
BUILD_HOST_SHARED_LIBRARY:=$(BUILD_SYSTEM)/host_shared_library.mk
BUILD_HOST_EXECUTABLE:= $(BUILD_SYSTEM)/host_executable.mk
BUILD_HOST_THIRDPART:=$(BUILD_SYSTEM)/host_thirdpart.mk

#gtest#
BUILD_HOST_NATIVE_TEST := $(BUILD_SYSTEM)/host_native_test.mk
BUILD_NATIVE_TEST := $(BUILD_SYSTEM)/native_test.mk

define my-dir
$(strip \
  $(eval LOCAL_MODULE_MAKEFILE := $$(lastword $$(MAKEFILE_LIST))) \
  $(if $(filter $(BUILD_SYSTEM)/% $(OUT_DIR)/%,$(LOCAL_MODULE_MAKEFILE)), \
    $(error my-dir must be called before including any other makefile.) \
   , \
    $(patsubst %/,%,$(dir $(LOCAL_MODULE_MAKEFILE))) \
   ) \
 )
endef

define all-makefiles-under
$(wildcard $(1)/*/Build.mk)
endef

define all-subdir-makefiles
$(call all-makefiles-under,$(call my-dir))
endef
