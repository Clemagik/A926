## 4.4 **手动创建Build.mk**

### 4.4.1 **添加第三方模块**

> 在添加第三方模块时，首先要确保模块能够正常编译通过，再添加到platform上，在添加放置第三方功能模块时，建议放置到external目录下，并参照external目录下各个的Build.mk文件编写所添模块的Build.mk文件，对所添模块进行指导性编译，如下几个宏对第三方的编译提供支持。
>
> * 核心编译宏：使用include引入如下两个宏中的一个（实为.mk文件）完成模块的编译工作。

```
BUILD_HOST_THIRDPART                #负责HOST端第三方模块的编译，参见示例一
BUILD_THIRDPART                     #负责DEVICES端第三方模块的编译，参见示例二
```

> * 接口宏：

```
LOCAL_MODULE                        #模块名
LOCAL_MODULE_TAGS                   #模块所属开发模式
LOCAL_MODULE_GEN_BINRARY_FILES      #模块产生的可执行程序（需含路径）
LOCAL_MODULE_GEN_STATIC_FILES       #模块产生的静态库文件 （含路径）
LOCAL_MODULE_GEN_SHARED_FILES       #模块产生的动态库文件 （含路径）
LOCAL_MODULE_PATH                   #指定目标文件的copy路径，不定义此宏目标文件将copy到默认路径下
LOCAL_MODULE_CONFIG_FILES           #模块配置后生成的文件
LOCAL_MODULE_CONFIG                 #模块的配置方法
LOCAL_MODULE_COMPILE                #模块的编译方法
LOCAL_MODULE_COMPILE_CLEAN          #模块的clean方法
LOCAL_EXPORT_C_INCLUDE_FILES        #模块对外导出的文件，所导出文件被其他模块使用
LOCAL_EXPORT_C_INCLUDE_DIRS         #模块对外导出的目录，所导出目录下的文件被其他模块使用
LOCAL_DEPANNER_MODULES   #本模块的编译依赖的其他模块（生成的库，头文件等），此处为所依赖模块的module名，即LOCAL_MODULE的值。
```

**注：target端静态库的路径默认存放在system的usr/lib下，可执行程序默认存放在system的usr/bin下。**

**Host端默认路径在out/host下**

**其他的参见工程下build/core/envsetup.mk文件或者上述的out目录介绍。**

* 示例一：

  本示例为工程external/e2fsprogs/Build.mk

```
LOCAL_PATH := $(my-dir)
include $(CLEAR_VARS)
#vversion e2fsprogs-1.42.9
LOCAL_MODULE=e2fsprogs
LOCAL_MODULE_TAGS:=optional

LOCAL_MODULE_GEN_BINRARY_FILES=e2fsck/e2fsck \ 
                                misc/tune2fs \
LOCAL_MODULE_CONFIG_FILES:=Makefile
LOCAL_MODULE_CONFIG=./configure
LOCAL_MODULE_COMPILE=make -j$(MAKE_JLEVEL)
LOCAL_MODULE_COMPILE_CLEAN=make distclean                
include $(BUILD_HOST_THIRDPART)
```

### 4.4.2 **添加自己的模块**

> Manhattan平台支持如下几种类型文件的编译：
>
> * 可执行bin文件（host，device）
>
> * 动态库（host，device）
>
> * 静态库（host，device）
>
> 各类型模块分别由如下几个宏负责编译：
>
> * 核心编译宏：使用include引入如下宏中的一个（实为.mk文件）完成模块的编译工作
>
> * 生成可执行bin文件

```
BUILD_EXECUTABLE（device）
BUILD_HOST_EXECUTABLE（host）
```

> * 生成动态库

```
BUILD_SHARED_LIBRARY（device）
BUILD_HOST_SHARED_LIBRARY（host）
```

> * 生成静态库

```
BUILD_STATIC_LIBRARY（device）
BUILD_HOST_STATIC_LIBRARY（host）
```

