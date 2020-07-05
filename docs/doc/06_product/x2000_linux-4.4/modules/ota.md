# OTA

## 1.OTA升级简介

OTA: 是一整套的设备在线升级方案，支持升级kernel,recovery,system分区。

### 1.1.OTA源码目录

```c
  packages/updater/
  ├── default_ota_res
  ├── docker_server
  ├── libupdater
  ├── ota_package_maker
  ├── recovery
  └── rootfs-recovery
```

* libupdater目录实现了一种升级检测的本地策略，和一些升级相关的基础函数。
* recovery目录是升级程序。
* ota_package_maker目录用于制作升级包。
* default_ota_res目录存放升级需要的配置文件、秘钥、提示音文件等。
* docker_server目录用于搭建服务器。

## 1.2. 升级策略

* 本方案提供了一种本地升级策略，是否升级由设备本地决定。
* 在服务器端升级包路径下存放VERSION文件，记录升级包的版本，在设备/usr/data/VERSION文件中记录设备当前的系统版本。
* 检测升级时，设备通过/usr/data/ota_res/recovery.conf配置文件中的url得到current_version_full.conf。解析这个文件得到升级包路径，获取升级包版本，与当前系统版本做比较，如果升级包版本大于当前系统版本，则启动升级程序进行升级。升级成功后，更新本地/usr/data/VERSION文件，否则不进行升级。

## ２、服务器搭建

### 2.1.docker方式搭建服务器

为方便测试，本方案提供了一个通过nginx+python实现的https服务器，该服务器使用docker搭建，具体使用方法如下：

* 拷贝docker相关文件到服务器（以192.168.4.111为例）

  ```c
  scp -r packages/updater/docker_server user@192.168.4.111:/home/user/
  ```

* 在运行服务器的主机上安装docker，linux下执行以下命令

  ```c
  sudo apt-get install docker
  sudo apt install docker.io
  ```

* 构建镜像

  ```c
  cd /home/user/docker_server
  docker build -t test .
  ```

  其中，test是生成image的名字，执行成功后，使用docker images命令可以看到test镜像，如下图：

  ![ ](/assets/ota_11.png "build test ok.")

* 运行容器

  ```c
  export DOCK_SERVER_PATH=/home/user/docker_server
  docker run -d -p 8082:8082 -p 443:443 -p 80:80 -v $DOCK_SERVER_PATH/ota:/ota -v $DOCK_SERVER_PATH/nginx/conf.d:/etc/nginx/conf.d -v $DOCK_SERVER_PATH/nginx/nginx.conf:/etc/nginx/nginx.conf -v $DOCK_SERVER_PATH/nginx/ssl:/ssl -v $DOCK_SERVER_PATH/nginx/log:/var/log/nginx -t test
  ```

  其中DOCK_SERVER_PATH设置为docker_server目录存放的绝对路径，此时执行docker ps可以看到当前运行的容器，如下图：

  ![ ](/assets/ota_22.png "docker ps.")

* 进入运行的容器

  其中576ff9b465d0为CONTAINER ID，可通过上面docker ps命令得到。

  ```c
  docker exec -it 576ff9b465d0 /bin/bash
  ```

* 启动nginx https服务

  ```c
  service nginx start
  ```

* 此时需要输入openssl生成证书时设置的密码，如果您直接使用docker_server/ssl目录中的文件，密码为123456，您也可以自己生成证书，方法稍后讲解。
* 测试搭建的服务器

  执行成功后，其他设备就可以通过wget命令得到服务器中的文件了。此时，服务器上的docker_server/ota目录被设置为http服务器的根目录，在其他设备上通过下面命令测试服务器工作是否正常。
  
  ```c
  wget https://192.168.4.111/index.html --no-check-certificate
  ```

  其中，192.168.4.111是运行docker的主机IP地址，要替换为您测试的主机IP地址。

* 服务器环境搭建完成。

## 2.2.openssl生成证书

支持https，需要添加证书到服务器，生成证书步骤如下：

* 生成2048bit的RSA私钥文件server.key
  
  ```c
  openssl genrsa -des3 -out server.key 2048
  输入两次相同的密码
  ```

* 生成CSR证书签名请求文件server.csr

  ```c
  openssl req -new -key server.key -out server.csr
  输入之前的密码
  Country Name (2 letter code) [XX]:CN    [国籍]
  State or Province Name (full name) []:beijing [省份]
  Locality Name (eg, city) [Default City]:beijing    [城市]
  Organization Name (eg, company) [Default Company Ltd]:www.server.com [公司]
  Organizational Unit Name (eg, section) []:www.server.com [行业]
  Common Name (eg, your name or your server's hostname) []:www.server.com  [自己的域名]
  Email Address []:

  Please enter the following 'extra' attributes
  to be sent with your certificate request
  A challenge password []:    [这里不填]
  An optional company name []:    [这里不填]
  ```

* 写RSA秘钥

  ```c
  openssl rsa -in server.key -out server_nopwd.key
  输入之前的密码
  ```

