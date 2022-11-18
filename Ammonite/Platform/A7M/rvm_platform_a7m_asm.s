;/*****************************************************************************
;Filename    : rvm_platform_a7m_asm.s
;Author      : pry
;Date        : 19/01/2017
;Description : The Cortex-M user-level assembly scheduling support of the RVM hypervisor.
;*****************************************************************************/
                
;/* Begin Header *************************************************************/
    ;2^3=8 Byte alignment
    AREA                STACK, NOINIT, READWRITE, ALIGN=3
    SPACE               0x00000100
__initial_sp

    AREA                ARCH,CODE,READONLY,ALIGN=3
    THUMB
    REQUIRE8
    PRESERVE8
;/* End Header ***************************************************************/

;/* Begin Exports ************************************************************/
    ;Required by keil
    EXPORT              __initial_sp
    ;User entry stub
    EXPORT              _RVM_Entry
    ;User level stub for thread creation and synchronous invocation
    EXPORT              _RVM_Jmp_Stub
    ;Triggering an invocation
    EXPORT              RVM_Inv_Act
    ;Returning from an invocation
    EXPORT              RVM_Inv_Ret
    ;Receive thread scheduler notification
    EXPORT              RVM_Thd_Sched_Rcv
    ;System call gate
    EXPORT              RVM_Svc
    ;Kernel function system call gate
    EXPORT              RVM_A7M_Svc_Kern
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
;Return      : None.
;*****************************************************************************/
_RVM_Entry
    LDR                 R0, =__main
    BX                  R0
;/* End Function:_RVM_Entry **************************************************/

;/* Begin Function:_RVM_Jmp_Stub **********************************************
;Description : The user level stub for thread/invocation creation.
;Input       : R4 - rvm_ptr_t Entry - The entry address.
;              R5 - rvm_ptr_t Param - The parameter to be passed to it.
;Output      : None.
;Return      : None.
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
;Input       : R0 - rvm_cid_t Cap_Inv - The capability slot to the invocation stub. 2-Level.
;              R1 - rvm_ptr_t Param - The parameter for the call.
;Output      : R2 - rvm_ptr_t* Retval - The return value from the call.
;Return      : R0 - rvm_ptr_t - The return value of the system call itself.
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
    
    MOV                 R0,R4               ; If we reach here, then return failed
    BX                  LR
;/* End Function:RVM_Inv_Ret *************************************************/

;/* Begin Function:RVM_Thd_Sched_Rcv ******************************************
;Description : Try to receive a notification from the scheduler queue. This
;              can only be called from the same core the thread is on.
;Input       : R0- rvm_cid_t Cap_Thd - The capability to the scheduler thread. 
;                                      We are going to get timeout or fault
;                                      notifications for the threads that it is 
;                                      responsible for scheduling. This capability
;                                      must point to a thread on the same core.
;                                      2-Level.
;Output      : R1 - rvm_ptr_t* Fault - The return value from the call.
;Return      : R0 - rvm_ret_t - If successful, the thread ID; or an error code.
;*****************************************************************************/
RVM_Thd_Sched_Rcv
    PUSH                {R4-R5}             ; Manual clobbering
    MOV                 R4,#0x1D0000        ; RVM_SVC_THD_SCHED_RCV
    MOV                 R5,R0               ; Parameter
                
    SVC                 #0x00               ; System call
    ISB                                     ; Instruction barrier - wait for instruction to complete
                
    MOV                 R0,R4               ; This is the return value of the system call itself
    
    CMP                 R1,#0x00            ; See if this return value is desired.
    IT                  NE
    STRNE               R5,[R1]             ; This is the reason of the fault
    
    POP                 {R4-R5}             ; Manual recovering
    BX                  LR                  ; Return from the call
;/* End Function:RVM_Thd_Sched_Rcv *******************************************/

;/* Begin Function:RVM_Svc ****************************************************
;Description : Trigger a system call.
;Input       : R0 - rvm_ptr_t Num - The system call number/other information.
;              R1 - rvm_ptr_t Param1 - Argument 1.
;              R2 - rvm_ptr_t Param2 - Argument 2.
;              R3 - rvm_ptr_t Param3 - Argument 3.
;Output      : None.
;Return      : R0 - rvm_ret_t - The system call return value.
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
;/* End Function:RVM_Svc *****************************************************/

;/* Begin Function:RVM_A7M_Svc_Kern *******************************************
;Description : Trigger a system call. This is ARMv7-M specific, and does not expand
;              to other architectures, and is only used for kernel functions.
;              This specially crafted system call allows up to 8 parameters to
;              be passed and returned.
;Input       : R0 - rvm_ptr_t Num - The system call number/other information.
;              R1 - rvm_ptr_t ID - The func ID and sub ID of the kernel function call.
;              R2 - rvm_ptr_t Args[6] - Array of 6 arguments.
;Output      : R2 - rvm_ptr_t Args[6] - Array of 6 return values.
;Return      : R0 - rvm_ret_t - The system call return value.
;*****************************************************************************/
RVM_A7M_Svc_Kern
    PUSH                {R4-R12}            ; Manual clobbering
    MOV                 R4,R0               ; Manually pass the parameters according to ARM calling convention
    MOV                 R5,R1
    MOV                 R12,R2              ; Pass parameters
    LDR                 R6,[R12,#0x00]
    LDR                 R7,[R12,#0x04]
    LDR                 R8,[R12,#0x08]
    LDR                 R9,[R12,#0x0C]
    LDR                 R10,[R12,#0x10]
    LDR                 R11,[R12,#0x14]
                
    SVC                 #0x00               ; System call
    ISB                                     ; Instruction barrier - wait for instruction to complete
                
    MOV                 R0,R4               ; This is the return value
    STR                 R6,[R12,#0x00]      ; These are extra return values - R5 is not used here.
    STR                 R7,[R12,#0x04]
    STR                 R8,[R12,#0x08]
    STR                 R9,[R12,#0x0C]
    STR                 R10,[R12,#0x10]
    STR                 R11,[R12,#0x14]
    
    POP                 {R4-R12}             ; Manual recovering
    BX                  LR                   ; Return from the call
;/* End Function:RVM_A7M_Svc_Kern ********************************************/

;/* Begin Function:_RVM_MSB_Get ***********************************************
;Description : Get the MSB of the word.
;Input       : R0 - rvm_ptr_t Val - The value.
;Output      : None.
;Return      : R0 - rvm_ptr_t - The MSB position.
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
;Return      : None.
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
