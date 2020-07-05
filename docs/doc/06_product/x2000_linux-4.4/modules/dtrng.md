# DTRNG模块

# 一、模块简介

DTRNG（数字真随机数发生器）用于为系统提供真随机数加密种子。

# 二、内核空间

**驱动位置**

```
drivers/char/hw_random/ingenic-rng.c
```

## 1. 设备树配置

**设备树代码位置arch/mips/boot/dts/ingenic/x2000-v12.dtsi**

```
dtrng: dtrng@0x10072000 {
                        compatible = "ingenic,dtrng";
                        reg = <0x10072000 0x100>;
                        interrupt-parent = <&core_intc>;
                        interrupts = <IRQ_DTRNG>;
                        status = "disable";
                };
```

## 2. 驱动配置

```
Symbol: HW_RANDOM_INGENIC [=y]                                              
Type  : tristate                                                            
Prompt: Ingenic HW Random Number Generator support                          
  Location:                                                                 
    -> Device Drivers                                                       
      -> Character devices                                                  
(1)     -> Hardware Random Number Generator Core support (HW_RANDOM [=y])   
  Defined at drivers/char/hw_random/Kconfig:384                             
  Depends on: HW_RANDOM [=y] && MIPS [=y] && SOC_X2000_V12 [=y]
```

# 三、用户空间

## 1. 设备节点

```
ls /dev/hwrng
```

## 2. 测试方法

**通过应用程序进行数据的读取**

```
#include<stdio.h>
#include<stdlib.h>
#include<linux/watchdog.h>
#include<sys/ioctl.h>
#include<sys/types.h>
#include<fcntl.h>
#include<unistd.h>
#include<errno.h>

int main(int argc, char *argv[])
{
        int fd ,retval;
        char buf[64] = {0};
        int cmd = 0;
        if (!argv[1])
        {
                printf("argv[1] is NULL \n");
                exit(errno);
        }
        fd = open(argv[1],O_RDONLY);
        if(fd < 0)
        {
                printf("open %s false\n",argv[1]);
                exit(errno);
        }
        if(read(fd,buf,32) < 0)
                printf(" read false\n");
        else
                printf("random_num register value %d\n",*(unsigned int *)buf);


        close(fd);
        return retval;
}
```

**读取结果**

```
# ./hwrng /dev/hwrng 
random_num register value 9363246
# ./hwrng /dev/hwrng 
random_num register value 1329736129
# ./hwrng /dev/hwrng 
random_num register value 567871915
# ./hwrng /dev/hwrng 
random_num register value 966768232
```



