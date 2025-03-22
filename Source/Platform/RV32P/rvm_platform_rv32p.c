/******************************************************************************
Filename    : rvm_platform_rv32p.c
Author      : pry
Date        : 26/06/2017
Licence     : The Unlicense; see LICENSE for details.
Description : The RV32P system library platform specific implementation.
******************************************************************************/

/* Include *******************************************************************/
#include "rvm.h"

#define __HDR_DEF__
#include "Platform/RV32P/rvm_platform_rv32p.h"
#include "Monitor/rvm_monitor.h"
#undef __HDR_DEF__

#define __HDR_STRUCT__
#include "Platform/RV32P/rvm_platform_rv32p.h"
#include "Monitor/rvm_monitor.h"
#undef __HDR_STRUCT__

/* Private include */
#include "Platform/RV32P/rvm_platform_rv32p.h"

#define __HDR_PUBLIC__
#include "Monitor/rvm_monitor.h"
#undef __HDR_PUBLIC__
/* End Include ***************************************************************/

/* Function:RVM_Stack_Init ****************************************************
Description : Initialize a thread's stack for synchronous invocation or thread 
              creation.
Input       : rvm_ptr_t Stack - The start(lower) address of the stub.
              rvm_ptr_t Stack - The size of the stack.
              rvm_ptr_t* Entry - The entry address of the thread.
              rvm_ptr_t Stub - The jump stub address.
Output      : rvm_ptr_t* Entry - The entry address of the thread.
Return      : rvm_ptr_t - The actual stack address to use for system call.
******************************************************************************/
rvm_ptr_t RVM_Stack_Init(rvm_ptr_t Stack,
                         rvm_ptr_t Size,
                         rvm_ptr_t* Entry,
                         rvm_ptr_t Stub)

{

    struct RVM_RV32P_Stack* Ptr;
    
    Ptr=(struct RVM_RV32P_Stack*)(Stack+Size-
                                  RVM_STACK_SAFE_RDCY*RVM_WORD_BYTE-
                                  sizeof(struct RVM_RV32P_Stack));

    /* Jump to the stub so we can load the gp at start, for linker relaxation */
    Ptr->PC=*Entry;
    *Entry=Stub;
    
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
    if(Attr!=RVM_RV32P_ATTR_NONE)
        return sizeof(struct RVM_RV32P_Cop_Struct);
#else
    RVM_ASSERT(Attr==RVM_RV32P_ATTR_NONE);
#endif

    return 0U;
}
/* End Function:RVM_Thd_Cop_Size *********************************************/

/* Function:RVM_RV32P_Kfn_Act *************************************************
Description : Activate kernel functions that must use ARMv7-M specific calling
              convention to pass extra parameters.
Input       : rvm_cid_t Cap_Kfn - The capability to the kernel capability. 2-Level.
              rvm_ptr_t Func_ID - The function ID to invoke.
              rvm_ptr_t Sub_ID - The subfunction ID to invoke.
              rvm_ptr_t* Param - The parameters, stored in an array of size 6.
Output      : rvm_ptr_t* Param - The return values, stored in an array of size 6.
Return      : rvm_ret_t - If successful, 0; else a negative value.
******************************************************************************/
rvm_ret_t RVM_RV32P_Kfn_Act(rvm_cid_t Cap_Kfn,
                            rvm_ptr_t Func_ID,
                            rvm_ptr_t Sub_ID,
                            rvm_ptr_t* Param)
{
    return RVM_RV32P_Svc_Kfn((RVM_SVC_KFN<<RVM_WORD_BIT_D1)|((rvm_ptr_t)Cap_Kfn),
                             RVM_PARAM_D1(Sub_ID)|RVM_PARAM_D0(Func_ID),
                             Param);
}
/* End Function:RVM_RV32P_Kfn_Act ********************************************/

