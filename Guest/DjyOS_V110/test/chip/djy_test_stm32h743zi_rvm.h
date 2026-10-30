/*
 * DjyOS V1.1.0 benchmark testbench for the STM32H743ZI, running as a
 * guest of the RVM hypervisor.
 * Copyright (C) 2026 Evo-Devo Instrum. All rights reserved.
 *
 * This file is NOT part of the DjyOS distribution.  It belongs to the
 * DjyOS-on-RVM test suite and is released under the Unlicense; refer
 * to the LICENSE file for details.  The DjyOS kernel itself is
 * copyright DjyOS Open Source Development team and is not modified by
 * this file.
 *
 * This file holds all the hardware dependent parts of the DjyOS
 * benchmark:  the counter the tests are timed with, and the interrupt
 * source that drives the interrupt tests.  The kernel's own board glue
 * lives in the port's board.c and needs none of it, so the RVM project
 * generator only selects this file - through the generated "djy_test.h"
 * header - in benchmark builds.
 *
 * Benchmark archive (STM32H743ZIT6 rev.Y @ 400MHz, VOS scale 1,
 * integer-only port, vector stack 0x800):
 *
 * arm-none-eabi-gcc 13.2.1 -O3
 * ====================================================
 * Test (CPU cycles)                 : AVG / MAX / MIN
 *
 * Yield                             : 1017 / 4896 /  928
 * Notification                      :  805 / 4696 /  764
 * Activation                        : 1065 / 5144 / 1008
 * Semaphore                         :  892 / 4512 /  768
 * Message queue (4B)                : 1488 / 5256 / 1368
 * Memory (Malloc/Free 128B)         :  288 / 3876 /  256
 * ISR Notification                  : 1015 / 2216 /  940
 * ISR Activation                    : 1381 / 1508 / 1280
 * ISR Semaphore                     : 1220 / 2356 / 1132
 * ISR Message queue (4B)            : 1873 / 2104 / 1716
 * ISR RVM relay                     : 2822 / 4380 / 1864
 *
 * The table above is the pre-split build.  Since the boot glue moved
 * out of this benchmark board file into the port's board.c (so that a
 * plain application links no test hardware at all), the layout moved
 * and the averages below are the split build - same image, same
 * sentinels, reproducible exactly across resets, but up to ~10% away
 * from the old table (short paths are placement sensitive):
 *
 * Yield                             : 1061 / 4944 /  936
 * Notification                      :  888 / 4832 /  780
 * Activation                        : 1157 / 5080 / 1012
 * Semaphore                         :  867 / 4728 /  732
 * Message queue (4B)                : 1516 / 5340 / 1364
 * Memory (Malloc/Free 128B)         :  296 / 3728 /  256
 * ISR Notification                  :  984 / 2192 /  924
 * ISR Activation                    : 1313 / 1420 / 1232
 * ISR Semaphore                     : 1223 / 2432 / 1112
 * ISR Message queue (4B)            : 1802 / 1944 / 1676
 * ISR RVM relay                     : 2569 / 4252 / 1748
 *
 * The per-thread stack paint is switched off in this build: djyos.c
 * guards its three 'd' memsets with #ifndef CFG_STACK_PAINT_DISABLE and
 * the port's project_config.h defines that macro.  Measured on the same
 * board, the paint costs about 1050 cycles on every item that creates
 * or recycles a thread per round (with the paint on: Yield 2073,
 * Notification 1863, Activation 2159, ISR Notification 1943, ISR
 * Activation 2299) and nothing on the paths that post to a parked worker,
 * allocate memory or only relay the interrupt (Semaphore, MsgQ, Memory
 * and the ISR Semaphore/MsgQ/relay figures are within noise of the
 * numbers above).  The paint is the input of __DJY_CheckStack, so a
 * build that re-enables that check (CFG_IDLE_CHECK_STACK) must remove
 * the macro again.
 *
 * Notes: the interrupt source is TIM4 at a 40000 CPU cycle period; the
 * five ISR items run as an interrupt-driven phase sequence (10000
 * interrupts each) with no measuring thread.  A "Yield" here is one
 * full event turnover (pop -> completion -> CN_KEEP recycle -> entry
 * into the next event), not a bare context switch, which is why it sits
 * near Notification rather than near the other guests' yield; the 256B
 * control build (kept in the source, not reported) lands at about the
 * same ~1000 cycles, confirming that the remainder is the turnover
 * itself.
 *
 * The RVM relay item uses the shared page the hypervisor maps into
 * every virtual machine (DATA_SHARED_SHARED1_BASE = 0x24014000, START
 * in its second word): the kernel's TIM4 top half stamps the counter
 * there before it signals the vector, and the guest's ISR charges the
 * difference.  Both reads are 16 bit like the other guests' relay item,
 * and the 20000 tick interrupt period keeps the difference unambiguous.
 *
 * Boot order (important for any future edit of djy_bench_boot): the
 * driver event is popped by the idle event's first service pass, never
 * by djy_main.  A pop makes the ready queue differ from the running
 * event and schedules immediately; issued from the startup context it
 * would switch out a context that belongs to no thread and the kernel
 * would file the startup frame under the running event's thread - the
 * idle event's - whose stack the startup code then reuses, leaving the
 * idle thread with an unenterable frame.
 *
 * Activation recycling (important for any create/delete loop): the
 * kernel's static heap is a bump allocator whose free only rolls the
 * bump pointer back when the freed block is the LAST allocation; any
 * allocation interleaved after it permanently leaks that block.  The
 * Activation item therefore frees the previous round's thread at the
 * top of every round, before the next allocation, and only asks the
 * idle event for a free pass once every 512 rounds.  A batched (every
 * 8 rounds) idle-side reclaim leaks ~2kB per thread and dies of heap
 * exhaustion after ~14 rounds, with the kernel spinning forever in
 * __DJY_SelectEventToRun's "create the thread" loop.
 *
 * The archived runs show the resulting signature at zero: defers=0,
 * padded=0, badframes=0, foreign saves=0, "Benchmark complete", no
 * restart; the AVG figures repeat to about 1% between resets (the MAX
 * and MIN columns pick up interrupt interference, and the ISR items
 * occasionally report a sub-1000 MIN from the pop-then-stamp window).
 *
 * FPU: two virtual machine configurations are verified - without a
 * coprocessor (RME_COP_NUM=0) and with FPV5_DP on the chip and on
 * Virt1 (RME_COP_NUM=1, the guest makefile gains -mfpu=fpv5-d16 and
 * the kernel toggles CPACR and swaps the coprocessor context per
 * thread).  Both deliver the same eleven figures within the spread
 * above (the FPU build pays a few cycles per kernel thread switch for
 * the Cop swap).  The guest itself stays soft-float: the port builds
 * basic exception frames only, so a guest compiled with a float ABI
 * would not carry S16-S31 across a DjyOS thread switch; cpus.s turns
 * that into a build error instead of a silent corruption.
 */

