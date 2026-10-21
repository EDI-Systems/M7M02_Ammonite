/*
 * FreeRTOS V9.0.0 RVM port - Copyright (C) 2026 Evo-Devo Instrum.
 * All rights reserved.
 *
 * This file is NOT part of the FreeRTOS distribution.  It belongs to the
 * FreeRTOS-on-RVM test suite and is released under the Unlicense; refer
 * to the LICENSE file for details.  The FreeRTOS kernel itself is copyright
 * Real Time Engineers Ltd. and is not modified by this file.
 *
 * FreeRTOS is free software; you can redistribute it and/or modify it under
 * the terms of the GNU General Public License (version 2) as published by
 * the Free Software Foundation >>>> AND MODIFIED BY <<<< the FreeRTOS
 * exception.
 *
 * This file implements the assembly part of the porting layer for ARMv7-M
 * systems running as an unprivileged guest of the RVM hypervisor: the
 * fast-path yield routine.
 */

/*-----------------------------------------------------------*/

/* Symbols provided by the scheduler, the RVM paravirtualization library
 * and the port itself. */
    .extern         vTaskSwitchContext     /* The scheduler. */
    .extern         pxCurrentTCB           /* The TCB of the task to run. */
    .extern         RVM_Usr_Param          /* The user hypercall parameter block. */
    .extern         RVM_Virt_Int_Unmask    /* Unmask the RVM virtual interrupts. */
    .extern         vPortYield_Err         /* The dirty ICI/IT trap in port.c. */

/* Entry point implemented in this file, called by vPortYield() in port.c
 * when configUSE_RVM_OPTIMISED_YIELD is enabled and the peeked target
 * task holds a clean ICI/IT state.  The caller holds the virtual interrupt
 * mask throughout the peek, and this routine unmasks the interrupts itself
 * once the new stack has been loaded.  When that setting is disabled the
 * yields go through the RVM hypercall instead and this routine is not
 * referenced at all. */
    .global         vPortYield_Swt

/*-----------------------------------------------------------*/

/*
 * vPortYield_Swt switches the CPU from one task to another without a trap
 * through the hypervisor: the scheduler runs directly on the task stack.
 * The routine masquerades the current context as if it were stacked by an
 * exception entry, so the very same frame can also be recovered by the
 * interrupt driven switches through xPortPendSVHandler() in port.c.
 *
 * The exception frame built here, from the stack pointer up:
 *
 *   Basic                  Extended (CONTROL.FPCA set)
 *   XPSR                   [PAD]
 *   PC                     FPSCR
 *   LR                     S15 - S0
 *   R12                    XPSR
 *   R3                     PC
 *   R2                     LR
 *   R1                     R12
 *   R0                     R3 - R0
 *   LR_EXC (0xFFFFFFFD)    LR_EXC (0xFFFFFFED)
 *   S31 - S16
 *   R11 - R4
 *   Hypercall parameters
 *
 * The PC pushed in the frame is the exit label below, and the LR is the
 * return address of the caller of vPortYield(), so a task saved here and
 * later resumed through the interrupt path continues exactly where the
 * fast path would have returned to.
 *
 * Caveats:
 * 1. User-level code cannot clear CONTROL.FPCA (this is correct - if it
 *    could, then there would be a serious security loophole) hence all
 *    threads in the system are tainted with the FPU active flag once any
 *    of them touches the FPU, and will carry a full context save/restore.
 *    This is still hugely faster than the slow path through the
 *    hypervisor.
 * 2. The EPSR ICI/IT bits of the frame being unstacked are checked on the
 *    fly: a frame holding interrupted IT block or LDM/STM continuation
 *    state can only be restored by a real exception return, which the
 *    manual unstacking below cannot perform.  Such frames are filtered
 *    out by vPortYield() beforehand, so hitting vPortYield_Err() here
 *    indicates an internal port error.
 */

    .syntax unified
    .arch   armv7-m
    .thumb

/* See whether the stack pointer is 8 byte aligned.  TST SP is
 * unpredictable on ARMv7-M, so SP has to go through a register first. */
    .macro  portALIGN_PUSH label
    mov     r0, sp
    tst     r0, #0x00000007
    bne     \label              /* Two words pushed above, thus NE. */
    .endm

