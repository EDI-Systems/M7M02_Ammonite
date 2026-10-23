/*
*********************************************************************************************************
*                                               uC/OS-III
*                                         The Real-Time Kernel
*
*                                       RVM Hypervisor Guest Port
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
*                                            RISC-V Port - os_cpu_c.c
*
* File      : os_cpu_c.c
* Version   : V1.00.00
*********************************************************************************************************
*/


#define  OS_CPU_GLOBALS

#ifdef VSC_INCLUDE_SOURCE_FILE_NAMES
const  CPU_CHAR  *os_cpu_c__c = "$Id: $";
#endif


/*
*********************************************************************************************************
*                                            INCLUDE FILES
*********************************************************************************************************
*/

#include  "../../../../Source/os.h"
#include  "rvm.h"
#include  "os_cpu.h"

extern  CPU_STK  __RVM_Global;                                  /* The gp value for the user land, from the linker script. */


/*
*********************************************************************************************************
*                                           LOCAL VARIABLES
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                     EXTERNAL C LANGUAGE LINKAGE
*
* Note(s) : (1) C++ compilers MUST 'extern'ally declare ALL C function prototypes & variable/object
*               declarations for correct C language linkage.
*********************************************************************************************************
*/

#ifdef __cplusplus
extern  "C" {                                    /* See Note #1.                                       */
#endif


/*
*********************************************************************************************************
*                                             IDLE TASK HOOK
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
*                                          TASK DELETION HOOK
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
*                                           TASK RETURN HOOK
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
* Description: This function is called by OSTaskCreate() to initialize the stack frame of the task being
*              created. This function is highly processor specific.
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
* Note(s)    : 1) Interrupts are enabled when task starts executing.
*
*              2) There is no need to save register x0 since it is a hard-wired zero.
*
*              3) RISC-V calling convention register usage:
*
*                    +-------------+-------------+----------------------------------+
*                    |  Register   |   ABI Name  | Description                      |
*                    +-------------+-------------+----------------------------------+
*                    |  x31 - x28  |   t6 - t3   | Temporaries                      |
*                    +-------------+-------------+----------------------------------+
*                    |  x27 - x18  |  s11 - s2   | Saved registers                  |
*                    +-------------+-------------+----------------------------------+
*                    |  x17 - x12  |   a7 - a2   | Function arguments               |
*                    +-------------+-------------+----------------------------------+
*                    |  x11 - x10  |   a1 - a0   | Function arguments/return values |
*                    +-------------+-------------+----------------------------------+
*                    |     x9      |     s1      | Saved register                   |
*                    +-------------+-------------+----------------------------------+
*                    |     x8      |    s0/fp    | Saved register/frame pointer     |
*                    +-------------+-------------+----------------------------------+
*                    |   x7 - x5   |   t2 - t0   | Temporaries                      |
*                    +-------------+-------------+----------------------------------+
*                    |     x4      |     tp      | Thread pointer                   |
*                    +-------------+-------------+----------------------------------+
*                    |     x3      |     gp      | Global pointer                   |
*                    +-------------+-------------+----------------------------------+
*                    |     x2      |     sp      | Stack pointer                    |
*                    +-------------+-------------+----------------------------------+
*                    |     x1      |     ra      | return address                   |
*                    +-------------+-------------+----------------------------------+
*                    |     x0      |    zero     | Hard-wired zero                  |
*                    +-------------+-------------+----------------------------------+
*
*********************************************************************************************************
*/

CPU_STK  *OSTaskStkInit (OS_TASK_PTR    p_task,
                         void          *p_arg,
                         CPU_STK       *p_stk_base,
                         CPU_STK       *p_stk_limit,
                         CPU_STK_SIZE   stk_size,
                         OS_OPT         opt)
{
    CPU_STK  *p_stk;


    (void)p_stk_limit;                         /* Prevent compiler warning                             */
    (void)opt;

    p_stk = &p_stk_base[stk_size];             /* Load stack pointer and align it to 16-bytes          */
    p_stk = (CPU_STK *)((CPU_STK)(p_stk) & 0xFFFFFFF0u);

    *(--p_stk) = (CPU_STK) 0x31313131uL;              /* x31/t6                                */
    *(--p_stk) = (CPU_STK) 0x30303030uL;              /* x30/t5                                */
    *(--p_stk) = (CPU_STK) 0x29292929uL;              /* x29/t4                                */
    *(--p_stk) = (CPU_STK) 0x28282828uL;              /* x28/t3                                */
    *(--p_stk) = (CPU_STK) 0x27272727uL;              /* x27/s11                               */
    *(--p_stk) = (CPU_STK) 0x26262626uL;              /* x26/s10                               */
    *(--p_stk) = (CPU_STK) 0x25252525uL;              /* x25/s9                                */
    *(--p_stk) = (CPU_STK) 0x24242424uL;              /* x24/s8                                */
    *(--p_stk) = (CPU_STK) 0x23232323uL;              /* x23/s7                                */
    *(--p_stk) = (CPU_STK) 0x22222222uL;              /* x22/s6                                */
    *(--p_stk) = (CPU_STK) 0x21212121uL;              /* x21/s5                                */
    *(--p_stk) = (CPU_STK) 0x20202020uL;              /* x20/s4                                */
    *(--p_stk) = (CPU_STK) 0x19191919uL;              /* x19/s3                                */
    *(--p_stk) = (CPU_STK) 0x18181818uL;              /* x18/s2                                */
    *(--p_stk) = (CPU_STK) 0x17171717uL;              /* x17/a7                                */
    *(--p_stk) = (CPU_STK) 0x16161616uL;              /* x16/a6                                */
    *(--p_stk) = (CPU_STK) 0x15151515uL;              /* x15/a5                                */
    *(--p_stk) = (CPU_STK) 0x14141414uL;              /* x14/a4                                */
    *(--p_stk) = (CPU_STK) 0x13131313uL;              /* x13/a3                                */
    *(--p_stk) = (CPU_STK) 0x12121212uL;              /* x12/a2                                */
    *(--p_stk) = (CPU_STK) 0x11111111uL;              /* x11/a1                                */
    *(--p_stk) = (CPU_STK) p_arg;                     /* x10/a0 : argument                     */
    *(--p_stk) = (CPU_STK) 0x09090909uL;              /* x9/s1                                 */
    *(--p_stk) = (CPU_STK) 0x08080808uL;              /* x8/s0 : frame pointer                 */
    *(--p_stk) = (CPU_STK) 0x07070707uL;              /* x7/t2                                 */
    *(--p_stk) = (CPU_STK) 0x06060606uL;              /* x6/t1                                 */
    *(--p_stk) = (CPU_STK) 0x05050505uL;              /* x5/t0                                 */
    *(--p_stk) = (CPU_STK) 0x04040404uL;              /* x4/tp : not restored                  */
    *(--p_stk) = (CPU_STK) __RVM_Global;              /* x3/gp : the user global pointer       */
    *(--p_stk) = (CPU_STK) OS_TaskReturn;             /* x1/ra : task return address           */
    *(--p_stk) = (CPU_STK) p_task;                    /* pc   : entry point                    */

#if (RVM_RV32P_COP_RVF != 0) || (RVM_RV32P_COP_RVD != 0)
    *(--p_stk) = (CPU_STK) 0x00002088uL;                  /* mstatus: U-mode, FS=Initial - the first floating    */
                                                /* point instruction marks the state dirty and the     */
                                                /* context is lazily saved from there on.              */
#else
    *(--p_stk) = (CPU_STK) 0x00000088uL;                  /* mstatus: U-mode, FS=Off - no coprocessor.           */
#endif

    *(--p_stk) = (CPU_STK) 0u;                            /* The hypercall parameter words.                      */
    *(--p_stk) = (CPU_STK) 0u;
    *(--p_stk) = (CPU_STK) 0u;
    *(--p_stk) = (CPU_STK) 0u;
    *(--p_stk) = (CPU_STK) 0u;

    return (p_stk);
}


/*
*********************************************************************************************************
*                                          TASK SWITCH HOOK
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
*                                               TICK HOOK
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
#if (CPU_CFG_TS_EN > 0u)
    CPU_TS_Update();
#endif
}


/*
*********************************************************************************************************
/*
*********************************************************************************************************
*                                    PERFORM A CONTEXT SWITCH (FROM ISR LEVEL)
*
* Description: This function is called by OSIntExit() when a context switch is needed after an
*              interrupt handler has completed.  The RVM delivered vectors behave like the
*              FreeRTOS-on-RVM interrupt path: the switch is requested through the same yield
*              hypercall.
*********************************************************************************************************
*/

void  OSIntCtxSw (void)
{
    RVM_Virt_Yield();
}


/*
*********************************************************************************************************
*                                        THE RVM CONTEXT SWITCH HANDLER
*
* Description: This handler is registered through RVM_Virt_Ctx_Reg().  When a task yields through
*               the RVM_Virt_Yield() hypercall, the hypervisor saves the interrupted context and
*               calls this function with the register block made available through RVM_REG.  The
*               saved registers are moved onto the stack of the task that is being switched out
*               (in the frame layout documented in the file header), the switch bookkeeping of
*               the official port runs, and the registers of the task being switched in are
*               loaded into the register block.  Returning re-enters the new task exactly where
*               it called RVM_Virt_Yield().
*
* Note(s)    : 1) The floating point context of the F and D extensions, when the port is built
*               with them, is held in the hypervisor coprocessor block RVM_REG->Cop.  The FS
*               field of the saved mstatus decides whether the frame carries the floating point
*               registers.  The F0..F31 fields of the coprocessor structure are laid out
*               contiguously, so they are copied through a single walking pointer.
*********************************************************************************************************
*/

void  OS_Ctx_Handler (void)
{
#if (RVM_RV32P_COP_RVF != 0) || (RVM_RV32P_COP_RVD != 0)
    volatile struct RVM_RV32P_Cop_Struct  *p_cop;
    rvm_ptr_t                             *p_fpu;
    CPU_INT08U                             i;
#endif
    CPU_STK  *p_sp;
    CPU_STK   stk_mstatus;


                                                                /* Save the current context onto its task stack. */
    p_sp = (CPU_STK *)RVM_REG->Reg.X2_SP;
    p_sp -= 31u;                                                /* The integer registers.                        */
    p_sp[30u] = (CPU_STK)RVM_REG->Reg.X31_T6;
    p_sp[29u] = (CPU_STK)RVM_REG->Reg.X30_T5;
    p_sp[28u] = (CPU_STK)RVM_REG->Reg.X29_T4;
    p_sp[27u] = (CPU_STK)RVM_REG->Reg.X28_T3;
    p_sp[26u] = (CPU_STK)RVM_REG->Reg.X27_S11;
    p_sp[25u] = (CPU_STK)RVM_REG->Reg.X26_S10;
    p_sp[24u] = (CPU_STK)RVM_REG->Reg.X25_S9;
    p_sp[23u] = (CPU_STK)RVM_REG->Reg.X24_S8;
    p_sp[22u] = (CPU_STK)RVM_REG->Reg.X23_S7;
    p_sp[21u] = (CPU_STK)RVM_REG->Reg.X22_S6;
    p_sp[20u] = (CPU_STK)RVM_REG->Reg.X21_S5;
    p_sp[19u] = (CPU_STK)RVM_REG->Reg.X20_S4;
    p_sp[18u] = (CPU_STK)RVM_REG->Reg.X19_S3;
    p_sp[17u] = (CPU_STK)RVM_REG->Reg.X18_S2;
    p_sp[16u] = (CPU_STK)RVM_REG->Reg.X17_A7;
    p_sp[15u] = (CPU_STK)RVM_REG->Reg.X16_A6;
    p_sp[14u] = (CPU_STK)RVM_REG->Reg.X15_A5;
    p_sp[13u] = (CPU_STK)RVM_REG->Reg.X14_A4;
    p_sp[12u] = (CPU_STK)RVM_REG->Reg.X13_A3;
    p_sp[11u] = (CPU_STK)RVM_REG->Reg.X12_A2;
    p_sp[10u] = (CPU_STK)RVM_REG->Reg.X11_A1;
    p_sp[ 9u] = (CPU_STK)RVM_REG->Reg.X10_A0;
    p_sp[ 8u] = (CPU_STK)RVM_REG->Reg.X9_S1;
    p_sp[ 7u] = (CPU_STK)RVM_REG->Reg.X8_S0_FP;
    p_sp[ 6u] = (CPU_STK)RVM_REG->Reg.X7_T2;
    p_sp[ 5u] = (CPU_STK)RVM_REG->Reg.X6_T1;
    p_sp[ 4u] = (CPU_STK)RVM_REG->Reg.X5_T0;
    p_sp[ 3u] = (CPU_STK)RVM_REG->Reg.X4_TP;
    p_sp[ 2u] = (CPU_STK)RVM_REG->Reg.X3_GP;
    p_sp[ 1u] = (CPU_STK)RVM_REG->Reg.X1_RA;
    p_sp[ 0u] = (CPU_STK)RVM_REG->Reg.PC;

    stk_mstatus = (CPU_STK)RVM_REG->Reg.MSTATUS;                /* The FS field decides whether the frame        */
                                                                /* carries floating point registers.             */
#if (RVM_RV32P_COP_RVF != 0) || (RVM_RV32P_COP_RVD != 0)
    if ((stk_mstatus & 0x00004000uL) != 0u) {                   /* FPU registers in the frame?                   */
        p_cop = (volatile struct RVM_RV32P_Cop_Struct *)(RVM_REG->Cop);
        p_fpu  = (rvm_ptr_t *)&p_cop->F0;
#if (RVM_RV32P_COP_RVD != 0)
        p_sp  -= 65u;                                           /* The double-precision registers and the FCSR.  */
        p_sp[64u] = (CPU_STK)p_cop->FCSR;
        for (i = 0u; i < 32u; i++) {
            p_sp[i * 2u]      = (CPU_STK)p_fpu[i * 2u];
            p_sp[i * 2u + 1u] = (CPU_STK)p_fpu[i * 2u + 1u];
        }
#else
        p_sp  -= 33u;                                           /* The single-precision registers and the FCSR.  */
        p_sp[32u] = (CPU_STK)p_cop->FCSR;
        for (i = 0u; i < 32u; i++) {
            p_sp[i] = (CPU_STK)p_fpu[i];
        }
#endif
    }
#endif

    p_sp -= 1u;                                                 /* The saved mstatus.                            */
    p_sp[0u]  = stk_mstatus;

    p_sp -= 5u;                                                 /* The hypercall parameter words.                */
    p_sp[4u]  = (CPU_STK)RVM_STATE->Usr.Param[3];
    p_sp[3u]  = (CPU_STK)RVM_STATE->Usr.Param[2];
    p_sp[2u]  = (CPU_STK)RVM_STATE->Usr.Param[1];
    p_sp[1u]  = (CPU_STK)RVM_STATE->Usr.Param[0];
    p_sp[0u]  = (CPU_STK)RVM_STATE->Usr.Number;

    OSTCBCurPtr->StkPtr = p_sp;                                 /* Save the new stack top into the TCB.          */

                                                                /* The switch bookkeeping of the official port.  */
    OSTaskSwHook();

    OSPrioCur   = OSPrioHighRdy;
    OSTCBCurPtr = OSTCBHighRdyPtr;

                                                                /* Load the new task context from its stack.     */
    p_sp = OSTCBCurPtr->StkPtr;

    RVM_STATE->Usr.Number   = (rvm_ptr_t)p_sp[0u];               /* The hypercall parameter words.                */
    RVM_STATE->Usr.Param[0] = (rvm_ptr_t)p_sp[1u];
    RVM_STATE->Usr.Param[1] = (rvm_ptr_t)p_sp[2u];
    RVM_STATE->Usr.Param[2] = (rvm_ptr_t)p_sp[3u];
    RVM_STATE->Usr.Param[3] = (rvm_ptr_t)p_sp[4u];
    p_sp += 5u;

    stk_mstatus = p_sp[0u];                                     /* The saved mstatus.                            */
    p_sp += 1u;
#if (RVM_RV32P_COP_RVF != 0) || (RVM_RV32P_COP_RVD != 0)
    if ((stk_mstatus & 0x00004000uL) != 0u) {                   /* FPU registers in the frame?                   */
        p_cop = (volatile struct RVM_RV32P_Cop_Struct *)(RVM_REG->Cop);
        p_fpu  = (rvm_ptr_t *)&p_cop->F0;
#if (RVM_RV32P_COP_RVD != 0)
        for (i = 0u; i < 32u; i++) {
            p_fpu[i * 2u]      = (rvm_ptr_t)p_sp[i * 2u];
            p_fpu[i * 2u + 1u] = (rvm_ptr_t)p_sp[i * 2u + 1u];
        }
        p_cop->FCSR = (rvm_ptr_t)p_sp[64u];
        p_sp += 65u;
#else
        for (i = 0u; i < 32u; i++) {
            p_fpu[i] = (rvm_ptr_t)p_sp[i];
        }
        p_cop->FCSR = (rvm_ptr_t)p_sp[32u];
        p_sp += 33u;
#endif
    }
#endif
    RVM_REG->Reg.MSTATUS = (rvm_ptr_t)stk_mstatus;

    RVM_REG->Reg.PC      = (rvm_ptr_t)p_sp[ 0u];                /* The integer registers.                        */
    RVM_REG->Reg.X1_RA   = (rvm_ptr_t)p_sp[ 1u];
    RVM_REG->Reg.X3_GP   = (rvm_ptr_t)p_sp[ 2u];
    RVM_REG->Reg.X4_TP   = (rvm_ptr_t)p_sp[ 3u];
    RVM_REG->Reg.X5_T0   = (rvm_ptr_t)p_sp[ 4u];
    RVM_REG->Reg.X6_T1   = (rvm_ptr_t)p_sp[ 5u];
    RVM_REG->Reg.X7_T2   = (rvm_ptr_t)p_sp[ 6u];
    RVM_REG->Reg.X8_S0_FP = (rvm_ptr_t)p_sp[ 7u];
    RVM_REG->Reg.X9_S1   = (rvm_ptr_t)p_sp[ 8u];
    RVM_REG->Reg.X10_A0  = (rvm_ptr_t)p_sp[ 9u];
    RVM_REG->Reg.X11_A1  = (rvm_ptr_t)p_sp[10u];
    RVM_REG->Reg.X12_A2  = (rvm_ptr_t)p_sp[11u];
    RVM_REG->Reg.X13_A3  = (rvm_ptr_t)p_sp[12u];
    RVM_REG->Reg.X14_A4  = (rvm_ptr_t)p_sp[13u];
    RVM_REG->Reg.X15_A5  = (rvm_ptr_t)p_sp[14u];
    RVM_REG->Reg.X16_A6  = (rvm_ptr_t)p_sp[15u];
    RVM_REG->Reg.X17_A7  = (rvm_ptr_t)p_sp[16u];
    RVM_REG->Reg.X18_S2  = (rvm_ptr_t)p_sp[17u];
    RVM_REG->Reg.X19_S3  = (rvm_ptr_t)p_sp[18u];
    RVM_REG->Reg.X20_S4  = (rvm_ptr_t)p_sp[19u];
    RVM_REG->Reg.X21_S5  = (rvm_ptr_t)p_sp[20u];
    RVM_REG->Reg.X22_S6  = (rvm_ptr_t)p_sp[21u];
    RVM_REG->Reg.X23_S7  = (rvm_ptr_t)p_sp[22u];
    RVM_REG->Reg.X24_S8  = (rvm_ptr_t)p_sp[23u];
    RVM_REG->Reg.X25_S9  = (rvm_ptr_t)p_sp[24u];
    RVM_REG->Reg.X26_S10 = (rvm_ptr_t)p_sp[25u];
    RVM_REG->Reg.X27_S11 = (rvm_ptr_t)p_sp[26u];
    RVM_REG->Reg.X28_T3  = (rvm_ptr_t)p_sp[27u];
    RVM_REG->Reg.X29_T4  = (rvm_ptr_t)p_sp[28u];
    RVM_REG->Reg.X30_T5  = (rvm_ptr_t)p_sp[29u];
    RVM_REG->Reg.X31_T6  = (rvm_ptr_t)p_sp[30u];
    RVM_REG->Reg.X2_SP   = (rvm_ptr_t)(p_sp + 31u);
}


/*
*********************************************************************************************************
*                                            THE RVM TICK HANDLER
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
*                                       INITIALIZE THE RVM PORTING LAYER
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

/*
*********************************************************************************************************
*                                   EXTERNAL C LANGUAGE LINKAGE END
*********************************************************************************************************
*/

#ifdef __cplusplus
}                                                 /* End of 'extern'al C lang linkage.                 */
#endif
