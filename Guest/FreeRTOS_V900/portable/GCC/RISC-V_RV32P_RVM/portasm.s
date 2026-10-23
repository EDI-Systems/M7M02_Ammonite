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
 * This file implements the assembly part of the porting layer for RV32
 * systems running as an unprivileged guest of the RVM hypervisor: the
 * fast-path yield routines and the startup of the first task.
 */

/*-----------------------------------------------------------*/

/* Symbols provided by the linker script, the scheduler and the RVM
 * paravirtualization library. */
    .extern        __RVM_Global        /* The gp value for kernel calls. */
    .extern        __RVM_Stack        /* The sp value for kernel calls. */
    .extern        vTaskSwitchContext    /* The scheduler. */
    .extern        pxCurrentTCB        /* The TCB of the task to run. */
    .extern        RVM_Usr_Param        /* The user hypercall parameter block. */
    .extern        RVM_Virt_Int_Mask    /* Mask the RVM virtual interrupts. */
    .extern        RVM_Virt_Int_Unmask    /* Unmask the RVM virtual interrupts. */
    .extern        RVM_Hyp_Int_Ena        /* Enable the VM interrupts. */

/* Entry points implemented in this file.  The yield routines come in the
 * three coprocessor flavours that portYIELD() selects from in portmacro.h
 * when configUSE_RVM_OPTIMISED_YIELD is enabled: no coprocessor,
 * single-precision FPU (RV32F) and double-precision FPU (RV32D).  When
 * that setting is disabled the yields go through the RVM hypercall
 * instead and these routines are not referenced at all. */
    .global        xPortStartFirstTask
    .global        vPortYield
    .global        vPortYieldRVF
    .global        vPortYieldRVD

/*-----------------------------------------------------------*/

/*
 * The task stack frames built by these routines, from the stack pointer
 * upwards:
 *
 *   0*4(sp) .. 30*4(sp)  the integer registers (pc, x1, x3 .. x31)
 *   above those, when the task dirtied its FPU and the port was built
 *   with one, 33 (RVF) or 65 (RVD) more words of FPU registers + FCSR
 *   -1*4(sp)             the saved mstatus, whose FS field records
 *                        whether the frame contains FPU registers
 *   -2*4(sp) .. -6*4(sp) the RVM user hypercall parameter area
 *
 * x2 (sp) is implied by the frame itself, and x4 (tp) carries the resume
 * address on the way back because mepc cannot be reached from user mode
 * - GCC and LLVM never allocate it, so it is safe to trash.  The frames
 * are identical to the ones that xPortCtxHandler() in port.c builds for
 * the interrupt driven switches.  The RISC-V calling convention requires
 * a 16 byte aligned stack whenever doubleword values are passed, which
 * never happens along the RVM_Virt_Int_Mask/RVM_Virt_Int_Unmask call
 * chain, so the frames may stay unaligned.
 */
/*
 * Save the integer registers of the current task onto its stack along
 * with the address of the point where the task shall resume, then mask
 * the virtual interrupts - they cannot stay enabled while the scheduler
 * runs because their enable flag lives in the RVM state block rather
 * than in the hardware.
 */
    .macro    portSAVE_CONTEXT label
    addi    sp, sp, -31*4        /* Save the registers. */
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
    call    RVM_Virt_Int_Mask    /* Disable interrupts. */
    .endm

