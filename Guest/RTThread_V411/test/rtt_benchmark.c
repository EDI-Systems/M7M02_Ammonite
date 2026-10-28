/*
 * RT-Thread V4.1.1 benchmark application, running as a guest of the RVM
 * hypervisor.
 * Copyright (C) 2026 Evo-Devo Instrum. All rights reserved.
 *
 * This file is NOT part of the RT-Thread distribution.  It belongs to the
 * RT-Thread-on-RVM test suite and is released under the Unlicense; refer
 * to the LICENSE file for details.  The RT-Thread kernel itself is
 * copyright RT-Thread Development Team and is not modified by this file.
 *
 * File      : rtt_benchmark.c
 * Version   : V1.00.00
 *
 * Note(s)  : (1) This file measures the performance of the RT-Thread kernel
 *                when it runs as a guest of the RVM hypervisor.  All the
 *                hardware dependent code is held in the chip specific test
 *                header that is selected by the generated "rtt_test.h"
 *                header.  The tests mirror the FreeRTOS-on-RVM and the
 *                uC/OS-III-on-RVM benchmarks one by one: RT-Thread has no
 *                direct-to-thread notification primitive, so the
 *                notification slot is measured with the single word
 *                mailbox instead, and the yield test uses the round-robin
 *                yield with both threads at the same priority.
 */

/* RT-Thread includes. */
#include <rtthread.h>
#include "rvm.h"

/* RT-Thread has no public declaration of the startup routine that the
 * components layer provides, and the BSPs are expected to declare it
 * themselves - do the same here to keep the prototype checking happy. */
extern int rtthread_startup(void);

/* Benchmark includes. */
#include "rtt_test.h"


/* The number of rounds each test is repeated for before its average,
 * maximum and minimum results are calculated. */
#ifndef BENCH_TEST_ROUNDS
    #define BENCH_TEST_ROUNDS                      ((rt_uint32_t)10000u)
#endif

/* The number of counter overflows that are counted between two progress
 * reports that are printed once the benchmark itself has finished. */
#define BENCH_OVERFLOW_REPORT_INTERVAL             ((rt_uint32_t)10000u)

/* The size of the stack allocated to each benchmark thread, measured in
 * bytes.  This is port dependent, so every chip header must define it -
 * there is no fallback. */
#ifndef BENCH_TASK_STK_SIZE
    #error "BENCH_TASK_STK_SIZE must be defined by the chip specific test header"
#endif

/* The two priorities the benchmark threads run at.  RT-Thread counts
 * lower priority numbers as higher priorities, exactly like uC/OS-III. */
#define BENCH_TASK_PRIO_HIGH                       ((rt_uint8_t)4u)
#define BENCH_TASK_PRIO_NORM                       ((rt_uint8_t)5u)


/* Reset the statistics that are accumulated by the BENCH_STAT_RECORD()
 * macro. */
#define BENCH_STAT_RESET()                         \
do                                                 \
{                                                  \
    Total = 0u;                                    \
    Maximum = 0u;                                  \
    Minimum = (rt_uint16_t)0xFFFFu;                \
}                                                  \
while (0)

/* Accumulate the result of a single round into the current statistics.  The
 * counter difference is calculated using 16 bit arithmetic so that a single
 * overflow of the counter between the two reads is handled transparently. */