/* Function:RVM_RV32P_Pgt_Entry_Mod *******************************************
Description : Consult or modify the page table attributes. RISC-V only allows 
              consulting page table attributes but does not allow modifying them,
              because there are no architecture-specific flags.
Input       : rvm_cid_t Cap_Kfn - The kernel function capability.
              rvm_cid_t Cap_Pgt - The capability to the top-level page table to consult.
              rvm_ptr_t Vaddr - The virtual address to consult.
              rvm_ptr_t Type - The consult type, see manual for details.
Output      : None.
Return      : rvm_ret_t - If successful, the flags; else an error code.
******************************************************************************/
rvm_ret_t RVM_RV32P_Pgt_Entry_Mod(rvm_cid_t Cap_Kfn,
                                  rvm_cid_t Cap_Pgt,
                                  rvm_ptr_t Vaddr,
                                  rvm_ptr_t Type)
{
    return RVM_Kfn_Act(Cap_Kfn,RVM_KFN_PGT_ENTRY_MOD,(rvm_ptr_t)Cap_Pgt,Vaddr,Type);
}
/* End Function:RVM_RV32P_Pgt_Entry_Mod **************************************/

/* Function:RVM_RV32P_Int_Local_Mod *******************************************
Description : Consult or modify the local interrupt controller's vector state.
Input       : rvm_cid_t Cap_Kfn - The kernel function capability.
              rvm_ptr_t Int_Num - The interrupt number to consult or modify.
              rvm_ptr_t Operation - The operation to conduct.
              rvm_ptr_t Param - The parameter, could be state or priority.
Output      : None.
Return      : rvm_ret_t - If successful, 0 or the desired value; else an error code.
******************************************************************************/
rvm_ret_t RVM_RV32P_Int_Local_Mod(rvm_cid_t Cap_Kfn,
                                  rvm_ptr_t Int_Num,
                                  rvm_ptr_t Operation,
                                  rvm_ptr_t Param)
{
    return RVM_Kfn_Act(Cap_Kfn,RVM_KFN_INT_LOCAL_MOD,Int_Num,Operation,Param);
}
/* End Function:RVM_RV32P_Int_Local_Mod **************************************/

/* Function:RVM_RV32P_Int_Local_Trig ******************************************
Description : Trigger a CPU's local event source.
Input       : rvm_cid_t Cap_Kfn - The kernel function capability.
              rvm_ptr_t Evt_Num - The event ID.
Output      : None.
Return      : rvm_ret_t - If successful, 0; else an error code.
******************************************************************************/
rvm_ret_t RVM_RV32P_Int_Local_Trig(rvm_cid_t Cap_Kfn,
                                   rvm_ptr_t Int_Num)
{
    return RVM_Kfn_Act(Cap_Kfn,RVM_KFN_INT_LOCAL_TRIG,0U,Int_Num,0U);
}
/* End Function:RVM_RV32P_Int_Local_Trig *************************************/

/* Function:RVM_RV32P_Cache_Mod ***********************************************
Description : Modify cache state. We do not make assumptions about cache contents.
Input       : rvm_cid_t Cap_Kfn - The kernel function capability.
              rvm_ptr_t Cache_ID - The ID of the cache to enable, disable or consult.
              rvm_ptr_t Operation - The operation to perform.
              rvm_ptr_t Param - The parameter.
Output      : None.
Return      : If successful, 0; else RVM_ERR_KERN_OPFAIL.
******************************************************************************/
rvm_ret_t RVM_RV32P_Cache_Mod(rvm_cid_t Cap_Kfn,
                              rvm_ptr_t Cache_ID,
                              rvm_ptr_t Operation,
                              rvm_ptr_t Param)
{
    return RVM_Kfn_Act(Cap_Kfn,RVM_KFN_CACHE_MOD,Cache_ID,Operation,Param);
}
/* End Function:RVM_RV32P_Cache_Mod ******************************************/

