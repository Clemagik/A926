LOCAL_EXT_NAME:=.so

LOCAL_OUT_INSTALL_DIR:=$(OUT_DEVICE_SHARED_DIR)

ifneq (x$(OUT_DEVICE_SHARED_SYSROOT_DIR),x)
LOCAL_OUT_INSTALL_SYSROOT_DIR:=$(OUT_DEVICE_SHARED_SYSROOT_DIR)
endif
ifeq ($(strip $(LOCAL_TARGET_MODULE_CLASS)),)
LOCAL_TARGET_MODULE_CLASS := SHARED_LIBRARIES
endif

include $(BUILD_SYSTEM)/base_ruler.mk


### for buildroot sysroot.
ifneq (x$(LOCAL_INSTALL_MODULE_SYSROOT),x)

$(LOCAL_INSTALL_MODULE_SYSROOT):$(LOCAL_OUT_MODULE_BUILD)
	$(hide) $(call host-mkdir,$(dir $@))
	$(hide) cp -f $< $@

$(LOCAL_INSTALL_MODULE):$(LOCAL_INSTALL_MODULE_SYSROOT)
	$(hide) $(call host-mkdir,$(dir $@))
	$(hide) $(TARGET_STRIP) -o $@ $<

else
$(LOCAL_INSTALL_MODULE):$(LOCAL_OUT_MODULE_BUILD)
	$(hide) $(call host-mkdir,$(dir $@))
	$(hide) $(TARGET_STRIP) -o $@ $<

endif
$(LOCAL_OUT_MODULE_BUILD):$(LOCAL_MODULE_OBJS)
	$(tranforms_device_o_to_so)
