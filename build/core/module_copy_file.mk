define separate_vars
	$(eval myword:=$(subst :, ,$(strip $(1))))
	$(eval src:=$(strip $(word 1,$(myword))))
	$(eval target:=$(strip $(word 2,$(myword))))
endef

# $(1) src file
# $(2) dest file
# $(3) src path
# $(4) dest path
define target_copy_file
$(eval SOURCE_FILE:= $(3)/$(1))
$(eval TARGET_FILE:=$(BUILDROOT_TARGET)/$(2))
$(eval STAMP_INSTALL:=$(4)/$(1).stamp_install)
$(STAMP_INSTALL):
	@mkdir -p $(dir $(TARGET_FILE))
	@cp -dvrf $(SOURCE_FILE) $(TARGET_FILE)
	@mkdir -p $(dir $(STAMP_INSTALL))
	@install -D /dev/null  $(STAMP_INSTALL)

$(LOCAL_MODULE):$(STAMP_INSTALL)
$(LOCAL_MODULE)-clean:
	rm -rf $(STAMP_INSTALL)
endef
