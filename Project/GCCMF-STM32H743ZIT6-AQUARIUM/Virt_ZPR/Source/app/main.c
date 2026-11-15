/* Aquarium heartbeat for Zephyr on RVM: one line every 1 s through
 * k_sleep, while the idle loop parks in the monitor via zpr_cpu_idle(). */
#include <zephyr/kernel.h>

int main(void)
{
    int n = 0;

    for(;;)
    {
        printk("[ZPR] alive %d\n", n);
        n++;
        k_sleep(K_MSEC(1000));
    }
}
