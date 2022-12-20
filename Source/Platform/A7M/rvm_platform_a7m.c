/******************************************************************************
Filename    : rvm_platform_a7m.c
Author      : pry
Date        : 26/06/2017
Licence     : The Unlicense; see LICENSE for details.
Description : The Cortex-M system library platform specific header.
******************************************************************************/

/* Includes ******************************************************************/
#include "rvm.h"

#define __HDR_DEFS__
#include "Platform/A7M/rvm_platform_a7m.h"
#include "Monitor/rvm_syssvc.h"
#undef __HDR_DEFS__

#define __HDR_STRUCTS__
#include "Platform/A7M/rvm_platform_a7m.h"
#include "Monitor/rvm_syssvc.h"
#undef __HDR_STRUCTS__

/* Private include */
#include "Platform/A7M/rvm_platform_a7m.h"

#define __HDR_PUBLIC_MEMBERS__
#include "Monitor/rvm_syssvc.h"
#undef __HDR_PUBLIC_MEMBERS__
/* End Includes **************************************************************/

/* Begin Function:RVM_Putchar *************************************************
Description : Output a character to console. This is for user-level debugging 
              only.
Input       : char Char - The character to print.
Output      : None.
Return      : rvm_ptr_t - Always 0.
******************************************************************************/
#if(RVM_DEBUG_PRINT==1U)
rvm_ptr_t RVM_Putchar(char Char)
{
    RVM_A7M_PUTCHAR(Char);
    return 0;
}
#endif
/* End Function:RVM_Putchar **************************************************/

/* Begin Function:RVM_Stack_Init **********************************************
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
              rvm_ptr_t Entry_Addr - The entry address of the thread, not used here.
              rvm_ptr_t Stub_Addr - The jump stub address.
Output      : None.
Return      : rvm_ptr_t - The actual stack address to use for system call.
******************************************************************************/
rvm_ptr_t RVM_Stack_Init(rvm_ptr_t Stack_Base,
                         rvm_ptr_t Stack_Size,
                         rvm_ptr_t Entry_Addr,
                         rvm_ptr_t Stub_Addr)

{
    struct RVM_A7M_Ret_Stack* Stack_Ptr;
    
    Stack_Ptr=(struct RVM_A7M_Ret_Stack*)(Stack_Base+Stack_Size-
                                          RVM_STACK_SAFE_RDCY*sizeof(rvm_ptr_t)-
                                          sizeof(struct RVM_A7M_Ret_Stack));
    Stack_Ptr->R12=0U;
    Stack_Ptr->LR=0U;
    Stack_Ptr->PC=Stub_Addr|0x01U;
    /* Initialize the xPSR to avoid a transition to ARM state */
    Stack_Ptr->XPSR=0x01000200U;
    
    return (rvm_ptr_t)Stack_Ptr;
}
/* End Function:RVM_Stack_Init ***********************************************/

/* Begin Function:RVM_Idle ****************************************************
Description : Put the processor into idle state.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
void RVM_Idle(void)
{
    /* Put us to sleep */
    RVM_Kern_Act(RVM_BOOT_INIT_KERN, RVM_KERN_IDLE_SLEEP, 0U, 0U, 0U);
}
/* End Function:RVM_Idle *****************************************************/

/* Begin Function:RVM_A7M_Kern_Act ********************************************
Description : Activate kernel functions that must use ARMv7-M specific calling
              convention to pass extra parameters.
Input       : rvm_cid_t Cap_Kern - The capability to the kernel capability. 2-Level.
              rvm_ptr_t Func_ID - The function ID to invoke.
              rvm_ptr_t Sub_ID - The subfunction ID to invoke.
              rvm_ptr_t* Params - The parameters, stored in an array of size 6.
Output      : rvm_ptr_t* Params - The return values, stored in an array of size 6.
Return      : rvm_ret_t - If successful, 0; else a negative value.
******************************************************************************/
rvm_ret_t RVM_A7M_Kern_Act(rvm_cid_t Cap_Kern,
                           rvm_ptr_t Func_ID,
                           rvm_ptr_t Sub_ID,
                           rvm_ptr_t* Params)
{
    return RVM_A7M_Svc_Kern((RVM_SVC_KERN<<(sizeof(rvm_ptr_t)*4U))|((rvm_ptr_t)Cap_Kern),
                            RVM_PARAM_D1(Sub_ID)|RVM_PARAM_D0(Func_ID),
                            Params);
}
/* End Function:RVM_A7M_Kern_Act *********************************************/

