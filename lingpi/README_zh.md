# 小凌派-RK2206开发板

小凌派-RK2206开发板主控器为瑞芯微高性能、高性价比的RK2206芯片，搭载OpenHarmony操作系统，内置WiFi/AP功能、NFC功能、液晶显示接口以及E53接口，E53接口兼容各类传感器模块，便于多样化的IoT物联网应用；目前小凌派-RK2006开发板已经拥有20+个成熟的应用案例，以及完善的教学课程，可广泛的应用于智慧城市、智能家居、智慧教学、智慧车载以及智慧医疗等多种场景。

## 介绍

小凌派-RK2206开发板是一款由福州市凌睿智捷电子有限公司专为OpenHarmony轻量级系统打造的OpenHarmony开发板。

小凌派-RK2206开发板主要功能为：

* 采用IoTCluB的E53接口标准，可接入所有的E53案例扩展板。
* 主控为瑞芯微RK2206芯片，200MHz主频，RAM 256KB、ROM 16KB、PSRAM 8M、FLASH 8M。
* 板载NFC Forum Type 2 Tag芯片及天线，可实现HarmonyOS“碰一碰”功能。
* 支持 IEEE 802.11b/g/n 制式，AP 模式。
* USB 2.0 、SD card /SPI/I2C 接口、麦克风接口、E53 接口（包含 UART、 I2C、ADC、DAC、PWM、5 个 GPIO）。
* 一根TypeC USB线，即可实现供电、下载等功能。
* 一根TypeC USB线，可支持串口调试功能。

![小凌派-RK2206开发板](docs/figures/lockzhiner-rk2206.jpg)

## 开发板规格

| 编号 | 技术参数类型 | 技术规格参数                                                                   |
| ---- | ------------ | ------------------------------------------------------------------------------ |
| 1    | 芯片类型     | 瑞芯微RK2206                                                                   |
| 2    | CPU          | Cortex-M4F，主频高达200MHz                                                     |
| 3    | DSP          | Tensilica HiFi                                                                 |
| 4    | WLAN         | Cortex-M0内核，支持IEEE 802.11b/g/n制式，支持AP模式                            |
| 5    | 操作系统     | OpenHarmony轻量级操作系统                                                      |
| 6    | 存储         | RAM 256KB192KB DTCMROM 16KBPSRAM 8MFlash 8M                                    |
| 7    | 显示         | 支持SPI LCD接口，I2C OLED接口                                                  |
| 8    | NFC          | 支持NFC Forum Type 2 Tag协议                                                   |
| 9    | E53接口      | 1组UART、1组I2C、1组SPI、ADC、5个GPIO（包括3通道PWM），应用于E53传感器模块案例 |
| 10   | USB接口      | 1个USB OTG 2.0，1个USB转串口用于调试                                           |
| 11   | 外部存储     | SD Card                                                                        |
| 12   | LED灯        | 1个NFC指示灯，1个用户指示灯、1个电源指示灯                                     |
| 13   | 按键         | 1个复位键、1个烧录按键，4个用户键                                              |
| 14   | 主板供电     | USB 5V供电，2个接口均支持                                                      |
| 15   | 开发板应用   | IoT物联网应用                                                                  |
| 16   | 尺寸         | 72.5mm * 60.7 mm                                                               |

## 关键特性

* *支持LiteOS-M；*
* 支持HDF相关驱动；
* 支持LittleFS；
* 支持WiFi。

## 搭建开发环境

### 1、准备工作

准备一台电脑，安装Windows系统

### 2、开始下载

#### 2.1、下载Ubuntu系统镜像

下载网址（百度云）：https://pan.baidu.com/s/1IfT0onLb1kcoByhOUU-kyA
提取码：eikl

```
Ubuntu系统镜像是Ubuntu 20.04.3 LTS 64位系统版本，已安装相对应的编译环境。其中，账号主要有2个：
（1）lzdz
    账号：lzdz
    密码：123456
（2）root
    账号：root
    密码：123456
```

