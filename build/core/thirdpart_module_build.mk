
__local_export_compiler_path := $(if $(EXPORT_COMPILER_PATH),$(EXPORT_COMPILER_PATH),)

__local_module_path:=$(if $(LOCAL_MODULE_PATH),$(LOCAL_MODULE_PATH),$(OUT_$(BUILD_MODE)_$(1)_DIR))
__module_install:= $(__local_stamp_build)@$(__local_module_path)/$(notdir $(THIRDPART_MODULE))
__module:=$(LOCAL_PATH)/$(THIRDPART_MODULE)

$(__module): PRIVATE_LOCAL_PATH:=$(LOCAL_PATH)
$(__module): PRIVATE_AUTOGEN_COMMAND := \
	$(__local_export_compiler_path) $(LOCAL_MODULE_AUTOGEN)
$(__module): PRIVATE_CONFIG_COMMAND := \
	$(__local_export_compiler_path) $(LOCAL_MODULE_CONFIG)
$(__module):PRIVATE_COMPILER_COMMAND := \
	$(__local_export_compiler_path) $(LOCAL_MODULE_COMPILE)

$(__module_install): $(__module) $(ALL_INCLUDE_DEPANNER)
	$(hide) mkdir -p $(dir $(word 2, $(subst @, ,$(@))))
	$(hide) cp -d $< $(word 2, $(subst @, ,$(@)))
	$(hide) /usr/bin/install -D /dev/null $@

$(__module): $(THIRDPART_MODULE_CONFIG_FILE)
	cd $(PRIVATE_LOCAL_PATH)  && \
	$(PRIVATE_COMPILER_COMMAND)


ifeq ($(USE_LEGACY_BUILD_SYSTEM),"NO")
ifeq (x$(1),xSHARED)
__module_install_sysroot:= $(__local_stamp_build)@$(OUT_DEVICE_SHARED_SYSROOT_DIR)/$(notdir $(THIRDPART_MODULE))
$(__module_install):$(__module_install_sysroot)
$(__module_install_sysroot):$(__module) $(ALL_INCLUDE_DEPANNER)
	$(hide) mkdir -p $(dir $(word 2, $(subst @, ,$(@))))
	$(hide) cp -d $< $(word 2, $(subst @, ,$(@)))
	$(hide) /usr/bin/install -D /dev/null $@
endif
endif
