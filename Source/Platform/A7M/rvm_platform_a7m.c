/******************************************************************************
Filename    : rvm_platform_a7m.c
Author      : pry
Date        : 26/06/2017
Licence     : The Unlicense; see LICENSE for details.
Description : The ARMv7-M system library platform specific implementation.
******************************************************************************/

/* Include *******************************************************************/
#define __HDR_DEF__
#include "Platform/A7M/rvm_platform_a7m.h"
#include "Syslib/rvm_syslib.h"
#undef __HDR_DEF__

#define __HDR_STRUCT__
#include "Platform/A7M/rvm_platform_a7m.h"
#include "Syslib/rvm_syslib.h"
#undef __HDR_STRUCT__

/* Private include */
#include "Platform/A7M/rvm_platform_a7m.h"

#define __HDR_PUBLIC__
#include "Syslib/rvm_syslib.h"
#undef __HDR_PUBLIC__
/* End Include ***************************************************************/

/* Function:RVM_Stack_Init ****************************************************
Description : Initialize a thread's stack for synchronous invocation or thread 
              creation. The reason why we do not place entry address here directly
              is:
              1. We wish to use the PC address specified in the Exec_Set.
              2. We wish that the stack is uncorrupted when the processor receives
                 an interrupt when we just enter an invocation, and we can design
                 the stub to guarantee that the first instruction will change the SP.
                 In this case,
                 (1) The interrupt occurs when no instruction has executed, now all
                     registers gets pushed to stack, and this overwrite should produce
                     the same content that we initialize it with, so this is fine.
                 (2) At least one instruction is executed, and in this case, the first
                     instruction already changed SP.
Input       : rvm_ptr_t Stack_Base - The start(lower) address of the stub.
              rvm_ptr_t Stack_Size - The size of the stack.
              rvm_ptr_t* Entry - The entry address of the thread, not used here.
              rvm_ptr_t Stub - The jump stub address.
Output      : rvm_ptr_t* Entry - The entry address of the thread, not used here.
Return      : rvm_ptr_t - The actual stack address to use for system call.
******************************************************************************/
rvm_ptr_t RVM_Stack_Init(rvm_ptr_t Stack,
                         rvm_ptr_t Size,
                         rvm_ptr_t* Entry,
                         rvm_ptr_t Stub)

{
    struct RVM_A7M_Stack* Ptr;
    
    /* No macro provided because not every processor would require this */
    Ptr=(struct RVM_A7M_Stack*)(Stack+Size-
                                RVM_STACK_SAFE_RDCY*RVM_WORD_BYTE-
                                sizeof(struct RVM_A7M_Stack));
    Ptr->R12=0U;
    Ptr->LR=0U;
    Ptr->PC=Stub|0x01U;

    /* Initialize the xPSR to avoid a transition to ARM state */
    Ptr->XPSR=0x01000000U;
    
    return (rvm_ptr_t)Ptr;
}
/* End Function:RVM_Stack_Init ***********************************************/

/* Function:RVM_Thd_Cop_Size **************************************************
Description : Query coprocessor register size for this CPU.
Input       : rme_ptr_t Attr - The thread context attributes.
Output      : None.
Return      : rme_ptr_t - The coprocessor context size.
******************************************************************************/
rvm_ptr_t RVM_Thd_Cop_Size(rvm_ptr_t Attr)
{
#if(RVM_COP_NUM!=0U)
    if(Attr!=RVM_A7M_ATTR_NONE)
        return sizeof(struct RVM_A7M_Cop_Struct);
#else
    RVM_ASSERT(Attr==RVM_A7M_ATTR_NONE);
#endif
    
    return 0U;
}
/* End Function:RVM_Thd_Cop_Size *********************************************/

