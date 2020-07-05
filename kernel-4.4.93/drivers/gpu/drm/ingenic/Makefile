ccflags-y := -Iinclude/linux -Iinclude/drm
ifeq (, $(findstring -W,$(EXTRA_CFLAGS)))

endif

ingenic-y := \
	panel-kd035hvfmd057.o \
	panel-kd035hvfbd037.o \
	ingenic_dpu.o \
	ingenic_crtc.o \
	ingenic_drv.o \
	drm_sync_helper.o \
	ingenic_plane.o \
	ingenic_disport.o

obj-$(CONFIG_DRM_INGENIC_X2000) += ingenic.o
