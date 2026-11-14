/****************************************************************************
 * arch/arm/src/armv7-m-rvm/armv7m_rvm_idle.c
 *
 * SPDX-License-Identifier: Apache-2.0
 ****************************************************************************/

#include <nuttx/config.h>

#include <nuttx/arch.h>

#include "arm_internal.h"
#include "armv7m_rvm_internal.h"

#include "rvm.h"

void up_idle(void)
{
  /* Do not park while the guest has virtual interrupts masked: the
   * monitor would have no way to wake this thread through the parked
   * vector path.  RVM_Hyp_Vct_Wait() is the RVM idle primitive; WFI is
   * deliberately not used because the guest does not own the timer.
   */

  if (g_ntx_irqnest == 0 && RVM_STATE->Vct_Act == 0)
    {
      (void)RVM_Hyp_Vct_Wait();
    }
}