/* Function:RVM_RV32P_Cache_Maint *********************************************
Description : Do cache maintenance. Integrity of the data is always protected.
Input       : rvm_cid_t Cap_Kfn - The kernel function capability.
              rvm_ptr_t Cache_ID - The ID of the cache to do maintenance on.
              rvm_ptr_t Operation - The maintenance operation to perform.
              rvm_ptr_t Param - The parameter for that operation.
Output      : None.
Return      : If successful, 0; else an error code.
******************************************************************************/
rvm_ret_t RVM_RV32P_Cache_Maint(rvm_cid_t Cap_Kfn,
                                rvm_ptr_t Cache_ID,
                                rvm_ptr_t Operation,
                                rvm_ptr_t Param)
{
    return RVM_Kfn_Act(Cap_Kfn,RVM_KFN_CACHE_MAINT,Cache_ID,Operation,Param);
}
/* End Function:RVM_RV32P_Cache_Maint ****************************************/

/* Function:RVM_RV32P_Prfth_Mod ***********************************************
Description : Modify prefetcher state.
Input       : rvm_cid_t Cap_Kfn - The kernel function capability.
              rvm_ptr_t Prfth_ID - The ID of the prefetcher to enable, disable or consult.
              rvm_ptr_t Operation - The operation to perform.
              rvm_ptr_t Param - The parameter.
Output      : None.
Return      : If successful, 0; else RVM_ERR_KERN_OPFAIL.
******************************************************************************/
rvm_ret_t RVM_RV32P_Prfth_Mod(rvm_cid_t Cap_Kfn,
                              rvm_ptr_t Prfth_ID,
                              rvm_ptr_t Operation,
                              rvm_ptr_t Param)
{
    return RVM_Kfn_Act(Cap_Kfn,RVM_KFN_PRFTH_MOD,Prfth_ID,Operation,Param);
}
/* End Function:RVM_RV32P_Prfth_Mod ******************************************/

/* Function:RVM_RV32P_Perf_CPU_Func *******************************************
Description : CPU feature detection for RISC-V.
Input       : rvm_cid_t Cap_Kfn - The kernel function capability.
              rvm_ptr_t Freg_ID - The capability to the thread to consult.
Output      : rvm_ptr_t* Content - The content of the register.
Return      : rvm_ret_t - If successful, 0;  else an error code.
******************************************************************************/
rvm_ret_t RVM_RV32P_Perf_CPU_Func(rvm_cid_t Cap_Kfn,
                                  rvm_ptr_t Freg_ID,
                                  rvm_ptr_t* Content)
{
    rvm_ptr_t Param[6];
    rvm_ret_t Retval;

    Retval=RVM_RV32P_Kfn_Act(Cap_Kfn,RVM_KFN_PERF_CPU_FUNC,Freg_ID,Param);

    if(Retval<0)
        return Retval;

    if(Content!=0)
        Content[0]=Param[0];

    return Retval;
}
/* End Function:RVM_RV32P_Perf_CPU_Func **************************************/

/* Function:RVM_RV32P_Perf_Mon_Mod ********************************************
Description : Read or write performance monitor settings.
Input       : rvm_ptr_t Perf_ID - The performance monitor identifier.
              rvm_ptr_t Operation - The operation to perform.
              rvm_ptr_t Param - An extra parameter.
Output      : None.
Return      : rvm_ret_t - If successful, the desired value; if a negative value, failed.
******************************************************************************/
rvm_ret_t RVM_RV32P_Perf_Mon_Mod(rvm_cid_t Cap_Kfn,
                                 rvm_ptr_t Perf_ID,
                                 rvm_ptr_t Operation,
                                 rvm_ptr_t Param)
{
    return RVM_Kfn_Act(Cap_Kfn,RVM_KFN_PERF_MON_MOD,Perf_ID,Operation,Param);
}
/* End Function:RVM_RV32P_Perf_Mon_Mod ***************************************/

