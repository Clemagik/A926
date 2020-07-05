# WATCHDOG

### Documentation
>kernel/Documentation/watchdog/


### X1000 watchdog:
>kernel/drivers/watchdog/jz_wdt.c
/* #define ENABLE_WATCHDOG_ON_BOOT */


### platform data:

```
/* arch/mips/xburst/soc-x1000/common/platform.c */
#ifdef CONFIG_JZ_WDT
static struct resource jz_wdt_resources[] = {
	{
		.start = WDT_IOBASE,
		.end   = WDT_IOBASE + 0x10 - 1,
		.flags = IORESOURCE_MEM,
	},
};
struct platform_device jz_wdt_device = {
	.name	       = "jz-wdt",
	.id	       = -1,
	.num_resources = ARRAY_SIZE(jz_wdt_resources),
	.resource      = jz_wdt_resources,
};
#endif

/* arch/mips/xburst/soc-x1000/chip-x1000/halley2/common/board_base.c */
#ifdef CONFIG_JZ_WDT
       DEF_DEVICE(&jz_wdt_device, 0, 0),
#endif
```


### kernel config:
>CONFIG_WATCHDOG=y
CONFIG_WATCHDOG_CORE=y
CONFIG_JZ_WDT=y

### *softdog:*
>CONFIG_SOFT_WATCHDOG=y



### device node:

```
/ # ls -l /dev/watchdog*
crw-rw----    1 root     root       10, 130 Aug 21 12:00 /dev/watchdog
crw-rw----    1 root     root      253,   0 Aug 21 12:00 /dev/watchdog0
/ # 
```


### watchdog sample:
* kernel/Documentation/watchdog/src/watchdog-simple.c
* kernel/Documentation/watchdog/src/watchdog-test.c
* "busybox-1.26.2_watchdog.c"
>    ioctl(jz_wdt, WDIOC_SETTIMEOUT, &htimer_duration);

build for x1000:
```
$ mips-linux-gnu-gcc watchdog-test.c -o watchdog-test
$ mips-linux-gnu-gcc watchdog-simple.c -o watchdog-simple
```

```
$ ./watchdog-simple
$ ./watchdog-test -e  /* WDIOS_ENABLECARD */
$ ./watchdog-test -d  /* WDIOS_DISABLECARD */

//  set watchdog timeout
$ watchdog -t 3 -T 5 /dev/watchdog

// set watchdog timeout error. (test wdt Reset)
$ watchdog -t 35 -T 5 /dev/watchdog

$ watchdog 
BusyBox v1.26.2 (2017-07-29 14:04:58 CST) multi-call binary.
Usage: watchdog [-t N[ms]] [-T N[ms]] [-F] DEV
Periodically write to watchdog device DEV
        -T N    Reboot after N seconds if not reset (default 60)
        -t N    Reset every N seconds (default 30)
        -F      Run in foreground
Use 500ms to specify period in milliseconds

```

---


### device node: jz_wdt + softdog
>CONFIG_JZ_WDT=y
CONFIG_SOFT_WATCHDOG=y

There are 2 device nodes:
```
/ # ls -l /dev/watchdog*
crw-rw----    1 root     root       10, 130 Aug 21 13:00 /dev/watchdog       // first wdt dev: jz_wdt
crw-rw----    1 root     root      253,   0 Aug 21 13:00 /dev/watchdog0      // jz_wdt
crw-rw----    1 root     root      253,   1 Aug 21 13:00 /dev/watchdog1      // softdog
/ #
```


### test /dev/watchdog1
<pre><code class="diff">
+++kernel/Documentation/watchdog/src/watchdog-test.c
-    fd = open("/dev/watchdog", O_WRONLY);
+    fd = open("/dev/watchdog1", O_WRONLY);
</code></pre>


### Test log message:

