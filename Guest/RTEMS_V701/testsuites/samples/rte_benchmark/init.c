/* SPDX-License-Identifier: Unlicense */

/*
 * RTE on RVM standard benchmark.
 *
 * This is the RTEMS counterpart of the RMP benchmark.  It runs the RMP
 * thread-level items (Yield, Semaphore, FIFO, Message queue, Blocking
 * message queue, Alarm combination, Memory allocation/free pair) and the
 * RMP interrupt-level items (ISR Semaphore, ISR Message queue, ISR Blocking
 * message queue) plus the RVM activation relay.  The Notification and
 * Activation rows are RTEMS-specific extras.  TIM2 is the free running cycle
 * counter; TIM4 is the 100 us interrupt source delivered to the guest through
 * a virtual vector, exactly like the uC/OS-II and uC/OS-III RVM test headers.
 */

#include <rtems.h>
#include <rtems/chain.h>
#include <rtems/rtems/clock.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <rtems/score/armv7m.h>

#include "rvm.h"
#include "rte_test.h"

#define BENCH_TEST_ROUNDS 10000u
#define BENCH_MEM_BLK_QTY 8u
#define BENCH_MEM_BLK_SIZE 32u
#define BENCH_QUEUE_LENGTH 8u
#define BENCH_ALARM_QTY 5u

#define BENCH_TASK_STK_SIZE 8192u

#define BENCH_TASK1_PRIO 2u
#define BENCH_TASK2_PRIO 1u
#define BENCH_YIELD_PRIO 2u

/* Event/ISR modes. */
#define BENCH_INT_EVENT 0u
#define BENCH_INT_SEM 1u
#define BENCH_INT_QUEUE 2u
#define BENCH_INT_ACT 3u
#define BENCH_INT_BMQ 4u

#define BENCH_KERNEL_TS_GET() (START)

#define START \
  (*((volatile uint16_t *) (DATA_SHARED_SHARED1_BASE + 4U)))

/* Statistics. */
static uint32_t Total;
static uint32_t Maximum;
static uint32_t Minimum;
static uint32_t CntStart;
static uint32_t CntEnd;
static uint32_t RelayTotal;
static uint32_t RelayMax;
static uint32_t RelayMin;

/* Benchmark objects. */
static rtems_id Bench_Task1_Id;
static rtems_id Bench_Task2_Id;
static rtems_id Bench_Sem_Id;
static rtems_id Bench_Q_Id;
static rtems_id Bench_Bmq_Id;
static rtems_id Bench_Alarm_Id[BENCH_ALARM_QTY];
static rtems_chain_control Bench_Fifo;
static rtems_chain_node Bench_Fifo_Node;
static volatile uint32_t Bench_Alarm_Count[BENCH_ALARM_QTY];
static const rtems_interval Bench_Alarm_Period[BENCH_ALARM_QTY] =
  { 1U, 2U, 3U, 5U, 7U };
static uint32_t MsgValue = 0x5A5A5A5Au;

/* ISR phase. */
static volatile uint32_t Bench_IntMode;
static volatile uint32_t Bench_IntRounds;

static void Bench_Report(const char *name)
{
  printf("%s : %lu / %lu / %lu\r\n", name,
    (unsigned long) (Total / BENCH_TEST_ROUNDS),
    (unsigned long) Maximum, (unsigned long) Minimum);
}

static void Bench_ReportScaled(const char *name, uint32_t divisor)
{
  printf("%s : %lu / %lu / %lu\r\n", name,
    (unsigned long) (Total / (BENCH_TEST_ROUNDS * divisor)),
    (unsigned long) (Maximum / divisor),
    (unsigned long) (Minimum / divisor));
}

static void Bench_StatReset(void)
{
  Total = 0U;
  Maximum = 0U;
  Minimum = 0xFFFFFFFFUL;
}

static void Bench_StatRecord(void)
{
  uint32_t diff = CntEnd - CntStart;

  Total += diff;
  if (diff > Maximum) {
    Maximum = diff;
  }
  if (diff < Minimum) {
    Minimum = diff;
  }
}

