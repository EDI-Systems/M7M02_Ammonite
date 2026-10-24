;********************************************************************************************************
;                                              uC/OS-III
;                                        The Real-Time Kernel
;
;                                     RVM Hypervisor Guest Port
;
;                               Copyright (C) 2026 Evo-Devo Instrum.
;                                       All rights reserved.
;
;                This file is NOT part of the uC/OS-III distribution.  It belongs
;                to the uC/OS-III-on-RVM test suite and is released under the
;                Unlicense; refer to the LICENSE file for details.  The uC/OS-III
;                kernel itself is copyright Silicon Laboratories Inc. and is not
;                modified by this file.
;
;********************************************************************************************************
;********************************************************************************************************
;
;                                            ARMv7-M Port
;                                    RVM Hypervisor Guest Layer
;
; File      : os_cpu_a.s
; Version   : V1.00.00
;********************************************************************************************************
; For       : ARMv7-M Cortex-M4F running as an unprivileged RVM guest
; Mode      : Thumb
; Toolchain : ARM C Compiler (armasm)
;********************************************************************************************************
; Note(s)   : (1) This port supports ARMv7-M systems with the FPv4-SP coprocessor
;                 running as unprivileged guests of the RVM hypervisor.  The guest
;                 cannot trigger the PendSV exception, so the task level context
;                 switch is performed directly on the task stack instead.
;
;             (2) OSCtxSw_Swt() is the assembly half of OSCtxSw() in os_cpu_c.c.
;                 It masquerades the calling context as if it were stacked by an
;                 exception entry - the very same frame is also recovered by the
;                 interrupt driven switches through OS_Ctx_Handler() in
;                 os_cpu_c.c, and by OSTaskStkInit() for the never-run tasks, so
;                 all three producers stay interchangeable.
;
;             (3) The frame, from the stack pointer saved in the TCB up, is:
;                  a) The five hypercall parameter words;
;                  b) R4-R11 and the interrupted LR (the saved EXC_RETURN);
;                  c) The S16-S31 block, only when the frame is extended;
;                  d) A hardware exception frame - basic, or extended with
;                     S0-S15/FPSCR - with one padding word when the stack
;                     pointer was not 8 byte aligned at entry, marked by
;                     XPSR bit 9 like the hardware does.
;
;             (4) uC/OS-III selects the next task outside of the switch: the
;                 scheduler has already stored its choice in OSTCBHighRdyPtr
;                 when OSCtxSw_Swt() runs, so no scheduler call - and no
;                 scheduler peek of any kind - is made here.
;
;             (5) The EPSR ICI/IT bits of the frame being unstacked cannot be
;                 restored by the manual unstacking below - only a real
;                 exception return can.  Frames carrying such interrupted
;                 instruction state are filtered out by OSCtxSw() in os_cpu_c.c
;                 beforehand, so reaching OSCtxSw_Err() here indicates an
;                 internal port error.
;********************************************************************************************************

;********************************************************************************************************
;                                          PUBLIC FUNCTIONS
;********************************************************************************************************

                                                                ; External references.
    IMPORT  OSTCBCurPtr
    IMPORT  OSTCBHighRdyPtr
    IMPORT  OSPrioCur
    IMPORT  OSPrioHighRdy
    IMPORT  OSRunning
    IMPORT  OSTaskSwHook
    IMPORT  OS_CPU_Port_Init
    IMPORT  OSCtxSw_Err
    IMPORT  RVM_Usr_Param
    IMPORT  RVM_Virt_Int_Mask
    IMPORT  RVM_Virt_Int_Unmask

    EXPORT  OSStartHighRdy                                      ; Functions declared in this file.
    EXPORT  OSCtxSw_Swt

;********************************************************************************************************
;                                          LOCAL MACROS
;********************************************************************************************************
;
; Note(s) : 1) See whether the stack pointer is 8 byte aligned.  TST SP is
;              unpredictable on ARMv7-M, so SP has to go through a register
;              first.  Two words have been pushed above the tested value, thus
;              an aligned original stack pointer lands here unaligned, and
;              vice versa.
;********************************************************************************************************

    MACRO
    OS_CPU_ALIGN_PUSH  $LABEL
    MOV                 R0, SP                                  ; Is SP aligned to 8 bytes?
    TST                 R0, #0x00000007
    BNE                 $LABEL                                  ; Two words pushed above, thus NE.
    MEND

