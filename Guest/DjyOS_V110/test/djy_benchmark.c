/*
 * DjyOS V1.1.0 RVM port - Copyright (C) 2026 Evo-Devo Instrum.
 * All rights reserved
 *
 * This file is NOT part of the DjyOS distribution.  It belongs to the
 * DjyOS-on-RVM test suite and is released under the Unlicense; refer
 * to the LICENSE file for details.  The DjyOS kernel itself is
 * copyright DjyOS Open Source Development team and is not modified by
 * this file.
 *
 * The DjyOS benchmark: nine standard items plus two DjyOS-signature
 * items, mapped onto the native event machinery of the system.
 *
 *  Yield            - event turnover ping-pong (CN_KEEP completion
 *                     switches between two same-priority event types)
 *  Notification     - thread level DJY_EventPop onto a CN_KEEP pooled
 *                     event type; the full keep-pool dispatch cycle
 *  Activation       - thread level DJY_EventPop onto an EN_INDEPENDENCE
 *                     type; the full create-run-destroy lifecycle
 *  Semaphore        - Lock_SempPost/Pend ping-pong between two looping
 *                     event scripts
 *  Message queue    - MsgQ_Send/Receive ping-pong, 4 byte messages
 *  Memory           - M_Malloc/M_Free pairs
 *  ISR Notification - timer async signal DJY_EventPop onto the CN_KEEP
 *                     type, 256B and 2048B stack tiers
 *  ISR Activation   - timer async signal DJY_EventPop onto the
 *                     EN_INDEPENDENCE type
 *  ISR Semaphore    - timer async signal Lock_SempPost
 *  ISR Message queue- timer async signal MsgQ_Send, 4 byte messages
 *  ISR RVM relay    - the kernel-side interrupt stamp readback
 *
 * All timing is in CPU cycles through the free-running TIM2 of the
 * chip header.  The measurement loops are silent; the reports print
 * after each phase completes.
 */

#include "rvm.h"
#include "djy_test.h"
#include "djyos.h"
#include "int.h"
#include "lock.h"
#include "msgqueue.h"
#include "heap.h"
#include "systime.h"

/* Port helpers. */
extern void djy_dbg_puts(const char *str);
extern void djy_dbg_putc(char c);
extern void djy_dbg_putu32(u32 val);
extern void djy_bench_int_init(void);
extern void djy_set_idle_hook(void (*hook)(void));

/* The statistics block. */
#define BENCH_TEST_ROUNDS                       10000UL
#define BENCH_DISCARD_ROUNDS                    2UL

struct djy_stat
{
    u32 max;
    u32 min;
    u64 sum;
    u32 cnt;
};

static void djy_stat_reset(struct djy_stat *s)
{
    s->max = 0U;
    s->min = 0xFFFFFFFFUL;
    s->sum = 0U;
    s->cnt = 0U;
}

static void djy_stat_record(struct djy_stat *s, u32 val)
{
    if(val > s->max)
        s->max = val;
    if(val < s->min)
        s->min = val;
    s->sum += val;
    s->cnt++;
}

static void djy_stat_report(const char *name, struct djy_stat *s)
{
    djy_dbg_puts(name);
    djy_dbg_puts(" : ");
    djy_dbg_putu32((u32)(s->sum / s->cnt));
    djy_dbg_puts(" / ");
    djy_dbg_putu32(s->max);
    djy_dbg_puts(" / ");
    djy_dbg_putu32(s->min);
    djy_dbg_puts("\r\n");
}

/*-----------------------------------------------------------*/

/* The ISR entry stamp - latched by the board ISR at the earliest point. */
volatile u32 djy_isr_stamp;

void djy_bench_isr_stamp(void)
{
    djy_isr_stamp = BENCH_TIM_READ();
}

/*-----------------------------------------------------------*/
/* Event type registration table.
 *
 * The benchmark registers all its event types in djy_bench_boot(),
 * before the multi event scheduling starts.  Every type is
 * correlative with a priority below 0x80, so its threads are kept in
 * the pool after completion (the CN_KEEP path) unless the type is
 * independent, in which case every pop creates a fresh thread that
 * is queued for idle freeing at completion (the CN_DELETE path). */

static u16 djy_yield_a_id;
static u16 djy_yield_b_id;
static u16 djy_notify_id;
static u16 djy_notify_lo_id;
static u16 djy_activate_id;
static u16 djy_sem_a_id;
static u16 djy_sem_b_id;
static u16 djy_mq_a_id;
static u16 djy_mq_b_id;

/* The driver event is popped from the idle event's service hook on its
 * first pass, never from the startup context.  DjyOS starts scheduling
 * the moment an event pop makes the ready queue differ from the running
 * event - a pop done in djy_main would therefore switch the startup
 * context out and have the kernel file that startup frame under the
 * running event's thread (the idle event's).  The idle thread is the
 * only genuine thread alive at StartOs; popping from its hook keeps the
 * "the suspended context belongs to the thread that is charged with it"
 * invariant of every switch. */
static u16 djy_driver_evtt_id;
static volatile u32 djy_driver_pending;

