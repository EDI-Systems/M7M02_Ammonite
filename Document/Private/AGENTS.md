# RVM 客机移植工程手册（坑库与方法论）

> **覆盖**：FreeRTOS V9 / uC-OS-III V3.08 / RT-Thread V4.1.1 的 A6M·RV32P·A7M 三架构 × GCC·Keil·IAR 链；
> RMP-on-RVM（IAR 三链）；**DjyOS V1.1.0 纪念性移植（M7 + GCC 单组合，11/11 真机定档）**。
> 面向贡献者：开工读 §1~§3；排障查 §4、§5；改端口前必读 §6（契约理论）与对应客机章；
> 动手前扫一眼 §13 军规；复盘查 §14 判例索引。
> **正式数据一律以各客机 `test/**/chip/*_test_*_rvm.h` 档案块为准**——本文件只留索引、判例与方法论。

---

## 1. 阅读路线与本文件结构

### 1.1 按任务选路线

| 你要做什么 | 路线 |
|---|---|
| 第一次接手本仓 | §2 拓扑 → §3 工作流 → §13 军规；先跑通一条链再动手 |
| 排一个具体故障 | §4 排障方法论（按威力排序）→ §5 坑库 → §14 判例索引 |
| 改 / 新增客机端口 | §6 移植契约理论 → 对应客机章（§7~§10）→ §6.5 移植五准则 |
| 改 A7M 快路径 | §7.1 fast yield（含 ICI/IT 脏帧过滤的三层架构） |
| 加一个新 RTOS | §6 契约理论 + §7.1 模板 + §11 生成器（rvp / rva / Guest_Type） |
| 只关心成绩 | §12 成果基线（数值在 test 头档案块里） |

### 1.2 章节地图

| 章 | 内容 | 何时读 |
|---|---|---|
| §1 | 阅读路线、章节地图、维护约定 | 开工 |
| §2 | 项目拓扑、路径速查、工具链、客机端口目录布局 | 开工 |
| §3 | 标准工作流：生成 / 构建 / 烧录 / 捕获 / 收尾 | 每次改代码 |
| §4 | 排障方法论（断点裁决 / 写黑洞 / 现场活捉 / 对账计数 …） | 一出故障 |
| §5 | 坑库（文件编辑 / PowerShell / 生成器 / iasmarm / IAR / OpenOCD / 硬件） | 一出故障 |
| §6 | RTOS 移植契约理论（四 OS 契约 / 原子性范式 / RVM 中断模型 / 五准则） | 设计端口 |
| §7 | FreeRTOS 端口设计决策（A7M fast yield、IAR 改编、格式规范、双工具链） | 改 FRT |
| §8 | uC/OS-III 移植要点（RV32P / A7M / Keil / IAR 四轮沉淀） | 改 UO3 |
| §9 | RT-Thread 移植要点（三端口 / 配置坑 / 三编译器 / 四板判例） | 改 RTT |
| §10 | DjyOS 纪念性移植（事件驱动内核压进 RVM 两线程模型） | 改 DJY |
| §11 | 生成器知识（rvgen 重建、Guest 镜像规范、rvp 不变量） | 改生成器 / rvp |
| §12 | 成果基线索引（各客机链状态 + 数据判读量级） | 查进度 |
| §13 | 一句话军规（事故浓缩，收工自检） | 收工 |
| §14 | 历史判例总索引（一案一行） | 复盘 |

### 1.3 维护约定

- **新判例**：先在正文写清「现象 / 根因 / 修法 / 落点」，再在 §14 顺延加一行。
- **数据不进正文**：成绩写进对应 `*_test_*_rvm.h` 档案块，本文件只留索引。
- **交叉引用**：一律 `§章.节`；改章号必须全库替换（本轮做过一次整编，旧稿见 git 历史）。
- **会话收尾**：见 §3.7 三清；`git status --porcelain -- Project/` 必须空。

---

## 2. 项目拓扑与路径速查

### 2.1 仓库关系

