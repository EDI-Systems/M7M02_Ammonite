/* SPDX-License-Identifier: Unlicense */

/**
 * @file
 *
 * @ingroup ZPR
 *
 * @brief Masquerade the current ZPR context as an exception frame.
 *
 * This is the task level half of _CPU_Context_switch().  It saves the
 * calling context in the same layout the RVM context handler and the
 * initial stack builder use, records the heir, and wakes the vector thread.
 * The physical switch itself is performed by the monitor's real exception
 * return when zpr_ctx_handler() has loaded the heir into RVM_REG.
 *
 * The frame, from the stack pointer stored in the context upwards:
 *
 *   Number, Param[0..3]
 *   R4..R11, EXC_RETURN
 *   S16..S31                 (extended only)
 *   R0..R3, R12, LR, PC, xPSR
 *   S0..S15, FPSCR, [pad]    (extended only)
 *
 * The resume PC is zpr_ctx_exit; the hardware frame's LR is the return
 * address of the C caller, so the task returns from _CPU_Context_switch()
 * when the context handler resumes it.
 */

    .syntax unified
    .thumb
    .arch   armv7-m

#include "zpr_abi.h"

    .extern zpr_pending_heir
    .extern RVM_Usr_Param
    .extern RVM_Virt_Yield
    .extern RVM_Virt_Int_Unmask
    .extern _RVM_CPU_ISR_Set_level

    .global _RVM_CPU_Context_switch_yield
    .type   _RVM_CPU_Context_switch_yield, %function