#ifndef DJY_TEST_STM32H743ZI_RVM_H
#define DJY_TEST_STM32H743ZI_RVM_H

/* RVM hypervisor interface includes - the kernel side of the benchmark
 * latches the timestamp of its own interrupt handler entry into the shared
 * memory this virtual machine can read back, so the activation relay
 * latency can be measured as well. */
#define RVM_TEST_VIRTUAL_INC
#include "rvm_test.h"
#undef RVM_TEST_VIRTUAL_INC

/*-----------------------------------------------------------*/

/* The benchmark print macros - the results print through the RVM debug
 * console hypercall (one character per hypercall), so no console driver
 * is needed at all.  The benchmark formats the numbers itself. */
#define BENCH_PRINT_STRING(pcString)            djy_dbg_puts(pcString)
#define BENCH_PRINT_CHAR(cChar)                 djy_dbg_putc(cChar)
#define BENCH_PRINT_U32(ulValue)                djy_dbg_putu32(ulValue)

/* The APB1 clock gate - the hypervisor leaves the peripheral clocks
 * off; both benchmark timers hang off APB1 (the same registers the
 * other RVM guest tests on this chip use). */
#define DJY_RCC_BASE                           (0x58024400UL)
#define DJY_RCC_APB1ENR                        (*((volatile u32 *)(DJY_RCC_BASE + 0xE8UL)))
#define DJY_RCC_APB1ENR_TIM2                   (1UL << 0UL)
#define DJY_RCC_APB1ENR_TIM4                   (1UL << 2UL)

