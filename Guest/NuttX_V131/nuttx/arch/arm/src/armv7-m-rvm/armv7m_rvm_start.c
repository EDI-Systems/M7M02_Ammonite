/****************************************************************************
 * arch/arm/src/armv7-m-rvm/armv7m_rvm_start.c
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * The RVM platform entry (__RVM_Entry) has already copied .data and cleared
 * .bss before reaching main().  NuttX therefore starts directly at nx_start().
 ****************************************************************************/

#include <nuttx/config.h>

#include <stdint.h>

#include <nuttx/arch.h>
#include <nuttx/init.h>

#include "arm_internal.h"

extern void __RVM_Entry(void);

/* The NuttX heap starts after a CONFIG_IDLETHREAD_STACKSIZE reservation
 * below the RVM register window.  The boot thread actually runs on the
 * monitor-provided stack region (STK in ntx.ld), which is also declared as
 * the IDLE task stack by up_initial_state(); the reservation below is only
 * the historic flat-build gap and keeps the heap start well defined.
 */

const uintptr_t g_idle_topstack =
  (uintptr_t)_ebss + CONFIG_IDLETHREAD_STACKSIZE;

/* NuttX passes --entry=__start to the linker.  The RVM image is actually
 * entered through __RVM_Entry (the descriptor's user entry), which performs
 * the data/bss load and then calls main().  This alias only satisfies the
 * conventional NuttX entry symbol.
 */

void __start(void)
{
  __RVM_Entry();
}

int main(void)
{
  nx_start();

  for (; ; )
    {
    }
}
