###########################################################
## Standard rules for copying files that are prebuilt
##
## Additional inputs from base_rules.make:
## None.
##
###########################################################

__local_stamp_build := $(LOCAL_STAMP_BUILD)
__local_stamp_install := $(LOCAL_STAMP_INSTALL)
__local_path := $(LOCAL_PATH)
include $(BUILD_SYSTEM)/choose_src_target_path.mk

$(call choose_src_target_path, $(LOCAL_COPY_FILES))

__out_module_file := $(LOCAL_MODULE_PATH)/$(LOCAL_TARGET_FILES)
__local_module_file :=$(LOCAL_PATH)/$(LOCAL_SRC_FILES)

include $(BUILD_SYSTEM)/module_install.mk
include $(BUILD_SYSTEM)/mma_build.mk

ifeq ($(LOCAL_MODULE_CLASS),DIR)
__out_module := $(__local_stamp_build)@$(__out_module_file).dir@
$(LOCAL_MODULE_BUILD): $(__local_stamp_install)
$(__out_module):$(__local_module_file)
	$(eval __dest := $(subst .dir,, $(word 2, $(subst @, , $@))))
	$(hide) mkdir -p $(__dest)
	$(hide) cp -adrf $< $(dir $(__dest))
	$(hide) /usr/bin/install -D /dev/null $@
else
__out_module := $(__local_stamp_build)@$(__out_module_file)
$(LOCAL_MODULE_BUILD): $(__local_stamp_install)
$(__out_module):$(__local_module_file)
	$(eval __dest := $(word 2, $(subst @, , $@)))
	$(hide) mkdir -p $(dir $(__dest))
	$(hide) cp -adr $< $(__dest)
	$(hide) /usr/bin/install -D /dev/null $@
endif
$(__local_stamp_build): $(__out_module)

CLEAN_DEP_FILES.$(LOCAL_MODULE_BUILD) += $(__out_module_file)
ifneq ($(LOCAL_FILTER_MODULE),)
ALL_PREBUILT_MODULES += $(__out_module)
endif
