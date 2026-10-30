//----------------------------------------------------
// Copyright (c) 2026, Evo-Devo Instrum. All rights reserved.
//
// This file is NOT part of the DjyOS distribution.  It belongs to the
// DjyOS-on-RVM port and is released under the Unlicense; refer to the
// LICENSE file for details.  The DjyOS kernel itself is copyright the
// DjyOS Open Source Development team and is not modified by this file.
//----------------------------------------------------
//Module: Scheduler CPU assembly (RVM guest)
//Author:  Evo-Devo Instrum.
//Version: V1.0.0
//Description: Replaces bsp/arch/arm/cortex-m/armv7-m/cortex-m3/src/
//             cpus.S for ARMv7(E)-M systems running as an unprivileged
//             guest of the RVM hypervisor.  The switches are performed
//             directly on the thread stacks (the masquerade-frame
//             technique of the proven RT-Thread, FreeRTOS and uC/OS-III
//             RVM ports) instead of through the SVC exception, because
//             under RVM the SVC trap belongs to the hypervisor.  The six
//             primitives keep the exact DjyOS API and calling
//             conventions.
//Notes: Frame layout, from the stack pointer saved in vm->stack upwards
//       (22 words, integer only - this port compiles the guest
//       soft-float):
//         the five RVM hypercall parameter words;
//         r4-r11;
//         the saved EXC_RETURN (0xFFFFFFFD, basic frame marker);
//         a hardware exception frame: r0,r1,r2,r3,r12,lr,pc,XPSR, with
//         one padding word below it when the entry stack was not 8 byte
//         aligned, marked by XPSR bit 9 exactly like the hardware.
//       The frames are built in three interchangeable places: by
//       __asm_reset_thread below for freshly pooled threads, by
//       __asm_switch_context below for the blocking switches, and by
//       the context switch handler in int_hard.c for the
//       hypervisor-initiated (deferred) switches.
//       DjyOS always switches with the virtual interrupts masked (its
//       scheduler runs inside Int_SaveAsynSignal), so every switch
//       below runs under the caller's mask and re-enables at the only
//       boundary where the switch state is coherent again - the
//       choreography the original SVC handler performed, translated to
//       direct switching.
//       Floating point: the frames carry the basic exception frame only,
//       so the guest is built with the soft float ABI and never executes
//       FPU instructions (the RVM coprocessor attribute may still be
//       present - the VM owns the FPU, the guest code just does not use
//       it).  A build that lets the compiler generate VFP code would
//       need the S0-S15, FPSCR and S16-S31 halves of the extended frame
//       in every path below, which this port does not implement.
//Revision history:
//1. Date: 2026-10-29
//   Author:  Evo-Devo Instrum.
//   Version: V1.0.0
//   Change: Initial version for the RVM guest.
//----------------------------------------------------

#if defined(__ARM_FP)
#error "The DjyOS RVM port builds integer-only frames; compile the guest with -mfloat-abi=soft."
#endif

    .syntax unified
    .cpu    cortex-m7
    .thumb
    .text

/* The RVM paravirtualization library. */
    .extern RVM_Usr_Param
    .extern RVM_Virt_Int_Unmask
    .extern RVM_Virt_Yield
    .extern RVM_Hyp_Int_Ena

/* The deferred-switch handshake and the dirty frame trap. */
    .extern djy_from_vm
    .extern djy_to_vm
    .extern djy_switch_flag
    .extern djy_sci_calls
    .extern djy_defer_hits
    .extern djy_unalign_hits
    .extern djy_turnto_lr
    .extern djy_turnto_vm
    .extern djy_rs_new_vm
    .extern djy_switch_err

/* The thread entry engine of the DjyOS kernel. */
    .extern __DJY_VmEngine

/* The stack frame alignment padding indicator of XPSR bit 9. */
    .equ    XPSR_ALIGN_PAD, 0x00000200
/* The EPSR ICI/IT field that only a real exception return restores. */
    .equ    XPSR_ICI_IT_MASK, 0x0600FC00

/*
@ See whether the stack pointer is 8 byte aligned.  Two words (the
@ protected r0 and the read XPSR) have been pushed above the tested
@ value at every site below, so the polarity is NE - matching the
@ ARMv7-M RVM ports, NOT the three word entry of the ARMv6-M ports.
 */
    .macro  DJY_ALIGN_PUSH label
    mov     r0, sp
    tst     r0, #0x00000007
    bne     \label              /* two words pushed above, thus NE */
    .endm

