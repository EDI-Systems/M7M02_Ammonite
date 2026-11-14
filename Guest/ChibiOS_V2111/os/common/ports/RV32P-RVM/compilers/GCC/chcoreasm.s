/*
    CHB port for the RVM hypervisor.
    Copyright (C) 2026 Evo-Devo Instrum. All rights reserved.

    This file is NOT part of the ChibiOS distribution.  It belongs to the
    ChibiOS-on-RVM guest port and is released under the Unlicense; refer to
    the LICENSE file for details.  The ChibiOS kernel itself is copyright
    Giovanni Di Sirio and is not modified by this file.
*/

/**
 * @file    RV32P-RVM/compilers/GCC/chcoreasm.s
 * @brief   CHB RV32P port low level code.
 *
 * The slow path is implemented in C on the vector thread.  This file
 * provides the task-level direct switch used by the CHB fast path.  It
 * builds the same frame as chb_ctx_switch(): five hypercall parameters,
 * the saved mstatus, the optional RVF register block and the 31-word
 * integer frame, from the lowest address upwards.
 *
 * x4 (tp) is used to carry the resume address, as in the FreeRTOS RV32P
 * RVM port: the user-mode code cannot write mepc, and GCC does not use
 * tp unless TLS is enabled.
 */

#if !defined(FALSE)
#define FALSE 0
#endif
#if !defined(TRUE)
#define TRUE 1
#endif

#include "chconf.h"

#if !defined(CHB_FAST_PATH)
#define CHB_FAST_PATH 0
#endif

#if CHB_FAST_PATH

    .section .text.chb_fast_switch
    .align  3
    .globl  chb_fast_switch
    .type   chb_fast_switch, @function
chb_fast_switch:
    /* Integer frame: PC, X1, X3..X31. */
    addi    sp, sp, -31*4
    sw      x31, 30*4(sp)
    sw      x30, 29*4(sp)
    sw      x29, 28*4(sp)
    sw      x28, 27*4(sp)
    sw      x27, 26*4(sp)
    sw      x26, 25*4(sp)
    sw      x25, 24*4(sp)
    sw      x24, 23*4(sp)
    sw      x23, 22*4(sp)
    sw      x22, 21*4(sp)
    sw      x21, 20*4(sp)
    sw      x20, 19*4(sp)
    sw      x19, 18*4(sp)
    sw      x18, 17*4(sp)
    sw      x17, 16*4(sp)
    sw      x16, 15*4(sp)
    sw      x15, 14*4(sp)
    sw      x14, 13*4(sp)
    sw      x13, 12*4(sp)
    sw      x12, 11*4(sp)
    sw      x11, 10*4(sp)
    sw      x10, 9*4(sp)
    sw      x9, 8*4(sp)
    sw      x8, 7*4(sp)
    sw      x7, 6*4(sp)
    sw      x6, 5*4(sp)
    sw      x5, 4*4(sp)
    sw      x3, 2*4(sp)
    sw      x1, 1*4(sp)
    la      t0, .Lchb_fast_exit
    sw      t0, 0*4(sp)

