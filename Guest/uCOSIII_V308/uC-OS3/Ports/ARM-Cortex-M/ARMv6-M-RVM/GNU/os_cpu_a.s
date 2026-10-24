@********************************************************************************************************
@                                              uC/OS-III
@                                        The Real-Time Kernel
@
@                                     RVM Hypervisor Guest Port
@
@                               Copyright (C) 2026 Evo-Devo Instrum.
@                                       All rights reserved.
@
@                This file is NOT part of the uC/OS-III distribution.  It belongs
@                to the uC/OS-III-on-RVM test suite and is released under the
@                Unlicense; refer to the LICENSE file for details.  The uC/OS-III
@                kernel itself is copyright Silicon Laboratories Inc. and is not
@                modified by this file.
@
@********************************************************************************************************

@********************************************************************************************************
@
@                                             ARMv6-M Port
@                                    RVM Hypervisor Guest Layer
@
@ File      : os_cpu_a.s
@ Version   : V1.00.00
@********************************************************************************************************
@ For       : ARMv6-M Cortex-M0+ running as an unprivileged RVM guest
@ Mode      : Thumb
@ Toolchain : GNU C Compiler
@********************************************************************************************************
@ Note(s)   : (1) This port supports ARM Cortex-M0+ systems running as unprivileged
@                 guests of the RVM hypervisor.  The guest cannot access the NVIC, so
@                 the context switch cannot go through the PendSV exception; it is
@                 performed directly on the task stack instead, and the initial task
@                 startup abandons the startup stack and enters the task immediately.
@
@             (2) OSCtxSw() builds the same frame that the official PendSV handler
@                 would have built - the five hypercall parameter words, R4-R11, the
@                 interrupted LR and a basic exception frame - so that a task that
@                 was switched out by the fast path can be switched back in by the
@                 interrupt driven path through OS_Ctx_Handler() in os_cpu_c.c, and
@                 vice versa.
@********************************************************************************************************

@********************************************************************************************************
@                                          PUBLIC FUNCTIONS
@********************************************************************************************************

                                                                @ External references.
    .extern  OSTCBCurPtr
    .extern  OSTCBHighRdyPtr
    .extern  OSPrioCur
    .extern  OSPrioHighRdy
    .extern  OSRunning
    .extern  OSTaskSwHook
    .extern  OS_CPU_Port_Init
    .extern  RVM_Usr_Param
    .extern  RVM_Virt_Int_Mask
    .extern  RVM_Virt_Int_Unmask

    .global  OSStartHighRdy                                      @ Functions declared in this file.
    .global  OSCtxSw


@********************************************************************************************************
@                                          CODE GENERATION DIRECTIVES
@********************************************************************************************************

   .text
   .align 2
   .thumb
   .syntax unified


@********************************************************************************************************
@                                          LOCAL MACROS
@********************************************************************************************************
@
@ Note(s) : 1) See whether the stack pointer is 8 byte aligned.  TST SP is unpredictable on
@              ARMv6-M, so SP has to go through a register first.  Three words have been
@              pushed above the tested value, so an aligned original stack pointer lands
@              here unaligned, and vice versa.
@********************************************************************************************************

    .macro  OS_CPU_ALIGN_PUSH label
    MOV     R0, SP
    LDR     R1, =0x00000007
    TST     R0, R1
    BEQ     \label
    .endm


