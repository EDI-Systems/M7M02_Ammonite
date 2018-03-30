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

### Context switch time and interrupt response time are inflated by 4x. Is it acceptable?
&ensp;&ensp;&ensp;&ensp;In most cases, **yes**. Because this VMM targets high-end MCUs, where the bare-metal RTOS response time is a **overkill** for many sensors and actuators. A Cortex-M4 @ 180MHz running virtualized FreeRTOS will have the same interrupt response time and context switch time of a Cortex-M3 @ 72MHz running bare-metal FreeRTOS. 

&ensp;&ensp;&ensp;&ensp;In terms of I/O response time, the Cortex-M3 is sufficient; we choose to use Cortex-M4 in these applications for **enhanced computing capability, not response time**. If your application does need a very low response time, you can write **RME native applications, which have similar response time when compared to FreeRTOS**. If you are using something faster than Cortex-M4 (e.g. Cortex-M7, TMS320C66X), you can ignore the overhead issue.

### How much do I need to change my existing code to port it to the VMM?
&ensp;&ensp;&ensp;&ensp;Very little; please read the manual for details. Actually many software packets are already available on the platform, so there's no need to port by yourself. For a comprehensive list of them, read the section below.

## Available system components
&ensp;&ensp;&ensp;&ensp;All the systems that have been virtualized on RVM is shown below. If a github link is provided, the component is available for now. New ports are being added at a constant rate.  
- **[RVM/Lib](https://github.com/EDI-Systems/M7M2_MuAmmonite)**, the microcontroller-oriented user-level library for RME.
- **[RVM/RMP](https://github.com/EDI-Systems/M5P1_MuProkaron)**, a port of the simplistic RMP on RVM, with all functionalities retained.
- **[RVM/FreeRTOS](https://github.com/EDI-Systems/FreeRTOS)**, a port of the widely-used [FreeRTOS](https://www.freertos.org/) to RVM.
- **[RVM/RT-Thread](https://github.com/EDI-Systems/rt-thread)**, a port of the promising [RT-Thread](https://www.rt-thread.org/) to RVM, with all frameworks retained.
- **[RVM/uCOSIII](https://github.com/EDI-Systems/uCOSIII)**, a port of the famous [uC/OS III](https://www.micrium.com/) to RVM. You should have a commercial license to use this port.
- **[RVM/MicroPython](https://github.com/EDI-Systems/micropython)**, a port of the popular [MicroPython](https://micropython.org/) to RVM.
- **[RVM/Lua](https://github.com/EDI-Systems/lua)**, a port of the easy-to-use [Lua](https://www.lua.org/) language to RVM.
- **[RVM/Duktape](https://github.com/EDI-Systems/duktape)**, a port of the emerging [JavaScript](https://github.com/svaarala/duktape) language to RVM.
- **[RVM/Essentials](https://github.com/EDI-Systems/M5P1_MuProkaron)**, a port of [lwip](https://savannah.nongnu.org/projects/lwip/), [fatfs](http://elm-chan.org/fsw/ff/00index_e.html) and [emWin](https://www.segger.com/products/user-interface/emwin/) to RVM, all packed in one [RMP](https://github.com/EDI-Systems/M5P1_MuProkaron) virtual machine. Be sure to obtain license to use these softwares.

## List of system calls


### Typical performance figures for all supported architectures

|Machine      |Toolchain     |RTOS      |Flash|SRAM|Yield|Mail |Sem  |Mail/I|Sem/I|Mem  |
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
  
*Cortex-R4 and Cortex-R5 are listed here as single-core architectures because their main selling point is CPU redundancy, thus from the viewpoint of the programmer they behave as if they have only one core. Dual-core mode of these two processors are not supported.  

&ensp;&ensp;&ensp;&ensp;**Flash and SRAM consumption is calculated in kB, while the other figures are calculated in CPU clock cycles. All values listed here are typical (useful system) values, not minimum values, because minimum values on system size seldom make any real sense. HAL library are also included in the size numbers. The absolute minimum value for microcontroller-profile RME is about 32k ROM/16k RAM.**

- Cortex-M4 is evaluated with STM32F405RGT6.
- Cortex-M7 is evaluated with STM32F767IGT6.
- Cortex-R4 is evaluated with TMS570LS0432.
- Cortex-R5 is evaluated with TMS570LC4357.
- MIPS M14k is evaluated with PIC32MZEFM100.

**Multi-core microcontrollers**

|Machine      |Toolchain     |RTOS      |Flash|SRAM|Yield|Mail |Sem  |Mail/I|Sem/I|Mem  |
|:-----------:|:------------:|:--------:|:---:|:--:|:---:|:---:|:---:|:----:|:---:|:---:|
|Cortex-R7    |TBD           |RMP       |     |    |     |     |     |      |     |     |
|Cortex-R8    |TBD           |RMP       |     |    |     |     |     |      |     |     |
|TMS320C66X   |TI CCS7       |RMP       |     |    |     |     |     |      |     |     |

&ensp;&ensp;&ensp;&ensp;**Flash and SRAM consumption is calculated in kB, while the other figures are calculated in CPU clock cycles. HAL library are also included in the size numbers. The absolute minimum value for MPU-based microprocessor-profile RME is about 64k ROM/32k RAM.**

- Cortex-R7 is evaluated with TBD.
- Cortex-R8 is evaluated with TBD.
- TMS320C66X is evaluated with TMS320C6678.

&ensp;&ensp;&ensp;&ensp;In the 2 tables above, all compiler options are the highest optimization (usually -O3) and optimized for time. 
- Yield    : The time to yield between different threads.  
- Mail     : The mailbox communication time between two threads.  
- Sem      : The semaphore communication time between two threads.  
- Mail/I   : The time to send to a mailbox from interrupt.  
- Sem/I    : The time to post to a semaphore from interrupt.  
- Mem      : The time to do an operation on memory, e.g. allocation/free. 

## Getting Started

&ensp;&ensp;&ensp;&ensp;These instructions will get you a copy of the project up and running on your local machine for development and testing purposes. See deployment for notes on how to deploy the project on a live system.

### Prerequisites

&ensp;&ensp;&ensp;&ensp;You need to choose a hardware platform listed above to run the tests. M7M1 is also needed t run the tests.

### Compilation
&ensp;&ensp;&ensp;&ensp;The **Vendor Toolchain** or **GNU Makefile** projects for various microcontrollers are available in the **_Project_** folder. Refer to the readme files in each folder for specific instructions about how to run them. However, keep in mind that some examples may need vendor-specific libraries such as the STM HAL. Some additional drivers may be required too.

### Running the tests
&ensp;&ensp;&ensp;&ensp;To run the sample programs, simply download them into the development board and start step-by-step debugging. All hardware the example will use is the serial port, and it is configured for you in the example.


### Deployment
&ensp;&ensp;&ensp;&ensp;When deploying this into a production system, it is recommended that you read the manual in the **_Documents_** folder carefully to configure all options correctly. It is not recommended to configure the kernel yourself, anyway; it included too many details. Please use the default configuration file as much as possible. Also, read the user guide for the specific platform you are using.

## Built With

- Keil uVision 5 (armcc)
- Code composer studio
- GCC/Clang-LLVM

&ensp;&ensp;&ensp;&ensp;Other toolchains are neither recommended nor supported at this point, though it might be possible to support them later on.

## Contributing

&ensp;&ensp;&ensp;&ensp;Please read [CONTRIBUTING.md](CONTRIBUTING.md) for details on our code of conduct, and the process for submitting pull requests to us.

## EDI Project Information
&ensp;&ensp;&ensp;&ensp;Mutate - Mesazoa - Ammonite (M7M2 R2T1)
