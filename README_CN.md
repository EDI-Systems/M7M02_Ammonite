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

- 应用程序编码错误。由于传统的RTOS不提供任何形式的内存隔离，因此一个任务内的错误会很容易传播到其他任务，然后毁掉整个软件系统。

- 辐射和[电磁干扰](https://en.wikipedia.org/wiki/Electromagnetic_interference)。这些外部干扰会翻转硬件状态位，从而在整个系统中造成偶发错误。

- [拒绝服务](https://en.wikipedia.org/wiki/Denial-of-service_attack)。一个高优先级任务如果在某个循环中死机，有可能导致整个系统失去响应。如果它频繁地调用某个可能导致锁调度器的系统调用，那就更糟，因为这必然会摧毁整个系统的实时性。

- 人为破坏。由于系统没有内存保护，黑客可以侵入IoT设备并且把系统功能任意破坏。

&emsp;&emsp;尽管这些弱点可以通过使用[看门狗定时器](https://en.wikipedia.org/wiki/Watchdog)重启系统（watch-dog timer，WDT）加以缓解，这种方法只对低复杂度的系统奏效。对于那些更复杂的应用或者关键应用，我们不能允许它们完全重启，这时传统的RTOS对此束手无策。

### Security
&ensp;&ensp;&ensp;&ensp;Traditional RTOSes generally does not consider information security as their design goal. They fail to meet security goals for future IoT systems. As stated in the reliability section, hackers can hack into the system and sabotage any part if they want to; usually a buffer overflow attack is sufficient. This is not the worst yet. The following situations can be much worse than a simple sabotage:

- For IoT blockchain applications such as [IOTA](https://iota.org/), the attacker can read your passwords, hack into your bank accounts, and possess all your money. 

- For smart home applications, the hackers can infiltrate your cameras and appliance controls and turn your life into a real-world Truman Show.

- For smart cars, the hackers can force you to drive as if you are in a Formula 1 race. This has happened on Tesla model 3 not long ago; nobody wants to experience this for sure.

- For critical life-support device such as medical ventilators, hackers can simply shut them down. No lives are lost to this so far, but if no measures are taken there will soon be some.

- For critical industry applications, hackers can turn off whole production line, or at least parts of it.

### Flexibility
&ensp;&ensp;&ensp;&ensp;The embedded software market have long been fragmented. Different RTOSes or software stacks target different markets, and each of them supports a mutually incompatible software stack. This was fine because embedded systems have long been designed for a dedicated purpose; nevertheless this assumption is no longer true today. IoT devices face increasing needs to integrate multiple functions. 

&ensp;&ensp;&ensp;&ensp;Instead of developing an RTOS that supports all the features, virtualization technology can let the different stacks coexist on the same physical machine. This will allow many new possibilities, yet it does not require the programmer to changed thei mental model of RTOS software development.

### Wouldn't the virtualization solution be very costly?
&ensp;&ensp;&ensp;&ensp;Short answer: **No**.  

&ensp;&ensp;&ensp;&ensp;Long answer: If the virtualization features are **designed carefully and used correctly** (especially the communication mechanisms), the virtualization overhead is negligible. Despite being virtualized, FreeRTOS's context switch and interrupt response is just 4x of the original FreeRTOS, and is still 25x better than RT-Linux. 

&ensp;&ensp;&ensp;&ensp;Some may argue that an Unix designed for MCUs will be good enough and we don't need virtualization; however this will involve reinventing the wheel and in the end you get something no better than Linux, or another VxWorks. Linux is not considered reliable and secure at all in embedded systems; it is flexible but incurs unacceptable memory overhead, so this doesn't mitigate the overhead issue and instead exacerbate it. On the other hand, virtual-machines are inherently safe and modular, not to mention that the underlying RME system provides capability-based security.

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