#if CHB_USE_FPU
    /* The user cannot probe mstatus.fs, so always save the RVF block and
     * mark the frame dirty.  A frame that did not carry FPU state is still
     * restored correctly because its own mstatus controls the load. */
    addi    sp, sp, -33*4
    .hword  0x25F3              /* frcsr a1 */
    .hword  0x0030
    sw      a1, 32*4(sp)
    .hword  0xFEFE              /* fsw f31, 31*4(sp) */
    .hword  0xFCFA              /* fsw f30, 30*4(sp) */
    .hword  0xFAF6              /* fsw f29, 29*4(sp) */
    .hword  0xF8F2              /* fsw f28, 28*4(sp) */
    .hword  0xF6EE              /* fsw f27, 27*4(sp) */
    .hword  0xF4EA              /* fsw f26, 26*4(sp) */
    .hword  0xF2E6              /* fsw f25, 25*4(sp) */
    .hword  0xF0E2              /* fsw f24, 24*4(sp) */
    .hword  0xEEDE              /* fsw f23, 23*4(sp) */
    .hword  0xECDA              /* fsw f22, 22*4(sp) */
    .hword  0xEAD6              /* fsw f21, 21*4(sp) */
    .hword  0xE8D2              /* fsw f20, 20*4(sp) */
    .hword  0xE6CE              /* fsw f19, 19*4(sp) */
    .hword  0xE4CA              /* fsw f18, 18*4(sp) */
    .hword  0xE2C6              /* fsw f17, 17*4(sp) */
    .hword  0xE0C2              /* fsw f16, 16*4(sp) */
    .hword  0xFE3E              /* fsw f15, 15*4(sp) */
    .hword  0xFC3A              /* fsw f14, 14*4(sp) */
    .hword  0xFA36              /* fsw f13, 13*4(sp) */
    .hword  0xF832              /* fsw f12, 12*4(sp) */
    .hword  0xF62E              /* fsw f11, 11*4(sp) */
    .hword  0xF42A              /* fsw f10, 10*4(sp) */
    .hword  0xF226              /* fsw f9, 9*4(sp) */
    .hword  0xF022              /* fsw f8, 8*4(sp) */
    .hword  0xEE1E              /* fsw f7, 7*4(sp) */
    .hword  0xEC1A              /* fsw f6, 6*4(sp) */
    .hword  0xEA16              /* fsw f5, 5*4(sp) */
    .hword  0xE812              /* fsw f4, 4*4(sp) */
    .hword  0xE60E              /* fsw f3, 3*4(sp) */
    .hword  0xE40A              /* fsw f2, 2*4(sp) */
    .hword  0xE206              /* fsw f1, 1*4(sp) */
    .hword  0xE002              /* fsw f0, 0*4(sp) */
    li      t0, 0x6088          /* mstatus: FS dirty, back to U-mode. */
#else
    li      t0, 0x0088          /* mstatus: no FPU, back to U-mode. */
#endif
    addi    sp, sp, -4
    sw      t0, 0*4(sp)

    /* Save the hypercall parameter area below the frame. */
    addi    sp, sp, -5*4
    la      t0, RVM_Usr_Param
    lw      t0, 0*4(t0)
    lw      t1, 0*4(t0)
    sw      t1, 0*4(sp)
    lw      t1, 1*4(t0)
    sw      t1, 1*4(sp)
    lw      t1, 2*4(t0)
    sw      t1, 2*4(sp)
    lw      t1, 3*4(t0)
    sw      t1, 3*4(sp)
    lw      t1, 4*4(t0)
    sw      t1, 4*4(sp)

    /* Store the outgoing frame and fetch the heir's. */
    mv      a0, sp
    andi    sp, sp, -16
    call    chb_fast_save
    call    chb_fast_load
    mv      sp, a0

    /* Restore the hypercall parameter area of the heir. */
    la      t0, RVM_Usr_Param
    lw      t0, 0*4(t0)
    lw      t1, 0*4(sp)
    sw      t1, 0*4(t0)
    lw      t1, 1*4(sp)
    sw      t1, 1*4(t0)
    lw      t1, 2*4(sp)
    sw      t1, 2*4(t0)
    lw      t1, 3*4(sp)
    sw      t1, 3*4(t0)
    lw      t1, 4*4(sp)
    sw      t1, 4*4(t0)
    addi    sp, sp, 5*4

    lw      t0, 0*4(sp)         /* Heir mstatus. */
    addi    sp, sp, 4

