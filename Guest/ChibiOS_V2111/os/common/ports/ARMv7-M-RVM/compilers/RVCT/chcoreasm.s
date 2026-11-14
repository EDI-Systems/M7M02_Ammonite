; CHB port for the RVM hypervisor.
; Copyright (C) 2026 Evo-Devo Instrum. All rights reserved.
;
; This file is NOT part of the ChibiOS distribution.  It belongs to the
; ChibiOS-on-RVM guest port and is released under the Unlicense; refer to
; the LICENSE file for details.  The ChibiOS kernel itself is copyright
; Giovanni Di Sirio and is not modified by this file.

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

; Read the current xPSR for the fast-path ICI/IT checks.
        EXPORT  chb_get_xpsr
chb_get_xpsr    PROC
        MRS     r0, xPSR
        BX      lr
        ENDP

; Direct task-to-task switch.
        EXPORT  chb_fast_switch
chb_fast_switch PROC
        MRS     r0, CONTROL
        TST     r0, #0x00000004
        BNE     chb_fast_extended

chb_fast_basic
        SUB     sp, sp, #32
        STMIA   sp, {r0-r3, r12, lr}
        LDR     r0, =chb_fast_exit
        BIC     r0, r0, #1
        STR     r0, [sp, #24]
        MRS     r0, xPSR
        ORR     r0, r0, #0x01000000
        STR     r0, [sp, #28]
        LDR     lr, =0xFFFFFFFD
        B       chb_fast_stacked

chb_fast_extended
        SUB     sp, sp, #104
        STMIA   sp, {r0-r3, r12, lr}
        LDR     r0, =chb_fast_exit
        BIC     r0, r0, #1
        STR     r0, [sp, #24]
        MRS     r0, xPSR
        ORR     r0, r0, #0x01000000
        STR     r0, [sp, #28]
        ADD     r0, sp, #32
        DCW     0xECA0, 0x0A10      ; vstmia r0!, {s0-s15}
        DCW     0xEEF1, 0x1A10      ; vmrs r1, fpscr
        STR     r1, [r0]
        DCW     0xED2D, 0x8A10      ; vpush {s16-s31}
        LDR     lr, =0xFFFFFFED

chb_fast_stacked
        PUSH    {r4-r11, lr}

        LDR     r0, =RVM_Usr_Param
        LDR     r0, [r0]
        LDMIA   r0, {r1-r5}
        PUSH    {r1-r5}

        MOV     r0, sp
        BL      chb_fast_save
        BL      chb_fast_load
        MOV     sp, r0

        BL      RVM_Virt_Int_Unmask

        LDR     r0, =RVM_Usr_Param
        LDR     r0, [r0]
        LDMIA   sp, {r1-r5}
        STMIA   r0, {r1-r5}
        ADD     sp, sp, #20
        POP     {r4-r11, lr}

        TST     lr, #0x00000010
        BEQ     chb_fast_unstack_extended

        LDR     lr, [sp, #20]
        LDR     r1, [sp, #28]
        BIC     r1, r1, #0x00000200
        MSR     APSR_nzcvq, r1
        LDR     r2, [sp, #24]
        ORR     r2, r2, #1
        LDR     r3, [sp, #0]
        STR     r1, [sp, #20]
        STR     r3, [sp, #24]
        STR     r2, [sp, #28]
        LDR     r1, [sp, #4]
        LDR     r2, [sp, #8]
        LDR     r3, [sp, #12]
        LDR     r12, [sp, #16]
        ADD     sp, sp, #24
        LDMIA   sp!, {r0, pc}

chb_fast_unstack_extended
        DCW     0xECBD, 0x8A10      ; vpop {s16-s31}
        ADD     r0, sp, #96
        LDR     r1, [r0]
        DCW     0xEEE1, 0x1A10      ; vmsr fpscr, r1
        DCW     0xED30, 0x0A10      ; vldmdb r0!, {s0-s15}
        LDR     lr, [sp, #20]
        LDR     r1, [sp, #28]
        BIC     r1, r1, #0x00000200
        MSR     APSR_nzcvq, r1
        LDR     r2, [sp, #24]
        ORR     r2, r2, #1
        LDR     r3, [sp, #0]
        STR     r1, [sp, #92]
        STR     r3, [sp, #96]
        STR     r2, [sp, #100]
        LDR     r1, [sp, #4]
        LDR     r2, [sp, #8]
        LDR     r3, [sp, #12]
        LDR     r12, [sp, #16]
        ADD     sp, sp, #96
        LDMIA   sp!, {r0, pc}

chb_fast_exit
        BX      lr
        ENDP

        ALIGN
        LTORG

        END
