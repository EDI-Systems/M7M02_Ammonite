/* SPDX-License-Identifier: Unlicense */

/**
 * @file
 *
 * @ingroup RTEMSBSPsARMV7MRVM
 *
 * @brief RVM virtual timer clock driver for the RTE guest.
 *
 * RVM_Virt_Init() runs in the higher priority vector thread before the user
 * thread starts.  Therefore the user thread registers the virtual timer
 * handler first (install the vector); rte_clock_initialize() only marks the
 * tick ready.  RVM interrupts stay masked until the first context restore
 * enables them in rte_rvm_start_heir().
 */

#include <bsp.h>

#include <rtems.h>
#include <rtems/score/armv7m.h>

#include <stdbool.h>

#include "rvm.h"

#include "rte_rvm_context.h"

static volatile bool rte_clock_ready;

static void Clock_isr( void *arg );

void rte_rvm_tick_handler( void );

void rte_rvm_register_vectors( void );

void rte_rvm_tick_handler( void )
{
  if ( rte_clock_ready ) {
    _ARMV7M_Interrupt_service_enter();
    Clock_isr( NULL );
    _ARMV7M_Interrupt_service_leave();
  }
}

/*
 * Called by the user thread after the vector thread completed RVM_Virt_Init().
 * Installing the handler here guarantees it is not wiped by RVM_Virt_Init().
 */
void rte_rvm_register_vectors( void )
{
  RVM_Virt_Tim_Reg( rte_rvm_tick_handler );
  RVM_Virt_Ctx_Reg( rte_rvm_ctx_handler );
}

static void rte_clock_initialize( void )
{
  rte_clock_ready = true;
}

/*
 * There is no hardware timecounter.  Use the dummy mode so that every virtual
 * timer delivery advances the RTEMS clock tick.
 */
#define CLOCK_DRIVER_USE_DUMMY_TIMECOUNTER

#define Clock_driver_support_initialize_hardware() \
  rte_clock_initialize()

/* Include shared source clock driver code */
#include "../../../shared/dev/clock/clockimpl.h"
