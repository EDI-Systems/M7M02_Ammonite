/*
 * RT-Thread V4.1.1 benchmark testbench for the STM32G071RB, running as a
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
 * Benchmark archive (STM32G071RBT6 @ 64MHz, small memory allocator,
 * no coprocessor, fast path, switch_to flag=0, 2026-10-28):
 *
 * arm-none-eabi-gcc 13.2.1 -O3
 * ====================================================
 * Test (number in CPU cycles)        : AVG / MAX / MIN
 * Yield                              : 1080 / 3438 / 1077
 * Notification                       : 1755 / 4111 / 1749
 * Semaphore                          : 1450 / 8275 / 1445
 * Message queue                      : 2228 / 4583 / 2218
 * Memory allocation/free pair        : 14899 / 19726 / 14817
 * ISR Notification                   : 3042 / 5392 / 2813
 * ISR Semaphore                      : 2694 / 4998 / 2694
 * ISR Message queue                  : 3498 / 7717 / 3484
 * ISR RVM activation relay           : 3473 / 5828 / 3440
 *
 * ICCARM 9.40 -Oh (context_iar.s)
 * Yield 1031/3459/1028, Notification 1680/4107/1674, Semaphore 1415/3841/1410,
 * Message queue 2073/4502/2067, Memory 13861/20546/13789, ISR Notification
 * 2859/5213/2858, ISR Semaphore 2589/7368/2577, ISR Message queue
 * 3263/5690/3248, ISR relay 3501/6426/3213.
 *
 * ARMCLANG 6.23 -O3 (context_rvds.s)
 * Yield 950/3521/949, Notification 1581/4291/1575, Semaphore 1317/4028/1314,
 * Message queue 1996/4704/1988, Memory 13221/20231/13144, ISR Notification
 * 2760/7910/2746, ISR Semaphore 2464/3660/2464, ISR Message queue
 * 3172/7700/3158, ISR relay 3441/6086/3437.
 *
 * Note(s): all three toolchains 9/9, zero faults; Notification is the
 * item the original archive reported as Mailbox (same primitive, renamed).
  */

#ifndef RTT_TEST_STM32G071RB_RVM_H
#define RTT_TEST_STM32G071RB_RVM_H

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
 * bytes. */
#define BENCH_TASK_STK_SIZE                     ((rt_uint32_t)512)

/* The benchmark is timed with TIM2, which counts up at the CPU clock
 * frequency and is only 16 bits wide on this chip. */
#define BENCH_TIME_GET()                        ((rt_uint32_t)(TIM2_CNT))

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

/* Hardware register definitions, so that the benchmark does not depend on
 * any vendor library.  The register names follow the ST reference manual. */
#define TIM2_BASE                               (0x40000000UL)
#define TIM3_BASE                               (0x40000400UL)
#define RCC_BASE                                (0x40021000UL)

#define RCC_APBENR1                             (*((volatile rt_uint32_t *)(RCC_BASE + 0x3CUL)))
#define RCC_APBENR1_TIM2                        (1UL << 0UL)
#define RCC_APBENR1_TIM3                        (1UL << 1UL)

#define TIM2_CR1                                (*((volatile rt_uint32_t *)(TIM2_BASE + 0x00UL)))
#define TIM2_PSC                                (*((volatile rt_uint32_t *)(TIM2_BASE + 0x28UL)))
#define TIM2_ARR                                (*((volatile rt_uint32_t *)(TIM2_BASE + 0x2CUL)))
#define TIM2_CNT                                (*((volatile rt_uint32_t *)(TIM2_BASE + 0x24UL)))

#define TIM3_CR1                                (*((volatile rt_uint32_t *)(TIM3_BASE + 0x00UL)))
#define TIM3_DIER                               (*((volatile rt_uint32_t *)(TIM3_BASE + 0x0CUL)))
#define TIM3_SR                                 (*((volatile rt_uint32_t *)(TIM3_BASE + 0x10UL)))
#define TIM3_PSC                                (*((volatile rt_uint32_t *)(TIM3_BASE + 0x28UL)))
#define TIM3_ARR                                (*((volatile rt_uint32_t *)(TIM3_BASE + 0x2CUL)))

#define TIM_CR1_CEN                             (1UL << 0UL)
#define TIM_CR1_DIR                             (1UL << 4UL)
#define TIM_SR_UIF                              (1UL << 0UL)
#define TIM_DIER_UIE                            (1UL << 0UL)

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
    /* Enable the TIM2 clock. */
    RCC_APBENR1 |= RCC_APBENR1_TIM2;

    /* Let TIM2 count up through its full 16 bit range at the CPU clock
     * frequency, without any prescaling. */
    TIM2_CR1 = 0x0UL;
    TIM2_PSC = 0x0UL;
    TIM2_ARR = 0xFFFFUL;
    TIM2_CR1 |= TIM_CR1_CEN;
}

void Bench_IntSetup(void)
{
    /* Connect the physical TIM3 interrupt to vector 2 of this virtual
     * machine. */
    RVM_ASSERT(RVM_Hyp_Vct_Phys(16U, 2U) == 0U);

    /* Enable the physical interrupt at the lowest priority. */
    RVM_ASSERT(RVM_A6M_Int_Local_Mod(KFN_INT_LOCAL_MOD, 16U, RVM_A6M_KFN_INT_LOCAL_MOD_PRIO_SET, 0xFFU) == 0U);
    RVM_ASSERT(RVM_A6M_Int_Local_Mod(KFN_INT_LOCAL_MOD, 16U, RVM_A6M_KFN_INT_LOCAL_MOD_STATE_SET, 1U) == 0U);

    /* Register the handler that is implemented by the benchmark itself. */
    RVM_Virt_Vct_Reg(2U, Bench_TestIntHandler, RVM_VCT_MASKABLE);

    /* Enable the TIM3 clock, and let the counter count down from the
     * reload value, which is reached every 12800 CPU cycles. */
    RCC_APBENR1 |= RCC_APBENR1_TIM3;
    TIM3_CR1 = TIM_CR1_DIR;
    TIM3_PSC = 0x0UL;
    TIM3_ARR = 12800U;
    TIM3_SR &= ~TIM_SR_UIF;
    TIM3_DIER |= TIM_DIER_UIE;
    TIM3_CR1 |= TIM_CR1_CEN;
}

void Bench_IntDisable(void)
{
    /* Disable the physical interrupt. */
    RVM_ASSERT(RVM_A6M_Int_Local_Mod(KFN_INT_LOCAL_MOD, 16U, RVM_A6M_KFN_INT_LOCAL_MOD_STATE_SET, 0U) == 0U);

    /* Reverse the vector registration. */
    RVM_Virt_Vct_Reg(2U, RVM_NULL, RVM_VCT_MASKABLE);
}

#endif /* RTT_TEST_STM32G071RB_RVM_H */
