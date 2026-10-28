/*
 * RT-Thread V4.1.1 RVM port - Copyright (C) 2026 Evo-Devo Instrum.
 * All rights reserved
 *
 * This file is NOT part of the RT-Thread distribution.  It belongs to the
 * RT-Thread-on-RVM test suite and is released under the Unlicense; refer
 * to the LICENSE file for details.  The RT-Thread kernel itself is
 * copyright RT-Thread Development Team and is not modified by this file.
 *
 * RT-Thread is free software; you can redistribute it and/or modify it
 * under the terms of the Apache License 2.0 as published by The Apache
 * Software Foundation.

 * Change Logs:
 * Date           Author       Notes
 * 2026-10-27     Evo-Devo     first version for the RV32P RVM guest.
 */

/**
 * @addtogroup rv32p_rvm
 */
/*@{*/

/* *
 * This file implements the assembly part of the RT-Thread porting layer
 * for RV32 systems running as an unprivileged guest of the RVM
 * hypervisor: the fast-path context switch and the startup of the first
 * thread.
 */

/*
 * Note(s)    : (1) The thread stack frames built by these routines, from
 *                  the stack pointer stored in the thread control block
 *                  upwards:
 *
 *                    the five RVM hypercall parameter words;
 *                    the saved mstatus, whose FS field records whether
 *                    the frame contains FPU registers;
 *                    the S0-S31 block and the FCSR - 33 words, only when
 *                    the saved mstatus carries FS dirty;
 *                    the integer registers - the resume pc, ra, gp, tp
 *                    and x5 through x31 - 31 words, with x2/sp implied
 *                  by the frame itself.
 *
 *                  The frames are identical to the ones that
 *                  PendSV_Handler() in cpuport.c builds for the
 *                  interrupt driven switches, and to the ones that
 *                  rt_hw_stack_init() lays down for the never-run
 *                  threads, so all three producers stay
 *                  interchangeable.
 *
 *              (2) x4/tp carries the resume address on the way back
 *                  because mepc cannot be reached from user mode -
 *                  GCC and LLVM never allocate it, so it is safe to
 *                  trash.  The RISC-V calling convention requires a 16
 *                  byte aligned stack whenever doubleword values are
 *                  passed, which never happens along the hypercall call
 *                  chain, so the frames may stay unaligned.
 *
 *              (3) The single-precision FPU registers and the FCSR are
 *                  always pushed on the fast path - the FS state cannot
 *                  be probed from user mode, so the switch cannot know
 *                  whether the thread dirtied them - and the frame is
 *                  marked FS dirty.  On the way back the FPU registers
 *                  are only restored when the frame switched to
 *                  actually carries them, which the FS field of its
 *                  saved mstatus records.  The FPU instructions are
 *                  emitted as raw halfwords so this file also assembles
 *                  for targets without an FPU.
 */

/* Symbols provided by the RVM paravirtualization library. */
    .extern        RVM_Usr_Param
    .extern        RVM_Virt_Int_Mask
    .extern        RVM_Virt_Int_Unmask
    .extern        RVM_Hyp_Int_Ena
    .extern        rt_interrupt_from_thread
    .extern        rt_interrupt_to_thread
    .extern        rt_thread_switch_interrupt_flag

/* The mstatus value that marks a frame as carrying FPU registers -
 * FS dirty (11) with the virtual interrupts enabled in user mode. */
    .equ           RTT_MSTATUS_UMODE,       0x0088
    .equ           RTT_MSTATUS_FS_DIRTY,    0x6000

/*
 * Save the integer registers of the current thread onto its stack along
 * with the address of the point where the thread shall resume, then
 * mask the virtual interrupts - they cannot stay enabled while the
 * switch runs because their enable flag lives in the RVM state block
 * rather than in the hardware.
 */
    .macro         RTT_SAVE_CONTEXT label
    addi    sp, sp, -31*4
    sw    x31, 30*4(sp)
    sw    x30, 29*4(sp)
    sw    x29, 28*4(sp)
    sw    x28, 27*4(sp)
    sw    x27, 26*4(sp)
    sw    x26, 25*4(sp)
    sw    x25, 24*4(sp)
    sw    x24, 23*4(sp)
    sw    x23, 22*4(sp)
    sw    x22, 21*4(sp)
    sw    x21, 20*4(sp)
    sw    x20, 19*4(sp)
    sw    x19, 18*4(sp)
    sw    x18, 17*4(sp)
    sw    x17, 16*4(sp)
    sw    x16, 15*4(sp)
    sw    x15, 14*4(sp)
    sw    x14, 13*4(sp)
    sw    x13, 12*4(sp)
    sw    x12, 11*4(sp)
    sw    x11, 10*4(sp)
    sw    x10, 9*4(sp)
    sw    x9, 8*4(sp)
    sw    x8, 7*4(sp)
    sw    x7, 6*4(sp)
    sw    x6, 5*4(sp)
    sw    x5, 4*4(sp)
    sw    x4, 3*4(sp)
    sw    x3, 2*4(sp)
    sw    x1, 1*4(sp)
    la    a0, \label        /* Save the resume address as the pc. */
    sw    a0, 0*4(sp)
    call  RVM_Virt_Int_Mask /* Disable interrupts. */
    .endm

