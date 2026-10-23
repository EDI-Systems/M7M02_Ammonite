/*
*********************************************************************************************************
*                                               uC/OS-III
*                                         The Real-Time Kernel
*
*                                       RVM Benchmark Test Suite
*
*                               Copyright (C) 2026 Evo-Devo Instrum.
*                                       All rights reserved.
*
*                This file is NOT part of the uC/OS-III distribution.  It belongs
*                to the uC/OS-III-on-RVM test suite and is released under the
*                Unlicense; refer to the LICENSE file for details.  The uC/OS-III
*                kernel itself is copyright Silicon Laboratories Inc. and is not
*                modified by this file.
*
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                          STM32H743ZI TESTBENCH
*
* File      : uo3_test_stm32h743zi_rvm.h
* Version   : V1.00.00
* For       : STM32H743ZIT6 @ 400MHz (Nucleo-144), RVM hypervisor guest
*********************************************************************************************************
*/

/*
*********************************************************************************************************
* Note(s)  : (1) This file holds all the hardware dependent parts of the uC/OS-III benchmark: the
*                counter the tests are timed with, and the interrupt source that drives the
*                interrupt tests.  The RVM project generator selects it through the generated
*                "uo3_test.h" header.
*
*            (2) Benchmark archive (STM32H743ZIT6 rev.Y @ 400MHz, Nucleo-144, task-stack fast yield
*                path with the ICI/IT filter, FPV5 coprocessor, hardware CLZ priority scan):
*
* GNU Arm 13.2.1 -O3
* uC/OS-III V3.08 on RVM - Standard benchmark test
* ====================================================
* Test (number in CPU cycles)        : AVG / MAX / MIN
* Yield                              : 366 / 1840 / 364
* Notification                       : 549 / 1944 / 524
* Semaphore                          : 545 / 680 / 528
* Message queue                      : 529 / 2252 / 520
* Memory allocation/free pair        : 163 / 373 / 163
* ISR Notification                   : 1141 / 2576 / 1060
* ISR Semaphore                      : 1223 / 1336 / 1128
* ISR Message queue                  : 1191 / 1308 / 1104
* ISR RVM activation relay           : 2008 / 3392 / 1892
*
*                Historical note: the first bring-up round ran the priority scan on the C lookup
*                tables (Yield 368, ISR-Noti 1203, ISR-MsgQ 1242); the assembly CLZ versions above
*                supersede them.  With OS_CFG_RVM_FAST_YIELD_EN disabled - every task level yield
*                through the hypervisor - the same build measures Yield 1526 / Notification 1738 /
*                Semaphore 1738 / Message queue 1753, with the ISR rows unchanged (they never use
*                the task level path).
*
*
*            (3) Setting up a project that uses this testbench:
* 1. Add the benchmark source file to the project.
* 2. Grant the virtual machine access to the peripheral registers:
* <Type>V_Device</Type>
* <Base>0x40000000</Base>
* <Size>0x20000000</Size>
* <Align>Auto</Align>
* <Type>Device</Type>
* <Attribute>RWS</Attribute>
* 3. Add the kernel function that manages the physical interrupt:
* <Name>INT_LOCAL_MOD</Name>
* <Start>0xF100</Start>
* <End>0xF100</End>
* 4. Declare the interrupt vector that drives the tests:
* <Name>Tim4</Name>
* <Number>30</Number>
*********************************************************************************************************
*/

#ifndef UO3_TEST_STM32H743ZI_RVM_H

#define  UO3_TEST_STM32H743ZI_RVM_H


/* RVM hypervisor interface includes. */

#include  "rvm.h"

/* Kernel test interface includes - the kernel side of the benchmark
 * latches the timestamp of its own interrupt handler entry into the shared
 * memory this virtual machine can read back, so the activation relay
 * latency can be measured as well. */

#define  RVM_TEST_VIRTUAL_INC
#include  "rvm_test.h"
#undef RVM_TEST_VIRTUAL_INC


