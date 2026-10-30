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

**RMP 不在本库分发**：RMP 原生自带 RVM 平台层（RMP 源码树的
`Source/Platform/*_RVM/`），无需客机侧打包。

本目录内容是由 `update-maintainer-only.sh` 生成的**镜像**；请勿直接在此编辑——请修改
源码树后在本目录重新执行 `./update-maintainer-only.sh`。

### 本库周边的仓库布局

本库是一个七仓库体系的组成部分。工程生成器（`rvgen`）按相对路径定位
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
└─ DjyOS_V110/         DjyOS V1.1.0 源码树——端口在 bsp/arch/arm/cortex-m/armv7e-m_rvm，
                       基准在 test/
```

按下一节的方法把本库覆盖到你的官方 FreeRTOS / uC/OS-III / RT-Thread /
DjyOS 源码树之后，那些源码树就顶替了上表布局中 `*_Vxxx/` 的位置。

---

## 1. 安装

本库的目录结构就是目标源码树相对布局的镜像，且**只包含我方新增文件**——
官方内核文件一律不在镜像内，因此直接覆盖是安全的：既不会动你的任何官方
文件，每个端口也自然落到正确位置。

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


### 1.2 工程生成

所有客机都由 RVM 工程生成器驱动（RVM `Project/` 目录下的 `rvgen`，
经 `benchmark.sh` 调用）。`.rvp` 工程文件选择芯片、平台与客机类型
（`<Guest_Type>FRT|UO3|RMP|RTT|DJY</Guest_Type>`）；生成器把端口文件拷入
工作区，并产出构建系统（Makefile / IAR / RVDS），include 顺序已排好。
手工建工程可行，但不在此文档范围内。

`generate.sh` 走同一个生成器但**不带** benchmark 选项：客机由
`<guest>_main.c` 里的普通应用启动，该文件被生成器复制成工作区的
`main.c`（含 §2.8 的空闲停车 hook）。两个脚本必须给同一组源码根。


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


### 2.8 空闲停车（所有客机）

RVM 用户手册 §4.4.3 要求客机的空闲线程反复调用 `RVM_Hyp_Vct_Wait()`
（超调用 9）：把用户线程挂起、从运行队列摘除，直到下一个虚拟中断送达——
这是空闲客机不把核心空转掉的前提。该调用是**透明挂起**：恢复点就在调用处，
不改任何内核状态，也不在内核意义上阻塞。

hook 本体放在**应用**里（生成器复制成工作区 `main.c` 的那个文件），不在
端口：各家用自己的官方机制——FreeRTOS 的 `vApplicationIdleHook`
（`configUSE_IDLE_HOOK 1`）、uC/OS-III 的 `OS_AppIdleTaskHookPtr`、
RT-Thread 的 `rt_thread_idle_sethook`（`RT_USING_HOOK`/`RT_USING_IDLE_HOOK`）、
DjyOS 的 `djy_set_idle_hook()`（由 `CleanWakeupEvent` 在虚拟掩码开的位置
回调）。benchmark 工程故意让 hook 空转（FreeRTOS 因配置共用必须定义一个空
实现），使 idle 永不进入测量路径。


---

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