#if CHB_USE_FPU
    li      t1, 0x4000
    and     t1, t1, t0
    beqz    t1, .Lchb_fast_load_skip
    .hword  0x6002              /* flw f0, 0*4(sp) */
    .hword  0x6092              /* flw f1, 1*4(sp) */
    .hword  0x6122              /* flw f2, 2*4(sp) */
    .hword  0x61B2              /* flw f3, 3*4(sp) */
    .hword  0x6242              /* flw f4, 4*4(sp) */
    .hword  0x62D2              /* flw f5, 5*4(sp) */
    .hword  0x6362              /* flw f6, 6*4(sp) */
    .hword  0x63F2              /* flw f7, 7*4(sp) */
    .hword  0x7402              /* flw f8, 8*4(sp) */
    .hword  0x7492              /* flw f9, 9*4(sp) */
    .hword  0x7522              /* flw f10, 10*4(sp) */
    .hword  0x75B2              /* flw f11, 11*4(sp) */
    .hword  0x7642              /* flw f12, 12*4(sp) */
    .hword  0x76D2              /* flw f13, 13*4(sp) */
    .hword  0x7762              /* flw f14, 14*4(sp) */
    .hword  0x77F2              /* flw f15, 15*4(sp) */
    .hword  0x6806              /* flw f16, 16*4(sp) */
    .hword  0x6896              /* flw f17, 17*4(sp) */
    .hword  0x6926              /* flw f18, 18*4(sp) */
    .hword  0x69B6              /* flw f19, 19*4(sp) */
    .hword  0x6A46              /* flw f20, 20*4(sp) */
    .hword  0x6AD6              /* flw f21, 21*4(sp) */
    .hword  0x6B66              /* flw f22, 22*4(sp) */
    .hword  0x6BF6              /* flw f23, 23*4(sp) */
    .hword  0x7C06              /* flw f24, 24*4(sp) */
    .hword  0x7C96              /* flw f25, 25*4(sp) */
    .hword  0x7D26              /* flw f26, 26*4(sp) */
    .hword  0x7DB6              /* flw f27, 27*4(sp) */
    .hword  0x7E46              /* flw f28, 28*4(sp) */
    .hword  0x7ED6              /* flw f29, 29*4(sp) */
    .hword  0x7F66              /* flw f30, 30*4(sp) */
    .hword  0x7FF6              /* flw f31, 31*4(sp) */
    lw      a1, 32*4(sp)
    .hword  0x9073              /* fscsr a1 */
    .hword  0x0035
    addi    sp, sp, 33*4
.Lchb_fast_load_skip:
#endif

    /* Keep the exact integer-frame pointer across the alignment adjustment
     * needed by the C call; s0 is restored from the frame below. */
    mv      s0, sp
    andi    sp, sp, -16
    call    RVM_Virt_Int_Unmask
    mv      sp, s0

    lw      tp, 0*4(sp)         /* Resume address; x4 is not restored. */
    lw      x1, 1*4(sp)
    lw      x3, 2*4(sp)
    lw      x5, 4*4(sp)
    lw      x6, 5*4(sp)
    lw      x7, 6*4(sp)
    lw      x8, 7*4(sp)
    lw      x9, 8*4(sp)
    lw      x10, 9*4(sp)
    lw      x11, 10*4(sp)
    lw      x12, 11*4(sp)
    lw      x13, 12*4(sp)
    lw      x14, 13*4(sp)
    lw      x15, 14*4(sp)
    lw      x16, 15*4(sp)
    lw      x17, 16*4(sp)
    lw      x18, 17*4(sp)
    lw      x19, 18*4(sp)
    lw      x20, 19*4(sp)
    lw      x21, 20*4(sp)
    lw      x22, 21*4(sp)
    lw      x23, 22*4(sp)
    lw      x24, 23*4(sp)
    lw      x25, 24*4(sp)
    lw      x26, 25*4(sp)
    lw      x27, 26*4(sp)
    lw      x28, 27*4(sp)
    lw      x29, 28*4(sp)
    lw      x30, 29*4(sp)
    lw      x31, 30*4(sp)
    addi    sp, sp, 31*4
    jalr    zero, tp

.Lchb_fast_exit:
    ret

    .size   chb_fast_switch, .-chb_fast_switch

#endif /* CHB_FAST_PATH */

/** @} */