#define BENCH_STAT_RECORD()                        \
do                                                 \
{                                                  \
    CntDiff = (rt_uint16_t)(CntEnd - CntStart);    \
    Total += (rt_uint32_t)CntDiff;                 \
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
    rt_uint16_t relay_cnt;                         \
    relay_cnt = (rt_uint16_t)BENCH_TIME_GET();     \
    relay_cnt = (rt_uint16_t)(relay_cnt - BENCH_KERNEL_TS_GET()); \
    RelayTotal += (rt_uint32_t)relay_cnt;          \
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
 * in a "2" are called from Bench_Task2 and receive the workload. */
static void Bench_YieldTest1(void);
static void Bench_YieldTest2(void);
static void Bench_MailTest1(void);
static void Bench_MailTest2(void);
static void Bench_SemTest1(void);
static void Bench_SemTest2(void);
static void Bench_QueueTest1(void);
static void Bench_QueueTest2(void);
static void Bench_MailFromISRTest(void);
static void Bench_SemFromISRTest(void);
static void Bench_QueueFromISRTest(void);
static void Bench_MemTest(void);
static rt_uint32_t Bench_Rand(void);
static void Bench_Swap(rt_uint32_t *p_first, rt_uint32_t *p_second);

/* The statistics of the test that is currently running, and of the relay
 * measurement that the interrupt handler accumulates in parallel. */
static rt_uint32_t Total;
static rt_uint16_t Maximum;
static rt_uint16_t Minimum;
static rt_uint16_t CntDiff;
static rt_uint32_t CntStart;
static rt_uint32_t CntEnd;
static volatile rt_bool_t Flip;

/* The count of the interrupt handler entries that found the previous entry's
 * workload still unprocessed - a latched, print-free replacement of the
 * per-entry diagnostic print: the debug print blocks on the UART for a few
 * milliseconds per call, which is far slower than the 12800 cycle period of
 * the benchmark interrupt, so a per-entry print builds up an ever growing
 * backlog in the hypervisor's vector delivery queue and starves the guest's
 * own threads for good. */
static volatile rt_uint32_t ReenterCnt = 0u;
static rt_uint32_t OverflowCnt;
#ifdef BENCH_KERNEL_TS_GET
static rt_uint32_t RelayTotal;
static rt_uint16_t RelayMax;
static rt_uint16_t RelayMin;
#endif

/* The kernel objects that the benchmark uses.  RT-Thread has no
 * direct-to-thread notification primitive, so the notification slot of
 * the other benchmarks is measured with the single word mailbox. */
static rt_thread_t Task1Tid;
static rt_thread_t Task2Tid;
static rt_sem_t TestSem;
static rt_mailbox_t TestMb;
static rt_mq_t TestMq;
static rt_uint32_t MsgValue = 1u;

/* The interrupt driven test results are accumulated separately, because
 * they print after all three of them have finished. */
static rt_uint32_t MailISRTotal;
static rt_uint16_t MailISRMax;
static rt_uint16_t MailISRMin;
static rt_uint32_t SemISRTotal;
static rt_uint16_t SemISRMax;
static rt_uint16_t SemISRMin;
static rt_uint32_t QueueISRTotal;
static rt_uint16_t QueueISRMax;
static rt_uint16_t QueueISRMin;


/*
 * ROUND-ROBIN YIELD TEST: GENERATE SIDE
 *
 * Generate the workload of the round-robin yield test.
 */
static void Bench_YieldTest1(void)
{
    rt_uint32_t cnt;

    for (cnt = 0u; cnt < BENCH_TEST_ROUNDS; cnt++)
    {
        /* Verify that execution returned from Bench_Task2 as expected. */
        if (Flip != RT_FALSE)
        {
            BENCH_PRINT_STRING("Yield error in task 1 @ round ");
            BENCH_PRINT_NUMBER(cnt);
            BENCH_PRINT_STRING(".\r\n");
        }

        Flip = RT_TRUE;

        /* Read the counter that opens the bracket around the yield. */
        CntStart = BENCH_TIME_GET();

        rt_thread_yield();
    }
}

/*
 * ROUND-ROBIN YIELD TEST: RECEIVE SIDE
 *
 * Receive the workload of the round-robin yield test and report its statistics.
 */
static void Bench_YieldTest2(void)
{
    rt_uint32_t cnt;

    for (cnt = 0u; cnt < BENCH_TEST_ROUNDS; cnt++)
    {
        rt_thread_yield();

        /* Read the counter that closes the bracket around the yield, then
         * record the result of this round. */
        CntEnd = BENCH_TIME_GET();
        BENCH_STAT_RECORD();

        /* Verify that execution came from Bench_Task1 as expected. */
        if (Flip == RT_FALSE)
        {
            BENCH_PRINT_STRING("Yield error in task 2 @ round ");
            BENCH_PRINT_NUMBER(cnt);
            BENCH_PRINT_STRING(", ");
            BENCH_PRINT_NUMBER(CntStart);
            BENCH_PRINT_STRING(", ");
            BENCH_PRINT_NUMBER(CntEnd);
            BENCH_PRINT_STRING(".\r\n");
        }

        Flip = RT_FALSE;
    }
}

/*
 * MAILBOX TEST: GENERATE SIDE
 *
 * Generate the workload of the mailbox test - the RT-Thread counterpart of
 * the task notification test of the other benchmarks.
 */
static void Bench_MailTest1(void)
{
    rt_uint32_t cnt;

    for (cnt = 0u; cnt < BENCH_TEST_ROUNDS; cnt++)
    {
        CntStart = BENCH_TIME_GET();
        rt_mb_send(TestMb, (rt_ubase_t)MsgValue);
    }
}

/*
 * MAILBOX TEST: RECEIVE SIDE
 *
 * Receive the workload of the mailbox test and report its statistics.
 */
static void Bench_MailTest2(void)
{
    rt_uint32_t cnt;
    rt_ubase_t recv_value;

    for (cnt = 0u; cnt < BENCH_TEST_ROUNDS; cnt++)
    {
        rt_mb_recv(TestMb, &recv_value, RT_WAITING_FOREVER);

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
    rt_uint32_t cnt;

    for (cnt = 0u; cnt < BENCH_TEST_ROUNDS; cnt++)
    {
        CntStart = BENCH_TIME_GET();
        rt_sem_release(TestSem);
    }
}

/*
 * COUNTING SEMAPHORE TEST: RECEIVE SIDE
 *
 * Receive the workload of the counting semaphore test and report its statistics.
 */
static void Bench_SemTest2(void)
{
    rt_uint32_t cnt;

    for (cnt = 0u; cnt < BENCH_TEST_ROUNDS; cnt++)
    {
        rt_sem_take(TestSem, RT_WAITING_FOREVER);

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
    rt_uint32_t cnt;

    for (cnt = 0u; cnt < BENCH_TEST_ROUNDS; cnt++)
    {
        CntStart = BENCH_TIME_GET();
        rt_mq_send(TestMq, &MsgValue, sizeof(MsgValue));
    }
}

/*
 * MESSAGE QUEUE TEST: RECEIVE SIDE
 *
 * Receive the workload of the message queue test and report its statistics.
 */
static void Bench_QueueTest2(void)
{
    rt_uint32_t cnt;
    rt_uint32_t msg_value;

    for (cnt = 0u; cnt < BENCH_TEST_ROUNDS; cnt++)
    {
        rt_mq_recv(TestMq, &msg_value, sizeof(msg_value), RT_WAITING_FOREVER);

        CntEnd = BENCH_TIME_GET();
        BENCH_STAT_RECORD();
    }
}

/*
 * INTERRUPT DRIVEN MAILBOX TEST
 *
 * Receive the workload of the interrupt driven mailbox test and report its statistics.
 */
static void Bench_MailFromISRTest(void)
{
    rt_uint32_t cnt;
    rt_ubase_t recv_value;

    /* NOTE: no print here!  The interrupt source is already running when
     * this function is entered, and the debug print takes a few thousand
     * cycles - ten plus interrupt periods.  The mailbox can only buffer
     * eight mails, so every print spent here makes the sender drop mails
     * with -RT_EFULL before this thread reaches its first receive, and
     * the test then starves waiting for mails that were never delivered.
     * The uC/OS and FreeRTOS benchmarks enter their interrupt driven
     * phases silently for exactly this reason. */
    for (cnt = 0u; cnt < BENCH_TEST_ROUNDS; cnt++)
    {
        rt_mb_recv(TestMb, &recv_value, RT_WAITING_FOREVER);

        CntEnd = BENCH_TIME_GET();
        BENCH_STAT_RECORD();

        Flip = RT_FALSE;
    }
}

/*
 * INTERRUPT DRIVEN COUNTING SEMAPHORE TEST
 *
 * Receive the workload of the interrupt driven counting semaphore test and report its statistics.
 */
static void Bench_SemFromISRTest(void)
{
    rt_uint32_t cnt;

    for (cnt = 0u; cnt < BENCH_TEST_ROUNDS; cnt++)
    {
        rt_sem_take(TestSem, RT_WAITING_FOREVER);

        CntEnd = BENCH_TIME_GET();
        BENCH_STAT_RECORD();

        Flip = RT_FALSE;
    }
}

/*
 * INTERRUPT DRIVEN MESSAGE QUEUE TEST
 *
 * Receive the workload of the interrupt driven message queue test and report its statistics.
 */
static void Bench_QueueFromISRTest(void)
{
    rt_uint32_t cnt;
    rt_uint32_t msg_value;

    for (cnt = 0u; cnt < BENCH_TEST_ROUNDS; cnt++)
    {
        rt_mq_recv(TestMq, &msg_value, sizeof(msg_value), RT_WAITING_FOREVER);

        CntEnd = BENCH_TIME_GET();
        BENCH_STAT_RECORD();

        Flip = RT_FALSE;
    }
}

/* A small maximal length 16 bit LFSR that is used to randomise the order
 * in which the memory blocks are allocated and freed. */
/*
 * PSEUDO RANDOM NUMBER
 *
 * Return the next value of a maximal length 16 bit linear feedback shift register.
 */
static rt_uint32_t Bench_Rand(void)
{
    static rt_uint32_t RandState = 0xACE1u;
    rt_uint32_t rand_val;

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
static void Bench_Swap(rt_uint32_t *p_first, rt_uint32_t *p_second)
{
    rt_uint32_t temp;

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
    rt_uint32_t alloc_order[BENCH_MEM_BLK_QTY];
    rt_uint32_t free_order[BENCH_MEM_BLK_QTY];
    rt_uint32_t case_cnt;
    rt_uint32_t test_cnt;

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
        p_mem[alloc_order[0]] = rt_malloc(BENCH_MEM_BLK_SIZE);
        p_mem[alloc_order[1]] = rt_malloc(BENCH_MEM_BLK_SIZE);
        p_mem[alloc_order[2]] = rt_malloc(BENCH_MEM_BLK_SIZE);
        p_mem[alloc_order[3]] = rt_malloc(BENCH_MEM_BLK_SIZE);
        p_mem[alloc_order[4]] = rt_malloc(BENCH_MEM_BLK_SIZE);
        p_mem[alloc_order[5]] = rt_malloc(BENCH_MEM_BLK_SIZE);
        p_mem[alloc_order[6]] = rt_malloc(BENCH_MEM_BLK_SIZE);
        p_mem[alloc_order[7]] = rt_malloc(BENCH_MEM_BLK_SIZE);

        /* Free tests - in the randomised order. */
        rt_free(p_mem[free_order[0]]);
        rt_free(p_mem[free_order[1]]);
        rt_free(p_mem[free_order[2]]);
        rt_free(p_mem[free_order[3]]);
        rt_free(p_mem[free_order[4]]);
        rt_free(p_mem[free_order[5]]);
        rt_free(p_mem[free_order[6]]);
        rt_free(p_mem[free_order[7]]);

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
    static rt_uint32_t cnt = 0u;

    /* enter interrupt */
    rt_interrupt_enter();

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
    if (Flip != RT_FALSE)
    {
        ReenterCnt++;
    }

    Flip = RT_TRUE;

    /* The RT-Thread interrupt protocol: every kernel API is safe to call
     * from an interrupt handler, and the scheduler pass that the posts
     * trigger resolves to rt_hw_context_switch_interrupt() - which under
     * RVM only marks the context switch pending, because we are already
     * on the interrupt thread.  The actual switch happens when the
     * interrupt thread's dispatch loop calls PendSV_Handler() after this
     * handler returns. */
    if (cnt < BENCH_TEST_ROUNDS)
    {
        cnt++;
        CntStart = BENCH_TIME_GET();
        rt_mb_send(TestMb, (rt_ubase_t)MsgValue);
    }
    else if (cnt < (BENCH_TEST_ROUNDS * 2u))
    {
        cnt++;
        CntStart = BENCH_TIME_GET();
        rt_sem_release(TestSem);
    }
    else if (cnt < (BENCH_TEST_ROUNDS * 3u))
    {
        cnt++;
        CntStart = BENCH_TIME_GET();
        rt_mq_send(TestMq, &MsgValue, sizeof(MsgValue));
    }
    else
    {
        Bench_IntDisable();
    }

    /* leave interrupt */
    rt_interrupt_leave();
}

/*
 * BENCHMARK TASK 1
 *
 * Run the workload generating side of every benchmark test.
 */
static void Bench_Task1(void *p_arg)
{
    rt_uint8_t high_prio;

    (void)p_arg;

    BENCH_PRINT_STRING("Test (number in CPU cycles)        : AVG / MAX / MIN\r\n");

    /* Both tasks run at the same priority for the yield test.  Task 2 was
     * created first and thus owns the head of the ready list, and it hands
     * the CPU over to this task with the round-robin yield at the head of
     * its own loop - so by the time we get here the list rotation is in
     * its steady alternating phase and every yield of this task switches
     * straight to task 2, exactly like the FreeRTOS benchmark does with
     * its priority-drop handover. */
    Bench_YieldTest1();

    /* Raise the priority of task 2 so that it takes over and prints the
     * results of each test as it completes.  RT-Thread counts lower
     * priority numbers as higher priorities. */
    high_prio = BENCH_TASK_PRIO_HIGH;
    rt_thread_control(Task2Tid, RT_THREAD_CTRL_CHANGE_PRIORITY, &high_prio);

    Bench_MailTest1();
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
    rt_uint8_t high_prio;

    (void)p_arg;

    /* Yield tests. */
    BENCH_STAT_RESET();
    Bench_YieldTest2();
    BENCH_STAT_REPORT("Yield                             ");

    /* The remaining tests run with this task at the higher priority. */
    high_prio = BENCH_TASK_PRIO_HIGH;
    rt_thread_control(rt_thread_self(), RT_THREAD_CTRL_CHANGE_PRIORITY, &high_prio);

    /* Mailbox tests. */
    BENCH_STAT_RESET();
    Bench_MailTest2();
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

    /* Mailbox from interrupt tests. */
    BENCH_STAT_RESET();
    Bench_MailFromISRTest();
    MailISRTotal = Total;
    MailISRMax = Maximum;
    MailISRMin = Minimum;

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
    Total = MailISRTotal;
    Maximum = MailISRMax;
    Minimum = MailISRMin;
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
        BENCH_PRINT_STRING("rtt: NOTE: interrupt reentered ");
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
 * MAIN
 *
 * Boot the kernel on the first entry, then create the benchmark objects
 * and threads on the main thread - which returns right after, leaving the
 * head of the ready list to the benchmark threads.
 */
int main(void)
{
    /* The virtual machine enters through this function before the
     * scheduler runs; the second entry happens on the main thread, after
     * rtthread_startup() has switched to it. */
    if (rt_thread_self() == RT_NULL)
    {
        rtthread_startup();             /* never returns */
    }

    /* Set up the counter that is used to time the tests. */
    Bench_TimerSetup();

    /* Create the kernel objects that are used by the benchmark. */
    TestSem = rt_sem_create("tsem", 0u, RT_IPC_FLAG_FIFO);
    TestMb = rt_mb_create("tmb", BENCH_QUEUE_LENGTH, RT_IPC_FLAG_FIFO);
    TestMq = rt_mq_create("tmq", BENCH_QUEUE_ITEM_SIZE, BENCH_QUEUE_LENGTH, RT_IPC_FLAG_FIFO);

    BENCH_PRINT_STRING("\r\n");
    BENCH_PRINT_STRING("RT-Thread V4.1.1 on RVM - Standard benchmark test\r\n");
    BENCH_PRINT_STRING("====================================================\r\n");

    /* Create the two test threads inside a critical section: the scheduler
     * is already running when the main thread executes, and the startup of
     * the first thread must not preempt the creation of the second one -
     * both have to sit in the ready list before the rotation begins.
     * RT-Thread inserts the readied threads at the head of their priority
     * list, so thread 1 is created first to leave thread 2 at the head -
     * the scheduler starts thread 2, whose leading round-robin yield hands
     * the CPU over to thread 1, exactly like the other benchmarks. */
    rt_enter_critical();

    Task1Tid = rt_thread_create("test1", Bench_Task1, RT_NULL,
                                BENCH_TASK_STK_SIZE, BENCH_TASK_PRIO_NORM, 20);
    RT_ASSERT(Task1Tid != RT_NULL);
    rt_thread_startup(Task1Tid);

    Task2Tid = rt_thread_create("test2", Bench_Task2, RT_NULL,
                                BENCH_TASK_STK_SIZE, BENCH_TASK_PRIO_NORM, 20);
    RT_ASSERT(Task2Tid != RT_NULL);
    rt_thread_startup(Task2Tid);

    /* The main thread exits here; the benchmark threads take over. */
    rt_exit_critical();

    return 0;
}
