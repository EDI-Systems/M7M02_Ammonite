/* SPDX-License-Identifier: Unlicense */

#ifndef ZPR_ABI_H
#define ZPR_ABI_H

/* Layout shared by zpr_context.c, thread.h and the context-switch assembly. */
#define ZPR_CTX_SP_OFFSET 36U
#define ZPR_CTX_NEST_OFFSET 40U
#define ZPR_BASIC_FRAME_WORDS 22U
#define ZPR_EXT_FRAME_WORDS 56U
#define ZPR_BASIC_HW_WORD 14U
#define ZPR_EXT_HW_WORD 30U
#define ZPR_FRAME_PARAMS 5U
#define ZPR_FRAME_GP 9U
#define ZPR_FRAME_FP16 16U
#define ZPR_FRAME_BASIC 8U
#define ZPR_FRAME_FP_AREA 18U
#define ZPR_FRAME_BASIC_PUSH 8U
#define ZPR_FRAME_EXT_PUSH 26U
#define ZPR_FRAME_GP_BYTES 36U
#define ZPR_FRAME_EXT_TAIL_BYTES 100U

#define ZPR_EXC_RETURN_BASIC 0xFFFFFFFDU
#define ZPR_EXC_RETURN_EXT 0xFFFFFFEDU
#define ZPR_XPSR_ICI_IT_MASK 0x0600FC00U
#define ZPR_XPSR_ALIGN_MASK 0x00000200U

#endif /* ZPR_ABI_H */
