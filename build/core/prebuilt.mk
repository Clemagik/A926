###########################################################
## Standard rules for copying files that are prebuilt
##
## Additional inputs from base_rules.make:
## None.
##
###########################################################

LOCAL_MODULE_BUILD := $(LOCAL_MODULE)
ALL_MODULES_CLEAN += $(LOCAL_MODULE)
__local_path := $(LOCAL_PATH)

ifeq ($(strip $(LOCAL_TARGET_MODULE_CLASS)),)
LOCAL_TARGET_MODULE_CLASS := DEPANNER
endif

$(eval $(call define-intermediate,$(OUT_DEVICE_OBJ_DIR),$(LOCAL_TARGET_MODULE_CLASS),$(LOCAL_MODULE_BUILD)))

ifneq ($(strip x$(LOCAL_DEPANNER_MODULES)),x)
$(warning "Prebuilt can't have depanner modules.")
endif

include $(BUILD_SYSTEM)/choose_src_target_path.mk

$(call choose_src_target_path, $(LOCAL_COPY_FILES))

__out_module_file := $(LOCAL_MODULE_PATH)/$(LOCAL_TARGET_FILES)
__local_module_file :=$(LOCAL_PATH)/$(LOCAL_SRC_FILES)

include $(BUILD_SYSTEM)/module_install.mk
include $(BUILD_SYSTEM)/mma_build.mk

__out_module := $(__local_stamp_build)@$(__out_module_file)
$(LOCAL_MODULE_BUILD): $(__local_stamp_install)

$(__local_stamp_install): $(__local_stamp_build)
	$(hide) install -D /dev/null $@
$(__local_stamp_build): $(__out_module)


$(__out_module):$(__local_module_file)
	$(eval __dest := $(word 2, $(subst @, , $@)))
	$(hide) mkdir -p $(dir $(__dest))
	$(hide) cp -adr $< $(__dest)
	$(hide) /usr/bin/install -D /dev/null $@

CLEAN_DEP_FILES.$(LOCAL_MODULE_BUILD) := $(__out_module_file)
CLEAN_DEP_FILES.$(LOCAL_MODULE_BUILD) += $(__local_intermediate)
$(LOCAL_MODULE_BUILD)-clean:PRIVATE_MODULE_FILES_CLEAN:=$(CLEAN_DEP_FILES.$(LOCAL_MODULE_BUILD))
ifneq ($(LOCAL_FILTER_MODULE),)
ALL_PREBUILT_MODULES += $(__out_module)
endif
