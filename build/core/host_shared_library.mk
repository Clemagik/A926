HOST:=true
LOCAL_EXT_NAME:=.so
LOCAL_OUT_INSTALL_DIR:=$(OUT_HOST_SHARED_DIR)

include $(BUILD_SYSTEM)/base_ruler.mk

$(LOCAL_INSTALL_MODULE):$(LOCAL_OUT_MODULE_BUILD)
	$(hide)mkdir -p $(dir $@)
	$(hide) $(HOST_STRIP) -o $@ $<

$(LOCAL_OUT_MODULE_BUILD):$(LOCAL_MODULE_OBJS)
	$(tranforms_host_o_to_so)
