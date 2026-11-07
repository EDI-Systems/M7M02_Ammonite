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

RTE is distributed as the `RTEMS_V701` BSP/spec/sample subset.  Its generated
build-time staging files (`prc_virt1.c`/`_desc.c`, `rte_prc.h`,
`rte_config.h` and `app/`) are produced by RTE_Gen and `rte_copy.sh`, so they
are not part of the mirror.  The RTE BSP references the M7M02 RVM `Include/`
and `Source/` trees through `RVM_ROOT`; all RVM headers, including the generated
platform adapters, are staged by `rte_copy.sh`, so no RVM implementation is
kept in the BSP source tree.  RTE defaults to the direct task-stack context
switch fast path; setting `RTE_FAST_PATH` to `False` in the generated
`rte_config.ini` routes every switch through the RVM vector thread and omits
the assembly fast routine from the image.  Cortex-M7/STM32H743 and
Cortex-M4F/STM32F429 (the connected board reports NUCLEO-F439ZI, same 0x419
family) are both verified; `RTE_CPU_FLAGS` selects the CPU/FPU options from
the `.rvp` coprocessor.  The RTEMS build uses the BSP `-O2` optimization
flags; performance archives live in
`testsuites/samples/rte_benchmark/chip/rte_test_<chip>.h`.

Installation is a full-file overlay:

1. Have a complete official RTEMS 7.0-rc1 tree.
2. Copy this `RTEMS_V701/` tree over it, overwriting same-name files.

The RVM CPU port is the new `cpukit/score/cpu/arm/v7m_rvm/` directory, plus
the `cpuarm_rvm` spec and the small `cpuarm`/`librtemscpu`/`optincludes`
selection changes; the shared `cpukit/score/cpu/arm` sources are unmodified.
The port consists of 8 `cpukit` files: seven `v7m_*_rvm` sources and the
`include/rtems/score/cpu.h` overlay.
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

This library is one piece of a ten-repository system. The project generator
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
└─ RTEMS_V701/         the RTEMS 7.0-rc1 tree — RTE BSP/spec/sample subset
                       overlay, not a complete RTEMS tree
```

When you overlay this library onto your official FreeRTOS / uC/OS-III /
RT-Thread / DjyOS / KLite / uC/OS-II / RTEMS trees (next section), those
trees take the place of the `*_Vxxx/` entries in the layout above.

---

## 1. Installation

The library mirrors the relative layout of the target trees and contains
**only the files we added or changed** — official kernel files are never
included, so installing never deletes your files; the only official files
overwritten are the RTE `spec/build` selection files and the top-level
`wscript`, and every port lands in its intended place.

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
- For RTEMS, first prepare a complete official RTEMS **7.0-rc1** tree, then
  copy `RTEMS_V701/` over it, overwriting same-name files.  This directory
  is an overlay, not a standalone RTEMS tree; do not copy the original
  `cpukit` into it.  Pass `-rte` to the project generator.

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
| `RTE_CPU_FLAGS` | `-mcpu/-mfpu` for the selected Cortex-M variant | set by `RTE_Gen` from the `.rvp` coprocessor: FPV4_SP -> M4F, FPV5_SP/FPV5_DP -> M7 |

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

