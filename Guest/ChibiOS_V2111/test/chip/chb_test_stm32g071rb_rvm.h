/*
    CHB benchmark testbench for the STM32G071RB, running as a guest of RVM.
    Copyright (C) 2026 Evo-Devo Instrum. All rights reserved.

    This file is NOT part of the ChibiOS distribution. It belongs to the
    ChibiOS-on-RVM test suite and is released under the Unlicense; refer to
    the LICENSE file for details.

    Benchmark archive (NUCLEO-G071RB, STM32G071RBT6, RVM, CHB 21.11 LTS,
    GNU Arm 13.2.1 -O3, 100 Hz tick, direct fast path, basic 22-word frame,
    no FPU):
    ====================================================
    Test (number in CPU cycles)        : AVG / MAX / MIN
    Yield                              :  365 / 1544 /  364
    Notification                       :  877 / 3237 /  875
    Semaphore                          :  757 / 3117 /  755
    Message queue                      : 1081 / 5424 / 1077
    Memory allocation/free pair        :  993 / 1544 /  990
    ISR Notification                   : 2046 / 4404 / 2037
    ISR Semaphore                      : 2001 / 6386 / 1992
    ISR Message queue                  : 2176 / 4518 / 2173
    ISR RVM activation relay           : 4480 / 6814 / 3316

    Keil MDK 5 (ARMCLANG 6.23), same fast path:
    ====================================================
    Test (number in CPU cycles)        : AVG / MAX / MIN
    Yield                              :  363 / 1718 /  362
    Notification                       :  868 / 3577 /  864
    Semaphore                          :  751 / 3461 /  748
    Message queue                      : 1055 / 5732 / 1050
    Memory allocation/free pair        :  939 / 1527 /  936
    ISR Notification                   : 1948 / 4654 / 1938
    ISR Semaphore                      : 1915 / 4634 / 1907
    ISR Message queue                  : 2095 / 4744 / 2095
    ISR RVM activation relay           : 4570 / 7951 / 3743

    IAR EWARM 9.2.2, same fast path:
    ====================================================
    Test (number in CPU cycles)        : AVG / MAX / MIN
    Yield                              :  383 / 1597 /  382
    Notification                       :  946 / 3327 /  943
    Semaphore                          :  800 / 3230 /  797
    Message queue                      : 1097 / 7355 / 1092
    Memory allocation/free pair        : 1142 / 1927 / 1138
    ISR Notification                   : 1977 / 4402 / 1968
    ISR Semaphore                      : 1929 / 6313 / 1920
    ISR Message queue                  : 2082 / 4492 / 2077
    ISR RVM activation relay           : 4513 / 7438 / 3080

    Two consecutive resets produced identical rows in the GCC build.
    Thread working areas are 512 bytes on this device (CHB_BENCH_WA_SIZE)
    and the ChibiOS heap is 4 KiB; neither affects the cycle counts.

    The benchmark is timed with TIM2 counting up at the CPU clock through
    its full 16 bit range.  The ISR rows are driven by TIM3 downcounting
    from 12800 (physical IRQ 16 relayed to virtual vector 2), the same
    source the FRT/UO3/RTT G071 testbenches use.
*/

#ifndef CHB_TEST_STM32G071RB_RVM_H
#define CHB_TEST_STM32G071RB_RVM_H

#include "rvm.h"

#define RVM_TEST_VIRTUAL_INC
#include "rvm_test.h"
#undef RVM_TEST_VIRTUAL_INC

/* TIM2 is a 16 bit counter running at the CPU clock on this device. */
/* TIM2 is 16 bit on this device and wraps about once per millisecond at
 * the CPU clock; every measurement is far shorter than that, so the
 * difference is taken modulo 2^16 like the FRT/UO3 G071 testbenches do. */
#define CHB_TEST_CNT_READ()        ( ( uint16_t ) ( TIM2_CNT ) )
#define CHB_TEST_CNT_DIFF(a, b)  ( ( uint32_t ) ( ( uint16_t ) ( ( a ) - ( b ) ) ) )
#define CHB_TEST_KERNEL_STAMP()    ( START )

#define CHB_BENCH_WA_SIZE         ( 512 )

