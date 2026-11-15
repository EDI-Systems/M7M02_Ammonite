/* Aquarium heartbeat for uC/OS-III on RVM: one task prints one line every
 * 1 s through OSTimeDly (100 ticks at the 100 Hz guest tick), and the
 * application idle hook parks the machine in the hypervisor. */
#include <stddef.h>
#include <os.h>
#include "rvm.h"

static OS_TCB Beat_TCB;
static CPU_STK Beat_Stk[128];

static void Beat_Task(void *p_arg)
{
    OS_ERR os_err;
    int n = 0;
    (void)p_arg;
    for(;;)
    {
        char num[12];
        int j = 0;
        int v = n;
        RVM_Hyp_Putchar('\r');
        RVM_Hyp_Putchar('\n');
        RVM_Hyp_Putchar('[');
        RVM_Hyp_Putchar('U');
        RVM_Hyp_Putchar('O');
        RVM_Hyp_Putchar('3');
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
        OSTimeDly((OS_TICK)100, OS_OPT_TIME_DLY, &os_err);
        n++;
    }
}

/* The idle parking hook - called by OSIdleTaskHook() from the idle task.
 * RVM_Hyp_Vct_Wait() blocks the user thread until the next virtual
 * interrupt is delivered and resumes it right here; it is transparent to
 * the kernel and never blocks in the uC/OS-III sense. */
static void App_IdleHook(void)
{
    RVM_Hyp_Vct_Wait();
}

int main(void)
{
    OS_ERR os_err;

    OSInit(&os_err);

    /* The idle policy belongs to the application: the port only forwards
     * OSIdleTaskHook() to this pointer. */
    OS_AppIdleTaskHookPtr = App_IdleHook;

    OSTaskCreate(&Beat_TCB, "beat", Beat_Task, (void *)0, 5u,
                 &Beat_Stk[0], 64u, 128u,
                 (OS_MSG_QTY)0u, (OS_TICK)0u, (void *)0u, OS_OPT_TASK_NONE, &os_err);

    OSStart(&os_err);
    return 0;
}
