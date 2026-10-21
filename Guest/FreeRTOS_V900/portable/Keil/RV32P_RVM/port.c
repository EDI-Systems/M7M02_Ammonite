/*
    FreeRTOS V9.0.0 RVM port - Copyright (C) 2026 Evo-Devo Instrum.
    All rights reserved

    This file is NOT part of the FreeRTOS distribution.  It belongs to the
    FreeRTOS-on-RVM test suite and is released under the Unlicense; refer
    to the LICENSE file for details.  The FreeRTOS kernel itself is
    copyright Real Time Engineers Ltd. and is not modified by this file.

    FreeRTOS is free software; you can redistribute it and/or modify it
    under the terms of the GNU General Public License (version 2) as
    published by the Free Software Foundation >>>> AND MODIFIED BY <<<<
    the FreeRTOS exception.

    ***************************************************************************

    This file implements the FreeRTOS porting layer for the RV32
    architecture under the RVM hypervisor.  It maps FreeRTOS primitives
    onto the RVM system call interface so that FreeRTOS runs as an
    unprivileged guest of the hypervisor.
*/

/*-----------------------------------------------------------
 * Implementation of functions defined in portable.h for the RV32P RVM port.
 *----------------------------------------------------------*/

/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "portmacro.h"

/* Standard includes. */
#include "string.h"

#ifdef configCLINT_BASE_ADDRESS
    #warning The configCLINT_BASE_ADDRESS constant has been deprecated.  configMTIME_BASE_ADDRESS and configMTIMECMP_BASE_ADDRESS are currently being derived from the (possibly 0) configCLINT_BASE_ADDRESS setting.  Please update to define configMTIME_BASE_ADDRESS and configMTIMECMP_BASE_ADDRESS dirctly in place of configCLINT_BASE_ADDRESS.  See https://www.FreeRTOS.org/Using-FreeRTOS-on-RISC-V.html
#endif

#ifndef configMTIME_BASE_ADDRESS
    #warning configMTIME_BASE_ADDRESS must be defined in FreeRTOSConfig.h.  If the target chip includes a memory-mapped mtime register then set configMTIME_BASE_ADDRESS to the mapped address.  Otherwise set configMTIME_BASE_ADDRESS to 0.  See https://www.FreeRTOS.org/Using-FreeRTOS-on-RISC-V.html
#endif

#ifndef configMTIMECMP_BASE_ADDRESS
    #warning configMTIMECMP_BASE_ADDRESS must be defined in FreeRTOSConfig.h.  If the target chip includes a memory-mapped mtimecmp register then set configMTIMECMP_BASE_ADDRESS to the mapped address.  Otherwise set configMTIMECMP_BASE_ADDRESS to 0.  See https://www.FreeRTOS.org/Using-FreeRTOS-on-RISC-V.html
#endif

/* Let the user override the pre-loading of the initial LR with the address of
prvTaskExitError() in case it messes up unwinding of the stack in the
debugger. */
#ifdef configTASK_RETURN_ADDRESS
    #define portTASK_RETURN_ADDRESS             configTASK_RETURN_ADDRESS
#else
    #define portTASK_RETURN_ADDRESS             prvTaskExitError
#endif

#if( configUSE_PORT_RVF != 0 ) || ( configUSE_PORT_RVD != 0 )
    /* The mstatus.FS[1] bit - the bit that marks a stack frame as containing
     * live coprocessor context (see the RISC-V privileged spec, mstatus.FS).
     * The initial (01) and off (00) states do not carry any FPU state. */
    #define portFPU_CONTEXT_DIRTY_BIT           ( ( uint32_t ) 1UL << 14UL )

    /* The coprocessor register save area that the hypervisor latches for the
     * interrupted task before this handler is entered; see the RVM_Thd_Reg
     * structure in "rvm.h". */
    #define portFPU_CONTEXT                     ( ( volatile struct RVM_RV32P_Cop_Struct * ) ( RVM_REG->Cop ) )
#endif

/* The stack used by interrupt service routines.  Set configISR_STACK_SIZE_WORDS
to use a statically allocated array as the interrupt stack.  Alternative leave
configISR_STACK_SIZE_WORDS undefined and update the linker script so that a
linker variable names __freertos_irq_stack_top has the same value as the top
of the stack used by main.  Using the linker script method will repurpose the
stack that was used by main before the scheduler was started for use as the
interrupt stack after the scheduler has started. */
#ifdef configISR_STACK_SIZE_WORDS
    static __attribute__ ((aligned(16))) StackType_t xISRStack[ configISR_STACK_SIZE_WORDS ] = { 0 };
    const StackType_t xISRStackTop = ( StackType_t ) &( xISRStack[ configISR_STACK_SIZE_WORDS & ~portBYTE_ALIGNMENT_MASK ] );

    /* Don't use 0xa5 as the stack fill bytes as that is used by the kernerl for
    the task stacks, and so will legitimately appear in many positions within
    the ISR stack. */
    #define portISR_STACK_FILL_BYTE             0xee
#else
    /* __freertos_irq_stack_top define by .ld file */
    extern const uint32_t __freertos_irq_stack_top[];
    const StackType_t xISRStackTop = ( StackType_t ) __freertos_irq_stack_top;
#endif

static UBaseType_t uxCriticalNesting = 0xaaaaaaaa;
/*
 * Setup the timer to generate the tick interrupts.  The implementation in this
 * file is weak to allow application writers to change the timer used to
 * generate the tick interrupt.
 */
void vPortSetupTimerInterrupt( void ) __attribute__(( weak ));
void xPortSysTickHandler(void);
void xPortCtxHandler(void);

/*-----------------------------------------------------------*/
#if( configMTIME_BASE_ADDRESS != 0 ) && ( configMTIMECMP_BASE_ADDRESS != 0 )
/* Used to program the machine timer compare register. */
uint64_t ullNextTime = 0ULL;
const uint64_t *pullNextTime = &ullNextTime;
const uint64_t uxTimerIncrementsForOneTick = ( uint64_t) ( ( configCPU_CLOCK_HZ )/( configTICK_RATE_HZ ) ); /* Assumes increment won't go over 32-bits. */
uint64_t const ullMachineTimerCompareRegisterBase = configMTIMECMP_BASE_ADDRESS;
volatile uint64_t * pullMachineTimerCompareRegister = NULL;
#endif

