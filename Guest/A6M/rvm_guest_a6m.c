/******************************************************************************
Filename    : rvm_guest_a6m.c
Author      : pry
Date        : 09/02/2018
Licence     : The Unlicense; see LICENSE for details.
Description : The guest OS Cortex-M wrapper.
******************************************************************************/

/* Includes ******************************************************************/
#include "rvm.h"

#define __HDR_PRIVATE_MEMBERS__
#include "rvm_guest_conf.h"
#undef __HDR_PRIVATE_MEMBERS__

#include "rvm_guest.h"
/* End Includes **************************************************************/

/* Begin Function:RVM_A6M_Kfn_Act *********************************************
Description : Activate kernel functions that must use ARMv7-M specific calling
              convention to pass extra parameters.
Input       : rvm_cid_t Cap_Kfn - The capability to the kernel capability. 2-Level.
              rvm_ptr_t Func_ID - The function ID to invoke.
              rvm_ptr_t Sub_ID - The subfunction ID to invoke.
              rvm_ptr_t* Param - The parameters, stored in an array of size 6.
Output      : rvm_ptr_t* Param - The return values, stored in an array of size 6.
Return      : rvm_ret_t - If successful, 0; else a negative value.
******************************************************************************/
rvm_ret_t RVM_A6M_Kfn_Act(rvm_cid_t Cap_Kfn,
                          rvm_ptr_t Func_ID,
                          rvm_ptr_t Sub_ID,
                          rvm_ptr_t* Param)
{
    return RVM_A6M_Svc_Kfn((RVM_SVC_KFN<<(sizeof(rvm_ptr_t)*4U))|((rvm_ptr_t)Cap_Kfn),
                           RVM_PARAM_D1(Sub_ID)|RVM_PARAM_D0(Func_ID),
                           Param);
}
/* End Function:RVM_A6M_Kfn_Act *********************************************/

/* Begin Function:RVM_A6M_Pgt_Entry_Mod *************************************
Description : Consult or modify the page table attributes. ARMv7-M only allows 
              consulting page table attributes but does not allow modifying them,
              because there are no architecture-specific flags.
Input       : rvm_cid_t Cap_Kfn - The kernel function capability.
              rvm_cid_t Cap_Pgt - The capability to the top-level page table to consult.
              rvm_ptr_t Vaddr - The virtual address to consult.
              rvm_ptr_t Type - The consult type, see manual for details.
Output      : None.
Return      : rvm_ret_t - If successful, the flags; else an error code.
******************************************************************************/
rvm_ret_t RVM_A6M_Pgt_Entry_Mod(rvm_cid_t Cap_Kfn,
                                rvm_cid_t Cap_Pgt,
                                rvm_ptr_t Vaddr,
                                rvm_ptr_t Type)
{
    return RVM_Kfn_Act(Cap_Kfn, RVM_KFN_PGT_ENTRY_MOD, (rvm_ptr_t)Cap_Pgt, Vaddr, Type);
}
/* End Function:RVM_A6M_Pgt_Entry_Mod **************************************/

/* Begin Function:RVM_A6M_Int_Local_Mod ***************************************
Description : Consult or modify the local interrupt controller's vector state.
Input       : rvm_cid_t Cap_Kfn - The kernel function capability.
              rvm_ptr_t Int_Num - The interrupt number to consult or modify.
              rvm_ptr_t Operation - The operation to conduct.
              rvm_ptr_t Param - The parameter, could be state or priority.
Output      : None.
Return      : rvm_ret_t - If successful, 0 or the desired value; else an error code.
******************************************************************************/
rvm_ret_t RVM_A6M_Int_Local_Mod(rvm_cid_t Cap_Kfn,
                                rvm_ptr_t Int_Num,
                                rvm_ptr_t Operation,
                                rvm_ptr_t Param)
{
    return RVM_Kfn_Act(Cap_Kfn, RVM_KFN_INT_LOCAL_MOD, Int_Num, Operation, Param);
}
/* End Function:RVM_A6M_Int_Local_Mod ****************************************/

/* Begin Function:RVM_A6M_Int_Local_Trig **************************************
Description : Trigger a CPU's local event source.
Input       : rvm_cid_t Cap_Kfn - The kernel function capability.
              rvm_ptr_t Evt_Num - The event ID.
Output      : None.
Return      : rvm_ret_t - If successful, 0; else an error code.
******************************************************************************/
rvm_ret_t RVM_A6M_Int_Local_Trig(rvm_cid_t Cap_Kfn,
                                 rvm_ptr_t Int_Num)
{
    return RVM_Kfn_Act(Cap_Kfn, RVM_KFN_INT_LOCAL_TRIG, 0, Int_Num, 0);
}
/* End Function:RVM_A6M_Int_Local_Trig ***************************************/

