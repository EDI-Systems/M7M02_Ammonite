/****************************************************************************
 * arch/arm/src/armv7-m-rvm/armv7m_rvm_irq.c
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * The guest has no physical NVIC.  IRQ attach/dispatch remains the generic
 * NuttX implementation; only the architecture-level control hooks are
 * stubbed here.
 ****************************************************************************/

#include <nuttx/config.h>

#include <stdint.h>

#include <nuttx/arch.h>
#include <nuttx/irq.h>

#include "armv7m_rvm_internal.h"

void up_irqinitialize(void)
{
}

void up_enable_irq(int irq)
{
  (void)irq;
}

void up_disable_irq(int irq)
{
  (void)irq;
}

void up_ack_irq(int irq)
{
  (void)irq;
}

void up_prioritize_irq(int irq, int priority)
{
  (void)irq;
  (void)priority;
}

void up_trigger_irq(int irq, cpu_set_t cpuset)
{
  (void)irq;
  (void)cpuset;
}
