; CHB port for the RVM hypervisor.
; Copyright (C) 2026 Evo-Devo Instrum. All rights reserved.
;
; This file is NOT part of the ChibiOS distribution.  It belongs to the
; ChibiOS-on-RVM guest port and is released under the Unlicense; refer to
; the LICENSE file for details.  The ChibiOS kernel itself is copyright
; Giovanni Di Sirio and is not modified by this file.
;
; ARMv6-M variant: no FPU and no IT blocks, so every frame is a basic
; frame.  armasm runs without the C preprocessor.  ARMv6-M PUSH/POP only
; take the low registers plus LR, so R8-R11 travel through R0-R3 and
; R12/LR are stored with explicit STR; every non-byte immediate is loaded
; as a literal.

        PRESERVE8
        THUMB
        AREA    |.text|, CODE, READONLY

        IMPORT  chb_thread_start
        IMPORT  chb_fast_save
        IMPORT  chb_fast_load
        IMPORT  RVM_Usr_Param
        IMPORT  RVM_Virt_Int_Unmask

; The first-entry trampoline stored in a new thread's basic frame.
        EXPORT  __port_thread_start
__port_thread_start PROC
        MOV     r0, r4
        MOV     r1, r5
        BL      chb_thread_start
chb_thread_start_hang
        B       chb_thread_start_hang
        ENDP

; Read the current xPSR.  The ICI/IT field always reads zero on ARMv6-M;
; the routine keeps the C side identical to the ARMv7-M port.
        EXPORT  chb_get_xpsr
chb_get_xpsr    PROC
        MRS     r0, xPSR
        BX      lr
        ENDP

; Direct task-to-task switch.  The hardware frame is [PAD] XPSR PC LR R12
; R3-R0 from the stack pointer upwards; entry protects R0, XPSR and R1 by
; three pushes, so the frame build reads them back from there.
        EXPORT  chb_fast_switch
chb_fast_switch PROC
        PUSH    {r0}
        MRS     r0, xPSR
        PUSH    {r0}
        PUSH    {r1}

        MOV     r0, sp
        LDR     r1, =0x00000007
        TST     r0, r1
        BEQ     chb_fast_unaligned

; Originally 8 byte aligned: one word of room for R12/LR/PC/XPSR.
chb_fast_aligned
        LDR     r0, [sp, #8]
        LDR     r1, [sp]
        SUB     sp, sp, #4
        PUSH    {r0-r3}
        MOV     r0, r12
        MOV     r1, lr
        STR     r0, [sp, #16]
        STR     r1, [sp, #20]
        LDR     r0, [sp, #24]
        LDR     r1, =0x01000000
        ORRS    r0, r0, r1
        STR     r0, [sp, #28]
        LDR     r0, =chb_fast_exit
        LDR     r1, =0xFFFFFFFE
        ANDS    r0, r0, r1
        STR     r0, [sp, #24]
        LDR     r1, =0xFFFFFFFD
        MOV     lr, r1
        B       chb_fast_stacked

; Originally unaligned: two words of room and the pad marker in XPSR.
chb_fast_unaligned
        LDR     r0, [sp, #8]
        LDR     r1, [sp]
        SUB     sp, sp, #8
        PUSH    {r0-r3}
        MOV     r0, r12
        MOV     r1, lr
        STR     r0, [sp, #16]
        STR     r1, [sp, #20]
        LDR     r0, [sp, #28]
        LDR     r1, =0x01000200
        ORRS    r0, r0, r1
        STR     r0, [sp, #28]
        LDR     r0, =chb_fast_exit
        LDR     r1, =0xFFFFFFFE
        ANDS    r0, r0, r1
        STR     r0, [sp, #24]
        LDR     r1, =0xFFFFFFFD
        MOV     lr, r1

chb_fast_stacked
        PUSH    {lr}
        MOV     r0, r8
        MOV     r1, r9
        MOV     r2, r10
        MOV     r3, r11
        PUSH    {r0-r3}
        PUSH    {r4-r7}

        LDR     r0, =RVM_Usr_Param
        LDR     r0, [r0]
        LDMIA   r0!, {r1-r5}
        PUSH    {r1-r5}

        MOV     r0, sp
        BL      chb_fast_save
        BL      chb_fast_load
        MOV     sp, r0

        BL      RVM_Virt_Int_Unmask

        LDR     r0, =RVM_Usr_Param
        LDR     r0, [r0]
        POP     {r1-r5}
        STMIA   r0!, {r1-r5}

        POP     {r4-r7}
        POP     {r0-r3}
        MOV     r8, r0
        MOV     r9, r1
        MOV     r10, r2
        MOV     r11, r3
        POP     {r0}
        MOV     lr, r0

        LDR     r0, [sp, #28]
        LDR     r1, =0x00000200
        TST     r0, r1
        BNE     chb_fast_pop9

; Unstack the 8 word hardware frame in place.
chb_fast_pop8
        LDR     r0, [sp, #16]
        LDR     r1, [sp, #20]
        MOV     r12, r0
        MOV     lr, r1
        POP     {r0}
        LDR     r1, [sp, #24]
        LDR     r3, =0xFFFFFDFF
        ANDS    r1, r1, r3
        LDR     r2, [sp, #20]
        MOVS    r3, #1
        ORRS    r2, r2, r3
        STR     r1, [sp, #16]
        STR     r0, [sp, #20]
        STR     r2, [sp, #24]
        POP     {r1-r3}
        ADD     sp, sp, #4
        POP     {r0}
        MSR     APSR_nzcvq, r0
        POP     {r0, pc}

; Unstack the 9 word hardware frame (alignment pad above XPSR).
chb_fast_pop9
        LDR     r0, [sp, #16]
        LDR     r1, [sp, #20]
        MOV     r12, r0
        MOV     lr, r1
        POP     {r0}
        LDR     r1, [sp, #24]
        LDR     r3, =0xFFFFFDFF
        ANDS    r1, r1, r3
        LDR     r2, [sp, #20]
        MOVS    r3, #1
        ORRS    r2, r2, r3
        STR     r1, [sp, #20]
        STR     r0, [sp, #24]
        STR     r2, [sp, #28]
        POP     {r1-r3}
        ADD     sp, sp, #8
        POP     {r0}
        MSR     APSR_nzcvq, r0
        POP     {r0, pc}

chb_fast_exit
        BX      lr
        ENDP

        ALIGN
        LTORG

        END
