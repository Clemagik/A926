HOST:=true
LOCAL_EXT_NAME:=.a
LOCAL_OUT_INSTALL_DIR:=$(OUT_HOST_STATIC_DIR)
include $(BUILD_SYSTEM)/base_ruler.mk

$(LOCAL_INSTALL_MODULE):$(LOCAL_OUT_MODULE_BUILD)
	mkdir -p $(dir $@)
	$(hide) cp $^ $@

$(LOCAL_OUT_MODULE_BUILD):$(LOCAL_MODULE_OBJS)
	$(hide) $(HOST_AR) -rsv $@ $^
