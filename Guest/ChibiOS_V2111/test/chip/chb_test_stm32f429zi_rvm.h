/*
    CHB benchmark testbench for the STM32F429ZI, running as a guest of RVM.
    Copyright (C) 2026 Evo-Devo Instrum. All rights reserved.

    This file is NOT part of the ChibiOS distribution.  It belongs to the
    ChibiOS-on-RVM test suite and is released under the Unlicense; refer to
    the LICENSE file for details.

    Benchmark archive (NUCLEO-F429ZI, STM32F429ZIT6, RVM, CHB 21.11 LTS,
    GNU Arm 13.2.1 -O3, 100 Hz tick, direct fast path, FPV4-SP, hard-float,
    56-word extended frames):
    ====================================================
    Test (number in CPU cycles)        : AVG / MAX / MIN
    Yield                              :  269 / 3020 /  268
    Notification                       :  649 / 6148 /  648
    Semaphore                          :  540 / 6040 /  540
    Message queue                      :  802 / 6388 /  800
    Memory allocation/free pair        :  550 / 1266 /  549
    ISR Notification                   : 1704 / 2212 / 1704
    ISR Semaphore                      : 1668 / 5288 / 1660
    ISR Message queue                  : 1825 / 2332 / 1824
    ISR RVM activation relay           : 3272 / 4120 / 2264

    Same build with CHB_FAST_PATH=0 (all-Vct), FPU context PASS:
    ====================================================
    Test (number in CPU cycles)        : AVG / MAX / MIN
    Yield                              : 1143 / 3464 / 1140
    Notification                       : 2372 / 5960 / 2368
    Semaphore                          : 2280 / 2300 / 2280
    Message queue                      : 2551 / 6012 / 2544
    Memory allocation/free pair        :  550 / 1255 /  549
    ISR Notification                   : 1635 / 5172 / 1628
    ISR Semaphore                      : 1584 / 2048 / 1584
    ISR Message queue                  : 1780 / 5368 / 1772
    ISR RVM activation relay           : 3293 / 5576 / 3208

    IAR EWARM 9.2.2, FPV4-SP, hard-float, same fast path:
    ====================================================
    Test (number in CPU cycles)        : AVG / MAX / MIN
    Yield                              :  248 / 3378 /  248
    Notification                       :  617 / 6892 /  616
    Semaphore                          :  521 / 6792 /  520
    Message queue                      :  762 / 7028 /  760
    Memory allocation/free pair        :  573 / 1355 /  572
    ISR Notification                   : 1775 / 5756 / 1760
    ISR Semaphore                      : 1716 / 1716 / 1716
    ISR Message queue                  : 1875 / 5952 / 1856
    ISR RVM activation relay           : 3856 / 6368 / 2548

    Keil MDK 5 (ARMCLANG 6.23), FPV4-SP, hard-float, same fast path:
    ====================================================
    Test (number in CPU cycles)        : AVG / MAX / MIN
    Yield                              :  232 / 2836 /  232
    Notification                       :  550 / 5740 /  548
    Semaphore                          :  484 / 3976 /  484
    Message queue                      :  700 / 5804 /  696
    Memory allocation/free pair        :  525 / 1164 /  524
    ISR Notification                   : 1532 / 1556 / 1532
    ISR Semaphore                      : 1544 / 4992 / 1532
    ISR Message queue                  : 1696 / 1696 / 1696
    ISR RVM activation relay           : 3059 / 3060 / 3056
    FPU context PASS in both IAR and Keil builds.

    2026-11-12: the benchmark now stops TIM4 and unregisters the virtual
    vector before printing; the four ISR rows are emitted as one block after
    the interrupt phase ends (FRT/UO3/RTE/ZPR/NTX convention).  The archived
    numbers above were taken with row-by-row printing; statistics were closed
    before each print in both modes, so the values are directly comparable.
*/

#ifndef CHB_TEST_STM32F429ZI_RVM_H
#define CHB_TEST_STM32F429ZI_RVM_H

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
#define RCC_BASE                   ( 0x40023800UL )

#define RCC_APB1ENR                ( * ( ( volatile uint32_t * ) ( RCC_BASE + 0x40UL ) ) )
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

#endif /* CHB_TEST_STM32F429ZI_RVM_H */
