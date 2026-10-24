/*
    FreeRTOS V9.0.0 benchmark testbench for the STM32H743ZI, running as a
    guest of the RVM hypervisor.
    Copyright (C) 2026 Evo-Devo Instrum. All rights reserved.

    This file is NOT part of the FreeRTOS distribution.  It belongs to the
    FreeRTOS-on-RVM test suite and is released under the Unlicense; refer
    to the LICENSE file for details.  The FreeRTOS kernel itself is copyright
    Real Time Engineers Ltd. and is not modified by this file.

    This file holds all the hardware dependent parts of the FreeRTOS
    benchmark:  the counter the tests are timed with, and the interrupt
    source that drives the interrupt tests.  The RVM project generator
    selects it through the generated "frt_test.h" header.

    Benchmark archive (STM32H743ZIT6 rev.Y @ 400MHz, VOS scale 1, ICI/IT-aware
    fast yield enabled, w/FPU context, L1 caches enabled):

    GNU Arm 13.2.1 -O3
    ====================================================
    Test (number in CPU cycles)        : AVG / MAX / MIN
    Yield                              : 228 / 1700 /  228
    Notification                       : 434 /  540 /  428
    Semaphore                          : 530 / 1988 /  528
    Message queue                      : 604 / 2020 /  588
    Memory allocation/free pair        : 338 /  616 /  329
    ISR Notification                   : 1167 / 1440 / 1072
    ISR Semaphore                      : 1045 / 1176 / 1000
    ISR Message queue                  : 1217 / 1324 / 1148
    ISR RVM activation relay           : 2031 / 3892 / 1904

    ARMCLANG 6.23 -O3
    ====================================================
    Test (number in CPU cycles)        : AVG / MAX / MIN
    Yield                              : 250 /  276 /  228
    Notification                       : 428 / 1884 /  424
    Semaphore                          : 527 / 2040 /  524
    Message queue                      : 604 / 1992 /  592
    Memory allocation/free pair        : 307 /  555 /  297
    ISR Notification                   : 955 / 1356 /  904
    ISR Semaphore                      : 981 / 1124 /  924
    ISR Message queue                  : 1113 / 2592 / 1048
    ISR RVM activation relay           : 1851 / 3436 / 1812

    ICCARM 9.40 -Oh
    ====================================================
    Test (number in CPU cycles)        : AVG / MAX / MIN
    Yield                              : 220 / 300 / 220
    Notification                       : 456 / 1992 / 420
    Semaphore                          : 533 / 2076 / 532
    Message queue                      : 635 / 716 / 632
    Memory allocation/free pair        : 283 / 510 / 272
    ISR Notification                   : 1112 / 3912 / 1056
    ISR Semaphore                      : 1103 / 1160 / 1064
    ISR Message queue                  : 1230 / 1280 / 1196
    ISR RVM activation relay           : 2136 / 3580 / 2056

    Historical note: an earlier bring-up round ran with the kernel L1 caches
    disabled (Yield 1082/995); those figures are superseded by the ones above.

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
         <Number>30</Number>
      5. Place the following lines into the kernel's spurious handler, or
         into a dedicated handler, so that the interrupt flag is cleared
         before the interrupt is delivered to the virtual machine:
         #define TIM4_SR    ( * ( ( volatile uint32_t * ) 0x40000810 ) )
         TIM4_SR = 0U;
         return RVM_VCT_SIG_INIT;
    All of these steps can be completed automatically by the RVM project
    generator.
*/

#ifndef FRT_TEST_STM32H743ZI_RVM_H
#define FRT_TEST_STM32H743ZI_RVM_H

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

/* The benchmark print macros - the FreeRTOS kernel itself has no printing
facility, so the results are printed through the RVM debug printing
functions. */
#define benchPRINT_STRING( pcString )            RVM_DBG_S( pcString )
#define benchPRINT_NUMBER( ulNumber )            RVM_DBG_I( ulNumber )

/* The size of the stack allocated to each benchmark task, measured in
words rather than bytes. */
#define benchTASK_STACK_SIZE                    ( ( uint16_t ) 256 )

/* The benchmark is timed with TIM2, which counts up at half the CPU clock
frequency, so the counter value is scaled back up to CPU cycles. */
#define benchGET_RUN_TIME_COUNTER_VALUE()        ( ( TIM2_CNT ) << 1 )

/* The timestamp latched by the kernel side of the benchmark when it took
the very interrupt that is being relayed to us - the difference between
this and the counter value at our own handler entry is the RVM activation
relay latency. */
#define benchGET_KERNEL_TIMESTAMP()              ( START )
/* The size of the pool that the memory test carves into blocks, expressed
in 32 bit words. */
#define benchMEM_POOL_SIZE                       ( 8192U )

/* The maximum count of the counting semaphore, and the length and the
item size of the queue, that are used by the benchmark. */
#define benchMAX_SEMAPHORE_COUNT                 ( 32U )
#define benchQUEUE_LENGTH                        ( 32U )
#define benchQUEUE_ITEM_SIZE                     ( sizeof( uint32_t ) )

/*-----------------------------------------------------------*/

