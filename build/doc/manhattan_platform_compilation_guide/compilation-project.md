# 3.**编译工程**

## 3.1 **编译整个工程**

> 执行如下命令开始编译整个工程

```
$sourcebuild/envsetup.sh(编译环境初始化后，此命令可以跳过)
$ lunch
$ make
```

> 编译完成后，生成out目录,结构如下：

```
├──host//host端生成文件的存放路径
│├──include//host 端编译过程所需头文件路径
│├──static//host 端生成的静态库文件路径
││└──lib
│└──tools//host 端编译过程中所需工具的存放路径
└──product
└──phoenix// 产品
├──image// 生成的目标文件（uboot、kernel、文件系统）
├──include//target 编译过程中所需头文件的存放路径
├──obj//模块编译过程中生成的中间文件及目标文件（strip前）
├──static//target 端生成的静态库文件（strip前）
└──system//文件系统（包含strip后的模块）
```

**注：本编译系统会将生成的可执行文件及库统一strip到system路径，暂不支持控制单个模块的strip动作。**

> 目标文件

```
$ ls out/product/开发板名/image/
    u-boot-with-spl.bin
    uImage    //kernel
    System.jffs2 //file system
```

> 使用烧录工具“cloner”将上诉三个文件烧入开发板中。
>
> Manhattan工程的编译系统暂不支持make  -j多线程编译的操作，但是你可以修改device/板级/device.mk文件中的MAKE\_JLEVEL的值，提高每个模块的多线程编译，以期加快工程的整体编译速度。

## 3.2 **Make clean工程**

> 执行如下命令：

```
$ make clean
```

> 即可clean整个工程。

**注：本编译系统暂不支持make distclean操作，clean操作执行的是各个模块在相应Build.mk中定义的clean动作。**

## 3.3 **编译工程单个模块**

> 本编译系统支持对单个模块的编译及对单个模块的clean操作。工程的编译系统初始化后，存在两套单模块的编译方法，一种是通用的模块编译方法mma命令，另外一种是区分target与host分目标编译方法。

### 3.3.1 **mma命令**

> 此命令为通用单模块编译方法，不区分此模块为host端还是target端，此命令的使用方法为：
>
> 1. 进入到所要编译模块目录下，此模块下含有Build.mk文件。
> 2. 执行mma命令，此模块以及此模块所依赖的模块都会被编译。

**注：执行该操作之前需要在工程主目录下执行source buiild/envsetup.sh\(编译环境初始化后，此命令可以跳过\)**

### 3.3.2 **Target端模块编译**

> Target单个模块编译规则如下，在工程的顶层目录下执行：

```
$ make $(LOCAL_MODULE)
```

> 即make “模块名”就可单独编译单个模块，以packages/example/App/grab 下的grab测试模块为例，此模块LOCAL\_MODULE（Build.mk中）赋值为：grab，执行如下命令即可单独编译grab模块：

```
$ make grab
```

### 3.3.3 **Host端模块编译**

> Host端单模块编译规则如下，在工程的顶层目录下执行：

```
$ make HOST-$(LOCAL_MODULE)
```

> 以external/e2fsprogs/ 下e2fsprogs模块为例，此模块的LOCAL\_MODULE \(Build.mk\)赋值为：e2fsprogs所以此模块的单独编译命令如下：

```
$ make HOST-e2fsprogs
```

### 3.3.4 **Target端模块clean**

> Target端模块clean规则如下，在工程的顶层目录下执行：

```
$ make$(LOCAL_MODULE)-clean
```

> 还以上述的grab模块为例，grab模块的单独clean命令如下：

```
$ make grab-clean
```

### 3.3.5 **Host端模块clean**

> Host端单模块clean规则如下，在工程的顶层目录下执行：

```
$ make HOST-$(LOCAL_MODULE)-clean
```

> 还以上述的e2fsprogs模块为例，其clean命令如下：

```
$ make HOST-e2fsprogs-clean
```

## 3.4 **单独编译文件系统镜像**

> 单独编译文件系统之所以在此单独阐述，是因为工程的编译系统设计上促使文件系统的编译不在上述介绍的两种方法之中，编译文件系统的方法为，工程目录下执行：

```
$ make systemimage
```

> 就会在out目录下重新生成system的镜像（前提为之前的system镜像已被清除）。



