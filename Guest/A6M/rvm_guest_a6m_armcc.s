;/*****************************************************************************
;Filename    : rvm_guest_a6m_asm.s
;Author      : pry
;Date        : 09/02/2018
;Description : The assembly portion of the RVM guest library for ARMv6-M.
;*****************************************************************************/

;/* Begin Header *************************************************************/
                        ; The align is "(2^3)/8=1(Byte)." In fact it does not take effect.            
                        AREA                RESET, CODE, READONLY, ALIGN=3
                                    
                        THUMB
                        REQUIRE8
                        PRESERVE8
;/* End Header ***************************************************************/

;/* Begin Imports ************************************************************/
                        IMPORT              __main
;/* End Imports **************************************************************/

;/* Begin Exports ************************************************************/
                        ; Triggering an invocation
                        EXPORT              RVM_Inv_Act
                        ; Returning from an invocation
                        EXPORT              RVM_Inv_Ret
                        ; System call gate
                        EXPORT              RVM_Svc
                        ; Kernel function system call gate
                        EXPORT              RVM_A6M_Svc_Kfn
                        ; Get the MSB in the word
                        EXPORT              _RVM_MSB_Get
                        ; The jump stub and entry stub
                        EXPORT              _RVM_Jmp_Stub
;/* End Exports **************************************************************/

;/* Entry point **************************************************************/
                        LDR                 R0, =__main
                        BX                  R0
;/* Entry point **************************************************************/

;/* Begin Function:RVM_Inv_Act ************************************************
;Description : Activate an invocation. If the return value is not desired, pass
;              0 into R2. This is a default implementation that saves all general
;              purpose registers.
;Input       : R0 - rvm_cid_t Cap_Inv - The capability slot to the invocation stub. 2-Level.
;              R1 - rvm_ptr_t Param - The parameter for the call.
;Output      : R2 - rvm_ptr_t* Retval - The return value from the call.
;Return      : R0 - rvm_ptr_t - The return value of the system call itself.
;*****************************************************************************/
RVM_Inv_Act             PROC
                        PUSH                {R4-R7}             ; Save registers
                        MOV                 R4, R8
                        MOV                 R5, R9
                        MOV                 R6, R10
                        MOV                 R7, R11
                        PUSH                {R4-R7}
                        
                        MOVS                R4, #0x1            ; RVM_SVC_INV_ACT
                        LSLS                R4, #16
                        ORRS                R4, R0
                        MOV                 R5, R1              ; Parameter
                                    
                        SVC                 #0x00               ; System call
                        ISB
                                    
                        MOV                 R0, R4              ; System call return value
                        
                        CMP                 R2, #0x00           ; Invocation return value
                        BEQ                 SKIP
                        STR                 R5, [R2]
SKIP

                        POP                 {R4-R7}             ; Restore registers
                        MOV                 R8, R4
                        MOV                 R9, R5
                        MOV                 R10, R6
                        MOV                 R11, R7
                        POP                 {R4-R7}
                        BX                  LR
                        ENDP
;/* End Function:RVM_Inv_Act *************************************************/

;/* Begin Function:RVM_Inv_Ret ************************************************
;Description : Manually return from an invocation, and set the return value to
;              the old register set. This function does not need a capability
;              table to work, and never returns.
;Input       : R0 - The returning result from the invocation.
;Output      : None.
;Return      : None.
;*****************************************************************************/
RVM_Inv_Ret             PROC
                        MOVS                R4, #0x00           ; RVM_SVC_INV_RET
                        MOV                 R5, R0              ; Return value
                                    
                        SVC                 #0x00               ; System call
                        ISB
                        
                        MOV                 R0, R4              ; If we reach here, then return failed
                        BX                  LR
                        ENDP
;/* End Function:RVM_Inv_Ret *************************************************/

