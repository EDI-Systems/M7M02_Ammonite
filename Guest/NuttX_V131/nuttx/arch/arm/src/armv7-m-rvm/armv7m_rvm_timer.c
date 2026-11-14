/****************************************************************************
 * arch/arm/src/armv7-m-rvm/armv7m_rvm_timer.c
 *
 * SPDX-License-Identifier: Apache-2.0
 ****************************************************************************/

#include <nuttx/config.h>

#include <nuttx/arch.h>

#include "armv7m_rvm_internal.h"

void up_timer_initialize(void)
{
  ntx_vector_init();
}
