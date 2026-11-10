# ZPR — Zephyr 3.7.0 LTS on RVM

This directory is an **overlay**, not a second Zephyr tree.  Apply it on top of
a Zephyr 3.7.0 west workspace.  It contains:

- `port/` — the RVM guest port: `__zpr_entry`, context switch glue,
  virtual interrupt mask, virtual timer and console.  The VM descriptor and
  `Thd_Vct` come from the generator's `prc_<vm>.c` / `prc_<vm>_desc.c`.
- This directory is the RVM arch port itself.  The upstream Cortex-M
  `prep_c.c`/`thread.c`/`swap.c`/`cpu_idle.c` and headers stay original; the
  build selects this port under `CONFIG_RVM_GUEST`.
- `zephyr/arch/...` — the `CONFIG_RVM_GUEST` hooks in the Cortex-M arch.
  These files retain their upstream Apache-2.0 license.  The virtual ARMv7-M
  SoC has no vendor initialization or HAL.
- `zephyr/boards/rvm/armv7m/` and `zephyr/soc/arm/v7m_rvm/` — one virtual
  RVM board and SoC family.  The board target is always `armv7m`; the
  generator writes the per-VM `zpr_board.dts` (CPU compatible, flash/SRAM
  windows and `chosen`) and passes it as `DTS_SOURCE`, so a VM memory change
  never leaves a stale board DTS.  The CPU is selected with
  `CONFIG_RVM_CPU_M7`/`CONFIG_RVM_CPU_M4`; there is no STM32 dtsi, RCC/PLL or
  peripheral node.  Applications add the devices granted to them through
  `app.overlay`.
- `test/` — the 15-item benchmark (9 thread-level + 6 ISR), its
  `chip/zpr_test_*.h` peripheral headers (which, like the RMP test headers,
  also define `ZPR_Test_Timer_Init`/`ZPR_Test_Int_Init`/
  `ZPR_Test_Int_Disable`), and the per-chip result tables kept in those
  headers.  `bench.c` defines the ISR handler and the measurement flow.  Only
  the benchmark app compiles these files; a normal app never sees `test/` or
  `zpr_test.h`.

Build through the project generator:

```
rvgen -p gccmf-stm32h743zit6-zpr.rvp \
      -zpr ../../Zephyr_V370/ ...
```

The generated `prc_<vm>` drives west, converts `zephyr.bin` with bincopy and
feeds the RVM Kernel; the generator never edits the Zephyr tree.  The generated
`RVM_Desc` is an ordinary `const` array in `.rodata`; `zpr_desc.ld` matches the
generated `*prc_*_desc*.o*` object and keeps its `.rodata` at the image base, so
the descriptor does not need a custom section attribute.  The app
directory contains a generated `zpr_app.cmake` with the VM's source list,
includes, fast-path definition and linker snippet.  A missing `CMakeLists.txt`
is generated as a thin wrapper that includes that fragment; a hand-written
`CMakeLists.txt` is preserved and gets the include appended once, while a
previously generated wrapper is refreshed.  `prj.conf` is treated the same
way: a generated file is refreshed, a hand-written one is kept and stale
generator-owned link entries are removed; missing ZPR defaults are appended
once.  For normal (non-benchmark) ZPR images,
`generate.sh` adds `-b` so the native benchmark process supplies the TIM4
heartbeat the RVM time wheel needs; the user app then runs after the native
benchmark.

FPU support is enabled by the board defconfig (`CONFIG_FPU=y`,
`CONFIG_FPU_SHARING=y`, hard-float ABI).  Every thread starts from the 22-word
basic frame, including threads created with `K_FP_REGS`: no FP context exists
before the first VFP instruction.  That instruction sets `CONTROL.FPCA`, and
the next save builds the 56-word extended frame (`S16..S31`, the hardware
frame, `S0..S15/FPSCR`).  `zpr_context.c` builds, captures and restores both
frame classes; the assembly fast path handles the same two classes and falls
back to the Vct path for dirty frames.  When `CONFIG_FPU` is enabled, the
`.rvp`/process header must provide an RVM coprocessor; the build errors if
`RVM_COP_NUM` is zero.  The two-thread FP test and a timer-driven preemption
test pass on H743 (FPv5-DP) with both the fast path and `ZPR_FAST_PATH=0`,
and on F429/F439 (FPv4-SP) with the fast path; the all-Vct frame path is
shared by both SoCs.  Those two validation programs were temporary and are
not kept under `test/`; the retained application is the integer benchmark.
The normal benchmark remains integer-only.

RVM interrupts stay masked through `__zpr_entry`/`z_prep_c`/`z_cstart`, as in
native Zephyr.  The timer driver init only registers the virtual timer;
`zpr_switch_to_main()` publishes `_current` and the pending main-thread frame,
then calls `RVM_Hyp_Int_Ena()`/`RVM_Virt_Int_Unmask()` immediately before the
final yield, matching `arch_switch_to_main_thread()`'s final
`arch_irq_unlock(0)`.

The board has one fallback DTS.  For every Virt the generator writes a full
`zpr_board.dts` with the allocated CPU, `flash0`/`sram0` and `chosen`, and
passes it as `DTS_SOURCE`; multi-guest projects therefore link each guest at
its own physical window.  The generated M7 window advertises 114 KiB usable
SRAM and the M4 window 66 KiB.  The generator
rounds the VM data size down to whole KiB, so up to 848 bytes at the top of
the window are not advertised (and must not be used for a second guest).  A user
`.c` file in the Virt's `Source/app/` folder takes precedence over the
benchmark in both build modes; the benchmark is used only when that folder has
no `.c` and `ZPR_NORMAL` is unset.  The benchmark's TIM2/TIM4 setup lives in
the selected `chip/zpr_test_<chip>.h`
(`ZPR_Test_Timer_Init`/`ZPR_Test_Int_Init`/`ZPR_Test_Int_Disable`) and its ISR
handler in `test/bench.c`; `zpr_test.h` and `bench.c` are added only to the
benchmark app, so a normal guest never compiles them or touches TIM4.  The six
ISR rows are labels over one TIM4 vector hook, not six different interrupt
mechanisms; the benchmark prints that caveat before the rows.  Two ZPR guests
have been run simultaneously on the H743 with this setup; two benchmark
guests would contend for TIM4 and are not a supported configuration.

RVM does not grant this guest a free-running hardware cycle counter.  The
virtual timer tick is the only timebase, so `sys_clock_cycle_get_32()` and
`sys_clock_cycle_get_64()` synthesize a cycle count from `k_uptime_ticks()`
at `CONFIG_SYS_CLOCK_HW_CYCLES_PER_SEC`.  The rate is correct, but the
resolution is one tick; an application that needs fine-grained delays or
measurements must use a timer device granted by the `.rvp`.

Zephyr's `irq_enable()`/`irq_connect_dynamic()` API is not wired to the RVM
virtual vector table: the guest does not own the NVIC.  A device ISR must be
connected with `RVM_Hyp_Vct_Phys()` plus `RVM_Virt_Vct_Reg()`, as the
benchmark chip header does.  `CONFIG_NUM_IRQS` must not exceed the VM's
`RVM_VIRT_VCT_NUM` (the generated projects currently use 8).

Licensing: the port files under `port/` and `test/`, and the added board and
SoC files, are Unlicense.  The modified upstream arch files remain
Apache-2.0, as indicated in their headers.  See the main library's
`Guest/GUEST.md` for the full contract, deviations and known limitations.
