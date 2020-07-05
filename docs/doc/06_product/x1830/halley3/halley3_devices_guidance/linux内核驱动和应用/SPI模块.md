# SPI模块

x1830 的 spi 控制器为 SSI，SSI 支持的接口有 Microwire，SSP，SPI。SPI 接口可连接至 spi nor，支持 spi 读写功能。内核的 spi驱动程序是基于 spi 子系统架构编写的。应用程序可以使用 spi\_demo进行测试。Spi 总线下，可以挂普通的char设备，也可以挂 mtd 设备。可以在内核空间通过 spi驱动操作 spi 接口，也可以在用户空间通过 spi\_ioc\_transfer 操作 spi 接口。\`

## 文件介绍

```
内核驱动基于 spi 驱动架构,所有的硬件资源在板级定义。

按照 spi 驱动编写规范，需要提供 spi_board_info。

以下介绍驱动对应在内核中的路径
    板级资源定义路径:
        ingenic/x1830/board/common/spi_bus.c，这个文件下面是 spi 的设备。

与 X1830 spi 相关的驱动所在目录和文件说明,忽略目录中存在的其他文件。
drivers/spi/
    ├── jz_spi.c
    ├── jz_spi.h
    ├── spi-bitbang.c
    ├── spi.c
    ├── spidev.c
```

## 编译配置

```
    一般发布的软件版本中会默认配置 spi 驱动,如果需要自己更改 spi 的编译选项,可以通过以下方式进行配置。
在工作电脑上执行:
    $ make menuconfig

配置以下选项:
--- SPI support
  [ ]   Debug support for SPI drivers
          *** SPI Master Controller Drivers ***
  < >   Altera SPI Controller
  < >   Ingeinc JZ47XX SPI controller test driver
  <*>   Ingenic JZ series SPI driver
  [ ]     Ingenic SoC SSI controller 0 for SPI Host driver
  [*]     Ingenic SoC SSI controller 1 for SPI Host driver
  [ ]       Disable DMA (always use PIO) on JZ SSI controller 1
              JZ SSI1 controller function pins select (PORT C 19BIT)  --->
  [*]     Board info associated by spi master
  [ ]     Use GPIO CE on JZ SSI controller 0
  < >     Ingenic spi test driver
  [ ]     Ingenic SoC SSI controller select external clk
  -*-   Utilities for Bitbanging SPI masters  
          *** SPI Protocol Masters ***
      /*配置以下选项，可以把 spi 的设备节点导出到/dev 下，供用户空间使用*/             
  <*>   User mode SPI device driver support                                      
  < >   Infineon TLE62X0 (for power switching)
```

## 测试应用

```
    在Documentation/spi/spidev\_test.c中， 通过更改device编号（eg：/dev/spidev1.1），然后将编译生成的二进制文件导入文件系统并执行测试。
```

```
#include <stdint.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>

#define ARRAY_SIZE(a) (sizeof(a) / sizeof((a)[0]))

static void pabort(const char *s)
{
        perror(s);
        abort();
}

static const char *device = "/dev/spidev1.1";
static uint8_t mode;
static uint8_t bits = 8;
static uint32_t speed = 500000;
static uint16_t delay;

static void transfer(int fd)
{
        int ret;
        uint8_t tx[] = {
                0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
                0x40, 0x00, 0x00, 0x00, 0x00, 0x95,
                0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
                0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
                0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
                0xDE, 0xAD, 0xBE, 0xEF, 0xBA, 0xAD,
                0xF0, 0x0D,
        };
        uint8_t rx[ARRAY_SIZE(tx)] = {0, };
        struct spi_ioc_transfer tr = {
                .tx_buf = (unsigned long)tx,
                .rx_buf = (unsigned long)rx,
                .len = ARRAY_SIZE(tx),
                .delay_usecs = delay,
                .speed_hz = speed,
                .bits_per_word = bits,
        };

        ret = ioctl(fd, SPI_IOC_MESSAGE(1), &tr);
        if (ret < 1)
                pabort("can't send spi message");

        for (ret = 0; ret < ARRAY_SIZE(tx); ret++) {
                if (!(ret % 6))
                        puts("");
                printf("%.2X ", rx[ret]);
        }
        puts("");
}

