/****************************************************************************
 * arch/arm/include/armv7-m-rvm/irq.h
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.  The
 * ASF licenses this file to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance with the
 * License.  You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  See the
 * License for the specific language governing permissions and limitations
 * under the License.
 *
 ****************************************************************************/

#ifndef __ARCH_ARM_INCLUDE_ARMV7_M_RVM_IRQ_H
#define __ARCH_ARM_INCLUDE_ARMV7_M_RVM_IRQ_H

/* The RVM monitor owns the physical vector table.  Virtual vectors are
 * selected by the RVM project; this is only the generic NuttX IRQ table
 * size used by the scheduler.
 */

#define NR_IRQS                       64

/* The register frame is the ordinary NuttX ARMv7-M frame.  The RVM monitor
 * frame is translated to/from this frame only at the vector boundary.
 */

#include <arch/armv7-m/irq.h>

#ifndef __ASSEMBLY__

struct tcb_s;

/* Interrupt state is virtual.  The RVM monitor owns BASEPRI and PRIMASK. */

irqstate_t ntx_irq_save(void);
void ntx_irq_restore(irqstate_t flags);
void ntx_irq_disable(void);
void ntx_irq_enable(void);
bool ntx_in_interrupt(void);
void ntx_switch_context(struct tcb_s *tcb, struct tcb_s *rtcb);
void ntx_fullcontextrestore(void);

#define up_irq_save()                 ntx_irq_save()
#define up_irq_restore(flags)         ntx_irq_restore(flags)
#define up_irq_disable()              ntx_irq_disable()
#define up_irq_enable()               ntx_irq_enable()
#define up_interrupt_context()        ntx_in_interrupt()
#define up_switch_context(tcb, rtcb)  ntx_switch_context(tcb, rtcb)
#define arm_fullcontextrestore()      ntx_fullcontextrestore()

#endif /* __ASSEMBLY__ */

#endif /* __ARCH_ARM_INCLUDE_ARMV7_M_RVM_IRQ_H */
