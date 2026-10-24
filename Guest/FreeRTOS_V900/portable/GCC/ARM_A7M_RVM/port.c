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

    This file implements the FreeRTOS porting layer for the ARMv7-M
    architecture under the RVM hypervisor.  It maps FreeRTOS primitives
    onto the RVM system call interface so that FreeRTOS runs as an
    unprivileged guest of the hypervisor.
*/

/*-----------------------------------------------------------
 * Implementation of functions defined in portable.h for the ARMv7-M RVM port.
 *----------------------------------------------------------*/

/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "rvm.h"

#ifndef __VFP_FP__
    #error This port can only be used when the project options are configured to enable hardware floating point support.
#endif

#ifndef configSYSTICK_CLOCK_HZ
    #define configSYSTICK_CLOCK_HZ              configCPU_CLOCK_HZ
    /* Ensure the SysTick is clocked at the same frequency as the core. */
    #define portNVIC_SYSTICK_CLK_BIT            (1UL<<2UL)
#else
    /* The way the SysTick is clocked is not modified in case it is not the same
    as the core. */
    #define portNVIC_SYSTICK_CLK_BIT            (0U)
#endif

/* FPU registers */
#if( RVM_COP_NUM != 0U )
#define FRT_FPU                                 ((volatile struct RVM_A7M_Cop_Struct*)(RVM_REG->Cop))
#endif

/* Constants required to set up the initial stack. */
#define portINITIAL_XPSR                        (0x01000000)
#define portINITIAL_EXEC_RETURN                 (0xFFFFFFFD)

/* A fiddle factor to estimate the number of SysTick counts that would have
occurred while the SysTick counter is stopped during tickless idle
calculations. */
#define portMISSED_COUNTS_FACTOR                (45UL)

/* For strict compliance with the Cortex-M spec the task start address should
have bit-0 clear, as it is loaded into the PC on exit from an ISR. */
#define portSTART_ADDRESS_MASK                  ((StackType_t)0xFFFFFFFEUL)

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
 * The RVM virtual interrupts cannot be masked through the Cortex-M4
 * BASEPRI, so these keep the FreeRTOS kernel API shape while delegating
 * the actual masking to the vPortEnterCritical()/vPortExitCritical()
 * pair above.
 */
uint32_t ulPortRaiseBASEPRI( void )
{
    return 0U;
}
/*-----------------------------------------------------------*/

void vPortSetBASEPRI( uint32_t ulNewMaskValue )
{
    ( void ) ulNewMaskValue;
}

/*
 * Setup the timer to generate the tick interrupts.  The implementation in this
 * file is weak to allow application writers to change the timer used to
 * generate the tick interrupt.
 */
void vPortSetupTimerInterrupt( void );

/*
 * Exception handlers.
 */
void xPortPendSVHandler( void );
void xPortSysTickHandler( void );
void vPortSVCHandler( void );

/*
 * Start first task is a separate function so it can be tested in isolation.
 */
static void prvPortStartFirstTask( void );

/*
 * Used to catch tasks that attempt to return from their implementing function.
 */
static void prvTaskExitError( void );

/*-----------------------------------------------------------*/

/*
 * The number of SysTick increments that make up one tick period.
 */
#if configUSE_TICKLESS_IDLE == 1
    static uint32_t ulTimerCountsForOneTick = 0;
#endif /* configUSE_TICKLESS_IDLE */

/*
 * The maximum number of tick periods that can be suppressed is limited by the
 * 24 bit resolution of the SysTick timer.
 */
#if configUSE_TICKLESS_IDLE == 1
    static uint32_t xMaximumPossibleSuppressedTicks = 0;
#endif /* configUSE_TICKLESS_IDLE */

/*
 * Compensate for the CPU cycles that pass while the SysTick is stopped (low
 * power functionality only.
 */
#if configUSE_TICKLESS_IDLE == 1
    static uint32_t ulStoppedTimerCompensation = 0;
#endif /* configUSE_TICKLESS_IDLE */

/*-----------------------------------------------------------*/

uint32_t SystemCoreClock=180000000U;

/*
 * See header file for description.
 */
