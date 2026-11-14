/****************************************************************************
 * arch/arm/src/armv7-m-rvm/armv7m_rvm_internal.h
 *
 * SPDX-License-Identifier: Apache-2.0
 ****************************************************************************/

#ifndef __ARCH_ARM_SRC_ARMV7_M_RVM_INTERNAL_H
#define __ARCH_ARM_SRC_ARMV7_M_RVM_INTERNAL_H

#include <nuttx/config.h>

#include <stdbool.h>
#include <stdint.h>

#include <nuttx/arch.h>
#include <nuttx/irq.h>

struct tcb_s;
struct xcptcontext;

/* Software virtual-interrupt nesting.  Zero means the RVM mask is open. */

extern volatile uint32_t g_ntx_irqnest;

/* Masquerade the current task context and request a context switch.  The
 * routine returns when the monitor resumes this thread at ntx_ctx_exit.
 */

void ntx_context_switch(struct xcptcontext *xcp, const void *params);

#ifdef CONFIG_ARMV7M_RVM_FAST_PATH
void ntx_fast_switch(struct xcptcontext *from, struct xcptcontext *to,
                     const void *params);
#endif

/* Vector-thread side. */

void ntx_vector_init(void);
void ntx_timer_handler(void);
void ntx_ctx_handler(void);
void ntx_irq_entry(int irq);

/* Board/application glue. */

int ntx_main(int argc, char *argv[]);

#endif /* __ARCH_ARM_SRC_ARMV7_M_RVM_INTERNAL_H */
