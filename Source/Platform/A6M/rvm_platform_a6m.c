/******************************************************************************
Filename    : rvm_platform_a6m.c
Author      : pry
Date        : 26/06/2017
Licence     : The Unlicense; see LICENSE for details.
Description : The ARMv6-M system library platform specific header.
******************************************************************************/

/* Include *******************************************************************/
#include "rvm.h"
#include "rvm_boot.h"

#define __HDR_DEF__
#include "Platform/A6M/rvm_platform_a6m.h"
#include "Monitor/rvm_monitor.h"
#undef __HDR_DEF__

#define __HDR_STRUCT__
#include "Platform/A6M/rvm_platform_a6m.h"
#include "Monitor/rvm_monitor.h"
#undef __HDR_STRUCT__

/* Private include */
#include "Platform/A6M/rvm_platform_a6m.h"

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
    RVM_A6M_PUTCHAR(Char);
    return 0;
}
#endif
/* End Function:RVM_Putchar **************************************************/

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
Input       : rvm_ptr_t Stack - The start(lower) address of the stub.
              rvm_ptr_t Size - The size of the stack.
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
    struct RVM_A6M_Stack* Ptr;
    
    /* No macro provided because not every processor would require this */
    Ptr=(struct RVM_A6M_Stack*)(Stack+Size-
                                RVM_STACK_SAFE_RDCY*sizeof(rvm_ptr_t)-
                                sizeof(struct RVM_A6M_Stack));
    Ptr->R12=0U;
    Ptr->LR=0U;
    Ptr->PC=Stub|0x01U;

    /* Initialize the xPSR to avoid a transition to ARM state */
    Ptr->XPSR=0x01000000U;

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

/* Function:RVM_A6M_Kfn_Act ***************************************************
Description : Activate kernel functions that must use ARMv6-M specific calling
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
    return RVM_A6M_Svc_Kfn((RVM_SVC_KFN<<RVM_WORD_BIT_D1)|((rvm_ptr_t)Cap_Kfn),
                           RVM_PARAM_D1(Sub_ID)|RVM_PARAM_D0(Func_ID),
                           Param);
}
/* End Function:RVM_A6M_Kfn_Act **********************************************/

