# RVM 客机库（Guest Library）

本目录分发 RVM 虚拟机监视器的**半虚拟化 RTOS 端口**：即插即用的移植层，
让未经修改的 RTOS 内核以非特权客机身份运行于 RVM 之上。

当前分发内容：

| 客机 | 版本 | 架构 | 工具链 |
|---|---|---|---|
| FreeRTOS | V9.0.0 | ARMv6-M、ARMv7-M、RV32P | GCC、RVDS (ARMCC)、IAR |
| uC/OS-III | uC/OS-3 V3.08.02 + uC/CPU V1.32.01 + uC/LIB V1.39.01 | ARMv6-M、ARMv7-M、RV32 | GCC、ARM、IAR |
| RT-Thread | V4.1.1 | ARMv6-M、ARMv7-M、RV32P | GCC、RVDS (ARMCC)、IAR（RV32P 仅 GCC） |
| DjyOS | V1.1.0 | ARMv7-M | GCC（纪念性移植，单组合：H743） |
| KLite | V5.2.1 | ARMv7-M | GCC（纪念性移植，单组合：H743） |
| ChibiOS（CHB） | 21.11 LTS | ARMv7-M（H743/F429）、RV32P（CH32V307） | GCC、IAR EWARM、Keil（RV32P 仅 GCC） |
| uC/OS-II | V2.93 | ARMv6-M、ARMv7-M、RV32 | GCC、ARM、IAR（RV32 仅 GCC） |
| RTEMS（RTE） | 7.0-rc1 | ARMv7-M（H743/F429） | GCC |
| Zephyr（ZPR） | 3.7.0 LTS | ARMv7-M（H743/F429） | GCC |

RTE 以 `RTEMS_V701` 的 BSP/spec/样例子集分发。构建期生成的 staging 文件
（`prc_virt1.c`/`_desc.c`、`rte_prc.h`、`rte_config.h`、`app/`）由 RTE_Gen
和 `rte_copy.sh` 产生，不在镜像中。RTE BSP 通过 `RVM_ROOT` 引用 M7M02 的
RVM `Include/` 与 `Source/`；生成的 VM 头由 `rte_copy.sh` 暂存，共享的
`rvm.h` 与平台头直接引用，BSP 源码树不维护任何 RVM 实现。RTE 默认使用任务栈上的
直接上下文切换快路径；生成的 `rte_config.ini` 里的 `RTE_FAST_PATH` 设为
`False` 时，所有切换都走 RVM 向量线程，且汇编快路径例程不会编入镜像。
已验证 Cortex-M7/STM32H743 与 Cortex-M4F/STM32F429（板卡为 NUCLEO-F439ZI，
同 0x419 平台）两种芯片；CPU/FPU 与 float-ABI 选项由 `RTE_CPU_FLAGS` 按
`.rvp` 的 coprocessor 自动选择。BSP spec 的布局默认值是 0 哨兵，`linkcmds`
用 `ASSERT` 要求所有布局值来自生成的 `rte_config.ini`，直接构建不会静默
复用旧内存映射。RTEMS 构建使用 BSP 的 `-O2` 优化选项；性能档案放在
`testsuites/samples/rte_benchmark/chip/rte_test_<chip>.h`。同一个芯片头还负责
benchmark 的 TIM2/TIM4 初始化与中断清理
（`RTE_Test_Timer_Init`/`RTE_Test_Int_Init`/`RTE_Test_Int_Disable`/
`RTE_TEST_INT_CLEAR`）；`init.c` 只定义 handler 和测量流程，与 RMP 的测试头
分工一致。基准工程（`-b`）
会给每个 Virtual 客机都编 benchmark；该 Virt 的 `Source/app/` 里若有用户
`.c`，用户 app 优先。普通 VM 没有用户源时，生成器写一个与 SoC 无关的桩：
打印 `RTE guest <vm>: no application selected`，永不离开 `Init`，不用时钟
或外设，因此工程仍能链接并启动。BSP 的入口、配置与 app 文件始终从当前构建
的 Virt 重新暂存，因此两个 RTE 客机可以共用一个 RTEMS 树串行构建而不串镜像；
`rte_copy.sh` 在复制前删除上一个 VM 的 `prc_*.h` 和 app 目录，复制失败时
清理半成品。上下文帧的偏移和字长集中在
`bsps/arm/v7m_rvm/include/rte_frame.h`，由 C 上下文代码和汇编切换共同
include；C 侧静态断言检查 `Context_Control` 偏移，防止两份常量漂移。
初始帧一律为基本帧，与原生 ARM“线程启动时尚无 FP 状态”的规则一致；
`is_fp`/`RTEMS_FLOATING_POINT` 存入 TCB 但不参与初始帧选择。第一条 VFP
指令置 FPCA，下一次保存才生成扩展帧；`rte_two_task_fpu` 已按此方式跑通。

安装方式是整文件覆盖：

1. 准备一份完整的官方 RTEMS 7.0-rc1 树。
2. 把本 `RTEMS_V701/` 树覆盖上去，同名文件选择覆盖。

RVM CPU port 是新目录 `cpukit/score/cpu/arm/v7m_rvm/`，加上 `cpuarm_rvm`
spec 和 `cpuarm`/`librtemscpu`/`optincludes` 的选择改动；共享的
`cpukit/score/cpu/arm` 源码未修改。新 port 共 4 个 `cpukit` 文件：3 个
`v7m_*_rvm` 源文件和 `include/rtems/score/cpu.h` overlay。CPU 初始化/ISR level
包装已并入 `v7m_rvm.c`；`bsp_start()`/`bsp_reset()` 并入 `start/rvm_entry.c`，
dummy `start-file` 已删除。其余 `cpukit`、
RTEMS 其他目录和 BSP 公共代码都来自官方树。**不要把原版 cpukit 拷进这个
增量树，也不要把它当独立 RTEMS 树使用**：它是覆盖包，不是完整树。如果
要另建构建目录，先把完整官方 RTEMS 树复制过去，再覆盖本 `RTEMS_V701/`。
版权头保留，不需要 patch 工具。

**RMP 不在本库分发**：RMP 原生自带 RVM 平台层（RMP 源码树的
`Source/Platform/*_RVM/`），无需客机侧打包。

本目录内容是由 `update-maintainer-only.sh` 生成的**镜像**；请勿直接在此编辑——请修改
源码树后在本目录重新执行 `./update-maintainer-only.sh`。

### 本库周边的仓库布局

本库是一个十一仓库体系的组成部分。工程生成器（`rvgen`）按相对路径定位
各仓库，请将它们并排放在同一根目录下：

```
<根目录>/
├─ M7M02_Ammonite/     RVM 虚拟机监视器侧总仓——本库就在其 Guest/ 下
│  ├─ Include/         端口编译所依赖的监视器接口头
│  │                   （rvm.h、Virtlib/、Syslib/、Platform/）
│  ├─ Source/          Monitor、虚拟化库、平台驱动
│  ├─ Project/         工程生成器（rvgen）、benchmark.sh、.rvp 工程文件
│  ├─ Guest/           本库（镜像，由 update-maintainer-only.sh 刷新）
│  └─ Utility/         生成器源码与主机工具
├─ M7M01_Eukaron/      RME 微内核——RVM 所构建于其上的能力内核
├─ M5P01_Prokaron/     RMP RTOS——自带 RVM 平台层（无需客机端口）
├─ FreeRTOS_V900/      FreeRTOS 源码树——移植层源文件在此，Guest 是其镜像
├─ uCOSIII_V308/       uC/OS-III 源码树（uC-OS3 + uC-CPU + uC-LIB）——同上
├─ RTThread_V411/      RT-Thread V4.1.1 源码树——端口在 libcpu/*_rvm，测试在 test/
├─ DjyOS_V110/         DjyOS V1.1.0 源码树——端口在 bsp/arch/arm/cortex-m/armv7e-m_rvm，
│                      基准在 test/
├─ KLite_V521/         KLite V5.2.1 源码树——端口在 sources/cpu/armv7-m_rvm，
│                      基准在 test/
├─ uCOSII_V293/        uC/OS-II V2.93 源码树——端口在 Ports/*-RVM，基准在 Test/
├─ ChibiOS_V2111/      ChibiOS 21.11 LTS 源码树——CHB 端口在
│                      os/common/ports/ARMv7-M-RVM + RV32P-RVM，基准在 test/
├─ RTEMS_V701/         RTEMS 7.0-rc1 源码树——RTE BSP/spec/样例子集覆盖包，
│                      不是完整 RTEMS 树
└─ Zephyr_V370/        Zephyr 3.7.0 LTS west 工作区覆盖包——ZPR 架构端口、
                       RVM 板级支持与 ZPR 基准应用
```

