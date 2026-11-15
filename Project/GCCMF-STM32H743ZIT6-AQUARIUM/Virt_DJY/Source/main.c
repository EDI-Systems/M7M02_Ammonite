/*
 * Aquarium heartbeat for DjyOS on RVM.  One event script prints one line
 * every 1 s through DJY_EventDelay (100 ticks at the 100 Hz guest tick),
 * and the idle hook parks the machine in the hypervisor between the
 * passes.  The script is launched by the idle event's first service pass -
 * popping it in the startup context, before the kernel is started, is the
 * corruption the port trips on.
 */
#include "rvm.h"
#include "djyos.h"
#include "systime.h"

extern void djy_port_boot(void);
extern void djy_port_start(void);
extern void djy_set_idle_hook(void (*hook)(void));
extern void djy_dbg_puts(const char *str);
extern void djy_dbg_putu32(u32 val);

/* The stack pool entry for the heartbeat event type. */
static u8 Beat_Stk[2048] __attribute__((aligned(8)));
static u16 Beat_Evtt_Id;
static volatile u32 Beat_Pending = 1U;

static ptu32_t Beat_Script(void)
{
    u32 n = 0U;
    for(;;)
    {
        djy_dbg_puts("\r\n[DJY] alive ");
        djy_dbg_putu32(n);
        DJY_EventDelay(1000U*(u32)mS);
        n++;
    }
    return 0;
}

/*
 * The idle event hook: the first pass launches the heartbeat script - the
 * DjyOS-idiomatic way to start a runnable event, the pop is issued by a
 * real event thread whose saved context lives on its own stack - and every
 * later pass parks in the hypervisor.  The hook is invoked from
 * CleanWakeupEvent, with the virtual interrupt mask open - see the note
 * in djy_support.c.
 */
static void Aqua_Idle(void)
{
    if(Beat_Pending!=0U)
    {
        Beat_Pending=0U;
        if(DJY_EventPop(Beat_Evtt_Id, NULL, 0U, 0U, 0U, 0U)
                ==CN_EVENT_ID_INVALID)
            Beat_Pending=1U;        /* no free event block - retry */
        return;
    }
    RVM_Hyp_Vct_Wait();
}

int main(void)
{
    /* The idle policy belongs to the application; the port only calls
     * the hook back from its idle event. */
    djy_set_idle_hook(Aqua_Idle);

    djy_port_boot();

    Beat_Evtt_Id = DJY_EvttRegist(EN_CORRELATIVE, 30, 1, 1,
                                  Beat_Script, Beat_Stk, sizeof(Beat_Stk),
                                  "beat");

    djy_port_start();               /* never returns */
    return 0;
}
