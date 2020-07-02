## 4.3 **制作自己的文件系统patch**

### 4.3.1 **Patch制作机制**

> 该工程下的文件系统生成原理如下图所示：

![](file:////tmp/wps-user/ksohtml/wpsILWot3.png)

**1.model1模型文件系统的制作机制：**

`Common system + device system patch ------>>  device system`

> 还以phoenix 板级为例，及工程在编译的过程中利用device/common/system下的common system及device/phoenix/norflash/model1/system.patch/下的phoenix model1产品模型的文件系统patch相结合，最终生成phoenix model1产品模型自己的文件系统。

**2.model2模型文件系统的制作机制：**

> 还以phoenix 板级为例，及工程在编译的过程中利用device/common/system下的common system及device/phoenix/norflash/model2/system.patch/和device/phoenix/norflash/model2/system.patch/下的phoenix model2产品模型的文件系统patch相结合，最终生成phoenix model2产品模型自己的文件系统。

### 4.3.2 **制作normal文件系统patch**

> 1. 首先你必须已经利用buildroot或者busybox等一些方法制作出了自己的文件系统。我们暂时将这个文件系统的目录定义为device\_system.
> 2. 工程目录下执行sourcebuild/envsetup.sh; lunch选择自己所需的板级及对应的存储选项
> 3. **将device\_system 拷贝到工程的目录下（推荐）。**

---

> 执行$ patch\_system命令，会出现如下输入提示：

```
device_system(the system must in current directory --> relative path):
```

> 此处输入的为device\_system的路径（基于当前路径下的相对路径），此项输入后，回车确定，出现另外一条如下输入提示：

```
which model (the value of 'MODEL' in device.mk) ?:
```

> 此项输入为所选择的产品模型，如：model1或者model2，此项输入回车确认，即可在对应的“板级”/“存储”/“模型”/下生成system.patch目录及system\_patch.sh文件。如：device/phoenix/norflash/model1/下的system.patch目录及system\_patch.sh文件。

### 4.3.3 **制作扩展功能文件系统patch**

> 1. 首先你必须已经利用buildroot或者busybox等一些方法制作出了自己的两个文件系统，normal文件系统以及带扩展功能的文件。扩展功能以ota为例。我们暂时将扩展功能（ota）文件系统的目录定义为device\_ota\_system。
> 2. 工程目录下执行sourcebuild/envsetup.sh；lunch选择自己所需的板级及对应的存储选项。
> 3. **将device\_ota\_system 拷贝到工程的目录下（推荐）。**

```
device_ext_system(the system which support ext function must in current directory --> relative path):
```

> 此处输入的为device\_ota\_system的路径（基于当前路径下的相对路径），此项输入后，回车确定，出现另外一条如下输入提示

```
which model (the value of 'MODEL' in device.mk) ?:
```

> 此项输入为所选择的产品模型，如：model1或者model2，此项输入回车确认，即可在对应的“板级”/“存储”/“模型”/下生成system.patch目录及system\_patch.sh文件。如：device/phoenix/norflash/model1/下的system.patch目录及system\_patch.sh文件。



