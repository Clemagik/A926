#include <fence.h>
#include <drm/drm_flip_work.h>
#include <drm/drm_plane_helper.h>
#include <drm/drm_fourcc.h>
#include <drm/drm_atomic.h>
#include <drm/drm_atomic_helper.h>
#include "ingenic_drv.h"
#include "dpu_reg.h"

#define to_ingenic_crtc(x)	container_of(x, struct ingenic_crtc, base)

static const uint32_t dpu_formats[] = {
	DRM_FORMAT_RGB565,
	DRM_FORMAT_XRGB8888,
	DRM_FORMAT_XBGR8888,
	DRM_FORMAT_ARGB8888,
	DRM_FORMAT_ABGR8888,
	DRM_FORMAT_ARGB1555,
	DRM_FORMAT_XRGB1555,
	DRM_FORMAT_NV12,
	DRM_FORMAT_YUV422
};

void ingenicfb_clk_enable(struct drm_crtc *crtc)
{
	struct drm_device *dev = crtc->dev;
	struct ingenic_drm_private *priv = dev->dev_private;
	clk_prepare_enable(priv->disp_clk);
	clk_prepare_enable(priv->clk);
}

void ingenicfb_clk_disable(struct drm_crtc *crtc)
{
	struct drm_device *dev = crtc->dev;
	struct ingenic_drm_private *priv = dev->dev_private;
	clk_disable_unprepare(priv->clk);
	clk_disable_unprepare(priv->disp_clk);
}

void ingenic_crtc_update_clk(struct drm_crtc *crtc)
{
	struct drm_device *dev = crtc->dev;
	struct ingenic_drm_private *priv = dev->dev_private;
	unsigned int lcd_clk;
	int ret;

	ret = clk_set_rate(priv->disp_clk, crtc->mode.clock * 1000);
	if (ret) {
		dev_err(dev->dev, "failed to set display clock rate to: %d\n",
				crtc->mode.clock);
	}

	lcd_clk = clk_get_rate(priv->clk);
}

void enable_vblank(struct drm_crtc *crtc, bool enable)
{
	struct ingenic_crtc *ingenic_crtc = to_ingenic_crtc(crtc);
	ingenic_crtc->dpu_ops->enable_vblank(ingenic_crtc, enable);
}

irqreturn_t ingenic_crtc_irq(struct drm_crtc *crtc)
{
	struct ingenic_crtc *ingenic_crtc = to_ingenic_crtc(crtc);
	return ingenic_crtc->dpu_ops->irq_handler(ingenic_crtc);
}

int ingenic_disable_plane(struct drm_crtc *crtc, struct ingenic_drm_plane *plane)
{
	struct ingenic_crtc *ingenic_crtc = to_ingenic_crtc(crtc);
	ingenic_crtc->dpu_ops->disable_plane(ingenic_crtc, plane);

	return 0;
}

int ingenic_update_plane(struct drm_crtc *crtc)
{
	struct ingenic_crtc *ingenic_crtc = to_ingenic_crtc(crtc);
	ingenic_crtc->dpu_ops->update_plane(ingenic_crtc);

	return 0;
}

static void ingenic_crtc_enable(struct drm_crtc *crtc)
{
	struct ingenic_crtc *ingenic_crtc = to_ingenic_crtc(crtc);
	ingenic_crtc->dpu_ops->enable(ingenic_crtc);
}

static void ingenic_crtc_disable(struct drm_crtc *crtc)
{
	struct ingenic_crtc *ingenic_crtc = to_ingenic_crtc(crtc);
	ingenic_crtc->dpu_ops->disable(ingenic_crtc);
}

static int ingenicfb_alloc_devmem(struct drm_crtc *crtc)
{
	struct ingenic_crtc *ingenic_crtc = to_ingenic_crtc(crtc);
	struct drm_device *dev = crtc->dev;
	uint32_t buff_size;
	uint8_t *addr;
	dma_addr_t addr_phy;
	int i, j;

	buff_size = sizeof(struct ingenicfb_framedesc);
	buff_size = ALIGN(buff_size, DESC_ALIGN);
	addr = dma_alloc_coherent(dev->dev, buff_size * MAX_DESC_NUM,
				  &addr_phy, GFP_KERNEL);
	if(addr == NULL) {
		return -ENOMEM;
	}
	for(i = 0; i < MAX_DESC_NUM; i++) {
		ingenic_crtc->framedesc[i] =
			(struct ingenicfb_framedesc *)(addr + i * buff_size);
		ingenic_crtc->framedesc_phys[i] = addr_phy + i * buff_size;
	}

	buff_size = sizeof(struct ingenicfb_layerdesc);
	buff_size = ALIGN(buff_size, DESC_ALIGN);
	addr = dma_alloc_coherent(dev->dev, buff_size * MAX_DESC_NUM * MAX_LAYER_NUM,
				  &addr_phy, GFP_KERNEL);
	if(addr == NULL) {
		return -ENOMEM;
	}
	for(j = 0; j < MAX_DESC_NUM; j++) {
		for(i = 0; i < MAX_LAYER_NUM; i++) {
			ingenic_crtc->layerdesc[j][i] = (struct ingenicfb_layerdesc *)
				(addr + i * buff_size + j * buff_size * MAX_LAYER_NUM);
			ingenic_crtc->layerdesc_phys[j][i] =
				addr_phy + i * buff_size + j * buff_size * MAX_LAYER_NUM;
		}
	}

	return 0;
}

