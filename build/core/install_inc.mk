ifeq ($(LOCAL_MODULE_BUILD),)
$(error "install_inc: called without LOCAL MODULE")
endif

PREFIX_INCLUDE_FILES_TAR := $(addprefix $(__local_stamp_build)@,\
	$(addprefix $(OUT_$(BUILD_MODE)_INCLUDE_DIR)/,\
		$(subst include/,,$(filter include/%,$(LOCAL_EXPORT_C_INCLUDE_FILES)))))

INCLUDE_FILES_TAR := $(addprefix $(__local_stamp_build)@,\
	$(addprefix $(OUT_$(BUILD_MODE)_INCLUDE_DIR)/,\
		$(filter-out include/%,$(LOCAL_EXPORT_C_INCLUDE_FILES))))

PREFIX_INCLUDE_DIRS_TAR := $(addprefix $(__local_stamp_build)@,\
	$(addprefix $(OUT_$(BUILD_MODE)_INCLUDE_DIR)/,\
		$(subst include/,,$(filter include/%,$(LOCAL_EXPORT_C_INCLUDE_DIRS)))))

INCLUDE_DIRS_TAR := $(addprefix $(__local_stamp_build)@,\
	$(addprefix $(OUT_$(BUILD_MODE)_INCLUDE_DIR)/,\
		$(filter-out include/%,$(LOCAL_EXPORT_C_INCLUDE_DIRS))))

ALL_INCLUDE_DEPANNER:= $(strip $(PREFIX_INCLUDE_FILES_TAR) $(INCLUDE_FILES_TAR) $(PREFIX_INCLUDE_DIRS_TAR) $(INCLUDE_DIRS_TAR))

$(PREFIX_INCLUDE_FILES_TAR):$(__local_stamp_build)@$(OUT_$(BUILD_MODE)_INCLUDE_DIR)/%.h:$(LOCAL_PATH)/include/%.h
	$(eval __dest := $(word 2, $(subst @, , $@)))
	$(hide)mkdir -p $(dir $(__dest))
	$(hide)cp -df $< $(__dest)
	$(hide)/usr/bin/install -D /dev/null $@

$(INCLUDE_FILES_TAR):$(__local_stamp_build)@$(OUT_$(BUILD_MODE)_INCLUDE_DIR)/%.h:$(LOCAL_PATH)/%.h
	$(eval __dest := $(word 2, $(subst @, , $@)))
	$(hide)mkdir -p $(dir $(__dest))
	$(hide)cp -df $< $(__dest)
	$(hide)/usr/bin/install -D /dev/null $@

$(PREFIX_INCLUDE_DIRS_TAR):$(__local_stamp_build)@$(OUT_$(BUILD_MODE)_INCLUDE_DIR)/%:$(LOCAL_PATH)/include/%
	$(eval __dest := $(word 2, $(subst @, , $@)))
	$(hide)mkdir -p $(dir $(__dest))
	$(hide)cp -Tdfr $< $(__dest)
	$(hide)/usr/bin/install -D /dev/null $@

$(INCLUDE_DIRS_TAR):$(__local_stamp_build)@$(OUT_$(BUILD_MODE)_INCLUDE_DIR)/%:$(LOCAL_PATH)/%
	$(eval __dest := $(word 2, $(subst @, , $@)))
	$(hide)mkdir -p $(dir $(__dest))
	$(hide)cp -Tdfr $< $(__dest)
	$(hide)/usr/bin/install -D /dev/null $@