/*
 * Hand the CPU over to another task through the scheduler: stash the
 * mstatus passed in a0 along with the hypercall parameters below the
 * frame, switch onto the kernel stack with the kernel gp, run
 * vTaskSwitchContext(), then reload the frame of the task that was
 * selected.  The mstatus of the frame switched to is returned in a0 -
 * its FS field decides whether FPU registers follow the frame.
 */
    .macro    portSWITCH_CONTEXT
    addi    sp, sp, -4        /* Save the mstatus. */
    sw    a0, 0*4(sp)
    addi    sp, sp, -5*4        /* Push the hypercall parameters. */
    la    a0, RVM_Usr_Param
    lw    a0, (a0)
    lw    a1, 4*4(a0)
    sw    a1, 4*4(sp)
    lw    a1, 3*4(a0)
    sw    a1, 3*4(sp)
    lw    a1, 2*4(a0)
    sw    a1, 2*4(sp)
    lw    a1, 1*4(a0)
    sw    a1, 1*4(sp)
    lw    a1, 0*4(a0)
    sw    a1, 0*4(sp)
    .option    push
    .option    norelax
    la    gp, __RVM_Global    /* gp for the kernel - from the linker script. */
    .option    pop

    lw    a0, pxCurrentTCB    /* Load pxCurrentTCB. */
    sw    sp, 0(a0)        /* Write sp to first TCB member. */
    la    sp, __RVM_Stack        /* sp for the kernel - from the linker script. */
    call    vTaskSwitchContext    /* Select the highest priority ready task. */
    lw    a0, pxCurrentTCB    /* Load pxCurrentTCB. */
    lw    sp, 0(a0)        /* Read sp from first TCB member. */

    la    a0, RVM_Usr_Param
    lw    a0, (a0)        /* Pop the hypercall parameters. */
    lw    a1, 0*4(sp)
    sw    a1, 0*4(a0)
    lw    a1, 1*4(sp)
    sw    a1, 1*4(a0)
    lw    a1, 2*4(sp)
    sw    a1, 2*4(a0)
    lw    a1, 3*4(sp)
    sw    a1, 3*4(a0)
    lw    a1, 4*4(sp)
    sw    a1, 4*4(a0)
    addi    sp, sp, 5*4
    lw    a0, 0*4(sp)        /* Read the mstatus to decide the FPU status. */
    addi    sp, sp, 4
    .endm

/*
 * Restore the integer registers of the task being switched to from its
 * stack frame, resume at the address that portSAVE_CONTEXT stashed
 * into tp, and unmask the virtual interrupts again.  x4 (tp) is not
 * restored - it was trashed to carry the resume address.
 */
    .macro    portRESTORE_CONTEXT
    call    RVM_Virt_Int_Unmask    /* Enable interrupts. */
    lw    tp, 0*4(sp)        /* Load the pc - x4 (tp) is trashed for this. */
    lw    x1, 1*4(sp)        /* Load the registers, skipping x4 (tp). */
    lw    x3, 2*4(sp)
    /* x4 (tp) is intentionally skipped. */
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
    addi    sp, sp, 31*4
    jalr    zero, tp
    .endm

/*-----------------------------------------------------------*/

/*
 * Yield the CPU from one task to another through the RVM fast-path: the
 * scheduler runs directly on the task stack and no trap through the
 * hypervisor is taken.  This variant stores no coprocessor state and
 * marks the frame with FS off, so no FPU registers are restored when
 * the task is resumed.
 */
    .section    .text.vPortYield
    .align        3

vPortYield:
    portSAVE_CONTEXT vPortYield_Exit
    li    a0, 0x0088        /* mstatus for no FPU, back to U-mode. */
    portSWITCH_CONTEXT        /* Do the context switch. */
    portRESTORE_CONTEXT        /* Enable interrupts and restore registers. */
vPortYield_Exit:
    ret

/*-----------------------------------------------------------*/

/*
 * Same as vPortYield, but for targets with a single-precision FPU: all
 * 32 FPU registers and FCSR are always pushed below the integer frame -
 * the FS state cannot be probed from user mode, so the yield cannot
 * know whether the task dirtied them - and the frame is marked FS
 * dirty.  On the way back the FPU registers are only restored when the
 * frame switched to actually carries them, which the FS field of its
 * saved mstatus records.  The FPU instructions are emitted as raw
 * halfwords so this file also assembles for targets without an FPU.
 */
    .section    .text.vPortYieldRVF
    .align        3

