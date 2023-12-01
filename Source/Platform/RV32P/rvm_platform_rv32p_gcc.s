/******************************************************************************
Filename    : rvm_platform_rv32p_gcc.s
Author      : pry
Date        : 19/01/2017
Description : The RISC-V 32bit PMP user-level assembly support of RVM.
******************************************************************************/

/* Import ********************************************************************/
    /* Locations provided by the linker */
    .extern             __RVM_Stack
    .extern             __RVM_Global
    .extern             __RVM_Data_Load
    .extern             __RVM_Data_Start
    .extern             __RVM_Data_End
    .extern             __RVM_Zero_Start
    .extern             __RVM_Zero_End
    /* C library entry */
    .extern             main
    /* All four daemons */
    .extern             RVM_Sftd
    .extern             RVM_Vmmd
/* End Import ****************************************************************/

/* Export ********************************************************************/
    /* Entry point */
    .global             __RVM_Entry
    /* Jump stub */
    .global             __RVM_Stub
    /* Triggering an invocation */
    .global             RVM_Inv_Act
    /* Returning from an invocation */
    .global             RVM_Inv_Ret
    /* System call gate */
    .global             RVM_Svc
    /* Kernel function system call gate */
    .global             RVM_RV32P_Svc_Kfn
/* End Export ****************************************************************/

/* Header ********************************************************************/
    .section            .text.rvm_header
    .align              3

    .word               0x49535953          /* Magic number for native process */
    .word               0x00000004          /* Four entries specified */
    .word               __RVM_Entry         /* Init thread entry */
    .word               RVM_Sftd            /* All four daemons */
    .word               RVM_Vmmd
    .word               __RVM_Stub          /* Jump stub */
    NOP                                     /* Catch something in the middle */
    NOP
    NOP
    NOP
    NOP
    NOP
    NOP
    NOP
    NOP
    NOP
/* End Header ****************************************************************/

/* Entry *********************************************************************/
    .section            .text.rvm_entry
    .align              3

__RVM_Entry:
    /* Set GP and SP */
    .option             push
    .option             norelax
    LA                  gp,__RVM_Global
    .option             pop
    LA                  sp,__RVM_Stack
    /* Load data section from flash to RAM */
    LA                  a0,__RVM_Data_Load
    LA                  a1,__RVM_Data_Start
    LA                  a2,__RVM_Data_End
__RVM_Data_Load:
    LW                  t0,(a0)
    SW                  t0,(a1)
    ADDI                a0,a0,4
    ADDI                a1,a1,4
    BLTU                a1,a2,__RVM_Data_Load
    /* Clear bss zero section */
    LA                  a0,__RVM_Zero_Start
    LA                  a1,__RVM_Zero_End
__RVM_Zero_Clear:
    SW                  zero,(a0)
    ADDI                a0,a0,4
    BLTU                a0,a1,__RVM_Zero_Clear
    /* Branch to main function */
    J                   main
/* End Entry *****************************************************************/

/* Function:__RVM_Stub ********************************************************
Description : The user level stub for thread creation.
Input       : a0 - The parameter, which is passed on without change.
Output      : None.
Return      : None.
******************************************************************************/
    .section            .text.__rvm_stub
    .align              3

__RVM_Stub:
    .option             push
    .option             norelax
    LA                  gp,__RVM_Global     /* Load gp for every thread */
    .option             pop
    LW                  a1,(sp)
    ADDI                sp,sp,-12           /* Align stack to 16 bytes */
    JR                  a1                  /* Branch to the actual entry address */
/* End Function:__RVM_Stub ***************************************************/

/* Function:RVM_Inv_Act *******************************************************
Description : Activate an invocation. If the return value is not desired, pass
              0 into R2. This is a default implementation that saves all general
              purpose registers and doesn't save FPU context. If you need a faster
              version, consider inline functions; if you need to save FPU contexts,
              please DIY.
Input       : a0 - rvm_cid_t Cap_Inv - The capability slot to the invocation stub. 2-Level.
              a1 - rvm_ptr_t Param - The parameter for the call.
Output      : a2 - rvm_ptr_t* Retval - The return value from the call.
Return      : a0 - rvm_ptr_t - The return value of the system call itself.
******************************************************************************/
    .section            .text.rvm_inv_act
    .align              3

