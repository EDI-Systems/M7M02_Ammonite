/*
 * RT-Thread V4.1.1 benchmark testbench for the CH32V307VC, running as a
 * guest of the RVM hypervisor.
 * Copyright (C) 2026 Evo-Devo Instrum. All rights reserved.
 *
 * This file is NOT part of the RT-Thread distribution.  It belongs to the
 * RT-Thread-on-RVM test suite and is released under the Unlicense; refer
 * to the LICENSE file for details.  The RT-Thread kernel itself is
 * copyright RT-Thread Development Team and is not modified by this file.
 *
 * This file holds all the hardware dependent parts of the RT-Thread
 * benchmark:  the counter the tests are timed with, and the interrupt
 * source that drives the interrupt tests.  The RVM project generator
 * selects it through the generated "rtt_test.h" header.
 *
 * Benchmark archive (CH32V307VCT6 @ 144MHz, small memory allocator,
 * RV32F coprocessor):
 *
 * riscv-none-elf-gcc 12.2.0 -O3
 * ====================================================
 * Test (number in CPU cycles)        : AVG / MAX / MIN
 * Yield                              : 533 / 570 / 533
 * Mailbox                            : 849 / 893 / 849
 * Semaphore                          : 697 / 728 / 697
 * Message queue                      : 1026 / 1088 / 1026
 * Memory allocation/free pair        : 6020 / 6129 / 5980
 * ISR Mailbox                        : 2451 / 2506 / 2451
 * ISR Semaphore                      : 2272 / 2290 / 2269
 * ISR Message queue                  : 2613 / 2651 / 2613
 * ISR RVM activation relay           : 2352 / 2431 / 2352

 * Re-verified 2026-10-28 after the official-style reformat (zero
 * instruction change, board run 9/9 zero faults, values reproduce
 * the archive bit-exact: Yield 533/567/533).
  */

#ifndef RTT_TEST_CH32V307VC_RVM_H
#define RTT_TEST_CH32V307VC_RVM_H

/* RVM hypervisor interface includes. */
#include "rvm.h"

/* Kernel test interface includes - the kernel side of the benchmark
 * latches the timestamp of its own interrupt handler entry into the shared
 * memory this virtual machine can read back, so the activation relay
 * latency can be measured as well. */
#define RVM_TEST_VIRTUAL_INC
#include "rvm_test.h"
#undef RVM_TEST_VIRTUAL_INC

/*-----------------------------------------------------------*/

/* The benchmark print macros - the results print through the RVM debug
 * printing functions, so that no console driver is needed at all. */
#define BENCH_PRINT_STRING(pcString)            RVM_DBG_S(pcString)
#define BENCH_PRINT_NUMBER(ulNumber)            RVM_DBG_I(ulNumber)

/* The size of the stack allocated to each benchmark thread, measured in
 * bytes.  The RV32 switch frame is 37 integer words plus the 33 word FPU
 * transient of the fast path - 280 bytes in total - so the stacks need
 * to be at least double the ARM size, and some margin is added for the
 * benchmark's own call depth. */
#define BENCH_TASK_STK_SIZE                     ((rt_uint32_t)1024)

/* The benchmark is timed with TIM1, which counts up at the CPU clock
 * frequency and is only 16 bits wide on this chip. */
#define BENCH_TIME_GET()                        ((rt_uint32_t)(TIM1_CNT))

/* The timestamp latched by the kernel side of the benchmark when it took
 * the very interrupt that is being relayed to us - the difference between
 * this and the counter value at our own handler entry is the RVM activation
 * relay latency. */
#define BENCH_KERNEL_TS_GET()                   (START)

/* The size of the blocks that the memory test allocates and frees, and
 * the maximum count of the semaphore, and the length and the item size of
 * the mailbox and the message queue, that are used by the benchmark. */
#define BENCH_MEM_BLK_SIZE                      ((rt_size_t)32)
#define BENCH_MEM_BLK_QTY                       (8u)
#define BENCH_MAX_SEMAPHORE_COUNT               (8u)
#define BENCH_QUEUE_LENGTH                      (8u)
#define BENCH_QUEUE_ITEM_SIZE                   (sizeof(rt_uint32_t))

/*-----------------------------------------------------------*/

/* The handful of peripheral registers that the benchmark touches, named
 * after the WCH reference manual - no vendor library is needed. */
#define TIM1_BASE                               (0x40012C00UL)
#define TIM4_BASE                               (0x40000800UL)
#define RCC_BASE                                (0x40021000UL)

#define RCC_APB2PCENR                           (*((volatile rt_uint32_t *)(RCC_BASE + 0x18UL)))
#define RCC_APB2PCENR_TIM1EN                    (1UL << 11UL)

