/* SPDX-License-Identifier: Unlicense */

/**
 * @file
 *
 * @ingroup RTEMSScoreCPUARM
 *
 * @brief RVM ARMv7-M CPU port header overlay.
 *
 * The RVM guest is unprivileged and cannot use BASEPRI.  Include the common
 * ARM CPU header first, then redirect the interrupt-mask primitives to the
 * RVM virtual interrupt mask implementation.
 */

#ifndef _RTEMS_SCORE_CPU_RVM_H
#define _RTEMS_SCORE_CPU_RVM_H

#include_next <rtems/score/cpu.h>

#if !defined(__ASSEMBLER__) && defined(RTEMS_RVM)

#undef _CPU_ISR_Disable
#undef _CPU_ISR_Enable
#undef _CPU_ISR_Flash

uint32_t _RVM_CPU_ISR_Disable( void );

void _RVM_CPU_ISR_Enable( uint32_t level );

void _RVM_CPU_ISR_Flash( uint32_t level );

uint32_t _RVM_CPU_ISR_Get_level( void );

void _RVM_CPU_ISR_Set_level( uint32_t level );

#define _CPU_ISR_Disable( _isr_cookie ) \
  do { \
    (_isr_cookie) = _RVM_CPU_ISR_Disable(); \
  } while ( 0 )

#define _CPU_ISR_Enable( _isr_cookie ) \
  _RVM_CPU_ISR_Enable( _isr_cookie )

#define _CPU_ISR_Flash( _isr_cookie ) \
  _RVM_CPU_ISR_Flash( _isr_cookie )

#endif /* RTEMS_RVM */

#endif /* _RTEMS_SCORE_CPU_RVM_H */
