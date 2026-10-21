/*
    FreeRTOS V9.0.0 benchmark testbench for the STM32G071RB, running as a
    guest of the RVM hypervisor.
    Copyright (C) 2026 Evo-Devo Instrum. All rights reserved.

    This file is NOT part of the FreeRTOS distribution.  It belongs to the
    FreeRTOS-on-RVM test suite and is released under the Unlicense; refer to
    the LICENSE file for details.  The FreeRTOS kernel itself is copyright
    Real Time Engineers Ltd. and is not modified by this file.

    This file holds all the hardware dependent parts of the FreeRTOS
    benchmark:  the counter the tests are timed with, and the interrupt
    source that drives the interrupt tests.  The RVM project generator
    selects it through the generated "frt_test.h" header.

    Setting up a project that uses this testbench:
      1. Add the benchmark source file to the project.
      2. Grant the virtual machine access to the peripheral registers:
         <Type>V_Device</Type>
         <Base>0x40000000</Base>
         <Size>0x20000000</Size>
         <Align>Auto</Align>
         <Type>Device</Type>
         <Attribute>RWS</Attribute>
      3. Add the shared memory that the kernel handler uses to latch its
         timestamps, and grant it to the virtual machine as well:
         <Name>Shared1</Name>
         <Base>Auto</Base>
         <Size>0x100</Size>
         <Align>Auto</Align>
         <Type>Data</Type>
         <Attribute>RWBCS</Attribute>
      4. Add the kernel function that manages the physical interrupt:
         <Name>INT_LOCAL_MOD</Name>
         <Start>0xF100</Start>
         <End>0xF100</End>
      5. Declare the interrupt vector that drives the tests:
         <Name>TIM3</Name>
         <Number>16</Number>
      6. Place the following lines into the kernel's spurious handler, or
         into a dedicated handler, so that the interrupt flag is cleared
         before the interrupt is delivered to the virtual machine:
         #define TIM3_SR   ( * ( ( volatile uint32_t * ) 0x40000410 ) )
         TIM3_SR = 0U;
         return RVM_VCT_SIG_INIT;
    All of these steps can be completed automatically by the RVM project
    generator.
*/

#ifndef FRT_TEST_STM32G071RB_RVM_H
#define FRT_TEST_STM32G071RB_RVM_H

/* RVM hypervisor interface includes. */
#include "rvm.h"

/* Kernel test interface includes - the kernel side of the benchmark
 * latches the timestamp of its own interrupt handler entry into the shared
 * memory this virtual machine can read back, so the activation relay
 * latency can be measured as well. */
#define RVM_TEST_VIRTUAL_INC
#include "rvm_test.h"
#undef RVM_TEST_VIRTUAL_INC

/*-----------------------------------------------------------*/

/* The benchmark is timed with TIM2, which counts up at the CPU clock
 * frequency and is only 16 bits wide on this chip. */
#define benchGET_RUN_TIME_COUNTER_VALUE()        ( ( uint16_t ) ( TIM2_CNT ) )

/* The timestamp latched by the kernel side of the benchmark when it took
 * the very interrupt that is being relayed to us - the difference between
 * this and the counter value at our own handler entry is the RVM activation
 * relay latency. */
#define benchGET_KERNEL_TIMESTAMP()              ( START )
/* The size of the pool that the memory test carves into blocks, expressed
 * in 32 bit words - the RAM of this chip only allows a modest pool. */
#define benchMEM_POOL_SIZE                       ( 1024U )

/* The maximum count of the counting semaphore, and the length and the
 * item size of the queue, that are used by the benchmark. */
#define benchMAX_SEMAPHORE_COUNT                 ( 8U )
#define benchQUEUE_LENGTH                        ( 8U )
#define benchQUEUE_ITEM_SIZE                     ( sizeof ( uint32_t ) )

/*-----------------------------------------------------------*/

/* Hardware register definitions, so that the benchmark does not depend on
 * any vendor library.  The register names follow the ST reference manual. */
#define TIM2_BASE                                ( 0x40000000UL )
#define TIM3_BASE                                ( 0x40000400UL )
#define RCC_BASE                                 ( 0x40021000UL )