/* Hardware register definitions, so that the benchmark does not depend on
any vendor library. */
#define TIM2_BASE                                ( 0x40000000UL )
#define TIM4_BASE                                ( 0x40000800UL )
#define RCC_BASE                                 ( 0x58024400UL )

#define RCC_APB1ENR                              ( * ( ( volatile uint32_t * ) ( RCC_BASE + 0xE8UL ) ) )
#define RCC_APB1ENR_TIM2EN                       ( 1UL << 0UL )
#define RCC_APB1ENR_TIM4EN                       ( 1UL << 2UL )

#define TIM2_CR1                                 ( * ( ( volatile uint32_t * ) ( TIM2_BASE + 0x00UL ) ) )
#define TIM2_PSC                                 ( * ( ( volatile uint32_t * ) ( TIM2_BASE + 0x28UL ) ) )
#define TIM2_ARR                                 ( * ( ( volatile uint32_t * ) ( TIM2_BASE + 0x2cUL ) ) )
#define TIM2_CNT                                 ( * ( ( volatile uint32_t * ) ( TIM2_BASE + 0x24UL ) ) )

#define TIM4_CR1                                 ( * ( ( volatile uint32_t * ) ( TIM4_BASE + 0x00UL ) ) )
#define TIM4_DIER                                ( * ( ( volatile uint32_t * ) ( TIM4_BASE + 0x0cUL ) ) )
#define TIM4_SR                                  ( * ( ( volatile uint32_t * ) ( TIM4_BASE + 0x10UL ) ) )
#define TIM4_PSC                                 ( * ( ( volatile uint32_t * ) ( TIM4_BASE + 0x28UL ) ) )
#define TIM4_ARR                                 ( * ( ( volatile uint32_t * ) ( TIM4_BASE + 0x2cUL ) ) )

#define TIM_CR1_CEN                              ( 1UL << 0UL )
#define TIM_CR1_DIR                              ( 1UL << 4UL )
#define TIM_SR_UIF                               ( 1UL << 0UL )
#define TIM_DIER_UIE                             ( 1UL << 0UL )


/*-----------------------------------------------------------*/

/*
 * The benchmark interrupt handler, which is implemented by the benchmark
 * source file itself.  It is registered with the RVM hypervisor by the
 * vSetupTestInterrupt() function below.
 */
void xTestIntHandler( void );

/*
 * Initialise the counter that the benchmark is timed with.
 */
void vSetupBenchmarkTimer( void );

/*
 * Initialise the periodic interrupt source that drives the interrupt
 * tests, and connect it to this virtual machine.
 */
void vSetupTestInterrupt( void );

/*
 * Disable the periodic interrupt source again.
 */
void vDisableTestInterrupt( void );

/*-----------------------------------------------------------*/

void vSetupBenchmarkTimer( void )
{
    /* Enable the TIM2 clock. */
    RCC_APB1ENR |= RCC_APB1ENR_TIM2EN;

    /* Let TIM2 count up through its full 32 bit range at half the CPU
    clock frequency, without any prescaling. */
    TIM2_CR1 = 0x0UL;
    TIM2_PSC = 0x0UL;
    TIM2_ARR = 0xffffffffUL;
    TIM2_CR1 |= TIM_CR1_CEN;
}
/*-----------------------------------------------------------*/

void vSetupTestInterrupt( void )
{
    /* Connect the physical TIM4 interrupt to vector 2 of this virtual
    machine. */
    RVM_ASSERT( RVM_Hyp_Vct_Phys( 30U, 2U ) == 0U );

    /* Enable the physical interrupt at the lowest priority. */
    RVM_ASSERT( RVM_A7M_Int_Local_Mod( KFN_INT_LOCAL_MOD, 30U, RVM_A7M_KFN_INT_LOCAL_MOD_STATE_SET, 1U ) == 0U );
    RVM_ASSERT( RVM_A7M_Int_Local_Mod( KFN_INT_LOCAL_MOD, 30U, RVM_A7M_KFN_INT_LOCAL_MOD_PRIO_SET, 0xffU ) == 0U );

    /* Register the handler that is implemented by the benchmark itself. */
    RVM_Virt_Vct_Reg( 2U, xTestIntHandler, RVM_VCT_MASKABLE );

    /* Enable the TIM4 clock, and let the counter count down from the
    reload value, which is reached every 40000 half CPU cycles. */
    RCC_APB1ENR |= RCC_APB1ENR_TIM4EN;
    TIM4_CR1 = TIM_CR1_DIR;
    TIM4_PSC = 0x0UL;
    TIM4_ARR = 40000U;
    TIM4_SR &= ~TIM_SR_UIF;
    TIM4_DIER |= TIM_DIER_UIE;
    TIM4_CR1 |= TIM_CR1_CEN;
}
/*-----------------------------------------------------------*/

void vDisableTestInterrupt( void )
{
    /* Disable the physical interrupt. */
    RVM_ASSERT( RVM_A7M_Int_Local_Mod( KFN_INT_LOCAL_MOD, 30U, RVM_A7M_KFN_INT_LOCAL_MOD_STATE_SET, 0U ) == 0U );

    /* Reverse the vector registration. */
    RVM_Virt_Vct_Reg( 2U, RVM_NULL, RVM_VCT_MASKABLE );
}

#endif /* FRT_TEST_STM32H743ZI_RVM_H */

