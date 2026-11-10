# The RVM Guest Library

This directory distributes the **paravirtualized RTOS ports** for the RVM
hypervisor: drop-in port layers that let unmodified RTOS kernels run as
unprivileged RVM guests.

Currently distributed:

| Guest | Versions | Architectures | Toolchains |
|---|---|---|---|
| FreeRTOS | V9.0.0 | ARMv6-M, ARMv7-M, RV32P | GCC, RVDS (ARMCC), IAR |
| uC/OS-III | uC/OS-3 V3.08.02 + uC/CPU V1.32.01 + uC/LIB V1.39.01 | ARMv6-M, ARMv7-M, RV32 | GCC, ARM, IAR |
| RT-Thread | V4.1.1 | ARMv6-M, ARMv7-M, RV32P | GCC, RVDS (ARMCC), IAR (RV32P: GCC only) |
| DjyOS | V1.1.0 | ARMv7-M | GCC (commemorative port, single combination: H743) |
| KLite | V5.2.1 | ARMv7-M | GCC (commemorative port, single combination: H743) |
| uC/OS-II | V2.93 | ARMv6-M, ARMv7-M, RV32 | GCC, ARM, IAR (RV32: GCC only) |
| RTEMS (RTE) | 7.0-rc1 | ARMv7-M (H743/F429) | GCC |
| Zephyr (ZPR) | 3.7.0 LTS | ARMv7-M (H743/F429) | GCC |

RTE is distributed as the `RTEMS_V701` BSP/spec/sample subset.  Its generated
build-time staging files (`prc_virt1.c`/`_desc.c`, `rte_prc.h`,
`rte_config.h` and `app/`) are produced by RTE_Gen and `rte_copy.sh`, so they
are not part of the mirror.  The RTE BSP references the M7M02 RVM `Include/`
and `Source/` trees through `RVM_ROOT`; the generated VM headers are staged by
`rte_copy.sh`, while the shared `rvm.h` and platform header are included
directly, so no RVM implementation is maintained in the BSP source tree.  RTE
defaults to the direct task-stack context switch fast path; setting
`RTE_FAST_PATH` to `False` in the generated `rte_config.ini` routes every
switch through the RVM vector thread and omits the assembly fast routine from
the image.  Cortex-M7/STM32H743 and
Cortex-M4F/STM32F429 (the connected board reports NUCLEO-F439ZI, same 0x419
family) are both verified; `RTE_CPU_FLAGS` selects the CPU/FPU and float-ABI
options from the `.rvp` coprocessor.  The BSP spec layout defaults are zero
sentinels and `linkcmds` asserts that every layout value came from the
generated `rte_config.ini`, so a direct build cannot silently reuse a stale
memory map.  The RTEMS build uses the BSP `-O2` optimization
flags; performance archives live in
`testsuites/samples/rte_benchmark/chip/rte_test_<chip>.h`.  That same chip
header owns the benchmark's TIM2/TIM4 setup and IRQ clear
(`RTE_Test_Timer_Init`/`RTE_Test_Int_Init`/`RTE_Test_Int_Disable`/
`RTE_TEST_INT_CLEAR`); `init.c` only defines the handler and measurement
flow, following the RMP test-header split.  A benchmark
project (`-b`) builds the benchmark into every Virtual guest; a user `.c`
file in that Virt's `Source/app/` takes precedence.  A normal VM with no user
source gets a generated SoC-agnostic stub that prints `RTE guest <vm>: no
application selected`, never leaves `Init`, and uses no clock or peripheral,
so the project still links and boots.  The BSP entry, config
and app files are re-staged from the Virt being built, so two RTE guests can
share one RTEMS tree and build serially without leaking each other's
descriptors; `rte_copy.sh` removes the previous VM's `prc_*.h` and app folder
before copying and cleans half-written files on failure.  The context frame
offsets and word counts are in `bsps/arm/v7m_rvm/include/rte_frame.h`, shared
by the C context code and the assembly switch; static assertions check the
`Context_Control` offsets so the two copies cannot drift.  Initial contexts are
always basic frames, matching the native ARM rule that no FP state exists at
thread start; `is_fp`/`RTEMS_FLOATING_POINT` is stored in the TCB but ignored
for frame selection.  The first VFP instruction sets FPCA and the next save
creates the extended frame; `rte_two_task_fpu` passes with this scheme.
`RTE_FAST_PATH`
also has a single generated source of truth: the `.ini` value becomes both
`rte_config.h` and the assembly compile definition.

Installation is a full-file overlay:

1. Have a complete official RTEMS 7.0-rc1 tree.
2. Copy this `RTEMS_V701/` tree over it, overwriting same-name files.

The RVM CPU port is the new `cpukit/score/cpu/arm/v7m_rvm/` directory, plus
the `cpuarm_rvm` spec and the small `cpuarm`/`librtemscpu`/`optincludes`
selection changes; the shared `cpukit/score/cpu/arm` sources are unmodified.
The port consists of 4 `cpukit` files: three `v7m_*_rvm` sources and the
`include/rtems/score/cpu.h` overlay.  The CPU-level initialization and ISR
level wrappers were folded into `v7m_rvm.c`; `bsp_start()`/`bsp_reset()` were
folded into `start/rvm_entry.c`, and the dummy `start-file` was removed.
The rest of `cpukit`, the other RTEMS directories and the BSP shared code come
from the official tree.  Do not copy the original `cpukit` into this delta tree
and do not use this delta tree standalone: it is an overlay, not a complete
RTEMS tree.  If a separate build tree is wanted, copy the whole official
RTEMS tree first, then overlay this `RTEMS_V701/` tree on top.  Copyright
headers are preserved; no patch utility is required.

