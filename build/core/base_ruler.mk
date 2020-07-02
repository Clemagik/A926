#
# This file is processed by the compiler dependent'macro
#

BUILD_MODE:=
ifeq ($(strip x$(HOST)),xtrue)
BUILD_MODE:=HOST
PREFIX:=HOST-
else
BUILD_MODE:=DEVICE
PREFIX:=
endif

ifeq ($(strip x$(LOCAL_STATIC_SUFFIX)),x)
LOCAL_STATIC_SUFFIX:=.a
endif

LOCAL_MODULE_BUILD:=$(PREFIX)$(LOCAL_MODULE)
PREFIX_LOCAL_DEPANNER_MODULES := $(strip $(if $(LOCAL_DEPANNER_MODULES),$(addprefix $(PREFIX),$(LOCAL_DEPANNER_MODULES)),))
ALL_MODULES_CLEAN += $(PREFIX)$(LOCAL_MODULE)

__local_compile:=$(BUILD_MODE)_COMPILER
__local_out_obj_dir:=$(OUT_$(BUILD_MODE)_OBJ_DIR)
__local_out_static_dir:=$(OUT_$(BUILD_MODE)_STATIC_DIR)

__global_target_includes := $(OUT_$(BUILD_MODE)_INCLUDE_DIR)
__global_target_android_includes := $(OUT_DEVICE_ANDROID_INCLUDE_DIR)
__local_module_c_include:=$(addprefix $(LOCAL_PATH)/,$(LOCAL_C_INCLUDES) $(LOCAL_CPP_INCLUDES))
ifneq ($(strip x$(LOCAL_DISABLE_GLOBAL_CFLAGS)),xtrue)
__global_target_cflags := $(GLOBAL_$(BUILD_MODE)_CFLAGS)
else
__global_target_cflags:=
endif

$(eval $(call define-intermediate,$(__local_out_obj_dir),$(LOCAL_TARGET_MODULE_CLASS),$(LOCAL_MODULE_BUILD)))

define BUILD_TYPE_FILE
$(2):PRIVATE_BUILD:=$($(__local_compile).$(1))
$(2):PRIVATE_MODULE_C_INCLUDES:=$(__local_module_c_include)
$(2):PRIVATE_MODULE_CFLAGS:=$(LOCAL_CFLAGS) $(LOCAL_CPPFLAGS)
$(2):PRIVATE_TARGET_INCLUDES:=$(__global_target_includes)
$(2):PRIVATE_TARGET_ANDROID_INCLUDES:=$(__global_target_android_includes)
$(2):PRIVATE_TARGET_CFLAGS:=$(__global_target_cflags)
endef

define SPLIT_FILES
$(eval _src_files := $(filter %.$(1),$(LOCAL_SRC_FILES)))
$(eval MODULE_OBJ_FILES.$(1) := $(addprefix $(__local_intermediate)/,$(_src_files:.$(1)=.o)))
$(foreach f,$(filter %.$(1),$(LOCAL_SRC_FILES)), \
	$(eval $(call BUILD_TYPE_FILE,$(1),$(__local_intermediate)/$(f:.$(1)=.o),$(LOCAL_PATH)/$(f))) \
	)
endef

$(eval LOCAL_MODULE_OBJS:=$(foreach v,$(BUILD_TYPE), \
	$(eval $(call SPLIT_FILES,$(v),$(LOCAL_MODULE),$(LOCAL_PATH))) \
	$(MODULE_OBJ_FILES.$(v)) \
	))

LOCAL_OUT_MODULE_BUILD:=$(__local_intermediate)/$(LOCAL_MODULE)$(LOCAL_EXT_NAME)

$(LOCAL_OUT_MODULE_BUILD):PRIVATE_STATIC_LIBRARIES:= \
	$(addsuffix $(LOCAL_STATIC_SUFFIX), \
	$(addprefix $(__local_out_static_dir)/,$(basename $(LOCAL_STATIC_LIBRARIES))))

