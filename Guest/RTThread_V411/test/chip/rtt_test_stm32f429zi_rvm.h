/*
 * RT-Thread V4.1.1 benchmark testbench for the STM32F429ZI, running as a
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
 * Benchmark archive (STM32F429ZIT6 @ 180MHz, w/FPU context, vector stack
 * 0x1000; fast path + armv7m macro fix + switch_to flag=0, 2026-10-28):
 *
 * ICCARM 9.40 -Oh (context_iar.s)
 * ====================================================
 * Test (number in CPU cycles)        : AVG / MAX / MIN
 * Yield                              : 612 / 2892 / 612
 * Notification                       : 1053 / 3420 / 1052
 * Semaphore                          : 852 / 3228 / 852
 * Message queue                      : 1369 / 3680 / 1364
 * Memory allocation/free pair        : 7646 / 10064 / 7624
 * ISR Notification                   : 2231 / 5648 / 2228
 * ISR Semaphore                      : 2020 / 4196 / 2016
 * ISR Message queue                  : 2484 / 3300 / 2484
 * ISR RVM activation relay           : 3260 / 5360 / 3044
 *
 * arm-none-eabi-gcc 13.2.1 -O3 (context_gcc.s)
 * Yield 648/2752/648, Notification 1173/3244/1172, Semaphore 893/3052/892,
 * Message queue 1517/3600/1516, Memory 7764/9948/7748, ISR Notification
 * 2236/2240/2236, ISR Semaphore 1975/3936/1972, ISR Message queue
 * 2523/4500/2520, ISR relay 2883/4792/2820.
 *
 * ARMCLANG 6.23 -O3 (context_rvds.s)
 * Yield 532/2484/532, Notification 1036/3016/1008, Semaphore 824/2832/780,
 * Message queue 1338/3380/1336, Memory 7307/9416/7284, ISR Notification
 * 1948/2676/1948, ISR Semaphore 1743/3660/1740, ISR Message queue
 * 2256/2256/2256, ISR relay 2704/4356/2496.
 *
 * Note(s): (1) All three toolchains 9/9, zero faults, fast path enabled
 * (2026-10-28, official-style reformat build).  (2) Historical
 * all-slow-path GCC numbers (2026-10-27): Yield 2546, Mailbox 3006,
 * Semaphore 2767, Message queue 3311, Memory 7764, ISR Mailbox 2264,
 * ISR Semaphore 1996, ISR Message queue 2548, ISR relay 2823 - the fast
 * path is worth 3.9x-4.8x on the scheduler level items.
 */

#ifndef RTT_TEST_STM32F429ZI_RVM_H
#define RTT_TEST_STM32F429ZI_RVM_H

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

/* The benchmark is timed with TIM2, which counts up at half the CPU clock
 * frequency (the APB1 timer clock when the APB1 prescaler is not 1), so
 * the counter value is scaled back up to CPU cycles. */
#define BENCH_TIME_GET()                        ((rt_uint32_t)(TIM2_CNT << 1UL))

/* The timestamp latched by the kernel side of the benchmark when it took
 * the very interrupt that is being relayed to us - the difference between
 * this and the counter value at our own handler entry is the RVM activation
 * relay latency. */
#define BENCH_KERNEL_TS_GET()                   (START)

/* The size of the blocks that the memory test allocates and frees, and
 * the maximum count of the semaphore, and the length and the item size of
 * the mailbox and the message queue, that are used by the benchmark. */
#define BENCH_MEM_BLK_SIZE                      ((rt_size_t)32)
#define BENCH_MEM_BLK_QTY                      (8u)
#define BENCH_MAX_SEMAPHORE_COUNT               (8u)
#define BENCH_QUEUE_LENGTH                      (8u)
#define BENCH_QUEUE_ITEM_SIZE                   (sizeof(rt_uint32_t))

/*-----------------------------------------------------------*/

/* Hardware register definitions, so that the benchmark does not depend on
 * any vendor library. */
