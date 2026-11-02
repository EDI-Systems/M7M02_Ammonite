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
*                                       ARMv7-M Port - os_cpu_c.c
*
* File      : os_cpu_c.c
* Version   : V1.00.00
*********************************************************************************************************
*/

/*
*********************************************************************************************************
* Note(s)  : (1) This file is derived from the official ARMv7-M os_cpu_c.c: the kernel hooks are
*                carried over, the initial stack carries the five RVM hypercall parameter words
*                below the register block, and the SysTick functions are replaced by the RVM tick
*                and context handlers.  The task level context switches go through the C level
*                filter OSCtxSw() which routes clean frames onto the assembly fast path in
*                os_cpu_a.s and dirty frames onto the RVM yield hypercall; the interrupt driven
*                switches go through the same yield hypercall.
*
*            (2) uC/OS-II does not use the uC/CPU abstraction layer: the critical section functions
*                are defined here as C functions that map onto the RVM virtual interrupt mask.
*
*            (3) The EPSR ICI/IT bits of a frame that is being switched to can only be restored
*                by a real exception return.  OS_CPU_Frame_Dirty() checks the target's saved
*                xPSR through the same TCB contract the assembly fast path uses.
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
*********************************************************************************************************
*/

void  OSInitHookBegin (void)
{
                                                                /* The exception stack and the BASEPRI boundary    */
                                                                /* belong to the hypervisor under RVM.              */
}

/*
*********************************************************************************************************
*                                       OS INITIALIZATION HOOK (END)
*********************************************************************************************************
*/

void  OSInitHookEnd (void)
{
}

/*
*********************************************************************************************************
*                                          TASK CREATION HOOK
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
* Note(s)  : (1) The frame, from the returned SP up, matches the layout that the fast path in
*                  os_cpu_a.s and the interrupt path in OS_Ctx_Handler() both build:
*
*                    word  0 -  4 : the five hypercall parameter words
*                    word  5 - 12 : R4 - R11
*                    word 13      : the saved EXC_RETURN
*                    word 14 - 20 : R0 - R3, R12, LR, PC, xPSR (basic frame)
*
*                  (Extended frames with S0-S15/FPSCR/S16-S31 are never built
*                   here - they only arise at runtime through the lazy stacking
*                   when a task actually uses the FPU.)
*
*              (2) The SP must be 8-byte aligned in conforming to the Procedure Call Standard.
*********************************************************************************************************
*/

OS_STK  *OSTaskStkInit (void (*task)(void *), void *p_arg, OS_STK *ptos, INT16U opt)
{
    OS_STK  *p_stk;

    (void)opt;

    p_stk  = &ptos[0];
    p_stk  = (OS_STK *)((OS_STK)(p_stk) & 0xFFFFFFF8u);

    /* The basic hardware frame - every new task starts with a basic frame
     * (EXC_RETURN = 0xFFFFFFFD); if the task ever uses the FPU, the lazy
     * stacking mechanism promotes the frame to extended from there on, and
     * the fast path and the context handler both handle the two frame
     * types at runtime.  No floating point registers are pre-initialized. */
    *(--p_stk) = (OS_STK)0x01000000uL;                          /* xPSR: the Thumb bit set. */
    *(--p_stk) = (OS_STK)task;                                  /* PC: the entry point. */
    *(--p_stk) = (OS_STK)OS_TaskReturn;                         /* R14 (LR): the return handler. */
    *(--p_stk) = (OS_STK)0x12121212uL;                          /* R12. */
    *(--p_stk) = (OS_STK)0x03030303uL;                          /* R3. */
    *(--p_stk) = (OS_STK)0x02020202uL;                          /* R2. */
    *(--p_stk) = (OS_STK)0x01010101uL;                          /* R1. */
    *(--p_stk) = (OS_STK)p_arg;                                 /* R0: the argument. */
    *(--p_stk) = (OS_STK)0xFFFFFFFDuL;                          /* EXC_RETURN: basic frame, PSP. */
    *(--p_stk) = (OS_STK)0x11111111uL;                          /* R11. */
    *(--p_stk) = (OS_STK)0x10101010uL;                          /* R10. */
    *(--p_stk) = (OS_STK)0x09090909uL;                          /* R9. */
    *(--p_stk) = (OS_STK)0x08080808uL;                          /* R8. */
    *(--p_stk) = (OS_STK)0x07070707uL;                          /* R7. */
    *(--p_stk) = (OS_STK)0x06060606uL;                          /* R6. */
    *(--p_stk) = (OS_STK)0x05050505uL;                          /* R5. */
    *(--p_stk) = (OS_STK)0x04040404uL;                          /* R4. */

    *(--p_stk) = (OS_STK)0x00000000uL;                          /* Param[3]. */
    *(--p_stk) = (OS_STK)0x00000000uL;                          /* Param[2]. */
    *(--p_stk) = (OS_STK)0x00000000uL;                          /* Param[1]. */
    *(--p_stk) = (OS_STK)0x00000000uL;                          /* Param[0]. */
    *(--p_stk) = (OS_STK)0x00000000uL;                          /* Number. */

    return (p_stk);
}

