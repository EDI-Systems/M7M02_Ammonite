/* SPDX-License-Identifier: Unlicense */

/**
 * @file
 *
 * @ingroup RTEMSBSPsARMV7MRVM
 *
 * @brief RTE RVM context frame layout.
 *
 * This is the single source of truth shared by the C context code and the
 * assembly switch.  The numbers describe the masqueraded exception frame,
 * from the lowest address upwards:
 *
 *   Number, Param[0..3]                 5 words
 *   R4..R11, EXC_RETURN                 9 words
 *   S16..S31                           16 words, extended frame only
 *   R0..R3, R12, LR, PC, xPSR           8 words, basic hardware frame
 *   S0..S15, FPSCR, [pad]              18 words, extended frame only
 */

#ifndef LIBBSP_ARM_V7M_RVM_RTE_FRAME_H
#define LIBBSP_ARM_V7M_RVM_RTE_FRAME_H

/* Offsets in Context_Control, also used by the assembly switch. */
#define RTE_CC_SP_OFFSET 36
#define RTE_CC_NEST_OFFSET 40

/* Hypercall parameter area and saved general purpose registers. */
#define RTE_FRAME_PARAMS 5
#define RTE_FRAME_GP 9

/* Extended-frame floating point areas. */
#define RTE_FRAME_FP16 16
#define RTE_FRAME_FP_AREA 18

/* Basic hardware frame. */
#define RTE_FRAME_BASIC 8

/* Total frame words. */
#define RTE_FRAME_BASIC_WORDS 22
#define RTE_FRAME_EXT_WORDS 56

/* Offset of the basic hardware frame in each frame type. */
#define RTE_FRAME_BASIC_HW (RTE_FRAME_PARAMS + RTE_FRAME_GP)
#define RTE_FRAME_EXT_HW \
  (RTE_FRAME_PARAMS + RTE_FRAME_GP + RTE_FRAME_FP16)

/*
 * Hardware frame built by the assembly prologue before the S16-S31 block:
 * R0-R3, R12, LR, PC, xPSR plus the S0-S15/FPSCR area.  The two protected
 * entry words are part of this count.
 */
#define RTE_FRAME_BASIC_PUSH RTE_FRAME_BASIC
#define RTE_FRAME_EXT_PUSH (RTE_FRAME_BASIC + RTE_FRAME_FP_AREA)

/* Byte sizes used by the assembly prologue/epilogue. */
#define RTE_FRAME_GP_BYTES (RTE_FRAME_GP * 4)
#define RTE_FRAME_EXT_TAIL_BYTES \
  (RTE_FRAME_GP_BYTES + RTE_FRAME_FP16 * 4)

/* EXC_RETURN values with the FPCA bit selecting the frame type. */
#define RTE_EXC_RETURN_BASIC 0xFFFFFFFDU
#define RTE_EXC_RETURN_EXT 0xFFFFFFEDU

/* Bit 9 marks an alignment padding word in the stacked xPSR. */
#define RTE_XPSR_ALIGN_MASK 0x00000200U

/* EPSR ICI/IT continuation bits; a dirty frame must use the slow path. */
#define RTE_XPSR_ICI_IT_MASK 0x0600FC00U

#endif /* LIBBSP_ARM_V7M_RVM_RTE_FRAME_H */