``` log
[2018-11-28 14:24:34.731] # find /sys/ -name "*wdt*"
[2018-11-28 14:24:46.584] /sys/bus/platform/devices/wdt_reset.0
[2018-11-28 14:24:46.587] /sys/bus/platform/devices/jz-wdt
[2018-11-28 14:24:46.592] /sys/bus/platform/drivers/jz-wdt
[2018-11-28 14:24:46.595] /sys/bus/platform/drivers/jz-wdt/jz-wdt
[2018-11-28 14:24:46.598] /sys/bus/platform/drivers/wdt_reset
[2018-11-28 14:24:46.602] /sys/bus/platform/drivers/wdt_reset/wdt_reset.0
[2018-11-28 14:24:46.728] /sys/devices/platform/wdt_reset.0
[2018-11-28 14:24:46.793] /sys/devices/platform/jz-wdt
[2018-11-28 14:24:46.906] # 
[2018-11-28 14:24:47.650] # 
# find /sys/ -name "*watchdog*"
[2018-11-28 14:24:51.509] /sys/devices/virtual/misc/watchdog
[2018-11-28 14:24:51.523] /sys/devices/virtual/watchdog
[2018-11-28 14:24:51.526] /sys/devices/virtual/watchdog/watchdog0
[2018-11-28 14:24:51.530] /sys/devices/virtual/watchdog/watchdog1
[2018-11-28 14:24:51.627] /sys/class/misc/watchdog
[2018-11-28 14:24:51.630] /sys/class/watchdog
[2018-11-28 14:24:51.632] /sys/class/watchdog/watchdog0
[2018-11-28 14:24:51.634] /sys/class/watchdog/watchdog1
[2018-11-28 14:24:51.697] # 
[2018-11-28 14:24:52.378] # 
[2018-11-28 14:24:52.523] # ls -l /dev/watchdog*
[2018-11-28 14:25:01.930] crw-rw----    1 root     root       10, 130 Aug 21 12:17 /dev/watchdog
[2018-11-28 14:25:01.938] crw-rw----    1 root     root      253,   0 Aug 21 12:17 /dev/watchdog0
[2018-11-28 14:25:01.944] crw-rw----    1 root     root      253,   1 Aug 21 12:17 /dev/watchdog1
[2018-11-28 14:25:01.951] # 
[2018-11-28 14:25:03.458] # 
[2018-11-28 14:25:03.642] # # set watchdog timeout error. (test wdt Reset)
[2018-11-28 14:25:15.663] # watchdog -t 35 -T 5 /dev/watchdog
[2018-11-28 14:25:15.679] # 
[2018-11-28 14:25:17.289] # ps
[2018-11-28 14:25:18.664] PID   USER     TIME   COMMAND
[2018-11-28 14:25:18.666]     1 root       0:01 {linuxrc} init
[2018-11-28 14:25:18.670]     2 root       0:00 [kthreadd]
[2018-11-28 14:25:18.673]     3 root       0:00 [ksoftirqd/0]
[2018-11-28 14:25:18.676]     4 root       0:00 [kworker/0:0]
[2018-11-28 14:25:18.679]     5 root       0:00 [kworker/0:0H]
[2018-11-28 14:25:18.682]     6 root       0:00 [kworker/u2:0]
[2018-11-28 14:25:18.686]     7 root       0:00 [rcu_preempt]
[2018-11-28 14:25:18.689]     8 root       0:00 [rcu_bh]
[2018-11-28 14:25:18.693]     9 root       0:00 [rcu_sched]
[2018-11-28 14:25:18.695]    10 root       0:00 [khelper]
[2018-11-28 14:25:18.697]    11 root       0:00 [writeback]
[2018-11-28 14:25:18.700]    12 root       0:00 [bioset]
[2018-11-28 14:25:18.703]    13 root       0:00 [kblockd]
[2018-11-28 14:25:18.706]    14 root       0:00 [khubd]
[2018-11-28 14:25:18.709]    15 root       0:00 [kworker/0:1]
[2018-11-28 14:25:18.712]    16 root       0:00 [cfg80211]
[2018-11-28 14:25:18.715]    17 root       0:00 [irq/158-usb-det]
[2018-11-28 14:25:18.718]    18 root       0:00 [irq/157-usb-hos]
[2018-11-28 14:25:18.722]    19 root       0:00 [kswapd0]
[2018-11-28 14:25:18.725]    20 root       0:00 [crypto]
[2018-11-28 14:25:18.728]    33 root       0:00 [kworker/u2:1]
[2018-11-28 14:25:18.731]    36 root       0:00 [irq/9-jz-asoc-a]
[2018-11-28 14:25:18.735]    37 root       0:00 [kworker/u2:2]
[2018-11-28 14:25:18.738]    38 root       0:00 [krfcommd]
[2018-11-28 14:25:18.741]    39 root       0:00 [deferwq]
[2018-11-28 14:25:18.744]    40 root       0:00 [mmcqd/0]
[2018-11-28 14:25:18.746]    41 root       0:00 [wl_event_handle]
[2018-11-28 14:25:18.750]    42 root       0:00 [dhd_watchdog_th]
[2018-11-28 14:25:18.754]    43 root       0:00 [dhd_dpc]
[2018-11-28 14:25:18.756]    44 root       0:00 [dhd_rxf]
[2018-11-28 14:25:18.759]    45 root       0:00 [f_mtp]
[2018-11-28 14:25:18.762]    46 root       0:00 [file-storage]
[2018-11-28 14:25:18.765]    55 root       0:00 [jffs2_gcd_mtd2]
[2018-11-28 14:25:18.769]    66 root       0:00 /sbin/syslogd -n
[2018-11-28 14:25:18.772]    69 root       0:00 /sbin/klogd -n
[2018-11-28 14:25:18.776]    78 root       0:00 [kworker/0:1H]
[2018-11-28 14:25:18.779]    99 root       0:00 {exe} ash /sbin/adbserver.sh
[2018-11-28 14:25:18.783]   104 root       0:00 /sbin/adbd
[2018-11-28 14:25:18.786]   106 root       0:00 -/bin/sh
[2018-11-28 14:25:18.789]   118 root       0:00 [kworker/0:2]
[2018-11-28 14:25:18.792]   124 root       0:00 wpa_supplicant -Dnl80211 -iwlan0 -c/etc/wpa_supplicant.co
[2018-11-28 14:25:18.799]   140 root       0:00 watchdog -t 35 -T 5 /dev/watchdog
[2018-11-28 14:25:18.804]   141 root       0:00 ps
[2018-11-28 14:25:18.806] # 
[2018-11-28 14:25:19.299] # 
[2018-11-28 14:25:20.726] U-Boot SPL 2013.07-00006-g9615684 (Nov 14 2018 - 17:29:39)
[2018-11-28 14:25:20.731] ERROR EPC 8000db34
[2018-11-28 14:25:20.734] CPM_CPAPCR:a9008520
[2018-11-28 14:25:20.736] CPM_CPMPCR:98000083
[2018-11-28 14:25:20.738] CPM_CPCCR:9a052210
[2018-11-28 14:25:20.743] WIN25Q256JVEQ 00ef4019
[2018-11-28 14:25:20.775] 
[2018-11-28 14:25:20.775] 
[2018-11-28 14:25:20.776] U-Boot 2013.07-00006-g9615684 (Nov 14 2018 - 17:29:39)
[2018-11-28 14:25:20.780] 
[2018-11-28 14:25:20.781] Board: Halley2 (Ingenic XBurst X1000 SoC)
[2018-11-28 14:25:20.784] DRAM:  32 MiB
[2018-11-28 14:25:20.786] Top of RAM usable for U-Boot at: 82000000

```