**注：可执行文件的编译请参考packages/example/Sample/下的模块，库的编译请参考development/service/下的模块**

> * 接口宏：

```
LOCAL_MODULE             #模块名
LOCAL_MODULE_TAGS        #模块所属开发模式
LOCAL_SRC_FILES          #源文件
LOCAL_LDLIBS             #链接参数
LOCAL_CFLAGS             #C编译参数
LOCAL_CPPFLAGS           #CPP编译参数
LOCAL_DOC_FILES          #指定导出api的文件
LOCAL_C_INCLUDES         #编译所需头文件
LOCAL_MODULE_PATH        #指定目标文件的copy路径，不定义此宏目标文件copy到默认路径下
LOCAL_SHARED_LIBRARIES   #本模块编译所依赖的其他动态库，此动态库为工程中其他模块产生。
LOCAL_STATIC_LIBRARIES   #本模块编译所依赖的其他静态库，此静态库为工程中其他模块产生。
LOCAL_DEPANNER_MODULES   #本模块的编译依赖的其他模块（生成的库，头文件等），此处为所依赖模块的module名，即LOCAL_MODULE的值。
```

**注：target端静态库的路径默认存放在system的usr/lib下，可执行程序默认存放在system的usr/bin下。**

**Host端默认路径在out/host下**

**其他的参见工程下build/core/envsetup.mk文件或者上述的out目录介绍。**

* 示例二：

  本示例为工程packages/example/Sample/grab/Build.mk

```
LOCAL_PATH := $(my-dir)                                                               
#==================================================
#build grub
include $(CLEAR_VARS)
LOCAL_MODULE=grab
LOCAL_MODULE_TAGS:=optional
LOCAL_SRC_FILES:= camera.c  \
                grab.c  \
                savejpeg.c  \
                v4l2uvc.c
LOCAL_CFLAGS := -Wall -O --static -DVERSION=\"0.1.4\"
LOCAL_LDLIBS := -lc
LOCAL_MODULE_PATH:=$(TARGET_FS_BUILD)/$(TARGET_TESTSUIT_DIR)/$(LOCAL_MODULE)

LOCAL_STATIC_LIBRARIES := libjpeg.a
LOCAL_DEPANNER_MODULES:= libjpeg
#depend on the file (basename)
include $(BUILD_EXECUTABLE)
```

* 示例三：

  本示例为工程development/source/jpg\\_api/Build.mk

```
LOCAL_PATH := $(my-dir)                                          

include $(CLEAR_VARS)
LOCAL_MODULE=libjpeg-hw
LOCAL_MODULE_TAGS:=optional

LOCAL_SRC_FILES:= genhead.c  \
                genyuv.c \
                hard_en.c  \
                jpeg_enc.c  \
                jpeg_encode.c  \
                jpge_private.c  \
                jz_mem.c  \
                jzm_jpeg_enc.c  \
                soft_en.c  \
                vpu_common.c
LOCAL_EXPORT_C_INCLUDE_FILES:= include/head.h  \
                                include/ht.h  \
                                include/jpeg.h  \
                                include/jpeg_private.h  \
                                include/jz_mem.h  \
                                include/jzm_jpeg_enc.h \
                                include/qt.h \
                                include/vpu_common.h #export for others to use
LOCAL_C_INCLUDES := include
LOCAL_CFLAGS := -Wa,-mips32r2 -O2 -G 0 -Wall -fPIC -shared
include $(BUILD_SHARED_LIBRARY)
```

### 4.4.3**添加复制模块——prebuild**

> 在开发过程中经常需要将一些库和头文件不加任何修改的从一个路径下copy到另外一个目标路径下，为此编译系统添加了prebuild机制。
>
> * 核心编译宏：使用include引入如下宏中的一个（实为.mk文件）完成模块的prebuild工作

