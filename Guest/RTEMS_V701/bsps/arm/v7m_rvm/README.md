# RTEMS on RVM (RTE) BSP

This BSP runs RTEMS 7 as an unprivileged RVM guest on ARMv7-M
(Cortex-M4F and Cortex-M7).  The guest is called **RTE** in RVM terms; the
RTEMS BSP name is `arm/v7m_rvm`.  GCC is the only supported toolchain.
The verified targets are NUCLEO-H743ZI (STM32H743ZIT6, Cortex-M7/FPv5-DP) and
NUCLEO-F429ZI/F439ZI (STM32F429ZI class, Cortex-M4F/FPv4-SP).

## Licensing

Two licenses apply:

- Files derived from existing RTEMS sources retain their original terms.
  The three `v7m_isr_*_rvm.c` sources and the top-level `wscript` are
  BSD-2-Clause.  The files under `spec/build/` follow the RTEMS build-spec
  license (`CC-BY-SA-4.0 OR BSD-2-Clause`).
- All other files added by this port (BSP, CPU port, samples, benchmark and
  documentation) are released under the Unlicense (https://unlicense.org/).

The SPDX header in each file is authoritative if this summary and the file
ever disagree.

## Build chain

The RVM project generator owns the RTE workspace.  It reads the `.rvp` file
and produces three things:

1. `<Virt>/Project/rte_config.ini` - the RTEMS `--rtems-config` file, derived
   from the RVM data block and stack sizes.  It also carries `RVM_ROOT`, the
   M7M02 tree location used by the BSP spec for the shared RVM Include/Source
   files, and `RTE_FAST_PATH` (default `True`).  Set `RTE_FAST_PATH=0` in the
   generator environment before regenerating to produce an all-Vct build; the
   value can also be changed in an existing `rte_config.ini` and rebuilt
   without regenerating the project.  `RTE_CPU_FLAGS` carries the selected
   `-mcpu`/`-mfpu` pair: Cortex-M7/FPv5 for `FPV5_DP` and Cortex-M4/FPv4-SP
   for `FPV4_SP`.
2. `<Virt>/Include/rte_config.h` - the RTE layout header, generated inside the
   workspace.  It includes the staged `rte_prc.h` for the project macros
   (code/data bases, RVM register/state blocks, vector count, coprocessor
   selection) and adds the RTE data-region and stack macros.
3. `<Virt>/Source/app/` - the per-VM application folder; the generator
   preserves it across regeneration.  Any `*.c` files placed there are
   user-application sources and take precedence over the benchmark special
   case.  If no user source exists, benchmark `Virt1` uses
   `testsuites/samples/<app>/init.c` and the folder only contains `rte_test.h`
   (selects the chip header `rte_benchmark/chip/rte_test_<chip>.h` from the
   `.rvp` chip); other VMs get an empty `rte_app.c` template.
4. `<Virt>/Project/rte_copy.sh` - a staging script generated with absolute
   source/destination paths.  The generator writes nothing into the RTEMS
   tree; the `prc_<vm>` makefile runs this script before every waf invocation,
   copying `Source/prc_<vm>.c` and `prc_<vm>_desc.c` into
   `bsps/arm/v7m_rvm/start/` as `rte_entry.c` and `rte_entry_desc.c`,
   `Include/prc_<vm>.h`, `rvm_platform.h`, `rvm_platform_a7m_conf.h` and
   `rte_config.h` into `bsps/arm/v7m_rvm/include/` (the generated platform
   headers are staged, not kept in the source tree), the M7M02
   `Source/Platform/A7M/rvm_platform_a7m_gcc.s` as
   `bsps/arm/v7m_rvm/start/rvm_platform_a7m_gcc.S`, and the contents of
   `Source/app/` into `bsps/arm/v7m_rvm/app/`.  The BSP spec references the
   shared M7M02 RVM headers and C sources directly through `RVM_ROOT`; the
   BSP source tree contains no copied M7M02 RVM implementation or adapter.
   The app spec compiles `${RTE_APP_SOURCES}`: the benchmark `init.c` for
   benchmark `Virt1`, or every staged user `.c` file in the app folder.  A
   multi-file user app is therefore compiled and linked as separate
   translation units.
   `linkcmds.v7m_rvm` places the descriptor `.rodata` into `.PROCESS_DESC` at
   the image base (0x080B0000).  `start/rvm_entry.c` keeps only `main()` and
   the RVM handler registration, not a duplicate entry or descriptor.
5. `<Virt>/Project/prc_<vm>` - a makefile that configures and builds RTEMS
   with waf, then converts the guest executable:

```text
make -f prc_<vm>
  -> sh rte_copy.sh
  -> waf configure --rtems-config=rte_config.ini --prefix=<RTEMS prefix>
  -> sh rte_copy.sh
  -> waf
  -> arm-rtems7-objcopy -O binary .../testsuites/samples/prc_<vm>.exe <vm>.bin
  -> bincopy.exe -i <vm>.bin -o <vm>_image.c
```

The RVM Kernel links `<Virt>/Project/<vm>_image.c`; no prebuilt RTE binary is
embedded by hand.  `RTE` skips `GCC_Gen`'s per-Virt linker-script generation:
`Proj_Gen::Process_Linker` removes any stale `prc_<vm>_*.ld` and logs that the
BSP `linkcmds.v7m_rvm` is authoritative.  Other guests still generate their
own linker scripts.  The RTE application target is always named after the VM:
`testsuites/samples/prc_<vm>.exe`.  The benchmark is a separate application
(`rte_benchmark`); `RTE_Gen` uses it only for `Virt1` in benchmark mode when
the app folder has no user `.c` files.  `RTE_APP_SOURCES` is the benchmark
`init.c` or the staged per-VM user `.c` files; the whole app folder is copied
before waf.  The generated `rte_test.h` selects the chip header, so the
benchmark's TIM2/TIM4/RCC definitions are not hard-coded in the test body.

The RTEMS tree and `build/arm/v7m_rvm` are shared by all RTE VMs, so the
makefile re-stages this VM's headers/sources and reruns `waf configure` with
this VM's `rte_config.ini` on every Virt build before running waf.  This lets
a shared tree build Virt1 and Virt2 in sequence with different Data/Code sizes
or `Vector_Num`; the Virt builds must not run concurrently.  The waf environment needs an MSYS2 `PATH`; the generator
converts the Windows RTEMS prefix with `cygpath -u`, because waf's Python
splits `PATH` at `:` and would otherwise truncate `C:/...`.

A complete workspace build order is Monitor, Proc1, Proc2, each Virt in
sequence, Kernel last.

The BSP group links the standard `opto2` option before `bspopts`, so the
`BSP_OPTIMIZATION_FLAGS` substitution sees `OPTIMIZATION_FLAGS`
(`-O2 -g -fdata-sections -ffunction-sections`).  This ordering is required
for the optimization flags to reach CFLAGS.

## Memory layout (128 KiB RVP)

`gccmf-stm32h743zit6-rte.rvp` uses:

| Region | Base | Size |
|---|---|---:|
| Code | 0x080B0000 | 0x40000 |
| Data block | 0x30000000 | 0x20000 |
| RTE data (`RVM_DATA_EXACT`) | 0x30000000 | 0x1CF50 |
| RTE usable data (`RVM_DATA_SIZE`) | 0x30000000 | 0x1CB50 |
| RVM registers | 0x3001CF50 | 0x70 |
| RVM state | 0x3001CFC0 | 0x40 |
| User stack | 0x3001D000 | 0x2000 |
| Vector stack | 0x3001F000 | 0x1000 |
| Data block end | 0x30020000 | - |

`RVM_DATA_SIZE` is `RVM_DATA_EXACT` minus a 1 KiB guard.  RTEMS links its
data/bss/workspace inside `RVM_DATA_SIZE`; `.work` fills the remaining space
up to that boundary.  The RVM register and state blocks, the user stack and
the vector stack sit above it.  Changing the `.rvp` data block or stack sizes
regenerates both `rte_config.ini` and `rte_config.h`.

## CPU port

The RVM CPU port is a separate port under
`cpukit/score/cpu/arm/v7m_rvm/`; the shared `cpukit/score/cpu/arm` sources are
unmodified.  The `cpuarm_rvm` spec is selected by `RTEMS_RVM`, while the
standard `cpuarm` spec is disabled for this BSP.  A small
`include/rtems/score/cpu.h` overlay puts the RVM interrupt-mask macros in
front of the common ARM header.

The guest cannot program BASEPRI, PendSV, SVC, NVIC, SysTick or the vector
table.  The port therefore replaces the standard ARMv7-M mechanisms:

- Interrupt disable/enable is a software nesting count plus the RVM virtual
  interrupt mask.
- `_CPU_Initialize` has an RVM-specific empty implementation.  The common ARM
  version programs the SVC/PendSV priorities through the NVIC system
  registers, which an unprivileged guest must not touch; the monitor owns
  those exceptions and their priorities.
- `Thd_Vct` comes from the generated `prc_virt1.c`; `RVM_Desc` comes from the
  generated `prc_virt1_desc.c`.  The BSP owns only `main()` and the handler
  registration.
- The fast path in `_CPU_Context_switch` resumes a clean heir directly on the
  task stacks.  It is used only when the current XPSR has no EPSR ICI/IT
  continuation and the heir frame is executable, thumb, free of ICI/IT, and
  marked by `register_lr != 0` (a dispatcher or initial frame).  The routine
  builds the same masqueraded frame, swaps SP, republishes the heir's
  hypercall parameters in `RVM_STATE->Usr`, restores the GP/FP registers and
  resumes without entering the monitor.
- Dirty frames - the bodies captured by an ISR whose EPSR ICI/IT state can
  only be restored by a real exception return - always take the slow path:
  build the masqueraded frame, store the heir in `rte_pending_heir`, call
  `RVM_Virt_Yield()`, and let the vector thread's context handler perform the
  switch.
- `RTE_FAST_PATH` is a BSP boolean build option (default `True`) carried by
  `rte_config.ini` and translated to `-DRTE_FAST_PATH=1/0`.  When it is 0, the
  C fast-path branch is compiled out and the
  `_RVM_CPU_Context_switch_swt` assembly routine is excluded by its own
  preprocessor guard, so an all-Vct build contains no fast-path stacking
  code at all.
- The basic frame is 22 words; the hard-float extended frame is 56 words.
  Extended frames keep S16-S31 above the R4-R11/EXC_RETURN block, then the
  hardware frame R0-R3/R12/LR/PC/xPSR, then S0-S15/FPSCR.
- `Context_Control::register_lr` is reused as a context-kind marker: 1 means
  a dispatcher or initial frame (the context handler restores
  `thread_dispatch_disable_level = 1`), 0 means an ISR-captured body frame
  (restore level 0).  This is required for a task to continue correctly after
  a deferred interrupt-driven dispatch.
- In the vector thread, `_CPU_Context_switch` only records the heir; after
  `_Thread_Dispatch` returns, `rte_rvm_dispatch_complete()` yields to the
  monitor.  The context handler loads the heir frame into `RVM_REG`.
- `RVM_Virt_Init()` clears the virtual handler table and runs in the
  higher-priority vector thread.  The user thread registers the tick and
  context handlers afterwards, then calls `RVM_Hyp_Int_Ena()` and
  `RVM_Virt_Int_Unmask()`.

The RTE tick is the RVM virtual timer; the `.rvp` `Period` is in units of the
RME 10 ms tick, so `Period = 10` gives a 100 ms (10 Hz) RTEMS tick.

## Console, reset and idle

- Console output is polled through `RVM_Hyp_Putchar`; input is EOF.
- `bsp_reset()` calls `RVM_Hyp_Reboot()`.
- The idle task body calls `RVM_Hyp_Vct_Wait()`, following the RVM guest
  convention of parking the idle context in the monitor.

## Benchmark

`testsuites/samples/rte_benchmark` is the RTEMS counterpart of the RMP
benchmark: Yield, Semaphore, FIFO, Message queue, Blocking message queue,
Alarm combination, Memory, ISR Semaphore, ISR Message queue, ISR Blocking
message queue and the RVM activation relay.  Notification and Activation are
RTEMS-specific extra rows.  TIM2 is the free-running cycle counter and TIM4 is
a 100 us virtual-vector interrupt source.  The per-chip performance archive is
in the chip test headers
(`testsuites/samples/rte_benchmark/chip/rte_test_stm32h743zi.h` and
`rte_test_stm32f429zi.h`); the baseline documents keep the build configuration
and the exact RMP-to-RTEMS mapping.

## Known limitations

- RTEMS message queue send is non-blocking, so RMP's blocking-send queue item
  is measured through the blocking receive path with a one-slot queue.
- RTEMS has no public alarm-manager entry point; the alarm benchmark advances
  the classic clock with `rtems_clock_tick()` and masks the virtual tick while
  doing so.
- The RTEMS tree is a local fork: the RVM CPU port lives in its own
  `cpukit/score/cpu/arm/v7m_rvm/` directory and is selected by the
  `cpuarm_rvm` spec; the common `arm` sources are not modified.  RTE
  references the M7M02 RVM `Include/` and `Source/` trees through `RVM_ROOT`;
  all RVM headers used by the BSP are staged from the generated workspace by
  `rte_copy.sh`.
- The local RTEMS 7 GCC toolchain originally shipped C libc/libgcc for many
  multilibs but `libatomic`/`libstdc++` only for Cortex-M7.  The missing
  target libraries were built from the GCC 15.2.0 source and installed for
  `thumb/cortex-m3`, `thumb/cortex-m4+nofp`, `thumb/cortex-m4/hard` and
  `thumb/cortex-m7/hard`, together with the per-multilib C++ headers.  Native
  `arm/stm32f4` C and C++ samples (`hello.exe`, `iostream.exe`) link, and the
  RTE/H743 builds include `librtemscxx` successfully.
