###########################################################
## Standard rules for copying files.
###########################################################


LOCAL_MODULE_PATH := $(strip $(LOCAL_MODULE_PATH))
__local_path := $(LOCAL_PATH)
__out_module_dir := $(LOCAL_MODULE_PATH)
__local_stamp_build := $(LOCAL_STAMP_BUILD)
__local_stamp_install := $(LOCAL_STAMP_INSTALL)

include $(BUILD_SYSTEM)/choose_src_target_path.mk
include $(BUILD_SYSTEM)/module_install.mk
include $(BUILD_SYSTEM)/mma_build.mk
$(call choose_src_target_path, $(LOCAL_COPY_FILES))


__local_depanner := \
	$(addsuffix -intermediate/stamp_install, \
		$(addprefix $(OUT_DEVICE_OBJ_DIR)/DEPANNER/, \
		$(COPY_PREFIX_LOCAL_DEPANNER_MODULES))) \
	$(addsuffix -intermediate/stamp_install, \
		$(addprefix $(OUT_DEVICE_OBJ_DIR)/SHARED_LIBRARIES/, \
		$(basename $(LOCAL_SHARED_LIBRARIES)))) \
	$(addsuffix -intermediate/stamp_install, \
		$(addprefix $(OUT_DEVICE_OBJ_DIR)/STATIC_LIBRARIES/, \
		$(basename $(LOCAL_STATIC_LIBRARIES))))

ifneq (x$(LOCAL_MODULE_OUT_DIR),x)
__out_module_dir := $(strip $(__out_module_dir)/$(LOCAL_MODULE_OUT_DIR))
endif

__out_module_file := $(__out_module_dir)/$(LOCAL_TARGET_FILES)
__local_module_file := $(LOCAL_PATH)/$(LOCAL_SRC_FILES)

ifeq ($(LOCAL_MODULE_CLASS),DIR)
__out_module_file := $(__out_module_dir)
ifeq (x$(LOCAL_MODULE_OUT_DIR),x)
__out_module := $(__local_stamp_build)@$(__out_module_file).dir
$(LOCAL_MODULE_BUILD): $(__local_stamp_install)
$(__local_module_file): $(__local_depanner)
$(__out_module):$(__local_module_file)
	$(eval __dest := $(subst .dir,, $(word 2, $(subst @, , $@))))
	$(hide) mkdir -p $(__dest)
	$(hide) cp -Tadrf $< $(__dest)
	$(hide) /usr/bin/install -D /dev/null $@
else
__out_module := $(__local_stamp_build)@$(__out_module_file).dir
$(LOCAL_MODULE_BUILD): $(__local_stamp_install)
$(__local_module_file): $(__local_depanner)
# For directory rename, __dest is <out_dest_dir>/$LOCAL_MODULE_DIR.
# though $(dir $(__dest)) get realy <out_dest_dir>
$(__out_module):$(__local_module_file)
	$(eval __dest := $(subst .dir,, $(word 2, $(subst @, , $@))))
	$(hide) mkdir -p $(dir $(__dest))
	$(hide) cp -Tadrf $< $(dir $(__dest))
	$(hide) /usr/bin/install -D /dev/null $@
endif
else
__out_module := $(__local_stamp_build)@$(__out_module_file)
$(LOCAL_MODULE_BUILD): $(__local_stamp_install)
$(__local_module_file): $(__local_depanner)
$(__out_module):$(__local_module_file)
	$(eval __dest := $(word 2, $(subst @, , $@)))
	$(hide) mkdir -p $(dir $(__dest))
	$(hide) cp -adr  $< $(__dest)
	$(hide) /usr/bin/install -D /dev/null $@
endif
$(__local_stamp_build): $(__out_module)

CLEAN_DEP_FILES.$(LOCAL_MODULE_BUILD) += $(__out_module_file)
ifneq ($(LOCAL_FILTER_MODULE),)
ALL_COPY_MODULES += $(__out_module)
ALL_MODULES += $(COPY_PREFIX_LOCAL_DEPANNER_MODULES)
endif