/* Stack pools for the static event type registrations. */
static u8 djy_yield_a_stk[DJY_EVTT_STK_SIZE] __attribute__((aligned(8)));
static u8 djy_yield_b_stk[DJY_EVTT_STK_SIZE] __attribute__((aligned(8)));
static u8 djy_notify_stk[DJY_EVTT_STK_SIZE] __attribute__((aligned(8)));
static u8 djy_notify_lo_stk[DJY_LOW_STK_SIZE] __attribute__((aligned(8)));
static u8 djy_sem_a_stk[DJY_EVTT_STK_SIZE] __attribute__((aligned(8)));
static u8 djy_sem_b_stk[DJY_EVTT_STK_SIZE] __attribute__((aligned(8)));
static u8 djy_mq_a_stk[DJY_EVTT_STK_SIZE] __attribute__((aligned(8)));
static u8 djy_mq_b_stk[DJY_EVTT_STK_SIZE] __attribute__((aligned(8)));

/* Round counters and statistics, shared between the driver and the
 * scripts (each script only touches its own). */
static volatile u32 djy_round;
static volatile u32 djy_t0;
static struct djy_stat djy_stat_one;

static struct SemaphoreLCB *djy_sem;
static struct SemaphoreLCB *djy_done_sem;
static struct SemaphoreLCB *djy_idle_sem;
static volatile u32 djy_idle_req;
static struct MsgQueue *djy_mq_a;
static struct MsgQueue *djy_mq_b;
static u32 djy_msg_buf[4];

/* The idle handshake: the driver asks for one idle pass and blocks;
 * the idle event pulses the semaphore from its CleanWakeupEvent hook
 * after it has freed the deferred threads.  This replaces any timer
 * based delay - the benchmark never needs the kernel tick. */
void djy_bench_idle_pulse(void)
{
    /* First idle pass: launch the driver.  This is the DjyOS-idiomatic
     * way to start a runnable event - the pop is issued by a real event
     * thread (the system service/idle thread) whose saved context lives
     * on its own stack, so the switch it triggers is coherent. */
    if(djy_driver_pending != 0U)
    {
        djy_driver_pending = 0U;
        if(DJY_EventPop(djy_driver_evtt_id, NULL, 0U, 0U, 0U, 0U)
                == CN_EVENT_ID_INVALID)
            djy_driver_pending = 1U;        /* no free event block - retry */
    }

    if(djy_idle_req != 0U)
    {
        djy_idle_req = 0U;
        Lock_SempPost(djy_idle_sem);
    }
}

static void djy_bench_idle_once(void)
{
    djy_idle_req = 1U;
    Lock_SempPend(djy_idle_sem, CN_TIMEOUT_FOREVER);
}



/* The deferred thread reclaim, exactly what the idle event's service
 * loop does with the kernel's wait-free list: take the vm off the list
 * and hand the block back to the heap.  Doing it here keeps the
 * activation item free of any dependency on the idle event or on the
 * kernel tick - what is measured is the create/delete cost itself. */
extern struct ThreadVm *g_ptWaitFreeVm;

static void djy_bench_reclaim(void)
{
    struct ThreadVm *vm;

    while((vm = g_ptWaitFreeVm) != NULL)
    {
        g_ptWaitFreeVm = vm->next;
        if(M_Free(vm, 0U) == false)
        {
            g_ptWaitFreeVm = vm;        /* heap busy - leave it for later */
            break;
        }
    }
}

/*-----------------------------------------------------------*/
/* Yield: two same-priority event types hand the CPU over by
 * popping each other and COMPLETING - DjyOS never preempts between
 * equal priorities, so the turnover runs through the completion path
 * (EventFinal's CN_KEEP reset switch), which is exactly the native
 * "yield" of an event driven kernel.  Each b dispatch measures one
 * full turnover: a's entry to b's entry. */

static struct djy_stat djy_yield_stat;

static ptu32_t djy_yield_a_script(void)
{
    djy_t0 = BENCH_TIM_READ();
    DJY_EventPop(djy_yield_b_id, NULL, 0U, 0U, 0U, 0U);
    return 0U;              /* completes: the switch happens in EventFinal */
}

static ptu32_t djy_yield_b_script(void)
{
    u32 t1 = BENCH_TIM_READ();
    if(djy_round >= BENCH_DISCARD_ROUNDS && djy_round < BENCH_TEST_ROUNDS + BENCH_DISCARD_ROUNDS)
        djy_stat_record(&djy_yield_stat, t1 - djy_t0);
    djy_round++;
    if(djy_round < BENCH_TEST_ROUNDS + BENCH_DISCARD_ROUNDS)
        DJY_EventPop(djy_yield_a_id, NULL, 0U, 0U, 0U, 0U);
    return 0U;              /* no pop at the end: a closes the chain */
}

static void djy_bench_yield(void)
{
    djy_dbg_puts("\r\nYield test (event turnover ping-pong).\r\n");
    djy_round = 0U;
    djy_stat_reset(&djy_yield_stat);
    DJY_EventPop(djy_yield_a_id, NULL, 0U, 0U, 0U, 0U);
    /* Preempted by the a event; when the round budget is spent, the
     * chain stops popping and the driver is the next ready event. */
    djy_stat_report("Yield", &djy_yield_stat);
}

/*-----------------------------------------------------------*/
/* Notification: the driver pops a pooled event; the event script
 * runs to completion (a whole CN_KEEP dispatch cycle including the
 * stack repaint), then the driver measures. */

static struct djy_stat djy_notify_stat;
static struct djy_stat djy_notify_lo_stat;
static struct djy_stat *djy_notify_target;

