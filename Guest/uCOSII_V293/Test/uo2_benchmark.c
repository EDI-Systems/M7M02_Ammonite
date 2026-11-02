/*
*********************************************************************************************************
*                                                uC/OS-II
*                                          The Real-Time Kernel
*
*                               Copyright (C) 2026 Evo-Devo Instrum.
*                                       All rights reserved.
*
*                This file is NOT part of the uC/OS-II distribution.  It belongs
*                to the uC/OS-II-on-RVM test suite and is released under the
*                Unlicense; refer to the LICENSE file for details.  The original
*                uC/OS-II kernel is copyright Weston Embedded and is not
*                modified by this file.
*
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                       uo2_benchmark.c
*
* File      : uo2_benchmark.c
* Version   : V1.00.00
*********************************************************************************************************
*/

/*
*********************************************************************************************************
* Note(s)  : (1) This file measures the performance of the uC/OS-II kernel when it runs as a
*                guest of the RVM hypervisor.  All the hardware dependent code is held in the
*                chip specific test header that is selected by the generated "uo2_test.h"
*                header, so this file does not have to be modified when the benchmark is ported
*                to a new chip.
*
*            (2) uC/OS-II enforces one task per priority, so the yield test of the other
*                benchmarks is not measured here (the kernel's OS_Sched always picks the
*                highest ready task, there is no same-priority round-robin to exercise).
*                The yield row in the README tables is left empty for this guest.
*
*            (3) The notification test uses the one-word mailbox (OSMboxPost/OSMboxPend),
*                uC/OS-II's closest counterpart of a direct task notification.
*
*            (4) The ISR-driven tests post their workloads from the interrupt handler and the
*                switch is requested through OSIntCtxSw(), which under RVM only marks the
*                context switch flag - the actual switch happens when the interrupt thread's
*                dispatch loop calls OS_Ctx_Handler() after the handler returns.
*********************************************************************************************************
*/

#include  <ucos_ii.h>
#include  "rvm.h"
#include  "uo2_test.h"

/*
*********************************************************************************************************
*                                              CONSTANTS
*********************************************************************************************************
*/

#ifndef BENCH_TEST_ROUNDS
#define  BENCH_TEST_ROUNDS                      ((INT32U)10000u)
#endif

#define  BENCH_OVERFLOW_REPORT_INTERVAL         ((INT32U)10000u)

#ifndef BENCH_TASK_STK_SIZE
#error   "BENCH_TASK_STK_SIZE must be defined by the chip specific test header"
#endif

/* Task priorities - uC/OS-II counts lower numbers as higher priorities.
 * Task1 generates the workload and must sit at the LOWER priority so that
 * posting to a kernel object switches to the receiver; Task2 receives the
 * workload, records the statistics and prints the results, and must sit at
 * the HIGHER priority to preempt the generator as soon as data arrives. */
#define  BENCH_TASK1_PRIO                       5u
#define  BENCH_TASK2_PRIO                       4u

/*
*********************************************************************************************************
*                                        MACROS AND DEFINES
*********************************************************************************************************
*/

#define  BENCH_STAT_RESET()                                                              \
do                                                                                       \
{                                                                                        \
    Total    = 0u;                                                                       \
    Maximum  = 0u;                                                                       \
    Minimum  = (INT16U)0xFFFFu;                                                          \
}                                                                                        \
while (0)

#define  BENCH_STAT_RECORD()                                                             \
do                                                                                       \
{                                                                                        \
    CntDiff = (INT16U)(CntEnd - CntStart);                                               \
    Total  += (INT32U)CntDiff;                                                           \
    if (CntDiff > Maximum) Maximum = CntDiff;                                            \
    if (CntDiff < Minimum) Minimum = CntDiff;                                            \
}                                                                                        \
while (0)

