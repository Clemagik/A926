define check_micro
ifeq ( x$(strip $($(1))),x)
$(info Please define $(1) in $(LOCAL_PATH)/$(LOCAL_MODULE))
endif
endef

LOCAL_MODULE_BUILD:=$(PREFIX)$(LOCAL_MODULE)
ALL_MODULES_CLEAN += $(PREFIX)$(LOCAL_MODULE)
LOCAL_PATH := $(strip $(LOCAL_PATH))
LOCAL_MODULE_PATH := $(strip $(LOCAL_MODULE_PATH))
THIRDPART_DEPANNER_MODULES := $(strip $(addprefix $(PREFIX),$(LOCAL_DEPANNER_MODULES)))
PREFIX_LOCAL_DEPANNER_MODULES := $(THIRDPART_DEPANNER_MODULES)

ifeq ($(strip $(LOCAL_TARGET_MODULE_CLASS)),)
LOCAL_TARGET_MODULE_CLASS := DEPANNER
endif

$(eval $(call define-intermediate,$(OUT_DEVICE_OBJ_DIR),$(LOCAL_TARGET_MODULE_CLASS),$(LOCAL_MODULE_BUILD)))

$(call check_micro LOCAL_MODULE_CONFIG_FILES)
THIRDPART_MODULE_CONFIG_FILE := $(LOCAL_PATH)/$(strip $(LOCAL_MODULE_CONFIG_FILES))

__local_dep_stamp_build :=$(addsuffix -intermediate/stamp_build, \
	$(addprefix $(OUT_DEVICE_OBJ_DIR)/DEPANNER/, $(THIRDPART_DEPANNER_MODULES)))

__local_dep_stamp_install :=  \
	$(addsuffix -intermediate/stamp_install, \
		$(addprefix $(OUT_DEVICE_OBJ_DIR)/DEPANNER/, \
		$(THIRDPART_DEPANNER_MODULES))) \
	$(addsuffix -intermediate/stamp_install, \
		$(addprefix $(OUT_DEVICE_OBJ_DIR)/STATIC_LIBRARIES/, \
		$(basename $(LOCAL_STATIC_LIBRARIES)))) \
	$(addsuffix -intermediate/stamp_install, \
		$(addprefix $(OUT_DEVICE_OBJ_DIR)/SHARED_LIBRARIES/, \
		$(basename $(LOCAL_SHARED_LIBRARIES))))

__local_dep_stamp_install := $(if $(__local_dep_stamp_install), | $(__local_dep_stamp_install),)

__local_build_module := $(foreach t,$(ALL_GEN_FILE_TYPE),\
	$(addprefix $(LOCAL_PATH)/, $(LOCAL_MODULE_GEN_$(t)_FILES)))

__local_installed_module := $(foreach t,$(ALL_GEN_FILE_TYPE),\
	$(eval __local_module_path:= $(if $(LOCAL_MODULE_PATH),\
			$(LOCAL_MODULE_PATH),\
			$(OUT_$(BUILD_MODE)_$(t)_DIR))) \
	$(addprefix $(__local_stamp_build)@$(__local_module_path)/,$(notdir $(LOCAL_MODULE_GEN_$(t)_FILES))))

__local_installed_module := $(__local_installed_module)

THIRDPART_LOCAL_STATIC_LIBRARIES := $(addprefix $(OUT_$(BUILD_MODE)_STATIC_DIR)/,$(LOCAL_STATIC_LIBRARIES))
THIRDPART_LOCAL_SHARED_LIBRARIES := $(addprefix $(OUT_$(BUILD_MODE)_SHARED_DIR)/,$(LOCAL_SHARED_LIBRARIES))

#__local_export_compiler_path := $(if $(EXPORT_COMPILER_PATH),$(EXPORT_COMPILER_PATH);,)
#$(LOCAL_MODULE_BUILD):PRIVATE_CONFIG_COMMAND:=$(__local_export_compiler_path)cd $(LOCAL_PATH);$(LOCAL_MODULE_CONFIG)
#$(LOCAL_MODULE_BUILD):PRIVATE_COMPILER_COMMAND:=$(__local_export_compiler_path)cd $(LOCAL_PATH);$(LOCAL_MODULE_COMPILE)

include $(BUILD_SYSTEM)/module_install.mk

LOCAL_MODULE_COMPILE_CLEAN:=$(LOCAL_MODULE_COMPILE_CLEAN)
THIRDPART_DEPANNER_MODULES:=$(if $(LOCAL_FILTER_MODULE),$(THIRDPART_DEPANNER_MODULES),)
THIRDPART_LOCAL_SHARED_LIBRARIES:=$(if $(LOCAL_FILTER_MODULE),$(THIRDPART_LOCAL_SHARED_LIBRARIES),)
THIRDPART_LOCAL_STATIC_LIBRARIES:=$(if $(LOCAL_FILTER_MODULE),$(THIRDPART_LOCAL_STATIC_LIBRARIES),)
ALL_MODULES += $(LOCAL_FILTER_MODULE)

