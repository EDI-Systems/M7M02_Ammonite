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

    This file implements the FreeRTOS porting layer for the ARMv6-M
    architecture under the RVM hypervisor.  It maps FreeRTOS primitives
    onto the RVM system call interface so that FreeRTOS runs as an
    unprivileged guest of the hypervisor.
*/

/*-----------------------------------------------------------
 * Implementation of functions defined in portable.h for the ARMv6-M RVM port.
 *----------------------------------------------------------*/

/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "rvm.h"

/* Constants required to set up the initial stack. */
#define portINITIAL_XPSR                        ( 0x01000000 )

/* Let the user override the pre-loading of the initial LR with the address of
prvTaskExitError() in case it messes up unwinding of the stack in the
debugger. */
#ifdef configTASK_RETURN_ADDRESS
    #define portTASK_RETURN_ADDRESS             configTASK_RETURN_ADDRESS
#else
    #define portTASK_RETURN_ADDRESS             prvTaskExitError
#endif

/* Each task maintains its own interrupt status in the critical nesting
variable. */
static UBaseType_t uxCriticalNesting = 0xaaaaaaaa;

/* Set when portYIELD() is called while the critical nesting count is raised;
 * vPortExitCritical() runs the deferred switch when the count hits zero -
 * the software equivalent of the official ports' pending PendSV. */
static BaseType_t xPortYieldPend = pdFALSE;

/*
 * Returns the interrupt mask state before the call - the RVM virtual
 * interrupts cannot be masked from the A6M BASEPRI, so this is a stub
 * that keeps the FreeRTOS kernel API shape.
 */
uint32_t ulPortRaiseBASEPRI( void )
{
    return 0U;
}

/*
 * Setup the timer to generate the tick interrupts.
 */
static void prvSetupTimerInterrupt( void );

/*
 * Exception handlers.
 */
void xPortPendSVHandler( void );
void xPortSysTickHandler( void );
void vPortSVCHandler( void );

/*
 * Start first task is a separate function so it can be tested in isolation.
 */
static void vPortStartFirstTask( void );

/*
 * Used to catch tasks that attempt to return from their implementing function.
 */
static void prvTaskExitError( void );

/*-----------------------------------------------------------*/

/*
 * See header file for description.
 */
