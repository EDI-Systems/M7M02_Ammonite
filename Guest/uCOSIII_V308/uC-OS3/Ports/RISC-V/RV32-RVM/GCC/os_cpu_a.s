#********************************************************************************************************
#                                              uC/OS-III
#                                        The Real-Time Kernel
#
#                                     RVM Hypervisor Guest Port
#
#                               Copyright (C) 2026 Evo-Devo Instrum.
#                                       All rights reserved.
#
#                This file is NOT part of the uC/OS-III distribution.  It belongs
#                to the uC/OS-III-on-RVM test suite and is released under the
#                Unlicense; refer to the LICENSE file for details.  The uC/OS-III
#                kernel itself is copyright Silicon Laboratories Inc. and is not
#                modified by this file.
#
#********************************************************************************************************

#********************************************************************************************************
#
#                                        ASSEMBLY LANGUAGE PORT
#                                         RISC-V RV32 PORT
#
# File      : os_cpu_a.s
# Version   : V1.00.00
#********************************************************************************************************
# For       : RISC-V RV32 running as an unprivileged RVM guest
# Toolchain : GNU C Compiler
#********************************************************************************************************
# Note(s)   : (1) The guest cannot trigger the software interrupt the official port uses for its
#                 context switches - the MSIP register sits in the machine control block, out of
#                 the guest's reach.  The task level switch is therefore performed directly on
#                 the task stack by the fast path below, without a trap through the hypervisor,
#                 and the interrupt driven switches go through the RVM yield hypercall and the
#                 handler in os_cpu_c.c.
#
#             (2) The task stack frames, from the stack pointer upwards, are the five hypercall
#                 parameter words, the saved mstatus, optionally the floating point registers of
#                 the F or D extension (their presence is marked by the FS field of the saved
#                 mstatus), and the integer registers (pc, x1, x3 .. x31).  x2 (sp) is implied by
#                 the frame itself, and x4 (tp) carries the resume address on the way back
#                 because mepc cannot be reached from user mode.  The frames are identical to
#                 the ones that OS_Ctx_Handler() in os_cpu_c.c builds for the interrupt driven
#                 switches.
#
#             (3) The floating point instructions of the F and D flavours are emitted as raw
#                 halfword pairs, so that the assembler does not have to be told about the F and
#                 D extensions when it processes this file.
#********************************************************************************************************

#********************************************************************************************************
#                                          PUBLIC FUNCTIONS
#********************************************************************************************************

                                                                # External references.
    .extern  OSRunning
    .extern  OSPrioCur
    .extern  OSPrioHighRdy
    .extern  OSTCBCurPtr
    .extern  OSTCBHighRdyPtr
    .extern  OSTaskSwHook
    .extern  OS_CPU_Port_Init
    .extern  __RVM_Global
    .extern  __RVM_Stack
    .extern  RVM_Usr_Param
    .extern  RVM_Virt_Int_Mask
    .extern  RVM_Virt_Int_Unmask

    .global  OSStartHighRdy                                      # Functions declared in this file.
    .global  OSCtxSw
    .global  OSCtxSwRVF
    .global  OSCtxSwRVD


#********************************************************************************************************
#                                               EQUATES
#********************************************************************************************************

    .equ  RISCV_MSTATUS_UMODE,      0x0088                      # User mode, FS off.
    .equ  RISCV_MSTATUS_FS_DIRTY,   0x6000                      # User mode with the FS field dirty.
    .equ  RISCV_MSTATUS_FS_MASK,    0x4000                      # The dirty indicator bit of the FS field.


#********************************************************************************************************
#                                     CODE GENERATION DIRECTIVES
#********************************************************************************************************

.section .text


