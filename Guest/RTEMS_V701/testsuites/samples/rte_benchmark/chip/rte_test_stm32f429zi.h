/* SPDX-License-Identifier: Unlicense */

/*
 * STM32F429ZI test bench definitions for the RTE benchmark.
 *
 * The generated per-VM rte_test.h selects this header from the .rvp chip
 * name, so the benchmark does not hard-code the board registers.
 *
 * Yield measurement is one-way: task 1 timestamps before
 * rtems_task_wake_after(RTEMS_YIELD_PROCESSOR); task 2 timestamps after it is
 * scheduled.  The peer's yield back is outside the measured interval.
 *
 * Benchmark archive (NUCLEO-F439ZI, STM32F429ZI platform @ 180 MHz, RVM,
 * RTEMS 7.0-rc1, arm-rtems7-gcc 15.2.0, -O2 -g -fdata-sections
 * -ffunction-sections, TIM4 100 us, default fast path):
 *
 * Test (number in CPU cycles)        : AVG / MAX / MIN
 * Yield                               : 1226 /  7104 / 1224
 * Notification                        : 1638 /  7552 / 1636
 * Activation                          : 1478 /  7404 / 1476
 * Semaphore                           : 1726 /  3884 / 1724
 * Message queue                       : 1802 /  7692 / 1800
 * Blocking message queue              : 1802 /  7832 / 1796
 * FIFO                                :  200 /  2244 /  200
 * Alarm combination (1/2/3/5/7)       : 1946 /  4712 / 1208
 * Memory allocation/free pair         : 2113 /  2861 / 2110
 * ISR Notification                    : 2971 /  6240 / 2180
 * ISR RVM activation relay            : 6369 /  9636 / 5080
 * ISR Semaphore                       : 3042 /  8848 / 3036
 * ISR Message queue                   : 3124 /  4080 / 3124
 * ISR Blocking message queue          : 3130 /  8940 / 3124
 * ISR Activation                      : 2816 /  2816 / 2816
 */

#ifndef RTE_TEST_STM32F429ZI_H
#define RTE_TEST_STM32F429ZI_H

#include <stdint.h>

#include "rvm.h"

#define RTE_TEST_CHIP_NAME "STM32F429ZI"

/* Physical TIM4 interrupt and the guest virtual vector it is relayed to. */
#define RTE_TEST_INT_PHYS 30U
#define RTE_TEST_INT_VCT 2U

/* TIM4 reload for the benchmark interrupt source (APB1 timer clock). */
#define RTE_TEST_TIM_ARR 18000U

#define TIM2_BASE (0x40000000UL)
#define TIM4_BASE (0x40000800UL)
#define RCC_BASE (0x40023800UL)

#define RCC_APB1ENR (*((volatile uint32_t *) (RCC_BASE + \
	0x40UL)))
#define RCC_APB1ENR_TIM2 (1UL << 0UL)
#define RCC_APB1ENR_TIM4 (1UL << 2UL)

#define TIM2_CR1 (*((volatile uint32_t *) (TIM2_BASE + 0x00UL)))
#define TIM2_PSC (*((volatile uint32_t *) (TIM2_BASE + 0x28UL)))
#define TIM2_ARR (*((volatile uint32_t *) (TIM2_BASE + 0x2CUL)))
#define TIM2_CNT (*((volatile uint32_t *) (TIM2_BASE + 0x24UL)))

#define TIM4_CR1 (*((volatile uint32_t *) (TIM4_BASE + 0x00UL)))
#define TIM4_DIER (*((volatile uint32_t *) (TIM4_BASE + 0x0CUL)))
#define TIM4_SR (*((volatile uint32_t *) (TIM4_BASE + 0x10UL)))
#define TIM4_PSC (*((volatile uint32_t *) (TIM4_BASE + 0x28UL)))
#define TIM4_ARR (*((volatile uint32_t *) (TIM4_BASE + 0x2CUL)))

#define TIM_CR1_CEN (1UL << 0UL)
#define TIM_CR1_DIR (1UL << 4UL)
#define TIM_SR_UIF (1UL << 0UL)
#define TIM_DIER_UIE (1UL << 0UL)

#define BENCH_TIME_GET() ((uint32_t) (TIM2_CNT << 1))
#define RTE_TEST_INT_CLEAR() do { TIM4_SR = 0U; } while (0)

static inline void RTE_Test_Timer_Init(void)
{
	RCC_APB1ENR |= RCC_APB1ENR_TIM2;
	TIM2_CR1 = 0U;
	TIM2_PSC = 0U;
	TIM2_ARR = 0xFFFFFFFFUL;
	TIM2_CR1 |= TIM_CR1_CEN;
}

static inline void RTE_Test_Int_Init(void (*Handler)(void))
{
	RVM_ASSERT(RVM_Hyp_Vct_Phys(RTE_TEST_INT_PHYS,
		RTE_TEST_INT_VCT) == 0);
	RVM_ASSERT(RVM_A7M_Int_Local_Mod(KFN_INT_LOCAL_MOD,
		RTE_TEST_INT_PHYS, RVM_A7M_KFN_INT_LOCAL_MOD_PRIO_SET,
		0xFFU) == 0);
	RVM_ASSERT(RVM_A7M_Int_Local_Mod(KFN_INT_LOCAL_MOD,
		RTE_TEST_INT_PHYS, RVM_A7M_KFN_INT_LOCAL_MOD_STATE_SET,
		1U) == 0);
	RVM_Virt_Vct_Reg(RTE_TEST_INT_VCT, Handler, RVM_VCT_MASKABLE);

	RCC_APB1ENR |= RCC_APB1ENR_TIM4;
	TIM4_CR1 = TIM_CR1_DIR;
	TIM4_PSC = 0U;
	TIM4_ARR = RTE_TEST_TIM_ARR;
	TIM4_SR &= ~TIM_SR_UIF;
	TIM4_DIER |= TIM_DIER_UIE;
	TIM4_CR1 |= TIM_CR1_CEN;
}

static inline void RTE_Test_Int_Disable(void)
{
	TIM4_CR1 = 0U;
	TIM4_SR = 0U;
	TIM4_DIER = 0U;
	RVM_ASSERT(RVM_A7M_Int_Local_Mod(KFN_INT_LOCAL_MOD,
		RTE_TEST_INT_PHYS, RVM_A7M_KFN_INT_LOCAL_MOD_STATE_SET,
		0U) == 0);
	RVM_Virt_Vct_Reg(RTE_TEST_INT_VCT, RVM_NULL, RVM_VCT_MASKABLE);
}

#endif /* RTE_TEST_STM32F429ZI_H */
