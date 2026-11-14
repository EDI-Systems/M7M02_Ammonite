/****************************************************************************
 * arch/arm/include/armv7-m-rvm/chip.h
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * The RVM guest has no physical NVIC, SysTick, MPU or SoC peripherals.  The
 * monitor owns those resources.  Only the few architectural constants needed
 * by the generic ARMv7-M headers are provided here.
 ****************************************************************************/

#ifndef __ARCH_ARM_INCLUDE_ARMV7_M_RVM_CHIP_H
#define __ARCH_ARM_INCLUDE_ARMV7_M_RVM_CHIP_H

/* Priorities are retained only so that the common ARMv7-M inline helpers
 * compile.  No guest code may program BASEPRI or the NVIC.
 */

#define NVIC_SYSH_PRIORITY_DEFAULT    0x80
#define NVIC_SYSH_PRIORITY_STEP       0x10

#define ARMV7M_PERIPHERAL_INTERRUPTS  0

#endif /* __ARCH_ARM_INCLUDE_ARMV7_M_RVM_CHIP_H */