static void Bench_RelayReset(void)
{
  RelayTotal = 0U;
  RelayMax = 0U;
  RelayMin = 0xFFFFFFFFUL;
}

static void Bench_RelayRecord(void)
{
  uint16_t relay = (uint16_t) (BENCH_TIME_GET() - BENCH_KERNEL_TS_GET());

  RelayTotal += relay;
  if (relay > RelayMax) {
    RelayMax = relay;
  }
  if (relay < RelayMin) {
    RelayMin = relay;
  }
}

static void Bench_AlarmHandler(rtems_id timer_id, void *arg)
{
  uint32_t idx = (uint32_t) (uintptr_t) arg;

  ++Bench_Alarm_Count[idx];
  (void) rtems_timer_fire_after(
    timer_id, Bench_Alarm_Period[idx], Bench_AlarmHandler, arg);
}

/* RMP drives its virtual alarm manager one tick at a time.  RTEMS has no
 * public alarm-manager entry point, so advance the classic clock by hand
 * instead; the real virtual tick is masked for the duration. */
static void Bench_AlarmTest(void)
{
  static const uint32_t expect[BENCH_ALARM_QTY] =
    { 10000U, 5000U, 3333U, 2000U, 1428U };
  uint32_t cnt;
  uint32_t idx;

  RVM_Hyp_Int_Dis();

  for (idx = 0U; idx < BENCH_ALARM_QTY; ++idx) {
    Bench_Alarm_Count[idx] = 0U;
    (void) rtems_timer_fire_after(
      Bench_Alarm_Id[idx], Bench_Alarm_Period[idx], Bench_AlarmHandler,
      (void *) (uintptr_t) idx);
  }

  Bench_StatReset();
  for (cnt = 0U; cnt < BENCH_TEST_ROUNDS; ++cnt) {
    CntStart = BENCH_TIME_GET();
    (void) rtems_clock_tick();
    CntEnd = BENCH_TIME_GET();
    Bench_StatRecord();
  }

  for (idx = 0U; idx < BENCH_ALARM_QTY; ++idx) {
    (void) rtems_timer_cancel(Bench_Alarm_Id[idx]);
  }

  RVM_Hyp_Int_Ena();

  Bench_Report("Alarm combination (1/2/3/5/7)      ");

  for (idx = 0U; idx < BENCH_ALARM_QTY; ++idx) {
    if (Bench_Alarm_Count[idx] != expect[idx]) {
      printf("Alarm %lu count %lu, expected %lu\r\n",
        (unsigned long) Bench_Alarm_Period[idx],
        (unsigned long) Bench_Alarm_Count[idx],
        (unsigned long) expect[idx]);
    }
  }
}

static void Bench_FifoTest(void)
{
  uint32_t cnt;
  rtems_chain_node *node;

  rtems_chain_initialize_empty(&Bench_Fifo);
  rtems_chain_initialize_node(&Bench_Fifo_Node);

  Bench_StatReset();
  for (cnt = 0U; cnt < BENCH_TEST_ROUNDS; ++cnt) {
    CntStart = BENCH_TIME_GET();
    rtems_chain_append(&Bench_Fifo, &Bench_Fifo_Node);
    node = rtems_chain_get(&Bench_Fifo);
    CntEnd = BENCH_TIME_GET();
    Bench_StatRecord();

    if (node != &Bench_Fifo_Node) {
      printf("FIFO mismatch\r\n");
    }
  }

  Bench_Report("FIFO                              ");
}

static void Bench_TimerSetup(void)
{
  RCC_APB1ENR |= RCC_APB1ENR_TIM2;
  TIM2_CR1 = 0U;
  TIM2_PSC = 0U;
  TIM2_ARR = 0xFFFFFFFFUL;
  TIM2_CR1 |= TIM_CR1_CEN;
}

