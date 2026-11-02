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
*                                         STM32H743ZI TESTBENCH
*
* File      : uo2_test_stm32h743zi_rvm.h
* Version   : V1.00.00
* For      : STM32H743ZIT6 @ 400MHz, RVM hypervisor guest
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
*                not measured here (the kernel's OS_Sched always picks the
*                highest ready task, there is no same-priority round-robin to
*                exercise).  The yield row in the README tables is left empty.
*
*            (3) Benchmark archive (STM32H743ZIT6 @ 400MHz, stack-switching fast
*                yield path, no coprocessor, two consecutive resets gave
*                identical rows):
*
* GNU Arm Embedded Toolchain 13.2-2024 -O3
* uC/OS-II V2.93 on RVM - Standard benchmark test
* ====================================================
* Test (number in CPU cycles)        : AVG / MAX / MIN
*
* Yield                              : (not measured - see Note 2)
* Notification                       :  503 /  1972 /  492
* Semaphore                          :  460 /  1764 /  444
* Message queue                      :  466 /  1812 /  440
* Memory allocation/free pair        :  909 /  2168 /  908
* ISR Notification                   : 1526 / 63532 /  540
* ISR Semaphore                      :  567 /  1864 /  532
* ISR Message queue                  :  586 /   652 /  536
* ISR RVM activation relay           : 2190 /  3180 / 1800
*
*                The yield row is empty by design.  The ISR Notification average
*                sits above the other ISR items because the first interrupt phase
*                includes the timer startup latency.
*********************************************************************************************************
*/

#ifndef UO2_TEST_STM32H743ZI_RVM_H

#define  UO2_TEST_STM32H743ZI_RVM_H

#include "rvm.h"

#define  RVM_TEST_VIRTUAL_INC
#include "rvm_test.h"
#undef RVM_TEST_VIRTUAL_INC

#define  BENCH_PRINT_STR(String)                  RVM_DBG_S(String)
#define  BENCH_PRINT_INT(Number)                  RVM_DBG_I(Number)

#define  BENCH_TIME_GET()                         ((INT16U)(TIM2_CNT << 1u))
#define  BENCH_KERNEL_TS_GET()                    (START)
#define  BENCH_TASK_STK_SIZE                      (256u)
#define  BENCH_MEM_BLK_QTY                        (8u)
#define  BENCH_MEM_BLK_SIZE                       (32u)
#define  BENCH_SEM_MAX_COUNT                      (8u)
#define  BENCH_QUEUE_LENGTH                       (8u)
#define  BENCH_MSG_SIZE                           (sizeof(INT32U))

#define  TIM2_BASE                                (0x40000000UL)
#define  TIM4_BASE                                (0x40000800UL)
#define  RCC_BASE                                 (0x58024400UL)

#define  RCC_APB1ENR                              (* ((volatile INT32U *) (RCC_BASE + 0xE8UL)))
#define  RCC_APB1ENR_TIM2                         (1UL << 0UL)
#define  RCC_APB1ENR_TIM4                         (1UL << 2UL)

#define  TIM2_CR1                                 (* ((volatile INT32U *) (TIM2_BASE + 0x00UL)))
#define  TIM2_PSC                                 (* ((volatile INT32U *) (TIM2_BASE + 0x28UL)))
#define  TIM2_ARR                                 (* ((volatile INT32U *) (TIM2_BASE + 0x2CUL)))
#define  TIM2_CNT                                 (* ((volatile INT32U *) (TIM2_BASE + 0x24UL)))

#define  TIM4_CR1                                 (* ((volatile INT32U *) (TIM4_BASE + 0x00UL)))
#define  TIM4_DIER                                (* ((volatile INT32U *) (TIM4_BASE + 0x0CUL)))
#define  TIM4_SR                                  (* ((volatile INT32U *) (TIM4_BASE + 0x10UL)))
#define  TIM4_PSC                                 (* ((volatile INT32U *) (TIM4_BASE + 0x28UL)))
#define  TIM4_ARR                                 (* ((volatile INT32U *) (TIM4_BASE + 0x2CUL)))

#define  TIM_CR1_CEN                              (1UL << 0UL)
#define  TIM_CR1_DIR                              (1UL << 4UL)
#define  TIM_SR_UIF                               (1UL << 0UL)
#define  TIM_DIER_UIE                             (1UL << 0UL)

void Bench_TestIntHandler(void);
void Bench_TimerSetup(void);
void Bench_TestIntSetup(void);
void Bench_TestIntDisable(void);

void Bench_TimerSetup(void)
{
    RCC_APB1ENR |= RCC_APB1ENR_TIM2;
    TIM2_CR1 = 0x0UL;
    TIM2_PSC = 0x0UL;
    TIM2_ARR = 0xFFFFFFFFUL;
    TIM2_CR1 |= TIM_CR1_CEN;
}

void Bench_TestIntSetup(void)
{
    RVM_ASSERT(RVM_Hyp_Vct_Phys(30u, 2u) == 0u);
    RVM_ASSERT(RVM_A7M_Int_Local_Mod(KFN_INT_LOCAL_MOD, 30u, RVM_A7M_KFN_INT_LOCAL_MOD_PRIO_SET, 0xFFu) == 0u);
    RVM_ASSERT(RVM_A7M_Int_Local_Mod(KFN_INT_LOCAL_MOD, 30u, RVM_A7M_KFN_INT_LOCAL_MOD_STATE_SET, 1u) == 0u);
    RVM_Virt_Vct_Reg(2u, Bench_TestIntHandler, RVM_VCT_MASKABLE);
    RCC_APB1ENR |= RCC_APB1ENR_TIM4;
    TIM4_CR1 = TIM_CR1_DIR;
    TIM4_PSC = 0x0UL;
    TIM4_ARR = 20000u;
    TIM4_SR &= ~TIM_SR_UIF;
    TIM4_DIER |= TIM_DIER_UIE;
    TIM4_CR1 |= TIM_CR1_CEN;
}

void Bench_TestIntDisable(void)
{
    RVM_ASSERT(RVM_A7M_Int_Local_Mod(KFN_INT_LOCAL_MOD, 30u, RVM_A7M_KFN_INT_LOCAL_MOD_STATE_SET, 0u) == 0u);
    RVM_Virt_Vct_Reg(2u, RVM_NULL, RVM_VCT_MASKABLE);
}

#endif /* UO2_TEST_STM32H743ZI_RVM_H */
