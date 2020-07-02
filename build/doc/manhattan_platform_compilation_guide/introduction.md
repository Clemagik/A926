## 1.1 **简介**

> Manhattan平台是君正开发的一套linux系统的发布、开发平台，平台的编译系统支持c文件，cpp文件的编译，同时也支持动态库，静态库以及可执行bin文件的编译，Manhattan平台作为linux系统的发布平台，最大的特点就是能够很方便的集成、添加第三方应用、库。
>
> 平台的编译系统以指导编译为思想，各个需编译的模块通过Build.mk进行指导性编译。工程支持模块的单独编译、选择性编译。极大的方便定制自己的SDK。

## 1.2 **目录结构介绍**

```
├──build                  //工程的编译系统所在目录，实现文件系统的整体编译功能
├──development            //包含一些开发中需要的库（本地），服务，以及工具等
│├──libutils              //底层工具以及IPC接口
│├──libcutils             //IPC接口
│├──service               //提供了一些服务，包含休眠唤醒服务、watchdog等
│├──source                //封装了一些硬件接口，上层可以调用，用户也可以参考该│目录下的代码实现自己的接口。
│└──tools                 //包含一些开发中用到的工具，如gdb，adb等。
├──device                 //板级存放目录，包含文件系统公共的部分common以及板级自己│的patch
│├──common
│└──phoenix
├──docs                   //工程说明文档
│├──doc                   //开发人员编译的使用文档
│├──html                  //动态生成的API文档
├──external               //第三方模块，包括：库和工具
│├──alsa-lib
│├──alsa-utils
│├──e2fsprogs
│├──gtest
│├──libbacktrace
│├──libjpeg-turbo
│├──libunwind
│├──minigui
│├──tinyalsa
├──hardware              //hardware所在目录，集成了一些操作硬件的接口
│├──init
│├──SampleOnTime
│├──tests
├──kernel                //kernel代码
├──Makefile
├──out                   //编译生成目录
│├──host                 //编译生成的host端库以及可执行程序
│└──product              //编译生成的device端库以及可执行程序
├──packages              //Sample、App及测试用例
├──prebuilts             //烧录工具及编译工具链
│├──burnertools          //烧录工具
│├──eclipse_install
│├──toolchains           //编译工具链
│└──win-tool             //windows工具
└──u-boot                //u-boot代码
```

## 1.3 **特别注意**

> 该文档所有内容均以X1000平台下的phoenix开发板为例进行说明，开发人员可根据自己实际下载到的代码进行参考。



