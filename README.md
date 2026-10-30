<h1 align="center">
	<img width="300" src="/Document/Demo/logo.png" alt="logo">
</h1>

# Unity Platform (RVM)
<div align="center">

[![Github release](https://img.shields.io/github/release/EDI-Systems/M7M02_Ammonite.svg)](https://github.com/EDI-Systems/M7M02_Ammonite/releases/latest)
[![Github commits](https://img.shields.io/github/commits-since/EDI-Systems/M7M02_Ammonite/main@{30day}.svg)](https://github.com/EDI-Systems/M7M02_Ammonite/compare/main@{30day}...main)
[![Discord](https://img.shields.io/badge/chat-Discord-purple)](https://discord.gg/VxCFSFC6bW)
![QQ Group](https://img.shields.io/badge/QQ_Group-1038895132-red)

</div>
<div align="center">

![language](https://img.shields.io/badge/language-C-orange.svg)
[![OpenSSF Best Practices](https://www.bestpractices.dev/projects/1836/badge)](https://www.bestpractices.dev/projects/1836) 
[![Codacy Badge](https://app.codacy.com/project/badge/Grade/fb3f40e96daa42169333ba698b4fd083)](https://app.codacy.com/gh/EDI-Systems/M7M02_Ammonite/dashboard?utm_source=gh&utm_medium=referral&utm_content=&utm_campaign=Badge_grade)

</div>

点击 **[这里](README_CN.md)** 查看中文版。

&ensp;&ensp;&ensp;&ensp;**RVM** is a virtual machine monitor (VMM) specifially designed for microcontrollers (MCUs). This VMM can efficiently paravirtualize many RTOSes and bare-metal frameworks. This is a groundbreaking technology and the future of IoT systems, and delivers many possibilities previously considered impossible. With unprecedented scalability, it can run 64 virtual-machines on 1MB SRAM.

&ensp;&ensp;&ensp;&ensp;The manual of the virtual machine monitor can be found **[here](https://github.com/EDI-Systems/M7M2_MuAmmonite/blob/master/Documents/M7M2_RTVMM-User-Manual.pdf)**.

&ensp;&ensp;&ensp;&ensp;Read **[Contributing](CONTRIBUTING.md)** and **[Code of Conduct](CODE_OF_CONDUCT.md)** if you want to contribute, and **[Pull Request Template](PULL_REQUEST_TEMPLATE.md)** when you make pull requests.
This software is an official work of EDI, and thus belongs to the **public domain**. All copyrights reserved by EDI are granted to all entities under all applicable laws to the maximum extent.

&ensp;&ensp;&ensp;&ensp;For vendor-supplied packages and hardware abstraction libraries, please refer to the **[M0P0_Library](https://github.com/EDI-Systems/M0P0_Library)** repo to download and use them properly. This VMM depends on **[M7M1 micro-kernel OS](https://github.com/EDI-Systems/M7M1_MuEukaron)** as well.

## Introduction to virtualization
&ensp;&ensp;&ensp;&ensp;[Virtualization](https://en.wikipedia.org/wiki/Virtualization) is a technology that creates logical copies of the same hardware thus that multiple bare-metal programs and operating systems can coexist on the same hardware platform. On the PC architecture, virtual machine monitors such as [VMware](https://www.vmware.com/) and [Virtual Box](https://www.virtualbox.org/) have been around for decades, and have since shaped our IT infrastructure till today. The main reason to run multiple virtual machines on the same piece of hardware include increased isolation, ease of deployment and flexible management of computational resources.  

&ensp;&ensp;&ensp;&ensp;There are two types of virtualization: [full-virtalization](https://en.wikipedia.org/wiki/Full_virtualization) and [para-virtualization](https://en.wikipedia.org/wiki/Paravirtualization). The former can run an unmodified image of the operating system; while the latter requires the operating system to be modified to some extent. RVM employs the para-virtuliation technology to reduce the time and memory overhead. It provided hypercalls for many sensitive priviledged operations.

## Why do we need virtualization on MCUs?
&ensp;&ensp;&ensp;&ensp;The virtualization technology will shape the future of [Internet-of-Things](https://en.wikipedia.org/wiki/Internet_of_things) (IoT) systems, just like how it shaped our IT infrastructure today. However, the reasons for this are different: on microcontrollers, the reasons to use this technology is reliability, security and flexibility.

### Reliability
&ensp;&ensp;&ensp;&ensp;Traditional RTOSes employ static code analysis and certification to reach its reliability goal. However, this have proved insufficient for many scenarios; besides the RTOS itself, the following will also introduce errors in the system:  

- Application bugs. Because traditional RTOSes does not have memory isolation of any kind, a bug in a single application will easily propagate and destroy the whole software system.

- Radiation and [EMI](https://en.wikipedia.org/wiki/Electromagnetic_interference). These external interferences flip bits in hardwares and can cause sporadic errors throughout the system.

- [Denial of service](https://en.wikipedia.org/wiki/Denial-of-service_attack). One high-priority thread, if dead in a loop due to some error, will cause the whole system to be responseless. If it repeatedly calls some system call that may lock the scheduler, this will degrade performance of the hard real-time system significantly.

- Sabotage. Due to lack of memory protection, hackers can easily infiltrate the IoT device and destroy its functionality.  

&ensp;&ensp;&ensp;&ensp;Though these weaknesses can be mitigated with watch-dog timers (WDTs) by rebooting the system, this method only works for simple applications. For more complex applications or critical applications that cannot afford a complete reboot, traditional RTOSes can do nothing about this situation.

### Security
&ensp;&ensp;&ensp;&ensp;Traditional RTOSes generally does not consider information security as their design goal. They fail to meet security goals for future IoT systems. As stated in the reliability section, hackers can hack into the system and sabotage any part if they want to; usually a buffer overflow attack is sufficient. This is not the worst yet. The following situations can be much worse than a simple sabotage:

- For smart home applications, the hackers can infiltrate cameras and appliance controls and start a real-world Truman Show. (This is possibly the source of some interesting racy video snippets that are currently circulating the internet.)

- For smart cars, the hackers can force the driver to drive as if you are in a Formula 1 race. This has happened on Tesla model 3 not long ago.

- For critical life-support device such as medical ventilators, hackers can simply shut them down.

- For critical industry applications, hackers can turn off whole production line, or at least parts of it.

### Flexibility
&ensp;&ensp;&ensp;&ensp;The embedded software market have long been fragmented. Different RTOSes or software stacks target different markets, and they are mutually incompatible. This was fine because embedded systems have long been designed for a dedicated purpose; nevertheless this assumption is no longer true today. IoT devices face increasing needs to integrate multiple functions. 

&ensp;&ensp;&ensp;&ensp;Instead of developing an RTOS that supports all the features, virtualization technology can let the different stacks coexist on the same physical machine. This will allow many new possibilities, yet it does not require the programmer to changed thei mental model of RTOS software development.

### Wouldn't the virtualization solution be very costly?
&ensp;&ensp;&ensp;&ensp;Short answer: **No**.  

&ensp;&ensp;&ensp;&ensp;Long answer: If the virtualization features are **designed carefully and used correctly** (especially the communication mechanisms), the virtualization overhead is negligible. Despite being virtualized, FreeRTOS's context switch and interrupt response is just 4x of the original FreeRTOS, and is still 25x better than RT-Linux. For those CPU-hungry virtual machines that do not do I/O much, the overhead is negligible.

&ensp;&ensp;&ensp;&ensp;Some may argue that an Unix designed for MCUs will be good enough and we don't need virtualization; however this will involve reinventing the wheel and in the end you get something no better than Linux, or another VxWorks (there are some small RTOSes that dash in this direction which we don't appreciate). Linux is not considered reliable and secure* at all in embedded systems; it is flexible but incurs unacceptable memory overhead, so this doesn't mitigate the overhead issue and instead exacerbate it. On the other hand, virtual-machines are inherently safe and modular, not to mention that the underlying RME system provides capability-based security.  
*The reason why Linux is considered unsafe in embedded realm is that many embedded devices never update their kernel to the latest version. This caused them to rely on an outdated kernel whose vulnerabilities are well-known to all. You can randomly pick a ready-made tool then scan these routers and IP cameras, and get as many zombies as you want. Oh yes, this also plagues the old Android mobile phones.

### Context switch time and interrupt response time are inflated by 4x. Is it acceptable?
&ensp;&ensp;&ensp;&ensp;For MCUs that are supported by RVM, **yes**. Because this VMM targets high-end MCUs, where the bare-metal RTOS response time is a **overkill** for many sensors and actuators. A Cortex-M4 @ 180MHz running virtualized FreeRTOS will have the same interrupt response time and context switch time of a Cortex-M3 @ 72MHz running bare-metal FreeRTOS. 

&ensp;&ensp;&ensp;&ensp;In terms of I/O response time, the Cortex-M3 is sufficient; we choose to use Cortex-M4 in these applications for **enhanced computing capability, not response time**. If your application does need a very low response time, you can write **RME native applications, which have similar response time when compared to FreeRTOS**. If you are using something faster than Cortex-M4 (e.g. Cortex-M7, TMS320C66X), you can ignore the overhead issue.

### How much do I need to change my existing code to port it to the VMM?
&ensp;&ensp;&ensp;&ensp;Very little; please read the manual for details. Actually many software packets are already available on the platform, so there's no need to port by yourself.

## List of hyper calls

|Hypercall        |Number|Description                                                       |
|:---------------:|:----:|:----------------------------------------------------------------:|
|RVM_HYP_SPURIOUS |0     |Spurious hypercall, ignored (timer interrupt arrived before the request)|
|RVM_HYP_PUTCHAR  |1     |Print one character to the console                                |
|RVM_HYP_REBOOT   |2     |Reboot the current virtual machine                                |
|RVM_HYP_INT_ENA  |3     |Enable virtual machine interrupts                                 |
|RVM_HYP_INT_DIS  |4     |Disable virtual machine interrupts                                |
|RVM_HYP_VCT_PHYS |5     |Map a virtual vector of the virtual machine to a physical vector  |
|RVM_HYP_VCT_EVT  |6     |Map a virtual vector of the virtual machine to an event channel   |
|RVM_HYP_VCT_DEL  |7     |Delete a virtual vector mapping                                   |
|RVM_HYP_VCT_LCK  |8     |Lockdown the virtual vector mappings of the virtual machine       |
|RVM_HYP_VCT_WAIT |9     |Sleep the virtual machine until a vector comes in                 |
|RVM_HYP_EVT_ADD  |10    |Add the send capability of an event source to the virtual machine |
|RVM_HYP_EVT_DEL  |11    |Delete the send capability of an event source from the virtual machine|
|RVM_HYP_EVT_SND  |12    |Send an event to an event channel                                 |
|RVM_HYP_WDG_CLR  |13    |Start and feed the watchdog                                       |

### Commemorative ports

|Machine      |Toolchain     |RTOS      |Flash|SRAM|Yield|Mail |Sem  |Mail/I|Sem/I|Mem  |
|:-----------:|:------------:|:--------:|:---:|:--:|:---:|:---:|:---:|:----:|:---:|:---:|
|Cortex-M7    |GCC           |DjyOS     |27.49|63.38| 1017| 1488|  892|  1873| 1220|  288|
|...          |...           |Litex     |     |    |     |     |     |      |     |     |
|...          |...           |Contiki   |     |    |     |     |     |      |     |     |



### Practical ports

|Machine      |Toolchain     |RTOS      |Flash|SRAM|Yield|Mail |Sem  |Mail/I|Sem/I|Mem  |
|:-----------:|:------------:|:--------:|:---:|:--:|:---:|:---:|:---:|:----:|:---:|:---:|
|  Cortex-M0+|Keil uVision 5|       RMP|16.71|3.88|  465|  783|  681|  1677| 1572|  N/A|
|         ...|          ...|  FreeRTOS|12.74|9.57|  433|  949| 1023|  1789| 1808|  843|
|         ...|          ...|   uCOSIII|13.63|8.09|  863| 1117| 1143|  2125| 2156|  363|
|         ...|          ...| RT-Thread|18.69|8.27|  950| 1581| 1317|  2760| 2464|13221|
|         ...|    IAR EWARM|       RMP|12.38|3.88|  511|  876|  770|  1764| 1642|  N/A|
|         ...|          ...|  FreeRTOS|11.29|9.57|  432|  944| 1078|  1804| 1859|  944|
|         ...|          ...|   uCOSIII|11.89|8.08|  892| 1154| 1187|  2161| 2189|  360|
|         ...|          ...| RT-Thread|13.15|8.26| 1031| 1680| 1415|  2859| 2589|13861|
|         ...|          GCC|       RMP|22.72|2.88|  507|  883|  773|  1746| 1680|  N/A|
|         ...|          ...|  FreeRTOS|15.38|8.57|  455|  986| 1108|  1859| 1926| 1012|
|         ...|          ...|   uCOSIII|15.38|7.09|  871| 1144| 1185|  2188| 2246|  396|
|         ...|          ...| RT-Thread|18.57|7.27| 1080| 1755| 1450|  3042| 2694|14899|
|   Cortex-M4|Keil uVision 5|       RMP|22.72|37.06|  336|  519|  452|  1278| 1176|  391|
|         ...|          ...|  FreeRTOS|14.73|41.57|  308|  612|  776|  1394| 1428|  527|
|         ...|          ...|   uCOSIII|15.50|9.09|  504|  691|  713|  1570| 1588|  196|
|         ...|          ...| RT-Thread|20.34|8.27|  532| 1036|  824|  1948| 1743| 7307|
|         ...|    IAR EWARM|       RMP|14.29|36.07|  376|  602|  551|  1512| 1438|  403|
|         ...|          ...|  FreeRTOS|11.48|40.57|  316|  648|  833|  1592| 1667|  549|
|         ...|          ...|   uCOSIII|11.65|8.08|  601|  809|  829|  1852| 1868|  207|
|         ...|          ...| RT-Thread|13.78|7.26|  612| 1053|  852|  2231| 2020| 7646|
|         ...|          GCC|       RMP|24.20|36.07|  416|  709|  552|  1434| 1344|  382|
|         ...|          ...|  FreeRTOS|15.44|40.57|  372|  726|  882|  1544| 1611|  724|
|         ...|          ...|   uCOSIII|14.67|8.09|  580|  812|  849|  1759| 1820|  227|
|         ...|          ...| RT-Thread|17.63|7.27|  648| 1173|  893|  2236| 1975| 7764|
|   Cortex-M7|Keil uVision 5|       RMP|22.73|37.06|  244|  384|  370|   920|  811|  284|
|         ...|          ...|  FreeRTOS|15.06|41.57|  229|  494|  595|   954|  987|  347|
|         ...|          ...|   uCOSIII|15.75|9.09|  410|  590|  594|  1227| 1252|  141|
|         ...|          ...| RT-Thread|20.52|8.28|  457|  798|  651|  1537| 1391| 5197|
|         ...|    IAR EWARM|       RMP|14.37|36.07|  272|  459|  412|  1030|  985|  284|
|         ...|          ...|  FreeRTOS|11.50|40.57|  220|  526|  609|  1122| 1109|  363|
|         ...|          ...|   uCOSIII|11.70|8.08|  411|  592|  620|  1282| 1313|  151|
|         ...|          ...| RT-Thread|13.85|7.26|  424|  829|  675|  1585| 1441| 5710|
|         ...|          GCC|       RMP|24.29|36.07|  248|  476|  414|   886|  939|  283|
|         ...|          ...|  FreeRTOS|15.29|40.57|  225|  511|  596|  1020| 1036|  354|
|         ...|          ...|   uCOSIII|14.66|8.09|  404|  561|  578|  1216| 1291|  150|
|         ...|          ...| RT-Thread|17.75|7.27|  465|  819|  674|  1570| 1435| 6402|
|      RISC-V|          GCC|       RMP|29.61|25.29|  322|  499|  437|  1719| 1652|  360|
|         ...|          ...|  FreeRTOS|17.55|24.77|  319|  580|  630|  1778| 1827|  440|
|         ...|          ...|   uCOSIII|16.72|8.09|  478|  606|  611|  2050| 2079|  182|
|         ...|          ...| RT-Thread|19.45|9.77|  533|  846|  701|  2429| 2280| 6003|

&ensp;&ensp;&ensp;&ensp;**Flash and SRAM consumption is calculated in kB, while the other figures are calculated in CPU clock cycles. All values listed here are typical values that exercises the full system benchmark, not minimum values, because minimum values on system size seldom make any real sense.**

- Cortex-M0+ is evaluated with STM32G071RBT6.
- Cortex-M4 is evaluated with STM32F429ZIT6.
- Cortex-M7 is evaluated with STM32H743ZIT6.
- RV32IMAC is evaluated with CH32V307VCT6.
- All figures are measured with the same benchmark suite as RVM guests, on real hardware, across all listed toolchain/RTOS combinations: GCC at -O3, IAR/Keil at their highest optimization. Flash/SRAM are guest image footprints (code+data / data+bss); all other figures are CPU clock cycles.


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

&ensp;&ensp;&ensp;&ensp;If you are using the windows operating system, copy the **[binary to C converter](https://github.com/EDI-Systems/M7M2_MuAmmonite/blob/master/MAmmonite/Guest/bincopy.exe)** to the folder that contains your compiler.  
- For Keil uVision 5, this is located at
```
    Keil\ARM\ARMCC\bin\bincopy.exe
```
- For Eclipse-GCC, copy it to the gcc installation folder.
```
    gcc-installation-folder\bincopy.exe
```

### Running the tests
&ensp;&ensp;&ensp;&ensp;To run the sample programs, simply download them into the development board and start step-by-step debugging. All hardware the example will use is the serial port, and it is configured for you in the example.


### Deployment
&ensp;&ensp;&ensp;&ensp;When deploying this into a production system, it is recommended that you read the manual in the **_Documents_** folder carefully to configure all options correctly. It is not recommended to configure the kernel yourself, anyway; it included too many details. Please use the default configuration file as much as possible. Also, read the user guide for the specific platform you are using.

&ensp;&ensp;&ensp;&ensp;If you are deploying an RTOS (FreeRTOS, uC/OS-III or RMP) as a guest, see [Guest/GUEST.md](Guest/GUEST.md) for how to install the paravirtualized guest ports, and for each port's contract, implementation strategy and the subtle deviations between the two.

## Built With

- Keil uVision 5 (armcc)
- Code composer studio
- GCC/Clang-LLVM

&ensp;&ensp;&ensp;&ensp;Other toolchains are neither recommended nor supported at this point, though it might be possible to support them later on.

## Contributing

&ensp;&ensp;&ensp;&ensp;Please read [CONTRIBUTING.md](CONTRIBUTING.md) for details on our code of conduct, and the process for submitting pull requests to us.

### EDI Project Information
- M7M02 R3T1

### Starring Contributors
- Bichen Li - Graphical User Interface for Project Generator.