/*
*********************************************************************************************************
*                                        BENCHMARK CONFIGURATION MACROS
*
* Note(s)  : (1) The uC/OS-III kernel itself has no printing facility, so the benchmark results
*                are printed through the RVM debug printing functions.
*********************************************************************************************************
*/

#define  BENCH_PRINT_STR(String)              RVM_DBG_S(String)
#define  BENCH_PRINT_INT(Number)              RVM_DBG_I(Number)

/* The benchmark is timed with TIM2, an APB1 timer that counts up at half
 * the CPU clock frequency (no APB timer on this chip reaches the full
 * 400MHz core clock), so the counter value is doubled back up to CPU
 * cycles - the low half of the doubled 32 bit counter then wraps every
 * 65536 CPU cycles exactly like the 16 bit timers of the other chips. */

#define  BENCH_TIME_GET()                     ((CPU_INT16U)(TIM2_CNT << 1u))

/* The timestamp latched by the kernel side of the benchmark when it took the
 * very interrupt that is being relayed to us - the difference between this
 * and the counter value at our own handler entry is the RVM activation relay
 * latency. */

#define  BENCH_KERNEL_TS_GET()                (START)

/* The size of the stack allocated to each benchmark task, measured in words
 * rather than bytes.  This is port dependent: the ARMv7-M extended frame is
 * up to 27 hardware words deep (26 plus one padding), and the S16-S31 block
 * and the hypercall parameters stack on top of it, so the 128 words of the
 * ARMv6-M testbench are doubled here. */

#define  BENCH_TASK_STK_SIZE                  ((CPU_STK_SIZE)256)

/* The memory partition that the memory test gets its blocks from: the number
 * of fixed size blocks, and the size of each block in bytes. */

#define  BENCH_MEM_BLK_QTY                    (8u)
#define  BENCH_MEM_BLK_SIZE                   (512u)

/* The maximum count of the counting semaphore, and the length and the item
 * size of the queue, that are used by the benchmark. */

#define  BENCH_SEM_MAX_COUNT                  (8u)
#define  BENCH_QUEUE_LENGTH                   (8u)
#define  BENCH_MSG_SIZE                       (sizeof(CPU_INT32U))

/*
*********************************************************************************************************
*                                          HARDWARE REGISTER DEFINITIONS
*
* Note(s)  : (1) The register definitions are made directly, so that the benchmark does not depend
*                on any vendor library.
*********************************************************************************************************
*/

#define  TIM2_BASE                                (0x40000000UL)
#define  TIM4_BASE                                (0x40000800UL)
#define  RCC_BASE                                 (0x58024400UL)

#define  RCC_APB1ENR                              (*(((volatile CPU_INT32U *)(RCC_BASE + 0xE8UL))))
#define  RCC_APB1ENR_TIM2EN                       ((CPU_INT32U)1UL << 0UL)
#define  RCC_APB1ENR_TIM4EN                       ((CPU_INT32U)1UL << 2UL)

#define  TIM2_CR1                                 (*(((volatile CPU_INT32U *)(TIM2_BASE + 0x00UL))))
#define  TIM2_PSC                                 (*(((volatile CPU_INT32U *)(TIM2_BASE + 0x28UL))))
#define  TIM2_ARR                                 (*(((volatile CPU_INT32U *)(TIM2_BASE + 0x2CUL))))
#define  TIM2_CNT                                 (*(((volatile CPU_INT32U *)(TIM2_BASE + 0x24UL))))

#define  TIM4_CR1                                 (*(((volatile CPU_INT32U *)(TIM4_BASE + 0x00UL))))
#define  TIM4_DIER                                (*(((volatile CPU_INT32U *)(TIM4_BASE + 0x0CUL))))
#define  TIM4_SR                                  (*(((volatile CPU_INT32U *)(TIM4_BASE + 0x10UL))))
#define  TIM4_PSC                                 (*(((volatile CPU_INT32U *)(TIM4_BASE + 0x28UL))))
#define  TIM4_ARR                                 (*(((volatile CPU_INT32U *)(TIM4_BASE + 0x2CUL))))