| 仓库 | 角色 | 关键事实 |
|---|---|---|
| `../FreeRTOS_V900`（姊妹仓） | FreeRTOS 官方完整树 + RVM 移植层 | **内核文件永不改**（tasks.c/list.c/queue.c/timers.c/include）；改动只落 portable/*_RVM/ 与 test/ |
| `../uCOSIII_V308`（姊妹仓） | uC/OS-III 客机包（三仓嵌套） | uC-OS3 V3.08.02 + uC-CPU + uC-LIB，全部官方 Apache-2.0 零修改；我们的件在 *-RVM 目录 |
| `../RTThread_V411`（姊妹仓） | RT-Thread V4.1.1 官方树 + RVM 移植层 | 官方源零逻辑修改（仅 kservice.c 版本打印的字符串参数，见 §9.4）；我们的件在 libcpu/*_rvm/ 与 test/ |
| `../M7M01_Eukaron`（姊妹仓） | RME 微内核（RVM 内核底座） | 平台头 `Include\Platform\{A6M,A7M}\Chip\<CHIP>\rme_platform_*.h`——**只是模板**（见坑 5.3 头条） |
| `../M5P01_Prokaron`（姊妹仓） | RMP RTOS + RMP-on-RVM 移植 | A7M_RVM fast yield 参考实现；IAR 版 A7M 汇编金矿；**RTT armv7m 端口宏的修正蓝本**（rmp_platform_a7m_rvm_gcc.s） |
| `../DjyOS_V110`（姊妹仓） | DjyOS V1.1.0 官方树 + RVM 移植层 | 内核源码**零修改**；我们的件在 `bsp/arch/arm/cortex-m/armv7e-m_rvm/` 与 `test/djy_*.c`，要点见 §10 |
| 本仓库（M7M02_Ammonite） | RVM hypervisor + 生成器 + 工作区 | 生成与构建主战场，见下 |

### 2.2 M7M02_Ammonite 内部

- `Project\*.rvp`：工程描述 XML（`gccmf-*`=GCC/Makefile，`rvmdk-*`=Keil，`ewarm-*`=IAR；`-frt`/`-uo3`/`-rtt`/`-djy` 后缀=客机类型；`-nofpu`=无协处理器）
- `Project\<TOOLCHAIN>-<CHIP>-<GUEST>\`：生成的工作区（Kernel/Monitor/Proc1/Proc2/Virt1 五子工程）
- `Project\benchmark.sh <rvp>`：生成（带 `-b` 基准模式，挂 test 头/源）——**客机链必用**，否则 Virt1 编不过；`generate.sh` 出静默版
- `Utility\M7M02_Final\rvgen.exe`：生成器；源在 `Utility\M7M02_Projgen\Source\`，改后 g++ @resp.txt 重建（§11.1）
- `Utility\M7M02_Final\bincopy.exe`：把 VM .bin 转成 `*_image.c` 供 Kernel 链接
- `Guest\update.sh`：发布镜像刷新（增量单向，不参与构建，规范见 §11.2）

### 2.3 工具链

| 工具 | 版本 | 备注 |
|---|---|---|
| arm-none-eabi-gcc | 13.2.1 | GNU Arm 工具链（`C:\Toolchain\arm-none-eabi-mingw64`） |
| make | MinGW 版 | Windows 下驱动生成的 Makefile |
| armclang / UV4 | ARMCLANG 6.23 | UV4.exe -b 命令行构建 |
| ICCARM | 9.40 | iarbuild/ielftool/ielfdumparm/iasmarm 全家 |
| riscv-none-elf-gcc | 12.2.0 (xPack) | `C:\MounRiver\MounRiver_Studio\toolchain\RISC-V Embedded GCC12\bin` |
| MinGW g++ | 12.2 | 重建 rvgen 用 |
| OpenOCD（WCH fork） | 0.11.0 | 坑多见 5.6；STM32 烧录主用 STM32CubeProgrammer CLI |
| 调试串口 | 板载调试器 VCP | 115200-8N1；COM 口号随系统枚举变化（发现方法见 5.2），勿写死 |

### 2.4 客机端口目录布局（现行）

```
FreeRTOS_V900\portable\
├── GCC\{ARM_A6M_RVM, ARM_A7M_RVM, RISC-V_RV32P_RVM}
├── IAR\{ARM_A6M_RVM, ARM_A7M_RVM}          ← RVDS 承载 armcc 端口（官方命名）
└── RVDS\{ARM_A6M_RVM, ARM_A7M_RVM}          ← RV32P 仅 GCC（Keil/IAR 生成会被存在性检查拦）

uCOSIII_V308\
├── uC-OS3\Ports\ARM-Cortex-M\{ARMv6-M-RVM, ARMv7-M-RVM}\ 与 Ports\RISC-V\RV32-RVM\
│   -RVM 内：os_cpu_c.c（架构层）+ 编译器子层 GNU|GCC\{os_cpu.h, os_cpu_a.s}
│   + 架构层 Template\{os_cfg.h, os_cfg_app.h}
├── uC-CPU\ARM-Cortex-M\{ARMv6-M-RVM, ARMv7-M-RVM}\{cpu_rvm.c} + Template\{cpu_cfg.h, lib_cfg.h}
│   + 编译器子层 GNU|ARM|IAR\cpu.h（A7M 另有 GNU\cpu_a.s=CLZ/RBIT 加速，存在即编译）
├── uC-LIB\（官方头走 include path，.c 不编译）
└── uC-OS3\Test\{uo3_benchmark.c, uo3_main.c, chip\uo3_test_{stm32g071rb,stm32f429zi,stm32h743zi,ch32v307vc}_rvm.h}

RTThread_V411\
├── libcpu\arm\{armv6m_rvm, armv7m_rvm}\ 与 libcpu\risc-v\rv32p_rvm\
│   各含 {context_gcc.s, cpuport.c, Template\rtconfig.h}（armv7m 为 F429/H743 共用）
└── test\{rtt_board.c, rtt_benchmark.c, rtt_main.c, rtt_layout_knob.h,
         chip\rtt_test_{stm32g071rb,ch32v307vc,stm32f429zi,stm32h743zi}_rvm.h}

DjyOS_V110\
├── bsp\arch\arm\cortex-m\armv7e-m_rvm\{cpus.s, ints.s, int_hard.c, cpu.c, djy_support.c,
│   board.c（内核必需的板级胶水：SYS_Start 序列、虚拟定时器 tick、Ctx 处理器注册、
│            Board_Init/Sys_ModuleInit/__DjyInitTick，不含任何测试硬件）}
│   + include\（15 个自包含垫片：stdint/heap/list/shell/blackbox/errno/stat/dirent/…）
│   + template\（配置层：project_config、4 个 component_config_*、board-config）
└── test\{djy_benchmark.c, djy_board.c（仅 benchmark：TIM2 计数器/TIM4 中断源 + benchmark 引导）,
         djy_main.c（普通 main）, chip\djy_test_stm32h743zi_rvm.h（仅 benchmark）}
```

FRT 端口五件套：FreeRTOSConfig.h / portmacro.h / port.c / portasm.s（/ tasks_rvm.c 仅 A7M）。
**FreeRTOSConfig.h 在各端口目录的 `Template\` 子目录**（模板源，按架构三组各一份、组内跨编译器同哈希）；
生成器把它**拷贝进工作区** `<WS>/<Proc>/Include/FreeRTOSConfig.h`——Config 是用户工程资产，每工程
独立可改，重生成不覆盖已存在副本（rvp `Virtual_Header_Overwrite=Yes` 才强制刷新）；**Template 目录
绝不进工程 include path**（rmp_gen conf 头、RTT rtconfig.h 同款拷贝定式）。
IAR 与 GCC/Keil(RVDS) 的 C 件差异仅三处：portNOP→`__asm("nop")`、`__VFP_FP__` 守卫扩 `__ARMVFP__`、GCC 字面量池内联 asm→`LDR R3,=pxCurrentTCB`（IAR 内联汇编直接支持 =label）。
RTT 端口三件套：context_gcc.s（快路径 swt + interrupt 锁存版 + switch_to）/ cpuport.c（stack_init + PendSV_Handler + 快照式 disable/enable + ICI/IT 脏帧过滤）/ Template\rtconfig.h。协议与判例见 §9。

---

## 3. 标准工作流

### 3.1 生成

```sh
cd Project            # 本仓库根起
bash benchmark.sh gccmf-stm32h743zit6-frt.rvp   # 20-28s/链（benchmark 工程）
bash generate.sh  gccmf-stm32h743zit6-frt.rvp   # 普通工程（main.c = <guest>_main.c）
```

- 两个脚本都带齐 `-rmp/-frt/-uo3/-rtt/-djy` 的源码根；`generate.sh` 少任何一条都会在生成期报错（2026-10-29 前它还缺三家，普通工程根本生成不出来）。
- 改 rvp 或生成器后**必须删工作区再生成**（Project_Overwrite=No，旧 .ewp/.icf/Makefile 不刷新——白编两轮的教训）；改了**配置模板**（FreeRTOSConfig.h / rtconfig.h / 端口 template/）或**客机 main 源文件**（`<guest>_main.c` 与 `djy_benchmark.c`）同理：它们都被复制进工作区，旧副本不会刷新。
- 30s 命令上限内恰好一趟；**detached 并发多链会被拖超时**，逐链前台 + `timeout 28` 兜底。
- 生成日志确认 Virt1 源路径含 `portasm.s`（RV32P/A6M/A7M）、A7M 另有 `tasks_rvm.c`；RTT 链含 `context_gcc.s`。

### 3.2 GCC 构建（顺序固定：镜像先、Kernel 后）

Monitor → Proc1 → Proc2 → Virt1 → Kernel（Kernel 链接各子工程 `*_image.c`）。
**makefile 不是默认名**：裸 `make -C 目录` 必报 No targets；名字=子工程输出名：`kernel`/`monitor`/`prc_proc1`/`prc_proc2`/`prc_virt1`（如 `make -C Kernel/Project -f kernel -j8`）。被超时杀掉的 make 直接重跑（对象缓存续编）。
**rebuild 脚本必须全五链 clean**（Kernel 的 image .o 规则在 Monitor/Proc 目录，只 clean Virt1+Kernel 会断链）。

### 3.3 Keil 构建

`UV4.exe -b <dir>\Project\<name>.uvprojx -j0 -o <dir>\build.log`，读 log 尾判 0 Error。
Kernel 平坦 bin 手动 `fromelf --bincombined --output kernel_keil.bin Object\Kernel.axf`（**必须 --bincombined**，普通 --bin 多区段输出 1 字节废文件；文件名被句柄锁死时换名）。

### 3.4 IAR 构建

`common\bin\iarbuild.exe <工程>.ewp -build Debug`（无 GUI 无需 .ewd）；依赖序同 3.2，**Kernel 最后**。
镜像转换 `arm\bin\ielftool.exe --bin xxx.out xxx.bin`；postbuild 会自动跑 ielftool+bincopy 出 `*_image.c`（`&&` 须写 XML 实体 `&amp;&amp;`）。
**子镜像重编后 Kernel 必须重编**——内嵌 bin 会过期（判据：Kernel.bin 尺寸差几十字节，F429 复现过 230968→230884）。

### 3.5 烧录

STM32 主用：`STM32_Programmer_CLI -c port=SWD -w Kernel.bin 0x08000000 -v -rst`。
**多板环境 CubeProg 必须带 `sn=`**（否则连到枚举顺序第一块板；SN 用 `vcp_pair.ps1` 或既有脚本查）。
**Kernel.bin 偏移=flash 地址−0x08000000**（单平坦镜像内嵌全系统，别用子工程基址算）。
flash 读回：`-c port=SWD -r <addr> <size> <out>`，**输出必须 .bin 后缀**（.txt 报 Wrong file path）。
OpenOCD 备用（模板 Project\Script\）：tcl 要点 `set CPUTAPID 0`（新硅片 idcode 拒识，放 source target cfg **之前**）→ init → reset halt → flash write_image erase → verify → reset run；F429 新硅片 flash 前加 `reset_config srst_only srst_nogate connect_assert_srst`（H743 同款可用）。CH32V307 继承坑：多区段 hex 烧录会损坏，一律烧平坦 kernel.bin；OpenOCD 烧 H743 需 connect assert srst 或先手动开 DBGMCU（0x5C001004），否则 "Cannot identify STM32H7xx"。

### 3.6 串口捕获与判读

Start-Process/后台任务分离跑捕获脚本（窗口期内存活），**先开端口再烧录/复位**（DiscardInBuffer 别用——flash→capture 手动间隔会吃掉主段打印），事后读文件。波特率从内核平台头 BRR 反推。
**基准完成判据（铁律）**：①中断相关条目全部打印齐（FRT 链：ISR-Noti/ISR-Sem/ISR-Queue/ISR-RVM relay 四行；DjyOS 链是五条 ISR 行 + 六条线程行 + `Benchmark complete`，见 §10.5）——按硬件中断节拍慢出是**正常现象**（G071@TIM3 5kHz 需 90s+；H743 类高速芯片几乎紧随主体）；②终局标志=开始打印 `10000 overflows`。未齐前禁止结束捕获/判读/当挂死处理。反过来只见到 overflow 刷屏=旧固件空转输出，别急着怀疑新链——重烧+确认看到**自家横幅**为止。
捕获脚本本身有 ~20-40% 竞态失手率（COM 打开时机）——判"捕获失败"重试，别判"板崩"。
**旧数据回放陷阱（2026-10-29 实证）**：ST-Link VCP 在端口关闭期间仍会缓存字节，重新打开后先吐出上一轮的整段输出（本次三次不同镜像的捕获得到逐字节相同的 7358 字节段）。判据：捕获段里若出现**当前镜像不可能产生的字符串**（例如刚烧的空实现却出现上一版专有的标记字符），整段作废重测；标记字符类验证必须"复位→捕获窗口内先见自家横幅→再见标记"才算数。

### 3.7 会话收尾三清（必做）

1. **删工作区**：GCCMF-*/RVMDK-*/EWARM-* 全删（可由 rvp+benchmark.sh 复生）
2. **收生成物**：烧录/捕获脚本与原始日志收进 `Project\Script\`（git 已忽略）；可再生的 log.txt/report.txt 删
3. **结果入档**：benchmark 数据写进 `*_test_*_rvm.h` 头文件档案块（正式档案），AGENTS.md 只留索引
验收线：Project 根只允许 *.rvp 与仓库自带 sh；`git status --porcelain -- Project/` 必须空。
被句柄锁住的目录用 `cmd /c del /f /q /s` 强删（PowerShell Remove-Item 会弹"朗读提示不可用"失败）。

---

## 4. 排障方法论（按威力排序）

1. **断点裁决法**（启动类死机）：反汇编/nm 拿候选等待环与里程碑地址（VOS/HSE/PLL/Boot），各下硬件断点，resume 数秒后读 PC——命中即进度。注意 do-while 首过也碰断点，**命中≠卡死**；判卡死用"resume+sleep+halt 后读 PC"。
2. **写黑洞测试**（数据全垃圾类）：可疑地址写魔数读回，读不回/挂死=无 RAM。**硬件结论必须双仪器交叉验证**（CubeProg 与 OpenOCD 冲突时优先 ST 官方工具——H743"DTCM 黑洞"即 WCH fork 仪器假象被误诊，§14 判例 8）。
3. **Fault 现场活捉**：内核善后会清 PSP/CFSR——在 `UsageFault_Handler`/`_RME_Thd_Fatal` 入口下断点（第一次 fault），读 PSP 帧 8 字，PC/XPSR 即真相（PC=0/XPSR=0→INVSTATE，配 CFSR 互证）。Sftd 转储（串口白送）含 Vct/Usr 两组寄存器+栈帧前几字——A6M 上唯一现场；**判读要点：任何 UsageFault 的 LR 槽都自动是 EXC_RETURN（handler 入口压栈），无信息量；有信息量的是帧内 Stack-PC/Stack-XPSR/Stack-LR**。Sftd 的 SP 值+链接脚本落区对照是栈溢出排障零成本第一步——但先算对栈方向（满递减，栈顶=高地址；㉗ 曾把"距底剩 1768B"算成"剩 24B 要溢出"）。
4. **面包屑取证法**：可疑函数入口塞临时打印/计数——**注意取证代码本身改变布局与时序（"治好"效应），结论必须用无扰动手段复核**。UNALIGNED 类故障：HW 帧 LR 槽=BL 返回点、PC 槽=真故障指令。CFSR 按内核自己的头解码（别按网上直觉猜位号）。取证完清零残留。
5. **物证原则**："烧的是什么/编的是什么"用 flash 指令字比对 objdump；make 依赖**不含外部系统头**，改平台头后 `rm -rf Object` 强制全编再反汇编确认常量变了。**msys make 增量编译时间戳竞态**：改端口文件后 make 可能不重编（实验数据被污染）——改后必 clean 全编。**"偶发"故障先核对 bin 来源与源码状态**（旧脏 bin vs 新 bin 的行为差极易误诊为布局/概率问题；同尺寸 bin ≠ 同布局）。
6. **防超时三原则**：单命令<30s（构建拆单工程）；跨超时存活的用分离进程写文件；被杀的 openocd **会锁 ST-Link**（下次报 read version failed）→ 先 `Stop-Process -Name openocd -Force`；反复 CubeProg 连接可把 ST-Link USB 挂死（DEV_USB_COMM_ERR）→ 物理重插。
7. **栈深刷漆法**：paint cfg（halt+mww 填魔数）→ 跑基准 → check cfg（mrw 扫非 marker 边界）——VM 的 Vct/USR 栈实测深度一测便知（RME 内核不刷线程栈、Vct 区不在内核 bss 搬运范围，marker 复位后保留）。
8. **对账计数器法**（数据消失类）：给生产者/消费者/计数器各挂无声计数对账，差额直接指向丢失路径（RTT 邮件"消失"案即 Queue 19993 vs 发送 20000 定案）；探针只读不打印=零扰动。
9. **GDB 静态变量取证**：探针增删会使 .bss 符号跨构建漂移——**每次重编后必须重跑 nm 取当前地址**；DWARF `ptype /o` 实测结构体布局（勿背）。

---

## 5. 坑库

### 5.1 文件编辑类（三条同根：分段拼接产生错位）

1. **头注释收尾丢失**：`*/` 被裁 → `.syntax unified/.arch` 被吞进注释 → GAS 退回 divided 语法 → 满屏 `lo register required`。**指纹：见这批错先查汇编指示字是否在注释里**。
2. **悬空 `/*`**：插入点落在注释块中间 → 函数体吞成文件域 → `expected identifier before volatile`。
3. **armasm 宏缺 MEND**：尾部裁剪吃掉 → `A1291E: Macro definitions cannot be nested`。

**规矩：大文件 PowerShell 单引号 here-string 整写**（`[IO.File]::WriteAllText($p,$c.Replace("`n","`r`n"))`），写完验行数+抽查首尾。editor 分段插入只用于 <100 行小改。
**PS here-string 分段缝合坑**：上一段未以 `'@` 收尾就接 marker，下段 `Add @'` 被吞进字符串落进产物（症状：产物含 `Add @'` 行、相邻行粘连）——每段必须 `'@` 收尾再放 marker。
**汇编文件必须整文件成文或逐函数核对**（历史判例：分段把 `_RMP_A6M_Yield` 腰斩，DSB/BX LR 落在 END 后成死文本，首切即 HardFault 仅 2 字节输出）。

### 5.2 PowerShell/工具环境类

- **原生命令 stderr=错误记录**：openocd/make/gdb 的 stderr 首行中断管道。用 `*>&1 | Out-File` 或 Start-Process -RedirectStandard*（最稳）。
- **cmd /c 嵌套引号必失效**：含 `>` 的长命令经 PS→cmd 丢输出。拆进 .tcl/.bat 或 Start-Process 参数数组。
- **msys 抢 PATH**：装了 msys/Git Bash 后裸 `cmd` 可能解析到其 cmd.exe——需要真系统 cmd 时用绝对路径调用。
- **-creplace 反向引用数字粘联**：`'$1'+'0x14000'` 解析为组 10 → 用 `'${1}0x14000'`；-replace 不分大小写，必须 -creplace。
- **Remove-Item 对被锁文件弹"朗读提示不可用"失败** → `cmd /c del /f /q`。
- **Get-Content 数组负索引回绕**（尾+头拼接酷似文件翻倍）——先看总行数再下结论。
- **run_commands JSON 吃一层反斜杠**：含 `\\` 的 Replace 用 `[char]92` 拼 needle；大文件内容一律走 editor 工具不走 PS。
- **批量文本替换两坑**：①删除规则必须白名单式列名或 `^#define` 行首锚——前缀式（如 `^#define CPU_INT_`）会撞车误杀 CPU_INT_DIS/EN 宏（链接期 undefined reference 才暴露）；裸匹配宏名会误伤调用处（把调用行改成 define）。②双引号串里反引号转义 `` `$1 `` 会把字面 `` ` `` 写进产物（stray '`' 编译错）。
- **COM 口发现**：`Get-CimInstance Win32_PnPEntity | ? Name -match 'COM\d'`（随插拔变，勿写死）。
- **Compare-Object 不带位置信息**——"哪一行变了"必须用带行号的逐行比对。
- **PowerShell 类型化数组参数（`[string[]]$x`）触发转换复制**，函数内改动不影响外部——参数不标类型即传引用。
- **正则中标识符的 `+`（如 M0+）是量词**——标识符进正则一律转义或 `Regex::Escape`。
- **PowerShell 5.1 读无 BOM 的 .ps1 按 ANSI**——脚本内中文常量乱码、中文锚点匹配静默失败；含中文的脚本必须存带 BOM 或用 `[char]` 码点构造。
- **输出通道偶发"剪切板污染"（乱码混入命令/输出）**——重要结论须用脚本文件方式复核，不信任单次内联输出。
- **grep 引号转义会被 shell 静默吞掉**（曾致 `__TIME__` 漏检、stderr 有 `command not found` 却没看见）——管道里 grep 无输出时查 stderr。

### 5.3 生成器/构建类

- **【头条】M7M01 平台头只是模板**：生成器向工作区 `Kernel\Include\rme_platform_*.h` 注入 ~17 个布局常量（RME_KOM_VA_BASE/RME_GEN_ENABLE 等），源头文件是 GEN_ENABLE=0+占位地址。改内核平台头：源头改**非布局代码** → 删工作区重新生成。**绝不能把源头文件手工拷进工作区**（布局常量被抹成模板值 → 内核对象建错地址 → init panic、转储全垃圾指针）。判别：panic 转储 R4 恒定垃圾值 → 先查 GEN_ENABLE 是否被抹。
- **工作区 Include 是生成器拷贝**：改源头平台头后已生成工作区不自动更新（同上解法）。
- **Kernel 链接依赖各 VM 的 `*_image.c`**：报 `No rule to make target 'virt1_image.o'` 即构建顺序错了。
- **生成器源里列了不存在的 .c** → make `No rule to make target`（实证：V3.08 无 os_int.c，OSIntEnter/Exit 在 os_core.c）。
- V3.08 uCOS API 签名（与老版本假设不同）：OSTaskCreate **13 参**（多 q_size/time_quanta/p_ext）、OSSchedRoundRobinCfg(en,quanta,&err) **en 在前**、OSSemCreate 4 参无 max、RoundRobinYield 带 &err。
- **断线旋钮识别**：改了某宏重编 bin 尺寸纹丝不动=该宏可能根本不进镜像（如 BENCH_TASK_STK_SIZE 是 rt_thread_create 的运行期参数→堆分配，不进 bin；RT_THREAD_IDLE_STACK_SIZE 在 rtconfig 里是断线旋钮、idle.c 只认 IDLE_THREAD_STACK_SIZE）——扫描前先确认旋钮真的连着（bin 尺寸随值变化）。

### 5.4 iasmarm 汇编类（与 armasm/GAS 差异速查，全是实证）

**宏语法（最大坑）**：
- 原型 **`name MACRO 参数1, 参数2`**（名字与 MACRO 同行、标号字段），结尾 **ENDM**（非 MEND）。armasm 三件套（MACRO 换行写名 / `$param` / 带缩进）全报 Error[35] No name on macro——别再试。
- 正文引用参数=**裸文本 token 替换（无 $ 前缀）**：`LDR R1,=EXCXPSR`。
- **参数名禁用裸寄存器名**：参数叫 `LR` 则 `MOV LR,R1` 的 LR 被替换成立即数！用独名（现定稿 LABEL/SZ/XPSR/EXC_RETURN，宏定义行=宏名补齐 16 列+MACRO+3 空格→参数列 col 25）。
- 跳转标签当普通参数传（`ALIGN_PUSH MACRO skipTo … BEQ skipTo`）；宏内 `IF 表达式/ENDIF` 条件展开可用。

**字面量与指令**：
- ★★ **`LDR Rx,=代码段标号` 字面量自动 +1（Thumb 位）**（armcc/gcc 不加，唯独 IAR 加）。当 BX/BLX 跳转目标恰好正确；**当段内数据表指针用=奇地址非对齐 fault**（实证：Cop_Clear 字面量 0x08006239 vs 符号 0x08006238，UNALIGNED→HardFault）。**军规：=标号只准用于跳转目标或数值字面量；数据指针用途必须加载后 `BIC Rx,Rx,#0x01`**。`ADR` 对 THUMB 标号直接 Error[559]（反证其行为）；`DATA` 指令可解但引 Lt056 警告（破坏 0 警标准）。注意：RMP 客户 asm 里伪造异常帧 PC 的 `_RMP_A7M_Skip` 奇字面量是**故意加位，勿"修"**。
- 无 `DCI`（armasm 专属）——CODE 段手工编码指令用 `DC16`（触发 Warning 410，压法见 5.5）；**禁止改成真指令**（无 FPU 配置下汇编会挂——原作者手工编码的原因）。
- 段名**不许含内部点**：`.text.rme_vector` 报 Error[116] → 单点段名 `.rme_vector` 等，icf 选择器同步。
- `sfe(CSTACK)` 需模块内先 `SECTION CSTACK:DATA:NOROOT(3)` 声明（IAR 官方启动模板同款），否则 Invalid syntax；空段 GC 无副作用。
- **双操作数移位=Warning[408]**：`LSLS R4,#16` 必须写三操作数 `LSLS R4,R4,#16`（AND/ORR/SUB 同理写三操作数形式）。
- `MOV LR,#0xFFFFFFFD` 会被自动优化为 `MVN.W LR,#2`（编码等价，正常）。
- `MSR XPSR,R0` 被 iasmarm 映射为 XPSR_nzcvq 编码（非特权语义等价——APSR 可写、ICI/IT 不可写本就要过滤）。
- 无 PROC/ENDP；导出 PUBLIC、导入 EXTERN、段 SECTION；文件尾显式 LTORG。
- **单文件快检法**：`iasmarm --cpu=Cortex-M0+ <file> -o t.o`（M4: --cpu=Cortex-M4 --fpu=VFPv4；M7: --cpu=Cortex-M7.fp.dp）——进 iarbuild 前先过这关省整链时间。

### 5.5 IAR 工程与工具类

**.ewp 生成要点**（模板=M5P01 已验证工程，选项逐项直写不用 helper——本仓房规）：
- 结构 XML fileVersion 4；设置块版本必须自洽（实证 G071：General v36/ICCARM v38/AARM v12/ILINK v27）——版本号与选项名配套，错配报 unknown name 加载错误。
- CoreVariant 表：34=M0、35=M0+、36=M3、39=M4、41=M7；M0+ 全 FPU2/NrRegs/DSP=0；M4/M7 FPU2=4/6/7 对应 FPV4_SP/FPV5_SP/FPV5_DP（NrRegs=1、DSP=1）；GBECoreSlave/GFPUCoreSlave2/OG_32_64DeviceCoreSlave 与 CoreVariant 同值。**内核/监视器工程 FPU2=0 是正确设计**（内核 FPU 干净，FPU 上下文全靠 DC16 手编；只有 Virt1 按 rvp 协处理器拿 FPU 配置）。
- 器件串两处（OGChipSelectEditMenu+GFPUDeviceSlave），如 `STM32G071RB\tST STM32G071RB`。
- `$TOOLKIT_DIR$ 本身=<安装>\arm`：写 `$TOOLKIT_DIR$\arm\bin` 会得 arm\arm\bin → postbuild 找不到。
- **IAR IDE 保存会洗 .ewp**（注释头消失、压警表被清）→ **生成器是 .ewp 唯一真源，改配置一律改生成器源码再重生成**。
- 压警终策：CCDiagSuppress=**仅 Pa082**（volatile 多访问顺序，内核风格刚需）；Pa078/Pa079=X=X= 双赋值笔误病根，一律源头修不压；AARM Warning 410（DC16 in CODE）压法=`AWarnEnable=1/AWarnWhat=1/AWarnOne=410`（数据块版本≥12）；iasmarm 的 `-w` **只能出现一次**（AExtraOptionsV2 传 -w-410 与自动 -w+ 冲突）。
- icf：自定义必须 `IlinkIcfOverride=1`（否则**静默**用默认 icf）；`place at address + keep`（向量/入口/desc/镜像全 keep 防 GC）；镜像 C 数组落 .rodata → `readonly section .rodata object xxx_image.o`；CSTACK 空 block `define block CSTACK with alignment=8, size=值 { }` + place at address。
- HAL 宏陷阱（仅 HAL 用户，RVM 链纯寄存器级免疫）：G0 要求 -DSTM32G071xx 手写 CCDefines；HSE_VALUE 不带引号。
- 输出布局：`Debug\Output\`（.o 在 IAR 原生 <组名>_<哈希>.dir 子目录，无法展平）+ `Debug\Listings\`。
- gitignore：忽略 settings//*.ewt/*.ewd/*.bak/*.pbi 等；保留 *.eww/*.ewp。

**工具细则**：
- ielfdumparm **无 --text 选项** → `--code --range 0xA-0xB -o out.txt file.out`；从段中途起反汇编必对不齐（Ex008），按 $t/$d 边界或已知标号解。
- **IAR DLib cstartup 在进程里自动使能 FPU**（工程带 FPU 配置时启动序列含 __iar_init_vfp 写 CPACR）——RVM 进程非特权写 CPACR 必炸（precise BusFault）。修法=平台 asm 接管：`PUBLIC __iar_program_start` 自定义为直接 `BL __cmain`（跳过 vfp init；`BL __iar_init_core` 报 Li005 不可外链）。rme/rvm 两个 a7m iccarm asm 都改；**a6m 无 FPU 不需要**。FPU 使能归属=内核 __RME_Thd_Cop_Swap 按线程属性做（与上游一致）。
- **CPACR 写后补屏障**：`__RME_Thd_Cop_Swap` 的 CPACR|=FPU_MASK 后加 `__RME_A7M_Barrier()`（现成 DSB+ISB，勿自造）——ARM 要求 CPACR 变更同步后才能执行协处理器指令。

### 5.6 OpenOCD (MounRiver fork) 类

- 回显裁剪（reg/mdw/poll/mww 无回显）→ `mem2array a 32 <addr> 1; echo "$a(0)"`；对某些地址（0x5C001004）失败中断脚本 → `catch {...}` 包裹。
- 新硅片 idcode 拒识（F429 报 expected 0x2BA01477 实测 0x6BA02477；H743 DPID 0x6BA02477）→ `set CPUTAPID 0` 在 source target cfg 之前。
- 运行态设 flash 断点必败（无报错直接死）→ 先 reset halt/halt。
- gdb 连接（3333 起 server）在 ARM 板实测协商异常连不通，弃用；RV32P（V307）GDB 可用（openocd 3333 + riscv-none-elf-gdb + kernel.elf 符号）。
- H743 特有：flash 编程 probe 报 "Cannot identify STM32H7xx"=DBGMCU 时钟未开——需 connect assert srst 或先 mdw 开 DBGMCU_CR（0x5C001004）；**DWT 值匹配 watchpoint 不支持**（resource not available）；FPB flash 断点 resume 后可不命中。
- **CubeProg `-coreReg` 会把核按在复位向量**（PC 恒 __RME_Entry 首指令）——不能采样运行态 PC；RAM 读（-r32）安全。
- 命令行传带空格参数给 OpenOCD 会被拆分——复杂命令一律走 cfg 文件；路径必须 `C:/` 风格（不识别 msys `/c/`）。

### 5.7 硬件与芯片头类

**H743 档案（NUCLEO-H743ZI，rev.Y）**：
- **VOS 复盘（定稿）**：正确编码 D3CR VOS[15:14]=11:Scale1(0xC000)，复位值 0x6000=Scale3。原宏 0xC000 从头到尾是对的。**真 bug=缺供电配置**：官方顺序=ConfigSupply(写 CR3：清 SCUEN/BYPASS 置 LDOEN，等 CSR1.ACTVOSRDY)→VOS 配置→等 VOSRDY；缺第一步则 VOSRDY 永不置位启动死等。已修 M7M01 头。教训：**拿不准 ST 外设用法先抄 M5P01 同芯片工程+M0A00_Library 的 Cube HAL——同板跑通的代码是唯一真理**；凭记忆不查库连错两轮。
- DTCM 128KB 完整可用（曾误诊"高 64KB 写黑洞"——OpenOCD WCH fork 仪器假象，CubeProg 实测推翻；防御性布局保留无害）。教训：写黑洞类硬件结论必须双仪器交叉验证。
- HSE=8MHz 来自 ST-Link MCO（HSEBYP 直注）；400MHz=8/4*400/2，APB1/APB2=100MHz，APB1 timer=200MHz（测试头 TIM2_CNT<<1 基于此）。
- rev.Y 调试口怪象：DPID 0x6BA02477；DBGMCU IDCODE 经 WCH fork 读出 0x70003F（非标准，未解，仅影响调试通道）。
- **M7 性能数据必须开 L1 Cache 才有意义**（关 Cache Yield 1082/995 → 开 228/250，4~5 倍——无 Cache 跑 Flash 每取指吃 AXI 等待）。cache 开关在 `rme_platform_stm32h743zi.h` 的 `__RME_A7M_Cache_Init()`（改它=内核平台头改动→删工作区重新生成）。

**芯片头寄存器坑（FLASH ACR 三坑 + 读回断言）**：
- **语句宏当值用=Pa078 病根**：`ACR=LATENCY(2U)|PRFTEN` 而 LATENCY(X) 是赋值语句宏 → 展开成双重赋值 UB。修法：语句宏只当语句调用，禁止套进表达式。
- **位域抄错芯片**：G0 FLASH_ACR 只有 LATENCY[1:0]（~0x03；~0x07 是 F1 残留）；G0 无 PRFTEN（bit8 是 F4 残留、写保留位违规）——PRFTH 钩子不能删，宏定义改 (0U) 自然 no-op。
- **LATENCY 写后必须读回断言**：`RME_ASSERT((ACR&0x03U)==2U);` 紧跟写入（A7M 四头全有，G071/L071 已补）。HAL 语义：FLASH_LATENCY_2=2 等待态=域值 2。
- **X=X= 双赋值笔误是本仓库高发模式**（G071 FLASH ACR、rme_platform_a7m.c:1237 LR_GET 行两例）——见 Pa078/Pa079 一律源头修。
- IAR DLib 进程入口链：flash 里 desc 后 `00 48 00 47`+字面量（LDR R0,[PC];BX R0→__iar_program_start），cstartup 在进程内照常完成 data/bss 初始化。

---

## 6. RTOS 移植契约理论（四 OS 88 端口普查 + 多轮 bug 修复沉淀）

> 本章是跨 OS 的理论总纲：剥离平台后的 OS 本体契约、切换原子性范式、RVM 中断安全模型、
> 四 OS on RVM 的同构模板与移植准则。行号均为实证可回查。

### 6.1 四 OS 本体契约（内核源码固有属性，与平台无关）

| 本体属性 | FreeRTOS V9 | uC/OS-III V3.08 | RMP | RT-Thread V4.1.1 |
|---|---|---|---|---|
| 切换请求的合法状态 | **任意**（含临界区嵌套内，tasks.c L4212 官方注释自白） | **仅开中断态**（Pend API 一律 L467 EXIT→L469 OSSched） | 仅锁外（锁内只 Pend） | **任意**（rt_schedule 在中断掩蔽临界区内直调 rt_hw_context_switch；sched_lock_nest>0 时内核侧抑制切换不发起） |
| 数据操作与切换耦合 | 耦合（临界区内 yield：L4199→L4216/L1730） | 解耦（先退再请） | 解耦（锁内 Pend、Unlock 归零统一切） | 耦合（选择+预更新 current+调用端口同在一临界区，scheduler.c L466 附近） |
| 调度决策位置 | 切换内（vTaskSwitchContext） | **切换外**（OSSched 先选，OSCtxSw 纯执行——官方仅 pend） | 切换内（_RMP_Run_High L830） | **切换外**（rt_schedule 先选 from/to 传参给端口；且 **rt_current_thread 预更新为 to** 后才调端口——四 OS 唯一） |
| 切换请求点 | 分散（7+ 处任务级+ISR 级） | **封闭**（任务级唯一 OSSched L482；中断级唯一 OSIntExit，自重选 L342-344） | 统一（一个宏+一个标志） | **封闭**（任务级/中断级都只在 rt_schedule 一处，函数内按中断嵌套分流到 rt_hw_context_switch / _interrupt） |
| 锁与中断 | 解耦（SuspendAll 不掩中断） | 耦合（SchedLock 在临界区内计数） | **合一**（Lock=掩蔽+计数，L752-756） | **合一**（rt_enter_critical=中断掩蔽+sched_lock_nest 计数；锁内抑制调度） |
| deferred 责任 | **推给端口** | 内核结构消灭 | 内核内建（Pend 二态 L1769-1781） | **推给端口**（官方端口全靠 PendSV/软中断的硬件性延迟；内核不做端口协作式 pend） |
| 端口所需中断原语 | 两对（无条件+计数临界区） | 两对（无条件+快照 save/restore） | **一对**（掩蔽） | **一对**（disable/enable 快照式） |
| 官方端口形态（§6.6 普查） | ~12% 直切 | ~54% 直切 | ~85% 直切 | **0% 直切（14/14 全 pend 型）** |

身份一句话：FRT *"我可能在任何时候请你切，请务必万能"*；UO3 *"我只在干净状态、选好下一任后
才叫你——你只管换别动脑"*；RMP *"锁就是我的全部——给我一个掩蔽原语和一处让出点"*；
RTT *"我在临界区里选好下一任、名字也改好，然后叫你——你怎么切我不管，官方端口靠 PendSV
等到开中断才真切"*。
**核心问题：谁为"切换请求的时机安全性"负责——FRT 答端口、UO3 答内核结构、RMP 答内核机制、
RTT 答端口但给了闭式请求点+锁存协议当拐杖；RVM 适配成本 21 : 1 : 0 : 4 正是这一回答方式的度量**
（RTT 居中：决策在外免 peek、请求点封闭好锁存，但临界区内直调仍要端口自配 pend——四板的
RTT 端口=锁存+flag+PendSV 双守卫+脏帧过滤，恰好是 UO3 执行机构+FRT 延迟策略的杂交体）。

**DjyOS（第五家，2026-10-30 定档）**：*"我在关异步信号的临界区里选好下一任，然后叫你当场换——
别拖到开中断"*。与 RMP 同族：决策在切换内（`__DJY_SelectEventToRun` 在 `__DJY_Schedule` /
`__DJY_ScheduleAsynSignal` 内），请求点收敛于这两处，端口免 peek；但"异步信号开关"是内核自己的
**计数式**虚拟掩蔽，切换必须在 Unmask 之前当场完成（掩码下 Yield 只抬旗不切换）。唯一要 defer 的
是需要真异常返回才能恢复的脏帧（ICI/IT）——而 turnto/首切这类永不返回的切入**绝不能** defer（§10.6 ⑮）。

### 6.2 切换原子性三范式（直切端口如何补偿无硬件原子性）

| 范式 | 机制 | 实证 |
|---|---|---|
| ① **帧存中断状态** | I 位物理住在状态寄存器，压栈即存、硬件 POP 即还 | UO3：MicroBlaze/AVR/Cortex-R；FRT：MSP430/MicroBlaze |
| ② **临界区包裹调用** | 调用者显式 ENTER_CRITICAL 包住 yield | FRT MicroBlaze port.c L277 |
| ③ **全局真值收敛** | 掩蔽是软件全局标志→切换全程掩蔽+出口归零+恢复点控制流闭环 | RMP 全系直切 asm、UO3-RVM、FRT-RVM、RTT-RVM——RVM 家族全体 |

**规律：中断状态的载体决定范式**——CPU 状态字→①；调用链可控→②；内存标志（虚拟化）→③。
RVM 直切端口被迫全落③：恢复一致性由"每个被切走任务的恢复路径必经一次无条件 EN"承担。

### 6.3 RVM 掩蔽三层 gate 与中断安全模型

- **三层 gate**（全部只认 `RVM_Int_Mask`，对物理 CPSID/BASEPRI 无感）：发送侧（Vct_Trig：mask=1 只置标志）/取出侧（Vct_Get：mask=1 跳过 MASKABLE）/执行侧（Vct_Loop：mask=1 时 tick/ctx 搁置并置 Vct_Pend）。标志**搁置不丢失**，Unmask 补投；透明向量穿透=设计特性。
- **抢占方向**：Vct prio 3 > Usr prio 2——mask=0 的每条指令边界都是 ISR 插入点。
- **帧模型**：被抢占上下文存于 RVM_REG（VM 级寄存器块），Ctx 处理器转存任务栈——与快路径伪装帧、StkInit 初始帧构成**三生产者互换**。内核侧所有线程切换发生在一次 SVC 内，`POP {R4-R11,PC}` 弹出目标线程保存的 EXC_RETURN 完成真实异常返回；`RME_A7M_EXC_RET_FIX` 强制 Thread/PSP 位但**保留 bit4 帧类型**（无错配）。
- Virt1 类纯整数客机**零浮点指令**→全基本帧→FPU/惰性压栈路径整体死代码（FPCCR 全库未配置——记录在案的潜在隐患，浮点客机启用时需评审 LSPEN/ASPEN 语义）。

### 6.4 四 OS on RVM 契约重审结论（同构模板）

四家的 RVM yield 收敛到**同一模板**（RMP 为原作，UO3/FRT/RTT 为同模板移植）：
```
入口 Mask → peek/直取（帧脏过滤）→
   快路径：asm 直切（全程掩蔽 · 出口 Unmask）
   慢路径：Yield(pend) → Unmask(arm)（超调用 · ctx 端点切换处理器重选/自愈）
   无切换：Unmask
```
- 差异仅在 peek 角色（UO3/RTT 无——调度在外，切换请求自带 from/to；FRT/RMP 有——优化预判）
  与慢路径自愈依据（FRT 切换内重选最强 > RMP ctx 内 Run_High ≈ UO3/RTT HighRdy/预更新自愈——
  RTT 的自愈依赖"切回路径必经 rt_schedule 重选+官方 LIFO 链序"，内核升级需复验）。
- 慢路径窗口是帧脏过滤架构的固有代价（ICI/IT 必须真异常返回→必须超调用→必须先 pend 后 arm），
  四家皆有、均近乎死代码路径、纸面自愈成立。

### 6.5 移植五准则（最安全假设清单）

1. **内核当不可信调用者**：切换入口自足（入口掩蔽/软件 pend），除非逐行验证过内核契约
   （FRT 21 文件返工 vs UO3 1 处 Mask 的根源；RTT 折中：闭式请求点+锁存协议让入口自足
   收敛为端口三件套，但 fall-through/残留块判例证明"结构对了还要逐指令对"）。
2. **域验证先于语义假设**："关中断"假设的安全性=映射触碰的域。
3. **参考代码默认全错**：跨架构/编译器抄码逐字核上下文（入口序/跳转极性/符号链接/调用约定——"N words pushed" 是可执行规格）。
4. **载体变化驱动范式切换**：状态字在 CPU→范式①；在内存→范式③（照抄裸机直切=竞态）。
5. **证明优于测试**：竞态正确性要控制流级证明（收敛闭环），板测只证伪——"没炸≠正确"。

### 6.6 端口形态普查（四 OS 88 端口实证）

| 系统 | trap/pend 型 | 直切型 | 直切占比 |
|---|---|---|---|
| FreeRTOS（34） | ~28（CM 系 pend PendSV、A 系 SWI、PPC SC、RL78 BRK、TriCore syscall…） | 4（ATMega323 naked/MSP430F449 压 SR/MicroBlaze 临界区包裹/IA32 call） | ~12% |
| uC/OS-III（26） | 10（ARMv7-M PendSV/RISC-V MSIP/MIPS syscall/C28x TRAP#16/PPC405 sc…） | 14（MicroBlaze/PowerPC/AVR/Cortex-R/MSP430X/NiosII/ARC/PIC24/POSIX/Win32…） | ~54% |
| RMP（13 平台） | CM 系（A7M 写 PendSV 即返，L146-152） | 11（AVR/MSP430/RL78/C28X/DSPIC/M6502/MP32P/UNSP/X86L/RV32P/LINT 桩） | ~85% |
| RT-Thread V4.1.1（14） | **14**（CM×6 PendSV、RV×4 软中断 msip、arm926/armv6 SWI、cortex-a/cortex-r4 SVC/TRAP） | **0** | **0%** |

**三定律**：①形态由硬件能力定（有 PendSV→pend；有 trap 指令→trap；都没有→直切）；
②直切补偿由载体定（状态字→帧存；软件标志→全局收敛）；③直切比例由契约宽窄定
（FRT 宽契约 12% 直切且每个自带补偿 ←→ UO3 窄契约 54% 一行注释即够 ←→ RMP 85% 原生③范式
←→ RTT 宽契约+闭式请求点 **0% 直切**——官方全部押注中断式延迟，deferred 责任整体交给硬件；
我们的 RTT-RVM 直切端口因此是全家族唯一偏离官方形态的选择，ICI/IT 脏帧过滤是它的必要代价）。

### 6.7 四类内核契约与对应的 RVM 策略（心智负担由契约宽度决定）

1. 内核保证 Yield 时候中断一定关闭，而且允许 port 进行实质性 Yield；Yield 完毕后必须还原 Yield 前中断状态：对此系统（uC/OS III）的 RVM 策略可以是：快路径在 Yield 调用点进行，原则上需要保存和恢复各任务在切换点的中断状态（实际读过源码可确认不需还原，开中断即可，因为内核总在 Yield 后立刻开中断且开中断幂等：Disable; Yield; Enable）。对快路径要求最宽松，心智负担最大。
2. 内核对 Yield 不做任何中断状态保证，也不关心 port 是否实质性 Yield。对此系统（FreeRTOS）的 RVM 策略可以是：快路径推迟到开中断时进行，而且起手需关中断（不知调用时中断关没关），心智负担中等。
3. 内核在关中断状态的 Yield 用 Pend 标志代替，待中断使能时检查该标志并决定是否 Yield——内核已帮 port 推迟了所有 Yield。对此系统（RMP）的 RVM 策略可以是：快路径直接进行 Yield，起手原则上需关中断，心智负担最小。
4. 内核在临界区内（关中断）完成选择并**预更新 current**，随即直接调用 port；内核不推迟也不管 port 是否实质性 Yield，官方端口一律 pend（PendSV/软中断），切换实际发生在开中断瞬间。对此系统（RT-Thread）的 RVM 策略可以是：快路径起手关中断立即切（调度已在外完成、from/to 传参，天然免 peek），慢路径 pend-then-arm；锁存 from/to+flag 协议加 PendSV 双守卫用来消化"锁存跨推迟窗过期"（官方 PendSV 尾链即时分发的假设在 RVM 延迟分发下不成立），心智负担中等——与 FreeRTOS 同档，但闭式请求点（唯一 rt_schedule）使入口自足收敛为端口三件套。

我们的移植策略：不管他是什么要求，一律到开中断时进行 Yield，而且总是起手关中断。

### 6.8 客机空闲停车契约（hook 在应用 main，端口只给机制）

- **契约**：RVM 手册 §4.4.3 要求客机空闲线程**反复调用** `RVM_Hyp_Vct_Wait()`（超调用 9）——把 Usr 线程挂起并从运行队列摘除，下一个虚拟中断到来时由 Vct 线程唤醒。不调用则客机全速空转（Vct 线程只在向量/定时器到达时被调度），多 VM 与低功耗都不成立。
- **放置（2026-10-29 定案）**：hook 本体写在**各客机的非 benchmark main**（生成器复制成 `main.c` 的那个文件），端口只提供机制。理由：idle hook 在四个 OS 里本来就是应用契约（FRT `vApplicationIdleHook` / UO3 `OS_AppIdleTaskHookPtr` / RTT `rt_thread_idle_sethook`），端口应保持"只是这家 OS 的官方端口"。`RVM_Hyp_Vct_Wait` 对内核是**透明挂起**（恢复点就在调用处，不进内核阻塞态、不碰任何内核数据结构），因此不违反各家 "idle hook 不得阻塞" 的约束。
- **四家机制**：FRT——`vApplicationIdleHook` 调 `RVM_Hyp_Vct_Wait()`，模板 `configUSE_IDLE_HOOK 1`，`prvIdleTask` 负责调用；UO3——`OS_AppIdleTaskHookPtr = AppIdleHook`（端口 `OSIdleTaskHook` 转发）；RTT——`rt_thread_idle_sethook(App_IdleHook)`，模板加 `RT_USING_HOOK`/`RT_USING_IDLE_HOOK`（`idle.c` 只在这两个开关下编译 hook 列表与 sethook）；DJY——`djy_set_idle_hook(djy_idle_wait)`，由 `CleanWakeupEvent` 回调（调用点的掩码约束见 §10.9）。
- **benchmark 一律保持忙等**：`configUSE_IDLE_HOOK` 是内核级宏、普通与 benchmark 共用同一份模板，所以 `frt_benchmark.c` 必须定义**空**的 hook（否则链接失败，且停车延迟会混进周期测量）；UO3/RTT/DJY 的 benchmark main 不注册即天然忙等。
- **非 benchmark 生成链（同日修复）**：`generate.sh` 补齐 `-uo3/-rtt/-djy`；命令行解析由 if 级联改为 if/else-if 链（旧级联在最后一项不是 `-b` 时读 `argv[argc]`=NULL → strcmp 必崩，普通工程从来就生成不出来）；DjyOS 的板级胶水一分为二——内核必需的（SYS_Start 序列、tick、Ctx 注册、`Board_Init`/`Sys_ModuleInit`/`__DjyInitTick`）进端口 `board.c`，**测试硬件（TIM2 计数器、TIM4 中断源 + benchmark 引导）留在 `test/djy_board.c`，只随 benchmark 编**：普通工程源码表里一个 `test/` 文件都没有、也不含测试头，自然不需要任何 stub；idle 动作统一由应用注册（普通 main 注册停车，benchmark 在 `djy_bench_boot` 里注册 driver pulse）。
- **⚠ 普通工程里 guest 不跑是正常形态（2026-10-30 实测，用户裁定）**：`generate.sh` 生成的 Proc1/Proc2 线程是空模板 `while(1);`，优先级在 VM 之上，会一直占着 CPU——所以普通镜像里看不到任何 guest 输出、PC 采样总落在 Proc1，**不是缺陷**，把 proc 线程填成会阻塞/返回的实现（benchmark 工程由 `Test/rvm_benchmark.h` 替换 proc 线程体，所以它的 guest 能跑）后 guest 才上电。因此 §6.8 的四个 hook 目前是**静态证据 + benchmark 链真机回归**；若要在普通工程上做停车验证，需临时把四个 proc 线程体改成空实现（验证完即弃，别改生成器）。

---

## 7. FreeRTOS 端口设计决策（A7M fast yield / IAR 改编 / 格式与双工具链）

### 7.1 A7M_RVM fast yield（改 port 前必读）

**为什么必须 ICI/IT 检测**：ARMv7-M EPSR 携带 IT 块/可续传 LDM/STM 状态（掩码 0x0600FC00），三方不对称——硬件异常入口/出口自动保存恢复；线程态软件 MRS EPSR 读 0、MSR 写忽略。fast yield 手工伪造/恢复异常帧，脏帧（xPSR 带 ICI/IT）**只能经真实异常返回恢复**（慢路径），否则静默丢状态。A6M 无 IT/ICI 无此约束。

**三层架构（零内核文件改动）**：
```
port.c vPortYield():
  mask → xTaskPeekNextToRun()【tasks_rvm.c 提供】
   ├─ peek==当前 → unmask, return（零切换）
   ├─ 目标帧脏(LR_EXC.4 选偏移, XPSR&mask) → unmask + RVM_Virt_Yield() 慢路径
   └─ 干净 → vPortYield_Swt【portasm.s】(真 vTaskSwitchContext 必选同任务——mask 冻结调度状态;
      EXC_POP 内 0x0600FC00 检测→vPortYield_Err 纵深断言)
```

**tasks_rvm.c**：编译它**取代** tasks.c（不得同时入链）；`#include "../../../tasks.c"` 暴露文件内静态量，内核源零改动。peek=V9 通用选择的只读镜像（不推进 pxIndex、不写 pxCurrentTCB）。**真 bug 判例**：V9 `listGET_OWNER_OF_NEXT_ENTRY` 有 end-marker 绕回检查——pxIndex 前进落在 xListEnd 时需再进一步（end 的 pvOwner 是 padding=0），漏抄则 peek 返回 NULL→解引用 0 炸（H743 首测暴露）：
```c
pxNextItem = pxList->pxIndex->pxNext;
if( pxNextItem == (const ListItem_t*)&(pxList->xListEnd) )
    pxNextItem = pxNextItem->pxNext;
return (TaskHandle_t)(pxNextItem->pvOwner);
```
约束：configUSE_PORT_OPTIMISED_TASK_SELECTION 必须 0（Config 已锁）；调度器挂起期禁 yield；peek 须在虚拟中断屏蔽下调。

**关键约定速查**：
- 帧字偏移（自 pxTopOfStack=TCB 偏移 0）：超调用参数 5 字 / R4-R11 8 字 / **LR_EXC@13** / 基本帧 XPSR@**21** / 扩展帧（S16-31 插 16 字）XPSR@**37**。pxPortInitialiseStack、xPortPendSVHandler(C)、portasm.s(ASM) 三处布局必须一致。
- `configUSE_RVM_OPTIMISED_YIELD`（Config 裸 #define）：1→portYIELD()=vPortYield()；0→RVM_Virt_Yield()。OFF 态全被链接器 GC（GCC 实测 -804B）。
- asm 的 `.short`/DC16 半字对是 VFP 指令（VSTMIA/VPUSH 等），**勿"改漂亮"**。六对手编码：ECA0 0A10/EEF1 1A10/ED2D 8A10/ECBD 8A10/EEE1 1A10/ED30 0A10。
- 假帧 PC=vPortYield_Exit，LR=wrapper 返回地址——中断路径恢复该帧时经 Exit→bx lr 无缝续跑；EXC_PUSH 的 0x200（XPSR bit9）是对齐填充私有标记。

### 7.2 IAR 端口改编定式（从 Keil/GCC 版翻译）

| 件 | 改法 |
|---|---|
| FreeRTOSConfig.h | 在 Template\ 子目录（生成器拷入工作区 Include，见 §2.4；__ICCARM__ 分支已在） |
| portmacro.h | 仅 portNOP→`__asm("nop")` |
| port.c | ①`__VFP_FP__` 守卫扩 `|| defined(__ARMVFP__)` ②GCC 手工字面量池内联 asm→`LDR R3,=pxCurrentTCB`（IAR 内联汇编支持 =label）③dsb/isb 去 volatile |
| portasm.s | iasmarm 宏形态直译：`名 MACRO 参数…ENDM`、裸参数引用、IF/ENDIF 条件展开、DCI→DC16、二操作数→三操作数、`LDR =代码标号`后跟 BIC/ANDS 剥位、参数名大写独名 |
| tasks_rvm.c | 原样拷 |

### 7.3 端口格式规范（清扫定稿；辖 FRT/UO3 家族，RMP 沿用 M5P01 官方风格；**RTT 家族自 2026-10-28 起改随 RT-Thread 官方 libcpu 风格，见 §9.5 同尘定则**）

- 缩进 4sp；操作数列 col 25；注释列 col 45（0-based 44，**勿写成 53**）；宏定义行=宏名补齐 16 列+`MACRO`+3sp→参数列 col 25；iasmarm 的 MACRO 名保持标号列 col 1（语法要求）。
- 宏参数**全大写**按 GCC 家族语义命名（LABEL/SZ/XPSR/EXC_RETURN），禁寄存器名。
- asm 文件无尾部版权脚注（仅 C 文件有）；A6M 的 EXC_POP 尾不加 `B vPortYield_Exit`（POP {R0,PC} 必跳转，死代码）；A7M 家族三版皆有故保留。
- C 件 GCC≡Keil(RVDS) 逐字节同（哈希校验）；portasm.s 语法不同属预期（IAR 用指令流比对验证等价）。

### 7.4 双工具链同步约定

port.c/portmacro.h/FreeRTOSConfig.h/tasks_rvm.c 跨目录**必须哈希一致**（改完立即 Copy-Item+Get-FileHash 校验）；改 port 后跑 `Guest/update.sh` 刷新发布镜像。

---

## 8. uC/OS-III on RVM 移植要点

- **客机包**：`uCOSIII_V308` 三仓嵌套（uC-OS3 V3.08.02 + uC-CPU + uC-LIB），官方 Apache-2.0 零修改。
- **依赖闭合**：lib 头用官方（include path 指 uC-LIB 根，.c 不编译——函数引用全在 NAME_EN=0 死代码）；cpu_cfg.h 注意：TS_TMR_SIZE 必须 `CPU_WORD_SIZE_32`（非 32u）、**不可定义 LEAD/TRAIL_ZEROS_ASM_PRESENT**（#ifndef 语义，定义了就把 C 版 CLZ 裁掉链接缺符号）、TS_TMR_EN/PERF_MON_EN/INT_DIS_MEAS_EN 别定义（cpu_core.h 自带默认会重定义警告）。
- **CPU 侧替代**（cpu_rvm.c 六函数）：IntDis/En/SR_Save/Restore→RVM 掩蔽（官方直译快照制——`Save=(CPU_SR)RVM_Int_Mask 快照`+条件 Mask / `Restore=条件 Unmask(全路径)/Mask`；**Restore(sr==0) 必须走完整 Unmask 路径**（含 pending 投递检查），裸写标志会让 pending 虚拟中断饿死——这是唯一不能逐字直译 CSR 的点）；WaitForInt/Except→空存根（客机 WFI 停核饿死全家）。
- **OS 侧**（os_cpu_c.c=官方全文+手术）：九 Hook 保留；StkInit 尾加 5 超调用参数字（帧 22 字与 FRT 同构）；OSInitHook 的 ExceptStkBase 行摘除（客机无 MSP）；OSStartHighRdy 必须设 **OSRunning=1**（官方 asm 有，重写易漏——漏了 OSTaskChangePrio(NULL) 报 OS_NOT_RUNNING）；首任务入口跳转 `pop{r4}(返回地址,弃) pop{r0}(入口) mov lr,r0`。
- **ISR 协议**：Vct 线程上跑的 ISR 必须 `OSIntEnter()→post(NO_SCHED)→OSIntExit()`——Exit 里才更新 OSTCBHighRdyPtr 并触发 OSIntCtxSw；裸 post+RVM_Virt_Yield（FRT 模式）会让 Vct_Loop 尾部的 OS_Ctx_Handler 拿旧 HighRdy 切错对象（"Interrupt reentered." 刷屏）。
- **Yield 编排**：uCOS RR yield 不变式 cur==就绪链 head；benchmark 两任务同优先级创建（T2 先建=head、领先 yield），勿用 OSTaskChangePrio 破坏链序（插尾）。V3.08 OSSched 在临界区内调 OS_TASK_SW——Yield 落 pend 分支、由临界区退出的 Unmask 补发信号，属正常路径。
- **双路径切换（P1 定稿）**：OSCtxSw=GNU/os_cpu_a.s 的 asm 快路径（伪造异常帧+R4-R11+超调用参数 5 字=22 字，任务栈上直接换帧零陷入；vTaskSwitchContext 块换成 uCOS bookkeeping——OSSched 已选好 HighRdy，asm 只做 hook/OSPrioCur/OSTCBCurPtr/TCB 偏移 0 直写 StkPtr）；OSIntCtxSw=慢路径（ISR 上下文 Vct_Act=1 时 Yield 只设标志，Vct_Loop 尾部真切）。**帧 22 字三处一致**（StkInit/OS_Ctx_Handler/os_cpu_a.s）——快路径切走的帧可被 tick 慢路径恢复，反之亦然；XPSR bit9=0x200 是对齐 PAD 私有标记，EXC_POP 恢复时必须清除。
- **bsp_os_dt.c 弃用**（动态 tick BSP 模板）：RVM 下 tickless 禁死，tick 走 RVM_Virt_Tim_Reg。
- **同尘规范（UO3 包内所有自有件，长期有效）**：文件头=官方三框；asm=104 列星线框/大写助记符/注释列 64/OS_CPU_ 前缀宏/官方 Note(s) 编号体；类型=官方 cpu.h 的 C 原生 typedef（**禁引 stdint.h**）；benchmark 命名=BENCH_* 全大写宏 / Bench_* 驼峰函数 / 静态全局驼峰 / 局部小写下划线 / 指针参数 p_ 前缀 / `for (`、`0u` 后缀小写、`(cast)值` 紧贴。**同尘以本树官方发行版现状为唯一基准**；官方文件自带的列混排原样保留不动，自有文件宏值列单列统一；os_cfg.h / os_cfg_app.h / lib_cfg.h 三件保留官方模板头。
- **SCS 瘦身（cpu.h 与源清单）**：cpu.h 删尽系统控制空间内容；官方 cpu_c.c **整体退出编译**。CLZ/CTZ 全家在 cpu_core.c（查表 C 版）照常可用——Cortex-M0/M0+ 无 CLZ/RBIT，C 查表即官方同款正解；汇编加速（CLZ 指令版 cpu_a.s + cpu_cfg.h 开 CPU_CFG_LEAD_ZEROS_ASM_PRESENT）是 **v7-M 模式**，A7M 端口时提供。
- **测试选择头职责（FRT/UO3/RTT 通用定式）**：frt_test.h/uo3_test.h/rtt_test.h（生成器产物）=纯选择头，只 include chip header；benchPRINT_*/BENCH_PRINT_* 等实体宏一律放 chip header。
- **布局判据**：端口目录跟客机自家惯例（FRT 用 portable/、UO3 用原生 Ports 树、RTT 用 libcpu 树）；-RVM 后缀挂被变体化的官方节点上；兄弟同类拒绝混层。
- **生成器**：Guest_Type=UO3（上游预留 -uo3）；rva Compatible 开三元组。

