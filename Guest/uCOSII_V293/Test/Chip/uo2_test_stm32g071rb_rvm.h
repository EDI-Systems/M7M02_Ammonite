/*
*********************************************************************************************************
*                                                uC/OS-II
*                                          The Real-Time Kernel
*
*                                       RVM Benchmark Test Suite
*
*                               Copyright (C) 2026 Evo-Devo Instrum.
*                                       All rights reserved.
*
*                This file is NOT part of the uC/OS-II distribution.  It belongs
*                to the uC/OS-II-on-RVM test suite and is released under the
*                Unlicense; refer to the LICENSE file for details.  The uC/OS-II
*                kernel is copyright Weston Embedded and is not modified by
*                this file.
*
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                         STM32G071RB TESTBENCH
*
* File      : uo2_test_stm32g071rb_rvm.h
* Version   : V1.00.00
* For      : STM32G071RBT6 @ 64MHz, RVM hypervisor guest
*********************************************************************************************************
*/

/*
*********************************************************************************************************
* Note(s)  : (1) This file holds all the hardware dependent parts of the uC/OS-II
*                benchmark: the counter the tests are timed with, and the interrupt
*                source that drives the interrupt tests.  The RVM project generator
*                selects it through the generated "uo2_test.h" header.
*
*            (2) uC/OS-II enforces one task per priority, so the yield test is
*                replaced by a priority-exchange handoff and the notification test
*                uses the one-word mailbox (OSMboxPost/OSMboxPend).
*
*            (3) Benchmark archive (STM32G071RBT6 @ 64MHz, stack-switching fast
*                yield path, no coprocessor, two consecutive resets gave
*                identical rows):
*
* GNU Arm Embedded Toolchain 13.2-2024 -O3
* uC/OS-II V2.93 on RVM - Standard benchmark test
* ====================================================
* Test (number in CPU cycles)        : AVG / MAX / MIN
*
* Yield                              : (not measured - see Note 2)
* Notification                       : 1111 /  3469 / 1108
* Semaphore                          : 1090 /  3447 / 1086
* Message queue                      : 1108 /  7935 / 1104
* Memory allocation/free pair        : 2876 /  5230 / 2865
* ISR Notification                   : 5112 / 62017 / 1119
* ISR Semaphore                      : 1097 /  3456 / 1092
* ISR Message queue                  : 1117 /  3475 / 1113
* ISR RVM activation relay           : 4029 /  7748 / 3300
*
*                The yield row is empty by design: uC/OS-II enforces one task
*                per priority and has no same-priority round-robin to exercise,
*                so a yield test would measure nothing the notification test
*                does not already cover.  The ISR Notification average sits
*                above the other ISR items because the first interrupt phase
*                includes the timer startup latency.
*********************************************************************************************************
*/

#ifndef UO2_TEST_STM32G071RB_RVM_H

#define  UO2_TEST_STM32G071RB_RVM_H

/* RVM hypervisor interface includes. */
#include "rvm.h"

/* Kernel test interface includes - the kernel side of the benchmark
 * latches the timestamp of its own interrupt handler entry into the shared
 * memory this virtual machine can read back, so the activation relay
 * latency can be measured as well. */

#define  RVM_TEST_VIRTUAL_INC
#include "rvm_test.h"
#undef RVM_TEST_VIRTUAL_INC

/*
*********************************************************************************************************
*                                            BENCHMARK PRINT MACROS
*
* Note(s)  : (1) The uC/OS-II kernel itself has no printing facility, so the benchmark results
*                are printed through the RVM debug printing functions.
*********************************************************************************************************
*/

#define  BENCH_PRINT_STR(String)                  RVM_DBG_S(String)
#define  BENCH_PRINT_INT(Number)                  RVM_DBG_I(Number)

/* The benchmark is timed with TIM2, which counts up at the CPU clock
 * frequency and is only 16 bits wide on this chip. */

#define  BENCH_TIME_GET()                         ((INT16U)(TIM2_CNT))

/* The timestamp latched by the kernel side of the benchmark when it took
 * the very interrupt that is being relayed to us. */

#define  BENCH_KERNEL_TS_GET()                    (START)

/* The size of the stack allocated to each benchmark task, measured in
 * OS_STK words. */

#define  BENCH_TASK_STK_SIZE                      (128u)

/* The memory partition that the memory test gets its blocks from. */

#define  BENCH_MEM_BLK_QTY                        (8u)

#define  BENCH_MEM_BLK_SIZE                       (32u)

/* The maximum count of the counting semaphore, and the length and the
 * item size of the queue. */

#define  BENCH_SEM_MAX_COUNT                      (8u)

#define  BENCH_QUEUE_LENGTH                       (8u)

#define  BENCH_MSG_SIZE                           (sizeof(INT32U))

/* Hardware register definitions, so that the benchmark does not depend on
 * any vendor library. */

#define  TIM2_BASE                                (0x40000000UL)
#define  TIM3_BASE                                (0x40000400UL)
#define  RCC_BASE                                 (0x40021000UL)

