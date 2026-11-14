/*
    CHB benchmark testbench for the CH32V307VC, running as a
    guest of the RVM hypervisor.
    Copyright (C) 2026 Evo-Devo Instrum. All rights reserved.

    This file is NOT part of the ChibiOS distribution.  It belongs to the
    ChibiOS-on-RVM test suite and is released under the Unlicense; refer to
    the LICENSE file for details.  The ChibiOS kernel itself is copyright
    Giovanni Di Sirio and is not modified by this file.

    This file holds all the hardware dependent parts of the CHB
    benchmark:  the counter the tests are timed with, and the interrupt
    source that drives the interrupt tests.  The RVM project generator
    selects it through the generated "chb_test.h" header.

    Benchmark archive (CH32V307VCT6, RV32IMAF, RVF/hard-float, 100 Hz tick,
    fast-path build (CHB_FAST_PATH=1), 33-word RVF frames selected from the
    RVM_RV32P_COP_RVF grant):
    ====================================================
    Test (number in CPU cycles)        : AVG / MAX / MIN
    Yield                              :  206 /  659 /  206
    Notification                       :  489 /  511 /  489
    Semaphore                          :  434 /  445 /  434
    Message queue                      :  591 /  626 /  588
    Memory allocation/free pair        :  467 /  480 /  467
    ISR Notification                   : 1974 / 2075 / 1971
    ISR Semaphore                      : 1941 / 1968 / 1941
    ISR Message queue                  : 2062 / 2086 / 2061
    ISR RVM activation relay           : 2811 / 2886 / 2811
    FPU context: PASS; Benchmark complete.

    The slow-path all-Vct build (CHB_FAST_PATH=0) produced Yield 1081,
    Notification 2231, Semaphore 2173, Message queue 2333, Memory 468,
    ISR Notification 1503, ISR Semaphore 1467, ISR Message queue 1578 and
    relay 2810.  The ISR rows are higher with the fast path because the
    user cannot probe mstatus.FS, so the direct switch always saves and
    restores the 33-word RVF block; the task-level rows improve by 4-5x.

    Setting up a project that uses this testbench:
      1. Add the benchmark source file to the project.
      2. Grant the virtual machine access to the peripheral registers:
         <Type>V_Device</Type>
         <Base>0x40000000</Base>
         <Size>0x20000000</Size>
         <Align>Auto</Align>
         <Type>Device</Type>
         <Attribute>RWS</Attribute>
      3. Add the kernel function that manages the physical interrupt:
         <Name>INT_LOCAL_MOD</Name>
         <Start>0xF100</Start>
         <End>0xF100</End>
      4. Declare the interrupt vector that drives the tests:
         <Name>TIM4</Name>
         <Number>46</Number>
      5. Place the following lines into the kernel's spurious handler, or
         into a dedicated handler, so that the interrupt flag is cleared
         before the interrupt is delivered to the virtual machine:
         #define TIM4_INTFR ( * ( ( volatile uint16_t * ) 0x40000810 ) )
         TIM4_INTFR = 0U;
         return RVM_VCT_SIG_INIT;
    All of these steps can be completed automatically by the RVM project
    generator.

    2026-11-12: the benchmark now stops TIM4 and unregisters the virtual
    vector before printing; the four ISR rows are emitted as one block after
    the interrupt phase ends (FRT/UO3/RTE/ZPR/NTX convention).  The archived
    numbers above were taken with row-by-row printing; statistics were closed
    before each print in both modes, so the values are directly comparable.
*/

#ifndef CHB_TEST_CH32V307VC_RVM_H
#define CHB_TEST_CH32V307VC_RVM_H

/* RVM hypervisor interface includes. */
#include "rvm.h"

/* Kernel test interface includes - the kernel side of the benchmark
latches the timestamp of its own interrupt handler entry into a location
this virtual machine can read back, so the activation relay latency can
be measured as well. */
#define RVM_TEST_VIRTUAL_INC
#include "rvm_test.h"
#undef RVM_TEST_VIRTUAL_INC

/*-----------------------------------------------------------*/

/* The benchmark is timed with TIM1, which counts up at the CPU clock
frequency. */
#define CHB_TEST_CNT_READ()        ( TIM1_CNT )
#define CHB_TEST_CNT_DIFF(a, b)  ((uint16_t)((uint16_t)(a) - (uint16_t)(b)))

/* The timestamp latched by the kernel side of the benchmark when it took
the very interrupt that is being relayed to us - the difference between
this and the counter value at our own handler entry is the RVM activation
relay latency. */
#define CHB_TEST_KERNEL_STAMP()              ( START )
/*-----------------------------------------------------------*/

/* Hardware register definitions, so that the benchmark does not depend on
any vendor library.  The register names follow the WCH reference manual. */
#define TIM1_BASE                                ( 0x40012c00UL )
#define TIM4_BASE                                ( 0x40000800UL )
#define RCC_BASE                                 ( 0x40021000UL )

#define RCC_APB2PCENR                            ( * ( ( volatile uint32_t * ) ( RCC_BASE + 0x18UL ) ) )
#define RCC_APB2PCENR_TIM1EN                     ( 1UL << 11UL )