#define  BENCH_STAT_REPORT(TestName)                                                     \
do                                                                                       \
{                                                                                        \
    BENCH_PRINT_STR(TestName);                                                           \
    BENCH_PRINT_STR(" : ");                                                              \
    BENCH_PRINT_INT(Total / BENCH_TEST_ROUNDS);                                          \
    BENCH_PRINT_STR(" / ");                                                              \
    BENCH_PRINT_INT(Maximum);                                                            \
    BENCH_PRINT_STR(" / ");                                                              \
    BENCH_PRINT_INT(Minimum);                                                            \
    BENCH_PRINT_STR("\r\n");                                                             \
}                                                                                        \
while (0)

#ifdef BENCH_KERNEL_TS_GET
#define  BENCH_RELAY_STAT_RECORD()                                                       \
do                                                                                       \
{                                                                                        \
    INT16U  relay_cnt;                                                                   \
    relay_cnt = (INT16U)BENCH_TIME_GET();                                                \
    relay_cnt = (INT16U)(relay_cnt - BENCH_KERNEL_TS_GET());                             \
    RelayTotal += (INT32U)relay_cnt;                                                     \
    if (relay_cnt > RelayMax)        RelayMax = relay_cnt;                               \
    if (relay_cnt < RelayMin)        RelayMin = relay_cnt;                               \
}                                                                                        \
while (0)
#endif

/*
*********************************************************************************************************
*                                          GLOBAL VARIABLES
*********************************************************************************************************
*/

/* The statistics of the test that is currently running. */
static  INT32U      Total;
static  INT16U      Maximum;
static  INT16U      Minimum;
static  INT16U      CntDiff;
static  INT32U      CntStart;
static  INT32U      CntEnd;
static  INT16U      Flip;

/* The count of the interrupt handler entries that found the previous entry's
 * workload still unprocessed. */
static  INT32U      ReenterCnt  = 0u;
static  INT32U      OverflowCnt;

#ifdef BENCH_KERNEL_TS_GET
static  INT32U      RelayTotal;
static  INT16U      RelayMax;
static  INT16U      RelayMin;
#endif

/* The kernel objects that the benchmark uses. */
static  OS_STK      Bench_Task1Stk[BENCH_TASK_STK_SIZE];
static  OS_STK      Bench_Task2Stk[BENCH_TASK_STK_SIZE];
static  OS_EVENT   *TestSem;
static  OS_EVENT   *TestMbox;
static  OS_EVENT   *TestQ;
static  OS_MEM     *TestMemPart;
static  INT8U       TestMemPool[BENCH_MEM_BLK_QTY * BENCH_MEM_BLK_SIZE];
static  void       *MsgStorage[BENCH_QUEUE_LENGTH];
static  INT32U      MsgValue = 1u;

/* The interrupt driven test results are accumulated separately. */
static  INT32U      MboxISRTotal;
static  INT16U      MboxISRMax;
static  INT16U      MboxISRMin;
static  INT32U      SemISRTotal;
static  INT16U      SemISRMax;
static  INT16U      SemISRMin;
static  INT32U      QISRTotal;
static  INT16U      QISRMax;
static  INT16U      QISRMin;

/*
*********************************************************************************************************
*                                        FUNCTION PROTOTYPES
*********************************************************************************************************
*/

static  void  Bench_Task1  (void *p_arg);
static  void  Bench_Task2  (void *p_arg);

static  void  Bench_MailboxTest1  (void);
static  void  Bench_MailboxTest2  (void);
static  void  Bench_SemTest1      (void);
static  void  Bench_SemTest2      (void);
static  void  Bench_QTest1        (void);
static  void  Bench_QTest2        (void);
static  void  Bench_MemTest       (void);

static  void  Bench_MailboxFromISRTest (void);
static  void  Bench_SemFromISRTest     (void);
static  void  Bench_QFromISRTest       (void);

static  INT32U  Bench_Rand (void);