static void print_usage(const char *prog)
{
        printf("Usage: %s [-DsbdlHOLC3]\n", prog);
        puts("  -D --device   device to use (default /dev/spidev1.1)\n"
             "  -s --speed    max speed (Hz)\n"
             "  -d --delay    delay (usec)\n"
             "  -b --bpw      bits per word \n"
             "  -l --loop     loopback\n"
             "  -H --cpha     clock phase\n"
             "  -O --cpol     clock polarity\n"
             "  -L --lsb      least significant bit first\n"
             "  -C --cs-high  chip select active high\n"
             "  -3 --3wire    SI/SO signals shared\n");
        exit(1);
}

static void parse_opts(int argc, char *argv[])
{
        while (1) {
                static const struct option lopts[] = {
                        { "device",  1, 0, 'D' },
                        { "speed",   1, 0, 's' },
                        { "delay",   1, 0, 'd' },
                        { "bpw",     1, 0, 'b' },
                        { "loop",    0, 0, 'l' },
                        { "cpha",    0, 0, 'H' },
                        { "cpol",    0, 0, 'O' },
                        { "lsb",     0, 0, 'L' },
                        { "cs-high", 0, 0, 'C' },
                        { "3wire",   0, 0, '3' },
                        { "no-cs",   0, 0, 'N' },
                        { "ready",   0, 0, 'R' },
                        { NULL, 0, 0, 0 },
                };
                int c;

                c = getopt_long(argc, argv, "D:s:d:b:lHOLC3NR", lopts, NULL);

                if (c == -1)
                        break;

                switch (c) {
                case 'D':
                        device = optarg;
                        break;
                case 's':
                        speed = atoi(optarg);
                        break;
                case 'd':
                        delay = atoi(optarg);
                        break;
                case 'b':
                        bits = atoi(optarg);
                        break;
                case 'l':
                        mode |= SPI_LOOP;
                        break;
                case 'H':
                        mode |= SPI_CPHA;
                        break;
                case 'O':
                        mode |= SPI_CPOL;
                        break;
                case 'L':
                        mode |= SPI_LSB_FIRST;
                        break;
                case 'C':
                        mode |= SPI_CS_HIGH;
                        break;
                case '3':
                        mode |= SPI_3WIRE;
                        break;
                case 'N':
                        mode |= SPI_NO_CS;
                        break;
                case 'R':
                        mode |= SPI_READY;
                        break;
                default:
                        print_usage(argv[0]);
                        break;
                }
        }
}

int main(int argc, char *argv[])
{
        int ret = 0;
        int fd;

        parse_opts(argc, argv);

        fd = open(device, O_RDWR);
        if (fd < 0)
                pabort("can't open device");

        /*
         * spi mode
         */
        ret = ioctl(fd, SPI_IOC_WR_MODE, &mode);
        if (ret == -1)
                pabort("can't set spi mode");

        ret = ioctl(fd, SPI_IOC_RD_MODE, &mode);
        if (ret == -1)
                pabort("can't get spi mode");

        /*
         * bits per word
         */
        ret = ioctl(fd, SPI_IOC_WR_BITS_PER_WORD, &bits);
        if (ret == -1)
                pabort("can't set bits per word");

        ret = ioctl(fd, SPI_IOC_RD_BITS_PER_WORD, &bits);
        if (ret == -1)
                pabort("can't get bits per word");

        /*
         * max speed hz
         */
        ret = ioctl(fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed);
        if (ret == -1)
                pabort("can't set max speed hz");

        ret = ioctl(fd, SPI_IOC_RD_MAX_SPEED_HZ, &speed);
        if (ret == -1)
                pabort("can't get max speed hz");

        printf("spi mode: %d\n", mode);
        printf("bits per word: %d\n", bits);
        printf("max speed: %d Hz (%d KHz)\n", speed, speed/1000);

        transfer(fd);

        close(fd);

        return ret;
}
```



