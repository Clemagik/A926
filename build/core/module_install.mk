# Only the tags mentioned in this test are expected to be set by module
# makefiles. Anything else is either a typo or a source of unexpected
# behaviors.

include $(TOPDIR)/device/$(TARGET_DEVICE)/$(TARGET_DEVICE)_base.mk
ifneq ($(filter-out $(ALL_SUPPORT_MODULE_TAGS),$(LOCAL_MODULE_TAGS)),)
$(warning unusual tags $(LOCAL_MODULE_TAGS) on $(LOCAL_MODULE) at $(LOCAL_PATH))
endif


ifeq ($(LOCAL_MODULE_TAGS),)
$(warning $(LOCAL_PATH) $(LOCAL_MODULE) not define LOCAL_MODULE_TAGS , we will give the default value - optional - to it)
LOCAL_MODULE_TAGS := optional
endif


LOCAL_FILTER_MODULE :=
__local_filter_base_modules:=
__local_filter_tags_modules:=
ifeq ($(filter-out $(tags_to_install),$(LOCAL_MODULE_TAGS)),)
__local_filter_tags_modules:= $(LOCAL_MODULE_BUILD)
endif


__local_filter_base_modules:= $(if $(filter $(LOCAL_MODULE_BUILD),$(PRODUCT_MODULES)),$(LOCAL_MODULE_BUILD),)

LOCAL_FILTER_MODULE := $(sort $(__local_filter_base_modules) $(__local_filter_tags_modules))

#$(info 11111111111111 $(all_module_tags))
#$(info 22222222222222 $(LOCAL_MODULE_TAGS))
#$(info 33333333333333 $(filter-out $(all_module_tags),$(LOCAL_MODULE_TAGS)))
#$(info 44444444444444 $(LOCAL_MODULE_BUILD))
#$(info 55555555555555 $(LOCAL_INSTALL_MODULE))
#$(info 66666666666666 $(LOCAL_FILTER_MODULE))

