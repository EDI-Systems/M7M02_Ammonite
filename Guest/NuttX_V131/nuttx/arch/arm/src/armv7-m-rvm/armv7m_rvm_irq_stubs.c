/****************************************************************************
 * arch/arm/src/armv7-m-rvm/armv7m_rvm_irq_stubs.c
 *
 * SPDX-License-Identifier: Apache-2.0
 ****************************************************************************/

#include <nuttx/config.h>

#include <stdint.h>

#include <nuttx/arch.h>

#include "rvm.h"

void up_systemreset(void)
{
  RVM_Hyp_Reboot();

  for (; ; )
    {
    }
}