按下一节的方法把本库覆盖到你的官方 FreeRTOS / uC/OS-III / RT-Thread /
DjyOS / KLite / ChibiOS / uC/OS-II / RTEMS / Zephyr 源码树之后，那些源码树就顶替了
上表布局中 `*_Vxxx/` 的位置。

---

## 1. 安装

本库的目录结构就是目标源码树相对布局的镜像，且
**只包含我方新增或修改的文件**——官方内核文件一律不在镜像内，因此安装
不会删除你的文件；唯一会覆盖的官方文件是 RTE 的 `spec/build` 选择文件、RTEMS 顶层
`wscript`，以及 Zephyr 的 arch 选择文件；RVM 板级/SoC 目录是新增内容，
每个端口都自然落到正确位置。

- 把 `FreeRTOS_V900/` 整体覆盖到你的官方 FreeRTOS **V9.0.0** 源码树根目录
  （目录合并）。
- 把 `uCOSIII_V308/` 整体覆盖到你的官方 Micrium uC/OS-III 发行包根目录
  ——三组件组合 **uC/OS-3 V3.08.02 + uC/CPU V1.32.01 + uC/LIB V1.39.01**
  （目录合并；覆盖只向 uC/OS-3 与 uC/CPU 增加端口文件——uC/LIB 仅为构建
  依赖、保持原样，本库不为它分发任何文件）。
- 把 `RTThread_V411/` 整体覆盖到你的官方 RT-Thread **V4.1.1** 源码树根目录
  （目录合并；覆盖新增三个 `libcpu/*_rvm` 端口目录与 `test/` 下的基准树）。
- 把 `DjyOS_V110/` 整体覆盖到 DjyOS **V1.1.0** 发行包根目录（目录合并；
  覆盖新增 `bsp/arch/arm/cortex-m/armv7e-m_rvm/` 与 `test/` 下四个基准
  文件）。这是纪念性移植，只有单一组合：Cortex-M7 + GCC + H743 板。
- 把 `KLite_V521/` 覆盖到你的 KLite **V5.2.1** 源码树根目录（合并目录；
  覆盖添加 `sources/cpu/armv7-m_rvm/` 与 `test/` 下的基准文件）。同为
  纪念性移植，单组合 Cortex-M7 + GCC + H743 板——给工程生成器传
  `-klt` 旗标即可使用。
- 把 `uCOSII_V293/` 覆盖到你的官方 Micrium uC/OS-II **V2.93** 源码树根目录
  （合并目录；覆盖添加 ARMv6-M、ARMv7-M、RV32 的 `*-RVM` 端口与 `Test/`
  基准树）。

### 先搭环境、装工具链、跑通原生应用（RTE/ZPR/CHB）

不要一上来就编译虚拟化镜像。先准备客机源码树和对应工具链，确认未经 RVM 覆盖的原生应用能编译、链接，再覆盖 RVM 端口：

- RTEMS：安装 `arm-rtems7-gcc` 15.2.0（含 Cortex-M3/M4/M7 的 newlib 与
  C++ multilib），配置并构建官方 RTEMS **7.0-rc1** 树，再构建原生 BSP
  样例，例如 `arm/stm32f4` 的 `hello.exe`（C++ 路径再加 `iostream.exe`）。
- Zephyr：建立 **Zephyr 3.7.0 LTS** 的 west 工作区，安装 `west`、
  CMake/Ninja 与 GNU Arm 工具链（或 Zephyr SDK），先用 `west build` 构建
  官方板级样例，例如 `nucleo_h743zi` 的 `hello_world`（M4F 路径用
  `nucleo_f429zi`）。
- ChibiOS：使用 **21.11 LTS** 源码树和 GNU Arm 13.2.1 工具链，先构建官方
  `demos/STM32/RT-STM32-MULTI` 的 H743 配置（MinGW 下加 `USE_LTO=no`，
  LTO wrapper 在该环境无法生成临时 makefile），原生构建通过后再加 CHB
  RVM 端口。

原生构建通过后，再覆盖 `RTEMS_V701/`、`Zephyr_V370/` 或 `ChibiOS_V2111/`，
并构建 RTE/ZPR/CHB 虚拟化镜像。

- RTEMS：准备一份完整的官方 RTEMS **7.0-rc1** 树，再把 `RTEMS_V701/`
  覆盖上去，同名文件选择覆盖。本目录是覆盖包，不是独立 RTEMS 树；不要把
  原版 `cpukit` 拷进本目录。生成工程时传 `-rte`；生成的 makefile 从 PATH
  找 `arm-rtems7-gcc`，可用 `RTEMS_PREFIX` make 变量覆盖。
- Zephyr：先准备一份 **Zephyr 3.7.0 LTS** 的 west 工作区，再把
  `Zephyr_V370/` 覆盖上去，同名文件选择覆盖（`zephyr/` 下的 arch/SoC 文件
  是有意保留的 RVM 改动）。生成工程时只传 `-zpr <west-root>`；
  生成的 makefile 从 PATH 找 `arm-none-eabi-gcc` 和 `west`，可用
  `PREFIX`/`WEST` make 变量覆盖。覆盖包只含端口、一块 RVM 板与基准应用，不含第二份 Zephyr
  源码树。
- ChibiOS：先准备一份 **21.11 LTS** 源码树，再把 `ChibiOS_V2111/`
  覆盖上去，保留新增的 `os/common/ports/ARMv7-M-RVM` 与 `test/` 文件。
  生成工程时传 `-chb <tree>`；GCC 变体使用标准 RVM makefile，IAR/Keil 则
  由 RVM 构建层生成 EWARM/uVision 工程，编译同一份 CHB 源列表。

覆盖后的目录结构（RVM 端口目录与官方端口目录并排，就像额外的官方端口）：

