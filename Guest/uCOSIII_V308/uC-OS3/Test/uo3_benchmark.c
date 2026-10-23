/*
*********************************************************************************************************
*                                               uC/OS-III
*                                          The Real-Time Kernel
*
*                               Copyright (C) 2026 Evo-Devo Instrum.
*                                       All rights reserved.
*
*                This file is NOT part of the uC/OS-III distribution.  It belongs
*                to the uC/OS-III-on-RVM test suite and is released under the
*                Unlicense; refer to the LICENSE file for details.  The original
*                uC/OS-III and uC/CPU distributions are copyright Silicon
*                Laboratories Inc. and are not modified by this file.
*
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                         BENCHMARK APPLICATION
*                                          RVM Hypervisor Guest
*
* File      : uo3_benchmark.c
* Version   : V1.00.00
*********************************************************************************************************
*/

/*
*********************************************************************************************************
* Note(s)  : (1) This file measures the performance of the uC/OS-III kernel when it runs as a
*                guest of the RVM hypervisor.  All the hardware dependent code is held in the
*                chip specific test header that is selected by the generated "uo3_test.h" header.
*                The tests mirror the FreeRTOS-on-RVM benchmark one by one: the task notification
*                test uses the direct-to-task semaphores, the yield test uses the round-robin
*                yield with both tasks at the same priority, and the memory test uses the fixed
*                size partitions.
*********************************************************************************************************
*/

/* uC/OS-III includes. */
#include  "os.h"
#include  "rvm.h"

/* Benchmark includes. */
#include  "uo3_test.h"


/* The number of rounds each test is repeated for before its average,
 * maximum and minimum results are calculated. */
#ifndef BENCH_TEST_ROUNDS
    #define  BENCH_TEST_ROUNDS                      ((CPU_INT32U)10000u)
#endif

/* The number of counter overflows that are counted between two progress
 * reports that are printed once the benchmark itself has finished. */

#define  BENCH_OVERFLOW_REPORT_INTERVAL             ((CPU_INT32U)10000u)

/* The size of the stack allocated to each benchmark task, measured in
 * words rather than bytes.  This is port dependent, so every chip header
 * must define it - there is no fallback. */

#ifndef BENCH_TASK_STK_SIZE
#error  "BENCH_TASK_STK_SIZE must be defined by the chip specific test header"
#endif

/* The two priorities the benchmark tasks run at. */

#define  BENCH_TASK_PRIO_HIGH                       ((OS_PRIO)4u)

#define  BENCH_TASK_PRIO_NORM                       ((OS_PRIO)5u)


/* Reset the statistics that are accumulated by the BENCH_STAT_RECORD()
 * macro. */

#define  BENCH_STAT_RESET()                         \
do                                                    \
{                                                    \
    Total = 0u;                                    \
    Maximum = 0u;                                    \
    Minimum = (CPU_INT16U)0xFFFFu;                \
}                                                    \
while (0)

/* Accumulate the result of a single round into the current statistics.  The
 * counter difference is calculated using 16 bit arithmetic so that a single
 * overflow of the counter between the two reads is handled transparently. */

#define  BENCH_STAT_RECORD()                        \
do                                                    \
{                                                    \
    CntDiff = (CPU_INT16U)(CntEnd - CntStart);\
    Total += (CPU_INT32U)CntDiff;            \
    if (CntDiff > Maximum)                    \
    {                                                \
        Maximum = CntDiff;                    \
    }                                                \
    if (CntDiff < Minimum)                    \
    {                                                \
        Minimum = CntDiff;                    \
    }                                                \
}                                                    \
while (0)

/* Print the name of a test followed by its average, maximum and minimum
 * results, all of them expressed in CPU cycles. */

#define  BENCH_STAT_REPORT(pcTestName)              \
do                                                    \
{                                                    \
    BENCH_PRINT_STR(pcTestName);                        \
    BENCH_PRINT_STR(" : ");                            \
    BENCH_PRINT_INT(Total / BENCH_TEST_ROUNDS);        \
    BENCH_PRINT_STR(" / ");                            \
    BENCH_PRINT_INT(Maximum);                        \
    BENCH_PRINT_STR(" / ");                            \
    BENCH_PRINT_INT(Minimum);                        \
    BENCH_PRINT_STR("\r\n");                            \
}                                                    \
while (0)

#ifdef BENCH_KERNEL_TS_GET
/* Accumulate the RVM activation relay latency of a single interrupt into
 * the relay statistics.  The chip specific test header must provide the
 * kernel side interrupt timestamp for this to be available. */