#********************************************************************************************************
#                                          LOCAL MACROS
#
# Note(s)  : 1) Save the integer registers of the current task onto its stack along with the
#                address of the point where the task shall resume, then mask the virtual
#                interrupts.
#********************************************************************************************************

    .macro  OS_CPU_SAVE_CONTEXT label
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
    la    a0, \label                                            # Save the resume address as the pc.
    sw    a0, 0*4(sp)
    call    RVM_Virt_Int_Mask                                    # Disable interrupts.
    .endm


#********************************************************************************************************
#
#                                          LOCAL MACROS
#
# Note(s)  : 1) Hand the CPU over to another task: stash the mstatus passed in a0 along with the
#                hypercall parameters below the frame, switch onto the kernel stack with the
#                kernel gp, run the official switch bookkeeping, then reload the frame of the
#                task that was selected.  The mstatus of the frame switched to is returned in
#                a0 - its FS field decides whether FPU registers follow the frame.
#********************************************************************************************************

    .macro  OS_CPU_SWITCH_CONTEXT
    addi    sp, sp, -4                                          # Save the mstatus.
    sw    a0, 0*4(sp)
    addi    sp, sp, -5*4                                        # Push the hypercall parameters.
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

    lw    a0, OSTCBCurPtr                                        # OSTCBCurPtr->StkPtr = sp;
    sw    sp, 0(a0)

    jal    OSTaskSwHook                                          # The official switch bookkeeping -
                                                                # runs on the task stack, like the
                                                                # proven G071 port (the bookkeeping
                                                                # is shallow and all globals share
                                                                # the user gp).

    la    a0, OSPrioHighRdy                                      # OSPrioCur = OSPrioHighRdy;
    lb    a1, (a0)
    la    a0, OSPrioCur
    sb    a1, (a0)

    la    a0, OSTCBHighRdyPtr                                   # OSTCBCurPtr = OSTCBHighRdyPtr;
    lw    a1, (a0)
    la    a0, OSTCBCurPtr
    sw    a1, (a0)

    lw    sp, 0(a1)                                              # SP = OSTCBHighRdyPtr->StkPtr;

    la    a0, RVM_Usr_Param
    lw    a0, (a0)                                               # Pop the hypercall parameters.
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
    lw    a0, 0*4(sp)                                            # Read the mstatus to decide the FPU status.
    addi    sp, sp, 4
    .endm


#********************************************************************************************************
#
#                                          LOCAL MACROS
#
# Note(s)  : 1) Restore the integer registers of the task being switched to from its stack frame,
#                resume at the address that OS_CPU_SAVE_CONTEXT stashed into tp, and unmask the
#                virtual interrupts again.  x4 (tp) is not restored - it was trashed to carry
#                the resume address.
#********************************************************************************************************

    .macro  OS_CPU_RESTORE_CONTEXT
    call    RVM_Virt_Int_Unmask                                  # Enable interrupts.
    lw    tp, 0*4(sp)                                            # Load the pc - x4 (tp) is trashed for this.
    lw    x1, 1*4(sp)                                            # Load the registers, skipping x4 (tp).
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
    addi    sp, sp, 31*4
    jalr    x0, tp
    .endm
#********************************************************************************************************
#                                         START MULTITASKING
#                                      void  OSStartHighRdy(void)
#
# Note(s) : 1) This function hooks the tick and the context switch handler up to the hypervisor,
#                marks the kernel as running, then abandons the startup stack and enters the
#                highest priority ready task directly on its own stack.  The initial stack frame
#                is laid down by OSTaskStkInit() in os_cpu_c.c; its hypercall parameter words and
#                its mstatus are simply skipped, because the first task starts with a clean
#                hypercall state and its floating point state is marked initial.
#********************************************************************************************************

