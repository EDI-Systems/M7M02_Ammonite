# FreeRTOS-on-RVM 移植与真机调试：方法论与坑库（Agent 工作笔记）

> 更新日期：2026-10-21。覆盖范围：CH32V307/RV32P 基线 → A6M_RVM(G071) 双链 → A7M_RVM(F429 计划) →
> H743ZI 真机全流程。本文写给零上下文接手的 agent：先读第 1、2 章即可开工，排障时查第 3、4 章，
> 改 A7M port 前必读第 5 章。

---

## 1. 项目拓扑与路径速查

### 1.1 仓库关系

| 仓库 | 角色 | 关键事实 |
|---|---|---|
| `c:\Code_Library\FreeRTOS_V900` | FreeRTOS 内核 + RVM 移植层 | **内核文件永不改动**（tasks.c/list.c/queue.c/timers.c/include）；改动只落 `portable/{GCC,Keil}/*_RVM/` 与 `test/` |
| `c:\Code_Library\M7M01_Eukaron` | RME 微内核（RVM 的内核底座） | 平台头在 `Include\Platform\A7M\Chip\<CHIP>\rme_platform_*.h`；本次改过 STM32H743ZI 头（VOS） |
| `c:\Code_Library\M5P01_Prokaron` | RMP RTOS + RMP-on-RVM 移植 | A7M_RVM port 的参考实现（fast yield/脏位检测的出处） |
| `c:\Code_Library\M7M02_Ammonite` | RVM hypervisor + 生成器 + 工作区 | 生成与构建主战场，见下 |

### 1.2 M7M02_Ammonite 内部

