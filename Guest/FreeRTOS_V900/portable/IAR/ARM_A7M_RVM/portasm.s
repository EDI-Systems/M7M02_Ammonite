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
; * This file implements the assembly part of the porting layer for ARMv7-M
; * systems running as an unprivileged guest of the RVM hypervisor: the
; * fast-path yield routine.  This is the IAR (iasmarm) variant; the macro
; * syntax follows the iasmarm form proven in rmp_platform_a7m_rvm_iccarm.s
; * (name MACRO params ... ENDM, bare parameter references in the body).
; */

;/* Symbols provided by the scheduler, the RVM paravirtualization library
; * and the port itself. */
    EXTERN              vTaskSwitchContext  ;The scheduler
    EXTERN              pxCurrentTCB        ;The TCB of the task to run
    EXTERN              RVM_Usr_Param       ;The user hypercall parameter block
    EXTERN              RVM_Virt_Int_Unmask ;Unmask the RVM virtual interrupts
    EXTERN              vPortYield_Err      ;The dirty ICI/IT trap in port.c

;/* Entry point implemented in this file, called by vPortYield() in port.c
; * when configUSE_RVM_OPTIMISED_YIELD is enabled and the peeked target
; * task holds a clean ICI/IT state.  The caller holds the virtual interrupt
; * mask throughout the peek, and this routine unmasks the interrupts itself
; * once the new stack has been loaded. */
    PUBLIC              vPortYield_Swt

;/*-----------------------------------------------------------*/
;/*
; * vPortYield_Swt switches the CPU from one task to another without a trap
; * through the hypervisor: the scheduler runs directly on the task stack.
; * The routine masquerades the current context as if it were stacked by an
; * exception entry, so the very same frame can also be recovered by the
; * interrupt driven switches through xPortPendSVHandler() in port.c.
; *
; * The exception frame built here, from the stack pointer up:
; *
; *   Basic                  Extended (CONTROL.FPCA set)
; *   XPSR                   [PAD]
; *   PC                     FPSCR
; *   LR                     S15 - S0
; *   R12                    XPSR
; *   R3                     PC
; *   R2                     LR
; *   R1                     R12
; *   R0                     R3 - R0
; *   LR_EXC (0xFFFFFFFD)    LR_EXC (0xFFFFFFED)
; *   S31 - S16
; *   R11 - R4
; *   Hypercall parameters
; *
; * The PC pushed in the frame is the exit label, and the LR is the return
; * address of the caller of vPortYield(), so a task saved here and later
; * resumed through the interrupt path continues exactly where the fast
; * path would have returned to.
; *
; * Caveats:
; * 1. User-level code cannot clear CONTROL.FPCA (this is correct - if it
; *    could, then there would be a serious security loophole) hence all
; *    threads in the system are tainted with the FPU active flag once any
; *    of them touches the FPU, and will carry a full context save/restore.
; *    This is still hugely faster than the slow path through the
; *    hypervisor.
; * 2. The EPSR ICI/IT bits of the frame being unstacked are checked on the
; *    fly: a frame holding interrupted IT block or LDM/STM continuation
; *    state can only be restored by a real exception return, which the
; *    manual unstacking below cannot perform.  Such frames are filtered
; *    out by vPortYield() beforehand, so hitting vPortYield_Err() here
; *    indicates an internal port error.
; */

    SECTION             .vportyield:CODE:NOROOT(3)
    THUMB
    REQUIRE8
    PRESERVE8

;/*-----------------------------------------------------------*/
;/* See whether the stack pointer is 8 byte aligned.  TST SP is
; * unpredictable on ARMv7-M, so SP has to go through a register first. */
portALIGN_PUSH  MACRO   LABEL
    MOV                 R0,SP               ;Is SP aligned to 8 bytes?
    TST                 R0,#0x00000007
    BNE                 LABEL               ;Two words pushed above, thus NE
    ENDM

