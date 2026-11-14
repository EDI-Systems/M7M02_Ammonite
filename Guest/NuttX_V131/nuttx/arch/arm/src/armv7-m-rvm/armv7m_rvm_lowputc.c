/****************************************************************************
 * arch/arm/src/armv7-m-rvm/armv7m_rvm_lowputc.c
 *
 * SPDX-License-Identifier: Apache-2.0
 ****************************************************************************/

#include <nuttx/config.h>

#include <nuttx/arch.h>

#include "rvm.h"

void up_putc(int ch)
{
  RVM_Hyp_Putchar((char)ch);
}
