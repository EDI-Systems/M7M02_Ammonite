;/*
; * RT-Thread V4.1.1 RVM port - Copyright (C) 2026 Evo-Devo Instrum.
; * All rights reserved
; *
; * This file is NOT part of the RT-Thread distribution.  It belongs to the
; * RT-Thread-on-RVM test suite and is released under the Unlicense; refer
; * to the LICENSE file for details.  The RT-Thread kernel itself is
; * copyright RT-Thread Development Team and is not modified by this file.
; *
; * Change Logs:
; * Date           Author       Notes
; * 2026-10-28     Evo-Devo     first version for the ARMv7-M RVM guest,
; *                             translated from context_gcc.s.
; */

;/**
; * @addtogroup armv7m_rvm
; */
;/*@{*/

XPSR_ALIGN_PAD    EQU     0x00000200              ; XPSR bit 9 marks the frame alignment padding.

; See whether the stack pointer is 8 byte aligned.  The entry sequence
; below pushed two words (r0 and XPSR), so an 8 byte aligned SP here
; means the stack was already aligned on entry: the polarity is
; therefore NE, like the RMP and uC/OS-III ARMv7-M ports - NOT the
; ARMv6-M original, whose three word entry runs the test the other way
; round.
    MACRO
    RT_ALIGN_PUSH  $label
    MOV                 r0, sp
    TST                 r0, #0x00000007
    BNE                 $label                  ; two words pushed above, thus NE
    MEND