;/* Begin Function:RVM_Svc ****************************************************
;Description : Trigger a system call.
;Input       : R0 - rvm_ptr_t Num - The system call number/other information.
;              R1 - rvm_ptr_t Param1 - Argument 1.
;              R2 - rvm_ptr_t Param2 - Argument 2.
;              R3 - rvm_ptr_t Param3 - Argument 3.
;Output      : None.
;Return      : None.
;*****************************************************************************/
RVM_Svc                 PROC
                        PUSH                {R4-R7}             ; Save registers
                        MOV                 R4, R0              ; Pass parameters
                        MOV                 R5, R1
                        MOV                 R6, R2
                        MOV                 R7, R3
                                    
                        SVC                 #0x00               ; System call
                        ISB
                                    
                        MOV                 R0, R4              ; Return value
                        POP                 {R4-R7}             ; Restore registers
                        BX                  LR
                        ENDP
;/* End Function:RVM_Svc *****************************************************/

;/* Begin Function:RVM_A6M_Svc_Kfn ********************************************
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
RVM_A6M_Svc_Kfn         PROC
                        PUSH                {R4-R7}             ; Save registers
                        MOV                 R4, R8
                        MOV                 R5, R9
                        MOV                 R6, R10
                        MOV                 R7, R11
                        PUSH                {R4-R7}
                        
                        MOV                 R4, R0              ; Pass the parameters
                        MOV                 R5, R1
                        LDR                 R6, [R2,#0x10]
                        LDR                 R7, [R2,#0x14]
                        MOV                 R10, R6
                        MOV                 R11, R7
                        LDR                 R6, [R2,#0x08]
                        LDR                 R7, [R2,#0x0C]
                        MOV                 R8, R6
                        MOV                 R9, R7
                        LDR                 R6, [R2,#0x00]
                        LDR                 R7, [R2,#0x04]
                                    
                        SVC                 #0x00               ; System call
                        ISB
                                    
                        MOV                 R0, R4              ; System call return value
                        STR                 R6, [R2,#0x00]      ; Extra return values
                        STR                 R7, [R2,#0x04]
                        MOV                 R6, R8
                        MOV                 R7, R9
                        STR                 R6, [R2,#0x08]
                        STR                 R7, [R2,#0x0C]
                        MOV                 R6, R10
                        MOV                 R7, R11
                        STR                 R6, [R2,#0x10]
                        STR                 R7, [R2,#0x14]
                        
                        POP                 {R4-R7}             ; Restore registers
                        MOV                 R8, R4
                        MOV                 R9, R5
                        MOV                 R10, R6
                        MOV                 R11, R7
                        POP                 {R4-R7}
                        BX                  LR
                        ENDP
;/* End Function:RVM_A6M_Svc_Kfn *********************************************/

;/* Begin Function:_RVM_MSB_Get ***********************************************
;Description : Get the MSB of the word.
;Input       : rvm_ptr_t Val - The value.
;Output      : None.
;Return      : rvm_ptr_t - The MSB position.   
;*****************************************************************************/
                        MACRO
$Label                  CHECK_BITS          $BITS
                        LSRS                R2, R1, #$BITS
                        BEQ                 $Label.Skip
                        ADDS                R0, #$BITS
                        MOV                 R1, R2
$Label.Skip
                        MEND

_RVM_MSB_Get
                        ;See if the word passed in is zero. In this case, we return -1.
                        CMP                 R0, #0
                        BEQ                 ZERO
                        MOVS                R1, R0
                        MOVS                R0, #0
HEX                     CHECK_BITS          16
OCT                     CHECK_BITS          8
QUAD                    CHECK_BITS          4
BIN                     CHECK_BITS          2
ONE                     CHECK_BITS          1
                        BX                  LR
ZERO
                        SUBS                R0, #1
                        BX                  LR
;/* End Function:_RVM_MSB_Get ************************************************/

;/* Begin Function:_RVM_Jmp_Stub **********************************************
;Description : The user level stub for thread creation.
;Input       : R4 - rvm_ptr_t Entry - The entry address.
;              R5 - rvm_ptr_t Stack - The stack address that we are using now.
;Output      : None.
;Return      : None.
;*****************************************************************************/
_RVM_Jmp_Stub           PROC
                        SUB                 SP, #0x40           ; Protect the stack
                        MOV                 R0, R5
                        BX                  R4                  ; Branch to the entry
                        ENDP
;/* End Function:_RVM_Jmp_Stub ***********************************************/
                        ALIGN
                        END
;/* End Of File **************************************************************/

;/* Copyright (C) Evo-Devo Instrum. All rights reserved **********************/