;********************************************************************************************************
;
; Note(s) : 1) Stack the remaining part of an exception frame of $SZ words, with
;              the given XPSR bits and EXC_RETURN, and the exit label as the PC.
;              The two words pushed at the entry double as the frame's XPSR/PC
;              slots when the frame is basic, and sit just above it otherwise.
;********************************************************************************************************

    MACRO
    OS_CPU_EXC_PUSH    $SZ, $XPSR, $EXC_RETURN
    LDR                 R0, [SP, #4]                            ; Load the real R0 value pushed at entry.
    IF                  (($SZ-8)!=0)                            ; Reserve the frame's upper words.
    SUB                 SP, #4*($SZ-8)
    ENDIF
    PUSH                {R0-R3, R12, LR}                        ; Push the frame's GP registers.
    LDR                 R0, [SP, #4*($SZ-2)]                    ; Load the real XPSR value pushed at entry.
    LDR                 R1, =$XPSR
    ORRS                R0, R1
    STR                 R0, [SP, #4*7]                          ; Merge in the flags and the padding mark.
    LDR                 R0, =OSCtxSw_Exit                       ; Push the resume PC with bit[0] cleared.
    LDR                 R1, =0xFFFFFFFE
    ANDS                R0, R1
    STR                 R0, [SP, #4*6]
    LDR                 R1, =$EXC_RETURN                        ; Make up the EXC_RETURN.
    MOV                 LR, R1
    MEND


;********************************************************************************************************
;
; Note(s) : 1) See whether the frame holds an alignment padding word: bit 9 of
;              the stacked XPSR is used to mark it.
;********************************************************************************************************

    MACRO
    OS_CPU_ALIGN_POP   $LABEL
    LDR                 R0, [SP, #4*7]                          ; Load up the XPSR.
    TST                 R0, #0x00000200
    BNE                 $LABEL
    MEND


;********************************************************************************************************
;
; Note(s) : 1) Unstack an exception frame of $SZ words.  The original sequence
;              is [PAD] [FPU] XPSR PC LR R12 R3-R0, which is not ideal for a
;              manual restoring, because restoring PC and SP simultaneously must
;              be the very last step.  Thus the frame is transformed in place
;              first:
;                1. With [PAD] or [FPU]: [PAD] [FPU] XPSR PC LR R12 R3-R0 -> PC R0 XPSR [EMPTY] R12 R3-R1 LR
;                2. Without either:      XPSR PC LR R12 R3-R0             -> PC R0 XPSR R12 R3-R1 LR
;              The transformation restores LR first so it cannot be overwritten,
;              pops R0, loads XPSR and PC and rearranges them at the end, pops
;              R1-R3 and LR while skipping the middle, and finally restores XPSR
;              through R0, then R0 and PC.  Nothing is ever placed below the
;              current SP, because a racing interrupt would clobber it.
;
;           2) The EPSR ICI/IT field of the frame is checked on the fly: dirty
;              bits cannot be restored here (see note #5 of the file header).
;********************************************************************************************************

    MACRO
    OS_CPU_EXC_POP     $SZ
    LDR                 LR, [SP, #4*5]                          ; Restore the frame's LR.
    POP                 {R0}                                    ; Load R0/XPSR/PC into R0/R1/R2.
    LDR                 R1, [SP, #4*6]
    LDR                 R12, =0x0600FC00                        ; The EPSR ICI/IT field.
    TST                 R1, R12                                 ; Dirty ICI/IT cannot be restored here.
    BNE                 OSCtxSw_Err                             ; Defense in depth - see the header.
    AND                 R1, #0xFFFFFDFF                         ; Clear XPSR[9].
    LDR                 R2, [SP, #4*5]
    ORR                 R2, #0x00000001                         ; Set PC[0].
    STR                 R1, [SP, #4*($SZ-4)]                    ; Rearrange to H-PC-R0-XPSR-L.
    STR                 R0, [SP, #4*($SZ-3)]
    STR                 R2, [SP, #4*($SZ-2)]
    POP                 {R1-R3, R12}                            ; Pop the GP registers.
    IF                  (($SZ-8)!=0)                            ; Skip the FPU area and the padding.
    ADD                 SP, #4*($SZ-8)
    ENDIF
    POP                 {R0}                                    ; Pop the XPSR through R0.
    MSR                 XPSR, R0
    POP                 {R0, PC}                                ; Pop R0 and PC.
    MEND


;********************************************************************************************************
;                                          START MULTITASKING
;                                       void  OSStartHighRdy(void)
;
; Note(s) : 1) This function abandons the startup stack and switches to the stack of the highest
;              priority ready task, whose TCB pointer is provided by the kernel in OSTCBHighRdyPtr.
;              The virtual timers and the context switch handler are hooked up before the switch,
;              and the virtual interrupts are enabled just like the official ports do before the
;              first task runs.  The initial stack frame is laid down by OSTaskStkInit() in
;              os_cpu_c.c; its five hypercall parameter words are simply discarded, because the
;              first task starts with a clean hypercall state.
;
;           2) The task entry point is entered through LR, with the task return address pushed
;              below the stack pointer, so that a task returning from its body would observe a
;              stack that the official ports would have presented as well.
;********************************************************************************************************

    AREA                OS_CPU_A, CODE, READONLY, ALIGN=2
    THUMB
    REQUIRE8
    PRESERVE8

OSStartHighRdy     PROC
    BL                  OS_CPU_Port_Init                        ; Hook the tick and the context handler.

    LDR                 R3, =OSRunning                          ; The kernel is running from now on.
    MOVS                R1, #1
    STRB                R1, [R3]

    LDR                 R3, =OSTCBHighRdyPtr                    ; The TCB of the task to start.
    LDR                 R3, [R3]
    LDR                 R0, [R3]                                ; The task stack pointer.
    MOV                 SP, R0                                  ; Abandon the startup stack.

    POP                 {R0-R4}                                 ; Discard the hypercall parameters.
    POP                 {R4-R7}                                 ; Restore R4-R7.
    POP                 {R0-R3}                                 ; Restore R8-R11 through R0-R3.
    MOV                 R8, R0
    MOV                 R9, R1
    MOV                 R10, R2
    MOV                 R11, R3
    POP                 {R0}                                    ; Discard the stacked EXC_RETURN.
    POP                 {R0-R4}                                 ; Restore R0-R3, R12.
    MOV                 R12, R4
    POP                 {R4}                                    ; The task return address, discarded.
    POP                 {R0}                                    ; The task entry point.
    MOV                 LR, R0                                  ; Enter the task through LR.
    PUSH                {R4}                                    ; Keep the return address below SP.
    BX                  LR                                      ; Branch to the task entry.
    ENDP


;********************************************************************************************************
;                              PERFORM A CONTEXT SWITCH (FROM TASK LEVEL)
;                                       void  OSCtxSw_Swt(void)
;
; Note(s) : 1) This is the fast-path half of OSCtxSw() in os_cpu_c.c, entered
;              with the virtual interrupts already masked by the scheduler
;              critical section of OSSched(), and with the next task already
;              selected in OSTCBHighRdyPtr.  It performs the switch directly on
;              the task stacks and unmasks the interrupts itself once the new
;              stack has been loaded.
;
;           2) The current context is masqueraded as if it were stacked by an
;              exception entry, so a task saved here can be recovered by the
;              interrupt driven switches through OS_Ctx_Handler() in
;              os_cpu_c.c, and vice versa.
;
;           3) The register stacking may be nested by an interrupt that arrives
;              before the interrupts are masked.  This is allowed, but must be
;              taken into account when dimensioning the task stacks.
;
;           4) The masking around the TCB exchange is idempotent, and the
;              unmasking happens after the new task frame has been loaded, so
;              the mask snapshot carried by CPU_SR in cpu_rvm.c remains
;              consistent across the switch.
;********************************************************************************************************

OSCtxSw_Swt        PROC
    PUSH                {R0}                                    ; Protect R0 and XPSR.
    MRS                 R0, XPSR
    PUSH                {R0}
    MRS                 R0, CONTROL
    TST                 R0, #0x00000004                         ; CONTROL.FPCA - the frame type.
    BNE                 OSCtxSw_Extended

OSCtxSw_Basic                                                   ; Basic frame.
    OS_CPU_ALIGN_PUSH  OSCtxSw_Basic_Unaligned
OSCtxSw_Basic_Aligned                                           ; Originally 8 byte aligned stack.
    OS_CPU_EXC_PUSH    8, 0x01000000, 0xFFFFFFFD
    B                   OSCtxSw_Stacked
OSCtxSw_Basic_Unaligned                                         ; Originally unaligned stack.
    OS_CPU_EXC_PUSH    8+1, 0x01000200, 0xFFFFFFFD
    B                   OSCtxSw_Stacked

OSCtxSw_Extended                                                ; Extended frame.
    OS_CPU_ALIGN_PUSH  OSCtxSw_Extended_Unaligned
OSCtxSw_Extended_Aligned                                        ; Originally 8 byte aligned stack.
    OS_CPU_EXC_PUSH    8+17+1, 0x01000000, 0xFFFFFFED
    B                   OSCtxSw_Extended_Saved
OSCtxSw_Extended_Unaligned                                      ; Originally unaligned stack.
    OS_CPU_EXC_PUSH    8+17+2, 0x01000200, 0xFFFFFFED

OSCtxSw_Extended_Saved                                          ; Extended frame extra pushing.
    ADD                 R0, SP, #4*8                            ; Locate the FPU stacking area.
    DCI                 0xECA0                                  ; Push FPU regs.
    DCI                 0x0A10                                  ; VSTMIA R0!,{S0-S15}
    DCI                 0xEEF1                                  ; Push FPSCR.
    DCI                 0x1A10                                  ; VMRS R1,FPSCR
    STR                 R1, [R0]
    DCI                 0xED2D                                  ; Push OS-managed FPU regs.
    DCI                 0x8A10                                  ; VPUSH {S16-S31}


OSCtxSw_Stacked
    PUSH                {R4-R11, LR}                            ; Push GP registers.

    LDR                 R0, =RVM_Usr_Param                      ; Push the hypercall parameters.
    LDR                 R0, [R0]
    LDMIA               R0!, {R1-R5}
    PUSH                {R1-R5}

    BL                  RVM_Virt_Int_Mask                       ; Disable the virtual interrupts.

    LDR                 R1, =OSTCBCurPtr                        ; OSTCBCurPtr->StkPtr = SP;
    LDR                 R1, [R1]
    MOV                 R2, SP
    STR                 R2, [R1]

    BL                  OSTaskSwHook                            ; The switch bookkeeping of the
                                                                ; official PendSV handler.
    LDR                 R0, =OSPrioCur                          ; OSPrioCur   = OSPrioHighRdy;
    LDR                 R1, =OSPrioHighRdy
    LDRB                R2, [R1]
    STRB                R2, [R0]

    LDR                 R0, =OSTCBCurPtr                        ; OSTCBCurPtr = OSTCBHighRdyPtr;
    LDR                 R1, =OSTCBHighRdyPtr
    LDR                 R2, [R1]
    STR                 R2, [R0]

    LDR                 R1, [R2]                                ; R1 is new process SP; SP = OSTCBHighRdyPtr->StkPtr;
    MOV                 SP, R1

    BL                  RVM_Virt_Int_Unmask                     ; Enable the virtual interrupts.

    LDR                 R0, =RVM_Usr_Param                      ; Pop the hypercall parameters.
    LDR                 R0, [R0]
    POP                 {R1-R5}
    STMIA               R0!, {R1-R5}

    POP                 {R4-R11, LR}                            ; Pop GP registers.

    TST                 LR, #0x00000010                         ; LR.EXTENDED - the frame type.
    BEQ                 OSCtxSw_Unstack_Extended


OSCtxSw_Unstack_Basic                                           ; Basic frame.
    OS_CPU_ALIGN_POP   OSCtxSw_Unstack_Basic_Pad
OSCtxSw_Unstack_Basic_NoPad                                     ; Frame without alignment padding.
    OS_CPU_EXC_POP     8
    B                   OSCtxSw_Exit
OSCtxSw_Unstack_Basic_Pad                                       ; Frame with alignment padding.
    OS_CPU_EXC_POP     8+1
    B                   OSCtxSw_Exit

OSCtxSw_Unstack_Extended                                        ; Extended frame extra popping.
    DCI                 0xECBD                                  ; Pop FPU regs.
    DCI                 0x8A10                                  ; VPOP {S16-S31}
    ADD                 R0, SP, #4*(8+16)
    LDR                 R1, [R0]
    DCI                 0xEEE1                                  ; Pop FPSCR.
    DCI                 0x1A10                                  ; VMSR FPSCR,R1
    DCI                 0xED30                                  ; Pop FPU regs.
    DCI                 0x0A10                                  ; VLDMDB R0!,{S0-S15}
    OS_CPU_ALIGN_POP   OSCtxSw_Unstack_Extended_Pad
OSCtxSw_Unstack_Extended_NoPad                                  ; Frame without alignment padding.
    OS_CPU_EXC_POP     8+17+1
    B                   OSCtxSw_Exit
OSCtxSw_Unstack_Extended_Pad                                    ; Frame with alignment padding.
    OS_CPU_EXC_POP     8+17+2

OSCtxSw_Exit                                                    ; Exit location - also the resume PC.
    BX                  LR
    ENDP


    LTORG


;********************************************************************************************************
;                                             END OF FILE
;********************************************************************************************************

    END