#define  BENCH_RELAY_STAT_RECORD()              \
do                                                    \
{                                                    \
    CPU_INT16U relay_cnt;                            \
    relay_cnt = (CPU_INT16U)BENCH_TIME_GET();\
    relay_cnt = (CPU_INT16U)(relay_cnt - BENCH_KERNEL_TS_GET());\
    RelayTotal += (CPU_INT32U)relay_cnt;\
    if (relay_cnt > RelayMax)        \
    {                                                \
        RelayMax = relay_cnt;        \
    }                                                \
    if (relay_cnt < RelayMin)        \
    {                                                \
        RelayMin = relay_cnt;        \
    }                                                \
}                                                    \
while (0)
#endif


/* The two tasks that run the benchmark.  Bench_Task1 starts the tests and
 * generates the workload while Bench_Task2 receives the workload and
 * prints the results as they become available. */
/*
*********************************************************************************************************
*                                     BENCHMARK TASK 1
*
* Description: Run the workload generating side of every benchmark test.
*
* Arguments  : see the source.
*
* Returns    : none.
*********************************************************************************************************
*/

static void  Bench_Task1 (void *p_arg);
/*
*********************************************************************************************************
*                                     BENCHMARK TASK 2
*
* Description: Run the workload receiving side of every benchmark test, and print the results as they become available.
*
* Arguments  : see the source.
*
* Returns    : none.
*********************************************************************************************************
*/

static void  Bench_Task2 (void *p_arg);

/* One function per benchmark.  The functions that end in a "1" are called
 * from Bench_Task1 and generate the workload, their counterparts that end
 * in a "2" are called from Bench_Task2 and receive the workload. */
/*
*********************************************************************************************************
*                          ROUND-ROBIN YIELD TEST: GENERATE SIDE
*
* Description: Generate the workload of the round-robin yield test.
*
* Arguments  : none.
*
* Returns    : none.
*********************************************************************************************************
*/

static void  Bench_YieldTest1 (void);
/*
*********************************************************************************************************
*                           ROUND-ROBIN YIELD TEST: RECEIVE SIDE
*
* Description: Receive the workload of the round-robin yield test and report its statistics.
*
* Arguments  : none.
*
* Returns    : none.
*********************************************************************************************************
*/

static void  Bench_YieldTest2 (void);
/*
*********************************************************************************************************
*                       DIRECT-TO-TASK SEMAPHORE TEST: GENERATE SIDE
*
* Description: Generate the workload of the direct-to-task semaphore test.
*
* Arguments  : none.
*
* Returns    : none.
*********************************************************************************************************
*/

static void  Bench_TaskSemTest1 (void);
/*
*********************************************************************************************************
*                        DIRECT-TO-TASK SEMAPHORE TEST: RECEIVE SIDE
*
* Description: Receive the workload of the direct-to-task semaphore test and report its statistics.
*
* Arguments  : none.
*
* Returns    : none.
*********************************************************************************************************
*/

static void  Bench_TaskSemTest2 (void);
/*
*********************************************************************************************************
*                          COUNTING SEMAPHORE TEST: GENERATE SIDE
*
* Description: Generate the workload of the counting semaphore test.
*
* Arguments  : none.
*
* Returns    : none.
*********************************************************************************************************
*/

static void  Bench_SemTest1 (void);
/*
*********************************************************************************************************
*                          COUNTING SEMAPHORE TEST: RECEIVE SIDE
*
* Description: Receive the workload of the counting semaphore test and report its statistics.
*
* Arguments  : none.
*
* Returns    : none.
*********************************************************************************************************
*/

static void  Bench_SemTest2 (void);
/*
*********************************************************************************************************
*                            MESSAGE QUEUE TEST: GENERATE SIDE
*
* Description: Generate the workload of the message queue test.
*
* Arguments  : none.
*
* Returns    : none.
*********************************************************************************************************
*/

static void  Bench_QueueTest1 (void);
/*
*********************************************************************************************************
*                             MESSAGE QUEUE TEST: RECEIVE SIDE
*
* Description: Receive the workload of the message queue test and report its statistics.
*
* Arguments  : none.
*
* Returns    : none.
*********************************************************************************************************
*/

static void  Bench_QueueTest2 (void);
/*
*********************************************************************************************************
*                      INTERRUPT DRIVEN DIRECT-TO-TASK SEMAPHORE TEST
*
* Description: Receive the workload of the interrupt driven direct-to-task semaphore test and report its statistics.
*
* Arguments  : none.
*
* Returns    : none.
*********************************************************************************************************
*/

static void  Bench_TaskSemFromISRTest (void);
/*
*********************************************************************************************************
*                         INTERRUPT DRIVEN COUNTING SEMAPHORE TEST
*
* Description: Receive the workload of the interrupt driven counting semaphore test and report its statistics.
*
* Arguments  : none.
*
* Returns    : none.
*********************************************************************************************************
*/