/* Function:RVM_RV32P_Perf_Cycle_Mod ******************************************
Description : Cycle performance counter read or write for RISC-V.
Input       : rvm_cid_t Cap_Kfn - The kernel function capability.
              rvm_ptr_t Cycle_ID - The performance counter to read or write.
Output      : rvm_ptr_t* Content - The content of the register in read mode.
Return      : rvm_ret_t - If successful, 0; if a negative value, failed.
******************************************************************************/
rvm_ret_t RVM_RV32P_Perf_Cycle_Mod(rvm_cid_t Cap_Kfn,
                                   rvm_ptr_t Cycle_ID, 
                                   rvm_ptr_t Operation,
                                   rvm_ptr_t Value,
                                   rvm_ptr_t* Content)
{
    rvm_ptr_t Param[6];
    rvm_ret_t Retval;

    Param[0]=Operation;
    Param[1]=Value;
    Retval=RVM_RV32P_Kfn_Act(Cap_Kfn,RVM_KFN_PERF_CYCLE_MOD,Cycle_ID,Param);

    if(Retval<0)
        return Retval;

    if(Content!=RVM_NULL)
        Content[0]=Param[0];

    return Retval;
}
/* End Function:RVM_RV32P_Perf_Cycle_Mod *************************************/

/* Function:RVM_RV32P_Debug_Print *********************************************
Description : Debug printing function (through the kernel) for RISC-V. This is 
              a blocking system call through the serial port.
Input       : rvm_cid_t Cap_Kfn - The kernel function capability.
              rvm_cid_t Cap_Thd - The capability to the thread to consult.
              rvm_ptr_t Operation - The operation, e.g. which register to read or write.
              rvm_ptr_t Value - The value to write into the register.
Output      : rvm_ptr_t* Content - The content of the register in read mode.
Return      : rvm_ret_t - If successful, 0; if a negative value, failed.
******************************************************************************/
rvm_ret_t RVM_RV32P_Debug_Print(rvm_cid_t Cap_Kfn,
                                char Char)
{
    return RVM_Kfn_Act(Cap_Kfn,RVM_KFN_DEBUG_PRINT,(rvm_ptr_t)Char,0U,0U);
}
/* End Function:RVM_RV32P_Debug_Print ****************************************/

/* Function:RVM_RV32P_Debug_Reg_Mod *******************************************
Description : Debug regular register modification implementation for RISC-V.
Input       : rvm_cid_t Cap_Kfn - The kernel function capability.
              rvm_cid_t Cap_Thd - The capability to the thread to consult.
              rvm_ptr_t Operation - The operation, e.g. which register to read or write.
              rvm_ptr_t Value - The value to write into the register.
Output      : rvm_ptr_t* Content - The content of the register in read mode.
Return      : rvm_ret_t - If successful, 0; if a negative value, failed.
******************************************************************************/
rvm_ret_t RVM_RV32P_Debug_Reg_Mod(rvm_cid_t Cap_Kfn,
                                  rvm_cid_t Cap_Thd, 
                                  rvm_ptr_t Operation,
                                  rvm_ptr_t Value,
                                  rvm_ptr_t* Content)
{
    rvm_ptr_t Param[6];
    rvm_ret_t Retval;

    Param[0]=Operation;
    Param[1]=Value;
    Retval=RVM_RV32P_Kfn_Act(Cap_Kfn,RVM_KFN_DEBUG_REG_MOD,(rvm_ptr_t)Cap_Thd,Param);

    if(Retval<0)
        return Retval;

    if(Content!=0)
        Content[0]=Param[0];

    return Retval;
}
/* End Function:RVM_RV32P_Debug_Reg_Mod **************************************/

/* Function:RVM_RV32P_Debug_Inv_Mod *******************************************
Description : Debug invocation register modification implementation for RISC-V.
Input       : rvm_cid_t Cap_Kfn - The kernel function capability.
              rvm_cid_t Cap_Thd - The capability to the thread to consult.
              rvm_ptr_t Layer - The layer to consult, 0 is the invocation stack top.
              rvm_ptr_t Operation - The operation, e.g. which register to read or write.
              rvm_ptr_t Value - The value to write into the register.
Output      : rvm_ptr_t* Content - The content of the register in read mode.
Return      : rvm_ret_t - If successful, 0; if a negative value, failed.
******************************************************************************/
rvm_ret_t RVM_RV32P_Debug_Inv_Mod(rvm_cid_t Cap_Kfn,
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
    Retval=RVM_RV32P_Kfn_Act(Cap_Kfn,RVM_KFN_DEBUG_INV_MOD,(rvm_ptr_t)Cap_Thd,Param);

    if(Retval<0)
        return Retval;

    if(Content!=0)
        Content[0]=Param[0];

    return Retval;
}
/* End Function:RVM_RV32P_Debug_Inv_Mod **************************************/