/*
 * Push the rest of the hardware frame.  sz is the hardware frame size in
 * words (8 basic, 26/27 extended); the two words pushed at entry are the
 * protected real R0 and XPSR and are consumed by the frame's top slots.
 */
    .macro  ZPR_EXC_PUSH sz, xpsr, exc_return
    ldr     r0, [sp, #4]
    .if     ((\sz-8)!=0)
    sub     sp, #4*(\sz-8)
    .endif
    push    {r0-r3, r12, lr}
    ldr     r0, [sp, #4*(\sz-2)]
    ldr     r1, =\xpsr
    orrs    r0, r1
    str     r0, [sp, #4*7]
    ldr     r0, =zpr_ctx_exit
    ldr     r1, =0xFFFFFFFE
    ands    r0, r1
    str     r0, [sp, #4*6]
    ldr     r1, =\exc_return
    mov     lr, r1
    .endm

/* See whether the frame holds an alignment padding word: bit 9 of the
 * stacked XPSR is used to mark it. */
    .macro  ZPR_ALIGN_POP label
    ldr     r0, [sp, #4*7]
    tst     r0, #0x00000200
    bne     \label
    .endm

/*
 * Manually unstack an exception frame of \sz words.  The frame is
 * transformed in place so that PC and SP are restored as the last step;
 * dirty ICI/IT bits are rejected here as a defence-in-depth check.
 */
    .macro  ZPR_EXC_POP sz
    ldr     lr, [sp, #4*5]              /* restore the frame's LR */
    pop     {r0}                        /* R0/XPSR/PC -> R0/R1/R2 */
    ldr     r1, [sp, #4*6]
    ldr     r12, =0x0600FC00
    tst     r1, r12
    bne     zpr_fast_err
    and     r1, #0xFFFFFDFF             /* clear XPSR[9] */
    ldr     r2, [sp, #4*5]
    orr     r2, #0x00000001             /* set PC[0] */
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

    .text
    .align  2
    .thumb_func
_RVM_CPU_Context_switch_yield:
    push    {r0}                    /* protect the executing pointer */
    mrs     r0, xpsr
    push    {r0}                    /* protect the real XPSR */
    mrs     r0, control
    tst     r0, #0x00000004         /* CONTROL.FPCA selects the frame type */
    bne     1f

    /* Basic frame. */
    mov     r0, sp
    tst     r0, #0x00000007
    bne     2f
    ZPR_EXC_PUSH 8, 0x01000000, 0xFFFFFFFD
    movs    r12, #36                /* hardware frame is 36 bytes above */
    b       3f
2:
    ZPR_EXC_PUSH 9, 0x01000200, 0xFFFFFFFD
    movs    r12, #36
    b       3f

1:
    /* Extended frame. */
    mov     r0, sp
    tst     r0, #0x00000007
    bne     4f
    ZPR_EXC_PUSH 26, 0x01000000, 0xFFFFFFED
    b       5f
4:
    ZPR_EXC_PUSH 27, 0x01000200, 0xFFFFFFED
5:
    add     r0, sp, #4*8            /* FP area just above the basic frame */
    .short  0xECA0, 0x0A10          /* vstmia r0!, {s0-s15} */
    .short  0xEEF1, 0x1A10          /* vmrs r1, fpscr */
    str     r1, [r0]
    .short  0xED2D, 0x8A10          /* vpush {s16-s31} */
    movs    r12, #100               /* 36 GP bytes + 64 S16-S31 bytes */

3:
    /* Save the GP registers; the hardware frame's pointers are above. */
    push    {r4-r11, lr}
    add     r3, sp, r12
    ldr     r4, [r3]                /* executing (hardware frame R0) */
    ldr     r5, [r3, #4]            /* heir (hardware frame R1) */
    ldr     r6, [r3, #8]            /* usr params (hardware frame R2) */
    ldmia   r6, {r7-r11}
    push    {r7-r11}                /* Number, Param[0..3] */

    str     sp, [r4, #ZPR_CTX_SP_OFFSET]
    ldr     r0, =zpr_pending_heir
    str     r5, [r0]

    bl      RVM_Virt_Yield
    bl      RVM_Virt_Int_Unmask

    /*
     * The signal to the vector endpoint switches away here.  This context
     * resumes at zpr_ctx_exit() through the monitor's exception return, so
     * the GP registers are restored from the frame and the C caller's ABI
     * is intact.  Reaching the loop means the signal did not switch; stay
     * put rather than running with half-updated scheduler state.
     */
    b       .

/*
 * Direct task-to-task fast path.  Entered only for a clean current context
 * and a clean heir frame.  It builds the same frame on the current stack,
 * switches SP to the heir frame, unmasks according to the heir's ISR level,
 * and manually unstacks the frame.  Dirty frames are filtered in C and never
 * reach this routine.  The whole routine vanishes when the fast path is
 * disabled, so an all-Vct build has no fast-path stack frame at all.
 */
#if defined(ZPR_FAST_PATH) && (ZPR_FAST_PATH != 0)
    .global _RVM_CPU_Context_switch_swt
    .type   _RVM_CPU_Context_switch_swt, %function
    .thumb_func
_RVM_CPU_Context_switch_swt:
    push    {r0}
    mrs     r0, xpsr
    push    {r0}
    mrs     r0, control
    tst     r0, #0x00000004
    bne     11f

    /* Basic frame. */
    mov     r0, sp
    tst     r0, #0x00000007
    bne     12f
    ZPR_EXC_PUSH 8, 0x01000000, 0xFFFFFFFD
    movs    r12, #36
    b       13f
12:
    ZPR_EXC_PUSH 9, 0x01000200, 0xFFFFFFFD
    movs    r12, #36
    b       13f

11:
    /* Extended frame. */
    mov     r0, sp
    tst     r0, #0x00000007
    bne     14f
    ZPR_EXC_PUSH 26, 0x01000000, 0xFFFFFFED
    b       15f
14:
    ZPR_EXC_PUSH 27, 0x01000200, 0xFFFFFFED
15:
    add     r0, sp, #4*8
    .short  0xECA0, 0x0A10          /* vstmia r0!, {s0-s15} */
    .short  0xEEF1, 0x1A10          /* vmrs r1, fpscr */
    str     r1, [r0]
    .short  0xED2D, 0x8A10          /* vpush {s16-s31} */
    movs    r12, #100

13:
    push    {r4-r11, lr}
    add     r3, sp, r12
    ldr     r4, [r3]                /* executing */
    ldr     r5, [r3, #4]            /* heir */
    ldr     r6, [r3, #8]            /* usr params */
    ldmia   r6, {r7-r11}
    push    {r7-r11}                /* Number, Param[0..3] */

    str     sp, [r4, #ZPR_CTX_SP_OFFSET]
    ldr     sp, [r5, #ZPR_CTX_SP_OFFSET]
    ldr     r0, [r5, #40]           /* heir isr_nest_level */
    bl      _RVM_CPU_ISR_Set_level

    /*
     * Publish the heir's hypercall parameters.  The Vct context handler
     * does the same before the monitor resumes the frame.
     */
    ldr     r0, =RVM_Usr_Param
    ldr     r0, [r0]
    ldmia   sp, {r1-r5}
    stmia   r0!, {r1-r5}

    add     sp, #4*5                /* discard the hypercall parameters */
    pop     {r4-r11, lr}
    tst     lr, #0x00000010
    bne     20f

    /* Extended frame: pop S16-S31 first, then reload S0-S15/FPSCR. */
    .short  0xECBD, 0x8A10          /* vpop {s16-s31} */
    add     r0, sp, #4*(8+16)       /* FPSCR after the basic frame */
    ldr     r1, [r0]
    .short  0xEEE1, 0x1A10          /* vmsr fpscr, r1 */
    .short  0xED30, 0x0A10          /* vldmdb r0!, {s0-s15} */
    ZPR_ALIGN_POP 21f
    ZPR_EXC_POP 26
21:
    ZPR_EXC_POP 27

20:
    ZPR_ALIGN_POP 22f
    ZPR_EXC_POP 8
22:
    ZPR_EXC_POP 9

zpr_fast_err:
    b       zpr_fast_err
#endif /* ZPR_FAST_PATH */

    .thumb_func
zpr_ctx_exit:
    bx      lr

    .ltorg

    .size   _RVM_CPU_Context_switch_yield, . - _RVM_CPU_Context_switch_yield

/* End of file. */
