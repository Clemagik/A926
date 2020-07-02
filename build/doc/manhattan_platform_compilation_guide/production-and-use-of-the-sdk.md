# 5.**SDK的制作与使用**

## 5.1**SDK的制作**

### 5.1.1**Linux SDK的制作**

```
$ source build/envsetup.sh（初始化编译环境）
$ lunch <lunch_name> (选择要编译的板级及模式)
$ make sdk
```

> 生成的SDK在out/host/sdk/manhattan-sdk\_$\(USER\)\_linux目录下

### 5.1.2**Windows SDK的制作**

```
$ source build/envsetup.sh（初始化编译环境）
$ lunch <lunch_name> (选择要编译的板级及模式)
$ make win_sdk
```

> 生成的SDK在out/host/sdk/manhattan-sdk\_$\(USER\)\_windows目录下。
>
> SDK中安装的库和Sample是根据源码中选择的模块进行选择性安装。

## 5.2**SDK的使用**

### 5.2.1**SDK目录结构**

```
├──build                    //工程编译系统所在目录
├──docs                     //工程中提供的API文档
├──Makefile                
├──platform                 //sysroot目录
├──prebuilts                //工具链所在目录
├──Sample                   //实例目录
├──sdk-build                //编译脚本
└──source                   //工程开放源码目录
```

### 5.2.2**SDK中编译package**

```
$sdk-build <package_path>    //<package_path>是要编译的package在sdk中的相对路径
```

* 示例：

```
$sdk-build Sample/speech_tinyalsa
```

> 编译后生成的程序在obj/&lt;package\_path&gt;目录下。