static void Bench_TestIntDisable(void)
{
  TIM4_CR1 = 0U;
  TIM4_SR = 0U;
  TIM4_DIER = 0U;

  (void) RVM_A7M_Int_Local_Mod(
    KFN_INT_LOCAL_MOD, RTE_TEST_INT_PHYS,
    RVM_A7M_KFN_INT_LOCAL_MOD_STATE_SET, 0U);
  RVM_Virt_Vct_Reg(RTE_TEST_INT_VCT, RVM_NULL, RVM_VCT_MASKABLE);
}

static void Bench_TestIntHandler(void)
{
  _ARMV7M_Interrupt_service_enter();

  TIM4_SR = 0U;
  CntStart = BENCH_TIME_GET();

  if (Bench_IntMode == BENCH_INT_EVENT) {
    (void) rtems_event_send(Bench_Task2_Id, RTEMS_EVENT_0);
  } else if (Bench_IntMode == BENCH_INT_SEM) {
    (void) rtems_semaphore_release(Bench_Sem_Id);
  } else if (Bench_IntMode == BENCH_INT_QUEUE) {
    (void) rtems_message_queue_send(
      Bench_Q_Id, &MsgValue, sizeof(MsgValue));
  } else if (Bench_IntMode == BENCH_INT_BMQ) {
    (void) rtems_message_queue_send(
      Bench_Bmq_Id, &MsgValue, sizeof(MsgValue));
  } else if (Bench_IntMode == BENCH_INT_ACT) {
    (void) rtems_task_resume(Bench_Task2_Id);
  } else {
    /* No operation required. */
  }

  if (Bench_IntRounds > 0U) {
    --Bench_IntRounds;
  }

  _ARMV7M_Interrupt_service_leave();
}

static void Bench_TestIntSetup(void)
{
  if (RVM_Hyp_Vct_Phys(RTE_TEST_INT_PHYS, RTE_TEST_INT_VCT) != 0) {
    printf("vct phys failed\r\n");
  }
  (void) RVM_A7M_Int_Local_Mod(
    KFN_INT_LOCAL_MOD, RTE_TEST_INT_PHYS,
    RVM_A7M_KFN_INT_LOCAL_MOD_PRIO_SET, 0xFFU);
  (void) RVM_A7M_Int_Local_Mod(
    KFN_INT_LOCAL_MOD, RTE_TEST_INT_PHYS,
    RVM_A7M_KFN_INT_LOCAL_MOD_STATE_SET, 1U);
  RVM_Virt_Vct_Reg(
    RTE_TEST_INT_VCT, Bench_TestIntHandler, RVM_VCT_MASKABLE);

  RCC_APB1ENR |= RCC_APB1ENR_TIM4;
  TIM4_CR1 = TIM_CR1_DIR;
  TIM4_PSC = 0U;
  TIM4_ARR = RTE_TEST_TIM_ARR;
  TIM4_SR &= ~TIM_SR_UIF;
  TIM4_DIER |= TIM_DIER_UIE;
  TIM4_CR1 |= TIM_CR1_CEN;
}