/* Function:RVM_A7M_Kfn_Act ***************************************************
Description : Activate kernel functions that must use ARMv7-M specific calling
              convention to pass extra parameters.
Input       : rvm_cid_t Cap_Kfn - The capability to the kernel capability. 2-Level.
              rvm_ptr_t Func_ID - The function ID to invoke.
              rvm_ptr_t Sub_ID - The subfunction ID to invoke.
              rvm_ptr_t* Param - The parameters, stored in an array of size 6.
Output      : rvm_ptr_t* Param - The return values, stored in an array of size 6.
Return      : rvm_ret_t - If successful, 0; else a negative value.
******************************************************************************/
rvm_ret_t RVM_A7M_Kfn_Act(rvm_cid_t Cap_Kfn,
                          rvm_ptr_t Func_ID,
                          rvm_ptr_t Sub_ID,
                          rvm_ptr_t* Param)
{
    return RVM_A7M_Svc_Kfn((RVM_SVC_KFN<<RVM_WORD_BIT_D1)|((rvm_ptr_t)Cap_Kfn),
                           RVM_PARAM_D1(Sub_ID)|RVM_PARAM_D0(Func_ID),
                           Param);
}
/* End Function:RVM_A7M_Kfn_Act **********************************************/

/* Function:RVM_A7M_Pgt_Entry_Mod *******************************************
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
rvm_ret_t RVM_A7M_Pgt_Entry_Mod(rvm_cid_t Cap_Kfn,
                                rvm_cid_t Cap_Pgt,
                                rvm_ptr_t Vaddr,
                                rvm_ptr_t Type)
{
    return RVM_Kfn_Act(Cap_Kfn,RVM_KFN_PGT_ENTRY_MOD,(rvm_ptr_t)Cap_Pgt,Vaddr,Type);
}
/* End Function:RVM_A7M_Pgt_Entry_Mod **************************************/

/* Function:RVM_A7M_Int_Local_Mod *********************************************
Description : Consult or modify the local interrupt controller's vector state.
Input       : rvm_cid_t Cap_Kfn - The kernel function capability.
              rvm_ptr_t Int_Num - The interrupt number to consult or modify.
              rvm_ptr_t Operation - The operation to conduct.
              rvm_ptr_t Param - The parameter, could be state or priority.
Output      : None.
Return      : rvm_ret_t - If successful, 0 or the desired value; else an error code.
******************************************************************************/
rvm_ret_t RVM_A7M_Int_Local_Mod(rvm_cid_t Cap_Kfn,
                                rvm_ptr_t Int_Num,
                                rvm_ptr_t Operation,
                                rvm_ptr_t Param)
{
    return RVM_Kfn_Act(Cap_Kfn,RVM_KFN_INT_LOCAL_MOD,Int_Num,Operation,Param);
}
/* End Function:RVM_A7M_Int_Local_Mod ****************************************/

/* Function:RVM_A7M_Int_Local_Trig ********************************************
Description : Trigger a CPU's local event source.
Input       : rvm_cid_t Cap_Kfn - The kernel function capability.
              rvm_ptr_t Evt_Num - The event ID.
Output      : None.
Return      : rvm_ret_t - If successful, 0; else an error code.
******************************************************************************/
rvm_ret_t RVM_A7M_Int_Local_Trig(rvm_cid_t Cap_Kfn,
                                 rvm_ptr_t Int_Num)
{
    return RVM_Kfn_Act(Cap_Kfn,RVM_KFN_INT_LOCAL_TRIG,0U,Int_Num,0U);
}
/* End Function:RVM_A7M_Int_Local_Trig ***************************************/

/* Function:RVM_A7M_Cache_Mod *************************************************
Description : Modify cache state. We do not make assumptions about cache contents.
Input       : rvm_cid_t Cap_Kfn - The kernel function capability.
              rvm_ptr_t Cache_ID - The ID of the cache to enable, disable or consult.
              rvm_ptr_t Operation - The operation to perform.
              rvm_ptr_t Param - The parameter.
Output      : None.
Return      : If successful, 0; else RVM_ERR_KERN_OPFAIL.
******************************************************************************/
rvm_ret_t RVM_A7M_Cache_Mod(rvm_cid_t Cap_Kfn,
                            rvm_ptr_t Cache_ID,
                            rvm_ptr_t Operation,
                            rvm_ptr_t Param)
{
    return RVM_Kfn_Act(Cap_Kfn,RVM_KFN_CACHE_MOD,Cache_ID,Operation,Param);
}
/* End Function:RVM_A7M_Cache_Mod ********************************************/

