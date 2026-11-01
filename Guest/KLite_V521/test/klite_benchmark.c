/*
 * KLite V5.2.1 benchmark application for the RVM hypervisor.
 * Copyright (C) 2026 Evo-Devo Instrum. All rights reserved.
 *
 * This file is NOT part of the KLite distribution.  It belongs to the
 * KLite-on-RVM test suite and is released under the Unlicense; refer to
 * the LICENSE file for details.  The KLite kernel itself is copyright
 * jiangxiaogang and is not modified by this file.
 *
 * This file measures the performance of the KLite kernel when it runs
 * as a guest of the RVM hypervisor.  All the hardware dependent code is
 * held in the chip specific test header that is selected by the generated
 * "klt_test.h" header, so this file does not have to be modified when
 * the benchmark is ported to a new chip.
 */

#include "rvm.h"
#include "kernel.h"
#include "queue.h"

/* Benchmark includes. */
#include "klt_test.h"


/* The number of rounds each test is repeated for before its average,
 * maximum and minimum results are calculated. */
#ifndef BENCH_TEST_ROUNDS
    #define BENCH_TEST_ROUNDS                      ((uint32_t)10000u)
#endif

/* The number of counter overflows that are counted between two progress
 * reports that are printed once the benchmark itself has finished. */
#define BENCH_OVERFLOW_REPORT_INTERVAL             ((uint32_t)10000u)

/* The size of the stack allocated to each benchmark thread, measured in
 * bytes.  This is port dependent, so every chip header must define it -
 * there is no fallback. */
#ifndef BENCH_TASK_STK_SIZE
    #error "BENCH_TASK_STK_SIZE must be defined by the chip specific test header"
#endif

/* The blocking timeout of the benchmark's queue receives - KLite counts
 * a timeout of zero as a poll, so the waits use a value that can never
 * elapse before the matching send arrives. */
#define BENCH_WAIT_FOREVER                         ((uint32_t)0xFFFFFFFFu)


/* Reset the statistics that are accumulated by the BENCH_STAT_RECORD()
 * macro. */
#define BENCH_STAT_RESET()                         \
do                                                 \
{                                                  \
    Total = 0u;                                    \
    Maximum = 0u;                                  \
    Minimum = (uint16_t)0xFFFFu;                   \
}                                                  \
while (0)

/* Accumulate the result of a single round into the current statistics.  The
 * counter difference is calculated using 16 bit arithmetic so that a single
 * overflow of the counter between the two reads is handled transparently. */
#define BENCH_STAT_RECORD()                        \
do                                                 \
{                                                  \
    CntDiff = (uint16_t)(CntEnd - CntStart);       \
    Total += (uint32_t)CntDiff;                    \
    if (CntDiff > Maximum)                         \
    {                                              \
        Maximum = CntDiff;                         \
    }                                              \
    if (CntDiff < Minimum)                         \
    {                                              \
        Minimum = CntDiff;                         \
    }                                              \
}                                                  \
while (0)

/* Print the name of a test followed by its average, maximum and minimum
 * results, all of them expressed in CPU cycles. */
#define BENCH_STAT_REPORT(pcTestName)              \
do                                                 \
{                                                  \
    BENCH_PRINT_STRING(pcTestName);                \
    BENCH_PRINT_STRING(" : ");                     \
    BENCH_PRINT_NUMBER(Total / BENCH_TEST_ROUNDS); \
    BENCH_PRINT_STRING(" / ");                     \
    BENCH_PRINT_NUMBER(Maximum);                   \
    BENCH_PRINT_STRING(" / ");                     \
    BENCH_PRINT_NUMBER(Minimum);                   \
    BENCH_PRINT_STRING("\r\n");                    \
}                                                  \
while (0)

#ifdef BENCH_KERNEL_TS_GET
/* Accumulate the RVM activation relay latency of a single interrupt into
 * the relay statistics.  The chip specific test header must provide the
 * kernel side interrupt timestamp for this to be available. */
