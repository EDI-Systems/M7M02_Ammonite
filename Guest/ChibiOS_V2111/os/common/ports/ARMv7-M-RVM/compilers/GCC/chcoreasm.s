/*
    CHB port for the RVM hypervisor.
    Copyright (C) 2026 Evo-Devo Instrum. All rights reserved.

    This file is NOT part of the ChibiOS distribution.  It belongs to the
    ChibiOS-on-RVM guest port and is released under the Unlicense; refer to
    the LICENSE file for details.  The ChibiOS kernel itself is copyright
    Giovanni Di Sirio and is not modified by this file.
*/

/**
 * @file    ARMv7-M-RVM/compilers/GCC/chcoreasm.s
 * @brief   CHB port low level code.
 *
 * The slow path is implemented in C on the vector thread.  This file
 * provides the first-entry trampoline and the task-level direct switch
 * used by the CHB fast path.  The frame is the same masqueraded exception
 * frame as the C context handler: five hypercall parameters, R4-R11 plus
 * EXC_RETURN, the optional S16-S31 block, and the hardware frame.
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
                .arch   armv7-m
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

                .section .text.chb_get_xpsr
                .align  3
                .thumb_func
                .globl  chb_get_xpsr
chb_get_xpsr:
                mrs     r0, xpsr
                bx      lr

#if CHB_FAST_PATH

/* See whether the stack pointer is 8 byte aligned. */
                .macro  chb_align_push label
                mov     r0, sp
                tst     r0, #0x00000007
                bne     \label
                .endm

/* Stack the hardware part of an exception frame of \sz words.  The two
   words pushed by the entry double as the frame's XPSR/PC slots when the
   frame is basic, and sit just above it otherwise. */
                .macro  chb_exc_push sz, xpsr, exc_return
                ldr     r0, [sp, #4]
                .if     ((\sz-8)!=0)
                sub     sp, #4*(\sz-8)
                .endif
                push    {r0-r3, r12, lr}
                ldr     r0, [sp, #4*(\sz-2)]
                ldr     r1, =\xpsr
                orr     r0, r1
                str     r0, [sp, #4*7]
                ldr     r0, =chb_fast_exit
                and     r0, #0xFFFFFFFE
                str     r0, [sp, #4*6]
                mov     lr, #\exc_return
                .endm

/* Unstack \sz words of hardware frame, transforming it in place so that
   PC and SP are restored last.  A dirty ICI/IT frame is a port invariant
   violation because the C side filters it before entering this routine. */
                .macro  chb_exc_pop sz
                ldr     lr, [sp, #4*5]
                pop     {r0}
                ldr     r1, [sp, #4*6]
                ldr     r12, =0x0600FC00
                tst     r1, r12
                bne     chb_fast_err
                and     r1, #0xFFFFFDFF
                ldr     r2, [sp, #4*5]
                orr     r2, #0x00000001
                str     r1, [sp, #4*(\sz-4)]
                str     r0, [sp, #4*(\sz-3)]
                str     r2, [sp, #4*(\sz-2)]
                pop     {r1-r3, r12}
                .if     ((\sz-8)!=0)
                add     sp, #4*(\sz-8)
                .endif
                pop     {r0}
                msr     xpsr_nzcvq, r0
                pop     {r0, pc}
                .endm

/*
 * Direct task-to-task switch.  Entered from chb_port_switch() with the
 * virtual interrupts masked, after the C side has verified that both the
 * current xPSR and the heir frame are clean.  It builds a complete user
 * frame on the outgoing stack, stores it in otp->ctx.sp, loads the heir
 * frame from ntp->ctx.sp, unmasks and manually unstacks the heir.
 */
                .section .text.chb_fast_switch
                .align  3
                .thumb_func
                .globl  chb_fast_switch
chb_fast_switch:
                push    {r0}
                mrs     r0, xpsr
                push    {r0}
#if CORTEX_USE_FPU
                mrs     r0, control
                tst     r0, #0x00000004
                bne     chb_fast_extended
#endif

chb_fast_basic:
                chb_align_push chb_fast_basic_unaligned
                chb_exc_push 8, 0x01000000, 0xFFFFFFFD
                b       chb_fast_stacked
chb_fast_basic_unaligned:
                chb_exc_push 8+1, 0x01000200, 0xFFFFFFFD
                b       chb_fast_stacked

#if CORTEX_USE_FPU
chb_fast_extended:
                chb_align_push chb_fast_extended_unaligned
                chb_exc_push 8+17+1, 0x01000000, 0xFFFFFFED
                b       chb_fast_extended_saved
chb_fast_extended_unaligned:
                chb_exc_push 8+17+2, 0x01000200, 0xFFFFFFED
chb_fast_extended_saved:
                add     r0, sp, #4*8
                .short  0xECA0, 0x0A10      /* vstmia r0!, {s0-s15} */
                .short  0xEEF1, 0x1A10      /* vmrs r1, fpscr */
                str     r1, [r0]
                .short  0xED2D, 0x8A10      /* vpush {s16-s31} */
#endif

chb_fast_stacked:
                push    {r4-r11, lr}

                ldr     r0, =RVM_Usr_Param
                ldr     r0, [r0]
                ldmia   r0, {r1-r5}
                push    {r1-r5}

                mov     r0, sp
                bl      chb_fast_save
                bl      chb_fast_load
                mov     sp, r0

                bl      RVM_Virt_Int_Unmask

                ldr     r0, =RVM_Usr_Param
                ldr     r0, [r0]
                pop     {r1-r5}
                stmia   r0, {r1-r5}
                pop     {r4-r11, lr}

#if CORTEX_USE_FPU
                tst     lr, #0x00000010
                beq     chb_fast_unstack_extended
#endif

                ldr     r0, [sp, #4*7]
                tst     r0, #0x00000200
                bne     chb_fast_unstack_basic_pad
                chb_exc_pop 8
                b       chb_fast_exit
chb_fast_unstack_basic_pad:
                chb_exc_pop 8+1
                b       chb_fast_exit

#if CORTEX_USE_FPU
chb_fast_unstack_extended:
                .short  0xECBD, 0x8A10      /* vpop {s16-s31} */
                add     r0, sp, #4*(8+16)
                ldr     r1, [r0]
                .short  0xEEE1, 0x1A10      /* vmsr fpscr, r1 */
                .short  0xED30, 0x0A10      /* vldmdb r0!, {s0-s15} */
                ldr     r0, [sp, #4*7]
                tst     r0, #0x00000200
                bne     chb_fast_unstack_extended_pad
                chb_exc_pop 8+17+1
                b       chb_fast_exit
chb_fast_unstack_extended_pad:
                chb_exc_pop 8+17+2
#endif

chb_fast_exit:
                bx      lr

chb_fast_err:
                b       chb_fast_err

#endif /* CHB_FAST_PATH */

#endif /* !defined(__DOXYGEN__) */

/** @} */
