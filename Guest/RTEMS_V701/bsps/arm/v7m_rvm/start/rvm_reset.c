/* SPDX-License-Identifier: Unlicense */

/**
 * @file
 *
 * @ingroup RTEMSBSPsARMV7MRVM
 *
 * @brief RVM guest reset support.
 */

#include <bsp.h>
#include <bsp/bootcard.h>

#include "rvm.h"

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