```
FreeRTOS_V9.0.0/                      <- 你的官方源码树根
└─ portable/
   ├─ GCC/
   │  ├─ ARM_CM0/  ARM_CM3/  ARM_CM4F/ ...    <- 官方，原样不动
   │  ├─ ARM_A6M_RVM/                          <- 覆盖新增
   │  ├─ ARM_A7M_RVM/                          <- 覆盖新增
   │  └─ RISC-V_RV32P_RVM/                     <- 覆盖新增
   ├─ RVDS/  ... ARM_A6M_RVM/  ARM_A7M_RVM/    <- 同上
   └─ IAR/   ... ARM_A6M_RVM/  ARM_A7M_RVM/    <- 同上
                                       test/   <- 与镜像的 test/ 合并

uCOSIII-V3.08/                        <- 你的官方发行包根
├─ uC-OS3/
│  ├─ Ports/
│  │  ├─ ARM-Cortex-M/
│  │  │  ├─ ARMv6-M/  ARMv7-M/                <- 官方，原样不动
│  │  │  ├─ ARMv6-M-RVM/                       <- 覆盖新增
│  │  │  └─ ARMv7-M-RVM/                       <- 覆盖新增
│  │  └─ RISC-V/
│  │     ├─ RV32/                              <- 官方，原样不动
│  │     └─ RV32-RVM/                          <- 覆盖新增
│  └─ Test/                                    <- 合并
└─ uC-CPU/
   ├─ ARM-Cortex-M/
   │  ├─ ARMv6-M/  ARMv7-M/                    <- 官方，原样不动
   │  ├─ ARMv6-M-RVM/                           <- 覆盖新增
   │  └─ ARMv7-M-RVM/                           <- 覆盖新增
   └─ RISC-V-RVM/                               <- 覆盖新增

RT-Thread-V4.1.1/                     <- 你的官方源码树根
├─ libcpu/
│  ├─ arm/
│  │  ├─ cortex-m0/  cortex-m3/  ...           <- 官方，原样不动
│  │  ├─ armv6m_rvm/                            <- 覆盖新增
│  │  └─ armv7m_rvm/                            <- 覆盖新增
│  └─ risc-v/
│     └─ rv32p_rvm/                             <- 覆盖新增
└─ test/                                       <- 合并

DjyOS-V1.1.0/                         <- 你的官方发行包根
├─ bsp/arch/arm/cortex-m/
│  ├─ armv7e-m/                                <- 官方，原样不动
│  └─ armv7e-m_rvm/                            <- 覆盖新增
└─ test/                                       <- 合并（仅四个基准文件）

uC/OS-II-V2.93/                       <- 你的官方发行包根
├─ Ports/
│  ├─ ARM-Cortex-M/ARMv6-M-RVM/  ARMv7-M-RVM/  <- 覆盖新增
│  └─ RISC-V/RV32-RVM/                         <- 覆盖新增
└─ Test/                                       <- 合并基准树

RTEMS-7.0-rc1/                        <- 先准备完整官方 RTEMS 树
├─ bsps/arm/v7m_rvm/                           <- RTE BSP，覆盖新增
├─ cpukit/score/cpu/arm/v7m_rvm/               <- 新 CPU port，覆盖新增
├─ spec/build/bsps/arm/v7m_rvm/                <- BSP/spec 选项，覆盖新增
├─ spec/build/cpukit/cpuarm_rvm.yml            <- 新 CPU port spec，覆盖新增
└─ testsuites/samples/rte_*/                   <- RTE 样例/基准，覆盖新增
```

### 1.1 配置宏

FreeRTOS（写在 `FreeRTOSConfig.h`）：

| 宏 | 含义 | 备注 |
|---|---|---|
| `configUSE_RVM_OPTIMISED_YIELD` | 1=任务栈快路径切换；0=纯超调用让出 | 0 的 Yield 延迟约劣化 4 倍，语义完全相同 |
| `configUSE_PORT_RVF` / `configUSE_PORT_RVD` | RV32P 单/双精度 FPU 帧 | 二选一；须与进程头中声明的协处理器一致 |

uC/OS-III（写在端口的 `os_cfg.h`，各编译器目录各一份）：

| 宏 | 含义 |
|---|---|
| `OS_CFG_RVM_FAST_YIELD_EN` | 1=任务栈快路径切换；0=纯超调用让出 |

RT-Thread 与 DjyOS 没有切换策略开关（策略固定：干净帧走快路径、脏帧走
延迟路径；DjyOS 的事件完成转身一律不延迟）。它们各有一条构建级要求：

| 客机 | 要求 |
|---|---|
| RT-Thread | 定义 `RT_USING_LIBC`（内核 `va_list` 的回退分支只在"定义 `__GNUC__` 且未定义 `__ARMCC_VERSION`"时存在；ARM Compiler 与 IAR 必须走 libc 头） |
| DjyOS | 客机必须以 `-mfloat-abi=soft` 编译（端口只构建整数帧；浮点 ABI 会被 `cpus.s` 的 `#error` 直接挡下） |

RTEMS/RTE（写在生成的 `rte_config.ini`）：

| 宏 | 含义 | 备注 |
|---|---|---|
| `RTE_FAST_PATH` | 1=干净任务帧在任务栈上直接切换；0=所有切换都走 RVM 向量线程 | 设 `False` 时汇编快路径例程也不会编入镜像 |
| `RTE_CPU_FLAGS` | 所选 Cortex-M 变体的 `-mcpu/-mfpu/-mfloat-abi` | `RTE_Gen` 按 `.rvp` coprocessor 自动选择：FPV4_SP→M4F，FPV5_SP/FPV5_DP→M7，无 coprocessor→soft-float |

### 1.2 工程生成

所有客机都由 RVM 工程生成器驱动（RVM `Project/` 目录下的 `rvgen`，
经 `benchmark.sh` 调用）。`.rvp` 工程文件选择芯片、平台与客机类型
（`<Guest_Type>FRT|UO3|RMP|RTT|DJY|KLT|UO2|RTE</Guest_Type>`）；生成器把
端口文件拷入工作区，并产出构建系统（Makefile / IAR / RVDS），include
顺序已排好。手工建工程可行，但不在此文档范围内。RTE 在构建侧是例外：
它的 Virt makefile 先跑 `rte_copy.sh`，再驱动 RTEMS waf，而不是直接编译
一份复制过来的端口树。

`generate.sh` 走同一个生成器但**不带** benchmark 选项：客机由
`<guest>_main.c` 里的普通应用启动，该文件被生成器复制成工作区的
`main.c`（含 §2.10 的空闲停车 hook）。两个脚本必须给同一组源码根。

### 1.3 布局归属

VM 的内存布局由生成器拥有，只有测试接线是逐芯片的。

- **ZPR**：固定一块板 `armv7m`。生成器为每个 Virt 写完整
  `zpr_board.dts`（CPU compatible、`flash0`/`sram0` 窗口、`chosen`），
  生成的 makefile 以 `DTS_SOURCE` 选中它；`prj.conf` 只放 CPU 选择、
  栈、事件与堆。没有 M7/M4 板级 variant。
- **RTE**：`rte_config.ini` 提供全部代码/数据/栈值，`linkcmds` 把它们
  替换进 `MEMORY`/`__RVM_Stack`；spec 默认值是 0 哨兵，`linkcmds` 断言
  这些值来自生成器。CPU/FPU 与 float ABI 来自 `RTE_CPU_FLAGS`。
- **DjyOS**：端口 `template/*.h`（board/project/component 配置）复制到
  `<Virt>/Include/`；已存在则不覆盖，用户可逐 VM 修改 clock/tick/stack/
  component 限制。上下文 PC 校验用生成的 `CODE_CODE1_BASE`/
  `CODE_CODE1_SIZE`，`board.c` 断言内核栈放得进 `DATA_DATA1_SIZE`。

各客机 `test/chip` 下的逐芯片文件只是 benchmark 接线（TIM/RCC/IRQ/ARR），
不属于普通 VM 镜像，也不属于 VM 内存布局。

---

## 2. 各端口的契约与移植策略

本库中每个端口都做同样五件事：临界区、让出（yield）、上下文切换、
中断投递钩子、启动。各 OS 的差异在于**内核自身保证什么**——即*契约*——
以及由此决定端口要独立承担多少——即*策略*。

### 2.1 内核侧契约（内核固有属性，与平台无关）

| | FreeRTOS | uC/OS-III | RMP | RT-Thread | DjyOS |
|---|---|---|---|---|---|
| Yield 请求的状态 | **任意**——内核会在嵌套临界区内发起 yield，`tasks.c` 注释明文承认合法 | **恒为掩蔽态**——`OSSched()` 用自己平坦的一层临界区包住"选择+请求"（`Disable; Yield; Enable`） | 已归一——锁态请求转为 Pend 标志，由内核在解锁点补发 | **任意**——`rt_schedule()` 在持调度锁、中断已掩蔽的临界区内被调用 | **恒为掩蔽态**——选择与切换都在内核的异步信号掩蔽内完成 |
| 对切换时机的要求 | 无（立即或推迟都正确，内核注释自白） | 无（`Enable` 幂等，其前其后切换皆安全） | 该问题到不了端口（内核已把一切推迟掉） | 内核侧无要求；但官方端口一律推迟到 PendSV | 切换必须在**解掩蔽之前**完成（脏帧例外：它需要一次真异常返回） |
| 要求端口提供的中断原语 | 无条件开关对 + 嵌套计数临界区对 | 无条件开关对 + 快照 save/restore 对 | 单一掩蔽原语 | 单一快照式 disable/enable 对 | 单一 mask/unmask 对（快照语义） |
| 端口义务 | 让 yield 在**任何状态**下都安全 | 切换完成时新任务以**开中断态**起步 | 只处理一种请求形态：开态立即切换 | 消费内核已选好的 from/to（`rt_current_thread` 已预更新），并保证后续请求不会撕裂帧 | 搬运事件周转帧，且永不推迟"不返回的转身" |

