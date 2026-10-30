/*
 * DjyOS V1.1.0 RVM port - Copyright (C) 2026 Evo-Devo Instrum.
 * All rights reserved
 *
 * This file is NOT part of the DjyOS distribution.  It belongs to the
 * DjyOS-on-RVM test suite and is released under the Unlicense; refer
 * to the LICENSE file for details.  The DjyOS kernel itself is
 * copyright DjyOS Open Source Development team and is not modified by
 * this file.
 *
 * The plain application of the port - the generator copies this file
 * into the workspace as main.c when the project is generated without
 * the benchmark mode.  It boots the kernel through the port's board
 * glue and parks the idle event in the hypervisor; no benchmark timer,
 * no interrupt source and no test header is involved.
 */

#include "rvm.h"

extern void djy_port_boot(void);
extern void djy_port_start(void);
extern void djy_set_idle_hook(void (*hook)(void));

/*
 * IDLE HOOK
 *
 * The RVM user manual requires the guest's idle thread to park in the
 * hypervisor while there is nothing to run, so that the core is not
 * spun away.  RVM_Hyp_Vct_Wait() blocks the user thread until the next
 * virtual interrupt is delivered and resumes it right here; the kernel
 * only sees an idle pass that took a little longer.  The idle event
 * invokes the hook from CleanWakeupEvent, with the virtual interrupt
 * mask open - see the note in djy_support.c.
 */
static void djy_idle_wait(void)
{
    RVM_Hyp_Vct_Wait();
}

int main(void)
{
    /* The idle policy belongs to the application; the port only calls
     * the hook back from its idle event. */
    djy_set_idle_hook(djy_idle_wait);

    djy_port_boot();
    djy_port_start();

    return 0;
}
