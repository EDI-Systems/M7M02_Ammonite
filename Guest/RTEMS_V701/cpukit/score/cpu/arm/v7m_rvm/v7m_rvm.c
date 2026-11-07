/* SPDX-License-Identifier: Unlicense */

/**
 * @file
 *
 * @ingroup RTEMSScoreCPUARM
 *
 * @brief RVM guest interrupt masking.
 *
 * The RVM guest is an unprivileged thread and cannot program BASEPRI.  The
 * virtual interrupt mask provided by the RVM paravirtualization library is
 * used instead.  The saved cookie is the nesting level before the disable.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/score/armv7m.h>
#include <rtems/score/cpu.h>

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

#endif /* ARM_MULTILIB_ARCH_V7M */