/* Stack the remaining part of an exception frame of \sz words, with the
 * given XPSR bits and EXC_RETURN, and the exit label as the PC.  The two
 * words pushed at the entry double as the frame's XPSR/PC slots when the
 * frame is basic, and sit just above it otherwise. */
    .macro  portEXC_PUSH sz, xpsr, exc_return
    ldr     r0, [sp, #4]        /* Load the real R0 value pushed at entry. */
    .if     ((\sz-8)!=0)        /* Reserve the frame's upper words. */
    sub     sp, #4*(\sz-8)
    .endif
    push    {r0-r3, r12, lr}    /* Push the frame's GP registers. */
    ldr     r0, [sp, #4*(\sz-2)]/* Load the real XPSR value pushed at entry. */
    ldr     r1, =\xpsr
    orr     r0, r1
    str     r0, [sp, #4*7]      /* Merge in the flags and the padding mark. */
    ldr     r0, =vPortYield_Exit/* Push the resume PC with bit[0] cleared. */
    and     r0, #0xFFFFFFFE
    str     r0, [sp, #4*6]
    mov     lr, #\exc_return    /* Make up the EXC_RETURN. */
    .endm


/* See whether the frame holds an alignment padding word: bit 9 of the
 * stacked XPSR is used to mark it. */
    .macro  portALIGN_POP label
    ldr     r0, [sp, #4*7]      /* Load up the XPSR. */
    tst     r0, #0x00000200
    bne     \label
    .endm

/* Unstack an exception frame of \sz words.  The original sequence is
 * [PAD] [FPU] XPSR PC LR R12 R3-R0, which is not ideal for a manual
 * restoring, because restoring PC and SP simultaneously must be the very
 * last step.  Thus the frame is transformed in place first:
 * 1. With [PAD] or [FPU]: [PAD] [FPU] XPSR PC LR R12 R3-R0 -> PC R0 XPSR [EMPTY] R12 R3-R1 LR
 * 2. Without either:      XPSR PC LR R12 R3-R0             -> PC R0 XPSR R12 R3-R1 LR
 * The transformation restores LR first so it cannot be overwritten, pops
 * R0, loads XPSR and PC and rearranges them at the end, pops R1-R3 and LR
 * while skipping the middle, and finally restores XPSR through R0, then
 * R0 and PC.  Nothing is ever placed below the current SP, because a
 * racing interrupt would clobber it. */
    .macro  portEXC_POP sz
    ldr     lr, [sp, #4*5]      /* Restore the frame's LR. */
    pop     {r0}                /* Load R0/XPSR/PC into R0/R1/R2. */
    ldr     r1, [sp, #4*6]
    ldr     r12, =0x0600FC00    /* The EPSR ICI/IT field. */
    tst     r1, r12             /* Dirty ICI/IT cannot be restored here. */
    bne     vPortYield_Err      /* Defense in depth - see the header. */
    and     r1, #0xFFFFFDFF     /* Clear XPSR[9]. */
    ldr     r2, [sp, #4*5]
    orr     r2, #0x00000001     /* Set PC[0]. */
    str     r1, [sp, #4*(\sz-4)]/* Rearrange to H-PC-R0-XPSR-L. */
    str     r0, [sp, #4*(\sz-3)]
    str     r2, [sp, #4*(\sz-2)]
    pop     {r1-r3, r12}        /* Pop the GP registers. */
    .if     ((\sz-8)!=0)        /* Skip the FPU area and the padding. */
    add     sp, #4*(\sz-8)
    .endif
    pop     {r0}                /* Pop the XPSR through R0. */
    msr     xpsr_nzcvq, r0
    pop     {r0, pc}            /* Pop R0 and PC. */
    .endm


/*
 * The fast-path switch entry point - see the header comment above.
 */
    .section        .text.vPortYield_Swt
    .align          3
    .thumb_func

vPortYield_Swt:                 /* Entered with the virtual interrupts
                                 * already masked by vPortYield(). */
    push    {r0}                /* Protect R0 and XPSR. */
    mrs     r0, xpsr
    push    {r0}
    mrs     r0, control
    tst     r0, #0x00000004     /* CONTROL.FPCA - the frame type. */
    bne     vPortYield_Extended

vPortYield_Basic:               /* Basic frame. */
    portALIGN_PUSH vPortYield_Basic_Unaligned
vPortYield_Basic_Aligned:       /* Originally 8 byte aligned stack. */
    portEXC_PUSH 8, 0x01000000, 0xFFFFFFFD
    b       vPortYield_Stacked
vPortYield_Basic_Unaligned:     /* Originally unaligned stack. */
    portEXC_PUSH 8+1, 0x01000200, 0xFFFFFFFD
    b       vPortYield_Stacked

vPortYield_Extended:            /* Extended frame. */
    portALIGN_PUSH vPortYield_Extended_Unaligned
vPortYield_Extended_Aligned:    /* Originally 8 byte aligned stack. */
    portEXC_PUSH 8+17+1, 0x01000000, 0xFFFFFFED
    b       vPortYield_Extended_Saved
vPortYield_Extended_Unaligned:  /* Originally unaligned stack. */
    portEXC_PUSH 8+17+2, 0x01000200, 0xFFFFFFED
vPortYield_Extended_Saved:      /* Extended frame extra pushing. */
    add     r0, sp, #4*8        /* Locate the FPU stacking area. */
    .short  0xECA0, 0x0A10      /* vstmia r0!, {s0-s15} */
    .short  0xEEF1, 0x1A10      /* vmrs r1, fpscr */
    str     r1, [r0]
    .short  0xED2D, 0x8A10      /* vpush {s16-s31} */

vPortYield_Stacked:
    push    {r4-r11, lr}        /* Push GP registers. */

    ldr     r0, =RVM_Usr_Param  /* Push the hypercall parameters. */
    ldr     r0, [r0]
    ldmia   r0, {r1-r5}
    push    {r1-r5}

    ldr     r1, =pxCurrentTCB   /* Save sp to the first TCB member. */
    ldr     r1, [r1]
    str     sp, [r1]
    bl      vTaskSwitchContext  /* Select the highest priority ready task. */
    ldr     r1, =pxCurrentTCB   /* Load sp from the first TCB member. */
    ldr     r1, [r1]
    ldr     sp, [r1]

    bl      RVM_Virt_Int_Unmask /* Enable interrupts. */

    ldr     r0, =RVM_Usr_Param  /* Pop the hypercall parameters. */
    ldr     r0, [r0]
    pop     {r1-r5}
    stmia   r0, {r1-r5}
    pop     {r4-r11, lr}        /* Pop GP registers. */

    tst     lr, #0x00000010     /* LR.EXTENDED - the frame type. */
    beq     vPortYield_Unstack_Extended

vPortYield_Unstack_Basic:       /* Basic frame. */
    portALIGN_POP vPortYield_Unstack_Basic_Pad
vPortYield_Unstack_Basic_NoPad: /* Frame without alignment padding. */
    portEXC_POP 8
    b       vPortYield_Exit
vPortYield_Unstack_Basic_Pad:   /* Frame with alignment padding. */
    portEXC_POP 8+1
    b       vPortYield_Exit

vPortYield_Unstack_Extended:    /* Extended frame extra popping. */
    .short  0xECBD, 0x8A10      /* vpop {s16-s31} */
    add     r0, sp, #4*(8+16)
    ldr     r1, [r0]
    .short  0xEEE1, 0x1A10      /* vmsr fpscr, r1 */
    .short  0xED30, 0x0A10      /* vldmdb r0!, {s0-s15} */
    portALIGN_POP vPortYield_Unstack_Extended_Pad
vPortYield_Unstack_Extended_NoPad:  /* Frame without alignment padding. */
    portEXC_POP 8+17+1
    b       vPortYield_Exit
vPortYield_Unstack_Extended_Pad:    /* Frame with alignment padding. */
    portEXC_POP 8+17+2

vPortYield_Exit:                /* Exit location - also the resume PC. */
    bx      lr