/* Empty application hooks - uC/OS-II calls these when OS_APP_HOOKS_EN is set;
 * only the idle hook carries the parking policy (not used by the benchmark). */
void  App_TaskCreateHook (OS_TCB *ptcb) { (void)ptcb; }
void  App_TaskDelHook    (OS_TCB *ptcb) { (void)ptcb; }
void  App_TaskReturnHook (OS_TCB *ptcb) { (void)ptcb; }
void  App_TaskStatHook   (void)         { }
void  App_TaskSwHook     (void)         { }
void  App_TCBInitHook    (OS_TCB *ptcb) { (void)ptcb; }
void  App_TaskIdleHook   (void)         { }

/*
*********************************************************************************************************
*                                          BENCHMARK TASK 1
*
* Description: Run the workload generating side of every benchmark test.
*********************************************************************************************************
*/

static  void  Bench_Task1 (void *p_arg)
{
    (void)p_arg;

    BENCH_PRINT_STR("Test (number in CPU cycles)        : AVG / MAX / MIN\r\n");

    Bench_MailboxTest1();
    Bench_SemTest1();
    Bench_QTest1();
    Bench_MemTest();

    /* This task has the higher priority; blocking on a semaphore lets the
     * receiver task take over and print the results of each test. */

    for (;;) {
        OSTimeDly(10000u);
    }
}

/*
*********************************************************************************************************
*                                          BENCHMARK TASK 2
*
* Description: Run the workload receiving side of every benchmark test, and print the results.
*********************************************************************************************************
*/

static  void  Bench_Task2 (void *p_arg)
{
    INT8U  os_err;

    (void)p_arg;

    /* Mailbox tests (the notification slot). */
    BENCH_STAT_RESET();
    Bench_MailboxTest2();
    BENCH_STAT_REPORT("Notification                        ");

    /* Counting semaphore tests. */
    BENCH_STAT_RESET();
    Bench_SemTest2();
    BENCH_STAT_REPORT("Semaphore                           ");

    /* Blocking message queue tests. */
    BENCH_STAT_RESET();
    Bench_QTest2();
    BENCH_STAT_REPORT("Message queue                       ");

    /* Memory allocation/free pair tests. */
    BENCH_STAT_RESET();
    Bench_MemTest();
    BENCH_STAT_REPORT("Memory allocation/free pair         ");

#ifdef BENCH_KERNEL_TS_GET
    RelayTotal = 0u;
    RelayMax   = 0u;
    RelayMin   = 0xFFFFu;
#endif

    /* Prepare the interrupt driven tests. */
    Bench_TestIntSetup();

    /* Mailbox from interrupt tests. */
    BENCH_STAT_RESET();
    Bench_MailboxFromISRTest();
    MboxISRTotal = Total;
    MboxISRMax   = Maximum;
    MboxISRMin   = Minimum;

    /* Semaphore from interrupt tests. */
    BENCH_STAT_RESET();
    Bench_SemFromISRTest();
    SemISRTotal = Total;
    SemISRMax   = Maximum;
    SemISRMin   = Minimum;

    /* Message queue from interrupt tests. */
    BENCH_STAT_RESET();
    Bench_QFromISRTest();
    QISRTotal = Total;
    QISRMax   = Maximum;
    QISRMin   = Minimum;

    /* Print the results of the interrupt driven tests. */
    Total   = MboxISRTotal;
    Maximum = MboxISRMax;
    Minimum = MboxISRMin;
    BENCH_STAT_REPORT("ISR Notification                    ");

    Total   = SemISRTotal;
    Maximum = SemISRMax;
    Minimum = SemISRMin;
    BENCH_STAT_REPORT("ISR Semaphore                       ");

    Total   = QISRTotal;
    Maximum = QISRMax;
    Minimum = QISRMin;
    BENCH_STAT_REPORT("ISR Message queue                   ");

#ifdef BENCH_KERNEL_TS_GET
    Total   = RelayTotal;
    Maximum = RelayMax;
    Minimum = RelayMin;
    BENCH_STAT_REPORT("ISR RVM activation relay            ");
#endif

    if (ReenterCnt != 0u) {
        BENCH_PRINT_STR("uo2: NOTE: interrupt reentered ");
        BENCH_PRINT_INT(ReenterCnt);
        BENCH_PRINT_STR(" times.\r\n");
    }

    /* The benchmark itself is done. */
    for (;;) {
        CntStart = CntEnd;
        CntEnd   = BENCH_TIME_GET();
        if (CntStart > CntEnd) {
            OverflowCnt++;
            if ((OverflowCnt % BENCH_OVERFLOW_REPORT_INTERVAL) == 0u) {
                BENCH_PRINT_INT(BENCH_OVERFLOW_REPORT_INTERVAL);
                BENCH_PRINT_STR(" overflows\r\n");
            }
        }
        OSTimeDly(1000u);
    }
}

