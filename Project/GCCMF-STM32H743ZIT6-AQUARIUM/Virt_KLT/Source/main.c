/* Aquarium heartbeat for KLite on RVM: one thread prints one line every
 * 1 s through thread_sleep (100 ticks at the 100 Hz guest tick), and the
 * idle hook registered with the port parks the machine in the hypervisor
 * from the kernel's idle thread. */
#include "kernel.h"
#include "rvm.h"

extern void klt_set_idle_hook(void (*hook)(void));

/* The kernel heap - carved out of the virtual machine's data section. */
static uint8_t m_heap[4096] __attribute__((aligned(8)));

/* The idle parking hook - RVM_Hyp_Vct_Wait() blocks the user thread until
 * the next virtual interrupt is delivered and resumes it right here.  The
 * port calls the hook from cpu_sys_sleep(), which the kernel's idle thread
 * enters with the virtual interrupt mask held - the port opens the mask
 * around the hook, see the note in cpu.c. */
static void Klt_Idle_Park(void)
{
    RVM_Hyp_Vct_Wait();
}

/* The idle thread entry - the official pattern: kernel_idle() never
 * returns, it cleans up dead threads and parks through cpu_sys_sleep(). */
static void Klt_Idle_Entry(void *arg)
{
    (void)arg;
    kernel_idle();
}

static void Beat_Fn(void *arg)
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
        RVM_Hyp_Putchar('K');
        RVM_Hyp_Putchar('L');
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
        thread_sleep(100);
        n++;
    }
}

int main(void)
{
    /* The idle policy belongs to the application; the port only calls
     * the hook back from its idle path. */
    klt_set_idle_hook(Klt_Idle_Park);

    kernel_init(m_heap, sizeof(m_heap));
    thread_create(Klt_Idle_Entry, NULL, 0);
    thread_create(Beat_Fn, NULL, 0);
    kernel_start();

    /* kernel_start() releases the boot critical section and switches into
     * the first thread; the startup context is abandoned inside that
     * switch.  Block in the hypervisor instead of spinning if control
     * ever falls through to here anyway. */
    for(;;)
    {
        RVM_Virt_Yield();
    }
}
