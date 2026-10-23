;/*
; * FreeRTOS V9.0.0 RVM port - Copyright (C) 2026 Evo-Devo Instrum.
; * All rights reserved.
; *
; * This file is NOT part of the FreeRTOS distribution.  It belongs to the
; * FreeRTOS-on-RVM test suite and is released under the Unlicense; refer to
; * the LICENSE file for details.  The FreeRTOS kernel itself is copyright
; * Real Time Engineers Ltd. and is not modified by this file.
; *
; * FreeRTOS is free software; you can redistribute it and/or modify it under
; * the terms of the GNU General Public License (version 2) as published by
; * the FreeRTOS Foundation >>>> AND MODIFIED BY <<<< the FreeRTOS
; * exception.
; *
; * This file implements the assembly part of the porting layer for ARMv6-M
; * systems running as an unprivileged guest of the RVM hypervisor: the
; * fast-path yield routine.  This is the IAR (iasmarm) variant; the macro
; * syntax follows the iasmarm form proven in rmp_platform_a7m_rvm_iccarm.s
; * (name MACRO params ... ENDM, bare parameter references in the body).
; */

;/* Symbols provided by the scheduler and the RVM paravirtualization library */
    EXTERN              vTaskSwitchContext  ;The scheduler
    EXTERN              pxCurrentTCB        ;The TCB of the task to run
    EXTERN              RVM_Usr_Param       ;The user hypercall parameter block
    EXTERN              RVM_Virt_Int_Mask   ;Mask the RVM virtual interrupts
    EXTERN              RVM_Virt_Int_Unmask ;Unmask the RVM virtual interrupts

;/* Entry point implemented in this file, selected by portYIELD() from
; * portmacro.h when configUSE_RVM_OPTIMISED_YIELD is enabled.  When that
; * setting is disabled the yields go through the RVM hypercall instead
; * and this routine is not referenced at all. */
    PUBLIC              vPortYield

;/*-----------------------------------------------------------*/
;/*
; * vPortYield yields the CPU from one task to another without a trap
; * through the hypervisor: the scheduler runs directly on the task stack.
; * The routine masquerades the current context as if it were stacked by
; * an exception entry, so the very same frame can also be recovered by
; * the interrupt driven switches through xPortPendSVHandler() in port.c.
; *
; * The exception frame built here, from the stack pointer up:
; *
; *   Unaligned           Aligned
; *   Reserved
; *   XPSR                XPSR
; *   PC                  PC
; *   LR                  LR
; *   R12                 R12
; *   R3                  R3
; *   R2                  R2
; *   R1                  R1
; *   R0                  R0
; *
; * Caveats:
; * 1. The register stacking may be nested by an interrupt that arrives
; *    before the interrupts are masked.  This is allowed, but must be
; *    taken into account when dimensioning the task stacks.
; * 2. User-level code cannot probe the interrupt state, hence nothing can
; *    be short-circuited before the masking takes effect.
; */

    SECTION             .vportyield:CODE:NOROOT(3)
    THUMB
    REQUIRE8
    PRESERVE8

;/*-----------------------------------------------------------*/
;/* See whether the stack pointer is 8 byte aligned.  TST SP is
; * unpredictable on ARMv6-M, so SP has to go through a register first. */
portALIGN_PUSH  MACRO   LABEL
    MOV                 R0,SP               ;Is SP aligned to 8 bytes?
    LDR                 R1,=0x00000007
    TST                 R0,R1
    BEQ                 LABEL               ;We pushed three words before, thus EQ
    ENDM

