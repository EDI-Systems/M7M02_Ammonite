;/*
; * FreeRTOS V9.0.0 RVM port - Copyright (C) 2026 Evo-Devo Instrum.
; * All rights reserved.
; *
; * This file is NOT part of the FreeRTOS distribution.  It belongs to the
; * FreeRTOS-on-RVM test suite and is released under the Unlicense; refer
; * to the LICENSE file for details.  The FreeRTOS kernel itself is copyright
; * Real Time Engineers Ltd. and is not modified by this file.
; *
; * This file implements the assembly part of the porting layer for ARMv7-M
; * systems running as an unprivileged guest of the RVM hypervisor: the
; * fast-path yield routine.  This is the armasm syntax variant; the GNU as
; * variant of the very same routine lives in portable/GCC/A7M_RVM.
; */

;/* Symbols provided by the scheduler, the RVM paravirtualization library
; * and the port itself. */
    IMPORT              vTaskSwitchContext  ;The scheduler.
    IMPORT              pxCurrentTCB        ;The TCB of the task to run.
    IMPORT              RVM_Usr_Param       ;The user hypercall parameter block.
    IMPORT              RVM_Virt_Int_Unmask ;Unmask the RVM virtual interrupts.
    IMPORT              vPortYield_Err      ;The dirty ICI/IT trap in port.c.

;/* Entry point implemented in this file, called by vPortYield() in port.c
; * when configUSE_RVM_OPTIMISED_YIELD is enabled and the peeked target
; * task holds a clean ICI/IT state.  The caller holds the virtual interrupt
; * mask throughout the peek, and this routine unmasks the interrupts itself
; * once the new stack has been loaded. */
    EXPORT              vPortYield_Swt

;/* The exception frame built here, from the stack pointer up:
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

;/* Header *******************************************************************/
    AREA                PORTMACRO,CODE,READONLY,ALIGN=3
    THUMB
    REQUIRE8
    PRESERVE8
;/* End Header ***************************************************************/

;/* See whether the stack pointer is 8 byte aligned.  TST SP is
; * unpredictable on ARMv7-M, so SP has to go through a register first. */
    MACRO
    portALIGN_PUSH      $LABEL
    MOV                 R0,SP               ;Is SP aligned to 8 bytes?
    TST                 R0,#0x00000007
    BNE                 $LABEL              ;Two words pushed above, thus NE
    MEND