/*
*********************************************************************************************************
*                                           TASK SWITCH HOOK
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
}

/*
*********************************************************************************************************
*                                   CRITICAL SECTION FUNCTIONS
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
*                                   DIRTY FRAME FILTER
*
* Description: Check whether the frame of the target task carries EPSR ICI/IT state that only a
*              real exception return can restore.  The check reads through the TCB's first
*              member (OSTCBStkPtr) at the same word offsets the fast path and the context
*              handler use, so no kernel internals are included.
*
* Note(s)    : The ARMv7-M EPSR holds the continuation state of a partially executed IT block
*              or LDM/STM in its ICI/IT field; thread mode software cannot write the EPSR, so
*              manual unstacking cannot restore such state.
*********************************************************************************************************
*/

#define  OS_CPU_OFFSET_TO_LR_EXC          (13U)             /* The saved EXC_RETURN.                              */
#define  OS_CPU_OFFSET_TO_XPSR_BASIC      (21U)             /* xPSR of the basic frame.                           */
#define  OS_CPU_OFFSET_TO_XPSR_EXTENDED   (37U)             /* xPSR of the extended frame.                        */
#define  OS_CPU_EXC_RETURN_EXTENDED_MASK  (0x10U)           /* Bit 4 clear means extended.                        */
#define  OS_CPU_XPSR_ICI_IT_MASK          (0x0600FC00UL)    /* The EPSR ICI/IT field.                             */

static  BOOLEAN  OS_CPU_Frame_Dirty (OS_TCB *ptcb)
{
    OS_STK  *p_sp;

    p_sp = *(OS_STK **)ptcb;                                     /* OSTCBStkPtr - the first TCB member.           */

    if ((p_sp[OS_CPU_OFFSET_TO_LR_EXC] & OS_CPU_EXC_RETURN_EXTENDED_MASK) == 0u) {
        return ((p_sp[OS_CPU_OFFSET_TO_XPSR_EXTENDED] & OS_CPU_XPSR_ICI_IT_MASK) != 0u) ? OS_TRUE : OS_FALSE;
    }

    return ((p_sp[OS_CPU_OFFSET_TO_XPSR_BASIC] & OS_CPU_XPSR_ICI_IT_MASK) != 0u) ? OS_TRUE : OS_FALSE;
}

/*
*********************************************************************************************************
*                                   PERFORM A CONTEXT SWITCH (FROM TASK LEVEL)
*
* Description: The C level filter: clean target frames take the assembly fast path that switches
*              directly on the task stacks; dirty ones take the slow path through the hypervisor,
*              whose real exception return restores what manual unstacking cannot.
*
* Note(s)    : OS_Sched() has already selected OSTCBHighRdy and holds the scheduler critical
*              section (the virtual interrupts are masked) when this runs.
*********************************************************************************************************
*/