/* Begin Function:RVM_A6M_Prfth_Mod *******************************************
Description : Modify prefetcher state. Due to the fact that the ARMv7-M architectural
              prefetch is usually permanently enabled, this only controls manufacturer
              specific Flash accelerators. The accelerator is always enabled at
              power-on, and the only reason you want to fiddle with it is to save power.
Input       : rvm_cid_t Cap_Kfn - The kernel function capability.
              rvm_ptr_t Prfth_ID - The ID of the prefetcher to enable, disable or consult.
              rvm_ptr_t Operation - The operation to perform.
              rvm_ptr_t Param - The parameter.
Output      : None.
Return      : If successful, 0; else RVM_ERR_KERN_OPFAIL.
******************************************************************************/
rvm_ret_t RVM_A6M_Prfth_Mod(rvm_cid_t Cap_Kfn,
                            rvm_ptr_t Prfth_ID,
                            rvm_ptr_t Operation,
                            rvm_ptr_t Param)
{
    return RVM_Kfn_Act(Cap_Kfn, RVM_KFN_PRFTH_MOD, Prfth_ID, Operation, Param);
}
/* End Function:RVM_A6M_Prfth_Mod ********************************************/

/* Begin Function:RVM_A6M_Perf_CPU_Func ***************************************
Description : CPU feature detection for ARMv7-M.
Input       : rvm_cid_t Cap_Kfn - The kernel function capability.
              rvm_ptr_t Freg_ID - The capability to the thread to consult.
Output      : rvm_ptr_t* Content - The content of the register.
Return      : rvm_ret_t - If successful, 0;  else an error code.
******************************************************************************/
rvm_ret_t RVM_A6M_Perf_CPU_Func(rvm_cid_t Cap_Kfn,
                                rvm_ptr_t Freg_ID,
                                rvm_ptr_t* Content)
{
    rvm_ptr_t Params[6];
    rvm_ret_t Retval;

    Retval=RVM_A6M_Kfn_Act(Cap_Kfn, RVM_KFN_PERF_CPU_FUNC, Freg_ID, Params);

    if(Retval<0)
        return Retval;

    if(Content!=0)
        Content[0]=Params[0];

    return Retval;
}
/* End Function:RVM_A6M_Perf_CPU_Func ****************************************/

/* Begin Function:RVM_A6M_Perf_Mon_Mod ****************************************
Description : Read or write performance monitor settings. This only works for
              a single performance counter, CYCCNT, and only works for enabling 
              or disabling operations.
Input       : rvm_ptr_t Perf_ID - The performance monitor identifier.
              rvm_ptr_t Operation - The operation to perform.
              rvm_ptr_t Param - An extra parameter.
Output      : None.
Return      : rvm_ret_t - If successful, the desired value; if a negative value, failed.
******************************************************************************/
rvm_ret_t RVM_A6M_Perf_Mon_Mod(rvm_cid_t Cap_Kfn,
                               rvm_ptr_t Perf_ID,
                               rvm_ptr_t Operation,
                               rvm_ptr_t Param)
{
    return RVM_Kfn_Act(Cap_Kfn, RVM_KFN_PERF_MON_MOD, Perf_ID, Operation, Param);
}
/* End Function:RVM_A6M_Perf_Mon_Mod *****************************************/

/* Begin Function:RVM_A6M_Perf_Cycle_Mod **************************************
Description : Cycle performance counter read or write for ARMv7-M. Only supports
              CYCCNT register.
Input       : rvm_cid_t Cap_Kfn - The kernel function capability.
              rvm_ptr_t Cycle_ID - The performance counter to read or write.
Output      : rvm_ptr_t* Content - The content of the register in read mode.
Return      : rvm_ret_t - If successful, 0; if a negative value, failed.
******************************************************************************/
rvm_ret_t RVM_A6M_Perf_Cycle_Mod(rvm_cid_t Cap_Kfn,
                                 rvm_ptr_t Cycle_ID, 
                                 rvm_ptr_t Operation,
                                 rvm_ptr_t Value,
                                 rvm_ptr_t* Content)
{
    rvm_ptr_t Params[6];
    rvm_ret_t Retval;

    Params[0]=Operation;
    Params[1]=Value;
    Retval=RVM_A6M_Kfn_Act(Cap_Kfn, RVM_KFN_PERF_CYCLE_MOD, Cycle_ID, Params);

    if(Retval<0)
        return Retval;

    if(Content!=RVM_NULL)
        Content[0]=Params[0];

    return Retval;
}
/* End Function:RVM_A6M_Perf_Cycle_Mod ***************************************/

