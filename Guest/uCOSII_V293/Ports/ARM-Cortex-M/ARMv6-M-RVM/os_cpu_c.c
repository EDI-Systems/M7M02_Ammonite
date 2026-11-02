/*
*********************************************************************************************************
*                                                uC/OS-II
*                                          The Real-Time Kernel
*
*                               Copyright (C) 2026 Evo-Devo Instrum.
*                                       All rights reserved.
*
*                This file is NOT part of the uC/OS-II distribution.  It belongs
*                to the uC/OS-II-on-RVM test suite and is released under the
*                Unlicense; refer to the LICENSE file for details.  The original
*                uC/OS-II kernel is copyright Weston Embedded and is not
*                modified by this file.
*
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                       ARMv6-M Port - os_cpu_c.c
*
* File      : os_cpu_c.c
* Version   : V1.00.00
*********************************************************************************************************
*/

/*
*********************************************************************************************************
* Note(s)  : (1) This file is derived from the official ARMv6-M os_cpu_c.c: the kernel hooks are
*                carried over, the initial stack carries the five RVM hypercall parameter words
*                below the register block, and the SysTick functions are replaced by the RVM tick
*                and context handlers.  The task level context switches go through the assembly
*                fast path in os_cpu_a.s, the interrupt driven ones through the RVM virtual yield
*                hypercall.
*
*            (2) uC/OS-II does not use the uC/CPU abstraction layer: the critical section functions
*                are defined here as C functions that map onto the RVM virtual interrupt mask.
*                The kernel does not include any file from the uC-CPU or uC-LIB distributions.
*********************************************************************************************************
*/

#define   OS_CPU_GLOBALS

/*
*********************************************************************************************************
*                                             INCLUDE FILES
*********************************************************************************************************
*/

#include  <ucos_ii.h>
#include  "rvm.h"

#if OS_TMR_EN > 0u
static  INT16U  OSTmrCtr;
#endif