static void  Bench_SemFromISRTest (void);
/*
*********************************************************************************************************
*                            INTERRUPT DRIVEN MESSAGE QUEUE TEST
*
* Description: Receive the workload of the interrupt driven message queue test and report its statistics.
*
* Arguments  : none.
*
* Returns    : none.
*********************************************************************************************************
*/

static void  Bench_QueueFromISRTest (void);

#ifdef BENCH_MEM_BLK_QTY
/*
*********************************************************************************************************
*                             FIXED SIZE MEMORY PARTITION TEST
*
* Description: Exercise the fixed size memory partition with randomised get and put orders, then report the statistics.
*
* Arguments  : none.
*
* Returns    : none.
*********************************************************************************************************
*/

    static void  Bench_MemTest (void);
/*
*********************************************************************************************************
*                                   PSEUDO RANDOM NUMBER
*
* Description: Return the next value of a maximal length 16 bit linear feedback shift register.
*
* Arguments  : none.
*
* Returns    : none.
*********************************************************************************************************
*/

    static CPU_INT32U  Bench_Rand (void);
/*
*********************************************************************************************************
*                                EXCHANGE TWO 32 BIT VALUES
*
* Description: Exchange the two 32 bit values that the arguments point to.
*
* Arguments  : see the source.
*
* Returns    : none.
*********************************************************************************************************
*/

    static void  Bench_Swap (CPU_INT32U *p_first, CPU_INT32U *p_second);
#endif

/* The flag that is used to verify that execution ping-pongs between the
 * two test tasks, and between the tasks and the interrupt handler, exactly
 * as expected. */
static volatile CPU_BOOLEAN  Flip = DEF_FALSE;

/* The counter values that bracket the operation under test, and the
 * statistics that are accumulated from the difference between them. */
static volatile CPU_INT16U CntStart = 0u;
static volatile CPU_INT16U CntEnd = 0u;
static volatile CPU_INT16U CntDiff = 0u;
static volatile CPU_INT16U Minimum = 0u;
static volatile CPU_INT16U Maximum = 0u;
static volatile CPU_INT32U Total = 0u;
static volatile CPU_INT32U OverflowCnt = 0u;

/* The statistics of the interrupt driven tests are accumulated while the
 * receiving task waits, so they are parked here between the tests. */
#ifdef BENCH_KERNEL_TS_GET
    static volatile CPU_INT32U RelayTotal = 0u;
    static volatile CPU_INT16U RelayMax = 0u;
    static volatile CPU_INT16U RelayMin = 0xFFFFu;
#endif
static volatile CPU_INT32U NotifyISRTotal = 0u;
static volatile CPU_INT16U NotifyISRMax = 0u;
static volatile CPU_INT16U NotifyISRMin = 0u;
static volatile CPU_INT32U SemISRTotal = 0u;
static volatile CPU_INT16U SemISRMax = 0u;
static volatile CPU_INT16U SemISRMin = 0u;
static volatile CPU_INT32U QueueISRTotal = 0u;
static volatile CPU_INT16U QueueISRMax = 0u;
static volatile CPU_INT16U QueueISRMin = 0u;

/* The kernel objects that are used by the benchmark. */
static OS_TCB    Task1TCB;
static OS_TCB    Task2TCB;
static CPU_STK   Task1Stk[ BENCH_TASK_STK_SIZE ];
static CPU_STK   Task2Stk[ BENCH_TASK_STK_SIZE ];
static OS_SEM    TestSem;
static OS_Q      TestQ;
static CPU_INT32U MsgValue = 1u;
#ifdef BENCH_MEM_BLK_QTY
    static OS_MEM      TestMemPart;
    static CPU_INT08U  MemPool[ BENCH_MEM_BLK_QTY ][ BENCH_MEM_BLK_SIZE ];
#endif


/*
*********************************************************************************************************
*                          ROUND-ROBIN YIELD TEST: GENERATE SIDE
*
* Description: Generate the workload of the round-robin yield test.
*
* Arguments  : none.
*
* Returns    : none.
*********************************************************************************************************
*/

static void  Bench_YieldTest1 (void)
{
CPU_INT32U cnt;
OS_ERR  os_err;

    for (cnt = 0u; cnt < BENCH_TEST_ROUNDS; cnt++)
    {
        /* Verify that execution returned from Bench_Task2 as expected. */
        if (Flip != DEF_FALSE)
        {
            BENCH_PRINT_STR("Yield error in task 1 @ round ");
            BENCH_PRINT_INT(cnt);
            BENCH_PRINT_STR(".\r\n");
        }

        Flip = DEF_TRUE;

        /* Read the counter that opens the bracket around the yield. */
        CntStart = BENCH_TIME_GET();

        OSSchedRoundRobinYield(&os_err);
    }
}