#define RCC_APB1PCENR                           (*((volatile rt_uint32_t *)(RCC_BASE + 0x1CUL)))
#define RCC_APB1PCENR_TIM4EN                    (1UL << 2UL)

#define TIM1_CTLR1                              (*((volatile rt_uint16_t *)(TIM1_BASE + 0x00UL)))
#define TIM1_SMCFGR                             (*((volatile rt_uint16_t *)(TIM1_BASE + 0x08UL)))
#define TIM1_CNT                                (*((volatile rt_uint16_t *)(TIM1_BASE + 0x24UL)))

#define TIM4_CTLR1                              (*((volatile rt_uint16_t *)(TIM4_BASE + 0x00UL)))
#define TIM4_DMAINTENR                          (*((volatile rt_uint16_t *)(TIM4_BASE + 0x0CUL)))
#define TIM4_INTFR                              (*((volatile rt_uint16_t *)(TIM4_BASE + 0x10UL)))
#define TIM4_PSC                                (*((volatile rt_uint16_t *)(TIM4_BASE + 0x28UL)))
#define TIM4_ATRLR                              (*((volatile rt_uint16_t *)(TIM4_BASE + 0x2CUL)))

#define TIM_CTLR1_CEN                           (1UL << 0UL)
#define TIM_CTLR1_DIR                           (1UL << 4UL)
#define TIM_INTFR_UIF                           (1UL << 0UL)
#define TIM_DMAINTENR_UIE                       (1UL << 0UL)

/*-----------------------------------------------------------*/

/*
 * The benchmark interrupt handler, which is implemented by the benchmark
 * source file itself.  It is registered with the RVM hypervisor by the
 * Bench_IntSetup() function below.
 */
void Bench_TestIntHandler(void);

/*
 * Initialise the counter that the benchmark is timed with.
 */
void Bench_TimerSetup(void);

/*
 * Initialise the periodic interrupt source that drives the interrupt
 * tests, and connect it to this virtual machine.
 */
void Bench_IntSetup(void);

/*
 * Disable the periodic interrupt source again.
 */
void Bench_IntDisable(void);

/*-----------------------------------------------------------*/

void Bench_TimerSetup(void)
{
    /* Enable the TIM1 clock. */
    RCC_APB2PCENR |= RCC_APB2PCENR_TIM1EN;

    /* Let TIM1 count up through its full 16 bit range at the CPU clock
     * frequency, without any prescaling or slave mode selection. */
    TIM1_CTLR1 = 0x0UL;
    TIM1_SMCFGR = 0x0UL;
    TIM1_CTLR1 |= TIM_CTLR1_CEN;
}

void Bench_IntSetup(void)
{
    /* Connect the physical TIM4 interrupt to vector 2 of this virtual
     * machine. */
    RVM_ASSERT(RVM_Hyp_Vct_Phys(46U, 2U) == 0U);

    /* Enable the physical interrupt at the lowest priority. */
    RVM_ASSERT(RVM_RV32P_Int_Local_Mod(KFN_INT_LOCAL_MOD, 46U, RVM_RV32P_KFN_INT_LOCAL_MOD_STATE_SET, 1U) == 0U);
    RVM_ASSERT(RVM_RV32P_Int_Local_Mod(KFN_INT_LOCAL_MOD, 46U, RVM_RV32P_KFN_INT_LOCAL_MOD_PRIO_SET, 0xFFU) == 0U);

    /* Register the handler that is implemented by the benchmark itself. */
    RVM_Virt_Vct_Reg(2U, Bench_TestIntHandler, RVM_VCT_MASKABLE);

    /* Enable the TIM4 clock, and let the counter count down from the
     * reload value, which is reached every 57600 CPU cycles. */
    RCC_APB1PCENR |= RCC_APB1PCENR_TIM4EN;
    TIM4_CTLR1 = TIM_CTLR1_DIR;
    TIM4_PSC = 0x0UL;
    TIM4_ATRLR = 14400U * 4U;
    TIM4_INTFR = 0x0UL;
    TIM4_DMAINTENR |= TIM_DMAINTENR_UIE;
    TIM4_CTLR1 |= TIM_CTLR1_CEN;
}

void Bench_IntDisable(void)
{
    /* Disable the physical interrupt. */
    RVM_ASSERT(RVM_RV32P_Int_Local_Mod(KFN_INT_LOCAL_MOD, 46U, RVM_RV32P_KFN_INT_LOCAL_MOD_STATE_SET, 0U) == 0U);

    /* Reverse the vector registration. */
    RVM_Virt_Vct_Reg(2U, RVM_NULL, RVM_VCT_MASKABLE);
}

#endif /* RTT_TEST_CH32V307VC_RVM_H */