#### 2.2、下载并安装VBox工具

下载网址（百度云）：https://pan.baidu.com/s/1EYgUAO1_2N0GluF7h8HvBQ
提取码：eekp

#### 2.3、下载并安装MobaXterm工具

下载网址（百度云）：https://pan.baidu.com/s/1FGz4TEJ3Np-ppP1-o0E05g
提取码：n0ll

#### 2.4、下载并安装烧写工具

下载网址（百度云）：https://pan.baidu.com/s/1SU99yvtPehkRzBKH2jzNtA
提取码：1adc

#### 2.5、下载并安装USB驱动

下载网址（百度云）：https://pan.baidu.com/s/1s_zEuDU_OpcltaJUZNvbLA
提取码：9slw

#### 2.6、下载并安装VSCode

下载网址（百度云）：https://pan.baidu.com/s/1gi5froBzuSjLjvV4syKQ0A
提取码：iv3k

### 3、部署环境

#### 3.1、安装VBox虚拟机

请安装 `VirtualBox-6.1.32-149290-Win.exe`可执行文件（请参考网上[详细教程](https://www.jianshu.com/p/bfb4f4415411 "VirtualBox安装")）。

#### 3.2、加载Ubuntu系统镜像

##### 3.2.1、加载Ubuntu镜像

将Ubuntu.rar文件解压到某一个盘下，打开 `Oracle VM VirutalBox`，点击 `注册`，出现如下图所示：

![](image/README/1646027282190.png)

出现打开对话框，选中Ubuntu镜像文件（即Ubuntu.vbox）。如下图所示：

![](image/README/1646027345989.png)

#### **3.2.2、配置Ubuntu镜像**

点击选中 `Ubuntu`，点击设置 `按钮`，如下图所示：

![](image/README/1646027609813.png)

（1）配置内存。

弹出设置对话框，点击 `系统`，再点击 `主板`，在 `内存大小`中选择合适的内存（建议为Windows最大内存的一半）。

![](image/README/1646027865495.png)

（2）配置CPU

在设置对话框，点击 `系统`，再点击 `主板`，在 `处理器`中选择合适的处理器数量（建议为Windows处理器的一半）。

![](image/README/1646028120971.png)

（3）配置网络

在设置对话框，点击 `网络`，再点击 `网卡1`，选中 `启用网络连接`，在 `连接方式`中选择 `桥接方式`，在 `界面名称`中选择可以连接外部网络的一个网卡。

注意：用户也可以在 `连接方式`中选择 `网络地址转换(NAT)`。

![](image/README/1646028326232.png)

（4）配置USB

在设置对话框，点击 `USB设备`，再取消 `启用USB控制器`。

![](image/README/1646028543416.png)

#### 3.2.3、启动Ubuntu镜像

上述设置完毕后，点击 `启动(T)`。

![img](image/README/1646028696928.png)

Ubuntu系统正式启动，如下所示：

![](image/README/1646028811521.png)

##### 3.2.4、获知Ubuntu系统的IP地址

点击左下角的图标，在搜索框中输入 `terminal`

![](image/README/1646029260335.png)

弹出终端界面，输入命令：ip addr show

![](image/README/1646029338664.png)

请记住，该IP地址为Ubuntu操作系统的IP地址。后续需要使用该IP地址

### 4、在windows上远程连接服务器

（1）打开 `MobaXterm`工具，并点击：`Seesion`，`SSH`按钮。

![1646018532933.png](image/README/1646018532933.png)

（2）输入连接信息，远程地址，并点击OK

![](image/README/1646029559988.png)

（3）弹出对话框，点击 `是(Y)`

![](image/README/1646029599181.png)

（3）输入账号和密码

Ubuntu操作系统分为2个账号：

* 账号：lzda；密码：123456（一般用账号lzda)
* 账号：root；密码：123456

![](image/README/1646029690327.png)

登录成功后，进入终端界面。

![](image/README/1646029806257.png)

MobaXterm的Session页面中会有连接成功的连接，后续我们直接点击该连接，即可重新连接。