static void Bench_IntTest(uint32_t mode, const char *name)
{
  uint32_t cnt;
  rtems_event_set events;
  uint32_t msg;
  size_t size;

  Bench_IntMode = mode;
  Bench_IntRounds = BENCH_TEST_ROUNDS;
  Bench_StatReset();
  Bench_RelayReset();

  for (cnt = 0U; cnt < BENCH_TEST_ROUNDS; ++cnt) {
    if (mode == BENCH_INT_EVENT) {
      (void) rtems_event_receive(
        RTEMS_EVENT_0, RTEMS_WAIT | RTEMS_EVENT_ANY, RTEMS_NO_TIMEOUT,
        &events);
    } else if (mode == BENCH_INT_SEM) {
      (void) rtems_semaphore_obtain(
        Bench_Sem_Id, RTEMS_WAIT, RTEMS_NO_TIMEOUT);
    } else if (mode == BENCH_INT_QUEUE) {
      size = sizeof(msg);
      (void) rtems_message_queue_receive(
        Bench_Q_Id, &msg, &size, RTEMS_WAIT, RTEMS_NO_TIMEOUT);
    } else if (mode == BENCH_INT_BMQ) {
      size = sizeof(msg);
      (void) rtems_message_queue_receive(
        Bench_Bmq_Id, &msg, &size, RTEMS_WAIT, RTEMS_NO_TIMEOUT);
    } else {
      (void) rtems_task_suspend(RTEMS_SELF);
    }

    CntEnd = BENCH_TIME_GET();
    Bench_StatRecord();

    if (mode == BENCH_INT_EVENT) {
      Bench_RelayRecord();
    }
  }

  Bench_Report(name);

  if (mode == BENCH_INT_EVENT) {
    printf("ISR RVM activation relay          : %lu / %lu / %lu\r\n",
      (unsigned long) (RelayTotal / BENCH_TEST_ROUNDS),
      (unsigned long) RelayMax, (unsigned long) RelayMin);
  }
}

static void Bench_Task1(rtems_task_argument arg)
{
  uint32_t cnt;
  rtems_task_priority old;
  rtems_event_set events;

  (void) arg;

  printf("\r\nRTE on RVM - Standard benchmark test\r\n");
  printf("Test (number in CPU cycles)        : AVG / MAX / MIN\r\n");

  /* Yield test: both tasks run at the same priority. */
  (void) rtems_task_set_priority(RTEMS_SELF, BENCH_YIELD_PRIO, &old);
  for (cnt = 0U; cnt < BENCH_TEST_ROUNDS; ++cnt) {
    CntStart = BENCH_TIME_GET();
    (void) rtems_task_wake_after(RTEMS_YIELD_PROCESSOR);
  }

  /* Wait until the receiver is actually blocked in the event test. */
  (void) rtems_event_receive(
    RTEMS_EVENT_1, RTEMS_WAIT | RTEMS_EVENT_ANY, RTEMS_NO_TIMEOUT, &events);

  for (cnt = 0U; cnt < BENCH_TEST_ROUNDS; ++cnt) {
    CntStart = BENCH_TIME_GET();
    (void) rtems_event_send(Bench_Task2_Id, RTEMS_EVENT_0);
  }

  for (cnt = 0U; cnt < BENCH_TEST_ROUNDS; ++cnt) {
    CntStart = BENCH_TIME_GET();
    (void) rtems_task_resume(Bench_Task2_Id);
  }

  for (cnt = 0U; cnt < BENCH_TEST_ROUNDS; ++cnt) {
    CntStart = BENCH_TIME_GET();
    (void) rtems_semaphore_release(Bench_Sem_Id);
  }

  for (cnt = 0U; cnt < BENCH_TEST_ROUNDS; ++cnt) {
    CntStart = BENCH_TIME_GET();
    (void) rtems_message_queue_send(
      Bench_Q_Id, &MsgValue, sizeof(MsgValue));
  }

  for (cnt = 0U; cnt < BENCH_TEST_ROUNDS; ++cnt) {
    CntStart = BENCH_TIME_GET();
    (void) rtems_message_queue_send(
      Bench_Bmq_Id, &MsgValue, sizeof(MsgValue));
  }

  /* The alarm test advances the clock by hand; keep this task out of the
   * way instead of giving it a timeout that those ticks would expire. */
  (void) rtems_task_suspend(RTEMS_SELF);
}