/* Function:RVM_RV32P_Debug_Exc_Get *******************************************
Description : Debug error register extraction implementation for RISC-V.
Input       : rvm_cid_t Cap_Kfn - The kernel function capability.
              rvm_cid_t Cap_Thd - The capability to the thread to consult.
              rvm_ptr_t Operation - The operation, e.g. which register to read.
Output      : rvm_ptr_t* Content - The content of the register.
Return      : rvm_ret_t - If successful, 0; if a negative value, failed.
******************************************************************************/
rvm_ret_t RVM_RV32P_Debug_Exc_Get(rvm_cid_t Cap_Kfn,
                                  rvm_cid_t Cap_Thd,
                                  rvm_ptr_t Operation,
                                  rvm_ptr_t* Content)
{
    rvm_ptr_t Param[6];
    rvm_ret_t Retval;

    Param[0]=RVM_PARAM_D0(Operation);
    Retval=RVM_RV32P_Kfn_Act(Cap_Kfn,RVM_KFN_DEBUG_EXC_GET,(rvm_ptr_t)Cap_Thd,Param);

    if(Retval<0)
        return Retval;

    if(Content!=0)
        Content[0]=Param[0];

    return Retval;
}
/* End Function:RVM_RV32P_Debug_Exc_Get **************************************/