/* Function:RVM_A7M_Cache_Maint ***********************************************
Description : Do cache maintenance. Integrity of the data is always protected.
Input       : rvm_cid_t Cap_Kfn - The kernel function capability.
              rvm_ptr_t Cache_ID - The ID of the cache to do maintenance on.
              rvm_ptr_t Operation - The maintenance operation to perform.
              rvm_ptr_t Param - The parameter for that operation.
Output      : None.
Return      : If successful, 0; else an error code.
******************************************************************************/
rvm_ret_t RVM_A7M_Cache_Maint(rvm_cid_t Cap_Kfn,
                              rvm_ptr_t Cache_ID,
                              rvm_ptr_t Operation,
                              rvm_ptr_t Param)
{
    return RVM_Kfn_Act(Cap_Kfn,RVM_KFN_CACHE_MAINT,Cache_ID,Operation,Param);
}
/* End Function:RVM_A7M_Cache_Maint ******************************************/

/* Function:RVM_A7M_Prfth_Mod *************************************************
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
rvm_ret_t RVM_A7M_Prfth_Mod(rvm_cid_t Cap_Kfn,
                            rvm_ptr_t Prfth_ID,
                            rvm_ptr_t Operation,
                            rvm_ptr_t Param)
{
    return RVM_Kfn_Act(Cap_Kfn,RVM_KFN_PRFTH_MOD,Prfth_ID,Operation,Param);
}
/* End Function:RVM_A7M_Prfth_Mod ********************************************/

/* Function:RVM_A7M_Perf_CPU_Func *********************************************
Description : CPU feature detection for ARMv7-M.
Input       : rvm_cid_t Cap_Kfn - The kernel function capability.
              rvm_ptr_t Freg_ID - The capability to the thread to consult.
Output      : rvm_ptr_t* Content - The content of the register.
Return      : rvm_ret_t - If successful, 0;  else an error code.
******************************************************************************/
rvm_ret_t RVM_A7M_Perf_CPU_Func(rvm_cid_t Cap_Kfn,
                                rvm_ptr_t Freg_ID,
                                rvm_ptr_t* Content)
{
    rvm_ptr_t Param[6];
    rvm_ret_t Retval;

    Retval=RVM_A7M_Kfn_Act(Cap_Kfn,RVM_KFN_PERF_CPU_FUNC,Freg_ID,Param);

    if(Retval<0)
        return Retval;

    if(Content!=0)
        Content[0]=Param[0];

    return Retval;
}
/* End Function:RVM_A7M_Perf_CPU_Func ****************************************/

/* Function:RVM_A7M_Perf_Mon_Mod **********************************************
Description : Read or write performance monitor settings. This only works for
              a single performance counter, CYCCNT, and only works for enabling 
              or disabling operations.
Input       : rvm_ptr_t Perf_ID - The performance monitor identifier.
              rvm_ptr_t Operation - The operation to perform.
              rvm_ptr_t Param - An extra parameter.
Output      : None.
Return      : rvm_ret_t - If successful, the desired value; if a negative value, failed.
******************************************************************************/
rvm_ret_t RVM_A7M_Perf_Mon_Mod(rvm_cid_t Cap_Kfn,
                               rvm_ptr_t Perf_ID,
                               rvm_ptr_t Operation,
                               rvm_ptr_t Param)
{
    return RVM_Kfn_Act(Cap_Kfn,RVM_KFN_PERF_MON_MOD,Perf_ID,Operation,Param);
}
/* End Function:RVM_A7M_Perf_Mon_Mod *****************************************/