static ptu32_t djy_notify_script(void)
{
    u32 t1 = BENCH_TIM_READ();
    if(djy_round >= BENCH_DISCARD_ROUNDS && djy_round < BENCH_TEST_ROUNDS + BENCH_DISCARD_ROUNDS)
        djy_stat_record(djy_notify_target, t1 - djy_t0);
    return 0U;
}

static void djy_bench_notify_once(u16 id, struct djy_stat *s, const char *name)
{
    u32 round;

    djy_dbg_puts("\r\n");
    djy_dbg_puts(name);
    djy_dbg_puts(" test (pooled event dispatch cycle).\r\n");

    djy_stat_reset(s);
    djy_notify_target = s;
    for(round = 0U; round < BENCH_TEST_ROUNDS + BENCH_DISCARD_ROUNDS; round++)
    {
        djy_round = round;
        djy_t0 = BENCH_TIM_READ();
        /* The synchronous pop is what makes this a dispatch cycle: the
         * driver blocks here, the pooled event runs to completion, the
         * completion switch returns control - one sample per round. */
        DJY_EventPop(id, NULL, CN_TIMEOUT_FOREVER, 0U, 0U, 0U);
    }
    djy_stat_report(name, s);
}

/*-----------------------------------------------------------*/
/* Activation: the driver synchronously pops an EN_INDEPENDENCE type
 * registered at priority 200 (>= 0x80): the kernel therefore takes
 * the CN_DELETE completion path - every round is a fresh thread
 * (M_Malloc + paint + frame + engine entry) whose deferred free the
 * idle event performs.  The synchronous pop makes the driver sleep
 * through each dispatch, so the event still runs immediately. */

static ptu32_t djy_activate_script(void)
{
    u32 t1 = BENCH_TIM_READ();
    if(djy_round >= BENCH_DISCARD_ROUNDS && djy_round < BENCH_TEST_ROUNDS + BENCH_DISCARD_ROUNDS)
        djy_stat_record(&djy_stat_one, t1 - djy_t0);
    return 0U;
}

static void djy_bench_activate(void)
{
    u32 round;

    djy_dbg_puts("\r\nActivation test (independent event full cycle).\r\n");

    djy_stat_reset(&djy_stat_one);
    for(round = 0U; round < BENCH_TEST_ROUNDS + BENCH_DISCARD_ROUNDS; round++)
    {
        /* Recycle the thread the previous round completed BEFORE this
         * round allocates the next one.  The kernel's static heap only
         * rolls its bump pointer back when the freed block is the last
         * allocation, so a create/delete cycle has to run as a strict
         * stack: free the previous thread, then allocate the next. */
        djy_bench_reclaim();
        djy_round = round;
        djy_t0 = BENCH_TIM_READ();
        DJY_EventPop(djy_activate_id, NULL, CN_TIMEOUT_FOREVER, 0U, 0U, 0U);

        /* Occasionally let the idle event run its own free pass as well
         * - it drains whatever the reclaim above could not (a busy heap)
         * and keeps the idle handshake exercised. */
        if((round & 511U) == 511U)
            djy_bench_idle_once();
    }
    djy_stat_report("Activation", &djy_stat_one);
}

/*-----------------------------------------------------------*/
/* Semaphore: two long-lived events ping-pong a semaphore; the
 * finishing side releases the driver through the done semaphore
 * before parking. */

static struct djy_stat djy_sem_stat;

static ptu32_t djy_sem_a_script(void)
{
    while(1)
    {
        Lock_SempPend(djy_sem, CN_TIMEOUT_FOREVER);
        if(djy_round >= BENCH_TEST_ROUNDS + BENCH_DISCARD_ROUNDS)
        {
            Lock_SempPost(djy_done_sem);                /* release the driver */
            Lock_SempPend(djy_sem, CN_TIMEOUT_FOREVER); /* park */
        }
        djy_t0 = BENCH_TIM_READ();
        djy_round++;
        Lock_SempPost(djy_sem);
    }
    return 0U;
}

static ptu32_t djy_sem_b_script(void)
{
    while(1)
    {
        Lock_SempPend(djy_sem, CN_TIMEOUT_FOREVER);
        u32 t1 = BENCH_TIM_READ();
        if(djy_round >= BENCH_DISCARD_ROUNDS && djy_round < BENCH_TEST_ROUNDS + BENCH_DISCARD_ROUNDS)
            djy_stat_record(&djy_sem_stat, t1 - djy_t0);
        if(djy_round >= BENCH_TEST_ROUNDS + BENCH_DISCARD_ROUNDS)
        {
            Lock_SempPost(djy_done_sem);                /* release the driver */
            Lock_SempPend(djy_sem, CN_TIMEOUT_FOREVER); /* park */
        }
        Lock_SempPost(djy_sem);
    }
    return 0U;
}

static void djy_bench_sem(void)
{
    djy_dbg_puts("\r\nSemaphore test (Lock_SempPost/Pend ping-pong).\r\n");
    djy_round = 0U;
    djy_stat_reset(&djy_sem_stat);
    /* Start the two workers: they immediately park in their pend. */
    DJY_EventPop(djy_sem_a_id, NULL, 0U, 0U, 0U, 0U);
    DJY_EventPop(djy_sem_b_id, NULL, 0U, 0U, 0U, 0U);
    Lock_SempPost(djy_sem);            /* start the ping-pong */
    Lock_SempPend(djy_done_sem, CN_TIMEOUT_FOREVER);    /* wait for completion */
    djy_stat_report("Semaphore", &djy_sem_stat);
}