#define  RCC_APBENR1                              (* ((volatile INT32U *) (RCC_BASE + 0x3CUL)))
#define  RCC_APBENR1_TIM2                         (1UL << 0UL)
#define  RCC_APBENR1_TIM3                         (1UL << 1UL)

#define  TIM2_CR1                                 (* ((volatile INT32U *) (TIM2_BASE + 0x00UL)))
#define  TIM2_PSC                                 (* ((volatile INT32U *) (TIM2_BASE + 0x28UL)))
#define  TIM2_ARR                                 (* ((volatile INT32U *) (TIM2_BASE + 0x2CUL)))
#define  TIM2_CNT                                 (* ((volatile INT32U *) (TIM2_BASE + 0x24UL)))

#define  TIM3_CR1                                 (* ((volatile INT32U *) (TIM3_BASE + 0x00UL)))
#define  TIM3_DIER                                (* ((volatile INT32U *) (TIM3_BASE + 0x0CUL)))
#define  TIM3_SR                                  (* ((volatile INT32U *) (TIM3_BASE + 0x10UL)))
#define  TIM3_PSC                                 (* ((volatile INT32U *) (TIM3_BASE + 0x28UL)))
#define  TIM3_ARR                                 (* ((volatile INT32U *) (TIM3_BASE + 0x2CUL)))

#define  TIM_CR1_CEN                              (1UL << 0UL)
#define  TIM_CR1_DIR                              (1UL << 4UL)
#define  TIM_SR_UIF                               (1UL << 0UL)
#define  TIM_DIER_UIE                             (1UL << 0UL)

/*
*********************************************************************************************************
*                              PERIODIC TEST INTERRUPT HANDLER
*********************************************************************************************************
*/

void Bench_TestIntHandler(void);

/*
*********************************************************************************************************
*                                SETUP THE BENCHMARK TIMER
*********************************************************************************************************
*/

void Bench_TimerSetup(void);

/*
*********************************************************************************************************
*                                 SETUP THE TEST INTERRUPT
*********************************************************************************************************
*/

void Bench_TestIntSetup(void);

/*
*********************************************************************************************************
*                                DISABLE THE TEST INTERRUPT
*********************************************************************************************************
*/

void Bench_TestIntDisable(void);

/*
*********************************************************************************************************
*                                SETUP THE BENCHMARK TIMER
*********************************************************************************************************
*/

void Bench_TimerSetup(void)
{
    RCC_APBENR1 |= RCC_APBENR1_TIM2;

    TIM2_CR1 = 0x0UL;
    TIM2_PSC = 0x0UL;
    TIM2_ARR = 0xFFFFUL;
    TIM2_CR1 |= TIM_CR1_CEN;
}

/*
*********************************************************************************************************
*                                 SETUP THE TEST INTERRUPT
*********************************************************************************************************
*/

void Bench_TestIntSetup(void)
{
    /* Connect the physical TIM3 interrupt to vector 2 of this virtual machine. */
    RVM_ASSERT(RVM_Hyp_Vct_Phys(16u, 2u) == 0u);

    /* Enable the physical interrupt at the lowest priority. */
    RVM_ASSERT(RVM_A6M_Int_Local_Mod(KFN_INT_LOCAL_MOD, 16u, RVM_A6M_KFN_INT_LOCAL_MOD_PRIO_SET, 0xFFu) == 0u);
    RVM_ASSERT(RVM_A6M_Int_Local_Mod(KFN_INT_LOCAL_MOD, 16u, RVM_A6M_KFN_INT_LOCAL_MOD_STATE_SET, 1u) == 0u);

    /* Register the handler that is implemented by the benchmark itself. */
    RVM_Virt_Vct_Reg(2u, Bench_TestIntHandler, RVM_VCT_MASKABLE);

    /* Enable the TIM3 clock, and let the counter count down from the
     * reload value, which is reached every 12800 CPU cycles. */
    RCC_APBENR1 |= RCC_APBENR1_TIM3;
    TIM3_CR1 = TIM_CR1_DIR;
    TIM3_PSC = 0x0UL;
    TIM3_ARR = 12800u;
    TIM3_SR &= ~TIM_SR_UIF;
    TIM3_DIER |= TIM_DIER_UIE;
    TIM3_CR1 |= TIM_CR1_CEN;
}

/*
*********************************************************************************************************
*                                DISABLE THE TEST INTERRUPT
*********************************************************************************************************
*/

void Bench_TestIntDisable(void)
{
    RVM_ASSERT(RVM_A6M_Int_Local_Mod(KFN_INT_LOCAL_MOD, 16u, RVM_A6M_KFN_INT_LOCAL_MOD_STATE_SET, 0u) == 0u);
    RVM_Virt_Vct_Reg(2u, RVM_NULL, RVM_VCT_MASKABLE);
}

#endif /* UO2_TEST_STM32G071RB_RVM_H */
