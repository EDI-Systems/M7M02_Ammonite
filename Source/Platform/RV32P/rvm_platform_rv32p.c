/******************************************************************************
Filename    : rvm_platform_rv32p.c
Author      : pry
Date        : 26/06/2017
Licence     : The Unlicense; see LICENSE for details.
Description : The RISC-V system library platform specific header.
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

/* Function:RVM_Putchar *******************************************************
Description : Output a character to console. This is for user-level debugging 
              only.
Input       : char Char - The character to print.
Output      : None.
Return      : rvm_ptr_t - Always 0.
******************************************************************************/
#if(RVM_DBGLOG_ENABLE!=0U)
rvm_ptr_t RVM_Putchar(char Char)
{
    RVM_RV32P_PUTCHAR(Char);
    return 0;
}
#endif
/* End Function:RVM_Putchar **************************************************/

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

/* Function:RVM_Idle **********************************************************
Description : Put the processor into idle state.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
void RVM_Idle(void)
{
    /* Put us to sleep */
    RVM_Kfn_Act(RVM_BOOT_INIT_KFN,RVM_KFN_IDLE_SLEEP,0U,0U,0U);
}
/* End Function:RVM_Idle *****************************************************/

/* Function:RVM_Thd_Cop_Size **************************************************
Description : Query coprocessor register size for this CPU.
Input       : rme_ptr_t Attr - The thread context attributes.
Output      : None.
Return      : rme_ptr_t - The coprocessor context size.
******************************************************************************/
rvm_ptr_t RVM_Thd_Cop_Size(rvm_ptr_t Attr)
{
    if(Attr!=RVM_RV32P_ATTR_NONE)
        return sizeof(struct RVM_RV32P_Cop_Struct);
            
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

/* Function:RVM_Thd_Print_Exc *************************************************
Description : Print the exact reason of the fault.
Input       : rvm_tid_t TID - The thread ID.
Output      : None.
Return      : None.
******************************************************************************/
void RVM_Thd_Print_Exc(rvm_tid_t TID)
{
#if(RVM_DBGLOG_ENABLE!=0U)
    rvm_ptr_t Cause;
    rvm_ptr_t Addr;
    rvm_ptr_t Value;
    rvm_ptr_t Param[6];
    
    /* Get exception cause */
    Param[0]=RVM_RV32P_KFN_DEBUG_EXC_CAUSE_GET;
    RVM_ASSERT(RVM_RV32P_Kfn_Act(RVM_BOOT_INIT_KFN, 
                                 RVM_KFN_DEBUG_EXC_GET,
                                 (rvm_ptr_t)TID,
                                 Param)==0);
    Cause=Param[0];
    
    /* Get exception address */
    Param[0]=RVM_RV32P_KFN_DEBUG_EXC_ADDR_GET;
    RVM_ASSERT(RVM_RV32P_Kfn_Act(RVM_BOOT_INIT_KFN, 
                                 RVM_KFN_DEBUG_EXC_GET,
                                 (rvm_ptr_t)TID,
                                 Param)==0);
    Addr=Param[0];
    
    /* Get exception value */
    Param[0]=RVM_RV32P_KFN_DEBUG_EXC_VALUE_GET;
    RVM_ASSERT(RVM_RV32P_Kfn_Act(RVM_BOOT_INIT_KFN, 
                                 RVM_KFN_DEBUG_EXC_GET,
                                 (rvm_ptr_t)TID,
                                 Param)==0);
    Value=Param[0];
    
    /* Parse exception reason */
    switch(Cause)
    {
        case RVM_RV32P_MCAUSE_IMALIGN:  RVM_DBG_S("Sftd: Instruction misaligned"); break;
        case RVM_RV32P_MCAUSE_IACCFLT:  RVM_DBG_S("Sftd: Instruction access fault"); break;
        case RVM_RV32P_MCAUSE_IUNDEF:   RVM_DBG_S("Sftd: Undefined instruction"); break;
        case RVM_RV32P_MCAUSE_IBRKPT:   RVM_DBG_S("Sftd: Breakpoint"); break;
        case RVM_RV32P_MCAUSE_LALIGN:   RVM_DBG_S("Sftd: Data load misaligned"); break;
        case RVM_RV32P_MCAUSE_LACCFLT:  RVM_DBG_S("Sftd: Data load access fault"); break;
        case RVM_RV32P_MCAUSE_SALIGN:   RVM_DBG_S("Sftd: Data store misaligned"); break;
        case RVM_RV32P_MCAUSE_SACCFLT:  RVM_DBG_S("Sftd: Data store access fault"); break;
        /* In most cases these below shall never happen, because we're assuming MCUs */
        case RVM_RV32P_MCAUSE_U_ECALL:  RVM_DBG_S("Sftd: System call from user mode"); break;
        case RVM_RV32P_MCAUSE_S_ECALL:  RVM_DBG_S("Sftd: System call from supervisor mode"); break;
        case RVM_RV32P_MCAUSE_H_ECALL:  RVM_DBG_S("Sftd: System call from hypervisor mode"); break;
        case RVM_RV32P_MCAUSE_M_ECALL:  RVM_DBG_S("Sftd: System call from machine mode"); break;
        case RVM_RV32P_MCAUSE_IPGFLT:   RVM_DBG_S("Sftd: Instruction page fault"); break;
        case RVM_RV32P_MCAUSE_LPGFLT:   RVM_DBG_S("Sftd: Load page fault"); break;
        case RVM_RV32P_MCAUSE_SPGFLT:   RVM_DBG_S("Sftd: Store page fault"); break;
        default:                        RVM_DBG_S("Sftd: Unknown cause"); break;
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
    /* TODO */
#endif
    return 0;
}
/* End Function:RVM_Thd_Print_Reg ********************************************/

/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
