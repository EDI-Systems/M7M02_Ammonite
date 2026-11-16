/*
 * RT-Thread V4.1.1 normal-mode sample on the RVM virtual machines.
 * Copyright (C) 2026 Evo-Devo Instrum. All rights reserved.
 *
 * This file is NOT part of the RT-Thread distribution.  It belongs to the
 * RT-Thread-on-RVM test suite and is released under the Unlicense; refer
 * to the LICENSE file for details.  The RT-Thread kernel itself is
 * copyright RT-Thread Development Team and is not modified by this file.
 *
 * The RT-Thread boot protocol on RVM gives main() two identities: on the
 * first call it runs in the raw virtual machine context (no current
 * thread), where it must hook the idle loop up to the hypervisor parking
 * call and start the kernel; rtthread_startup() then re-enters main()
 * as the "main" thread.  The main thread does its business and retires
 * with a permanent self-suspension - returning from it is not supported
 * by the port.
 */
#include <rtthread.h>
#include "rvm.h"

/* Park the virtual machine in the idle loop - the official idle hook
 * mechanism of the kernel, no port code involved. */
static void App_IdleHook(void)
{
    RVM_Hyp_Vct_Wait();
}

/* The kernel entry point - declared the way components.c does it. */
int rtthread_startup(void);

int main(void)
{
    if(rt_thread_self() == RT_NULL)
    {
        rt_thread_idle_sethook(App_IdleHook);
        rtthread_startup();
    }

    RVM_Hyp_Putchar('S');
    RVM_Hyp_Putchar('\r');
    RVM_Hyp_Putchar('\n');

    /* Sleep for two kernel ticks; the nominal tick rate is declared in
     * rtconfig.h - the actual delivery rate is set by the hypervisor. */
    rt_thread_mdelay(2);

    RVM_Hyp_Putchar('D');
    RVM_Hyp_Putchar('\r');
    RVM_Hyp_Putchar('\n');

    /* Retire the main thread for good. */
    rt_thread_suspend(rt_thread_self());
    rt_schedule();

    return 0;
}