#define BENCH_RELAY_STAT_RECORD()                  \
do                                                 \
{                                                  \
    uint16_t relay_cnt;                            \
    relay_cnt = (uint16_t)BENCH_TIME_GET();        \
    relay_cnt = (uint16_t)(relay_cnt - BENCH_KERNEL_TS_GET()); \
    RelayTotal += (uint32_t)relay_cnt;             \
    if (relay_cnt > RelayMax)                      \
    {                                              \
        RelayMax = relay_cnt;                      \
    }                                              \
    if (relay_cnt < RelayMin)                      \
    {                                              \
        RelayMin = relay_cnt;                      \
    }                                              \
}                                                  \
while (0)
#endif


/* The two threads that run the benchmark.  Bench_Task1 starts the tests and
 * generates the workload while Bench_Task2 receives the workload and prints
 * the results as they become available. */
static void Bench_Task1(void *p_arg);
static void Bench_Task2(void *p_arg);

/* One function per benchmark.  The functions that end in a "1" are called
 * from Bench_Task1 and generate the workload, their counterparts that end
 * in "2" are called from Bench_Task2 and receive the workload. */
static void Bench_YieldTest1(void);
static void Bench_YieldTest2(void);
static void Bench_EventTest1(void);
static void Bench_EventTest2(void);
static void Bench_SemTest1(void);
static void Bench_SemTest2(void);
static void Bench_QueueTest1(void);
static void Bench_QueueTest2(void);
static void Bench_EventFromISRTest(void);
static void Bench_SemFromISRTest(void);
static void Bench_QueueFromISRTest(void);
static void Bench_MemTest(void);
static uint32_t Bench_Rand(void);
static void Bench_Swap(uint32_t *p_first, uint32_t *p_second);

/* The statistics of the test that is currently running, and of the relay
 * measurement that the interrupt handler accumulates in parallel. */
static uint32_t Total;
static uint16_t Maximum;
static uint16_t Minimum;
static uint16_t CntDiff;
static uint32_t CntStart;
static uint32_t CntEnd;
static volatile uint32_t Flip;

/* The count of the interrupt handler entries that found the previous entry's
 * workload still unprocessed - a latched, print-free replacement of the
 * per-entry diagnostic print: the debug print blocks on the debug console
 * hypercall for far longer than the 40000 cycle period of the benchmark
 * interrupt, so a per-entry print builds up an ever growing backlog in the
 * hypervisor's vector delivery queue and starves the guest's own threads
 * for good. */
static volatile uint32_t ReenterCnt = 0u;
static uint32_t OverflowCnt;
#ifdef BENCH_KERNEL_TS_GET
static uint32_t RelayTotal;
static uint16_t RelayMax;
static uint16_t RelayMin;
#endif

/* The kernel objects that the benchmark uses.  KLite has no direct-to-thread
 * notification primitive, so the notification slot of the other benchmarks
 * is measured with the auto-reset event: event_set() wakes exactly one
 * waiter and resets itself, the closest counterpart of a notification. */
static thread_t Task1Tid;
static thread_t Task2Tid;
static sema_t TestSem;
static event_t TestEvent;
static queue_t TestQueue;
static uint32_t MsgValue = 1u;

/* The interrupt driven test results are accumulated separately, because
 * they print after all three of them have finished. */
static uint32_t EventISRTotal;
static uint16_t EventISRMax;
static uint16_t EventISRMin;
static uint32_t SemISRTotal;
static uint16_t SemISRMax;
static uint16_t SemISRMin;
static uint32_t QueueISRTotal;
static uint16_t QueueISRMax;
static uint16_t QueueISRMin;

/* The kernel heap - the chip header sizes it, the kernel owns it after
 * kernel_init(). */
static uint8_t HeapBuf[BENCH_HEAP_SIZE] __attribute__((aligned(8)));


/*
 * ROUND-ROBIN YIELD TEST: GENERATE SIDE
 *
 * Generate the workload of the round-robin yield test.
 */