void  OSCtxSw (void)
{
#if OS_CFG_RVM_FAST_YIELD_EN != 0
    RVM_Virt_Int_Mask();                                         /* Idempotent - already masked by OS_Sched.     */

    if (OS_CPU_Frame_Dirty(OSTCBHighRdy) == OS_TRUE) {
        RVM_Virt_Yield();                                        /* Dirty ICI/IT -> the slow path.               */
        RVM_Virt_Int_Unmask();
    } else {
        OSCtxSw_Swt();                                           /* Clean -> switch on the task stacks.          */
    }
#else
    RVM_Virt_Yield();
#endif
}

/*
*********************************************************************************************************
*                                   THE FAST-PATH DIRTY FRAME TRAP
*********************************************************************************************************
*/

void  OSCtxSw_Err (void)
{
    RVM_Virt_Int_Mask();
    RVM_DBG_S("UO2 A7M RVM port: dirty XPSR ICI/IT on the fast switch path.\r\n");
    while (1) {
    }
}

/*
*********************************************************************************************************
*                                   PERFORM A CONTEXT SWITCH (FROM ISR LEVEL)
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
* Note(s)    : The first field of OS_TCB is OSTCBStkPtr, so the TCB pointer is also the address
*              of the saved stack pointer word.
*********************************************************************************************************
*/