/*
 * Restore the integer registers of the thread being switched to from
 * its stack frame, resume at the address that RTT_SAVE_CONTEXT stashed
 * into tp, and unmask the virtual interrupts again.  x4/tp is not
 * restored - it was trashed to carry the resume address.
 */
    .macro         RTT_RESTORE_CONTEXT
    call  RVM_Virt_Int_Unmask /* Enable interrupts. */
    lw    tp, 0*4(sp)        /* Load the pc - x4/tp is trashed for this. */
    lw    x1, 1*4(sp)        /* Load the registers, skipping x4/tp. */
    lw    x3, 2*4(sp)
    lw    x5, 4*4(sp)
    lw    x6, 5*4(sp)
    lw    x7, 6*4(sp)
    lw    x8, 7*4(sp)
    lw    x9, 8*4(sp)
    lw    x10, 9*4(sp)
    lw    x11, 10*4(sp)
    lw    x12, 11*4(sp)
    lw    x13, 12*4(sp)
    lw    x14, 13*4(sp)
    lw    x15, 14*4(sp)
    lw    x16, 15*4(sp)
    lw    x17, 16*4(sp)
    lw    x18, 17*4(sp)
    lw    x19, 18*4(sp)
    lw    x20, 19*4(sp)
    lw    x21, 20*4(sp)
    lw    x22, 21*4(sp)
    lw    x23, 22*4(sp)
    lw    x24, 23*4(sp)
    lw    x25, 24*4(sp)
    lw    x26, 25*4(sp)
    lw    x27, 26*4(sp)
    lw    x28, 27*4(sp)
    lw    x29, 28*4(sp)
    lw    x30, 29*4(sp)
    lw    x31, 30*4(sp)
    addi  sp, sp, 31*4
    jalr  x0, tp
    .endm

/*
 * void rt_hw_context_switch_to(rt_ubase_t to);
 * a0 --> to - the address of the to-thread's stack pointer slot
 *
 * Starts the first thread.  The tick and the context switch handler
 * have already been hooked up by the board initialization, and the
 * virtual interrupts are enabled here just like the official ports
 * enable them at the machine level before the first thread runs.  The
 * initial stack frame is laid down by rt_hw_stack_init() in cpuport.c;
 * its five hypercall parameter words and its mstatus are simply
 * skipped, because the first thread starts with a clean hypercall
 * state and its floating point state is marked initial.  The startup
 * stack is abandoned.
 */
    .section       .text.rt_hw_context_switch_to
    .align         2
    .global        rt_hw_context_switch_to
rt_hw_context_switch_to:
    /* store the to-thread sp slot address into the port global, exactly
     * like the official ports do - PendSV_Handler() in cpuport.c reads
     * this global as well, and nobody else ever sets it */
    la    t0, rt_interrupt_to_thread
    sw    a0, 0(t0)

    /* no thread has ever been switched out yet */
    li    t1, 0
    la    t0, rt_interrupt_from_thread
    sw    t1, 0(t0)
    la    t0, rt_thread_switch_interrupt_flag
    sw    t1, 0(t0)

    /* enable the virtual interrupts at the hypervisor level */
    call  RVM_Hyp_Int_Ena

    la    t0, rt_interrupt_to_thread
    lw    t0, 0(t0)
    lw    sp, 0(t0)          /* The first thread's stack pointer. */

    addi  sp, sp, 6*4        /* Skip the hypercall parameters and the mstatus. */

    lw    tp, 0*4(sp)        /* Load the pc - x4/tp is trashed for this. */
    lw    x1, 1*4(sp)        /* Load the registers, skipping x4/tp. */
    lw    x3, 2*4(sp)
    lw    x5, 4*4(sp)
    lw    x6, 5*4(sp)
    lw    x7, 6*4(sp)
    lw    x8, 7*4(sp)
    lw    x9, 8*4(sp)
    lw    x10, 9*4(sp)
    lw    x11, 10*4(sp)
    lw    x12, 11*4(sp)
    lw    x13, 12*4(sp)
    lw    x14, 13*4(sp)
    lw    x15, 14*4(sp)
    lw    x16, 15*4(sp)
    lw    x17, 16*4(sp)
    lw    x18, 17*4(sp)
    lw    x19, 18*4(sp)
    lw    x20, 19*4(sp)
    lw    x21, 20*4(sp)
    lw    x22, 21*4(sp)
    lw    x23, 22*4(sp)
    lw    x24, 23*4(sp)
    lw    x25, 24*4(sp)
    lw    x26, 25*4(sp)
    lw    x27, 26*4(sp)
    lw    x28, 27*4(sp)
    lw    x29, 28*4(sp)
    lw    x30, 29*4(sp)
    lw    x31, 30*4(sp)
    addi  sp, sp, 31*4
    jalr  x0, tp             /* Branch to the thread entry. */