@********************************************************************************************************
@
@                                          LOCAL MACROS
@
@ Note(s) : 1) Stack the remaining part of a basic exception frame of \sz words, with the given
@              XPSR and EXC_RETURN, and the resume point as the PC.
@********************************************************************************************************

    .macro  OS_CPU_EXC_PUSH sz, xpsr, exc_return
    LDR     R0, [SP, #4*2]                                       @ Load real R0/R1 values pushed at start.
    LDR     R1, [SP]
    SUB     SP, #4*(\sz-7)                                       @ Adjust SP to push GP registers.
    PUSH    {R0-R3}                                              @ Push stack frame GP registers.
    MOV     R0, R12
    MOV     R1, LR
    STR     R0, [SP, #4*4]
    STR     R1, [SP, #4*5]
    LDR     R0, [SP, #4*(\sz-2)]                                 @ Load real XPSR value pushed at start.
    LDR     R1, =\xpsr
    ORRS    R0, R1
    STR     R0, [SP, #4*7]
    LDR     R0, =OSCtxSw_Exit                                    @ Push PC with bit[0] cleared.
    LDR     R1, =0xFFFFFFFE
    ANDS    R0, R1
    STR     R0, [SP, #4*6]
    LDR     R1, =\exc_return                                     @ Make up the EXC_RETURN.
    MOV     LR, R1
    .endm


@********************************************************************************************************
@
@                                          LOCAL MACROS
@
@ Note(s) : 1) See whether the stacked XPSR carries the 8 byte alignment padding.
@********************************************************************************************************

    .macro  OS_CPU_ALIGN_POP label
    LDR     R0, [SP, #4*7]                                       @ Load up XPSR.
    LDR     R1, =XPSR_ALIGN_PAD
    TST     R0, R1
    BNE     \label
    .endm
@********************************************************************************************************
@                                               EQUATES
@********************************************************************************************************

                                                                @ The stack frame alignment padding
                                                                @ indicator of XPSR bit 9.
.equ XPSR_ALIGN_PAD, 0x00000200


@********************************************************************************************************
@
@                                          LOCAL MACROS
@
@ Note(s) : 1) Unstack a basic exception frame of \sz words.  The original layout
@              [PAD] XPSR PC LR R12 R3-R0 is inconvenient for manual restoring,
@              because loading PC must be the last step, so it is transformed in
@              place first:
@                1. With [PAD]:    [PAD] XPSR PC LR R12 R3-R0 -> PC R0 XPSR [EMPTY] R12 R3-R1 LR
@                2. Without [PAD]: XPSR PC LR R12 R3-R0       -> PC R0 XPSR R12 R3-R1 LR
@              The transformation restores LR first so it cannot be overwritten,
@              pops R0, loads XPSR and PC and rearranges them at the end, pops
@              R1-R3 and LR while skipping the middle, and finally restores XPSR
@              through R0, then R0 and PC.  Nothing is ever placed below the
@              current SP, because a racing interrupt would clobber it.
@********************************************************************************************************

    .macro  OS_CPU_EXC_POP sz
    LDR     R0, [SP, #4*4]                                       @ Restore LR/R12.
    LDR     R1, [SP, #4*5]
    MOV     R12, R0
    MOV     LR, R1
    POP     {R0}                                                 @ Load R0/XPSR/PC into R0/R1/R2.
    LDR     R1, [SP, #4*6]
    LDR     R3, =0xFFFFFDFF                                      @ Clear XPSR[9].
    ANDS    R1, R3
    LDR     R2, [SP, #4*5]
    LDR     R3, =0x00000001                                      @ Set PC[0].
    ORRS    R2, R3
    STR     R1, [SP, #4*(\sz-4)]                                 @ Rearrange to H-PC-R0-XPSR-L.
    STR     R0, [SP, #4*(\sz-3)]
    STR     R2, [SP, #4*(\sz-2)]
    POP     {R1-R3}                                              @ Pop GP registers.
    ADD     SP, #4*(\sz-7)                                       @ Skip R12/[PAD].
    POP     {R0}                                                 @ Pop XPSR through R0.
    MSR     XPSR_NZCVQ, R0
    POP     {R0, PC}                                              @ Pop R0 and PC.
    .endm


@********************************************************************************************************
@                                         START MULTITASKING
@                                      void  OSStartHighRdy(void)
@
@ Note(s) : 1) This function abandons the startup stack and switches to the stack of the highest
@              priority ready task, whose TCB pointer is provided by the kernel in OSTCBHighRdyPtr.
@              The virtual timers and the context switch handler are hooked up before the switch,
@              and the virtual interrupts are enabled just like the official ports do before the
@              first task runs.  The initial stack frame is laid down by OSTaskStkInit() in
@              os_cpu_c.c; its five hypercall parameter words are simply discarded, because the
@              first task starts with a clean hypercall state.
@
@           2) The task entry point is entered through LR, with the task return address pushed
@              below the stack pointer, so that a task returning from its body would observe a
@              stack that the official ports would have presented as well.
@********************************************************************************************************

.thumb_func
OSStartHighRdy:
    BL      OS_CPU_Port_Init                                    @ Hook the tick and the context handler.

    LDR     R3, =OSRunning                                      @ The kernel is running from now on.
    MOVS    R1, #1
    STRB    R1, [R3]

    LDR     R3, =OSTCBHighRdyPtr                                @ The TCB of the task to start.
    LDR     R3, [R3]
    LDR     R0, [R3]                                            @ The task stack pointer.
    MOV     SP, R0                                              @ Abandon the startup stack.

    POP     {R0-R4}                                             @ Discard the hypercall parameters.
    POP     {R4-R7}                                             @ Restore R4-R7.
    POP     {R0-R3}                                             @ Restore R8-R11 through R0-R3.
    MOV     R8, R0
    MOV     R9, R1
    MOV     R10, R2
    MOV     R11, R3
    POP     {R0}                                                 @ Discard the stacked EXC_RETURN.
    POP     {R0-R4}                                             @ Restore R0-R3, R12.
    MOV     R12, R4
    POP     {R4}                                                 @ The task return address, discarded.
    POP     {R0}                                                 @ The task entry point.
    MOV     LR, R0                                              @ Enter the task through LR.
    PUSH    {R4}                                                 @ Fix up SP.
    BX      LR                                                   @ Branch to the task entry.

    .pool
@********************************************************************************************************
@                       PERFORM A CONTEXT SWITCH (From task level) - OSCtxSw()
@
@ Note(s) : 1) OSCtxSw() is entered from OS_TASK_SW() when the kernel wants to perform a task
@              context switch.  OSSched() has already selected OSTCBHighRdyPtr and verified that
@              it differs from OSTCBCurPtr, so unlike on the official ports no exception needs to
@              be triggered: the switch is performed right here, directly on the task stack,
@              without a trap into the hypervisor.
@
@           2) Pseudo-code is:
@              a) Build the frame that the official PendSV handler would have built, with the
@                 resume point OSCtxSw_Exit as the PC;
@              b) Disable the virtual interrupts;
@              c) Save the process SP in its TCB, OSTCBCurPtr->StkPtr = SP;
@              d) Call OSTaskSwHook();
@              e) Get current high priority, OSPrioCur = OSPrioHighRdy;
@              f) Get current ready thread TCB, OSTCBCurPtr = OSTCBHighRdyPtr;
@              g) Get new process SP from TCB, SP = OSTCBHighRdyPtr->StkPtr;
@              h) Enable the virtual interrupts;
@              i) Unstack the new task frame and resume it at the point where it called
@                 OSCtxSw() (or at its entry, when it is scheduled for the first time).
@
@           3) The frame, from the stack pointer saved in the TCB up, is:
@              a) The five hypercall parameter words;
@              b) R4-R11 and the interrupted LR;
@              c) A basic exception frame, with one padding word when the stack pointer was
@                 not 8 byte aligned at entry, marked by XPSR bit 9 like the hardware does.
@
@           4) The register stacking may be nested by an interrupt that arrives before the
@              interrupts are masked.  This is allowed, but must be taken into account when
@              dimensioning the task stacks.
@
@           5) OS_TASK_SW() is called inside a scheduler critical section; the masking here is
@              idempotent and the unmasking happens after the new task frame has been loaded,
@              so the mask snapshot carried by CPU_SR in cpu_rvm.c remains consistent across
@              the switch.
@********************************************************************************************************

