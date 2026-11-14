/*
    CHB benchmark testbench for the STM32H743ZI, running as a guest of RVM.
    Copyright (C) 2026 Evo-Devo Instrum. All rights reserved.

    This file is NOT part of the ChibiOS distribution.  It belongs to the
    ChibiOS-on-RVM test suite and is released under the Unlicense; refer to
    the LICENSE file for details.

    Benchmark archive (NUCLEO-H743ZI, STM32H743ZIT6, RVM, CHB 21.11 LTS,
    GNU Arm 13.2.1 -O3, 100 Hz tick, FPV5_DP hard-float, direct fast path,
    56-word extended frames (FPU context test PASS):
    ====================================================
    Test (number in CPU cycles)        : AVG / MAX / MIN
    Yield                              :  161 / 2106 /  160
    Notification                       :  443 / 4256 /  404
    Semaphore                          :  371 / 3116 /  360
    Message queue                      :  581 / 4520 /  556
    Memory allocation/free pair        :  345 /  825 /  345
    ISR Notification                   : 1229 / 1736 / 1132
    ISR Semaphore                      : 1189 / 1344 / 1084
    ISR Message queue                  : 1319 / 1484 / 1200
    ISR RVM activation relay           : 2393 / 3592 / 1672

    The FPU build also passes with CHB_FAST_PATH=0 (all-Vct): Yield 678,
    Notification 1478, Semaphore 1364, Message queue 1535, Memory 345,
    ISR Notification 1171, ISR Semaphore 1083, ISR Message queue 1161,
    relay 2430, FPU context PASS.

    IAR EWARM 9.2.2, FPV5_DP, hard-float, same fast path:
    ====================================================
    Test (number in CPU cycles)        : AVG / MAX / MIN
    Yield                              :  162 /  530 /  162
    Notification                       :  475 / 4652 /  456
    Semaphore                          :  386 / 4656 /  376
    Message queue                      :  588 / 4776 /  572
    Memory allocation/free pair        :  324 /  852 /  319
    ISR Notification                   : 1252 / 1416 / 1196
    ISR Semaphore                      : 1234 / 1316 / 1196
    ISR Message queue                  : 1350 / 4248 / 1284
    ISR RVM activation relay           : 2706 / 2864 / 2596

    Keil MDK 5 (ARMCLANG 6.23), FPV5_DP, hard-float, same fast path:
    ====================================================
    Test (number in CPU cycles)        : AVG / MAX / MIN
    Yield                              :  177 / 1510 /  170
    Notification                       :  429 /  616 /  424
    Semaphore                          :  380 / 4196 /  360
    Message queue                      :  549 / 4264 /  544
    Memory allocation/free pair        :  358 /  829 /  357
    ISR Notification                   : 1146 / 3752 / 1060
    ISR Semaphore                      : 1183 / 1672 / 1076
    ISR Message queue                  : 1271 / 1388 / 1168
    ISR RVM activation relay           : 2368 / 4152 / 2232
    FPU context PASS in both IAR and Keil builds.

    2026-11-12: the benchmark now stops TIM4 and unregisters the virtual
    vector before printing; the four ISR rows are emitted as one block after
    the interrupt phase ends (FRT/UO3/RTE/ZPR/NTX convention).  The archived
    numbers above were taken with row-by-row printing; statistics were closed
    before each print in both modes, so the values are directly comparable.
*/

#ifndef CHB_TEST_STM32H743ZI_RVM_H
#define CHB_TEST_STM32H743ZI_RVM_H

#include "rvm.h"

#define RVM_TEST_VIRTUAL_INC
#include "rvm_test.h"
#undef RVM_TEST_VIRTUAL_INC

/* TIM2 counts at half the CPU clock, so shift it back to CPU cycles. */
#define CHB_TEST_CNT_READ()        ( ( TIM2_CNT ) << 1 )
#define CHB_TEST_CNT_DIFF(a, b)  ((uint32_t)(a) - (uint32_t)(b))
#define CHB_TEST_KERNEL_STAMP()    ( START )

#define TIM2_BASE                  ( 0x40000000UL )
#define TIM4_BASE                  ( 0x40000800UL )
#define RCC_BASE                   ( 0x58024400UL )