static void ingenic_crtc_destroy(struct drm_crtc *crtc)
{
	struct ingenic_crtc *ingenic_crtc = to_ingenic_crtc(crtc);
	struct drm_device *dev = crtc->dev;
	struct drm_pending_vblank_event *event;
	bool vblank_put = false;
	unsigned long flags;

	spin_lock_irqsave(&dev->event_lock, flags);

	event = ingenic_crtc->event;
	ingenic_crtc->event = NULL;

	spin_unlock_irqrestore(&dev->event_lock, flags);

	if (event)
		event->base.destroy(&event->base);

	if (vblank_put)
		drm_vblank_put(dev, 0);

	drm_crtc_cleanup(crtc);

	kfree(ingenic_crtc);
}

static bool ingenic_crtc_mode_fixup(struct drm_crtc *crtc,
		const struct drm_display_mode *mode,
		struct drm_display_mode *adjusted_mode)
{
	return true;
}


static void ingenic_crtc_atomic_begin(struct drm_crtc *crtc,
                struct drm_crtc_state *old_crtc_state)
{
}
static void ingenic_crtc_atomic_flush(struct drm_crtc *crtc,
                struct drm_crtc_state *old_crtc_state)
{
	struct ingenic_crtc *ingenic_crtc = to_ingenic_crtc(crtc);
	struct drm_display_mode *mode = &crtc->state->adjusted_mode;

	ingenic_crtc_update_clk(crtc);
	ingenic_crtc->dpu_ops->mode_set(ingenic_crtc, mode);
}

static const struct drm_crtc_funcs ingenic_crtc_funcs = {
	.destroy = ingenic_crtc_destroy,
	.reset = drm_atomic_helper_crtc_reset,
	.set_config = drm_atomic_helper_set_config,
	.page_flip = drm_atomic_helper_page_flip,
	.atomic_duplicate_state = drm_atomic_helper_crtc_duplicate_state,
	.atomic_destroy_state = drm_atomic_helper_crtc_destroy_state,
};

static const struct drm_crtc_helper_funcs ingenic_crtc_helper_funcs = {
	.mode_set = drm_helper_crtc_mode_set,
	.mode_fixup = ingenic_crtc_mode_fixup,
	.disable = ingenic_crtc_disable,
	.enable = ingenic_crtc_enable,
	.atomic_begin = ingenic_crtc_atomic_begin,
	.atomic_flush = ingenic_crtc_atomic_flush,
};

struct drm_crtc *ingenic_crtc_create(struct drm_device *dev)
{
	struct ingenic_crtc *ingenic_crtc;
	struct ingenic_drm_plane *primary_plane;
	struct drm_crtc *crtc;
	enum drm_plane_type type;
	unsigned int zpos;
	int ret;

	ingenic_crtc = kzalloc(sizeof(*ingenic_crtc), GFP_KERNEL);
	if (!ingenic_crtc) {
		dev_err(dev->dev, "allocation failed\n");
		return NULL;
	}

	ingenic_crtc->lcd_type = LCD_TYPE_TFT;
	dpu_register(ingenic_crtc);

	for (zpos = 0; zpos < MAX_LAYER_NUM; zpos++) {
		type = ingenic_plane_get_type(zpos);
		ret = ingenic_plane_init(dev, &ingenic_crtc->plane[zpos],
					1, type, dpu_formats,
					ARRAY_SIZE(dpu_formats), zpos);
		if (ret)
			return NULL;
	}

	crtc = &ingenic_crtc->base;

	ingenic_crtc->dpms = DRM_MODE_DPMS_OFF;

	primary_plane = &ingenic_crtc->plane[0];
	ret = drm_crtc_init_with_planes(dev, crtc, &primary_plane->base, NULL,
					&ingenic_crtc_funcs);
	if (ret < 0)
		goto fail;

	drm_crtc_helper_add(crtc, &ingenic_crtc_helper_funcs);

	ingenicfb_alloc_devmem(crtc);

	return crtc;

fail:
	ingenic_crtc_destroy(crtc);
	return NULL;
}
