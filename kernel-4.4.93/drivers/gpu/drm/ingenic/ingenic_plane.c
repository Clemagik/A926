#include <drm/drmP.h>
#include <drm/drm_plane_helper.h>
#include <drm/drm_atomic_helper.h>
#include "ingenic_drv.h"
#include "dpu_reg.h"

static int ingenic_plane_get_size(int start, unsigned length, unsigned last)
{
	int end = start + length;
	int size = 0;

	if (start <= 0) {
		if (end > 0)
			size = min_t(unsigned, end, last);
	} else if (start <= last) {
		size = min_t(unsigned, last - start, length);
	}

	return size;
}

static void ingenic_plane_compute_base(struct drm_plane *plane,
					 struct drm_framebuffer *fb,
					 int x, int y)
{
	struct ingenic_drm_plane *ingenic_plane = to_ingenic_plane(plane);
	struct drm_gem_cma_object *gem;
	unsigned int bpp;

	/* one framebufer may have 4 plane */
	gem = drm_fb_cma_get_gem_obj(fb, 0);
	bpp = fb->bits_per_pixel;
	bpp = ingenic_plane->nv_en ? 8 : bpp;
	ingenic_plane->addr_offset[0] = gem->paddr + fb->offsets[0]
		       + y * fb->pitches[0] + x * (bpp >> 3);

	if (ingenic_plane->nv_en) {
		bpp = fb->bits_per_pixel - 8;
		ingenic_plane->addr_offset[1] = gem->paddr + fb->offsets[1]
			       + y  * fb->pitches[1]
			       + x * (bpp >> 3);
	}
	ingenic_plane->stride = fb->width;
}

void ingenic_plane_set_formt(struct drm_plane *plane,  struct drm_framebuffer *fb)
{
	struct ingenic_drm_plane *ingenic_plane = to_ingenic_plane(plane);
	switch(fb->pixel_format) {
		case DRM_FORMAT_XRGB8888:
			ingenic_plane->format = LAYER_CFG_FORMAT_RGB888;
			ingenic_plane->color = LAYER_CFG_COLOR_RGB;
			break;
		case DRM_FORMAT_XBGR8888:
			ingenic_plane->format = LAYER_CFG_FORMAT_RGB888;
			ingenic_plane->color = LAYER_CFG_COLOR_BGR;
			break;
		case DRM_FORMAT_ARGB8888:
			ingenic_plane->format = LAYER_CFG_FORMAT_ARGB8888;
			ingenic_plane->color = LAYER_CFG_COLOR_RGB;
			break;
		case DRM_FORMAT_ABGR8888:
			ingenic_plane->format = LAYER_CFG_FORMAT_ARGB8888;
			ingenic_plane->color = LAYER_CFG_COLOR_BGR;
			break;
		case DRM_FORMAT_ARGB1555:
			ingenic_plane->format = LAYER_CFG_FORMAT_ARGB1555;
			ingenic_plane->color = LAYER_CFG_COLOR_RGB;
			break;
		case DRM_FORMAT_XRGB1555:
			ingenic_plane->format = LAYER_CFG_FORMAT_RGB555;
			ingenic_plane->color = LAYER_CFG_COLOR_RGB;
			break;
		case DRM_FORMAT_RGB565:
			ingenic_plane->format = LAYER_CFG_FORMAT_RGB565;
			ingenic_plane->color = LAYER_CFG_COLOR_RGB;
			break;
		case DRM_FORMAT_NV12:
			ingenic_plane->format = LAYER_CFG_FORMAT_NV12;
			ingenic_plane->color = 0;
			break;
		case DRM_FORMAT_NV21:
			ingenic_plane->format = LAYER_CFG_FORMAT_NV21;
			ingenic_plane->color = 0;
			break;
		case DRM_FORMAT_YUV422:
			ingenic_plane->format = LAYER_CFG_FORMAT_YUV422;
			ingenic_plane->color = 0;
			break;
		default:
			printk("Err: plane not support formates!\n");
			break;
	}
}

