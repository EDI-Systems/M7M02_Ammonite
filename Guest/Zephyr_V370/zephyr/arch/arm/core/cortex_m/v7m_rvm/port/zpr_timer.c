/* SPDX-License-Identifier: Unlicense */

#include <zephyr/kernel.h>
#include <zephyr/drivers/timer/system_timer.h>

#include "rvm.h"

extern void zpr_reschedule_from_vector(void);

static void zpr_timer_isr(void)
{
	sys_clock_announce(1);
	zpr_reschedule_from_vector();
}

static int sys_clock_driver_init(void)
{
	/*
	 * Register the virtual timer, but keep the RVM interrupt gates closed
	 * until zpr_switch_to_main().  Native Zephyr keeps interrupts disabled
	 * through z_cstart() and only unmasks at the final switch to main.
	 */
	RVM_Virt_Tim_Reg(zpr_timer_isr);
	return 0;
}

SYS_INIT(sys_clock_driver_init, PRE_KERNEL_2,
	 CONFIG_SYSTEM_CLOCK_INIT_PRIORITY);

uint32_t sys_clock_elapsed(void)
{
	return 0U;
}

void sys_clock_set_timeout(int32_t ticks, bool idle)
{
	(void) ticks;
	(void) idle;
}

BUILD_ASSERT(
	(CONFIG_SYS_CLOCK_HW_CYCLES_PER_SEC % CONFIG_SYS_CLOCK_TICKS_PER_SEC) == 0,
	"cycle counter scale must be an integer");

/*
 * RVM does not grant the guest a free-running hardware cycle counter; the
 * virtual timer tick is the only timebase.  Synthesize a cycle count from
 * the tick count so its rate matches CONFIG_SYS_CLOCK_HW_CYCLES_PER_SEC.
 * The resolution is one tick, and the 32-bit result wraps at the same rate
 * as a hardware counter of that frequency.
 */
static uint64_t zpr_cycle_get_64(void)
{
	return (uint64_t) k_uptime_ticks() *
	       (CONFIG_SYS_CLOCK_HW_CYCLES_PER_SEC /
		CONFIG_SYS_CLOCK_TICKS_PER_SEC);
}

uint32_t sys_clock_cycle_get_32(void)
{
	return (uint32_t) zpr_cycle_get_64();
}

uint64_t sys_clock_cycle_get_64(void)
{
	return zpr_cycle_get_64();
}

