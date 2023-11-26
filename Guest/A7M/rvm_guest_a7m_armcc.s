;/*****************************************************************************
;Filename    : rvm_guest_a7m_asm.s
;Author      : pry
;Date        : 09/02/2018
;Description : The assembly portion of the RVM guest library for ARMv7-M.
;*****************************************************************************/

;/* Import *******************************************************************/
    IMPORT              __main
;/* End Import ***************************************************************/

;/* Export *******************************************************************/
    ;Entry point
    EXPORT              _RVM_Entry
    ;The jump stub and entry stub
    EXPORT              _RVM_Jmp_Stub
    ;Triggering an invocation
    EXPORT              RVM_Inv_Act
    ;Returning from an invocation
    EXPORT              RVM_Inv_Ret
    ;System call gate
    EXPORT              RVM_Svc
    ;Kernel function system call gate
    EXPORT              RVM_A7M_Svc_Kfn
;/* End Export ***************************************************************/

;/* Entry ********************************************************************/
    AREA                RVM_ENTRY,CODE,READONLY,ALIGN=3
    THUMB
    REQUIRE8
    PRESERVE8

_RVM_Entry              PROC
    LDR                 R0,=__main
    BX                  R0
    ENDP
;/* End Entry ****************************************************************/

;/* Function:_RVM_Jmp_Stub ****************************************************
;Description : The user level stub for thread creation.
;Input       : R4 - rvm_ptr_t Entry - The entry address.
;              R5 - rvm_ptr_t Stack - The stack address that we are using now.
;Output      : None.
;Return      : None.
;*****************************************************************************/
    AREA                _RVM_JMP_STUB,CODE,READONLY,ALIGN=3
    THUMB
    REQUIRE8
    PRESERVE8

_RVM_Jmp_Stub           PROC
    SUB                 SP,#0x40            ;Protect the stack
    MOV                 R0,R5
    BX                  R4                  ;Branch to the entry
    ENDP
;/* End Function:_RVM_Jmp_Stub ***********************************************/

;/* Function:RVM_Inv_Act ******************************************************
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
    AREA                RVM_INV_ACT,CODE,READONLY,ALIGN=3
    THUMB
    REQUIRE8
    PRESERVE8

RVM_Inv_Act             PROC
    PUSH                {R4-R11}            ;Save registers
    
    MOV                 R4,#0x10000         ;RVM_SVC_INV_ACT
    ORR                 R4,R0
    MOV                 R5,R1               ;Parameter
                
    SVC                 #0x00               ;System call
    ISB
                
    MOV                 R0,R4               ;System call return value
    CMP                 R2,#0x00            ;Invocation return value
    IT                  NE
    STRNE               R5,[R2]
                
    POP                 {R4-R11}            ;Restore registers
    BX                  LR
    ENDP
;/* End Function:RVM_Inv_Act *************************************************/

;/* Function:RVM_Inv_Ret ******************************************************
;Description : Manually return from an invocation, and set the return value to
;              the old register set. This function does not need a capability
;              table to work, and never returns.
;Input       : R0 - The returning result from the invocation.
;Output      : None.
;Return      : None.
;*****************************************************************************/
    AREA                RVM_INV_RET,CODE,READONLY,ALIGN=3
    THUMB
    REQUIRE8
    PRESERVE8

RVM_Inv_Ret             PROC
    MOV                 R4,#0x00            ;RVM_SVC_INV_RET
    MOV                 R5,R0               ;Set return value
                
    SVC                 #0x00               ;System call
    ISB
    
    MOV                 R0,R4               ;If we reach here, then return failed
    BX                  LR
    ENDP
;/* End Function:RVM_Inv_Ret *************************************************/

;/* Function:RVM_Svc **********************************************************
;Description : Trigger a system call.
;Input       : R0 - rvm_ptr_t Num - The system call number/other information.
;              R1 - rvm_ptr_t Param1 - Argument 1.
;              R2 - rvm_ptr_t Param2 - Argument 2.
;              R3 - rvm_ptr_t Param3 - Argument 3.
;Output      : None.
;Return      : None.
;*****************************************************************************/
    AREA                RVM_SVC,CODE,READONLY,ALIGN=3
    THUMB
    REQUIRE8
    PRESERVE8

RVM_Svc                 PROC
    PUSH                {R4-R7}             ;Save registers
    MOV                 R4,R0               ;Pass parameters
    MOV                 R5,R1
    MOV                 R6,R2
    MOV                 R7,R3
                
    SVC                 #0x00               ;System call
    ISB
                
    MOV                 R0,R4               ;Return value
    POP                 {R4-R7}             ;Restore registers
    BX                  LR
    ENDP
;/* End Function:RVM_Svc *****************************************************/

;/* Function:RVM_A7M_Svc_Kfn **************************************************
;Description : Trigger a system call. This is ARMv7-M specific, and does not expand
;              to other architectures, and is only used for kernel functions.
;              This specially crafted system call allows up to 8 parameters to
;              be passed and returned. This number may be different in your system.
;Input       : R0 - rvm_ptr_t Num - The system call number/other information.
;              R1 - rvm_ptr_t ID - The func ID and sub ID of the kernel function call.
;              R2 - rvm_ptr_t Args[6] - Array of 6 arguments.
;Output      : R2 - rvm_ptr_t Args[6] - Array of 6 return values.
;Return      : R0 - rvm_ret_t - The system call return value.
;*****************************************************************************/
    AREA                RVM_A7M_SVC_KFN,CODE,READONLY,ALIGN=3
    THUMB
    REQUIRE8
    PRESERVE8

RVM_A7M_Svc_Kfn         PROC
    PUSH                {R4-R12}            ;Save registers
    MOV                 R4,R0               ;Pass parameters
    MOV                 R5,R1
    MOV                 R12,R2              ;Pass extra parameters
    LDR                 R6,[R12,#0x00]
    LDR                 R7,[R12,#0x04]
    LDR                 R8,[R12,#0x08]
    LDR                 R9,[R12,#0x0C]
    LDR                 R10,[R12,#0x10]
    LDR                 R11,[R12,#0x14]
                
    SVC                 #0x00               ;System call
    ISB
                
    MOV                 R0,R4               ;System call return value
    STR                 R6,[R12,#0x00]      ;Extra return values
    STR                 R7,[R12,#0x04]
    STR                 R8,[R12,#0x08]
    STR                 R9,[R12,#0x0C]
    STR                 R10,[R12,#0x10]
    STR                 R11,[R12,#0x14]
    
    POP                 {R4-R12}            ;Restore registers
    BX                  LR
    ENDP
;/* End Function:RVM_A7M_Svc_Kfn *********************************************/
    ALIGN
    END
;/* End Of File **************************************************************/

;/* Copyright (C) Evo-Devo Instrum. All rights reserved **********************/