### 8.1 RV32P 移植要点（V307 一轮实战沉淀）
- **目录布局**：OS 侧 `Ports/RISC-V/RV32-RVM/GCC/`（RISC-V 官方编译器名=GCC 非 GNU）+架构层 os_cpu_c.c；CPU 侧**扁平特例** `uC-CPU/RISC-V-RVM/`+`GCC/cpu.h`（官方 RISC-V 下无架构细分层）。
- **帧布局（RV32）**：params5+mstatus（FS 位标记惰性 FPU 帧：RVF 33 字/RVD 65 字）+31 整数字（pc@0,x1@1,x3..x31；x2 sp 由帧隐含、x4 tp 承载恢复地址——mepc 用户态不可达，恢复用 jalr x0,tp）。
- **快路径三入口**：OSCtxSw/OSCtxSwRVF/OSCtxSwRVD（os_cpu.h 按 RVM_RV32P_COP_RVD/RVF 三选一）——FPU 指令 **.hword 手编码**（fsd/fld/fsw/flw 各 32 对表在 os_cpu_a.s，**逐字抄 FRT portasm.s 勿自算**）；SWITCH 的 bookkeeping **直接在任务栈上跑**（浅调用且全局变量同用户 gp；切内核栈+gp 留给深调度器，uC/OS 不需要）。
- **任务栈预算（V307 三轮破案的根因）**：RV32 切换帧=37 整数字+快路径 FPU 瞬态 33 字=280B（A6M 仅 88B）→基准任务栈 **256 字起步**（128 字会溢出踩 .bss：就绪链/信号量表/RVM_Int_Mask 全毁——症状为计数器乱码+ISR 链 reenter 刷屏，极易误判为切换逻辑 bug）。**BENCH_TASK_STK_SIZE 已下放芯片头 per-port**（G071=128/V307=256）——benchmark.c 无兜底，缺失即 `#error`——新端口头文件必给此宏，按帧深×2.5 以上估。
- **调度时序正统（uC/OS 特有）**：HighRdy 的选择在 switch **外部**由 OSSched/OSIntExit 完成，Ctx handler 只消费不重算——勿仿 FRT/RMP 把调度塞进 handler。
- **GDB 静态变量取证**：探针增删会使 .bss 符号跨构建漂移——每次重编后必须重跑 nm 取当前地址；无声计数器是零扰动判决利器。

