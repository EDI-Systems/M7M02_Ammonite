/****************************************************************************
 * boards/arm/armv7-m-rvm/ntx/test/chip/ntx_test_stm32h743zi_rvm.h
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * STM32H743ZI test bench definitions for the NTX benchmark.
 *
 * Benchmark archive (NUCLEO-H743ZI, STM32H743ZIT6, RVM, generic ntx board,
 * NuttX 13.1.0, arm-none-eabi-gcc, TIM2 cycle counter, default fast path,
 * FPU enabled, hard float):
 *
 * Test (number in CPU cycles)        : AVG / MAX / MIN
 * Yield                               : 1091 / 2540 / 1088
 * Semaphore                           :   168 / 1440 /  168
 * Message queue                       : 2103 / 6620 / 2068
 * Blocking message queue              : 1470 / 6916 / 1444
 * Alarm                               :   540 / 2024 /  492
 * Memory allocation/free pair         : 1634 / 5256 / 1628
 * ISR Semaphore                       : 2640 / 5764 / 2496
 * ISR Message queue                   : 4338 / 7804 / 4084
 * ISR RVM activation relay            :   812 / 4076 /  744
 *
 * Nine rows: eight NuttX-native primitives plus the RVM activation relay
 * baseline (bare vector delivery -> vector thread -> resume of the spinning
 * thread, no kernel object in the path; read the other ISR rows against it).
 * FIFO and the event/activation rows have no NuttX primitive and are
 * deliberately absent.  No user app was present in the Virt app folder.
 *
 * The ISR rows are driven by TIM4 through the RVM virtual vector 2 and are
 * printed as one block after the interrupt source stops, so their output
 * cannot interleave with the measurement path.
 */

#ifndef NTX_TEST_STM32H743ZI_RVM_H
#define NTX_TEST_STM32H743ZI_RVM_H

#include <stdint.h>

#include "rvm.h"

#define NTX_TEST_INT_PHYS 30U
#define NTX_TEST_INT_VCT 2U
#define NTX_TEST_TIM_ARR 20000U

#define NTX_TEST_TIM2_BASE (0x40000000UL)
#define NTX_TEST_TIM4_BASE (0x40000800UL)
#define NTX_TEST_RCC_BASE (0x58024400UL)

#define NTX_TEST_RCC_APB1ENR (*((volatile uint32_t *)(NTX_TEST_RCC_BASE + \
                              0xE8UL)))
#define NTX_TEST_RCC_APB1ENR_TIM2 (1UL << 0UL)
#define NTX_TEST_RCC_APB1ENR_TIM4 (1UL << 2UL)

#define NTX_TEST_TIM2_CR1 (*((volatile uint32_t *)(NTX_TEST_TIM2_BASE + 0x00UL)))
#define NTX_TEST_TIM2_CNT (*((volatile uint32_t *)(NTX_TEST_TIM2_BASE + 0x24UL)))
#define NTX_TEST_TIM2_PSC (*((volatile uint32_t *)(NTX_TEST_TIM2_BASE + 0x28UL)))
#define NTX_TEST_TIM2_ARR (*((volatile uint32_t *)(NTX_TEST_TIM2_BASE + 0x2CUL)))

#define NTX_TEST_TIM4_CR1 (*((volatile uint32_t *)(NTX_TEST_TIM4_BASE + 0x00UL)))
#define NTX_TEST_TIM4_DIER (*((volatile uint32_t *)(NTX_TEST_TIM4_BASE + 0x0CUL)))
#define NTX_TEST_TIM4_SR (*((volatile uint32_t *)(NTX_TEST_TIM4_BASE + 0x10UL)))
#define NTX_TEST_TIM4_PSC (*((volatile uint32_t *)(NTX_TEST_TIM4_BASE + 0x28UL)))
#define NTX_TEST_TIM4_ARR (*((volatile uint32_t *)(NTX_TEST_TIM4_BASE + 0x2CUL)))

#define NTX_TEST_CNT_READ() ((NTX_TEST_TIM2_CNT) * 2U)
#define NTX_TEST_INT_CLEAR() (NTX_TEST_TIM4_SR = 0U)

static inline void NTX_Test_Timer_Init(void)
{
  NTX_TEST_RCC_APB1ENR |= NTX_TEST_RCC_APB1ENR_TIM2;
  NTX_TEST_TIM2_CR1 = 0U;
  NTX_TEST_TIM2_PSC = 0U;
  NTX_TEST_TIM2_ARR = 0xFFFFFFFFU;
  NTX_TEST_TIM2_CR1 = 1U;
}

static inline void NTX_Test_Int_Init(void (*Handler)(void))
{
  RVM_ASSERT(RVM_Hyp_Vct_Phys(NTX_TEST_INT_PHYS, NTX_TEST_INT_VCT) == 0);
  RVM_ASSERT(RVM_A7M_Int_Local_Mod(KFN_INT_LOCAL_MOD, NTX_TEST_INT_PHYS,
                                   RVM_A7M_KFN_INT_LOCAL_MOD_PRIO_SET,
                                   0xFFU) == 0);
  RVM_ASSERT(RVM_A7M_Int_Local_Mod(KFN_INT_LOCAL_MOD, NTX_TEST_INT_PHYS,
                                   RVM_A7M_KFN_INT_LOCAL_MOD_STATE_SET,
                                   1U) == 0);
  RVM_Virt_Vct_Reg(NTX_TEST_INT_VCT, Handler, RVM_VCT_MASKABLE);
}

static inline void NTX_Test_Int_Start(void)
{
  NTX_TEST_RCC_APB1ENR |= NTX_TEST_RCC_APB1ENR_TIM4;
  NTX_TEST_TIM4_CR1 = (1UL << 4UL);
  NTX_TEST_TIM4_PSC = 0U;
  NTX_TEST_TIM4_ARR = NTX_TEST_TIM_ARR;
  NTX_TEST_TIM4_SR = 0U;
  NTX_TEST_TIM4_DIER = (1UL << 0UL);
  NTX_TEST_TIM4_CR1 |= (1UL << 0UL);
}

static inline void NTX_Test_Int_Disable(void)
{
  NTX_TEST_TIM4_CR1 = 0U;
  NTX_TEST_TIM4_SR = 0U;
  NTX_TEST_TIM4_DIER = 0U;
  RVM_Virt_Vct_Reg(NTX_TEST_INT_VCT, (void (*)(void))0, RVM_VCT_MASKABLE);
  RVM_ASSERT(RVM_A7M_Int_Local_Mod(KFN_INT_LOCAL_MOD, NTX_TEST_INT_PHYS,
                                   RVM_A7M_KFN_INT_LOCAL_MOD_STATE_SET,
                                   0U) == 0);
}

#endif /* NTX_TEST_STM32H743ZI_RVM_H */