/*
 * void rt_hw_context_switch_swt(rt_ubase_t from, rt_ubase_t to);
 * a0 --> from - the address of the from-thread's stack pointer slot
 * a1 --> to   - the address of the to-thread's stack pointer slot
 *
 * The raw fast-path switch, entered from rt_hw_context_switch() in the
 * C layer with the virtual interrupts already masked by the scheduler's
 * critical section.  rt_schedule() has already selected the to-thread,
 * so the switch is performed right here, directly on the thread stacks:
 * the current context is saved in the same frame layout that
 * PendSV_Handler() builds, the from-slot receives the saved stack
 * pointer, and the to-slot provides the frame to restore.  The from/to
 * slot addresses are taken from the port globals rather than from the
 * entry arguments, because the a0/a1 argument registers are trashed by
 * the saving sequence below - every register is, but all of them are
 * reloaded from the frame on the way back anyway.
 */
    .section       .text.rt_hw_context_switch_swt
    .align         2
    .global        rt_hw_context_switch_swt
rt_hw_context_switch_swt:
    RTT_SAVE_CONTEXT rt_hw_context_switch_swt_exit

    addi  sp, sp, -33*4      /* The FPU context, always saved. */
    .hword        0x25F3     /* frcsr a1 */
    .hword        0x0030
    sw    a1, 32*4(sp)
    .hword        0xFEFE     /* fsw f31, 31*4(sp) */
    .hword        0xFCFA     /* fsw f30, 30*4(sp) */
    .hword        0xFAF6     /* fsw f29, 29*4(sp) */
    .hword        0xF8F2     /* fsw f28, 28*4(sp) */
    .hword        0xF6EE     /* fsw f27, 27*4(sp) */
    .hword        0xF4EA     /* fsw f26, 26*4(sp) */
    .hword        0xF2E6     /* fsw f25, 25*4(sp) */
    .hword        0xF0E2     /* fsw f24, 24*4(sp) */
    .hword        0xEEDE     /* fsw f23, 23*4(sp) */
    .hword        0xECDA     /* fsw f22, 22*4(sp) */
    .hword        0xEAD6     /* fsw f21, 21*4(sp) */
    .hword        0xE8D2     /* fsw f20, 20*4(sp) */
    .hword        0xE6CE     /* fsw f19, 19*4(sp) */
    .hword        0xE4CA     /* fsw f18, 18*4(sp) */
    .hword        0xE2C6     /* fsw f17, 17*4(sp) */
    .hword        0xE0C2     /* fsw f16, 16*4(sp) */
    .hword        0xFE3E     /* fsw f15, 15*4(sp) */
    .hword        0xFC3A     /* fsw f14, 14*4(sp) */
    .hword        0xFA36     /* fsw f13, 13*4(sp) */
    .hword        0xF832     /* fsw f12, 12*4(sp) */
    .hword        0xF62E     /* fsw f11, 11*4(sp) */
    .hword        0xF42A     /* fsw f10, 10*4(sp) */
    .hword        0xF226     /* fsw f9, 9*4(sp) */
    .hword        0xF022     /* fsw f8, 8*4(sp) */
    .hword        0xEE1E     /* fsw f7, 7*4(sp) */
    .hword        0xEC1A     /* fsw f6, 6*4(sp) */
    .hword        0xEA16     /* fsw f5, 5*4(sp) */
    .hword        0xE812     /* fsw f4, 4*4(sp) */
    .hword        0xE60E     /* fsw f3, 3*4(sp) */
    .hword        0xE40A     /* fsw f2, 2*4(sp) */
    .hword        0xE206     /* fsw f1, 1*4(sp) */
    .hword        0xE002     /* fsw f0, 0*4(sp) */
    li    a2, RTT_MSTATUS_FS_DIRTY | RTT_MSTATUS_UMODE

    /* the switch body: stash the mstatus and the hypercall parameters
     * below the frame, write the frame pointer into the from-slot, then
     * take the to-slot's frame */
    addi  sp, sp, -4         /* Save the mstatus. */
    sw    a2, 0*4(sp)
    addi  sp, sp, -5*4       /* Push the hypercall parameters. */
    la    a2, RVM_Usr_Param
    lw    a2, (a2)
    lw    a3, 4*4(a2)
    sw    a3, 4*4(sp)
    lw    a3, 3*4(a2)
    sw    a3, 3*4(sp)
    lw    a3, 2*4(a2)
    sw    a3, 2*4(sp)
    lw    a3, 1*4(a2)
    sw    a3, 1*4(sp)
    lw    a3, 0*4(a2)
    sw    a3, 0*4(sp)

    la    a2, rt_interrupt_from_thread  /* The from-slot address - reloaded */
    lw    a2, 0(a2)                     /* from the port globals, because */
    sw    sp, 0(a2)                     /* a0/a1 did not survive the save. */
    la    a2, rt_interrupt_to_thread    /* The to-slot provides the frame. */
    lw    a2, 0(a2)
    lw    sp, 0(a2)

    la    a2, RVM_Usr_Param  /* Pop the hypercall parameters. */
    lw    a2, (a2)
    lw    a3, 0*4(sp)
    sw    a3, 0*4(a2)
    lw    a3, 1*4(sp)
    sw    a3, 1*4(a2)
    lw    a3, 2*4(sp)
    sw    a3, 2*4(a2)
    lw    a3, 3*4(sp)
    sw    a3, 3*4(a2)
    lw    a3, 4*4(sp)
    sw    a3, 4*4(a2)
    addi  sp, sp, 5*4
    lw    a2, 0*4(sp)        /* Read the mstatus to decide the FPU status. */
    addi  sp, sp, 4

    lui   a3, 4              /* See if FPU is used (mstatus.fs[1]==1). */
    and   a3, a3, a2
    beqz  a3, rt_hw_context_switch_swt_loadskip
    .hword        0x6002     /* flw f0, 0*4(sp) */
    .hword        0x6092     /* flw f1, 1*4(sp) */
    .hword        0x6122     /* flw f2, 2*4(sp) */
    .hword        0x61B2     /* flw f3, 3*4(sp) */
    .hword        0x6242     /* flw f4, 4*4(sp) */
    .hword        0x62D2     /* flw f5, 5*4(sp) */
    .hword        0x6362     /* flw f6, 6*4(sp) */
    .hword        0x63F2     /* flw f7, 7*4(sp) */
    .hword        0x7402     /* flw f8, 8*4(sp) */
    .hword        0x7492     /* flw f9, 9*4(sp) */
    .hword        0x7522     /* flw f10, 10*4(sp) */
    .hword        0x75B2     /* flw f11, 11*4(sp) */
    .hword        0x7642     /* flw f12, 12*4(sp) */
    .hword        0x76D2     /* flw f13, 13*4(sp) */
    .hword        0x7762     /* flw f14, 14*4(sp) */
    .hword        0x77F2     /* flw f15, 15*4(sp) */
    .hword        0x6806     /* flw f16, 16*4(sp) */
    .hword        0x6896     /* flw f17, 17*4(sp) */
    .hword        0x6926     /* flw f18, 18*4(sp) */
    .hword        0x69B6     /* flw f19, 19*4(sp) */
    .hword        0x6A46     /* flw f20, 20*4(sp) */
    .hword        0x6AD6     /* flw f21, 21*4(sp) */
    .hword        0x6B66     /* flw f22, 22*4(sp) */
    .hword        0x6BF6     /* flw f23, 23*4(sp) */
    .hword        0x7C06     /* flw f24, 24*4(sp) */
    .hword        0x7C96     /* flw f25, 25*4(sp) */
    .hword        0x7D26     /* flw f26, 26*4(sp) */
    .hword        0x7DB6     /* flw f27, 27*4(sp) */
    .hword        0x7E46     /* flw f28, 28*4(sp) */
    .hword        0x7ED6     /* flw f29, 29*4(sp) */
    .hword        0x7F66     /* flw f30, 30*4(sp) */
    .hword        0x7FF6     /* flw f31, 31*4(sp) */
    lw    a3, 32*4(sp)
    .hword        0x9073     /* fscsr a3 */
    .hword        0x0035
    addi  sp, sp, 33*4
rt_hw_context_switch_swt_loadskip:
    RTT_RESTORE_CONTEXT      /* Enable interrupts and restore registers. */
rt_hw_context_switch_swt_exit:
    ret
