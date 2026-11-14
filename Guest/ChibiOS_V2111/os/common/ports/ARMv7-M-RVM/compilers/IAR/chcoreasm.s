/*
    CHB port for the RVM hypervisor.
    Copyright (C) 2026 Evo-Devo Instrum. All rights reserved.

    This file is NOT part of the ChibiOS distribution.  It belongs to the
    ChibiOS-on-RVM guest port and is released under the Unlicense; refer to
    the LICENSE file for details.  The ChibiOS kernel itself is copyright
    Giovanni Di Sirio and is not modified by this file.
*/

/**
 * @file    ARMv7-M-RVM/compilers/IAR/chcoreasm.s
 * @brief   CHB port low level code for IAR EWARM.
 *
 * The slow path is implemented in C on the vector thread.  This file
 * provides the first-entry trampoline and the task-level direct switch used
 * by the CHB fast path.  The frame layout is identical to the GCC port:
 * five hypercall parameters, R4-R11/EXC_RETURN, the optional S16-S31 block,
 * the hardware frame and the S0-S15/FPSCR area.
 */

#if !defined(FALSE)
#define FALSE 0
#endif
#if !defined(TRUE)
#define TRUE 1
#endif

#define _FROM_ASM_
#include "chlicense.h"
#include "chconf.h"
#include "chcore.h"

#if !defined(__DOXYGEN__)

                MODULE  ?chcoreasm_v7m

                AAPCS INTERWORK, VFP_COMPATIBLE
                PRESERVE8

                SECTION .text:CODE:NOROOT(2)

                EXTERN  chb_thread_start
                EXTERN  chb_fast_save
                EXTERN  chb_fast_load
                EXTERN  RVM_Usr_Param
                EXTERN  RVM_Virt_Int_Unmask

                THUMB

/* The first-entry trampoline stored in a new thread's basic frame. */
                PUBLIC  __port_thread_start
__port_thread_start:
                MOV     r0, r4
                MOV     r1, r5
                BL      chb_thread_start
chb_thread_start_hang:
                B       chb_thread_start_hang

/* Read the current xPSR for the fast-path ICI/IT checks. */
                PUBLIC  chb_get_xpsr
chb_get_xpsr:
                MRS     r0, xPSR
                BX      lr

#if CHB_FAST_PATH

/*
 * Direct task-to-task switch.  Entered from chb_port_switch() with the
 * virtual interrupts masked.  The C side has already checked the target
 * frame and the current ICI/IT state.
 */
                PUBLIC  chb_fast_switch
chb_fast_switch:
                MRS     r0, CONTROL
#if CHB_USE_FPU
                TST     r0, #0x00000004
                BNE     chb_fast_extended
#endif

chb_fast_basic:
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

#if CHB_USE_FPU
chb_fast_extended:
                SUB     sp, sp, #104
                STMIA   sp, {r0-r3, r12, lr}
                LDR     r0, =chb_fast_exit
                BIC     r0, r0, #1
                STR     r0, [sp, #24]
                MRS     r0, xPSR
                ORR     r0, r0, #0x01000000
                STR     r0, [sp, #28]
                ADD     r0, sp, #32
                VSTMIA  r0!, {s0-s15}
                VMRS    r1, FPSCR
                STR     r1, [r0]
                VPUSH   {s16-s31}
                LDR     lr, =0xFFFFFFED
#endif

chb_fast_stacked:
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

#if CHB_USE_FPU
                TST     lr, #0x00000010
                BEQ     chb_fast_unstack_extended
#endif

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

chb_fast_exit:
                BX      lr

#if CHB_USE_FPU
chb_fast_unstack_extended:
                VPOP    {s16-s31}
                ADD     r0, sp, #96
                LDR     r1, [r0]
                VMSR    FPSCR, r1
                ADD     r0, sp, #32
                VLDMIA  r0!, {s0-s15}
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
#endif

#endif /* CHB_FAST_PATH */

                END

#endif /* !defined(__DOXYGEN__) */

/** @} */