/*
*********************************************************************************************************
*                           ROUND-ROBIN YIELD TEST: RECEIVE SIDE
*
* Description: Receive the workload of the round-robin yield test and report its statistics.
*
* Arguments  : none.
*
* Returns    : none.
*********************************************************************************************************
*/

static void  Bench_YieldTest2 (void)
{
CPU_INT32U cnt;
OS_ERR  os_err;

    for (cnt = 0u; cnt < BENCH_TEST_ROUNDS; cnt++)
    {
        OSSchedRoundRobinYield(&os_err);

        /* Read the counter that closes the bracket around the yield, then
         * record the result of this round. */
        CntEnd = BENCH_TIME_GET();
        BENCH_STAT_RECORD();

        /* Verify that execution came from Bench_Task1 as expected. */
        if (Flip == DEF_FALSE)
        {
            BENCH_PRINT_STR("Yield error in task 2 @ round ");
            BENCH_PRINT_INT(cnt);
            BENCH_PRINT_STR(", ");
            BENCH_PRINT_INT(CntStart);
            BENCH_PRINT_STR(", ");
            BENCH_PRINT_INT(CntEnd);
            BENCH_PRINT_STR(".\r\n");
        }

        Flip = DEF_FALSE;
    }
}

/*
*********************************************************************************************************
*                       DIRECT-TO-TASK SEMAPHORE TEST: GENERATE SIDE
*
* Description: Generate the workload of the direct-to-task semaphore test.
*
* Arguments  : none.
*
* Returns    : none.
*********************************************************************************************************
*/

static void  Bench_TaskSemTest1 (void)
{
CPU_INT32U cnt;
OS_ERR  os_err;

    for (cnt = 0u; cnt < BENCH_TEST_ROUNDS; cnt++)
    {
        CntStart = BENCH_TIME_GET();
        OSTaskSemPost(&Task2TCB, OS_OPT_POST_NONE, &os_err);
    }
}

/*
*********************************************************************************************************
*                        DIRECT-TO-TASK SEMAPHORE TEST: RECEIVE SIDE
*
* Description: Receive the workload of the direct-to-task semaphore test and report its statistics.
*
* Arguments  : none.
*
* Returns    : none.
*********************************************************************************************************
*/

static void  Bench_TaskSemTest2 (void)
{
CPU_INT32U cnt;
CPU_TS  ts;
OS_ERR  os_err;

    for (cnt = 0u; cnt < BENCH_TEST_ROUNDS; cnt++)
    {
        OSTaskSemPend(0u, OS_OPT_PEND_BLOCKING, &ts, &os_err);

        CntEnd = BENCH_TIME_GET();
        BENCH_STAT_RECORD();
    }
}

/*
*********************************************************************************************************
*                          COUNTING SEMAPHORE TEST: GENERATE SIDE
*
* Description: Generate the workload of the counting semaphore test.
*
* Arguments  : none.
*
* Returns    : none.
*********************************************************************************************************
*/

static void  Bench_SemTest1 (void)
{
CPU_INT32U cnt;
OS_ERR  os_err;

    for (cnt = 0u; cnt < BENCH_TEST_ROUNDS; cnt++)
    {
        CntStart = BENCH_TIME_GET();
        OSSemPost(&TestSem, OS_OPT_POST_1, &os_err);
    }
}

/*
*********************************************************************************************************
*                          COUNTING SEMAPHORE TEST: RECEIVE SIDE
*
* Description: Receive the workload of the counting semaphore test and report its statistics.
*
* Arguments  : none.
*
* Returns    : none.
*********************************************************************************************************
*/

static void  Bench_SemTest2 (void)
{
CPU_INT32U cnt;
CPU_TS  ts;
OS_ERR  os_err;

    for (cnt = 0u; cnt < BENCH_TEST_ROUNDS; cnt++)
    {
        OSSemPend(&TestSem, 0u, OS_OPT_PEND_BLOCKING, &ts, &os_err);

        CntEnd = BENCH_TIME_GET();
        BENCH_STAT_RECORD();
    }
}

/*
*********************************************************************************************************
*                            MESSAGE QUEUE TEST: GENERATE SIDE
*
* Description: Generate the workload of the message queue test.
*
* Arguments  : none.
*
* Returns    : none.
*********************************************************************************************************
*/

static void  Bench_QueueTest1 (void)
{
CPU_INT32U cnt;
OS_ERR  os_err;

    for (cnt = 0u; cnt < BENCH_TEST_ROUNDS; cnt++)
    {
        CntStart = BENCH_TIME_GET();
        OSQPost(&TestQ, (void *)&MsgValue, sizeof(MsgValue), OS_OPT_POST_FIFO, &os_err);
    }
}