RMP is **not** distributed here: RMP carries its RVM platform layers natively
(`Source/Platform/*_RVM/` in the RMP tree) and needs no guest-side packaging.

The contents of this directory are a **mirror** produced by `update-maintainer-only.sh`; never
edit files here — edit the source trees and re-run `./update-maintainer-only.sh` from this
directory.

### The repositories around this library

This library is one piece of a eleven-repository system. The project generator
(`rvgen`) locates them by relative path, so keep them as siblings under one
root:

```
<root>/
├─ M7M02_Ammonite/     the RVM hypervisor side — THIS library lives in its Guest/ folder
│  ├─ Include/         hypervisor interface headers the ports compile against
│  │                   (rvm.h, Virtlib/, Syslib/, Platform/)
│  ├─ Source/          the monitor, the virtualization library, platform drivers
│  ├─ Project/         project generator (rvgen), benchmark.sh, the .rvp files
│  ├─ Guest/           THIS LIBRARY (a mirror, refreshed by update-maintainer-only.sh)
│  └─ Utility/         generator sources and host tools
├─ M7M01_Eukaron/      the RME microkernel — the capability kernel RVM builds on
├─ M5P01_Prokaron/     the RMP RTOS — ships its own RVM platform layers (no guest port)
├─ FreeRTOS_V900/      the FreeRTOS tree — the port sources live here; Guest mirrors them
├─ uCOSIII_V308/       the uC/OS-III tree (uC-OS3 + uC-CPU + uC-LIB) — same
├─ RTThread_V411/      the RT-Thread V4.1.1 tree — ports in libcpu/*_rvm, tests in test/
├─ DjyOS_V110/         the DjyOS V1.1.0 tree — port in bsp/arch/arm/cortex-m/armv7e-m_rvm,
│                      benchmark in test/
├─ KLite_V521/         the KLite V5.2.1 tree — port in sources/cpu/armv7-m_rvm,
│                      benchmark in test/
├─ uCOSII_V293/        the uC/OS-II V2.93 tree — ports in Ports/*-RVM,
│                      benchmark in Test/
├─ RTEMS_V701/         the RTEMS 7.0-rc1 tree — RTE BSP/spec/sample subset
│                      overlay, not a complete RTEMS tree
└─ Zephyr_V370/        Zephyr 3.7.0 LTS west-workspace overlay — ZPR arch
                       port, the RVM board and the ZPR benchmark app
```

When you overlay this library onto your official FreeRTOS / uC/OS-III /
RT-Thread / DjyOS / KLite / uC/OS-II / RTEMS / Zephyr trees (next section),
those trees take the place of the `*_Vxxx/` entries in the layout above.

---

## 1. Installation

The library mirrors the relative layout of the target trees and contains
**only the files we added or changed** — official kernel files are never
included, so installing never deletes your files; the only official files
overwritten are the RTE `spec/build` selection files, the RTEMS top-level
`wscript`, and the Zephyr arch selection files; the RVM board and SoC
directories are new additions, and every port lands in its intended place.

- Copy `FreeRTOS_V900/` over the root of your official FreeRTOS **V9.0.0**
  tree (merge directories).
- Copy `uCOSIII_V308/` over the root of your official Micrium uC/OS-III
  distribution — the three-component set **uC/OS-3 V3.08.02 + uC/CPU
  V1.32.01 + uC/LIB V1.39.01** (merge directories; the overlay only adds
  port files to uC/OS-3 and uC/CPU — uC/LIB is a build dependency and is
  shipped unchanged, we distribute nothing for it).
- Copy `RTThread_V411/` over the root of your official RT-Thread **V4.1.1**
  tree (merge directories; the overlay adds the three `libcpu/*_rvm` port
  directories and the benchmark tree under `test/`).
- Copy `DjyOS_V110/` over the root of the DjyOS **V1.1.0** distribution
  (merge directories; the overlay adds
  `bsp/arch/arm/cortex-m/armv7e-m_rvm/` and four benchmark files under
  `test/`). This is a commemorative port with a single combination:
  Cortex-M7 + GCC + the H743 board.
- Copy `KLite_V521/` over the root of your KLite **V5.2.1** tree (merge
  directories; the overlay adds `sources/cpu/armv7-m_rvm/` and the
  benchmark tree under `test/`). This is also a commemorative port with
  the same single combination: Cortex-M7 + GCC + the H743 board — pass
  the `-klt` flag to the project generator to use it.
- Copy `uCOSII_V293/` over the root of your official Micrium uC/OS-II
  **V2.93** tree (merge directories; the overlay adds the ARMv6-M,
  ARMv7-M and RV32 `*-RVM` ports and the `Test/` benchmark tree).

### Environment and native build first (RTE and ZPR)

Do not start with the virtualized image.  Set up the guest tree and its
toolchain, then prove that an unmodified native application builds and links
before copying the RVM overlay:

- RTEMS: install `arm-rtems7-gcc` 15.2.0 with the Cortex-M3/M4/M7 newlib and
  C++ multilibs, configure and build the official RTEMS **7.0-rc1** tree, and
  build a native BSP sample such as `arm/stm32f4` `hello.exe` (and
  `iostream.exe` for the C++ path).
- Zephyr: create a Zephyr **3.7.0 LTS** west workspace, install `west`,
  CMake/Ninja and the GNU Arm toolchain (or Zephyr SDK), and build a stock
  board sample such as `nucleo_h743zi` `hello_world` with `west build`
  (`nucleo_f429zi` for the M4F path).

Only after the native build is green should you apply `RTEMS_V701/` or
`Zephyr_V370/` and build the RTE/ZPR virtualized image.

