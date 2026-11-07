/* SPDX-License-Identifier: Unlicense */

/**
 * @file
 *
 * @ingroup RTEMSScoreCPUARM
 *
 * @brief RVM guest CPU initialization.
 *
 * The RVM guest is unprivileged and does not own the physical exception
 * priorities: SVC and PendSV belong to the monitor.  Programming the NVIC
 * system registers here would fault, so the standard _CPU_Initialize()
 * implementation is replaced by this empty one for the RVM port.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/score/armv7m.h>

#ifdef ARM_MULTILIB_ARCH_V7M

void _CPU_Initialize( void )
{
  /* Nothing to program: the monitor owns SVC, PendSV and the NVIC. */
}

#endif /* ARM_MULTILIB_ARCH_V7M */