.thumb_func
OSCtxSw:
    PUSH    {R0}                                                 @ Protect R0, XPSR and R1.
    MRS     R0, XPSR
    PUSH    {R0}
    PUSH    {R1}

    OS_CPU_ALIGN_PUSH OSCtxSw_Unaligned
OSCtxSw_Aligned:                                                 @ Originally 8 byte aligned stack.
    OS_CPU_EXC_PUSH 8, 0x01000000, 0xFFFFFFFD
    B       OSCtxSw_Stacked
OSCtxSw_Unaligned:                                               @ Originally unaligned stack.
    OS_CPU_EXC_PUSH 8+1, 0x01000200, 0xFFFFFFFD

OSCtxSw_Stacked:
    MOV     R3, R11                                              @ Push GP registers.
    MOV     R2, R10
    MOV     R1, R9
    MOV     R0, R8
    PUSH    {R0-R3, LR}
    PUSH    {R4-R7}

    LDR     R0, =RVM_Usr_Param                                   @ Push the hypercall parameters.
    LDR     R0, [R0]
    LDMIA   R0!, {R1-R5}
    PUSH    {R1-R5}

    BL      RVM_Virt_Int_Mask                                    @ Disable the virtual interrupts.

    LDR     R1, =OSTCBCurPtr                                     @ OSTCBCurPtr->StkPtr = SP;
    LDR     R1, [R1]
    MOV     R2, SP
    STR     R2, [R1]

    BL      OSTaskSwHook                                         @ OSTaskSwHook();

    LDR     R0, =OSPrioCur                                       @ OSPrioCur   = OSPrioHighRdy;
    LDR     R1, =OSPrioHighRdy
    LDRB    R2, [R1]
    STRB    R2, [R0]

    LDR     R0, =OSTCBCurPtr                                     @ OSTCBCurPtr = OSTCBHighRdyPtr;
    LDR     R1, =OSTCBHighRdyPtr
    LDR     R2, [R1]
    STR     R2, [R0]

    LDR     R1, [R2]                                             @ R1 is new process SP; SP = OSTCBHighRdyPtr->StkPtr;
    MOV     SP, R1

    BL      RVM_Virt_Int_Unmask                                  @ Enable the virtual interrupts.

    LDR     R0, =RVM_Usr_Param                                   @ Pop the hypercall parameters.
    LDR     R0, [R0]
    POP     {R1-R5}
    STMIA   R0!, {R1-R5}

    POP     {R4-R7}                                              @ Pop GP registers.
    POP     {R0-R3}
    MOV     R8, R0
    MOV     R9, R1
    MOV     R10, R2
    MOV     R11, R3
    POP     {R0}
    MOV     LR, R0

    OS_CPU_ALIGN_POP OSCtxSw_PadSkip
OSCtxSw_NoPad:                                                   @ Frame without alignment padding.
    OS_CPU_EXC_POP 8
OSCtxSw_PadSkip:                                                 @ Frame with alignment padding.
    OS_CPU_EXC_POP 8+1

OSCtxSw_Exit:                                                    @ Exit location.
    BX      LR

    .pool

@********************************************************************************************************
@                                             END OF FILE
@********************************************************************************************************