#ifdef __cplusplus
extern  "C" {
#endif

/*
*********************************************************************************************************
*                                           IDLE TASK HOOK
*
* Description: This function is called by the idle task.  The idle parking hook is registered here
*              by the application's main; the port does not define the parking policy.
*
* Arguments  : None.
*
* Note(s)    : None.
*********************************************************************************************************
*/

void  OSTaskIdleHook (void)
{
#if OS_APP_HOOKS_EN > 0u
    App_TaskIdleHook();
#endif
}

/*
*********************************************************************************************************
*                                       OS INITIALIZATION HOOK (BEGIN)
*
* Description: This function is called by OSInit() at the beginning of OSInit().
*
* Arguments  : None.
*
* Note(s)    : The exception stack and the BASEPRI boundary belong to the hypervisor under RVM -
*              the guest has no MSP of its own to set up and no NVIC to configure.
*********************************************************************************************************
*/

void  OSInitHookBegin (void)
{
}

/*
*********************************************************************************************************
*                                       OS INITIALIZATION HOOK (END)
*
* Description: This function is called by OSInit() at the end of OSInit().
*
* Arguments  : None.
*
* Note(s)    : None.
*********************************************************************************************************
*/

void  OSInitHookEnd (void)
{
}

/*
*********************************************************************************************************
*                                          TASK CREATION HOOK
*
* Description: This function is called when a task is created.
*
* Arguments  : ptcb   Pointer to the task control block of the task being created.
*
* Note(s)    : None.
*********************************************************************************************************
*/

void  OSTaskCreateHook (OS_TCB *ptcb)
{
#if OS_APP_HOOKS_EN > 0u
    App_TaskCreateHook(ptcb);
#else
    (void)ptcb;
#endif
}

/*
*********************************************************************************************************
*                                           TASK DELETION HOOK
*
* Description: This function is called when a task is deleted.
*
* Arguments  : ptcb   Pointer to the task control block of the task being deleted.
*
* Note(s)    : None.
*********************************************************************************************************
*/

void  OSTaskDelHook (OS_TCB *ptcb)
{
#if OS_APP_HOOKS_EN > 0u
    App_TaskDelHook(ptcb);
#else
    (void)ptcb;
#endif
}

/*
*********************************************************************************************************
*                                            TASK RETURN HOOK
*
* Description: This function is called if a task accidentally returns.
*
* Arguments  : ptcb   Pointer to the task control block of the task that is returning.
*
* Note(s)    : None.
*********************************************************************************************************
*/

void  OSTaskReturnHook (OS_TCB *ptcb)
{
#if OS_APP_HOOKS_EN > 0u
    App_TaskReturnHook(ptcb);
#else
    (void)ptcb;
#endif
}

/*
*********************************************************************************************************
*                                         STATISTIC TASK HOOK
*
* Description: This function is called every second by uC/OS-II's statistics task.
*
* Arguments  : None.
*
* Note(s)    : None.
*********************************************************************************************************
*/

void  OSTaskStatHook (void)
{
#if OS_APP_HOOKS_EN > 0u
    App_TaskStatHook();
#endif
}

/*
*********************************************************************************************************
*                                        INITIALIZE A TASK'S STACK
*
* Description: This function is called by OSTaskCreate() to initialize the stack frame of the task
*              being created.  This function is highly processor specific.
*
* Arguments  : task   Pointer to the task entry point address.
*
*              p_arg  Pointer to a user supplied data area that will be passed to the task
*                         when the task first executes.
*
*              ptos   Pointer to the top of the stack.
*
*              opt    Options used to alter the behavior of OSTaskStkInit().
*
* Returns    : Always returns the location of the new top-of-stack once the processor registers have
*              been placed on the stack in the proper order.
*
* Note(s)    : (1) The frame, from the returned SP up, matches the layout that the fast path in
*                  os_cpu_a.s and the interrupt path in OS_Ctx_Handler() both build:
*
*                    word  0 -  4 : the five hypercall parameter words
*                    word  5 - 12 : R4 - R11
*                    word 13      : the saved EXC_RETURN
*                    word 14 - 21 : R0 - R3, R12, LR, PC, xPSR (basic frame)
*
*              (2) The SP must be 8-byte aligned in conforming to the Procedure Call Standard
*                  for the ARM architecture.
*********************************************************************************************************
*/

OS_STK  *OSTaskStkInit (void (*task)(void *), void *p_arg, OS_STK *ptos, INT16U opt)
{
    OS_STK  *p_stk;

    (void)opt;

    p_stk  = &ptos[0];
    p_stk  = (OS_STK *)((OS_STK)(p_stk) & 0xFFFFFFF8u);

                                                                /* The basic hardware frame.                          */
    *(--p_stk) = (OS_STK)0x01000000uL;                          /* xPSR: the Thumb bit set.                           */
    *(--p_stk) = (OS_STK)task;                                  /* PC: the entry point (bit 0 already set by C).       */
    *(--p_stk) = (OS_STK)OS_TaskReturn;                         /* R14 (LR): the return handler.                      */
    *(--p_stk) = (OS_STK)0x12121212uL;                          /* R12.                                               */
    *(--p_stk) = (OS_STK)0x03030303uL;                          /* R3.                                                */
    *(--p_stk) = (OS_STK)0x02020202uL;                          /* R2.                                                */
    *(--p_stk) = (OS_STK)0x01010101uL;                          /* R1.                                                */
    *(--p_stk) = (OS_STK)p_arg;                                 /* R0: the argument.                                  */
                                                                /* The software-saved registers.                      */
    *(--p_stk) = (OS_STK)0xFFFFFFFDuL;                          /* The EXC_RETURN: basic frame, PSP.                  */
    *(--p_stk) = (OS_STK)0x11111111uL;                          /* R11.                                               */
    *(--p_stk) = (OS_STK)0x10101010uL;                          /* R10.                                               */
    *(--p_stk) = (OS_STK)0x09090909uL;                          /* R9.                                                */
    *(--p_stk) = (OS_STK)0x08080808uL;                          /* R8.                                                */
    *(--p_stk) = (OS_STK)0x07070707uL;                          /* R7.                                                */
    *(--p_stk) = (OS_STK)0x06060606uL;                          /* R6.                                                */
    *(--p_stk) = (OS_STK)0x05050505uL;                          /* R5.                                                */
    *(--p_stk) = (OS_STK)0x04040404uL;                          /* R4.                                                */
                                                                /* The RVM hypercall parameters, all clean.           */
    *(--p_stk) = (OS_STK)0x00000000uL;                          /* Param[3].                                          */
    *(--p_stk) = (OS_STK)0x00000000uL;                          /* Param[2].                                          */
    *(--p_stk) = (OS_STK)0x00000000uL;                          /* Param[1].                                          */
    *(--p_stk) = (OS_STK)0x00000000uL;                          /* Param[0].                                          */
    *(--p_stk) = (OS_STK)0x00000000uL;                          /* Number.                                            */

    return (p_stk);
}

/*
*********************************************************************************************************
*                                           TASK SWITCH HOOK
*
* Description: This function is called when a task switch is performed.
*
* Arguments  : None.
*
* Note(s)    : 1) Interrupts are disabled during this call.
*********************************************************************************************************
*/

void  OSTaskSwHook (void)
{
#if OS_TASK_SW_HOOK_EN > 0u
    App_TaskSwHook();
#endif
}

/*
*********************************************************************************************************
*                                         TCB INITIALIZATION HOOK
*
* Description: This function is called by OSTCBInit() after a TCB has been initialized.
*
* Arguments  : ptcb   Pointer to the task control block of the task being created.
*
* Note(s)    : None.
*********************************************************************************************************
*/

void  OSTCBInitHook (OS_TCB *ptcb)
{
#if OS_APP_HOOKS_EN > 0u
    App_TCBInitHook(ptcb);
#else
    (void)ptcb;
#endif
}

/*
*********************************************************************************************************
*                                              TICK HOOK
*
* Description: This function is called every tick.
*
* Arguments  : None.
*
* Note(s)    : 1) This function is assumed to be called from the tick ISR.
*********************************************************************************************************
*/

void  OSTimeTickHook (void)
{
#if OS_TIME_TICK_HOOK_EN > 0u
#if OS_TMR_EN > 0u
    OSTmrCtr++;
    if (OSTmrCtr >= (OS_TICKS_PER_SEC / OS_TMR_CFG_TICKS_PER_SEC)) {
        OSTmrCtr = 0u;
        OSTmrSignal();
    }
#endif
#endif
    /* The RVM tick is always driven by the virtual timer; nothing else
     * needs to be done here. */
    (void)0;
}

/*
*********************************************************************************************************
*                                   CRITICAL SECTION FUNCTIONS
*
* Description: These functions replace the assembly-implemented OS_CPU_SR_Save/Restore of the
*              official ports.  The RVM virtual interrupt mask is a flat flag; the snapshot is
*              taken on entry and restored on exit, so nesting is balanced by the snapshot
*              rather than by a counter.
*
* Note(s)    : Writing back "unmasked" must take the full unmask path (which re-signals the
*              vector thread if anything pended while masked), or pending virtual interrupts
*              would starve on a bare flag write.
*********************************************************************************************************
*/

OS_CPU_SR  OS_CPU_SR_Save (void)
{
    OS_CPU_SR  cpu_sr;

    cpu_sr = (OS_CPU_SR)RVM_Int_Mask;
    if (cpu_sr == 0u) {
        RVM_Virt_Int_Mask();
    }
    return (cpu_sr);
}

void  OS_CPU_SR_Restore (OS_CPU_SR  cpu_sr)
{
    if (cpu_sr == 0u) {
        RVM_Virt_Int_Unmask();
    } else {
        RVM_Virt_Int_Mask();
    }
}

/*
*********************************************************************************************************
*                                   PERFORM A CONTEXT SWITCH (FROM ISR LEVEL)
*
* Description: This function is called by OSIntExit() when a context switch is needed after an
*              interrupt handler has completed.  Under RVM the interrupt driven switches request
*              the switch through the yield hypercall and the actual context transfer happens in
*              OS_Ctx_Handler() at the vector thread's dispatch tail.
*********************************************************************************************************
*/

void  OSIntCtxSw (void)
{
    RVM_Virt_Yield();
}

/*
*********************************************************************************************************
*                                       THE RVM CONTEXT SWITCH HANDLER
*
* Description: This handler is registered through RVM_Virt_Ctx_Reg().  When a task yields through
*              the RVM_Virt_Yield() hypercall, the hypervisor saves the interrupted context and
*              calls this function with the register block made available through RVM_REG.  The
*              saved registers are moved onto the stack of the task that is being switched out
*              (below the frame that the hypervisor already saved), the switch bookkeeping of
*              the official PendSV handler runs, and the registers of the task being switched in
*              are loaded into the register block.
*
* Note(s)    : The first field of OS_TCB is OSTCBStkPtr, so the TCB pointer is also the address
*              of the saved stack pointer word - same layout the assembly fast path relies on.
*********************************************************************************************************
*/

void  OS_Ctx_Handler (void)
{
    OS_STK  *p_sp;

                                                                /* Save the current context onto its task stack.     */
    p_sp = (OS_STK *)RVM_REG->Reg.SP;

    *(--p_sp) = (OS_STK)RVM_REG->Reg.LR;                        /* The saved EXC_RETURN.                             */
    *(--p_sp) = (OS_STK)RVM_REG->Reg.R11;
    *(--p_sp) = (OS_STK)RVM_REG->Reg.R10;
    *(--p_sp) = (OS_STK)RVM_REG->Reg.R9;
    *(--p_sp) = (OS_STK)RVM_REG->Reg.R8;
    *(--p_sp) = (OS_STK)RVM_REG->Reg.R7;
    *(--p_sp) = (OS_STK)RVM_REG->Reg.R6;
    *(--p_sp) = (OS_STK)RVM_REG->Reg.R5;
    *(--p_sp) = (OS_STK)RVM_REG->Reg.R4;

    *(--p_sp) = (OS_STK)RVM_STATE->Usr.Param[3];                /* The hypercall parameters.                        */
    *(--p_sp) = (OS_STK)RVM_STATE->Usr.Param[2];
    *(--p_sp) = (OS_STK)RVM_STATE->Usr.Param[1];
    *(--p_sp) = (OS_STK)RVM_STATE->Usr.Param[0];
    *(--p_sp) = (OS_STK)RVM_STATE->Usr.Number;

    OSTCBCur->OSTCBStkPtr = p_sp;                               /* Save the new stack top into the TCB.            */

                                                                /* The switch bookkeeping of the official          */
    OSTaskSwHook();                                             /* PendSV handler.                                  */
    OSPrioCur   = OSPrioHighRdy;
    OSTCBCur    = OSTCBHighRdy;

    p_sp = OSTCBCur->OSTCBStkPtr;                               /* Load the new task's stack.                      */

    RVM_STATE->Usr.Number = *(p_sp++);                          /* Restore the hypercall parameters.               */
    RVM_STATE->Usr.Param[0] = *(p_sp++);
    RVM_STATE->Usr.Param[1] = *(p_sp++);
    RVM_STATE->Usr.Param[2] = *(p_sp++);
    RVM_STATE->Usr.Param[3] = *(p_sp++);

    RVM_REG->Reg.R4  = *(p_sp++);
    RVM_REG->Reg.R5  = *(p_sp++);
    RVM_REG->Reg.R6  = *(p_sp++);
    RVM_REG->Reg.R7  = *(p_sp++);
    RVM_REG->Reg.R8  = *(p_sp++);
    RVM_REG->Reg.R9  = *(p_sp++);
    RVM_REG->Reg.R10 = *(p_sp++);
    RVM_REG->Reg.R11 = *(p_sp++);
    RVM_REG->Reg.LR  = *(p_sp++);

    RVM_REG->Reg.SP  = (rvm_ptr_t)p_sp;                         /* Return to the new task.                         */
}

/*
*********************************************************************************************************
*                                           THE RVM TICK HANDLER
*
* Description: This handler is registered through RVM_Virt_Tim_Reg() and feeds the kernel tick.
*********************************************************************************************************
*/

void  OS_Tick_Handler (void)
{
    OSIntEnter();
    OSTimeTick();
    OSIntExit();
}

/*
*********************************************************************************************************
*                                        INITIALIZE THE RVM PORTING LAYER
*
* Description: This function hooks the tick and the context switch handler up to the hypervisor.
*              It is called by OSStartHighRdy() before the first task is entered.
*********************************************************************************************************
*/

void  OS_CPU_Port_Init (void)
{
    RVM_Virt_Tim_Reg(OS_Tick_Handler);
    RVM_Virt_Ctx_Reg(OS_Ctx_Handler);
    RVM_Hyp_Int_Ena();
}

#ifdef __cplusplus
}
#endif
