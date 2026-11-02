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

#define   OS_CPU_GLOBALS

#include  <ucos_ii.h>
#include  "rvm.h"

#ifdef __cplusplus
extern  "C" {
#endif

void  OSTaskIdleHook (void) { }
void  OSInitHookBegin (void) { }
void  OSInitHookEnd (void) { }
void  OSTaskCreateHook (OS_TCB *ptcb) { (void)ptcb; }
void  OSTaskDelHook (OS_TCB *ptcb) { (void)ptcb; }
void  OSTaskReturnHook (OS_TCB *ptcb) { (void)ptcb; }
void  OSTaskStatHook (void) { }
void  OSTaskSwHook (void) { }
void  OSTCBInitHook (OS_TCB *ptcb) { (void)ptcb; }
void  OSTimeTickHook (void) { }

OS_CPU_SR  OS_CPU_SR_Save (void)
{
    OS_CPU_SR  cpu_sr;
    cpu_sr = (OS_CPU_SR)RVM_Int_Mask;
    if (cpu_sr == 0u) { RVM_Virt_Int_Mask(); }
    return (cpu_sr);
}

void  OS_CPU_SR_Restore (OS_CPU_SR  cpu_sr)
{
    if (cpu_sr == 0u) { RVM_Virt_Int_Unmask(); }
    else              { RVM_Virt_Int_Mask(); }
}

OS_STK  *OSTaskStkInit (void (*task)(void *), void *p_arg, OS_STK *ptos, INT16U opt)
{
    OS_STK  *p_stk;

    (void)opt;

    p_stk  = &ptos[0];
    p_stk  = (OS_STK *)((OS_STK)(p_stk) & 0xFFFFFFF0u);

    *(--p_stk) = (OS_STK)0x31313131u;
    *(--p_stk) = (OS_STK)0x30303030u;
    *(--p_stk) = (OS_STK)0x29292929u;
    *(--p_stk) = (OS_STK)0x28282828u;
    *(--p_stk) = (OS_STK)0x27272727u;
    *(--p_stk) = (OS_STK)0x26262626u;
    *(--p_stk) = (OS_STK)0x25252525u;
    *(--p_stk) = (OS_STK)0x24242424u;
    *(--p_stk) = (OS_STK)0x23232323u;
    *(--p_stk) = (OS_STK)0x22222222u;
    *(--p_stk) = (OS_STK)0x21212121u;
    *(--p_stk) = (OS_STK)0x20202020u;
    *(--p_stk) = (OS_STK)0x19191919u;
    *(--p_stk) = (OS_STK)0x18181818u;
    *(--p_stk) = (OS_STK)0x17171717u;
    *(--p_stk) = (OS_STK)0x16161616u;
    *(--p_stk) = (OS_STK)0x15151515u;
    *(--p_stk) = (OS_STK)0x14141414u;
    *(--p_stk) = (OS_STK)0x13131313u;
    *(--p_stk) = (OS_STK)0x12121212u;
    *(--p_stk) = (OS_STK)0x11111111u;
    *(--p_stk) = (OS_STK)p_arg;
    *(--p_stk) = (OS_STK)0x09090909u;
    *(--p_stk) = (OS_STK)0x08080808u;
    *(--p_stk) = (OS_STK)0x07070707u;
    *(--p_stk) = (OS_STK)0x06060606u;
    *(--p_stk) = (OS_STK)0x05050505u;
    *(--p_stk) = (OS_STK)0x04040404u;
    *(--p_stk) = (OS_STK)0u;
    *(--p_stk) = (OS_STK)OS_TaskReturn;
    *(--p_stk) = (OS_STK)task;
    *(--p_stk) = (OS_STK)0x00000088u;

    *(--p_stk) = (OS_STK)0u;
    *(--p_stk) = (OS_STK)0u;
    *(--p_stk) = (OS_STK)0u;
    *(--p_stk) = (OS_STK)0u;
    *(--p_stk) = (OS_STK)0u;

    return (p_stk);
}

void  OSIntCtxSw (void)
{
    RVM_Virt_Yield();
}

void  OS_Ctx_Handler (void)
{
    OS_STK  *p_sp;
    OS_STK   stk_mstatus;

    p_sp = (OS_STK *)RVM_REG->Reg.X2_SP;
    p_sp -= 31u;
    p_sp[30u] = (OS_STK)RVM_REG->Reg.X31_T6;
    p_sp[29u] = (OS_STK)RVM_REG->Reg.X30_T5;
    p_sp[28u] = (OS_STK)RVM_REG->Reg.X29_T4;
    p_sp[27u] = (OS_STK)RVM_REG->Reg.X28_T3;
    p_sp[26u] = (OS_STK)RVM_REG->Reg.X27_S11;
    p_sp[25u] = (OS_STK)RVM_REG->Reg.X26_S10;
    p_sp[24u] = (OS_STK)RVM_REG->Reg.X25_S9;
    p_sp[23u] = (OS_STK)RVM_REG->Reg.X24_S8;
    p_sp[22u] = (OS_STK)RVM_REG->Reg.X23_S7;
    p_sp[21u] = (OS_STK)RVM_REG->Reg.X22_S6;
    p_sp[20u] = (OS_STK)RVM_REG->Reg.X21_S5;
    p_sp[19u] = (OS_STK)RVM_REG->Reg.X20_S4;
    p_sp[18u] = (OS_STK)RVM_REG->Reg.X19_S3;
    p_sp[17u] = (OS_STK)RVM_REG->Reg.X18_S2;
    p_sp[16u] = (OS_STK)RVM_REG->Reg.X17_A7;
    p_sp[15u] = (OS_STK)RVM_REG->Reg.X16_A6;
    p_sp[14u] = (OS_STK)RVM_REG->Reg.X15_A5;
    p_sp[13u] = (OS_STK)RVM_REG->Reg.X14_A4;
    p_sp[12u] = (OS_STK)RVM_REG->Reg.X13_A3;
    p_sp[11u] = (OS_STK)RVM_REG->Reg.X12_A2;
    p_sp[10u] = (OS_STK)RVM_REG->Reg.X11_A1;
    p_sp[ 9u] = (OS_STK)RVM_REG->Reg.X10_A0;
    p_sp[ 8u] = (OS_STK)RVM_REG->Reg.X9_S1;
    p_sp[ 7u] = (OS_STK)RVM_REG->Reg.X8_S0_FP;
    p_sp[ 6u] = (OS_STK)RVM_REG->Reg.X7_T2;
    p_sp[ 5u] = (OS_STK)RVM_REG->Reg.X6_T1;
    p_sp[ 4u] = (OS_STK)RVM_REG->Reg.X5_T0;
    p_sp[ 3u] = (OS_STK)RVM_REG->Reg.X4_TP;
    p_sp[ 2u] = (OS_STK)RVM_REG->Reg.X3_GP;
    p_sp[ 1u] = (OS_STK)RVM_REG->Reg.X1_RA;
    p_sp[ 0u] = (OS_STK)RVM_REG->Reg.PC;

    stk_mstatus = (OS_STK)RVM_REG->Reg.MSTATUS;

    p_sp -= 1u;
    p_sp[0u]  = stk_mstatus;

    p_sp -= 5u;
    p_sp[4u]  = (OS_STK)RVM_STATE->Usr.Param[3];
    p_sp[3u]  = (OS_STK)RVM_STATE->Usr.Param[2];
    p_sp[2u]  = (OS_STK)RVM_STATE->Usr.Param[1];
    p_sp[1u]  = (OS_STK)RVM_STATE->Usr.Param[0];
    p_sp[0u]  = (OS_STK)RVM_STATE->Usr.Number;

    OSTCBCur->OSTCBStkPtr = p_sp;

    OSTaskSwHook();
    OSPrioCur   = OSPrioHighRdy;
    OSTCBCur    = OSTCBHighRdy;

    p_sp = OSTCBCur->OSTCBStkPtr;

    RVM_STATE->Usr.Number   = (rvm_ptr_t)p_sp[0u];
    RVM_STATE->Usr.Param[0] = (rvm_ptr_t)p_sp[1u];
    RVM_STATE->Usr.Param[1] = (rvm_ptr_t)p_sp[2u];
    RVM_STATE->Usr.Param[2] = (rvm_ptr_t)p_sp[3u];
    RVM_STATE->Usr.Param[3] = (rvm_ptr_t)p_sp[4u];
    p_sp += 5u;

    stk_mstatus = p_sp[0u];
    p_sp += 1u;

    RVM_REG->Reg.PC       = (rvm_ptr_t)p_sp[0u];
    RVM_REG->Reg.X1_RA    = (rvm_ptr_t)p_sp[1u];
    RVM_REG->Reg.X3_GP    = (rvm_ptr_t)p_sp[2u];
    RVM_REG->Reg.X4_TP    = (rvm_ptr_t)p_sp[3u];
    RVM_REG->Reg.X5_T0    = (rvm_ptr_t)p_sp[4u];
    RVM_REG->Reg.X6_T1    = (rvm_ptr_t)p_sp[5u];
    RVM_REG->Reg.X7_T2    = (rvm_ptr_t)p_sp[6u];
    RVM_REG->Reg.X8_S0_FP = (rvm_ptr_t)p_sp[7u];
    RVM_REG->Reg.X9_S1    = (rvm_ptr_t)p_sp[8u];
    RVM_REG->Reg.X10_A0   = (rvm_ptr_t)p_sp[9u];
    RVM_REG->Reg.X11_A1   = (rvm_ptr_t)p_sp[10u];
    RVM_REG->Reg.X12_A2   = (rvm_ptr_t)p_sp[11u];
    RVM_REG->Reg.X13_A3   = (rvm_ptr_t)p_sp[12u];
    RVM_REG->Reg.X14_A4   = (rvm_ptr_t)p_sp[13u];
    RVM_REG->Reg.X15_A5   = (rvm_ptr_t)p_sp[14u];
    RVM_REG->Reg.X16_A6   = (rvm_ptr_t)p_sp[15u];
    RVM_REG->Reg.X17_A7   = (rvm_ptr_t)p_sp[16u];
    RVM_REG->Reg.X18_S2   = (rvm_ptr_t)p_sp[17u];
    RVM_REG->Reg.X19_S3   = (rvm_ptr_t)p_sp[18u];
    RVM_REG->Reg.X20_S4   = (rvm_ptr_t)p_sp[19u];
    RVM_REG->Reg.X21_S5   = (rvm_ptr_t)p_sp[20u];
    RVM_REG->Reg.X22_S6   = (rvm_ptr_t)p_sp[21u];
    RVM_REG->Reg.X23_S7   = (rvm_ptr_t)p_sp[22u];
    RVM_REG->Reg.X24_S8   = (rvm_ptr_t)p_sp[23u];
    RVM_REG->Reg.X25_S9   = (rvm_ptr_t)p_sp[24u];
    RVM_REG->Reg.X26_S10  = (rvm_ptr_t)p_sp[25u];
    RVM_REG->Reg.X27_S11  = (rvm_ptr_t)p_sp[26u];
    RVM_REG->Reg.X28_T3   = (rvm_ptr_t)p_sp[27u];
    RVM_REG->Reg.X29_T4   = (rvm_ptr_t)p_sp[28u];
    RVM_REG->Reg.X30_T5   = (rvm_ptr_t)p_sp[29u];
    RVM_REG->Reg.X31_T6   = (rvm_ptr_t)p_sp[30u];
    p_sp += 31u;

    RVM_REG->Reg.MSTATUS  = (rvm_ptr_t)stk_mstatus;
    RVM_REG->Reg.X2_SP    = (rvm_ptr_t)p_sp;
}

void  OS_Tick_Handler (void)
{
    OSIntEnter();
    OSTimeTick();
    OSIntExit();
}

void  OS_CPU_Port_Init (void)
{
    RVM_Virt_Tim_Reg(OS_Tick_Handler);
    RVM_Virt_Ctx_Reg(OS_Ctx_Handler);
    RVM_Hyp_Int_Ena();
}

#ifdef __cplusplus
}
#endif