```
BUILD_PREBUILT            //一次copy一个文件
BUILD_MULTI_PREBUILT      //一次copy多个文件
```

> * 接口宏

```
LOCAL_MODULE                    #模块名
LOCAL_MODULE_TAGS               #模块所属开发模式
LOCAL_MODULE_CLASS              #模块所属哪种prebuild模式
LOCAL_MODULE_PATH               #文件（夹）copy的目标路径
LOCAL_COPY_FILES                #源文件
LOCAL_MODULE_DIR                #源文件夹
```

> Prebuild的使用，可参考如下示例：

* 示例一：

  本示例为工程external/minigui/resource/Build.mk

```
LOCAL_PATH := $(my-dir)                            
include $(CLEAR_VARS)

LOCAL_MODULE := minigui_resource
LOCAL_MODULE_TAGS :=optional
LOCAL_MODULE_CLASS:=DIR
LOCAL_MODULE_PATH := $(OUT_DEVICE_LOCAL_DIR)
LOCAL_MODULE_DIR := share

include $(BUILD_MULTI_PREBUILT)
####################copy Minigui.cfg
include $(CLEAR_VARS)
LOCAL_MODULE := MiniGUI.cfg
LOCAL_MODULE_TAGS :=optional
LOCAL_MODULE_PATH :=$(TARGET_FS_BUILD)/etc
LOCAL_COPY_FILES := MiniGUI.cfg
include $(BUILD_PREBUILT)
BUILD_PREBUILT 一次copy文件的规则：
LOCAL_COPY_FILES :=out1:src1
BUILD_MULTI_PREBUILT 对文件支持一种一次copy多个文件的规则：
LOCAL_COPY_FILES :=out1:src1 out2:src2 out3:src3
```

* 示例二

  external/alsa-lib/Build.mk

```
#===================================================
 # copy the a alsa.conf
 # #
 include $(CLEAR_VARS)
 LOCAL_MODULE := alsa.conf
 LOCAL_MODULE_TAGS := optional
 LOCAL_MODULE_PATH := $(TARGET_FS_BUILD)/usr/share
 LOCAL_COPY_FILES := alsa.conf:./src/conf/alsa.conf
 include $(BUILD_PREBUILT)
```

* 示例三

  packages/example/Sample/speech/Build.mk

```
#==================================================
# copy the lib
include $(CLEAR_VARS)
LOCAL_MODULE := aiengine-lib
LOCAL_MODULE_TAGS :=optional
LOCAL_MODULE_PATH :=$(OUT_DEVICE_SHARED_DIR)
LOCAL_PREBUILT_LIBS := libaiengine.so:./lib/libaiengine.so libecho_wakeup.so\
                :./lib/libecho_wakeup.so libula.so:./lib/x1000-4mic/libula.so
include $(BUILD_MULTI_PREBUILT)
```

## 4.5**通过命令自动生成Build.mk**

> 创建好模块目录以后，在该目录下运行autotouchmk，选择所要添加的模块是host端还是device端的，然后选择所要添加的模块类型（包括可执行模块、第三方模块、复制模块、静态库模块、动态库模块）。
>
> 运行autotouchmk命令以后，会有以下提示：

```
/*创建host端输入A；device端输入B，不区分大小写，输入其他字符创建失败*/
Host module or device module(A:host   B:device):
```

**Host端模块：**

> 然后会弹出如下输入提示：

```
Please select host target type you want to build (A:execute   B:thirdpart   C:static library   \
                                        D:shared library)：（可输入字符为A、B、C、D，不区分大小写，输入其他字符创建失败）
Host端可执行模块：直接输入A；
Host端第三方模块：直接输入B；
Host端静态库模块：直接输入C；
Host端动态库模块：直接输入D；
```

**Device端模块：**

> 然后会弹出如下输入提示：

