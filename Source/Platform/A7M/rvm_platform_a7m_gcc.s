/******************************************************************************
Filename    : rvm_platform_a7m_gcc.s
Author      : pry
Date        : 19/01/2017
Description : The ARMv7-M user-level assembly scheduling support of the RVM
              hypervisor. This file is intended to be used with gcc.
******************************************************************************/
    .syntax             unified
    .arch               armv7-m
    .thumb
/* Import ********************************************************************/
    /* Locations provided by the linker */
    .extern             __RVM_Stack
    .extern             __RVM_Data_Load
    .extern             __RVM_Data_Start
    .extern             __RVM_Data_End
    .extern             __RVM_Zero_Start
    .extern             __RVM_Zero_End
    /* The main function */
    .extern             main
/* End Import ****************************************************************/

/* Export ********************************************************************/
    /* User entry stub */
    .global             __RVM_Entry
    /* User level stub for thread creation and synchronous invocation */
    .global             __RVM_Jmp_Stub
    /* Triggering an invocation */
    .global             RVM_Inv_Act
    /* Returning from an invocation */
    .global             RVM_Inv_Ret
    /* System call gate */
    .global             RVM_Svc
    /*Kernel function system call gate*/
    .global             RVM_A7M_Svc_Kfn
    /* Get the MSB in a word */
    .global             __RVM_A7M_MSB_Get
/* End Export ****************************************************************/

/* Header ********************************************************************/
    .section            .text.rvm_header
    .align              3

    .long               0x49535953          /* Magic number for native process */
    .long               0x00000004          /* Four entries specified */
    .long               __RVM_Entry         /* Init thread entry */
    .long               RVM_Sftd            /* All four daemons */
    .long               RVM_Vmmd
    .long               __RVM_Jmp_Stub      /* Jump stub */
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

    .thumb_func
__RVM_Entry:
    /* Load data section from flash to RAM */
    LDR                 R0,=__RVM_Data_Start
    LDR                 R1,=__RVM_Data_End
    LDR                 R2,=__RVM_Data_Load
__RVM_Data_Load:
    CMP                 R0,R1
    BEQ                 __RVM_Data_Done
    LDR					R3,[R2]
    STR                 R3,[R0]
    ADD                 R0,#0x04
    ADD                 R2,#0x04
    B                   __RVM_Data_Load
__RVM_Data_Done:
    /* Clear bss zero section */
    LDR                 R0,=__RVM_Zero_Start
    LDR                 R1,=__RVM_Zero_End
    LDR                 R2,=0x00
__RVM_Zero_Clear:
    CMP                 R0,R1
    BEQ                 __RVM_Zero_Done
    STR                 R2,[R0]
    ADD                 R0,#0x04
    B                   __RVM_Zero_Clear
__RVM_Zero_Done:
    LDR                 R0,=main
    BX                  R0
/* End Entry *****************************************************************/

/* Function:__RVM_Jmp_Stub ****************************************************
Description : The user level stub for thread creation.
Input       : R4 - rvm_ptr_t Entry - The entry address.
              R5 - rvm_ptr_t Stack - The stack address that we are using now.
Output      : None.
Return      : None.
******************************************************************************/
    .section            .text.__rvm_jmp_stub
    .align              3

    .thumb_func
__RVM_Jmp_Stub:
    SUB                 SP,#0x40            /* In order not to destroy the stack */
    MOV                 R0,R5
    BLX                 R4                  /* Branch to the actual entry address */
/* End Function:__RVM_Jmp_Stub ***********************************************/

/* Function:__RVM_A7M_MSB_Get *************************************************
Description : Get the MSB of the word.
Input       : R0 - rvm_ptr_t Val - The value.
Output      : None.
Return      : R0 - rvm_ptr_t - The MSB position.
******************************************************************************/
    .section            .text.__rvm_a7m_msb_get
    .align              3

    .thumb_func
__RVM_A7M_MSB_Get:
    CLZ                 R1,R0
    MOV                 R0,#31
    SUB                 R0,R1
    BX                  LR
/* End Function:__RVM_A7M_MSB_Get ********************************************/

/* Function:RVM_Inv_Act *******************************************************
Description : Activate an invocation. If the return value is not desired, pass
              0 into R2. This is a default implementation that saves all general
              purpose registers and doesn't save FPU context. If you need a faster
              version, consider inline functions; if you need to save FPU contexts,
              please DIY.
Input       : R0 - rvm_cid_t Cap_Inv - The capability slot to the invocation stub. 2-Level.
              R1 - rvm_ptr_t Param - The parameter for the call.
Output      : R2 - rvm_ptr_t* Retval - The return value from the call.
Return      : R0 - rvm_ptr_t - The return value of the system call itself.
******************************************************************************/
    .section            .text.rvm_inv_act
    .align              3

    .thumb_func