OSStartHighRdy:
    jal    OS_CPU_Port_Init                                      # Hook the tick and the context handler.

    li    t0, 0x01                                              # OSRunning = TRUE;
    la    t1, OSRunning
    sb    t0, 0(t1)

    la    t0, OSTCBHighRdyPtr                                   # The TCB of the task to start.
    lw    t1, 0(t0)
    lw    sp, 0(t1)                                             # The task stack pointer.

    addi    sp, sp, 6*4                                         # Skip the hypercall parameters and the mstatus.

    lw    tp, 0*4(sp)                                           # Load the pc - x4 (tp) is trashed for this.
    lw    x1, 1*4(sp)                                           # Load the registers, skipping x4 (tp).
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
    addi    sp, sp, 31*4
    jalr    x0, tp                                               # Branch to the task entry.


#********************************************************************************************************
#                       PERFORM A CONTEXT SWITCH (From task level) - OSCtxSw()
#
# Note(s) : 1) OSCtxSw() is entered from OS_TASK_SW() when the kernel wants to perform a task
#                context switch.  OSSched() has already selected OSTCBHighRdyPtr and verified
#                that it differs from OSTCBCurPtr, so the switch is performed right here,
#                directly on the task stack.  This variant stores no coprocessor state and
#                marks the frame with FS off, so no FPU registers are restored when the task
#                is resumed.
#********************************************************************************************************

.section    .text.osctxsw
.align      3

OSCtxSw:
    OS_CPU_SAVE_CONTEXT OSCtxSw_Exit
    li    a0, RISCV_MSTATUS_UMODE                                # mstatus for no FPU, back to U-mode.
    OS_CPU_SWITCH_CONTEXT                                       # Do the context switch.
    OS_CPU_RESTORE_CONTEXT                                      # Enable interrupts and restore registers.
OSCtxSw_Exit:                                                   # Exit location.
    ret


#********************************************************************************************************
#                     PERFORM A CONTEXT SWITCH (From task level, RV32F) - OSCtxSwRVF()
#
# Note(s) : 1) Same as OSCtxSw, but for targets with a single-precision FPU: all 32 FPU registers
#                and the FCSR are always pushed below the integer frame - the FS state cannot
#                be probed from user mode, so the yield cannot know whether the task dirtied
#                its floating point state.
#********************************************************************************************************

.section    .text.osctxswrvf
.align      3

