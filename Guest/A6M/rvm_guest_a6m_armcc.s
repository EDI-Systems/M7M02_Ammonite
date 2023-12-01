;/*****************************************************************************
;Filename    : rvm_guest_a6m_asm.s
;Author      : pry
;Date        : 09/02/2018
;Description : The assembly portion of the RVM guest library for ARMv6-M.
;*****************************************************************************/

;/* Import *******************************************************************/
    IMPORT              __main
;/* End Import ***************************************************************/

;/* Export *******************************************************************/
    ;Entry point
    EXPORT              _RVM_Entry
    ;Jump stub
    EXPORT              _RVM_Stub
    ;Triggering an invocation
    EXPORT              RVM_Inv_Act
    ;Returning from an invocation
    EXPORT              RVM_Inv_Ret
    ;System call gate
    EXPORT              RVM_Svc
    ;Kernel function system call gate
    EXPORT              RVM_A6M_Svc_Kfn
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

;/* Function:_RVM_Stub ********************************************************
;Description : The user level stub for thread creation.
;Input       : R4 - rvm_ptr_t Entry - The entry address.
;              R5 - rvm_ptr_t Stack - The stack address that we are using now.
;Output      : None.
;Return      : None.
;*****************************************************************************/
    AREA                _RVM_STUB,CODE,READONLY,ALIGN=3
    THUMB
    REQUIRE8
    PRESERVE8

_RVM_Stub               PROC
    SUB                 SP,#0x40            ;Protect the stack
    MOV                 R0,R5
    BX                  R4                  ;Branch to the entry
    ENDP
;/* End Function:_RVM_Stub ***************************************************/

;/* Function:RVM_Inv_Act ******************************************************
;Description : Activate an invocation. If the return value is not desired, pass
;              0 into R2. This is a default implementation that saves all general
;              purpose registers.
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
    PUSH                {R4-R7}             ;Save registers
    MOV                 R4,R8
    MOV                 R5,R9
    MOV                 R6,R10
    MOV                 R7,R11
    PUSH                {R4-R7}

    MOVS                R4,#0x1             ;RVM_SVC_INV_ACT
    LSLS                R4,#16
    ORRS                R4,R0
    MOV                 R5,R1               ;Parameter
                
    SVC                 #0x00               ;System call
    ISB

    MOV                 R0,R4               ;System call return value
    
    CMP                 R2,#0x00            ;Invocation return value
    BEQ                 RVM_Inv_Act_Skip
    STR                 R5, [R2]

RVM_Inv_Act_Skip
    POP                 {R4-R7}             ;Restore registers
    MOV                 R8,R4
    MOV                 R9,R5
    MOV                 R10,R6
    MOV                 R11,R7
    POP                 {R4-R7}
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
    MOVS                R4,#0x00            ;RVM_SVC_INV_RET
    MOV                 R5,R0               ;Return value
                
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

;/* Function:RVM_A6M_Svc_Kfn **************************************************
;Description : Trigger a system call. This is ARMv7-M specific, and does not expand
;              to other architectures, and is only used for kernel functions.
;              This specially crafted system call allows up to 8 parameters to
;              be passed and returned. This number may be different in your system.
;Input       : R0 - rvm_ptr_t Num - The system call number/other information.
;              R1 - rvm_ptr_t ID - The func ID and sub ID of the kernel function call.
;              R2 - rvm_ptr_t Param[6] - Array of 6 parameters.
;Output      : R2 - rvm_ptr_t Param[6] - Array of 6 return values.
;Return      : R0 - rvm_ret_t - The system call return value.
;*****************************************************************************/
    AREA                RVM_A6M_SVC_KFN,CODE,READONLY,ALIGN=3
    THUMB
    REQUIRE8
    PRESERVE8

RVM_A6M_Svc_Kfn         PROC
    PUSH                {R4-R7}             ;Save registers
    MOV                 R4,R8
    MOV                 R5,R9
    MOV                 R6,R10
    MOV                 R7,R11
    PUSH                {R4-R7}

    MOV                 R4,R0               ;Pass the parameters
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

    SVC                 #0x00               ;System call
    ISB

    MOV                 R0,R4               ;System call return value
    STR                 R6,[R2,#0x00]       ;Extra return values
    STR                 R7,[R2,#0x04]
    MOV                 R6,R8
    MOV                 R7,R9
    STR                 R6,[R2,#0x08]
    STR                 R7,[R2,#0x0C]
    MOV                 R6,R10
    MOV                 R7,R11
    STR                 R6,[R2,#0x10]
    STR                 R7,[R2,#0x14]
    
    POP                 {R4-R7}             ;Restore registers
    MOV                 R8,R4
    MOV                 R9,R5
    MOV                 R10,R6
    MOV                 R11,R7
    POP                 {R4-R7}
    BX                  LR
    ENDP
;/* End Function:RVM_A6M_Svc_Kfn *********************************************/
    END
;/* End Of File **************************************************************/

;/* Copyright (C) Evo-Devo Instrum. All rights reserved **********************/