/* Set configCHECK_FOR_STACK_OVERFLOW to 3 to add ISR stack checking to task
stack checking.  A problem in the ISR stack will trigger an assert, not call the
stack overflow hook function (because the stack overflow hook is specific to a
task stack, not the ISR stack). */
#if defined( configISR_STACK_SIZE_WORDS ) && ( configCHECK_FOR_STACK_OVERFLOW > 2 )
    #warning This path not tested, or even compiled yet.

    static const uint8_t ucExpectedStackBytes[] = {
                                    portISR_STACK_FILL_BYTE, portISR_STACK_FILL_BYTE, portISR_STACK_FILL_BYTE, portISR_STACK_FILL_BYTE,     \
                                    portISR_STACK_FILL_BYTE, portISR_STACK_FILL_BYTE, portISR_STACK_FILL_BYTE, portISR_STACK_FILL_BYTE,     \
                                    portISR_STACK_FILL_BYTE, portISR_STACK_FILL_BYTE, portISR_STACK_FILL_BYTE, portISR_STACK_FILL_BYTE,     \
                                    portISR_STACK_FILL_BYTE, portISR_STACK_FILL_BYTE, portISR_STACK_FILL_BYTE, portISR_STACK_FILL_BYTE,     \
                                    portISR_STACK_FILL_BYTE, portISR_STACK_FILL_BYTE, portISR_STACK_FILL_BYTE, portISR_STACK_FILL_BYTE };   \

    #define portCHECK_ISR_STACK()               configASSERT( ( memcmp( ( void * ) xISRStack, ( void * ) ucExpectedStackBytes, sizeof( ucExpectedStackBytes ) ) == 0 ) )
#else
    /* Define the function away. */
    #define portCHECK_ISR_STACK()
#endif /* configCHECK_FOR_STACK_OVERFLOW > 2 */

/*-----------------------------------------------------------*/

#if( configMTIME_BASE_ADDRESS != 0 ) && ( configMTIMECMP_BASE_ADDRESS != 0 )

    void vPortSetupTimerInterrupt( void )
    {
    uint32_t ulCurrentTimeHigh, ulCurrentTimeLow;
    volatile uint32_t * const pulTimeHigh = ( volatile uint32_t * const ) ( ( configMTIME_BASE_ADDRESS ) + 4UL ); /* 8-byte typer so high 32-bit word is 4 bytes up. */
    volatile uint32_t * const pulTimeLow = ( volatile uint32_t * const ) ( configMTIME_BASE_ADDRESS );
    volatile uint32_t ulHartId;

        __asm volatile( "csrr %0, mhartid" : "=r"( ulHartId ) );
        pullMachineTimerCompareRegister  = ( volatile uint64_t * ) ( ullMachineTimerCompareRegisterBase + ( ulHartId * sizeof( uint64_t ) ) );

        do
        {
            ulCurrentTimeHigh = *pulTimeHigh;
            ulCurrentTimeLow = *pulTimeLow;
        } while( ulCurrentTimeHigh != *pulTimeHigh );

        ullNextTime = ( uint64_t ) ulCurrentTimeHigh;
        ullNextTime <<= 32ULL; /* High 4-byte word is 32-bits up. */
        ullNextTime |= ( uint64_t ) ulCurrentTimeLow;
        ullNextTime += ( uint64_t ) uxTimerIncrementsForOneTick;
        *pullMachineTimerCompareRegister = ullNextTime;

        /* Prepare the time to use after the next tick interrupt. */
        ullNextTime += ( uint64_t ) uxTimerIncrementsForOneTick;
    }

#else
void xPortCtxHandler( void );
/* There is no machine timer under the hypervisor - the RVM virtual timer
provides the tick, and the RVM context handler provides the switching. */
void vPortSetupTimerInterrupt( void )
{
    /* Hook the tick and the context switch handler. */
    RVM_Virt_Tim_Reg(xPortSysTickHandler);
    RVM_Virt_Ctx_Reg(xPortCtxHandler);
}

#endif /* ( configMTIME_BASE_ADDRESS != 0 ) && ( configMTIME_BASE_ADDRESS != 0 ) */
/*-----------------------------------------------------------*/

BaseType_t xPortStartScheduler( void )
{
extern void xPortStartFirstTask( void );

    /* If there is a CLINT then it is ok to use the default implementation
    in this file, otherwise vPortSetupTimerInterrupt() must be implemented to
    configure whichever clock is to be used to generate the tick interrupt. */
    vPortSetupTimerInterrupt();
    /* Initialise the critical nesting count ready for the first task. */
    uxCriticalNesting = 0;
    xPortStartFirstTask();

    /* Should not get here as after calling xPortStartFirstTask() only tasks
    should be executing. */
    return pdFAIL;
}
/*-----------------------------------------------------------*/

void vPortEndScheduler( void )
{
    /* Not implemented. */
    for( ;; );
}
/*-----------------------------------------------------------*/
void xPortSysTickHandler( void )
{
    portDISABLE_INTERRUPTS();
    if( xTaskIncrementTick() != pdFALSE )
    {
        /* Trigger yield. */
    }
    portENABLE_INTERRUPTS();
}