### 2.2 共同的 RVM 切换模板

所有 RVM 端口收敛到同一骨架（RMP 是原作，FreeRTOS、uC/OS-III、RT-Thread
与 DjyOS 端口都是它的移植）：

```
入口：RVM_Virt_Int_Mask()                    -- 无条件、总是先关
       窥探/消费内核的选择                    -- 附加脏帧检查
       |- 无需切换   -> 解掩蔽，返回
       |- 干净的帧   -> asm 快路径：任务栈上直切，掩蔽下完成调度簿记，
       |                出口解掩蔽
       +- 脏 ICI/IT 帧 -> 慢路径：解掩蔽，超调用（真异常返回恢复目标）
```

### 2.3 FreeRTOS：契约、策略与微妙出入

**契约**：零状态保证、零时机要求。

**策略**：模拟官方 PendSV pend 的**软件延迟让出**——`vPortYield()` 先掩
蔽，再读内核的临界区嵌套计数：非零则只置 `xPortYieldPend` 并返回；
`vPortExitCritical()` 在计数归零时补做切换（先切后开）。计数为零时快路
径在入口掩蔽保护下立即切换。

**出入与为何无碍：**

1. *官方参考实现用硬件 PendSV pend 实现延迟——状态无关、免掩蔽；我们
   用软件标志——被迫入口掩蔽 + 读计数。* 无碍原因：软件 pend 精确复刻
   了 PendSV 的时序语义（请求只被记录，执行推迟到临界区退出之后）；
   入口掩蔽买回了硬件标志免费享有的状态无关性。
2. *在可证明为开态的调用点（如 `xTaskResumeAll` 之后的队列发送）上，
   入口掩蔽是冗余的。* 无碍原因：掩蔽幂等，代价仅数个时钟周期——这
   是为保住官方"任意状态可调"契约对所有现存与未来调用点成立而付的价。


### 2.4 uC/OS-III：契约、策略与微妙出入

**契约**：`OSSched()` 是 `Disable; Yield; Enable` 三段式。Yield 请求恒在
掩蔽态发出；两种切换时机（Enable 前后）都正确。

**策略**：在调用点立即切换。快路径把调用者上下文伪装成异常帧，在掩蔽
下完成官方 PendSV 的簿记三件套，载入新栈，出口解掩蔽——新任务以**开中
断态**起步。

**出入与为何无碍：**

1. *直切型参考端口（MicroBlaze 等）在每任务帧里保存/恢复中断状态；我
   们不恢复任何状态——新任务直接开态起步。* 无碍原因：内核自身的
   `D;Y;E` 结构中 `Enable` 无条件且幂等，因此开态起步与官方 CM 端口的
   行为逐位等价（官方在 `Enable` **之后**切换，新任务同样开态恢复）。
   切换点到 `Enable` 之间的代码不触碰任何受保护数据——切换点就是临界
   区的语义边界。
2. *`OSCtxSw()` 入口掩蔽是冗余的——唯一合法调用者 `OSSched` 已经掩蔽
   过了。* 无碍原因：这是"入口自足"原则——官方 pend 型 `OSCtxSw` 可在
   任意状态被调；入口掩蔽让直切实现保住了同一条契约。
3. *脏帧慢路径在掩蔽下 pend 切换、由解掩蔽激活：`RVM_Virt_Yield()` 在
   掩蔽态只置切换标志与 pending 标志（既不发信号也不切换——yield 超调用
   在发送侧检查 `RVM_Int_Mask`，切换端点在执行侧再受一次 gate），随后
   的解掩蔽在其超调用边界补发——切换与新任务的开态起步一次性完成，中
   间不存在任何开态裸窗。下述抢占自愈是双保险论证：每条把被挂起任务
   切回的路径都会先重选 `OSTCBHighRdyPtr`，补做的让出退化为 no-op。*

### 2.5 RMP（供参照——不随本库分发）

**契约**：锁与掩蔽是同一个原语；锁态请求转为内核 Pend 标志、在解锁点
补发；端口只会见到开态的立即切换请求。

**策略**：C 层让出入口掩蔽（asm 里 `masked externally` 注释指的就是
它）、窥探，然后要么任务栈直切（`_RMP_Run_High` 在切换内做调度），要
么走超调用慢路径。

**出入与为何无碍**：asm 信任"外部掩蔽"——形式上同 MicroBlaze 式契约，
但掩蔽不是靠注释约束调用者，而是由**同一个 C 函数的入口**提供——双层
结构把契约从文档编译进了代码。

### 2.6 RT-Thread：契约、策略与微妙出入

**契约**：`rt_schedule()` 在持调度锁、中断已掩蔽的临界区内选出下一线程，
**先把 `rt_current_thread` 预更新**，再调端口。切换请求点封闭（任务级
一个函数、中断级一个函数）。

**策略**：锁存 from/to 与请求旗标；干净帧在任务栈上直切，脏 ICI/IT 帧
（以及中断级请求）交给上下文切换处理器在超调用边界消费锁存。处理器
保留官方 PendSV 的两道守卫（flag==0 直接返回、from==0 跳过保存）——正是
它们让"请求过期后的残留锁存"不会补做一次陈旧切换。

**出入与为何无碍**：官方端口一律推迟到 PendSV；我们对干净帧立即切换。
无碍原因：入口先掩蔽、造的帧是真异常帧，而所有把半切换上下文切回的
路径都会经 `rt_schedule` 重新选择——补做的请求退化为 no-op。
`rt_current_thread` 的预更新则是"快路径恢复途中被抢占"仍然安全的原因：
投递锁存到的 from 槽，正是那个上下文只恢复了一半的线程自己的槽。

### 2.7 DjyOS：契约、策略与微妙出入

**契约**：DjyOS 调度的是**事件**，线程（`ThreadVm`）只是可复用的容器。
所有切换都发生在内核异步信号掩蔽抬起时，且下一事件的选出就在切换内部
（`__DJY_Schedule` 里调 `__DJY_SelectEventToRun`）——端口不需要窥探。

**策略**：一条 RVM 用户线程承载全部 DjyOS 线程，所以端口把当前上下文
伪装成线程栈上的 22 字异常帧来切换：5 个超调用参数、R4-R11、保存的
EXC_RETURN、8 字基本硬件帧。脏帧（EPSR ICI/IT）走延迟路径，交给内核
自己的异常返回来恢复手工弹栈恢复不了的状态。事件完成用**转身**（不保存
已耗尽上下文、直接切入下一个）；转身永不延迟。

**出入与为何无碍**：帧里的 PC 存奇数 Thumb 位，因为这帧也会被内核的
异常返回消费；客机按软浮点编译，因为端口只构建整数帧。

**布局与可编辑配置**：上下文 PC 校验使用生成的 `CODE_CODE1_BASE`/
`CODE_CODE1_SIZE`，`board.c` 静态断言内核栈放得进 `DATA_DATA1_SIZE`。
端口 `template/*.h` 配置头会复制到 `<Virt>/Include/`，已存在则不覆盖，
因此 clock/tick/stack/component 限制可按 VM 修改；`template/` 只是种子，
不在编译 include path 里。

**Benchmark 接线**：benchmark 的芯片相关代码全部在
`test/chip/djy_test_stm32h743zi_rvm.h`：`djy_bench_timer_init`（TIM2）、
`djy_bench_int_init(handler)`（DjyOS 中断线注册、`djy_attach_phys`、TIM4）
和 `DJY_BENCH_INT_CLEAR`。`test/djy_board.c` 只保留 benchmark VM 入口/引导，
`test/djy_benchmark.c` 只定义 `djy_bench_isr` 和测量流程，与 RMP 的
测试头分工一致。


