## RTC

### hardware reuirement
X1000 RTC alarm wake up require RTC32KHz crystal.


### Documentation
>kernel/Documentation/rtc.txt


### X1000 rtc config:
>kernel/.config
CONFIG_RTC_LIB=y
CONFIG_RTC_CLASS=y
CONFIG_RTC_DRV_JZ=y

### X1000 rtc driver:
>kernel/drivers/rtc/rtc-jz.c

### platform data:
```
/* arch/mips/xburst/soc-x1000/common/platform.c */
#ifdef CONFIG_RTC_DRV_JZ
static struct resource jz_rtc_resource[] = {
	[0] = {
		.start = RTC_IOBASE,
		.end   = RTC_IOBASE + 0xff,
		.flags = IORESOURCE_MEM,
	},
	[1] = {
		.start = IRQ_RTC,
		.end   = IRQ_RTC,
		.flags = IORESOURCE_IRQ,
	}
};
struct platform_device jz_rtc_device = {
	.name             = "jz-rtc",
	.id               = 0,
	.num_resources    = ARRAY_SIZE(jz_rtc_resource),
	.resource         = jz_rtc_resource,
};
#endif

/* arch/mips/xburst/soc-x1000/chip-x1000/halley2/common/board_base.c */
#ifdef CONFIG_RTC_DRV_JZ
	DEF_DEVICE(&jz_rtc_device, 0, 0),
#endif
```


### device node:

```
rtc boot log:
[   20.042429] jz-rtc jz-rtc.0: rtc core: registered jz-rtc as rtc0
[   20.048766] jz-rtc: rtc power on reset !
[   20.789444] jz-rtc jz-rtc.0: setting system clock to 2014-08-21 12:00:01 UTC (1408622401)

# ls -l /dev/rtc*
crw-rw----    1 root     root      254,   0 Aug 21 12:00 /dev/rtc0
```

### rtcwake sample
```
# rtcwake --help
BusyBox v1.26.2 (2018-01-19 15:05:00 CST) multi-call binary.

Usage: rtcwake [-a | -l | -u] [-d DEV] [-m MODE] [-s SEC | -t TIME]

Enter a system sleep state until specified wakeup time

        -a,--auto       Read clock mode from adjtime
        -l,--local      Clock is set to local time
        -u,--utc        Clock is set to UTC time
        -d,--device=DEV Specify the RTC device
        -m,--mode=MODE  Set sleep state (default: standby)
        -s,--seconds=SEC Set timeout in SEC seconds from now
        -t,--time=TIME  Set timeout to TIME seconds from epoch
# 
```

### rtcwake test log

```
# rtcwake -s 10 -m mem
[2019-09-02 15:32:13] wakeup from "mem" at Thu Aug 21 12:00:27 2014
[2019-09-02 15:32:13] [   35.686956] PM: suspend entry 2014-08-21 12:00:16.397522607 UTC
[2019-09-02 15:32:13] [   35.693070] PM: Syncing filesystems ... done.
[2019-09-02 15:32:13] [   35.704377] clk_sleep_pm_callback PM_SUSPEND_PREPARE
[2019-09-02 15:32:13] [   35.709734] Freezing user space processes ... (elapsed 0.001 seconds) done.
[2019-09-02 15:32:13] [   35.735054] Freezing remaining freezable tasks ... (elapsed 0.001 seconds) done.
[2019-09-02 15:32:13] [   35.743942] Suspending console(s) (use no_console_suspend to debug)

[2019-09-02 15:32:25] [   35.751538] jz-dwc2 jz-dwc2: set vbus off(off) for host mode
[2019-09-02 15:32:25] [   35.752004] ======truly_tft240240_power_init==============
[2019-09-02 15:32:25] [   35.752013] set lcd_power.inited  =======1 
[2019-09-02 15:32:25] [   35.758714] device_may_wakeup(&pdev->dev) = 0
[2019-09-02 15:32:25] [   35.758754] device_may_wakeup(&pdev->dev) = 0
[2019-09-02 15:32:25] [   35.758817] PM: suspend of devices complete after 8.030 msecs
[2019-09-02 15:32:25] [   35.759186] PM: late suspend of devices complete after 0.356 msecs
[2019-09-02 15:32:25] [   35.759754] PM: noirq suspend of devices complete after 0.552 msecs
[2019-09-02 15:32:25] [    0.000000] clk suspend!
[2019-09-02 15:32:25] [    0.000000] #####lcr:0000ff01
[2019-09-02 15:32:25] [    0.000000] #####gate:07dabfde
[2019-09-02 15:32:25] [    0.000000] #####opcr:c28fff2e
[2019-09-02 15:32:25] [    0.000000] #####INT_MASK0:fffc3fff
[2019-09-02 15:32:25] [    0.000000] #####INT_MASK1:fffffffe
[2019-09-02 15:32:25] [    0.000000] clk resume!
[2019-09-02 15:32:25] [    0.000000] Suspended for 0.000 seconds
[2019-09-02 15:32:25] [   35.761637] PM: noirq resume of devices complete after 0.340 msecs
[2019-09-02 15:32:25] [   35.762240] PM: early resume of devices complete after 0.384 msecs
[2019-09-02 15:32:25] [   35.762699] must set rate before enable
[2019-09-02 15:32:25] [   35.776208] graphics fb0: LCD V COLOR BAR w,h,bpp(240,240,16) jzfb->vidmem=a3e00000
[2019-09-02 15:32:25] [   35.863539] DWC USB disconnect
[2019-09-02 15:32:25] [   35.863860] restore_pin_status is not defined
[2019-09-02 15:32:25] [   36.163726] PM: resume of devices complete after 401.458 msecs
[2019-09-02 15:32:25] [   36.271577] Restarting tasks ... done.
[2019-09-02 15:32:25] [   36.280352] clk_sleep_pm_callback PM_POST_SUSPEND
[2019-09-02 15:32:25] [   36.285787] PM: suspend exit 2014-08-21 12:00:28.122331393 UTC
[2019-09-02 15:32:25] # 
```
