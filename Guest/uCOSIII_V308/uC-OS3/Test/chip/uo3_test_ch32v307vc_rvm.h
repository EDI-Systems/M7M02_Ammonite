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
*                                          CH32V307VC TESTBENCH
*
* File      : uo3_test_ch32v307vc_rvm.h
* Version   : V1.00.00
* For       : CH32V307VCT6 @ 144MHz, RVM hypervisor guest
*********************************************************************************************************
*/

/*
*********************************************************************************************************
* Note(s)  : (1) This file holds all the hardware dependent parts of the uC/OS-III benchmark: the
*                counter the tests are timed with, and the interrupt source that drives the
*                interrupt tests.  The RVM project generator selects it through the generated
*                "uo3_test.h" header.
*
*            (2) Benchmark archive (CH32V307VCT6 @ 144MHz, task-stack fast yield
*                path, RVF coprocessor):
*
* GNU RISC-V Embedded GCC 13.2.1 -O3
* uC/OS-III V3.08 on RVM - Standard benchmark test
* ====================================================
* Test (number in CPU cycles)        : AVG / MAX / MIN
* Yield                              : 455 / 484 / 455
* Notification                       : 592 / 608 / 590
* Semaphore                          : 589 / 613 / 589
* Message queue                      : 602 / 625 / 602
* Memory allocation/free pair        : 195 / 201 / 195
* ISR Notification                   : 2075 / 2110 / 2062
* ISR Semaphore                      : 2069 / 2094 / 2068
* ISR Message queue                  : 2074 / 2088 / 2074
* ISR RVM activation relay           : 2274 / 2352 / 2274
*
*********************************************************************************************************
*/

#ifndef UO3_TEST_CH32V307VC_RVM_H

#define  UO3_TEST_CH32V307VC_RVM_H


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
*                                            BENCHMARK PRINT MACROS
*
* Note(s)  : (1) The uC/OS-III kernel itself has no printing facility, so the benchmark results
*                are printed through the RVM debug printing functions.
*********************************************************************************************************
*/

#define  BENCH_PRINT_STR(String)              RVM_DBG_S(String)
#define  BENCH_PRINT_INT(Number)              RVM_DBG_I(Number)


/* The benchmark is timed with TIM1, which counts up at the CPU clock
 * frequency and is 16 bits wide on this chip. */

#define  BENCH_TIME_GET()                     ((CPU_INT16U)(TIM1_CNT))

/* The timestamp latched by the kernel side of the benchmark when it took the
 * very interrupt that is being relayed to us - the difference between this
 * and the counter value at our own handler entry is the RVM activation relay
 * latency. */

#define  BENCH_KERNEL_TS_GET()                (START)

/* The size of the stack allocated to each benchmark task, measured in words
 * rather than bytes.  This is port dependent: the RV32 switch frame is 37
 * integer words plus a 33-word lazy FPU transient (280 bytes) - much deeper
 * than the ARMv6-M frame - and the kernel pend/post critical sections stack
 * on top of it.  128 words overflowed into .bss here and corrupted the
 * kernel lists (fixed by 256); see the archive note in AGENTS.md 8.1. */

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


/* Hardware register definitions, so that the benchmark does not depend on
 * any vendor library.  The register names follow the WCH reference manual. */

#define  TIM1_BASE                            (0x40012C00uL)
#define  TIM4_BASE                            (0x40000800uL)
#define  RCC_BASE                             (0x40021000uL)

#define  RCC_APB2PCENR                        (*((volatile CPU_INT32U *)(RCC_BASE + 0x18uL)))
#define  RCC_APB2PCENR_TIM1EN                 (1uL << 11uL)

#define  RCC_APB1PCENR                        (*((volatile CPU_INT32U *)(RCC_BASE + 0x1CuL)))
#define  RCC_APB1PCENR_TIM4EN                 (1uL << 2uL)

#define  TIM1_CTLR1                           (*((volatile CPU_INT16U *)(TIM1_BASE + 0x00uL)))
#define  TIM1_SMCFGR                          (*((volatile CPU_INT16U *)(TIM1_BASE + 0x08uL)))
#define  TIM1_CNT                             (*((volatile CPU_INT16U *)(TIM1_BASE + 0x24uL)))