/*
*********************************************************************************************************
*                                        MAILBOX TEST: GENERATE SIDE
*********************************************************************************************************
*/

static  void  Bench_MailboxTest1 (void)
{
    INT32U  cnt;
    INT8U   os_err;

    for (cnt = 0u; cnt < BENCH_TEST_ROUNDS; cnt++) {
        CntStart = BENCH_TIME_GET();
        OSMboxPost(TestMbox, (void *)&MsgValue);
    }
}

/*
*********************************************************************************************************
*                                        MAILBOX TEST: RECEIVE SIDE
*********************************************************************************************************
*/

static  void  Bench_MailboxTest2 (void)
{
    INT32U  cnt;
    INT8U   os_err;
    void   *p_msg;

    for (cnt = 0u; cnt < BENCH_TEST_ROUNDS; cnt++) {
        p_msg = OSMboxPend(TestMbox, 0u, &os_err);

        CntEnd = BENCH_TIME_GET();
        BENCH_STAT_RECORD();
    }
}

/*
*********************************************************************************************************
*                                     COUNTING SEMAPHORE TEST: GENERATE SIDE
*********************************************************************************************************
*/

static  void  Bench_SemTest1 (void)
{
    INT32U  cnt;
    INT8U   os_err;

    for (cnt = 0u; cnt < BENCH_TEST_ROUNDS; cnt++) {
        CntStart = BENCH_TIME_GET();
        OSSemPost(TestSem);
    }
}

/*
*********************************************************************************************************
*                                     COUNTING SEMAPHORE TEST: RECEIVE SIDE
*********************************************************************************************************
*/

static  void  Bench_SemTest2 (void)
{
    INT32U  cnt;
    INT8U   os_err;

    for (cnt = 0u; cnt < BENCH_TEST_ROUNDS; cnt++) {
        OSSemPend(TestSem, 0u, &os_err);

        CntEnd = BENCH_TIME_GET();
        BENCH_STAT_RECORD();
    }
}

/*
*********************************************************************************************************
*                                     MESSAGE QUEUE TEST: GENERATE SIDE
*********************************************************************************************************
*/

static  void  Bench_QTest1 (void)
{
    INT32U  cnt;
    INT8U   os_err;

    for (cnt = 0u; cnt < BENCH_TEST_ROUNDS; cnt++) {
        CntStart = BENCH_TIME_GET();
        OSQPost(TestQ, (void *)&MsgValue);
    }
}

/*
*********************************************************************************************************
*                                     MESSAGE QUEUE TEST: RECEIVE SIDE
*********************************************************************************************************
*/

static  void  Bench_QTest2 (void)
{
    INT32U  cnt;
    INT8U   os_err;
    void   *p_msg;

    for (cnt = 0u; cnt < BENCH_TEST_ROUNDS; cnt++) {
        p_msg = OSQPend(TestQ, 0u, &os_err);

        CntEnd = BENCH_TIME_GET();
        BENCH_STAT_RECORD();
    }
}