/* Begin Function:RVM_A6M_Debug_Print *****************************************
Description : Debug printing function (through the kernel) for ARMv7-M. This is 
              a blocking system call through the serial port.
Input       : rvm_cid_t Cap_Kfn - The kernel function capability.
              rvm_cid_t Cap_Thd - The capability to the thread to consult.
              rvm_ptr_t Operation - The operation, e.g. which register to read or write.
              rvm_ptr_t Value - The value to write into the register.
Output      : rvm_ptr_t* Content - The content of the register in read mode.
Return      : rvm_ret_t - If successful, 0; if a negative value, failed.
******************************************************************************/
rvm_ret_t RVM_A6M_Debug_Print(rvm_cid_t Cap_Kfn,
                              char Char)
{
    return RVM_Kfn_Act(Cap_Kfn, RVM_KFN_DEBUG_PRINT, (rvm_ptr_t)Char, 0U, 0U);
}
/* End Function:RVM_A6M_Debug_Print ******************************************/

/* Begin Function:RVM_A6M_Debug_Reg_Mod ***************************************
Description : Debug regular register modification implementation for ARMv7-M.
Input       : rvm_cid_t Cap_Kfn - The kernel function capability.
              rvm_cid_t Cap_Thd - The capability to the thread to consult.
              rvm_ptr_t Operation - The operation, e.g. which register to read or write.
              rvm_ptr_t Value - The value to write into the register.
Output      : rvm_ptr_t* Content - The content of the register in read mode.
Return      : rvm_ret_t - If successful, 0; if a negative value, failed.
******************************************************************************/
rvm_ret_t RVM_A6M_Debug_Reg_Mod(rvm_cid_t Cap_Kfn,
                                rvm_cid_t Cap_Thd, 
                                rvm_ptr_t Operation,
                                rvm_ptr_t Value,
                                rvm_ptr_t* Content)
{
    rvm_ptr_t Params[6];
    rvm_ret_t Retval;

    Params[0]=Operation;
    Params[1]=Value;
    Retval=RVM_A6M_Kfn_Act(Cap_Kfn, RVM_KFN_DEBUG_REG_MOD, (rvm_ptr_t)Cap_Thd, Params);

    if(Retval<0)
        return Retval;

    if(Content!=0)
        Content[0]=Params[0];

    return Retval;
}
/* End Function:RVM_A6M_Debug_Reg_Mod ****************************************/

/* Begin Function:RVM_A6M_Debug_Inv_Mod ***************************************
Description : Debug invocation register modification implementation for ARMv7-M.
Input       : rvm_cid_t Cap_Kfn - The kernel function capability.
              rvm_cid_t Cap_Thd - The capability to the thread to consult.
              rvm_ptr_t Layer - The layer to consult, 0 is the invocation stack top.
              rvm_ptr_t Operation - The operation, e.g. which register to read or write.
              rvm_ptr_t Value - The value to write into the register.
Output      : rvm_ptr_t* Content - The content of the register in read mode.
Return      : rvm_ret_t - If successful, 0; if a negative value, failed.
******************************************************************************/
rvm_ret_t RVM_A6M_Debug_Inv_Mod(rvm_cid_t Cap_Kfn,
                                rvm_cid_t Cap_Thd, 
                                rvm_ptr_t Layer,
                                rvm_ptr_t Operation,
                                rvm_ptr_t Value,
                                rvm_ptr_t* Content)
{
    rvm_ptr_t Params[6];
    rvm_ret_t Retval;

    Params[0]=RVM_PARAM_D1(Layer)|RVM_PARAM_D0(Operation);
    Params[1]=Value;
    Retval=RVM_A6M_Kfn_Act(Cap_Kfn, RVM_KFN_DEBUG_INV_MOD, (rvm_ptr_t)Cap_Thd, Params);

    if(Retval<0)
        return Retval;

    if(Content!=0)
        Content[0]=Params[0];

    return Retval;
}
/* End Function:RVM_A6M_Debug_Inv_Mod ****************************************/

/* Begin Function:RVM_A6M_Debug_Exc_Get ***************************************
Description : Debug error register extraction implementation for ARMv7-M.
Input       : rvm_cid_t Cap_Kfn - The kernel function capability.
              rvm_cid_t Cap_Thd - The capability to the thread to consult.
              rvm_ptr_t Operation - The operation, e.g. which register to read.
Output      : rvm_ptr_t* Content - The content of the register.
Return      : rvm_ret_t - If successful, 0; if a negative value, failed.
******************************************************************************/
rvm_ret_t RVM_A6M_Debug_Exc_Get(rvm_cid_t Cap_Kfn,
                                rvm_cid_t Cap_Thd,
                                rvm_ptr_t Operation,
                                rvm_ptr_t* Content)
{
    rvm_ptr_t Params[6];
    rvm_ret_t Retval;

    Params[0]=RVM_PARAM_D0(Operation);
    Retval=RVM_A6M_Kfn_Act(Cap_Kfn, RVM_KFN_DEBUG_EXC_GET, (rvm_ptr_t)Cap_Thd, Params);

    if(Retval<0)
        return Retval;

    if(Content!=0)
        Content[0]=Params[0];

    return Retval;
}
/* End Function:RVM_A6M_Debug_Exc_Get ****************************************/

/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