/*-----------------------------------------------------------*/

/* The benchmark timing counter - TIM2 free-running at the APB1 timer
 * clock (200MHz, half the core clock: APB1 prescaler 4 with timer
 * multiplier 2).  This is register access into the device memory the
 * hypervisor has mapped into this VM, no hypercall involved.  The
 * prescaler is 1 so every counter increment is one timer tick, and the
 * shift converts to the 400MHz CPU cycles the other RVM benchmarks
 * report on this chip. */
#define DJY_TIM2_BASE                           (0x40000000UL)
#define DJY_TIM2_CNT                            (*((volatile u32 *)(DJY_TIM2_BASE + 0x24UL)))
#define DJY_TIM2_PSC                            (*((volatile u32 *)(DJY_TIM2_BASE + 0x28UL)))
#define DJY_TIM2_ARR                            (*((volatile u32 *)(DJY_TIM2_BASE + 0x2CUL)))
#define DJY_TIM2_CR1                            (*((volatile u32 *)(DJY_TIM2_BASE + 0x00UL)))

#define BENCH_TIM_READ()                        ((u32)(DJY_TIM2_CNT << 1UL))

/* The interrupt source - TIM4, delivered as RVM vector 2 (the same
 * mapping as the RTT test on this chip).  The reload is programmed
 * for a 40000 CPU cycle interval: same frequency as the FRT/RTT ISR
 * tests, so the interrupt items stay comparable across the OSes. */
#define DJY_TIM4_BASE                           (0x40000800UL)
#define DJY_TIM4_CNT                            (*((volatile u32 *)(DJY_TIM4_BASE + 0x24UL)))
#define DJY_TIM4_PSC                            (*((volatile u32 *)(DJY_TIM4_BASE + 0x28UL)))
#define DJY_TIM4_ARR                            (*((volatile u32 *)(DJY_TIM4_BASE + 0x2CUL)))
#define DJY_TIM4_DIER                           (*((volatile u32 *)(DJY_TIM4_BASE + 0x0CUL)))
#define DJY_TIM4_SR                             (*((volatile u32 *)(DJY_TIM4_BASE + 0x10UL)))
#define DJY_TIM4_CR1                            (*((volatile u32 *)(DJY_TIM4_BASE + 0x00UL)))
#define DJY_TIM4_SR_UIF                         (1UL << 0UL)
#define DJY_TIM4_DIER_UIE                       (1UL << 0UL)
#define DJY_TIM4_CR1_CEN                        (1UL << 0UL)

#define DJY_PHYS_TIM4                           (30U)
#define DJY_VCT_TIM4                            (2U)
#define DJY_TIM4_RELOAD                         (20000UL)   /* x2 = 40000 CPU cycles */

/* The kernel tick source is the RVM virtual timer channel (registered
 * through RVM_Virt_Tim_Reg in djy_board.c), the same source the other
 * RVM guests on this hypervisor use.  A guest cannot own the physical
 * SysTick, so the virtual timer feeds DJY_ScheduleIsr(1) once per
 * delivery; no general purpose timer is spent on the tick. */

/* The stack sizes for the benchmark event types. */
#define DJY_EVTT_STK_SIZE                       (2048UL)
#define DJY_LOW_STK_SIZE                        (256UL)

#endif /* DJY_TEST_STM32H743ZI_RVM_H */
