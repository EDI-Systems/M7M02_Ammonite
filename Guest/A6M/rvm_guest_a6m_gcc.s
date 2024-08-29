/******************************************************************************
Filename    : rvm_guest_cmx_asm_gcc.S
Author      : pry
Date        : 09/02/2018
Description : The assembly portion of the RVM guest library for Cortex-M3/4/7.
              This file is for use with gcc.
******************************************************************************/
    .syntax             unified
    .arch               armv6s-m
    .thumb
/* Import ********************************************************************/
    /* Locations provided by the linker */
    .extern             _RVM_Stack
    .extern             _RVM_Data_Load
    .extern             _RVM_Data_Start
    .extern             _RVM_Data_End
    .extern             _RVM_Zero_Start
    .extern             _RVM_Zero_End
    /* The main function */
    .extern             main
/* End Import ****************************************************************/

/* Export ********************************************************************/
    /* Entry point */
    .global             _RVM_Entry
    /* Jump stub */
    .global             _RVM_Stub
    /* Triggering an invocation */
    .global             RVM_Inv_Act
    /* Returning from an invocation */
    .global             RVM_Inv_Ret
    /* System call gate */
    .global             RVM_Svc
    /* Kernel function system call gate */
    .global             RVM_A6M_Svc_Kfn
/* End Export ****************************************************************/

/* Entry *********************************************************************/
    .section            .text.rvm_entry
    .align              3

    .thumb_func
_RVM_Entry:
    /* Load data section from flash to RAM */
    LDR                 R0,=_RVM_Data_Start
    LDR                 R1,=_RVM_Data_End
    LDR                 R2,=_RVM_Data_Load
_RVM_Data_Load:
    CMP                 R0,R1
    BEQ                 _RVM_Data_Done
    LDR                 R3,[R2]
    STR                 R3,[R0]
    ADDS                R0,#0x04
    ADDS                R2,#0x04
    B                   _RVM_Data_Load
_RVM_Data_Done:
    /* Clear bss zero section */
    LDR                 R0,=_RVM_Zero_Start
    LDR                 R1,=_RVM_Zero_End
    LDR                 R2,=0x00
_RVM_Zero_Clear:
    CMP                 R0,R1
    BEQ                 _RVM_Zero_Done
    STR                 R2,[R0]
    ADDS                R0,#0x04
    B                   _RVM_Zero_Clear
_RVM_Zero_Done:
    LDR                 R0, =main
    BX                  R0
/* End Entry *****************************************************************/

/* Function:_RVM_Stub *********************************************************
Description : The user level stub for thread creation.
Input       : R4 - rvm_ptr_t Entry - The entry address.
              R5 - rvm_ptr_t Stack - The stack address that we are using now.
Output      : None.
Return      : None.
******************************************************************************/
    .section            .text._rvm_stub
    .align              3

    .thumb_func
_RVM_Stub:           
    SUB                 SP,#0x40            /* Protect the stack */
    MOV                 R0,R5
    BX                  R4                  /* Branch to the entry */
/* End Function:_RVM_Stub ****************************************************/

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
    PUSH                {R4-R7}             /* Save registers */
    MOV                 R4,R8
    MOV                 R5,R9
    MOV                 R6,R10
    MOV                 R7,R11
    PUSH                {R4-R7}
    
    MOVS                R4,#0x1             /* RVM_SVC_INV_ACT */
    LSLS                R4,#16
    MOV                 R5,R0
    MOV                 R6,R1               /* Parameter */

    SVC                 #0x00               /* System call */
    ISB

    MOV                 R0,R4               /* System call return value */
    
    CMP                 R2,#0x00            /* Invocation return value*/
    BEQ                 RVM_Inv_Act_Skip
    STR                 R5,[R2]

RVM_Inv_Act_Skip:
    POP                 {R4-R7}             /* Restore registers */
    MOV                 R8,R4
    MOV                 R9,R5
    MOV                 R10,R6
    MOV                 R11,R7
    POP                 {R4-R7}
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
    MOVS                R4,#0x00            /* RVM_SVC_INV_RET */
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

/* Function:RVM_A6M_Svc_Kfn ***************************************************
Description : Trigger a system call. This is ARMv6-M specific, and does not expand
              to other architectures, and is only used for kernel functions.
              This specially crafted system call allows up to 8 parameters to
              be passed and returned.
Input       : R0 - rvm_ptr_t Num - The system call number/other information.
              R1 - rvm_ptr_t ID - The func ID and sub ID of the kernel function call.
              R2 - rvm_ptr_t Args[6] - Array of 6 arguments.
Output      : R2 - rvm_ptr_t Args[6] - Array of 6 return values.
Return      : R0 - rvm_ret_t - The system call return value.
******************************************************************************/
    .section            .text.rvm_a6m_svc_kfn
    .align              3

    .thumb_func
RVM_A6M_Svc_Kfn:        
    PUSH                {R4-R7}             /* Save registers */
    MOV                 R4,R8
    MOV                 R5,R9
    MOV                 R6,R10
    MOV                 R7,R11
    PUSH                {R4-R7}

    MOV                 R4,R0               /* Pass parameters */
    MOV                 R5,R1
    LDR                 R6,[R2,#0x10]
    LDR                 R7,[R2,#0x14]
    MOV                 R10,R6
    MOV                 R11,R7
    LDR                 R6,[R2,#0x08]
    LDR                 R7,[R2,#0x0C]
    MOV                 R8,R6
    MOV                 R9,R7
    LDR                 R6,[R2,#0x00]
    LDR                 R7,[R2,#0x04]

    SVC                 #0x00               /* System call */
    ISB

    MOV                 R0,R4               /* System call return value */
    STR                 R6,[R2,#0x00]       /* Extra return values */
    STR                 R7,[R2,#0x04]
    MOV                 R6,R8
    MOV                 R7,R9
    STR                 R6,[R2,#0x08]
    STR                 R7,[R2,#0x0C]
    MOV                 R6,R10
    MOV                 R7,R11
    STR                 R6,[R2,#0x10]
    STR                 R7,[R2,#0x14]

    POP                 {R4-R7}             /* Restore registers */
    MOV                 R8,R4
    MOV                 R9,R5
    MOV                 R10,R6
    MOV                 R11,R7
    POP                 {R4-R7}
    BX                  LR
/* End Function:RVM_A6M_Svc_Kfn **********************************************/
    .end
/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
