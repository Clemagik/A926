PWD := $(shell pwd)
TOP_DIR:=$(PWD)
TOP := .
TOPDIR := .
ALL_MODULES:=
ALL_BUILD_MODULES:=
ALL_MODULE_PATH:=
ALL_BUILD_SAMPLE_PATH:=
ALL_MODULES_CLEAN:=
MMA_MODULES :=
ALL_INSTALL_MODULES:=
ALL_PREBUILT_MODULES:= prebuild
ALL_COPY_MODULES:=
THIRDPART_CLEAN_MODULES:=
tags_to_install :=
BUILD_SYSTEM := $(TOPDIR)/build/core

#all support tag is : eng user
ALL_SUPPORT_MODULE_TAGS := eng user optional userdebug

ifeq ($(TARGET_BUILD_VARIANT),eng)
tags_to_install := eng user
endif

ifeq ($(TARGET_BUILD_VARIANT),userdebug)
tags_to_install := eng user userdebug
endif

ifeq ($(TARGET_BUILD_VARIANT),user)
tags_to_install := user
endif
################################################################
#COMPILER_PATH : the host gcc compiler info when use gcc compile
#
# 1,COMPILER_PATH
# 2,LIBRARY_PATH
# 3,the path of the library ----> ld-linux-x86-64.so.2 or ld-linux.so.2
# 4,host ld link elf format ----> elf_x86_64 or
#
################################################################
LOCAL_COMPILER_INFO := $(shell $(BUILD_SYSTEM)/tools/compiler.py)
LOCAL_COMPILER_PATH := $(word 1,$(LOCAL_COMPILER_INFO))
LIBRARY_PATH := $(word 2,$(LOCAL_COMPILER_INFO))
LD_LIB_PATH := $(word 3,$(LOCAL_COMPILER_INFO))
HOST_LD_ELF_FORMAT:= $(word 4,$(LOCAL_COMPILER_INFO))
COMPILER_ERR:=$(wordlist 5,20,$(LOCAL_COMPILER_INFO))
$(if $(COMPILER_ERR),		\
	$(info $(COMPILER_ERR)) \
	$(error stop) )



ifeq (x$(strip $(TARGET_DEVICE)),x)
$(error Can not get  TARGET_DEVICE info ,you should ' source build/envsetup.sh ' firstly ,then ' lunch ' choice the config,finally ' make ' to build the project)
endif
include device/$(TARGET_DEVICE)/device.mk
#$(shell hardware/init/gen_hardware_scipt.py $(TARGET_BOARD_ARCH) $(TARGET_BOARD_PLATFORM) $(TARGET_PRODUCT_BOARD))

include $(BUILD_SYSTEM)/device-tools-chain.mk
include $(BUILD_SYSTEM)/host-tools-chain.mk
include $(BUILD_SYSTEM)/config.mk
include $(BUILD_SYSTEM)/device_ruler.mk
include $(BUILD_SYSTEM)/host_ruler.mk
include $(BUILD_SYSTEM)/definitions-host.mk

ifeq ($(strip $(USE_LEGACY_BUILD_SYSTEM)),YES)
$(shell cp -u $(BUILD_SYSTEM)/kernel_build.mk kernel/Build.mk)
$(shell cp -u  $(BUILD_SYSTEM)/uboot_build.mk u-boot/Build.mk)
DEVICE_PATH ?= device/$(TARGET_DEVICE)
endif


BUILD_TYPE := c cpp cc S

EXCEPT-FILES:=build include Makefile out

ifneq ($(MAKECMDGOALS),clean)
endif

$(shell if [ -e $(OUT_IMAGE_DIR) ];then echo;else \
		mkdir -p $(OUT_IMAGE_DIR);\
	fi)


ifneq ($(ONE_SHOT_MAKEFILE),)
include $(ONE_SHOT_MAKEFILE)
.PHONY: GET-INSTALL-PATH
GET-INSTALL-PATH:
	@$(foreach m, $(ALL_MODULES), $(if $(ALL_MODULES.$(m).INSTALLED), \
		echo 'INSTALL-PATH: $(m) $(ALL_MODULES.$(m).INSTALLED)';))
else # ONE_SHOT_MAKEFILE

subdirs:=$(TOP)
subdir_makefiles := \
	$(info Searching Build.mk ...)					\
	$(info Build.mk under $(subdirs)/$(OUT_DIR)* *.repo *.git 	\
		$(subdirs)/build and subdirs will be ignored.)		\
	$(shell $(BUILD_SYSTEM)/tools/findleaves.py $(DEVICE_PATH) Build.mk)	\
	$(shell $(BUILD_SYSTEM)/tools/findleaves.py --prune=$(OUT_DIR) --prune=.repo --prune=.git --prune=build --prune=device $(subdirs) Build.mk)