/*-----------------------------------------------------------*/
/* Message queue: two long-lived events ping-pong a 4-byte message;
 * the finishing side releases the driver through the done semaphore
 * before parking. */

static struct djy_stat djy_mq_stat;

static ptu32_t djy_mq_a_script(void)
{
    while(1)
    {
        MsgQ_Receive(djy_mq_a, (u8 *)djy_msg_buf, 4U, CN_TIMEOUT_FOREVER);
        if(djy_round >= BENCH_TEST_ROUNDS + BENCH_DISCARD_ROUNDS)
        {
            Lock_SempPost(djy_done_sem);                /* release the driver */
            Lock_SempPend(djy_sem, CN_TIMEOUT_FOREVER); /* park */
        }
        djy_t0 = BENCH_TIM_READ();
        djy_round++;
        MsgQ_Send(djy_mq_b, (u8 *)djy_msg_buf, 4U, 0U, false);
    }
    return 0U;
}

static ptu32_t djy_mq_b_script(void)
{
    while(1)
    {
        MsgQ_Receive(djy_mq_b, (u8 *)djy_msg_buf, 4U, CN_TIMEOUT_FOREVER);
        u32 t1 = BENCH_TIM_READ();
        if(djy_round >= BENCH_DISCARD_ROUNDS && djy_round < BENCH_TEST_ROUNDS + BENCH_DISCARD_ROUNDS)
            djy_stat_record(&djy_mq_stat, t1 - djy_t0);
        if(djy_round >= BENCH_TEST_ROUNDS + BENCH_DISCARD_ROUNDS)
        {
            Lock_SempPost(djy_done_sem);                /* release the driver */
            Lock_SempPend(djy_sem, CN_TIMEOUT_FOREVER); /* park */
        }
        MsgQ_Send(djy_mq_a, (u8 *)djy_msg_buf, 4U, 0U, false);
    }
    return 0U;
}

static void djy_bench_mq(void)
{
    djy_dbg_puts("\r\nMessage queue test (MsgQ 4B ping-pong).\r\n");
    djy_round = 0U;
    djy_stat_reset(&djy_mq_stat);
    /* Start the two workers: they immediately park in their receive. */
    DJY_EventPop(djy_mq_a_id, NULL, 0U, 0U, 0U, 0U);
    DJY_EventPop(djy_mq_b_id, NULL, 0U, 0U, 0U, 0U);
    MsgQ_Send(djy_mq_a, (u8 *)djy_msg_buf, 4U, 0U, false);
    Lock_SempPend(djy_done_sem, CN_TIMEOUT_FOREVER);    /* wait for completion */
    djy_stat_report("Message queue", &djy_mq_stat);
}

/*-----------------------------------------------------------*/
/* Memory: allocation/free pairs. */

static void djy_bench_mem(void)
{
    u32 round;
    u32 t0, t1;
    struct djy_stat s;
    void *p;

    djy_dbg_puts("\r\nMemory test (M_Malloc/M_Free pairs).\r\n");
    djy_stat_reset(&s);
    for(round = 0U; round < BENCH_TEST_ROUNDS + BENCH_DISCARD_ROUNDS; round++)
    {
        t0 = BENCH_TIM_READ();
        p = M_Malloc(128U, 0U);
        M_Free(p, 0U);
        t1 = BENCH_TIM_READ();
        if(round >= BENCH_DISCARD_ROUNDS)
            djy_stat_record(&s, t1 - t0);
    }
    djy_stat_report("Memory allocation/free pair", &s);
}

/*-----------------------------------------------------------*/
/* ISR items: the interrupt line pops the event types / posts the
 * semaphore / sends the message from the async signal engine. */

static volatile u32 djy_isr_round;
static struct djy_stat djy_isr_notify_stat;
static struct djy_stat djy_isr_activate_stat;
static struct djy_stat djy_isr_sem_stat;
static struct djy_stat djy_isr_mq_stat;
static struct djy_stat djy_isr_relay_stat;
static volatile u32 djy_isr_done;
static volatile u32 djy_isr_target;
static volatile u32 djy_isr_phase;
static volatile u32 djy_isr_sequence_active;
static volatile u32 djy_isr_kicked;
static volatile u32 djy_worker_runs;
static volatile u32 djy_pop_ok;

/* The interrupt-driven items need their own event types: the thread
 * level tests above already own the plain scripts on their types, and
 * a type carries exactly one thread routine. */
static u16 djy_isr_notify_evtt_id;
static u16 djy_isr_activate_evtt_id;
static u8 djy_isr_notify_stk[DJY_EVTT_STK_SIZE] __attribute__((aligned(8)));

static volatile u32 djy_isr_busy;

static ptu32_t djy_isr_notify_script(void)
{
    u32 t1 = BENCH_TIM_READ();
    djy_worker_runs++;
    djy_isr_busy = 0U;      /* the dispatch finished - the ISR may pop again */
    if(djy_isr_round >= BENCH_DISCARD_ROUNDS)
        djy_stat_record(&djy_isr_notify_stat, t1 - djy_isr_stamp);
    return 0U;
}

