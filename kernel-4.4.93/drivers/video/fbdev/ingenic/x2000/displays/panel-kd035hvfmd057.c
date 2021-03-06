/*
 * driver/video/fbdev/ingenic/x2000/displays/kd035hvfmd057.c
 *
 * Copyright (C) 2016 Ingenic Semiconductor Inc.
 *
 * Author:clwang<chunlei.wang@ingenic.com>
 *
 * This program is free software, you can redistribute it and/or modify it
 *
 * under the terms of the GNU General Public License version 2 as published by
 *
 * the Free Software Foundation.
 */

#include <linux/init.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/gpio.h>
#include <linux/pwm_backlight.h>
#include <linux/delay.h>
#include <linux/lcd.h>
#include <linux/of_gpio.h>

#include "../ingenicfb.h"

struct board_gpio {
	short gpio;
	short active_level;
};

struct panel_dev {
	/* ingenic frame buffer */
	struct device *dev;
	struct lcd_panel *panel;

	/* common lcd framework */
	struct lcd_device *lcd;
	struct backlight_device *backlight;
	int power;

	struct regulator *vcc;
	struct board_gpio cs;
	struct board_gpio rst;
	struct board_gpio ssi_clk;
	struct board_gpio ssi_dt;
	struct board_gpio pwm;
};

#define SSI_CS(panel, n)	gpio_direction_output(panel->cs.gpio, n)
#define SSI_CLK(panel, n)	gpio_direction_output(panel->ssi_clk.gpio, n)
#define SSI_DT(panel, n)	gpio_direction_output(panel->ssi_dt.gpio, n)

static void lcd_send_cmd(struct panel_dev *panel, uint8_t cmd) {

	int i = 8;
	SSI_CS(panel, 0);
	SSI_CLK(panel, 0);
	SSI_DT(panel, 0);
	SSI_CLK(panel, 1);
	while(i--) {
		SSI_CLK(panel, 0);
		SSI_DT(panel, !!(cmd & 0x80));
		SSI_CLK(panel, 1);
		cmd = cmd << 1;
	}
	SSI_CLK(panel, 0);
	SSI_CS(panel, 1);
}

static void lcd_send_param(struct panel_dev *panel, uint8_t param) {

	int i = 8;
	SSI_CS(panel, 0);
	SSI_CLK(panel, 0);
	SSI_DT(panel, 1);
	SSI_CLK(panel,1);
	while(i--) {
		SSI_CLK(panel, 0);
		SSI_DT(panel, !!(param & 0x80));
		SSI_CLK(panel, 1);
		param = param << 1;
	}
	SSI_CLK(panel, 0);
	SSI_CS(panel, 1);

}

