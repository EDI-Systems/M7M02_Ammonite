/* SPDX-License-Identifier: Unlicense AND BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSScoreCPUARM
 *
 * @brief RVM guest CPU support: initialization, interrupt masking and ISR
 *   dispatch.
 *
 * The RVM guest is an unprivileged thread and cannot program BASEPRI.  The
 * virtual interrupt mask provided by the RVM paravirtualization library is
 * used instead.  The saved cookie is the nesting level before the disable.
 * The interrupt service enter/leave functions are the RTEMS deferred-dispatch
 * path and are derived from the upstream ARMv7-M implementation.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/score/armv7m.h>
#include <rtems/score/cpu.h>
#include <rtems/score/isr.h>
#include <rtems/score/threaddispatch.h>

#ifdef ARM_MULTILIB_ARCH_V7M

void RVM_Virt_Int_Mask( void );

void RVM_Virt_Int_Unmask( void );

static uint32_t rvm_isr_nest_level;

uint32_t _RVM_CPU_ISR_Disable( void )
{
  uint32_t level = rvm_isr_nest_level;

  if ( level == 0 ) {
    RVM_Virt_Int_Mask();
  }

  rvm_isr_nest_level = level + 1;

  return level;
}

void _RVM_CPU_ISR_Enable( uint32_t level )
{
  rvm_isr_nest_level = level;

  if ( level == 0 ) {
    RVM_Virt_Int_Unmask();
  }
}

void _RVM_CPU_ISR_Flash( uint32_t level )
{
  _RVM_CPU_ISR_Enable( level );
  (void) _RVM_CPU_ISR_Disable();
}

uint32_t _RVM_CPU_ISR_Get_level( void )
{
  return rvm_isr_nest_level;
}

void _RVM_CPU_ISR_Set_level( uint32_t level )
{
  if ( level != 0 ) {
    rvm_isr_nest_level = 1;
    RVM_Virt_Int_Mask();
  } else {
    rvm_isr_nest_level = 0;
    RVM_Virt_Int_Unmask();
  }
}

/*
 * The RVM guest does not own SVC, PendSV or the NVIC, so the native ARMv7-M
 * _CPU_Initialize() must not program exception priorities here.
 */
void _CPU_Initialize( void )
{
}

uint32_t _CPU_ISR_Get_level( void )
{
  return _RVM_CPU_ISR_Get_level();
}

void _CPU_ISR_Set_level( uint32_t level )
{
  _RVM_CPU_ISR_Set_level( level );
}

/*
 * The following two functions are derived from
 * cpukit/score/cpu/arm/armv7m-isr-enter-leave.c:
 *
 * Copyright (c) 2011, 2017 Sebastian Huber.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

extern void rte_rvm_dispatch_complete( void );

void _ARMV7M_Interrupt_service_enter( void )
{
  Per_CPU_Control *cpu_self = _Per_CPU_Get();

  ++cpu_self->thread_dispatch_disable_level;
  ++cpu_self->isr_nest_level;
}

void _ARMV7M_Interrupt_service_leave( void )
{
  Per_CPU_Control *cpu_self = _Per_CPU_Get();

  --cpu_self->thread_dispatch_disable_level;
  --cpu_self->isr_nest_level;

  /*
   * The RVM guest cannot write SCB->ICSR.  The RTEMS dispatcher runs on the
   * vector thread when the nested interrupt levels reach zero; the physical
   * switch is deferred to the RVM context handler.
   */
  if (
    ( cpu_self->isr_nest_level | cpu_self->thread_dispatch_disable_level ) == 0
  ) {
    if ( cpu_self->dispatch_necessary ) {
      _Thread_Dispatch();
    }

    rte_rvm_dispatch_complete();
  }
}

#endif /* ARM_MULTILIB_ARCH_V7M */
