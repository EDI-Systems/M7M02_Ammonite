/*
 * KLite V5.2.1 benchmark testbench for the STM32H743ZI, running as a
 * guest of the RVM hypervisor.
 * Copyright (C) 2026 Evo-Devo Instrum. All rights reserved.
 *
 * This file is NOT part of the KLite distribution.  It belongs to the
 * KLite-on-RVM test suite and is released under the Unlicense; refer
 * to the LICENSE file for details.  The KLite kernel itself is
 * copyright jiangxiaogang and is not modified by this file.
 *
 * This file holds all the hardware dependent parts of the KLite
 * benchmark: the counter the tests are timed with, and the interrupt
 * source that drives the interrupt tests.  The kernel's own board glue
 * lives in the port's cpu.c and needs none of it, so the RVM project
 * generator only selects this file - through the generated "klt_test.h"
 * header - in benchmark builds.
 *
 * Benchmark archive (STM32H743ZIT6 rev.Y @ 400MHz, VOS scale 1,
 * integer-only port, two consecutive resets gave identical rows,
 * image 11.65 kB Flash / 20.27 kB SRAM including the 20 kB kernel heap):
 *
 * arm-none-eabi-gcc 13.2.1 -O3
 * ====================================================
 * Test (CPU cycles)                 : AVG / MAX / MIN
 *
 * Yield                              :  291 / 1496 /  284
 * Notification                       :  428 / 1612 /  424
 * Semaphore                          :  379 /  448 /  356
 * Message queue (4B)                 : 1402 / 2768 / 1392
 * Memory (Malloc/Free 8x32B)         : 2751 / 4100 / 2748
 * ISR Notification                   : 1059 / 1256 /  980
 * ISR Semaphore                      :  996 / 2156 /  928
 * ISR Message queue (4B)             : 1964 / 2032 / 1888
 * ISR RVM relay                      : 1883 / 2180 / 1740
 *
 * Notes: the notification slot is measured with the auto-reset event
 * (event_set/event_wait), KLite's closest counterpart of a direct task
 * notification; the message queue is the opt/queue component with its
 * mutex-backed node pool.  The yield sits far below the other guests
 * because KLite's scheduler is a plain 8-list bitmap picker and its
 * kernel has no IPC bookkeeping on the yield path.
 *
 * FPU / no-FPU dual configuration verified on the same board (all 9+1
 * items, no faults, no reentries in both): with the rvp Coprocessor set
 * to FPV5_DP in both the chip row and the Virt1 section (kernel
 * RME_COP_NUM=1, guest makefile gains -mfpu=fpv5-d16 with the soft-float
 * ABI kept), the same table reads: Yield 288/1636/288, Notification
 * 421/1736/400, Semaphore 369/1732/360, Message queue 1397/2960/1384,
 * Memory 2751/4192/2748, ISR Notification 1102/2456/1024, ISR Semaphore
 * 1072/1168/996, ISR Message queue 2024/3364/1952, ISR relay
 * 1885/3112/1784 - the deltas against the table above sit on the
 * vct-thread items (the kernel's per-thread-switch Cop swap) and stay
 * within tens of cycles; the thread-level fast path is untouched.  The
 * port's #if defined(__ARM_FP) #error in cpu_asm_gcc.s rejects a
 * hard-float ABI build at compile time instead of silently corrupting
 * the S registers across a switch (verified: soft passes, hard errors).
 */

#ifndef KLT_TEST_STM32H743ZI_RVM_H
#define KLT_TEST_STM32H743ZI_RVM_H

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

/* The timestamp latched by the kernel side of the benchmark when it took
 * the very interrupt that is being relayed to us - the difference between
 * this and the counter value at our own handler entry is the RVM activation
 * relay latency. */
#define BENCH_KERNEL_TS_GET()                   (START)

/* The size of the blocks that the memory test allocates and frees, and
 * the maximum count of the semaphore, and the depth and the item size of
 * the message queue, that are used by the benchmark. */
#define BENCH_MEM_BLK_SIZE                      ((uint32_t)32)
#define BENCH_MEM_BLK_QTY                       (8u)
#define BENCH_MAX_SEMAPHORE_COUNT               (8u)
#define BENCH_QUEUE_LENGTH                      (8u)
#define BENCH_QUEUE_ITEM_SIZE                   (sizeof(uint32_t))

/*-----------------------------------------------------------*/

/* Hardware register definitions, so that the benchmark does not depend on
 * any vendor library. */
#define TIM2_BASE                               (0x40000000UL)
#define TIM4_BASE                               (0x40000800UL)
#define RCC_BASE                                (0x58024400UL)

#define RCC_APB1ENR                             (*((volatile uint32_t *)(RCC_BASE + 0xE8UL)))
#define RCC_APB1ENR_TIM2                        (1UL << 0UL)
#define RCC_APB1ENR_TIM4                        (1UL << 2UL)

#define TIM2_CR1                                (*((volatile uint32_t *)(TIM2_BASE + 0x00UL)))
#define TIM2_PSC                                (*((volatile uint32_t *)(TIM2_BASE + 0x28UL)))
#define TIM2_ARR                                (*((volatile uint32_t *)(TIM2_BASE + 0x2CUL)))
#define TIM2_CNT                                (*((volatile uint32_t *)(TIM2_BASE + 0x24UL)))

#define TIM4_CR1                                (*((volatile uint32_t *)(TIM4_BASE + 0x00UL)))
#define TIM4_DIER                               (*((volatile uint32_t *)(TIM4_BASE + 0x0CUL)))
#define TIM4_SR                                 (*((volatile uint32_t *)(TIM4_BASE + 0x10UL)))
#define TIM4_PSC                                (*((volatile uint32_t *)(TIM4_BASE + 0x28UL)))
#define TIM4_ARR                                (*((volatile uint32_t *)(TIM4_BASE + 0x2CUL)))

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
     * reload value, which is reached every 40000 CPU cycles - the timer
     * counts at half the CPU clock, so the reload is half of that, the
     * same pacing the other guest benchmarks use on this chip. */
    RCC_APB1ENR |= RCC_APB1ENR_TIM4;
    TIM4_CR1 = TIM_CR1_DIR;
    TIM4_PSC = 0x0UL;
    TIM4_ARR = 20000U;
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

/*-----------------------------------------------------------*/

/* The size of the stack allocated to each benchmark thread, measured in
 * bytes - KLite's thread_create() takes bytes, and 0 would mean its
 * 1024 byte default. */
#define BENCH_TASK_STK_SIZE                     ((uint32_t)512)

/* The kernel heap the benchmark boots the kernel with: two benchmark
 * threads and the idle thread with their stacks and tcbs, the queue node
 * pool, and the memory test's transient blocks all come from here. */
#define BENCH_HEAP_SIZE                         ((uint32_t)(20*1024))

/* The two priorities the benchmark threads run at.  KLite counts higher
 * priority numbers as higher priorities. */
#define BENCH_TASK_PRIO_HIGH                    (THREAD_PRIORITY_HIGH)
#define BENCH_TASK_PRIO_NORM                    (THREAD_PRIORITY_NORMAL)

/* The benchmark is timed with TIM2, which counts up at half the CPU clock
 * frequency (the APB1 timer clock when the APB1 prescaler is not 1), so
 * the counter value is scaled back up to CPU cycles. */
#define BENCH_TIME_GET()                        ((uint32_t)(TIM2_CNT << 1UL))

#endif /* KLT_TEST_STM32H743ZI_RVM_H */