#define RCC_APB1ENR                ( * ( ( volatile uint32_t * ) ( RCC_BASE + 0xE8UL ) ) )
#define RCC_APB1ENR_TIM2EN         ( 1UL << 0UL )
#define RCC_APB1ENR_TIM4EN         ( 1UL << 2UL )

#define TIM2_CR1                   ( * ( ( volatile uint32_t * ) ( TIM2_BASE + 0x00UL ) ) )
#define TIM2_PSC                   ( * ( ( volatile uint32_t * ) ( TIM2_BASE + 0x28UL ) ) )
#define TIM2_ARR                   ( * ( ( volatile uint32_t * ) ( TIM2_BASE + 0x2cUL ) ) )
#define TIM2_CNT                   ( * ( ( volatile uint32_t * ) ( TIM2_BASE + 0x24UL ) ) )

#define TIM4_CR1                   ( * ( ( volatile uint32_t * ) ( TIM4_BASE + 0x00UL ) ) )
#define TIM4_DIER                  ( * ( ( volatile uint32_t * ) ( TIM4_BASE + 0x0cUL ) ) )
#define TIM4_SR                    ( * ( ( volatile uint32_t * ) ( TIM4_BASE + 0x10UL ) ) )
#define TIM4_PSC                   ( * ( ( volatile uint32_t * ) ( TIM4_BASE + 0x28UL ) ) )
#define TIM4_ARR                   ( * ( ( volatile uint32_t * ) ( TIM4_BASE + 0x2cUL ) ) )

#define TIM_CR1_CEN                ( 1UL << 0UL )
#define TIM_CR1_DIR                ( 1UL << 4UL )
#define TIM_SR_UIF                 ( 1UL << 0UL )
#define TIM_DIER_UIE               ( 1UL << 0UL )

void chb_test_int_handler(void);

void chb_test_timer_init(void) {
  RCC_APB1ENR |= RCC_APB1ENR_TIM2EN;
  TIM2_CR1 = 0U;
  TIM2_PSC = 0U;
  TIM2_ARR = 0xffffffffUL;
  TIM2_CR1 |= TIM_CR1_CEN;
}

static uint32_t chb_test_int_mapped;

void chb_test_int_init(void (*handler)(void)) {
  if (chb_test_int_mapped == 0U) {
    RVM_ASSERT(RVM_Hyp_Vct_Phys(30U, 2U) == 0U);
    RVM_ASSERT(RVM_A7M_Int_Local_Mod(KFN_INT_LOCAL_MOD, 30U,
                                     RVM_A7M_KFN_INT_LOCAL_MOD_STATE_SET, 1U) == 0U);
    RVM_ASSERT(RVM_A7M_Int_Local_Mod(KFN_INT_LOCAL_MOD, 30U,
                                     RVM_A7M_KFN_INT_LOCAL_MOD_PRIO_SET, 0xffU) == 0U);
    chb_test_int_mapped = 1U;
  }
  RVM_Virt_Vct_Reg(2U, handler, RVM_VCT_MASKABLE);

  RCC_APB1ENR |= RCC_APB1ENR_TIM4EN;
  TIM4_CR1 = TIM_CR1_DIR;
  TIM4_PSC = 0U;
  TIM4_ARR = 40000U;
  TIM4_SR &= ~TIM_SR_UIF;
  TIM4_DIER |= TIM_DIER_UIE;
  TIM4_CR1 |= TIM_CR1_CEN;
}

void chb_test_int_disable(void) {
  TIM4_CR1 &= ~TIM_CR1_CEN;
  TIM4_DIER &= ~TIM_DIER_UIE;
  RVM_ASSERT(RVM_A7M_Int_Local_Mod(KFN_INT_LOCAL_MOD, 30U,
                                   RVM_A7M_KFN_INT_LOCAL_MOD_STATE_SET, 0U) == 0U);
  RVM_Virt_Vct_Reg(2U, RVM_NULL, RVM_VCT_MASKABLE);
}

#endif /* CHB_TEST_STM32H743ZI_RVM_H */
