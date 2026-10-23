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
*                                         STM32G071RB TESTBENCH
*
* File      : uo3_test_stm32g071rb_rvm.h
* Version   : V1.00.00
* For      : STM32G071RBT6 @ 64MHz, RVM hypervisor guest
*********************************************************************************************************
*/

/*
*********************************************************************************************************
* Note(s)  : (1) This file holds all the hardware dependent parts of the uC/OS-III
*                benchmark: the counter the tests are timed with, and the interrupt
*                source that drives the interrupt tests.  The RVM project generator
*                selects it through the generated "uo3_test.h" header.
*
*            (2) Benchmark archive (STM32G071RBT6 @ 64MHz, stack-switching fast yield
*                path, no coprocessor):
*
* GNU Arm 13.2.1 -O3
* uC/OS-III V3.08 on RVM - Standard benchmark test
* ====================================================
* Test (number in CPU cycles)        : AVG / MAX / MIN
* Yield                              : 806 / 3107 / 804
* Notification                       : 1098 / 3457 / 1095
* Semaphore                          : 1136 / 3496 / 1132
* Message queue                      : 1025 / 3386 / 1020
* Memory allocation/free pair        : 277 / 874 / 277
* ISR Notification                   : 2009 / 3228 / 2009
* ISR Semaphore                      : 2064 / 4416 / 2055
* ISR Message queue                  : 2080 / 6197 / 2071
* ISR RVM activation relay           : 3342 / 5678 / 3293
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
* 3. Add the shared memory that the kernel handler uses to latch its
* timestamps, and grant it to the virtual machine as well:
* <Name>Shared1</Name>
* <Base>Auto</Base>
* <Size>0x100</Size>
* <Align>Auto</Align>
* <Type>Data</Type>
* <Attribute>RWBCS</Attribute>
* 4. Add the kernel function that manages the physical interrupt:
* <Name>INT_LOCAL_MOD</Name>
* <Start>0xF100</Start>
* <End>0xF100</End>
* 5. Declare the interrupt vector that drives the tests:
* <Name>TIM3</Name>
* <Number>16</Number>
* 6. Place the following lines into the kernel's spurious handler, or
* into a dedicated handler, so that the interrupt flag is cleared
* before the interrupt is delivered to the virtual machine:
* #define TIM3_SR   (* ((volatile CPU_INT32U *) 0x40000410))
* TIM3_SR = 0u;
* return RVM_VCT_SIG_INIT;
* All of these steps can be completed automatically by the RVM project generator.
*********************************************************************************************************
*/

#ifndef UO3_TEST_STM32G071RB_RVM_H

#define  UO3_TEST_STM32G071RB_RVM_H

/* RVM hypervisor interface includes. */
#include "rvm.h"

/* Kernel test interface includes - the kernel side of the benchmark
 * latches the timestamp of its own interrupt handler entry into the shared
 * memory this virtual machine can read back, so the activation relay
 * latency can be measured as well. */

#define  RVM_TEST_VIRTUAL_INC
#include "rvm_test.h"
#undef RVM_TEST_VIRTUAL_INC


/*
*********************************************************************************************************
*                                            BENCHMARK PRINT MACROS
*
* Note(s)  : (1) The uC/OS-III kernel itself has no printing facility, so the benchmark results
*                are printed through the RVM debug printing functions.
*********************************************************************************************************
*/

#define  BENCH_PRINT_STR(String)                  RVM_DBG_S(String)
#define  BENCH_PRINT_INT(Number)                  RVM_DBG_I(Number)


/* The benchmark is timed with TIM2, which counts up at the CPU clock
 * frequency and is only 16 bits wide on this chip. */

#define  BENCH_TIME_GET()                         ((CPU_INT16U)(TIM2_CNT))

/* The timestamp latched by the kernel side of the benchmark when it took
 * the very interrupt that is being relayed to us - the difference between
 * this and the counter value at our own handler entry is the RVM activation
 * relay latency. */

#define  BENCH_KERNEL_TS_GET()                    (START)

/* The size of the stack allocated to each benchmark task, measured in words
 * rather than bytes.  This is port dependent: the ARMv6-M switch frame is
 * only 22 words (88 bytes) deep, so the historical 128 words leave a wide
 * margin.  Bigger ports (see the RV32 header) need more. */

#define  BENCH_TASK_STK_SIZE                      ((CPU_STK_SIZE)128)

/* The memory partition that the memory test gets its blocks from: the
 * number of fixed size blocks, and the size of each block in bytes. */

#define  BENCH_MEM_BLK_QTY                        (8u)

#define  BENCH_MEM_BLK_SIZE                       (512u)

/* The maximum count of the counting semaphore, and the length and the
 * item size of the queue, that are used by the benchmark. */

#define  BENCH_SEM_MAX_COUNT                      (8u)

#define  BENCH_QUEUE_LENGTH                       (8u)

#define  BENCH_MSG_SIZE                           (sizeof (CPU_INT32U))


/* Hardware register definitions, so that the benchmark does not depend on
 * any vendor library.  The register names follow the ST reference manual. */

#define  TIM2_BASE                                (0x40000000UL)

#define  TIM3_BASE                                (0x40000400UL)

#define  RCC_BASE                                 (0x40021000UL)


#define  RCC_APBENR1                              (* ((volatile CPU_INT32U *) (RCC_BASE + 0x3CUL)))

#define  RCC_APBENR1_TIM2                         (1UL << 0UL)