OSCtxSwRVF:
    OS_CPU_SAVE_CONTEXT OSCtxSwRVF_Exit
    addi    sp, sp, -33*4                                        # The FPU context, always saved.
    .hword        0x25F3                                        # frcsr a1
    .hword        0x0030
    sw    a1, 32*4(sp)
    .hword        0xFEFE                                        # fsw f31, 31*4(sp)
    .hword        0xFCFA                                        # fsw f30, 30*4(sp)
    .hword        0xFAF6                                        # fsw f29, 29*4(sp)
    .hword        0xF8F2                                        # fsw f28, 28*4(sp)
    .hword        0xF6EE                                        # fsw f27, 27*4(sp)
    .hword        0xF4EA                                        # fsw f26, 26*4(sp)
    .hword        0xF2E6                                        # fsw f25, 25*4(sp)
    .hword        0xF0E2                                        # fsw f24, 24*4(sp)
    .hword        0xEEDE                                        # fsw f23, 23*4(sp)
    .hword        0xECDA                                        # fsw f22, 22*4(sp)
    .hword        0xEAD6                                        # fsw f21, 21*4(sp)
    .hword        0xE8D2                                        # fsw f20, 20*4(sp)
    .hword        0xE6CE                                        # fsw f19, 19*4(sp)
    .hword        0xE4CA                                        # fsw f18, 18*4(sp)
    .hword        0xE2C6                                        # fsw f17, 17*4(sp)
    .hword        0xE0C2                                        # fsw f16, 16*4(sp)
    .hword        0xFE3E                                        # fsw f15, 15*4(sp)
    .hword        0xFC3A                                        # fsw f14, 14*4(sp)
    .hword        0xFA36                                        # fsw f13, 13*4(sp)
    .hword        0xF832                                        # fsw f12, 12*4(sp)
    .hword        0xF62E                                        # fsw f11, 11*4(sp)
    .hword        0xF42A                                        # fsw f10, 10*4(sp)
    .hword        0xF226                                        # fsw f9, 9*4(sp)
    .hword        0xF022                                        # fsw f8, 8*4(sp)
    .hword        0xEE1E                                        # fsw f7, 7*4(sp)
    .hword        0xEC1A                                        # fsw f6, 6*4(sp)
    .hword        0xEA16                                        # fsw f5, 5*4(sp)
    .hword        0xE812                                        # fsw f4, 4*4(sp)
    .hword        0xE60E                                        # fsw f3, 3*4(sp)
    .hword        0xE40A                                        # fsw f2, 2*4(sp)
    .hword        0xE206                                        # fsw f1, 1*4(sp)
    .hword        0xE002                                        # fsw f0, 0*4(sp)
    li    a0, RISCV_MSTATUS_FS_DIRTY | RISCV_MSTATUS_UMODE      # mstatus with FS dirty, back to U-mode.
    OS_CPU_SWITCH_CONTEXT                                       # Do the context switch.
    lui    a1, 4                                                 # See if FPU is used (mstatus.fs[1]==1).
    and    a1, a1, a0
    beqz    a1, OSCtxSwRVF_LoadSkip
    .hword        0x6002                                        # flw f0, 0*4(sp)
    .hword        0x6092                                        # flw f1, 1*4(sp)
    .hword        0x6122                                        # flw f2, 2*4(sp)
    .hword        0x61B2                                        # flw f3, 3*4(sp)
    .hword        0x6242                                        # flw f4, 4*4(sp)
    .hword        0x62D2                                        # flw f5, 5*4(sp)
    .hword        0x6362                                        # flw f6, 6*4(sp)
    .hword        0x63F2                                        # flw f7, 7*4(sp)
    .hword        0x7402                                        # flw f8, 8*4(sp)
    .hword        0x7492                                        # flw f9, 9*4(sp)
    .hword        0x7522                                        # flw f10, 10*4(sp)
    .hword        0x75B2                                        # flw f11, 11*4(sp)
    .hword        0x7642                                        # flw f12, 12*4(sp)
    .hword        0x76D2                                        # flw f13, 13*4(sp)
    .hword        0x7762                                        # flw f14, 14*4(sp)
    .hword        0x77F2                                        # flw f15, 15*4(sp)
    .hword        0x6806                                        # flw f16, 16*4(sp)
    .hword        0x6896                                        # flw f17, 17*4(sp)
    .hword        0x6926                                        # flw f18, 18*4(sp)
    .hword        0x69B6                                        # flw f19, 19*4(sp)
    .hword        0x6A46                                        # flw f20, 20*4(sp)
    .hword        0x6AD6                                        # flw f21, 21*4(sp)
    .hword        0x6B66                                        # flw f22, 22*4(sp)
    .hword        0x6BF6                                        # flw f23, 23*4(sp)
    .hword        0x7C06                                        # flw f24, 24*4(sp)
    .hword        0x7C96                                        # flw f25, 25*4(sp)
    .hword        0x7D26                                        # flw f26, 26*4(sp)
    .hword        0x7DB6                                        # flw f27, 27*4(sp)
    .hword        0x7E46                                        # flw f28, 28*4(sp)
    .hword        0x7ED6                                        # flw f29, 29*4(sp)
    .hword        0x7F66                                        # flw f30, 30*4(sp)
    .hword        0x7FF6                                        # flw f31, 31*4(sp)
    lw    a1, 32*4(sp)
    .hword        0x9073                                        # fscsr a1
    .hword        0x0035
    addi    sp, sp, 33*4
OSCtxSwRVF_LoadSkip:
    OS_CPU_RESTORE_CONTEXT                                      # Enable interrupts and restore registers.
OSCtxSwRVF_Exit:                                                # Exit location.
    ret