### 8.2 A7M 移植要点（F429 一轮全绿沉淀）
- **目录/生成器**：OS 侧 `Ports/ARM-Cortex-M/ARMv7-M-RVM/{os_cpu_c.c,GNU/*}`+CPU 侧 `uC-CPU/ARM-Cortex-M/ARMv7-M-RVM/`；A7M 平台 rva 需加 `<Guest>UO3</Guest>`+`<C9>Makefile,GCC,UO3</C9>`（两处都要）；芯片头名用 rvp 的 **Class 名**（stm32f429zi 非 stm32f429zit6）。
- **快路径=伪装异常帧（FRT A7M portasm 机制）**：入口 PUSH R0/XPSR→CONTROL.FPCA 分基本/扩展帧→EXC_PUSH 造帧（PC=退出标号，XPSR bit9 标 pad）→vstmia s0-s15+fpscr+vpush s16-s31（.short 机器码逐字抄 FRT）→params5+R4-R11+LR 入栈→Mask→存 SP→**trio**→载 SP→Unmask→解栈（原地变换：LR 先出、XPSR 经 msr XPSR_NZCVQ、PC 最后弹）。**A7M 可直接 PUSH {R4-R11,LR}**（A6M 需经 R0-R3 中转）。
- **ICI/IT 脏帧过滤（A6M 没有的 A7M 特有）**：EPSR ICI/IT 位只有真异常返回能恢复；OSCtxSw 用 C 包装先查目标帧（LR_EXC@13 位4 定扩展，XPSR 基本@21/扩展@37——**pad 永在帧顶，偏移两态固定**），脏→RVM_Virt_Yield 慢路径，净→asm 快路径；EXC_POP 内再设防御陷阱 OSCtxSw_Err。
- **调度正统（与 RV32 轮同结论）**：V3.08 源码实证 OSSched 只算 HighRdy 就调 OS_TASK_SW——**trio（OSTaskSwHook/OSPrioCur/OSTCBCurPtr）是端口 switch 的职责**（官方 PendSV 同款）；uC/OS 天然免 Peek。
- **A7M 用汇编 CLZ（指令集事实）**：ARMv7-M 有 CLZ/RBIT——官方 A7M cpu_a.s 提供 CPU_CntLeadZeros/CntTrailZeros/RevBits 并在 cpu.h 定义 CPU_CFG_{LEAD,TRAIL}_ZEROS_ASM_PRESENT（定义在 cpu.h 不在 cpu_cfg.h）。RVM 版：cpu_rvm.c 内联汇编同语义，cpu_core.c 的 C 查表版被宏裁掉。H743 实测 ISR 路径提速 4-5%。A6M/RV32 保持 C 版（RISC-V base 不保证 clz，Zbb 非强制）。
- **快路径开关 OS_CFG_RVM_FAST_YIELD_EN（照抄 FRT configUSE_RVM_OPTIMISED_YIELD 三层模式）**：①os_cfg.h 定义选项（默认 1u=开）；②os_cpu.h `#if` 切换 OS_TASK_SW——开=快路径入口，关=`RVM_Virt_Yield()`；缺定义 `#error`（快败）；③A7M os_cpu_c.c 的 C 件包 `#if`，但 **OSCtxSw_Err 留在 `#if` 外**——asm 无条件汇编恸引它，包进去关态即链接缺符号。H743 双态实测：开=Yield 372，关=1526（4.1×）。
- **CPU 层 asm=存在即编译（分层铁律）**：CLZ/RBIT/REV 属于 **uC/CPU 层的 cpu_a.s**，不进端口也不内联进 cpu_rvm.c；uo3_gen：端口 os_cpu_a.s **必需**（硬检查+无条件编译）；CPU 层 cpu_a.s `exists()` 才编译——A6M/RV32 无此文件自动跳过走 C 查表。
- **生成器 Path_Conv 排序**：目录推送按"短路径优先"（Left<Right），官方目录与 -RVM 目录互不相交时不受影响。
- **官方 CPU 编译器目录彻底退出 include 路径（设计定则）**：RVM CPU 层**自我完备**，缺 cpu.h 就该响亮地编不过，而不是静默解析到官方头埋雷（官方 cpu.h 的 BASEPRI 方案与 RVM 配置根本不兼容）。
- **烧录（WCH-LinkE+SDI）**：`adapter driver wlinke`+`sdi newtap`+`target create .. wch_riscv -chain-position`+`flash bank .. wch_riscv 0x0`（**flash 驱动=wch_riscv**）；板级 wch-riscv.cfg 是 JTAG 版不可用。