static void Bench_Task2(rtems_task_argument arg)
{
  uint32_t cnt;
  uint32_t iter;
  rtems_task_priority old;
  rtems_event_set events;
  uint32_t msg;
  size_t size;
  void *mem[BENCH_MEM_BLK_QTY];

  (void) arg;

  /* Yield: share the generator's priority for this item only. */
  (void) rtems_task_set_priority(RTEMS_SELF, BENCH_YIELD_PRIO, &old);
  Bench_StatReset();

  for (cnt = 0U; cnt < BENCH_TEST_ROUNDS; ++cnt) {
    (void) rtems_task_wake_after(RTEMS_YIELD_PROCESSOR);

    CntEnd = BENCH_TIME_GET();
    Bench_StatRecord();
  }

  Bench_Report("Yield                              ");

  (void) rtems_task_set_priority(RTEMS_SELF, BENCH_TASK2_PRIO, &old);

  /* Notification: tell the generator we are ready to receive. */
  (void) rtems_event_send(Bench_Task1_Id, RTEMS_EVENT_1);
  Bench_StatReset();
  for (cnt = 0U; cnt < BENCH_TEST_ROUNDS; ++cnt) {
    (void) rtems_event_receive(
      RTEMS_EVENT_0, RTEMS_WAIT | RTEMS_EVENT_ANY, RTEMS_NO_TIMEOUT,
      &events);
    CntEnd = BENCH_TIME_GET();
    Bench_StatRecord();
  }
  Bench_Report("Notification                       ");

  /* Activation: the generator resumes us from the suspended state. */
  Bench_StatReset();
  for (cnt = 0U; cnt < BENCH_TEST_ROUNDS; ++cnt) {
    (void) rtems_task_suspend(RTEMS_SELF);
    CntEnd = BENCH_TIME_GET();
    Bench_StatRecord();
  }
  Bench_Report("Activation                         ");

  /* Counting semaphore. */
  Bench_StatReset();
  for (cnt = 0U; cnt < BENCH_TEST_ROUNDS; ++cnt) {
    (void) rtems_semaphore_obtain(
      Bench_Sem_Id, RTEMS_WAIT, RTEMS_NO_TIMEOUT);
    CntEnd = BENCH_TIME_GET();
    Bench_StatRecord();
  }
  Bench_Report("Semaphore                          ");

  /* Message queue. */
  Bench_StatReset();
  for (cnt = 0U; cnt < BENCH_TEST_ROUNDS; ++cnt) {
    size = sizeof(msg);
    (void) rtems_message_queue_receive(
      Bench_Q_Id, &msg, &size, RTEMS_WAIT, RTEMS_NO_TIMEOUT);
    CntEnd = BENCH_TIME_GET();
    Bench_StatRecord();
  }
  Bench_Report("Message queue                      ");

  /* Blocking message queue: one slot, blocking receive. */
  Bench_StatReset();
  for (cnt = 0U; cnt < BENCH_TEST_ROUNDS; ++cnt) {
    size = sizeof(msg);
    (void) rtems_message_queue_receive(
      Bench_Bmq_Id, &msg, &size, RTEMS_WAIT, RTEMS_NO_TIMEOUT);
    CntEnd = BENCH_TIME_GET();
    Bench_StatRecord();
  }
  Bench_Report("Blocking message queue             ");

  Bench_FifoTest();
  Bench_AlarmTest();

  /* Memory allocation/free pairs.  RMP reports one alloc/free pair, so
   * scale the eight pairs measured per round down to a single pair. */
  Bench_StatReset();
  for (cnt = 0U; cnt < BENCH_TEST_ROUNDS; ++cnt) {
    CntStart = BENCH_TIME_GET();
    for (iter = 0U; iter < BENCH_MEM_BLK_QTY; ++iter) {
      mem[iter] = malloc(BENCH_MEM_BLK_SIZE);
    }
    for (iter = 0U; iter < BENCH_MEM_BLK_QTY; ++iter) {
      free(mem[iter]);
    }
    CntEnd = BENCH_TIME_GET();
    Bench_StatRecord();
  }
  Bench_ReportScaled("Memory allocation/free pair        ", BENCH_MEM_BLK_QTY);

  /* Interrupt driven items. */
  printf("Interrupt driven tests, TIM4 @100 us\r\n");
  Bench_TestIntSetup();
  Bench_IntTest(BENCH_INT_EVENT, "ISR Notification                   ");
  Bench_IntTest(BENCH_INT_SEM, "ISR Semaphore                      ");
  Bench_IntTest(BENCH_INT_QUEUE, "ISR Message queue                  ");
  Bench_IntTest(BENCH_INT_BMQ, "ISR Blocking message queue         ");
  Bench_IntTest(BENCH_INT_ACT, "ISR Activation                     ");
  Bench_TestIntDisable();

  printf("Benchmark complete\r\n");

  for (;;) {
    (void) rtems_task_wake_after(10000U);
  }
}