/* Begin Function:RVM_Thd_Print_Fault *****************************************
Description : Print the exact reason of the fault.
Input       : rvm_ptr_t Fault - The exact reason of the fault.
Output      : None.
Return      : None.
******************************************************************************/
void RVM_Thd_Print_Fault(rvm_ptr_t Fault)
{
#if(RVM_DEBUG_PRINT==1U)
    if((Fault&RVM_A7M_HFSR_DEBUGEVT)!=0U)
        RVM_DBG_S("Sftd:Debug event.\r\n");
    if((Fault&RVM_A7M_HFSR_FORCED)!=0U)
        RVM_DBG_S("Sftd:Configurable-priority exception escalated to HardFault.\r\n");
    if((Fault&RVM_A7M_HFSR_VECTTBL)!=0U)
        RVM_DBG_S("Sftd:Vector table read fault.\r\n");
    if((Fault&RVM_A7M_UFSR_DIVBYZERO)!=0U)
        RVM_DBG_S("Sftd:Divide by zero.\r\n");
    if((Fault&RVM_A7M_UFSR_UNALIGNED)!=0U)
        RVM_DBG_S("Sftd:Unaligned load/store access.\r\n");
    if((Fault&RVM_A7M_UFSR_NOCP)!=0U)
        RVM_DBG_S("Sftd:No such coprocessor.\r\n");
    if((Fault&RVM_A7M_UFSR_INVPC)!=0U)
        RVM_DBG_S("Sftd:Invalid vector return LR or PC value.\r\n");
    if((Fault&RVM_A7M_UFSR_INVSTATE)!=0U)
        RVM_DBG_S("Sftd:Attempt to enter an invalid instruction set (ARM) state.\r\n");
    if((Fault&RVM_A7M_UFSR_UNDEFINSTR)!=0U)
        RVM_DBG_S("Sftd:Invalid IT or related instruction.\r\n");
    if((Fault&RVM_A7M_BFSR_LSPERR)!=0U)
        RVM_DBG_S("Sftd:Bus fault during FP lazy stacking.\r\n");
    if((Fault&RVM_A7M_BFSR_STKERR)!=0U)
        RVM_DBG_S("Sftd:Derived bus fault on exception entry.\r\n");
    if((Fault&RVM_A7M_BFSR_UNSTKERR)!=0U)
        RVM_DBG_S("Sftd:Derived bus fault on exception return.\r\n");
    if((Fault&RVM_A7M_BFSR_IMPRECISERR)!=0U)
        RVM_DBG_S("Sftd:Imprecise data access error.\r\n");
    if((Fault&RVM_A7M_BFSR_PRECISERR)!=0U)
        RVM_DBG_S("Sftd:Precise data access error.\r\n");
    if((Fault&RVM_A7M_BFSR_IBUSERR)!=0U)
        RVM_DBG_S("Sftd:Bus fault on instruction prefetch.\r\n");
    if((Fault&RVM_A7M_MFSR_MLSPERR)!=0U)
        RVM_DBG_S("Sftd:Memory management fault during FP lazy state preservation.\r\n");
    if((Fault&RVM_A7M_MFSR_MSTKERR)!=0U)
        RVM_DBG_S("Sftd:Derived memory management fault on exception entry.\r\n");
    if((Fault&RVM_A7M_MFSR_MUNSTKERR)!=0U)
        RVM_DBG_S("Sftd:Derived memory management fault on exception return.\r\n");
    if((Fault&RVM_A7M_MFSR_DACCVIOL)!=0U)
        RVM_DBG_S("Sftd:Data access violation.\r\n");
    if((Fault&RVM_A7M_MFSR_IACCVIOL)!=0U)
        RVM_DBG_S("Sftd:Instruction access violation.\r\n");
#endif
}
/* End Function:RVM_Thd_Print_Fault ******************************************/