/* Function:RVM_Thd_Print_Exc *************************************************
Description : Print the exact reason of the fault.
Input       : rvm_cid_t Cap_Thd - The capability to the thread.
Output      : None.
Return      : None.
******************************************************************************/
#ifndef RVM_VIRT_LIB_ENABLE
void RVM_Thd_Print_Exc(rvm_cid_t Cap_Thd)
{
#if(RVM_DBGLOG_ENABLE!=0U)
    rvm_ptr_t Cause;
    rvm_ptr_t Addr;
    rvm_ptr_t Value;
    
    /* Get exception cause */
    RVM_RV32P_Debug_Exc_Get(RVM_BOOT_INIT_KFN,Cap_Thd, 
                            RVM_RV32P_KFN_DEBUG_EXC_CAUSE_GET,&Cause);
    
    /* Get exception address */
    RVM_RV32P_Debug_Exc_Get(RVM_BOOT_INIT_KFN,Cap_Thd, 
                            RVM_RV32P_KFN_DEBUG_EXC_ADDR_GET,&Addr);
    
    /* Get exception value */
    RVM_RV32P_Debug_Exc_Get(RVM_BOOT_INIT_KFN,Cap_Thd, 
                            RVM_RV32P_KFN_DEBUG_EXC_VALUE_GET,&Value);
    
    /* Parse exception reason */
    switch(Cause)
    {
        case RVM_RV32P_MCAUSE_IMALIGN:  RVM_DBG_S("      Instruction misaligned"); break;
        case RVM_RV32P_MCAUSE_IACCFLT:  RVM_DBG_S("      Instruction access fault"); break;
        case RVM_RV32P_MCAUSE_IUNDEF:   RVM_DBG_S("      Undefined instruction"); break;
        case RVM_RV32P_MCAUSE_IBRKPT:   RVM_DBG_S("      Breakpoint"); break;
        case RVM_RV32P_MCAUSE_LALIGN:   RVM_DBG_S("      Data load misaligned"); break;
        case RVM_RV32P_MCAUSE_LACCFLT:  RVM_DBG_S("      Data load access fault"); break;
        case RVM_RV32P_MCAUSE_SALIGN:   RVM_DBG_S("      Data store misaligned"); break;
        case RVM_RV32P_MCAUSE_SACCFLT:  RVM_DBG_S("      Data store access fault"); break;
        /* In most cases these below shall never happen, because we're assuming MCUs */
        case RVM_RV32P_MCAUSE_U_ECALL:  RVM_DBG_S("      System call from user mode"); break;
        case RVM_RV32P_MCAUSE_S_ECALL:  RVM_DBG_S("      System call from supervisor mode"); break;
        case RVM_RV32P_MCAUSE_H_ECALL:  RVM_DBG_S("      System call from hypervisor mode"); break;
        case RVM_RV32P_MCAUSE_M_ECALL:  RVM_DBG_S("      System call from machine mode"); break;
        case RVM_RV32P_MCAUSE_IPGFLT:   RVM_DBG_S("      Instruction page fault"); break;
        case RVM_RV32P_MCAUSE_LPGFLT:   RVM_DBG_S("      Load page fault"); break;
        case RVM_RV32P_MCAUSE_SPGFLT:   RVM_DBG_S("      Store page fault"); break;
        default:                        RVM_DBG_S("      Unknown cause"); break;
    }
    
    /* Print value and address */
    RVM_DBG_S(" Addr 0x");
    RVM_DBG_H(Addr);
    RVM_DBG_S(" Value 0x");
    RVM_DBG_H(Value);
    RVM_DBG_S(".\r\n");
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

    /* Print all registers */
    RVM_RV32P_Debug_Reg_Mod(RVM_BOOT_INIT_KFN,Cap_Thd,
                            RVM_RV32P_KFN_DEBUG_REG_MOD_MSTATUS_GET,0U,&Content);
    RVM_DBG_SHS("      MSTATUS: 0x",Content,"\r\n");
    
    RVM_RV32P_Debug_Reg_Mod(RVM_BOOT_INIT_KFN,Cap_Thd,
                            RVM_RV32P_KFN_DEBUG_REG_MOD_PC_GET,0U,&Content);
    RVM_DBG_SHS("      PC: 0x",Content,"\r\n");
    
    RVM_RV32P_Debug_Reg_Mod(RVM_BOOT_INIT_KFN,Cap_Thd,
                            RVM_RV32P_KFN_DEBUG_REG_MOD_X1_RA_GET,0U,&Content);
    RVM_DBG_SHS("      X1_RA: 0x",Content,"\r\n");
    
    RVM_RV32P_Debug_Reg_Mod(RVM_BOOT_INIT_KFN,Cap_Thd,
                            RVM_RV32P_KFN_DEBUG_REG_MOD_X2_SP_GET,0U,&Content);
    RVM_DBG_SHS("      X2_SP: 0x",Content,"\r\n");
    
    RVM_RV32P_Debug_Reg_Mod(RVM_BOOT_INIT_KFN,Cap_Thd,
                            RVM_RV32P_KFN_DEBUG_REG_MOD_X3_GP_GET,0U,&Content);
    RVM_DBG_SHS("      X3_GP: 0x",Content,"\r\n");
    
    RVM_RV32P_Debug_Reg_Mod(RVM_BOOT_INIT_KFN,Cap_Thd,
                            RVM_RV32P_KFN_DEBUG_REG_MOD_X4_TP_GET,0U,&Content);
    RVM_DBG_SHS("      X4_TP: 0x",Content,"\r\n");
    
    RVM_RV32P_Debug_Reg_Mod(RVM_BOOT_INIT_KFN,Cap_Thd,
                            RVM_RV32P_KFN_DEBUG_REG_MOD_X5_T0_GET,0U,&Content);
    RVM_DBG_SHS("      X5_T0: 0x",Content,"\r\n");
    
    RVM_RV32P_Debug_Reg_Mod(RVM_BOOT_INIT_KFN,Cap_Thd,
                            RVM_RV32P_KFN_DEBUG_REG_MOD_X6_T1_GET,0U,&Content);
    RVM_DBG_SHS("      X6_T1: 0x",Content,"\r\n");
    
    RVM_RV32P_Debug_Reg_Mod(RVM_BOOT_INIT_KFN,Cap_Thd,
                            RVM_RV32P_KFN_DEBUG_REG_MOD_X7_T2_GET,0U,&Content);
    RVM_DBG_SHS("      X7_T2: 0x",Content,"\r\n");
    
    RVM_RV32P_Debug_Reg_Mod(RVM_BOOT_INIT_KFN,Cap_Thd,
                            RVM_RV32P_KFN_DEBUG_REG_MOD_X8_S0_FP_GET,0U,&Content);
    RVM_DBG_SHS("      X8_S0_FP: 0x",Content,"\r\n");
    
    RVM_RV32P_Debug_Reg_Mod(RVM_BOOT_INIT_KFN,Cap_Thd,
                            RVM_RV32P_KFN_DEBUG_REG_MOD_X9_S1_GET,0U,&Content);
    RVM_DBG_SHS("      X9_S1: 0x",Content,"\r\n");
    
    RVM_RV32P_Debug_Reg_Mod(RVM_BOOT_INIT_KFN,Cap_Thd,
                            RVM_RV32P_KFN_DEBUG_REG_MOD_X10_A0_GET,0U,&Content);
    RVM_DBG_SHS("      X10_A0: 0x",Content,"\r\n");
    
    RVM_RV32P_Debug_Reg_Mod(RVM_BOOT_INIT_KFN,Cap_Thd,
                            RVM_RV32P_KFN_DEBUG_REG_MOD_X11_A1_GET,0U,&Content);
    RVM_DBG_SHS("      X11_A1: 0x",Content,"\r\n");
    
    RVM_RV32P_Debug_Reg_Mod(RVM_BOOT_INIT_KFN,Cap_Thd,
                            RVM_RV32P_KFN_DEBUG_REG_MOD_X12_A2_GET,0U,&Content);
    RVM_DBG_SHS("      X12_A2: 0x",Content,"\r\n");
    
    RVM_RV32P_Debug_Reg_Mod(RVM_BOOT_INIT_KFN,Cap_Thd,
                            RVM_RV32P_KFN_DEBUG_REG_MOD_X13_A3_GET,0U,&Content);
    RVM_DBG_SHS("      X13_A3: 0x",Content,"\r\n");
    
    RVM_RV32P_Debug_Reg_Mod(RVM_BOOT_INIT_KFN,Cap_Thd,
                            RVM_RV32P_KFN_DEBUG_REG_MOD_X14_A4_GET,0U,&Content);
    RVM_DBG_SHS("      X14_A4: 0x",Content,"\r\n");
    
    RVM_RV32P_Debug_Reg_Mod(RVM_BOOT_INIT_KFN,Cap_Thd,
                            RVM_RV32P_KFN_DEBUG_REG_MOD_X15_A5_GET,0U,&Content);
    RVM_DBG_SHS("      X15_A5: 0x",Content,"\r\n");
    
    RVM_RV32P_Debug_Reg_Mod(RVM_BOOT_INIT_KFN,Cap_Thd,
                            RVM_RV32P_KFN_DEBUG_REG_MOD_X16_A6_GET,0U,&Content);
    RVM_DBG_SHS("      X16_A6: 0x",Content,"\r\n");
    
    RVM_RV32P_Debug_Reg_Mod(RVM_BOOT_INIT_KFN,Cap_Thd,
                            RVM_RV32P_KFN_DEBUG_REG_MOD_X17_A7_GET,0U,&Content);
    RVM_DBG_SHS("      X17_A7: 0x",Content,"\r\n");
    
    RVM_RV32P_Debug_Reg_Mod(RVM_BOOT_INIT_KFN,Cap_Thd,
                            RVM_RV32P_KFN_DEBUG_REG_MOD_X18_S2_GET,0U,&Content);
    RVM_DBG_SHS("      X18_S2: 0x",Content,"\r\n");
    
    RVM_RV32P_Debug_Reg_Mod(RVM_BOOT_INIT_KFN,Cap_Thd,
                            RVM_RV32P_KFN_DEBUG_REG_MOD_X19_S3_GET,0U,&Content);
    RVM_DBG_SHS("      X19_S3: 0x",Content,"\r\n");
    
    RVM_RV32P_Debug_Reg_Mod(RVM_BOOT_INIT_KFN,Cap_Thd,
                            RVM_RV32P_KFN_DEBUG_REG_MOD_X20_S4_GET,0U,&Content);
    RVM_DBG_SHS("      X20_S4: 0x",Content,"\r\n");
    
    RVM_RV32P_Debug_Reg_Mod(RVM_BOOT_INIT_KFN,Cap_Thd,
                            RVM_RV32P_KFN_DEBUG_REG_MOD_X21_S5_GET,0U,&Content);
    RVM_DBG_SHS("      X21_S5: 0x",Content,"\r\n");
    
    RVM_RV32P_Debug_Reg_Mod(RVM_BOOT_INIT_KFN,Cap_Thd,
                            RVM_RV32P_KFN_DEBUG_REG_MOD_X22_S6_GET,0U,&Content);
    RVM_DBG_SHS("      X22_S6: 0x",Content,"\r\n");
    
    RVM_RV32P_Debug_Reg_Mod(RVM_BOOT_INIT_KFN,Cap_Thd,
                            RVM_RV32P_KFN_DEBUG_REG_MOD_X23_S7_GET,0U,&Content);
    RVM_DBG_SHS("      X23_S7: 0x",Content,"\r\n");
    
    RVM_RV32P_Debug_Reg_Mod(RVM_BOOT_INIT_KFN,Cap_Thd,
                            RVM_RV32P_KFN_DEBUG_REG_MOD_X24_S8_GET,0U,&Content);
    RVM_DBG_SHS("      X24_S8: 0x",Content,"\r\n");
    
    RVM_RV32P_Debug_Reg_Mod(RVM_BOOT_INIT_KFN,Cap_Thd,
                            RVM_RV32P_KFN_DEBUG_REG_MOD_X25_S9_GET,0U,&Content);
    RVM_DBG_SHS("      X25_S9: 0x",Content,"\r\n");
    
    RVM_RV32P_Debug_Reg_Mod(RVM_BOOT_INIT_KFN,Cap_Thd,
                            RVM_RV32P_KFN_DEBUG_REG_MOD_X26_S10_GET,0U,&Content);
    RVM_DBG_SHS("      X26_S10: 0x",Content,"\r\n");
    
    RVM_RV32P_Debug_Reg_Mod(RVM_BOOT_INIT_KFN,Cap_Thd,
                            RVM_RV32P_KFN_DEBUG_REG_MOD_X27_S11_GET,0U,&Content);
    RVM_DBG_SHS("      X27_S11: 0x",Content,"\r\n");

    RVM_RV32P_Debug_Reg_Mod(RVM_BOOT_INIT_KFN,Cap_Thd,
                            RVM_RV32P_KFN_DEBUG_REG_MOD_X28_T3_GET,0U,&Content);
    RVM_DBG_SHS("      X28_T3: 0x",Content,"\r\n");
    
    RVM_RV32P_Debug_Reg_Mod(RVM_BOOT_INIT_KFN,Cap_Thd,
                            RVM_RV32P_KFN_DEBUG_REG_MOD_X29_T4_GET,0U,&Content);
    RVM_DBG_SHS("      X29_T4: 0x",Content,"\r\n");
    
    RVM_RV32P_Debug_Reg_Mod(RVM_BOOT_INIT_KFN,Cap_Thd,
                            RVM_RV32P_KFN_DEBUG_REG_MOD_X30_T5_GET,0U,&Content);
    RVM_DBG_SHS("      X30_T5: 0x",Content,"\r\n");
    
    RVM_RV32P_Debug_Reg_Mod(RVM_BOOT_INIT_KFN,Cap_Thd,
                            RVM_RV32P_KFN_DEBUG_REG_MOD_X31_T6_GET,0U,&Content);
    RVM_DBG_SHS("      X31_T6: 0x",Content,"\r\n");
#endif
    return 0;
}
#endif
/* End Function:RVM_Thd_Print_Reg ********************************************/

/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