#define RCC_APBENR1                              ( * ( ( volatile uint32_t * ) ( RCC_BASE + 0x3CUL ) ) )
#define RCC_APBENR1_TIM2                         ( 1UL << 0UL )
#define RCC_APBENR1_TIM3                         ( 1UL << 1UL )

#define TIM2_CR1                                 ( * ( ( volatile uint32_t * ) ( TIM2_BASE + 0x00UL ) ) )
#define TIM2_PSC                                 ( * ( ( volatile uint32_t * ) ( TIM2_BASE + 0x28UL ) ) )
#define TIM2_ARR                                 ( * ( ( volatile uint32_t * ) ( TIM2_BASE + 0x2CUL ) ) )
#define TIM2_CNT                                 ( * ( ( volatile uint32_t * ) ( TIM2_BASE + 0x24UL ) ) )

#define TIM3_CR1                                 ( * ( ( volatile uint32_t * ) ( TIM3_BASE + 0x00UL ) ) )
#define TIM3_DIER                                ( * ( ( volatile uint32_t * ) ( TIM3_BASE + 0x0CUL ) ) )
#define TIM3_SR                                  ( * ( ( volatile uint32_t * ) ( TIM3_BASE + 0x10UL ) ) )
#define TIM3_PSC                                 ( * ( ( volatile uint32_t * ) ( TIM3_BASE + 0x28UL ) ) )
#define TIM3_ARR                                 ( * ( ( volatile uint32_t * ) ( TIM3_BASE + 0x2CUL ) ) )

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
    RCC_APBENR1 |= RCC_APBENR1_TIM2;

    /* Let TIM2 count up through its full 16 bit range at the CPU clock
     * frequency, without any prescaling. */
    TIM2_CR1 = 0x0UL;
    TIM2_PSC = 0x0UL;
    TIM2_ARR = 0xFFFFUL;
    TIM2_CR1 |= TIM_CR1_CEN;
}
/*-----------------------------------------------------------*/

void vSetupTestInterrupt( void )
{
    /* Connect the physical TIM3 interrupt to vector 2 of this virtual
     * machine. */
    RVM_ASSERT( RVM_Hyp_Vct_Phys( 16U, 2U ) == 0U );

    /* Enable the physical interrupt at the lowest priority. */
    RVM_ASSERT( RVM_A6M_Int_Local_Mod( KFN_INT_LOCAL_MOD, 16U, RVM_A6M_KFN_INT_LOCAL_MOD_PRIO_SET, 0xFFU ) == 0U );
    RVM_ASSERT( RVM_A6M_Int_Local_Mod( KFN_INT_LOCAL_MOD, 16U, RVM_A6M_KFN_INT_LOCAL_MOD_STATE_SET, 1U ) == 0U );

    /* Register the handler that is implemented by the benchmark itself. */
    RVM_Virt_Vct_Reg( 2U, xTestIntHandler, RVM_VCT_MASKABLE );

    /* Enable the TIM3 clock, and let the counter count down from the
     * reload value, which is reached every 12800 CPU cycles. */
    RCC_APBENR1 |= RCC_APBENR1_TIM3;
    TIM3_CR1 = TIM_CR1_DIR;
    TIM3_PSC = 0x0UL;
    TIM3_ARR = 12800U;
    TIM3_SR &= ~TIM_SR_UIF;
    TIM3_DIER |= TIM_DIER_UIE;
    TIM3_CR1 |= TIM_CR1_CEN;
}
/*-----------------------------------------------------------*/

void vDisableTestInterrupt( void )
{
    /* Disable the physical interrupt. */
    RVM_ASSERT( RVM_A6M_Int_Local_Mod( KFN_INT_LOCAL_MOD, 16U, RVM_A6M_KFN_INT_LOCAL_MOD_STATE_SET, 0U ) == 0U );

    /* Reverse the vector registration. */
    RVM_Virt_Vct_Reg( 2U, RVM_NULL, RVM_VCT_MASKABLE );
}

#endif /* FRT_TEST_STM32G071RB_RVM_H */
