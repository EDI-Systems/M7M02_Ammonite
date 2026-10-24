/*
*********************************************************************************************************
*                                               uC/OS-III
*                                          The Real-Time Kernel
*
*                               Copyright (C) 2026 Evo-Devo Instrum.
*                                       All rights reserved.
*
*                This file is NOT part of the uC/OS-III distribution.  It belongs
*                to the uC/OS-III-on-RVM test suite and is released under the
*                Unlicense; refer to the LICENSE file for details.  The original
*                uC/OS-III and uC/CPU distributions are copyright Silicon
*                Laboratories Inc. and are not modified by this file.
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

#define   OS_CPU_GLOBALS

#ifdef VSC_INCLUDE_SOURCE_FILE_NAMES
const  CPU_CHAR  *os_cpu_c__c = "$Id: $";
#endif


/*
*********************************************************************************************************
*                                             INCLUDE FILES
*********************************************************************************************************
*/

#include  "os.h"
#include  "rvm.h"


#if (RVM_COP_NUM != 0u)
/* The coprocessor register set of the hypervisor - the S16-S31 half of the */
/* FPv4-SP coprocessor, reachable through the RVM_REG walking pointer.        */
#define  OS_CPU_FPU   ((volatile struct RVM_A7M_Cop_Struct *)(RVM_REG->Cop))
#endif