;/*-----------------------------------------------------------*/
;/* Stack the remaining part of an exception frame of SZ words, with
; * the given XPSR bits and EXC_RETURN, and the exit label as the PC.  The
; * two words pushed at the entry double as the frame's XPSR/PC slots when
; * the frame is basic, and sit just above it otherwise. */
portEXC_PUSH    MACRO   SZ, XPSR, EXC_RETURN
    LDR                 R0,[SP,#4]          ;Load the real R0 value pushed at entry
    IF                  ((SZ-8)!=0)         ;Reserve the frame's upper words
    SUB                 SP,SP,#4*(SZ-8)
    ENDIF
    PUSH                {R0-R3,R12,LR}      ;Push the frame's GP registers
    LDR                 R0,[SP,#4*(SZ-2)]   ;Load the real XPSR value pushed at entry
    LDR                 R1,=XPSR
    ORR                 R0,R1
    STR                 R0,[SP,#4*7]        ;Merge in the flags and the padding mark
    LDR                 R0,=vPortYield_Exit ;Push the resume PC with bit[0] cleared
    AND                 R0,R0,#0xFFFFFFFE
    STR                 R0,[SP,#4*6]
    MOV                 LR,#EXC_RETURN      ;Make up the EXC_RETURN
    ENDM

;/*-----------------------------------------------------------*/
;/* See whether the frame holds an alignment padding word: bit 9 of the
; * stacked XPSR is used to mark it. */
portALIGN_POP   MACRO   LABEL
    LDR                 R0,[SP,#4*7]        ;Load up the XPSR
    TST                 R0,#0x00000200
    BNE                 LABEL
    ENDM

;/*-----------------------------------------------------------*/
;/* Unstack an exception frame of SZ words.  The original sequence is
; * [PAD] [FPU] XPSR PC LR R12 R3-R0, which is not ideal for a manual
; * restoring, because restoring PC and SP simultaneously must be the very
; * last step.  Thus the frame is transformed in place first:
; * 1. With [PAD] or [FPU]: [PAD] [FPU] XPSR PC LR R12 R3-R0 -> PC R0 XPSR [EMPTY] R12 R3-R1 LR
; * 2. Without either:      XPSR PC LR R12 R3-R0             -> PC R0 XPSR R12 R3-R1 LR
; * The transformation restores LR first so it cannot be overwritten, pops
; * R0, loads XPSR and PC and rearranges them at the end, pops R1-R3 and LR
; * while skipping the middle, and finally restores XPSR through R0, then
; * R0 and PC.  Nothing is ever placed below the current SP, because a
; * racing interrupt would clobber it. */
portEXC_POP     MACRO   SZ
    LDR                 LR,[SP,#4*5]        ;Restore the frame's LR
    POP                 {R0}                ;Load R0/XPSR/PC into R0/R1/R2
    LDR                 R1,[SP,#4*6]
    LDR                 R12,=0x0600FC00     ;The EPSR ICI/IT field
    TST                 R1,R12              ;Dirty ICI/IT cannot be restored here
    BNE                 vPortYield_Err      ;Defense in depth - see the header
    AND                 R1,R1,#0xFFFFFDFF   ;Clear XPSR[9]
    LDR                 R2,[SP,#4*5]
    ORR                 R2,R2,#0x00000001   ;Set PC[0]
    STR                 R1,[SP,#4*(SZ-4)]   ;Rearrange to H-PC-R0-XPSR-L
    STR                 R0,[SP,#4*(SZ-3)]
    STR                 R2,[SP,#4*(SZ-2)]
    POP                 {R1-R3,R12}         ;Pop the GP registers
    IF                  ((SZ-8)!=0)         ;Skip the FPU area and the padding
    ADD                 SP,SP,#4*(SZ-8)
    ENDIF
    POP                 {R0}                ;Pop the XPSR through R0
    MSR                 XPSR,R0
    POP                 {R0,PC}             ;Pop R0 and PC
    ENDM

;/*-----------------------------------------------------------*/
;/*
; * The fast-path switch entry point - see the header comment above.
; */

vPortYield_Swt                             ;Entered with the virtual interrupts
                                           ;already masked by vPortYield()
    PUSH                {R0}                ;Protect R0 and XPSR
    MRS                 R0,XPSR
    PUSH                {R0}
    MRS                 R0,CONTROL
    TST                 R0,#0x00000004      ;CONTROL.FPCA - the frame type
    BNE                 vPortYield_Extended

vPortYield_Basic                           ;Basic frame
    portALIGN_PUSH      vPortYield_Basic_Unaligned
vPortYield_Basic_Aligned                   ;Originally 8 byte aligned stack
    portEXC_PUSH        8,0x01000000,0xFFFFFFFD
    B                   vPortYield_Stacked
vPortYield_Basic_Unaligned                 ;Originally unaligned stack
    portEXC_PUSH        8+1,0x01000200,0xFFFFFFFD
    B                   vPortYield_Stacked

vPortYield_Extended                        ;Extended frame
    portALIGN_PUSH      vPortYield_Extended_Unaligned
vPortYield_Extended_Aligned                ;Originally 8 byte aligned stack
    portEXC_PUSH        8+17+1,0x01000000,0xFFFFFFED
    B                   vPortYield_Extended_Saved
vPortYield_Extended_Unaligned              ;Originally unaligned stack
    portEXC_PUSH        8+17+2,0x01000200,0xFFFFFFED
vPortYield_Extended_Saved                  ;Extended frame extra pushing
    ADD                 R0,SP,#4*8          ;Locate the FPU stacking area
    DC16                0xECA0,0x0A10       ;VSTMIA R0!,{S0-S15}
    DC16                0xEEF1,0x1A10       ;VMRS R1,FPSCR
    STR                 R1,[R0]
    DC16                0xED2D,0x8A10       ;VPUSH {S16-S31}

vPortYield_Stacked
    PUSH                {R4-R11,LR}         ;Push GP registers
    LDR                 R0,=RVM_Usr_Param
    LDR                 R0,[R0]             ;Push hypercall parameters
    LDMIA               R0,{R1-R5}
    PUSH                {R1-R5}

    LDR                 R1,=pxCurrentTCB    ;Save sp to the first TCB member
    LDR                 R1,[R1]
    STR                 SP,[R1]
    BL                  vTaskSwitchContext  ;Select the highest priority ready task
    LDR                 R1,=pxCurrentTCB    ;Load sp from the first TCB member
    LDR                 R1,[R1]
    LDR                 SP,[R1]

    BL                  RVM_Virt_Int_Unmask ;Enable interrupts

    LDR                 R0,=RVM_Usr_Param
    LDR                 R0,[R0]             ;Pop hypercall parameters
    POP                 {R1-R5}
    STMIA               R0,{R1-R5}
    POP                 {R4-R11,LR}         ;Pop GP registers

    TST                 LR,#0x00000010      ;LR.EXTENDED - the frame type
    BEQ                 vPortYield_Unstack_Extended

vPortYield_Unstack_Basic                   ;Basic frame
    portALIGN_POP       vPortYield_Unstack_Basic_Pad
vPortYield_Unstack_Basic_NoPad             ;Frame without alignment padding
    portEXC_POP         8
    B                   vPortYield_Exit
vPortYield_Unstack_Basic_Pad               ;Frame with alignment padding
    portEXC_POP         8+1
    B                   vPortYield_Exit

vPortYield_Unstack_Extended                ;Extended frame extra popping
    DC16                0xECBD,0x8A10       ;VPOP {S16-S31}
    ADD                 R0,SP,#4*(8+16)
    LDR                 R1,[R0]
    DC16                0xEEE1,0x1A10       ;VMSR FPSCR,R1
    DC16                0xED30,0x0A10       ;VLDMDB R0!,{S0-S15}
    portALIGN_POP       vPortYield_Unstack_Extended_Pad
vPortYield_Unstack_Extended_NoPad          ;Frame without alignment padding
    portEXC_POP         8+17+1
    B                   vPortYield_Exit
vPortYield_Unstack_Extended_Pad            ;Frame with alignment padding
    portEXC_POP         8+17+2

vPortYield_Exit                            ;Exit location - also the resume PC
    BX                  LR

    LTORG
    END
