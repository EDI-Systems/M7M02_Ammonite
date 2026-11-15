/* Aquarium heartbeat for ChibiOS on RVM: the main thread prints one line
 * every 1 s through chThdSleepMilliseconds (100 ticks at the 100 Hz
 * guest tick).  The idle thread parks the machine in the hypervisor by
 * itself - the RVM port's port_wait_for_interrupt() is the parking call -
 * so no application hook is needed. */
#include "ch.h"
#include "rvm.h"

int main(void)
{
    int n = 0;

    chSysInit();

    for(;;)
    {
        char num[12];
        int j = 0;
        int v = n;
        RVM_Hyp_Putchar('\r');
        RVM_Hyp_Putchar('\n');
        RVM_Hyp_Putchar('[');
        RVM_Hyp_Putchar('C');
        RVM_Hyp_Putchar('H');
        RVM_Hyp_Putchar('B');
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
        chThdSleepMilliseconds(1000);
        n++;
    }
}
