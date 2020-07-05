#ifndef __ingenic_DRV_H__
#define __ingenic_DRV_H__

#include <linux/clk.h>
#include <linux/cpufreq.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/pm.h>
#include <linux/pm_runtime.h>
#include <linux/slab.h>
#include <linux/of.h>
#include <linux/of_device.h>
#include <linux/list.h>
#include <linux/reservation.h>

#include <drm/drmP.h>
#include <drm/drm_crtc_helper.h>
#include <drm/drm_gem_cma_helper.h>
#include <drm/drm_fb_cma_helper.h>
#include <drm/drm_flip_work.h>

/* Defaulting to maximum capability of DPU */
#define DPU_DEFAULT_MAX_PIXELCLOCK 300000
#define DPU_DEFAULT_MAX_WIDTH	2047
#define DPU_DEFAULT_MAX_HEIGHT	2047
#define DPU_DEFAULT_MAX_BANDWIDTH	(1080*720*60)
#define MAX_DESC_NUM 1
#define MAX_LAYER_NUM 2
#define DESC_ALIGN 8
#define MAX_BITS_PER_PIX (32)
#define MAX_STRIDE_VALUE (2047)

struct ingenic_drm_private {
	void __iomem *mmio;

	struct clk *disp_clk;    /* display dpll */
	struct clk *clk;         /* functional clock */
	int rev;                /* IP revision */

	/* don't attempt resolutions w/ higher W * H * Hz: */
	uint32_t max_bandwidth;
	/*
	 * Pixel Clock will be restricted to some value as
	 * defined in the device datasheet measured in KHz
	 */
	uint32_t max_pixelclock;
	/*
	 * Max allowable width is limited on a per device basis
	 * measured in pixels
	 */
	uint32_t max_width;
	/*
	 * Max allowable height is limited on a per device basis
	 * measured in pixels
	 */
	uint32_t max_height;

	struct workqueue_struct *wq;

	struct drm_fbdev_cma *fbdev;

	struct drm_crtc *crtc;

	struct drm_encoder *encoders;

	struct drm_connector *connectors;
	struct {
		struct drm_atomic_state *state;
		struct work_struct work;
		struct mutex lock;
	} commit;
	struct drm_device *drm_dev;
};

struct ingenic_drm_plane {
	struct drm_plane base;
	unsigned int nv_en;
	unsigned int lay_en;
	unsigned int scale_en;
	unsigned int zpos;
	unsigned int g_alpha_en;
	unsigned int g_alpha_val;
	unsigned int color;
	unsigned int domain_multi;
	unsigned int format;
	unsigned int src_x;
	unsigned int src_y;
	unsigned int src_w;
	unsigned int src_h;
	unsigned int disp_pos_x;
	unsigned int disp_pos_y;
	unsigned int scale_w;
	unsigned int scale_h;
	unsigned int stride;
	unsigned int h_ratio;
	unsigned int v_ratio;
	/*NV12, NV21 have two plane*/
	unsigned int addr_offset[2];
	struct drm_framebuffer *pending_fb;
};

enum ingenic_lcd_type {
	LCD_TYPE_TFT = 0,
	LCD_TYPE_SLCD =1,
};

enum smart_lcd_type {
	SMART_LCD_TYPE_6800,
	SMART_LCD_TYPE_8080,
	SMART_LCD_TYPE_SPI_3,
	SMART_LCD_TYPE_SPI_4,
};

/* smart lcd format */
enum smart_lcd_format {
	SMART_LCD_FORMAT_565,
	SMART_LCD_FORMAT_666,
	SMART_LCD_FORMAT_888,
};

/* smart lcd command width */
enum smart_lcd_cwidth {
	SMART_LCD_CWIDTH_8_BIT,
	SMART_LCD_CWIDTH_9_BIT,
	SMART_LCD_CWIDTH_16_BIT,
	SMART_LCD_CWIDTH_18_BIT,
	SMART_LCD_CWIDTH_24_BIT,
};

/* smart lcd data width */
enum smart_lcd_dwidth {
	SMART_LCD_DWIDTH_8_BIT,
	SMART_LCD_DWIDTH_9_BIT,
	SMART_LCD_DWIDTH_16_BIT,
	SMART_LCD_DWIDTH_18_BIT,
	SMART_LCD_DWIDTH_24_BIT,
};

enum smart_config_type {
	SMART_CONFIG_DATA,
	SMART_CONFIG_PRM,
	SMART_CONFIG_CMD,
	SMART_CONFIG_UDELAY,
};

struct smart_lcd_data_table {
	enum smart_config_type type;
	unsigned int value;
};

enum tft_lcd_color_even {
	TFT_LCD_COLOR_EVEN_RGB,
	TFT_LCD_COLOR_EVEN_RBG,
	TFT_LCD_COLOR_EVEN_BGR,
	TFT_LCD_COLOR_EVEN_BRG,
	TFT_LCD_COLOR_EVEN_GBR,
	TFT_LCD_COLOR_EVEN_GRB,
};

enum tft_lcd_color_odd {
	TFT_LCD_COLOR_ODD_RGB,
	TFT_LCD_COLOR_ODD_RBG,
	TFT_LCD_COLOR_ODD_BGR,
	TFT_LCD_COLOR_ODD_BRG,
	TFT_LCD_COLOR_ODD_GBR,
	TFT_LCD_COLOR_ODD_GRB,
};