#define TIM2_BASE                  ( 0x40000000UL )
#define TIM3_BASE                  ( 0x40000400UL )
#define RCC_BASE                   ( 0x40021000UL )

#define RCC_APBENR1                ( * ( ( volatile uint32_t * ) ( RCC_BASE + 0x3CUL ) ) )
#define RCC_APBENR1_TIM2EN         ( 1UL << 0UL )
#define RCC_APBENR1_TIM3EN         ( 1UL << 1UL )

#define TIM2_CR1                   ( * ( ( volatile uint32_t * ) ( TIM2_BASE + 0x00UL ) ) )
#define TIM2_PSC                   ( * ( ( volatile uint32_t * ) ( TIM2_BASE + 0x28UL ) ) )
#define TIM2_ARR                   ( * ( ( volatile uint32_t * ) ( TIM2_BASE + 0x2CUL ) ) )
#define TIM2_CNT                   ( * ( ( volatile uint32_t * ) ( TIM2_BASE + 0x24UL ) ) )

#define TIM3_CR1                   ( * ( ( volatile uint32_t * ) ( TIM3_BASE + 0x00UL ) ) )
#define TIM3_DIER                  ( * ( ( volatile uint32_t * ) ( TIM3_BASE + 0x0CUL ) ) )
#define TIM3_SR                    ( * ( ( volatile uint32_t * ) ( TIM3_BASE + 0x10UL ) ) )
#define TIM3_PSC                   ( * ( ( volatile uint32_t * ) ( TIM3_BASE + 0x28UL ) ) )
#define TIM3_ARR                   ( * ( ( volatile uint32_t * ) ( TIM3_BASE + 0x2CUL ) ) )

#define TIM_CR1_CEN                ( 1UL << 0UL )
#define TIM_CR1_DIR                ( 1UL << 4UL )
#define TIM_SR_UIF                 ( 1UL << 0UL )
#define TIM_DIER_UIE               ( 1UL << 0UL )

void chb_test_int_handler(void);

void chb_test_timer_init(void) {
  RCC_APBENR1 |= RCC_APBENR1_TIM2EN;
  TIM2_CR1 = 0U;
  TIM2_PSC = 0U;
  TIM2_ARR = 0xFFFFUL;
  TIM2_CR1 |= TIM_CR1_CEN;
}

static uint32_t chb_test_int_mapped;

void chb_test_int_init(void (*handler)(void)) {
  if (chb_test_int_mapped == 0U) {
    RVM_ASSERT(RVM_Hyp_Vct_Phys(16U, 2U) == 0U);
    RVM_ASSERT(RVM_A6M_Int_Local_Mod(KFN_INT_LOCAL_MOD, 16U,
                                     RVM_A6M_KFN_INT_LOCAL_MOD_STATE_SET, 1U) == 0U);
    RVM_ASSERT(RVM_A6M_Int_Local_Mod(KFN_INT_LOCAL_MOD, 16U,
                                     RVM_A6M_KFN_INT_LOCAL_MOD_PRIO_SET, 0xFFU) == 0U);
    chb_test_int_mapped = 1U;
  }
  RVM_Virt_Vct_Reg(2U, handler, RVM_VCT_MASKABLE);

  RCC_APBENR1 |= RCC_APBENR1_TIM3EN;
  TIM3_CR1 = TIM_CR1_DIR;
  TIM3_PSC = 0U;
  TIM3_ARR = 12800U;
  TIM3_SR &= ~TIM_SR_UIF;
  TIM3_DIER |= TIM_DIER_UIE;
  TIM3_CR1 |= TIM_CR1_CEN;
}

void chb_test_int_disable(void) {
  TIM3_CR1 &= ~TIM_CR1_CEN;
  TIM3_DIER &= ~TIM_DIER_UIE;
  RVM_ASSERT(RVM_A6M_Int_Local_Mod(KFN_INT_LOCAL_MOD, 16U,
                                   RVM_A6M_KFN_INT_LOCAL_MOD_STATE_SET, 0U) == 0U);
  RVM_Virt_Vct_Reg(2U, RVM_NULL, RVM_VCT_MASKABLE);
}

#endif /* CHB_TEST_STM32G071RB_RVM_H */
