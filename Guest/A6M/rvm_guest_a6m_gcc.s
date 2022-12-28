/******************************************************************************
Filename    : rvm_guest_cmx_asm_gcc.S
Author      : pry
Date        : 09/02/2018
Description : The assembly portion of the RVM guest library for Cortex-M3/4/7.
              This file is for use with gcc.
******************************************************************************/

/* Begin Header **************************************************************/
    .syntax             unified
    .thumb

	.section            ".noinit"
RVM_User_Stack:
	.space              0x00000100
__initial_sp:

RVM_Int_Stack:
	.space				0x00000400

    .section            ".text"
    .align              3
/* End Header ****************************************************************/

/* Begin Exports *************************************************************/
    /* The user stack pointer position */
    .global             RVM_User_Stack
    /* The interrupt stack pointer position */
    .global             RVM_Int_Stack
    /* The interrupt handling routine receive endpoint */ 
    .global             RVM_Int_Rcv
    /* Get the MSB in the word */
    .global             RVM_MSB_Get  
    /* Yield the processor, trigger the PendSV */
    .global             _RVM_Yield  
    /* The atomic AND/OR operation */
    .global             RVM_Fetch_And
    .global             RVM_Fetch_Or
    /* The hypercall maker */
    .global             _RVM_Hypercall
    /* The entry of the RVM */
    .global             _RVM_Entry
    /* The kernel function calling stub */
    .global             _RVM_Kern
/* End Exports ***************************************************************/

/* Begin Imports *************************************************************/
    /* The RVM image header */
    .global             RVM_Img
    /* The real entry */
    .global             main
/* End Imports ***************************************************************/

/* Begin Global Variable *****************************************************/
    .long               RVM_Img
/* End Global Variable *******************************************************/

/* Begin Function:RVM_Entry ***************************************************
Description : The entry of the program, when running in an virtualized environment.
              This is also responsible for initializing the system.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
	/* Addresses of variable sections */
	.global  			_sidata
	.global  			__data_start__
	.global  			__data_end__
	.global  			__bss_start__
	.global  			__bss_end__
	.thumb_func
_RVM_Entry:
  	LDR					SP, =_estack
    /* Initialize the data section */
    LDR                 R0,=__data_start__
    LDR                 R1,=__data_end__
    LDR                 R2,=_sidata
load_data:
    CMP                 R0,R1
    BEQ                 load_done
    LDR					R3,[R2]
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
	/* Call static constructors */
    BL 					__libc_init_array
	/* Call the application's entry point.*/
    LDR                 R0, =main
    BX                  R0
/* End Function:_RMP_Entry ***************************************************/

/* Begin Function:RVM_MSB_Get *************************************************
Description : Get the MSB of the word.
Input       : rvm_ptr_t Val - The value.
Output      : None.
Return      : rvm_ptr_t - The MSB position.   
******************************************************************************/
	.thumb_func
RVM_MSB_Get:
    CLZ                 R1,R0
    MOV                 R0,#31
    SUB                 R0,R1
    BX                  LR
/* End Function:RVM_MSB_Get **************************************************/

/* Begin Function:RVM_Fetch_And ***********************************************
Description : The fetch-and-logic-and atomic instruction. Logic AND the pointer
              value with the operand, and return the value before logic AND.
Input       : rvm_ptr_t* Ptr - The pointer to the data.
              rvm_ptr_t Operand - The number to logic AND with the destination.
Output      : rvm_ptr_t* Ptr - The pointer to the data.
Return      : rvm_ptr_t - The value before the AND operation.
******************************************************************************/
	.thumb_func
RVM_Fetch_And:
    PUSH                {R4}
RVM_Fetch_And_Fail:
    LDREX               R2,[R0]             /* Load exclusive */
    AND                 R3,R2,R1            /* AND operation */
    STREX               R4,R3,[R0]          /* Store exclusive */
    CMP                 R4,#0x00            /* See if we succeeded */
    BNE                 RVM_Fetch_And_Fail  /* Go back if we fail */
    MOV                 R0,R2
    POP                 {R4}
    BX                  LR
/* End Function:RVM_Fetch_And ************************************************/