/*
*********************************************************************************************************
*                             MESSAGE QUEUE TEST: RECEIVE SIDE
*
* Description: Receive the workload of the message queue test and report its statistics.
*
* Arguments  : none.
*
* Returns    : none.
*********************************************************************************************************
*/

static void  Bench_QueueTest2 (void)
{
CPU_INT32U cnt;
OS_MSG_SIZE  msg_size;
CPU_TS  ts;
OS_ERR  os_err;

    for (cnt = 0u; cnt < BENCH_TEST_ROUNDS; cnt++)
    {
        OSQPend(&TestQ, 0u, OS_OPT_PEND_BLOCKING, &msg_size, &ts, &os_err);

        CntEnd = BENCH_TIME_GET();
        BENCH_STAT_RECORD();
    }
}

/*
*********************************************************************************************************
*                      INTERRUPT DRIVEN DIRECT-TO-TASK SEMAPHORE TEST
*
* Description: Receive the workload of the interrupt driven direct-to-task semaphore test and report its statistics.
*
* Arguments  : none.
*
* Returns    : none.
*********************************************************************************************************
*/

static void  Bench_TaskSemFromISRTest (void)
{
CPU_INT32U cnt;
CPU_TS  ts;
OS_ERR  os_err;

    for (cnt = 0u; cnt < BENCH_TEST_ROUNDS; cnt++)
    {
        OSTaskSemPend(0u, OS_OPT_PEND_BLOCKING, &ts, &os_err);

        CntEnd = BENCH_TIME_GET();
        BENCH_STAT_RECORD();

        Flip = DEF_FALSE;
    }
}

/*
*********************************************************************************************************
*                         INTERRUPT DRIVEN COUNTING SEMAPHORE TEST
*
* Description: Receive the workload of the interrupt driven counting semaphore test and report its statistics.
*
* Arguments  : none.
*
* Returns    : none.
*********************************************************************************************************
*/

static void  Bench_SemFromISRTest (void)
{
CPU_INT32U cnt;
CPU_TS  ts;
OS_ERR  os_err;

    for (cnt = 0u; cnt < BENCH_TEST_ROUNDS; cnt++)
    {
        OSSemPend(&TestSem, 0u, OS_OPT_PEND_BLOCKING, &ts, &os_err);

        CntEnd = BENCH_TIME_GET();
        BENCH_STAT_RECORD();

        Flip = DEF_FALSE;
    }
}

/*
*********************************************************************************************************
*                            INTERRUPT DRIVEN MESSAGE QUEUE TEST
*
* Description: Receive the workload of the interrupt driven message queue test and report its statistics.
*
* Arguments  : none.
*
* Returns    : none.
*********************************************************************************************************
*/

static void  Bench_QueueFromISRTest (void)
{
CPU_INT32U cnt;
OS_MSG_SIZE  msg_size;
CPU_TS  ts;
OS_ERR  os_err;

    for (cnt = 0u; cnt < BENCH_TEST_ROUNDS; cnt++)
    {
        OSQPend(&TestQ, 0u, OS_OPT_PEND_BLOCKING, &msg_size, &ts, &os_err);

        CntEnd = BENCH_TIME_GET();
        BENCH_STAT_RECORD();

        Flip = DEF_FALSE;
    }
}

#ifdef BENCH_MEM_BLK_QTY

/* A small maximal length 16 bit LFSR that is used to randomise the order
 * in which the partition blocks are got and put back. */
/*
*********************************************************************************************************
*                                   PSEUDO RANDOM NUMBER
*
* Description: Return the next value of a maximal length 16 bit linear feedback shift register.
*
* Arguments  : none.
*
* Returns    : The next pseudo random number.
*********************************************************************************************************
*/

static CPU_INT32U  Bench_Rand (void)
{
static CPU_INT32U RandState = 0xACE1u;
CPU_INT32U rand_val;

    RandState ^= RandState << 13u;
    RandState ^= RandState >> 9u;
    RandState ^= RandState << 7u;
    rand_val = RandState;

    return rand_val;
}

/*
*********************************************************************************************************
*                                EXCHANGE TWO 32 BIT VALUES
*
* Description: Exchange the two 32 bit values that the arguments point to.
*
* Arguments  : see the source.
*
* Returns    : none.
*********************************************************************************************************
*/

static void  Bench_Swap (CPU_INT32U *p_first, CPU_INT32U *p_second)
{
CPU_INT32U temp;

    temp = *p_first;
    *p_first = *p_second;
    *p_second = temp;
}

/*
*********************************************************************************************************
*                             FIXED SIZE MEMORY PARTITION TEST
*
* Description: Exercise the fixed size memory partition with randomised get and put orders, then report the statistics.
*
* Arguments  : none.
*
* Returns    : none.
*********************************************************************************************************
*/

