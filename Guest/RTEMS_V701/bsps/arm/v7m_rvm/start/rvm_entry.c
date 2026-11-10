/* SPDX-License-Identifier: Unlicense */

/**
 * @file
 *
 * @ingroup RTEMSBSPsARMV7MRVM
 *
 * @brief RTE guest entry glue.
 *
 * The RVM monitor starts two threads from the descriptor at the image base.
 * Both the descriptor (rte_entry_desc.c) and the vector thread entry
 * (rte_entry.c) are staged from the RVM generator and compiled by this BSP;
 * this file only supplies the user-thread entry point and the vector handler
 * registration.  RTEMS does not own a hardware vector table; the CPU port
 * uses the RVM virtual vector API.
 */

#include <bsp.h>
#include <bsp/bootcard.h>

#include <rtems.h>

#include "rvm.h"

void rvm_bsp_register_handlers( void );

void rte_rvm_register_vectors( void );

void bsp_start( void )
{
  /*
   * The RVM monitor owns the physical vector table, VTOR, NVIC and SysTick.
   * There is nothing to program here.  Virtual vectors are registered with
   * the RVM vector thread below.
   */
}

RTEMS_NO_RETURN void bsp_reset(
  rtems_fatal_source source,
  rtems_fatal_code code
)
{
  (void) source;
  (void) code;

  RVM_Hyp_Reboot();

  for (;;) {
    /* RVM_Hyp_Reboot() does not return if the hypervisor is alive. */
  }
}

/*
 * Called by the user thread after the higher priority vector thread has
 * completed RVM_Virt_Init().  RVM_Virt_Init() clears the RVM handler table,
 * so the timer and context handlers must be installed here, not during RTEMS
 * initialization.
 */
void rvm_bsp_register_handlers( void )
{
  rte_rvm_register_vectors();
}

int main( void )
{
  /*
   * The vector thread has a higher priority and completes RVM_Virt_Init()
   * before this user thread starts.  Install the virtual vectors here; RVM
   * interrupts remain masked until the first context restore enables them.
   */
  rvm_bsp_register_handlers();

  boot_card( "" );

  for (;;) {
    /* boot_card() does not return. */
  }
}