StackType_t *pxPortInitialiseStack( StackType_t *pxTopOfStack, TaskFunction_t pxCode, void *pvParameters )
{
    /* Simulate the stack frame as it would be created by a context switch
    interrupt. */

    /* Offset added to account for the way the MCU uses the stack on entry/exit
    of interrupts, and to ensure alignment. */
    pxTopOfStack--;

    *pxTopOfStack = portINITIAL_XPSR;    /* xPSR */
    pxTopOfStack--;
    *pxTopOfStack = ( ( StackType_t ) pxCode ) & portSTART_ADDRESS_MASK;    /* PC */
    pxTopOfStack--;
    *pxTopOfStack = ( StackType_t ) portTASK_RETURN_ADDRESS;    /* LR */

    /* Save code space by skipping register initialisation. */
    pxTopOfStack -= 5;    /* R12, R3, R2 and R1. */
    *pxTopOfStack = ( StackType_t ) pvParameters;    /* R0 */

    /* A save method is being used that requires each task to maintain its
    own exec return value. */
    pxTopOfStack--;
    *pxTopOfStack = portINITIAL_EXEC_RETURN;
    
    /* R11, R10, R9, R8, R7, R6, R5 and R4. */
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

extern PRIVILEGED_DATA struct tskTaskControlBlock * volatile pxCurrentTCB;
static void prvPortStartFirstTask( void )
{
    /* Lockdown - not changing this in the future */
    // RVM_Hyp_Vct_Lck();
    /* Enable virtual interrupts */
    RVM_Hyp_Int_Ena();
    
    /* Switch to first stack's entry & PC; startup stack is abandoned */
    __asm volatile("LDR     R1,TCB      \n"
                   "LDR     R0,[R1]     \n"
                   "LDR     SP,[R0]     \n"
                   "POP     {R0-R4}     \n"     /* Abandon hypercall parameters */
                   "POP     {R4-R11}    \n"     /* Restore R4-R11 */
                   "POP     {R12}       \n"     /* Abandon "EXEC_RETURN" */
                   "POP     {R0-R3,R12} \n"     /* Restore R0-R3,R12 */
                   "POP     {LR}        \n"     /* Restore PC in LR */
                   "POP     {LR}        \n"
                   "SUB     SP,4        \n"     /* Fix up SP/LR */
                   "ORR     LR,#0x01    \n"
                   "BX      LR          \n"     /* Branch to entry */
                   ".align  4           \n"
                   "TCB:                \n"
                   ".word pxCurrentTCB  \n");
}

/*
 * See header file for description.
 */
BaseType_t xPortStartScheduler( void )
{
    /* configMAX_SYSCALL_INTERRUPT_PRIORITY must not be set to 0.
    See http://www.FreeRTOS.org/RTOS-Cortex-M3-M4.html */

    /* Start the timer that generates the tick ISR.  Interrupts are disabled
    here already. */
    vPortSetupTimerInterrupt();

    /* Initialise the critical nesting count ready for the first task. */
    uxCriticalNesting = 0;

    /* Start the first task. */
    prvPortStartFirstTask();

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

    /* This is not the interrupt safe version of the enter critical function so
    assert() if it is being called from an interrupt context.  Only API
    functions that end in "FromISR" can be used in an interrupt.  Only assert if
    the critical nesting count is 1 to protect against recursive calls if the
    assert function also uses a critical section. */
    if( uxCriticalNesting == 1 )
    {
        //configASSERT( ( portNVIC_INT_CTRL_REG & portVECTACTIVE_MASK ) == 0 );
    }
}

void vPortExitCritical( void )
{
    configASSERT(uxCriticalNesting);
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

void xPortPendSVHandler(void)
{
    uint32_t* SP;
    
    /* MRS      R0,PSP
     * ISB */
    SP=(uint32_t*)(RVM_REG->Reg.SP);
    
    /* LDR      R3,pxCurrentTCB
     * LDR      R2,[R3] */
    
#if( RVM_COP_NUM != 0U )
    /* TST      LR,#0x10
     * IT       EQ
     * VSTMDBEQ R0!,{S16-S31} */
    if((RVM_REG->Reg.LR&0x10U)==0U)
    {
        *(--SP)=FRT_FPU->S31;
        *(--SP)=FRT_FPU->S30;
        *(--SP)=FRT_FPU->S29;
        *(--SP)=FRT_FPU->S28;
        *(--SP)=FRT_FPU->S27;
        *(--SP)=FRT_FPU->S26;
        *(--SP)=FRT_FPU->S25;
        *(--SP)=FRT_FPU->S24;
        *(--SP)=FRT_FPU->S23;
        *(--SP)=FRT_FPU->S22;
        *(--SP)=FRT_FPU->S21;
        *(--SP)=FRT_FPU->S20;
        *(--SP)=FRT_FPU->S19;
        *(--SP)=FRT_FPU->S18;
        *(--SP)=FRT_FPU->S17;
        *(--SP)=FRT_FPU->S16;
    }
#endif /* RVM_COP_NUM */
    
    /* STMDB    R0!,{R4-R11,LR} */
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
    
    /* LDMIA    R0!,{R4-R11,LR} */
    RVM_REG->Reg.R4=*(SP++);
    RVM_REG->Reg.R5=*(SP++);
    RVM_REG->Reg.R6=*(SP++);
    RVM_REG->Reg.R7=*(SP++);
    RVM_REG->Reg.R8=*(SP++);
    RVM_REG->Reg.R9=*(SP++);
    RVM_REG->Reg.R10=*(SP++);
    RVM_REG->Reg.R11=*(SP++);
    RVM_REG->Reg.LR=*(SP++);
    
#if( RVM_COP_NUM != 0U )
    /* TST      LR,#0x10
     * IT       EQ
     * VLDMIAEQ R0!,{S16-S31} */
    if((RVM_REG->Reg.LR&0x10U)==0U)
    {
        FRT_FPU->S16=*(SP++);
        FRT_FPU->S17=*(SP++);
        FRT_FPU->S18=*(SP++);
        FRT_FPU->S19=*(SP++);
        FRT_FPU->S20=*(SP++);
        FRT_FPU->S21=*(SP++);
        FRT_FPU->S22=*(SP++);
        FRT_FPU->S23=*(SP++);
        FRT_FPU->S24=*(SP++);
        FRT_FPU->S25=*(SP++);
        FRT_FPU->S26=*(SP++);
        FRT_FPU->S27=*(SP++);
        FRT_FPU->S28=*(SP++);
        FRT_FPU->S29=*(SP++);
        FRT_FPU->S30=*(SP++);
        FRT_FPU->S31=*(SP++);
    }
#endif /* RVM_COP_NUM */
    
    /* MSR      PSP,R0 */
    RVM_REG->Reg.SP=(uint32_t)SP;
    /* BX       LR */
    return;
}

void xPortSysTickHandler( void )
{
    portDISABLE_INTERRUPTS();
    
    if(xTaskIncrementTick()!=pdFALSE)
    {
        /* Trigger PendSV */
    }
    
    portENABLE_INTERRUPTS();
}

#if(configUSE_TICKLESS_IDLE==1)
#error RVM: FRT: Tickless idle is not supported in virtualized environment
#endif

void vPortSetupTimerInterrupt( void )
{
    /* Hook Systick & PendSV */
    RVM_Virt_Tim_Reg(xPortSysTickHandler);
    RVM_Virt_Ctx_Reg(xPortPendSVHandler);
}
/*-----------------------------------------------------------*/

#if( configUSE_RVM_OPTIMISED_YIELD != 0 )

/* The scheduler lookahead provided by tasksrvm.c - this file is compiled
 * instead of tasks.c and appends the peek below to it.  It returns the
 * task that the next vTaskSwitchContext() would select, without
 * committing any scheduler state: neither the round-robin cursor is
 * advanced, nor pxCurrentTCB is written.  Must be called with the virtual
 * interrupts masked, so that the scheduler state cannot change between
 * the peek and the real selection that follows in the assembly routine -
 * the two are then guaranteed to pick the very same task. */
extern TaskHandle_t xTaskPeekNextToRun( void );

/* The fast-path assembly routine - see portasm.s.  It must be entered
 * with the RVM virtual interrupts already masked; it performs the switch
 * directly on the task stacks and unmasks the interrupts itself once the
 * new stack has been loaded. */
extern void vPortYield_Swt( void );

/* Word offsets into a saved task frame, counted from pxTopOfStack - the
 * first member of the TCB.  The layout is created by pxPortInitialiseStack()
 * above, by xPortPendSVHandler() and by the fast path in portasm.s, and
 * is identical for all three:
 *   word  0 -  4 : the five hypercall parameter words
 *   word  5 - 12 : R4 - R11
 *   word 13      : the saved EXC_RETURN (bit 4 clear means extended)
 *   word 14 - 20 : R0 - R3, R12, LR, PC            } basic frame
 *   word 21      : XPSR                            }
 *   word 14 - 29 : the S16 - S31 block, only when extended
 *   word 30 - 36 : R0 - R3, R12, LR, PC            } extended frame
 *   word 37      : XPSR                            } */
#define portOFFSET_TO_LR_EXC                    ( 13U )
#define portOFFSET_TO_XPSR_BASIC                ( 21U )
#define portOFFSET_TO_XPSR_EXTENDED             ( 37U )

/* The EPSR ICI/IT field - see the ARMv7-M ARM.  These bits hold the
 * continuation state of a partially executed IT block or LDM/STM, and can
 * only be restored by a real exception return - never by the manual
 * unstacking of the fast path, since thread mode software cannot write
 * the EPSR.  A task saved with these bits set must therefore be resumed
 * through the hypervisor's slow path, which does perform an exception
 * return, exactly like an interrupt would. */
#define portXPSR_ICI_IT_MASK                    ( 0x0600FC00UL )

/*
 * Check whether the frame saved on the given TCB carries ICI/IT state.
 */
#if( configUSE_RVM_OPTIMISED_YIELD != 0 )
static BaseType_t xPortFrameDirty( void * pvNextTCB )
{
const uint32_t * pulTopOfStack;

    /* pxTopOfStack is the first member of the TCB. */
    pulTopOfStack = *( const uint32_t ** ) pvNextTCB;

    if( ( pulTopOfStack[ portOFFSET_TO_LR_EXC ] & 0x10UL ) == 0UL )
    {
        /* Extended frame - the XPSR sits above the S16 - S31 block. */
        return ( ( pulTopOfStack[ portOFFSET_TO_XPSR_EXTENDED ] & portXPSR_ICI_IT_MASK ) != 0UL ) ? pdTRUE : pdFALSE;
    }

    return ( ( pulTopOfStack[ portOFFSET_TO_XPSR_BASIC ] & portXPSR_ICI_IT_MASK ) != 0UL ) ? pdTRUE : pdFALSE;
}
#endif /* configUSE_RVM_OPTIMISED_YIELD */
/*-----------------------------------------------------------*/

/*
 * The fast yield entry point selected by portYIELD().  The scheduler is
 * first peeked under the virtual interrupt mask, then one of three
 * outcomes follows:
 * 1. The peeked task is the calling task itself - just unmask and return,
 *    no switch happens at all.
 * 2. The peeked task holds a clean frame - call the assembly fast path,
 *    which runs the real selection (guaranteed to pick the same task) and
 *    switches directly on the task stacks.
 * 3. The peeked task holds dirty ICI/IT bits - unmask and take the slow
 *    hypervisor yield, whose real exception return resumes the target
 *    exactly like an interrupt would, IT blocks and LDM/STM continuation
 *    included.  This is the only branch that traps, and it can only be
 *    hit when the target was last preempted in the middle of such an
 *    instruction.
 */
void vPortYield( void )
{
#if( configUSE_RVM_OPTIMISED_YIELD != 0 )
void * pvNextTCB;
#endif

    /* The official ARMv7-M ports pend the PendSV and only switch once the
     * critical section exits - the kernel does call portYIELD() from inside
     * one (ulTaskNotifyTake et al).  RVM_Virt_Int_Unmask() is a flat flag,
     * so yielding under a raised nesting count would open the interrupts in
     * the middle of the caller's critical section; pend instead, and let
     * vPortExitCritical() run the switch when the count hits zero. */
    if( uxCriticalNesting != 0U )
    {
        xPortYieldPend = pdTRUE;
        return;
    }

#if( configUSE_RVM_OPTIMISED_YIELD != 0 )
    RVM_Virt_Int_Mask();
    pvNextTCB = ( void * ) xTaskPeekNextToRun();

    if( pvNextTCB != ( void * ) pxCurrentTCB )
    {
        if( xPortFrameDirty( pvNextTCB ) == pdFALSE )
        {
            /* The fast path unmasks the interrupts before returning. */
            vPortYield_Swt();
        }
        else
        {
            /* Pend the switch under mask; the unmask arms it at its
             * hypercall boundary - no unmasked window in between. */
            RVM_Virt_Yield();
            RVM_Virt_Int_Unmask();
        }
    }
    else
    {
        RVM_Virt_Int_Unmask();
    }
#else
    /* Yield through the hypervisor - simpler, but traps on every switch. */
    RVM_Virt_Yield();
#endif
}
/*-----------------------------------------------------------*/

/*
 * The manual unstacking in portasm.s jumps here when it meets a frame
 * with dirty ICI/IT bits.  vPortYield() above filters such frames out
 * before the fast path is ever taken, so reaching this function means
 * the filter itself is broken - trap loudly instead of corrupting the
 * program flow silently.
 */
void vPortYield_Err( void )
{
    RVM_DBG_S( "FRT A7M RVM port: dirty XPSR ICI/IT bits on the fast yield path.\r\n" );
    configASSERT( 0 );
}
/*-----------------------------------------------------------*/

#endif /* configUSE_RVM_OPTIMISED_YIELD */

/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