vPortYieldRVF:
    portSAVE_CONTEXT vPortYieldRVF_Exit
    addi    sp, sp, -33*4        /* The FPU context, always saved. */
    .hword        0x25F3        /* frcsr a1 */
    .hword        0x0030
    sw    a1, 32*4(sp)
    .hword        0xFEFE        /* fsw f31, 31*4(sp) */
    .hword        0xFCFA        /* fsw f30, 30*4(sp) */
    .hword        0xFAF6        /* fsw f29, 29*4(sp) */
    .hword        0xF8F2        /* fsw f28, 28*4(sp) */
    .hword        0xF6EE        /* fsw f27, 27*4(sp) */
    .hword        0xF4EA        /* fsw f26, 26*4(sp) */
    .hword        0xF2E6        /* fsw f25, 25*4(sp) */
    .hword        0xF0E2        /* fsw f24, 24*4(sp) */
    .hword        0xEEDE        /* fsw f23, 23*4(sp) */
    .hword        0xECDA        /* fsw f22, 22*4(sp) */
    .hword        0xEAD6        /* fsw f21, 21*4(sp) */
    .hword        0xE8D2        /* fsw f20, 20*4(sp) */
    .hword        0xE6CE        /* fsw f19, 19*4(sp) */
    .hword        0xE4CA        /* fsw f18, 18*4(sp) */
    .hword        0xE2C6        /* fsw f17, 17*4(sp) */
    .hword        0xE0C2        /* fsw f16, 16*4(sp) */
    .hword        0xFE3E        /* fsw f15, 15*4(sp) */
    .hword        0xFC3A        /* fsw f14, 14*4(sp) */
    .hword        0xFA36        /* fsw f13, 13*4(sp) */
    .hword        0xF832        /* fsw f12, 12*4(sp) */
    .hword        0xF62E        /* fsw f11, 11*4(sp) */
    .hword        0xF42A        /* fsw f10, 10*4(sp) */
    .hword        0xF226        /* fsw f9, 9*4(sp) */
    .hword        0xF022        /* fsw f8, 8*4(sp) */
    .hword        0xEE1E        /* fsw f7, 7*4(sp) */
    .hword        0xEC1A        /* fsw f6, 6*4(sp) */
    .hword        0xEA16        /* fsw f5, 5*4(sp) */
    .hword        0xE812        /* fsw f4, 4*4(sp) */
    .hword        0xE60E        /* fsw f3, 3*4(sp) */
    .hword        0xE40A        /* fsw f2, 2*4(sp) */
    .hword        0xE206        /* fsw f1, 1*4(sp) */
    .hword        0xE002        /* fsw f0, 0*4(sp) */
    li    a0, 0x6088        /* mstatus with FS dirty, back to U-mode. */
    portSWITCH_CONTEXT        /* Do the context switch. */
    lui    a1, 4            /* See if FPU is used (mstatus.fs[1]==1). */
    and    a1, a1, a0
    beqz    a1, vPortYieldRVF_LoadSkip
    .hword        0x6002        /* flw f0, 0*4(sp) */
    .hword        0x6092        /* flw f1, 1*4(sp) */
    .hword        0x6122        /* flw f2, 2*4(sp) */
    .hword        0x61B2        /* flw f3, 3*4(sp) */
    .hword        0x6242        /* flw f4, 4*4(sp) */
    .hword        0x62D2        /* flw f5, 5*4(sp) */
    .hword        0x6362        /* flw f6, 6*4(sp) */
    .hword        0x63F2        /* flw f7, 7*4(sp) */
    .hword        0x7402        /* flw f8, 8*4(sp) */
    .hword        0x7492        /* flw f9, 9*4(sp) */
    .hword        0x7522        /* flw f10, 10*4(sp) */
    .hword        0x75B2        /* flw f11, 11*4(sp) */
    .hword        0x7642        /* flw f12, 12*4(sp) */
    .hword        0x76D2        /* flw f13, 13*4(sp) */
    .hword        0x7762        /* flw f14, 14*4(sp) */
    .hword        0x77F2        /* flw f15, 15*4(sp) */
    .hword        0x6806        /* flw f16, 16*4(sp) */
    .hword        0x6896        /* flw f17, 17*4(sp) */
    .hword        0x6926        /* flw f18, 18*4(sp) */
    .hword        0x69B6        /* flw f19, 19*4(sp) */
    .hword        0x6A46        /* flw f20, 20*4(sp) */
    .hword        0x6AD6        /* flw f21, 21*4(sp) */
    .hword        0x6B66        /* flw f22, 22*4(sp) */
    .hword        0x6BF6        /* flw f23, 23*4(sp) */
    .hword        0x7C06        /* flw f24, 24*4(sp) */
    .hword        0x7C96        /* flw f25, 25*4(sp) */
    .hword        0x7D26        /* flw f26, 26*4(sp) */
    .hword        0x7DB6        /* flw f27, 27*4(sp) */
    .hword        0x7E46        /* flw f28, 28*4(sp) */
    .hword        0x7ED6        /* flw f29, 29*4(sp) */
    .hword        0x7F66        /* flw f30, 30*4(sp) */
    .hword        0x7FF6        /* flw f31, 31*4(sp) */
    lw    a1, 32*4(sp)
    .hword        0x9073        /* fscsr a1 */
    .hword        0x0035
    addi    sp, sp, 33*4
