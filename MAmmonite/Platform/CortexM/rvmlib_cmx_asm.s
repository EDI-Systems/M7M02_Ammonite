;/*****************************************************************************
;Filename    : rvmlib_cmx_asm.s
;Author      : pry
;Date        : 19/01/2017
;Description : The Cortex-M user-level assembly scheduling support of the RVM RTOS.
;*****************************************************************************/
                
;/* Begin Header *************************************************************/
    ;The align is "(2^3)/8=1(Byte)." In fact it does not take effect.
    AREA                ARCH,CODE,READONLY,ALIGN=3
    THUMB
    REQUIRE8
    PRESERVE8
;/* End Header ***************************************************************/

;/* Begin Exports ************************************************************/
    ;User entry stub
    EXPORT              _RVM_Entry
    ;User level stub for thread creation and synchronous invocation
    EXPORT              _RVM_Jmp_Stub
    ;Triggering an invocation
    EXPORT              RVM_Inv_Act
    ;Returning from an invocation
    EXPORT              RVM_Inv_Ret
    ;System call gate
    EXPORT              RVM_Svc
    ;Get the MSB in a word
    EXPORT              _RVM_MSB_Get
    ;Shut the semihosting up
    EXPORT              __user_setup_stackheap
;/* End Exports **************************************************************/

;/* Begin Imports ************************************************************/
    ;The ARM C library entrance. This will do all the dirty init jobs for us.
    IMPORT              __main
;/* End Imports **************************************************************/

;/* Begin Function:_RVM_Entry *************************************************
;Description : The entry of the process.
;Input       : None.
;Output      : None.
;*****************************************************************************/
_RVM_Entry
    LDR                 R0, =__main
    BX                  R0
;/* End Function:_RVM_Entry **************************************************/

;/* Begin Function:_RVM_Jmp_Stub **********************************************
;Description : The user level stub for thread creation.
;Input       : R4 - The entry address.
;              R5 - The stack address that we are using now.
;Output      : None.
;*****************************************************************************/
_RVM_Jmp_Stub
    SUB                 SP,#0x40            ; In order not to destroy the stack
    MOV                 R0,R5
    BLX                 R4                  ; Branch to the actual entry address.
;/* End Function:_RVM_Jmp_Stub ***********************************************/

;/* Begin Function:RVM_Inv_Act ************************************************
;Description : Activate an invocation. If the return value is not desired, pass
;              0 into R2. This is a default implementation that saves all general
;              purpose registers and doesn't save FPU context. If you need a faster
;              version, consider inline functions; if you need to save FPU contexts,
;              please DIY.
;Input       : R0 - cid_t Cap_Inv - The capability slot to the invocation stub. 2-Level.
;              R1 - ptr_t Param - The parameter for the call.
;Output      : R2 - ptr_t* Retval - The return value from the call.
;Return      : R0 - ptr_t - The return value of the system call itself.
;*****************************************************************************/
RVM_Inv_Act
    PUSH                {R4-R11}            ; User-level is responsible for all clobbering
    
    MOV                 R4,#0x10000         ; RVM_SVC_INV_ACT
    ORR                 R4,R0
    MOV                 R5,R1               ; Parameter
                
    SVC                 #0x00               ; System call
    ISB                                     ; Instruction barrier - wait for instruction to complete
                
    MOV                 R0,R4               ; This is the return value of the system call itself
                
    CMP                 R2,#0x00            ; See if this return value is desired.
    IT                  NE
    STRNE               R5,[R2]             ; This is the return value of the invocation
                
    POP                 {R4-R11}            ; Manual recovering
    BX                  LR                  ; Return from the call
;/* End Function:RVM_Inv_Act *************************************************/

;/* Begin Function:RVM_Inv_Ret ************************************************
;Description : Manually return from an invocation, and set the return value to
;              the old register set. This function does not need a capability
;              table to work, and never returns.
;Input       : R0 - The returning result from the invocation.
;Output      : None.
;Return      : None.
;*****************************************************************************/
RVM_Inv_Ret
    MOV                 R4,#0x00            ; RVM_SVC_INV_RET
    MOV                 R5,R0               ; Set return value to the register
                
    SVC                 #0x00               ; System call
    ISB                                     ; Instruction barrier - wait for instruction to complete
                
    B                   .                   ; Shouldn't reach here.
;/* End Function:RVM_Inv_Ret *************************************************/

;/* Begin Function:RVM_Svc ****************************************************
;Description : Trigger a system call.
;Input       : R0 - The system call number/other information.
;              R1 - Argument 1.
;              R2 - Argument 2.
;              R3 - Argument 3.
;Output      : None.                              
;*****************************************************************************/
RVM_Svc
    PUSH                {R4-R7}             ; Manual clobbering
    MOV                 R4,R0               ; Manually pass the parameters according to ARM calling convention
    MOV                 R5,R1               ; Pass parameters
    MOV                 R6,R2
    MOV                 R7,R3
                
    SVC                 #0x00               ; System call
    ISB                                     ; Instruction barrier - wait for instruction to complete
                
    MOV                 R0,R4               ; This is the return value
    POP                 {R4-R7}             ; Manual recovering
    BX                  LR                  ; Return from the call
                
    B                   .                   ; Shouldn't reach here.
;/* End Function:RVM_Svc *****************************************************/

;/* Begin Function:_RVM_MSB_Get ***********************************************
;Description    : Get the MSB of the word.
;Input          : ptr_t Val - The value.
;Output         : None.
;Return         : ptr_t - The MSB position.   
;Register Usage : None. 
;*****************************************************************************/
_RVM_MSB_Get
    CLZ                 R1,R0
    MOV                 R0,#31
    SUB                 R0,R1
    BX                  LR
;/* End Function:_RVM_MSB_Get ************************************************/

;/* Begin Function:__user_setup_stackheap *************************************
;Description : We place the function here to shut the SEMIHOSTING up.
;Input       : None.
;Output      : None.
;*****************************************************************************/
__user_setup_stackheap
    MOV                 R0,R1               ; Stack as where we came in, and definitely no heap
    MOV                 R2,R1
    BX                  LR
;/* End Function:__user_setup_stackheap **************************************/

    ALIGN
    END
;/* End Of File **************************************************************/

;/* Copyright (C) Evo-Devo Instrum. All rights reserved **********************/
