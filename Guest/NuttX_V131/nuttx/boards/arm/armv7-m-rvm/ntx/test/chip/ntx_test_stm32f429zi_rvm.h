/****************************************************************************
 * boards/arm/armv7-m-rvm/ntx/test/chip/ntx_test_stm32f429zi_rvm.h
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * STM32F429ZI test bench definitions for the NTX benchmark.
 *
 * Benchmark archive (NUCLEO-F429ZI/F439ZI, STM32F429ZIT6, RVM, generic ntx
 * board, NuttX 13.1.0, arm-none-eabi-gcc, TIM2 cycle counter, default fast
 * path, FPU enabled, hard float):
 *
 * Test (number in CPU cycles)        : AVG / MAX / MIN
 * Yield                               : 1602 / 7764 / 1600
 * Semaphore                           :   204 / 2188 /  204
 * Message queue                       : 2775 / 7104 / 2772
 * Blocking message queue              : 1993 / 4020 / 1528
 * Alarm                               :   716 / 6920 /  716
 * Memory allocation/free pair         : 2202 / 6544 / 2200
 * ISR Semaphore                       : 3444 / 9492 / 3440
 * ISR Message queue                   : 5191 / 8724 / 5188
 * ISR RVM activation relay            : 1110 / 5416 / 1104
 *
 * Nine rows: eight NuttX-native primitives plus the RVM activation relay
 * baseline (bare vector delivery -> vector thread -> resume of the spinning
 * thread, no kernel object in the path; read the other ISR rows against it).
 * FIFO and the event/activation rows have no NuttX primitive and are
 * deliberately absent.  The image was the generated benchmark image.
 *
 * The ISR rows are driven by TIM4 through the RVM virtual vector 2 and are
 * printed as one block after the interrupt source stops, so their output
 * cannot interleave with the measurement path.
 */

#ifndef NTX_TEST_STM32F429ZI_RVM_H
#define NTX_TEST_STM32F429ZI_RVM_H

#include <stdint.h>

#include "rvm.h"

#define NTX_TEST_INT_PHYS 30U
#define NTX_TEST_INT_VCT 2U
#define NTX_TEST_TIM_ARR 18000U

#define NTX_TEST_TIM2_BASE (0x40000000UL)
#define NTX_TEST_TIM4_BASE (0x40000800UL)
#define NTX_TEST_RCC_BASE (0x40023800UL)

#define NTX_TEST_RCC_APB1ENR (*((volatile uint32_t *)(NTX_TEST_RCC_BASE + \
                              0x40UL)))
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

#endif /* NTX_TEST_STM32F429ZI_RVM_H */