/* Function:RVM_A7M_Perf_Cycle_Mod ********************************************
Description : Cycle performance counter read or write for ARMv7-M. Only supports
              CYCCNT register.
Input       : rvm_cid_t Cap_Kfn - The kernel function capability.
              rvm_ptr_t Cycle_ID - The performance counter to read or write.
Output      : rvm_ptr_t* Content - The content of the register in read mode.
Return      : rvm_ret_t - If successful, 0; if a negative value, failed.
******************************************************************************/
rvm_ret_t RVM_A7M_Perf_Cycle_Mod(rvm_cid_t Cap_Kfn,
                                 rvm_ptr_t Cycle_ID, 
                                 rvm_ptr_t Operation,
                                 rvm_ptr_t Value,
                                 rvm_ptr_t* Content)
{
    rvm_ptr_t Param[6];
    rvm_ret_t Retval;

    Param[0]=Operation;
    Param[1]=Value;
    Retval=RVM_A7M_Kfn_Act(Cap_Kfn,RVM_KFN_PERF_CYCLE_MOD,Cycle_ID,Param);

    if(Retval<0)
        return Retval;

    if(Content!=RVM_NULL)
        Content[0]=Param[0];

    return Retval;
}
/* End Function:RVM_A7M_Perf_Cycle_Mod ***************************************/

/* Function:RVM_A7M_Debug_Print ***********************************************
Description : Debug printing function (through the kernel) for ARMv7-M. This is 
              a blocking system call through the serial port.
Input       : rvm_cid_t Cap_Kfn - The kernel function capability.
              rvm_cid_t Cap_Thd - The capability to the thread to consult.
              rvm_ptr_t Operation - The operation, e.g. which register to read or write.
              rvm_ptr_t Value - The value to write into the register.
Output      : rvm_ptr_t* Content - The content of the register in read mode.
Return      : rvm_ret_t - If successful, 0; if a negative value, failed.
******************************************************************************/
rvm_ret_t RVM_A7M_Debug_Print(rvm_cid_t Cap_Kfn,
                              char Char)
{
    return RVM_Kfn_Act(Cap_Kfn,RVM_KFN_DEBUG_PRINT,(rvm_ptr_t)Char,0U,0U);
}
/* End Function:RVM_A7M_Debug_Print ******************************************/

/* Function:RVM_A7M_Debug_Reg_Mod *********************************************
Description : Debug regular register modification implementation for ARMv7-M.
Input       : rvm_cid_t Cap_Kfn - The kernel function capability.
              rvm_cid_t Cap_Thd - The capability to the thread to consult.
              rvm_ptr_t Operation - The operation, e.g. which register to read or write.
              rvm_ptr_t Value - The value to write into the register.
Output      : rvm_ptr_t* Content - The content of the register in read mode.
Return      : rvm_ret_t - If successful, 0; if a negative value, failed.
******************************************************************************/
rvm_ret_t RVM_A7M_Debug_Reg_Mod(rvm_cid_t Cap_Kfn,
                                rvm_cid_t Cap_Thd, 
                                rvm_ptr_t Operation,
                                rvm_ptr_t Value,
                                rvm_ptr_t* Content)
{
    rvm_ptr_t Param[6];
    rvm_ret_t Retval;

    Param[0]=Operation;
    Param[1]=Value;
    Retval=RVM_A7M_Kfn_Act(Cap_Kfn,RVM_KFN_DEBUG_REG_MOD,(rvm_ptr_t)Cap_Thd,Param);

    if(Retval<0)
        return Retval;

    if(Content!=0)
        Content[0]=Param[0];

    return Retval;
}
/* End Function:RVM_A7M_Debug_Reg_Mod ****************************************/