static ptu32_t djy_isr_activate_script(void)
{
    u32 t1 = BENCH_TIM_READ();
    djy_worker_runs++;
    djy_isr_busy = 0U;
    if(djy_isr_round >= BENCH_DISCARD_ROUNDS)
        djy_stat_record(&djy_isr_activate_stat, t1 - djy_isr_stamp);
    return 0U;
}

static struct SemaphoreLCB *djy_isr_sem;
static struct SemaphoreLCB *djy_isr_gate;
static struct SemaphoreLCB *djy_isr_done_sem;
static u16 djy_isr_sem_evtt_id;

static ptu32_t djy_isr_sem_script(void)
{
    while(1)
    {
        Lock_SempPend(djy_isr_sem, CN_TIMEOUT_FOREVER);
        u32 t1 = BENCH_TIM_READ();
        if(djy_isr_round >= BENCH_DISCARD_ROUNDS)
            djy_stat_record(&djy_isr_sem_stat, t1 - djy_isr_stamp);
        if(djy_isr_done != 0U)
            Lock_SempPend(djy_isr_sem, CN_TIMEOUT_FOREVER);   /* park */
    }
    return 0U;
}

static u16 djy_isr_mq_evtt_id;
static struct MsgQueue *djy_isr_mq;
static u32 djy_isr_msg_buf[4];

static ptu32_t djy_isr_mq_script(void)
{
    while(1)
    {
        MsgQ_Receive(djy_isr_mq, (u8 *)djy_isr_msg_buf, 4U, CN_TIMEOUT_FOREVER);
        u32 t1 = BENCH_TIM_READ();
        if(djy_isr_round >= BENCH_DISCARD_ROUNDS)
            djy_stat_record(&djy_isr_mq_stat, t1 - djy_isr_stamp);
        if(djy_isr_done != 0U)
            Lock_SempPend(djy_isr_sem, CN_TIMEOUT_FOREVER);   /* park */
    }
    return 0U;
}

/* The interrupt line hook: the engine calls the ISR before the event
 * pop, so this drives the counting for the four interrupt items. */
static volatile u32 djy_isr_mode;
#define DJY_ISR_MODE_NOTIFY     1U
#define DJY_ISR_MODE_ACTIVATE   2U
#define DJY_ISR_MODE_SEM        3U
#define DJY_ISR_MODE_MQ         4U
#define DJY_ISR_MODE_RELAY      5U

u32 djy_bench_isr(ptu32_t para);
#define DJY_ISR_PHASE_NOTIFY        0U
#define DJY_ISR_PHASE_ACTIVATE      1U
#define DJY_ISR_PHASE_SEM           2U
#define DJY_ISR_PHASE_MQ            3U
#define DJY_ISR_PHASE_RELAY         4U
#define DJY_ISR_PHASE_NUM           5U

/* The RVM relay item: the hypervisor's own TIM4 top half stamps the
 * counter into the shared page (START) before it signals the vector,
 * so the guest's ISR entry can charge the whole delivery chain - both
 * clock reads are 16 bit, exactly like the other guests' relay item,
 * and the 20000 tick interrupt period keeps the difference unambiguous. */
#define DJY_RELAY_NOW()             ((u16)(BENCH_TIM_READ()))
#define DJY_RELAY_DELTA()           ((u32)((u16)(DJY_RELAY_NOW() - (u16)START)))

/* The interrupt-driven sequence: one phase per measured item, advanced
 * by the interrupt round count itself.  Each phase reports its own
 * statistics when its budget is spent - no measuring thread runs, so
 * nothing can observe or disturb the interrupt path. */
static struct djy_stat * const djy_isr_stats[DJY_ISR_PHASE_NUM] =
{
    &djy_isr_notify_stat,
    &djy_isr_activate_stat,
    &djy_isr_sem_stat,
    &djy_isr_mq_stat,
    &djy_isr_relay_stat
};