### 8.3 Keil/armasm 端口要点（G071/F429 P2 一轮全绿沉淀）
- **编译器子层=官方 Micrium 名 `ARM/`**；头件/cfg 从 GNU/ 拷贝+Toolchain 行改 ARM C Compiler；rvp 命名 rvmdk-*-uo3.rvp。
- **armasm 译文规则**：`;` 注释；IMPORT/EXPORT；`MACRO`/`宏名 $参数`/`MEND`；$LABEL 跳转；PROC/ENDP；AREA …,CODE,READONLY,ALIGN=2；THUMB/REQUIRE8/PRESERVE8；两操作数→armasm 允许（与 iasmarm 不同）；`MSR XPSR,R0`；条件宏 `IF … ENDIF`；**VFP 用 DCI 半字对逐字保真**；LTORG 收尾。A6M 与 A7M 的宏体差异：A6M 用 LDR Rx,=imm+TST Rx,Ry（无立即数 TST）、EXC_PUSH 无 IF；A7M 用 TST Rx,#imm、EXC_PUSH/POP 带 `IF (($SZ-8)!=0)` 条件。
- **唯一警告 A1581W**（字面量池对齐填充）无害勿修。
- **Keil 构建链**：UV4.exe -b -j0 -o build.log；**rc=1 只是有警告**（0 Error 即过）；链序同 IAR。
- **RV32P×Keil 结构性 N/A**：keil_gen 硬编码 pCCUsed=ARMCC+uAC6=1——MDK-RISCV 属生成器级新功能。

### 8.4 IAR/iasmarm 端口要点（G071/F429 P3 一轮全绿沉淀）
- **编译器子层=官方 Micrium 名 `IAR/`**；头件/cfg 从 GNU/ 拷贝+两处改：Toolchain 行→IAR Assembler；`OS_TASK_SW_SYNC()` 内联 asm→`__asm volatile ("isb")`。rvp 命名 ewarm-*-uo3.rvp。
- **iasmarm 译文规则（§5.4 全集）**：`SECTION .name:CODE:NOROOT(2)`；EXTERN/PUBLIC；**`宏名 MACRO 参数…ENDM` 同行式**（无 MEND）；裸参数引用；无 PROC/ENDP；**A7M EXC_PUSH 用 `MOV LR,#EXC_RETURN` 立即数**+三操作数 AND/OR；`IF/ENDIF` 照用；**VFP 用 `DC16` 对**；LTORG+END 收尾。A6M 保持 APSR_nzcvq 版 MSR，A7M 用 `MSR XPSR,R0`。
- **PUBLIC 名必与标签一致（Error[67] 教训）**：克隆改造后必查 PUBLIC/EXTERN 名单与实际标签一一对应。
- **IAR 构建链**：`iarbuild.exe <ewp> -build Debug`；rc=0 过（有警告也 0，与 UV4 语义不同）；"Total number of errors: 0" 为准。宿客镜像=**Virt1.bin**（UO3 符号全在其中，Kernel 工程是宿主侧不含）。
- **机码取证定位法**：Virt1.bin 基址 0x8020000（按芯片 rvp Data_Base），map 符号地址含 **Thumb 位（奇数）**——bin 内按 偶偏移=符号-1 搜半字模式（CLZ=`80 F0 B0 FA`、RBIT=`90 FA A0 F0`、VFP 六对 LE 半字对）。
- **H743/V307**：H743 IAR/ 与 F429 同套零改动；RV32P×IAR 结构性 N/A。
- **【判例】OSCtxSw C 包装无入口掩蔽=共享竞态**：OSSched 选定 HighRdy 到 OSCtxSw_Swt asm 内部 Mask 生效之间有几十条指令的**无掩蔽窗口**——TIM4 中断恰在此打断→慢路径改写调度状态→被中断的快路径用错乱状态完成切换→INVSTATE。修法=**OSCtxSw 入口先 RVM_Virt_Int_Mask()**。
- **【判例】OS_CPU_ALIGN_PUSH 极性抄错=帧整体错位一字（IAR 特有案）**：A6M 入口压 **3 字**（R0+XPSR+R1——R1 作无立即数 TST 的掩码），宏极性 **BEQ**；A7M 入口只压 **2 字**（有 CONTROL.FPCA 判断不需 R1），极性应 **BNE**。IAR A7M 版从 A6M 抄宏没翻极性——帧系统性错位一字→INVSTATE 100% 必挂。军规：**汇编宏的跳转极性注释（"N words pushed above"）是可执行规格——改入口压栈数必同步翻极性；跨架构抄宏必核入口序**。
- **【判例】FRT 全端口 vPortYield 缺 deferred-yield=临界区内切换击穿**：FreeRTOS 官方契约允许临界区内 yield；FRT RVM 全端口是立即切换型且虚拟掩蔽是平铺全局标志——修复=软件 pend：vPortYield 入口 `if(uxCriticalNesting!=0){xPortYieldPend=pdTRUE;return;}`，vPortExitCritical 归零时**先切后开**。六 ARM 端口收敛为单一 C 层 vPortYield；RV32P 同构。军规：**虚拟化下客机的"中断开关"是全局标志不随任务走——任何立即切换型 portYIELD 都必须自配软件 pend；切/开顺序必须"先切后开"**。
- **【判例】慢路径 Unmask→Yield 调序=Yield 掩蔽语义误读**：`RVM_Virt_Yield` 在 mask=1 时是 no-op pend（只置 VCTF->Ctx+Vct_Pend）——正确用法是**先 pend 再用 Unmask 激活**（`Yield; Unmask;`：全程掩蔽，Unmask 边界一次性完成激活+切换）。军规：**凡超调用切换，一律 pend-then-arm（先 Yield 后 Unmask），asm 直切才用出口 Unmask**。
- **【判例】CPU_SR 计数制→官方直译快照制**：官方 uC/CPU 无计数——`CPU_SR_Save=MRS+CPSID`（快照+关）、`CPU_SR_Restore=MSR`（无条件写回）。直译可行的前提：把 `RVM_Int_Mask/Disable/Vct_Pend` 从 header-static 改为 **extern 导出**（static 时每 TU 副本，客机读到的与 rvm_virtlib.o 的真值不同步）。军规：**移植层 API 语义必须优先对齐原版（快照/写回），自创计数制属防御性超集，仅当原版语义不可直译时才保留并在头注释说明差异点**。

---

## 9. RT-Thread V4.1.1 on RVM 移植要点（四板定档沉淀）

### 9.1 端口最终形态（三端口：armv6m / armv7m / rv32p）

- **协议（A7M 全景，A6M/RV32 同构减 FPU/ICI）**：
  - 调度级 `rt_hw_context_switch`（C）：锁存 from/to 全局 → ICI/IT 脏帧过滤（LR_EXC@13 位4 选 XPSR@21/37）→ 净帧走 asm 快路径 `rt_hw_context_switch_swt`，脏帧走慢路径（flag=1 + `RVM_Virt_Yield()` + `RVM_Virt_Int_Unmask()`，pend-then-arm）。
  - ISR 级 `rt_hw_context_switch_interrupt`（asm，跑在 Vct 线程）：flag 已置则只更新 to（_reswitch），否则置 flag+锁存 from/to；然后 `PUSH {LR}; BL RVM_Virt_Yield; POP {PC}` 返回。**绝不 fall-through 进快路径**（㉕ 判例：会把 Vct 上下文写进被打断线程的 TCB 槽）。
  - `PendSV_Handler`（C，Vct_Loop 尾部消费）：**双守卫** flag==0 跳过 + from==0 跳保存；保存=RVM_REG→from 线程栈（S16-S31 按 LR.4、EXC_RETURN、R4-R11、params5 共 22 字）；恢复=to 线程帧→RVM_REG，`Reg.SP` 指向硬件帧。
  - `rt_hw_context_switch_to`（asm，一次性）：**入参 STR 进 rt_interrupt_to_thread**（⑰ 判例：漏掉=空读 Sftd 无限恢复循环）+ from=0 + flag=1 + `RVM_Hyp_Int_Ena` + 手工弹初始帧 `BX LR` 进首线程。
  - `rt_hw_interrupt_disable/enable`=RVM 掩蔽快照制（同 UO3 cpu_rvm.c 定式）；`rt_hw_interrupt_check`=`(RVM_STATE->Vct_Act != 0)`（IPSR 在客机恒 0 失真）。
- **asm 快路径宏必须与入口序匹配（㉜ 终局判例，与 8.4 极性案同族）**：A7M 入口压 **2 字**（R0+XPSR）→ ALIGN 极性 **BNE**、EXC_PUSH 的 R0 重载在 `[SP,#4]`、XPSR 重载在 `[SP,#4*(SZ-2)]`、净压=净弹=`SZ-2` 字。**勿从 armv6m 版抄宏**（其入口 3 字、BEQ、`[SP,#8]` 深度、sz-3 压栈——三处全不通用）；修正蓝本=RMP `rmp_platform_a7m_rvm_gcc.s` 或 UO3 A7M GNU `os_cpu_a.s`。错抄的实际危害：伪装帧 R0 槽=陈旧字、**XPSR 槽=被打断线程的 R0 值|0x01000000**——帧生而为脏，"filter 漏脏帧"是误诊。
- **帧 22 字三处一致**（stack_init / PendSV / context_gcc.s）；FPU 全走惰性（Virt1 实测零浮点指令时全基本帧）。
- **RTT 特有语义**：`rt_current_thread` 在 rt_schedule 内**预更新为 to**（scheduler.c L466）；同优先级**插链头 LIFO**（最后就绪者先跑——benchmark 两任务创建顺序决定领跑者）；调度器在 `rtthread_startup()` 内已启动——main 里创建多任务必须包 `rt_enter_critical()/exit_critical()`。
- **switch_to 的 flag 终值已统一为 0（2026-10-28 终审发现并当日修复）**：ARM 两端口原置 flag=1 是官方模板残留——我们的 switch_to 自己 BX 进首线程、不 pend PendSV，flag=1+from=0 成悬挂锁存；若首个 tick 恰落在"首线程开跑→首次线程级切换"的微秒窗口，ISR 版走 _reswitch 只更新 to、PendSV 因 from==0 跳保存→首线程被抢占现场丢失→下次恢复从入口静默重启。三端口现统一置 0（RV32P 原生即 0）——首个中断驱动切换正确锁存 from=首线程。H743 三轮板测全绿（Yield 495）；G071 编译验证过、板测待板。顺带确认：快路径恢复中途被 Vct 抢占时"半恢复现场存进目的地线程自己的槽"之所以正确，正是依赖 rt_current_thread 预更新（from 锁存=rt_current_thread=to）——A6M asm L248-251 注释明示，三端口同构成立。

### 9.2 v4.1.1 配置与 API 坑（G071 一轮实战全部板上实证）