#	$(shell find $(DEVICE_PATH) -name Build.mk)			\
#	$(shell find $(subdirs) -mindepth 1 -name "Build.mk" -print -o 	\
#		-path "$(subdirs)/$(OUT_DIR)*" -prune , 		\
#		-path *.repo -prune , 					\
#		-path *.git -prune , 					\
#		-path $(subdirs)/build -prune , 			\
#		-path $(subdirs)/device -prune)
#
#	$(shell $(BUILD_SYSTEM)/tools/findleaves.py --prune=$(OUT_DIR) --prune=.repo --prune=.git --prune=build --prune=device $(subdirs) Build.mk)

#find -name "Build.mk" -print -o -path "./packages" -prune , -path "./out*" -prune

# special Build.mk for each device.
#subdir_makefiles += \
#	$(shell $(BUILD_SYSTEM)/tools/findleaves.py $(DEVICE_PATH) Build.mk)

.PHONY:target
target:systemimage

$(foreach mk, $(subdir_makefiles), $(warning including $(mk) ...) $(eval include $(mk)))
endif # ONE_SHOT_MAKEFILE

ALL_MODULES := $(sort $(ALL_MODULES))
MMA_MODULES := $(sort $(MMA_MODULES))
ALL_MODULES_CLEAN := $(sort $(ALL_MODULES_CLEAN))
define CLEAN_MODULE
.PHONY: $(1)
$(1):|$(2)
	$(hide) rm -rf $$(PRIVATE_MODULE_FILES_CLEAN) $$(PRIVATE_MODULE_DIR_CLEAN) $(ALL_MODULE_PATH.$(m))/$(m).code-workspace $(ALL_MODULE_PATH.$(m))/.vscode/
endef

define CLEAN_FILES
.PHONY: $(1)
$(1):
	$(hide) rm -rf $$(PRIVATE_MODULE_FILES_CLEAN) $$(PRIVATE_MODULE_DIR_CLEAN)
endef

$(foreach m,$(ALL_MODULES_CLEAN), \
	$(if $(strip $(CLEAN_DEP_MODULES.$(m))), \
		$(eval $(call CLEAN_MODULE,$(m)-clean,$(addsuffix -clean,$(CLEAN_DEP_MODULES.$(m))))), \
		$(if $(strip $(CLEAN_DEP_FILES.$(m))), \
			$(eval $(call CLEAN_FILES,$(m)-clean)),) \
))

ALL_MODULE_PATH = $(sort $(foreach m,$(sort $(ALL_BUILD_MODULES)),$(ALL_MODULE_PATH.$(m))))

define DEP_FILES
$(1):|$(2)
endef

prebuild: | rootfs

define MODULE_VSCODE
.PHONY: $(1)
$(1):
	$(hide) $(TOP_DIR)/build/createvscode.sh $(m) $(OUT_DEVICE_OBJ_DIR)/DEPANNER/$(m)-intermediate $(ALL_MODULE_PATH.$(m)) $(TOP_DIR)
endef

$(foreach m,$(MMA_MODULES),\
		$(eval $(call MODULE_VSCODE,$(m)-vscode))\
		)

$(foreach m,$(ALL_INSTALL_MODULES),\
 		$(eval $(call DEP_FILES,$(m),$(ALL_PREBUILT_MODULES)))\
 		)
$(foreach m,$(ALL_COPY_MODULES),\
 		$(eval $(call DEP_FILES,$(m),$(ALL_PREBUILT_MODULES)))\
 		)
ifeq (x$(strip $(OTA)),x)
include $(BUILD_SYSTEM)/build_system.mk
else
include $(BUILD_SYSTEM)/build_ota_system.mk
endif

.PHONY: all_modules
ifndef BUILD_MODULES_IN_PATHS
all_modules: $(MMA_MODULES)
else
# BUILD_MODULES_IN_PATHS is a list of paths relative to the top of the tree
module_path_patterns := $(foreach p, $(BUILD_MODULES_IN_PATHS),\
    $(if $(filter %/,$(p)),$(p)%,$(p)/%))
mma_modules := $(sort $(foreach m, $(ALL_MODULES_CLEAN),$(if $(filter\
    $(module_path_patterns), $(addsuffix /,$(ALL_MODULE_PATH.$(m)))),$(m))))
all_modules: $(mma_modules)
endif

include $(BUILD_SYSTEM)/build_sdk.mk

.PHONY: sdk win_sdk
ALL_SDK_TARGETS := $(INSTALL_SDK_TARGET)
sdk: $(ALL_SDK_TARGETS)
win_sdk: $(ALL_SDK_TARGETS)

.PHONY: clean
clean: $(THIRDPART_CLEAN_MODULES)
	@rm -rf docs/html
	@rm -rf docs/latex
	@rm -rf docs/ModuleDescription
	@rm -rf ModuleDescription.html
	@rm -rf $(OUT_DIR)
	@echo "Entire build directory removed."

.PHONY: nothing
nothing:
	@echo Successfully read the makefiles.

.PHONY: show-modules
show-modules:
	$(info )
	$(info ----------- all modules compiled ----------)
	$(foreach p, $(ALL_MODULES), $(info $p))
	$(info )


include $(BUILD_SYSTEM)/doc.mk
include $(BUILD_SYSTEM)/moduledesc.mk
include $(BUILD_SYSTEM)/cmake-device-template.mk