/*
*********************************************************************************************************
*                                     MEMORY ALLOCATION/FREE PAIR TEST
*********************************************************************************************************
*/

static  void  Bench_MemTest (void)
{
    INT32U  cnt;
    INT8U   os_err;
    void   *p_mem[BENCH_MEM_BLK_QTY];
    INT32U  alloc_order[BENCH_MEM_BLK_QTY];
    INT32U  free_order[BENCH_MEM_BLK_QTY];
    INT32U  idx;

    for (cnt = 0u; cnt < BENCH_TEST_ROUNDS; cnt++) {
        for (idx = 0u; idx < BENCH_MEM_BLK_QTY; idx++) {
            alloc_order[idx] = idx;
            free_order[idx]  = idx;
        }

        for (idx = BENCH_MEM_BLK_QTY - 1u; idx > 0u; idx--) {
            INT32U  j;
            void   *temp;

            j = Bench_Rand() % (idx + 1u);

            temp = (void *)alloc_order[idx];
            alloc_order[idx] = alloc_order[j];
            alloc_order[j] = (INT32U)temp;

            j = Bench_Rand() % (idx + 1u);

            temp = (void *)free_order[idx];
            free_order[idx] = free_order[j];
            free_order[j] = (INT32U)temp;
        }

        CntStart = BENCH_TIME_GET();

        for (idx = 0u; idx < BENCH_MEM_BLK_QTY; idx++) {
            p_mem[idx] = OSMemGet(TestMemPart, &os_err);
        }

        for (idx = 0u; idx < BENCH_MEM_BLK_QTY; idx++) {
            OSMemPut(TestMemPart, p_mem[free_order[idx]]);
        }

        CntEnd = BENCH_TIME_GET();
        BENCH_STAT_RECORD();
    }
}

/*
*********************************************************************************************************
*                                     INTERRUPT DRIVEN MAILBOX TEST
*********************************************************************************************************
*/

static  void  Bench_MailboxFromISRTest (void)
{
    INT32U  cnt;
    INT8U   os_err;
    void   *p_msg;

    for (cnt = 0u; cnt < BENCH_TEST_ROUNDS; cnt++) {
        p_msg = OSMboxPend(TestMbox, 0u, &os_err);

        CntEnd = BENCH_TIME_GET();
        BENCH_STAT_RECORD();

        Flip = 0u;
    }
}

/*
*********************************************************************************************************
*                                     INTERRUPT DRIVEN SEMAPHORE TEST
*********************************************************************************************************
*/

static  void  Bench_SemFromISRTest (void)
{
    INT32U  cnt;
    INT8U   os_err;

    for (cnt = 0u; cnt < BENCH_TEST_ROUNDS; cnt++) {
        OSSemPend(TestSem, 0u, &os_err);

        CntEnd = BENCH_TIME_GET();
        BENCH_STAT_RECORD();

        Flip = 0u;
    }
}

/*
*********************************************************************************************************
*                                     INTERRUPT DRIVEN MESSAGE QUEUE TEST
*********************************************************************************************************
*/

static  void  Bench_QFromISRTest (void)
{
    INT32U  cnt;
    INT8U   os_err;
    void   *p_msg;

    for (cnt = 0u; cnt < BENCH_TEST_ROUNDS; cnt++) {
        p_msg = OSQPend(TestQ, 0u, &os_err);

        CntEnd = BENCH_TIME_GET();
        BENCH_STAT_RECORD();

        Flip = 0u;
    }
}

/*
*********************************************************************************************************
*                                       PSEUDO RANDOM NUMBER
*********************************************************************************************************
*/

static  INT32U  Bench_Rand (void)
{
    static  INT32U  RandState = 0xACE1u;
            INT32U  rand_val;

    RandState ^= RandState << 13u;
    RandState ^= RandState >> 9u;
    RandState ^= RandState << 7u;
    rand_val = RandState;

    return (rand_val);
}