static const char * const djy_isr_names[DJY_ISR_PHASE_NUM] =
{
    "ISR Notification",
    "ISR Activation",
    "ISR Semaphore",
    "ISR Message queue",
    "ISR RVM relay"
};
u32 djy_bench_isr(ptu32_t para)
{
    u16 popped = CN_EVENT_ID_INVALID;
    u32 did = 0U;

    (void)para;
    DJY_TIM4_SR &= ~DJY_TIM4_SR_UIF;    /* rc_w0: only a zero bit clears */
    if((djy_isr_sequence_active == 0U) ||
       (djy_isr_phase >= DJY_ISR_PHASE_NUM))
        return 0U;                      /* no sequence running */
    switch(djy_isr_mode)
    {
        case DJY_ISR_MODE_NOTIFY:
            popped = DJY_EventPop(djy_isr_notify_evtt_id, NULL, 0U, 0U, 0U, 0U);
            if(popped != CN_EVENT_ID_INVALID)
                djy_isr_stamp = BENCH_TIM_READ();
            did = 1U;
            break;
        case DJY_ISR_MODE_ACTIVATE:
            popped = DJY_EventPop(djy_isr_activate_evtt_id, NULL, 0U, 0U, 0U, 0U);
            if(popped != CN_EVENT_ID_INVALID)
                djy_isr_stamp = BENCH_TIM_READ();
            /* The wait-free list belongs to the idle event alone: a
             * second drainer here raced with it and double-freed. */
            did = 1U;
            break;
        case DJY_ISR_MODE_SEM:
            if(djy_isr_kicked == 0U)
            {
                /* First interrupt of the phase: start the worker; it
                 * parks in its pend for the rest of the phase. */
                djy_isr_kicked = 1U;
                DJY_EventPop(djy_isr_sem_evtt_id, NULL, 0U, 0U, 0U, 0U);
            }
            else
            {
                djy_isr_stamp = BENCH_TIM_READ();
                Lock_SempPost(djy_isr_sem);
            }
            did = 1U;
            break;
        case DJY_ISR_MODE_MQ:
            if(djy_isr_kicked == 0U)
            {
                djy_isr_kicked = 1U;
                DJY_EventPop(djy_isr_mq_evtt_id, NULL, 0U, 0U, 0U, 0U);
            }
            else
            {
                djy_isr_stamp = BENCH_TIM_READ();
                MsgQ_Send(djy_isr_mq, (u8 *)djy_isr_msg_buf, 4U, 0U, false);
            }
            did = 1U;
            break;
        case DJY_ISR_MODE_RELAY:
            /* Nothing to dispatch: charge the whole chain from the
             * hypervisor's entry stamp to this point of the guest. */
            if(djy_isr_round >= BENCH_DISCARD_ROUNDS)
                djy_stat_record(&djy_isr_relay_stat, DJY_RELAY_DELTA());
            did = 1U;
            break;
        default:
            break;
    }
    djy_isr_round++;
    if(djy_isr_round >= BENCH_TEST_ROUNDS + BENCH_DISCARD_ROUNDS)
    {
        /* Advance only - printing from interrupt level would put a
         * long chain of console hypercalls inside the dispatch path. */
        djy_isr_round = 0U;
        djy_isr_kicked = 0U;
        djy_isr_phase++;
        if(djy_isr_phase >= DJY_ISR_PHASE_NUM)
        {
            djy_isr_mode = 0U;
            Lock_SempPost(djy_isr_done_sem);
        }
        else
        {
            djy_isr_mode = djy_isr_phase + 1U;
        }
    }
    return 0U;
}
static void djy_bench_isr_sequence(void)
{
    u32 i;

    for(i = 0U; i < DJY_ISR_PHASE_NUM; i++)
        djy_stat_reset(djy_isr_stats[i]);

    /* Nothing is queued from thread level here: every worker is started
     * by its phase's first interrupt, so the measuring context never
     * triggers a scheduling round of its own. */
    djy_isr_round = 0U;
    djy_isr_kicked = 0U;
    djy_isr_phase = DJY_ISR_PHASE_NOTIFY;
    djy_isr_mode = DJY_ISR_PHASE_NOTIFY + 1U;
    djy_isr_sequence_active = 1U;

    /* Block until the last phase reports - the sequence itself is run
     * entirely from interrupt level. */
    Lock_SempPend(djy_isr_done_sem, CN_TIMEOUT_FOREVER);
    djy_isr_sequence_active = 0U;

    for(i = 0U; i < DJY_ISR_PHASE_NUM; i++)
        djy_stat_report(djy_isr_names[i], djy_isr_stats[i]);
    djy_dbg_puts("\r\nThe interrupt-driven sequence is complete.\r\n");
}

static void djy_bench_isr_run(u32 mode, u32 rounds, struct djy_stat *s,
                              const char *name, void (*kick)(void))
{
    djy_dbg_puts("\r\n");
    djy_dbg_puts(name);
    djy_dbg_puts(" test.\r\n");

    djy_isr_done = 0U;
    djy_stat_reset(s);
    /* Arm the counters before the interrupt source is allowed to act:
     * once the mode is set the source starts popping events, and the
     * first worker may complete before the driver even blocks. */
    djy_isr_round = 0U;
    djy_isr_target = rounds + BENCH_DISCARD_ROUNDS;
    djy_worker_runs = 0U;
    djy_isr_busy = 0U;
    djy_pop_ok = 0U;
    {
        extern volatile u32 djy_ctx_dbg[4];
        djy_ctx_dbg[0] = 0U;
    }
    djy_isr_mode = mode;

    if(kick != NULL)
        kick();              /* start the worker event before counting */

    Lock_SempPend(djy_isr_gate, CN_TIMEOUT_FOREVER);

    djy_dbg_puts(" [w=");
    djy_dbg_putu32(djy_worker_runs);
    djy_dbg_puts(" p=");
    djy_dbg_putu32(djy_pop_ok);
    djy_dbg_puts(" r=");
    djy_dbg_putu32(djy_isr_round);
    djy_dbg_puts(" t=");
    djy_dbg_putu32(djy_isr_target);
    {
        extern volatile u32 djy_ctx_runs;
        extern volatile u32 djy_sci_calls;
        extern volatile u32 djy_ctx_dbg[4];
        extern volatile u32 djy_switch_flag;
        djy_dbg_puts(" c=");
        djy_dbg_putu32(djy_ctx_runs);
        djy_dbg_puts(" s=");
        djy_dbg_putu32(djy_sci_calls);
        djy_dbg_puts(" m=");
        djy_dbg_putu32(RVM_Int_Mask);
        djy_dbg_puts(" f=");
        djy_dbg_putu32(djy_switch_flag);
        djy_dbg_puts(" vm=");
        djy_dbg_putu32(djy_ctx_dbg[0]);
        djy_dbg_puts(" sp=");
        djy_dbg_putu32(djy_ctx_dbg[1]);
        djy_dbg_puts(" pc=");
        djy_dbg_putu32(djy_ctx_dbg[2]);
        djy_dbg_puts(" xs=");
        djy_dbg_putu32(djy_ctx_dbg[3]);
        djy_ctx_runs = 0U;
    }
    djy_dbg_puts("]");

    djy_isr_mode = 0U;
    djy_isr_done = 1U;
    djy_stat_report(name, s);
}

