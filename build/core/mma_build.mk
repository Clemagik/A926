# Only the tags mentioned in this test are expected to be set by module
# makefiles. Anything else is either a typo or a source of unexpected
# behaviors.

ALL_MODULES += $(LOCAL_FILTER_MODULE)
ifeq (<$(LOCAL_PATH)>,<$(BUILD_MODULES_IN_PATHS)>)
MMA_MODULES += $(LOCAL_MODULE_BUILD)
ifneq ($(LOCAL_DEPANNER_MODULES),)
MMA_MODULES += $(PREFIX_LOCAL_DEPANNER_MODULES)
endif
else
ifeq ($(BUILD_MODULES_IN_PATHS),)
MMA_MODULES += $(LOCAL_MODULE_BUILD)
endif
endif


