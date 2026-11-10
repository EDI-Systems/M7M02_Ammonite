/* SPDX-License-Identifier: Unlicense */

/**
 * @file
 *
 * @ingroup RTEMSBSPsARMV7MRVM
 *
 * @brief Interrupt definitions for the RVM ARMv7-M guest.
 *
 * The guest owns no hardware vector table.  The RVM monitor delivers virtual
 * vectors to the guest vector thread; the count below must match the
 * Vector_Num value of the RVM project.
 */

#ifndef LIBBSP_ARM_V7M_RVM_IRQ_H
#define LIBBSP_ARM_V7M_RVM_IRQ_H

#ifndef ASM

#include <rtems.h>
#include <rtems/irq.h>
#include <rtems/irq-extension.h>
#include <bspopts.h>

#include "rte_config.h"

#endif /* ASM */

#define BSP_INTERRUPT_VECTOR_COUNT RVM_VIRT_VCT_NUM

#endif /* LIBBSP_ARM_V7M_RVM_IRQ_H */