;/* Stack the remaining part of an exception frame of $SZ words, with the
; * given XPSR bits and EXC_RETURN, and the exit label as the PC.  The two
; * words pushed at the entry double as the frame's XPSR/PC slots when the
; * frame is basic, and sit just above it otherwise. */
    MACRO
    portEXC_PUSH        $SZ,$XPSR,$LR
    LDR                 R0,[SP,#4]          ;Load the real R0 value pushed at entry
    IF                  (($SZ-8)!=0)        ;Reserve the frame's upper words
    SUB                 SP,#4*($SZ-8)
    ENDIF
    PUSH                {R0-R3,R12,LR}      ;Push the frame's GP registers
    LDR                 R0,[SP,#4*($SZ-2)]  ;Load the real XPSR value pushed at entry
    LDR                 R1,=$XPSR
    ORR                 R0,R1
    STR                 R0,[SP,#4*7]        ;Merge in the flags and the padding mark
    LDR                 R0,=vPortYield_Exit ;Push the resume PC with bit[0] cleared
    AND                 R0,#0xFFFFFFFE
    STR                 R0,[SP,#4*6]
    MOV                 LR,#$LR             ;Make up the EXC_RETURN
    MEND

;/* See whether the frame holds an alignment padding word: bit 9 of the
; * stacked XPSR is used to mark it. */
    MACRO
    portALIGN_POP       $LABEL
    LDR                 R0,[SP,#4*7]        ;Load up the XPSR
    TST                 R0,#0x00000200
    BNE                 $LABEL
    MEND

;/* Unstack an exception frame of $SZ words.  The original sequence is
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
    MACRO
    portEXC_POP         $SZ
    LDR                 LR,[SP,#4*5]        ;Restore the frame's LR
    POP                 {R0}                ;Load R0/XPSR/PC into R0/R1/R2
    LDR                 R1,[SP,#4*6]
    LDR                 R12,=0x0600FC00     ;The EPSR ICI/IT field
    TST                 R1,R12              ;Dirty ICI/IT cannot be restored here
    BNE                 vPortYield_Err      ;Defense in depth - see the header
    AND                 R1,#0xFFFFFDFF      ;Clear XPSR[9]
    LDR                 R2,[SP,#4*5]
    ORR                 R2,#0x00000001      ;Set PC[0]
    STR                 R1,[SP,#4*($SZ-4)]  ;Rearrange to H-PC-R0-XPSR-L
    STR                 R0,[SP,#4*($SZ-3)]
    STR                 R2,[SP,#4*($SZ-2)]
    POP                 {R1-R3,R12}         ;Pop the GP registers
    IF                  (($SZ-8)!=0)        ;Skip the FPU area and the padding
    ADD                 SP,#4*($SZ-8)
    ENDIF
    POP                 {R0}                ;Pop the XPSR through R0
    MSR                 XPSR,R0
    POP                 {R0,PC}             ;Pop R0 and PC
    MEND

;/* User-level Context Switch ************************************************/
    AREA                FASTYIELD,CODE,READONLY,ALIGN=3
vPortYield_Swt       PROC                    ;Entered with the virtual interrupts
                                            ;already masked by vPortYield()
    PUSH                {R0}                ;Protect R0 and XPSR
    MRS                 R0,XPSR
    PUSH                {R0}
    MRS                 R0,CONTROL
    TST                 R0,#0x00000004      ;CONTROL.FPCA - the frame type
    BNE                 vPortYield_Extended

vPortYield_Basic                              ;Basic frame
    portALIGN_PUSH      vPortYield_Basic_Unaligned
vPortYield_Basic_Aligned                      ;Originally 8 byte aligned stack
    portEXC_PUSH        8,0x01000000,0xFFFFFFFD
    B                   vPortYield_Stacked
vPortYield_Basic_Unaligned                    ;Originally unaligned stack
    portEXC_PUSH        8+1,0x01000200,0xFFFFFFFD
    B                   vPortYield_Stacked

vPortYield_Extended                           ;Extended frame
    portALIGN_PUSH      vPortYield_Extended_Unaligned
vPortYield_Extended_Aligned                   ;Originally 8 byte aligned stack
    portEXC_PUSH        8+17+1,0x01000000,0xFFFFFFED
    B                   vPortYield_Extended_Saved
vPortYield_Extended_Unaligned                 ;Originally unaligned stack
    portEXC_PUSH        8+17+2,0x01000200,0xFFFFFFED
vPortYield_Extended_Saved                     ;Extended frame extra pushing
    ADD                 R0,SP,#4*8          ;Locate the FPU stacking area
    DCI                 0xECA0              ;Push FPU regs
    DCI                 0x0A10              ;VSTMIA R0!,{S0-S15}
    DCI                 0xEEF1              ;Push FPSCR
    DCI                 0x1A10              ;VMRS R1,FPSCR
    STR                 R1,[R0]
    DCI                 0xED2D              ;Push OS-managed FPU regs
    DCI                 0x8A10              ;VPUSH {S16-S31}

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

vPortYield_Unstack_Basic                      ;Basic frame
    portALIGN_POP       vPortYield_Unstack_Basic_Pad
vPortYield_Unstack_Basic_NoPad                ;Frame without alignment padding
    portEXC_POP         8
    B                   vPortYield_Exit
vPortYield_Unstack_Basic_Pad                  ;Frame with alignment padding
    portEXC_POP         8+1
    B                   vPortYield_Exit

vPortYield_Unstack_Extended                   ;Extended frame extra popping
    DCI                 0xECBD              ;Pop OS-managed FPU regs
    DCI                 0x8A10              ;VPOP {S16-S31}
    ADD                 R0,SP,#4*(8+16)
    LDR                 R1,[R0]
    DCI                 0xEEE1              ;Pop FPSCR
    DCI                 0x1A10              ;VMSR FPSCR,R1
    DCI                 0xED30              ;Pop FPU regs
    DCI                 0x0A10              ;VLDMDB R0!,{S0-S15}
    portALIGN_POP       vPortYield_Unstack_Extended_Pad
vPortYield_Unstack_Extended_NoPad             ;Frame without alignment padding
    portEXC_POP         8+17+1
    B                   vPortYield_Exit
vPortYield_Unstack_Extended_Pad               ;Frame with alignment padding
    portEXC_POP         8+17+2

vPortYield_Exit                               ;Exit location - also the resume PC
    BX                  LR
    ENDP
;/* End Of Function:vPortYield_Swt *******************************************/

    ALIGN
    LTORG
    END