static void Bench_YieldTest1(void)
{
    uint32_t cnt;

    for (cnt = 0u; cnt < BENCH_TEST_ROUNDS; cnt++)
    {
        /* Verify that execution returned from Bench_Task2 as expected. */
        if (Flip != 0u)
        {
            BENCH_PRINT_STRING("Yield error in task 1 @ round ");
            BENCH_PRINT_NUMBER(cnt);
            BENCH_PRINT_STRING(".\r\n");
        }

        Flip = 1u;

        /* Read the counter that opens the bracket around the yield. */
        CntStart = BENCH_TIME_GET();

        thread_yield();
    }
}

/*
 * ROUND-ROBIN YIELD TEST: RECEIVE SIDE
 *
 * Receive the workload of the round-robin yield test and report its statistics.
 */
static void Bench_YieldTest2(void)
{
    uint32_t cnt;

    for (cnt = 0u; cnt < BENCH_TEST_ROUNDS; cnt++)
    {
        thread_yield();

        /* Read the counter that closes the bracket around the yield, then
         * record the result of this round. */
        CntEnd = BENCH_TIME_GET();
        BENCH_STAT_RECORD();

        /* Verify that execution came from Bench_Task1 as expected. */
        if (Flip == 0u)
        {
            BENCH_PRINT_STRING("Yield error in task 2 @ round ");
            BENCH_PRINT_NUMBER(cnt);
            BENCH_PRINT_STRING(", ");
            BENCH_PRINT_NUMBER(CntStart);
            BENCH_PRINT_STRING(", ");
            BENCH_PRINT_NUMBER(CntEnd);
            BENCH_PRINT_STRING(".\r\n");
        }

        Flip = 0u;
    }
}

/*
 * EVENT TEST: GENERATE SIDE
 *
 * Generate the workload of the event test - the KLite counterpart of the
 * task notification test of the other benchmarks.
 */
static void Bench_EventTest1(void)
{
    uint32_t cnt;

    for (cnt = 0u; cnt < BENCH_TEST_ROUNDS; cnt++)
    {
        CntStart = BENCH_TIME_GET();
        event_set(TestEvent);
    }
}

/*
 * EVENT TEST: RECEIVE SIDE
 *
 * Receive the workload of the event test and report its statistics.
 */
static void Bench_EventTest2(void)
{
    uint32_t cnt;

    for (cnt = 0u; cnt < BENCH_TEST_ROUNDS; cnt++)
    {
        event_wait(TestEvent);

        CntEnd = BENCH_TIME_GET();
        BENCH_STAT_RECORD();
    }
}

/*
 * COUNTING SEMAPHORE TEST: GENERATE SIDE
 *
 * Generate the workload of the counting semaphore test.
 */
static void Bench_SemTest1(void)
{
    uint32_t cnt;

    for (cnt = 0u; cnt < BENCH_TEST_ROUNDS; cnt++)
    {
        CntStart = BENCH_TIME_GET();
        sema_post(TestSem);
    }
}

/*
 * COUNTING SEMAPHORE TEST: RECEIVE SIDE
 *
 * Receive the workload of the counting semaphore test and report its statistics.
 */
static void Bench_SemTest2(void)
{
    uint32_t cnt;

    for (cnt = 0u; cnt < BENCH_TEST_ROUNDS; cnt++)
    {
        sema_wait(TestSem);

        CntEnd = BENCH_TIME_GET();
        BENCH_STAT_RECORD();
    }
}

/*
 * MESSAGE QUEUE TEST: GENERATE SIDE
 *
 * Generate the workload of the message queue test.
 */
static void Bench_QueueTest1(void)
{
    uint32_t cnt;

    for (cnt = 0u; cnt < BENCH_TEST_ROUNDS; cnt++)
    {
        CntStart = BENCH_TIME_GET();
        (void)queue_send(TestQueue, &MsgValue, 0u);
    }
}

/*
 * MESSAGE QUEUE TEST: RECEIVE SIDE
 *
 * Receive the workload of the message queue test and report its statistics.
 */