/* Function:RVM_A7M_Debug_Inv_Mod *********************************************
Description : Debug invocation register modification implementation for ARMv7-M.
Input       : rvm_cid_t Cap_Kfn - The kernel function capability.
              rvm_cid_t Cap_Thd - The capability to the thread to consult.
              rvm_ptr_t Layer - The layer to consult, 0 is the invocation stack top.
              rvm_ptr_t Operation - The operation, e.g. which register to read or write.
              rvm_ptr_t Value - The value to write into the register.
Output      : rvm_ptr_t* Content - The content of the register in read mode.
Return      : rvm_ret_t - If successful, 0; if a negative value, failed.
******************************************************************************/
rvm_ret_t RVM_A7M_Debug_Inv_Mod(rvm_cid_t Cap_Kfn,
                                rvm_cid_t Cap_Thd, 
                                rvm_ptr_t Layer,
                                rvm_ptr_t Operation,
                                rvm_ptr_t Value,
                                rvm_ptr_t* Content)
{
    rvm_ptr_t Param[6];
    rvm_ret_t Retval;

    Param[0]=RVM_PARAM_D1(Layer)|RVM_PARAM_D0(Operation);
    Param[1]=Value;
    Retval=RVM_A7M_Kfn_Act(Cap_Kfn,RVM_KFN_DEBUG_INV_MOD,(rvm_ptr_t)Cap_Thd,Param);

    if(Retval<0)
        return Retval;

    if(Content!=0)
        Content[0]=Param[0];

    return Retval;
}
/* End Function:RVM_A7M_Debug_Inv_Mod ****************************************/

/* Function:RVM_A7M_Debug_Exc_Get *********************************************
Description : Debug error register extraction implementation for ARMv7-M.
Input       : rvm_cid_t Cap_Kfn - The kernel function capability.
              rvm_cid_t Cap_Thd - The capability to the thread to consult.
              rvm_ptr_t Operation - The operation, e.g. which register to read.
Output      : rvm_ptr_t* Content - The content of the register.
Return      : rvm_ret_t - If successful, 0; if a negative value, failed.
******************************************************************************/
rvm_ret_t RVM_A7M_Debug_Exc_Get(rvm_cid_t Cap_Kfn,
                                rvm_cid_t Cap_Thd,
                                rvm_ptr_t Operation,
                                rvm_ptr_t* Content)
{
    rvm_ptr_t Param[6];
    rvm_ret_t Retval;

    Param[0]=RVM_PARAM_D0(Operation);
    Retval=RVM_A7M_Kfn_Act(Cap_Kfn,RVM_KFN_DEBUG_EXC_GET,(rvm_ptr_t)Cap_Thd,Param);

    if(Retval<0)
        return Retval;

    if(Content!=0)
        Content[0]=Param[0];

    return Retval;
}
/* End Function:RVM_A7M_Debug_Exc_Get ****************************************/