extern PRIVILEGED_DATA struct tskTaskControlBlock * volatile pxCurrentTCB;
void xPortCtxHandler( void )
{
    StackType_t * pulStackPointer;
    uint32_t ulMStatus;
    
    /* ADDI     sp,sp,-31*4         Save registers
     * SW       x31,30*4(sp)
     * SW       x30,29*4(sp)
     * SW       x29,28*4(sp)
     * SW       x28,27*4(sp)
     * SW       x27,26*4(sp)
     * SW       x26,25*4(sp)
     * SW       x25,24*4(sp)
     * SW       x24,23*4(sp)
     * SW       x23,22*4(sp)
     * SW       x22,21*4(sp)
     * SW       x21,20*4(sp)
     * SW       x20,19*4(sp)
     * SW       x19,18*4(sp)
     * SW       x18,17*4(sp)
     * SW       x17,16*4(sp)
     * SW       x16,15*4(sp)
     * SW       x15,14*4(sp)
     * SW       x14,13*4(sp)
     * SW       x13,12*4(sp)
     * SW       x12,11*4(sp)
     * SW       x11,10*4(sp)
     * SW       x10,9*4(sp)
     * SW       x9,8*4(sp)
     * SW       x8,7*4(sp)
     * SW       x7,6*4(sp)
     * SW       x6,5*4(sp)
     * SW       x5,4*4(sp)
     * SW       x4,3*4(sp)
     * SW       x3,2*4(sp)
     * SW       x1,1*4(sp)
     * CSRR     a0,mepc
     * SW       a0,1*4(sp)
     * CSRR     a0,mstatus */
    pulStackPointer=((uint32_t*)(RVM_REG->Reg.X2_SP))-31U;
    pulStackPointer[30U]=RVM_REG->Reg.X31_T6;
    pulStackPointer[29U]=RVM_REG->Reg.X30_T5;
    pulStackPointer[28U]=RVM_REG->Reg.X29_T4;
    pulStackPointer[27U]=RVM_REG->Reg.X28_T3;
    pulStackPointer[26U]=RVM_REG->Reg.X27_S11;
    pulStackPointer[25U]=RVM_REG->Reg.X26_S10;
    pulStackPointer[24U]=RVM_REG->Reg.X25_S9;
    pulStackPointer[23U]=RVM_REG->Reg.X24_S8;
    pulStackPointer[22U]=RVM_REG->Reg.X23_S7;
    pulStackPointer[21U]=RVM_REG->Reg.X22_S6;
    pulStackPointer[20U]=RVM_REG->Reg.X21_S5;
    pulStackPointer[19U]=RVM_REG->Reg.X20_S4;
    pulStackPointer[18U]=RVM_REG->Reg.X19_S3;
    pulStackPointer[17U]=RVM_REG->Reg.X18_S2;
    pulStackPointer[16U]=RVM_REG->Reg.X17_A7;
    pulStackPointer[15U]=RVM_REG->Reg.X16_A6;
    pulStackPointer[14U]=RVM_REG->Reg.X15_A5;
    pulStackPointer[13U]=RVM_REG->Reg.X14_A4;
    pulStackPointer[12U]=RVM_REG->Reg.X13_A3;
    pulStackPointer[11U]=RVM_REG->Reg.X12_A2;
    pulStackPointer[10U]=RVM_REG->Reg.X11_A1;
    pulStackPointer[9U]=RVM_REG->Reg.X10_A0;
    pulStackPointer[8U]=RVM_REG->Reg.X9_S1;
    pulStackPointer[7U]=RVM_REG->Reg.X8_S0_FP;
    pulStackPointer[6U]=RVM_REG->Reg.X7_T2;
    pulStackPointer[5U]=RVM_REG->Reg.X6_T1;
    pulStackPointer[4U]=RVM_REG->Reg.X5_T0;
    pulStackPointer[3U]=RVM_REG->Reg.X4_TP;
    pulStackPointer[2U]=RVM_REG->Reg.X3_GP;
    pulStackPointer[1U]=RVM_REG->Reg.X1_RA;
    pulStackPointer[0U]=RVM_REG->Reg.PC;
    ulMStatus=RVM_REG->Reg.MSTATUS;   /* Read mstatus to decide FPU status, but don't save yet */

#if( configUSE_PORT_RVF != 0 ) || ( configUSE_PORT_RVD != 0 )
    /* LUI      a1,4                See if FPU is used (mstatus.fs[1]==1)
     * AND      a1,a1,a0
     * BEQZ     a1,skip */
    if( ( ulMStatus & portFPU_CONTEXT_DIRTY_BIT ) != 0U )
    {
#endif
#if( configUSE_PORT_RVD != 0 )
        /* ADDI     sp,sp,-65*4     FPU active, saving context
         * FRCSR    a1
         * SW       a1,32*8(sp)
         * FSD      f31,31*8(sp)
         * FSD      f30,30*8(sp)
         * FSD      f29,29*8(sp)
         * FSD      f28,28*8(sp)
         * FSD      f27,27*8(sp)
         * FSD      f26,26*8(sp)
         * FSD      f25,25*8(sp)
         * FSD      f24,24*8(sp)
         * FSD      f23,23*8(sp)
         * FSD      f22,22*8(sp)
         * FSD      f21,21*8(sp)
         * FSD      f20,20*8(sp)
         * FSD      f19,19*8(sp)
         * FSD      f18,18*8(sp)
         * FSD      f17,17*8(sp)
         * FSD      f16,16*8(sp)
         * FSD      f15,15*8(sp)
         * FSD      f14,14*8(sp)
         * FSD      f13,13*8(sp)
         * FSD      f12,12*8(sp)
         * FSD      f11,11*8(sp)
         * FSD      f10,10*8(sp)
         * FSD      f9,9*8(sp)
         * FSD      f8,8*8(sp)
         * FSD      f7,7*8(sp)
         * FSD      f6,6*8(sp)
         * FSD      f5,5*8(sp)
         * FSD      f4,4*8(sp)
         * FSD      f3,3*8(sp)
         * FSD      f2,2*8(sp)
         * FSD      f1,1*8(sp)
         * FSD      f0,0*8(sp) */
        pulStackPointer-=65U;
        pulStackPointer[64U]=portFPU_CONTEXT->FCSR;
        pulStackPointer[63U]=portFPU_CONTEXT->F31[1U];
        pulStackPointer[62U]=portFPU_CONTEXT->F31[0U];
        pulStackPointer[61U]=portFPU_CONTEXT->F30[1U];
        pulStackPointer[60U]=portFPU_CONTEXT->F30[0U];
        pulStackPointer[59U]=portFPU_CONTEXT->F29[1U];
        pulStackPointer[58U]=portFPU_CONTEXT->F29[0U];
        pulStackPointer[57U]=portFPU_CONTEXT->F28[1U];
        pulStackPointer[56U]=portFPU_CONTEXT->F28[0U];
        pulStackPointer[55U]=portFPU_CONTEXT->F27[1U];
        pulStackPointer[54U]=portFPU_CONTEXT->F27[0U];
        pulStackPointer[53U]=portFPU_CONTEXT->F26[1U];
        pulStackPointer[52U]=portFPU_CONTEXT->F26[0U];
        pulStackPointer[51U]=portFPU_CONTEXT->F25[1U];
        pulStackPointer[50U]=portFPU_CONTEXT->F25[0U];
        pulStackPointer[49U]=portFPU_CONTEXT->F24[1U];
        pulStackPointer[48U]=portFPU_CONTEXT->F24[0U];
        pulStackPointer[47U]=portFPU_CONTEXT->F23[1U];
        pulStackPointer[46U]=portFPU_CONTEXT->F23[0U];
        pulStackPointer[45U]=portFPU_CONTEXT->F22[1U];
        pulStackPointer[44U]=portFPU_CONTEXT->F22[0U];
        pulStackPointer[43U]=portFPU_CONTEXT->F21[1U];
        pulStackPointer[42U]=portFPU_CONTEXT->F21[0U];
        pulStackPointer[41U]=portFPU_CONTEXT->F20[1U];
        pulStackPointer[40U]=portFPU_CONTEXT->F20[0U];
        pulStackPointer[39U]=portFPU_CONTEXT->F19[1U];
        pulStackPointer[38U]=portFPU_CONTEXT->F19[0U];
        pulStackPointer[37U]=portFPU_CONTEXT->F18[1U];
        pulStackPointer[36U]=portFPU_CONTEXT->F18[0U];
        pulStackPointer[35U]=portFPU_CONTEXT->F17[1U];
        pulStackPointer[34U]=portFPU_CONTEXT->F17[0U];
        pulStackPointer[33U]=portFPU_CONTEXT->F16[1U];
        pulStackPointer[32U]=portFPU_CONTEXT->F16[0U];
        pulStackPointer[31U]=portFPU_CONTEXT->F15[1U];
        pulStackPointer[30U]=portFPU_CONTEXT->F15[0U];
        pulStackPointer[29U]=portFPU_CONTEXT->F14[1U];
        pulStackPointer[28U]=portFPU_CONTEXT->F14[0U];
        pulStackPointer[27U]=portFPU_CONTEXT->F13[1U];
        pulStackPointer[26U]=portFPU_CONTEXT->F13[0U];
        pulStackPointer[25U]=portFPU_CONTEXT->F12[1U];
        pulStackPointer[24U]=portFPU_CONTEXT->F12[0U];
        pulStackPointer[23U]=portFPU_CONTEXT->F11[1U];
        pulStackPointer[22U]=portFPU_CONTEXT->F11[0U];
        pulStackPointer[21U]=portFPU_CONTEXT->F10[1U];
        pulStackPointer[20U]=portFPU_CONTEXT->F10[0U];
        pulStackPointer[19U]=portFPU_CONTEXT->F9[1U];
        pulStackPointer[18U]=portFPU_CONTEXT->F9[0U];
        pulStackPointer[17U]=portFPU_CONTEXT->F8[1U];
        pulStackPointer[16U]=portFPU_CONTEXT->F8[0U];
        pulStackPointer[15U]=portFPU_CONTEXT->F7[1U];
        pulStackPointer[14U]=portFPU_CONTEXT->F7[0U];
        pulStackPointer[13U]=portFPU_CONTEXT->F6[1U];
        pulStackPointer[12U]=portFPU_CONTEXT->F6[0U];
        pulStackPointer[11U]=portFPU_CONTEXT->F5[1U];
        pulStackPointer[10U]=portFPU_CONTEXT->F5[0U];
        pulStackPointer[9U]=portFPU_CONTEXT->F4[1U];
        pulStackPointer[8U]=portFPU_CONTEXT->F4[0U];
        pulStackPointer[7U]=portFPU_CONTEXT->F3[1U];
        pulStackPointer[6U]=portFPU_CONTEXT->F3[0U];
        pulStackPointer[5U]=portFPU_CONTEXT->F2[1U];
        pulStackPointer[4U]=portFPU_CONTEXT->F2[0U];
        pulStackPointer[3U]=portFPU_CONTEXT->F1[1U];
        pulStackPointer[2U]=portFPU_CONTEXT->F1[0U];
        pulStackPointer[1U]=portFPU_CONTEXT->F0[1U];
        pulStackPointer[0U]=portFPU_CONTEXT->F0[0U];
#elif( configUSE_PORT_RVF != 0 )
        /* ADDI    sp,sp,-33*4      FPU active, saving context
         * FRCSR   a1
         * SW      a1,32*4(sp)
         * FSW     f31, 31*4(sp)
         * FSW     f30, 30*4(sp)
         * FSW     f29, 29*4(sp)
         * FSW     f28, 28*4(sp)
         * FSW     f27, 27*4(sp)
         * FSW     f26, 26*4(sp)
         * FSW     f25, 25*4(sp)
         * FSW     f24, 24*4(sp)
         * FSW     f23, 23*4(sp)
         * FSW     f22, 22*4(sp)
         * FSW     f21, 21*4(sp)
         * FSW     f20, 20*4(sp)
         * FSW     f19, 19*4(sp)
         * FSW     f18, 18*4(sp)
         * FSW     f17, 17*4(sp)
         * FSW     f16, 16*4(sp)
         * FSW     f15, 15*4(sp)
         * FSW     f14, 14*4(sp)
         * FSW     f13, 13*4(sp)
         * FSW     f12, 12*4(sp)
         * FSW     f11, 11*4(sp)
         * FSW     f10, 10*4(sp)
         * FSW     f9, 9*4(sp)
         * FSW     f8, 8*4(sp)
         * FSW     f7, 7*4(sp)
         * FSW     f6, 6*4(sp)
         * FSW     f5, 5*4(sp)
         * FSW     f4, 4*4(sp)
         * FSW     f3, 3*4(sp)
         * FSW     f2, 2*4(sp)
         * FSW     f1, 1*4(sp)
         * FSW     f0, 0*4(sp) */
        pulStackPointer-=33U;
        pulStackPointer[32U]=portFPU_CONTEXT->FCSR;
        pulStackPointer[31U]=portFPU_CONTEXT->F31;
        pulStackPointer[30U]=portFPU_CONTEXT->F30;
        pulStackPointer[29U]=portFPU_CONTEXT->F29;
        pulStackPointer[28U]=portFPU_CONTEXT->F28;
        pulStackPointer[27U]=portFPU_CONTEXT->F27;
        pulStackPointer[26U]=portFPU_CONTEXT->F26;
        pulStackPointer[25U]=portFPU_CONTEXT->F25;
        pulStackPointer[24U]=portFPU_CONTEXT->F24;
        pulStackPointer[23U]=portFPU_CONTEXT->F23;
        pulStackPointer[22U]=portFPU_CONTEXT->F22;
        pulStackPointer[21U]=portFPU_CONTEXT->F21;
        pulStackPointer[20U]=portFPU_CONTEXT->F20;
        pulStackPointer[19U]=portFPU_CONTEXT->F19;
        pulStackPointer[18U]=portFPU_CONTEXT->F18;
        pulStackPointer[17U]=portFPU_CONTEXT->F17;
        pulStackPointer[16U]=portFPU_CONTEXT->F16;
        pulStackPointer[15U]=portFPU_CONTEXT->F15;
        pulStackPointer[14U]=portFPU_CONTEXT->F14;
        pulStackPointer[13U]=portFPU_CONTEXT->F13;
        pulStackPointer[12U]=portFPU_CONTEXT->F12;
        pulStackPointer[11U]=portFPU_CONTEXT->F11;
        pulStackPointer[10U]=portFPU_CONTEXT->F10;
        pulStackPointer[9U]=portFPU_CONTEXT->F9;
        pulStackPointer[8U]=portFPU_CONTEXT->F8;
        pulStackPointer[7U]=portFPU_CONTEXT->F7;
        pulStackPointer[6U]=portFPU_CONTEXT->F6;
        pulStackPointer[5U]=portFPU_CONTEXT->F5;
        pulStackPointer[4U]=portFPU_CONTEXT->F4;
        pulStackPointer[3U]=portFPU_CONTEXT->F3;
        pulStackPointer[2U]=portFPU_CONTEXT->F2;
        pulStackPointer[1U]=portFPU_CONTEXT->F1;
        pulStackPointer[0U]=portFPU_CONTEXT->F0;
#endif
#if( configUSE_PORT_RVF != 0 ) || ( configUSE_PORT_RVD != 0 )
    }
#endif

    /* ADDI     sp,sp,-4            Save mstatus
     * SW       a0,0*4(sp) */
    pulStackPointer--;
    pulStackPointer[0U]=ulMStatus;

    /* Store all the user-accessible hypercall structure to stack */
    pulStackPointer-=5U;
    pulStackPointer[4U]=RVM_STATE->Usr.Param[3];
    pulStackPointer[3U]=RVM_STATE->Usr.Param[2];
    pulStackPointer[2U]=RVM_STATE->Usr.Param[1];
    pulStackPointer[1U]=RVM_STATE->Usr.Param[0];
    pulStackPointer[0U]=RVM_STATE->Usr.Number;

    /* STR      R0,[R2] */
    *((uint32_t*)pxCurrentTCB)=(uint32_t)pulStackPointer;
    
    vTaskSwitchContext();
    
    /* LDR      R1,[R3] */
    /* LDR      R0,[R1] */
    pulStackPointer=(uint32_t*)(*((uint32_t*)pxCurrentTCB));

    /* Load the user-accessible hypercall structure to stack */
    RVM_STATE->Usr.Number=pulStackPointer[0U];
    RVM_STATE->Usr.Param[0]=pulStackPointer[1U];
    RVM_STATE->Usr.Param[1]=pulStackPointer[2U];
    RVM_STATE->Usr.Param[2]=pulStackPointer[3U];
    RVM_STATE->Usr.Param[3]=pulStackPointer[4U];
    pulStackPointer+=5U;

    /* LW       a0,0*4(sp)          Read mstatus to decide FPU status, but don't load yet
     * ADDI     sp,sp,4 */
    ulMStatus=pulStackPointer[0U];
    pulStackPointer++;

#if( configUSE_PORT_RVF != 0 ) || ( configUSE_PORT_RVD != 0 )
    /* LUI      a1,4                See if FPU is used (mstatus.fs[1]==1)
     * AND      a1,a1,a0
     * BEQZ     a1,skip */
    if( ( ulMStatus & portFPU_CONTEXT_DIRTY_BIT ) != 0U )
    {
#endif
#if( configUSE_PORT_RVD != 0 )
        /* FLD      f0,0*8(sp)      FPU active, loading context
         * FLD      f1,1*8(sp)
         * FLD      f2,2*8(sp)
         * FLD      f3,3*8(sp)
         * FLD      f4,4*8(sp)
         * FLD      f5,5*8(sp)
         * FLD      f6,6*8(sp)
         * FLD      f7,7*8(sp)
         * FLD      f8,8*8(sp)
         * FLD      f9,9*8(sp)
         * FLD      f10,10*8(sp)
         * FLD      f11,11*8(sp)
         * FLD      f12,12*8(sp)
         * FLD      f13,13*8(sp)
         * FLD      f14,14*8(sp)
         * FLD      f15,15*8(sp)
         * FLD      f16,16*8(sp)
         * FLD      f17,17*8(sp)
         * FLD      f18,18*8(sp)
         * FLD      f19,19*8(sp)
         * FLD      f20,20*8(sp)
         * FLD      f21,21*8(sp)
         * FLD      f22,22*8(sp)
         * FLD      f23,23*8(sp)
         * FLD      f24,24*8(sp)
         * FLD      f25,25*8(sp)
         * FLD      f26,26*8(sp)
         * FLD      f27,27*8(sp)
         * FLD      f28,28*8(sp)
         * FLD      f29,29*8(sp)
         * FLD      f30,30*8(sp)
         * FLD      f31,31*8(sp)
         * LW       a1,32*8(sp)
         * FSCSR    a1
         * ADDI     sp,sp,65*4 */
        portFPU_CONTEXT->F0[0U]=pulStackPointer[0U];
        portFPU_CONTEXT->F0[1U]=pulStackPointer[1U];
        portFPU_CONTEXT->F1[0U]=pulStackPointer[2U];
        portFPU_CONTEXT->F1[1U]=pulStackPointer[3U];
        portFPU_CONTEXT->F2[0U]=pulStackPointer[4U];
        portFPU_CONTEXT->F2[1U]=pulStackPointer[5U];
        portFPU_CONTEXT->F3[0U]=pulStackPointer[6U];
        portFPU_CONTEXT->F3[1U]=pulStackPointer[7U];
        portFPU_CONTEXT->F4[0U]=pulStackPointer[8U];
        portFPU_CONTEXT->F4[1U]=pulStackPointer[9U];
        portFPU_CONTEXT->F5[0U]=pulStackPointer[10U];
        portFPU_CONTEXT->F5[1U]=pulStackPointer[11U];
        portFPU_CONTEXT->F6[0U]=pulStackPointer[12U];
        portFPU_CONTEXT->F6[1U]=pulStackPointer[13U];
        portFPU_CONTEXT->F7[0U]=pulStackPointer[14U];
        portFPU_CONTEXT->F7[1U]=pulStackPointer[15U];
        portFPU_CONTEXT->F8[0U]=pulStackPointer[16U];
        portFPU_CONTEXT->F8[1U]=pulStackPointer[17U];
        portFPU_CONTEXT->F9[0U]=pulStackPointer[18U];
        portFPU_CONTEXT->F9[1U]=pulStackPointer[19U];
        portFPU_CONTEXT->F10[0U]=pulStackPointer[20U];
        portFPU_CONTEXT->F10[1U]=pulStackPointer[21U];
        portFPU_CONTEXT->F11[0U]=pulStackPointer[22U];
        portFPU_CONTEXT->F11[1U]=pulStackPointer[23U];
        portFPU_CONTEXT->F12[0U]=pulStackPointer[24U];
        portFPU_CONTEXT->F12[1U]=pulStackPointer[25U];
        portFPU_CONTEXT->F13[0U]=pulStackPointer[26U];
        portFPU_CONTEXT->F13[1U]=pulStackPointer[27U];
        portFPU_CONTEXT->F14[0U]=pulStackPointer[28U];
        portFPU_CONTEXT->F14[1U]=pulStackPointer[29U];
        portFPU_CONTEXT->F15[0U]=pulStackPointer[30U];
        portFPU_CONTEXT->F15[1U]=pulStackPointer[31U];
        portFPU_CONTEXT->F16[0U]=pulStackPointer[32U];
        portFPU_CONTEXT->F16[1U]=pulStackPointer[33U];
        portFPU_CONTEXT->F17[0U]=pulStackPointer[34U];
        portFPU_CONTEXT->F17[1U]=pulStackPointer[35U];
        portFPU_CONTEXT->F18[0U]=pulStackPointer[36U];
        portFPU_CONTEXT->F18[1U]=pulStackPointer[37U];
        portFPU_CONTEXT->F19[0U]=pulStackPointer[38U];
        portFPU_CONTEXT->F19[1U]=pulStackPointer[39U];
        portFPU_CONTEXT->F20[0U]=pulStackPointer[40U];
        portFPU_CONTEXT->F20[1U]=pulStackPointer[41U];
        portFPU_CONTEXT->F21[0U]=pulStackPointer[42U];
        portFPU_CONTEXT->F21[1U]=pulStackPointer[43U];
        portFPU_CONTEXT->F22[0U]=pulStackPointer[44U];
        portFPU_CONTEXT->F22[1U]=pulStackPointer[45U];
        portFPU_CONTEXT->F23[0U]=pulStackPointer[46U];
        portFPU_CONTEXT->F23[1U]=pulStackPointer[47U];
        portFPU_CONTEXT->F24[0U]=pulStackPointer[48U];
        portFPU_CONTEXT->F24[1U]=pulStackPointer[49U];
        portFPU_CONTEXT->F25[0U]=pulStackPointer[50U];
        portFPU_CONTEXT->F25[1U]=pulStackPointer[51U];
        portFPU_CONTEXT->F26[0U]=pulStackPointer[52U];
        portFPU_CONTEXT->F26[1U]=pulStackPointer[53U];
        portFPU_CONTEXT->F27[0U]=pulStackPointer[54U];
        portFPU_CONTEXT->F27[1U]=pulStackPointer[55U];
        portFPU_CONTEXT->F28[0U]=pulStackPointer[56U];
        portFPU_CONTEXT->F28[1U]=pulStackPointer[57U];
        portFPU_CONTEXT->F29[0U]=pulStackPointer[58U];
        portFPU_CONTEXT->F29[1U]=pulStackPointer[59U];
        portFPU_CONTEXT->F30[0U]=pulStackPointer[60U];
        portFPU_CONTEXT->F30[1U]=pulStackPointer[61U];
        portFPU_CONTEXT->F31[0U]=pulStackPointer[62U];
        portFPU_CONTEXT->F31[1U]=pulStackPointer[63U];
        portFPU_CONTEXT->FCSR=pulStackPointer[64U];
        pulStackPointer+=65U;
#elif( configUSE_PORT_RVF != 0 )
        /* FLW     f0, 0*4(sp)      FPU active, loading context
         * FLW     f1, 1*4(sp)
         * FLW     f2, 2*4(sp)
         * FLW     f3, 3*4(sp)
         * FLW     f4, 4*4(sp)
         * FLW     f5, 5*4(sp)
         * FLW     f6, 6*4(sp)
         * FLW     f7, 7*4(sp)
         * FLW     f8, 8*4(sp)
         * FLW     f9, 9*4(sp)
         * FLW     f10, 10*4(sp)
         * FLW     f11, 11*4(sp)
         * FLW     f12, 12*4(sp)
         * FLW     f13, 13*4(sp)
         * FLW     f14, 14*4(sp)
         * FLW     f15, 15*4(sp)
         * FLW     f16, 16*4(sp)
         * FLW     f17, 17*4(sp)
         * FLW     f18, 18*4(sp)
         * FLW     f19, 19*4(sp)
         * FLW     f20, 20*4(sp)
         * FLW     f21, 21*4(sp)
         * FLW     f22, 22*4(sp)
         * FLW     f23, 23*4(sp)
         * FLW     f24, 24*4(sp)
         * FLW     f25, 25*4(sp)
         * FLW     f26, 26*4(sp)
         * FLW     f27, 27*4(sp)
         * FLW     f28, 28*4(sp)
         * FLW     f29, 29*4(sp)
         * FLW     f30, 30*4(sp)
         * FLW     f31, 31*4(sp)
         * LW      a1,32*4(sp)
         * FSCSR   a1
         * ADDI    sp,sp,33*4 */
        portFPU_CONTEXT->F0=pulStackPointer[0U];
        portFPU_CONTEXT->F1=pulStackPointer[1U];
        portFPU_CONTEXT->F2=pulStackPointer[2U];
        portFPU_CONTEXT->F3=pulStackPointer[3U];
        portFPU_CONTEXT->F4=pulStackPointer[4U];
        portFPU_CONTEXT->F5=pulStackPointer[5U];
        portFPU_CONTEXT->F6=pulStackPointer[6U];
        portFPU_CONTEXT->F7=pulStackPointer[7U];
        portFPU_CONTEXT->F8=pulStackPointer[8U];
        portFPU_CONTEXT->F9=pulStackPointer[9U];
        portFPU_CONTEXT->F10=pulStackPointer[10U];
        portFPU_CONTEXT->F11=pulStackPointer[11U];
        portFPU_CONTEXT->F12=pulStackPointer[12U];
        portFPU_CONTEXT->F13=pulStackPointer[13U];
        portFPU_CONTEXT->F14=pulStackPointer[14U];
        portFPU_CONTEXT->F15=pulStackPointer[15U];
        portFPU_CONTEXT->F16=pulStackPointer[16U];
        portFPU_CONTEXT->F17=pulStackPointer[17U];
        portFPU_CONTEXT->F18=pulStackPointer[18U];
        portFPU_CONTEXT->F19=pulStackPointer[19U];
        portFPU_CONTEXT->F20=pulStackPointer[20U];
        portFPU_CONTEXT->F21=pulStackPointer[21U];
        portFPU_CONTEXT->F22=pulStackPointer[22U];
        portFPU_CONTEXT->F23=pulStackPointer[23U];
        portFPU_CONTEXT->F24=pulStackPointer[24U];
        portFPU_CONTEXT->F25=pulStackPointer[25U];
        portFPU_CONTEXT->F26=pulStackPointer[26U];
        portFPU_CONTEXT->F27=pulStackPointer[27U];
        portFPU_CONTEXT->F28=pulStackPointer[28U];
        portFPU_CONTEXT->F29=pulStackPointer[29U];
        portFPU_CONTEXT->F30=pulStackPointer[30U];
        portFPU_CONTEXT->F31=pulStackPointer[31U];
        portFPU_CONTEXT->FCSR=pulStackPointer[32U];
        pulStackPointer+=33U;
#endif
#if( configUSE_PORT_RVF != 0 ) || ( configUSE_PORT_RVD != 0 )
    }
#endif

    /* LI       a1,0x1880           Load mstatus - force M mode with enabled interrupt
     * OR       a0,a0,a1
     * CSRW     mstatus,a0
     * The RVM port is actually forcing U mode; we keep the original for reference */
    RVM_REG->Reg.MSTATUS=ulMStatus;

    /* LW       a0,0*4(sp)          Load pc
     * CSRW     mepc,a0
     * LW       x1,1*4(sp)          Load registers
     * LW       x3,2*4(sp)
     * LW       x4,3*4(sp)
     * LW       x5,4*4(sp)
     * LW       x6,5*4(sp)
     * LW       x7,6*4(sp)
     * LW       x8,7*4(sp)
     * LW       x9,8*4(sp)
     * LW       x10,9*4(sp)
     * LW       x11,10*4(sp)
     * LW       x12,11*4(sp)
     * LW       x13,12*4(sp)
     * LW       x14,13*4(sp)
     * LW       x15,14*4(sp)
     * LW       x16,15*4(sp)
     * LW       x17,16*4(sp)
     * LW       x18,17*4(sp)
     * LW       x19,18*4(sp)
     * LW       x20,19*4(sp)
     * LW       x21,20*4(sp)
     * LW       x22,21*4(sp)
     * LW       x23,22*4(sp)
     * LW       x24,23*4(sp)
     * LW       x25,24*4(sp)
     * LW       x26,25*4(sp)
     * LW       x27,26*4(sp)
     * LW       x28,27*4(sp)
     * LW       x29,28*4(sp)
     * LW       x30,29*4(sp)
     * LW       x31,30*4(sp)
     * ADDI     sp,sp,31*4 */
    RVM_REG->Reg.PC=pulStackPointer[0U];
    RVM_REG->Reg.X1_RA=pulStackPointer[1U];
    RVM_REG->Reg.X3_GP=pulStackPointer[2U];
    RVM_REG->Reg.X4_TP=pulStackPointer[3U];
    RVM_REG->Reg.X5_T0=pulStackPointer[4U];
    RVM_REG->Reg.X6_T1=pulStackPointer[5U];
    RVM_REG->Reg.X7_T2=pulStackPointer[6U];
    RVM_REG->Reg.X8_S0_FP=pulStackPointer[7U];
    RVM_REG->Reg.X9_S1=pulStackPointer[8U];
    RVM_REG->Reg.X10_A0=pulStackPointer[9U];
    RVM_REG->Reg.X11_A1=pulStackPointer[10U];
    RVM_REG->Reg.X12_A2=pulStackPointer[11U];
    RVM_REG->Reg.X13_A3=pulStackPointer[12U];
    RVM_REG->Reg.X14_A4=pulStackPointer[13U];
    RVM_REG->Reg.X15_A5=pulStackPointer[14U];
    RVM_REG->Reg.X16_A6=pulStackPointer[15U];
    RVM_REG->Reg.X17_A7=pulStackPointer[16U];
    RVM_REG->Reg.X18_S2=pulStackPointer[17U];
    RVM_REG->Reg.X19_S3=pulStackPointer[18U];
    RVM_REG->Reg.X20_S4=pulStackPointer[19U];
    RVM_REG->Reg.X21_S5=pulStackPointer[20U];
    RVM_REG->Reg.X22_S6=pulStackPointer[21U];
    RVM_REG->Reg.X23_S7=pulStackPointer[22U];
    RVM_REG->Reg.X24_S8=pulStackPointer[23U];
    RVM_REG->Reg.X25_S9=pulStackPointer[24U];
    RVM_REG->Reg.X26_S10=pulStackPointer[25U];
    RVM_REG->Reg.X27_S11=pulStackPointer[26U];
    RVM_REG->Reg.X28_T3=pulStackPointer[27U];
    RVM_REG->Reg.X29_T4=pulStackPointer[28U];
    RVM_REG->Reg.X30_T5=pulStackPointer[29U];
    RVM_REG->Reg.X31_T6=pulStackPointer[30U];
    RVM_REG->Reg.X2_SP=(uint32_t)(pulStackPointer+31U);

    /* MRET */
    return;
}

