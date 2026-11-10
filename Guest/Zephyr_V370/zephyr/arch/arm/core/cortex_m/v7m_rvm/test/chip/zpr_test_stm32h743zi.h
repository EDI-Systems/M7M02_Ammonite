/* SPDX-License-Identifier: Unlicense */

/*
 * STM32H743ZI test bench definitions for the ZPR benchmark.
 *
 * Benchmark archive (NUCLEO-H743ZI, STM32H743ZIT6 @ 480 MHz, RVM,
 * virtual armv7m board, Zephyr 3.7.0, arm-none-eabi-gcc 13.2.1,
 * -Os, TIM2 cycle counter, default fast path, CONFIG_FPU=y /
 * CONFIG_FPU_SHARING=y, hard float):
 *
 * Test (number in CPU cycles)        : AVG / MAX / MIN
 * Yield                               :  473 /  612 /  472
 * Notification                        :  408 /  776 /  408
 * Activation                          :  398 / 1776 /  388
 * Semaphore                           :  190 /  304 /  184
 * FIFO                                :  244 /  468 /  244
 * Message queue                       :  328 / 1640 /  328
 * Blocking message queue              :  451 / 4584 /  244
 * Alarm                               :  468 / 1952 /  460
 * Memory allocation/free pair         :  732 / 2112 /  732
 * ISR Notification                    :  604 / 3076 /  576
 * ISR Activation                      :  604 /  620 /  604
 * ISR Semaphore                       :  604 / 3088 /  576
 * ISR Message queue                   :  604 /  616 /  604
 * ISR Blocking message queue          :  604 / 3100 /  576
 * ISR RVM activation relay            :  604 /  616 /  604
 *
 * The ISR rows sample in the RVM virtual-vector hook and expose the result to
 * a polling benchmark thread; this is the same 10 kHz TIM4 source used by the
 * native benchmark and the RTE port.  The six ISR rows share that single
 * hook; their labels do not select different ISR mechanisms.
 */

#ifndef ZPR_TEST_STM32H743ZI_H
#define ZPR_TEST_STM32H743ZI_H

#include <stdint.h>

#include "rvm.h"


#define ZPR_TEST_INT_PHYS 30U
#define ZPR_TEST_INT_VCT 2U
#define ZPR_TEST_TIM_ARR 20000U

#define ZPR_TEST_TIM2_BASE (0x40000000UL)
#define ZPR_TEST_TIM4_BASE (0x40000800UL)
#define ZPR_TEST_RCC_BASE (0x58024400UL)

#define ZPR_TEST_RCC_APB1ENR (*((volatile uint32_t *) (ZPR_TEST_RCC_BASE + \
	0xE8UL)))
#define ZPR_TEST_RCC_APB1ENR_TIM2 (1UL << 0UL)
#define ZPR_TEST_RCC_APB1ENR_TIM4 (1UL << 2UL)

#define ZPR_TEST_TIM2_CR1 (*((volatile uint32_t *) (ZPR_TEST_TIM2_BASE + 0x00UL)))
#define ZPR_TEST_TIM2_CNT (*((volatile uint32_t *) (ZPR_TEST_TIM2_BASE + 0x24UL)))
#define ZPR_TEST_TIM2_PSC (*((volatile uint32_t *) (ZPR_TEST_TIM2_BASE + 0x28UL)))
#define ZPR_TEST_TIM2_ARR (*((volatile uint32_t *) (ZPR_TEST_TIM2_BASE + 0x2CUL)))

#define ZPR_TEST_TIM4_CR1 (*((volatile uint32_t *) (ZPR_TEST_TIM4_BASE + 0x00UL)))
#define ZPR_TEST_TIM4_DIER (*((volatile uint32_t *) (ZPR_TEST_TIM4_BASE + 0x0CUL)))
#define ZPR_TEST_TIM4_SR (*((volatile uint32_t *) (ZPR_TEST_TIM4_BASE + 0x10UL)))
#define ZPR_TEST_TIM4_PSC (*((volatile uint32_t *) (ZPR_TEST_TIM4_BASE + 0x28UL)))
#define ZPR_TEST_TIM4_ARR (*((volatile uint32_t *) (ZPR_TEST_TIM4_BASE + 0x2CUL)))

#define ZPR_TEST_CNT_READ() ((ZPR_TEST_TIM2_CNT) * 2U)
#define ZPR_TEST_INT_CLEAR() (ZPR_TEST_TIM4_SR = 0U)

static inline void ZPR_Test_Timer_Init(void)
{
	ZPR_TEST_RCC_APB1ENR |= ZPR_TEST_RCC_APB1ENR_TIM2;
	ZPR_TEST_TIM2_CR1 = 0U;
	ZPR_TEST_TIM2_PSC = 0U;
	ZPR_TEST_TIM2_ARR = 0xFFFFFFFFU;
	ZPR_TEST_TIM2_CR1 = 1U;
}

static inline void ZPR_Test_Int_Init(void (*Handler)(void))
{
	RVM_ASSERT(RVM_Hyp_Vct_Phys(ZPR_TEST_INT_PHYS,
		ZPR_TEST_INT_VCT) == 0);
	RVM_ASSERT(RVM_A7M_Int_Local_Mod(KFN_INT_LOCAL_MOD,
		ZPR_TEST_INT_PHYS, RVM_A7M_KFN_INT_LOCAL_MOD_PRIO_SET,
		0xFFU) == 0);
	RVM_ASSERT(RVM_A7M_Int_Local_Mod(KFN_INT_LOCAL_MOD,
		ZPR_TEST_INT_PHYS, RVM_A7M_KFN_INT_LOCAL_MOD_STATE_SET,
		1U) == 0);
	RVM_Virt_Vct_Reg(ZPR_TEST_INT_VCT, Handler, RVM_VCT_MASKABLE);

	ZPR_TEST_RCC_APB1ENR |= ZPR_TEST_RCC_APB1ENR_TIM4;
	ZPR_TEST_TIM4_CR1 = (1UL << 4UL);
	ZPR_TEST_TIM4_PSC = 0U;
	ZPR_TEST_TIM4_ARR = ZPR_TEST_TIM_ARR;
	ZPR_TEST_TIM4_SR = 0U;
	ZPR_TEST_TIM4_DIER = (1UL << 0UL);
	ZPR_TEST_TIM4_CR1 |= (1UL << 0UL);
}

static inline void ZPR_Test_Int_Disable(void)
{
	ZPR_TEST_TIM4_CR1 = 0U;
	ZPR_TEST_TIM4_SR = 0U;
	ZPR_TEST_TIM4_DIER = 0U;
	RVM_Virt_Vct_Reg(ZPR_TEST_INT_VCT,
		(void (*)(void))0, RVM_VCT_MASKABLE);
	RVM_ASSERT(RVM_A7M_Int_Local_Mod(KFN_INT_LOCAL_MOD,
		ZPR_TEST_INT_PHYS, RVM_A7M_KFN_INT_LOCAL_MOD_STATE_SET,
		0U) == 0);
}

#endif /* ZPR_TEST_STM32H743ZI_H */
