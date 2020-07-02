# 6.**API文档生成及使用**

> 在工程中有一个专门存放文档的目录，docs，开发人员可以讲自己写的文档放到docs/doc目录下。也可以通过make doc命令生成相关API帮助文档。

## 6.1 **DOC目录**

> Doc目录下存放的是手动写好的开发类文档，如开发指南，说明文档等。

## 6.2 **html目录**

> 工程中格式为html的API帮助文档为动态生成，生成命令为：

```
$make doc
```

> 运行完该命令会在docs目录下生成html目录供上层开发人员使用。在使用时直接将docs/html目录拷贝给上层开发人员即可。阅读方式为用浏览器打开index.html。

### 6.2.1**添加自己的html帮助文档**

> 编辑需要添加帮助文档的模块Build.mk，在宏LOCAL\_DOC\_FILES后面添加自己要写成文档的文件。下面以development/source/gpio/Build.mk为例进行说明。该示例中是将gpio\_device.h的内容写到了html帮助文档中。

* 如下示例：development/source/gpio/Build.mk

```
LOCAL_PATH := $(my-dir)
include $(CLEAR_VARS)
LOCAL_MODULE := libgpio
LOCAL_MODULE_TAGS := eng
LOCAL_SRC_FILES := gpio_device.cpp
LOCAL_DOC_FILES := gpio_device.h
LOCAL_EXPORT_C_INCLUDE_FILES := gpio_device.h
LOCAL_LDLIBS := -lc -lstdc++
LOCAL_CPPFLAGS := -fPIC
include $(BUILD_SHARED_LIBRARY)
```

**注：该方法是基于docxgen开发的功能，docxgen已经在环境安装命令autoenvsetup中默认安装。要写到html中的文件是有固定格式的，详细的语法以及用法，可以上网参考docxgen用法。**

## 6.3 **latex目录**

> 该目录同样是make doc命令生成的目录，其内容与html一致，只是格式不同。

## 6.4 **ModuleDescription目录**

> 该目录是在编译（make）时自动生成的，存放用于描述工程中各个模块的功能、添加方式以及使用方式等的文档，目录下另有html和latex两个目录，分别存放内容相同但格式不同的两种描述文档。
>
> 并且在生成该目录的同时在工程的根目录下会生成一个名为ModuleDescription.html 的链接，可以直接用浏览器打开该链接浏览此文档。