### 2.8 KLite：契约、策略与微妙出入

**契约**：KLite 天生纯延迟——内核在临界区内调 `cpu_contex_switch()`，
官方 M 系列端口在那里只 pend 一个 PendSV，真正的切换发生在外层
`cpu_leave_critical()` 重开中断时。void/void 递归临界区 API 由端口
内部的嵌套计数承载，且调度器在调端口**之前**就把目标预选进了
`sched_tcb_next` 全局。

**策略**：RMP 形态——`cpu_contex_switch()` 只置端口旗标（在向量线程上
再补一个 `RVM_Virt_Yield()`，即 RMP_YIELD_ISR 臂）；最外层
`cpu_leave_critical()` 经 yield 分派器消费：自切吃掉旗标、脏帧布防
慢路径、干净帧走 `cpu_asm_gcc.s` 的伪装快路径（RMP 金标准换上 KLite
的两个调度全局，含弃启动栈的首切分支）。所有上下文冻结点都落在
嵌套计数==0——这正是共享 void/void 计数无需每线程状态而保持自洽的
原因。

**出入与为何无碍**：内核只通过官方端口契约使用——`kernel.h`、两个
调度全局、`stack` 为 `struct tcb` 首字段（官方 PendSV 自己依赖的
偏移 0 盲解引用）；脏帧 peek 经同一契约读目标保存的 XPSR，不 include
也不修改任何内核内部。KLite 的加锁/调度类内核调用（一个
`queue_create` 就要拿互斥）要求 `sched_tcb_now` 非 NULL，所以基准与
普通应用都**在线程里**创建内核对象、绝不在 `kernel_start()` 之前——
与官方例程的用法完全一致。

### 2.9 uC/OS-II：契约、策略与微妙出入

**契约**：uC/OS-II V2.93 自含（不依赖 uC-CPU / uC-LIB），天生纯延迟——
内核在调度临界区内经 `OS_TASK_SW()` 调 `OSCtxSw()`，官方 M 系列端口在那里
pend 一个 PendSV。每优先级一任务（无轮转）；调度器在调端口**之前**就把目标
预选进了 `OSTCBHighRdy` 全局。

**策略**：uC/OS-III 形态的直接翻译——`OS_CPU_SR_Save/Restore` 是端口内的
C 函数，映射到 RVM 虚拟中断掩码快照；`OSCtxSw()` 是 C 过滤器，净 ICI/IT 帧
走 `os_cpu_a.s` 的汇编快路径，脏帧走 `RVM_Virt_Yield()`；`OSIntCtxSw()` 即
yield 超调用；`OS_Ctx_Handler()` 和 `OS_Tick_Handler()` 经
`RVM_Virt_Ctx_Reg()` / `RVM_Virt_Tim_Reg()` 注册。

**出入与为何无碍**：内核只通过官方端口契约使用——两个调度全局、
`OSTCBStkPtr` 首字段、hook 集；不 include 也不修改任何内核内部。基准不测
yield（每优先级一任务使其无意义）；README 的 yield 列留空。内核对象必须在
任务上下文创建（一个 `OSMemCreate` 就要碰 TCB），基准与普通应用都从 init
任务启动。

### 2.10 空闲停车（所有客机）

RVM 用户手册 §4.4.3 要求客机的空闲线程反复调用 `RVM_Hyp_Vct_Wait()`
（超调用 9）：把用户线程挂起、从运行队列摘除，直到下一个虚拟中断送达——
这是空闲客机不把核心空转掉的前提。该调用是**透明挂起**：恢复点就在调用处，
不改任何内核状态，也不在内核意义上阻塞。

hook 本体放在**应用**里（生成器复制成工作区 `main.c` 的那个文件），不在
端口：各家用自己的官方机制——FreeRTOS 的 `vApplicationIdleHook`
（`configUSE_IDLE_HOOK 1`）、uC/OS-III 的 `OS_AppIdleTaskHookPtr`、
RT-Thread 的 `rt_thread_idle_sethook`（`RT_USING_HOOK`/`RT_USING_IDLE_HOOK`）、
KLite 的 `klt_set_idle_hook()`（由 `cpu_sys_sleep()` 在掩码包裹中回调，内核调它时掩码是合上的）；
DjyOS 的 `djy_set_idle_hook()`（由 `CleanWakeupEvent` 在虚拟掩码开的位置
回调）。RTEMS 在应用里提供空闲任务体（`CONFIGURE_IDLE_TASK_BODY`），RTE
benchmark 的空闲任务体调用 `RVM_Hyp_Vct_Wait()`。benchmark 工程故意让
hook 空转（FreeRTOS 因配置共用必须定义一个空实现），使 idle 永不进入测量
路径。


### 2.11 RTEMS（RTE）：契约、策略与微妙出入

**契约。** RTEMS 以非特权 RVM 用户线程运行，不能编程 NVIC、SCB、VTOR、
SysTick，也不能进入 PendSV。中断驱动的调度在 RVM 向量线程上跑派发器，
而任务级上下文切换发生在用户线程。因此端口只能二选一：干净帧在任务栈上
直接切换，或者把脏帧/ISR 捕获帧交给 RVM ctx handler。

**策略。** 任务帧采用与其他 A7M 端口相同的伪装异常帧：5 个超调用参数字、
R4-R11 加保存的 `EXC_RETURN`、扩展帧的 S16-S31，以及硬件帧
R0-R3/R12/LR/PC/xPSR 与其上的 S0-S15/FPSCR。基本帧 22 字，硬浮点扩展帧
56 字。干净的任务间切换走 `v7m_context_switch_rvm.S` 的快路径；脏帧、
ISR 捕获帧或任何校验失败的帧保存 `rte_pending_heir` 并调用
`RVM_Virt_Yield()`，由 Vct ctx handler 把 heir 装入 `RVM_REG`。
`register_lr` 是帧种类标记（1=派发/初始帧，0=ISR 捕获帧）。
`_CPU_Initialize()` 刻意为空：公共 ARM 实现会通过 NVIC 写 SVC/PendSV 优先级，
客机不能这么做。ISR enter/leave 更新 per-CPU 嵌套计数，并在向量线程上派发；
物理切换仍延迟到 ctx handler。

**出入与为何无碍**：快路径只在当前 XPSR 无 EPSR ICI/IT、heir 帧可执行且
干净、没有 pending heir 时绕过 monitor；脏帧一律走真实异常返回。
初始帧/派发帧按构造是干净的。扩展帧用 `RTE_EXC_POP` 的原地变换恢复，
包括 XPSR bit9 对齐 pad；把 basic 当 extended（或反过来）会恢复错误的
PC/SP。`RTE_FAST_PATH=0` 时 C 分支和汇编快例程都编译掉，全 Vct 构建里
不存在快路径压栈。`RTE_CPU_FLAGS` 按 `.rvp` coprocessor 选择
Cortex-M4F/FPv4-SP 或 Cortex-M7/FPv5；RTEMS 构建使用 BSP 的 `-O2` 选项。
RTE 构建以 C 应用/基准为主；`librtemscxx` 可构建，但没有 C++ 客机测试。

### 2.12 Zephyr（ZPR）：契约、策略与微妙出入

**契约。** Zephyr 3.7.0 LTS 以非特权 RVM 用户线程运行。
`CONFIG_RVM_GUEST` 架构端口位于 `arch/arm/core/cortex_m/v7m_rvm/`，替换
正常的 Cortex-M 入口、SysTick、NVIC、SVC、PendSV、MPU 与 FPU 路径；
上游 Cortex-M 源文件保持原样，由构建系统选择新端口。调度仍用 Zephyr
自己的 `_kernel` ready queue，但物理切换要么走 RVM 快路径，要么走 Vct
ctx handler。