/* The semaphore-park wake trick: post once after done so the parked
 * scripts fall into their final pend. */
static void djy_bench_isr_park_release(void)
{
    Lock_SempPost(djy_isr_sem);
    Lock_SempPost(djy_isr_sem);
}

/*-----------------------------------------------------------*/
/* The relay: measure from the kernel-side stamp to the guest entry.
 * This needs the shared memory stamp that the kernel test-side
 * writes on its own interrupt handler entry - the rvm_test.h
 * interface of the benchmark infrastructure. */

#if defined(RVM_TEST_VIRTUAL_INC)
extern volatile rvm_ptr_t RVM_TEST_INC_STAMP;
#endif

/*-----------------------------------------------------------*/
/* Boot and drive. */

extern void djy_main(void);

int main(void)
{
    djy_main();
    return 0;
}

/* The two long-lived worker events of the ISR semaphore and message
 * queue items: popped once, they park in their pend for the rest of
 * the benchmark. */
static void djy_isr_sem_kick(void)
{
    DJY_EventPop(djy_isr_sem_evtt_id, NULL, 0U, 0U, 0U, 0U);
}

static void djy_isr_mq_kick(void)
{
    DJY_EventPop(djy_isr_mq_evtt_id, NULL, 0U, 0U, 0U, 0U);
}

static ptu32_t djy_driver_script(void)
{
    djy_dbg_puts("\r\nDjyOS V1.1.0 on RVM - Standard benchmark test\r\n");
    djy_dbg_puts("====================================================\r\n");
    djy_dbg_puts("Test (number in CPU cycles)        : AVG / MAX / MIN\r\n");

    djy_bench_int_init();

    /* The interrupt-driven items run first, while the ready queue holds
     * nothing but this driver: any event left over from a thread level
     * test would otherwise be picked up by the very first dispatch. */
    djy_bench_isr_sequence();

    djy_bench_yield();
    djy_bench_notify_once(djy_notify_id, &djy_notify_stat, "Notification");
    /* The 256B variant is a diagnostic control for the stack repaint,
     * not one of the agreed eleven items - it stays available but is
     * not part of the reported sequence. */
    /* djy_bench_notify_once(djy_notify_lo_id, &djy_notify_lo_stat, "Notification (256B)"); */
    djy_bench_activate();
    djy_bench_sem();
    djy_bench_mq();
    djy_bench_mem();

    /* The four interrupt-driven items have already run at the head of
     * the script (see djy_bench_isr_sequence). */
    /* djy_bench_isr_sequence(); */

    djy_bench_isr_park_release();

    {
        extern volatile u32 djy_defer_hits;
        extern volatile u32 djy_unalign_hits;
        extern volatile u32 djy_bad_frames;
        extern volatile u32 djy_foreign_saves;
        djy_dbg_puts("\r\n[slowpath defers=");
        djy_dbg_putu32(djy_defer_hits);
        djy_dbg_puts(" padded=");
        djy_dbg_putu32(djy_unalign_hits);
        djy_dbg_puts(" badframes=");
        djy_dbg_putu32(djy_bad_frames);
        djy_dbg_puts(" foreign=");
        djy_dbg_putu32(djy_foreign_saves);
        {
            extern volatile u32 djy_bad_dbg[4];
            djy_dbg_puts(" bad[to=");
            djy_dbg_putu32(djy_bad_dbg[0]);
            djy_dbg_puts(" from=");
            djy_dbg_putu32(djy_bad_dbg[1]);
            djy_dbg_puts(" pc=");
            djy_dbg_putu32(djy_bad_dbg[2]);
            djy_dbg_puts(" xs=");
            djy_dbg_putu32(djy_bad_dbg[3]);
            djy_dbg_puts("]");
            {
                extern volatile u32 djy_bad_dbg2[4];
                djy_dbg_puts(" trace[lr=");
                djy_dbg_putu32(djy_bad_dbg2[0]);
                djy_dbg_puts(" vm=");
                djy_dbg_putu32(djy_bad_dbg2[1]);
                djy_dbg_puts(" run=");
                djy_dbg_putu32(djy_bad_dbg2[2]);
                djy_dbg_puts(" rdy=");
                djy_dbg_putu32(djy_bad_dbg2[3]);
                djy_dbg_puts("]");
            }
        }
        djy_dbg_puts("]\r\n");
    }
    djy_dbg_puts("\r\nBenchmark complete.\r\n");
    return 0U;
}

/* Stacks for the driver and the ISR worker scripts. */
static u8 djy_drv_stk[DJY_EVTT_STK_SIZE] __attribute__((aligned(8)));
static u8 djy_isr_sem_stk[DJY_EVTT_STK_SIZE] __attribute__((aligned(8)));
static u8 djy_isr_mq_stk[DJY_EVTT_STK_SIZE] __attribute__((aligned(8)));