#define RCC_APB1PCENR                            ( * ( ( volatile uint32_t * ) ( RCC_BASE + 0x1cUL ) ) )
#define RCC_APB1PCENR_TIM4EN                     ( 1UL << 2UL )

#define TIM1_CTLR1                               ( * ( ( volatile uint16_t * ) ( TIM1_BASE + 0x00UL ) ) )
#define TIM1_SMCFGR                              ( * ( ( volatile uint16_t * ) ( TIM1_BASE + 0x08UL ) ) )
#define TIM1_CNT                                 ( * ( ( volatile uint16_t * ) ( TIM1_BASE + 0x24UL ) ) )

#define TIM4_CTLR1                               ( * ( ( volatile uint16_t * ) ( TIM4_BASE + 0x00UL ) ) )
#define TIM4_DMAINTENR                           ( * ( ( volatile uint16_t * ) ( TIM4_BASE + 0x0cUL ) ) )
#define TIM4_INTFR                               ( * ( ( volatile uint16_t * ) ( TIM4_BASE + 0x10UL ) ) )
#define TIM4_PSC                                 ( * ( ( volatile uint16_t * ) ( TIM4_BASE + 0x28UL ) ) )
#define TIM4_ATRLR                               ( * ( ( volatile uint16_t * ) ( TIM4_BASE + 0x2cUL ) ) )

#define TIM_CTLR1_CEN                            ( 1UL << 0UL )
#define TIM_CTLR1_DIR                            ( 1UL << 4UL )
#define TIM_INTFR_UIF                            ( 1UL << 0UL )
#define TIM_DMAINTENR_UIE                        ( 1UL << 0UL )


/*-----------------------------------------------------------*/

/*
 * The benchmark interrupt handler, which is implemented by the benchmark
 * source file itself.  It is registered with the RVM hypervisor by the
 * chb_test_int_init() function below.
 */
void chb_test_int_handler( void );

/*
 * Initialise the counter that the benchmark is timed with.
 */
void chb_test_timer_init( void );

/*
 * Initialise the periodic interrupt source that drives the interrupt
 * tests, and connect it to this virtual machine.
 */
void chb_test_int_init( void (*handler)(void) );

/*
 * Disable the periodic interrupt source again.
 */
void chb_test_int_disable( void );

/*-----------------------------------------------------------*/

void chb_test_timer_init( void )
{
    /* Enable the TIM1 clock. */
    RCC_APB2PCENR |= RCC_APB2PCENR_TIM1EN;

    /* Let TIM1 count up through its full 16 bit range at the CPU clock
    frequency, without any prescaling or slave mode selection. */
    TIM1_CTLR1 = 0x0U;
    TIM1_SMCFGR = 0x0U;
    TIM1_CTLR1 |= TIM_CTLR1_CEN;
}
/*-----------------------------------------------------------*/

void chb_test_int_init( void (*handler)(void) )
{
    /* Connect the physical TIM4 interrupt to vector 2 of this virtual
    machine. */
    RVM_ASSERT( RVM_Hyp_Vct_Phys( 46U, 2U ) == 0U );

    /* Enable the physical interrupt at the lowest priority. */
    RVM_ASSERT( RVM_RV32P_Int_Local_Mod( KFN_INT_LOCAL_MOD, 46U, RVM_RV32P_KFN_INT_LOCAL_MOD_STATE_SET, 1U ) == 0U );
    RVM_ASSERT( RVM_RV32P_Int_Local_Mod( KFN_INT_LOCAL_MOD, 46U, RVM_RV32P_KFN_INT_LOCAL_MOD_PRIO_SET, 0xffU ) == 0U );

    /* Register the handler that is implemented by the benchmark itself. */
    RVM_Virt_Vct_Reg( 2U, handler, RVM_VCT_MASKABLE );

    /* Enable the TIM4 clock, and let the counter count down from the
    reload value, which is reached every 57600 CPU cycles. */
    RCC_APB1PCENR |= RCC_APB1PCENR_TIM4EN;
    TIM4_CTLR1 = TIM_CTLR1_DIR;
    TIM4_PSC = 0x0U;
    TIM4_ATRLR = 14400U * 4U;
    TIM4_INTFR = 0x0U;
    TIM4_DMAINTENR |= TIM_DMAINTENR_UIE;
    TIM4_CTLR1 |= TIM_CTLR1_CEN;
}
/*-----------------------------------------------------------*/

void chb_test_int_disable( void )
{
    /* Stop the timer first, matching the ARM testbenches: a running source
     * keeps firing interrupts the monitor must reject one by one. */
    TIM4_CTLR1 &= ~TIM_CTLR1_CEN;
    TIM4_DMAINTENR &= ~TIM_DMAINTENR_UIE;

    /* Disable the physical interrupt. */
    RVM_ASSERT( RVM_RV32P_Int_Local_Mod( KFN_INT_LOCAL_MOD, 46U, RVM_RV32P_KFN_INT_LOCAL_MOD_STATE_SET, 0U ) == 0U );

    /* Reverse the vector registration. */
    RVM_Virt_Vct_Reg( 2U, RVM_NULL, RVM_VCT_MASKABLE );
}

#endif /* CHB_TEST_CH32V307VC_RVM_H */