rtems_task Init(rtems_task_argument ignored);

rtems_task Init(rtems_task_argument ignored)
{
  uint32_t iter;

  (void) ignored;

  Bench_TimerSetup();

  if (rtems_semaphore_create(
      rtems_build_name('B', 'S', 'E', 'M'), 0U, RTEMS_COUNTING_SEMAPHORE,
      8U, &Bench_Sem_Id) != RTEMS_SUCCESSFUL) {
    printf("sem create failed\r\n");
  }
  if (rtems_message_queue_create(
      rtems_build_name('B', 'Q', 'U', 'E'), BENCH_QUEUE_LENGTH,
      sizeof(uint32_t), RTEMS_DEFAULT_ATTRIBUTES, &Bench_Q_Id)
      != RTEMS_SUCCESSFUL) {
    printf("queue create failed\r\n");
  }
  if (rtems_message_queue_create(
      rtems_build_name('B', 'B', 'M', 'Q'), 1U,
      sizeof(uint32_t), RTEMS_DEFAULT_ATTRIBUTES, &Bench_Bmq_Id)
      != RTEMS_SUCCESSFUL) {
    printf("bmq create failed\r\n");
  }

  for (iter = 0U; iter < BENCH_ALARM_QTY; ++iter) {
    if (rtems_timer_create(
        rtems_build_name('B', 'A', 'L', (char) ('0' + iter)),
        &Bench_Alarm_Id[iter]) != RTEMS_SUCCESSFUL) {
      printf("timer create failed\r\n");
    }
  }

  if (rtems_task_create(
      rtems_build_name('B', 'T', '0', '1'), BENCH_TASK1_PRIO,
      BENCH_TASK_STK_SIZE, RTEMS_DEFAULT_MODES, RTEMS_DEFAULT_ATTRIBUTES,
      &Bench_Task1_Id) != RTEMS_SUCCESSFUL) {
    printf("task1 create failed\r\n");
  } else {
    (void) rtems_task_start(Bench_Task1_Id, Bench_Task1, 0U);
  }

  if (rtems_task_create(
      rtems_build_name('B', 'T', '0', '2'), BENCH_TASK2_PRIO,
      BENCH_TASK_STK_SIZE, RTEMS_DEFAULT_MODES, RTEMS_DEFAULT_ATTRIBUTES,
      &Bench_Task2_Id) != RTEMS_SUCCESSFUL) {
    printf("task2 create failed\r\n");
  } else {
    (void) rtems_task_start(Bench_Task2_Id, Bench_Task2, 0U);
  }

  (void) rtems_task_delete(RTEMS_SELF);
}

static void *rte_bench_idle(uintptr_t ignored)
{
  (void) ignored;

  for (;;) {
    RVM_Hyp_Vct_Wait();
  }

  return NULL;
}

#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER

#define CONFIGURE_MAXIMUM_TASKS 8
#define CONFIGURE_MAXIMUM_SEMAPHORES 2
#define CONFIGURE_MAXIMUM_MESSAGE_QUEUES 2
#define CONFIGURE_MAXIMUM_TIMERS 5
#define CONFIGURE_RTEMS_INIT_TASKS_TABLE
#define CONFIGURE_INIT_TASK_STACK_SIZE 8192
#define CONFIGURE_IDLE_TASK_BODY rte_bench_idle
#define CONFIGURE_IDLE_TASK_STACK_SIZE 4096
#define CONFIGURE_MICROSECONDS_PER_TICK 100000

#define CONFIGURE_INIT
#include <rtems/confdefs.h>
