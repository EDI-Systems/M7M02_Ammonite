/* SPDX-License-Identifier: Unlicense */

/**
 * @file
 *
 * @ingroup RTEMSBSPsARMV7MRVM
 *
 * @brief RVM guest startup support.
 */

#include <bsp.h>
#include <bsp/bootcard.h>

void bsp_start( void )
{
  /*
   * The RVM monitor owns the physical vector table, VTOR, NVIC and SysTick.
   * There is nothing to program here.  Virtual vectors are registered with
   * the RVM vector thread, see rvm_entry.c.
   */
}