; Stack the remaining part of a basic exception frame of $sz words, with
; the given XPSR and EXC_RETURN, and the resume point as the PC.  The
; two word entry prologue (r0 at [sp,#4], XPSR at [sp]) is reflected in
; the reload offsets below: r1-r3, r12 and lr are still live registers,
; so only r0 needs reloading, and the six register push leaves the
; saved XPSR exactly at [sp,#4*($sz-2)].
    MACRO
    RT_EXC_PUSH    $sz, $xpsr, $exc_return
    LDR                 r0, [sp, #4]            ; load the real r0 value pushed at entry
    IF                  (($sz-8)!=0)            ; adjust SP to push GP registers when needed
    SUB                 sp, #4*($sz-8)
    ENDIF
    PUSH                {r0-r3, r12, lr}        ; push stack frame GP registers
    LDR                 r0, [sp, #4*($sz-2)]    ; load the real XPSR value pushed at entry
    LDR                 r1, =$xpsr
    ORRS                r0, r1
    STR                 r0, [sp, #4*7]          ; merge in the flags and the padding mark
    LDR                 r0, =rt_hw_context_switch_exit
    LDR                 r1, =0xFFFFFFFE         ; push the resume PC with bit[0] cleared
    ANDS                r0, r1
    STR                 r0, [sp, #4*6]
    LDR                 r1, =$exc_return        ; make up the EXC_RETURN
    MOV                 lr, r1
    MEND

; See whether the stacked XPSR carries the 8 byte alignment padding.
    MACRO
    RT_ALIGN_POP   $label
    LDR                 r0, [sp, #4*7]          ; load up the XPSR
    TST                 r0, #XPSR_ALIGN_PAD
    BNE                 $label
    MEND

; Unstack a basic exception frame of $sz words.  The original layout
; [PAD] [FPU] XPSR PC LR R12 R3-R0 is inconvenient for manual
; restoring, because loading PC must be the last step, so it is
; transformed in place first:
;   1. With [PAD] or [FPU]: [PAD] [FPU] XPSR PC LR R12 R3-R0 -> PC R0 XPSR [EMPTY] R12 R3-R1 LR
;   2. Without either:      XPSR PC LR R12 R3-R0             -> PC R0 XPSR R12 R3-R1 LR
; Nothing is ever placed below the current SP, because a racing
; interrupt would clobber it.  The EPSR ICI/IT field of the frame is
; checked on the fly: dirty bits cannot be restored here - only the
; real exception return of the slow path can - so the switch bails out
; into the C layer's error trap.
    MACRO
    RT_EXC_POP     $sz
    LDR                 lr, [sp, #4*5]          ; restore the frame's LR
    POP                 {r0}                    ; load r0/XPSR/PC into r0/r1/r2
    LDR                 r1, [sp, #4*6]
    LDR                 r12, =0x0600FC00        ; the EPSR ICI/IT field
    TST                 r1, r12                 ; dirty ICI/IT cannot be restored here
    BNE                 rt_hw_context_switch_err
    AND                 r1, #0xFFFFFDFF         ; clear XPSR[9]
    LDR                 r2, [sp, #4*5]
    ORR                 r2, #0x00000001         ; set PC[0]
    STR                 r1, [sp, #4*($sz-4)]    ; rearrange to H-PC-R0-XPSR-L
    STR                 r0, [sp, #4*($sz-3)]
    STR                 r2, [sp, #4*($sz-2)]
    POP                 {r1-r3, r12}            ; pop GP registers
    IF                  (($sz-8)!=0)            ; skip the FPU area and the padding
    ADD                 sp, #4*($sz-8)
    ENDIF
    POP                 {r0}                    ; pop the XPSR through r0
    MSR                 XPSR, r0
    POP                 {r0, pc}                ; pop r0 and PC
    MEND


    AREA |.text|, CODE, READONLY, ALIGN=2
    THUMB
    REQUIRE8
    PRESERVE8

    IMPORT  RVM_Usr_Param
    IMPORT  RVM_Virt_Int_Mask
    IMPORT  RVM_Virt_Int_Unmask
    IMPORT  RVM_Virt_Yield
    IMPORT  RVM_Hyp_Int_Ena
    IMPORT  rt_interrupt_from_thread
    IMPORT  rt_interrupt_to_thread
    IMPORT  rt_thread_switch_interrupt_flag
    IMPORT  rt_hw_context_switch_err

;/*
; * void rt_hw_context_switch_interrupt(rt_uint32 from, rt_uint32 to);
; * r0 --> from
; * r1 --> to
; *
; * Called by rt_schedule() when a switch is requested from an
; * interrupt context.  Only the switch bookkeeping is done here: the
; * from/to stack pointer locations are latched and the flag is raised,
; * and the switch itself is deferred through the RVM yield hypercall.
; * Never fall through into the fast path here.
; */
rt_hw_context_switch_interrupt    PROC
    EXPORT  rt_hw_context_switch_interrupt
    ; set rt_thread_switch_interrupt_flag to 1
    LDR                 r2, =rt_thread_switch_interrupt_flag
    LDR                 r3, [r2]
    CMP                 r3, #1
    BEQ                 _reswitch
    MOVS                r3, #1
    STR                 r3, [r2]

    LDR                 r2, =rt_interrupt_from_thread   ; set rt_interrupt_from_thread
    STR                 r0, [r2]

_reswitch
    LDR                 r2, =rt_interrupt_to_thread     ; set rt_interrupt_to_thread
    STR                 r1, [r2]

    ; the interrupt thread only marks the switch pending; the real
    ; switch happens at the tail of its dispatch loop
    PUSH                {lr}
    BL                  RVM_Virt_Yield
    POP                 {pc}
    ENDP

;/*
; * void rt_hw_context_switch_swt(void);
; *
; * The raw fast-path switch, entered from rt_hw_context_switch() in
; * the C layer with the switch pair already latched into the port
; * globals and with the virtual interrupts already masked by the
; * scheduler's critical section.  It masquerades the calling context
; * as if it were stacked by an exception entry, performs the switch
; * directly on the thread stacks, and unmasks the interrupts itself
; * once the new frame has been loaded.
; */
rt_hw_context_switch_swt    PROC
    EXPORT  rt_hw_context_switch_swt
    ; cancel any interrupt request that is still pending: this switch
    ; supersedes it, and PendSV_Handler() must never replay it later
    LDR                 r2, =rt_thread_switch_interrupt_flag
    MOVS                r3, #0
    STR                 r3, [r2]

    PUSH                {r0}                    ; protect r0 and XPSR
    MRS                 r0, XPSR
    PUSH                {r0}
    MRS                 r0, CONTROL
    TST                 r0, #0x00000004         ; CONTROL.FPCA - the frame type
    BNE                 swt_extended

swt_basic                                        ; basic frame
    RT_ALIGN_PUSH       swt_basic_unaligned
swt_basic_aligned                                ; originally 8 byte aligned stack
    RT_EXC_PUSH         8, 0x01000000, 0xFFFFFFFD
    B                   swt_stacked
swt_basic_unaligned                              ; originally unaligned stack
    RT_EXC_PUSH         8+1, 0x01000200, 0xFFFFFFFD
    B                   swt_stacked

swt_extended                                     ; extended frame
    RT_ALIGN_PUSH       swt_ext_unaligned
swt_ext_aligned                                  ; originally 8 byte aligned stack
    RT_EXC_PUSH         8+17+1, 0x01000000, 0xFFFFFFED
    B                   swt_ext_saved
swt_ext_unaligned                                ; originally unaligned stack
    RT_EXC_PUSH         8+17+2, 0x01000200, 0xFFFFFFED

swt_ext_saved                                    ; extended frame extra pushing
    ADD                 r0, sp, #4*8            ; locate the FPU stacking area
    DCI                 0xECA0                  ; vstmia r0!, {s0-s15}
    DCI                 0x0A10
    DCI                 0xEEF1                  ; vmrs r1, fpscr
    DCI                 0x1A10
    STR                 r1, [r0]
    DCI                 0xED2D                  ; vpush {s16-s31}
    DCI                 0x8A10

swt_stacked
    PUSH                {r4-r11, lr}            ; push GP registers

    LDR                 r0, =RVM_Usr_Param      ; push the hypercall parameters
    LDR                 r0, [r0]
    LDMIA               r0!, {r1-r5}
    PUSH                {r1-r5}

    BL                  RVM_Virt_Int_Mask       ; disable the virtual interrupts

    LDR                 r1, =rt_interrupt_from_thread
    LDR                 r1, [r1]                ; write sp to the from thread's TCB slot
    MOV                 r2, sp
    STR                 r2, [r1]

    LDR                 r1, =rt_interrupt_to_thread
    LDR                 r1, [r1]
    LDR                 r1, [r1]                ; load the to thread's stack pointer
    MOV                 sp, r1

    ; enable the virtual interrupts through the wrapper: its
    ; re-delivery signal drains any vectors and context switch requests
    ; that piled up while the mask was raised, right here at the only
    ; boundary where the switch state is coherent again
    BL                  RVM_Virt_Int_Unmask

    LDR                 r0, =RVM_Usr_Param      ; pop the hypercall parameters
    LDR                 r0, [r0]
    POP                 {r1-r5}
    STMIA               r0!, {r1-r5}

    POP                 {r4-r11, lr}            ; pop GP registers

    TST                 lr, #0x00000010         ; LR.EXTENDED - the frame type
    BEQ                 unstack_extended

unstack_basic                                    ; basic frame
    RT_ALIGN_POP        unstack_basic_pad
unstack_basic_nopad                              ; frame without alignment padding
    RT_EXC_POP          8
    B                   rt_hw_context_switch_exit
unstack_basic_pad                                ; frame with alignment padding
    RT_EXC_POP          8+1
    B                   rt_hw_context_switch_exit

unstack_extended                                 ; extended frame extra popping
    DCI                 0xECBD                  ; vpop {s16-s31}
    DCI                 0x8A10
    ADD                 r0, sp, #4*(8+16)
    LDR                 r1, [r0]
    DCI                 0xEEE1                  ; vmsr fpscr, r1
    DCI                 0x1A10
    DCI                 0xED30                  ; vldmdb r0!, {s0-s15}
    DCI                 0x0A10
    RT_ALIGN_POP        unstack_ext_pad
unstack_ext_nopad                                ; frame without alignment padding
    RT_EXC_POP          8+17+1
    B                   rt_hw_context_switch_exit
unstack_ext_pad                                  ; frame with alignment padding
    RT_EXC_POP          8+17+2

rt_hw_context_switch_exit                        ; exit location - also the resume PC
    BX                  lr
    ENDP

;/*
; * void rt_hw_context_switch_to(rt_uint32 to);
; * r0 --> to - the address of the to thread's stack pointer slot
; *
; * Starts the first thread.  The tick and the context switch handler
; * have already been hooked up by the board initialization, and the
; * virtual interrupts are enabled here just like the official ports do
; * before the first thread runs.  The startup stack is abandoned.
; *
; * The switch request flag is left clear: this port jumps into the
; * first thread itself instead of pending the context switch handler,
; * so there is no request to latch - keeping the flag set with a null
; * from-thread would make the first interrupt driven switch skip
; * saving the first thread's preempted context.
; */
rt_hw_context_switch_to    PROC
    EXPORT  rt_hw_context_switch_to
    LDR                 r1, =rt_interrupt_to_thread
    STR                 r0, [r1]                ; store the to-thread sp slot address

    LDR                 r1, =rt_interrupt_from_thread
    MOVS                r0, #0                  ; no thread has ever been switched out yet
    STR                 r0, [r1]

    LDR                 r1, =rt_thread_switch_interrupt_flag
    MOVS                r0, #0                  ; no request to latch
    STR                 r0, [r1]

    BL                  RVM_Hyp_Int_Ena         ; enable the virtual interrupts

    LDR                 r1, =rt_interrupt_to_thread
    LDR                 r1, [r1]
    LDR                 r0, [r1]                ; the first thread's stack pointer
    MOV                 sp, r0                  ; abandon the startup stack

    POP                 {r0-r4}                 ; discard the hypercall parameters
    POP                 {r4-r7}                 ; restore r4-r7
    POP                 {r0-r3}                 ; restore r8-r11 through r0-r3
    MOV                 r8, r0
    MOV                 r9, r1
    MOV                 r10, r2
    MOV                 r11, r3
    POP                 {r0}                    ; discard the stacked EXC_RETURN
    POP                 {r0-r4}                 ; restore r0-r3, r12
    MOV                 r12, r4
    POP                 {r4}                    ; the thread return address, discarded
    POP                 {r0}                    ; the thread entry point
    MOV                 lr, r0                  ; enter the thread through lr
    PUSH                {r4}                    ; fix up sp/lr
    BX                  lr                      ; branch to the thread entry
    ENDP

; compatible with old version
rt_hw_interrupt_thread_switch    PROC
    EXPORT  rt_hw_interrupt_thread_switch
    BX                  lr
    ENDP

    ALIGN   4

    END

;/*
; *             END OF FILE
; */
