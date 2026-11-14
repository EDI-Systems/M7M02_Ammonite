/****************************************************************************
 * arch/arm/src/armv7-m-rvm/armv7m_rvm_fpu.c
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * The RVM monitor owns CPACR, FPCCR and lazy stacking.  The guest must not
 * program them.  arm_fpuconfig() is therefore intentionally empty.
 ****************************************************************************/

#include <nuttx/config.h>

#include <stdbool.h>
#include <string.h>

#include <nuttx/irq.h>

#include "arm_internal.h"

void arm_fpuconfig(void)
{
}

#ifdef CONFIG_ARCH_FPU
bool up_fpucmp(const void *saveregs1, const void *saveregs2)
{
  const uint32_t *regs1 = saveregs1;
  const uint32_t *regs2 = saveregs2;

  return memcmp(&regs1[REG_S16], &regs2[REG_S16],
                4 * SW_FPU_REGS) == 0;
}
#endif
