/* Aquarium heartbeat for FreeRTOS on RVM: one task prints one line every
 * 1 s through vTaskDelay (100 ticks at the 100 Hz guest tick), and the
 * idle hook parks the machine in the hypervisor - see RVM_Hyp_Vct_Wait. */
#include "FreeRTOS.h"
#include "task.h"
#include "rvm.h"

static void Beat(void *arg)
{
    int n = 0;
    (void)arg;
    for(;;)
    {
        char num[12];
        int j = 0;
        int v = n;
        RVM_Hyp_Putchar('\r');
        RVM_Hyp_Putchar('\n');
        RVM_Hyp_Putchar('[');
        RVM_Hyp_Putchar('F');
        RVM_Hyp_Putchar('R');
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
        vTaskDelay((TickType_t)100);
        n++;
    }
}

/* The idle parking hook - RVM_Hyp_Vct_Wait() blocks the user thread until
 * the next virtual interrupt is delivered and resumes it right here; it is
 * transparent to the kernel and does not block in the FreeRTOS sense. */
void vApplicationIdleHook(void)
{
    RVM_Hyp_Vct_Wait();
}

int main(void)
{
    xTaskCreate(Beat, "beat", 256, NULL, tskIDLE_PRIORITY+1, NULL);
    vTaskStartScheduler();
    for(;;)
    {
    }
}