/* Function:RVM_Thd_Print_Exc *************************************************
Description : Print the exact reason of the fault.
Input       : rvm_tid_t TID - The thread ID.
Output      : None.
Return      : None.
******************************************************************************/
void RVM_Thd_Print_Exc(rvm_tid_t TID)
{
#if(RVM_DBGLOG_ENABLE!=0U)
    RVM_DBG_S("Sftd: Unknown cause - ARMv6-M does not support fault handling.\r\n");
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
    rvm_ptr_t Param[6];
    struct RVM_A6M_Stack* Stack;
    
    /* Print all registers that we can see */
    Param[0]=RVM_A6M_KFN_DEBUG_REG_MOD_R4_GET;
    RVM_ASSERT(RVM_A6M_Kfn_Act(RVM_BOOT_INIT_KFN, 
                               RVM_KFN_DEBUG_REG_MOD,
                               (rvm_ptr_t)Cap_Thd,
                               Param)==0);
    RVM_DBG_SHS("Sftd: R4:0x",Param[0],"\r\n");
    
    Param[0]=RVM_A6M_KFN_DEBUG_REG_MOD_R5_GET;
    RVM_ASSERT(RVM_A6M_Kfn_Act(RVM_BOOT_INIT_KFN, 
                               RVM_KFN_DEBUG_REG_MOD,
                               (rvm_ptr_t)Cap_Thd,
                               Param)==0);
    RVM_DBG_SHS("Sftd: R5:0x",Param[0],"\r\n");
    
    Param[0]=RVM_A6M_KFN_DEBUG_REG_MOD_R6_GET;
    RVM_ASSERT(RVM_A6M_Kfn_Act(RVM_BOOT_INIT_KFN, 
                               RVM_KFN_DEBUG_REG_MOD,
                               (rvm_ptr_t)Cap_Thd,
                               Param)==0);
    RVM_DBG_SHS("Sftd: R6:0x",Param[0],"\r\n");
    
    Param[0]=RVM_A6M_KFN_DEBUG_REG_MOD_R7_GET;
    RVM_ASSERT(RVM_A6M_Kfn_Act(RVM_BOOT_INIT_KFN, 
                               RVM_KFN_DEBUG_REG_MOD,
                               (rvm_ptr_t)Cap_Thd,
                               Param)==0);
    RVM_DBG_SHS("Sftd: R7:0x",Param[0],"\r\n");
    
    Param[0]=RVM_A6M_KFN_DEBUG_REG_MOD_R8_GET;
    RVM_ASSERT(RVM_A6M_Kfn_Act(RVM_BOOT_INIT_KFN, 
                               RVM_KFN_DEBUG_REG_MOD,
                               (rvm_ptr_t)Cap_Thd,
                               Param)==0);
    RVM_DBG_SHS("Sftd: R8:0x",Param[0],"\r\n");
    
    Param[0]=RVM_A6M_KFN_DEBUG_REG_MOD_R9_GET;
    RVM_ASSERT(RVM_A6M_Kfn_Act(RVM_BOOT_INIT_KFN, 
                               RVM_KFN_DEBUG_REG_MOD,
                               (rvm_ptr_t)Cap_Thd,
                               Param)==0);
    RVM_DBG_SHS("Sftd: R9:0x",Param[0],"\r\n");
    
    Param[0]=RVM_A6M_KFN_DEBUG_REG_MOD_R10_GET;
    RVM_ASSERT(RVM_A6M_Kfn_Act(RVM_BOOT_INIT_KFN, 
                               RVM_KFN_DEBUG_REG_MOD,
                               (rvm_ptr_t)Cap_Thd,
                               Param)==0);
    RVM_DBG_SHS("Sftd: R10:0x",Param[0],"\r\n");
    
    Param[0]=RVM_A6M_KFN_DEBUG_REG_MOD_R11_GET;
    RVM_ASSERT(RVM_A6M_Kfn_Act(RVM_BOOT_INIT_KFN, 
                               RVM_KFN_DEBUG_REG_MOD,
                               (rvm_ptr_t)Cap_Thd,
                               Param)==0);
    RVM_DBG_SHS("Sftd: R11:0x",Param[0],"\r\n");
    
    Param[0]=RVM_A6M_KFN_DEBUG_REG_MOD_SP_GET;
    RVM_ASSERT(RVM_A6M_Kfn_Act(RVM_BOOT_INIT_KFN, 
                               RVM_KFN_DEBUG_REG_MOD,
                               (rvm_ptr_t)Cap_Thd,
                               Param)==0);
    RVM_DBG_SHS("Sftd: SP:0x",Param[0],"\r\n");
    Stack=(struct RVM_A6M_Stack*)Param[0];
    
    Param[0]=RVM_A6M_KFN_DEBUG_REG_MOD_LR_GET;
    RVM_ASSERT(RVM_A6M_Kfn_Act(RVM_BOOT_INIT_KFN, 
                               RVM_KFN_DEBUG_REG_MOD,
                               (rvm_ptr_t)Cap_Thd,
                               Param)==0);
    RVM_DBG_SHS("Sftd: LR:0x",Param[0],"\r\n");

    /* We know where SP is, but we are not sure whether accessing SP in RVM
     * will cause issues. We assume that the user program will not fiddle
     * with SP maliciously (especially during the debugging phase) and we 
     * always print the stack trace, though this is quite dangerous. When 
     * developing products, this register printing function will be disabled
     * anyway and thus does not cause security breaches. */
    RVM_DBG_SHS("Sftd: Stack-R0:0x",Stack->R0,"\r\n");
    RVM_DBG_SHS("Sftd: Stack-R1:0x",Stack->R1,"\r\n");
    RVM_DBG_SHS("Sftd: Stack-R2:0x",Stack->R2,"\r\n");
    RVM_DBG_SHS("Sftd: Stack-R3:0x",Stack->R3,"\r\n");
    RVM_DBG_SHS("Sftd: Stack-R12:0x",Stack->R12,"\r\n");
    RVM_DBG_SHS("Sftd: Stack-LR:0x",Stack->LR,"\r\n");
    RVM_DBG_SHS("Sftd: Stack-PC:0x",Stack->PC,"\r\n");
    RVM_DBG_SHS("Sftd: Stack-XPSR:0x",Stack->XPSR,"\r\n");
#endif

    return 0;
}
/* End Function:RVM_Thd_Print_Reg ********************************************/

/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
