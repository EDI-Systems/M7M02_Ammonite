/******************************************************************************
Filename    : rvm_platform_cmx_asm_gcc.S
Author      : pry
Date        : 19/01/2017
Description : The Cortex-M user-level assembly scheduling support of the RVM
              hypervisor. This file is intended to be used with gcc.
******************************************************************************/

/* Begin Header **************************************************************/
    .section            .text
    .arch               armv7-m
    .thumb_func
    .syntax             unified
    .align              3
/* End Header ****************************************************************/

/* Begin Exports *************************************************************/
    /* User entry stub */
    .global             _RVM_Entry
    /* User level stub for thread creation and synchronous invocation */
    .global             _RVM_Jmp_Stub
    /* Triggering an invocation */
    .global             RVM_Inv_Act
    /* Returning from an invocation */
    .global             RVM_Inv_Ret
    /* System call gate */
    .global             RVM_Svc
    /*Kernel function system call gate*/
    .global        RVM_A7M_Svc_Kfn
    /* Get the MSB in a word */
    .global             _RVM_MSB_Get
/* End Exports ***************************************************************/

/* Begin Imports *************************************************************/
    /* The main function. */
    .global             main
/* End Imports ***************************************************************/

/* Begin Function:_RVM_Entry **************************************************
Description : The entry of the process.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
    .thumb_func
_RVM_Entry:
    /* Initialize the data section */
    LDR                 R0,=__data_start__
    LDR                 R1,=__data_end__
    LDR                 R2,=_sidata
load_data:
    CMP                 R0,R1
    BEQ                 load_done
    LDR                 R3,[R2]
    STR                 R3,[R0]
    ADD                 R0,#0x04
    ADD                 R2,#0x04
    B                   load_data
load_done:
    /* Initialize the bss section */
    LDR                 R0,=__bss_start__
    LDR                 R1,=__bss_end__
    LDR                 R2,=0x00
clear_bss:
    CMP                 R0,R1
    BEQ                 clear_done
    STR                 R2,[R0]
    ADD                 R0,#0x04
    B                   clear_bss
clear_done:
    LDR                 R0, =main
    BX                  R0
/* End Function:_RVM_Entry ***************************************************/

/* Begin Function:_RVM_Jmp_Stub ***********************************************
Description : The user level stub for thread creation.
Input       : R4 - rvm_ptr_t Entry - The entry address.
              R5 - rvm_ptr_t Stack - The stack address that we are using now.
Output      : None.
Return      : None.
******************************************************************************/
    .thumb_func
_RVM_Jmp_Stub:
    SUB                 SP,#0x40            /* In order not to destroy the stack */
    MOV                 R0,R5
    BLX                 R4                  /* Branch to the actual entry address */
/* End Function:_RVM_Jmp_Stub ************************************************/

/* Begin Function:RVM_Inv_Act ************************************************
Description : Activate an invocation. If the return value is not desired, pass
              0 into R2. This is a default implementation that saves all general
              purpose registers and doesn't save FPU context. If you need a faster
              version, consider inline functions; if you need to save FPU contexts,
              please DIY.
Input       : R0 - rvm_cid_t Cap_Inv - The capability slot to the invocation stub. 2-Level.
              R1 - rvm_ptr_t Param - The parameter for the call.
Output      : R2 - rvm_ptr_t* Retval - The return value from the call.
Return      : R0 - rvm_ptr_t - The return value of the system call itself.
*****************************************************************************/
    .thumb_func
RVM_Inv_Act:
    PUSH                {R4-R11}            /* User-level is responsible for all clobbering */
    
    MOV                 R4,#0x10000         /* RVM_SVC_INV_ACT */
    ORR                 R4,R0
    MOV                 R5,R1               /* Parameter */
                
    SVC                 #0x00               /* System call */
    ISB                                     /* Instruction barrier - wait for instruction to complete */
                
    MOV                 R0,R4               /* This is the return value of the system call itself */
                
    CMP                 R2,#0x00            /* See if this return value is desired */
    IT                  NE
    STRNE               R5,[R2]             /* This is the return value of the invocation */
                
    POP                 {R4-R11}            /* Manual recovering */
    BX                  LR                  /* Return from the call */
/* End Function:RVM_Inv_Act *************************************************/