$(LOCAL_OUT_MODULE_BUILD):PRIVATE_SHARED_LIBRARIES:= \
	$(addprefix -l,$(subst lib,,$(basename $(LOCAL_SHARED_LIBRARIES))))

$(LOCAL_OUT_MODULE_BUILD):PRIVATE_LOCAL_LDFLAGS:=$(LOCAL_LDFLAGS)
$(LOCAL_OUT_MODULE_BUILD):PRIVATE_LOCAL_LDLIBS:=$(LOCAL_LDLIBS)


ifeq ($(strip x$(SDK_BUILD)),x)
ifeq (x$(strip $(LOCAL_MODULE_PATH)),x)
LOCAL_INSTALL_MODULE:=$(LOCAL_OUT_INSTALL_DIR)/$(LOCAL_MODULE)$(LOCAL_EXT_NAME)
else
LOCAL_INSTALL_MODULE:= $(LOCAL_MODULE_PATH)/$(LOCAL_MODULE)$(LOCAL_EXT_NAME)
endif
else
LOCAL_INSTALL_MODULE:=$(LOCAL_PATH)/$(LOCAL_MODULE)$(LOCAL_EXT_NAME)
endif

ifneq ($(strip x$(LOCAL_OUT_INSTALL_SYSROOT_DIR)),x)
LOCAL_INSTALL_MODULE_SYSROOT:=$(OUT_DEVICE_SHARED_SYSROOT_DIR)/$(LOCAL_MODULE)$(LOCAL_EXT_NAME)
endif

CLEAN_DEP_MODULES.$(LOCAL_MODULE_BUILD):= \
	$(sort $(basename $(LOCAL_STATIC_LIBRARIES)) \
	$(basename $(LOCAL_SHARED_LIBRARIES)) $(PREFIX_LOCAL_DEPANNER_MODULES))
CLEAN_DEP_FILES.$(LOCAL_MODULE_BUILD):= $(LOCAL_INSTALL_MODULE)


$(LOCAL_MODULE_BUILD)-clean:PRIVATE_MODULE_FILES_CLEAN:=$(CLEAN_DEP_FILES.$(LOCAL_MODULE_BUILD))
$(LOCAL_MODULE_BUILD)-clean:PRIVATE_MODULE_DIR_CLEAN:=$(__local_out_obj_dir)/$(LOCAL_PATH) $(__local_intermediate)

ifeq ($(strip x$(SDK_BUILD)),x)
include $(BUILD_SYSTEM)/module_install.mk
include $(BUILD_SYSTEM)/mma_build.mk
endif

ALL_INSTALL_MODULES += $(LOCAL_INSTALL_MODULE)

ALL_MODULE_PATH.$(LOCAL_MODULE) := $(LOCAL_PATH)
ALL_DOC_FILES.$(LOCAL_MODULE) := $(addprefix $(LOCAL_PATH)/,$(LOCAL_DOC_FILES))

ifeq ($(strip x$(SDK_BUILD)),x)
ALL_MODULES += $(LOCAL_FILTER_MODULE)
ALL_BUILD_MODULES += $(LOCAL_FILTER_MODULE)
ifneq ($(strip x$(LOCAL_FILTER_MODULE)),x)
ALL_BUILD_MODULES += $(CLEAN_DEP_MODULES.$(LOCAL_MODULE_BUILD))
endif
else
ALL_MODULES += $(LOCAL_MODULE_BUILD)
endif

define dump_my_var
$(info "base ruler var dump:")
$(info LOCAL_MODULE_BUILD: = $(LOCAL_MODULE_BUILD))
$(info LOCAL_OUT_MODULE_BUILD: = $(LOCAL_OUT_MODULE_BUILD))
$(info LOCAL_INSTALL_MODULE: = $(LOCAL_INSTALL_MODULE))
endef

define transform-d-to-p-args
$(hide) cp $(1) $(2); \
	sed -e 's/#.*//' -e 's/^[^:]*: *//' -e 's/ *\\$$//' \
		-e '/^$$/ d' -e 's/$$/ :/' < $(1) > $(2);       \
	rm -f $(1)
endef