static void Bench_QueueTest2(void)
{
    uint32_t cnt;
    uint32_t msg_value;

    for (cnt = 0u; cnt < BENCH_TEST_ROUNDS; cnt++)
    {
        (void)queue_recv(TestQueue, &msg_value, BENCH_WAIT_FOREVER);

        CntEnd = BENCH_TIME_GET();
        BENCH_STAT_RECORD();
    }
}

/*
 * INTERRUPT DRIVEN EVENT TEST
 *
 * Receive the workload of the interrupt driven event test and report its statistics.
 */
static void Bench_EventFromISRTest(void)
{
    uint32_t cnt;

    /* NOTE: no print here!  The interrupt source is already running when
     * this function is entered, and the debug print takes a few thousand
     * cycles - well over one interrupt period.  The queue behind the event
     * can only hold so much, so every print spent here delays the first
     * receive and the delivery backlog it creates starves the guest's own
     * threads.  The uC/OS and FreeRTOS benchmarks enter their interrupt
     * driven phases silently for exactly this reason. */
    for (cnt = 0u; cnt < BENCH_TEST_ROUNDS; cnt++)
    {
        event_wait(TestEvent);

        CntEnd = BENCH_TIME_GET();
        BENCH_STAT_RECORD();

        Flip = 0u;
    }
}

/*
 * INTERRUPT DRIVEN COUNTING SEMAPHORE TEST
 *
 * Receive the workload of the interrupt driven counting semaphore test and report its statistics.
 */
static void Bench_SemFromISRTest(void)
{
    uint32_t cnt;

    for (cnt = 0u; cnt < BENCH_TEST_ROUNDS; cnt++)
    {
        sema_wait(TestSem);

        CntEnd = BENCH_TIME_GET();
        BENCH_STAT_RECORD();

        Flip = 0u;
    }
}

/*
 * INTERRUPT DRIVEN MESSAGE QUEUE TEST
 *
 * Receive the workload of the interrupt driven message queue test and report its statistics.
 */
static void Bench_QueueFromISRTest(void)
{
    uint32_t cnt;
    uint32_t msg_value;

    for (cnt = 0u; cnt < BENCH_TEST_ROUNDS; cnt++)
    {
        (void)queue_recv(TestQueue, &msg_value, BENCH_WAIT_FOREVER);

        CntEnd = BENCH_TIME_GET();
        BENCH_STAT_RECORD();

        Flip = 0u;
    }
}

/*
 * PSEUDO RANDOM NUMBER
 *
 * Return the next value of a maximal length 16 bit linear feedback shift register.
 */
static uint32_t Bench_Rand(void)
{
    static uint32_t RandState = 0xACE1u;
    uint32_t rand_val;

    RandState ^= RandState << 13u;
    RandState ^= RandState >> 9u;
    RandState ^= RandState << 7u;
    rand_val = RandState;

    return rand_val;
}

/*
 * EXCHANGE TWO 32 BIT VALUES
 *
 * Exchange the two 32 bit values that the arguments point to.
 */
static void Bench_Swap(uint32_t *p_first, uint32_t *p_second)
{
    uint32_t temp;

    temp = *p_first;
    *p_first = *p_second;
    *p_second = temp;
}

/*
 * MEMORY ALLOCATION/FREE PAIR TEST
 *
 * Exercise the heap allocator with randomised allocation and free orders, then report the statistics.
 */