/*-----------------------------------------------------------*/
void vPortEnterCritical( void )
{
    portDISABLE_INTERRUPTS();
    uxCriticalNesting++;
}

/*-----------------------------------------------------------*/
void vPortExitCritical( void )
{
    configASSERT( uxCriticalNesting );
    uxCriticalNesting--;

    if( uxCriticalNesting == 0 )
    {
        portENABLE_INTERRUPTS();
    }
}
/*-----------------------------------------------------------*/
portUBASE_TYPE xPortSetInterruptMask(void)
{
    return 0;
}

/*-----------------------------------------------------------*/
extern uint32_t __RVM_Global;
StackType_t *pxPortInitialiseStack( StackType_t *pxTopOfStack, TaskFunction_t pxCode, void *pvParameters )
{
    /* x31...x11 */
    pxTopOfStack--;
    *pxTopOfStack = 0x31313131U;
    pxTopOfStack--;
    *pxTopOfStack = 0x30303030U;
    pxTopOfStack--;
    *pxTopOfStack = 0x29292929U;
    pxTopOfStack--;
    *pxTopOfStack = 0x28282828U;
    pxTopOfStack--;
    *pxTopOfStack = 0x27272727U;
    pxTopOfStack--;
    *pxTopOfStack = 0x26262626U;
    pxTopOfStack--;
    *pxTopOfStack = 0x25252525U;
    pxTopOfStack--;
    *pxTopOfStack = 0x24242424U;
    pxTopOfStack--;
    *pxTopOfStack = 0x23232323U;
    pxTopOfStack--;
    *pxTopOfStack = 0x22222222U;
    pxTopOfStack--;
    *pxTopOfStack = 0x21212121U;
    pxTopOfStack--;
    *pxTopOfStack = 0x20202020U;
    pxTopOfStack--;
    *pxTopOfStack = 0x19191919U;
    pxTopOfStack--;
    *pxTopOfStack = 0x18181818U;
    pxTopOfStack--;
    *pxTopOfStack = 0x17171717U;
    pxTopOfStack--;
    *pxTopOfStack = 0x16161616U;
    pxTopOfStack--;
    *pxTopOfStack = 0x15151515U;
    pxTopOfStack--;
    *pxTopOfStack = 0x14141414U;
    pxTopOfStack--;
    *pxTopOfStack = 0x13131313U;
    pxTopOfStack--;
    *pxTopOfStack = 0x12121212U;
    pxTopOfStack--;
    *pxTopOfStack = 0x11111111U;
    /* push pvParameters */
    pxTopOfStack--;
    *pxTopOfStack = (uint32_t)pvParameters;
    /* x9...x3 */
    pxTopOfStack--;
    *pxTopOfStack = 0x09090909U;
    pxTopOfStack--;
    *pxTopOfStack = 0x08080808U;
    pxTopOfStack--;
    *pxTopOfStack = 0x07070707U;
    pxTopOfStack--;
    *pxTopOfStack = 0x06060606U;
    pxTopOfStack--;
    *pxTopOfStack = 0x05050505U;
    pxTopOfStack--;
    *pxTopOfStack = 0x04040404U;
    pxTopOfStack--;
    *pxTopOfStack = (uint32_t)(&__RVM_Global);

    /* x1 - return address */
    pxTopOfStack--;
    *pxTopOfStack = 0x0U;

    /* push pxCode */
    pxTopOfStack--;
    *pxTopOfStack = (uint32_t)pxCode;

    /* mstatus */
    pxTopOfStack--;
#if( configUSE_PORT_RVF != 0 ) || ( configUSE_PORT_RVD != 0 )
    /* U-mode with the FPU in its initial state - the first floating point
     * instruction executed by the task will mark the state dirty, and the
     * context will then be lazily saved from there on. */
    *pxTopOfStack = 0x0088U|0x2000U;
#else
    *pxTopOfStack = 0x0088U;
#endif

    /* Clean up hypercall registers */
    pxTopOfStack--;
    *pxTopOfStack = 0U;
    pxTopOfStack--;
    *pxTopOfStack = 0U;
    pxTopOfStack--;
    *pxTopOfStack = 0U;
    pxTopOfStack--;
    *pxTopOfStack = 0U;
    pxTopOfStack--;
    *pxTopOfStack = 0U;

    return pxTopOfStack;
}
/*-----------------------------------------------------------*/

static void prvTaskExitError( void )
{
    /* A function that implements a task must not exit or attempt to return to
    its caller as there is nothing to return to.  If a task wants to exit it
    should instead call vTaskDelete( NULL ).

    Artificially force an assert() to be triggered if configASSERT() is
    defined, then stop here so application writers can catch the error. */
    configASSERT( uxCriticalNesting == ~0UL );
    portDISABLE_INTERRUPTS();
    for( ;; );
}
/*-----------------------------------------------------------*/