void djy_bench_boot(void)
{
    /* The driver event type - the lowest priority benchmark event. */
    u16 driver_id;

    /* Create the primitives. */
    djy_sem = Lock_SempCreate(1, 0, CN_BLOCK_PRIO, "bench_sem");
    djy_done_sem = Lock_SempCreate(1, 0, CN_BLOCK_PRIO, "bench_done");
    djy_idle_sem = Lock_SempCreate(1, 0, CN_BLOCK_PRIO, "bench_idle");
    djy_isr_gate = Lock_SempCreate(1, 0, CN_BLOCK_PRIO, "bench_gate");
    djy_isr_done_sem = Lock_SempCreate(1, 0, CN_BLOCK_PRIO, "bench_isdone");
    djy_isr_sem = Lock_SempCreate(1, 0, CN_BLOCK_PRIO, "bench_isr_sem");
    djy_mq_a = MsgQ_Create(8U, 4U, 0U);
    djy_mq_b = MsgQ_Create(8U, 4U, 0U);
    djy_isr_mq = MsgQ_Create(16U, 4U, 0U);

    /* Register the event types, priorities chosen so that:
     *   ISR scripts (20) > ping-pong pairs (30) > driver (40). */
    djy_yield_a_id = DJY_EvttRegist(EN_CORRELATIVE, 30, 1, 1,
                    djy_yield_a_script, djy_yield_a_stk, DJY_EVTT_STK_SIZE, "y_a");
    djy_yield_b_id = DJY_EvttRegist(EN_CORRELATIVE, 30, 1, 1,
                    djy_yield_b_script, djy_yield_b_stk, DJY_EVTT_STK_SIZE, "y_b");
    djy_notify_id = DJY_EvttRegist(EN_CORRELATIVE, 25, 1, 1,
                    djy_notify_script, djy_notify_stk, DJY_EVTT_STK_SIZE, "noti");
    djy_notify_lo_id = DJY_EvttRegist(EN_CORRELATIVE, 25, 1, 1,
                    djy_notify_script, djy_notify_lo_stk, DJY_LOW_STK_SIZE, "notl");
    djy_activate_id = DJY_EvttRegist(EN_INDEPENDENCE, 200, 0, 8,
                    djy_activate_script, NULL, DJY_EVTT_STK_SIZE, "actv");
    djy_sem_a_id = DJY_EvttRegist(EN_CORRELATIVE, 30, 1, 1,
                    djy_sem_a_script, djy_sem_a_stk, DJY_EVTT_STK_SIZE, "s_a");
    djy_sem_b_id = DJY_EvttRegist(EN_CORRELATIVE, 30, 1, 1,
                    djy_sem_b_script, djy_sem_b_stk, DJY_EVTT_STK_SIZE, "s_b");
    djy_mq_a_id = DJY_EvttRegist(EN_CORRELATIVE, 30, 1, 1,
                    djy_mq_a_script, djy_mq_a_stk, DJY_EVTT_STK_SIZE, "m_a");
    djy_mq_b_id = DJY_EvttRegist(EN_CORRELATIVE, 30, 1, 1,
                    djy_mq_b_script, djy_mq_b_stk, DJY_EVTT_STK_SIZE, "m_b");

    /* The long-lived ISR workers - popped once by their kick function,
     * they park in their pend between the interrupt deliveries. */
    djy_isr_sem_evtt_id = DJY_EvttRegist(EN_CORRELATIVE, 20, 1, 1,
                    djy_isr_sem_script, djy_isr_sem_stk, DJY_EVTT_STK_SIZE, "is_s");
    djy_isr_mq_evtt_id = DJY_EvttRegist(EN_CORRELATIVE, 20, 1, 1,
                    djy_isr_mq_script, djy_isr_mq_stk, DJY_EVTT_STK_SIZE, "is_m");

    /* The interrupt-pop items: one pooled event per interrupt, run to
     * completion.  Notification keeps its thread (CN_KEEP), the
     * activation type is independent at a priority above 0x80 so every
     * interrupt-driven event takes the delete path. */
    djy_isr_notify_evtt_id = DJY_EvttRegist(EN_CORRELATIVE, 25, 1, 1,
                    djy_isr_notify_script, djy_isr_notify_stk, DJY_EVTT_STK_SIZE, "i_no");
    djy_isr_activate_evtt_id = DJY_EvttRegist(EN_INDEPENDENCE, 200, 0, 8,
                    djy_isr_activate_script, NULL, DJY_EVTT_STK_SIZE, "i_ac");

    driver_id = DJY_EvttRegist(EN_CORRELATIVE, 40, 1, 1,
                    djy_driver_script, djy_drv_stk, DJY_EVTT_STK_SIZE, "drv");

    /* Arm the driver launch: the pop itself is performed by the idle
     * thread's first service pass (see djy_bench_idle_pulse).  Popping
     * it here, in the startup context, would schedule a switch out of a
     * context that belongs to no thread and park that startup frame in
     * the running event's vm - the corruption the port then trips on. */
    djy_driver_evtt_id = driver_id;
    djy_driver_pending = 1U;

    /* The idle event calls the application hook on every pass; the
     * benchmark's hook is its driver pulse - it must not park, or the
     * wake-up latency would land in the measured cycles. */
    djy_set_idle_hook(djy_bench_idle_pulse);

    /* Connect the interrupt line to the engine hook - the pop-based
     * items use the line's my_evtt_id binding... which we instead
     * drive from the ISR itself for mode switching, so the line only
     * needs the ISR. */
}