static void Bench_MemTest(void)
{
    void *p_mem[BENCH_MEM_BLK_QTY];
    uint32_t alloc_order[BENCH_MEM_BLK_QTY];
    uint32_t free_order[BENCH_MEM_BLK_QTY];
    uint32_t case_cnt;
    uint32_t test_cnt;

    for (test_cnt = 0u; test_cnt < BENCH_TEST_ROUNDS; test_cnt++)
    {
        /* Generate a random allocation order and free order. */
        for (case_cnt = 0u; case_cnt < BENCH_MEM_BLK_QTY; case_cnt++)
        {
            alloc_order[case_cnt] = case_cnt;
            free_order[case_cnt] = case_cnt;
        }

        for (case_cnt = BENCH_MEM_BLK_QTY - 1u; case_cnt > 0u; case_cnt--)
        {
            Bench_Swap(&alloc_order[case_cnt], &alloc_order[Bench_Rand() % (case_cnt + 1u)]);
            Bench_Swap(&free_order[case_cnt], &free_order[Bench_Rand() % (case_cnt + 1u)]);
        }

        CntStart = BENCH_TIME_GET();

        /* Allocation tests - all of them must succeed, the heap always has
         * all of its blocks free at this point. */
        p_mem[alloc_order[0]] = heap_alloc(NULL, BENCH_MEM_BLK_SIZE);
        p_mem[alloc_order[1]] = heap_alloc(NULL, BENCH_MEM_BLK_SIZE);
        p_mem[alloc_order[2]] = heap_alloc(NULL, BENCH_MEM_BLK_SIZE);
        p_mem[alloc_order[3]] = heap_alloc(NULL, BENCH_MEM_BLK_SIZE);
        p_mem[alloc_order[4]] = heap_alloc(NULL, BENCH_MEM_BLK_SIZE);
        p_mem[alloc_order[5]] = heap_alloc(NULL, BENCH_MEM_BLK_SIZE);
        p_mem[alloc_order[6]] = heap_alloc(NULL, BENCH_MEM_BLK_SIZE);
        p_mem[alloc_order[7]] = heap_alloc(NULL, BENCH_MEM_BLK_SIZE);

        /* Free tests - in the randomised order. */
        heap_free(NULL, p_mem[free_order[0]]);
        heap_free(NULL, p_mem[free_order[1]]);
        heap_free(NULL, p_mem[free_order[2]]);
        heap_free(NULL, p_mem[free_order[3]]);
        heap_free(NULL, p_mem[free_order[4]]);
        heap_free(NULL, p_mem[free_order[5]]);
        heap_free(NULL, p_mem[free_order[6]]);
        heap_free(NULL, p_mem[free_order[7]]);

        CntEnd = BENCH_TIME_GET();
        BENCH_STAT_RECORD();
    }
}

/*
 * THE BENCHMARK INTERRUPT HANDLER
 *
 * Latch the start count and post the workload of the interrupt driven test
 * that is currently running.
 */
void Bench_TestIntHandler(void)
{
    static uint32_t cnt = 0u;

    /* Measure the RVM activation relay latency while the first test phase
     * is running.  The chip specific test header provides the timestamp
     * that the kernel latched when it took this very interrupt. */
#ifdef BENCH_KERNEL_TS_GET
    if (cnt < BENCH_TEST_ROUNDS)
    {
        BENCH_RELAY_STAT_RECORD();
    }
#endif

    /* Detect interrupt reentry - count it for the report instead of printing:
     * a print on every entry is slower than this interrupt's period and the
     * delivery backlog it creates would starve the guest's own threads. */
    if (Flip != 0u)
    {
        ReenterCnt++;
    }

    Flip = 1u;

    /* The KLite interrupt protocol: every kernel API is safe to call from
     * an interrupt handler as long as it cannot block, and the scheduler
     * pass that the posts trigger resolves to cpu_contex_switch() - which
     * under RVM only marks the switch pending, because we are already on
     * the vector thread.  The actual switch happens when the vector
     * thread's dispatch loop calls klt_ctx_handler() after this handler
     * returns.  The queue send is safe here for the same reason it is safe
     * on the official ports: its mutex region runs inside the critical
     * section, and no interrupt can be delivered into it. */
    if (cnt < BENCH_TEST_ROUNDS)
    {
        cnt++;
        CntStart = BENCH_TIME_GET();
        event_set(TestEvent);
    }
    else if (cnt < (BENCH_TEST_ROUNDS * 2u))
    {
        cnt++;
        CntStart = BENCH_TIME_GET();
        sema_post(TestSem);
    }
    else if (cnt < (BENCH_TEST_ROUNDS * 3u))
    {
        cnt++;
        CntStart = BENCH_TIME_GET();
        (void)queue_send(TestQueue, &MsgValue, 0u);
    }
    else
    {
        Bench_IntDisable();
    }
}