static void panel_init(struct panel_dev *panel) {
#define LCD_SEND_CMD(n) lcd_send_cmd(panel, n)
#define LCD_SEND_PARAM(n) lcd_send_param(panel, n)
	LCD_SEND_CMD(0xC0);
	LCD_SEND_PARAM(0x17);
	LCD_SEND_PARAM(0x17);

	LCD_SEND_CMD(0xC1);
	LCD_SEND_PARAM(0x44);

	LCD_SEND_CMD(0xC5);
	LCD_SEND_PARAM(0x00);
	LCD_SEND_PARAM(0x3A);
	LCD_SEND_PARAM(0x80);

	LCD_SEND_CMD(0x36);
	LCD_SEND_PARAM(0x48);

	LCD_SEND_CMD(0x3A); //Interface Mode Control
	LCD_SEND_PARAM(0x60);

	LCD_SEND_CMD(0xB1);   //Frame rate 70HZ
	LCD_SEND_PARAM(0xA0);

	LCD_SEND_CMD(0xB4);
	LCD_SEND_PARAM(0x02);

	LCD_SEND_CMD(0xB7);
	LCD_SEND_PARAM(0xC6);

	LCD_SEND_CMD(0xE9);
	LCD_SEND_PARAM(0x00);

	LCD_SEND_CMD(0XF7);
	LCD_SEND_PARAM(0xA9);
	LCD_SEND_PARAM(0x51);
	LCD_SEND_PARAM(0x2C);
	LCD_SEND_PARAM(0x82);

	LCD_SEND_CMD(0xE0);
	LCD_SEND_PARAM(0x01);
	LCD_SEND_PARAM(0x13);
	LCD_SEND_PARAM(0x1E);
	LCD_SEND_PARAM(0x00);
	LCD_SEND_PARAM(0x0D);
	LCD_SEND_PARAM(0x03);
	LCD_SEND_PARAM(0x3D);
	LCD_SEND_PARAM(0x55);
	LCD_SEND_PARAM(0x4F);
	LCD_SEND_PARAM(0x06);
	LCD_SEND_PARAM(0x10);
	LCD_SEND_PARAM(0x0B);
	LCD_SEND_PARAM(0x2C);
	LCD_SEND_PARAM(0x32);
	LCD_SEND_PARAM(0x0F);

	LCD_SEND_CMD(0xE1);
	LCD_SEND_PARAM(0x08);
	LCD_SEND_PARAM(0x10);
	LCD_SEND_PARAM(0x15);
	LCD_SEND_PARAM(0x03);
	LCD_SEND_PARAM(0x0E);
	LCD_SEND_PARAM(0x03);
	LCD_SEND_PARAM(0x32);
	LCD_SEND_PARAM(0x34);
	LCD_SEND_PARAM(0x44);
	LCD_SEND_PARAM(0x07);
	LCD_SEND_PARAM(0x10);
	LCD_SEND_PARAM(0x0E);
	LCD_SEND_PARAM(0x23);
	LCD_SEND_PARAM(0x2E);
	LCD_SEND_PARAM(0x0F);

	LCD_SEND_CMD(0xB6);
	LCD_SEND_PARAM(0xB0); //set rgb bypass mode
	LCD_SEND_PARAM(0x02); //GS,SS
	LCD_SEND_PARAM(0x3B);

	/****************************************/
	LCD_SEND_CMD(0XB0);  //Interface Mode Control
	LCD_SEND_PARAM(0x02);
	 /**************************************************/
	LCD_SEND_CMD(0x2A); //Frame rate control
	LCD_SEND_PARAM(0x00);
	LCD_SEND_PARAM(0x00);
	LCD_SEND_PARAM(0x01);
	LCD_SEND_PARAM(0x3F);

	LCD_SEND_CMD(0x2B); //Display function control
	LCD_SEND_PARAM(0x00);
	LCD_SEND_PARAM(0x00);
	LCD_SEND_PARAM(0x01);
	LCD_SEND_PARAM(0xDF);

	LCD_SEND_CMD(0x21);

	LCD_SEND_CMD(0x11);
	mdelay(20);
	LCD_SEND_CMD(0x29); //display on
	LCD_SEND_CMD(0x2c);
}

static void panel_enable(struct lcd_panel *panel)
{
}

static void panel_disable(struct lcd_panel *panel)
{
}

static struct lcd_panel_ops panel_ops = {
	.enable  = (void*)panel_enable,
	.disable = (void*)panel_disable,
};

static struct fb_videomode panel_modes[] = {
	[0] = {
		.name = "320x480",
		.refresh = 60,
		.xres = 320,
		.yres = 480,
		.pixclock = KHZ2PICOS(33264),
		.left_margin  = 3,
		.right_margin = 60,
		.upper_margin = 2,
		.lower_margin = 2,
		.hsync_len = 20,
		.vsync_len = 2,
		.sync = ~FB_SYNC_HOR_HIGH_ACT & ~FB_SYNC_VERT_HIGH_ACT,
		.vmode = FB_VMODE_NONINTERLACED,
		.flag = 0,
	},
};

static struct tft_config kd035hvfmd057_cfg = {
	.pix_clk_inv = 0,
	.de_dl = 0,
	.sync_dl = 0,
	.color_even = TFT_LCD_COLOR_EVEN_RGB,
	.color_odd = TFT_LCD_COLOR_ODD_RGB,
	.mode = TFT_LCD_MODE_PARALLEL_666,
};

struct lcd_panel lcd_panel = {
	.name = "kd035hvfmd057",
	.num_modes = ARRAY_SIZE(panel_modes),
	.modes = panel_modes,
	.bpp = 24,
	.width = 320,
	.height = 480,

	.lcd_type = LCD_TYPE_TFT,

	.tft_config = &kd035hvfmd057_cfg,

	.dither_enable = 0,
	.dither.dither_red = 0,
	.dither.dither_green = 0,
	.dither.dither_blue = 0,

	.ops = &panel_ops,
};


/* SGM3146 supports 16 brightness step */
#define MAX_BRIGHTNESS_STEP     16
/* System support 256 brightness step */
#define CONVERT_FACTOR          (256/MAX_BRIGHTNESS_STEP)