vPortYieldRVF_LoadSkip:
    portRESTORE_CONTEXT        /* Enable interrupts and restore registers. */
vPortYieldRVF_Exit:
    ret

/*-----------------------------------------------------------*/

/*
 * Same as vPortYieldRVF, but for targets with a double-precision FPU:
 * the 32 FPU registers occupy two words each, so the FPU part of the
 * frame is 65 words wide.  The FPU instructions are emitted as raw
 * halfword pairs for the same compatibility reasons.
 */
    .section    .text.vPortYieldRVD
    .align        3

vPortYieldRVD:
    portSAVE_CONTEXT vPortYieldRVD_Exit
    addi    sp, sp, -65*4        /* The FPU context, always saved. */
    .hword        0x25F3        /* frcsr a1 */
    .hword        0x0030
    sw    a1, 32*8(sp)
    .hword        0x3C27        /* fsd f31, 31*8(sp) */
    .hword        0x0FF1
    .hword        0x3827        /* fsd f30, 30*8(sp) */
    .hword        0x0FE1
    .hword        0x3427        /* fsd f29, 29*8(sp) */
    .hword        0x0FD1
    .hword        0x3027        /* fsd f28, 28*8(sp) */
    .hword        0x0FC1
    .hword        0x3C27        /* fsd f27, 27*8(sp) */
    .hword        0x0DB1
    .hword        0x3827        /* fsd f26, 26*8(sp) */
    .hword        0x0DA1
    .hword        0x3427        /* fsd f25, 25*8(sp) */
    .hword        0x0D91
    .hword        0x3027        /* fsd f24, 24*8(sp) */
    .hword        0x0D81
    .hword        0x3C27        /* fsd f23, 23*8(sp) */
    .hword        0x0B71
    .hword        0x3827        /* fsd f22, 22*8(sp) */
    .hword        0x0B61
    .hword        0x3427        /* fsd f21, 21*8(sp) */
    .hword        0x0B51
    .hword        0x3027        /* fsd f20, 20*8(sp) */
    .hword        0x0B41
    .hword        0x3C27        /* fsd f19, 19*8(sp) */
    .hword        0x0931
    .hword        0x3827        /* fsd f18, 18*8(sp) */
    .hword        0x0921
    .hword        0x3427        /* fsd f17, 17*8(sp) */
    .hword        0x0911
    .hword        0x3027        /* fsd f16, 16*8(sp) */
    .hword        0x0901
    .hword        0x3C27        /* fsd f15, 15*8(sp) */
    .hword        0x06F1
    .hword        0x3827        /* fsd f14, 14*8(sp) */
    .hword        0x06E1
    .hword        0x3427        /* fsd f13, 13*8(sp) */
    .hword        0x06D1
    .hword        0x3027        /* fsd f12, 12*8(sp) */
    .hword        0x06C1
    .hword        0x3C27        /* fsd f11, 11*8(sp) */
    .hword        0x04B1
    .hword        0x3827        /* fsd f10, 10*8(sp) */
    .hword        0x04A1
    .hword        0x3427        /* fsd f9, 9*8(sp) */
    .hword        0x0491
    .hword        0x3027        /* fsd f8, 8*8(sp) */
    .hword        0x0481
    .hword        0x3C27        /* fsd f7, 7*8(sp) */
    .hword        0x0271
    .hword        0x3827        /* fsd f6, 6*8(sp) */
    .hword        0x0261
    .hword        0x3427        /* fsd f5, 5*8(sp) */
    .hword        0x0251
    .hword        0x3027        /* fsd f4, 4*8(sp) */
    .hword        0x0241
    .hword        0x3C27        /* fsd f3, 3*8(sp) */
    .hword        0x0031
    .hword        0x3827        /* fsd f2, 2*8(sp) */
    .hword        0x0021
    .hword        0x3427        /* fsd f1, 1*8(sp) */
    .hword        0x0011
    .hword        0x3027        /* fsd f0, 0*8(sp) */
    .hword        0x0001
    li    a0, 0x6088        /* mstatus with FS dirty, back to U-mode. */
    portSWITCH_CONTEXT        /* Do the context switch. */
    lui    a1, 4            /* See if FPU is used (mstatus.fs[1]==1). */
    and    a1, a1, a0
    beqz    a1, vPortYieldRVD_LoadSkip
    .hword        0x3007        /* fld f0, 0*8(sp) */
    .hword        0x0001
    .hword        0x3087        /* fld f1, 1*8(sp) */
    .hword        0x0081
    .hword        0x3107        /* fld f2, 2*8(sp) */
    .hword        0x0101
    .hword        0x3187        /* fld f3, 3*8(sp) */
    .hword        0x0181
    .hword        0x3207        /* fld f4, 4*8(sp) */
    .hword        0x0201
    .hword        0x3287        /* fld f5, 5*8(sp) */
    .hword        0x0281
    .hword        0x3307        /* fld f6, 6*8(sp) */
    .hword        0x0301
    .hword        0x3387        /* fld f7, 7*8(sp) */
    .hword        0x0381
    .hword        0x3407        /* fld f8, 8*8(sp) */
    .hword        0x0401
    .hword        0x3487        /* fld f9, 9*8(sp) */
    .hword        0x0481
    .hword        0x3507        /* fld f10, 10*8(sp) */
    .hword        0x0501
    .hword        0x3587        /* fld f11, 11*8(sp) */
    .hword        0x0581
    .hword        0x3607        /* fld f12, 12*8(sp) */
    .hword        0x0601
    .hword        0x3687        /* fld f13, 13*8(sp) */
    .hword        0x0681
    .hword        0x3707        /* fld f14, 14*8(sp) */
    .hword        0x0701
    .hword        0x3787        /* fld f15, 15*8(sp) */
    .hword        0x0781
    .hword        0x3807        /* fld f16, 16*8(sp) */
    .hword        0x0801
    .hword        0x3887        /* fld f17, 17*8(sp) */
    .hword        0x0881
    .hword        0x3907        /* fld f18, 18*8(sp) */
    .hword        0x0901
    .hword        0x3987        /* fld f19, 19*8(sp) */
    .hword        0x0981
    .hword        0x3A07        /* fld f20, 20*8(sp) */
    .hword        0x0A01
    .hword        0x3A87        /* fld f21, 21*8(sp) */
    .hword        0x0A81
    .hword        0x3B07        /* fld f22, 22*8(sp) */
    .hword        0x0B01
    .hword        0x3B87        /* fld f23, 23*8(sp) */
    .hword        0x0B81
    .hword        0x3C07        /* fld f24, 24*8(sp) */
    .hword        0x0C01
    .hword        0x3C87        /* fld f25, 25*8(sp) */
    .hword        0x0C81
    .hword        0x3D07        /* fld f26, 26*8(sp) */
    .hword        0x0D01
    .hword        0x3D87        /* fld f27, 27*8(sp) */
    .hword        0x0D81
    .hword        0x3E07        /* fld f28, 28*8(sp) */
    .hword        0x0E01
    .hword        0x3E87        /* fld f29, 29*8(sp) */
    .hword        0x0E81
    .hword        0x3F07        /* fld f30, 30*8(sp) */
    .hword        0x0F01
    .hword        0x3F87        /* fld f31, 31*8(sp) */
    .hword        0x0F81
    lw    a1, 32*8(sp)
    .hword        0x9073        /* fscsr a1 */
    .hword        0x0035
    addi    sp, sp, 65*4
