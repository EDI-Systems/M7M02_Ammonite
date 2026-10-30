/*
 * RT-Thread V4.1.1 main file template, running as a guest of the RVM
 * hypervisor.
 * Copyright (C) 2026 Evo-Devo Instrum. All rights reserved.
 *
 * This file is NOT part of the RT-Thread distribution.  It belongs to the
 * RT-Thread-on-RVM test suite and is released under the Unlicense; refer
 * to the LICENSE file for details.  The RT-Thread kernel itself is
 * copyright RT-Thread Development Team and is not modified by this file.
 *
 * File      : rtt_main.c
 * Version   : V1.00.00
 *
 * Note(s)  : (1) This file is copied into a generated virtual machine as
 *                its "main.c" when the project is generated without the
 *                benchmark mode.  The first entry boots the kernel; the
 *                second entry runs on the main thread, and starts a single
 *                application thread that does nothing but delay.
 */

#include <rtthread.h>
#include "rvm.h"

/*-----------------------------------------------------------*/

/* The stack and the control block of the application thread. */
static struct rt_thread AppThread;
ALIGN(8)
static rt_uint8_t AppThreadStk[512];

/*-----------------------------------------------------------*/

/*
 * APPLICATION THREAD
 *
 * The application itself - replace the body with your own code.
 */
static void App_Thread(void *p_arg)
{
    (void)p_arg;

    for (;;)
    {
        rt_thread_mdelay(1000);
    }
}

/*
 * IDLE HOOK
 *
 * The RVM user manual requires the guest's idle thread to park in the
 * hypervisor while there is nothing to run, so that the core is not spun
 * away.  RVM_Hyp_Vct_Wait() blocks the user thread until the next virtual
 * interrupt is delivered and resumes it right here; it is transparent to
 * the kernel and does not suspend the thread in the RT-Thread sense.
 */
#ifdef RT_USING_IDLE_HOOK
static void App_IdleHook(void)
{
    RVM_Hyp_Vct_Wait();
}
#endif

/*
 * MAIN
 *
 * Boot the kernel on the first entry; run the application setup on the
 * main thread.
 */
int main(void)
{
    rt_err_t ret;

    /* The virtual machine enters through this function before the
     * scheduler runs; the second entry happens on the main thread, after
     * rtthread_startup() has switched to it. */
    if (rt_thread_self() == RT_NULL)
    {
        /* The idle policy belongs to the application: register the hook
         * before the kernel starts, the first entry never returns. */
#ifdef RT_USING_IDLE_HOOK
        rt_thread_idle_sethook(App_IdleHook);
#endif
        rtthread_startup();             /* never returns */
    }

    /* Initialize and start the application thread. */
    ret = rt_thread_init(&AppThread, "app", App_Thread, RT_NULL,
                         AppThreadStk, sizeof(AppThreadStk),
                         RT_THREAD_PRIORITY_MAX - 1, 20);
    RT_ASSERT(ret == RT_EOK);
    rt_thread_startup(&AppThread);

    /* The main thread exits here. */
    return 0;
}