#define  TIM4_CTLR1                           (*((volatile CPU_INT16U *)(TIM4_BASE + 0x00uL)))
#define  TIM4_DMAINTENR                       (*((volatile CPU_INT16U *)(TIM4_BASE + 0x0CuL)))
#define  TIM4_INTFR                           (*((volatile CPU_INT16U *)(TIM4_BASE + 0x10uL)))
#define  TIM4_PSC                             (*((volatile CPU_INT16U *)(TIM4_BASE + 0x28uL)))
#define  TIM4_ATRLR                           (*((volatile CPU_INT16U *)(TIM4_BASE + 0x2CuL)))

#define  TIM_CTLR1_CEN                        (1uL << 0uL)
#define  TIM_CTLR1_DIR                        (1uL << 4uL)
#define  TIM_INTFR_UIF                        (1uL << 0uL)
#define  TIM_DMAINTENR_UIE                    (1uL << 0uL)
/*
*********************************************************************************************************
*                                        BENCHMARK SUPPORT FUNCTIONS
*********************************************************************************************************
*/

void  Bench_TestIntHandler(void);
void  Bench_TimerSetup      (void);
void  Bench_TestIntSetup    (void);
void  Bench_TestIntDisable  (void);


/*
*********************************************************************************************************
*                                       SETUP THE BENCHMARK TIMER
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
    /* Enable the TIM1 clock. */
    RCC_APB2PCENR |= RCC_APB2PCENR_TIM1EN;

    /* Let TIM1 count up through its full 16 bit range at the CPU clock
     * frequency, without any prescaling or slave mode selection. */
    TIM1_CTLR1  = 0x0u;
    TIM1_SMCFGR = 0x0u;
    TIM1_CTLR1 |= TIM_CTLR1_CEN;
}


/*
*********************************************************************************************************
*                                       SETUP THE TEST INTERRUPT
*
* Description: Initialise the periodic interrupt source that drives the interrupt tests, and
*              connect it to this virtual machine.
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
    RVM_ASSERT(RVM_Hyp_Vct_Phys(46u, 2u) == 0u);

    /* Enable the physical interrupt at the lowest priority. */
    RVM_ASSERT(RVM_RV32P_Int_Local_Mod(KFN_INT_LOCAL_MOD, 46u, RVM_RV32P_KFN_INT_LOCAL_MOD_STATE_SET, 1u) == 0u);
    RVM_ASSERT(RVM_RV32P_Int_Local_Mod(KFN_INT_LOCAL_MOD, 46u, RVM_RV32P_KFN_INT_LOCAL_MOD_PRIO_SET, 0xFFu) == 0u);

    /* Register the handler that is implemented by the benchmark itself. */
    RVM_Virt_Vct_Reg(2u, Bench_TestIntHandler, RVM_VCT_MASKABLE);

    /* Enable the TIM4 clock, and let the counter count down from the
     * reload value, which is reached every 57600 CPU cycles. */
    RCC_APB1PCENR |= RCC_APB1PCENR_TIM4EN;
    TIM4_CTLR1     = TIM_CTLR1_DIR;
    TIM4_PSC       = 0x0u;
    TIM4_ATRLR     = 14400u * 4u;
    TIM4_INTFR     = 0x0u;
    TIM4_DMAINTENR |= TIM_DMAINTENR_UIE;
    TIM4_CTLR1    |= TIM_CTLR1_CEN;
}


/*
*********************************************************************************************************
*                                      DISABLE THE TEST INTERRUPT
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
    RVM_ASSERT(RVM_RV32P_Int_Local_Mod(KFN_INT_LOCAL_MOD, 46u, RVM_RV32P_KFN_INT_LOCAL_MOD_STATE_SET, 0u) == 0u);

    /* Reverse the vector registration. */
    RVM_Virt_Vct_Reg(2u, RVM_NULL, RVM_VCT_MASKABLE);
}


#endif /* UO3_TEST_CH32V307VC_RVM_H */