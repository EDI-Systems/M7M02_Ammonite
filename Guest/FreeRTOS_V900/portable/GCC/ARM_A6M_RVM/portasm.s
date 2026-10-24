/*
 * FreeRTOS V9.0.0 RVM port - Copyright (C) 2026 Evo-Devo Instrum.
 * All rights reserved.
 *
 * This file is NOT part of the FreeRTOS distribution.  It belongs to the
 * FreeRTOS-on-RVM test suite and is released under the Unlicense; refer to
 * the LICENSE file for details.  The FreeRTOS kernel itself is copyright
 * Real Time Engineers Ltd. and is not modified by this file.
 *
 * FreeRTOS is free software; you can redistribute it and/or modify it under
 * the terms of the GNU General Public License (version 2) as published by
 * the Free Software Foundation >>>> AND MODIFIED BY <<<< the FreeRTOS
 * exception.
 *
 * This file implements the assembly part of the porting layer for ARMv6-M
 * systems running as an unprivileged guest of the RVM hypervisor: the
 * fast-path yield routine.
 */

/*-----------------------------------------------------------*/

/* Symbols provided by the scheduler and the RVM paravirtualization
 * library. */
    .extern         vTaskSwitchContext     /* The scheduler. */
    .extern         pxCurrentTCB           /* The TCB of the task to run. */
    .extern         RVM_Usr_Param          /* The user hypercall parameter block. */
    .extern         RVM_Virt_Int_Mask      /* Mask the RVM virtual interrupts. */
    .extern         RVM_Virt_Int_Unmask    /* Unmask the RVM virtual interrupts. */

/* Entry point implemented in this file, selected by the C wrapper
 * vPortYield() in port.c when configUSE_RVM_OPTIMISED_YIELD is enabled.
 * When that setting is disabled the yields go through the RVM hypercall
 * instead and this routine is not referenced at all. */
    .global         vPortYield_Asm

/*-----------------------------------------------------------*/

/*
 * vPortYield_Asm yields the CPU from one task to another without a trap
 * through the hypervisor: the scheduler runs directly on the task stack.
 * The routine masquerades the current context as if it were stacked by
 * an exception entry, so the very same frame can also be recovered by
 * the interrupt driven switches through xPortPendSVHandler() in port.c.
 *
 * The exception frame built here, from the stack pointer up:
 *
 *   Unaligned           Aligned
 *   Reserved
 *   XPSR                XPSR
 *   PC                  PC
 *   LR                  LR
 *   R12                 R12
 *   R3                  R3
 *   R2                  R2
 *   R1                  R1
 *   R0                  R0
 *
 * Caveats:
 * 1. The register stacking may be nested by an interrupt that arrives
 *    before the interrupts are masked.  This is allowed, but must be
 *    taken into account when dimensioning the task stacks.
 * 2. User-level code cannot probe the interrupt state, hence nothing can
 *    be short-circuited before the masking takes effect.
 */

    .syntax unified
    .arch   armv6s-m
    .thumb

/* See whether the stack pointer is 8 byte aligned.  TST SP is
 * unpredictable on ARMv6-M, so SP has to go through a register first. */
    .macro  portALIGN_PUSH label
    mov     r0, sp
    ldr     r1, =0x00000007
    tst     r0, r1
    beq     \label              /* Three words pushed above, thus EQ. */
    .endm

/* Stack the remaining part of a basic exception frame of \sz words, with
 * the given XPSR and EXC_RETURN, and the resume point as the PC. */
    .macro  portEXC_PUSH sz, xpsr, exc_return
    ldr     r0, [sp, #4*2]      /* Load real R0/R1 values pushed at start. */
    ldr     r1, [sp]
    sub     sp, #4*(\sz-7)      /* Adjust SP to push GP registers. */
    push    {r0-r3}             /* Push stack frame GP registers. */
    mov     r0, r12
    mov     r1, lr
    str     r0, [sp, #4*4]
    str     r1, [sp, #4*5]
    ldr     r0, [sp, #4*(\sz-2)]/* Load real XPSR value pushed at start. */
    ldr     r1, =\xpsr
    orrs    r0, r1
    str     r0, [sp, #4*7]
    ldr     r0, =vPortYield_Exit/* Push PC with bit[0] cleared. */
    ldr     r1, =0xFFFFFFFE
    ands    r0, r1
    str     r0, [sp, #4*6]
    ldr     r1, =\exc_return    /* Make up the EXC_RETURN. */
    mov     lr, r1
    .endm

/* See whether the stacked XPSR carries the 8 byte alignment padding. */
    .macro  portALIGN_POP label
    ldr     r0, [sp, #4*7]      /* Load up XPSR. */
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
    .macro  portEXC_POP sz
    ldr     r0, [sp, #4*4]      /* Restore LR/R12. */
    ldr     r1, [sp, #4*5]
    mov     r12, r0
    mov     lr, r1
    pop     {r0}                /* Load R0/XPSR/PC into R0/R1/R2. */
    ldr     r1, [sp, #4*6]
    ldr     r3, =0xFFFFFDFF     /* Clear XPSR[9]. */
    ands    r1, r3
    ldr     r2, [sp, #4*5]
    ldr     r3, =0x00000001     /* Set PC[0]. */
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

/*-----------------------------------------------------------*/

/*
 * The fast-path yield entry point - see the header comment above.
 */
    .section        .text.vPortYield
    .align          3
    .thumb_func

vPortYield_Asm:
    push    {r0}                /* Protect R0, XPSR and R1. */
    mrs     r0, xpsr
    push    {r0}
    push    {r1}

    portALIGN_PUSH vPortYield_Unaligned
vPortYield_Aligned:             /* Originally 8 byte aligned stack. */
    portEXC_PUSH 8, 0x01000000, 0xFFFFFFFD
    b       vPortYield_Stacked
vPortYield_Unaligned:           /* Originally unaligned stack. */
    portEXC_PUSH 8+1, 0x01000200, 0xFFFFFFFD

vPortYield_Stacked:
    mov     r3, r11             /* Push GP registers. */
    mov     r2, r10
    mov     r1, r9
    mov     r0, r8
    push    {r0-r3, lr}
    push    {r4-r7}

    ldr     r0, =RVM_Usr_Param  /* Push the hypercall parameters. */
    ldr     r0, [r0]
    ldmia   r0!, {r1-r5}
    push    {r1-r5}

    bl      RVM_Virt_Int_Mask   /* Disable interrupts. */

    ldr     r1, =pxCurrentTCB   /* Load pxCurrentTCB. */
    ldr     r1, [r1]
    mov     r2, sp              /* Write sp to first TCB member. */
    str     r2, [r1]
    bl      vTaskSwitchContext  /* Select the highest priority ready task. */
    ldr     r1, =pxCurrentTCB   /* Load pxCurrentTCB. */
    ldr     r1, [r1]
    ldr     r1, [r1]            /* Read sp from first TCB member. */
    mov     sp, r1

    bl      RVM_Virt_Int_Unmask /* Enable interrupts. */

    ldr     r0, =RVM_Usr_Param  /* Pop the hypercall parameters. */
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

    portALIGN_POP vPortYield_PadSkip
vPortYield_NoPad:               /* Frame without alignment padding. */
    portEXC_POP 8
vPortYield_PadSkip:             /* Frame with alignment padding. */
    portEXC_POP 8+1

vPortYield_Exit:                /* Exit location. */
    bx      lr
