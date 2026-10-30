//----------------------------------------------------
// Copyright (c) 2026, Evo-Devo Instrum. All rights reserved.
//
// This file is NOT part of the DjyOS distribution.  It belongs to the
// DjyOS-on-RVM port and is released under the Unlicense; refer to the
// LICENSE file for details.  The DjyOS kernel itself is copyright the
// DjyOS Open Source Development team and is not modified by this file.
//----------------------------------------------------
//Module: Interrupt module assembly (RVM guest)
//Author:  Evo-Devo Instrum.
//Version: V1.0.0
//Description: Replaces bsp/arch/arm/cortex-m/armv7-m/cortex-m3/src/
//             ints.S for ARMv7(E)-M systems running as an unprivileged
//             guest of the RVM hypervisor.  The original file
//             manipulates BASEPRI and PRIMASK, which are physical CPU
//             registers the guest has no right to touch - and which
//             would not gate the RVM virtual interrupt delivery anyway.
//             Both atomic tiers therefore map onto the single RVM
//             virtual interrupt mask, with snapshot semantics preserved
//             exactly.
//Notes: The __start_asyn_signal/__start_real vector entries of the
//       original are not needed here: under RVM the hypervisor
//       dispatches vectors as plain function calls on its vector
//       thread, so the engines are entered directly from the C
//       trampolines in int_hard.c.
//Revision history:
//1. Date: 2026-10-29
//   Author:  Evo-Devo Instrum.
//   Version: V1.0.0
//   Change: Initial version for the RVM guest.
//----------------------------------------------------

    .syntax unified
    .cpu    cortex-m7
    .thumb
    .text

    .extern RVM_Int_Mask
    .extern RVM_Virt_Int_Mask
    .extern RVM_Virt_Int_Unmask

/*
@ atom_low_t Int_LowAtomStart(void);
@ Snapshot the current mask state, then mask.  The original reads and
@ raises BASEPRI; here the snapshot is the RVM mask flag and the
@ masking goes through the hypercall.  Idempotent, like the original.
@ The snapshot is carried in r4 - a callee-saved register - because
@ the masking call (and any vector delivered on top of it) clobbers
@ the caller-saved r0-r3/r12 the value would otherwise live in.
 */
    .global Int_LowAtomStart
    .type   Int_LowAtomStart, %function
Int_LowAtomStart:
    push    {r4, lr}
    ldr     r1, =RVM_Int_Mask
    ldr     r4, [r1]            /* the snapshot to return */
    bl      RVM_Virt_Int_Mask
    mov     r0, r4
    pop     {r4, pc}

/*
@ void Int_LowAtomEnd(atom_low_t low);
@ Restore the snapshotted state: a zero snapshot unmasks, a nonzero
@ one means an outer tier is still holding the mask - leave it.
 */
    .global Int_LowAtomEnd
    .type   Int_LowAtomEnd, %function
Int_LowAtomEnd:
    cbz     r0, Int_LowAtomEnd_Unmask
    bx      lr
Int_LowAtomEnd_Unmask:
    push    {lr}
    bl      RVM_Virt_Int_Unmask
    pop     {pc}

/*
@ atom_high_t Int_HighAtomStart(void);
@ The high tier maps onto the same single virtual mask - there is no
@ second mask level a guest could distinguish, and the DjyOS rule
@ that the high tier forbids system calls inside its scope keeps the
@ mapping honest (nothing inside a high atom observes the tier).
@ The snapshot survives in r4 for the same reason as the low tier.
 */
    .global Int_HighAtomStart
    .type   Int_HighAtomStart, %function
Int_HighAtomStart:
    push    {r4, lr}
    ldr     r1, =RVM_Int_Mask
    ldr     r4, [r1]
    bl      RVM_Virt_Int_Mask
    mov     r0, r4
    pop     {r4, pc}

/*
@ void Int_HighAtomEnd(atom_high_t high);
 */
    .global Int_HighAtomEnd
    .type   Int_HighAtomEnd, %function
Int_HighAtomEnd:
    cbz     r0, Int_HighAtomEnd_Unmask
    bx      lr
Int_HighAtomEnd_Unmask:
    push    {lr}
    bl      RVM_Virt_Int_Unmask
    pop     {pc}

    .pool
    .end