/* Begin Function:RVM_Thd_Print_Reg *******************************************
Description : Print the register set of a thread.
Input       : rvm_cid_t Cap_Thd - The capability to the thread.
Output      : None.
Return      : rvm_ret_t - If successful, 0; else a negative value.
******************************************************************************/
rvm_ret_t RVM_Thd_Print_Reg(rvm_cid_t Cap_Thd)
{
#if(RVM_DEBUG_PRINT==1U)
    rvm_ptr_t Params[6];
    struct RVM_A7M_Ret_Stack* Stack;
    
    /* Print all registers that we can see */
    Params[0]=RVM_A7M_KERN_DEBUG_REG_MOD_R4_GET;
    RVM_ASSERT(RVM_A7M_Kern_Act(RVM_BOOT_INIT_KERN, 
                                RVM_KERN_DEBUG_REG_MOD,
                                (rvm_ptr_t)Cap_Thd,
                                Params)==0);
    RVM_DBG_SHS("Sftd:R4:0x",Params[0],"\r\n");
    
    Params[0]=RVM_A7M_KERN_DEBUG_REG_MOD_R5_GET;
    RVM_ASSERT(RVM_A7M_Kern_Act(RVM_BOOT_INIT_KERN, 
                                RVM_KERN_DEBUG_REG_MOD,
                                (rvm_ptr_t)Cap_Thd,
                                Params)==0);
    RVM_DBG_SHS("Sftd:R5:0x",Params[0],"\r\n");
    
    Params[0]=RVM_A7M_KERN_DEBUG_REG_MOD_R6_GET;
    RVM_ASSERT(RVM_A7M_Kern_Act(RVM_BOOT_INIT_KERN, 
                                RVM_KERN_DEBUG_REG_MOD,
                                (rvm_ptr_t)Cap_Thd,
                                Params)==0);
    RVM_DBG_SHS("Sftd:R6:0x",Params[0],"\r\n");
    
    Params[0]=RVM_A7M_KERN_DEBUG_REG_MOD_R7_GET;
    RVM_ASSERT(RVM_A7M_Kern_Act(RVM_BOOT_INIT_KERN, 
                                RVM_KERN_DEBUG_REG_MOD,
                                (rvm_ptr_t)Cap_Thd,
                                Params)==0);
    RVM_DBG_SHS("Sftd:R7:0x",Params[0],"\r\n");
    
    Params[0]=RVM_A7M_KERN_DEBUG_REG_MOD_R8_GET;
    RVM_ASSERT(RVM_A7M_Kern_Act(RVM_BOOT_INIT_KERN, 
                                RVM_KERN_DEBUG_REG_MOD,
                                (rvm_ptr_t)Cap_Thd,
                                Params)==0);
    RVM_DBG_SHS("Sftd:R8:0x",Params[0],"\r\n");
    
    Params[0]=RVM_A7M_KERN_DEBUG_REG_MOD_R9_GET;
    RVM_ASSERT(RVM_A7M_Kern_Act(RVM_BOOT_INIT_KERN, 
                                RVM_KERN_DEBUG_REG_MOD,
                                (rvm_ptr_t)Cap_Thd,
                                Params)==0);
    RVM_DBG_SHS("Sftd:R9:0x",Params[0],"\r\n");
    
    Params[0]=RVM_A7M_KERN_DEBUG_REG_MOD_R10_GET;
    RVM_ASSERT(RVM_A7M_Kern_Act(RVM_BOOT_INIT_KERN, 
                                RVM_KERN_DEBUG_REG_MOD,
                                (rvm_ptr_t)Cap_Thd,
                                Params)==0);
    RVM_DBG_SHS("Sftd:R10:0x",Params[0],"\r\n");
    
    Params[0]=RVM_A7M_KERN_DEBUG_REG_MOD_R11_GET;
    RVM_ASSERT(RVM_A7M_Kern_Act(RVM_BOOT_INIT_KERN, 
                                RVM_KERN_DEBUG_REG_MOD,
                                (rvm_ptr_t)Cap_Thd,
                                Params)==0);
    RVM_DBG_SHS("Sftd:R11:0x",Params[0],"\r\n");
    
    Params[0]=RVM_A7M_KERN_DEBUG_REG_MOD_SP_GET;
    RVM_ASSERT(RVM_A7M_Kern_Act(RVM_BOOT_INIT_KERN, 
                                RVM_KERN_DEBUG_REG_MOD,
                                (rvm_ptr_t)Cap_Thd,
                                Params)==0);
    RVM_DBG_SHS("Sftd:SP:0x",Params[0],"\r\n");
    Stack=(struct RVM_A7M_Ret_Stack*)Params[0];
    
    Params[0]=RVM_A7M_KERN_DEBUG_REG_MOD_LR_GET;
    RVM_ASSERT(RVM_A7M_Kern_Act(RVM_BOOT_INIT_KERN, 
                                RVM_KERN_DEBUG_REG_MOD,
                                (rvm_ptr_t)Cap_Thd,
                                Params)==0);
    RVM_DBG_SHS("Sftd:LR:0x",Params[0],"\r\n");

    Params[0]=RVM_A7M_KERN_DEBUG_ERR_GET_CAUSE;
    RVM_ASSERT(RVM_A7M_Kern_Act(RVM_BOOT_INIT_KERN, 
                                RVM_KERN_DEBUG_ERR_GET,
                                (rvm_ptr_t)Cap_Thd,
                                Params)==0);
    RVM_DBG_SHS("Sftd:Cause:0x",Params[0],"\r\n");
    
    Params[0]=RVM_A7M_KERN_DEBUG_ERR_GET_ADDR;
    RVM_ASSERT(RVM_A7M_Kern_Act(RVM_BOOT_INIT_KERN, 
                                RVM_KERN_DEBUG_ERR_GET,
                                (rvm_ptr_t)Cap_Thd,
                                Params)==0);
    RVM_DBG_SHS("Sftd:Addr:0x",Params[0],"\r\n");

    /* We know where SP is, but we are not sure whether accessing SP in RVM
     * will cause issues. We assume that the user program will not fiddle
     * with SP maliciously (especially during the debugging phase) and we 
     * always print the stack trace, though this is quite dangerous. When 
     * developing products, this register printing function will be disabled
     * anyway and thus does not cause security breaches. */
    RVM_DBG_SHS("Sftd:Stack-R0:0x",Stack->R0,"\r\n");
    RVM_DBG_SHS("Sftd:Stack-R1:0x",Stack->R1,"\r\n");
    RVM_DBG_SHS("Sftd:Stack-R2:0x",Stack->R2,"\r\n");
    RVM_DBG_SHS("Sftd:Stack-R3:0x",Stack->R3,"\r\n");
    RVM_DBG_SHS("Sftd:Stack-R12:0x",Stack->R12,"\r\n");
    RVM_DBG_SHS("Sftd:Stack-LR:0x",Stack->LR,"\r\n");
    RVM_DBG_SHS("Sftd:Stack-PC:0x",Stack->PC,"\r\n");
    RVM_DBG_SHS("Sftd:Stack-XPSR:0x",Stack->XPSR,"\r\n");
    
#endif
    return 0;
}
/* End Function:RVM_Thd_Print_Regs *******************************************/

/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
