/*
    CHB port for the RVM hypervisor.
    Copyright (C) 2026 Evo-Devo Instrum. All rights reserved.

    This file is NOT part of the ChibiOS distribution. It belongs to the
    ChibiOS-on-RVM guest port and is released under the Unlicense; refer to
    the LICENSE file for details.  The ChibiOS kernel itself is copyright
    Giovanni Di Sirio and is not modified by this file.
*/

/**
 * @file    ARMv6-M-RVM/compilers/IAR/chcoreasm.s
 * @brief   CHB port low level code for IAR EWARM.
 *
 * The frame layout and the register choreography are identical to the GCC
 * port of this architecture: a basic 22 word masqueraded frame, R8-R11
 * travelling through R0-R3, and every non-byte immediate loaded as a
 * literal.  iasmarm macro handling is fragile, so the aligned and unaligned
 * frame paths are expanded inline instead of using macros.  See the GCC
 * file for the annotated algorithm.
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

                MODULE  ?chcoreasm_v6m

                PRESERVE8

                SECTION .text:CODE:NOROOT(2)

                EXTERN  chb_thread_start
                EXTERN  chb_fast_save
                EXTERN  chb_fast_load
                EXTERN  RVM_Usr_Param
                EXTERN  RVM_Virt_Int_Unmask

                THUMB

/* The first-entry trampoline stored in a new thread's basic frame. */
                PUBLIC  __port_thread_start
__port_thread_start:
                mov     r0, r4
                mov     r1, r5
                bl      chb_thread_start
chb_thread_start_hang:
                b       chb_thread_start_hang

#if CHB_FAST_PATH

/* Read the current xPSR.  The ICI/IT field always reads zero on ARMv6-M;
   the routine keeps the C side identical to the ARMv7-M port. */
                PUBLIC  chb_get_xpsr
chb_get_xpsr:
                mrs     r0, xpsr
                bx      lr

/*
 * Direct task-to-task switch.  The hardware frame is [PAD] XPSR PC LR R12
 * R3-R0 from the stack pointer upwards; entry protects R0, XPSR and R1 by
 * three pushes, so the frame build reads them back from there.  Both the
 * alignment cases are expanded inline.
 */
                PUBLIC  chb_fast_switch
chb_fast_switch:
                push    {r0}
                mrs     r0, xpsr
                push    {r0}
                push    {r1}

                mov     r0, sp
                ldr     r1, =0x00000007
                tst     r0, r1
                beq     chb_fast_unaligned

; Originally 8 byte aligned: one word of room for R12/LR/PC/XPSR.
chb_fast_aligned:
                ldr     r0, [sp, #8]
                ldr     r1, [sp]
                sub     sp, sp, #4
                push    {r0-r3}
                mov     r0, r12
                mov     r1, lr
                str     r0, [sp, #16]
                str     r1, [sp, #20]
                ldr     r0, [sp, #24]
                ldr     r1, =0x01000000
                orrs    r0, r0, r1
                str     r0, [sp, #28]
                ldr     r0, =chb_fast_exit
                ldr     r1, =0xFFFFFFFE
                ands    r0, r0, r1
                str     r0, [sp, #24]
                ldr     r1, =0xFFFFFFFD
                mov     lr, r1
                b       chb_fast_stacked

; Originally unaligned: two words of room and the pad marker in XPSR.
chb_fast_unaligned:
                ldr     r0, [sp, #8]
                ldr     r1, [sp]
                sub     sp, sp, #8
                push    {r0-r3}
                mov     r0, r12
                mov     r1, lr
                str     r0, [sp, #16]
                str     r1, [sp, #20]
                ldr     r0, [sp, #28]
                ldr     r1, =0x01000200
                orrs    r0, r0, r1
                str     r0, [sp, #28]
                ldr     r0, =chb_fast_exit
                ldr     r1, =0xFFFFFFFE
                ands    r0, r0, r1
                str     r0, [sp, #24]
                ldr     r1, =0xFFFFFFFD
                mov     lr, r1

chb_fast_stacked:
                push    {lr}
                mov     r0, r8
                mov     r1, r9
                mov     r2, r10
                mov     r3, r11
                push    {r0-r3}
                push    {r4-r7}

                ldr     r0, =RVM_Usr_Param
                ldr     r0, [r0]
                ldmia   r0!, {r1-r5}
                push    {r1-r5}

                mov     r0, sp
                bl      chb_fast_save
                bl      chb_fast_load
                mov     sp, r0

                bl      RVM_Virt_Int_Unmask

                ldr     r0, =RVM_Usr_Param
                ldr     r0, [r0]
                pop     {r1-r5}
                stmia   r0!, {r1-r5}

                pop     {r4-r7}
                pop     {r0-r3}
                mov     r8, r0
                mov     r9, r1
                mov     r10, r2
                mov     r11, r3
                pop     {r0}
                mov     lr, r0

                ldr     r0, [sp, #28]
                ldr     r1, =0x00000200
                tst     r0, r1
                bne     chb_fast_pop9

; Unstack the 8 word hardware frame in place.
chb_fast_pop8:
                ldr     r0, [sp, #16]
                ldr     r1, [sp, #20]
                mov     r12, r0
                mov     lr, r1
                pop     {r0}
                ldr     r1, [sp, #24]
                ldr     r3, =0xFFFFFDFF
                ands    r1, r1, r3
                ldr     r2, [sp, #20]
                movs    r3, #1
                orrs    r2, r2, r3
                str     r1, [sp, #16]
                str     r0, [sp, #20]
                str     r2, [sp, #24]
                pop     {r1-r3}
                add     sp, sp, #4
                pop     {r0}
                msr     APSR_nzcvq, r0
                pop     {r0, pc}

; Unstack the 9 word hardware frame (alignment pad above XPSR).
chb_fast_pop9:
                ldr     r0, [sp, #16]
                ldr     r1, [sp, #20]
                mov     r12, r0
                mov     lr, r1
                pop     {r0}
                ldr     r1, [sp, #24]
                ldr     r3, =0xFFFFFDFF
                ands    r1, r1, r3
                ldr     r2, [sp, #20]
                movs    r3, #1
                orrs    r2, r2, r3
                str     r1, [sp, #20]
                str     r0, [sp, #24]
                str     r2, [sp, #28]
                pop     {r1-r3}
                add     sp, sp, #8
                pop     {r0}
                msr     APSR_nzcvq, r0
                pop     {r0, pc}

chb_fast_exit:
                bx      lr

#endif /* CHB_FAST_PATH */

#endif /* !defined(__DOXYGEN__) */

                END