RVM_Inv_Act:
    PUSH                {R4-R11}            /* Save registers */
    
    MOV                 R4,#0x10000         /* RVM_SVC_INV_ACT */
    ORR                 R4,R0
    MOV                 R5,R1               /* Parameter */

    SVC                 #0x00               /* System call */
    ISB

    MOV                 R0,R4               /* System call return value */
    CMP                 R2,#0x00            /* Invocation return value*/
    IT                  NE
    STRNE               R5,[R2]
                
    POP                 {R4-R11}            /* Restore registers */
    BX                  LR
/* End Function:RVM_Inv_Act **************************************************/

/* Function:RVM_Inv_Ret *******************************************************
Description : Manually return from an invocation, and set the return value to
              the old register set. This function does not need a capability
              table to work, and never returns.
Input       : R0 - The returning result from the invocation.
Output      : None.
Return      : None.
******************************************************************************/
    .section            .text.rvm_inv_act
    .align              3

    .thumb_func
RVM_Inv_Ret:
    MOV                 R4,#0x00            /* RVM_SVC_INV_RET */
    MOV                 R5,R0               /* Set return value */
                
    SVC                 #0x00               /* System call */
    ISB
                
    MOV                 R0,R4               /* Return failed if we reach here */
    BX                  LR
/* End Function:RVM_Inv_Ret **************************************************/

/* Function:RVM_Svc ***********************************************************
Description : Trigger a system call.
Input       : R0 - rvm_ptr_t Num - The system call number/other information.
              R1 - rvm_ptr_t Param1 - Argument 1.
              R2 - rvm_ptr_t Param2 - Argument 2.
              R3 - rvm_ptr_t Param3 - Argument 3.
Output      : None.
Return      : None.
******************************************************************************/
    .section            .text.rvm_svc
    .align              3

    .thumb_func
RVM_Svc:
    PUSH                {R4-R7}             /* Save registers */
    MOV                 R4,R0               /* Pass parameters */
    MOV                 R5,R1
    MOV                 R6,R2
    MOV                 R7,R3
                
    SVC                 #0x00               /* System call */
    ISB
                
    MOV                 R0,R4               /* Return value */
    POP                 {R4-R7}             /* Restore registers */
    BX                  LR
/* End Function:RVM_Svc ******************************************************/

/* Function:RVM_A7M_Svc_Kfn ***************************************************
Description : Trigger a system call. This is ARMv7-M specific, and does not expand
              to other architectures, and is only used for kernel functions.
              This specially crafted system call allows up to 8 parameters to
              be passed and returned.
Input       : R0 - rvm_ptr_t Num - The system call number/other information.
              R1 - rvm_ptr_t ID - The func ID and sub ID of the kernel function call.
              R2 - rvm_ptr_t Args[6] - Array of 6 arguments.
Output      : R2 - rvm_ptr_t Args[6] - Array of 6 return values.
Return      : R0 - rvm_ret_t - The system call return value.
******************************************************************************/
    .section            .text.rvm_a7m_svc_kfn
    .align              3

    .thumb_func
RVM_A7M_Svc_Kfn:        
    PUSH                {R4-R12}            /* Save registers */
    MOV                 R4,R0               /* Pass parameters */
    MOV                 R5,R1
    MOV                 R12,R2              /* Pass extra parameters */
    LDR                 R6,[R12,#0x00]
    LDR                 R7,[R12,#0x04]
    LDR                 R8,[R12,#0x08]
    LDR                 R9,[R12,#0x0C]
    LDR                 R10,[R12,#0x10]
    LDR                 R11,[R12,#0x14]

    SVC                 #0x00               /* System call */
    ISB

    MOV                 R0,R4               /* System call return value */
    STR                 R6,[R12,#0x00]      /* Extra return values */
    STR                 R7,[R12,#0x04]
    STR                 R8,[R12,#0x08]
    STR                 R9,[R12,#0x0C]
    STR                 R10,[R12,#0x10]
    STR                 R11,[R12,#0x14]

    POP                 {R4-R12}            /* Restore registers */
    BX                  LR
/* End Function:RVM_A7M_Svc_Kfn **********************************************/
    .end
/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/

