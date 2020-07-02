# build_system can use two method
# 	* common system with patch. --legacy
#	* buildroot.		    --recommend
# for compatible with common system, check device/common/system dir.
# if exist, use build_system_legacy.mk
#




ifeq ($(strip $(USE_LEGACY_BUILD_SYSTEM)),YES)
include $(BUILD_SYSTEM)/build_system_legacy.mk
else
include $(BUILD_SYSTEM)/build_system_buildroot.mk
endif

