/* Aquarium heartbeat for uC/OS-II on RVM: one task prints one line every
 * 1 s through OSTimeDly (100 ticks at the 100 Hz guest tick), and the
 * application idle hook parks the machine in the hypervisor. */
#include <stddef.h>
#include <ucos_ii.h>
#include "rvm.h"

static OS_STK Beat_Stk[128];

static void Beat_Task(void *p_arg)
{
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
        RVM_Hyp_Putchar('2');
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
        OSTimeDly(100u);
        n++;
    }
}

/* The idle parking hook - called by OSTaskIdleHook() whenever the kernel
 * has nothing to run.  RVM_Hyp_Vct_Wait() blocks the user thread until the
 * next virtual interrupt is delivered and resumes it right here. */
void App_TaskIdleHook(void)
{
    RVM_Hyp_Vct_Wait();
}

/* Empty application hooks - uC/OS-II calls these when OS_APP_HOOKS_EN is
 * set; only the idle hook carries the parking policy. */
void App_TaskCreateHook(OS_TCB *ptcb) { (void)ptcb; }
void App_TaskDelHook(OS_TCB *ptcb) { (void)ptcb; }
void App_TaskReturnHook(OS_TCB *ptcb) { (void)ptcb; }
void App_TaskStatHook(void) { }
void App_TaskSwHook(void) { }
void App_TCBInitHook(OS_TCB *ptcb) { (void)ptcb; }

int main(void)
{
    OSInit();
    OSTaskCreate(Beat_Task, (void *)0, &Beat_Stk[127], 5u);
    OSStart();
    return 0;
}