enum tft_lcd_mode {
	TFT_LCD_MODE_PARALLEL_888,
	TFT_LCD_MODE_PARALLEL_666,
	TFT_LCD_MODE_PARALLEL_565,
	TFT_LCD_MODE_SERIAL_RGB,
	TFT_LCD_MODE_SERIAL_RGBD,
};

struct tft_config {
	unsigned int pix_clk_inv:1;
	unsigned int de_dl:1;
	unsigned int sync_dl:1;
	enum tft_lcd_color_even color_even;
	enum tft_lcd_color_odd color_odd;
	enum tft_lcd_mode mode;
};

struct smart_config {
	unsigned int frm_md:1;
	unsigned int rdy_switch:1;
	unsigned int rdy_dp:1;
	unsigned int rdy_anti_jit:1;
	unsigned int te_switch:1;
	unsigned int te_md:1;
	unsigned int te_dp:1;
	unsigned int te_anti_jit:1;
	unsigned int cs_en:1;
	unsigned int cs_dp:1;
	unsigned int dc_md:1;
	unsigned int wr_md:1;
	enum smart_lcd_type smart_type;
	enum smart_lcd_format pix_fmt;
	enum smart_lcd_dwidth dwidth;
	enum smart_lcd_cwidth cwidth;
	unsigned int bus_width;

	unsigned long write_gram_cmd;
	unsigned int length_cmd;
	struct smart_lcd_data_table *data_table;
	unsigned int length_data_table;
	int (*init) (void);
	int (*gpio_for_slcd) (void);
};

struct ingenic_crtc {
	struct drm_crtc base;

	const struct ingenic_panel_info *info;

	struct drm_pending_vblank_event *event;
	int dpms;
	bool enable;
	wait_queue_head_t frame_done_wq;
	bool frame_done;

	/* fb currently set to scanout 0/1: */
	struct drm_framebuffer *scanout[2];

	/* for deferred fb unref's: */
	struct drm_flip_work unref_work;

	/* DMA descriptors */
	struct ingenicfb_framedesc *framedesc[MAX_DESC_NUM];
	dma_addr_t framedesc_phys[MAX_DESC_NUM];
	struct ingenicfb_layerdesc *layerdesc[MAX_DESC_NUM][MAX_LAYER_NUM];
	dma_addr_t layerdesc_phys[MAX_DESC_NUM][MAX_LAYER_NUM];
	struct ingenic_drm_plane plane[MAX_LAYER_NUM];

	unsigned fence_context;
	atomic_t fence_seqno;
	struct fence *fence;
	bool async_flip;
	bool flip_busy;
	bool finish_flip;

	enum ingenic_lcd_type lcd_type;
	int slcd_continua;
	unsigned dither_enable;
	struct {
		unsigned dither_red;
		unsigned dither_green;
		unsigned dither_blue;
	} dither;
	int frm_start;
	int frm_end;

	struct dpu_dp_ops *dp_ops;
	struct ingenic_dpu_ops *dpu_ops;
};

struct ingenic_dpu_ops {
	void (*enable)(struct ingenic_crtc *crtc);
	void (*disable)(struct ingenic_crtc *crtc);
	void (*enable_vblank)(struct ingenic_crtc *crtc, bool enable);
	void (*mode_set)(struct ingenic_crtc *, struct drm_display_mode *);
	void (*disable_plane)(struct ingenic_crtc *crtc,
			      struct ingenic_drm_plane *plane);
	void (*update_plane)(struct ingenic_crtc *crtc);
	irqreturn_t (*irq_handler)(struct ingenic_crtc *crtc);
};

struct dpu_dp_ops {
	void (*enable)(struct ingenic_crtc *crtc);
	void (*disable)(struct ingenic_crtc *crtc);
	void (*mode_set)(struct ingenic_crtc *, struct drm_display_mode *);
	void (*common_mode_set)(struct ingenic_crtc *crtc);
};


#define to_ingenic_plane(x)	container_of(x, struct ingenic_drm_plane, base)

extern int ingenic_update_plane(struct drm_crtc *crtc);
extern int ingenic_disable_plane(struct drm_crtc *crtc, struct ingenic_drm_plane *plane);
enum drm_plane_type ingenic_plane_get_type(unsigned int zpos);
int ingenic_plane_init(struct drm_device *dev,
		      struct ingenic_drm_plane *ingenic_plane,
		      unsigned long possible_crtcs, enum drm_plane_type type,
		      const uint32_t *formats, unsigned int fcount,
		      unsigned int zpos);

struct drm_crtc *ingenic_crtc_create(struct drm_device *dev);
void ingenic_crtc_cancel_page_flip(struct drm_crtc *crtc,
				  struct drm_file *file);
extern irqreturn_t ingenic_crtc_irq(struct drm_crtc *crtc);
extern void ingenic_crtc_update_clk(struct drm_crtc *crtc);
int ingenic_crtc_mode_valid(struct drm_crtc *crtc,
			   struct drm_display_mode *mode);
struct reservation_object *ingenic_drv_lookup_resobj(struct drm_gem_object *obj);
extern int dpu_register(struct ingenic_crtc *crtc);
extern void enable_vblank(struct drm_crtc *crtc, bool enable);

#endif /* __ingenic_DRV_H__ */