RVM_Inv_Act:
    ADDI                sp,sp,-28*4         /* Allocate stack space for context */
    SW                  x31,27*4(sp)        /* Save GP registers except for zero, sp, a0 and a1 */
    SW                  x30,26*4(sp)
    SW                  x29,25*4(sp)
    SW                  x28,24*4(sp)
    SW                  x27,23*4(sp)
    SW                  x26,22*4(sp)
    SW                  x25,21*4(sp)
    SW                  x24,20*4(sp)
    SW                  x23,19*4(sp)
    SW                  x22,18*4(sp)
    SW                  x21,17*4(sp)
    SW                  x20,16*4(sp)
    SW                  x19,15*4(sp)
    SW                  x18,14*4(sp)
    SW                  x17,13*4(sp)
    SW                  x16,12*4(sp)
    SW                  x15,11*4(sp)
    SW                  x14,10*4(sp)
    SW                  x13,9*4(sp)
    SW                  x12,8*4(sp)
    SW                  x9,7*4(sp)
    SW                  x8,6*4(sp)
    SW                  x7,5*4(sp)
    SW                  x6,4*4(sp)
    SW                  x5,3*4(sp)
    SW                  x4,2*4(sp)
    SW                  x3,1*4(sp)
    SW                  x1,0*4(sp)
    
    LI                  a2,0x10000          /* RVM_SVC_INV_ACT */
    OR                  a0,a0,a2
    ECALL                                   /* System call */

    LW                  x1,0*4(sp)          /* Load GP registers except for zero, sp, a0 and a1 */
    LW                  x3,1*4(sp)
    LW                  x4,2*4(sp)
    LW                  x5,3*4(sp)
    LW                  x6,4*4(sp)
    LW                  x7,5*4(sp)
    LW                  x8,6*4(sp)
    LW                  x9,7*4(sp)
    LW                  x12,8*4(sp)
    LW                  x13,9*4(sp)
    LW                  x14,10*4(sp)
    LW                  x15,11*4(sp)
    LW                  x16,12*4(sp)
    LW                  x17,13*4(sp)
    LW                  x18,14*4(sp)
    LW                  x19,15*4(sp)
    LW                  x20,16*4(sp)
    LW                  x21,17*4(sp)
    LW                  x22,18*4(sp)
    LW                  x23,19*4(sp)
    LW                  x24,20*4(sp)
    LW                  x25,21*4(sp)
    LW                  x26,22*4(sp)
    LW                  x27,23*4(sp)
    LW                  x28,24*4(sp)
    LW                  x29,25*4(sp)
    LW                  x30,26*4(sp)
    LW                  x31,27*4(sp)
    ADDI                sp,sp,28*4          /* Restore stack space for context */

    BEQZ                a2,RVM_Inv_Act_Skip /* Invocation return value */
    SW                  a1,(a2)
RVM_Inv_Act_Skip:
    RET
/* End Function:RVM_Inv_Act **************************************************/

/* Function:RVM_Inv_Ret *******************************************************
Description : Manually return from an invocation, and set the return value to
              the old register set. This function does not need a capability
              table to work, and never returns.
Input       : a0 - The returning result from the invocation.
Output      : None.
Return      : None.
******************************************************************************/
    .section            .text.rvm_inv_ret
    .align              3

RVM_Inv_Ret:
    MV                  a1,a0               /* Set return value */
    MV                  a0,zero             /* RVM_SVC_INV_RET */
                
    ECALL                                   /* System call */

    RET                                     /* If we reach here, then return failed */
/* End Function:RVM_Inv_Ret **************************************************/

/* Function:RVM_Svc ***********************************************************
Description : Trigger a system call.
Input       : a0 - rvm_ptr_t Num - The system call number/other information.
              a1 - rvm_ptr_t Param1 - Argument 1.
              a2 - rvm_ptr_t Param2 - Argument 2.
              a3 - rvm_ptr_t Param3 - Argument 3.
Output      : None.
Return      : a0 - rvm_ret_t - The system call return value.
******************************************************************************/
    .section            .text.rvm_svc
    .align              3

RVM_Svc:
    ECALL                                   /* System call */
    RET
/* End Function:RVM_Svc ******************************************************/

/* Function:RVM_RV32P_Svc_Kfn *************************************************
Description : Trigger a system call. This is RV32P specific, and does not expand
              to other architectures, and is only used for kernel functions.
              RISC-V calling convention allows to pass up to 8 parameters in
              registers, however we only use up to 3. This is primarily for 
              return values, which would have to stay in an array.
Input       : a0 - rvm_ptr_t Num - The system call number/other information.
              a1 - rvm_ptr_t ID - The func ID and sub ID of the kernel function call.
              a2 - rvm_ptr_t Param[6] - Array of 6 arguments.
Output      : a2 - rvm_ptr_t Param[6] - Array of 6 return values.
Return      : a0 - rvm_ret_t - The system call return value.
******************************************************************************/
    .section            .text.rvm_rv32p_svc_kfn
    .align              3

RVM_RV32P_Svc_Kfn:
    ADDI                sp,sp,-7*4          /* Push used regs to stack */
    SW                  t0,6*4(sp)
    SW                  a7,5*4(sp)
    SW                  a6,4*4(sp)
    SW                  a5,3*4(sp)
    SW                  a4,2*4(sp)
    SW                  a3,1*4(sp)
    SW                  a2,0*4(sp)

    MV                  t0,a2               /* Pass extra parameters */
    LW                  a2,0*4(t0)
    LW                  a3,1*4(t0)
    LW                  a4,2*4(t0)
    LW                  a5,3*4(t0)
    LW                  a6,4*4(t0)
    LW                  a7,5*4(t0)

    ECALL                                   /* System call */

    SW                  a2,0*4(t0)          /* Extra return values */
    SW                  a3,1*4(t0)
    SW                  a4,2*4(t0)
    SW                  a5,3*4(t0)
    SW                  a6,4*4(t0)
    SW                  a7,5*4(t0)

    LW                  a2,0*4(sp)          /* Pop used regs from stack */
    LW                  a3,1*4(sp)
    LW                  a4,2*4(sp)
    LW                  a5,3*4(sp)
    LW                  a6,4*4(sp)
    LW                  a7,5*4(sp)
    LW                  t0,5*4(sp)
    ADDI                sp,sp,7*4
    RET
/* End Function:RVM_RV32P_Svc_Kfn ********************************************/

    .end
/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/