include $(BUILD_SYSTEM)/mma_build.mk

__local_module_clean:=

ifneq (x$(strip $(LOCAL_MODULE_COMPILE_CLEAN)),x)
$(LOCAL_MODULE_BUILD)-clean:PRIVATE_MODULE_FILES_CLEAN:=$(__local_installed_module) $(__local_intermediate)

__local_module_clean:= $(LOCAL_PATH)/$(LOCAL_MODULE_BUILD)-clean
$(__local_module_clean):PRIVATE_CLEAN_COMMAND:=$(__local_export_compiler_path) cd $(LOCAL_PATH);$(LOCAL_MODULE_COMPILE_CLEAN)
$(__local_module_clean):PRIVATE_CONFIG_COMMAND:=$(__local_export_compiler_path) cd $(LOCAL_PATH);$(LOCAL_MODULE_CONFIG)
$(__local_module_clean):PRIVATE_MODULE_FILES_CLEAN:=$(__local_installed_module) $(__local_intermediate)
$(LOCAL_MODULE_BUILD)-clean:$(__local_module_clean)

ifneq ($(strip x$(LOCAL_DISABLE_GLOBAL_CFLAGS)),xtrue)
__global_target_cflags := $(GLOBAL_$(BUILD_MODE)_CFLAGS)
else
__global_target_cflags :=
endif

CLEAN_DEP_FILES.$(LOCAL_MODULE_BUILD):=$(LOCAL_PATH)/$(LOCAL_MODULE_BUILD) $(__local_static_libraries) $(__local_static_libraries)
THIRDPART_CLEAN_MODULES += $(__local_module_clean)
endif

ifeq (x$(filter-out $(__local_module_config_file),$(ALL_THIRDPART_CONFIG_FILES)),x)
ALL_THIRDPART_CONFIG_FILES += $(__local_module_config_file)
else
__local_module_config_file:=
endif

ALL_MODULE_PATH.$(LOCAL_MODULE) := $(LOCAL_PATH)

include $(BUILD_SYSTEM)/install_inc.mk

define sub_modules_build
$(foreach sub,$(LOCAL_MODULE_GEN_$(1)_FILES), \
			$(eval THIRDPART_MODULE=$(sub)) \
			$(eval LOCAL_MODULE_BUILD=$(LOCAL_MODULE_BUILD)) \
			$(eval include $(BUILD_SYSTEM)/thirdpart_module_build.mk) \
)
endef
$(foreach t,$(ALL_GEN_FILE_TYPE),$(eval $(call sub_modules_build,$(t))))
ALL_INSTALL_MODULES += $(__local_installed_module)

$(LOCAL_MODULE_BUILD): $(__local_stamp_install)
$(__local_stamp_install): $(__local_installed_module) $(__local_stamp_build)
	$(hile) install -D /dev/null $@
$(__local_stamp_build): $(__local_build_module)
	$(hile) /usr/bin/install -D /dev/null $@
$(__local_stamp_config):
	$(hile) /usr/bin/install -D /dev/null $@

__global_target_cflags += --sysroot=$(ROOT_PATH) $(__global_target_cflags)
#  compile
$(THIRDPART_MODULE_CONFIG_FILE): PRIVATE_LOCAL_PATH:=$(LOCAL_PATH)
$(THIRDPART_MODULE_CONFIG_FILE): PRIVATE_TARGET_CFLAGS :=$(__global_target_cflags)
$(THIRDPART_MODULE_CONFIG_FILE): PRIVATE_MODULE_CFLAGS :=$(LOCAL_CFLAGS) $(LOCAL_CPPFLAGS)
$(THIRDPART_MODULE_CONFIG_FILE): \
	$(__local_stamp_config) \
	$(__local_dep_stamp_build) \
	$(THIRDPART_LOCAL_SHARED_LIBRARIES) \
	$(THIRDPART_LOCAL_STATIC_LIBRARIES) \
	$(__local_dep_stamp_install)
	cd $(PRIVATE_LOCAL_PATH); 	\
	$(PRIVATE_AUTOGEN_COMMAND);	\
	CPPFLAGS="$(PRIVATE_TARGET_CFLAGS) $(PRIVATE_MODULE_CFLAGS)" \
	CFLAGS="$(PRIVATE_TARGET_CFLAGS) $(PRIVATE_MODULE_CFLAGS)" \
	LDFLAGS="$(PRIVATE_TARGET_CFLAGS) $(PRIVATE_MODULE_CFLAGS)" \
	$(PRIVATE_CONFIG_COMMAND)
# end compile

ifneq (x$(strip $(__local_module_clean)),x)
$(__local_module_clean): PRIVATE_STAMP_CONFIG := $(__local_stamp_config)
$(__local_module_clean):$(__local_module_config_file)
	@if [ -f ${PRIVATE_STAMP_CONFIG} ];then \
	  $(PRIVATE_CLEAN_COMMAND); \
	  rm -rf ${PRIVATE_MODULE_FILES_CLEAN}; \
	else \
	  echo "$@: Without doing configure, ignore clean"; \
	fi
endif