**策略。** 生成器拥有的 `prc_<vm>.c` 提供 `Thd_Vct`，`prc_<vm>_desc.c`
提供 `RVM_Desc`。描述符策略是 `Guest_Gen` 的虚方法：ZPR 报告用户入口为
`__zpr_entry`；公共生成器直接写出入口、由入口名推导
`RVM_EXTERN void <entry>(void);` 声明。描述符是普通 `.rodata` 中的
`const RVM_Desc[]`，由 `zpr_desc.ld` 按生成对象 `*prc_*_desc*.o*` 的
`.rodata` 收集到镜像基址，不需要段属性或生成后的文本补丁。端口的 `zpr_entry.c` 只提供
`__zpr_entry`（Zephyr `z_prep_c`）；RVM 中断在 `z_prep_c`/`z_cstart` 期间保持
关闭，直到 `zpr_switch_to_main()` 最后切 main 时才使能。`zpr_context.c` 把 Zephyr 的
`arch_swap` 状态映射到与 RTE 相同的 22 字伪装帧；帧的 SP 位于偏移 36，
共享的 RVM 慢路径汇编因此可以原样复用。`zpr_timer.c` 通过
`RVM_Virt_Tim_Reg` 注册 `sys_clock_announce()`，`zpr_printk.c` 通过
`RVM_Hyp_Putchar` 输出控制台；benchmark 的芯片头
（`zpr_test_<chip>.h`）可在 benchmark app 调用其 `ZPR_Test_*` 初始化时把
物理 TIM4 转发到虚拟 VCT 2。空闲线程调用 `RVM_Hyp_Vct_Wait()`，再用
`z_reschedule_irqlock()` 回到调度器；从 idle 调 `k_yield()` 是错的，因为
idle 有意不在 ready queue 中。ctx handler 的注册放在
`SYS_INIT(PRE_KERNEL_1)`，确保在 `z_bss_zero()` 之后执行。定时器 ISR 在
`sys_clock_announce()` 后调用 `zpr_reschedule_from_vector()`：若有更高优先级
线程就绪，就捕获被中断帧并 yield；`k_thread_abort()` 使用 RVM 专用实现，
不写 SCB->ICSR、不读 IPSR。上下文层同时支持 22 字基本帧和 56 字 FPU 扩展帧
（`S16..S31`、硬件帧、`S0..S15/FPSCR`）：初始帧一律是基本帧，带 `K_FP_REGS`
的线程也一样；第一条 VFP 指令置 `CONTROL.FPCA`，下一次保存才生成扩展帧。
Vct 捕获/恢复处理 monitor 的 `EXC_RETURN=0xFFFFFFED`。H743 上两个 FP 线程与
定时器驱动的 FP 抢占在快路径和全 Vct 构建下均通过；F429 上快路径通过，全 Vct
帧路径两者共用。

**生成器与内存。** `ZPR_Gen` 只写 Virt 工作区（Zephyr 应用、
`zpr_config.h`、驱动 west 的 `prc_<vm>` makefile），绝不修改 Zephyr 树。
应用的生成源表/include/linker 片段放在 `zpr_app.cmake`；缺 `CMakeLists.txt`
时生成薄包装，手写文件保留并只追加一次 include，之前生成的包装则刷新。
`prj.conf` 同一规则：手写文件保留用户选项，生成器拥有的旧链接值会被清除，
缺失的 ZPR 默认项追加一次。只有一块虚拟板 `armv7m`，它把 Zephyr 镜像放在与
RTE 完全相同的代码窗口，数据/bss/栈放在 RTE 数据窗口。生成器为每个 VM
写 `zpr_board.dts`（CPU compatible、flash/SRAM 窗口与 `chosen`），并以
`DTS_SOURCE` 传给 west；CPU 由 `CONFIG_RVM_CPU_M7`/`CONFIG_RVM_CPU_M4`
选择。没有 STM32 dtsi、RCC/PLL 或外设节点。芯片外设
（RCC、TIM4 等）经生成的 `zpr_test.h` 从
`zephyr/arch/arm/core/cortex_m/v7m_rvm/test/chip/zpr_test_*.h` 引入；
`zpr_config.h` 只保留代码/数据大小与快路径开关。benchmark 的 TIM2/TIM4
接线就在该芯片头的 `ZPR_Test_Timer_Init`/`ZPR_Test_Int_Init`/
`ZPR_Test_Int_Disable` 里，ISR handler 在 `test/bench.c`，与 RMP 的芯片头
做法一致。`test/bench.c` 和 `zpr_test.h` 只属于 benchmark app；普通客机
不编译、不 include 它们，也不会碰 TIM4。正常 app 与 SoC 无关：虚拟 SoC
没有 STM32 HAL 或 SoC 初始化，唯一时钟值是
`CONFIG_SYS_CLOCK_HW_CYCLES_PER_SEC`；用户需要外设时通过 `app.overlay`
添加，并受 RVM 设备/IRQ 授权约束。六项 ISR
基准行只是同一个 TIM4 虚拟向量钩子的标签，不是六种中断机制，benchmark
会先打印该说明。多客机时板级
DTS 的单 VM 窗口不够用：生成器把该 Virt 分配到的代码/数据窗口写进
`zpr_board.dts`（CPU compatible、`flash0`/`sram0` 与 `chosen`），并以
`DTS_SOURCE` 传给 west，每个客机按自己的物理窗口链接。生成后的 M7 窗口是
114 KiB、M4 窗口是 66 KiB；
生成器把 VM 窗口向下取整到 KiB，因此最多 848 字节不对外声明。H743 上已实测
两个 ZPR 客机同时启动；两个
benchmark 客机会争用同一 TIM4，不是受支持配置。该 Virt 的
`Source/app/` 里有用户 `.c` 时，两种构建模式下都优先用户 app；只有目录为空
且未设 `ZPR_NORMAL` 时才选 benchmark。`v7m_rvm` 的 Kconfig 依赖
`!SMP && !USERSPACE && !ARM_MPU
&& !CORTEX_M_SYSTICK && !TICKLESS_KERNEL && !GEN_ISR_TABLES`，C 侧用编译期
断言钉住基本/扩展帧 ABI。已验证的 ZPR 基准使用
`CONFIG_SYS_CLOCK_TICKS_PER_SEC=10`、minimal libc，无 userspace、无 MPU；
FPU 支持单独启用（`CONFIG_FPU=y`、`CONFIG_FPU_SHARING=y`、hard-float ABI），
由两个 FP 线程和定时器抢占测试验证，整数 benchmark 本身不使用 FPU。

**普通构建与原生心跳。** RVM 的时间轮由物理定时器向量推进。`generate.sh`
对 ZPR 工程自动加 `-b`，用原生基准进程充当 TIM4 心跳；镜像能启动，先跑
完原生基准，再进入用户的 Zephyr 应用。普通客机既不编译 `test/bench.c`
也不 include `zpr_test.h`，不会注册或改写 TIM4。原生基准输出尚未静音。
`benchmark.sh` 使用同一心跳并运行 15 项 ZPR 基准。

### 2.13 ChibiOS（CHB）：契约、策略与微妙出入

**契约。** ChibiOS 21.11 LTS 以非特权 RVM 用户线程运行，保留自己的调度器和
`chconf.h`。端口由两个新增的架构级目录组成：`os/common/ports/ARMv7-M-RVM`
和 `os/common/ports/RV32P-RVM`，不修改 ChibiOS 内核。ARMv7-M 端口支持
GCC、IAR EWARM（ICCARM）和 Keil MDK（ARMCC/ARMCLANG）；RV32P 端口仅
GCC。客机用虚拟定时器作为系统 tick，干净任务帧走直切快路径，脏帧/向量切换
统一走 RVM 上下文处理器，空闲线程调用 `RVM_Hyp_Vct_Wait()`。SMP、MPU、
userspace、tickless 关闭。基本帧在 ARMv7-M 上为 22 字、在 RV32P 上为 37
字（5 个 hypercall 参数、mstatus 和 31 字整数帧）；扩展帧再加浮点区
（ARMv7-M 56 字，RV32P RVF 70 字、RVD 102 字）。RV32P 帧宽由 RVM 的
`RVM_COP_NUM`、`RVM_RV32P_COP_RVF`、`RVM_RV32P_COP_RVD` 授权决定，而不是
端口私有的开关；若与生成的 `CHB_USE_FPU` 不一致会在编译期报错。`.rvp` 的
`Period=1` 对应基准使用的 100 Hz CHB 系统 tick。