static void  Bench_MemTest (void)
{
void *p_mem[ 8 ];
CPU_INT32U alloc_order[ 8 ];
CPU_INT32U free_order[ 8 ];
CPU_INT32U case_cnt;
CPU_INT32U test_cnt;
OS_ERR os_err;

    for (test_cnt = 0u; test_cnt < BENCH_TEST_ROUNDS; test_cnt++)
    {
        /* Generate a random get order and put order. */
        for (case_cnt = 0u; case_cnt < 8u; case_cnt++)
        {
            alloc_order[ case_cnt ] = case_cnt;
            free_order[ case_cnt ] = case_cnt;
        }

        for (case_cnt = 7u; case_cnt > 0u; case_cnt--)
        {
            Bench_Swap(&alloc_order[ case_cnt ], &alloc_order[ Bench_Rand() % (case_cnt + 1u) ]);
            Bench_Swap(&free_order[ case_cnt ], &free_order[ Bench_Rand() % (case_cnt + 1u) ]);
        }

        CntStart = BENCH_TIME_GET();

        /* Partition get tests - all of them must succeed, the partition
         * always has all of its blocks free at this point. */
        p_mem[ alloc_order[ 0 ] ] = OSMemGet(&TestMemPart, &os_err);
        p_mem[ alloc_order[ 1 ] ] = OSMemGet(&TestMemPart, &os_err);
        p_mem[ alloc_order[ 2 ] ] = OSMemGet(&TestMemPart, &os_err);
        p_mem[ alloc_order[ 3 ] ] = OSMemGet(&TestMemPart, &os_err);
        p_mem[ alloc_order[ 4 ] ] = OSMemGet(&TestMemPart, &os_err);
        p_mem[ alloc_order[ 5 ] ] = OSMemGet(&TestMemPart, &os_err);
        p_mem[ alloc_order[ 6 ] ] = OSMemGet(&TestMemPart, &os_err);
        p_mem[ alloc_order[ 7 ] ] = OSMemGet(&TestMemPart, &os_err);
        if ((p_mem[ 0 ] == (void *)0) || (p_mem[ 1 ] == (void *)0) ||
            (p_mem[ 2 ] == (void *)0) || (p_mem[ 3 ] == (void *)0) ||
            (p_mem[ 4 ] == (void *)0) || (p_mem[ 5 ] == (void *)0) ||
            (p_mem[ 6 ] == (void *)0) || (p_mem[ 7 ] == (void *)0))
        {
            BENCH_PRINT_STR("Failure.\r\n");
        }

        /* Partition put tests. */
        OSMemPut(&TestMemPart, p_mem[ free_order[ 0 ] ], &os_err);
        OSMemPut(&TestMemPart, p_mem[ free_order[ 1 ] ], &os_err);
        OSMemPut(&TestMemPart, p_mem[ free_order[ 2 ] ], &os_err);
        OSMemPut(&TestMemPart, p_mem[ free_order[ 3 ] ], &os_err);
        OSMemPut(&TestMemPart, p_mem[ free_order[ 4 ] ], &os_err);
        OSMemPut(&TestMemPart, p_mem[ free_order[ 5 ] ], &os_err);
        OSMemPut(&TestMemPart, p_mem[ free_order[ 6 ] ], &os_err);
        OSMemPut(&TestMemPart, p_mem[ free_order[ 7 ] ], &os_err);

        CntEnd = BENCH_TIME_GET();
        BENCH_STAT_RECORD();
    }

    /* The statistics that were accumulated above cover eight get/put pairs
     * per round, so scale them down accordingly. */
    Total /= 8u;
    Maximum /= 8u;
    Minimum /= 8u;
}

#endif /* BENCH_MEM_BLK_QTY */

/*
 * The interrupt handler that generates the workload for the interrupt
 * driven tests.  The chip specific test header registers this handler with
 * the RVM hypervisor, and the RVM kernel calls it once the physical
 * interrupt has been delivered to this virtual machine.  The posts are
 * made with the no-schedule option, and the switch to the receiving task
 * is requested through the RVM virtual yield, exactly like the FreeRTOS
 * interrupt path of this benchmark family.
 */
/*
*********************************************************************************************************
*                              PERIODIC TEST INTERRUPT HANDLER
*
* Description: Service the periodic test interrupt: latch the start count and post the workload of the interrupt driven test that is currently running.
*
* Arguments  : none.
*
* Returns    : none.
*********************************************************************************************************
*/