/*
 * BENCHMARK TASK 1
 *
 * Run the workload generating side of every benchmark test.
 */
static void Bench_Task1(void *p_arg)
{
    (void)p_arg;

    BENCH_PRINT_STRING("Test (number in CPU cycles)        : AVG / MAX / MIN\r\n");

    /* Both tasks run at the same priority for the yield test.  KLite
     * appends readied threads at the tail of their ready list, and task 2
     * was created first, so it owns the head and starts first - its
     * leading round-robin yield hands the CPU over to this task, and by
     * the time we get here the list rotation is in its steady alternating
     * phase and every yield of this task switches straight to task 2,
     * exactly like the other benchmarks' priority handover. */
    Bench_YieldTest1();

    /* Raise the priority of task 2 so that it takes over and prints the
     * results of each test as it completes.  KLite counts higher priority
     * numbers as higher priorities. */
    thread_set_priority(Task2Tid, BENCH_TASK_PRIO_HIGH);

    Bench_EventTest1();
    Bench_SemTest1();
    Bench_QueueTest1();

    for (;;);
}

/*
 * BENCHMARK TASK 2
 *
 * Run the workload receiving side of every benchmark test, and print the results as they become available.
 */
static void Bench_Task2(void *p_arg)
{
    (void)p_arg;

    /* Yield tests. */
    BENCH_STAT_RESET();
    Bench_YieldTest2();
    BENCH_STAT_REPORT("Yield                             ");

    /* The remaining tests run with this task at the higher priority. */

    /* Event tests. */
    BENCH_STAT_RESET();
    Bench_EventTest2();
    BENCH_STAT_REPORT("Notification                      ");

    /* Semaphore tests. */
    BENCH_STAT_RESET();
    Bench_SemTest2();
    BENCH_STAT_REPORT("Semaphore                         ");

    /* Blocking message queue tests. */
    BENCH_STAT_RESET();
    Bench_QueueTest2();
    BENCH_STAT_REPORT("Message queue                     ");

    /* Memory allocation/free pair tests. */
    BENCH_STAT_RESET();
    Bench_MemTest();
    BENCH_STAT_REPORT("Memory allocation/free pair       ");

    /* Reset the relay statistics that are accumulated by the interrupt
     * handler itself. */
#ifdef BENCH_KERNEL_TS_GET
    RelayTotal = 0u;
    RelayMax = 0u;
    RelayMin = 0xFFFFu;
#endif

    /* Prepare the interrupt driven tests. */
    Bench_IntSetup();

    /* Event from interrupt tests. */
    BENCH_STAT_RESET();
    Bench_EventFromISRTest();
    EventISRTotal = Total;
    EventISRMax = Maximum;
    EventISRMin = Minimum;

    /* Semaphore from interrupt tests. */
    BENCH_STAT_RESET();
    Bench_SemFromISRTest();
    SemISRTotal = Total;
    SemISRMax = Maximum;
    SemISRMin = Minimum;

    /* Blocking message queue from interrupt tests. */
    BENCH_STAT_RESET();
    Bench_QueueFromISRTest();
    QueueISRTotal = Total;
    QueueISRMax = Maximum;
    QueueISRMin = Minimum;

    /* Print the results of the interrupt driven tests. */
    Total = EventISRTotal;
    Maximum = EventISRMax;
    Minimum = EventISRMin;
    BENCH_STAT_REPORT("ISR Notification                  ");

    Total = SemISRTotal;
    Maximum = SemISRMax;
    Minimum = SemISRMin;
    BENCH_STAT_REPORT("ISR Semaphore                     ");

    Total = QueueISRTotal;
    Maximum = QueueISRMax;
    Minimum = QueueISRMin;
    BENCH_STAT_REPORT("ISR Message queue                 ");

#ifdef BENCH_KERNEL_TS_GET
    Total = RelayTotal;
    Maximum = RelayMax;
    Minimum = RelayMin;
    BENCH_STAT_REPORT("ISR RVM activation relay          ");
#endif

    /* Report the latched reentry count, if the workload consumption ever fell
     * behind the interrupt rate. */
    if (ReenterCnt != 0u)
    {
        BENCH_PRINT_STRING("klt: NOTE: interrupt reentered ");
        BENCH_PRINT_NUMBER(ReenterCnt);
        BENCH_PRINT_STRING(" times.\r\n");
    }

    /* The benchmark itself is done.  Keep dumping the counter value so
     * that an incorrectly configured counter clock rate can be detected. */
    for (;;)
    {
        CntStart = CntEnd;
        CntEnd = BENCH_TIME_GET();
        if (CntStart > CntEnd)
        {
            OverflowCnt++;
            if ((OverflowCnt % BENCH_OVERFLOW_REPORT_INTERVAL) == 0u)
            {
                BENCH_PRINT_NUMBER(BENCH_OVERFLOW_REPORT_INTERVAL);
                BENCH_PRINT_STRING(" overflows\r\n");
            }
        }
    }
}