static int panel_update_status(struct backlight_device *bd)
{
	struct panel_dev *panel = dev_get_drvdata(&bd->dev);
	int brightness = bd->props.brightness;
	unsigned int i;
	int pulse_num = MAX_BRIGHTNESS_STEP - brightness / CONVERT_FACTOR - 1;

	if (bd->props.fb_blank == FB_BLANK_POWERDOWN) {
		return 0;
	}

	if (bd->props.state & BL_CORE_SUSPENDED)
		brightness = 0;

	if (brightness) {
		gpio_direction_output(panel->pwm.gpio,0);
		udelay(5000);
		gpio_direction_output(panel->pwm.gpio,1);
		udelay(100);

		for (i = pulse_num; i > 0; i--) {
			gpio_direction_output(panel->pwm.gpio,0);
			udelay(1);
			gpio_direction_output(panel->pwm.gpio,1);
			udelay(3);
		}
	} else
		gpio_direction_output(panel->pwm.gpio, 0);

	return 0;
}

static struct backlight_ops panel_backlight_ops = {
	.options = BL_CORE_SUSPENDRESUME,
	.update_status = panel_update_status,
};

static void panel_power_reset(struct board_gpio *rst)
{
	gpio_direction_output(rst->gpio, 0);
	mdelay(10);
	gpio_direction_output(rst->gpio, 1);
	mdelay(20);
}

#define POWER_IS_ON(pwr)        ((pwr) <= FB_BLANK_NORMAL)
static int panel_set_power(struct lcd_device *lcd, int power)
{
	struct panel_dev *panel = lcd_get_data(lcd);
	struct board_gpio *cs = &panel->cs;
	struct board_gpio *rst = &panel->rst;
	if(POWER_IS_ON(power) && !POWER_IS_ON(panel->power)) {
		gpio_direction_output(rst->gpio, 1);
		gpio_direction_output(cs->gpio, 1);
		panel_power_reset(rst);
		gpio_direction_output(cs->gpio, 0);
		panel_init(panel);
	}
	if(!POWER_IS_ON(power) && POWER_IS_ON(panel->power)) {
		gpio_direction_output(cs->gpio, 1);
		gpio_direction_output(rst->gpio, 0);
	}

	panel->power = power;
        return 0;
}

static int panel_get_power(struct lcd_device *lcd)
{
	struct panel_dev *panel = lcd_get_data(lcd);

	return panel->power;
}

/**
* @ pannel_kd035hvfmd057_lcd_ops, register to kernel common backlight/lcd.c framworks.
*/
static struct lcd_ops panel_lcd_ops = {
	.early_set_power = panel_set_power,
	.set_power = panel_set_power,
	.get_power = panel_get_power,
};

