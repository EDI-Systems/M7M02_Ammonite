;********************************************************************************************************
;                                                 uC/CPU
;                                     CPU CONFIGURATION & PORT LAYER
;
;                               Copyright (C) 2026 Evo-Devo Instrum.
;                                       All rights reserved.
;
;                This file is NOT part of the uC/CPU distribution.  It belongs
;                to the uC/OS-III-on-RVM test suite and is released under the
;                Unlicense; refer to the LICENSE file for details.  The original
;                uC/OS-III and uC/CPU distributions are copyright Silicon
;                Laboratories Inc. and are not modified by this file.
;
;********************************************************************************************************
;********************************************************************************************************
;
;                                            ARMv7-M Port
;                                    RVM Hypervisor Guest Layer
;
; File      : cpu_a.s
; Version   : V1.00.00
;********************************************************************************************************
; For       : ARMv7-M Cortex-M3/M4/M7 running as an unprivileged RVM guest
; Mode      : Thumb
; Toolchain : IAR Assembler
;********************************************************************************************************
; Note(s)   : (1) This file carries the count-zeros accelerations of the official
;                 cpu_a.s that the ARMv7-M instruction set actually offers: CLZ
;                 and RBIT.  The interrupt masking and wait-for-interrupt
;                 functions of the official cpu_a.s are replaced by cpu_rvm.c
;                 instead - a guest cannot mask the virtual interrupts of the
;                 hypervisor through PRIMASK or BASEPRI.
;
;             (2) The generator compiles this file only when it is present: the
;                 CPU layers whose instruction set has nothing to accelerate
;                 (ARMv6-M, RISC-V base) ship no cpu_a.s at all and run the C
;                 lookup tables in cpu_core.c, exactly like their official
;                 ports do.
;********************************************************************************************************

;********************************************************************************************************
;                                          PUBLIC FUNCTIONS
;********************************************************************************************************

    PUBLIC  CPU_CntLeadZeros                                    ; See cpu_core.h COUNT BITS FUNCTIONS.
    PUBLIC  CPU_CntTrailZeros
    PUBLIC  CPU_RevBits

;********************************************************************************************************
;                                      CODE GENERATION DIRECTIVES
;********************************************************************************************************

    SECTION             .cpu_a:CODE:NOROOT(2)
    THUMB
    REQUIRE8
    PRESERVE8

;********************************************************************************************************
;                                          COUNT LEADING ZEROS
;
; Description: Counts the number of contiguous, most-significant, leading zero bits before the
;              first binary one bit in a data value.
;
; Prototypes  : CPU_DATA  CPU_CntLeadZeros(CPU_DATA  val);
;
; Argument(s) : val         Data value to count leading zero bits.
;
; Return(s)   : Number of contiguous, most-significant, leading zero bits in 'val'.
;********************************************************************************************************

CPU_CntLeadZeros
    CLZ                 R0,R0                                   ; Count leading zeros
    BX                  LR

;********************************************************************************************************
;                                         COUNT TRAILING ZEROS
;
; Description: Counts the number of contiguous, least-significant, trailing zero bits before the
;              first binary one bit in a data value.
;
; Prototypes  : CPU_DATA  CPU_CntTrailZeros(CPU_DATA  val);
;
; Argument(s) : val         Data value to count trailing zero bits.
;
; Return(s)   : Number of contiguous, least-significant, trailing zero bits in 'val'.
;********************************************************************************************************

CPU_CntTrailZeros
    RBIT                R0,R0                                   ; Reverse bits
    CLZ                 R0,R0                                   ; Count trailing zeros
    BX                  LR

;********************************************************************************************************
;                                            REVERSE BITS
;
; Description: Reverses the bits in a data value.
;
; Prototypes  : CPU_DATA  CPU_RevBits(CPU_DATA  val);
;
; Argument(s) : val         Data value to reverse bits.
;
; Return(s)   : Data value with all bits reversed.
;********************************************************************************************************

CPU_RevBits
    RBIT                R0,R0                                   ; Reverse bits
    BX                  LR


;********************************************************************************************************
;                                             END OF FILE
;********************************************************************************************************

    END
