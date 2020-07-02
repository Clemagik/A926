## 4.2 **添加自己的lunch操作**

### 4.2.1 **Lunch选项详解**

> 本编译系统提供如下几个规则实现每个模块的选择性编译：
>
> **A.lunch 选择编译的模式**
>
> 1. eng 开发者模式编译
> 2. user 用户模式编译
> 3. userdebug 调试模式（此模式下会编译所有属于eng、user及userdebug模式的模块，且被编译模块可使用gdb进行调试，一些调试操作可在此模式下实现）
>
> **B.板级配置文件device/板级目录/“板级\_base.mk”文件**
>
> 如：device/phoenix/phoenix\_base.mk，定义该产品必须编译的模块。
>
> **C.模块的build.mk中LOCAL\_MODULE\_TAGS指定当前模块所属的开发编译模式**。
>
> 1. eng        指定当前模块所属eng开发模式，即lunch选择eng编译时，编译当前模块
> 2. user       指定当前模块所属user开发模式，即lunch选择user编译时，编译当前模块
> 3. optional 指定当前模块编译可选，若模块指定为该模式，只有模块在上述B项    描述的“板级\_base.mk”中被包含了，此模块才会被编译。

---

> 各个开发模式的关系可概括为如下逻辑：
>
> 1. 当lunch选择的编译模式为eng时，被编译的模块集合为：                                                                                                                           所有LOCAL\_MODULE\_TAGS为eng的模块+所有LOCAL\_MODULE\_TAGS为user的模块+ 板级\_base.mk”包含的所有模块
> 2. 当lunch选择的编译模式为user时，被编译的模块集合为：                                                                                                                                                 所有LOCAL\_MODULE\_TAGS为user的模块+ 板级\_base.mk”包含的所有模块

**注：文中所提到的创建板级特指创建linux工程的板级，不包含uboot以及kernel中的板级创建步骤。**

### 4.2.2 **Lunch选项的命名要求**

> 目前的定义格式如下：

```
$ 板级_存储介质_扩展功能-开发模式
如：phoenix_norflash_ota-eng
```

&gt;

> 如若没有特殊扩展功能则扩展功能项可以省略，如：phoenix\_norflash-eng

| eng | 开发者模式编译adb默认是可以使用的 |
| :--- | :--- |
| user | 用户模式编译默认是没有带adb的 |
| userdebug | 调试模式（此模式下会编译所有属于eng、user、及userdebug模式的模块，且被编译模块可使用gdb进行调试，一些调试操作可在此模式下实现）adb默认是可以使用的 |

### 4.2.3 **命名方式与板级目录的关系**

> Lunch选项的命名跟板级的目录结构存在一定的关系，还以phoenix板级为例，device下phoenix的目录结构如下：
>
> Phoenix//板级名称（与lunch选项保持一致）

```
├──device.mk//板级编译的配置文件
├──norflash// 存储介质（与lunch选项保持一致）
│├──model1//产品形态
│├──model2
│└──Readme
├──phoenix_base.mk//板级模块选择性编译的配置文件
└──spinand// 存储介质（与lunch选项保持一致）
├──model1//产品形态
└──vendorsetup.sh //lunch选项的定义文件
```

> Lunch选项解析后所对应的各个配置的定义在板级的device.mk里，如：

```
device/phoenix/device.mk.
```

### 4.2.4 **添加自己板级lunch选项方法**

> 编辑目录板级/存储介质/model/的vendorsetup.sh，在里面用add\_lunch\_combo关键字添加自己的板级相关的lunch选项名称，如add\_lunch\_combo phoenix\_norflash-eng就是添加了phoenix\_norflash-eng lunch选项。还是以板级phoenix，存储为norflash为例。

* 示例一：

  本示例为工程device/phoenix/norflash/model1/vendorsetup.sh

```
#!/bin/sh

# This file is executed by build/envsetup.sh, and can use anything
# defined in envsetup.sh.
#
# In particular, you can add lunch options with the add_lunch_combo
# function: add_lunch_combo generic-nand-eng

#note:
#   eng         --->    Developer mode
#   userdebug   --->    release mode

add_lunch_combo phoenix_norflash-eng
add_lunch_combo phoenix_norflash-user
add_lunch_combo phoenix_norflash-userdebug
```



