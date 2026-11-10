/* SPDX-License-Identifier: Unlicense */

#include "rvm.h"

extern void z_prep_c(void);

/*
 * RVM's descriptor Usr slot and the Zephyr ELF entry both point here.  The
 * __ prefix is deliberate: this is a port/linker-level entry, not a public
 * Zephyr API.
 */
void __zpr_entry(void)
{
	/*
	 * Keep RVM interrupts masked through z_prep_c()/z_cstart().  Native
	 * Zephyr runs the whole kernel initialization with interrupts disabled;
	 * zpr_switch_to_main() enables them at the final switch to main.
	 */
	z_prep_c();
}