static int of_panel_parse(struct device *dev)
{
	struct panel_dev *panel = dev_get_drvdata(dev);
	struct device_node *np = dev->of_node;
	enum of_gpio_flags flags;
	int ret = 0;

	panel->cs.gpio = of_get_named_gpio_flags(np, "ingenic,cs-gpio", 0, &flags);
	if(gpio_is_valid(panel->cs.gpio)) {
		panel->cs.active_level = OF_GPIO_ACTIVE_LOW ? 0 : 1;
		ret = gpio_request_one(panel->cs.gpio, GPIOF_DIR_OUT, "cs");
		if(ret < 0) {
			dev_err(dev, "Failed to request cs pin!\n");
			return ret;
		}
	} else {
		dev_warn(dev, "invalid gpio cs.gpio: %d\n", panel->cs.gpio);
	}

	panel->rst.gpio = of_get_named_gpio_flags(np, "ingenic,rst-gpio", 0, &flags);
	if(gpio_is_valid(panel->rst.gpio)) {
		panel->rst.active_level = OF_GPIO_ACTIVE_LOW ? 0 : 1;
		ret = gpio_request_one(panel->rst.gpio, GPIOF_DIR_OUT, "rst");
		if(ret < 0) {
			dev_err(dev, "Failed to request rst pin!\n");
			goto err_request_rst;
		}
	} else {
		dev_warn(dev, "invalid gpio rst.gpio: %d\n", panel->rst.gpio);
	}
	panel->ssi_clk.gpio = of_get_named_gpio_flags(np, "ingenic,ssi-clk-gpio", 0, &flags);
	if(gpio_is_valid(panel->ssi_clk.gpio)) {
		panel->ssi_clk.active_level = OF_GPIO_ACTIVE_LOW ? 0 : 1;
		ret = gpio_request_one(panel->ssi_clk.gpio, GPIOF_DIR_OUT, "ssi_clk");
		if(ret < 0) {
			dev_err(dev, "Failed to request ssi_clk pin!\n");
			goto err_request_rst;
		}
	} else {
		dev_warn(dev, "invalid gpio ssi_clk.gpio: %d\n", panel->ssi_clk.gpio);
	}
	panel->ssi_dt.gpio = of_get_named_gpio_flags(np, "ingenic,ssi-dt-gpio", 0, &flags);
	if(gpio_is_valid(panel->ssi_dt.gpio)) {
		panel->ssi_dt.active_level = OF_GPIO_ACTIVE_LOW ? 0 : 1;
		ret = gpio_request_one(panel->ssi_dt.gpio, GPIOF_DIR_OUT, "ssi_dt");
		if(ret < 0) {
			dev_err(dev, "Failed to request ssi_dt pin!\n");
			goto err_request_rst;
		}
	} else {
		dev_warn(dev, "invalid gpio ssi_dt.gpio: %d\n", panel->ssi_dt.gpio);
	}
	panel->pwm.gpio = of_get_named_gpio_flags(np, "ingenic,pwm-gpio", 0, &flags);
	if(gpio_is_valid(panel->pwm.gpio)) {
		panel->pwm.active_level = OF_GPIO_ACTIVE_LOW ? 0 : 1;
		ret = gpio_request_one(panel->pwm.gpio, GPIOF_DIR_OUT, "pwm");
		if(ret < 0) {
			dev_err(dev, "Failed to request pwm pin!\n");
			goto err_request_rst;
		}
	} else {
		dev_warn(dev, "invalid gpio pwm.gpio: %d\n", panel->pwm.gpio);
	}

	return 0;
err_request_rst:
	gpio_free(panel->cs.gpio);
	return ret;
}
/**
* @panel_probe
*
* 	1. Register to ingenicfb.
* 	2. Register to lcd.
* 	3. Register to backlight if possible.
*
* @pdev
*
* @Return -
*/
static int panel_probe(struct platform_device *pdev)
{
	int ret = 0;
	struct panel_dev *panel;
	struct backlight_properties props;

	memset(&props, 0, sizeof(props));
	panel = kzalloc(sizeof(struct panel_dev), GFP_KERNEL);
	if(panel == NULL) {
		dev_err(&pdev->dev, "Faile to alloc memory!");
		return -ENOMEM;
	}
	panel->dev = &pdev->dev;
	dev_set_drvdata(&pdev->dev, panel);

	ret = of_panel_parse(&pdev->dev);
	if(ret < 0) {
		goto err_of_parse;
	}

	panel->lcd = lcd_device_register("panel_lcd", &pdev->dev, panel, &panel_lcd_ops);
	if(IS_ERR_OR_NULL(panel->lcd)) {
		dev_err(&pdev->dev, "Error register lcd!\n");
		ret = -EINVAL;
		goto err_of_parse;
	}

	/* TODO: should this power status sync from uboot */
	panel->power = FB_BLANK_POWERDOWN;
	panel_set_power(panel->lcd, FB_BLANK_UNBLANK);

	props.type = BACKLIGHT_RAW;
	props.max_brightness = 255;
	panel->backlight = backlight_device_register("pwm-backlight.0",
						&pdev->dev, panel,
						&panel_backlight_ops,
						&props);
	if (IS_ERR_OR_NULL(panel->backlight)) {
		dev_err(panel->dev, "failed to register 'pwm-backlight.0'.\n");
		goto err_lcd_register;
	}
	panel->backlight->props.brightness = props.max_brightness;
	backlight_update_status(panel->backlight);

	ret = ingenicfb_register_panel(&lcd_panel);
	if(ret < 0) {
		dev_err(&pdev->dev, "Failed to register lcd panel!\n");
		goto err_lcd_register;
	}

	return 0;

err_lcd_register:
	lcd_device_unregister(panel->lcd);
err_of_parse:
	kfree(panel);
	return ret;
}

static int panel_remove(struct platform_device *pdev)
{
	struct panel_dev *panel = dev_get_drvdata(&pdev->dev);

	panel_set_power(panel->lcd, FB_BLANK_POWERDOWN);
	return 0;
}

#ifdef CONFIG_PM
static int panel_suspend(struct device *dev)
{
	struct panel_dev *panel = dev_get_drvdata(dev);

	panel_set_power(panel->lcd, FB_BLANK_POWERDOWN);
	return 0;
}

static int panel_resume(struct device *dev)
{
	struct panel_dev *panel = dev_get_drvdata(dev);

	panel_set_power(panel->lcd, FB_BLANK_UNBLANK);
	return 0;
}

static const struct dev_pm_ops panel_pm_ops = {
	.suspend = panel_suspend,
	.resume = panel_resume,
};
#endif
static const struct of_device_id panel_of_match[] = {
	{ .compatible = "ingenic,kd035hvfmd057", },
	{},
};

static struct platform_driver panel_driver = {
	.probe		= panel_probe,
	.remove		= panel_remove,
	.driver		= {
		.name	= "kd035hvfmd057",
		.of_match_table = panel_of_match,
#ifdef CONFIG_PM
		.pm = &panel_pm_ops,
#endif
	},
};

module_platform_driver(panel_driver);