/* Function:RVM_Thd_Print_Exc *************************************************
Description : Print the exact reason of the fault.
Input       : rvm_cid_t Cap_Thd - The capability to the thread.
Output      : None.
Return      : None.
******************************************************************************/
#ifndef RVM_VIRT_LIB_ENABLE
void RVM_Thd_Print_Exc(rvm_tid_t Cap_Thd)
{
#if(RVM_DBGLOG_ENABLE!=0U)
    rvm_ptr_t Exc;
    
    /* Get exception cause */
    RVM_A7M_Debug_Exc_Get(RVM_BOOT_INIT_KFN,Cap_Thd,
                          RVM_A7M_KFN_DEBUG_EXC_CAUSE_GET,&Exc);
    
    /* Parse exception reason */
    if((Exc&RVM_A7M_UFSR_DIVBYZERO)!=0U)
        RVM_DBG_S("      Divide by zero.\r\n");
    if((Exc&RVM_A7M_UFSR_UNALIGNED)!=0U)
        RVM_DBG_S("      Unaligned load/store access.\r\n");
    if((Exc&RVM_A7M_UFSR_NOCP)!=0U)
        RVM_DBG_S("      No such coprocessor.\r\n");
    if((Exc&RVM_A7M_UFSR_INVPC)!=0U)
        RVM_DBG_S("      Invalid vector return LR or PC value.\r\n");
    if((Exc&RVM_A7M_UFSR_INVSTATE)!=0U)
        RVM_DBG_S("      Attempt to enter an invalid instruction set (ARM) state.\r\n");
    if((Exc&RVM_A7M_UFSR_UNDEFINSTR)!=0U)
        RVM_DBG_S("      Invalid IT or related instruction.\r\n");
    if((Exc&RVM_A7M_BFSR_LSPERR)!=0U)
        RVM_DBG_S("      Bus fault during FP lazy stacking.\r\n");
    if((Exc&RVM_A7M_BFSR_STKERR)!=0U)
        RVM_DBG_S("      Derived bus fault on exception entry.\r\n");
    if((Exc&RVM_A7M_BFSR_UNSTKERR)!=0U)
        RVM_DBG_S("      Derived bus fault on exception return.\r\n");
    if((Exc&RVM_A7M_BFSR_IMPRECISERR)!=0U)
        RVM_DBG_S("      Imprecise data access error.\r\n");
    if((Exc&RVM_A7M_BFSR_PRECISERR)!=0U)
        RVM_DBG_S("      Precise data access error.\r\n");
    if((Exc&RVM_A7M_BFSR_IBUSERR)!=0U)
        RVM_DBG_S("      Bus fault on instruction prefetch.\r\n");
    if((Exc&RVM_A7M_MFSR_MLSPERR)!=0U)
        RVM_DBG_S("      Memory management fault during FP lazy state preservation.\r\n");
    if((Exc&RVM_A7M_MFSR_MSTKERR)!=0U)
        RVM_DBG_S("      Derived memory management fault on exception entry.\r\n");
    if((Exc&RVM_A7M_MFSR_MUNSTKERR)!=0U)
        RVM_DBG_S("      Derived memory management fault on exception return.\r\n");
    if((Exc&RVM_A7M_MFSR_DACCVIOL)!=0U)
        RVM_DBG_S("      Data access violation.\r\n");
    if((Exc&RVM_A7M_MFSR_IACCVIOL)!=0U)
        RVM_DBG_S("      Instruction access violation.\r\n");
#endif
}
/* End Function:RVM_Thd_Print_Exc ********************************************/

