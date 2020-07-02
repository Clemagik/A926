define choose_src_target_path
$(eval tw := $(subst :, ,$(strip $(1)))) \
$(if $(word 3,$(tw)),$(error $(LOCAL_PATH): Bad prebuilt filename '$(1)')) \
$(if $(word 2,$(tw)), \
	$(eval LOCAL_TARGET_FILES := $(word 1,$(tw))) \
	$(eval LOCAL_SRC_FILES := $(word 2,$(tw))) \
	, \
	$(eval LOCAL_PATH := $(strip $(__local_path))) \
	$(eval LOCAL_TARGET_FILES := $(notdir $(1))) \
	$(eval LOCAL_SRC_FILES := $(strip $(tw))) \
)
endef
