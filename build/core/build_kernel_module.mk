ifeq (x$(LOCAL_INSTALL_PATH),x)
LOCAL_INSTALL_PATH=lib/modules
endif
KERNEL_TARGET_LINK:=$(OUT_DEVICE_OBJ_DIR)/$(KERNEL_MODULE)-intermediate
DRIVER_TARGET_CONFIG:=$(KERNEL_TARGET_LINK)/.config
DRIVER_TARGET_LINK:=$(OUT_DEVICE_OBJ_DIR)/$(LOCAL_PATH)/$(LOCAL_MODULE)/Makefile

QTARGET_COPYS_INSTALL:=$(OUT_DEVICE_OBJ_DIR)/$(LOCAL_PATH)/$(LOCAL_MODULE)/stampinstall

TARGET_SRC_FILES:=$(addprefix $(dir $(DRIVER_TARGET_LINK)),$(LOCAL_SRC_FILES))

TARGET_INSTALL_DRIVER:=$(TARGET_FS_BUILD)/$(LOCAL_INSTALL_PATH)/$(LOCAL_MODULE).ko

define copy_src_files
$(1):$(2)
	mkdir -p $(dir $(1))
	cp $(2) $(1)
endef

$(foreach f, $(LOCAL_SRC_FILES), \
	$(eval SRC:=$(LOCAL_PATH)/$f) \
	$(eval DST:=$(dir $(DRIVER_TARGET_LINK))$f) \
	$(eval $(call copy_src_files,$(DST),$(SRC))) \
)

$(DRIVER_TARGET_LINK): LOCAL_MODULE_NAME:=$(LOCAL_MODULE)
$(DRIVER_TARGET_LINK): DRIVER_SRC_OBJS:=$(LOCAL_SRC_FILES:.c=.o)
$(DRIVER_TARGET_LINK): $(TARGET_SRC_FILES)
	mkdir -p $(dir $@)
	@echo "obj-m := " $(LOCAL_MODULE_NAME)".o" > $@
	@echo "$(LOCAL_MODULE_NAME)-objs = " $(DRIVER_SRC_OBJS) >> $@
	@echo "KERNELDIR:= " $(TOP_DIR)/$(dir $(DRIVER_TARGET_CONFIG)) >> $@
	@echo "modules: " >> $@
	@echo "	make -C " $(TOP_DIR)/$(dir $(DRIVER_TARGET_CONFIG)) M=$(TOP_DIR)/$(dir $@) modules >> $@


MAKEPARAM=-j1
ifneq (x$(MAKE_JLEVEL),x)
	MAKEPARAM=-j$(MAKE_JLEVEL)
endif

LOCAL_MODULE_BUILD:=$(LOCAL_MODULE)
LOCAL_DEPANNER_MODULES += $(KERNEL_MODULE)


include $(BUILD_SYSTEM)/module_install.mk
ifneq (x$(LOCAL_FILTER_MODULE),x)
ALL_MODULES += $(LOCAL_DEPANNER_MODULES)
ALL_MODULES += $(LOCAL_FILTER_MODULE)
ALL_BUILD_MODULES += $(LOCAL_MODULE_STAMP)
endif

$(TARGET_INSTALL_DRIVER):LOCAL_INSTALL_DRIVER:=$(dir $(DRIVER_TARGET_LINK))$(LOCAL_MODULE).ko
$(TARGET_INSTALL_DRIVER):$(KERNEL_MODULE)
$(TARGET_INSTALL_DRIVER):$(DRIVER_TARGET_LINK)
	make -C $(dir $<) modules $(MAKEPARAM)
	mkdir -p $(dir $@)
	@cp $(LOCAL_INSTALL_DRIVER) $@

$(TARGET_INSTALL_DRIVER)-clean:LOCAL_BUILD_PATH:=$(DRIVER_TARGET_LINK)
$(TARGET_INSTALL_DRIVER)-clean:
	@rm -fr $(dir $(LOCAL_BUILD_PATH))

$(LOCAL_MODULE_BUILD):$(TARGET_INSTALL_DRIVER)
	@echo $(LOCAL_MODULE_BUILD) "build ok."
$(LOCAL_MODULE_BUILD)-clean:$(TARGET_INSTALL_DRIVER)-clean



include $(BUILD_SYSTEM)/module_copy_file.mk


ifneq (x$(QTARGET_COPYS),x)

$(foreach filecell, $(strip $(QTARGET_COPYS)), \
	$(eval $(call separate_vars,$(filecell))) \
	$(eval $(call target_copy_file,$(src),$(target),$(LOCAL_PATH),$(QTARGET_COPYS_INSTALL))))

endif