- For RTEMS, prepare a complete official RTEMS **7.0-rc1** tree, then
  copy `RTEMS_V701/` over it, overwriting same-name files.  This directory
  is an overlay, not a standalone RTEMS tree; do not copy the original
  `cpukit` into it.  Pass `-rte` to the project generator; the generated
  makefile finds `arm-rtems7-gcc` in PATH, with `RTEMS_PREFIX` as an
  override.
- For Zephyr, start from a west workspace at **Zephyr 3.7.0 LTS**, then copy
  `Zephyr_V370/` over it, overwriting same-name files (the arch/SoC files
  under `zephyr/` are intentional RVM changes).  Pass `-zpr <west-root>` to the
  project generator; the generated makefile finds `arm-none-eabi-gcc` and
  `west` in PATH, with `PREFIX`/`WEST` make variables available as overrides.  The overlay ships the port,
  the RVM board and the benchmark app; it does not ship a second Zephyr
  tree.

After the overlay, the RVM port directories sit beside the official ones,
exactly like additional official ports:

```
FreeRTOS_V9.0.0/                      <- your official tree root
└─ portable/
   ├─ GCC/
   │  ├─ ARM_CM0/  ARM_CM3/  ARM_CM4F/ ...   <- official, untouched
   │  ├─ ARM_A6M_RVM/                          <- added by the overlay
   │  ├─ ARM_A7M_RVM/                          <- added by the overlay
   │  └─ RISC-V_RV32P_RVM/                     <- added by the overlay
   ├─ RVDS/  ... ARM_A6M_RVM/  ARM_A7M_RVM/    <- same pattern
   └─ IAR/   ... ARM_A6M_RVM/  ARM_A7M_RVM/    <- same pattern
                                       test/   <- merged with the overlay's test/

uCOSIII-V3.08/                        <- your official distribution root
├─ uC-OS3/
│  ├─ Ports/
│  │  ├─ ARM-Cortex-M/
│  │  │  ├─ ARMv6-M/  ARMv7-M/                <- official, untouched
│  │  │  ├─ ARMv6-M-RVM/                       <- added by the overlay
│  │  │  └─ ARMv7-M-RVM/                       <- added by the overlay
│  │  └─ RISC-V/
│  │     ├─ RV32/                              <- official, untouched
│  │     └─ RV32-RVM/                          <- added by the overlay
│  └─ Test/                                    <- merged
└─ uC-CPU/
   ├─ ARM-Cortex-M/
   │  ├─ ARMv6-M/  ARMv7-M/                    <- official, untouched
   │  ├─ ARMv6-M-RVM/                           <- added by the overlay
   │  └─ ARMv7-M-RVM/                           <- added by the overlay
   └─ RISC-V-RVM/                               <- added by the overlay

RT-Thread-V4.1.1/                     <- your official tree root
├─ libcpu/
│  ├─ arm/
│  │  ├─ cortex-m0/  cortex-m3/  ...           <- official, untouched
│  │  ├─ armv6m_rvm/                            <- added by the overlay
│  │  └─ armv7m_rvm/                            <- added by the overlay
│  └─ risc-v/
│     └─ rv32p_rvm/                             <- added by the overlay
└─ test/                                       <- merged

DjyOS-V1.1.0/                         <- your official distribution root
├─ bsp/arch/arm/cortex-m/
│  ├─ armv7e-m/                                <- official, untouched
│  └─ armv7e-m_rvm/                            <- added by the overlay
└─ test/                                       <- merged (four benchmark files only)

uC/OS-II-V2.93/                       <- your official distribution root
├─ Ports/
│  ├─ ARM-Cortex-M/ARMv6-M-RVM/  ARMv7-M-RVM/  <- added by the overlay
│  └─ RISC-V/RV32-RVM/                         <- added by the overlay
└─ Test/                                       <- merged benchmark tree

RTEMS-7.0-rc1/                        <- complete official RTEMS tree first
├─ bsps/arm/v7m_rvm/                           <- RTE BSP, added by the overlay
├─ cpukit/score/cpu/arm/v7m_rvm/               <- new CPU port, added by the overlay
├─ spec/build/bsps/arm/v7m_rvm/                <- BSP/spec options, added
├─ spec/build/cpukit/cpuarm_rvm.yml            <- new CPU port spec, added
└─ testsuites/samples/rte_*/                   <- RTE samples/benchmark, added
```

### 1.1 Configuration macros

FreeRTOS (in `FreeRTOSConfig.h`):

| Macro | Meaning | Notes |
|---|---|---|
| `configUSE_RVM_OPTIMISED_YIELD` | 1 = task-stack fast-path switch; 0 = pure hypercall yield | 0 costs ~4x Yield latency, keeps semantics identical |
| `configUSE_PORT_RVF` / `configUSE_PORT_RVD` | RV32P single/double-precision FPU frame | Exactly one; must match the coprocessor declared in the process header |

