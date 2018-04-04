<h1 align="center">
	<img width="300" src="https://raw.githubusercontent.com/EDI-Systems/M7M2_MuAmmonite/master/Documents/Demo/logo.png" alt="logo">
</h1>

# Unity Platform (RVM)

Click **[HERE](README.md)** for English version.

&emsp;&emsp;**RVM** 是一个专为微控制器（microcontroller，MCU）设计的虚拟机监视器（virtual machine monitor，VMM）。这个虚拟机监视器可以高效地准虚拟化很多RTOS和裸机程序。这是一种划时代的技术，并且也是IoT系统的未来。它使得之前很多无法想象的应用变成可能。它同时具备前无古人的可扩展性，可以在1MB的静态内存上运行多达64个虚拟机。

&emsp;&emsp;该虚拟机监视器的手册可以在 **[这里](https://github.com/EDI-Systems/M7M2_MuAmmonite/blob/master/Documents/M7M2_%E5%BE%AE%E6%8E%A7%E5%88%B6%E5%99%A8%E5%AE%9E%E6%97%B6%E5%87%86%E8%99%9A%E6%8B%9F%E6%9C%BA%E7%9B%91%E8%A7%86%E5%99%A8_%E6%8A%80%E6%9C%AF%E6%89%8B%E5%86%8C.pdf)** 找到。

&emsp;&emsp;如果想要参与开发，请阅读 **[参与](CONTRIBUTING.md)** 和 **[规范](CODE_OF_CONDUCT.md)** 两个指导文档。如果要提交拉取请求，请使用 **[拉取请求模板](PULL_REQUEST_TEMPLATE.md)** 。

本软件采用 **三种不同的授权** ：你可以选择 **[LGPL v3](LICENSE.md)** ，也可以选择 **[经修改的MIT协议](MODMIT.md)** 。 如果有特殊需求， 也可以联系我们请求**商业授权**。

&emsp;&emsp;对于那些由微控制器厂商提供的硬件抽象层软件包，请到 **[M0P0_Library](https://github.com/EDI-Systems/M0P0_Library)** 软件仓库自行下载。 本虚拟机监视器也依赖于 **[M7M1 微内核操作系统](https://github.com/EDI-Systems/M7M1_MuEukaron)** 。

## 虚拟化技术简介
&emsp;&emsp;[虚拟化](https://en.wikipedia.org/wiki/Virtualization)是一种可以模拟出多套逻辑硬件并允许多个操作系统或裸机程序共存于同一个物理硬件平台的技术。在PC架构上，[VMware](https://www.vmware.com/)和[Virtual Box](https://www.virtualbox.org/)已经有数十年历史，并且从那时起直至今日都深刻地塑造了我们的IT基础设施。在同一套物理硬件上运行多个虚拟机的主要原因包括进阶隔离、方便部署和易于管理。

&emsp;&emsp;有两种主要的虚拟化方式：一种是[全虚拟化](https://en.wikipedia.org/wiki/Full_virtualization)，一种是[准虚拟化](https://en.wikipedia.org/wiki/Paravirtualization)。前者可以运行完全无修改的二进制代码；后者则要求操作系统底层被适当修改以运行于虚拟机上。本虚拟机监视器（RVM）使用准虚拟化技术来减小运行时间消耗和内存消耗。它提供了一系列的超级调用（hypercall）来提供敏感的特权级操作。

## 为什么要在微控制器上使用虚拟化技术？
&emsp;&emsp;正像虚拟化技术塑造了我们今天的IT基础设施那样，虚拟化技术也会塑造[物联网](https://en.wikipedia.org/wiki/Internet_of_things)（IoT）系统的未来。在微控制器系统上，我们使用虚拟化技术的理由和在PC架构上是不太相同的：主要的原因是可靠性，安全性和灵活性。

### 可靠性
&emsp;&emsp;RTOS通常使用静态代码分析和认证来确保系统的可靠性。然而，这对很多应用场景来说是不够的。除了RTOS自身之外，以下各项也会在系统中导致错误风险：

- **应用程序编码错误。** 由于传统的RTOS不提供任何形式的内存隔离，因此一个任务内的错误会很容易传播到其他任务，然后毁掉整个软件系统。

- **辐射和[电磁干扰](https://en.wikipedia.org/wiki/Electromagnetic_interference)。** 这些外部干扰会翻转硬件状态位，从而在整个系统中造成偶发错误。

- **[拒绝服务](https://en.wikipedia.org/wiki/Denial-of-service_attack)。** 一个高优先级任务如果在某个循环中死机，有可能导致整个系统失去响应。如果它频繁地调用某个可能导致锁调度器的系统调用，那就更糟，因为这必然会摧毁整个系统的实时性。

- **人为破坏。** 由于系统没有内存保护，黑客可以侵入IoT设备并且把系统功能任意破坏。

&emsp;&emsp;尽管这些弱点可以通过使用[看门狗定时器](https://en.wikipedia.org/wiki/Watchdog)（watch-dog timer，WDT）重启系统加以缓解，这种方法只对低复杂度的系统奏效。对于那些更复杂的应用或者关键应用，我们不能允许它们完全重启，这时传统的RTOS对此束手无策。

### 安全性
&emsp;&emsp;传统的RTOS通常不把信息安全作为它们的一个设计目标。这样一来，它们根本无法满足未来的IoT系统对信息安全的需求。正如上节所讲，黑客可以黑进系统并且破坏系统的任意功能；通常而言，缓冲区溢出攻击就足够了。但这还不是最坏的状况。以下所列的情况比简单的破坏要糟糕多了：

- 对于以[IOTA](https://iota.org/)为代表的物联网[区块链](https://en.wikipedia.org/wiki/Blockchain)应用，攻击者能够读取你的密码，黑进你的账户，然后划走你所有的钱。

- 对于智能家居应用，黑客可以黑进你的摄像头和家用电器，然后把你的生活变成现实版真人秀。（不然你以为网上那么多有趣的小视频都是哪来的？）

- 对于智能汽车，黑客可以强迫你像一级方程式赛车手那样开车。这在Tesla model 3上刚发生过。肯定没有人想在赛场之外的地方赛车。

- 对于关键的生命支持系统，比如医用呼吸机，把它们关掉就足够了。目前还没有人因此丧生，但是如果不采取措施，很快就会有的。

- 对于关键的工业应用，黑客可以把整条生产线关掉，或者至少关掉它的一部分。

### 灵活性
&emsp;&emsp;嵌入式软件市场长久以来一直都是碎片化的。针对不同的市场有不同的RTOS和软件栈，而且它们之间互相不兼容。在过去这完全没问题，因为嵌入式系统一直都是针对一个特定用途进行设计的。然而，这个假设在今天不再是现实了。IoT设备面临着越来越多的功能整合需求。

&emsp;&emsp;虚拟化技术可以让不同的软件栈共存于同一个硬件平台，而不用新开发一个具备所有功能的RTOS。这会为我们打开新世界的大门，允许我们构建全新的应用形式。而且，这样做并不要求工程师们改变已有的RTOS开发的心智模式，因此基本上没有学习成本。

### 虚拟化不会有很高的开销吗？ 
&emsp;&emsp;简而言之： **不会** 。  
&emsp;&emsp;详细解释：如果虚拟机监视器 **被设计的很好，并且使用方法也正确** 的话（尤其指通信机制），虚拟化的开支可以忽略不计。对于虚拟化后的FreeRTOS而言，它的线程切换和中断响应时间和直接运行在物理硬件上相比只会恶化4倍，而此时的线程切换性能仍然高出RT-Linux整整25倍之多。对于那些CPU占用高而I/O占用低的虚拟机，这开销更是可以忽略不计。

&emsp;&emsp;有一些人也许会说一个针对微控制器设计的类Unix系统是完全足够好的，我们不需要虚拟化解决方案。然而，这无异于重新发明轮子，而且最终你会得到一个不比Linux好多少的玩意，或者另一个VxWorks（的确有一些小型实时系统在这条路上狂奔，我们不看好它们）。Linux在嵌入式领域并不被认为是可靠的，而且甚至也不安全*；它足够灵活，但是内存开销却难以忍受，因此这不仅没有解决资源消耗问题而反倒激化了它。然而，虚拟机却先天安全和模块化，而且其底层的RME系统还提供了基于权能的安全机制。  
*Linux在嵌入式系统市场被认为不安全的原因是很多嵌入式系统几乎不在线更新。这使得它们会长久地使用一个漏洞人尽皆知的旧版本内核。你现在去随便找个现有工具扫描下路由器和IP摄像头，还是能随意弄到大把肉鸡。哦，对了，老Android手机其实也差不多。

### 线程切换时间和中断响应时间膨胀了将近4倍。这能行吗？
&emsp;&emsp;在RVM支持的微控制器上， **没问题** 。因为这个虚拟机监视器的目标是支持高端MCU，而在高端MCU的硬件响应速度已经远超很多传感器和执行器的实际所需。一个运行在180MHz的Cortex-M4如果跑虚拟化的FreeRTOS，其中断响应时间和线程切换时间和一个跑在72MHz的Cortex-M3上的原生FreeRTOS差不多。

&emsp;&emsp;在I/O响应时间方面，Cortex-M3已经足够优秀了。我们选择使用Cortex-M4是为了 **增强的计算性能，不是为了响应时间** 。如果你的应用程序的确需要低的响应时间，你可以编写 **RME的原生应用，它们的响应时间和FreeRTOS差不多** 。如果你使用比Cortex-M4快的处理器（比如Cortex-M7和TMS320C66X），那么你完全可以忽略额外开销的问题。

### 为了移植到虚拟机监视器，我要修改多少代码？
&emsp;&emsp;非常少。请参阅手册以获取细节。实际上很多软件包在本平台上已经可用了，你不需要自己移植。详细的清单见下节。

## 可用的系统组件

&emsp;&emsp;所有的现有系统组件列于下表。如果提供了github链接，那么该组件现在就是可用的。新的组件现在还在被持续添加。  
- **[RVM/Lib](https://github.com/EDI-Systems/M7M2_MuAmmonite)**，一份RME的微控制器用户态库。
- **[RVM/RMP](https://github.com/EDI-Systems/M5P1_MuProkaron)**，RMP在RME上的一个全功能移植。
- **[RVM/FreeRTOS](https://github.com/EDI-Systems/FreeRTOS)**，广为应用的[FreeRTOS](https://www.freertos.org/)在RVM上的一个全功能移植。
- **[RVM/RT-Thread](https://github.com/EDI-Systems/rt-thread)**，颇有前景的[RT-Thread](https://www.rt-thread.org/)在RVM上的一个全功能移植，包括其所有框架。
- **[RVM/uCOSIII](https://github.com/EDI-Systems/uCOSIII)**，著名的[uC/OS III](https://www.micrium.com/)在RVM上的一个移植。如果要在产品中使用该系统，你应当持有该系统的商业许可。
- **[RVM/MicroPython](https://github.com/EDI-Systems/micropython)**，广为应用的[MicroPython](https://micropython.org/)在RVM上的一个移植。
- **[RVM/Lua](https://github.com/EDI-Systems/lua)**，广为应用的[Lua](https://www.lua.org/)在RVM上的一个移植。
- **[RVM/Duktape](https://github.com/EDI-Systems/duktape)**，新出现的[JavaScript](https://github.com/svaarala/duktape)在RVM上的一个移植。
- **[RVM/Essentials](https://github.com/EDI-Systems/M5P1_MuProkaron)**，一个包含了[lwip](https://savannah.nongnu.org/projects/lwip/)、[fatfs](http://elm-chan.org/fsw/ff/00index_e.html)和[emWin](https://www.segger.com/products/user-interface/emwin/)的[RMP](https://github.com/EDI-Systems/M5P1_MuProkaron)的RVM移植。要在产品中使用该系统，你应当持有相应的商业许可。

## 超级调用列表
|超级调用           |调用号|功能描述                                                            |
|:---------------:|:----:|:----------------------------------------------------------------:|
|RVM_HYP_ENAINT   |0     |开启虚拟机中断                                                       |
|RVM_HYP_DISINT   |1     |关闭虚拟机中断                                                       |
|RVM_HYP_REGINT   |2     |登记一个虚拟机中断                                                   |
|RVM_HYP_DELINT   |3     |删除一个虚拟机中断                                                   |
|RVM_HYP_REGEVT   |4     |登记一个虚拟机事件                                                   |
|RVM_HYP_DELEVT   |5     |删除一个虚拟机事件                                                   |
|RVM_HYP_WAITEVT  |6     |等待一个中断或事件到来                                                |
|RVM_HYP_SENDEVT  |7     |向虚拟机发送一个事件                                                  |
|RVM_HYP_QUERYEVT |8     |搜索一个事件                                                        |
|RVM_HYP_QUERYVM  |9     |搜索一个虚拟机                                                      |
|RVM_HYP_TIMPROG  |10    |重新编程虚拟机的定时器中断间隔                                         |
|RVM_HYP_PRINT    |11    |打印到控制台                                                        |

### 所有受支持架构上的典型性能数据
**单核微控制器**

|架构          |工具链         |实时系统   |Flash|SRAM|Yield|Mail |Sem  |Mail/I|Sem/I|Mem  |
|:-----------:|:------------:|:--------:|:---:|:--:|:---:|:---:|:---:|:----:|:---:|:---:|
|Cortex-M4    |Keil uVision 5|RMP       |     |    |     |     |     |      |     |     |
|Cortex-M4    |Keil uVision 5|FreeRTOS  |     |    |     |     |     |      |     |     |
|Cortex-M4    |Keil uVision 5|uCOSIII   |     |    |     |     |     |      |     |     |
|Cortex-M4    |Keil uVision 5|RT-Thread |     |    |     |     |     |      |     |     |
|Cortex-M7    |Keil uVision 5|RMP       |2.09 |2.29|1068 |1179 |1221 |1402  |1444 |176  |
|Cortex-M7    |Keil uVision 5|FreeRTOS  |     |    |     |     |     |      |     |     |
|Cortex-M7    |Keil uVision 5|uCOSIII   |     |    |     |     |     |      |     |     |
|Cortex-M7    |Keil uVision 5|RT-Thread |     |    |     |     |     |      |     |     |
|Cortex-R4    |TI CCS7       |RMP       |     |    |     |     |     |      |     |     |
|Cortex-R5    |TI CCS7       |RMP       |     |    |     |     |     |      |     |     |
|MIPS M14k    |XC32-GCC      |RMP       |     |    |     |     |     |      |     |     |

*Cortex-R4和Cortex-R5在这里被列为单核架构，因为它们的主要卖点是CPU冗余。因此从开发者的视角看来，它们的行为和单核系统一致。Cortex-R4和Cortex-R5上的双核模式不被RME支持。

&emsp;&emsp;**Flash和SRAM消耗以kB计，其他数据以CPU指令周期计。这里列出的所有值都是典型（有意义的系统配置）值而非绝对意义上的最小值，因为纯技术层面的最小配置在实际工程中很少是真正有用的。HAL库所造成的额外存储器消耗也被计算在内。**

- Cortex-M4平台使用STM32F405RGT6进行评估。
- Cortex-M7平台使用STM32F767IGT6进行评估。
- Cortex-R4平台使用TMS570LS0432进行评估。
- Cortex-R5平台使用TMS570LC4357进行评估。
- MIPS M14k平台使用PIC32MZ2048EFM100进行评估。

**多核微控制器**

|架构          |工具链         |实时系统   |Flash|SRAM|Yield|Mail |Sem  |Mail/I|Sem/I|Mem  |
|:-----------:|:------------:|:--------:|:---:|:--:|:---:|:---:|:---:|:----:|:---:|:---:|
|Cortex-R7    |TBD           |RMP       |     |    |     |     |     |      |     |     |
|Cortex-R8    |TBD           |RMP       |     |    |     |     |     |      |     |     |
|TMS320C66X   |TI CCS7       |RMP       |     |    |     |     |     |      |     |     |

&emsp;&emsp;**Flash和SRAM消耗以kB计，其他数据以CPU指令周期计。HAL库所造成的额外存储器消耗也被计算在内。**

- Cortex-R7平台使用（尚未决定）进行评估。
- Cortex-R8平台使用（尚未决定）进行评估。
- TMS320C66X平台使用TMS320C6678进行评估。

&emsp;&emsp;在上面所列的两个表格中，所有的编译器选项都被设置为最高优化（通常是-O3），而且针对运行时间进行了优化。
- Yield   ：两线程间进行切换所用的时间。  
- Mail    ：两线程间使用邮箱进行发送-接收操作的耗时。  
- Sem     ：两线程间使用计数信号量进行发布-获取操作的耗时。  
- Mail/I  ：从中断发送到某线程邮箱的耗时。 
- Sem/I   ：从中断发布信号量的耗时。 
- Mem     ：进行一次内存操作（比如分配或释放）的用时。 

## 新手上路

&emsp;&emsp;下面的说明会帮助你在本地快速建立一个可用来评估测试本系统的工程。请参看系统的中文文档以获取更多信息。

### 准备工作

&ensp;&ensp;你需要一个上面列出的硬件平台来进行测试。要运行测试，M7M1也是必要的。

### 编译指南
&emsp;&emsp;在 **_Project_** 文件夹下能够找到多种微控制器的移植好的 **厂商集成开发环境** 或 **Eclipse** 的工程样板。参看各个工程文件夹下的自述文件以获取更多关于如何编译和运行该工程的信息。某些工程需要额外的厂商硬件抽象层库的支持，它们可以在 **[M0P0_Library](https://github.com/EDI-Systems/M0P0_Library)** 软件仓库被找到。

&emsp;&emsp;如果你使用Windows系统，那么请将 **[二进制到C数组转换器](https://github.com/EDI-Systems/M7M2_MuAmmonite/blob/master/MAmmonite/Guest/bincopy.exe)** 拷贝到你的编译器可执行文件所在的文件夹内。
- 对于Keil uVision 5，这个位置是
```
    Keil\ARM\ARMCC\bin\bincopy.exe
```
- 对于Eclipse-GCC，请把它拷贝到GCC的安装文件夹
```
    gcc-installation-folder\bincopy.exe
```

### 运行测试
&emsp;&emsp;要运行测试，只要将测试下载到对应的开发板并开始单步调试即可。某些例子会采用两个LED来指示系统当前的状态，此时要填充LED的点亮和熄灭函数来运行该示例。


### 生产部署
&emsp;&emsp;当部署本系统到生产环境时，请仔细阅读本系统自带的手册，以确保各项配置正确。本系统的手册可以在 **_Documents_** 文件夹下找到。不推荐由用户自己配置内核；它包含太多的细节。请尽量使用提供好的默认配置文件。此外，一定要阅读对应架构的用户手册。

## 支持的工具链

- Keil uVision 5 (armcc)
- Code composer studio
- GCC/Clang-LLVM

&emsp;&emsp;其他的工具链现在不推荐或者当前不受支持，虽然要增加新的支持应该也很简单。

## 参与项目

&emsp;&emsp;请阅读[CONTRIBUTING.md](CONTRIBUTING.md)文档来获得关于行为规范和提交代码的相关信息。

## EDI 工程信息
&emsp;&emsp;演进 - 中生 - 菊石 (M7M2 R2T1)