/* Function:RVM_Thd_Print_Reg *************************************************
Description : Print the register set of a thread.
Input       : rvm_cid_t Cap_Thd - The capability to the thread.
Output      : None.
Return      : rvm_ret_t - If successful, 0; else a negative value.
******************************************************************************/
rvm_ret_t RVM_Thd_Print_Reg(rvm_cid_t Cap_Thd)
{
#if(RVM_DBGLOG_ENABLE!=0U)
    rvm_ptr_t Content;
    struct RVM_A7M_Stack* Stack;
    
    /* Print all registers */
    RVM_A7M_Debug_Reg_Mod(RVM_BOOT_INIT_KFN,Cap_Thd, 
                          RVM_A7M_KFN_DEBUG_REG_MOD_R4_GET,0U,&Content);
    RVM_DBG_SHS("      R4: 0x",Content,"\r\n");
    
    RVM_A7M_Debug_Reg_Mod(RVM_BOOT_INIT_KFN,Cap_Thd, 
                          RVM_A7M_KFN_DEBUG_REG_MOD_R5_GET,0U,&Content);
    RVM_DBG_SHS("      R5: 0x",Content,"\r\n");
    
    RVM_A7M_Debug_Reg_Mod(RVM_BOOT_INIT_KFN,Cap_Thd, 
                          RVM_A7M_KFN_DEBUG_REG_MOD_R6_GET,0U,&Content);
    RVM_DBG_SHS("      R6: 0x",Content,"\r\n");
    
    RVM_A7M_Debug_Reg_Mod(RVM_BOOT_INIT_KFN,Cap_Thd, 
                          RVM_A7M_KFN_DEBUG_REG_MOD_R7_GET,0U,&Content);
    RVM_DBG_SHS("      R7: 0x",Content,"\r\n");
    
    RVM_A7M_Debug_Reg_Mod(RVM_BOOT_INIT_KFN,Cap_Thd, 
                          RVM_A7M_KFN_DEBUG_REG_MOD_R8_GET,0U,&Content);
    RVM_DBG_SHS("      R8: 0x",Content,"\r\n");
    
    RVM_A7M_Debug_Reg_Mod(RVM_BOOT_INIT_KFN,Cap_Thd, 
                          RVM_A7M_KFN_DEBUG_REG_MOD_R9_GET,0U,&Content);
    RVM_DBG_SHS("      R9: 0x",Content,"\r\n");
    
    RVM_A7M_Debug_Reg_Mod(RVM_BOOT_INIT_KFN,Cap_Thd, 
                          RVM_A7M_KFN_DEBUG_REG_MOD_R10_GET,0U,&Content);
    RVM_DBG_SHS("      R10: 0x",Content,"\r\n");
    
    RVM_A7M_Debug_Reg_Mod(RVM_BOOT_INIT_KFN,Cap_Thd, 
                          RVM_A7M_KFN_DEBUG_REG_MOD_R11_GET,0U,&Content);
    RVM_DBG_SHS("      R11: 0x",Content,"\r\n");
    
    RVM_A7M_Debug_Reg_Mod(RVM_BOOT_INIT_KFN,Cap_Thd, 
                          RVM_A7M_KFN_DEBUG_REG_MOD_SP_GET,0U,&Content);
    RVM_DBG_SHS("      SP: 0x",Content,"\r\n");
    Stack=(struct RVM_A7M_Stack*)Content;
    
    RVM_A7M_Debug_Reg_Mod(RVM_BOOT_INIT_KFN,Cap_Thd, 
                          RVM_A7M_KFN_DEBUG_REG_MOD_LR_GET,0U,&Content);
    RVM_DBG_SHS("      LR: 0x",Content,"\r\n");

    RVM_A7M_Debug_Exc_Get(RVM_BOOT_INIT_KFN,Cap_Thd,
                          RVM_A7M_KFN_DEBUG_EXC_CAUSE_GET,&Content);
    RVM_DBG_SHS("      Cause: 0x",Content,"\r\n");
    
    RVM_A7M_Debug_Exc_Get(RVM_BOOT_INIT_KFN,Cap_Thd,
                          RVM_A7M_KFN_DEBUG_EXC_ADDR_GET,&Content);
    RVM_DBG_SHS("      Addr: 0x",Content,"\r\n");

    /* We know where SP is, but we are not sure whether accessing SP in RVM
     * will cause issues. We assume that the user program will not fiddle
     * with SP maliciously (especially during the debugging phase) and we 
     * always print the stack trace, though this is quite dangerous. When 
     * developing products, this register printing function will be disabled
     * anyway and thus does not cause security breaches. */
    RVM_DBG_SHS("      Stack-R0: 0x",Stack->R0,"\r\n");
    RVM_DBG_SHS("      Stack-R1: 0x",Stack->R1,"\r\n");
    RVM_DBG_SHS("      Stack-R2: 0x",Stack->R2,"\r\n");
    RVM_DBG_SHS("      Stack-R3: 0x",Stack->R3,"\r\n");
    RVM_DBG_SHS("      Stack-R12: 0x",Stack->R12,"\r\n");
    RVM_DBG_SHS("      Stack-LR: 0x",Stack->LR,"\r\n");
    RVM_DBG_SHS("      Stack-PC: 0x",Stack->PC,"\r\n");
    RVM_DBG_SHS("      Stack-XPSR: 0x",Stack->XPSR,"\r\n");
    
#endif
    return 0;
}
#endif
/* End Function:RVM_Thd_Print_Reg ********************************************/

/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