/*
 * The idle thread entry - the official pattern: kernel_idle() never
 * returns, it cleans up dead threads and parks through cpu_sys_sleep().
 * The benchmark registers no sleep hook, so its idle path stays busy and
 * never enters the measured cycles.
 */
static void Bench_Idle(void *p_arg)
{
    (void)p_arg;
    kernel_idle();
}

/*
 * BOOTSTRAP THREAD
 *
 * Create the benchmark objects and the two test threads, then exit.  KLite
 * kernel calls that lock or schedule run against sched_tcb_now/sched_tcb_next,
 * which are NULL until the first context switch - the official examples do
 * all of their object creation from a thread, never before kernel_start(),
 * and the benchmark follows the same pattern (mpool_create alone takes a
 * mutex, whose uncontended path dereferences sched_tcb_now).
 */
static void Bench_Init(void *p_arg)
{
    (void)p_arg;

    /* Create the kernel objects that are used by the benchmark. */
    TestEvent = event_create(true);
    TestSem = sema_create(0u);
    TestQueue = queue_create(BENCH_QUEUE_ITEM_SIZE, BENCH_QUEUE_LENGTH);

    BENCH_PRINT_STRING("\r\n");
    BENCH_PRINT_STRING("KLite V5.2.1 on RVM - Standard benchmark test\r\n");
    BENCH_PRINT_STRING("====================================================\r\n");

    /* Create the two test threads inside a critical section, so that the
     * startup of the first thread cannot preempt the creation of the
     * second one - both have to sit in the ready list before the rotation
     * begins.  KLite appends readied threads at the tail of their priority
     * list, so task 2 is created first to leave task 2 right behind this
     * thread - when this thread exits, the scheduler starts task 2, whose
     * leading round-robin yield hands the CPU over to task 1, exactly like
     * the other benchmarks. */
    cpu_enter_critical();
    Task2Tid = thread_create(Bench_Task2, NULL, BENCH_TASK_STK_SIZE);
    Task1Tid = thread_create(Bench_Task1, NULL, BENCH_TASK_STK_SIZE);
    cpu_leave_critical();

    thread_exit();
}

/*
 * MAIN
 *
 * Boot the kernel, create the idle and bootstrap threads, and start the
 * scheduler.  The virtual machine enters through this function on the
 * startup context; the first context switch abandons it.
 */
int main(void)
{
    /* Set up the counter that is used to time the tests - plain register
     * writes, safe before the scheduler runs. */
    Bench_TimerSetup();

    /* Boot the kernel on the benchmark heap. */
    kernel_init(HeapBuf, sizeof(HeapBuf));

    /* The idle thread and the bootstrap thread - the only two kernel calls
     * that are safe before kernel_start(), exactly like the official
     * examples' main. */
    (void)thread_create(Bench_Idle, NULL, 0);
    (void)thread_create(Bench_Init, NULL, 0);

    /* Start the scheduler - it releases the boot critical section and
     * never returns. */
    kernel_start();

    /* If control ever falls through to here, block in the hypervisor
     * instead of spinning the user thread. */
    for (;;)
    {
        RVM_Virt_Yield();
    }
}