![](image/README/1646029971027.png)

### 5、ubuntu文件映射到windows上

Ubuntu已搭建好samba服务器，windows上可以直接挂载Ubuntu的samba共享文件夹。

在文件管理器中输入\\\192.168.1.14（注意：该IP地址为Ubuntu操作系统的IP地址，具体请参考3.2.4），然后敲回车键，即可进入Ubuntu的samba共享文件夹。

![](image/README/1646030108437.png)

### 6、ubuntu获取源代码获取源

```shell
repo init -u https://gitee.com/openharmony/manifest.git -b master --no-repo-verify
repo sync -c
repo forall -c 'git lfs pull'
```

### 7、编译流程

编译步骤如下所示：

```shell
hb set -root .
hb set
lockzhiner
   lingpi

选择lingpi

hb build -f
```

编译出的目标文件在 `out/lingpi/lingpi/images`目录下。如下图所示：

![](image/README/1646018887782.png)

### 8、烧录打印

#### 8.1、安装USB驱动

打开主目录device/soc/rockchip/rk2206/tools/windows/DriverAssitant驱动目录，点击DriverInstall.exe，如下图所示：

![](image/README/1646019307817.png)

点击 `运行`，如下图所示：

![](image/README/1646019347452.png)

点击 `驱动安装`，如下图所示：

![](image/README/1646019384141.png)

安装完成后退出。

#### 8.2、烧写程序

##### 8.2.1、打开RKDevTool

打开烧写工具RKDevTool，点击RKDevTool.exe。如下图所示：

![](image/README/1646019737315.png)

点击运行，如下图所示：

![](image/README/1646019787261.png)

##### 8.2.2、指定烧写文件

（1）指定LoaderToDDR文件

点击红色箭头所指地方，如下图所示：

![](image/README/1646020804732.png)

弹出打开对话框，选择主目录下 `out/lingpi/lingpi/images`，选中 `rk2206_db_loader.bin`文件。如下图所示：

![](image/README/1646020466591.png)

（2）指定fireware文件

点击红色箭头所指地方，如下图所示：

![](image/README/1646020877760.png)

弹出打开对话框，选择主目录下 `out/lingpi/lingpi/images`，选中 `Firmware.img`文件。如下图所示：

![](image/README/1646020931916.png)

##### 8.2.3、开始烧写

使用USB线，连接小凌派-RK2206开发板的USB-OTG（即烧录口）。如下图所示：

![](image/README/1646019659588.png)

在小凌派-RK2206开发板上，长按MaskRom按键（详见板子的按钮），点击ReSet按键（详见板子的按钮），烧写工具出现：“发现一个MASKROM设备”。如下图所示：

![](image/README/1646019963421.png)

点击 `执行`，下载程序。如下图所示：

![](image/README/1646020068983.png)

下载完成，如下图所示：

![img](image/README/1646020023565.png)

#### 8.3、查看打印信息

#### 8.3.1、USB线连接设备调试串口

将USB线连接到小凌派-RK2206开发板的UART（即调试串口）。如下图所示：

![](image/README/1646030755794.png)

#### 8.3.2、Windows查看串口号

在Windows上点击 `我的电脑`，右击选中 `管理`，打开计算机管理对话框。如下所示：

![](image/README/1646030869997.png)

在计算机管理对话框中，点击 `设备管理器`，点击 `端口(COM和LPT)`，查看获知COM3是USB串口线。

![img](image/README/1646030975876.png)

#### 8.3.3、查看串口调试信息

打开MobaXterm软件，点击 `Session`，再点击 `Serial`。如下图所示：

![](image/README/1646031129497.png)

在 `Serial Port`中选择 `COM3`（即在8.3.2中查看的串口号），在 `Speed`中选择 `115200`，最后点击 `OK`。如下图所示：

![](image/README/1646031242219.png)

按下小凌派-RK2206开发板的RESET按键，可以查看到如下打印信息：

![img](image/README/1646031566099.png)