**策略。** `port_init()` 先掩蔽虚拟中断，再通过 `RVM_Virt_Tim_Reg` 注册
`chb_tick_handler`、通过 `RVM_Virt_Ctx_Reg` 注册 `chb_ctx_handler`。
`PORT_SETUP_CONTEXT` 构造与 RTE 相同的 22 字伪装帧：5 个 hypercall 参数、
R4-R11/EXC_RETURN 和硬件帧；首入口 trampoline 调 `chb_thread_start()`，
解掩蔽后进入 ChibiOS 线程函数。`port_switch()` 分两种情形：任务态记录
`ntp`/`otp`，调 `RVM_Virt_Yield()`，再调 `RVM_Virt_Int_Unmask()`；这个
解掩蔽是必需的，因为 ChibiOS 在持有内核锁时调用 `port_switch()`，且外出
线程已被标记为 sleeping/ready。向量线程态只记录切换对并 yield，真正的
物理切换留给 Ctx 处理器。这个分流是**必要**的：只有 Ctx 处理器运行时，
`RVM_REG` 才描述被中断的用户线程；定时器/中断处理器运行在向量线程时并非
如此。`chb_ctx_switch()` 保存/恢复 5 个 hypercall 字、R4-R11、EXC_RETURN，
扩展帧还保存/恢复 S16-S31 与 S0-S15/FPSCR，并把新的用户 SP 写回
`RVM_REG`。ARM 快路径位于各编译器子目录的 `chcoreasm.s`（GCC、IAR、
RVCT/armasm）：在旧栈上构造同样的帧并手工展开 heir，干净帧不经过 monitor。
IAR 与 armasm 版本实现相同的 basic/FPU 帧变换；Keil 版本把 VFP 指令写成
原始半字，因此即使使用旧 armasm、未开启汇编器 FPU 选项也能汇编。
`CHB_FAST_PATH=0` 为全 Vct 构建，脏目标帧和当前 ICI/IT 脏帧自动回退慢路径。

RV32P 端口沿用同样的分流。`chb_ctx_switch()` 保存 RV32P 整数帧和可选浮点
块；`chb_fpu_save()`/`load()` 按 RVM 选定的 33 字 RVF 或 65 字 RVD 布局
遍历协处理器结构。其快路径在 `RV32P-RVM/compilers/GCC/chcoreasm.s`：在
旧栈上构造 参数/mstatus/RVF/整数 帧，写 `otp->ctx.sp`，取
`ntp->ctx.sp`，复制 hypercall 参数，按 heir 保存的 mstatus 决定是否恢复
浮点块，解掩蔽，展开整数帧并经 `tp`（x4；无 TLS 时 GCC 不分配）恢复。
用户态读不到 `mstatus.FS`，所以 RV32P 快路径无条件保存 33 字 RVF 块并把
帧标为脏；这使 ISR 唤醒比慢路径多约 500 周期，但任务级各行快 4-5 倍。
RVD 帧已能由慢路径处理，但直切快路径目前只实现 RVF，RVD VM 会自动令
`CHB_FAST_PATH=0`。

临界区把 `chSysLock()/Unlock()` 映射成软件嵌套计数 +
`RVM_Virt_Int_Mask/Unmask`；tick 处理器调 `chSysTimerHandlerI()`，再调
`chSchDoPreemption()`；`PORT_IRQ_EPILOGUE()` 在向量线程上做同样的抢占，
完全不碰 PendSV/SCB。空闲线程经 `port_wait_for_interrupt()` 进入
`RVM_Hyp_Vct_Wait()`。控制台用 RVM debug putchar。

**生成器与基准。** `CHB_Gen` 采用小客机源列表模式：把 `templates/chconf.h`
复制到每个 VM（用户已改的副本不覆盖），生成 `chb_config.h`
（`CHB_ST_FREQUENCY=100`），加入端口和 `os/rt`/`os/oslib` 源列表，并选择
`chb_benchmark.c` 或 `chb_main.c`。生成器不写 ChibiOS 构建系统文件：GCC 使用
标准 RVM makefile/linker，IAR/Keil 使用 RVM 构建层生成的 EWARM/uVision 工程。
`CHB_Gen` 把 `ICCARM` 映射到 `IAR` 编译器子目录、`ARMCC` 映射到 `RVCT`，
选择对应的 `os/common/portability/IAR` 或 `ARMC6` 头，并把该编译器的汇编源
加入 VM。`CHB_USE_FPU` 跟随 VM 协处理器；GCC 的 FPU VM 首次生成时会写一个
`user` makefile 片段加 `-mfloat-abi=hard`（已存在不覆盖）。RV32P 端口会从
RVM 协处理器宏重新推导有效 FPU 宽度，并拒绝与之不一致的 `chb_config.h`。
`CHB_FAST_PATH` 在 A7M 和 RV32P 上都默认开启；生成时设环境变量
`CHB_FAST_PATH=0` 可构建全 Vct 变体。芯片测试头
`chb_test_stm32h743zi_rvm.h` 负责 TIM2/TIM4、RCC 和 TIM4 IRQ30→VCT 2
映射；普通 app 不 include 它。九行是 Yield、Notification、Semaphore、
Message queue、Memory allocation/free pair、ISR Notification、
ISR Semaphore、ISR Message queue 和 RVM activation relay。四条 ISR 行先全部
测完，`chb_test_int_disable()` 关闭 TIM4 并注销虚拟向量后再整块打印
（2026-11-12 对齐 FRT/UO3/RTE/ZPR/NTX 约定）；旧档为逐行打印期所测，每行统
计在打印前已封闭，数字可直接对比。H743 与 F429 档案分别在
`chb_test_stm32h743zi_rvm.h` 和 `chb_test_stm32f429zi_rvm.h` 中，包含 IAR 与
Keil 结果。ARMv6-M 端口（`os/common/ports/ARMv6-M-RVM`，仅 22 字 basic 帧、
无 FPU、无 ICI/IT）已在 STM32G071RB 上以 GCC/Keil/IAR 三链验证；档案在
`chb_test_stm32g071rb_rvm.h`（TIM2 16 位计数、按 2^16 模差、TIM3 向量源、
基准工作区 512 字节、堆 4 KiB；TIM2 是 16 位计数器，差值按 2^16 模减）。iasmarm 对宏支持不稳，IAR 版把对齐/非对齐
两条帧路径全部展开。`ewarm-*chb*` 和 `rvmdk-*chb*` 示例覆盖 H743 FPU 与 F429
FPV4-SP。RV32P CH32V307 端口是独立的 `os/common/ports/RV32P-RVM`：所有
寄存器都在 RVM_REG 中，任务帧为 RV32P 31 字整数帧 + mstatus + hypercall
参数 + 可选 RVF/RVD 块。该端口已在 WCH-Link/CH32V307 真机上以直切快路径、
RVM 继承的 RVF 帧跑通，`FPU context: PASS`；9 行快路径档案在
`chb_test_ch32v307vc_rvm.h`，其中也记录了慢路径对照值。没有 RVD 硬件，
RVD 慢路径未上真机验证。

**出入。** 任务态 `port_switch()` 在 yield 后主动解虚拟掩蔽，让 monitor
在返回 ChibiOS 调用者之前完成切换；这是其他端口慢路径解掩蔽的 CHB 等价
形式。ARMv7-M 端口支持 GCC、IAR EWARM 和 Keil MDK；RV32P 仅 GCC。
ARMv7-M 快路径要求当前 ICI/IT 和目标帧都干净；
RV32P 快路径因用户态读不到 `mstatus.FS` 而无条件保存 RVF，RVD 帧回退
慢路径。Memory 行用静态缓冲区上的 `chHeapObjectInit()`，不需要 linker
heap 符号。H743 FPU 变体（`gccmf-stm32h743zit6-chb.rvp`，FPV5_DP，
hard-float）和 F429 变体（`gccmf-stm32f429zit6-chb.rvp`，FPV4-SP，
hard-float）都使用扩展帧，双线程 FPU 上下文测试在快路径和
`CHB_FAST_PATH=0` 下均通过；同样的两块板也通过 IAR（`ewarm-*chb*`）和
Keil（`rvmdk-*chb*`）工程及直切快路径的测试。CH32V307 RVF 变体
（`gccmf-ch32v307vct6-chb.rvp`）也用 RV32P 快路径通过同一测试。

