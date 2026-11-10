# RTE on RVM benchmark baseline - STM32F429

## Configuration

- Board: NUCLEO-F439ZI (device ID 0x419, STM32F42xxx/F43xxx family; the
  RVM project and the BSP use the STM32F429ZI platform), RVM hypervisor,
  RTE (RTEMS 7.0-rc1) guest.
- Toolchain: `arm-rtems7-gcc` 15.2.0 (RTEMS 7.0-rc1, newlib nano).
- Core: Cortex-M4F, FPv4-SP, `-mcpu=cortex-m4 -mfpu=fpv4-sp-d16`.
- RVP: `gccmf-stm32f429zit6-rte.rvp` - Code 0x08020000/0x40000,
  Data block 0x20000000/0x14000 (80 KiB), Virt1 `Period` 10 (10 Hz RTE tick),
  user stack 0x2000, vector stack 0x1000, Vector_Num 8.
- RTE layout: `RVM_DATA_EXACT = 0x10F50`, `RVM_DATA_SIZE = 0x10B50`
  (1 KiB guard), `RVM_VIRT_REG_BASE = 0x20010F50`,
  `RVM_VIRT_STATE_BASE = 0x20010FC0`, user stack 0x20011000/0x2000,
  vector stack 0x20013000/0x1000, data block end 0x20014000.
- Build chain: `benchmark.sh gccmf-stm32f429zit6-rte.rvp`, then
  Monitor -> Proc1 -> Proc2 -> Virt1 (waf) -> Kernel; the Kernel
  `kernel.bin` is flashed at 0x08000000.  Virt1 code is placed at
  0x08020000 by the generated `rte_config.ini`; the monitor, Proc1, Proc2
  and Virt1 code regions are at 0x08010000, 0x08060000, 0x08068000 and
  0x08020000 respectively.
- The default direct context-switch fast path is enabled
  (`RTE_FAST_PATH = True`).
- The benchmark tasks use the default RTEMS task attributes (basic 22-word
  frames).
- TIM2 is the free-running cycle counter (`BENCH_TIME_GET = TIM2_CNT << 1`);
  TIM4 (ARR 18000, APB1 timer clock) drives the interrupt rows; RCC APB1ENR
  is at 0x40023840.  The chip header is
  `testsuites/samples/rte_benchmark/chip/rte_test_stm32f429zi.h`, selected by
  the generated `rte_test.h`.

## Results

The per-chip performance archive is in `chip/rte_test_stm32f429zi.h`; it
contains the optimized default fast-path table next to the board
definitions.  This file keeps the build configuration and the F429-specific
notes.

## F429-specific notes

- The common ARM `_CPU_Initialize()` programs SVC/PendSV priorities through
  the NVIC system registers.  The unprivileged RTE guest must not do that, so
  the RVM CPU port replaces it with an empty implementation
  (`cpukit/score/cpu/arm/v7m_rvm/v7m_rvm.c`).  Without this the
  F429 guest took a precise data access fault at startup.
- The local RTEMS 7 GCC toolchain originally had `libatomic`/`libstdc++`
  only for Cortex-M7.  The missing target libraries were built from the GCC
  15.2.0 source and installed for Cortex-M3, Cortex-M4-soft, Cortex-M4-hard
  and Cortex-M7-hard, with their per-multilib C++ headers.  The RTE link
  resolves `-latomic` against the real library; no stub is used.  Native
  `arm/stm32f4` `hello.exe` and `iostream.exe` now link, and `librtemscxx` is
  enabled again for RTE.
- The board enumerates as NUCLEO-F439ZI; the RVM F429 platform and the
  STM32F429ZI RVP run unchanged on it (same 0x419 device family).  The
  original instruction named the target STM32F429.
