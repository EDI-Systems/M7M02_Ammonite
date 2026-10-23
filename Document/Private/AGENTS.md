# RVM 客机移植工程手册（坑库与方法论）

> 覆盖：FRT(GCC/Keil/IAR)×(G071/F429/H743/V307)、RMP IAR 链、
> uC/OS-III 移植（A6M/RV32P/A7M 三端口已全部真机 9/9 定档）。
> 面向贡献者：开工先读 §1、§2；排障查 §3、§4；改 A7M port 前必读 §5.1；uC/OS 移植要点在 §8。

---

## 1. 项目拓扑与路径速查

### 1.1 仓库关系

| 仓库 | 角色 | 关键事实 |
|---|---|---|
| `../FreeRTOS_V900`（本仓库的姊妹仓） | FreeRTOS 官方完整树 + RVM 移植层 | **内核文件永不改**（tasks.c/list.c/queue.c/timers.c/include）；改动只落 portable/*_RVM/ 与 test/ |
| `../uCOSIII_V308`（姊妹仓） | uC/OS-III 客机包（三仓嵌套） | uC-OS3 V3.08.02 + uC-CPU + uC-LIB，全部官方 Apache-2.0 零修改；我们的件在 *-RVM 目录 |
| `../M7M01_Eukaron`（姊妹仓） | RME 微内核（RVM 内核底座） | 平台头 `Include\Platform\{A6M,A7M}\Chip\<CHIP>\rme_platform_*.h`——**只是模板**（见坑 4.3 头条） |
| `../M5P01_Prokaron`（姊妹仓） | RMP RTOS + RMP-on-RVM 移植 | A7M_RVM fast yield 参考实现；IAR 版 A7M 汇编金矿（裸机工程=FPU 配置实证值出处） |
| 本仓库（M7M02_Ammonite） | RVM hypervisor + 生成器 + 工作区 | 生成与构建主战场，见下 |

### 1.2 M7M02_Ammonite 内部

- `Project\*.rvp`：工程描述 XML（`gccmf-*`=GCC/Makefile，`rvmdk-*`=Keil，`ewarm-*`=IAR；`-frt`/`-uo3` 后缀=客机类型；`-nofpu`=无协处理器）
- `Project\<TOOLCHAIN>-<CHIP>-<GUEST>\`：生成的工作区（Kernel/Monitor/Proc1/Proc2/Virt1 五子工程）
- `Project\benchmark.sh <rvp>`：生成（带 `-b` 基准模式，挂 test 头/源）——**客机链必用**，否则 Virt1 编不过；`generate.sh` 出静默版
- `Utility\M7M02_Final\rvgen.exe`：生成器；源在 `Utility\M7M02_Projgen\Source\`，改后 g++ @resp.txt 重建（§6.1）
- `Utility\M7M02_Final\bincopy.exe`：把 VM .bin 转成 `*_image.c` 供 Kernel 链接
- `Guest\update.sh`：发布镜像刷新（增量单向，不参与构建，规范见 §6.3）

### 1.3 工具链

| 工具 | 版本 | 备注 |
|---|---|---|
| arm-none-eabi-gcc | 13.2.1 | GNU Arm 工具链 |
| make | MinGW 版 | Windows 下驱动生成的 Makefile |
| armclang / UV4 | ARMCLANG 6.23 | UV4.exe -b 命令行构建 |
| ICCARM | 9.40 | iarbuild/ielftool/ielfdumparm/iasmarm 全家 |
| MinGW g++ | 12.2 | 重建 rvgen 用 |
| OpenOCD（WCH fork） | 0.11.0 | 坑多见 4.6；STM32 烧录主用 STM32CubeProgrammer CLI |
| 调试串口 | 板载调试器 VCP | 115200-8N1；COM 口号随系统枚举变化（发现方法见 4.2），勿写死 |

### 1.4 客机端口目录布局（现行）

```
FreeRTOS_V900\portable\
├── GCC\{ARM_A6M_RVM, ARM_A7M_RVM, RISC-V_RV32P_RVM}
├── IAR\{ARM_A6M_RVM, ARM_A7M_RVM}          ← RVDS 承载 armcc 端口（官方命名）
└── RVDS\{ARM_A6M_RVM, ARM_A7M_RVM}          ← RV32P 仅 GCC（Keil/IAR 生成会被存在性检查拦）

uCOSIII_V308\
├── uC-OS3\Ports\ARM-Cortex-M\{ARMv6-M-RVM, ARMv7-M-RVM}\ 与 Ports\RISC-V\RV32-RVM\
│   -RVM 内：os_cpu_c.c（架构层）+ 编译器子层 GNU|GCC\{os_cpu.h, os_cpu_a.s, os_cfg.h, os_cfg_app.h}
│   （cfg 平铺在编译器子层=每编译器一份；ARM/IAR 子层预留）
├── uC-CPU\ARM-Cortex-M\{ARMv6-M-RVM, ARMv7-M-RVM}\{cpu_cfg.h, cpu_rvm.c, lib_cfg.h} + GNU\cpu.h
│   （cpu.h 按编译器分层；A7M 另有 GNU\cpu_a.s=CLZ/RBIT 加速，生成器存在即编译；
│    RISC-V 特例：官方 uC-CPU/RISC-V 下无架构细分层，RVM 变体扁平挂根 RISC-V-RVM）
├── uC-LIB\（官方头走 include path，.c 不编译）
└── uC-OS3\Test\{uo3_benchmark.c, uo3_main.c, chip\uo3_test_{stm32g071rb,stm32f429zi,stm32h743zi,ch32v307vc}_rvm.h}
```

端口内容五件套（FRT）：FreeRTOSConfig.h / portmacro.h / port.c / portasm.s（/ tasksrvm.c 仅 A7M）。
IAR 与 GCC/Keil(RVDS) 的 C 件差异仅三处：portNOP→`__asm("nop")`、`__VFP_FP__` 守卫扩 `__ARMVFP__`、GCC 字面量池内联 asm→`LDR R3,=pxCurrentTCB`（IAR 内联汇编直接支持 =label）。

---

## 2. 标准工作流

### 2.1 生成

```sh
cd Project            # 本仓库根起
bash benchmark.sh gccmf-stm32h743zit6-frt.rvp   # 20-28s/链
```

- 改 rvp 或生成器后**必须删工作区再生成**（Project_Overwrite=No，旧 .ewp/.icf/Makefile 不刷新——白编两轮的教训）。
- 30s 命令上限内恰好一趟；**detached 并发多链会被拖超时**，逐链前台 + `timeout 28` 兜底。
- 生成日志确认 Virt1 源路径含 `portasm.s`（RV32P/A6M/A7M）、A7M 另有 `tasksrvm.c`。

### 2.2 GCC 构建（顺序固定：镜像先、Kernel 后）

Monitor → Proc1 → Proc2 → Virt1 → Kernel（Kernel 链接各子工程 `*_image.c`）。
**makefile 不是默认名**：裸 `make -C 目录` 必报 No targets；名字=子工程输出名：`kernel`/`monitor`/`prc_proc1`/`prc_proc2`/`prc_virt1`（如 `make -C Kernel/Project -f kernel -j8`）。被超时杀掉的 make 直接重跑（对象缓存续编）。

### 2.3 Keil 构建

`UV4.exe -b <dir>\Project\<name>.uvprojx -j0 -o <dir>\build.log`，读 log 尾判 0 Error。
Kernel 平坦 bin 手动 `fromelf --bincombined --output kernel_keil.bin Object\Kernel.axf`（**必须 --bincombined**，普通 --bin 多区段输出 1 字节废文件；文件名被句柄锁死时换名）。

### 2.4 IAR 构建

`common\bin\iarbuild.exe <工程>.ewp -build Debug`（无 GUI 无需 .ewd）；依赖序同 2.2，**Kernel 最后**。
镜像转换 `arm\bin\ielftool.exe --bin xxx.out xxx.bin`；postbuild 会自动跑 ielftool+bincopy 出 `*_image.c`（`&&` 须写 XML 实体 `&amp;&amp;`）。
**子镜像重编后 Kernel 必须重编**——内嵌 bin 会过期（判据：Kernel.bin 尺寸差几十字节，F429 复现过 230968→230884）。

### 2.5 烧录

STM32 主用：`STM32_Programmer_CLI -c port=SWD -w Kernel.bin 0x08000000 -v -rst`。
**Kernel.bin 偏移=flash 地址−0x08000000**（单平坦镜像内嵌全系统，别用子工程基址算）。
flash 读回：`-c port=SWD -r <addr> <size> <out>`，**输出必须 .bin 后缀**（.txt 报 Wrong file path）。
OpenOCD 备用（模板 Project\Script\）：tcl 要点 `set CPUTAPID 0`（新硅片 idcode 拒识，放 source target cfg **之前**）→ init → reset halt → flash write_image erase → verify → reset run；F429 新硅片 flash 前加 `reset_config srst_only srst_nogate connect_assert_srst`（H743 同款可用）。CH32V307 继承坑：多区段 hex 烧录会损坏，一律烧平坦 kernel.bin。

### 2.6 串口捕获与判读

Start-Process 分离跑捕获脚本（30s 超时下存活），先开端口再烧录，事后读文件。波特率从内核平台头 BRR 反推。
**基准完成判据（铁律）**：①中断相关条目全部打印齐（FRT 链：ISR-Noti/ISR-Sem/ISR-Queue/ISR-RVM relay 四行）——按硬件中断节拍慢出是**正常现象**（G071@TIM3 5kHz 需 90s+；H743 类高速芯片几乎紧随主体）；②终局标志=开始打印 `10000 overflows`。未齐前禁止结束捕获/判读/当挂死处理。反过来只见到 overflow 刷屏=旧固件空转输出，别急着怀疑新链——重烧+确认看到**自家横幅**为止。

### 2.7 会话收尾三清（必做）

1. **删工作区**：GCCMF-*/RVMDK-*/EWARM-* 全删（可由 rvp+benchmark.sh 复生）
2. **收生成物**：烧录/捕获脚本与原始日志收进 `Project\Script\`（git 已忽略）；可再生的 log.txt/report.txt 删
3. **结果入档**：benchmark 数据写进 `*_test_*_rvm.h` 头文件档案块（正式档案），AGENTS.md 只留索引
验收线：Project 根只允许 *.rvp 与仓库自带 sh；`git status --porcelain -- Project/` 必须空。

---

## 3. 排障方法论（按威力排序）

1. **断点裁决法**（启动类死机）：反汇编/nm 拿候选等待环与里程碑地址（VOS/HSE/PLL/Boot），各下硬件断点，resume 数秒后读 PC——命中即进度。注意 do-while 首过也碰断点，**命中≠卡死**；判卡死用"resume+sleep+halt 后读 PC"。
2. **写黑洞测试**（数据全垃圾类）：可疑地址写魔数读回，读不回/挂死=无 RAM。H743 DTCM 标称 128KB 实测 64KB 即此法钉死。"字段全 0/全垃圾"先查 RAM 黑洞再查逻辑。
3. **Fault 现场活捉**：内核善后会清 PSP/CFSR——在 `UsageFault_Handler`/`_RME_Thd_Fatal` 入口下断点（第一次 fault），读 PSP 帧 8 字，PC/XPSR 即真相（PC=0/XPSR=0→INVSTATE，配 CFSR 互证）。
4. **面包屑取证法**（IAR 链验证两轮）：`__RME_A7M_Exc_Handler` 入口塞临时打印块——Reg->LR/SP/R4-R11 + IPSR + HFSR/CFSR/MMFAR/BFAR + 栈帧 [SP+24]=PC/[SP+28]=XPSR + MSP 后 24~30 词转储（找活值=案发函数指纹、找深层返回地址）。**UNALIGNED 类故障：HW 帧 LR 槽=BL 返回点、PC 槽=真故障指令**。CFSR 按内核自己的头解码（别按网上直觉猜位号）。取证完清零残留。
5. **物证原则**："烧的是什么/编的是什么"用 flash 指令字比对 objdump；make 依赖**不含外部系统头**，改平台头后 `rm -rf Object` 强制全编再反汇编确认常量变了。
6. **防超时三原则**：单命令<30s（构建拆单工程）；跨超时存活的用 Start-Process 分离写文件；被杀的 openocd **会锁 ST-Link**（下次报 read version failed）→ 先 `Stop-Process -Name openocd -Force`。

---

## 4. 坑库

### 4.1 文件编辑类（三条同根：分段拼接产生错位）

1. **头注释收尾丢失**：`*/` 被裁 → `.syntax unified/.arch` 被吞进注释 → GAS 退回 divided 语法 → 满屏 `lo register required`。**指纹：见这批错先查汇编指示字是否在注释里**。
2. **悬空 `/*`**：插入点落在注释块中间 → 函数体吞成文件域 → `expected identifier before volatile`。
3. **armasm 宏缺 MEND**：尾部裁剪吃掉 → `A1291E: Macro definitions cannot be nested`。

**规矩：大文件 PowerShell 单引号 here-string 整写**（`[IO.File]::WriteAllText($p,$c.Replace("`n","`r`n"))`），写完验行数+抽查首尾。editor 分段插入只用于 <100 行小改。
**PS here-string 分段缝合坑**：上一段未以 `'@` 收尾就接 marker，下段 `Add @'` 被吞进字符串落进产物（症状：产物含 `Add @'` 行、相邻行粘连）——每段必须 `'@` 收尾再放 marker。
**汇编文件必须整文件成文或逐函数核对**（历史判例：分段把 `_RMP_A6M_Yield` 腰斩，DSB/BX LR 落在 END 后成死文本，首切即 HardFault 仅 2 字节输出）。

### 4.2 PowerShell/工具环境类

- **原生命令 stderr=错误记录**：openocd/make/gdb 的 stderr 首行中断管道。用 `*>&1 | Out-File` 或 Start-Process -RedirectStandard*（最稳）。
- **cmd /c 嵌套引号必失效**：含 `>` 的长命令经 PS→cmd 丢输出。拆进 .tcl/.bat 或 Start-Process 参数数组。
- **msys 抢 PATH**：装了 msys/Git Bash 后裸 `cmd` 可能解析到其 cmd.exe——需要真系统 cmd 时用绝对路径调用。
- **-creplace 反向引用数字粘联**：`'$1'+'0x14000'` 解析为组 10 → 用 `'${1}0x14000'`；-replace 不分大小写，必须 -creplace。
- **Remove-Item 对被锁文件弹"朗读提示不可用"失败** → `cmd /c del /f /q`。
- **Get-Content 数组负索引回绕**（尾+头拼接酷似文件翻倍）——先看总行数再下结论。
- **run_commands JSON 吃一层反斜杠**：含 `\\` 的 Replace 用 `[char]92` 拼 needle；大文件内容一律走 editor 工具不走 PS。
- **批量文本替换两坑**：①删除规则必须白名单式列名或 `^#define` 行首锚——前缀式（如 `^#define CPU_INT_`）会撞车误杀 CPU_INT_DIS/EN 宏（链接期 undefined reference 才暴露）；裸匹配宏名会误伤调用处（把调用行改成 define）。②双引号串里反引号转义 `` `$1 `` 会把字面 `` ` `` 写进产物（stray '`' 编译错）。
- **COM 口发现**：`Get-CimInstance Win32_PnPEntity | ? Name -match 'COM\d'`（随插拔变，勿写死）。

### 4.3 生成器/构建类

- **【头条】M7M01 平台头只是模板**：生成器向工作区 `Kernel\Include\rme_platform_*.h` 注入 ~17 个布局常量（RME_KOM_VA_BASE/RME_GEN_ENABLE 等），源头文件是 GEN_ENABLE=0+占位地址。改内核平台头：源头改**非布局代码** → 删工作区重新生成。**绝不能把源头文件手工拷进工作区**（布局常量被抹成模板值 → 内核对象建错地址 → init panic、转储全垃圾指针）。判别：panic 转储 R4 恒定垃圾值 → 先查 GEN_ENABLE 是否被抹。
- **工作区 Include 是生成器拷贝**：改源头平台头后已生成工作区不自动更新（同上解法）。
- **Kernel 链接依赖各 VM 的 `*_image.c`**：报 `No rule to make target 'virt1_image.o'` 即构建顺序错了。
- **生成器源里列了不存在的 .c** → make `No rule to make target`（实证：V3.08 无 os_int.c，OSIntEnter/Exit 在 os_core.c）。
- V3.08 uCOS API 签名（与老版本假设不同）：OSTaskCreate **13 参**（多 q_size/time_quanta/p_ext）、OSSchedRoundRobinCfg(en,quanta,&err) **en 在前**、OSSemCreate 4 参无 max、RoundRobinYield 带 &err。

### 4.4 iasmarm 汇编类（与 armasm/GAS 差异速查，全是实证）

**宏语法（最大坑）**：
- 原型 **`name MACRO 参数1, 参数2`**（名字与 MACRO 同行、标号字段），结尾 **ENDM**（非 MEND）。armasm 三件套（MACRO 换行写名 / `$param` / 带缩进）全报 Error[35] No name on macro——别再试。
- 正文引用参数=**裸文本 token 替换（无 $ 前缀）**：`LDR R1,=EXCXPSR`。
- **参数名禁用裸寄存器名**：参数叫 `LR` 则 `MOV LR,R1` 的 LR 被替换成立即数！用独名（现定稿 LABEL/SZ/XPSR/EXC_RETURN，宏定义行=宏名补齐 16 列+MACRO+3 空格→参数列 col 25）。
- 跳转标签当普通参数传（`ALIGN_PUSH MACRO skipTo … BEQ skipTo`）；宏内 `IF 表达式/ENDIF` 条件展开可用。

**字面量与指令**：
- ★★ **`LDR Rx,=代码段标号` 字面量自动 +1（Thumb 位）**（armcc/gcc 不加，唯独 IAR 加）。当 BX/BLX 跳转目标恰好正确；**当段内数据表指针用=奇地址非对齐 fault**（实证：Cop_Clear 字面量 0x08006239 vs 符号 0x08006238，UNALIGNED→HardFault）。**军规：=标号只准用于跳转目标或数值字面量；数据指针用途必须加载后 `BIC Rx,Rx,#0x01`**。`ADR` 对 THUMB 标号直接 Error[559]（反证其行为）；`DATA` 指令可解但引 Lt056 警告（破坏 0 警标准）。注意：RMP 客户 asm 里伪造异常帧 PC 的 `_RMP_A7M_Skip` 奇字面量是**故意加位，勿"修"**。
- 无 `DCI`（armasm 专属）——CODE 段手工编码指令用 `DC16`（触发 Warning 410，压法见 4.5）；**禁止改成真指令**（无 FPU 配置下汇编会挂——原作者手工编码的原因）。
- 段名**不许含内部点**：`.text.rme_vector` 报 Error[116] → 单点段名 `.rme_vector` 等，icf 选择器同步。
- `sfe(CSTACK)` 需模块内先 `SECTION CSTACK:DATA:NOROOT(3)` 声明（IAR 官方启动模板同款），否则 Invalid syntax；空段 GC 无副作用。
- **双操作数移位=Warning[408]**：`LSLS R4,#16` 必须写三操作数 `LSLS R4,R4,#16`（AND/ORR/SUB 同理写三操作数形式）。
- `MOV LR,#0xFFFFFFFD` 会被自动优化为 `MVN.W LR,#2`（编码等价，正常）。
- `MSR XPSR,R0` 被 iasmarm 映射为 XPSR_nzcvq 编码（非特权语义等价——APSR 可写、ICI/IT 不可写本就要过滤）。
- 无 PROC/ENDP；导出 PUBLIC、导入 EXTERN、段 SECTION；文件尾显式 LTORG。
- **单文件快检法**：`iasmarm --cpu=Cortex-M0+ <file> -o t.o`（M4: --cpu=Cortex-M4 --fpu=VFPv4；M7: --cpu=Cortex-M7.fp.dp）——进 iarbuild 前先过这关省整链时间。

### 4.5 IAR 工程与工具类

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

### 4.6 OpenOCD (MounRiver fork) 类

- 回显裁剪（reg/mdw/poll/mww 无回显）→ `mem2array a 32 <addr> 1; echo "$a(0)"`；对某些地址（0x5C001004）失败中断脚本 → `catch {...}` 包裹。
- 新硅片 idcode 拒识（F429 报 expected 0x2BA01477 实测 0x6BA02477）→ `set CPUTAPID 0` 在 source target cfg 之前。
- 运行态设 flash 断点必败（无报错直接死）→ 先 reset halt/halt。
- gdb 连接（3333 起 server）实测协商异常连不通，弃用。

### 4.7 硬件与芯片头类

**H743 档案（NUCLEO-H743ZI，rev.Y）**：
- **VOS 复盘（定稿）**：正确编码 D3CR VOS[15:14]=11:Scale1(0xC000)，复位值 0x6000=Scale3。原宏 0xC000 从头到尾是对的。**真 bug=缺供电配置**：官方顺序=ConfigSupply(写 CR3：清 SCUEN/BYPASS 置 LDOEN，等 CSR1.ACTVOSRDY)→VOS 配置→等 VOSRDY；缺第一步则 VOSRDY 永不置位启动死等。已修 M7M01 头。教训：**拿不准 ST 外设用法先抄 M5P01 同芯片工程+M0A00_Library 的 Cube HAL——同板跑通的代码是唯一真理**；凭记忆不查库连错两轮。
- **DTCM 实测 64KB**（标称 128KB）：0x20010000+ 写黑洞。全链内存迁 AXI 0x24000000（Kernel Data_Base；SFTD/VMMD 栈留 0x20000000 低 64KB）。
- HSE=8MHz 来自 ST-Link MCO（HSEBYP 直注）；400MHz=8/4*400/2，APB1/APB2=100MHz，APB1 timer=200MHz（测试头 TIM2_CNT<<1 基于此）。
- rev.Y 调试口怪象：DPID 0x6BA02477；DBGMCU IDCODE 经 WCH fork 读出 0x70003F（非标准，未解，仅影响调试通道）。
- **M7 性能数据必须开 L1 Cache 才有意义**（关 Cache Yield 1082/995 → 开 228/250，4~5 倍——无 Cache 跑 Flash 每取指吃 AXI 等待）。

**芯片头寄存器坑（FLASH ACR 三坑 + 读回断言）**：
- **语句宏当值用=Pa078 病根**：`ACR=LATENCY(2U)|PRFTEN` 而 LATENCY(X) 是赋值语句宏 → 展开成双重赋值 UB。修法：语句宏只当语句调用，禁止套进表达式。
- **位域抄错芯片**：G0 FLASH_ACR 只有 LATENCY[1:0]（~0x03；~0x07 是 F1 残留）；G0 无 PRFTEN（bit8 是 F4 残留、写保留位违规）——PRFTH 钩子不能删，宏定义改 (0U) 自然 no-op。
- **LATENCY 写后必须读回断言**：`RME_ASSERT((ACR&0x03U)==2U);` 紧跟写入（A7M 四头全有，G071/L071 已补）。HAL 语义：FLASH_LATENCY_2=2 等待态=域值 2。
- **X=X= 双赋值笔误是本仓库高发模式**（G071 FLASH ACR、rme_platform_a7m.c:1237 LR_GET 行两例）——见 Pa078/Pa079 一律源头修。
- IAR DLib 进程入口链：flash 里 desc 后 `00 48 00 47`+字面量（LDR R0,[PC];BX R0→__iar_program_start），cstartup 在进程内照常完成 data/bss 初始化。

---

## 5. 设计决策记录

### 5.1 A7M_RVM fast yield（改 port 前必读）

**为什么必须 ICI/IT 检测**：ARMv7-M EPSR 携带 IT 块/可续传 LDM/STM 状态（掩码 0x0600FC00），三方不对称——硬件异常入口/出口自动保存恢复；线程态软件 MRS EPSR 读 0、MSR 写忽略。fast yield 手工伪造/恢复异常帧，脏帧（xPSR 带 ICI/IT）**只能经真实异常返回恢复**（慢路径），否则静默丢状态。A6M 无 IT/ICI 无此约束。

**三层架构（零内核文件改动）**：
```
port.c vPortYield():
  mask → xTaskPeekNextToRun()【tasksrvm.c 提供】
   ├─ peek==当前 → unmask, return（零切换）
   ├─ 目标帧脏(LR_EXC.4 选偏移, XPSR&mask) → unmask + RVM_Virt_Yield() 慢路径
   └─ 干净 → vPortYield_Swt【portasm.s】(真 vTaskSwitchContext 必选同任务——mask 冻结调度状态;
      EXC_POP 内 0x0600FC00 检测→vPortYield_Err 纵深断言)
```

**tasksrvm.c**：编译它**取代** tasks.c（不得同时入链）；`#include "../../../tasks.c"` 暴露文件内静态量，内核源零改动。peek=V9 通用选择的只读镜像（不推进 pxIndex、不写 pxCurrentTCB）。**真 bug 判例**：V9 `listGET_OWNER_OF_NEXT_ENTRY` 有 end-marker 绕回检查——pxIndex 前进落在 xListEnd 时需再进一步（end 的 pvOwner 是 padding=0），漏抄则 peek 返回 NULL→解引用 0 炸（H743 首测暴露）：
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

### 5.2 IAR 端口改编定式（从 Keil/GCC 版翻译）

| 件 | 改法 |
|---|---|
| FreeRTOSConfig.h | 原样拷（__ICCARM__ 分支已在） |
| portmacro.h | 仅 portNOP→`__asm("nop")` |
| port.c | ①`__VFP_FP__` 守卫扩 `|| defined(__ARMVFP__)` ②GCC 手工字面量池内联 asm→`LDR R3,=pxCurrentTCB`（IAR 内联汇编支持 =label）③dsb/isb 去 volatile |
| portasm.s | iasmarm 宏形态直译：`名 MACRO 参数…ENDM`、裸参数引用、IF/ENDIF 条件展开、DCI→DC16、二操作数→三操作数、`LDR =代码标号`后跟 BIC/ANDS 剥位、参数名大写独名 |
| tasksrvm.c | 原样拷 |

### 5.3 端口格式规范（清扫定稿，§10 家族标准）

- 缩进 4sp；操作数列 col 25；注释列 col 45（0-based 44，**勿写成 53**）；宏定义行=宏名补齐 16 列+`MACRO`+3sp→参数列 col 25；iasmarm 的 MACRO 名保持标号列 col 1（语法要求）。
- 宏参数**全大写**按 GCC 家族语义命名（LABEL/SZ/XPSR/EXC_RETURN），禁寄存器名。
- asm 文件无尾部版权脚注（仅 C 文件有）；A6M 的 EXC_POP 尾不加 `B vPortYield_Exit`（POP {R0,PC} 必跳转，死代码）；A7M 家族三版皆有故保留。
- C 件 GCC≡Keil(RVDS) 逐字节同（哈希校验）；portasm.s 语法不同属预期（IAR 用指令流比对验证等价）。

### 5.4 双工具链同步约定

port.c/portmacro.h/FreeRTOSConfig.h/tasksrvm.c 跨目录**必须哈希一致**（改完立即 Copy-Item+Get-FileHash 校验）；改 port 后跑 `Guest/update.sh` 刷新发布镜像。

---

## 6. 生成器知识

### 6.1 重建流程与坑

- 响应文件法：`g++ @resp.txt`（Release\resp.txt；~50 cpp + 预编译 xml.o；C++17/-O2/-static/-s）。**响应文件内路径必须正斜杠**（反斜杠被 GCC 当转义吃掉）；xml.c 是 C 不能混编 C++（用预编译 xml.o）。编译 ~60s 超 30s 红线 → Start-Process 分离跑，完成后校验 build_err.txt。
- **时间戳假新鲜**：旧 rvgen_new.exe 未删时 Test-Path 即真会换装陈货——换装前比对 exe 与 build_err 的 LastWriteTime。
- **resp.txt 输出名是 rvgen_new.exe**：重建完成后须手动复制部署到 Release\rvgen.exe 与 Utility\M7M02_Final\rvgen.exe 两处（benchmark.sh 用后者）。
- 产物部署两处：Release\rvgen.exe + Utility\M7M02_Final\rvgen.exe（benchmark.sh 用后者）。
- **客机生成器三件套 helper 家规**（FRT/UO3 同构）：`Plat_Get()`（平台→端口目录名）、`Toolchain_Get(Virt)`（工具链→目录名）、`Port_Dir_Get(Virt)`（合成端口路径）；hpp 声明+cpp 定义+完整函数注释块；Process_Virt_Hdr/Src 里调用之，不写裸三分支。
  - FRT 映射：A6M→ARM_A6M、A7M→ARM_A7M、RV32P→RISC-V_RV32P；ARMCC→RVDS、ICCARM→IAR、其余→GCC。
  - UO3 映射：A6M→ARMv6-M、A7M→ARMv7-M；ARMCC→ARM、ICCARM→IAR、其余→GNU（Micrium 命名）。
- 平台兼容表：rvp 的 Guest 组合要在 `Include\Platform\<ARCH>\rvm_platform_*.rva` 的 `<Guest>` 与 `<Compatible>`（C 行=Buildsystem,Toolchain,Guest 三元组）里开；漏开报 "Combination 'X/Y/Z' is not found in compatible list"。
- 命名约定：Buildsystem=IAREW / 工程前缀=EWARM / Toolchain=ICCARM / 汇编后缀 _iccarm.s。

### 6.2 Guest/ 镜像规范 v1.0

- 定位=全部客机"我方增量件"的镜像暂存区；官方件永不入镜。update.sh：`rm -rf ./*/` 后逐条 `cp -r`（幂等）。
- 条目规则：尾随空格分词、无尾斜杠、大小写逐字；按客机分段、段内变体成对、测试收尾；粒度随客机原生分类；循环带 `[ -d ] || skip` 守卫（可提前挂未来条目当 intent 文档）。
- 当前清单：FRT 8 条（portable/{GCC,RVDS,IAR}/ARM_*_RVM×2 + RISC-V_RV32P_RVM + test）+ UO3 7 条。

---

## 7. 成果基线索引（正式数据一律以 test 头文件档案块为准）

### 7.1 FRT（FreeRTOS-on-RVM）链状态

| 芯片 | GCC | Keil | IAR | 正式档案 |
|---|---|---|---|---|
| G071 (A6M) | ✅ 真机 | ✅ 真机 | ✅ 真机（Yield 408） | frt_test_stm32g071rb_rvm.h（GNU/ARMCLANG/ICCARM 三块） |
| F429 (A7M) | ✅ 真机（Yield 352） | ✅ 真机（300） | ✅ 真机（308） | frt_test_stm32f429zi_rvm.h（三块） |
| H743 (A7M) | ✅ 真机（Yield 228） | ✅ 真机（250） | ✅ 真机（220，三编译者最优） | frt_test_stm32h743zi_rvm.h（三块+历史注） |
| V307 (RV32P) | ✅ 真机（基线） | N/A | N/A | frt_test_ch32v307vc_rvm.h |

- 全矩阵编译回归：10/10 可测组合五工程全绿（V307×Keil/IAR 结构性 N/A——新树无 RV32P 的 RVDS/IAR 端口）。
- **UO3 P0+P1 真机闭环**：GCC/G071 快路径 9/9 全过、0 error（**Yield 810**；慢路径 2840 留历史注），
  档案 uo3_test_stm32g071rb_rvm.h；调试全史与根因（CPU_SR_Save 语义反转、ISR 缺 OSIntEnter/Exit 协议）见 §9 军规 13 与 §8。

### 7.2 UO3（uC/OS-III-on-RVM）链状态

| 芯片 | GCC | Keil | IAR | 正式档案 |
|---|---|---|---|---|
| G071 (A6M) | ✅ 真机（**快路径 Yield 806**，同尘化定档） | P2 待做 | P3 待做 | uo3_test_stm32g071rb_rvm.h（GNU 块+两段历史注） |
| V307 (RV32P) | ✅ 真机（**9/9 全绿，Yield 455** 对 FRT 同板 313；栈溢出根因见 §8.1） | N/A | N/A | uo3_test_ch32v307vc_rvm.h（GNU 块 Note(2)） |
| F429 (A7M) | ✅ 真机（**9/9 全绿，Yield 536/Relay 2777**；两案已破：烧录路径错→横幅断言、Relay 跨钟相位偏移→客机计时器换回内核同款 TIM2） | P2 待做 | P3 待做 | uo3_test_stm32f429zi_rvm.h（GNU 块 Note(2)） |
| H743 (A7M) | ✅ 真机（**9/9 全绿，Yield 366**（硬 CLZ 版）；A7M 端口与 F429 共享零改动） | P2 待做 | P3 待做 | uo3_test_stm32h743zi_rvm.h（GNU 块 Note(2)） |

- P0+P1（已闭环）：同优先级 RR 编排；OSCtxSw=asm 任务栈直接换帧（FRT portasm 骨架，
  **已全文重排为 uC/OS-III 官方 asm 风格**：104 列星线框/大写助记符/注释列 64/OS_CPU_ 前缀宏），
  OSIntCtxSw=慢路径超调用；RVM 7/7 + UO3 9/9 + 溢出标记全过。
- 定档演进：慢路径 2840 → 快路径 810 → 同尘化+CPU 类型 806（Mem 428→277、MsgQ 1157→1025，
  uint32_t→CPU_INT32U 影响 GCC 代码生成）。ISR relay 三轮恒 3340±。
- **同尘化彻底完成**：端口层（os_cpu_a.s 官方 asm 风格/cpu.h 补齐/cfg 每编译器一份平铺于 GNU/）
  + benchmark 层（BENCH_* 宏/Bench_* 函数/驼峰 static/小写局部/官方空格与函数头框注释）全部对齐
  uC/OS-III 官方规范；重命名后成绩逐项不变。

### 7.3 RMP-on-RVM IAR 链

G071/F429/H743 三板全部真机闭环（RVM 层数据归档 rvm_test_stm32*.h 各 ICCARM 块）。G071 真机 Kernel.bin 102876B；H743 342476B / F429 230968B（IAR RMP 链）。

### 7.4 数据判读参考量级

- ICCARM(-Oh) 通常介于 ARMCLANG -O3 与 GNU -O3 之间（三板 FRT 一致）。
- M7 板（H743）必须开 L1 Cache 的数据才有效（4~5 倍差）。
- 低端芯片（G071，TIM3 5kHz 节拍）ISR 项需 90s+ 出齐。

---

## 8. uCOS-III on RVM 移植要点

- **客机包**：`uCOSIII_V308` 三仓嵌套（uC-OS3 V3.08.02 + uC-CPU + uC-LIB），官方 Apache-2.0 零修改。
- **依赖闭合**：lib 头用官方（include path 指 uC-LIB 根，.c 不编译——函数引用全在 NAME_EN=0 死代码）；cpu_cfg.h 注意：TS_TMR_SIZE 必须 `CPU_WORD_SIZE_32`（非 32u）、**不可定义 LEAD/TRAIL_ZEROS_ASM_PRESENT**（#ifndef 语义，定义了就把 C 版 CLZ 裁掉链接缺符号）、TS_TMR_EN/PERF_MON_EN/INT_DIS_MEAS_EN 别定义（cpu_core.h 自带默认会重定义警告）。
- **CPU 侧替代**（cpu_rvm.c 六函数）：IntDis/En/SR_Save/Restore→RVM 掩蔽（嵌套深度放 CPU_SR 值）；
  **SR_Save 必须返回进入前的旧深度**（写成 `++Nesting` 返回新值会让 Restore 永远回不到 0，
  Mask 卡死 1，Vct 信号永不发出，tick/ctx 全灭——板上表现为"yield 返回 NONE 却切不动"+0 tick）；
  WaitForInt/Except→空存根（客机 WFI 停核饿死全家）。
- **OS 侧**（os_cpu_c.c=官方全文+手术）：九 Hook 保留；StkInit 尾加 5 超调用参数字（帧 22 字与 FRT 同构）；
  OSInitHook 的 ExceptStkBase 行摘除（客机无 MSP）；OSStartHighRdy 必须设 **OSRunning=1**（官方 asm 有，
  重写易漏——漏了 OSTaskChangePrio(NULL) 报 OS_NOT_RUNNING）；首任务入口跳转
  `pop{r4}(返回地址,弃) pop{r0}(入口) mov lr,r0`。
- **ISR 协议**：Vct 线程上跑的 ISR 必须 `OSIntEnter()→post(NO_SCHED)→OSIntExit()`——Exit 里才更新
  OSTCBHighRdyPtr 并触发 OSIntCtxSw；裸 post+RVM_Virt_Yield（FRT 模式）会让 Vct_Loop 尾部的
  OS_Ctx_Handler 拿旧 HighRdy 切错对象（"Interrupt reentered." 刷屏）。
- **Yield 编排**：uCOS RR yield 不变式 cur==就绪链 head；benchmark 两任务同优先级创建
  （T2 先建=head、领先 yield），勿用 OSTaskChangePrio 破坏链序（插尾）。V3.08 OSSched 在临界区内调
  OS_TASK_SW——Yield 落 pend 分支、由临界区退出的 Unmask 补发信号，属正常路径。
- **双路径切换（P1 定稿）**：OSCtxSw=GNU/os_cpu_a.s 的 asm 快路径（FRT vPortYield 骨架：伪造异常帧
  +R4-R11+超调用参数 5 字=22 字，任务栈上直接换帧零陷入；vTaskSwitchContext 块换成 uCOS bookkeeping
  ——OSSched 已选好 HighRdy，asm 只做 hook/OSPrioCur/OSTCBCurPtr/TCB 偏移 0 直写 StkPtr）；
  OSIntCtxSw=慢路径（ISR 上下文 Vct_Act=1 时 Yield 只设标志，Vct_Loop 尾部真切）。**帧 22 字三处一致**
  （StkInit/OS_Ctx_Handler/os_cpu_a.s）——快路径切走的帧可被 tick 慢路径恢复，反之亦然；
  XPSR bit9=0x200 是对齐 PAD 私有标记，EXC_POP 恢复时必须清除。
- **bsp_os_dt.c 弃用**（动态 tick BSP 模板）：RVM 下 tickless 禁死，tick 走 RVM_Virt_Tim_Reg。
- **同尘规范（UO3 包内所有自有件，长期有效）**：文件头=官方三框（产品框[uC/OS-III 或 uC/CPU+版权+NOT part
  声明]+文件框[File/Version/For]+Note(s) 框）；asm=104 列星线框/大写助记符/注释列 64/OS_CPU_ 前缀宏/
  官方 Note(s) 编号体；类型=官方 cpu.h 的 C 原生 typedef（**禁引 stdint.h**）；benchmark 命名=BENCH_*
  全大写宏 / Bench_* 驼峰函数 / 静态全局驼峰（Task1TCB 式）/ 局部小写下划线（cnt 式）/ 指针参数 p_ 前缀 /
  `for (`、`0u` 后缀小写、`(cast)值` 紧贴。**同尘以本树官方发行版现状为唯一基准**——V3.08 已无
  $PAGE/form feed，勿凭旧版记忆带入；官方文件自带的列混排（os_cfg.h 的 51/52、os_cfg_app.h 的
  58/59）原样保留不动，自有文件宏值列单列统一（chip header 51 / benchmark 53 / cpu_cfg.h 53+注释列 66）；
  os_cfg.h / os_cfg_app.h / lib_cfg.h 三件保留官方模板头。
- **SCS 瘦身（cpu.h 与源清单）**：cpu.h 删尽系统控制空间内容（SYSTICK/NVIC/SCB/CPUID/MPU 寄存器宏、
  中断源号、寄存器位/掩码段、MB/RMB/WMB、CPU_IntSrc*/CPU_BitBand/CPU_RevBits 原型）；官方 cpu_c.c
  **整体退出编译**（全是 guest 无权执行的特权死代码）。CLZ/CTZ 全家在 cpu_core.c（查表 C 版）照常可用
  ——Cortex-M0/M0+ 指令集无 CLZ/RBIT，官方 v6-M 端口同样无汇编 CLZ，C 查表即官方同款正解；汇编加速
  （CLZ 指令版 cpu_a.s + cpu_cfg.h 开 CPU_CFG_LEAD_ZEROS_ASM_PRESENT）是 **v7-M 模式**，A7M 端口时提供。
- **测试选择头职责**：uo3_test.h（生成器产物）=纯选择头，只 include chip header；BENCH_PRINT_* 等
  实体宏一律放 chip header。
- **布局判据**：端口目录跟客机自家惯例（FRT 用 portable/、UO3 用原生 Ports 树）；-RVM 后缀挂被变体化的官方节点上；兄弟同类（架构旁站架构）拒绝混层。
- **生成器**：Guest_Type=UO3（上游预留 -uo3）；A6M rva Compatible 已开三元组；RV32P 同开（C3 行）。

### 8.1 RV32P 移植要点（V307 一轮实战沉淀）
- **目录布局**：OS 侧 `Ports/RISC-V/RV32-RVM/GCC/`（RISC-V 官方编译器名=GCC 非 GNU）+架构层 os_cpu_c.c；
  CPU 侧**扁平特例** `uC-CPU/RISC-V-RVM/`+`GCC/cpu.h`（官方 RISC-V 下无架构细分层）。
- **帧布局（RV32）**：params5+mstatus（FS 位标记惰性 FPU 帧：RVF 33 字/RVD 65 字）+31 整数字
  （pc@0,x1@1,x3..x31；x2 sp 由帧隐含、x4 tp 承载恢复地址——mepc 用户态不可达，恢复用 jalr x0,tp）。
- **快路径三入口**：OSCtxSw/OSCtxSwRVF/OSCtxSwRVD（os_cpu.h 按 RVM_RV32P_COP_RVD/RVF 三选一）——
  FPU 指令 **.hword 手编码**（fsd/fld/fsw/flw 各 32 对表在 os_cpu_a.s，**逐字抄 FRT portasm.s 勿自算**）；
  SWITCH 的 bookkeeping（OSTaskSwHook+prio/tcb 三步）**直接在任务栈上跑**（G071 正统——浅调用且
  全局变量同用户 gp；FRT/RMP 的切内核栈+gp（__RVM_Stack/__RVM_Global）留给深调度器，uC/OS 不需要）。
- **任务栈预算（V307 三轮破案的根因）**：RV32 切换帧=37 整数字+快路径 FPU 瞬态 33 字=280B
  （A6M 仅 88B）→基准任务栈 **256 字起步**（128 字会溢出踩 .bss：就绪链/信号量表/RVM_Int_Mask
  全毁——症状为计数器乱码+ISR 链 reenter 刷屏，极易误判为切换逻辑 bug）。
  **BENCH_TASK_STK_SIZE 已下放芯片头 per-port**（G071=128/V307=256）——benchmark.c 无兜底，
  缺失即 `#error`（uC/OS 官方 os.h 风格）——新端口头文件必给此宏，按帧深×2.5 以上估。
- **调度时序正统（uC/OS 特有）**：HighRdy 的选择在 switch **外部**由 OSSched/OSIntExit 完成，
  Ctx handler 只消费不重算——勿仿 FRT/RMP 把调度塞进 handler（二者调度器在 handler 内跑是
  其架构使然，对 uC/OS 是画蛇添足且引入竞态）。
- **GDB 静态变量取证**：探针增删会使 .bss 符号跨构建漂移——**每次重编后必须重跑 nm 取当前地址**，
  否则读到的是别的变量（"垃圾值"假象）；无声计数器（handler++ 只读不打印）是零扰动判决利器。

### 8.2 A7M 移植要点（F429 一轮全绿沉淀）
- **目录/生成器**：OS 侧 `Ports/ARM-Cortex-M/ARMv7-M-RVM/{os_cpu_c.c,GNU/*}`+CPU 侧
  `uC-CPU/ARM-Cortex-M/ARMv7-M-RVM/`；A7M 平台 rva 需加 `<Guest>UO3</Guest>`+`<C9>Makefile,GCC,UO3</C9>`
  （两处都要）；芯片头名用 rvp 的 **Class 名**（stm32f429zi 非 stm32f429zit6）。
- **快路径=伪装异常帧（FRT A7M portasm 机制）**：入口 PUSH R0/XPSR→CONTROL.FPCA 分基本/扩展帧
  →EXC_PUSH 造帧（PC=退出标号，XPSR bit9 标 pad）→vstmia s0-s15+fpscr+vpush s16-s31（.short 机器码
  逐字抄 FRT）→params5+R4-R11+LR 入栈→Mask→存 SP→**trio**→载 SP→Unmask→解栈（原地变换：
  LR 先出、XPSR 经 msr XPSR_NZCVQ、PC 最后弹）。**A7M 可直接 PUSH {R4-R11,LR}**（A6M 需经 R0-R3 中转）。
- **ICI/IT 脏帧过滤（A6M 没有的 A7M 特有）**：EPSR ICI/IT 位只有真异常返回能恢复；OSCtxSw 用 C 包装
  先查目标帧（LR_EXC@13 位4 定扩展，XPSR 基本@21/扩展@37——**pad 永在帧顶，偏移两态固定**），
  脏→RVM_Virt_Yield 慢路径，净→asm 快路径；EXC_POP 内再设防御陷阱 OSCtxSw_Err。
- **调度正统（与 RV32 轮同结论）**：V3.08 源码实证 OSSched 只算 HighRdy 就调 OS_TASK_SW——
  **trio（OSTaskSwHook/OSPrioCur/OSTCBCurPtr）是端口 switch 的职责**（官方 PendSV 同款）；
  uC/OS 天然免 Peek（FRT 的 xTaskPeekNextToRun 是其调度器即提交所致）。
- **A7M 用汇编 CLZ（指令集事实，与 A6M/RV32 相异）**：ARMv7-M 有 CLZ/RBIT（A6M Thumb-1 无、
  RISC-V base 无）——官方 A7M cpu_a.s 提供 CPU_CntLeadZeros/CntTrailZeros/RevBits 并在 cpu.h 定义
  CPU_CFG_{LEAD,TRAIL}_ZEROS_ASM_PRESENT（定义在 cpu.h 不在 cpu_cfg.h）。RVM 版：cpu_rvm.c 内联
  汇编同语义（objdump 实证 clz r0,r0; bx lr 与官方逐指令一致），cpu_core.c 的 C 查表版被宏裁掉
  （#ifndef 守卫）。H743 实测 ISR 路径提速 4-5%（OSIntExit 每次扫 prio 表）。A6M/RV32 保持 C 版。
- **快路径开关 OS_CFG_RVM_FAST_YIELD_EN（照抄 FRT configUSE_RVM_OPTIMISED_YIELD 三层模式）**：
  ①os_cfg.h（端口编译器目录）定义选项（0u/1u 风格随本文件，默认 1u=开）+官方段落横幅注释；
  ②os_cpu.h `#if` 切换 OS_TASK_SW——开=快路径入口（A7M=OSCtxSw 包装/A6M=asm OSCtxSw/RV32=三选一），
  关=`RVM_Virt_Yield()`（"simpler, but traps on every switch"）；缺定义 `#error`（快败）；
  ③A7M os_cpu_c.c 的 C 件（偏移+Frame_Dirty+OSCtxSw）包 `#if`，但 **OSCtxSw_Err 留在 `#if` 外**——
  asm 无条件汇编恸引它，包进去关态即链接缺符号（实测验证）。H743 双态实测：开=Yield 372，
  关=**1526（4.1×）**，ISR 四项双态不变（ISR 路径恒走 OSIntCtxSw，天然自证开关语义）。三口
  （A6M/A7M/RV32）编译全绿。
- **CPU 层 asm=存在即编译（分层铁律）**：CLZ/RBIT/REV 等 CPU 抽象加速属于 **uC/CPU 层
  的 cpu_a.s**（`<arch>-RVM/<编译器>/cpu_a.s`，官方指令逐字），不进端口也不内联进 cpu_rvm.c——
  cpu_rvm.c 只留六个掩蔽函数。uo3_gen：端口 os_cpu_a.s **必需**（硬检查+无条件编译，上下文切换
  是端口本分）；CPU 层 cpu_a.s `exists()` 才编译——A6M/RV32 无此文件自动跳过走 C 查表（恰为其
  官方同款行为）。H743 全链+板上 9/9 实证。
- **生成器 Path_Conv 降序排序 bug（本轮修复）**：注释写"small to large"代码却是 `Left>Right`（手误翻转），
  使官方 `ARMv7-M/GNU` 排在 `ARMv7-M-RVM/GNU` 前→官方 cpu.h 抢先→CPU_CFG_KA_IPL_BOUNDARY 未定义。
  V307 幸免是因其官方 CPU 推送路径是拼错的 ARM 路径（不存在）。已改回 `Left<Right`（与注释一致），
  全链目录互不相交的 FRT/RMP/native 不受影响。
- **官方 CPU 编译器目录彻底退出 include 路径（设计定则）**：uo3_gen.cpp 不再推送官方
  `uC-CPU/ARM-Cortex-M/<arch>/<编译器>/` 目录——RVM CPU 层（RVM 架构层+编译器层 cpu.h/cpu_rvm.c）
  **自我完备**，缺 cpu.h 就该响亮地编不过，而不是静默解析到官方头埋雷（官方 cpu.h 的 BASEPRI 方案
  与 RVM 配置根本不兼容，兜底只会产生更晚爆的错）。F429/G071 双平台无官方目录全链 0 err 实证；
  与 FRT 生成器的单端口目录模型对齐。
- **CLZ**：RISC-V base 不保证 clz（Zbb 非强制）→ C 查表（cpu_core.c）即正解，与官方 RISC-V 端口一致。
- **烧录（WCH-LinkE+SDI）**：`adapter driver wlinke`+`sdi newtap`+`target create .. wch_riscv -chain-position`+
  `flash bank .. wch_riscv 0x0 ..`（**flash 驱动=wch_riscv**）；板级 wch-riscv.cfg 是 JTAG 版不可用；
  GDB 可用（openocd 3333+kernel.elf 符号）。**命令行传带空格参数给 OpenOCD 会被拆分——
  复杂命令一律走 cfg 文件**。

---

## 9. 一句话军规

1. 内核文件（tasks.c / uC-OS3 Source/os_*.c）永不改——要内核内部状态就 include 它（tasksrvm.c 模式）。
2. 大文件 here-string 整写；写完校验行数与首尾；汇编逐函数核对。
3. 改了源头平台头≠进了工作区；改了 .c≠make 会重编——物证（反汇编/flash 指令字）说话。
4. "数据全垃圾"先查 RAM 黑洞再查逻辑；"卡住"用断点裁决别猜；fault 用现场活捉/面包屑取证。
5. 单命令 30s 红线；跨超时用分离进程；ST-Link 被锁先杀 openocd。
6. GCC≡Keil(RVDS) 的 C 件哈希必须一致；port 改完跑 update.sh。
7. **会话收尾三清**：删工作区、生成物收进 Project\Script\、结果写进 *_test_*_rvm.h 档案块；git status 干净。
8. iasmarm 的 `LDR Rx,=代码标号` 自动 +1 Thumb 位——只准用于跳转目标；数据指针必须 BIC #1。
9. IAR 链子镜像先、Kernel 最后重编（内嵌 bin 会过期）；改 .ewp 配置改生成器源码，别在 IDE 里改存。
10. benchmark 未见到 overflow 刷屏就没跑完；只见到 overflow 刷屏=旧固件，先找自家横幅。
11. 拿不准 ST 外设用法，先抄 M5P01 同芯片工程+M0A00 Cube HAL——同板跑通的代码是唯一真理。
12. Pa078/Pa079（X=X= 双赋值）是本仓库高发笔误，一律源头修。
13. 自造 SR_Save/Restore 型掩蔽层时，**Save 必须返回进入前的旧状态**（`old = cnt++`，不是 `cnt++`）——
    返回新值会让配对的 Restore 永远回不到零 → 虚拟中断永久屏蔽 → tick/ctx 全灭、调度器死寂
    （症状：RVM 层黑匣子计数全 0、一切 yield 走 pend 无人处理）。FreeRTOS 的对称计数法与
    uC/OS 官方 PRIMASK 直读法均无此坑，自造实现最易踩。
