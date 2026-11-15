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
*                                        CH32V307VC TESTBENCH
*
* File      : uo2_test_ch32v307vc_rvm.h
* Version   : V1.00.00
* For      : CH32V307VCT6 @ 144MHz, RVM hypervisor guest
*********************************************************************************************************
*/

/*
*********************************************************************************************************
* Note(s)  : (1) Benchmark archive (CH32V307VCT6 @ 144MHz, two consecutive
*                resets gave identical rows):
*
* RISC-V Embedded GCC 13.2 -O3
* uC/OS-II V2.93 on RVM - Standard benchmark test
* ====================================================
* Test (number in CPU cycles)        : AVG / MAX / MIN
*
* Yield                              : (not measured)
* Notification                       :  579 /   611 /  579
* Semaphore                          :  559 /   570 /  559
* Message queue                      :  571 /   581 /  571
* Memory allocation/free pair        : 1337 /  1394 / 1332
* ISR Notification                   : 2591 /  3339 /  583
* ISR Semaphore                      :  570 /   584 /  570
* ISR Message queue                  :  579 /   593 /  577
* ISR RVM activation relay           : 3012 /  4099 / 2275
*********************************************************************************************************
*/

#ifndef UO2_TEST_CH32V307VC_RVM_H

#define  UO2_TEST_CH32V307VC_RVM_H

#include "rvm.h"

#define  RVM_TEST_VIRTUAL_INC
#include "rvm_test.h"
#undef RVM_TEST_VIRTUAL_INC

#define  BENCH_PRINT_STR(String)                  RVM_DBG_S(String)
#define  BENCH_PRINT_INT(Number)                  RVM_DBG_I(Number)

#define  BENCH_TIME_GET()                         ((INT16U)(TIM1_CNT))
#define  BENCH_KERNEL_TS_GET()                    (START)
#define  BENCH_TASK_STK_SIZE                      (256u)
#define  BENCH_MEM_BLK_QTY                        (8u)
#define  BENCH_MEM_BLK_SIZE                       (32u)
#define  BENCH_SEM_MAX_COUNT                      (8u)
#define  BENCH_QUEUE_LENGTH                       (8u)
#define  BENCH_MSG_SIZE                           (sizeof(INT32U))

#define  TIM1_BASE                                (0x40012C00uL)
#define  TIM4_BASE                                (0x40000800uL)
#define  RCC_BASE                                 (0x40021000uL)

#define  RCC_APB2PCENR                            (*((volatile INT32U *)(RCC_BASE + 0x18uL)))
#define  RCC_APB2PCENR_TIM1EN                     (1uL << 11uL)
#define  RCC_APB1PCENR                            (*((volatile INT32U *)(RCC_BASE + 0x1CuL)))
#define  RCC_APB1PCENR_TIM4EN                     (1uL << 2uL)

#define  TIM1_CTLR1                               (*((volatile INT16U *)(TIM1_BASE + 0x00uL)))
#define  TIM1_SMCFGR                              (*((volatile INT16U *)(TIM1_BASE + 0x08uL)))
#define  TIM1_CNT                                 (*((volatile INT16U *)(TIM1_BASE + 0x24uL)))

#define  TIM4_CTLR1                               (*((volatile INT16U *)(TIM4_BASE + 0x00uL)))
#define  TIM4_DMAINTENR                           (*((volatile INT16U *)(TIM4_BASE + 0x0CuL)))
#define  TIM4_INTFR                               (*((volatile INT16U *)(TIM4_BASE + 0x10uL)))
#define  TIM4_PSC                                 (*((volatile INT16U *)(TIM4_BASE + 0x28uL)))
#define  TIM4_ATRLR                               (*((volatile INT16U *)(TIM4_BASE + 0x2CuL)))

#define  TIM_CTLR1_CEN                            (1uL << 0uL)
#define  TIM_CTLR1_DIR                            (1uL << 4uL)
#define  TIM_INTFR_UIF                            (1uL << 0uL)
#define  TIM_DMAINTENR_UIE                        (1uL << 0uL)

void Bench_TestIntHandler(void);
void Bench_TimerSetup(void);
void Bench_TestIntSetup(void);
void Bench_TestIntDisable(void);

void Bench_TimerSetup(void)
{
    RCC_APB2PCENR |= RCC_APB2PCENR_TIM1EN;
    TIM1_CTLR1  = 0x0u;
    TIM1_SMCFGR = 0x0u;
    TIM1_CTLR1 |= TIM_CTLR1_CEN;
}

void Bench_TestIntSetup(void)
{
    RVM_ASSERT(RVM_Hyp_Vct_Phys(46u, 2u) == 0u);
    RVM_ASSERT(RVM_RV32P_Int_Local_Mod(KFN_INT_LOCAL_MOD, 46u, RVM_RV32P_KFN_INT_LOCAL_MOD_PRIO_SET, 0xFFu) == 0u);
    RVM_ASSERT(RVM_RV32P_Int_Local_Mod(KFN_INT_LOCAL_MOD, 46u, RVM_RV32P_KFN_INT_LOCAL_MOD_STATE_SET, 1u) == 0u);
    RVM_Virt_Vct_Reg(2u, Bench_TestIntHandler, RVM_VCT_MASKABLE);
    RCC_APB1PCENR |= RCC_APB1PCENR_TIM4EN;
    TIM4_CTLR1 = TIM_CTLR1_DIR;
    TIM4_PSC = 0x0u;
    TIM4_ATRLR = 7200u;
    TIM4_INTFR &= (INT16U)~TIM_INTFR_UIF;
    TIM4_DMAINTENR |= TIM_DMAINTENR_UIE;
    TIM4_CTLR1 |= TIM_CTLR1_CEN;
}

void Bench_TestIntDisable(void)
{
    RVM_ASSERT(RVM_RV32P_Int_Local_Mod(KFN_INT_LOCAL_MOD, 46u, RVM_RV32P_KFN_INT_LOCAL_MOD_STATE_SET, 0u) == 0u);
    RVM_Virt_Vct_Reg(2u, RVM_NULL, RVM_VCT_MASKABLE);
}

#endif /* UO2_TEST_CH32V307VC_RVM_H */
