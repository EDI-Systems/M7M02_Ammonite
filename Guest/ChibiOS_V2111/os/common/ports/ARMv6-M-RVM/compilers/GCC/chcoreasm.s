/*
    CHB port for the RVM hypervisor.
    Copyright (C) 2026 Evo-Devo Instrum. All rights reserved.

    This file is NOT part of the ChibiOS distribution. It belongs to the
    ChibiOS-on-RVM guest port and is released under the Unlicense; refer to
    the LICENSE file for details.  The ChibiOS kernel itself is copyright
    Giovanni Di Sirio and is not modified by this file.
*/

/**
 * @file    ARMv6-M-RVM/compilers/GCC/chcoreasm.s
 * @brief   CHB port low level code.
 *
 * The slow path is implemented in C on the vector thread.  This file
 * provides the first-entry trampoline and the task-level direct switch
 * used by the CHB fast path.  The frame is the same masqueraded exception
 * frame as the C context handler: five hypercall parameters, R4-R11 plus
 * EXC_RETURN, and the hardware frame.  ARMv6-M has no FPU and no IT
 * blocks, so every frame is a basic frame.
 *
 * ARMv6-M restriction notes (encoding rules used throughout):
 * - PUSH/POP only take the low registers plus LR/PC; R8-R11 travel through
 *   R0-R3 and R12/LR are stored with explicit STR.
 * - There is no immediate form of AND/ORR/MOV with arbitrary constants,
 *   so every mask is loaded with LDR from a literal.
 * - TST with SP is unpredictable, so SP is tested through R0.
 */

#if !defined(FALSE)
#define FALSE 0
#endif
#if !defined(TRUE)
#define TRUE 1
#endif

#define _FROM_ASM_
#include "chlicense.h"
#include "chconf.h"
#include "chcore.h"

#if !defined(__DOXYGEN__)

#if !defined(CHB_FAST_PATH)
#define CHB_FAST_PATH 0
#endif

                .syntax unified
                .arch   armv6s-m
                .thumb

/* The first-entry trampoline stored in a new thread's basic frame. */
                .section .text.__port_thread_start
                .align  3
                .thumb_func
                .globl  __port_thread_start
__port_thread_start:
                mov     r0, r4
                mov     r1, r5
                bl      chb_thread_start
1:              b       1b

#if CHB_FAST_PATH

                .section .text.chb_get_xpsr
                .align  3
                .thumb_func
                .globl  chb_get_xpsr
chb_get_xpsr:
                mrs     r0, xpsr
                bx      lr

/* See whether the stack pointer is 8 byte aligned.  TST SP is
 * unpredictable on ARMv6-M, so SP has to go through a register first.
 * Three words have been pushed above, thus EQ means the original stack
 * was unaligned and needs the padded frame. */
                .macro  chb_align_push label
                mov     r0, sp
                ldr     r1, =0x00000007
                tst     r0, r1
                beq     \label
                .endm