- `Project\*.rvp`：工程描述 XML（`gccmf-*`=GCC/Makefile，`rvmdk-*`=Keil，`-frt` 后缀=FreeRTOS guest，`-nofpu`=无协处理器）
- `Project\<TOOLCHAIN>-<CHIP>-FRT\`：生成的工作区（Kernel/Monitor/Proc1/Proc2/Virt1 五子工程）
- `Project\generate.sh <rvp>`：生成（不带 benchmark）；`Project\benchmark.sh <rvp>`：生成（带 `-b`，加入 frt_test.h/frt_benchmark.c/test 包含路径）——**必须用 benchmark.sh，否则 Virt1 编不过**
- `Utility\M7M02_Final\rvgen.exe`：生成器（备份 rvgen_backup.exe）；源在 `Utility\M7M02_Projgen\Source\`，改后用 MinGW g++ 重建（xml 库在 `c:\Code_Library\M5P02_Amoeba\Middleware\XML`，提供 xml.h；xml.o 已预编译在 Release\Source）
- `Guest\`：**发布镜像**（update.sh 单向刷新，不参与构建）；生成器直接读 `../../FreeRTOS_V900` 源树
- `Utility\M7M02_Final\bincopy.exe`：把 VM 的 .bin 转成 `*_image.c` 供 Kernel 链接

### 1.3 工具链

| 工具 | 路径/版本 | 备注 |
|---|---|---|
| arm-none-eabi-gcc | `C:\Toolchain\arm-none-eabi-mingw64\bin`（13.2.1） | make 亦在 PATH |
| armclang/UV4 | `C:\Keil_v5_ARM`（ARMCLANG 6.23） | UV4.exe -b 构建 |
| MinGW g++ | `C:\Toolchain\mingw32\bin`（12.2） | 重建 rvgen 用 |
| OpenOCD | `C:\MounRiver\MounRiver_Studio\toolchain\OpenOCD\bin`（0.11.0 WCH fork） | 坑极多，见 4.3 |
| 串口 | ST-Link VCP（本板 COM14），115200-8N1 | WCH-LinkE 是 COM6，勿混 |

---

## 2. 标准工作流

### 2.1 生成（必用 benchmark.sh）

```sh
cd c:/Code_Library/M7M02_Ammonite/Project
bash benchmark.sh gccmf-stm32h743zit6-frt.rvp   # 或其它 -frt.rvp
```

- 改了 rvp 或生成器后**必须删工作区再生成**（rvp 里 Project_Overwrite=No，旧 Makefile/uvprojx 不会刷新）。
- 生成器输出确认三行：`tasksrvm.c`（A7M 才有）、`portasm.s`（RV32P/A6M/A7M）、`test/frt_benchmark.c`。

### 2.2 GCC 构建（顺序：镜像先、Kernel 后；单命令勿超 30s）

Kernel 的 Makefile 把 `../../Virt1/Project/virt1_image.c` 等当输入，故顺序固定：
Monitor → Proc1 → Proc2 → Virt1 → Kernel。每工程 `cd <X>/Project && make -f <mk> -j8`。
命令行 30 秒超时是硬约束：一条命令最多编 2-3 个工程；被超时杀掉的 make 可直接重跑（对象缓存续编）。

### 2.3 Keil 构建

`UV4.exe -b <dir>\Project\<name>.uvprojx -j0 -o <dir>\build.log`，读 build.log 尾部判 0 Error。
Kernel 的平坦 bin 需手动 `fromelf --bincombined --output kernel_keil.bin Object\Kernel.axf`
（**必须 --bincombined**，普通 --bin 多区段输出 1 字节废文件；输出文件名被句柄锁死时换名输出）。

### 2.4 烧录（OpenOCD，模板在 Project\Script\——该目录被 .gitignore 的 Project/*/ 规则忽略，专放生成的脚本）

tcl 要点：`set CPUTAPID 0`（跳过 idcode 校验）→ `source target/<chip>.cfg` → init → reset halt →
flash write_image erase <kernel.bin> 0x08000000 → verify_image → reset run → shutdown。
F429 的 stm32f4x 驱动在新硅片上还会报 device id=0，flash 前加 `reset_config srst_only srst_nogate connect_assert_srst` 兜底（H743 同款可用）。

### 2.5 串口捕获（模板 Project\Script\capture.ps1，-Port 参数选 COM 口，默认 COM14=H743 板）

用 **Start-Process 分离进程**跑捕获脚本（能在工具 30s 超时下存活），先开端口再烧录，事后读文件。
波特率从内核平台头的 BRR 反推：H743 BRR=0x364@APB1 100MHz ≈115200；F429 BRR=0x187@45MHz ≈115200。

### 2.6 会话收尾三清（必做）

1. **删工作区**：生成的 `GCCMF-*` / `RVMDK-*` 全部删除（可随时由 rvp+benchmark.sh 复生）
2. **收生成物**：烧录/复位/捕获脚本与原始日志一律收进 `Project\Script\`（已被 .gitignore 的
   `Project/*/` 覆盖）；可再生的 log.txt/report.txt 直接删
3. **结果入档**：benchmark 数据写进 `frt_test_*_rvm.h` 头文件档案块（正式档案），AGENTS.md 只引用
验收线：**Project 根只允许 *.rvp 与仓库自带 sh；`git status --porcelain -- Project/` 必须为空。**
---

## 3. 排障方法论（按威力排序）

### 3.1 断点裁决法——“卡在哪”一击定位

对启动类死机：反汇编/nm 拿到候选等待环与里程碑地址（如 VOS 环、HSE 环、PLL 环、Boot 入口），
各下一个硬件断点，resume 数秒后 `reg pc`——命中的断点即当前进度。注意 do-while 的环体首过
也会碰断点，**命中≠卡死**；要判定卡死用“resume+sleep+halt 后读 PC/寄存器”法。

### 3.2 fork 无回显的逃生门

MounRiver OpenOCD fork 把 `reg/mdw/poll/mww` 的 Tcl 回显全裁了。可靠替代：

```tcl
mem2array a 32 0x58024818 1
echo "D3CR = $a(0)"
```

`mem2array` 偶发对某些地址（0x5C001004 调试域）失败并中断脚本，用 `catch { ... }` 包裹关键步骤。

### 3.3 “烧的是什么”要物证

怀疑固件版本时：halt 后读 flash 指令字比对 objdump（如 `mem2array w 32 0x08005eaa 1` 对比
`orr.w r3,#0xC000/0x8000` 的编码 0x4340f443/0x4300f443）。**make 的依赖不含 M7M01 系统头**，
改平台头后普通 make 可能不重编——`rm -rf Object` 强制全编，再用反汇编确认常量真的变了。

### 3.4 写黑洞测试——RAM 边界钉死

向可疑地址写魔数读回：读不回或命令挂死 = 无 RAM。H743 板 DTCM 标称 128KB 实测 64KB 即此法
钉死（写 0x20010000 挂死；0x2000FFFF 正常）。由此类推“数据全垃圾/字段为 0”类故障先查
内存是否黑洞，再查软件逻辑。

### 3.5 Fault 现场活捉

内核善后会把 PSP/CFSR 清掉，事后读不到。在 `UsageFault_Handler`/`_RME_Thd_Fatal` 入口下断点
（第一次 fault、现场未清），读 `reg psp` + mem2array 读 PSP 帧 8 字（R0-R3,R12,LR,PC,XPSR），
PC/XPSR 即 fault 真相（如 PC=0/XPSR=0 → INVSTATE，配合 CFSR=0x20000 互证）。

### 3.6 长耗时任务防超时三原则

1. 单条命令 <30s：构建拆单工程串行
2. 需跨超时存活的（串口捕获）：Start-Process 分离进程写文件
3. 超时被杀的 openocd 会**锁住 ST-Link**（下次报 `read version failed`）：先
   `Stop-Process -Name openocd -Force` 再重试

### 3.7 双工具链字节同步约定

port.c/portmacro.h/FreeRTOSConfig.h/tasksrvm.c 四文件 GCC 与 Keil 目录**必须哈希一致**
（改完一边立即 Copy-Item 到另一边并 Get-FileHash 校验）；portasm.s 两份语法不同（GAS vs armasm）
内容语义对应，属预期。改 port 后跑 `Guest/update.sh` 刷新发布镜像。

---

## 4. 坑库

### 4.1 文件编辑类（本次最痛，三条同根）

多段插入（editor insert_line 拼大文件）产生过三种错位，全部编译期/汇编期才暴露：

1. **头注释收尾丢失**：`*/` 残段被裁尾时删掉 → `.syntax unified/.arch/.thumb` 被吞进注释 →
   GAS 退回 divided 语法 → 满屏 `lo register required / unshifted register required`。
   **症状指纹：见到这批错先查汇编指示字是否在注释里。**
2. **悬空 `/*`**：插入点落在声明注释块中间 → 函数体被吞成文件域 → `expected identifier before volatile`。
3. **armasm 宏缺 MEND**：尾部裁剪吃掉 `MEND` → `A1291E: Macro definitions cannot be nested`。

**规矩：大文件一律 PowerShell 单引号 here-string 整体写入**（`[IO.File]::WriteAllText($p, $c.Replace("`n","`r`n"))`），
写完 `(Get-Content $p).Count` + 抽查首尾段。编辑器分段插入只用于 <100 行小改。

### 4.2 PowerShell/工具环境类

- **原生命令 stderr=错误记录**：openocd/make/gdb 的 stderr 首行会以 ErrorRecord 中断管道。捕获用
  `*>&1 | Out-File` 或 `Start-Process -RedirectStandardOutput/-RedirectStandardError`（后者最稳）。
- **cmd /c 嵌套引号必失效**：含 `>` 重定向的长命令经 PS→cmd 会丢输出文件。要么命令拆进 .tcl/.bat
  文件，要么 Start-Process 参数数组。
- **msys 抢 PATH**：裸写 `cmd` 可能解析到 `C:\Toolchain\msys64\usr\bin\cmd`。要真 cmd 用全路径
  `C:\Windows\System32\cmd.exe`。
- **-creplace 反向引用数字粘联**：`'$1'+'0x14000'` 被解析为组 10 → 字面量直写破坏 XML。用花括号
  `'${1}0x14000${2}'`。
- **-replace 不分大小写**：必须 `-creplace`。
- Remove-Item 在非交互会话对某些被锁文件会弹“朗读和提示不可用”而失败 → `cmd /c del /f /q`。

### 4.3 MounRiver OpenOCD fork 类

- 回显裁剪/逃生门见 3.2。
- **新硅片 idcode 拒识**（F429 报 expected 0x2BA01477 实测 0x6BA02477）：`set CPUTAPID 0` 放在
  source target cfg **之前**。
- **运行态设 flash 断点必败**（无报错直接脚本死）：先 `reset halt` 或 `halt` 成功后再 `bp`。
- `reset halt` 偶发被 examine-end 事件（读 0x5C001004）失败拖死：`catch { reset halt }` + `catch { halt }` 双保险。
- gdb_port 3333 起 server + arm-none-eabi-gdb 连接：本环境实测 qSupported 协商异常连不通，弃用。
- 烧录前务必杀残留 openocd 进程（见 3.6-3）。

### 4.4 生成器/构建类

- Project_Overwrite=No：见 2.1。
- **工作区 Include 是生成器拷贝**：改 M7M01/M7M02 源头平台头后，已生成工作区里的
  `Kernel\Include\rme_platform_*.h` **不会自动更新**——要么手工同步拷贝，要么删工作区重新生成。
  本次 VOS 修复“改了没生效”即栽在此。
- Kernel 链接依赖各 VM 的 `*_image.c`（bincopy 产物）：子工程成功后 Kernel 才能 make（报
  `No rule to make target 'virt1_image.o'` 即顺序错了）。
- **【血泪头条】生成器会向工作区 Kernel\Include\rme_platform_*.h 注入约 17 个布局常量**
  （RME_KOM_VA_BASE/RME_KSTK_VA_BASE/RME_RVM_PHYS_VCTF_BASE/RME_RVM_GEN_ENABLE 等），
  **源头 M7M01 文件只是模板（GEN_ENABLE=0+占位地址）**。改内核平台头必须：源头改非布局代码
  → 删工作区重新生成 → 让注入发生。**绝不能把源头文件手工拷进工作区**（会把布局常量抹成
  模板值 → 内核对象建在错误地址 → init 线程 panic、寄存器转储全是垃圾指针 R4=0x7024FB78）。
  判别法：panic 转储 R4 恒定垃圾值 + 转储时间戳可疑 → 先查 GEN_ENABLE 是否被抹。
- make 依赖不含外部系统头：见 3.3。
- Keil：`DCI` 写半字指令对；`MSR XPSR,R0`（GAS 用 `msr xpsr_nzcvq`）；fromelf 见 2.3。

### 4.5 硬件类（本块 NUCLEO-H743ZI 档案）

- **VOS 双重 bug**：详见第 6 章复盘。
- **DTCM 实测 64KB**（标称 128KB）：0x20010000+ 写黑洞。**全链内存已在 rvp 迁至 AXI SRAM
  0x24000000**（Kernel Data_Base 改 0x24000000，Auto 分配顺序跟随；SFTD/VMMD 栈仍在 0x20000000
  低 64KB 安全区）。
- HSE=8MHz 来自 ST-Link MCO（HSEBYP 直注）；内核 400MHz=8/4*400/2，APB1/APB2=100MHz，
  APB1 timer=200MHz=半主频（测试头 `TIM2_CNT<<1` 换算即基于此）。
- DPID 0x6BA02477；DBGMCU IDCODE 经此 fork 读出 0x70003F（非标准 0x450，未解，以实测行为为准）。
- MBED 拖拽盘（卷标 NODE_H743ZI）可作 OpenOCD 失效时的备用烧录通道（未启用）。
- CH32V307 历史坑（继承）：多区段 hex 烧录会损坏，一律烧平坦 kernel.bin。

---

## 5. 设计决策记录：A7M_RVM fast yield（改 port 前必读）

### 5.1 为什么必须有 ICI/IT 检测（原理）

ARMv7-M 的 EPSR 携带 IT 块/可续传 LDM/STM 状态（掩码 `0x0600FC00`）。三方不对称：硬件异常
入口/出口自动保存恢复；而**线程态软件 `MRS EPSR` 读作 0、`MSR EPSR` 写忽略**。fast yield 手工
伪造/恢复异常帧，若目标任务上次是被中断路径切出的（帧内 xPSR 带 ICI/IT），手工恢复会静默
丢状态（条件指令 UNPREDICTABLE/LDM 重启）。故脏帧**只能经真实异常返回恢复**（慢速路径），
A6M（v6-M 无 IT/ICI）无此约束。

### 5.2 架构（三层决策，全部零内核文件改动）

```
port.c vPortYield():
  mask(用户态标志写,无陷入) → xTaskPeekNextToRun()【tasksrvm.c 提供】
   ├─ peek==当前任务 → unmask, return（零切换）
   ├─ 目标帧脏(LR_EXC.4 选偏移,XPSR&mask) → unmask + RVM_Virt_Yield() 慢速(异常返回续传)
   └─ 干净 → vPortYield_Swt【portasm.s】(内部真 vTaskSwitchContext 必然选中同任务,
      因窗口内调度状态被 mask 冻结; 尾部解锁; EXC_POP 内 0x0600FC00 检测→vPortYield_Err 纵深断言)
```

### 5.3 tasksrvm.c——peek 的实现载体

**编译 tasksrvm.c 取代 tasks.c**（二者不得同时入链）：`#include "../../../tasks.c"` 让文件内
静态变量（pxReadyTasksLists/uxTopReadyPriority/pxIndex）在附加翻译单元可见，内核源零改动。
peek 是 V9 通用选择宏的只读镜像：不推进 pxIndex、不写 pxCurrentTCB。

**踩过的真 bug（真机 H743 首测暴露，F429 同样受益）**：V9 的 `listGET_OWNER_OF_NEXT_ENTRY`
**有 end-marker 绕回检查**——pxIndex 前进落在 xListEnd 时需再进一步（end 的 pvOwner 槽是
MiniListItem 的 padding=0）。漏抄该检查 → pxIndex 停在任务 item 且 pxNext==end 时 peek 返回
NULL → wrapper 解引用 0 → Data access violation。修复后的判据（照抄 V9 语义）：

```c
pxNextItem = pxList->pxIndex->pxNext;
if( pxNextItem == ( const ListItem_t * ) &( pxList->xListEnd ) )
    pxNextItem = pxNextItem->pxNext;
return ( TaskHandle_t ) ( pxNextItem->pvOwner );
```

约束：`configUSE_PORT_OPTIMISED_TASK_SELECTION` 必须 0（FreeRTOSConfig 已锁）；调度器挂起期
禁 yield；peek 须在虚拟中断屏蔽下调用。

### 5.4 关键约定速查

- 帧字偏移（自 pxTopOfStack=TCB 偏移 0）：params 5 字 / R4-R11 8 / **LR_EXC@13** /
  基本帧 XPSR@**21** / 扩展帧(S16-31 插 16 字) XPSR@**37**。pxPortInitialiseStack、
  xPortPendSVHandler(C)、portasm.s(ASM) 三处布局一致。
- `configUSE_RVM_OPTIMISED_YIELD`（FreeRTOSConfig 裸 #define）：1→portYIELD()=vPortYield()；
  0→RVM_Virt_Yield()。OFF 态 vPortYield_Swt/peek 全被链接器 GC（GCC 实测 -804B 符号消失）。
- 生成器 frt_gen.cpp：A7M 时 tasksrvm.c 替换 tasks.c、portasm.s 纳入（RV32P/A6M/A7M 白名单）。
- asm 的 `.short`/`DCI` 半字对是 VFP 指令（VSTMIA/VPUSH 等），两工具链各写法，勿“改漂亮”。
- 假帧 PC=vPortYield_Exit（asm 尾部 `bx lr` 处），LR=wrapper 返回地址——中断路径恢复该帧
  时经 Exit→bx lr 无缝续跑；EXC_PUSH 的 0x200(XPSR bit9) 是对齐填充私有标记。

---

## 6. H743 VOS 复盘（最终版：2026-10-21 三轮更正后定稿）

**芯片**：H743 rev.Y（用户确认）。**教训总纲：拿不准 ST 外设用法时，先去抄 M5P01 同芯片工程 +
M0A00_Library 里的 STM32Cube HAL——同一块板跑通过的代码就是唯一真理。**

**正确编码（STM32CubeH7 V1.9.1 stm32h7xx_hal_pwr.h，H743 D3CR 版）**：
VOS[15:14] = 00:Scale0(rev.V+专用路径) / 01:Scale3(**复位值**,D3CR 复位=0x6000) / 10:Scale2 /
11:Scale1(=0xC000)。**原宏 0xC000 从头到尾是对的**；笔者两轮误判（把编码当反、把 Scale1 当
Scale0）皆因凭记忆不查库。

**真正的 bug：原序列缺少供电源配置一步**。官方顺序（RMP rmp_platform_stm32h743zi.h）＝
`HAL_PWREx_ConfigSupply(PWR_LDO_SUPPLY)` → `__HAL_PWR_VOLTAGESCALING_CONFIG(SCALE1)` →
等 VOSRDY。缺第一步（写 PWR_CR3：清 SCUEN/BYPASS、置 LDOEN，再等 CSR1.ACTVOSRDY）时
调压序列机不动作 → VOSRDY 永不置位 → 启动死等。**已按寄存器级照抄修复 M7M01 头**，实测：
CR3=0x2、CSR1=0xE000、D3CR=0xE000（Scale1+RDY 双绿）——**SCALE1 正确生效**。

**附带勘误（笔者排查中的错误结论，勿信）**：
1. "PWR 时钟在 APB1LENR bit28"——错。H743 的 PWR 时钟在 RCC_APB4ENR，且复位默认开；
   APB1LENR bit28 是保留位（写 1 读 0 属正常，不是芯片缺陷）。
2. "这颗 die 调压器只支持 Scale2/写入 D3CR 会毒化系统"——错。缺 ConfigSupply 时一切
   VOS 写入都会死等；配好供电源后 Scale1 一次到位。

**遗留已破案（同日终局）**：panic 真凶是笔者调 VOS 时把 M7M01 模板头手工拷进工作区，抹掉了生成器注入的 17 个布局常量（KOM 基址等）——见坑库 4.4 血泪头条。删工作区重新生成后，官方 VOS 序列 + 正确布局合体，Scale1 下九项 benchmark 全过（Yield 1082——注：该轮 Cache 未开，终版数据见 §7.1）。rev.Y 样片的调试口怪象（IDCODE 读数、DTCM 调试口 64KB 界）仍为真，但仅影响调试通道，不影响系统运行。
## 7. 成果基线（截至本文更新）

### 7.1 H743ZI 真机（COM14@115200，VOS Scale1 官方序列，AXI 布局，L1 Cache 开启）

GCC 日志 `Project\Script\h743_cache_gcc.txt`：

```
Yield 228   Notification 434   Semaphore 530   Queue 604
MemPair 338   ISR-Noti 1167   ISR-Sem 1045   ISR-Queue 1217   ISR-RVM relay 2031
```

Keil 日志 `Project\Script\h743_cache_keil.txt`（双链几乎打平）：

```
Yield 250   Notification 428   Semaphore 527   Queue 604
MemPair 307   ISR-Noti 955   ISR-Sem 981   ISR-Queue 1113   ISR-RVM relay 1851
```

**Cache 前后对比（Yield）**：关 Cache 时 GCC 1082/Keil 995 → 开 Cache 后 228/250（4~5 倍）——
H743 无 Cache 跑 Flash 每取指都吃 AXI 等待，教训：M7 性能数据必须开 L1 Cache 才有意义。
vPortYield_Err 纵深断言零触发；FPU(FPV5_DP) 满负荷稳定。Cache 关闭轮（1082/995）与
更早轮的原始日志已随整理删除——**正式数据一律以 frt_test_*_rvm.h 头文件档案块为准**。
原生 RMP 基线对照见 rvm_test_stm32h743zi.h 头注（Yield self 285，同为开 Cache 条件）。

### 7.2 构建态

- H743：GCC+Keil 双链 0 错 0 警（Keil kernel_keil.bin 342980B）
- F429：GCC+Keil 双链 0 错 0 警；GCC 已回归重建（含 peek 修复）；F429 rvp/工作区就绪，**真机轮待板**
- 开关验证：GCC OFF 态符号全 GC、text -804B；Keil map 证实 port.o→tasksrvm.o(peek)→portasm.o 全链

### 7.3 本次沉淀的修复清单

1. M7M01 `rme_platform_stm32h743zi.h`：VOS 段重写（跳过切换，注释完整）——上游级修复
2. H743 rvp ×4（gccmf/rvmdk × fpu/nofpu 的 -frt 派生参考）：Kernel Data_Base→0x24000000（AXI）
3. `tasksrvm.c` peek 绕回检查修复（A7M 全系受益）
4. `frt_test_stm32h743zi_rvm.h`/`frt_test_stm32f429zi_rvm.h` + A7M port 五文件双链 + frt_gen.cpp

### 7.4 F429ZI 真机（2026-10-21 当日收官，双链全过）

GCC 数据（原始日志已删，正式档案见 frt_test_stm32f429zi_rvm.h 头文件）：

```
Yield 352   Notification 648   Semaphore 801   Queue 909
MemPair 640   ISR-Noti 1548   ISR-Sem 1610   ISR-Queue 1775   ISR-RVM relay 2778
```

Keil 数据（原始日志已删，正式档案见同头文件）：

```
Yield 300   Notification 552   Semaphore 700   Queue 780
MemPair 453   ISR-Noti 1386   ISR-Sem 1416   ISR-Queue 1551   ISR-RVM relay 2594
```

对照原生 RMP 基线（rvm_test_stm32f429zi.h 头注 GCC Yield self=336 / ARMCC 332）：
FreeRTOS 版 Yield 352(GCC)/300(Keil)，差值即 peek+脏检查+FreeRTOS 调度开销，量级吻合 ✓。
F429 一次点火成功——未遇到 H743 的任何坑（F4 电源序列本来就正确）。
烧录模板：Project\Script\flash_f429.tcl(GCC) / flash_f429_keil.tcl(Keil)（换板需按 §2.4 重建），均含 CPUTAPID 0 +
connect_assert_srst；F429 调试要点：DBGMCU_IDCODE@0xE0042000 读 DEV_ID=0x419 确认身份。

---

## 8. 一句话军规

1. 内核文件（tasks.c 等）永不改——需要内核内部状态就 include 它（tasksrvm.c 模式）。
2. 大文件用 here-string 整写；写完校验行数与首尾。
3. 改了源头平台头≠进了工作区；改了 .c≠make 会重编——物证（反汇编/flash 指令字）说话。
4. “数据全垃圾”先查 RAM 黑洞再查逻辑；“卡住”用断点裁决别猜。
5. 单命令 30s 红线；跨超时用分离进程；ST-Link 被锁先杀 openocd。
6. GCC/Keil 四文件哈希必须一致；port 改完跑 update.sh。
7. **会话收尾三清**：生成的工作区（GCCMF-*/RVMDK-*）全删、生成的脚本与日志
   一律收进 Project\Script\（git 已忽略）、测试结果写进 frt_test_*_rvm.h 档案块——
   Project 根只允许留 rvp 与仓库自带的 sh，git status 必须干净。