/* Begin Function:RVM_Inv_Ret ************************************************
Description : Manually return from an invocation, and set the return value to
              the old register set. This function does not need a capability
              table to work, and never returns.
Input       : R0 - The returning result from the invocation.
Output      : None.
Return      : None.
*****************************************************************************/
    .thumb_func
RVM_Inv_Ret:
    MOV                 R4,#0x00            /* RVM_SVC_INV_RET */
    MOV                 R5,R0               /* Set return value to the register */
                
    SVC                 #0x00               /* System call */
    ISB                                     /* Instruction barrier - wait for instruction to complete */
                
    B                   .                   /* Shouldn't reach here. */
/* End Function:RVM_Inv_Ret *************************************************/

/* Begin Function:RVM_Svc ****************************************************
Description : Trigger a system call.
Input       : R0 - rvm_ptr_t Num - The system call number/other information.
              R1 - rvm_ptr_t Param1 - Argument 1.
              R2 - rvm_ptr_t Param2 - Argument 2.
              R3 - rvm_ptr_t Param3 - Argument 3.
Output      : None.
Return      : None.
*****************************************************************************/
    .thumb_func
RVM_Svc:
    PUSH                {R4-R7}             /* Manual clobbering */
    MOV                 R4,R0               /* Manually pass the parameters according to ARM calling convention */
    MOV                 R5,R1               /* Pass parameters */
    MOV                 R6,R2
    MOV                 R7,R3
                
    SVC                 #0x00               /* System call */
    ISB                                     /* Instruction barrier - wait for instruction to complete */
                
    MOV                 R0,R4               /* This is the return value */
    POP                 {R4-R7}             /* Manual recovering */
    BX                  LR                  /* Return from the call */
                
    B                   .                   /* Shouldn't reach here */
/* End Function:RVM_Svc *****************************************************/

/* Begin Function:_RVM_MSB_Get ***********************************************
Description : Get the MSB of the word.
Input       : R0 - rvm_ptr_t Val - The value.
Output      : None.
Return      : R0 - rvm_ptr_t - The MSB position.
*****************************************************************************/
    .thumb_func
_RVM_MSB_Get:
    CLZ                 R1,R0
    MOV                 R0,#31
    SUB                 R0,R1
    BX                  LR
/* End Function:_RVM_MSB_Get ************************************************/

/* Begin Function:RVM_A7M_Svc_Kfn ********************************************
Description : Trigger a system call. This is ARMv7-M specific, and does not expand
              to other architectures, and is only used for kernel functions.
              This specially crafted system call allows up to 8 parameters to
              be passed and returned.
Input       : R0 - rvm_ptr_t Num - The system call number/other information.
              R1 - rvm_ptr_t ID - The func ID and sub ID of the kernel function call.
              R2 - rvm_ptr_t Args[6] - Array of 6 arguments.
Output      : R2 - rvm_ptr_t Args[6] - Array of 6 return values.
Return      : R0 - rvm_ret_t - The system call return value.*****************************************************************************/
    .thumb_func
RVM_A7M_Svc_Kfn:        
                        PUSH                {R4-R12}            //; Save registers
                        MOV                 R4, R0              //; Pass parameters
                        MOV                 R5, R1
                        MOV                 R12, R2             //; Pass extra parameters
                        LDR                 R6, [R12,#0x00]
                        LDR                 R7, [R12,#0x04]
                        LDR                 R8, [R12,#0x08]
                        LDR                 R9, [R12,#0x0C]
                        LDR                 R10, [R12,#0x10]
                        LDR                 R11, [R12,#0x14]
                                    
                        SVC                 #0x00               //; System call
                        ISB
                                    
                        MOV                 R0, R4              //; System call return value
                        STR                 R6, [R12,#0x00]     //; Extra return values
                        STR                 R7, [R12,#0x04]
                        STR                 R8, [R12,#0x08]
                        STR                 R9, [R12,#0x0C]
                        STR                 R10, [R12,#0x10]
                        STR                 R11, [R12,#0x14]
                        
                        POP                 {R4-R12}             //; Restore registers
                        BX                  LR
                        
/* End Function:RVM_A7M_Svc_Kfn *********************************************/

    .end
/* End Of File **************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved **********************/

