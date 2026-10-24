# The RVM Guest Library

This directory distributes the **paravirtualized RTOS ports** for the RVM
hypervisor: drop-in port layers that let unmodified RTOS kernels run as
unprivileged RVM guests.

Currently distributed:

| Guest | Versions | Architectures | Toolchains |
|---|---|---|---|
| FreeRTOS | V9.0.0 | ARMv6-M, ARMv7-M, RV32P | GCC, RVDS (ARMCC), IAR |
| uC/OS-III | V3.08 (+ uC/CPU V1.32) | ARMv6-M, ARMv7-M, RV32 | GCC, ARM, IAR |

RMP is **not** distributed here: RMP carries its RVM platform layers natively
(`Source/Platform/*_RVM/` in the RMP tree) and needs no guest-side packaging.

The contents of this directory are a **mirror** produced by `update.sh`; never
edit files here — edit the source trees and re-run `./update.sh` from this
directory.

### The repositories around this library

This library is one piece of a five-repository system. The project generator
(`rvgen`) locates them by relative path, so keep them as siblings under one
root:

```
<root>/
├─ M7M02_Ammonite/     the RVM hypervisor side — THIS library lives in its Guest/ folder
│  ├─ Include/         hypervisor interface headers the ports compile against
│  │                   (rvm.h, Virtlib/, Syslib/, Platform/)
│  ├─ Source/          the monitor, the virtualization library, platform drivers
│  ├─ Project/         project generator (rvgen), benchmark.sh, the .rvp files
│  ├─ Guest/           THIS LIBRARY (a mirror, refreshed by update.sh)
│  └─ Utility/         generator sources and host tools
├─ M7M01_Eukaron/      the RME microkernel — the capability kernel RVM builds on
├─ M5P01_Prokaron/     the RMP RTOS — ships its own RVM platform layers (no guest port)
├─ FreeRTOS_V900/      the FreeRTOS tree — the port sources live here; Guest mirrors them
└─ uCOSIII_V308/       the uC/OS-III tree (uC-OS3 + uC-CPU + uC-LIB) — same
```

When you overlay this library onto your official FreeRTOS / uC-OS-III trees
(next section), those trees take the place of `FreeRTOS_V900/` and
`uCOSIII_V308/` in the layout above.

---

## 1. Installation

The library mirrors the relative layout of the target trees and contains
**only the files we added** 鈥?official kernel files are never included, so
a plain overlay is safe: it cannot overwrite any of your official files,
and every port lands in its intended place.

- Copy `FreeRTOS_V900/` over the root of your official FreeRTOS V9.0.0 tree
  (merge directories).
- Copy `uCOSIII_V308/` over the root of your official uC/OS-III V3.08
  distribution, which contains both uC/OS-3 and uC/CPU (merge directories).

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

### 1.2 Project generation

Both guests are driven by the RVM project generator (`rvgen`, through
`benchmark.sh` in the RVM `Project/` directory). The `.rvp` project file
selects chip, platform and guest type (`<Guest_Type>FRT|UO3|RMP</Guest_Type>`);
the generator copies the port files into the generated workspace and emits the
build system (Makefile / IAR / RVDS) with correct include order. Building by
hand is possible but not covered here.


---

## 2. Port contracts and implementation strategies

Every port in this library implements the same five-part job: critical
sections, yield, context switch, interrupt delivery hooks, and startup. What
differs per OS is **what the kernel itself guarantees** — the *contract* — and
therefore how much the port must do on its own — the *strategy*.

### 2.1 The kernel-side contracts (intrinsic, platform-independent)

| | FreeRTOS | uC/OS-III | RMP |
|---|---|---|---|
| Yield request state | **anything** — the kernel yields from inside nested critical sections, documented as legal in `tasks.c` | **always masked** — `OSSched()` wraps selection + request in its own flat critical section (`Disable; Yield; Enable`) | normalized — locked-state requests become a Pend flag the kernel replays at the unlock point |
| Switch timing requirement | none (immediate or deferred, both correct per the kernel's own comment) | none (the `Enable` is idempotent, so switching before or after it are both sound) | the question never reaches the port (the kernel has already deferred everything) |
| Interrupt primitives required | unconditioned pair + nesting-count critical pair | unconditioned pair + snapshot save/restore pair | a single mask primitive |
| Port obligation | make a yield safe **in any state** | finish the switch with the incoming task running **unmasked** | handle one form of request: an unmasked, immediate switch |

### 2.2 The common RVM switch template

All three RVM ports converge on the same skeleton (RMP is the original; the
FreeRTOS and uC/OS-III ports are transplants of it):

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
  continuation state) is nearly dead code under typical workloads; its
  preemption-window soundness is argued, not exercised by the benchmarks.
- Transparent vectors bypass the mask by design; no port in this library
  uses them, and their interaction with kernel critical sections is
  untested.