1. 分配器宏=**RT_USING_SMALL_MEM_AS_HEAP + RT_USING_SMALL_MEM 双定义**（只定义老宏→malloc 落空分支恒 NULL→startup fault 死循环）。
2. IPC 宏开关（RT_USING_SEMAPHORE/MAILBOX/MESSAGEQUEUE）必须显式开。
3. 类型名 v4.1.1=rt_mailbox_t/rt_mq_t（非老版 rt_mb_t）；API 名 rt_interrupt_leave（非 exit）。
4. **RT_USING_CONSOLE 必须开**——未定义时 rt_kprintf 编译成空 inline，调用与字符串全被优化删除，端口错误陷阱打印静默失声。
5. main 栈 512 不足（rt_kprintf/vsnprintf 链溢出）→1024；idle 栈 256→512，且 **idle.c 只认 IDLE_THREAD_STACK_SIZE**（rtconfig 的 RT_THREAD_IDLE_STACK_SIZE 是断线旋钮，模板双定义备注）。
6. benchmark ISR 相入段**必须静默**（putchar 超调用 ≈87µs/字符@115200，24 字符≈11 个 200µs tick——入口打印=邮箱池满 EFULL 丢信+逐 tick 打印=自持活锁，㉒/⑱ 判例）。

### 9.3 布局与构建工具

- **rtt_layout_knob.h**：`RTT_LAYOUT_KNOB` 字符串长度=链接期确定性平移量（rtt_board.c 开机打印 `LK:...|` 标识当前布局）——__TIME__ 骰子根除后的可控布局扫描旋钮。注意它在链接序末段（rtt_board），只平移尾部；要平移全量需在 rtt_benchmark.c（首 TU）定义同名串。
- **__TIME__ 骰子**：`__DATE__/__TIME__` 进字符串池（SHF_MERGE 跨 TU 合并），make -j8 下各 TU 编译秒是否跨秒决定 ±12B 布局掷骰。定值化程序见 §14 判例 15。**当前状态：骰子处于还原态**（2026-10-28 狩猎需要；下次密闭构建前重定值）。
- 既有脚本（Project\Script\）：cap_h743.ps1（参数化捕获，COM14 硬编码注意改）、flashh743.ps1（CubeProg+SN）、runstats.sh（复位多轮统计）、sweep_lk.sh / dice_hunt.sh（布局扫描/掷骰狩猎）。

### 9.4 未结项

- **README RT-Thread 行已填**（2026-10-28 深夜）：双语 Practical ports 表十行（M0+/M4/M7×三链+RV×GCC）全部填入——基准值取官方风格重排后终板数据；Flash/SRAM 按既定口径**重建十链 Virt1 提取**（Flash=客机镜像尺寸、SRAM：GCC=data+bss / IAR=map readwrite / Keil=RW+ZI）。注意 RTT 的 Mail 列=benchmark 的 Notification 项（同原语 rt_mb，仅打印标签改名）。
- ㉑ 全 Deferred 实验版（rt_hw_context_switch≡interrupt 全走 PendSV）startup 帧错位一字——独立未解项，非现行配置，仅存档。
- __TIME__ 还原态（见 9.3）。

### 9.5 三编译器端口要点与判例（2026-10-28 armv6m/armv7m IAR+Keil 移植轮沉淀）

- **文件命名与生成器**：端口目录平铺 `context_{gcc,iar,rvds}.s`（小写！），rtt_gen 的 Toolchain_Get 自动映射（ARMCC→rvds、ICCARM→iar、其余→gcc）+存在性硬检查——零生成器改动即支持三链。rvp：ewarm/rvmdk-*-rtt.rvp 从同芯片 FRT 版克隆，仅改 Guest_Type+Vector_Stack_Size 两行。
- **【同尘定则（2026-10-28 二轮，用户裁定）】RTT 端口文件正文一律随 RT-Thread 官方 libcpu 风格**：小写寄存器+大写助记符、操作数列 ~col 13、函数前 `;/*` 小原型块（弃 UO3 式星线框）、IAR 用 EXPORT+`SECTION .text:CODE(2)`、armasm 用 `AREA |.text|`+`标签 PROC` 同行+尾部 `ALIGN 4`/`END`、文件头补官方 Change Logs 表+`@addtogroup`、rtconfig 模板用官方 `// <o>`/`// <c1>` wizard 注解。**两项豁免（刻意保留）**：①版权头保持 Evo-Devo + "NOT part of RT-Thread" Unlicense 声明不改；②文件名小写 .s 不改（Windows 大小写碰撞判例）。§7.3 房规自本轮起仅辖 FRT/UO3 家族，RTT 家族以 RTT 官方风格为准。
- **重排方法论（零语义变更的证明）**：重写前后各汇编一次，iasmarm `-l` 清单 / `fromelf --text -c` / objdump 三方指令流做序列对齐 diff——六份文件全部逐指令零差异（219/170/207/171/230/183 条）。**该方法当场抓到一个真 bug**：重写时手滑把 A6M EXC_PUSH 的 `LDR r1,=0xFFFFFFFE; ANDS r0,r1` 写成 `AND r0,r0,#0xFFFFFFFE` 立即数——**Thumb-1 无此指令**（iasmarm 静默产出 M0+ 非法编码），指令流 diff 的 delete 块直接暴露。军规：格式重排必须配指令流等价性复验，"汇编器没报错"不是免检理由。
- **【判例】官方大写 .S 遗留拷贝=Windows 大小写碰撞陷阱**：armv6m_rvm 目录里残留官方 cortex-m0 端口拷贝 `context_iar.S`/`context_rvds.S`（建目录时拷入）——Windows 文件系统大小写不敏感，我们的小写 context_iar.s 会撞上它，生成器存在性检查误命中，**构建会静默编入官方 PendSV 端口**。军规：在同一目录引入仅大小写不同的文件名前先清查遗留；官方原件仍在 libcpu/arm/cortex-m0/ 未动。
- **【判例】rtdef.h 的 va_list 三分支**：无 RT_USING_LIBC 时仅 `__GNUC__` 分支有 `__builtin_va_list` 回退；**armclang 定义 __ARMCC_VERSION 抢先走 ARM Compiler 分支**（即使它也定义 __GNUC__），ICCARM/armcc 全都需要 RT_USING_LIBC 拉 <stdarg.h>。修法=模板 rtconfig.h 条件定义 `#if !defined(__GNUC__) || defined(__ARMCC_VERSION)`（纯 GCC 零扰动；rtdef.h 是该宏在内核源里的唯一消费者，无行为副作用）。
- **iasmarm/armasm 译文新坑**：①armasm 宏原型行**不能顶格**（顶格=标号字段，报 A1203E"Illegal label parameter start"）——与 iasmarm 的"MACRO 名保持标号列 col 1"**恰好相反**；②iasmarm 需显式 EXTERN 全部引用符号（GAS 未声明符号自动外部化，含 err 陷阱函数易漏）。
- **板测基础设施**：cap_board.ps1（COM 参数化）/board_cycle.sh（rvp→生成→五链→烧录→捕获→判定一键）。早期"STLink VCP 楔死、唯一解=物理重插"的结论已被 §9.6 推翻（真实原因=CubeProg 停核粘滞，固件从未启动；CCM 残留数据不能证明在跑）——判别一律按 §9.6 的 RCC CR 法。

### 9.6 F429"VCP 楔死"终局破案：CubeProg 停核粘滞（2026-10-28 深夜，本轮最大工具链坑）

- **真相**：F429 的 STLink VCP **从未坏过**——所有"静默"都是**固件从未启动**。CubeProg 的 `-halt`/`--coreReg`/某些复位路径会在这块板上把核留在停核/复位态且**之后 CubeProg 的 `-rst`（软件复位）不能解锁**；唯一可靠解锁=**OpenOCD `reset run`**（srst 硬件复位，cfg 见 f429_rel.tcl：CPUTAPID 0+srst_only srst_nogate connect_assert_srst）。解锁后固件完整启动，COM11 持续输出（9602B/100s 完整两轮基准），三链 9/9 全绿收官：GCC 648 / IAR 612 / Keil 532（vs 全慢 2546=3.9~4.8×）。
- **误诊链复盘（三层连环假象，全部入坑库）**：①"VCP 楔死"——错，VCP 好的，固件没跑自然无输出；重插两次当然无效。②"固件在跑实证"——错，**CCM 的"初始化数据"是上午成功运行的残留**（CCM 只掉电清零、系统复位不清）；同理 ⑪-era 的"-coreReg 恒指复位向量"记录实为同一现象。③"COM 映射洗牌"——半错，重插确会重枚举，但 G071 周期输出混入复位测试造成假阳性（读文字内容才鉴明）。
- **判别法（判"固件没跑"的金标准）**：不停核读 **RCC CR**——HSI-only（0x00006383）+PLLCFGR=复位值=时钟初始化从未执行=固件没跑（RME 内核第一件事就是配 HSE/PLL）；读 console UART CR1=0 同证。**读寄存器采真 PC 只信 OpenOCD**（halt→reg pc→resume）；CubeProg 的 --coreReg 读数被自身的复位-停核行为污染，不可作为运行态证据。CCM/SRAM 数据**不能**证明"在跑"（残留）。
- **烧录工作法（F429 专属）**：读者先挂 COM11 → CubeProg `-w ... -v`（不带 -rst，带了也白带）→ OpenOCD `reset run`（f429_rel.tcl 一行流）→ 捕获判定。此流程三链实证稳定。

### 9.7 V307 复验与 COM 映射判例（2026-10-28 晚）

- **V307 重样式复验**：官方风格重排（仅头注释+模板 wizard 注解，指令流零变更）后全链重建（kernel.bin 148884B）+ OpenOCD/WCH-Link 烧录 + COM6 捕获——**9/9 零 fault，Yield 533/567/533 与档案逐字一致**。RTT 四板×三编译器可达矩阵至此全部官方风格统一。
- **【判例】重插/枚举洗牌后 COM↔板卡映射不可信**：F429 重插后 vcp_pair 报告的 SN→COM 映射与实际不符（空捕获≠VCP 死）。判别法=**持端口开+逐板硬复位+读内容**（banner/横幅文字鉴归属），勿信静态枚举；周期性输出（G071 的 RMP 多 VM 基准+overflow 标记）会造成"复位了别的板也有字节"的假阳性，必须读文字内容鉴别。

---

## 10. DjyOS V1.1.0 on RVM 纪念性移植（M7 + GCC 单组合，11/11 真机）

> 定位：作者 2024-02-22 封笔后的纪念性移植（M7M02 README "Commemorative ports"）。
> 组合唯一：NUCLEO-H743ZI + GCC + `-djy`；目标 = 11 项基准全部出数、**内核源码零修改**。
> 正式档案：`DjyOS_V110/test/chip/djy_test_stm32h743zi_rvm.h`；README 纪念性移植行已填。

### 10.1 端口形态：事件驱动内核压进 RVM 的两线程模型

- DjyOS 的调度单位是**事件**，线程（`ThreadVm`）只是事件的容器；RVM 每个 VM 只有 Usr/Vct 两条真实线程。
  端口用**伪装异常帧**在单一 Usr 线程上换 DjyOS 线程，帧 22 字 = 5 超调用参数 + R4-R11 八个寄存器 + EXC_RETURN + 8 字硬件帧。
  **帧 PC 必须存奇数**（Thumb 位）——该帧也会被内核的真实异常返回恢复，偶数 PC 直接 INVPC（判例 §14-19）。
- `cpus.s` 六原语：`__asm_reset_thread`（池化线程重建，`stack_top-88`）、`__asm_turnto_context`（不保存、直接切入）、
  `__asm_switch_context`（阻塞切换，快路径伪装帧）、`__asm_switch_context_int`（中断级：只锁存 + `RVM_Virt_Yield`）、
  `__asm_reset_switch`（CN_KEEP 复位旧线程再切）、`__asm_start_thread`（首切）。
  三个必踩点：`__asm_start_thread` 里的 `RVM_Hyp_Int_Ena` 会毁 R0（AAPCS 参数/返回寄存器）→ 必须 `PUSH {R0,LR}` 保护；
  `__asm_reset_switch` 的 new_vm 必须停 RAM 槽 `djy_rs_new_vm`（重建写的正是旧栈顶，压栈必被覆盖）；
  切换请求锁存 first-request-wins（in-flight 请求不得被后续中断请求覆写）。
- `int_hard.c`：中断引擎 `__Int_EngineReal/__Int_EngineAsynSignal` **逐字保留**；NVIC 相关全部空操作（客机无权碰）；
  `djy_attach_phys` = `RVM_Hyp_Vct_Phys` + 两条 `RVM_A7M_Int_Local_Mod`（PRIO 0xFF、STATE 1）+ `RVM_Virt_Vct_Reg`；
  `Djy_Ctx_Handler` 在 Vct 线程上完成 RVM_REG ↔ 线程栈的转存（慢路径，只有脏帧/中断级切换会走到）。
- 内核源码零修改；端口的头集合分成两个目录：`include/` 放**垫片**（stdint/heap/list/shell/multiplex/blackbox/dbug/errno/stat/dirent/fcntl/time/types/cpu_peri*/sys/time.h），`template/` 放**配置层**（project_config、4 个 component_config_*、board-config）。两层都直接进客机 include path（`djy_gen.cpp` 逐条 push），**不向工作区复制头文件**——makefile 生成器会把 include 列表按字典序排序，工作区路径（`../Include/...`）永远排在 `djyos/*` 之后，副本根本无法影子上游；而 `bsp/.../armv7e-m_rvm/include/` 与 `.../template/` 排在 `djyos/*` 之前，这才是垫片能压过上游的原因。端口目录是唯一真源，改 `template/project_config.h` 直接生效。
- **C89 隐式声明陷阱**：堆函数是**函数指针变量**（`M_Malloc` 等），没有原型时 C89 会编成对 BSS 变量地址的直接 `BL`
  → 首炸 UNDEFINSTR。垫片 stdint.h 必须写全 `extern void *(*M_Malloc)(unsigned int,unsigned int);` 形式。
- 中断原语映射：`Int_LowAtomStart/End` = `RVM_Int_Mask` 快照制（快照值放 R4 过超调用，放 R0 会被返回值毁）；
  `Int_HalfEnableAsynSignal` 只动内核计数，不碰 RVM 掩蔽。

### 10.2 启动契约：StartOs 之前禁止 pop 出可运行事件（判例 §14-17）

- `DJY_EventPop` 末尾的 `Int_RestoreAsynSignal()` 会**立刻调度**（`ready != running` 即切）。若在 `djy_main`
  （启动容器，**不是线程**）里 pop 出事件，切换的 from 却是 `g_ptEventRunning`（此时 = idle 服务事件）
  → **启动帧被记进 idle 线程的 `vm->stack`**，而这块"帧"住在容器栈上；djy_main 随后继续跑、栈生长把它覆盖，
  `__DJY_StartOs` 首切 idle 时拿到 `pc=0x20000` 的垃圾帧，`Djy_Ctx_Handler` 只能连发 BADFRAME。
- 正解：driver 事件交给 **idle/服务线程首轮 `CleanWakeupEvent`** 里 pop（一次性闸）。DjyOS 自己的 `__DJY_InitSys`
  也是手工植入服务事件、绕开 pop——同一个道理。**判据：任何在 StartOs 前 pop 出可运行事件都会复现。**
- 端口哨兵：`Djy_Ctx_Handler` 保存前校验 `RVM_REG->SP` 是否落在 from（或唯一合法混合窗口 to）自身栈区间；
  不落则计 `djy_foreign_saves`、打印一次 `[FSAVE from= to= sp= lr= num=]`、并**拒绝把外来帧记名**——
  否则只会很久以后在一个无关操作上炸。

### 10.3 静态堆只有 LIFO 回收（判例 §14-18）

- `__Heap_StaticFreeHeap` 只在被释放块恰为 `Cession->last`（最后一次分配）时回退 bump 指针；否则只摘链表节点、
  **空间永久丢失**（`heap_bottom` 只增不减）。
- 所以 create/delete 循环必须**先释放上一轮、再分配本轮**。批量回收（每 8 轮经 idle 清一次）会因中间分配打断
  LIFO 而每轮泄 ~2KB，32KB 堆约 14 轮耗尽 → `__DJY_CreateThread` 返回 NULL → 内核 `__DJY_SelectEventToRun`
  的 `while(g_ptEventReady->vm==NULL)` **死循环**（该循环假定线程必能创建）。现象：Activation 横幅后永久无输出。
- 验证法：`M_GetFreeMem()` 探针，10002 轮（10000 测量 + 2 预热）空闲恒 26872B（零漂移）；批量回收版 14 轮即见 144B。

### 10.4 tick、栈刷漆与 idle

- tick 走 **RVM 虚拟定时器**（`RVM_Virt_Tim_Reg`）；不占额外 TIM。基准中断源 TIM4（物理 30 → 虚拟 2）与 tick 无关，
  所以中断项**不需要内核时钟**（早先用 `DJY_EventDelay` 当"睡眠"是错的）。
