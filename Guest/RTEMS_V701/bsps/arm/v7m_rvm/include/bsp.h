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

#ifndef ASM

#include <bsp/default-initial-extension.h>

#include <rtems.h>

/* The default ARMv7 idle body executes WFI, which halts the physical core
 * under the hypervisor.  Park in the hypervisor instead: confdefs picks
 * this up for every application on this BSP unless the application
 * overrides CONFIGURE_IDLE_TASK_BODY explicitly. */
extern int RVM_Hyp_Vct_Wait( void );

static inline void *rvm_idle_body( uintptr_t ignored )
{
  (void) ignored;

  for (;;) {
    RVM_Hyp_Vct_Wait();
  }

  return NULL; /* not reached */
}

#define BSP_IDLE_TASK_BODY rvm_idle_body

#endif /* ASM */

#endif /* LIBBSP_ARM_V7M_RVM_BSP_H */