/*
@ Masquerade the calling context as if stacked by an exception entry:
@ complete the basic exception frame of \sz words around the two
@ words (r0 and XPSR) pushed at entry, with the resume point \label
@ as the pc and 0xFFFFFFFD as the EXC_RETURN.  On exit lr holds the
@ EXC_RETURN for the subsequent PUSH {R4-R11,LR}.
 */
    .macro  DJY_MASQ sz, label
    ldr     r0, [sp, #4]        /* load the real r0 value pushed at entry */
    .if     ((\sz-8)!=0)        /* reserve the alignment padding word */
    sub     sp, #4*(\sz-8)
    .endif
    push    {r0-r3, r12, lr}    /* push the frame's GP registers */
    ldr     r0, [sp, #4*(\sz-2)]/* load the real XPSR value pushed at entry */
    ldr     r1, =0x01000000
    orrs    r0, r1              /* the T flag reads as zero, set it */
    .if     (\sz==9)
    ldr     r1, =XPSR_ALIGN_PAD
    orrs    r0, r1              /* mark the padding word */
    .endif
    str     r0, [sp, #4*7]
    ldr     r0, =\label         /* the resume pc, thumb bit set: the frame */
    orr     r0, #1              /* must stay valid for a real exception    */
    str     r0, [sp, #4*6]      /* return through the hypervisor as well   */
    ldr     r1, =0xFFFFFFFD     /* make up the EXC_RETURN */
    mov     lr, r1
    .endm

/* See whether the stacked XPSR carries the 8 byte alignment padding. */
    .macro  DJY_ALIGN_POP label
    ldr     r0, [sp, #4*7]      /* load up the XPSR */
    tst     r0, #XPSR_ALIGN_PAD
    bne     \label
    .endm

/*
@ Unstack a basic exception frame of \sz words - the in-place
@ transform of the proven RVM ports, with the EPSR ICI/IT field
@ checked on the fly: the filters at the entry of every switch below
@ route dirty frames onto the deferred path, so reaching the trap
@ means the filter itself is broken.
 */
    .macro  DJY_EXC_POP sz
    ldr     lr, [sp, #4*5]      /* restore the frame's lr */
    pop     {r0}                /* load r0/XPSR/pc into r0/r1/r2 */
    ldr     r1, [sp, #4*6]
    ldr     r12, =XPSR_ICI_IT_MASK
    tst     r1, r12             /* dirty ICI/IT cannot be restored here */
    bne     djy_switch_err
    ldr     r3, =0xFFFFFDFF     /* clear XPSR[9] */
    ands    r1, r3
    ldr     r2, [sp, #4*5]
    orrs    r2, #0x00000001     /* set pc[0] */
    str     r1, [sp, #4*(\sz-4)]/* rearrange to H-pc-r0-XPSR-L */
    str     r0, [sp, #4*(\sz-3)]
    str     r2, [sp, #4*(\sz-2)]
    pop     {r1-r3, r12}        /* pop GP registers */
    .if     ((\sz-8)!=0)        /* skip the padding word when needed */
    add     sp, #4*(\sz-8)
    .endif
    pop     {r0}                /* pop the XPSR through r0 */
    msr     XPSR_NZCVQ, r0
    pop     {r0, pc}            /* pop r0 and pc */
    .endm

/*
@ The common frame-entering tail: unmask the virtual interrupts at
@ the coherent boundary, then restore the hypercall parameters, the
@ GP register bank and the hardware frame.  Entered with sp already
@ loaded from the target vm->stack and the target frame already
@ filtered clean.  The label parameter is the masquerade resume
@ point for callers that need one.
 */
    .macro  DJY_ENTER_FRAME
    bl      RVM_Virt_Int_Unmask /* the coherent boundary: re-enable */

    ldr     r0, =RVM_Usr_Param  /* pop the hypercall parameters */
    ldr     r0, [r0]
    pop     {r1-r5}
    stmia   r0!, {r1-r5}

    pop     {r4-r11, lr}        /* pop GP registers + the EXC_RETURN */
    .endm

/*
@ void __asm_reset_thread(ptu32_t (*thread_routine)(void),
@                         struct ThreadVm *vm);
@
@ Initialize the stack of a (pooled) thread and hang the engine onto
@ it: the standard 22 word port frame, with the routine in r0 because
@ __DJY_VmEngine takes it as its only argument, and the engine as the
@ initial pc.  The initial XPSR carries no padding - the pool stacks
@ are allocated 8 byte aligned by the benchmark board glue.
@ Registers r1-r11 start as 0xDEADBEEF so that any accidental use
@ shows up immediately.
 */
    .global __asm_reset_thread
    .type   __asm_reset_thread, %function
__asm_reset_thread:
    ldr     r2, [r1, #4]        /* vm->stack_top */
    sub     r2, #22*4           /* the whole port frame */
    movs    r3, #0
    str     r3, [r2, #0]        /* hypercall Number */
    str     r3, [r2, #4]        /* hypercall Param[0..3] */
    str     r3, [r2, #8]
    str     r3, [r2, #12]
    str     r3, [r2, #16]
    ldr     r3, =0xDEADBEEF     /* r4-r11 fill */
    str     r3, [r2, #20]
    str     r3, [r2, #24]
    str     r3, [r2, #28]
    str     r3, [r2, #32]
    str     r3, [r2, #36]
    str     r3, [r2, #40]
    str     r3, [r2, #44]
    str     r3, [r2, #48]
    ldr     r3, =0xFFFFFFFD     /* the EXC_RETURN marker */
    str     r3, [r2, #52]
    movs    r3, #0
    str     r3, [r2, #56]       /* r0 = the routine, set below */
    str     r3, [r2, #60]       /* r1 */
    str     r3, [r2, #64]       /* r2 */
    str     r3, [r2, #68]       /* r3 */
    str     r3, [r2, #72]       /* r12 */
    ldr     r3, =0xFFFFFFFF     /* lr: the engine never returns */
    str     r3, [r2, #76]
    ldr     r3, =__DJY_VmEngine
    str     r3, [r2, #80]       /* pc = the engine */
    ldr     r3, =0x01000000
    str     r3, [r2, #84]       /* XPSR = thumb */
    str     r0, [r2, #56]       /* r0 = thread_routine */
    str     r2, [r1]            /* vm->stack = frame bottom */
    bx      lr
    .pool

/*
@ The deferred-switch trampoline: pend the context switch through the
@ yield function and give the hypervisor the boundary it needs.  The
@ yield call only traps-and-blocks when the caller is unmasked; called
@ from the masked scheduler it merely raises the flag and returns, so
@ the unmask right after is what actually delivers: it lets the
@ hypervisor's vector loop run the registered context handler, which
@ performs the latched transfer - the caller is switched out exactly
@ at that hypercall boundary.  When the caller is later re-entered, it
@ resumes inside that same unmask (the fast-path entry has balanced
@ the mask by then), and the pop returns to the switch caller as if
@ the switch had completed synchronously - the same resume symmetry
@ the proven RT-Thread slow path uses.  The djy_to_vm/djy_from_vm
@ latch must already be set by the caller.  On the vector thread the
@ yield is a silent flag-raise and the loop tail itself performs the
@ transfer after the engine returns.
 */
    .global djy_switch_defer
    .type   djy_switch_defer, %function
djy_switch_defer:
    ldr     r2, =djy_defer_hits     /* how often the slow path is used */
    ldr     r3, [r2]
    adds    r3, #1
    str     r3, [r2]
    ldr     r2, =djy_switch_flag  /* arm the request: the context handler */
    movs    r3, #1                /* consumes exactly this latch once    */
    str     r3, [r2]
    push    {lr}
    bl      RVM_Virt_Yield
    bl      RVM_Virt_Int_Unmask
    pop     {pc}

/*
@ The dirty frame filter prologue: check the target frame's stacked
@ XPSR (word 21) for ICI/IT state before touching anything, so that
@ the bail-out happens with the current context still intact - the
@ handler then saves the real context and the kernel's exception
@ return restores the state that manual unstacking cannot.
@ Entered with r0 = target vm; leaves r0 untouched.
 */
    .macro  DJY_DIRTY_CHECK defer_label
    ldr     r2, [r0]            /* the target frame bottom */
    ldr     r2, [r2, #21*4]     /* the stacked XPSR */
    ldr     r3, =XPSR_ICI_IT_MASK
    tst     r2, r3
    bne     \defer_label
    .endm

/*
@ void __asm_turnto_context(struct ThreadVm *new_vm);
@
@ Enter the target context without saving the current one - the event
@ script has completed and its stack is abandoned (or is being reset
@ by the caller).  The target is either a freshly reset engine frame
@ or a masquerade frame of a thread that blocked earlier, so the entry
@ goes through the same alignment-aware unstack the switch uses; the
@ pair is latched first (from = NULL, nothing to save) so that a
@ delivery inside the unmask window lands in a coherent deferred
@ switch instead of a half-eaten stack.
 */
    .global __asm_turnto_context
    .type   __asm_turnto_context, %function
__asm_turnto_context:
    tst     r0, r0              /* diagnose: who passed the NULL vm */
    beq.W   djy_turnto_null
    ldr     r2, =djy_switch_flag /* this direct switch supersedes any  */
    movs    r3, #0               /* pending interrupt request          */
    str     r3, [r2]
    ldr     r2, =djy_to_vm
    str     r0, [r2]
    movs    r2, #0
    ldr     r3, =djy_from_vm
    str     r2, [r3]

    DJY_DIRTY_CHECK djy_turnto_dirty

    ldr     sp, [r0]            /* load the target frame */
    DJY_ENTER_FRAME
    DJY_ALIGN_POP djy_turnto_pad
    DJY_EXC_POP 8               /* frame without alignment padding */
    b       djy_turnto_done
djy_turnto_pad:
    DJY_EXC_POP 8+1             /* frame with alignment padding */
djy_turnto_done:
    .pool

/*
@ The dirty branch of a context entry: remember who asked and with
@ which vm, so a rejected frame can be traced to its source.
 */
djy_turnto_dirty:
    ldr     r2, =djy_turnto_lr
    str     lr, [r2]
    ldr     r2, =djy_turnto_vm
    str     r0, [r2]
    b       djy_switch_defer

/*
@ void __asm_switch_context(struct ThreadVm *new_vm,
@                           struct ThreadVm *old_vm);
@
@ Save the current context onto the old thread's stack, then enter
@ the new one - the blocking switch.  Called from the scheduler with
@ the virtual interrupts masked; the unmask happens at the new
@ context's entry, exactly where the original SVC handler did its
@ restore.  When switched back, the thread resumes at the exit label
@ below and returns to the scheduler caller with all registers as
@ they were at the call.
 */
    .global __asm_switch_context
    .type   __asm_switch_context, %function
__asm_switch_context:
    ldr     r2, =djy_switch_flag /* this direct switch supersedes any  */
    movs    r3, #0               /* pending interrupt request          */
    str     r3, [r2]
    ldr     r2, =djy_to_vm      /* latch the pair first: the args are */
    str     r0, [r2]            /* destroyed by the masquerade below, */
    ldr     r2, =djy_from_vm    /* and the latch is the deferred-path */
    str     r1, [r2]            /* handshake with the switch handler. */

    DJY_DIRTY_CHECK djy_switch_defer

    push    {r0}                /* protect r0 and the XPSR */
    mrs     r0, XPSR
    push    {r0}

    DJY_ALIGN_PUSH djy_swc_unaligned
djy_swc_aligned:
    DJY_MASQ 8, djy_swc_exit
    b       djy_swc_stacked
djy_swc_unaligned:
    ldr     r2, =djy_unalign_hits    /* padded frames must be rare */
    ldr     r3, [r2]
    adds    r3, #1
    str     r3, [r2]
    DJY_MASQ 8+1, djy_swc_exit

djy_swc_stacked:
    push    {r4-r11, lr}        /* the GP register bank + EXC_RETURN */

    ldr     r0, =RVM_Usr_Param  /* push the hypercall parameters */
    ldr     r0, [r0]
    ldmia   r0!, {r1-r5}
    push    {r1-r5}

    ldr     r1, =djy_from_vm    /* store the frame into old_vm->stack */
    ldr     r1, [r1]
    mov     r2, sp
    str     r2, [r1]

    ldr     r0, =djy_to_vm      /* enter the new context */
    ldr     r0, [r0]
    ldr     sp, [r0]
    DJY_ENTER_FRAME
    DJY_ALIGN_POP djy_swc_pad
    DJY_EXC_POP 8
    b       djy_swc_exit
djy_swc_pad:
    DJY_EXC_POP 8+1
djy_swc_exit:                   /* resumed here when switched back */
    bx      lr
    .pool

/*
@ void __asm_reset_switch(ptu32_t (*thread_routine)(void),
@                          struct ThreadVm *new_vm,
@                          struct ThreadVm *old_vm);
@
@ Reset the old thread's stack to a freshly initialized frame (the
@ keep-thread event completion path), then switch into the new one
@ without saving.
 */
    .global __asm_reset_switch
    .type   __asm_reset_switch, %function
/*
@ void __asm_reset_switch(ptu32_t (*thread_routine)(void),
@                          struct ThreadVm *new_vm,
@                          struct ThreadVm *old_vm);
@
@ Reset the old thread's stack to a freshly initialized frame (the
@ keep-thread event completion path), then switch into the new one
@ without saving.  new_vm must wait in RAM across the rebuild: the
@ stack region the rebuild writes is exactly the one below the old
@ stack top (the completing event tail-called all the way down, so sp
@ sits at the very top), and a vector delivered mid-rebuild restores
@ the interrupted thread's own r0-r3/r12 on return, so neither the
@ old stack nor a scratch register can carry the value.
 */
    .global __asm_reset_switch
    .type   __asm_reset_switch, %function
__asm_reset_switch:
    ldr     r3, =djy_rs_new_vm
    str     r1, [r3]            /* park new_vm while the frame rebuilds */
    mov     r1, r2              /* __asm_reset_thread(routine, old_vm) */
    bl      __asm_reset_thread
    ldr     r3, =djy_rs_new_vm
    ldr     r0, [r3]            /* r0 = new_vm again */
    b       __asm_turnto_context

/*
@ void __asm_switch_context_int(struct ThreadVm *new_vm,
@                                struct ThreadVm *old_vm);
@
@ Interrupt level switch, called by the async signal engine on the
@ vector thread with the new/old pair already decided.  The guest
@ cannot play exception-return games from here, so the switch is
@ deferred: latch the pair and pend the context switch; the vector
@ loop's handler performs the transfer right after this returns.
@ The latch keeps an already pending request instead of overwriting
@ it mid-flight - the same first-request-wins rule the proven RVM
@ ports use - so a delivery that lands inside another switch's
@ unmask window can never tear a half-built frame pair.
 */
    .global __asm_switch_context_int
    .type   __asm_switch_context_int, %function
__asm_switch_context_int:
    ldr     r2, =djy_sci_calls
    ldr     r3, [r2]
    adds    r3, #1
    str     r3, [r2]
    ldr     r2, =djy_switch_flag
    ldr     r3, [r2]
    cmp     r3, #0
    bne     djy_sci_pending     /* keep the in-flight pair */
    ldr     r2, =djy_to_vm
    str     r0, [r2]
    ldr     r2, =djy_from_vm
    str     r1, [r2]
    ldr     r2, =djy_switch_flag
    movs    r3, #1
    str     r3, [r2]
djy_sci_pending:
    push    {lr}
    bl      RVM_Virt_Yield      /* on the vct thread this only raises the
                                 * Ctx flag - the loop tail performs the
                                 * transfer right after the engine returns.
                                 * No unmask here: sending to our own vector
                                 * endpoint from the vector thread itself
                                 * would deadlock the delivery. */
    pop     {pc}

/*
@ void __asm_start_thread(struct ThreadVm *new_vm);
@
@ Start the multi event scheduling and enter the first event thread,
@ abandoning the startup stack.  The RVM virtual interrupts are
@ enabled here just like the official ports do before the first
@ thread runs; the startup context never returns.  The new_vm in r0
@ must be protected across the call: r0 is also the AAPCS argument
@ and return register, so the hypervisor enable would otherwise
@ replace it with the hypercall status - the original cpsie i the
@ Cortex-M3 port used here touches no register.
 */
    .global __asm_start_thread
    .type   __asm_start_thread, %function
__asm_start_thread:
    push    {r0, lr}
    bl      RVM_Hyp_Int_Ena
    pop     {r0, lr}            /* r0 = new_vm again */
    b       __asm_turnto_context

/*
@ void __asm_delay_cycle(u64 timeCoreClock);
@ A simple cycle-burning delay - the port only needs it for board
@ level bring-up helpers, precision is not critical.
 */
    .global __asm_delay_cycle
    .type   __asm_delay_cycle, %function
__asm_delay_cycle:
    lsrs    r0, r0, #6
    beq     djy_delay_done
djy_delay_loop:
    nop
    nop
    nop
    nop
    subs    r0, r0, #1
    bne     djy_delay_loop
djy_delay_done:
    bx      lr

    .pool
    .end
