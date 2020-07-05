# GPIO模块

x1830的gpio控制器有五组，A, B, C, D, Z。

支持输入输出和设备复用功能。 内核的gpio驱动程序是基于gpio子系统架构编写的。应用程序可以使用gpio\_demo进行测试。内核驱动可以在内核空间使用，也可以通过导出gpio sys节点到用户空间，在用户空间进行操作。

## 文件介绍

gpio 一般在进行开发板设计的时候就已经固定好了，有的gpio只能作为设备复用功能管脚， 有的gpio作为普通的输入输出和中断检测功能，对于固定设备复用的功能管脚在以下文件中定义：

```
arch/mips/xburst/soc-x1830/include/mach/platform.h

在arch/mips/xburst/soc-x1830/common/platform.c 会根据驱动配置，选中相应的设备功能管脚。
```

内核的gpio驱动基于gpio子系统实现的， 所以其他驱动程序可以通过内核提供的libgpio接口，很方便的进行gpio控制，例如 gpio\_request\_one,  gpio\_get\_value, gpio\_set\_value等。

gpio 驱动文件所在位置：

```
arch/mips/xburst/soc-x1830/common/gpio.c
```

## 编译配置

-\*- GPIO Support    /\*内核通过配置 CONFIG\_GPIOLIB 选项可以使用 gpio 功能，默认必须选上\*/

```
--- GPIO Support 

[]   Debug GPIO calls

/*如果要将 gpio 导出到用户节点/sys/class/gpio 下，对该节点下的文件操作，可以配置以下选项*/

[*]   /sys/class/gpio/... (sysfs interface)

*** Memory mapped GPIO drivers: ***

< > Generic memory-mapped GPIO controller support (MMIO platform device)
```

&lt; &gt; Dallas's 1-wire support  ---&gt;

## 用户空间

在内核导出gpio节点的前提下， 可以操作/sys/class/gpio节点，控制gpio输入输出。

```
  /sys/class/gpio/

  "export" 用户空间可以通过写其编号到这个文件， 要求内核导出一个GPIO的控制到用户空间。

   例如：如果内核代码没有申请GPIO #19，“echo 19; export”将会为 GPIO #19 创建一个 “gpio19” 节点。

  “unexpect” 导出到用户空间的逆操作。

   例如：“echo 19; unexpect” 将会移除使用“export”文件导出的“gpio19”节点。
```

GPIO信号的路径似  /sys/class/gpio/gpio42/ \(对于GPIO \#42 来说\)，并有如下的读写属性：

/sys/class/gpio/gpioN/

"direction" 读取得到“in” 或 “out”。这个值通常运行写入。

“edge”读取得到“none”、“rising”、“failing”或者“both”。

“value” 读取得到0（低电平）或1（高电平\)。

“active\_low” 读取得到 0 （假）或 1 （真）。