---

### 2.14 NuttX（NTX）：契约、策略与微妙出入

**契约。** NuttX 13.1.0 以内核扁平构建运行在 RVM 的非特权用户线程中。端口位于
`arch/arm/src/armv7-m-rvm/` 与 `arch/arm/include/armv7-m-rvm/`，板级为不含芯片名的
`boards/arm/armv7-m-rvm/ntx`，含 `virt`（Cortex-M7）与 `virt-m4`（Cortex-M4、
FPv4-SP）两套配置。NuttX 保留自己的调度器，但 VTOR、NVIC、SysTick、SVC、PendSV、
MPU 与 CPACR 都归 monitor；不支持用户态、MPU、SMP、tickless、SysTick 和物理 NVIC。

**切换策略。** `RVM_Virt_Yield()` 只置切换标志，不保存调用者；`armv7m_rvm_switch.S`
在当前栈上把现场伪装成 RVM 异常帧（参数、`R4..R11/EXC_RETURN`、硬件帧，FPU 上下文
再加 `S16..S31` 与 `S0..S15/FPSCR`，共 22 或 56 字），恢复 PC 为 `ntx_ctx_exit`，
硬件 LR 是 C 调用者返回地址。帧基址放在 `struct xcptcontext` 的第一个字
`rvm_sp`，随后是虚拟嵌套层数和帧类型标记；`xcp.regs` 保持 NULL。向量线程上请求的
切换把 monitor 的中断帧复制到 `RVM_REG->Reg.SP` 之下；任务级切换绝不从 `RVM_REG`
取帧。切窗内调度就绪链头不等于实际运行任务，端口用 `g_ntx_running` 抓帧并保护在途
帧不被定时器覆盖。`up_exit()` 的 `arm_fullcontextrestore()` 实现为直接装载新链头、
不保存将亡任务。启动线程成为 IDLE，使用 monitor 提供的 `STK` 栈区，而不是
`_ebss + CONFIG_IDLETHREAD_STACKSIZE`；注册向量处理时逻辑嵌套必须为 0，否则 idle
会在 RVM 掩码关闭的状态下永久停车。`CONFIG_ARMV7M_RVM_FAST_PATH` 只在当前 XPSR
与继任任务帧都干净时走直接栈交换；向量捕获的帧一律走 monitor。初始帧一律 basic；
第一条 VFP 指令置 `CONTROL.FPCA`，下次切换生成扩展帧。FPv5-DP（H743）与 FPv4-SP
（F439/F429）两条扩展帧路径均已在真机通过。

**生成器与基准。** `NTX_Gen` 只写 Virt 工作区：每 Virt 的 NuttX 板
（`configs/virt/defconfig`、`include/board.h`、含 RVM Kfunc ID 的
`include/prc_virt.h`、`scripts/ntx.ld`、`src/CMakeLists.txt`）、无用户源时的 smoke
app，以及运行
`cmake -S <NuttX 树> -B build -DBOARD_CONFIG=<Virt>/Project/ntx_board/configs/virt`
的 `prc_<vm>` makefile。框架的 `prc_<vm>.c` 提供 `Thd_Vct`，`prc_<vm>_desc.c` 提供
`RVM_Desc`；板级链接脚本把 `*_desc*.o` 的 `.rodata` 固定在镜像基址。Virt 的 app
目录中有用户 `.c` 时优先于 benchmark。基准源为
`boards/arm/armv7-m-rvm/ntx/test/ntx_benchmark.c`，芯片头为
`test/chip/ntx_test_stm32h743zi_rvm.h` 与 `ntx_test_stm32f429zi_rvm.h`，生成的
`ntx_test.h` 只在 benchmark 模式选中其一。共 9 项：Yield、Semaphore、Message queue、
Blocking message queue、Alarm、Memory allocation/free pair、ISR Semaphore、ISR
Message queue、ISR RVM activation relay。relay 行是 RVM 机制基线（纯向量投递 +
向量线程往返 + 自旋线程恢复，路径中无任何内核对象），其余 ISR 行以它为分母读数。
FIFO、事件通知、Activation 因 NuttX 没有原生原语而刻意缺失；只测 NuttX 原生 API。TIM2 作周期计数器，TIM4 经物理 30→虚拟 2 驱动两条 ISR 行，ISR 行
在中断源停止后整块打印。正式成绩在芯片头的档案块中。标准构建流程是在 `Project`
目录执行 `bash benchmark.sh gccmf-stm32h743zit6-ntx.rvp`（或 `generate.sh`），脚本
已带 `-ntx <NuttX 树>`；CMake、Ninja 与 kconfiglib 的 `olddefconfig` 须在 `PATH`。

**出入。** 基准子集小于 RTE/RMP 的 15 项；消息队列行测的是 NuttX 基于 fd 的
mqueue，比 Zephyr/RTE 的邮箱原语更重。由于 `xcp.regs` 为 NULL 且启用了
`CONFIG_DISABLE_ALL_SIGNALS`/`FDCLONE_DISABLE`，信号、coredump、backtrace 和栈
溢出检查不受支持。普通工程用 `generate.sh`，benchmark 用 `benchmark.sh`，两条
流程严格分开。

## 3. RVM 中断模型（所有端口的共同地基）

- **三层 gate**。`RVM_Int_Mask` 在投递发送处（`RVM_Virt_Vct_Trig`）、向
  量取出处（`RVM_Vct_Get`）、执行处（`RVM_Vct_Loop` 的 tick/ctx）三处被
  检查。掩蔽期间 pending 的可屏蔽向量被**搁置而非丢弃**——标志一直保
  留到解掩蔽时补投。透明向量（NMI 类）按设计穿透。
- **抢占方向**。向量线程的优先级高于客机用户线程。掩蔽清零时，中断可
  在任何指令边界抢占；掩蔽置位时，可屏蔽中断根本不执行。
- **物理域与虚拟域**。`CPSID`/`BASEPRI` 只关物理中断，对虚拟投递**零作
  用**。本库所有掩蔽都走 `RVM_Virt_Int_Mask()`/`RVM_Virt_Int_Unmask()`；
  若把内核中断原语映射到物理域，上述所有 gate 将被无声地全部废掉。
- **帧互换性**。快路径（伪装异常帧）、中断路径（Ctx 处理器转存寄存器
  块）与任务创建三者产出**逐字相同**的任务帧——任一生产者的帧可被任
  一消费者使用。

## 4. 已知局限

- 脏帧慢路径（目标帧的 EPSR 带 ICI/IT 续传状态）在典型负载下近乎死代
  码；然而，这些慢路径对最坏执行时间确有影响。
- 透明向量按设计绕过掩蔽；本库没有端口使用它们，它们与内核临界区的
  交互未测试。
- RTE 以 C 客机（C 应用与 C benchmark）验证；`librtemscxx` 可以构建，
  但没有实际运行 C++ RTE 应用。
- RTE 的 ISR relay 在任务恢复后采样，其他客机在 ISR 入口采样；relay 数字
  不能直接横比。
- ZPR 的 ISR 行在 RVM 虚拟向量钩子中取样、由轮询线程读出，尚不是
- NTX 的 `xcp.regs` 恒 NULL 且关闭信号——coredump、backtrace、栈溢出检查均不可用（`CONFIG_DISABLE_ALL_SIGNALS`）。
- CHB 的 RV32P 快路径无条件保存/恢复 RVF 块（不管线程是否用了 FPU），ISR 唤醒比慢路径多约 500 周期；RVD 快路径未实现（C 侧对 RVD 构建强制 `CHB_FAST_PATH=0`）。
  Zephyr ISR 上下文内的内核对象释放。