/* Stack the remaining part of a basic exception frame of \sz words, with
 * the given XPSR and EXC_RETURN, and the resume point as the PC.  R0, XPSR
 * and R1 have been protected by the entry pushes. */
                .macro  chb_exc_push sz, xpsr, exc_return
                ldr     r0, [sp, #4*2]      /* Real R0/R1 values. */
                ldr     r1, [sp]
                sub     sp, #4*(\sz-7)      /* Room for R12/LR/PC/XPSR/[PAD]. */
                push    {r0-r3}             /* Frame GP registers. */
                mov     r0, r12
                mov     r1, lr
                str     r0, [sp, #4*4]
                str     r1, [sp, #4*5]
                ldr     r0, [sp, #4*(\sz-2)]/* Real XPSR value. */
                ldr     r1, =\xpsr
                orrs    r0, r1
                str     r0, [sp, #4*7]
                ldr     r0, =chb_fast_exit  /* PC with bit[0] cleared. */
                ldr     r1, =0xFFFFFFFE
                ands    r0, r1
                str     r0, [sp, #4*6]
                ldr     r1, =\exc_return    /* EXC_RETURN into LR. */
                mov     lr, r1
                .endm

/* See whether the stacked XPSR carries the 8 byte alignment padding. */
                .macro  chb_align_pop label
                ldr     r0, [sp, #4*7]
                ldr     r1, =0x00000200
                tst     r0, r1
                bne     \label
                .endm

/* Unstack a basic exception frame of \sz words.  The original layout
 * [PAD] XPSR PC LR R12 R3-R0 is inconvenient for manual restoring,
 * because loading PC and SP must be the last step, so it is transformed
 * in place first:
 *   1. With [PAD]:    [PAD] XPSR PC LR R12 R3-R0 -> PC R0 XPSR [EMPTY] R12 R3-R1 LR
 *   2. Without [PAD]: XPSR PC LR R12 R3-R0       -> PC R0 XPSR R12 R3-R1 LR
 * The transformation restores LR first so it cannot be overwritten, pops
 * R0, loads XPSR and PC and rearranges them at the end, pops R1-R3 and
 * LR while skipping the middle, and finally restores XPSR through R0,
 * then R0 and PC.  Nothing is ever placed below the current SP, because
 * a racing interrupt would clobber it. */
                .macro  chb_exc_pop sz
                ldr     r0, [sp, #4*4]      /* Restore R12/LR. */
                ldr     r1, [sp, #4*5]
                mov     r12, r0
                mov     lr, r1
                pop     {r0}                /* R0/XPSR/PC into R0/R1/R2. */
                ldr     r1, [sp, #4*6]
                ldr     r3, =0xFFFFFDFF     /* Clear XPSR[9]. */
                ands    r1, r3
                ldr     r2, [sp, #4*5]
                movs    r3, #1              /* Set PC[0]. */
                orrs    r2, r3
                str     r1, [sp, #4*(\sz-4)]/* Rearrange to H-PC-R0-XPSR-L. */
                str     r0, [sp, #4*(\sz-3)]
                str     r2, [sp, #4*(\sz-2)]
                pop     {r1-r3}             /* Pop GP registers. */
                add     sp, #4*(\sz-7)      /* Skip R12/[PAD]. */
                pop     {r0}                /* Pop XPSR through R0. */
                msr     xpsr_nzcvq, r0
                pop     {r0, pc}            /* Pop R0 and PC. */
                .endm

/*
 * Direct task-to-task switch.  Entered from chb_port_switch() with the
 * virtual interrupts masked, after the C side has verified that the heir
 * frame is clean.  It builds a complete user frame on the outgoing stack,
 * stores it in otp->ctx.sp, loads the heir frame from ntp->ctx.sp,
 * unmasks and manually unstacks the heir.
 */
                .section .text.chb_fast_switch
                .align  3
                .thumb_func
                .globl  chb_fast_switch
chb_fast_switch:
                push    {r0}                /* Protect R0, XPSR and R1. */
                mrs     r0, xpsr
                push    {r0}
                push    {r1}

                chb_align_push chb_fast_unaligned
chb_fast_aligned:                           /* Originally 8 byte aligned. */
                chb_exc_push 8, 0x01000000, 0xFFFFFFFD
                b       chb_fast_stacked
chb_fast_unaligned:                         /* Originally unaligned. */
                chb_exc_push 8+1, 0x01000200, 0xFFFFFFFD

chb_fast_stacked:
                push    {lr}                /* GP block: R4-R11, LR. */
                mov     r0, r8
                mov     r1, r9
                mov     r2, r10
                mov     r3, r11
                push    {r0-r3}
                push    {r4-r7}

                ldr     r0, =RVM_Usr_Param  /* Push the hypercall params. */
                ldr     r0, [r0]
                ldmia   r0!, {r1-r5}
                push    {r1-r5}

                mov     r0, sp              /* otp->ctx.sp = frame base. */
                bl      chb_fast_save
                bl      chb_fast_load       /* ntp->ctx.sp into SP. */
                mov     sp, r0

                bl      RVM_Virt_Int_Unmask /* The signal can switch here. */

                ldr     r0, =RVM_Usr_Param  /* Publish the heir's params. */
                ldr     r0, [r0]
                pop     {r1-r5}
                stmia   r0!, {r1-r5}

                pop     {r4-r7}             /* Pop GP registers. */
                pop     {r0-r3}
                mov     r8, r0
                mov     r9, r1
                mov     r10, r2
                mov     r11, r3
                pop     {r0}
                mov     lr, r0

                chb_align_pop chb_fast_pad
chb_fast_nopad:                             /* Frame without padding. */
                chb_exc_pop 8
                b       chb_fast_exit
chb_fast_pad:                               /* Frame with padding. */
                chb_exc_pop 8+1

chb_fast_exit:
                bx      lr

#endif /* CHB_FAST_PATH */

#endif /* !defined(__DOXYGEN__) */