void Bench_TestIntHandler(void)
{
static CPU_INT32U cnt = 0u;
OS_ERR  os_err;

    /* Measure the RVM activation relay latency while the first test phase
     * is running.  The chip specific test header provides the timestamp
     * that the kernel latched when it took this very interrupt. */
#ifdef BENCH_KERNEL_TS_GET
    if (cnt < BENCH_TEST_ROUNDS)
    {
        BENCH_RELAY_STAT_RECORD();
    }
#endif

    /* Detect interrupt reentry. */
    if (Flip != DEF_FALSE)
    {
        BENCH_PRINT_STR("Interrupt reentered.\r\n");
    }

    Flip = DEF_TRUE;

    /* The uC/OS-III interrupt protocol: the nesting counter is bumped by
     * OSIntEnter(), the posts are made without a schedule, and OSIntExit()
     * performs the scheduler pass that selects the receiving task and requests
     * the context switch through OSIntCtxSw() - which under RVM only marks the
     * context switch flag, because we are already on the interrupt thread.
     * The actual switch happens when the interrupt thread's dispatch loop
     * calls OS_Ctx_Handler() after this handler returns. */
    if (cnt < BENCH_TEST_ROUNDS)
    {
        cnt++;
        CntStart = BENCH_TIME_GET();
        OSIntEnter();
        OSTaskSemPost(&Task2TCB, OS_OPT_POST_NO_SCHED, &os_err);
        OSIntExit();
    }
    else if (cnt < (BENCH_TEST_ROUNDS * 2u))
    {
        cnt++;
        CntStart = BENCH_TIME_GET();
        OSIntEnter();
        OSSemPost(&TestSem, OS_OPT_POST_NO_SCHED, &os_err);
        OSIntExit();
    }
    else if (cnt < (BENCH_TEST_ROUNDS * 3u))
    {
        cnt++;
        CntStart = BENCH_TIME_GET();
        OSIntEnter();
        OSQPost(&TestQ, (void *)&MsgValue, sizeof(MsgValue), OS_OPT_POST_FIFO | OS_OPT_POST_NO_SCHED, &os_err);
        OSIntExit();
    }
    else
    {
        Bench_TestIntDisable();
    }
}

/*
*********************************************************************************************************
*                                     BENCHMARK TASK 1
*
* Description: Run the workload generating side of every benchmark test.
*
* Arguments  : see the source.
*
* Returns    : none.
*********************************************************************************************************
*/

static void  Bench_Task1 (void *p_arg)
{
OS_ERR  os_err;

    (void)p_arg;

    BENCH_PRINT_STR("Test (number in CPU cycles)        : AVG / MAX / MIN\r\n");

    /* Both tasks run at the same priority for the yield test.  Task 2 was
     * created first and thus owns the head of the ready list, and it hands
     * the CPU over to this task with the round-robin yield at the head of
     * its own loop - so by the time we get here the list rotation is in
     * its steady alternating phase and every yield of this task switches
     * straight to task 2, exactly like the FreeRTOS benchmark does with
     * its priority-drop handover. */
    Bench_YieldTest1();

    /* Raise the priority of task 2 so that it takes over and prints the
     * results of each test as it completes.  uC/OS-III counts lower
     * priority numbers as higher priorities. */
    OSTaskChangePrio(&Task2TCB, BENCH_TASK_PRIO_HIGH, &os_err);

    Bench_TaskSemTest1();
    Bench_SemTest1();
    Bench_QueueTest1();

    for (;;);
}

/*
*********************************************************************************************************
*                                     BENCHMARK TASK 2
*
* Description: Run the workload receiving side of every benchmark test, and print the results as they become available.
*
* Arguments  : see the source.
*
* Returns    : none.
*********************************************************************************************************
*/