- 内核在**分配与池复用**时用 `memset('d')` 刷整段线程栈（供 `__DJY_CheckStack` 扫描溢出）。该刷漆现在是**可选**的：
  内核侧仅三处，各用 `#ifndef CFG_STACK_PAINT_DISABLE` 包住（`__DJY_CreateThread` / `__DJY_CreateStaticThread` /
  `__DJY_SelectEventToRun` 的池复用），宏由端口的 `project_config.h` 定义——这是本移植**唯一**处对内核源码的修改，
  默认（不定义）保持原行为。实测（同一镜像只切这个宏）：刷漆 ≈ **1050 周期/次线程周转**，凡是每轮"建线程/回收线程"的项
  都吃这个价（Yield 2073→1017、Notification 1863→805、Activation 2159→1065、ISR Notification 1943→1015、
  ISR Activation 2299→1381），而向驻留 worker post、malloc/free、relay 这些不周转的路径**没有差别**（±50 以内）。
  注意：刷漆是栈溢出检查的输入，若把 `CFG_IDLE_CHECK_STACK` 打开，必须同时去掉这个宏。
  newlib-nano 逐字节 memset 会把单次刷漆抬到约 5900 周期 → 端口仍提供**字对齐 memset**（链接期优先）；刷漆开启时它依然决定该项成本。
- `CFG_IDLESTACK_LIMIT` 1024 不够（idle 里要 `M_Free` + 信号量 post）→ 2048。
- 内核 tick 只喂 `DJY_ScheduleIsr(1)`；tickless 覆盖层不注册（op=NULL，调用点全是空操作）。

### 10.5 benchmark 与基线

- **11 项** = 6 线程级（Yield / Notification / Activation / Semaphore / MsgQ / Memory）+ 5 中断级
  （ISR Notification / Activation / Semaphore / Message queue / **RVM relay**）。
- 中断项按**轮次推进的相状态机**：ISR 里只计数、**绝不打印**（打印≈活锁），满 10000 转下一相；
  worker 由每相首个中断拉起；driver 收 done 信号量后统一打印。定时源 TIM4 = 20000 计数 @200MHz = 100µs = 40000 周期。
- **ISR RVM relay**：共享页 `DATA_SHARED_SHARED1_BASE = 0x24014000`（rvp `<Shmem><S0>Shared1`，Monitor 页表 order10），
  第二字 `START` 由**内核 TIM4 上半部**在投递前写入（`rme_handler_tim4.c` → `rme_test.h`；`TARGET==0` 时信号回本 VM 向量），
  客机 ISR 读 `(u16)(TIM2<<1) - (u16)START` 即全链延迟，观测点与 uC/OS-III 的 `Bench_TestIntHandler` 一致（16 位差，周期 < 65536 无歧义）。
- 基线（H743/GCC/-O3，AVG/MAX/MIN，CPU 周期；AVG 复位间重复性约 1%，MAX/MIN 受中断干扰）：
  Yield 1017/4896/928、Notification 805/4696/764、Activation 1065/5144/1008、Semaphore 892/4512/768、
  MsgQ 1488/5256/1368、Memory 288/3876/256、ISR Notification 1015/2216/940、ISR Activation 1381/1508/1280、
  ISR Semaphore 1220/2356/1132、ISR Message queue 1873/2104/1716、ISR RVM relay 2822/4380/1864。
  （以上是刷漆关闭的现行配置；刷漆开启时为 2073/1863/2159/929/1539/288/1943/2299/1270/1896/2769，差异只在五个周转项。）
- 判绿哨兵：`defers=0 padded=0 badframes=0 foreign=0` + `Benchmark complete` + 无第二次横幅（旧镜像特征）。

### 10.6 端口硬伤速查（全部真机踩过）

① 首切 `RVM_Hyp_Int_Ena` 毁 R0；② 漏 `Int_Register` → 线表槽 NULL，`Int_*` 全部静默；③ 漏物理 NVIC 两步（PRIO/STATE）；
④ 漏 RCC 时钟门；⑤ TIM SR 是 rc_w0（`&= ~UIF`，写 1 清不掉）；⑥ `BENCH_TIM_READ` 漏 `<<1`（TIM2 = 2×CPU）；
⑦ 掩码下 `RVM_Virt_Yield` 只抬旗不切换 → 必须紧跟 Unmask（而线程级脏切换必须当场完成，去掉 Unmask 会让事件带活锁存→内核 panic）；
⑧ 在 Vct 线程上调 Unmask 会自发送死锁；⑨ `reset_switch` 参数压进正在重建的帧区 → RAM 槽；
⑩ in-flight 切换锁存被覆写 → first-request-wins；⑪ 伪装帧存偶数 PC → INVPC；⑫ ISR 项误用线程级事件类型（每项需专用类型）；
⑬ 首版 idle 栈 1024 溢出；⑭ 深度诊断打印会改变布局/时序（取证本身是扰动源）；
⑮ **永不返回的切入（turnto / start_thread）绝不能走延迟路径**——超调用返回会复活被放弃的调用者；反过来，被放弃上下文的切换失败时也不能"保持当前"（turnto 的当前上下文本来就要扔掉）。

### 10.7 FPU / no-FPU 双配置验证（软浮点契约）

- **端口帧是整数帧**（22 字基本异常帧），客机按软浮点 ABI 编译（`-mfloat-abi=soft`，即生成器默认），编译器不产生 VFP 指令。
- **两种 VM 配置都真机通过**（同一 H743，11 项齐、哨兵全 0、`Benchmark complete`）：
  - `Coprocessor=None`：`gccmf-stm32h743zit6-djy.rvp`，内核 `RME_COP_NUM=0`，Yield ≈ 2071。
  - `Coprocessor=FPV5_DP`（rvp 的**芯片行**与 **Virt1 段**两处都要改）：内核 `RME_COP_NUM=1`，客机 makefile 自动得 `-mfpu=fpv5-d16`，内核按线程属性开关 CPACR 并做 Cop 上下文交换；11 项齐，Yield ≈ 2103、ISR relay ≈ 2844（多出的是每次内核线程切换的 Cop 交换）。
- **复现**：**临时**复制 rvp（例如 `gccmf-stm32h743zit6-djy-fpu.rvp`），改上述两处为 `FPV5_DP`，`bash benchmark.sh <新 rvp>` 生成到独立工作区（名字由 rvp 文件名派生），五链构建、烧录即可；**验完就删 rvp 与工作区**——`autobuild-gccmf.sh` 会扫 `gccmf-*.rvp`，留一条同客机链会破坏"单组合"矩阵。器件级 `-mfpu` 只影响库选择，不改变 ABI。
- **边界（有实测证据，勿想当然）**：如果客机真的执行 FPU 指令（`-mfloat-abi=softfp/hard`，或内联 VFP），**S16-S31 不会跨 DjyOS 线程保存**。实测法：驱动线程写 S16=1.5f → 切到 idle 线程写 S16=2.0f → 切回后读回，得到 2.0f（状态丢失）。根因是端口帧只有基本帧：扩展帧需要的 S0-S15、FPSCR、S16-S31 三段在所有路径（伪装/回弹、turnto、复位帧、Ctx 处理器）都没有实现。因此 `cpus.s` 用 `#if defined(__ARM_FP) #error` 把契约钉死——浮点 ABI 的构建**直接编译失败**，而不是运行时静默串状态。
- 其余 RVM 客机（FRT/UO3/RTT/RMP）同样是软浮点客机：它们的 FPU 帧路径是未测试的死代码（RTT 快路径与慢路径的扩展帧布局甚至互不一致），不要拿它们当"已支持 FPU"的先例。

### 10.8 端口代码风格（本纪念性移植的落定规则）

- **版权**：所有端口/测试自有文件一律 `Copyright (c) 2026, Evo-Devo Instrum.`，并带一句 "This file is NOT part of the DjyOS distribution ... the DjyOS kernel itself is copyright the DjyOS Open Source Development team"——**不要把端口文件的版权写给原作者**；从上游头文件改写来的垫片另加一行 provenance（"self-contained stand-in for the DjyOS V1.1.0 header of the same name"）。
- **语言**：程序注释一律英文（含 `include/`、`template/` 与 `benchmark`）；内核自带的 `djyos/**` 中文注释不动（零修改）。
- **结构随原作者**：端口层 C 文件头 = `//----` 分隔线 + `Module/Author/Version/Description/Notes/Revision history` 元数据；函数横幅用 `//----Name----` + `Function/Input/Output/Return/Notes`；汇编用**小写助记符**、`/* */` 块内 `@` 前缀注释、宏名大写（`DJY_*`）；缩进 4 空格、`if(` 无空格、类型用 `u32/ptu32_t/bool_t`。测试文件（`test/djy_*.c`）用同一 Evo-Devo 声明即可，不强制元数据块。
- **等价性证明**：汇编重排（大小写、注释装饰）后必须用 `objcopy -O binary --only-section=.text` 比对 .o 的代码段逐字节相同（`-g` 的调试行号会变，属正常）；C 侧重排只允许动注释。
- **`#include_next` 被自己的副本兜住的陷阱**（DjyOS 章踩过）：`include/time.h` 里 `#include_next <time.h>` 原本并**不会**落到 newlib——旧的复制机制让 `-I../Include/` 里存在同名副本，include_next 找到它、又因 include 卫哨已被定义而整份跳过，等价于"newlib 的 time.h 从未被包含"。一旦取消复制，include_next 真正落到 newlib，newlib 声明的 `localtime_r(..., struct tm*)` 与内核 `time.c` 用扩展 `tm_djy` 写的同名定义立刻冲突。修法：`time.h` 明确不链 newlib（客机的 time.h 只有 DjyOS 这一份），time_t/timeval 由 `<sys/time.h>` 链里的 `sys/types.h` 提供。军规：**影子头里用 include_next 之前，先想清楚"它到底会落到谁头上"，以及副本消失后行为是否变化**。
- **注释批量重排的教训**（DjyOS 章踩过）：用脚本把块注释改成 `@` 风格时，凡"首行 `/*` 后带正文、末行 `*/` 前带正文"的块会被整行替换掉——文字静默丢失而 `.text` 依旧全等。所以"只动注释"的证明不够，**还必须抽查每块注释的首行与末行**（或先 dump 一遍块边界再改）；发现丢失按原文补回。
- **命名**：端口自有符号一律 `djy_`/`Djy_` 前缀（`djy_from_vm`、`Djy_Ctx_Handler`、`djy_attach_phys`…），与内核自有的 `DJY_*`/`Djy*` API、`Int_*`/`__Int_*` 模块符号区分开——这是有意的偏离：端口加进内核命名空间时必须一眼看出归属。
- **改 .c 不等于重编**、**改平台头不等于进工作区**——见 §13 军规 3。

### 10.9 空闲超级调用的调用点（掩码约束，2026-10-29）

- 内核空闲事件 `__DJY_Service` 的顺序是：`CleanWakeupEvent()`（`djyos.c:3403`）→ `Int_LowAtomStart()`（3404）→ 算 `AllowPendTicks` → `LP_EntryLowPower()`（3417）→ `Int_LowAtomEnd()`。
- 因此**只能在 `CleanWakeupEvent` 里停车**：此刻 `RVM_Int_Mask==0`。LP 入口已在原子段内（mask≠0），而 `RVM_Vct_Loop` 的定时器/ctx 处理被 `if(RVM_Int_Mask==0)` 挡住（`rvm_virtlib.c:695`）——在 `LP_EntryLowPower`（或它的 `EntrySleep`）里调用 `RVM_Hyp_Vct_Wait()`，线程会挂起且**永远不会被唤醒**（睡死）。
- 端口只保存应用给的指针（`djy_set_idle_hook`）；`CleanWakeupEvent` 先 pulse benchmark 驱动，再调 hook（`if(hook != NULL)`）。benchmark main 不注册 → 指针为 NULL → 忙等不变。
- 语义上 `RVM_Hyp_Vct_Wait` 对内核只是一次"这趟 idle 跑得久了点"，不进 DjyOS 自己的阻塞态，也不动任何内核数据结构。

---

## 11. 生成器知识

### 11.1 重建流程与坑

- 响应文件法：`g++ @resp.txt`（Release\resp.txt；~50 cpp + 预编译 xml.o；C++17/-O2/-static/-s）。**响应文件内路径必须正斜杠**（反斜杠被 GCC 当转义吃掉）；xml.c 是 C 不能混编 C++（用预编译 xml.o）。编译 ~60s 超 30s 红线 → Start-Process 分离跑，完成后校验 build_err.txt。
- **时间戳假新鲜**：旧 rvgen_new.exe 未删时 Test-Path 即真会换装陈货——换装前比对 exe 与 build_err 的 LastWriteTime。
- **resp.txt 输出名是 rvgen_new.exe**：重建完成后须手动复制部署到 Release\rvgen.exe 与 Utility\M7M02_Final\rvgen.exe 两处（benchmark.sh 用后者）。
- **命令行解析（2026-10-29 修）**：`rvm_gen.cpp` 的 `Main::Main` 是 if/else-if 级联，**每趟只解析一个选项**。旧代码是并列 `if` 级联，靠"参数按固定顺序排列"一趟吃完；一旦最后一项不是 `-b`（即普通工程），`-d` 的 `strcmp(argv[Count])` 读到 `argv[argc]`=NULL 直接段错误——普通工程从未生成成功过。新增选项必须挂进这条 else-if 链，不许另起并列 if。
- **测试选择头只在 benchmark 模式生成**（`djy_test.h`、每个 VM 的 `rvm_test.h`，生成条件就是 `Main::Benchmark!=0`，设计如此，不要放宽）：DjyOS 的做法是把**内核必需的板级胶水**放进端口 `board.c`（内核的 `SYS_Start` 要 `Board_Init`/`Sys_ModuleInit`/`__DjyInitTick`，所以它们必须在任何模式都链接），测试硬件（TIM2/TIM4）留在 `test/djy_board.c`，该文件只在 benchmark 分支进源码表——普通工程的源码表里没有任何 `test/` 文件，也不需要 stub。
- **客机生成器三件套 helper 家规**（FRT/UO3 同构）：`Plat_Get()`（平台→端口目录名）、`Toolchain_Get(Virt)`（工具链→目录名）、`Port_Dir_Get(Virt)`（合成端口路径）；hpp 声明+cpp 定义+完整函数注释块；Process_Virt_Hdr/Src 里调用之，不写裸三分支。
  - FRT 映射：A6M→ARM_A6M、A7M→ARM_A7M、RV32P→RISC-V_RV32P；ARMCC→RVDS、ICCARM→IAR、其余→GCC。
  - UO3 映射：A6M→ARMv6-M、A7M→ARMv7-M；ARMCC→ARM、ICCARM→IAR、其余→GNU（Micrium 命名）。
  - RTT 映射：A6M→armv6m、A7M→armv7m、RV32P→rv32p（libcpu 子目录名）；生成器架构通用，RTT_Gen 挂 -rtt 树。
- 平台兼容表：rvp 的 Guest 组合要在 `Include\Platform\<ARCH>\rvm_platform_*.rva` 的 `<Guest>` 与 `<Compatible>`（C 行=Buildsystem,Toolchain,Guest 三元组）里开；漏开报 "Combination 'X/Y/Z' is not found in compatible list"。
- 命名约定：Buildsystem=IAREW / 工程前缀=EWARM / Toolchain=ICCARM / 汇编后缀 _iccarm.s。

### 11.2 Guest/ 镜像规范 v1.0

- 定位=全部客机"我方增量件"的镜像暂存区；官方件永不入镜。update.sh：`rm -rf ./*/` 后逐条 `cp -r`（幂等）。
- 条目规则：尾随空格分词、无尾斜杠、大小写逐字；按客机分段、段内变体成对、测试收尾；粒度随客机原生分类；循环对目录用 `[ -d ]`、对单文件用 `[ -f ]`（DjyOS 的 benchmark 文件寄生在发行版自己的 `test/` 里，整目录镜像会把作者的示例测试一起卷进来，只能逐文件列），两者都不命中才 skip（可提前挂未来条目当 intent 文档）。
- 当前清单：FRT 8 条 + UO3 7 条 + RTT 6 条（libcpu 三端口 + test 树）+ DjyOS 5 条（`bsp/arch/arm/cortex-m/armv7e-m_rvm` 端口目录 + 3 个 test 源文件 + 1 个 chip 测试头）。

### 11.3 rvp 体系不变量与基础设施