#********************************************************************************************************
#                     PERFORM A CONTEXT SWITCH (From task level, RV32D) - OSCtxSwRVD()
#
# Note(s) : 1) Same as OSCtxSwRVF, but for targets with a double-precision FPU: the 32 FPU registers
#                occupy two words each, so the FPU part of the frame is 65 words wide.
#********************************************************************************************************

.section    .text.osctxswrvd
.align      3

OSCtxSwRVD:
    OS_CPU_SAVE_CONTEXT OSCtxSwRVD_Exit
    addi    sp, sp, -65*4                                        # The FPU context, always saved.
    .hword        0x25F3                                        # frcsr a1
    .hword        0x0030
    sw    a1, 32*8(sp)
    .hword        0x3C27                                        # fsd f31, 31*8(sp)
    .hword        0x0FF1
    .hword        0x3827                                        # fsd f30, 30*8(sp)
    .hword        0x0FE1
    .hword        0x3427                                        # fsd f29, 29*8(sp)
    .hword        0x0FD1
    .hword        0x3027                                        # fsd f28, 28*8(sp)
    .hword        0x0FC1
    .hword        0x3C27                                        # fsd f27, 27*8(sp)
    .hword        0x0DB1
    .hword        0x3827                                        # fsd f26, 26*8(sp)
    .hword        0x0DA1
    .hword        0x3427                                        # fsd f25, 25*8(sp)
    .hword        0x0D91
    .hword        0x3027                                        # fsd f24, 24*8(sp)
    .hword        0x0D81
    .hword        0x3C27                                        # fsd f23, 23*8(sp)
    .hword        0x0B71
    .hword        0x3827                                        # fsd f22, 22*8(sp)
    .hword        0x0B61
    .hword        0x3427                                        # fsd f21, 21*8(sp)
    .hword        0x0B51
    .hword        0x3027                                        # fsd f20, 20*8(sp)
    .hword        0x0B41
    .hword        0x3C27                                        # fsd f19, 19*8(sp)
    .hword        0x0931
    .hword        0x3827                                        # fsd f18, 18*8(sp)
    .hword        0x0921
    .hword        0x3427                                        # fsd f17, 17*8(sp)
    .hword        0x0911
    .hword        0x3027                                        # fsd f16, 16*8(sp)
    .hword        0x0901
    .hword        0x3C27                                        # fsd f15, 15*8(sp)
    .hword        0x06F1
    .hword        0x3827                                        # fsd f14, 14*8(sp)
    .hword        0x06E1
    .hword        0x3427                                        # fsd f13, 13*8(sp)
    .hword        0x06D1
    .hword        0x3027                                        # fsd f12, 12*8(sp)
    .hword        0x06C1
    .hword        0x3C27                                        # fsd f11, 11*8(sp)
    .hword        0x04B1
    .hword        0x3827                                        # fsd f10, 10*8(sp)
    .hword        0x04A1
    .hword        0x3427                                        # fsd f9, 9*8(sp)
    .hword        0x0491
    .hword        0x3027                                        # fsd f8, 8*8(sp)
    .hword        0x0481
    .hword        0x3C27                                        # fsd f7, 7*8(sp)
    .hword        0x0271
    .hword        0x3827                                        # fsd f6, 6*8(sp)
    .hword        0x0261
    .hword        0x3427                                        # fsd f5, 5*8(sp)
    .hword        0x0251
    .hword        0x3027                                        # fsd f4, 4*8(sp)
    .hword        0x0241
    .hword        0x3C27                                        # fsd f3, 3*8(sp)
    .hword        0x0031
    .hword        0x3827                                        # fsd f2, 2*8(sp)
    .hword        0x0021
    .hword        0x3427                                        # fsd f1, 1*8(sp)
    .hword        0x0011
    .hword        0x3027                                        # fsd f0, 0*8(sp)
    .hword        0x0001
    li    a0, RISCV_MSTATUS_FS_DIRTY | RISCV_MSTATUS_UMODE      # mstatus with FS dirty, back to U-mode.
    OS_CPU_SWITCH_CONTEXT                                       # Do the context switch.
    lui    a1, 4                                                 # See if FPU is used (mstatus.fs[1]==1).
    and    a1, a1, a0
    beqz    a1, OSCtxSwRVD_LoadSkip
    .hword        0x3007                                        # fld f31, 31*8(sp)
    .hword        0x0001
    .hword        0x3087                                        # fld f30, 30*8(sp)
    .hword        0x0081
    .hword        0x3107                                        # fld f29, 29*8(sp)
    .hword        0x0101
    .hword        0x3187                                        # fld f28, 28*8(sp)
    .hword        0x0181
    .hword        0x3207                                        # fld f27, 27*8(sp)
    .hword        0x0201
    .hword        0x3287                                        # fld f26, 26*8(sp)
    .hword        0x0281
    .hword        0x3307                                        # fld f25, 25*8(sp)
    .hword        0x0301
    .hword        0x3387                                        # fld f24, 24*8(sp)
    .hword        0x0381
    .hword        0x3407                                        # fld f23, 23*8(sp)
    .hword        0x0401
    .hword        0x3487                                        # fld f22, 22*8(sp)
    .hword        0x0481
    .hword        0x3507                                        # fld f21, 21*8(sp)
    .hword        0x0501
    .hword        0x3587                                        # fld f20, 20*8(sp)
    .hword        0x0581
    .hword        0x3607                                        # fld f19, 19*8(sp)
    .hword        0x0601
    .hword        0x3687                                        # fld f18, 18*8(sp)
    .hword        0x0681
    .hword        0x3707                                        # fld f17, 17*8(sp)
    .hword        0x0701
    .hword        0x3787                                        # fld f16, 16*8(sp)
    .hword        0x0781
    .hword        0x3807                                        # fld f15, 15*8(sp)
    .hword        0x0801
    .hword        0x3887                                        # fld f14, 14*8(sp)
    .hword        0x0881
    .hword        0x3907                                        # fld f13, 13*8(sp)
    .hword        0x0901
    .hword        0x3987                                        # fld f12, 12*8(sp)
    .hword        0x0981
    .hword        0x3A07                                        # fld f11, 11*8(sp)
    .hword        0x0A01
    .hword        0x3A87                                        # fld f10, 10*8(sp)
    .hword        0x0A81
    .hword        0x3B07                                        # fld f9, 9*8(sp)
    .hword        0x0B01
    .hword        0x3B87                                        # fld f8, 8*8(sp)
    .hword        0x0B81
    .hword        0x3C07                                        # fld f7, 7*8(sp)
    .hword        0x0C01
    .hword        0x3C87                                        # fld f6, 6*8(sp)
    .hword        0x0C81
    .hword        0x3D07                                        # fld f5, 5*8(sp)
    .hword        0x0D01
    .hword        0x3D87                                        # fld f4, 4*8(sp)
    .hword        0x0D81
    .hword        0x3E07                                        # fld f3, 3*8(sp)
    .hword        0x0E01
    .hword        0x3E87                                        # fld f2, 2*8(sp)
    .hword        0x0E81
    .hword        0x3F07                                        # fld f1, 1*8(sp)
    .hword        0x0F01
    .hword        0x3F87                                        # fld f0, 0*8(sp)
    .hword        0x0F81
    lw    a1, 32*8(sp)
    .hword        0x9073                                        # fscsr a1
    .hword        0x0035
    addi    sp, sp, 65*4
OSCtxSwRVD_LoadSkip:
    OS_CPU_RESTORE_CONTEXT                                      # Enable interrupts and restore registers.
OSCtxSwRVD_Exit:                                                # Exit location.
    ret


#********************************************************************************************************
#                                             MODULE END
#********************************************************************************************************