define transform-d-to-p
$(call transform-d-to-p-args,$(@:%.o=%.d),$(@:%.o=%.P))
endef

# For system header, '-isystem' should apply to system header
# search path. Use '-I' to override a system  header file.
# If a directory specified with '-I' and '-isystem', the ‘-I’
# option is ignored.
# $(addprefix -isystem , $(PRIVATE_TARGET_INCLUDES)) \

define tranform-c-to-o
$(call host-mkdir,$(dir $@))
$(hide) $(PRIVATE_BUILD) \
		--sysroot=$(ROOT_PATH) \
		$(addprefix -I , $(PRIVATE_MODULE_C_INCLUDES)) 		   \
		$(addprefix -isystem, $(PRIVATE_TARGET_INCLUDES))	   \
		$(addprefix -isystem , $(PRIVATE_TARGET_ANDROID_INCLUDES)) \
		-c \
		$(PRIVATE_TARGET_CFLAGS) \
		$(PRIVATE_MODULE_CFLAGS) \
		-MD -MF $(@:.o=.d) -c -o $@ $<
$(call transform-d-to-p)
endef

ifeq ($(strip x$(SDK_BUILD)),x)
include $(BUILD_SYSTEM)/install_inc.mk
__local_dep_header := $(ALL_INCLUDE_DEPANNER)
__local_depanner :=  \
	$(addsuffix -intermediate/stamp_install, \
		$(addprefix $(OUT_DEVICE_OBJ_DIR)/DEPANNER/, \
		$(PREFIX_LOCAL_DEPANNER_MODULES))) \
	$(addsuffix -intermediate/stamp_install, \
	   $(addprefix $(OUT_DEVICE_OBJ_DIR)/STATIC_LIBRARIES/, \
	   $(basename $(LOCAL_STATIC_LIBRARIES)))) \
	$(addsuffix -intermediate/stamp_install, \
	   $(addprefix $(OUT_DEVICE_OBJ_DIR)/SHARED_LIBRARIES/, \
	   $(basename $(LOCAL_SHARED_LIBRARIES)))) \
	$(__local_dep_header)
__local_depanner := $(if $(__local_depanner), | $(__local_depanner),)
else
__local_dep_module :=
__local_dep_header :=
__local_depanner :=
endif

$(LOCAL_MODULE_BUILD): $(__local_stamp_install)
$(__local_stamp_install): $(__local_stamp_build)
	$(hide) install -D /dev/null $@

# For stamp_build do LOCAL_INSTALL_MODULE recipe because
# thirdpart THIRDPART_MODULE_CONFIG_FILE just depend
# on stamp_build to avoid every time stamp_install update
# on phony ruler.

$(__local_stamp_build): $(LOCAL_INSTALL_MODULE)
	$(hide) install -D /dev/null $@

ifneq (x$(LOCAL_INSTALL_MODULE_SYSROOT),x)
endif

$(__local_stamp_config): $(__local_depanner)
	$(hide) install -D /dev/null $@

MODULE_COMPILE_FILE:= $(MODULE_OBJ_FILES.c) $(MODULE_OBJ_FILES.cc) $(MODULE_OBJ_FILES.cpp) $(MODULE_OBJ_FILES.S)
-include $(MODULE_COMPILE_FILE:.o=.P)

$(MODULE_OBJ_FILES.S):$(__local_intermediate)/%.o:$(LOCAL_PATH)/%.S $(__local_stamp_config)
	$(tranform-c-to-o)

$(MODULE_OBJ_FILES.c):$(__local_intermediate)/%.o:$(LOCAL_PATH)/%.c $(__local_stamp_config)
	$(tranform-c-to-o)

$(MODULE_OBJ_FILES.cc):$(__local_intermediate)/%.o:$(LOCAL_PATH)/%.cc $(__local_stamp_config)
	$(tranform-c-to-o)

$(MODULE_OBJ_FILES.cpp):$(__local_intermediate)/%.o:$(LOCAL_PATH)/%.cpp $(__local_stamp_config)
	$(tranform-c-to-o)