* 获取私钥

  ```c
  openssl x509 -req -days 365 -in server.csr -signkey server_nopwd.key -out server.crt
  ```

将生成的server.crt server.csr server.key server_nopwd.key拷贝到docker_server/nginx/ssl目录下，保证docker_server/nginx/conf.d/default.conf中server_name和生成crs证书时添的域名相同即可。

## ３.OTA 生成新版本

### 3.1.升级包路径配置

* 修改packages/updater/ota_package_maker/example/config/current_version.conf文件

  ```python
  例: 升级包路径在docker_server/ota/version3目录下
    1. 将ip改为运行docker的主机IP地址
    2. url为升级包存放位置,将url设置为https://192.168.4.111/version3
    3. 将该文件拷贝到服务器端docker_server/ota目录下，改名为current_version_full.conf
  ```

  ![ ](/assets/ota-current_version.conf.png "current_version_full.conf")

* 修改packages/updater/default_ota_res/recovery.conf文件
  
  ```python
  例: current_version_full.conf存放在服务器的docker_server/ota目录下
    1. 将ip改为运行docker的主机IP地址
    2. 将url改为current_version_full.conf存放的位置，url设置为https://192.168.4.111
    3. 将该文件拷贝到服务器端docker_server/ota目录下，改名为current_version_full.conf
    4. 将rpt_url改为上报升级状态的服务器地址，如果没有，设置为空
  ```

  ![ ](/assets/ota-recovery.conf.png "recovery.conf")

### 3.2.修改其它配置

* wifi配置

  * 配置system的wifi配置
  
  ```c
  vi buildroot/package/wpa_supplicant/wpa_supplicant.conf
  ```
  
  * 配置ramdisk的wifi配置
  
  ```c
  vi packages/updater/rootfs-recovery/etc/init.d/S45network
  ```

* nand信息配置
  
  ```c
  vi packages/updater/ota_package_maker/example/config/partition_nand.conf
  ```

  根据nand大小和分区情况修改，应该与烧录工具cloner保持一致
  
  ![ ](/assets/ota-partition_nand_conf.png "partition_nand.conf")

* 升级信息配置
  
  ```c
  vi packages/updater/ota_package_maker/example/config/customization_nand.conf
  ```

  ![ ](/assets/ota-customization_nand_conf.png "customization_nand.conf")

### 3.3.制作升级包

* 制作升级包

```c
make ota_mkpackage
```

* 将编译生成的升级包和current_version.conf拷贝到服务器上

```python
cp packages/updater/ota_package_maker/example/config/current_version.conf user@192.168.4.111:/home/user/docker_server/ota/current_version_full.conf
scp -r out/product/halley5_v20/image/ota/* user@192.168.4.111:/home/user/docker_server/ota/version3/
```

* 在服务器升级包路径下创建VERSION文件

```c
cd /home/user/docker_server/version3
echo 13 > VERSION
```

## 4.快速测试

* 测试方法:
  1. cloner烧录uboot,nv,kernel,system
  2. 启动system后，执行wfi_up.sh
  3. 确保网络与服务器ping通后，执行update
  4. 执行recovery后，系统升级

* 测试现象:

  系统先升级recovery，然后启动ramdisk升级kernel和system，最后再次启动，本地VERSION被更新

## 5.OTA测试常见问题

### 5.1.升级包制作失败，出现如下错误信息

```c
Could not find the main class: com.android.signapk.SignApk. Program will exit.
    adding: update008/ (stored 0%)
    adding: update008/xImage_003 (deflated 0%)
Exception in thread "main" java.lang.UnsupportedClassVersionError: com/android/signapk/SignApk : Unsupported major.minor version 51.0
        at java.lang.ClassLoader.defineClass1(Native Method)
        at java.lang.ClassLoader.defineClassCond(ClassLoader.java:631)
        at java.lang.ClassLoader.defineClass(ClassLoader.java:615)
        at java.security.SecureClassLoader.defineClass(SecureClassLoader.java:141)
```

由于升级包制作程序中有提前编译好的jar包，编译jar包时的java版本和您当前制作升级包的服务器java版本不匹配导致。解决办法，执行如下命令：

```c
cd packages/kunpeng/updater/ota_package_maker/otapackage/depmod/signature/signapk
rm -f signapk.jar
./build.sh
cd -
rm out/product/j618/obj/ota/ -rf
rm out/product/j618/image/ota/ -rf
make ota_package
```

### 5.2.升级开始解析sha1Tab出错

每次制作升级包前需要删除之前升级包的sha1Tab

```c
rm out/product/halley5/image/ota/ -rf
make ota_mkpackage
```

### 5.3.无法启动recovery，Uncompressing Linux...后无打印

可能由于recovery镜像太大，内核自解压覆盖了自解压程序，需要裁剪ramdisk或recovery内核

```c
ramdisk源码:
  packages/updater/rootfs-recovery
recovery配置:
  kernel/arch/mips/configs/halley5_v20_linux_sfc_nand_recovery_defconfig
