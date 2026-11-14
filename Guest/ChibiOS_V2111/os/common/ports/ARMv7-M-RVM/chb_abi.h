/*
    CHB port for the RVM hypervisor.
    Copyright (C) 2026 Evo-Devo Instrum. All rights reserved.

    This file is NOT part of the ChibiOS distribution.  It belongs to the
    ChibiOS-on-RVM guest port and is released under the Unlicense; refer to
    the LICENSE file for details.  The ChibiOS kernel itself is copyright
    Giovanni Di Sirio and is not modified by this file.
*/

/**
 * @file    ARMv7-M-RVM/chb_abi.h
 * @brief   RVM guest context frame layout for the CHB port.
 *
 * The frame below is the masqueraded exception frame used by the RVM
 * monitor, from the lowest address upwards:
 *
 *   Number, Param[0..3]                 5 words
 *   R4..R11, EXC_RETURN                 9 words
 *   R0..R3, R12, LR, PC, xPSR           8 words
 *
 * The extended (FPU) frame adds S16..S31 before the hardware frame and
 * S0..S15/FPSCR above it.  New threads always start with a basic frame;
 * the first VFP instruction makes later saves extended.
 */

#ifndef CHB_ABI_H
#define CHB_ABI_H

#define CHB_FRAME_PARAMS        5U
#define CHB_FRAME_GP            9U
#define CHB_FRAME_BASIC         8U
#define CHB_FRAME_BASIC_WORDS   (CHB_FRAME_PARAMS + CHB_FRAME_GP + CHB_FRAME_BASIC)
#define CHB_FRAME_FP16          16U
#define CHB_FRAME_FP_AREA       18U
#define CHB_FRAME_EXT_HW        (CHB_FRAME_PARAMS + CHB_FRAME_GP + CHB_FRAME_FP16)
#define CHB_FRAME_EXT_WORDS     (CHB_FRAME_EXT_HW + CHB_FRAME_BASIC + CHB_FRAME_FP_AREA)

#define CHB_EXC_RETURN_BASIC    0xFFFFFFFDU
#define CHB_EXC_RETURN_EXT      0xFFFFFFEDU
#define CHB_XPSR_THUMB          0x01000000U
#define CHB_XPSR_ICI_IT_MASK    0x0600FC00U

#if !defined(CHB_FAST_PATH)
#define CHB_FAST_PATH           1
#endif

#endif /* CHB_ABI_H */