vPortYieldRVD_LoadSkip:
    portRESTORE_CONTEXT        /* Enable interrupts and restore registers. */
vPortYieldRVD_Exit:
    ret

/*-----------------------------------------------------------*/

/*
 * Start the first task by loading the startup frame that
 * pxPortInitialiseStack() built: the hypercall parameter area below the
 * frame is dropped, the VM interrupts are enabled, the registers are
 * loaded, and the task entry address is taken through x1 so that the
 * final ret enters it.  This function never returns.
 */
    .section    .text.xPortStartFirstTask
    .align        3
    .func
xPortStartFirstTask:
    lw    sp, pxCurrentTCB    /* Load pxCurrentTCB. */
    lw    sp, 0(sp)        /* Read sp from first TCB member. */
    addi    sp, sp, 5*4        /* Skip the hypercall parameters. */

    call    RVM_Hyp_Int_Ena

    lw    a0, 0*4(sp)        /* Read the mstatus. */
    lw    x1, 1*4(sp)        /* ra - receives the task entry address. */
    lw    x3, 3*4(sp)        /* gp */
    lw    x4, 4*4(sp)        /* tp */
    lw    x5, 5*4(sp)        /* t0 */
    lw    x6, 6*4(sp)        /* t1 */
    lw    x7, 7*4(sp)        /* t2 */
    lw    x8, 8*4(sp)        /* s0/fp */
    lw    x9, 9*4(sp)        /* s1 */
    lw    x10, 10*4(sp)        /* a0 */
    lw    x11, 11*4(sp)        /* a1 */
    lw    x12, 12*4(sp)        /* a2 */
    lw    x13, 13*4(sp)        /* a3 */
    lw    x14, 14*4(sp)        /* a4 */
    lw    x15, 15*4(sp)        /* a5 */
    lw    x16, 16*4(sp)        /* a6 */
    lw    x17, 17*4(sp)        /* a7 */
    lw    x18, 18*4(sp)        /* s2 */
    lw    x19, 19*4(sp)        /* s3 */
    lw    x20, 20*4(sp)        /* s4 */
    lw    x21, 21*4(sp)        /* s5 */
    lw    x22, 22*4(sp)        /* s6 */
    lw    x23, 23*4(sp)        /* s7 */
    lw    x24, 24*4(sp)        /* s8 */
    lw    x25, 25*4(sp)        /* s9 */
    lw    x26, 26*4(sp)        /* s10 */
    lw    x27, 27*4(sp)        /* s11 */
    lw    x28, 28*4(sp)        /* t3 */
    lw    x29, 29*4(sp)        /* t4 */
    lw    x30, 30*4(sp)        /* t5 */
    lw    x31, 31*4(sp)        /* t6 */

    addi    sp, sp, 32*4

    ret
    .endfunc