/* Begin Function:RVM_Fetch_Or ************************************************
Description : The fetch-and-logic-or atomic instruction. Logic OR the pointer
              value with the operand, and return the value before logic OR.
Input       : rvm_ptr_t* Ptr - The pointer to the data.
              rvm_ptr_t Operand - The number to logic AND with the destination.
Output      : rvm_ptr_t* Ptr - The pointer to the data.
Return      : rvm_ptr_t - The value before the OR operation.
******************************************************************************/
	.thumb_func
RVM_Fetch_Or:
    PUSH                {R4}
RVM_Fetch_Or_Fail:
    LDREX               R2,[R0]             /* Load exclusive */
    ORR                 R3,R2,R1            /* OR operation */
    STREX               R4,R3,[R0]          /* Store exclusive */
    CMP                 R4,#0x00            /* See if we succeeded */
    BNE                 RVM_Fetch_Or_Fail   /* Go back if we fail */
    MOV                 R0,R2
    POP                 {R4}
    BX                  LR
/* End Function:RVM_Fetch_Or *************************************************/

/* Begin Function:_RVM_Hypercall **********************************************
Description : The stub to make a hypercall. This is actually a RME system call(asnd).
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
	.thumb_func
_RVM_Hypercall:
    PUSH                {R4-R5}             /* Manual clobbering */
    MOV                 R4,#0x20000         /* Asynchronous send */
    MOV                 R5,#0               /* To capability number 0. */
                
    SVC                 #0x00               /* System call */
    ISB                                     /* Instruction barrier - wait for instruction to complete. */
                
    POP                 {R4-R5}             /* Manual recovering */
    BX                  LR                  /* Return from the call */
/* End Function:_RVM_Hypercall ***********************************************/

/* Begin Function:RVM_Int_Rcv *************************************************
Description : Cause the interrupt handler routine to wait on the interrupt endpoint.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
	.thumb_func
RVM_Int_Rcv:
    PUSH                {R4-R6}             /* Manual clobbering */
    MOV                 R4,#0x30000         /* Asynchronous receive */
    MOV                 R5,#1               /* From capability number 1. */
    MOV                 R6,#1               /* With blocking receive multi. */
                
    SVC                 #0x00               /* System call */
    ISB                                     /* Instruction barrier - wait for instruction to complete. */
                
    POP                 {R4-R6}             /* Manual recovering */
    BX                  LR                  /* Return from the call */
/* End Function:RVM_Int_Rcv **************************************************/

/* Begin Function:_RVM_Yield **************************************************
Description : Trigger a yield to another thread. This will actually send a interrupt
              to the interrupt thread.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
	.thumb_func
_RVM_Yield:
    PUSH                {R4-R5}             /* Manual clobbering */
    MOV                 R4,#0x20000         /* Asynchronous send */
    MOV                 R5,#1               /* Capability number */

    SVC                 #0x00               /* System call */
    ISB                                     /* Instruction barrier - wait for instruction to complete. */

    POP                 {R4-R5}             /* Manual recovering */
    BX                  LR                  /* Return from the call */
/* End Function:_RVM_Yield ***************************************************/

/* Begin Function:_RVM_Kern ***************************************************
Description : Invoke customized kernel functions which is compiled into the RME kernel.
Input       : rvm_cnt_t Cap_ID - The capability ID.
              rvm_ptr_t Func_ID - The function ID. The subfunction ID is also merged with it.
              rvm_ptr_t Param1 - The first parameter.
              rvm_ptr_t Param2 - The sencond parameter.
Output      : None.
Return      : ret_t - If successful, 0; else -1.
******************************************************************************/
	.thumb_func
_RVM_Kern:
    PUSH                {R4-R7}             /* Manual clobbering */
    ADD                 R4,R0,#2            /* Capability ID - plus 2 because the first two are occupied. */
    MOVT                R4,#0x4             /* Kernel function */
    MOV                 R5,R1               /* Parameters */
    MOV                 R6,R2
    MOV                 R7,R3

    SVC                 #0x00               /* System call */
    ISB                                     /* Instruction barrier - wait for instruction to complete. */

    POP                 {R4-R7}             /* Manual recovering */
    BX                  LR                  /* Return from the call */
/* End Function:_RVM_Kern ****************************************************/

    .end
/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