/*
*********************************************************************************************************
*                                     THE BENCHMARK INTERRUPT HANDLER
*********************************************************************************************************
*/

void  Bench_TestIntHandler (void)
{
    static  INT32U  cnt = 0u;
            INT8U   os_err;

#ifdef BENCH_KERNEL_TS_GET
    if (cnt < BENCH_TEST_ROUNDS) {
        BENCH_RELAY_STAT_RECORD();
    }
#endif

    if (Flip != 0u) {
        ReenterCnt++;
    }

    Flip = 1u;

    if (cnt < BENCH_TEST_ROUNDS) {
        cnt++;
        CntStart = BENCH_TIME_GET();
        OSMboxPost(TestMbox, (void *)&MsgValue);
    } else if (cnt < (BENCH_TEST_ROUNDS * 2u)) {
        cnt++;
        CntStart = BENCH_TIME_GET();
        OSSemPost(TestSem);
    } else if (cnt < (BENCH_TEST_ROUNDS * 3u)) {
        cnt++;
        CntStart = BENCH_TIME_GET();
        OSQPost(TestQ, (void *)&MsgValue);
    } else {
        Bench_TestIntDisable();
    }
}

/*
*********************************************************************************************************
*                                                MAIN
*
* Note(s)  : (1) The kernel objects must be created from a task context - uC/OS-II kernel calls
*                that lock or schedule run against OSTCBCur, which is NULL until the first
*                context switch.
*********************************************************************************************************
*/

static  void  Bench_Init (void *p_arg)
{
    INT8U  os_err;

    (void)p_arg;

    Bench_TimerSetup();

    /* Create the kernel objects that are used by the benchmark. */
    TestSem    = OSSemCreate(0u);
    TestMbox   = OSMboxCreate((void *)0);
    TestQ      = OSQCreate(&MsgStorage[0], BENCH_QUEUE_LENGTH);
    TestMemPart = OSMemCreate((void *)TestMemPool, BENCH_MEM_BLK_QTY, BENCH_MEM_BLK_SIZE, &os_err);

    BENCH_PRINT_STR("\r\n");
    BENCH_PRINT_STR("uC/OS-II V2.93 on RVM - Standard benchmark test\r\n");
    BENCH_PRINT_STR("====================================================\r\n");

    /* Task1 (the generator) sits at the lower priority; every post it makes
     * switches to Task2 (the receiver) at the higher priority, which records
     * the statistic and blocks again, switching back to Task1. */
    os_err = OSTaskCreateExt(Bench_Task2,
                             (void *)0,
                             &Bench_Task2Stk[BENCH_TASK_STK_SIZE - 1u],
                             BENCH_TASK2_PRIO,
                             BENCH_TASK2_PRIO,
                             &Bench_Task2Stk[0],
                             BENCH_TASK_STK_SIZE,
                             (void *)0,
                             OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);

    os_err = OSTaskCreateExt(Bench_Task1,
                             (void *)0,
                             &Bench_Task1Stk[BENCH_TASK_STK_SIZE - 1u],
                             BENCH_TASK1_PRIO,
                             BENCH_TASK1_PRIO,
                             &Bench_Task1Stk[0],
                             BENCH_TASK_STK_SIZE,
                             (void *)0,
                             OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);

    OSTaskDel(OS_PRIO_SELF);
}

static  OS_STK  Bench_InitStk[128u];

int  main (void)
{
    INT8U  os_err;

    OSInit();

    os_err = OSTaskCreateExt(Bench_Init,
                             (void *)0,
                             &Bench_InitStk[128u - 1u],
                             3u,
                             3u,
                             &Bench_InitStk[0],
                             128u,
                             (void *)0,
                             OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);

    OSStart();

    for (;;) {
        RVM_Virt_Yield();
    }
}
