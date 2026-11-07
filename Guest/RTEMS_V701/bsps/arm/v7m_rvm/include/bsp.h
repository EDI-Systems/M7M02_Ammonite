/* SPDX-License-Identifier: Unlicense */

/**
 * @file
 *
 * @ingroup RTEMSBSPsARMV7MRVM
 *
 * @brief Global BSP definitions for the RVM ARMv7-M guest.
 */

#ifndef LIBBSP_ARM_V7M_RVM_BSP_H
#define LIBBSP_ARM_V7M_RVM_BSP_H

/**
 * @defgroup RTEMSBSPsARMV7MRVM RVM ARMv7-M guest
 *
 * @ingroup RTEMSBSPsARM
 *
 * @brief RVM ARMv7-M guest support package.
 */

#include <bspopts.h>

/*
 * No hardware vector table, NVIC, VTOR or SysTick is owned by the guest.
 * IRQ priority definitions exist only for code that includes them; the
 * RVM variant of the CPU port never programs the NVIC.
 */
#define BSP_ARMV7M_IRQ_PRIORITY_DEFAULT (13 << 4)
#define BSP_ARMV7M_SYSTICK_PRIORITY (14 << 4)

#ifndef ASM

#include <bsp/default-initial-extension.h>

#include <rtems.h>

#endif /* ASM */

#endif /* LIBBSP_ARM_V7M_RVM_BSP_H */