#define  TIM_CR1_CEN                              ((CPU_INT32U)1UL << 0UL)
#define  TIM_CR1_DIR                              ((CPU_INT32U)1UL << 4UL)
#define  TIM_SR_UIF                               ((CPU_INT32U)1UL << 0UL)
#define  TIM_DIER_UIE                             ((CPU_INT32U)1UL << 0UL)


/*
*********************************************************************************************************
*                                            FUNCTION PROTOTYPES
*********************************************************************************************************
*/

void  Bench_TestIntHandler (void);                          /* Implemented by the benchmark source file.   */

void  Bench_TimerSetup     (void);

void  Bench_TestIntSetup   (void);

void  Bench_TestIntDisable (void);



/*
*********************************************************************************************************
*                                SETUP THE BENCHMARK TIMER
*
* Description: Initialise the free running counter that the benchmark is timed with.
*
* Arguments  : none.
*
* Returns    : none.
*********************************************************************************************************
*/

void  Bench_TimerSetup (void)
{
    /* Enable the TIM2 clock. */
    RCC_APB1ENR |= RCC_APB1ENR_TIM2EN;

    /* Let TIM2 count up through its full 32 bit range at half the CPU
     * clock frequency, without any prescaling. */
    TIM2_CR1 = 0x0UL;
    TIM2_PSC = 0x0UL;
    TIM2_ARR = 0xFFFFFFFFUL;
    TIM2_CR1 |= TIM_CR1_CEN;
}


/*
*********************************************************************************************************
*                                SETUP THE TEST INTERRUPT
*
* Description: Initialise the periodic interrupt source that drives the interrupt tests, and connect
*              it to this virtual machine.
*
* Arguments  : none.
*
* Returns    : none.
*********************************************************************************************************
*/

void  Bench_TestIntSetup (void)
{
    /* Connect the physical TIM4 interrupt to vector 2 of this virtual
     * machine. */
    RVM_ASSERT(RVM_Hyp_Vct_Phys(30u, 2u) == 0u);

    /* Enable the physical interrupt at the lowest priority. */
    RVM_ASSERT(RVM_A7M_Int_Local_Mod(KFN_INT_LOCAL_MOD, 30u, RVM_A7M_KFN_INT_LOCAL_MOD_STATE_SET, 1u) == 0u);
    RVM_ASSERT(RVM_A7M_Int_Local_Mod(KFN_INT_LOCAL_MOD, 30u, RVM_A7M_KFN_INT_LOCAL_MOD_PRIO_SET, 0xFFu) == 0u);

    /* Register the handler that is implemented by the benchmark itself. */
    RVM_Virt_Vct_Reg(2u, Bench_TestIntHandler, RVM_VCT_MASKABLE);

    /* Enable the TIM4 clock, and let the counter count down from the
     * reload value, which is reached every 40000 APB1 timer clock ticks -
     * the APB1 timers run at half the CPU clock, so this is every 0.2
     * milliseconds. */
    RCC_APB1ENR |= RCC_APB1ENR_TIM4EN;
    TIM4_CR1 = TIM_CR1_DIR;
    TIM4_PSC = 0x0UL;
    TIM4_ARR = 40000u;
    TIM4_SR &= ~TIM_SR_UIF;
    TIM4_DIER |= TIM_DIER_UIE;
    TIM4_CR1 |= TIM_CR1_CEN;
}


/*
*********************************************************************************************************
*                                DISABLE THE TEST INTERRUPT
*
* Description: Disable the periodic interrupt source again.
*
* Arguments  : none.
*
* Returns    : none.
*********************************************************************************************************
*/

void  Bench_TestIntDisable (void)
{
    /* Disable the physical interrupt. */
    RVM_ASSERT(RVM_A7M_Int_Local_Mod(KFN_INT_LOCAL_MOD, 30u, RVM_A7M_KFN_INT_LOCAL_MOD_STATE_SET, 0u) == 0u);

    /* Reverse the vector registration. */
    RVM_Virt_Vct_Reg(2u, RVM_NULL, RVM_VCT_MASKABLE);
}


#endif /* UO3_TEST_STM32H743ZI_RVM_H */