#define  RCC_APBENR1_TIM3                         (1UL << 1UL)


#define  TIM2_CR1                                 (* ((volatile CPU_INT32U *) (TIM2_BASE + 0x00UL)))

#define  TIM2_PSC                                 (* ((volatile CPU_INT32U *) (TIM2_BASE + 0x28UL)))

#define  TIM2_ARR                                 (* ((volatile CPU_INT32U *) (TIM2_BASE + 0x2CUL)))

#define  TIM2_CNT                                 (* ((volatile CPU_INT32U *) (TIM2_BASE + 0x24UL)))


#define  TIM3_CR1                                 (* ((volatile CPU_INT32U *) (TIM3_BASE + 0x00UL)))

#define  TIM3_DIER                                (* ((volatile CPU_INT32U *) (TIM3_BASE + 0x0CUL)))

#define  TIM3_SR                                  (* ((volatile CPU_INT32U *) (TIM3_BASE + 0x10UL)))

#define  TIM3_PSC                                 (* ((volatile CPU_INT32U *) (TIM3_BASE + 0x28UL)))

#define  TIM3_ARR                                 (* ((volatile CPU_INT32U *) (TIM3_BASE + 0x2CUL)))


#define  TIM_CR1_CEN                              (1UL << 0UL)

#define  TIM_CR1_DIR                              (1UL << 4UL)

#define  TIM_SR_UIF                               (1UL << 0UL)

#define  TIM_DIER_UIE                             (1UL << 0UL)


/*
 * The benchmark interrupt handler, which is implemented by the benchmark
 * source file itself.  It is registered with the RVM hypervisor by the
 * Bench_TestIntSetup() function below.
 */
/*
*********************************************************************************************************
*                              PERIODIC TEST INTERRUPT HANDLER
*
* Description: Service the periodic test interrupt: latch the start count and post the workload of the interrupt driven test that is currently running.
*
* Arguments  : none.
*
* Returns    : none.
*********************************************************************************************************
*/

void Bench_TestIntHandler(void);

/*
 * Initialise the counter that the benchmark is timed with.
 */
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

void Bench_TimerSetup(void);

/*
 * Initialise the periodic interrupt source that drives the interrupt
 * tests, and connect it to this virtual machine.
 */
/*
*********************************************************************************************************
*                                 SETUP THE TEST INTERRUPT
*
* Description: Initialise the periodic interrupt source that drives the interrupt tests, and connect it to this virtual machine.
*
* Arguments  : none.
*
* Returns    : none.
*********************************************************************************************************
*/

void Bench_TestIntSetup(void);

/*
 * Disable the periodic interrupt source again.
 */
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

void Bench_TestIntDisable(void);


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

void Bench_TimerSetup(void)
{
    /* Enable the TIM2 clock. */
    RCC_APBENR1 |= RCC_APBENR1_TIM2;

    /* Let TIM2 count up through its full 16 bit range at the CPU clock
     * frequency, without any prescaling. */
    TIM2_CR1 = 0x0UL;
    TIM2_PSC = 0x0UL;
    TIM2_ARR = 0xFFFFUL;
    TIM2_CR1 |= TIM_CR1_CEN;
}

/*
*********************************************************************************************************
*                                 SETUP THE TEST INTERRUPT
*
* Description: Initialise the periodic interrupt source that drives the interrupt tests, and connect it to this virtual machine.
*
* Arguments  : none.
*
* Returns    : none.
*********************************************************************************************************
*/

void Bench_TestIntSetup(void)
{
    /* Connect the physical TIM3 interrupt to vector 2 of this virtual
     * machine. */
    RVM_ASSERT(RVM_Hyp_Vct_Phys(16u, 2u) == 0u);

    /* Enable the physical interrupt at the lowest priority. */
    RVM_ASSERT(RVM_A6M_Int_Local_Mod(KFN_INT_LOCAL_MOD, 16u, RVM_A6M_KFN_INT_LOCAL_MOD_PRIO_SET, 0xFFu) == 0u);
    RVM_ASSERT(RVM_A6M_Int_Local_Mod(KFN_INT_LOCAL_MOD, 16u, RVM_A6M_KFN_INT_LOCAL_MOD_STATE_SET, 1u) == 0u);

    /* Register the handler that is implemented by the benchmark itself. */
    RVM_Virt_Vct_Reg(2u, Bench_TestIntHandler, RVM_VCT_MASKABLE);

    /* Enable the TIM3 clock, and let the counter count down from the
     * reload value, which is reached every 12800 CPU cycles. */
    RCC_APBENR1 |= RCC_APBENR1_TIM3;
    TIM3_CR1 = TIM_CR1_DIR;
    TIM3_PSC = 0x0UL;
    TIM3_ARR = 12800u;
    TIM3_SR &= ~TIM_SR_UIF;
    TIM3_DIER |= TIM_DIER_UIE;
    TIM3_CR1 |= TIM_CR1_CEN;
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

void Bench_TestIntDisable(void)
{
    /* Disable the physical interrupt. */
    RVM_ASSERT(RVM_A6M_Int_Local_Mod(KFN_INT_LOCAL_MOD, 16u, RVM_A6M_KFN_INT_LOCAL_MOD_STATE_SET, 0u) == 0u);

    /* Reverse the vector registration. */
    RVM_Virt_Vct_Reg(2u, RVM_NULL, RVM_VCT_MASKABLE);
}

#endif /* UO3_TEST_STM32G071RB_RVM_H */