StackType_t *pxPortInitialiseStack( StackType_t *pxTopOfStack, TaskFunction_t pxCode, void *pvParameters )
{
    /* Simulate the stack frame as it would be created by a context switch
    interrupt. */
    pxTopOfStack--; /* Offset added to account for the way the MCU uses the stack on entry/exit of interrupts. */
    *pxTopOfStack = portINITIAL_XPSR;   /* xPSR */
    pxTopOfStack--;
    *pxTopOfStack = ( StackType_t ) pxCode; /* PC */
    pxTopOfStack--;
    *pxTopOfStack = ( StackType_t ) prvTaskExitError;   /* LR */
    pxTopOfStack -= 5;  /* R12, R3, R2 and R1. */
    *pxTopOfStack = ( StackType_t ) pvParameters;   /* R0 */
    
    pxTopOfStack--;
    *pxTopOfStack = 0xFFFFFFFDU;
    
    /* R11...R4 */
    pxTopOfStack--;
    *pxTopOfStack = 0x11111111U;
    pxTopOfStack--;
    *pxTopOfStack = 0x10101010U;
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

void vPortSVCHandler( void )
{
    /* This function is no longer used, but retained for backward
    compatibility. */
}
/*-----------------------------------------------------------*/

extern PRIVILEGED_DATA struct tskTaskControlBlock * volatile pxCurrentTCB;
void vPortStartFirstTask( void )
{
    /* Lockdown - not changing this in the future */
    // RVM_Hyp_Vct_Lck();
    /* Enable virtual interrupts */
    RVM_Hyp_Int_Ena();
    
    /* Switch to first stack's entry & PC; startup stack is abandoned.
     * The IAR inline assembler accepts 'LDR Rx,=label' directly, so the
     * manual literal pool of the GCC variant is not needed here. */
    __asm("LDR     R3,=pxCurrentTCB\n"
          "LDR     R1,[R3]     \n"
          "LDR     R0,[R1]     \n"
          "MOV     SP,R0       \n"
          "POP     {R0-R4}     \n"     /* Discard hypercall parameters */
          "POP     {R4-R7}     \n"
          "POP     {R0-R3}     \n"     /* POP {R8-R11} */
          "MOV     R8,R0       \n"
          "MOV     R9,R1       \n"
          "MOV     R10,R2      \n"
          "MOV     R11,R3      \n"
          "POP     {R0}        \n"
          "POP     {R0-R4}     \n"     /* Restore R0-R3, R12 */
          "MOV     R12,R4      \n"
          "POP     {R4}        \n"     /* Restore PC in LR */
          "POP     {R4}        \n"
          "MOV     LR,R4       \n"
          "PUSH    {R4}        \n"     /* Fix up SP/LR */
          "BX      LR          \n");   /* Branch to entry */
}
/*-----------------------------------------------------------*/

/*
 * See header file for description.
 */
BaseType_t xPortStartScheduler( void )
{
    /* Start the timer that generates the tick ISR.  Interrupts are disabled
    here already. */
    prvSetupTimerInterrupt();

    /* Initialise the critical nesting count ready for the first task. */
    uxCriticalNesting = 0;

    /* Start the first task. */
    vPortStartFirstTask();

    /* Should never get here as the tasks will now be executing!  Call the task
    exit error function to prevent compiler warnings about a static function
    not being called in the case that the application writer overrides this
    functionality by defining configTASK_RETURN_ADDRESS. */
    prvTaskExitError();

    /* Should not get here! */
    return 0;
}
/*-----------------------------------------------------------*/

void vPortEndScheduler( void )
{
    /* Not implemented in ports where there is nothing to return to.
    Artificially force an assert. */
    configASSERT( uxCriticalNesting == 1000UL );
}
/*-----------------------------------------------------------*/

void vPortEnterCritical( void )
{
    portDISABLE_INTERRUPTS();
    uxCriticalNesting++;
    __asm( "dsb" );
    __asm( "isb" );
}
/*-----------------------------------------------------------*/

void vPortExitCritical( void )
{
    configASSERT( uxCriticalNesting );
    uxCriticalNesting--;
    if( uxCriticalNesting == 0 )
    {
        /* A yield pended from inside the critical section runs here, while
         * the interrupts are still masked - every exit path of vPortYield()
         * unmasks them itself (same order as RMP_Sched_Unlock). */
        if( xPortYieldPend != pdFALSE )
        {
            xPortYieldPend = pdFALSE;
            vPortYield();
        }
        portENABLE_INTERRUPTS();
    }
}

/*-----------------------------------------------------------*/

#if( configUSE_RVM_OPTIMISED_YIELD != 0 )
    /* The fast-path assembly routine - see portasm.s. */
    extern void vPortYield_Asm( void );
#endif

/*
 * The yield entry point selected by portYIELD().  The official ARMv6-M
 * ports pend the PendSV and only switch once the critical section exits -
 * the kernel does call portYIELD() from inside one (ulTaskNotifyTake et al).
 * RVM_Virt_Int_Unmask() is a flat flag, so yielding under a raised nesting
 * count would open the interrupts in the middle of the caller's critical
 * section; pend instead, and let vPortExitCritical() run the switch when
 * the count hits zero.
 */
void vPortYield( void )
{
    if( uxCriticalNesting != 0U )
    {
        xPortYieldPend = pdTRUE;
        return;
    }

#if( configUSE_RVM_OPTIMISED_YIELD != 0 )
    vPortYield_Asm();
#else
    /* Yield through the hypervisor - simpler, but traps on every switch. */
    RVM_Virt_Yield();
#endif
}
/*-----------------------------------------------------------*/

void xPortPendSVHandler( void )
{
    uint32_t* SP;

    /* MRS      R0,PSP */
    SP=(uint32_t*)(RVM_REG->Reg.SP);
    
    /* LDR      R3,pxCurrentTCB
     * LDR      R2,[R3] */
    
    *(--SP)=RVM_REG->Reg.LR;
    *(--SP)=RVM_REG->Reg.R11;
    *(--SP)=RVM_REG->Reg.R10;
    *(--SP)=RVM_REG->Reg.R9;
    *(--SP)=RVM_REG->Reg.R8;
    *(--SP)=RVM_REG->Reg.R7;
    *(--SP)=RVM_REG->Reg.R6;
    *(--SP)=RVM_REG->Reg.R5;
    *(--SP)=RVM_REG->Reg.R4;
    
    /* Save hypercall parameter to stack */
    *(--SP)=RVM_STATE->Usr.Param[3];
    *(--SP)=RVM_STATE->Usr.Param[2];
    *(--SP)=RVM_STATE->Usr.Param[1];
    *(--SP)=RVM_STATE->Usr.Param[0];
    *(--SP)=RVM_STATE->Usr.Number;

    /* STR      R0,[R2] */
    *((uint32_t*)pxCurrentTCB)=(uint32_t)SP;

    vTaskSwitchContext();

    /* LDR      R1,[R3] */
    /* LDR      R0,[R1] */
    SP=(uint32_t*)(*((uint32_t*)pxCurrentTCB));

    /* Restore hypercall parameter from stack */
    RVM_STATE->Usr.Number=*(SP++);
    RVM_STATE->Usr.Param[0]=*(SP++);
    RVM_STATE->Usr.Param[1]=*(SP++);
    RVM_STATE->Usr.Param[2]=*(SP++);
    RVM_STATE->Usr.Param[3]=*(SP++);

    RVM_REG->Reg.R4=*(SP++);
    RVM_REG->Reg.R5=*(SP++);
    RVM_REG->Reg.R6=*(SP++);
    RVM_REG->Reg.R7=*(SP++);
    RVM_REG->Reg.R8=*(SP++);
    RVM_REG->Reg.R9=*(SP++);
    RVM_REG->Reg.R10=*(SP++);
    RVM_REG->Reg.R11=*(SP++);
    RVM_REG->Reg.LR=*(SP++);

    /* MSR      PSP, R0 */
    RVM_REG->Reg.SP=(uint32_t)SP;

    /* BX       LR */
    return;
}
/*-----------------------------------------------------------*/

void xPortSysTickHandler( void )
{
    portDISABLE_INTERRUPTS();
    
    if(xTaskIncrementTick()!=pdFALSE)
    {
        /* Trigger PendSV */
    }
    
    portENABLE_INTERRUPTS();
}
/*-----------------------------------------------------------*/

/*
 * Setup the systick timer to generate the tick interrupts at the required
 * frequency.
 */
void prvSetupTimerInterrupt( void )
{
    /* Hook Systick & PendSV */
    RVM_Virt_Tim_Reg(xPortSysTickHandler);
    RVM_Virt_Ctx_Reg(xPortPendSVHandler);
}
/*-----------------------------------------------------------*/

