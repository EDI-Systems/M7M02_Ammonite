;/*****************************************************************************
;Filename    : rvm_proc_asm.s
;Author      : The RME project generator.
;Date        : 10/11/2019
;Licence     : LGPL v3+; see COPYING for details.
;Description : The assembly stub file for the process.
;*****************************************************************************/

;/* Begin Header *************************************************************/
    ;The align is "(2^3)/8=1(Byte)." In fact it does not take effect.
    AREA                ARCH,CODE,READONLY,ALIGN=3
    THUMB
    REQUIRE8
    PRESERVE8
;/* End Header ***************************************************************/

;/* Begin Imports ************************************************************/
    IMPORT              __main
    ;Threads
    IMPORT              Thd_Thd1
    ;Invocations
    IMPORT              Inv_Inv1
;/* End Imports **************************************************************/

;/* Begin Exports ************************************************************/
    EXPORT              _RVM_Jmp_Stub
    EXPORT              RVM_Entry
;/* End Exports **************************************************************/

;/* Begin Entry List *********************************************************/
    B                   __main
    SPACE               28
    B                   Inv_Inv1
    SPACE               28
;/* End Entry List ***********************************************************/

;/* Begin Function:_RVM_Jmp_Stub **********************************************
;Description : The user level stub for thread creation.
;Input       : R4 - rvm_ptr_t Entry - The entry address.
;              R5 - rvm_ptr_t Stack - The stack address that we are using now.
;Output      : None.
;Return      : None.
;*****************************************************************************/
_RVM_Jmp_Stub
    SUB                 SP,#0x40            ; In order not to destroy the stack
    MOV                 R0,R5
    BLX                 R4                  ; Branch to the actual entry address.
;/* End Function:_RVM_Jmp_Stub ***********************************************/

;/* Begin Function:RVM_Entry **************************************************
;Description : The jump to a desired position to start initialization of the system,
;              using a certain stack address.
;Input       : rvm_ptr_t R0 - The entry.
;              rvm_ptr_t R1 - The stack base.
;              rvm_ptr_t R2 - The stack length.
;              rvm_ptr_r R3 - The parameter.
;Output      : None.
;Return      : None.
;*****************************************************************************/
RVM_Entry
    ADD                 R1,R1,R2
    SUB                 R1,#0x100
    MOV                 SP,R1
    MOV                 R4,R0
    MOV                 R0,R3
    BX                  R4
;/* End Function:RVM_Entry ***************************************************/
    END
;/* End Of File **************************************************************/

;/* Copyright (C) Evo-Devo Instrum. All rights reserved **********************/


