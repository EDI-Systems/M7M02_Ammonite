/****************************************************************************
 * arch/arm/src/armv7-m-rvm/armv7m_rvm_abi.h
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * RVM ARMv7-M context frame, as seen by the guest.
 *
 * Basic frame, from the lowest address:
 *
 *   0..4    hypercall params (Number, Param[0..3])
 *   5..13   R4..R11, EXC_RETURN
 *   14..21  R0..R3, R12, LR, PC, xPSR
 *
 * Extended frame inserts S16..S31 between EXC_RETURN and the hardware frame,
 * and appends S0..S15/FPSCR after it:
 *
 *   0..4    hypercall params
 *   5..13   R4..R11, EXC_RETURN
 *   14..29  S16..S31
 *   30..37  R0..R3, R12, LR, PC, xPSR
 *   38..55  S0..S15, FPSCR, reserved
 ****************************************************************************/

#ifndef __ARCH_ARM_SRC_ARMV7_M_RVM_ABI_H
#define __ARCH_ARM_SRC_ARMV7_M_RVM_ABI_H

#define NTX_RVM_PARAMS          5
#define NTX_RVM_GP              9
#define NTX_RVM_FP16            16
#define NTX_RVM_HW              8
#define NTX_RVM_FP_AREA         18

#define NTX_RVM_BASIC_WORDS     (NTX_RVM_PARAMS + NTX_RVM_GP + NTX_RVM_HW)
#define NTX_RVM_BASIC_HW        (NTX_RVM_PARAMS + NTX_RVM_GP)
#define NTX_RVM_EXT_SP          (NTX_RVM_PARAMS + NTX_RVM_GP + NTX_RVM_FP16)
#define NTX_RVM_EXT_HW          NTX_RVM_EXT_SP
#define NTX_RVM_EXT_WORDS       (NTX_RVM_EXT_SP + NTX_RVM_HW + NTX_RVM_FP_AREA)
#define NTX_RVM_FRAME_MAX       NTX_RVM_EXT_WORDS

#define NTX_EXC_RETURN_BASIC    0xfffffffd
#define NTX_EXC_RETURN_EXT      0xffffffed
#define NTX_XPSR_THUMB          0x01000000
#define NTX_XPSR_ICI_IT_MASK    0x0600fc00

#endif /* __ARCH_ARM_SRC_ARMV7_M_RVM_ABI_H */