void  OS_Ctx_Handler (void)
{
    OS_STK  *p_sp;

    p_sp = (OS_STK *)RVM_REG->Reg.SP;

#if RVM_COP_NUM != 0u
    if ((RVM_REG->Reg.LR & 0x10u) == 0u)                        /* Extended frame: save S16-S31.              */
    {
        *(--p_sp) = (OS_STK)((volatile struct RVM_A7M_Cop_Struct *)(RVM_REG->Cop))->S31;
        *(--p_sp) = (OS_STK)((volatile struct RVM_A7M_Cop_Struct *)(RVM_REG->Cop))->S30;
        *(--p_sp) = (OS_STK)((volatile struct RVM_A7M_Cop_Struct *)(RVM_REG->Cop))->S29;
        *(--p_sp) = (OS_STK)((volatile struct RVM_A7M_Cop_Struct *)(RVM_REG->Cop))->S28;
        *(--p_sp) = (OS_STK)((volatile struct RVM_A7M_Cop_Struct *)(RVM_REG->Cop))->S27;
        *(--p_sp) = (OS_STK)((volatile struct RVM_A7M_Cop_Struct *)(RVM_REG->Cop))->S26;
        *(--p_sp) = (OS_STK)((volatile struct RVM_A7M_Cop_Struct *)(RVM_REG->Cop))->S25;
        *(--p_sp) = (OS_STK)((volatile struct RVM_A7M_Cop_Struct *)(RVM_REG->Cop))->S24;
        *(--p_sp) = (OS_STK)((volatile struct RVM_A7M_Cop_Struct *)(RVM_REG->Cop))->S23;
        *(--p_sp) = (OS_STK)((volatile struct RVM_A7M_Cop_Struct *)(RVM_REG->Cop))->S22;
        *(--p_sp) = (OS_STK)((volatile struct RVM_A7M_Cop_Struct *)(RVM_REG->Cop))->S21;
        *(--p_sp) = (OS_STK)((volatile struct RVM_A7M_Cop_Struct *)(RVM_REG->Cop))->S20;
        *(--p_sp) = (OS_STK)((volatile struct RVM_A7M_Cop_Struct *)(RVM_REG->Cop))->S19;
        *(--p_sp) = (OS_STK)((volatile struct RVM_A7M_Cop_Struct *)(RVM_REG->Cop))->S18;
        *(--p_sp) = (OS_STK)((volatile struct RVM_A7M_Cop_Struct *)(RVM_REG->Cop))->S17;
        *(--p_sp) = (OS_STK)((volatile struct RVM_A7M_Cop_Struct *)(RVM_REG->Cop))->S16;
    }
#endif

    *(--p_sp) = (OS_STK)RVM_REG->Reg.LR;
    *(--p_sp) = (OS_STK)RVM_REG->Reg.R11;
    *(--p_sp) = (OS_STK)RVM_REG->Reg.R10;
    *(--p_sp) = (OS_STK)RVM_REG->Reg.R9;
    *(--p_sp) = (OS_STK)RVM_REG->Reg.R8;
    *(--p_sp) = (OS_STK)RVM_REG->Reg.R7;
    *(--p_sp) = (OS_STK)RVM_REG->Reg.R6;
    *(--p_sp) = (OS_STK)RVM_REG->Reg.R5;
    *(--p_sp) = (OS_STK)RVM_REG->Reg.R4;

    *(--p_sp) = (OS_STK)RVM_STATE->Usr.Param[3];
    *(--p_sp) = (OS_STK)RVM_STATE->Usr.Param[2];
    *(--p_sp) = (OS_STK)RVM_STATE->Usr.Param[1];
    *(--p_sp) = (OS_STK)RVM_STATE->Usr.Param[0];
    *(--p_sp) = (OS_STK)RVM_STATE->Usr.Number;

    OSTCBCur->OSTCBStkPtr = p_sp;

    OSTaskSwHook();
    OSPrioCur   = OSPrioHighRdy;
    OSTCBCur    = OSTCBHighRdy;

    p_sp = OSTCBCur->OSTCBStkPtr;

    RVM_STATE->Usr.Number = *(p_sp++);
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

#if RVM_COP_NUM != 0u
    if ((RVM_REG->Reg.LR & 0x10u) == 0u)                        /* Extended frame: restore S16-S31.           */
    {
        ((volatile struct RVM_A7M_Cop_Struct *)(RVM_REG->Cop))->S16 = *(p_sp++);
        ((volatile struct RVM_A7M_Cop_Struct *)(RVM_REG->Cop))->S17 = *(p_sp++);
        ((volatile struct RVM_A7M_Cop_Struct *)(RVM_REG->Cop))->S18 = *(p_sp++);
        ((volatile struct RVM_A7M_Cop_Struct *)(RVM_REG->Cop))->S19 = *(p_sp++);
        ((volatile struct RVM_A7M_Cop_Struct *)(RVM_REG->Cop))->S20 = *(p_sp++);
        ((volatile struct RVM_A7M_Cop_Struct *)(RVM_REG->Cop))->S21 = *(p_sp++);
        ((volatile struct RVM_A7M_Cop_Struct *)(RVM_REG->Cop))->S22 = *(p_sp++);
        ((volatile struct RVM_A7M_Cop_Struct *)(RVM_REG->Cop))->S23 = *(p_sp++);
        ((volatile struct RVM_A7M_Cop_Struct *)(RVM_REG->Cop))->S24 = *(p_sp++);
        ((volatile struct RVM_A7M_Cop_Struct *)(RVM_REG->Cop))->S25 = *(p_sp++);
        ((volatile struct RVM_A7M_Cop_Struct *)(RVM_REG->Cop))->S26 = *(p_sp++);
        ((volatile struct RVM_A7M_Cop_Struct *)(RVM_REG->Cop))->S27 = *(p_sp++);
        ((volatile struct RVM_A7M_Cop_Struct *)(RVM_REG->Cop))->S28 = *(p_sp++);
        ((volatile struct RVM_A7M_Cop_Struct *)(RVM_REG->Cop))->S29 = *(p_sp++);
        ((volatile struct RVM_A7M_Cop_Struct *)(RVM_REG->Cop))->S30 = *(p_sp++);
        ((volatile struct RVM_A7M_Cop_Struct *)(RVM_REG->Cop))->S31 = *(p_sp++);
    }
#endif

    RVM_REG->Reg.SP  = (rvm_ptr_t)p_sp;
}

/*
*********************************************************************************************************
*                                           THE RVM TICK HANDLER
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