;/*-----------------------------------------------------------*/
;/* Stack the remaining part of a basic exception frame of SZ words, with
; * the given XPSR and EXC_RETURN, and the resume point as the PC. */
portEXC_PUSH    MACRO   SZ, XPSR, EXC_RETURN
    LDR                 R0,[SP,#4*2]        ;Load real R0/R1 values pushed at start
    LDR                 R1,[SP]
    SUB                 SP,#4*(SZ-7)        ;Adjust SP to push GP registers
    PUSH                {R0-R3}             ;Push stack frame GP registers
    MOV                 R0,R12
    MOV                 R1,LR
    STR                 R0,[SP,#4*4]
    STR                 R1,[SP,#4*5]
    LDR                 R0,[SP,#4*(SZ-2)]   ;Load real XPSR value pushed at start
    LDR                 R1,=XPSR
    ORRS                R0,R1
    STR                 R0,[SP,#4*7]
    LDR                 R0,=vPortYield_Exit ;Push PC with bit[0] cleared
    LDR                 R1,=0xFFFFFFFE
    ANDS                R0,R1
    STR                 R0,[SP,#4*6]
    LDR                 R1,=EXC_RETURN      ;Make up the EXC_RETURN
    MOV                 LR,R1
    ENDM

;/*-----------------------------------------------------------*/
;/* See whether the stacked XPSR carries the 8 byte alignment padding. */
portALIGN_POP   MACRO   LABEL
    LDR                 R0,[SP,#4*7]        ;Load up XPSR
    LDR                 R1,=0x00000200
    TST                 R0,R1
    BNE                 LABEL
    ENDM

;/*-----------------------------------------------------------*/
;/* Unstack a basic exception frame of SZ words.  The original layout
; * [PAD] XPSR PC LR R12 R3-R0 is inconvenient for manual restoring,
; * because loading PC and SP must be the last step, so it is transformed
; * in place first:
; *   1. With [PAD]:    [PAD] XPSR PC LR R12 R3-R0 -> PC R0 XPSR [EMPTY] R12 R3-R1 LR
; *   2. Without [PAD]: XPSR PC LR R12 R3-R0       -> PC R0 XPSR R12 R3-R1 LR
; * The transformation restores LR first so it cannot be overwritten, pops
; * R0, loads XPSR and PC and rearranges them at the end, pops R1-R3 and
; * LR while skipping the middle, and finally restores XPSR through R0,
; * then R0 and PC.  Nothing is ever placed below the current SP, because
; * a racing interrupt would clobber it. */
portEXC_POP     MACRO   SZ
    LDR                 R0,[SP,#4*4]        ;Restore LR/R12
    LDR                 R1,[SP,#4*5]
    MOV                 R12,R0
    MOV                 LR,R1
    POP                 {R0}                ;Load R0/XPSR/PC into R0/R1/R2
    LDR                 R1,[SP,#4*6]
    LDR                 R3,=0xFFFFFDFF      ;Clear XPSR[9]
    ANDS                R1,R3
    LDR                 R2,[SP,#4*5]
    LDR                 R3,=0x00000001      ;Set PC[0]
    ORRS                R2,R3
    STR                 R1,[SP,#4*(SZ-4)]   ;Rearrange to H-PC-R0-XPSR-L
    STR                 R0,[SP,#4*(SZ-3)]
    STR                 R2,[SP,#4*(SZ-2)]
    POP                 {R1-R3}             ;Pop GP registers
    ADD                 SP,#4*(SZ-7)        ;Skip R12/[PAD]
    POP                 {R0}                ;Pop XPSR through R0
    MSR                 APSR_nzcvq,R0
    POP                 {R0,PC}             ;Pop R0 and PC
    ENDM

;/*-----------------------------------------------------------*/
;/*
; * The fast-path yield entry point - see the header comment above.
; */

vPortYield
    PUSH                {R0}                ;Protect R0, XPSR and R1
    MRS                 R0,XPSR
    PUSH                {R0}
    PUSH                {R1}

    portALIGN_PUSH      vPortYield_Unaligned
vPortYield_Aligned                         ;Originally 8 byte aligned stack
    portEXC_PUSH        8,0x01000000,0xFFFFFFFD
    B                   vPortYield_Stacked
vPortYield_Unaligned                       ;Originally unaligned stack
    portEXC_PUSH        8+1,0x01000200,0xFFFFFFFD

vPortYield_Stacked
    MOV                 R3,R11              ;Push GP registers
    MOV                 R2,R10
    MOV                 R1,R9
    MOV                 R0,R8
    PUSH                {R0-R3,LR}
    PUSH                {R4-R7}

    LDR                 R0,=RVM_Usr_Param   ;Push the hypercall parameters
    LDR                 R0,[R0]
    LDMIA               R0!,{R1-R5}
    PUSH                {R1-R5}

    BL                  RVM_Virt_Int_Mask   ;Disable interrupts

    LDR                 R1,=pxCurrentTCB    ;Load pxCurrentTCB
    LDR                 R1,[R1]
    MOV                 R2,SP               ;Write sp to first TCB member
    STR                 R2,[R1]
    BL                  vTaskSwitchContext  ;Select the highest priority ready task
    LDR                 R1,=pxCurrentTCB    ;Load pxCurrentTCB
    LDR                 R1,[R1]
    LDR                 R1,[R1]             ;Read sp from first TCB member
    MOV                 SP,R1

    BL                  RVM_Virt_Int_Unmask ;Enable interrupts

    LDR                 R0,=RVM_Usr_Param   ;Pop the hypercall parameters
    LDR                 R0,[R0]
    POP                 {R1-R5}
    STMIA               R0!,{R1-R5}

    POP                 {R4-R7}             ;Pop GP registers
    POP                 {R0-R3}
    MOV                 R8,R0
    MOV                 R9,R1
    MOV                 R10,R2
    MOV                 R11,R3
    POP                 {R0}
    MOV                 LR,R0

    portALIGN_POP       vPortYield_PadSkip
vPortYield_NoPad                           ;Frame without alignment padding
    portEXC_POP         8
vPortYield_PadSkip                         ;Frame with alignment padding
    portEXC_POP         8+1

vPortYield_Exit                            ;Exit location
    BX                  LR

    LTORG
    END