- **rva 的 `<Compatible>` 一律按三元组 `(Buildsystem,Toolchain,Guest)` 字典序排列，索引 `C0..Cn` 连续**（生成器只做集合成员检查，顺序无语义；平台头 `<Coprocessor>` 的顺序同理只影响 `COP_NUM` 与按名 0/1 宏，不要为了"好看"去重排它——保持与既有生成物一致）。当前三份：A6M 15 条、A7M 16 条（多 DJY）、RV32P 5 条。
- **37 工程单一差异语法（终态不变量，全库字符级审计 0 违规）**：同芯片 rvp 两两差异仅三类——Guest_Type(1 行)/Coprocessor×4(NOFPU)/Buildsystem+Toolchain×11(跨链)；其余 350+ 行逐字符相同（含内存布局/优化级/缩进）。同芯片各链 RTOS 变体共用相同 Data 满配与内存布局（历史"方言"已收敛）。G071 的 hypervisor 侧 Optimization=Os 为 RMP/FRT/UO3 三链与 RTT（GCC，rvp 自 FRT 克隆承袭）共有的芯片级选择。
- **rvp 是从未跑过的资产=未验证资产，首次使用前先验完整性**（可解析/无 NUL 字节；曾发现 gccmf-g071-rmp 在 git 库中是损坏 blob——`git ls-files --eol` 出现 `i/-text` 的文本文件即坏 blob 信号）。
- **autobuild 检测定制正则（大小写不敏感）**：gcc `error:|fatal error|*** Error N`；IAR `Error[Pe..]`+非零 summary；Keil `error:`+`[1-9]+ Error(`；生成检测含 `^Error:` 变体。失败打印前 5 条错误原文；warning 全链计数报告（msys 无 bc，用 awk 求和）暂不判 FAIL。
- **autobench 探板**：`vcp_pair.ps1` 枚举 STLINK sn+COM 对，CubeProg 逐个 -c 查 Device ID 匹配芯片——多板环境自动寻址（勿写死 SN/COM）。

---

## 12. 成果基线索引（正式数据一律以 test 头文件档案块为准）

### 12.1 FRT（FreeRTOS-on-RVM）链状态

| 芯片 | GCC | Keil | IAR | 正式档案 |
|---|---|---|---|---|
| G071 (A6M) | ✅ 真机 | ✅ 真机 | ✅ 真机（Yield 408） | frt_test_stm32g071rb_rvm.h（GNU/ARMCLANG/ICCARM 三块） |
| F429 (A7M) | ✅ 真机（Yield 352→372 deferred 修复后） | ✅ 真机（300） | ✅ 真机（308→316） | frt_test_stm32f429zi_rvm.h（三块） |
| H743 (A7M) | ✅ 真机（Yield 228） | ✅ 真机（250） | ✅ 真机（220，三编译者最优） | frt_test_stm32h743zi_rvm.h（三块+历史注） |
| V307 (RV32P) | ✅ 真机（基线） | N/A | N/A | frt_test_ch32v307vc_rvm.h |

- 全矩阵编译回归：10/10 可测组合五工程全绿（V307×Keil/IAR 结构性 N/A——新树无 RV32P 的 RVDS/IAR 端口）。

### 12.2 UO3（uC/OS-III-on-RVM）链状态

| 芯片 | GCC | Keil | IAR | 正式档案 |
|---|---|---|---|---|
| G071 (A6M) | ✅ 真机（快路径 Yield 806） | ✅ 真机（915） | ✅ 真机（888） | uo3_test_stm32g071rb_rvm.h |
| V307 (RV32P) | ✅ 真机（9/9 全绿，Yield 455） | N/A | N/A | uo3_test_ch32v307vc_rvm.h（GNU 块 Note(2)） |
| F429 (A7M) | ✅ 真机（9/9，Yield 544） | ✅ 真机（9/9，Yield 481） | ✅ 真机（9/9，Yield 584→601） | uo3_test_stm32f429zi_rvm.h |
| H743 (A7M) | ✅ 真机（9/9，Yield 366 硬 CLZ 版） | ✅ 编译同享 | 端口同享（ewarm rvp 按需克隆） | uo3_test_stm32h743zi_rvm.h |

- P0+P1（已闭环）：同优先级 RR 编排；OSCtxSw=asm 任务栈直接换帧（官方 asm 风格），OSIntCtxSw=慢路径超调用；定档演进：慢路径 2840 → 快路径 806（同尘化后）。
- **同尘化彻底完成**：端口层+benchmark 层全部对齐 uC/OS-III 官方规范；重命名后成绩逐项不变。

### 12.3 RMP-on-RVM IAR 链

G071/F429/H743 三板全部真机闭环（rvm_test_stm32*.h 各 ICCARM 块）。G071 真机 Kernel.bin 102876B；H743 342476B / F429 230968B。

### 12.4 RTT（RT-Thread-on-RVM）链状态

| 芯片 | GCC | Keil | IAR | 正式档案 |
|---|---|---|---|---|
| G071 (A6M) | ✅ 真机 9/9（Yield 1073） | ✅ 真机 9/9（**940**，三编译器最优） | ✅ 真机 9/9（1035） | rtt_test_stm32g071rb_rvm.h（三编译器块） |
| V307 (RV32P) | ✅ 真机 9/9（Yield 533；RV32 无 ICI/IT 全快路径零竞态） | N/A | N/A | rtt_test_ch32v307vc_rvm.h |
| F429 (A7M) | ✅ 真机 9/9（**648**=全慢 2546 的 3.9×） | ✅ 真机 9/9（532） | ✅ 真机 9/9（612） | rtt_test_stm32f429zi_rvm.h（三编译器块） |
| H743 (A7M) | ✅ 真机 9/9（**快路径 Yield 495**） | ✅ 真机 9/9（449） | ✅ 真机 9/9（**424**，三编译器最优） | rtt_test_stm32h743zi_rvm.h（三编译器块） |

- **三编译器矩阵（2026-10-28 当日收官）**：armv6m/armv7m 各三份 context 汇编（gcc/iar/rvds 小写命名，rtt_gen 按 Toolchain_Get 自动映射 ARMCC→rvds、ICCARM→iar），指令流审计零语义差异（见 §9.5）。
- VSS 实况：H743-rtt=0x800、F429-rtt=0x1000、G071-rtt=0x800（㉙ 回退后 H743 未再改 0x1000，当日快路径全绿实证 0x800 足够——Vct 实测深度 280B）。
- VCT 栈深实测参考：M0+=68B、M7=280B（刷漆法）——各 RTOS tick 链深浅不同，新 RTOS 移植必须实测。

### 12.5 DJY（DjyOS-on-RVM）纪念性移植链

| 芯片 | GCC | Keil | IAR | 正式档案 |
|---|---|---|---|---|
| H743 (A7M) | ✅ 真机 11/11（Yield 2073，两次复位逐位相同） | N/A（单组合） | N/A（单组合） | djy_test_stm32h743zi_rvm.h |

- 组合唯一：**M7 + GCC + `-djy`**（纪念性移植，作者唯一工具链是 GCC，四板中仅 H743 有 DjyOS 官方板级包）。
- 11 项 = Yield / Notification / Activation / Semaphore / MsgQ / Memory + ISR Notification / Activation / Semaphore / Message queue / RVM relay。
- 判绿哨兵：`defers=0 padded=0 badframes=0 foreign=0` + `Benchmark complete`（细节与基线数字见 §10）。
- README 纪念性移植行已填：27.54 kB Flash / 63.38 kB SRAM（客机镜像含 32KB 静态堆与 2KB/事件类型栈）。

### 12.6 数据判读参考量级

- ICCARM(-Oh) 通常介于 ARMCLANG -O3 与 GNU -O3 之间（三板 FRT 一致）。
- M7 板（H743）必须开 L1 Cache 的数据才有效（4~5 倍差）。
- 低端芯片（G071，TIM3 5kHz 节拍）ISR 项需 90s+ 出齐。

---

## 13. 一句话军规

1. 内核文件（tasks.c / uC-OS3 Source/os_*.c / RTT src/*.c）永不改——要内核内部状态就 include 它（tasks_rvm.c 模式）。
2. 大文件 here-string 整写；写完校验行数与首尾；汇编逐函数核对。
3. 改了源头平台头≠进了工作区；改了 .c≠make 会重编——物证（反汇编/flash 指令字）说话；改端口文件后必 clean 全编。
4. "数据全垃圾"先查 RAM 黑洞再查逻辑（结论须双仪器交叉验证）；"卡住"用断点裁决别猜；fault 用现场活捉/面包屑取证（取证本身扰动布局，结论需无扰动复核）。
5. 单命令 30s 红线；跨超时用分离进程；ST-Link 被锁先杀 openocd；CubeProg 多板必带 sn=。
6. GCC≡Keil(RVDS) 的 C 件哈希必须一致；port 改完跑 update.sh。
7. **会话收尾三清**：删工作区、生成物收进 Project\Script\、结果写进 *_test_*_rvm.h 档案块；git status 干净。
8. iasmarm 的 `LDR Rx,=代码标号` 自动 +1 Thumb 位——只准用于跳转目标；数据指针必须 BIC #1。
9. IAR 链子镜像先、Kernel 最后重编（内嵌 bin 会过期）；改 .ewp 配置改生成器源码，别在 IDE 里改存。
10. benchmark 未见到 overflow 刷屏就没跑完；只见到 overflow 刷屏=旧固件，先找自家横幅。DjyOS 链的完成判据不同：`defers=0 padded=0 badframes=0 foreign=0` + `Benchmark complete`（§10.5）。
11. 拿不准 ST 外设用法，先抄 M5P01 同芯片工程+M0A00 Cube HAL——同板跑通的代码是唯一真理。
12. Pa078/Pa079（X=X= 双赋值）是本仓库高发笔误，一律源头修。
13. 自造 SR_Save/Restore 型掩蔽层时，**Save 必须返回进入前的旧状态**——返回新值会让配对的 Restore 永远回不到零 → 虚拟中断永久屏蔽 → tick/ctx 全灭。
14. **"关中断"假设的安全性 = 映射触碰的域**：CPSID/BASEPRI 是物理域操作，对 RVM 虚拟投递零作用；三层 gate 只认 `RVM_Int_Mask`。
15. **RVM 中断安全模型**：mask=0 时 Vct 线程（prio 3）在每条指令边界可抢占 Usr（prio 2）——客机 ISR=抢占式线程；mask=1 时 MASKABLE 投递被三层 gate 挡在门外（搁置不丢失、Unmask 补投）；透明向量按设计穿透。
16. **"被 gate 保护"≠"没被触发"**：低概率路径必须单独做窗口推演——板测覆盖率对它们≈0。
17. **跨架构/跨编译器抄汇编宏，必逐字核入口序/跳转极性/重载偏移/压弹对称**——宏注释里的 "N words pushed above" 是可执行规格（UO3 IAR 极性案 + RTT armv7m 错抄案两度验证；正确蓝本=RMP/UO3 的 A7M 版）。
18. **移植层数据消失类 bug 先挂无声计数器对账**（生产者/消费者/计数器）；**往高频 ISR 相加任何打印前先算打印耗时 vs 中断周期**。
19. **硬件结论双仪器交叉验证**；旧档案可被新证据推翻，存疑档案应标注验证方法与仪器。
20. **"偶发/布局敏感"故障先核对 bin 来源与源码状态**——旧脏 bin vs 新 bin 的行为差、同尺寸不同布局的 bin，都极易把已修复的 bug 误诊成玄学。
21. **事件驱动内核的"让下一个就绪者跑"本身就是一次切换请求**：在非线程上下文（启动容器、中断）里发起时，from 是个假的所有者——启动帧会被记到"正在运行的事件线程"名下。DjyOS 判据：StartOs 之前禁止 pop 出可运行事件（§10.2）。
22. **静态/一次性堆的 free 只在严格 LIFO 时真正回收**——create/delete 循环必须"先释放上一轮、再分配本轮"，并用空闲字节探针验证零漂移；批量回收会泄漏，堆尽后死在内核的重试循环里（§10.3）。
23. **永不返回的切入（first-entry/abandon）绝不能走延迟路径**：超调用返回会复活调用者，撕裂重启即由此而来；被放弃上下文的切换失败时也不能"保持当前"（§10.6 ⑮）。
24. **空闲超级调用写在应用 main（各 OS 自己的 idle hook），不写进端口**；端口只给机制。benchmark 保持忙等（FRT 的空 hook 是共享 `configUSE_IDLE_HOOK` 的必然产物）。
25. **停车点必须在虚拟掩码开的位置**：DjyOS 的 `CleanWakeupEvent` 在 `Int_LowAtomStart` 之前，`LP_EntryLowPower` 在原子段内——后者停车会睡死（§10.9）。

---

## 14. 历史判例总索引（一案一行，根因+教训；详情见对应章节）

| # | 系统/板 | 现象 | 根因 | 教训落点 |
|---|---|---|---|---|
| 1 | UO3 G071 | tick/ctx 全灭，黑匣子计数全 0 | CPU_SR_Save 返回新值非旧值→Restore 回不到 0 | §13 军规 13 |
| 2 | UO3 IAR A7M | INVSTATE 100% 必挂 | OS_CPU_ALIGN_PUSH 极性从 A6M 抄来没翻（3字/BEQ vs 2字/BNE） | §8.4；军规 17 |
| 3 | UO3 全 | 语义对齐 | CPU_SR 计数制→官方快照制直译（virtlib 函数须 extern 导出） | §8 CPU 侧 |
| 4 | UO3 F429 GCC/IAR | 75% 概率 INVSTATE | OSCtxSw C 包装无入口掩蔽，共享竞态窗口 | §8.4 |
| 5 | FRT 全端口 | 临界区内 yield 击穿 | 缺 deferred-yield 软件 pend | §8.4；军规 15 前身 |
| 6 | UO3/FRT/RMP 三家慢路径 | 慢路径裸窗 | Unmask→Yield 调序（应 pend-then-arm）；RTT 端口晚出，生而 pend-then-arm | §8.4；§6.4 |
| 7 | RMP V307 | FPU 帧 Instruction access fault | 未定义宏在 #if 静默求值为 0，同固件两种帧格式 | §14 注：任何引用 hypervisor 配置宏做编译期分支的文件必须自拉定义链或 #error；-Wundef 可现形 |
| 8 | H743 | "DTCM 高 64KB 写黑洞" | WCH fork OpenOCD 仪器假象（CubeProg 实测推翻） | §5.7；军规 19 |
| 9 | RTT G071 | Sftd 无限恢复循环 PC=context_gcc.s | rt_hw_context_switch_to 丢入参 STR | §9.1 |
| 10 | RTT G071 | ISR 相刷屏/饿死 | 逐 tick 诊断打印=自持活锁（putchar 超调用 87µs/字符） | §9.2-6；军规 18 |
| 11 | RTT G071 | 邮件 9993/10000 "停滞" | 入段打印吃掉 11 tick+池满 EFULL 丢 7 封（测试自伤非端口 bug） | §9.2-6；军规 18 |
| 12 | RTT A7M | 概率 fault+确定性自陷 | interrupt 版 fall-through 滑进快路径（Vct 上下文写入 Usr TCB） | §9.1 |
| 13 | RTT V307 | store access fault @swt | SAVE 宏内 call 破坏 a0——SWITCH 段必须从全局重载槽指针（UO3 `lw a0,OSTCBCurPtr` 的深意） | §9.1 |
| 14 | RTT H743 | "向量栈溢出"诊断 | SP 方向算反（满递减栈顶=高地址）；实际 Vct 深仅 280B | §4-3 |
| 15 | 全仓 | 同源重建 bin 抖动 ±12B | `__DATE__/__TIME__` 进 SHF_MERGE 字符串池跨秒合并骰子；定值化（kservice.c/rvm_syslib.h/rme_kernel.h 三处，程序见 git 历史或 §9.3） | §9.3；军规 20 |
| 16 | RTT H743/F429 | INVPC 崩环+快路径自陷+"布局敏感"玄学 | **armv7m 宏是 armv6m 错抄**（极性反/R0-XPSR 重载深度错/压弹不对称差一字）；脏 bin 时代错宏经 fall-through/残留块仍在执行，清理后从未复现 | §9.1；军规 17/20 |
| 17 | DJY H743 | 启动后 BADFRAME 连发，`to`=idle 的 `vm->stack` 落在容器栈 | StartOs 之前 `EventPop` 已触发调度，from 却是启动容器（非线程）→ 启动帧被记进 idle 线程，随后被启动栈覆盖 | §10.2；军规 21 |
| 18 | DJY H743 | Activation 横幅之后永久无输出 | 静态堆仅 LIFO 回收；批量回收每轮泄 ~2KB → 堆尽 → `__DJY_CreateThread` 返回 NULL → 内核建线程 `while` 死循环 | §10.3；军规 22 |
| 19 | DJY H743 | INVPC（"Invalid vector return LR or PC"） | 伪装帧 PC 存了偶数（缺 Thumb 位），而该帧要经内核真实异常返回恢复 | §10.6 ⑪ |
| 20 | DJY H743 | 复位后 djy_main 的 `while(1)` 复活、重启永不生效 | 永不返回的切入被走了延迟路径 → 超调用返回复活旧上下文 | §10.6 ⑮；军规 23 |
| 21 | 四客机 | 普通（非 benchmark）工程从未生成成功：rvgen 段错误退出 139 | 命令行解析是并列 if 级联，最后一参不是 `-b` 时读 `argv[argc]`=NULL → `strcmp` 崩 | §6.8/§11.1；军规 24 |
| 22 | 四客机 | 普通工程 idle 全速空转，从不调超调用 9（RVM 手册 §4.4.3 的硬性要求） | 各 OS 的 idle hook 全部缺席：FRT `configUSE_IDLE_HOOK 0`、UO3 指针恒 NULL、RTT 无 `RT_USING_HOOK`、DJY 的 `CleanWakeupEvent` 只 pulse benchmark | §6.8/§10.9；军规 24/25 |