static void ingenic_plane_mode_set(struct drm_plane *plane,
				  struct drm_crtc *crtc,
				  struct drm_framebuffer *fb,
				  int crtc_x, int crtc_y,
				  unsigned int crtc_w, unsigned int crtc_h,
				  uint32_t src_x, uint32_t src_y,
				  uint32_t src_w, uint32_t src_h)
{
	struct ingenic_drm_plane *ingenic_plane = to_ingenic_plane(plane);
	struct drm_display_mode *mode = &crtc->mode;
	unsigned int actual_w;
	unsigned int actual_h;

	actual_w = ingenic_plane_get_size(crtc_x, crtc_w, mode->hdisplay);
	actual_h = ingenic_plane_get_size(crtc_y, crtc_h, mode->vdisplay);

	if (crtc_x < 0) {
		if (actual_w)
			src_x -= crtc_x;
		crtc_x = 0;
	}

	if (crtc_y < 0) {
		if (actual_h)
			src_y -= crtc_y;
		crtc_y = 0;
	}

	/* set ratio */
	ingenic_plane->h_ratio = (src_w << 16) / crtc_w;
	ingenic_plane->v_ratio = (src_h << 16) / crtc_h;

	/* set drm framebuffer data. */
	ingenic_plane->src_x = src_x;
	ingenic_plane->src_y = src_y;
	ingenic_plane->src_w = (actual_w * ingenic_plane->h_ratio) >> 16;
	ingenic_plane->src_h = (actual_h * ingenic_plane->v_ratio) >> 16;

	/* set plane range to be displayed. */
	ingenic_plane->disp_pos_x = crtc_x;
	ingenic_plane->disp_pos_y = crtc_y;
	if((ingenic_plane->src_w != actual_w) || (ingenic_plane->src_h != actual_h)) {
		ingenic_plane->scale_en = 1;
		ingenic_plane->scale_w = actual_w;
		ingenic_plane->scale_h = actual_h;
	} else {
		ingenic_plane->scale_en = 0;
		ingenic_plane->scale_w = 0;
		ingenic_plane->scale_h = 0;
	}
	ingenic_plane_compute_base(plane, fb,
			ingenic_plane->src_x,
			ingenic_plane->src_y);
	ingenic_plane_set_formt(plane, fb);

	ingenic_plane->lay_en = 1;
	plane->crtc = crtc;
}

static void ingenic_plane_atomic_update(struct drm_plane *plane,
				       struct drm_plane_state *old_state)
{
	struct drm_plane_state *state = plane->state;
	struct ingenic_drm_plane *ingenic_plane = to_ingenic_plane(plane);

	if (!state->crtc)
		return;

	ingenic_plane_mode_set(plane, state->crtc, state->fb,
			      state->crtc_x, state->crtc_y,
			      state->crtc_w, state->crtc_h,
			      state->src_x >> 16, state->src_y >> 16,
			      state->src_w >> 16, state->src_h >> 16);

	ingenic_plane->pending_fb = state->fb;

	ingenic_update_plane(state->crtc);
}

static void ingenic_plane_atomic_disable(struct drm_plane *plane,
					struct drm_plane_state *old_state)
{
	struct ingenic_drm_plane *ingenic_plane = to_ingenic_plane(plane);

	if (!old_state->crtc)
		return;

	ingenic_disable_plane(old_state->crtc,
					ingenic_plane);
}

static const struct drm_plane_helper_funcs plane_helper_funcs = {
	.atomic_update = ingenic_plane_atomic_update,
	.atomic_disable = ingenic_plane_atomic_disable,
};
static struct drm_plane_funcs ingenic_plane_funcs = {
	.update_plane	= drm_atomic_helper_update_plane,
	.disable_plane	= drm_atomic_helper_disable_plane,
	.destroy	= drm_plane_cleanup,
	.reset = drm_atomic_helper_plane_reset,
	.atomic_duplicate_state = drm_atomic_helper_plane_duplicate_state,
	.atomic_destroy_state = drm_atomic_helper_plane_destroy_state,
};

enum drm_plane_type ingenic_plane_get_type(unsigned int zpos)
{
		if (zpos == 0)
			return DRM_PLANE_TYPE_PRIMARY;
		else
			return DRM_PLANE_TYPE_OVERLAY;
}

int ingenic_plane_init(struct drm_device *dev,
		      struct ingenic_drm_plane *ingenic_plane,
		      unsigned long possible_crtcs, enum drm_plane_type type,
		      const uint32_t *formats, unsigned int fcount,
		      unsigned int zpos)
{
	int err;

	err = drm_universal_plane_init(dev, &ingenic_plane->base, possible_crtcs,
				       &ingenic_plane_funcs, formats, fcount,
				       type);
	if (err) {
		DRM_ERROR("failed to initialize plane\n");
		return err;
	}

	drm_plane_helper_add(&ingenic_plane->base, &plane_helper_funcs);

	ingenic_plane->zpos = zpos;

	return 0;
}