uC/OS-III (in the port's `os_cfg.h`, one per compiler directory):

| Macro | Meaning |
|---|---|
| `OS_CFG_RVM_FAST_YIELD_EN` | 1 = task-stack fast-path switch; 0 = pure hypercall yield |

RT-Thread and DjyOS need no switch knob — their switch strategy is fixed
(RT-Thread: fast path for clean frames, deferred for dirty ones; DjyOS: same,
with the event-completion turnarounds never deferred). Two build-level
requirements instead:

| Guest | Requirement |
|---|---|
| RT-Thread | `RT_USING_LIBC` (the kernel's `va_list` fallback only exists on the `__GNUC__`-without-`__ARMCC_VERSION` branch; ARM Compiler and IAR need the libc header) |
| DjyOS | the guest must be compiled with `-mfloat-abi=soft` (the port builds integer-only frames; `cpus.s` turns a float ABI into a build error) |

RTEMS/RTE (in the generated `rte_config.ini`):

| Macro | Meaning | Notes |
|---|---|---|
| `RTE_FAST_PATH` | 1 = clean task frames switch directly on the task stacks; 0 = every switch goes through the RVM vector thread | `False` also removes the assembly fast routine from the image |
| `RTE_CPU_FLAGS` | `-mcpu/-mfpu/-mfloat-abi` for the selected Cortex-M variant | set by `RTE_Gen` from the `.rvp` coprocessor: FPV4_SP -> M4F, FPV5_SP/FPV5_DP -> M7, no coprocessor -> soft-float |

### 1.2 Project generation

All guests are driven by the RVM project generator (`rvgen`, through
`benchmark.sh` in the RVM `Project/` directory). The `.rvp` project file
selects chip, platform and guest type
(`<Guest_Type>FRT|UO3|RMP|RTT|DJY|KLT|UO2|RTE</Guest_Type>`); the generator
copies the port files into the generated workspace and emits the build system
(Makefile / IAR / RVDS) with correct include order. Building by hand is
possible but not covered here.  RTE is the exception on the build side: its
Virt makefile runs `rte_copy.sh` and then drives the RTEMS waf build, rather
than compiling a copied port tree directly.

`generate.sh` runs the same generator **without** the benchmark option: the
guest is then booted by the plain application in `<guest>_main.c`, which the
generator copies into the workspace as `main.c` (with the idle-parking hook
described in §2.10). Both scripts must be given the same source roots.

### 1.3 Generated layout ownership

A VM's memory layout is generator-owned; only the test wiring is per-chip.

- **ZPR**: one board target, `armv7m`.  For every Virt the generator writes a
  complete `zpr_board.dts` (CPU compatible, `flash0`/`sram0` windows,
  `chosen`) and the generated makefile passes it as `DTS_SOURCE`;
  `prj.conf` carries only the CPU choice, stacks, events and heap.  There are
  no M7/M4 board variants.
- **RTE**: `rte_config.ini` carries every code/data/stack value, and
  `linkcmds` substitutes them into its `MEMORY`/`__RVM_Stack`; spec defaults
  are zero sentinels and `linkcmds` asserts they were generated.  CPU/FPU and
  float ABI come from `RTE_CPU_FLAGS`.
- **DjyOS**: the port's `template/*.h` files (board, project and component
  configuration) are copied into `<Virt>/Include/`; an existing copy wins, so
  users can edit the clock/tick/stack/component limits per VM.  The context
  PC check uses the generated `CODE_CODE1_BASE`/`CODE_CODE1_SIZE`, and
  `board.c` asserts the kernel stacks fit `DATA_DATA1_SIZE`.

Per-chip files under each guest's `test/chip` are benchmark wiring only
(TIM/RCC/IRQ/ARR); they are not part of a normal VM image or of the VM memory
map.

---

## 2. Port contracts and implementation strategies

Every port in this library implements the same five-part job: critical
sections, yield, context switch, interrupt delivery hooks, and startup. What
differs per OS is **what the kernel itself guarantees** — the *contract* — and
therefore how much the port must do on its own — the *strategy*.

### 2.1 The kernel-side contracts (intrinsic, platform-independent)

| | FreeRTOS | uC/OS-III | RMP | RT-Thread | DjyOS |
|---|---|---|---|---|---|
| Yield request state | **anything** — the kernel yields from inside nested critical sections, documented as legal in `tasks.c` | **always masked** — `OSSched()` wraps selection + request in its own flat critical section (`Disable; Yield; Enable`) | normalized — locked-state requests become a Pend flag the kernel replays at the unlock point | **anything** — `rt_schedule()` is called with the scheduler lock held inside a masked critical section | **always masked** — selection and switch both run inside the kernel's async-signal mask |
| Switch timing requirement | none (immediate or deferred, both correct per the kernel's own comment) | none (the `Enable` is idempotent, so switching before or after it are both sound) | the question never reaches the port (the kernel has already deferred everything) | none from the kernel's view; the official ports still always defer to PendSV | the switch must complete **before** the unmask (except a dirty frame, which needs a real exception return) |
| Interrupt primitives required | unconditioned pair + nesting-count critical pair | unconditioned pair + snapshot save/restore pair | a single mask primitive | a single snapshot disable/enable pair | a single mask/unmask pair with snapshot semantics |
| Port obligation | make a yield safe **in any state** | finish the switch with the incoming task running **unmasked** | handle one form of request: an unmasked, immediate switch | consume the pair the kernel already chose (`rt_current_thread` is pre-updated) and keep a re-request from tearing the frame | carry the event-turnaround frame and never defer a never-returning turn |

### 2.2 The common RVM switch template

All RVM ports converge on the same skeleton (RMP is the original; the
FreeRTOS, uC/OS-III, RT-Thread and DjyOS ports are transplants of it):

```
entry: RVM_Virt_Int_Mask()                    -- always, unconditionally
        peek / consume the kernel's choice    -- plus a dirty-frame check
        |- no switch needed  -> unmask, return
        |- clean frame       -> asm fast path: switch on the task stacks,
        |                       scheduler bookkeeping under mask, unmask at exit
        +- dirty ICI/IT frame-> slow path: unmask, hypercall (a real exception
                                return resumes the target)
```

### 2.3 FreeRTOS: contract, strategy, and the subtle deviations

**Contract.** No state guarantee, no timing requirement.

**Strategy.** A software deferred-yield that mimics the official PendSV pend:
`vPortYield()` masks first, then reads the kernel's critical-section nesting
count — if nonzero it only sets `xPortYieldPend` and returns;
`vPortExitCritical()` replays the switch when the count reaches zero (switch
first, unmask after). With the count at zero the fast path switches
immediately, under the entry mask.

**Deviations and why they are sound:**

1. *The official reference implements the deferral with a hardware PendSV
   pend — state-free and mask-free. We implement it with a software flag,
   which forces an entry mask and a counter read.* Sound because the
   software pend reproduces exactly the PendSV timing semantics (the request
   is recorded, execution happens after the critical section exits); the
   entry mask buys back the state-independence that the hardware flag gets
   for free.
2. *The entry mask is redundant on call sites that are provably unmasked
   (e.g. queue send after `xTaskResumeAll`).* Sound because masking is
   idempotent and costs a handful of cycles — the price of keeping the
   official "any-state callable" contract intact for every present and
   future call site.


### 2.4 uC/OS-III: contract, strategy, and the subtle deviations

**Contract.** `OSSched()` is a `Disable; Yield; Enable` triple. The yield
request is always issued masked; either switch timing is correct.

**Strategy.** Switch immediately at the call site. The fast path masquerades
the caller's context as an exception frame, performs the official PendSV
bookkeeping under mask, loads the new stack, and unmasks at exit — so the
incoming task starts **unmasked**.

**Deviations and why they are sound:**

1. *The direct-switch reference ports (MicroBlaze et al.) save and restore
   the interrupt state in each task's frame; we do not restore anything —
   the incoming task simply starts unmasked.* Sound because of the kernel's
   own `D;Y;E` shape: the `Enable` is unconditional and idempotent, so
   starting unmasked is bit-for-bit equivalent to what the official CM port
   does (it switches *after* the `Enable`, which also leaves the incoming
   task unmasked). The stretch between the switch point and the `Enable`
   touches no protected data — the switch point is the semantic boundary of
   the critical section.
2. *`OSCtxSw()` masks at entry although the only legal caller (`OSSched`)
   has already masked.* Sound as the "self-sufficient entry" principle: the
   official pend-based `OSCtxSw` is callable in any state; the entry mask
   preserves that contract for the direct-switch implementation.
3. *The dirty-frame slow path pends the switch while masked and lets the
   unmask arm it: `RVM_Virt_Yield()` under mask only sets the context-switch
   flag and the pending flag (it neither signals nor switches — the yield
   hypercall checks `RVM_Int_Mask` on the send side, and the switch endpoint
   is gated again on the execute side), and the following unmask replays it
   at its hypercall boundary, where the switch and the incoming task's
   unmasked start happen at once. No unmasked window exists in between.
   The preemption self-healing below is the belt-and-suspenders argument:
   every path that resumes the suspended task re-selects
   `OSTCBHighRdyPtr` first, so a replayed yield degenerates into a no-op.*

### 2.5 RMP (for reference — not distributed here)

**Contract.** Lock and mask are the same primitive; locked-state requests
become a kernel Pend flag replayed at unlock; the port only ever sees
unmasked, immediate switch requests.

**Strategy.** The C-level yield entry masks (the `masked externally` comment
in the assembly refers to it), peeks, and either switches directly on the
task stacks (with `_RMP_Run_High` doing the scheduling inside the switch) or
takes the hypercall slow path.

**Deviation and why it is sound:** the assembly trusts "external masking"
the way the MicroBlaze-style contract does — but instead of a comment-bound
promise to callers, the mask is provided by the entry of the very same C
function. The two-layer structure compiles the contract into code rather
than documentation.

### 2.6 RT-Thread: contract, strategy, and the subtle deviations

**Contract.** `rt_schedule()` runs inside a masked critical section with the
scheduler lock held, selects the next thread, **pre-updates
`rt_current_thread`**, and then calls the port. The switch request points are
closed (one scheduler-level function, one interrupt-level function).

**Strategy.** Latch the from/to pair and a request flag; for a clean frame
switch directly on the thread stacks, for a dirty ICI/IT frame (and for the
interrupt-level request) let the context-switch handler consume the latch at
the hypercall boundary. The handler carries the same two guards the official
PendSV does (flag==0 skip, from==0 skip saving) — they are what keeps a
latch that outlives its request from replaying a stale switch.

**Deviations and why they are sound:** the official ports always defer
through PendSV; ours switches immediately for clean frames. Sound because the
entry masks first, the frame it builds is a real exception frame, and every
path that resumes a half-switched context re-selects the target through
`rt_schedule` — a replayed request degenerates into a no-op. The pre-updated
`rt_current_thread` is what makes the frame exchange safe when the fast path
is preempted halfway: the from slot a delivery latches is the thread whose
context is half-restored, i.e. the target's own slot.

### 2.7 DjyOS: contract, strategy, and the subtle deviations

**Contract.** DjyOS schedules *events*; threads (`ThreadVm`) are reusable
containers. Every switch happens with the kernel's async-signal mask raised,
and the kernel selects the next event inside the switch itself
(`__DJY_SelectEventToRun` is called by `__DJY_Schedule`), so the port does
not peek.

**Strategy.** One RVM user thread hosts every DjyOS thread, so the port
switches by masquerading the current context as a 22-word exception frame on
the thread stack: hypercall parameters, R4-R11, the saved EXC_RETURN and a
basic hardware frame. Dirty frames (EPSR ICI/IT) take the deferred path so
that the kernel's own exception return restores what manual unstacking
cannot. Event completion uses a *turn* (enter the next context without saving
the exhausted one); turns are never deferred.

**Deviations and why they are sound:** the frame PC is stored with the Thumb
bit set, because the frame is also consumed by the kernel's exception return;
the guest is compiled soft-float, because the port builds integer-only
frames.

**Generated layout and editable config.** The context PC check uses the
generated `CODE_CODE1_BASE`/`CODE_CODE1_SIZE`, and `board.c` statically
asserts that the kernel stacks fit `DATA_DATA1_SIZE`.  The port's
`template/*.h` configuration headers are copied into `<Virt>/Include/`; an
existing copy is never overwritten, so the clock, tick, stack and component
limits are per-VM editable.  `template/` is only the seed and is not on the
compile include path.

**Benchmark wiring.** The benchmark's chip-specific code lives in
`test/chip/djy_test_stm32h743zi_rvm.h`: `djy_bench_timer_init` (TIM2),
`djy_bench_int_init(handler)` (DjyOS interrupt-line registration,
`djy_attach_phys`, TIM4) and `DJY_BENCH_INT_CLEAR`.  `test/djy_board.c` is
only the benchmark VM entry/boot sequence and `test/djy_benchmark.c` only
defines `djy_bench_isr` and the measurement flow, following the RMP
test-header split.


### 2.8 KLite: contract, strategy, and the subtle deviations

**Contract.** KLite is always-deferred: the kernel calls
`cpu_contex_switch()` inside a critical section, and the official M-profile
ports only pend a PendSV there - the switch happens when the outermost
`cpu_leave_critical()` re-enables interrupts. The recursive void/void
critical section API is carried by a port-internal nesting counter, and the
scheduler pre-picks the target into the `sched_tcb_next` global before the
port is called.

**Strategy.** The RMP shape: `cpu_contex_switch()` only sets a port pend
flag (plus one `RVM_Virt_Yield()` when already on the vector thread, the
RMP_YIELD_ISR arm), and the outermost `cpu_leave_critical()` consumes it
through a yield dispatcher - self-switch consumes the flag, a dirty target
frame pends the slow path, a clean one takes the masquerade fast path in
`cpu_asm_gcc.s` (the RMP gold standard translated to KLite's two
scheduler globals, with a first-switch branch that abandons the startup
stack). Every context freeze lands at a nesting of zero, which is what
keeps the shared void/void counter consistent without per-thread state.

**Deviations and why they are sound:** the kernel is used strictly through
its official port contract - `kernel.h`, the two scheduler globals, and the
fact that `stack` is the first member of `struct tcb` (the blind
offset-0 dereference the official PendSV itself relies on); the dirty-frame
peek reads the target's saved XPSR through that same contract, so no kernel
internals are ever included or modified. KLite kernel calls that lock or
schedule (a `queue_create` alone takes a mutex) require a non-NULL
`sched_tcb_now`, so the benchmark and the plain application create their
kernel objects from a thread, never before `kernel_start()` - exactly the
official examples' pattern.

### 2.9 uC/OS-II: contract, strategy, and the subtle deviations

**Contract.** uC/OS-II V2.93 is self-contained (no uC-CPU, no uC-LIB) and
always-deferred: the kernel calls `OSCtxSw()` through `OS_TASK_SW()` inside a
scheduler critical section, and the official M-profile ports pend a PendSV
there. One task per priority (no round-robin); the scheduler pre-picks into
`OSTCBHighRdy` before the port runs.

**Strategy.** The uC/OS-III shape, translated: `OS_CPU_SR_Save/Restore` are
C functions in the port mapping onto the RVM virtual interrupt mask snapshot;
`OSCtxSw()` is a C filter that routes clean ICI/IT frames onto the assembly
fast path in `os_cpu_a.s` and dirty ones onto `RVM_Virt_Yield()`;
`OSIntCtxSw()` is the yield hypercall; `OS_Ctx_Handler()` and
`OS_Tick_Handler()` are registered through `RVM_Virt_Ctx_Reg()` and
`RVM_Virt_Tim_Reg()`.

**Deviations and why they are sound:** the kernel is used strictly through
its official port contract - the two scheduler globals, the `OSTCBStkPtr`
first field, and the hook set; no kernel internals are included or modified.
The benchmark does not measure yield (one task per priority makes it
meaningless); the README's yield column is left empty. Kernel objects must be
created from a task context (a `OSMemCreate` alone touches the TCB), so the
benchmark and plain application boot from an init task.

### 2.10 Idle parking (every guest)

The RVM user manual (§4.4.3) requires a guest's idle thread to call
`RVM_Hyp_Vct_Wait()` (hypercall 9) repeatedly: it suspends the user thread
and removes it from the run queue until the next virtual interrupt is
delivered, which is what stops an idle guest from spinning the core away.
The call is a *transparent* suspend — it resumes at the call site, changes no
kernel state, and never blocks in the RTOS sense.

The hook lives in the application, not in the port: the file the generator
copies into the workspace as `main.c`. Each guest uses its own official
mechanism — `vApplicationIdleHook` (`configUSE_IDLE_HOOK 1`) for FreeRTOS,
`OS_AppIdleTaskHookPtr` for uC/OS-III, `rt_thread_idle_sethook`
(`RT_USING_HOOK`/`RT_USING_IDLE_HOOK`) for RT-Thread, and
`klt_set_idle_hook()` for KLite, whose `cpu_sys_sleep()` runs it inside a
mask bracket (the kernel calls it with the mask held); and
`djy_set_idle_hook()` for DjyOS, whose `CleanWakeupEvent` invokes it with the
virtual interrupt mask open.  RTEMS supplies the idle task body
(`CONFIGURE_IDLE_TASK_BODY`) in the application; the RTE benchmark uses an
idle body that calls `RVM_Hyp_Vct_Wait()`.  Benchmark builds deliberately
keep the hook inert (FreeRTOS defines an empty one because the config is
shared) so idle never enters the measured paths.


### 2.11 RTEMS (RTE): contract, strategy, and the subtle deviations

**Contract.** RTEMS runs as an unprivileged RVM user thread and cannot
program the NVIC, SCB, VTOR or SysTick, nor enter PendSV.  The dispatcher
runs on the RVM vector thread for interrupt-driven scheduling, while a
task-level context switch happens in the user thread.  The port must
therefore either switch clean frames directly on the task stacks or hand a
dirty/ISR-captured frame to the RVM context handler.

**Strategy.** The task frame is the same masqueraded exception frame used by
the other A7M ports: five hypercall parameter words, R4-R11 plus the saved
`EXC_RETURN`, S16-S31 for an extended frame, and the hardware frame
R0-R3/R12/LR/PC/xPSR with S0-S15/FPSCR above it.  Basic frames are 22 words
and hard-float extended frames 56 words.  Clean task-to-task switches take
the fast path in `v7m_context_switch_rvm.S`; dirty frames, ISR-captured
frames and any failed validity check store `rte_pending_heir` and call
`RVM_Virt_Yield()`, and the Vct context handler loads the heir into
`RVM_REG`.  `register_lr` is the frame-kind marker (1 = dispatcher/initial
frame, 0 = ISR-captured frame).  `_CPU_Initialize()` is deliberately empty:
the common ARM implementation would program SVC/PendSV priorities through
the NVIC, which the guest must not touch.  ISR entry/leave updates the
per-CPU nesting counts and dispatches on the vector thread; the physical
switch remains deferred to the context handler.

**Deviations and why they are sound.** The fast path bypasses the monitor
only when the current XPSR has no EPSR ICI/IT continuation, the heir frame
is executable and clean, and no switch is pending; dirty frames always take
the real exception return through the Vct.  Initial and dispatcher frames
are clean by construction.  The extended frame is restored with the
`RTE_EXC_POP` in-place transformation, including the XPSR bit 9 alignment
pad; treating a basic frame as extended (or vice versa) would restore the
wrong PC/SP.  `RTE_FAST_PATH=0` compiles out both the C branch and the
assembly fast routine, so an all-Vct build has no fast-path push at all.
`RTE_CPU_FLAGS` selects Cortex-M4F/FPv4-SP or Cortex-M7/FPv5 from the `.rvp`
coprocessor; the RTEMS build uses the BSP `-O2` flags.  RTE guest
applications and the benchmark are C; `librtemscxx` is buildable but no C++
guest application is exercised.

### 2.12 Zephyr (ZPR): contract, strategy, and the subtle deviations

**Contract.** Zephyr 3.7.0 LTS runs as an unprivileged RVM user thread.
The `CONFIG_RVM_GUEST` arch port lives in
`arch/arm/core/cortex_m/v7m_rvm/` and replaces the normal Cortex-M entry,
SysTick, NVIC, SVC, PendSV, MPU and FPU paths; the upstream Cortex-M source
files remain untouched, and the build selects the new port.  The scheduler still uses Zephyr's own `_kernel` ready
queue, but the physical switch is either the RVM fast path or the Vct
context handler.

**Strategy.** The generator-owned `prc_<vm>.c` supplies `Thd_Vct` and the
generator-owned `prc_<vm>_desc.c` supplies `RVM_Desc`.  The descriptor policy
is a single virtual method on `Guest_Gen`: ZPR reports that its user entry is
`__zpr_entry`; the common generator emits the entry and derives its
`RVM_EXTERN void <entry>(void);` declaration.  The descriptor is a plain
`const RVM_Desc[]` in `.rodata`; `zpr_desc.ld` collects the generated
`*prc_*_desc*.o*` object's `.rodata` at the image base, so no section
attribute or post-generation text patch is needed.  The port's
`zpr_entry.c` supplies only `__zpr_entry` (Zephyr `z_prep_c`); RVM
interrupt gates stay masked through `z_prep_c`/`z_cstart`, and
`zpr_switch_to_main()` enables them only at the final switch to main.  `zpr_context.c` maps Zephyr's
`arch_swap` state onto the same 22-word masqueraded frame used by RTE; the
frame's SP sits at offset 36 so the shared RVM slow-path assembly can be
reused unchanged.  `zpr_timer.c` registers `sys_clock_announce()` through
`RVM_Virt_Tim_Reg`, `zpr_printk.c` sends the console through
`RVM_Hyp_Putchar`, and the benchmark chip header (`zpr_test_<chip>.h`) can
relay a physical TIM4 to virtual VCT 2 when the benchmark app calls its
`ZPR_Test_*` setup.  Idle calls `RVM_Hyp_Vct_Wait()` and re-enters the
scheduler with `z_reschedule_irqlock()`; calling `k_yield()` from idle is
wrong because idle is deliberately not in the ready queue.  The
context-handler registration is a `SYS_INIT(PRE_KERNEL_1)` so it runs after
`z_bss_zero()`.  The timer ISR calls `sys_clock_announce()` and then
`zpr_reschedule_from_vector()`, which captures the interrupted frame and
yields when a higher-priority thread became ready; `k_thread_abort()` uses an
RVM-specific implementation that never writes SCB->ICSR or reads IPSR.  The
context layer supports both the 22-word basic frame and the 56-word FPU
extended frame (`S16..S31`, hardware frame, `S0..S15/FPSCR`).  Initial frames
are always basic, including `K_FP_REGS` threads; the first VFP instruction sets
`CONTROL.FPCA`, and the next save creates the extended frame.  The Vct
capture/restore handles the monitor's `EXC_RETURN=0xFFFFFFED`
frame.  Two FP threads and timer-driven FP preemption pass on H743 with both
the fast path and all-Vct builds, and on F429 with the fast path; the all-Vct
frame path is shared.

**Generator and memory.** `ZPR_Gen` writes only the Virt workspace (the
Zephyr app, `zpr_config.h`, the west-driving `prc_<vm>` makefile) and never
edits the Zephyr tree.  The app's generated sources/includes/linker snippet
live in `zpr_app.cmake`; a missing `CMakeLists.txt` is a thin wrapper around
it, a hand-written one is preserved and gets the include appended once, and a
generated wrapper is refreshed.  `prj.conf` follows the same rule: a
hand-written file keeps its options and stale generator-owned link values are
removed, while missing ZPR defaults are appended.  One virtual board,
`armv7m`, carries the Zephyr image at exactly the RTE code window and the
data/bss/stack in the RTE data window.  The generator writes a per-VM
`zpr_board.dts` with the CPU compatible, flash/SRAM windows and `chosen`, and
passes it as `DTS_SOURCE`; the CPU is selected with
`CONFIG_RVM_CPU_M7`/`CONFIG_RVM_CPU_M4`.  There is no STM32 dtsi, RCC/PLL or
peripheral node.  Chip
peripherals (RCC, TIM4, ...) come from `zephyr/arch/arm/core/cortex_m/v7m_rvm/test/chip/zpr_test_*.h`
through the generated `zpr_test.h`; `zpr_config.h` carries only code/data
size and the fast-path switch.  The benchmark's TIM2/TIM4 setup lives in
that chip header (`ZPR_Test_Timer_Init`/`ZPR_Test_Int_Init`/
`ZPR_Test_Int_Disable`) and its ISR handler in `test/bench.c`, following the
RMP chip-header pattern.  `zpr_test.h` and `test/bench.c` are benchmark-app
files only; a normal guest never compiles or includes them and never touches
TIM4.  A normal app is SoC-agnostic: the virtual SoC has no STM32 HAL or SoC
init, and the only clock value is `CONFIG_SYS_CLOCK_HW_CYCLES_PER_SEC`.  A
user app that needs a device adds it through `app.overlay`, subject to the
RVM device/IRQ grant.  The six ISR benchmark rows are labels over one TIM4
virtual vector hook, not six different interrupt mechanisms; the benchmark
prints that caveat.  With more than one Virt the board
DTS window is not enough: the generator writes each VM's allocated code/data
window into that Virt's `zpr_board.dts` (CPU compatible, `flash0`/`sram0` and
`chosen`) and passes it as `DTS_SOURCE`, so every guest links at its own
physical window.  The generated M7 window is 114 KiB and the M4 window 66 KiB;
the generator rounds the VM window down
to whole KiB, leaving up to 848 bytes unadvertised.  Two ZPR guests have been run
simultaneously on the H743; two benchmark guests would contend for TIM4 and
are not a supported configuration.  A user `.c` file in that Virt's
`Source/app/` takes precedence over the benchmark in both build modes; the
benchmark is selected only when the folder has no `.c` and `ZPR_NORMAL` is
unset.  The `v7m_rvm` Kconfig port depends on `!SMP &&
!USERSPACE && !ARM_MPU && !CORTEX_M_SYSTICK && !TICKLESS_KERNEL &&
!GEN_ISR_TABLES`, and C-side build assertions pin the basic/extended-frame
ABI.  The validated ZPR benchmark uses `CONFIG_SYS_CLOCK_TICKS_PER_SEC=10`,
minimal libc, no userspace and no MPU; FPU support is enabled separately
(`CONFIG_FPU=y`, `CONFIG_FPU_SHARING=y`, hard-float ABI) and is exercised by
the two-thread and timer-preemption FP tests, not by the integer-only
benchmark.

**Normal builds and the native heartbeat.** The RVM time wheel is advanced by
a physical timer vector.  `generate.sh` therefore adds `-b` for ZPR projects
and uses the native benchmark process as the TIM4 heartbeat; the image boots,
runs the native benchmark first, then enters the user Zephyr application.  A
normal guest never compiles `test/bench.c` or includes `zpr_test.h`, so it
does not register or rewrite TIM4.  The native benchmark output is not yet
silenced.  `benchmark.sh` uses the same heartbeat and runs the 15-item ZPR
benchmark.

---

## 3. The RVM interrupt model (what every port relies on)

- **Three-layer gate.** `RVM_Int_Mask` is checked where interrupts are sent
  (`RVM_Virt_Vct_Trig`), where they are fetched (`RVM_Vct_Get`), and where
  they are executed (tick/ctx in `RVM_Vct_Loop`). Maskable vectors pending
  while masked are **held, not lost** — the pending flag survives until
  unmask replays them. Transparent (NMI-class) vectors pass through by
  design.
- **Preemption direction.** The vector thread runs at a higher priority
  than the guest's user thread. Whenever the mask is clear, an interrupt
  can preempt at any instruction boundary; whenever it is set, maskable
  interrupts do not execute at all.
- **Physical vs. virtual domain.** `CPSID`/`BASEPRI` only mask physical
  interrupts and have **no effect** on virtual delivery. All masking in
  these ports goes through `RVM_Virt_Int_Mask()`/`RVM_Virt_Int_Unmask()`;
  mapping a kernel interrupt primitive to the physical domain silently
  disables every gate above.
- **Frame interchangeability.** A task frame is produced identically by the
  fast path (masqueraded exception frame), by the interrupt path (the
  context handler converting the register block) and by task creation.
  Any producer's frame is consumable by any consumer.

## 4. Known limitations

- The dirty-frame slow path (targets whose saved EPSR carries ICI/IT
  continuation state) is nearly dead code under typical workloads;
  however, it does have an impact on worst-case execution time.
- Transparent vectors bypass the mask by design; no port in this library
  uses them, and their interaction with kernel critical sections is
  untested.
- RTE is validated as a C guest (C applications and the C benchmark);
  `librtemscxx` can be built, but no C++ RTE application is exercised.
- RTE's ISR-relay benchmark samples after the task resumes, while the
  other guests sample at ISR entry; the relay numbers are not directly
  comparable.
- ZPR's ISR rows sample in the RVM virtual-vector hook and expose the result
  to a polling thread; they are not yet a Zephyr ISR-context release.

