/******************************************************************************
Filename    : rmp_hook_rvm.c
Description : Aquarium heartbeat hook for RMP under RVM.  One thread prints
              one line every 1 s through RMP_Thd_Delay (100 ticks at the
              100 Hz guest tick), and the idle loop parks the machine in
              the hypervisor through RVM_Hyp_Vct_Wait.
******************************************************************************/
#include "rvm.h"
#include "rmp.h"

static void Aq_Beat(void* Param)
{
    rvm_ptr_t n = 0;
    rvm_ptr_t j;
    rvm_ptr_t v;
    char num[12];
    (void)Param;
    while(1)
    {
        j = 0;
        v = n;
        RVM_Hyp_Putchar(13);
        RVM_Hyp_Putchar(10);
        RVM_Hyp_Putchar('[');
        RVM_Hyp_Putchar('R');
        RVM_Hyp_Putchar('M');
        RVM_Hyp_Putchar('P');
        RVM_Hyp_Putchar(']');
        RVM_Hyp_Putchar(' ');
        RVM_Hyp_Putchar('a');
        RVM_Hyp_Putchar('l');
        RVM_Hyp_Putchar('i');
        RVM_Hyp_Putchar('v');
        RVM_Hyp_Putchar('e');
        RVM_Hyp_Putchar(' ');
        if(v==0)
            num[j++]='0';
        while(v>0)
        {
            num[j++]=(char)('0'+v%10);
            v=v/10;
        }
        while(j>0)
            RVM_Hyp_Putchar(num[--j]);
        RMP_Thd_Delay(100);
        n++;
    }
}

void RMP_Init_Hook(void)
{
    static rmp_ptr_t Aq_Stk[256];
    static struct RMP_Thd Aq_Thd;
    RMP_Thd_Crt(&Aq_Thd, Aq_Beat, (void*)0,
                Aq_Stk, (rvm_ptr_t)(sizeof(Aq_Stk)/sizeof(rmp_ptr_t)),
                20, 10);
}

/* The idle parking hook - called by the kernel's idle loop with the
 * scheduler unlocked and the interrupts unmasked.  RVM_Hyp_Vct_Wait()
 * blocks the user thread until the next virtual interrupt is delivered
 * and resumes it right here. */
void RMP_Init_Idle(void)
{
    RVM_Hyp_Vct_Wait();
}
