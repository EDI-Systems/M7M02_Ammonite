/* Aquarium heartbeat for RT-Thread on RVM: one thread prints one line
 * every 1 s through rt_thread_mdelay (100 ticks at the 100 Hz guest
 * tick), and the registered idle hook parks the machine in the hypervisor. */
#include <rtthread.h>
#include "rvm.h"

static struct rt_thread Beat_Thread;
static rt_uint8_t Beat_Stack[512];

static void Beat_Entry(void *param)
{
    int n = 0;
    (void)param;
    for(;;)
    {
        char num[12];
        int j = 0;
        int v = n;
        RVM_Hyp_Putchar('\r');
        RVM_Hyp_Putchar('\n');
        RVM_Hyp_Putchar('[');
        RVM_Hyp_Putchar('R');
        RVM_Hyp_Putchar('T');
        RVM_Hyp_Putchar('T');
        RVM_Hyp_Putchar(']');
        RVM_Hyp_Putchar(' ');
        RVM_Hyp_Putchar('a');
        RVM_Hyp_Putchar('l');
        RVM_Hyp_Putchar('i');
        RVM_Hyp_Putchar('v');
        RVM_Hyp_Putchar('e');
        RVM_Hyp_Putchar(' ');
        if(v == 0)
            num[j++] = '0';
        while(v > 0)
        {
            num[j++] = (char)('0'+v%10);
            v = v/10;
        }
        while(j > 0)
            RVM_Hyp_Putchar(num[--j]);
        rt_thread_mdelay(1000);
        n++;
    }
}

/* The idle parking hook - RVM_Hyp_Vct_Wait() blocks the user thread until
 * the next virtual interrupt is delivered and resumes it right here; it is
 * transparent to the kernel and does not suspend the thread in the
 * RT-Thread sense. */
static void App_IdleHook(void)
{
    RVM_Hyp_Vct_Wait();
}

int main(void)
{
    if(rt_thread_self() == RT_NULL)
    {
        /* Hypervisor entry - plays the role of the official entry():
         * register the idle hook before the kernel starts.  This first
         * entry never returns. */
        rt_thread_idle_sethook(App_IdleHook);
        rtthread_startup();
    }

    /* Main thread - the official user main body.  Initialize and start
     * the application thread. */
    rt_thread_init(&Beat_Thread, "beat", Beat_Entry, RT_NULL,
                   Beat_Stack, sizeof(Beat_Stack), 20, 20);
    rt_thread_startup(&Beat_Thread);

    /* Retire the main thread by self-suspension instead of returning:
     * the exit path of this port cannot complete under the hypervisor
     * (the dying context gets re-entered through stale kernel-side
     * state and corrupts a subsequent startup).  Suspended forever,
     * the scheduler runs the idle thread, which parks the machine;
     * the beat thread keeps it alive through its own delays. */
    rt_thread_suspend(rt_thread_self());
    rt_schedule();

    return 0; /* never reached */
}