#define TIM2_BASE                               (0x40000000UL)
#define TIM4_BASE                               (0x40000800UL)
#define RCC_BASE                                (0x40023800UL)

#define RCC_APB1ENR                             (*((volatile rt_uint32_t *)(RCC_BASE + 0x40UL)))
#define RCC_APB1ENR_TIM2                        (1UL << 0UL)
#define RCC_APB1ENR_TIM4                        (1UL << 2UL)

#define TIM2_CR1                                (*((volatile rt_uint32_t *)(TIM2_BASE + 0x00UL)))
#define TIM2_PSC                                (*((volatile rt_uint32_t *)(TIM2_BASE + 0x28UL)))
#define TIM2_ARR                                (*((volatile rt_uint32_t *)(TIM2_BASE + 0x2CUL)))
#define TIM2_CNT                                (*((volatile rt_uint32_t *)(TIM2_BASE + 0x24UL)))

#define TIM4_CR1                                (*((volatile rt_uint32_t *)(TIM4_BASE + 0x00UL)))
#define TIM4_DIER                               (*((volatile rt_uint32_t *)(TIM4_BASE + 0x0CUL)))
#define TIM4_SR                                 (*((volatile rt_uint32_t *)(TIM4_BASE + 0x10UL)))
#define TIM4_PSC                                (*((volatile rt_uint32_t *)(TIM4_BASE + 0x28UL)))
#define TIM4_ARR                                (*((volatile rt_uint32_t *)(TIM4_BASE + 0x2CUL)))

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
    RCC_APB1ENR |= RCC_APB1ENR_TIM2;

    /* Let TIM2 count up through its full 32 bit range at half the CPU
     * clock frequency, without any prescaling. */
    TIM2_CR1 = 0x0UL;
    TIM2_PSC = 0x0UL;
    TIM2_ARR = 0xFFFFFFFFUL;
    TIM2_CR1 |= TIM_CR1_CEN;
}

void Bench_IntSetup(void)
{
    /* Connect the physical TIM4 interrupt to vector 2 of this virtual
     * machine. */
    RVM_ASSERT(RVM_Hyp_Vct_Phys(30U, 2U) == 0U);

    /* Enable the physical interrupt at the lowest priority. */
    RVM_ASSERT(RVM_A7M_Int_Local_Mod(KFN_INT_LOCAL_MOD, 30U, RVM_A7M_KFN_INT_LOCAL_MOD_PRIO_SET, 0xFFU) == 0U);
    RVM_ASSERT(RVM_A7M_Int_Local_Mod(KFN_INT_LOCAL_MOD, 30U, RVM_A7M_KFN_INT_LOCAL_MOD_STATE_SET, 1U) == 0U);

    /* Register the handler that is implemented by the benchmark itself. */
    RVM_Virt_Vct_Reg(2U, Bench_TestIntHandler, RVM_VCT_MASKABLE);

    /* Enable the TIM4 clock, and let the counter count down from the
     * reload value, which is reached every 36000 CPU cycles - the timer
     * counts at half the CPU clock, so the reload is half of that, the
     * same pacing the FreeRTOS benchmark uses on this chip. */
    RCC_APB1ENR |= RCC_APB1ENR_TIM4;
    TIM4_CR1 = TIM_CR1_DIR;
    TIM4_PSC = 0x0UL;
    TIM4_ARR = 18000U;
    TIM4_SR &= ~TIM_SR_UIF;
    TIM4_DIER |= TIM_DIER_UIE;
    TIM4_CR1 |= TIM_CR1_CEN;
}

void Bench_IntDisable(void)
{
    /* Disable the physical interrupt. */
    RVM_ASSERT(RVM_A7M_Int_Local_Mod(KFN_INT_LOCAL_MOD, 30U, RVM_A7M_KFN_INT_LOCAL_MOD_STATE_SET, 0U) == 0U);

    /* Reverse the vector registration. */
    RVM_Virt_Vct_Reg(2U, RVM_NULL, RVM_VCT_MASKABLE);
}

#endif /* RTT_TEST_STM32F429ZI_RVM_H */