```
Please select device target type you want to build (A:execute   B:thirdpart   C:prebuilt   \
                D:static library   E:shared library):（可输入字符为A、B、C、D、E、F，不区分大小写，输入其他字符创建失败）

Device端可执行模块：直接输入A；
Device端第三方模块：直接输入B；
Device端复制模块：   直接输入C；
Device端静态库模块：直接输入D；
Device端动态库模块：直接输入E；
Device端多项复制模块：直接输入F；
```

> 然后会在当前目录下生成一个Build.mk文件，自动生成的Build.mk会将基本会用的宏都列出来，根据需要对其中的宏进行赋值，不需要的可以不进行修改。自动生成的Build.mk文件对于大多数宏都是没有赋值的，需要用户根据自己的需要进行赋值。

```
LOCAL_PATH := $(my-dir)
include $(CLEAR_VARS)

LOCAL_MODULE :=                              #编译生成的模块名称，需要自己填写          
LOCAL_MODULE_TAGS := optional                #开发模式默认为optional，可以修改        
LOCAL_SRC_FILES :=                           #要编译的源文件，需要自己添加哪些要编译
LOCAL_MODULE_PATH := $(TARGET_FS_BUILD)/     #目标文件生成目录                
LOCAL_CFLAGS :=#编译C文件所需编译参数                  
LOCAL_LDFLAGS :=                             #链接参数                  
LOCAL_LDLIBS :=                              #链接库            

LOCAL_C_INCLUDES :=                          #C文件所需头文件                    
LOCAL_CPP_INCLUDES:=                         #CPP文件所需头文件                 

LOCAL_DEPANNER_MODULES :=                    #该模块编译时依赖的模块名称                  
LOCAL_COPY_FILES :=                          #prebuild下需要拷贝的源文件
LOCAL_MODULE_DIR :=                          #prebuild下需要拷贝的源目录
LOCAL_MODULE_GEN_SHARED_FILES=               #模块产生的动态库文件 （含路径）
LOCAL_MODULE_GEN_STATIC_FILES =              #模块产生的静态库文件 （含路径）               
LOCAL_MODULE_GEN_BINRARY_FILES =             #模块产生的二进制文件 （含路径）               
LOCAL_MODULE_CONFIG_FILES :=                 #第三方模块的configure文件                 
LOCAL_MODULE_CONFIG :=                       #第三方模块的configure命令                       
LOCAL_MODULE_COMPILE =                       #第三方模块的编译命令                      
LOCAL_MODULE_COMPILE_CLEAN =                 #第三方模块的清除命令                 

LOCAL_EXPORT_C_INCLUDE_FILES :=              #模块对外导出的文件，所导出文件被其他模块使用             
LOCAL_EXPORT_C_INCLUDE_DIRS :=               #模块对外导出的目录，所导出文件被其他模块使用             
include $(BUILD_PREBUILT)                    #根据选择自动生成的
```

**注：该命令仅用于用于生成几种常用模式的模板，具体内容需要用户进行编辑修改。宏的各种含义可参考该文章的该章节前面的内容。**

---

> 生成的Build.mk可参考如下示例：

* 示例一：

  本示例为为创建device端的复制（prebuild）类型

```
This command is used to generate the commonly used template, specific content need to edit
Host module or device module(A:host   B:device):
b
Please select device target type you want to build(A:execute   B:thirdpart   C:prebuilt   D:static library\
                                                                  E:shared library    F:multi prebuilt):
c
```

```
LOCAL_PATH := $(my-dir)
include $(CLEAR_VARS)
#name of the module:
LOCAL_MODULE =
LOCAL_MODULE_TAGS := optional                   #Development mode of module ,userdebug,eng,optional
#which directory the target file copy 
LOCAL_MODULE_PATH := $(TARGET_FS_BUILD)/
LOCAL_DEPANNER_MODULES :=                       #depend on module name
LOCAL_COPY_FILES :=                             #src copy files

include $(BUILD_PREBUILT)
```



