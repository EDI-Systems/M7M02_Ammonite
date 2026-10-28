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
; * 2026-10-28     Evo-Devo     first version for the ARMv6-M RVM guest,
; *                             translated from context_gcc.s.
; */

;/**
; * @addtogroup armv6m_rvm
; */
;/*@{*/

XPSR_ALIGN_PAD    EQU     0x00000200              ; XPSR bit 9 marks the frame alignment padding.

; See whether the stack pointer is 8 byte aligned.  TST SP is
; unpredictable on ARMv6-M, so SP has to go through a register first.
; Three words have been pushed above the tested value, so an aligned
; original stack pointer lands here unaligned, and vice versa.
RT_ALIGN_PUSH     MACRO   LABEL
    MOV                 r0,sp                   ; is SP aligned to 8 bytes?
    LDR                 r1,=0x00000007
    TST                 r0,r1
    BEQ                 LABEL                   ; three words pushed above, thus EQ
    ENDM

; Stack the remaining part of a basic exception frame of SZ words, with
; the given XPSR and EXC_RETURN, and the resume point as the PC.  The
; three word entry prologue (r0 at [sp,#8], XPSR at [sp,#4], r1 at [sp])
; is reflected in the reload offsets below.
RT_EXC_PUSH       MACRO   SZ, XPSR, EXC_RETURN
    LDR                 r0,[sp,#4*2]            ; load the real r0/r1 values pushed at entry
    LDR                 r1,[sp]
    SUB                 sp,sp,#4*(SZ-7)         ; adjust SP to push GP registers
    PUSH                {r0-r3}                 ; push stack frame GP registers
    MOV                 r0,r12
    MOV                 r1,lr
    STR                 r0,[sp,#4*4]
    STR                 r1,[sp,#4*5]
    LDR                 r0,[sp,#4*(SZ-2)]       ; load the real XPSR value pushed at entry
    LDR                 r1,=XPSR
    ORRS                r0,r1
    STR                 r0,[sp,#4*7]            ; merge in the flags and the padding mark
    LDR                 r0,=rt_hw_context_switch_exit
    LDR                 r1,=0xFFFFFFFE         ; push the resume PC with bit[0] cleared
    ANDS                r0,r1
    STR                 r0,[sp,#4*6]
    LDR                 r1,=EXC_RETURN          ; make up the EXC_RETURN
    MOV                 lr,r1
    ENDM

; See whether the stacked XPSR carries the 8 byte alignment padding.
RT_ALIGN_POP      MACRO   LABEL
    LDR                 r0,[sp,#4*7]            ; load up the XPSR
    LDR                 r1,=XPSR_ALIGN_PAD
    TST                 r0,r1
    BNE                 LABEL
    ENDM

; Unstack a basic exception frame of SZ words.  The original layout
; [PAD] XPSR PC LR R12 R3-R0 is inconvenient for manual restoring,
; because loading PC must be the last step, so it is transformed in
; place first:
;   1. With [PAD]:    [PAD] XPSR PC LR R12 R3-R0 -> PC R0 XPSR [EMPTY] R12 R3-R1 LR
;   2. Without [PAD]: XPSR PC LR R12 R3-R0       -> PC R0 XPSR R12 R3-R1 LR
; Nothing is ever placed below the current SP, because a racing
; interrupt would clobber it.  ARMv6-M has no ICI/IT state, so no
; dirty frame filtering is needed here.
RT_EXC_POP        MACRO   SZ
    LDR                 r0,[sp,#4*4]            ; restore LR/R12
    LDR                 r1,[sp,#4*5]
    MOV                 r12,r0
    MOV                 lr,r1
    POP                 {r0}                    ; load r0/XPSR/PC into r0/r1/r2
    LDR                 r1,[sp,#4*6]
    LDR                 r3,=0xFFFFFDFF          ; clear XPSR[9]
    ANDS                r1,r3
    LDR                 r2,[sp,#4*5]
    LDR                 r3,=0x00000001          ; set PC[0]
    ORRS                r2,r3
    STR                 r1,[sp,#4*(SZ-4)]       ; rearrange to H-PC-R0-XPSR-L
    STR                 r0,[sp,#4*(SZ-3)]
    STR                 r2,[sp,#4*(SZ-2)]
    POP                 {r1-r3}                 ; pop GP registers
    ADD                 sp,sp,#4*(SZ-7)         ; skip r12/[PAD]
    POP                 {r0}                    ; pop the XPSR through r0
    MSR                 APSR_nzcvq,r0
    POP                 {r0,pc}                 ; pop r0 and PC
    ENDM

    SECTION    .text:CODE(2)
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
    EXPORT  rt_hw_context_switch_interrupt
rt_hw_context_switch_interrupt:
    ; set rt_thread_switch_interrupt_flag to 1
    LDR                 r2,=rt_thread_switch_interrupt_flag
    LDR                 r3,[r2]
    CMP                 r3,#1
    BEQ                 _reswitch
    MOVS                r3,#1
    STR                 r3,[r2]

    LDR                 r2,=rt_interrupt_from_thread   ; set rt_interrupt_from_thread
    STR                 r0,[r2]

_reswitch:
    LDR                 r2,=rt_interrupt_to_thread     ; set rt_interrupt_to_thread
    STR                 r1,[r2]

    ; the interrupt thread only marks the switch pending; the real
    ; switch happens at the tail of its dispatch loop
    PUSH                {lr}
    BL                  RVM_Virt_Yield
    POP                 {pc}

;/*
; * void rt_hw_context_switch(rt_uint32 from, rt_uint32 to);
; * r0 --> from
; * r1 --> to
; *
; * Called by rt_schedule() from thread context, with the virtual
; * interrupts already masked by the scheduler.  Unlike the official
; * ports, no exception needs to be pended: the scheduler has already
; * updated all of its own bookkeeping, so the switch is performed right
; * here, directly on the thread stack, without a trap into the
; * hypervisor.  ARMv6-M has no ICI/IT state, so every frame can take
; * this direct path.
; */
    EXPORT  rt_hw_context_switch
rt_hw_context_switch:
    ; latch the switch pair - the body below reads the slot addresses
    ; from these globals; any interrupt request that is still pending
    ; is cancelled at the same time, as this switch supersedes it
    LDR                 r2,=rt_interrupt_from_thread
    STR                 r0,[r2]
    LDR                 r2,=rt_interrupt_to_thread
    STR                 r1,[r2]
    LDR                 r2,=rt_thread_switch_interrupt_flag
    MOVS                r3,#0
    STR                 r3,[r2]

    PUSH                {r0}                    ; protect r0, XPSR and r1
    MRS                 r0,XPSR
    PUSH                {r0}
    PUSH                {r1}

    RT_ALIGN_PUSH       rt_hw_context_switch_unaligned
rt_hw_context_switch_aligned:
    RT_EXC_PUSH         8, 0x01000000, 0xFFFFFFFD
    B                   rt_hw_context_switch_stacked
rt_hw_context_switch_unaligned:
    RT_EXC_PUSH         8+1, 0x01000200, 0xFFFFFFFD

rt_hw_context_switch_stacked:
    MOV                 r3,r11                  ; push GP registers
    MOV                 r2,r10
    MOV                 r1,r9
    MOV                 r0,r8
    PUSH                {r0-r3,lr}
    PUSH                {r4-r7}

    LDR                 r0,=RVM_Usr_Param       ; push the hypercall parameters
    LDR                 r0,[r0]
    LDMIA               r0!,{r1-r5}
    PUSH                {r1-r5}

    BL                  RVM_Virt_Int_Mask       ; disable the virtual interrupts

    LDR                 r1,=rt_interrupt_from_thread
    LDR                 r1,[r1]                 ; write sp to the from thread's TCB slot
    MOV                 r2,sp
    STR                 r2,[r1]

    LDR                 r1,=rt_interrupt_to_thread
    LDR                 r1,[r1]
    LDR                 r1,[r1]                 ; load the to thread's stack pointer
    MOV                 sp,r1

    ; enable the virtual interrupts through the wrapper: its
    ; re-delivery signal drains any vectors and context switch requests
    ; that piled up while the mask was raised, right here at the only
    ; boundary where the switch state is coherent again
    BL                  RVM_Virt_Int_Unmask

    LDR                 r0,=RVM_Usr_Param       ; pop the hypercall parameters
    LDR                 r0,[r0]
    POP                 {r1-r5}
    STMIA               r0!,{r1-r5}

    POP                 {r4-r7}                 ; pop thread {r4 - r7} register
    POP                 {r0-r3}                 ; pop thread {r8 - r11} to {r4 - r7}
    MOV                 r8,r0
    MOV                 r9,r1
    MOV                 r10,r2
    MOV                 r11,r3
    POP                 {r0}                    ; pop the interrupted LR
    MOV                 lr,r0

    RT_ALIGN_POP        rt_hw_context_switch_padskip
rt_hw_context_switch_nopad:                     ; frame without alignment padding
    RT_EXC_POP          8
rt_hw_context_switch_padskip:                   ; frame with alignment padding
    RT_EXC_POP          8+1

rt_hw_context_switch_exit:                      ; exit location - also the resume PC
    BX                  lr

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
    EXPORT  rt_hw_context_switch_to
rt_hw_context_switch_to:
    LDR                 r1,=rt_interrupt_to_thread
    STR                 r0,[r1]                 ; store the to-thread sp slot address

    LDR                 r1,=rt_interrupt_from_thread
    MOVS                r0,#0                   ; no thread has ever been switched out yet
    STR                 r0,[r1]

    LDR                 r1,=rt_thread_switch_interrupt_flag
    MOVS                r0,#0                   ; no request to latch
    STR                 r0,[r1]

    BL                  RVM_Hyp_Int_Ena         ; enable the virtual interrupts

    LDR                 r1,=rt_interrupt_to_thread
    LDR                 r1,[r1]
    LDR                 r0,[r1]                 ; the first thread's stack pointer
    MOV                 sp,r0                   ; abandon the startup stack

    POP                 {r0-r4}                 ; discard the hypercall parameters
    POP                 {r4-r7}                 ; restore r4-r7
    POP                 {r0-r3}                 ; restore r8-r11 through r0-r3
    MOV                 r8,r0
    MOV                 r9,r1
    MOV                 r10,r2
    MOV                 r11,r3
    POP                 {r0}                    ; discard the stacked EXC_RETURN
    POP                 {r0-r4}                 ; restore r0-r3, r12
    MOV                 r12,r4
    POP                 {r4}                    ; the thread return address, discarded
    POP                 {r0}                    ; the thread entry point
    MOV                 lr,r0                   ; enter the thread through lr
    PUSH                {r4}                    ; fix up sp/lr
    BX                  lr                      ; branch to the thread entry

; compatible with old version
    EXPORT  rt_hw_interrupt_thread_switch
rt_hw_interrupt_thread_switch:
    BX                  lr

    END

;/*
; *             END OF FILE
; */
