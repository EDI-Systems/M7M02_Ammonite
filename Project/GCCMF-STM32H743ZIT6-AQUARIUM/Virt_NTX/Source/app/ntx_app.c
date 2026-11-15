/* Aquarium heartbeat for NuttX on RVM: print one line every 1 s through
 * usleep (the port synthesizes the tick from the RVM virtual timer), while
 * the idle task parks in the monitor through up_idle(). */
#include <nuttx/config.h>
#include <nuttx/arch.h>
#include <unistd.h>

int ntx_main(int argc, char* argv[])
{
    int n = 0;
    char num[12];
    int j;
    int v;

    (void)argc;
    (void)argv;

    for(;;)
    {
        up_putc('\r');
        up_putc('\n');
        up_putc('[');
        up_putc('N');
        up_putc('T');
        up_putc('X');
        up_putc(']');
        up_putc(' ');
        up_putc('a');
        up_putc('l');
        up_putc('i');
        up_putc('v');
        up_putc('e');
        up_putc(' ');
        j = 0;
        v = n;
        if(v == 0)
            num[j++] = '0';
        while(v > 0)
        {
            num[j++] = (char)('0' + v % 10);
            v = v / 10;
        }
        while(j > 0)
            up_putc(num[--j]);
        usleep(1000 * 1000);
        n++;
    }
}