#ifdef __cplusplus
extern  "C" {
#endif


/*
*********************************************************************************************************
*                                           IDLE TASK HOOK
*
* Description: This function is called by the idle task.  This hook has been added to allow you to do
*              such things as STOP the CPU to conserve power.
*
* Arguments  : None.
*
* Note(s)    : None.
*********************************************************************************************************
*/

void  OSIdleTaskHook (void)
{
#if OS_CFG_APP_HOOKS_EN > 0u
    if (OS_AppIdleTaskHookPtr != (OS_APP_HOOK_VOID)0) {
        (*OS_AppIdleTaskHookPtr)();
    }
#endif
}


/*
*********************************************************************************************************
*                                       OS INITIALIZATION HOOK
*
* Description: This function is called by OSInit() at the beginning of OSInit().
*
* Arguments  : None.
*
* Note(s)    : None.
*********************************************************************************************************
*/

void  OSInitHook (void)
{
                                                                /* The exception stack belongs to the hypervisor under   */
                                                                /* RVM - the guest has no MSP of its own to set up.      */
}


/*
*********************************************************************************************************
*                                           REDZONE HIT HOOK
*
* Description: This function is called when a task's stack overflowed.
*
* Arguments  : p_tcb        Pointer to the task control block of the offending task. NULL if ISR.
*
* Note(s)    : None.
*********************************************************************************************************
*/
#if (OS_CFG_TASK_STK_REDZONE_EN > 0u)
void  OSRedzoneHitHook (OS_TCB  *p_tcb)
{
#if OS_CFG_APP_HOOKS_EN > 0u
    if (OS_AppRedzoneHitHookPtr != (OS_APP_HOOK_TCB)0) {
        (*OS_AppRedzoneHitHookPtr)(p_tcb);
    } else {
        CPU_SW_EXCEPTION(;);
    }
#else
    (void)p_tcb;                                                /* Prevent compiler warning                             */
    CPU_SW_EXCEPTION(;);
#endif
}
#endif


/*
*********************************************************************************************************
*                                         STATISTIC TASK HOOK
*
* Description: This function is called every second by uC/OS-III's statistics task.  This allows your
*              application to add functionality to the statistics task.
*
* Arguments  : None.
*
* Note(s)    : None.
*********************************************************************************************************
*/

void  OSStatTaskHook (void)
{
#if OS_CFG_APP_HOOKS_EN > 0u
    if (OS_AppStatTaskHookPtr != (OS_APP_HOOK_VOID)0) {
        (*OS_AppStatTaskHookPtr)();
    }
#endif
}


/*
*********************************************************************************************************
*                                          TASK CREATION HOOK
*
* Description: This function is called when a task is created.
*
* Arguments  : p_tcb        Pointer to the task control block of the task being created.
*
* Note(s)    : None.
*********************************************************************************************************
*/

void  OSTaskCreateHook (OS_TCB  *p_tcb)
{
#if OS_CFG_APP_HOOKS_EN > 0u
    if (OS_AppTaskCreateHookPtr != (OS_APP_HOOK_TCB)0) {
        (*OS_AppTaskCreateHookPtr)(p_tcb);
    }
#else
    (void)p_tcb;                                                /* Prevent compiler warning                             */
#endif
}


/*
*********************************************************************************************************
*                                           TASK DELETION HOOK
*
* Description: This function is called when a task is deleted.
*
* Arguments  : p_tcb        Pointer to the task control block of the task being deleted.
*
* Note(s)    : None.
*********************************************************************************************************
*/

void  OSTaskDelHook (OS_TCB  *p_tcb)
{
#if OS_CFG_APP_HOOKS_EN > 0u
    if (OS_AppTaskDelHookPtr != (OS_APP_HOOK_TCB)0) {
        (*OS_AppTaskDelHookPtr)(p_tcb);
    }
#else
    (void)p_tcb;                                                /* Prevent compiler warning                             */
#endif
}


/*
*********************************************************************************************************
*                                            TASK RETURN HOOK
*
* Description: This function is called if a task accidentally returns.  In other words, a task should
*              either be an infinite loop or delete itself when done.
*
* Arguments  : p_tcb        Pointer to the task control block of the task that is returning.
*
* Note(s)    : None.
*********************************************************************************************************
*/

void  OSTaskReturnHook (OS_TCB  *p_tcb)
{
#if OS_CFG_APP_HOOKS_EN > 0u
    if (OS_AppTaskReturnHookPtr != (OS_APP_HOOK_TCB)0) {
        (*OS_AppTaskReturnHookPtr)(p_tcb);
    }
#else
    (void)p_tcb;                                                /* Prevent compiler warning                             */
#endif
}


/*
*********************************************************************************************************
*                                        INITIALIZE A TASK'S STACK
*
* Description: This function is called by either OSTaskCreate() or OSTaskCreateExt() to initialize the
*              stack frame of the task being created.  This function is highly processor specific.
*
* Arguments  : p_task       Pointer to the task entry point address.
*
*              p_arg        Pointer to a user supplied data area that will be passed to the task
*                               when the task first executes.
*
*              p_stk_base   Pointer to the base address of the stack.
*
*              stk_size     Size of the stack, in number of CPU_STK elements.
*
*              opt          Options used to alter the behavior of OS_Task_StkInit().
*                            (see OS.H for OS_TASK_OPT_xxx).
*
* Returns    : Always returns the location of the new top-of-stack once the processor registers have
*              been placed on the stack in the proper order.
*
* Note(s)    : (1) Interrupts are enabled when task starts executing.
*
*              (2) All tasks run in Thread mode, using process stack.
*
*             (3) The SP must be 8-byte aligned in conforming to the Procedure Call Standard for the ARM architecture
*
*                    (a) Section 2.1 of the  ABI for the ARM Architecture Advisory Note. SP must be 8-byte aligned
*                        on entry to AAPCS-Conforming functions states :
*
*                        The Procedure Call Standard for the ARM Architecture [AAPCS] requires primitive
*                        data types to be naturally aligned according to their sizes (for size = 1, 2, 4, 8 bytes).
*                        Doing otherwise creates more problems than it solves.
*
*                        In return for preserving the natural alignment of data, conforming code is permitted
*                        to rely on that alignment. To support aligning data allocated on the stack, the stack
*                        pointer (SP) is required to be 8-byte aligned on entry to a conforming function. In
*                        practice this requirement is met if:
*
*                           (1) At each call site, the current size of the calling function's stack frame is a multiple of 8 bytes.
*                               This places an obligation on compilers and assembly language programmers.
*
*                           (2) SP is a multiple of 8 when control first enters a program.
*                               This places an obligation on authors of low level OS, RTOS, and runtime library
*                               code to align SP at all points at which control first enters
*                               a body of (AAPCS-conforming) code.
*
*                       In turn, this requires the value of SP to be aligned to 0 modulo 8:
*
*                           (3) By exception handlers, before calling AAPCS-conforming code.
*
*                           (4) By OS/RTOS/run-time system code, before giving control to an application.
*
*                 (b) Section 2.3.1 corrective steps from the the SP must be 8-byte aligned on entry
*                     to AAPCS-conforming functions advisory note also states.
*
*                     " This requirement extends to operating systems and run-time code for all architecture versions
*                       prior to ARMV7 and to the A, R and M architecture profiles thereafter. Special considerations
*                       associated with ARMV7M are discussed in section 2.3.3"
*
*                     (1) Even if the SP 8-byte alignment is not a requirement for the ARMv7M profile, the stack is aligned
*                         to 8-byte boundaries to support legacy execution environments.
*
*                 (c) Section 5.2.1.2 from the Procedure Call Standard for the ARM
*                     architecture states :  "The stack must also conform to the following
*                     constraint at a public interface:
*
*                     (1) SP mod 8 = 0. The stack must be double-word aligned"
*
*                 (d) From the ARM Technical Support Knowledge Base. 8 Byte stack alignment.
*
*                     "8 byte stack alignment is a requirement of the ARM Architecture Procedure
*                      Call Standard [AAPCS]. This specifies that functions must maintain an 8 byte
*                      aligned stack address (e.g. 0x00, 0x08, 0x10, 0x18, 0x20) on all external
*                      interfaces. In practice this requirement is met if:
*
*                      (1) At each external interface, the current stack pointer
*                          is a multiple of 8 bytes.
*
*                      (2) Your OS maintains 8 byte stack alignment on its external interfaces
*                          e.g. on task switches"
*
*              (4) Exception Return Behavior(EXEC_RETURN)
*                  0xFFFFFFF1      Return to Thread mode. Exception return gets state from the Main Stack.
*                                  On Return execution uses the Main Stack.
*
*                  0xFFFFFFF9      Return to Thread mode. Exception return gets state from the Main Stack.
*                                  On Return execution uses the Main Stack.
*
*                  0xFFFFFFFD      Return to Thread mode. Exception return gets state from the Process Stack.
*                                  On Return execution uses the Process Stack.
**********************************************************************************************************
*/

CPU_STK  *OSTaskStkInit (OS_TASK_PTR    p_task,
                         void          *p_arg,
                         CPU_STK       *p_stk_base,
                         CPU_STK       *p_stk_limit,
                         CPU_STK_SIZE   stk_size,
                         OS_OPT         opt)
{
    CPU_STK    *p_stk;


    (void)opt;                                                  /* 'opt' is not used, prevent warning                   */

    p_stk = &p_stk_base[stk_size];                              /* Load stack pointer                                   */
                                                                /* Align the stack to 8-bytes.                          */
    p_stk = (CPU_STK *)((CPU_STK)(p_stk) & 0xFFFFFFF8u);
                                                                /* Registers stacked as if auto-saved on exception      */
    *(--p_stk) = (CPU_STK)0x01000000u;                          /* xPSR                                                 */
    *(--p_stk) = (CPU_STK)p_task;                               /* Entry Point                                          */
    *(--p_stk) = (CPU_STK)OS_TaskReturn;                        /* R14 (LR)                                             */
    *(--p_stk) = (CPU_STK)0x12121212u;                          /* R12                                                  */
    *(--p_stk) = (CPU_STK)0x03030303u;                          /* R3                                                   */
    *(--p_stk) = (CPU_STK)0x02020202u;                          /* R2                                                   */
    *(--p_stk) = (CPU_STK)p_stk_limit;                          /* R1                                                   */
    *(--p_stk) = (CPU_STK)p_arg;                                /* R0 : argument                                        */
                                                                /* Remaining registers saved on process stack           */
    *(--p_stk) = (CPU_STK)0xFFFFFFFDuL;                         /* R14: EXEC_RETURN; See Note 4                         */
    *(--p_stk) = (CPU_STK)0x11111111uL;                         /* R11                                                  */
    *(--p_stk) = (CPU_STK)0x10101010uL;                         /* R10                                                  */
    *(--p_stk) = (CPU_STK)0x09090909uL;                         /* R9                                                   */
    *(--p_stk) = (CPU_STK)0x08080808uL;                         /* R8                                                   */
    *(--p_stk) = (CPU_STK)0x07070707uL;                         /* R7                                                   */
    *(--p_stk) = (CPU_STK)0x06060606uL;                         /* R6                                                   */
    *(--p_stk) = (CPU_STK)0x05050505uL;                         /* R5                                                   */
    *(--p_stk) = (CPU_STK)0x04040404uL;                         /* R4                                                   */
                                                                /* The RVM hypercall parameters, all clean.            */
    *(--p_stk) = (CPU_STK)0x00000000u;                         /* Param[3].                                           */
    *(--p_stk) = (CPU_STK)0x00000000u;                         /* Param[2].                                           */
    *(--p_stk) = (CPU_STK)0x00000000u;                         /* Param[1].                                           */
    *(--p_stk) = (CPU_STK)0x00000000u;                         /* Param[0].                                           */
    *(--p_stk) = (CPU_STK)0x00000000u;                         /* Number.                                             */

    return (p_stk);
}


/*
*********************************************************************************************************
*                                           TASK SWITCH HOOK
*
* Description: This function is called when a task switch is performed.  This allows you to perform other
*              operations during a context switch.
*
* Arguments  : None.
*
* Note(s)    : 1) Interrupts are disabled during this call.
*              2) It is assumed that the global pointer 'OSTCBHighRdyPtr' points to the TCB of the task
*                 that will be 'switched in' (i.e. the highest priority task) and, 'OSTCBCurPtr' points
*                 to the task being switched out (i.e. the preempted task).
*********************************************************************************************************
*/

void  OSTaskSwHook (void)
{
#if OS_CFG_TASK_PROFILE_EN > 0u
    CPU_TS  ts;
#endif
#ifdef  CPU_CFG_INT_DIS_MEAS_EN
    CPU_TS  int_dis_time;
#endif
#if (OS_CFG_TASK_STK_REDZONE_EN > 0u)
    CPU_BOOLEAN  stk_status;
#endif

#if OS_CFG_APP_HOOKS_EN > 0u
    if (OS_AppTaskSwHookPtr != (OS_APP_HOOK_VOID)0) {
        (*OS_AppTaskSwHookPtr)();
    }
#endif

    OS_TRACE_TASK_SWITCHED_IN(OSTCBHighRdyPtr);

#if OS_CFG_TASK_PROFILE_EN > 0u
    ts = OS_TS_GET();
    if (OSTCBCurPtr != OSTCBHighRdyPtr) {
        OSTCBCurPtr->CyclesDelta  = ts - OSTCBCurPtr->CyclesStart;
        OSTCBCurPtr->CyclesTotal += (OS_CYCLES)OSTCBCurPtr->CyclesDelta;
    }

    OSTCBHighRdyPtr->CyclesStart = ts;
#endif

#ifdef  CPU_CFG_INT_DIS_MEAS_EN
    int_dis_time = CPU_IntDisMeasMaxCurReset();                 /* Keep track of per-task interrupt disable time        */
    if (OSTCBCurPtr->IntDisTimeMax < int_dis_time) {
        OSTCBCurPtr->IntDisTimeMax = int_dis_time;
    }
#endif

#if OS_CFG_SCHED_LOCK_TIME_MEAS_EN > 0u
                                                                /* Keep track of per-task scheduler lock time           */
    if (OSTCBCurPtr->SchedLockTimeMax < OSSchedLockTimeMaxCur) {
        OSTCBCurPtr->SchedLockTimeMax = OSSchedLockTimeMaxCur;
    }
    OSSchedLockTimeMaxCur = (CPU_TS)0;                          /* Reset the per-task value                             */
#endif

#if (OS_CFG_TASK_STK_REDZONE_EN > 0u)
                                                                /* Check if stack overflowed.                           */
    stk_status = OSTaskStkRedzoneChk((OS_TCB *)0u);
    if (stk_status != OS_TRUE) {
        OSRedzoneHitHook(OSTCBCurPtr);
    }
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
* Note(s)    : 1) This function is assumed to be called from the Tick ISR.
*********************************************************************************************************
*/

void  OSTimeTickHook (void)
{
#if OS_CFG_APP_HOOKS_EN > 0u
    if (OS_AppTimeTickHookPtr != (OS_APP_HOOK_VOID)0) {
        (*OS_AppTimeTickHookPtr)();
    }
#endif
}


/*
*********************************************************************************************************
*                                    PERFORM A CONTEXT SWITCH (FROM TASK LEVEL)
*
* Description: This function is called by OSSched() through the OS_TASK_SW() macro when a context
*              switch is needed.  The scheduler has already selected the next task: uC/OS-III
*              computes OSTCBHighRdyPtr before the switch is ever requested, so no scheduler
*              call - and no scheduler peek - happens under the switch, unlike the FreeRTOS and
*              RMP ports of this hypervisor.
*
*              The frame of the selected task is checked for EPSR ICI/IT state first: such
*              interrupted instruction state can only be restored by a real exception return,
*              which the assembly fast path in os_cpu_a.s cannot perform.  Clean frames switch
*              directly on the task stacks through OSCtxSw_Swt(); dirty frames take the slow
*              hypervisor yield, whose real exception return resumes the target exactly like an
*              interrupt would.
*
*              All of this is only compiled when OS_CFG_RVM_FAST_YIELD_EN is enabled in os_cfg.h;
*              when it is disabled, OS_TASK_SW() takes the hypervisor yield on every switch.
*********************************************************************************************************
*/

#if OS_CFG_RVM_FAST_YIELD_EN != 0u

                                                                /* Word offsets into a saved task frame,  */
                                                                /* counted from the StkPtr in the TCB.     */
#define  OS_CPU_OFFSET_TO_LR_EXC                        (13u)   /* The saved EXC_RETURN (bit 4 clear means */
                                                                /* extended).                              */
#define  OS_CPU_OFFSET_TO_XPSR_BASIC                    (21u)   /* The XPSR of a basic frame.              */
#define  OS_CPU_OFFSET_TO_XPSR_EXTENDED                 (37u)   /* The XPSR of an extended frame.          */

#define  OS_CPU_XPSR_ICI_IT_MASK                ((CPU_STK)0x0600FC00u)
#define  OS_CPU_EXC_RETURN_EXTENDED_MASK        ((CPU_STK)0x00000010u)
/*
*********************************************************************************************************
*                                       CHECK A FRAME FOR DIRTY ICI/IT BITS
*
* Description: Check whether the frame of the given task carries EPSR ICI/IT state that only a
*              real exception return can restore.
*
* Argument(s): p_tcb    The task whose saved frame is to be checked.
*
* Return(s)   : DEF_TRUE if the frame is dirty, DEF_FALSE otherwise.
*********************************************************************************************************
*/

static  CPU_BOOLEAN  OS_CPU_Frame_Dirty (OS_TCB  *p_tcb)
{
    CPU_STK  *p_sp;


    p_sp = p_tcb->StkPtr;
    if ((p_sp[OS_CPU_OFFSET_TO_LR_EXC] & OS_CPU_EXC_RETURN_EXTENDED_MASK) == 0u) {
        return ((p_sp[OS_CPU_OFFSET_TO_XPSR_EXTENDED] & OS_CPU_XPSR_ICI_IT_MASK) != 0u) ? DEF_TRUE : DEF_FALSE;
    }
    return ((p_sp[OS_CPU_OFFSET_TO_XPSR_BASIC] & OS_CPU_XPSR_ICI_IT_MASK) != 0u) ? DEF_TRUE : DEF_FALSE;
}


void  OSCtxSw (void)
{
    RVM_Virt_Int_Mask();                                          /* Close the preempt window:   */
                                                                   /* OSSched selects HighRdy and  */
                                                                   /* OSCtxSw_Swt() masks by itself */
                                                                   /* only later - an interrupt in */
                                                                   /* between would let the slow   */
                                                                   /* path rewrite the scheduler   */
                                                                   /* state behind our back.       */
    if (OS_CPU_Frame_Dirty(OSTCBHighRdyPtr) == DEF_TRUE) {        /* Dirty ICI/IT - the slow path, whose   */
        RVM_Virt_Yield();                                          /* real exception return restores it.    */
        RVM_Virt_Int_Unmask();                                     /* Pend under mask; the unmask below     */
                                                                    /* arms the switch at its hypercall      */
                                                                    /* boundary - no unmasked window.        */
    } else {
        OSCtxSw_Swt();                                            /* Clean - switch on the task stacks.     */
    }
}


#endif


/*
*********************************************************************************************************
*                                          THE FAST PATH DIRTY FRAME TRAP
*
* Description: The manual unstacking in os_cpu_a.s jumps here when it meets a frame with dirty
*              ICI/IT bits.  OSCtxSw() above filters such frames out before the fast path is
*              ever taken, so reaching this function means the filter itself is broken - trap
*              loudly instead of corrupting the program flow silently.
*********************************************************************************************************
*/

void  OSCtxSw_Err (void)
{
    CPU_IntDis();
    RVM_DBG_S("UO3 A7M RVM port: dirty XPSR ICI/IT bits on the fast switch path.\r\n");
    for (;;) {
        ;
    }
}




/*
*********************************************************************************************************
*                                   PERFORM A CONTEXT SWITCH (FROM ISR LEVEL)
*
* Description: This function is called by OSIntExit() when a context switch is needed after an
*              interrupt handler has completed.  The RVM delivered vectors behave like the
*              FreeRTOS-on-RVM interrupt path: the posts are made with the no-schedule option,
*              and the switch is requested through the same yield hypercall.
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
*               the RVM_Virt_Yield() hypercall, the hypervisor saves the interrupted context and
*               calls this function with the register block made available through RVM_REG.  The
*               saved registers are moved onto the stack of the task that is being switched out
*               (below the frame that the hypervisor already saved, in the layout documented in
*               the file header), the switch bookkeeping of the official PendSV handler runs, and
*               the registers of the task being switched in are loaded into the register block.
*               Returning re-enters the new task exactly where it called RVM_Virt_Yield().
*********************************************************************************************************
*/

void  OS_Ctx_Handler (void)
{
    CPU_STK  *p_sp;


                                                                /* Save the current context onto its task stack. */
    p_sp = (CPU_STK *)RVM_REG->Reg.SP;

#if (RVM_COP_NUM != 0u)
    if ((RVM_REG->Reg.LR & 0x10u) == 0u) {                      /* Extended frame - stack the S16-S31 half    */
        *(--p_sp) = (CPU_STK)OS_CPU_FPU->S31;                   /* of the coprocessor below the hardware       */
        *(--p_sp) = (CPU_STK)OS_CPU_FPU->S30;                   /* frame, like the lazy stacking does.         */
        *(--p_sp) = (CPU_STK)OS_CPU_FPU->S29;
        *(--p_sp) = (CPU_STK)OS_CPU_FPU->S28;
        *(--p_sp) = (CPU_STK)OS_CPU_FPU->S27;
        *(--p_sp) = (CPU_STK)OS_CPU_FPU->S26;
        *(--p_sp) = (CPU_STK)OS_CPU_FPU->S25;
        *(--p_sp) = (CPU_STK)OS_CPU_FPU->S24;
        *(--p_sp) = (CPU_STK)OS_CPU_FPU->S23;
        *(--p_sp) = (CPU_STK)OS_CPU_FPU->S22;
        *(--p_sp) = (CPU_STK)OS_CPU_FPU->S21;
        *(--p_sp) = (CPU_STK)OS_CPU_FPU->S20;
        *(--p_sp) = (CPU_STK)OS_CPU_FPU->S19;
        *(--p_sp) = (CPU_STK)OS_CPU_FPU->S18;
        *(--p_sp) = (CPU_STK)OS_CPU_FPU->S17;
        *(--p_sp) = (CPU_STK)OS_CPU_FPU->S16;
    }
#endif

    *(--p_sp) = (CPU_STK)RVM_REG->Reg.LR;                       /* The saved EXC_RETURN.                       */
    *(--p_sp) = (CPU_STK)RVM_REG->Reg.R11;
    *(--p_sp) = (CPU_STK)RVM_REG->Reg.R10;
    *(--p_sp) = (CPU_STK)RVM_REG->Reg.R9;
    *(--p_sp) = (CPU_STK)RVM_REG->Reg.R8;
    *(--p_sp) = (CPU_STK)RVM_REG->Reg.R7;
    *(--p_sp) = (CPU_STK)RVM_REG->Reg.R6;
    *(--p_sp) = (CPU_STK)RVM_REG->Reg.R5;
    *(--p_sp) = (CPU_STK)RVM_REG->Reg.R4;

    *(--p_sp) = (CPU_STK)RVM_STATE->Usr.Param[3];               /* The hypercall parameters.                   */
    *(--p_sp) = (CPU_STK)RVM_STATE->Usr.Param[2];
    *(--p_sp) = (CPU_STK)RVM_STATE->Usr.Param[1];
    *(--p_sp) = (CPU_STK)RVM_STATE->Usr.Param[0];
    *(--p_sp) = (CPU_STK)RVM_STATE->Usr.Number;

    OSTCBCurPtr->StkPtr = p_sp;                                 /* Save the new stack top into the TCB.        */

                                                                /* The switch bookkeeping of the official      */
    OSTaskSwHook();                                             /* PendSV handler.                             */
    OSPrioCur     = OSPrioHighRdy;
    OSTCBCurPtr   = OSTCBHighRdyPtr;

    p_sp = OSTCBCurPtr->StkPtr;                                 /* Load the new task's stack.                  */

    RVM_STATE->Usr.Number = *(p_sp++);                          /* Restore the hypercall parameters.           */
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

#if (RVM_COP_NUM != 0u)
    if ((RVM_REG->Reg.LR & 0x10u) == 0u) {                      /* Extended frame - recover the S16-S31 half   */
        OS_CPU_FPU->S16 = *(p_sp++);                            /* of the coprocessor.                         */
        OS_CPU_FPU->S17 = *(p_sp++);
        OS_CPU_FPU->S18 = *(p_sp++);
        OS_CPU_FPU->S19 = *(p_sp++);
        OS_CPU_FPU->S20 = *(p_sp++);
        OS_CPU_FPU->S21 = *(p_sp++);
        OS_CPU_FPU->S22 = *(p_sp++);
        OS_CPU_FPU->S23 = *(p_sp++);
        OS_CPU_FPU->S24 = *(p_sp++);
        OS_CPU_FPU->S25 = *(p_sp++);
        OS_CPU_FPU->S26 = *(p_sp++);
        OS_CPU_FPU->S27 = *(p_sp++);
        OS_CPU_FPU->S28 = *(p_sp++);
        OS_CPU_FPU->S29 = *(p_sp++);
        OS_CPU_FPU->S30 = *(p_sp++);
        OS_CPU_FPU->S31 = *(p_sp++);
    }
#endif

    RVM_REG->Reg.SP  = (rvm_ptr_t)p_sp;                         /* Return to the new task.                     */
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
*               It is called by OSStartHighRdy() before the first task is entered.
*********************************************************************************************************
*/

void  OS_CPU_Port_Init (void)
{
    RVM_Virt_Tim_Reg(OS_Tick_Handler);
    RVM_Virt_Ctx_Reg(OS_Ctx_Handler);
    RVM_Hyp_Int_Ena();
}
