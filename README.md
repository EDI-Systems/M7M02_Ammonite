<h1 align="center">
	<img width="300" src="https://raw.githubusercontent.com/EDI-Systems/M7M2_MuAmmonite/master/Documents/Demo/logo.png" alt="logo">
</h1>

# Unity Platform (RVM)

点击 **[这里](README_CN.md)** 查看中文版。

&ensp;&ensp;&ensp;&ensp;**RVM** is a virtual machine monitor (VMM) specifially designed for microcontrollers (MCUs). This VMM can efficiently paravirtualize many RTOSes and bare-metal frameworks. This is a groundbreaking technology and the future of IoT systems, and delivers many possibilities previously considered impossible. With unprecedented scalability, it can run 64 virtual-machines on 1MB SRAM.

&ensp;&ensp;&ensp;&ensp;The manual of the virtual machine monitor can be found **[here](https://github.com/EDI-Systems/M7M2_MuAmmonite/blob/master/Documents/M7M2_RTVMM-User-Manual.pdf)**.

&ensp;&ensp;&ensp;&ensp;Read **[Contributing](CONTRIBUTING.md)** and **[Code of Conduct](CODE_OF_CONDUCT.md)** if you want to contribute, and **[Pull Request Template](PULL_REQUEST_TEMPLATE.md)** when you make pull requests.
This software is **triple-licensed**: it is either **[LGPL v3](LICENSE.md)** or **[modified MIT license](MODMIT.md)**. **Commercial** licenses are also available upon request.

&ensp;&ensp;&ensp;&ensp;For vendor-supplied packages and hardware abstraction libraries, please refer to the **[M0P0_Library](https://github.com/EDI-Systems/M0P0_Library)** repo to download and use them properly. This VMM depends on **[M7M1 micro-kernel OS](https://github.com/EDI-Systems/M7M1_MuEukaron)** as well.

## Introduction to virtualization
&ensp;&ensp;&ensp;&ensp;Virtualization is a technology that creates logical copies of the same hardware thus that multiple bare-metal programs and operating systems can coexist on the same hardware platform. On the PC architecture, virtual machine monitors such as VMWare and Virtual Box have been around for decades, and have since shaped our IT infrastructure till today. The main reason to run multiple virtual machines on the same piece of hardware include increased isolation, ease of deployment and flexible management of computational resources.  

&ensp;&ensp;&ensp;&ensp;There are two types of virtualization: full-virtalization and para-virtualization. The former can run an unmodified image of the operating system; while the latter requires the operating system to be modified to some extent. RVM employs the para-virtuliation technology to reduce the time and memory overhead. It provided hypercalls for many sensitive priviledged operations.

## Why do we need virtualization on MCUs?
&ensp;&ensp;&ensp;&ensp;The virtualization technology will shape the future of IoT systems, just like how it shaped our IT infrastructure today. However, the reasons for this are different: on microcontrollers, the reasons to use this technology is reliability, security and flexibility.

### Reliability
&ensp;&ensp;&ensp;&ensp;Traditional RTOSes employ static code analysis and certification to reach its reliability goal. However, this have proved insufficient for many scenarios; besides the RTOS itself, the following will also introduce errors in the system:  
- Application bugs. Because traditional RTOSes does not have memory isolation of any kind, a bug in a single application will easily propagate and destroy the whole software system.
- Radiation and EMI. These external interferences flip bits in softwares and can cause sporadic errors throughout the system.
- Denial of service. One high-priority thread, if dead in a loop due to some error, will cause the whole system to be responseless. If it repeatedly calls some system call that may lock the scheduler, this will degrade performance of the hard real-time system significantly.
- Sabotage. Due to lack of memory protection, hackers can easily infiltrate the IoT device and render it their toy.  
Though these weaknesses can be mitigated with watch-dog timers (WDTs) by rebooting the system, this method only works for simple applications. For more complex applications or critical applications that cannot afford a complete reboot, traditional RTOSes can do nothing about this situation.

### Security
&ensp;&ensp;&ensp;&ensp;Traditional RTOSes generally does not consider information security as their design goal. They fail to meet security goals for future IoT systems. As stated in the reliability section, hackers can hack into the system and sabotage any part if they want to; usually a buffer overflow attack is sufficient. This is not the worst yet. The following situations can be much worse than a simple sabotage:
- For IoT blockchain applications such as [IOTA](https://iota.org/), the attacker can read your passwords, hack into your bank accounts, and possess all your money. 
- For smart home applications, the hackers can infiltrate your cameras and appliance controls and turn your life into a real-world Truman Show.
- For smart cars, the hackers can force you to drive as if you are in a Formula 1 race. This has happened on Tesla model 3 not long ago; nobody wants to experience this for sure.
- For critical life-support device such as medical ventilators, hackers can simply shut them down. No lives are lost to this so far, but if no measures are taken there will soon be some.
- For critical industry applications, hackers can turn off whole production line, or at least parts of it.

### Flexibility
&ensp;&ensp;&ensp;&ensp;The embedded software market have long been fragmented. Different RTOSes or software stacks target different markets, and each of them supports a mutually incompatible software stack. This was fine because embedded systems have long been designed for a dedicated purpose; nevertheless this assumption is no longer true today. IoT devices face increasing needs to integrate multiple functions. Instead of developing an RTOS that supports all the features, virtualization technology can let the different stacks coexist on the same physical machine. This will allow many new possibilities, yet it does not require the programmer to changed thei mental model of RTOS software development.

### Wouldn't the virtualization solution be very costly?
&ensp;&ensp;&ensp;&ensp;Short answer: **No**.  
&ensp;&ensp;&ensp;&ensp;Long answer: If the virtualization features are **designed carefully and used correctly** (especially the communication mechanisms), the virtualization overhead is negligible. Despite being virtualized, FreeRTOS's context switch and interrupt response is just 4x of the original FreeRTOS, and is still 25x better than RT-Linux. Some may argue that an Unix designed for MCUs will be good enough and we don't need virtualization; however this will involve reinventing the wheel and in the end you get something no better than Linux, or another VxWorks. Linux is not considered reliable and secure at all in embedded systems; it is flexible but incurs unacceptable memory overhead, so this doesn't mitigate the overhead issue and instead exacerbate it. On the other hand, virtual-machines are inherently safe and modular, not to mention that the underlying RME system provides capability-based security.

### Context switch time and interrupt response time are inflated by 4x. Is it acceptable?
&ensp;&ensp;&ensp;&ensp;In most cases, **yes**. Because this VMM targets high-end MCUs, where the bare-metal RTOS response time is a **overkill** for many sensors and actuators. A Cortex-M4 @ 180MHz running virtualized FreeRTOS will have the same interrupt response time and context switch time of a Cortex-M3 @ 72MHz running bare-metal FreeRTOS. In terms of I/O response time, the Cortex-M3 is sufficient; we choose to use Cortex-M4 in these applications for **enhanced computing capability, not response time**. If your application does need a very low response time, you can write **RME native applications, which have similar response time when compared to FreeRTOS**. If you are using something faster than Cortex-M4 (e.g. Cortex-M7, TMS320C66X), you can ignore the overhead issue.

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