static void  Bench_Task2 (void *p_arg)
{
OS_ERR  os_err;

    (void)p_arg;

    /* Yield tests. */
    BENCH_STAT_RESET();
    Bench_YieldTest2();
    BENCH_STAT_REPORT("Yield                             ");

    /* The remaining tests run with this task at the higher priority. */
    OSTaskChangePrio((OS_TCB *)0, BENCH_TASK_PRIO_HIGH, &os_err);

    /* Direct-to-task semaphore tests. */
    BENCH_STAT_RESET();
    Bench_TaskSemTest2();
    BENCH_STAT_REPORT("Notification                      ");

    /* Semaphore tests. */
    BENCH_STAT_RESET();
    Bench_SemTest2();
    BENCH_STAT_REPORT("Semaphore                         ");

    /* Blocking message queue tests. */
    BENCH_STAT_RESET();
    Bench_QueueTest2();
    BENCH_STAT_REPORT("Message queue                     ");

#ifdef BENCH_MEM_BLK_QTY
    /* Memory partition get/put pair tests. */
    BENCH_STAT_RESET();
    Bench_MemTest();
    BENCH_STAT_REPORT("Memory allocation/free pair       ");
#endif

    /* Reset the relay statistics that are accumulated by the interrupt
     * handler itself. */
#ifdef BENCH_KERNEL_TS_GET
    RelayTotal = 0u;
    RelayMax = 0u;
    RelayMin = 0xFFFFu;
#endif

    /* Prepare the interrupt driven tests. */
    Bench_TestIntSetup();

    /* Task semaphore from interrupt tests. */
    BENCH_STAT_RESET();
    Bench_TaskSemFromISRTest();
    NotifyISRTotal = Total;
    NotifyISRMax = Maximum;
    NotifyISRMin = Minimum;

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
    Total = NotifyISRTotal;
    Maximum = (CPU_INT16U)NotifyISRMax;
    Minimum = (CPU_INT16U)NotifyISRMin;
    BENCH_STAT_REPORT("ISR Notification                  ");

    Total = SemISRTotal;
    Maximum = (CPU_INT16U)SemISRMax;
    Minimum = (CPU_INT16U)SemISRMin;
    BENCH_STAT_REPORT("ISR Semaphore                     ");

    Total = QueueISRTotal;
    Maximum = (CPU_INT16U)QueueISRMax;
    Minimum = (CPU_INT16U)QueueISRMin;
    BENCH_STAT_REPORT("ISR Message queue                 ");

#ifdef BENCH_KERNEL_TS_GET
    Total = RelayTotal;
    Maximum = RelayMax;
    Minimum = RelayMin;
    BENCH_STAT_REPORT("ISR RVM activation relay          ");
#endif

    /* The benchmark itself is done.  Keep dumping the counter value so
     * that an incorrectly configured counter clock rate can be detected. */
    for (;;)
    {
        CntStart = CntEnd;
        CntEnd = (CPU_INT16U)BENCH_TIME_GET();
        if (CntStart > CntEnd)
        {
            OverflowCnt++;
            if ((OverflowCnt % BENCH_OVERFLOW_REPORT_INTERVAL) == 0u)
            {
                BENCH_PRINT_INT(BENCH_OVERFLOW_REPORT_INTERVAL);
                BENCH_PRINT_STR(" overflows\r\n");
            }
        }
    }
}

/*
*********************************************************************************************************
*                                           MAIN
*
* Description: Create the benchmark objects and tasks, then start the kernel.
*
* Arguments  : none.
*
* Returns    : none.
*********************************************************************************************************
*/

int main(void)
{
OS_ERR  os_err;

    /* Set up the counter that is used to time the tests. */
    Bench_TimerSetup();

    /* Initialize the kernel. */
    OSInit(&os_err);

    /* Keep the natural round-robin rotation off - the time quanta is set
     * far beyond anything the tick can reach, so that the tasks only
     * rotate through the explicit OSSchedRoundRobinYield() calls of the
     * yield test. */
    OSSchedRoundRobinCfg(DEF_ENABLED, (CPU_INT32U)0x00FFFFFFu, &os_err);

    /* Create the kernel objects that are used by the benchmark. */
    OSSemCreate(&TestSem, "Test Sem", 0u, &os_err);
    OSQCreate(&TestQ, "Test Q", BENCH_QUEUE_LENGTH, &os_err);
#ifdef BENCH_MEM_BLK_QTY
    OSMemCreate(&TestMemPart, "Test Mem", &MemPool[ 0 ][ 0 ], BENCH_MEM_BLK_QTY, BENCH_MEM_BLK_SIZE, &os_err);
#endif

    BENCH_PRINT_STR("\r\n");
    BENCH_PRINT_STR("uC/OS-III V3.08 on RVM - Standard benchmark test\r\n");
    BENCH_PRINT_STR("====================================================\r\n");

    /* Create the two test tasks at the same priority.  Task 2 is created
     * first and therefore owns the head of the ready list, so the scheduler
     * starts task 2, whose leading round-robin yield hands the CPU over to
     * task 1 - the same first-handover that the FreeRTOS benchmark gets
     * from its priority drop.  From then on both tasks strictly alternate
     * through their yields. */
    OSTaskCreate(&Task2TCB, "Test2", Bench_Task2, (void *)0x4321u, BENCH_TASK_PRIO_NORM,
                  &Task2Stk[ 0 ], BENCH_TASK_STK_SIZE / 10u, BENCH_TASK_STK_SIZE,
                  (OS_MSG_QTY)0u, (OS_TICK)0u, (void *)0u, OS_OPT_TASK_NONE, &os_err);
    OSTaskCreate(&Task1TCB, "Test1", Bench_Task1, (void *)0x1234u, BENCH_TASK_PRIO_NORM,
                  &Task1Stk[ 0 ], BENCH_TASK_STK_SIZE / 10u, BENCH_TASK_STK_SIZE,
                  (OS_MSG_QTY)0u, (OS_TICK)0u, (void *)0u, OS_OPT_TASK_NONE, &os_err);

    /* Start the uC/OS-III scheduler. */
    OSStart(&os_err);

    for (;;);
}
