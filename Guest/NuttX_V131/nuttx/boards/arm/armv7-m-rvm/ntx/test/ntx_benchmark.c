/****************************************************************************
 * boards/arm/armv7-m-rvm/ntx/test/ntx_benchmark.c
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * NTX standard benchmark.
 *
 * Only primitives that NuttX itself provides are measured: sched_yield,
 * nxsem, nxsgmq, the watchdog timer and malloc/free.  The two ISR rows run
 * through a real NuttX IRQ dispatch entry (TIM4 -> RVM virtual vector 2 ->
 * ntx_irq_entry -> irq_dispatch) and use ISR-safe nxsem/nxmq operations.
 *
 * TIM2 is the free-running cycle counter; its clock is twice the CPU clock
 * on the STM32F4/H7 APB1 timer bus, matching the other RVM guests.
 ****************************************************************************/

#include <nuttx/config.h>

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>

#include <nuttx/arch.h>
#include <nuttx/irq.h>
#include <nuttx/mqueue.h>
#include <nuttx/sched.h>
#include <nuttx/semaphore.h>
#include <nuttx/wdog.h>

#include <sched.h>

#include "armv7m_rvm_internal.h"
#include "ntx_test.h"

#define BENCH_ROUNDS 10000U
#define BENCH_STACK 2048U
#define BENCH_PRIO 100U
#define BENCH_WAKE_PRIO 110U

static uint64_t Total;
static uint32_t Maximum;
static uint32_t Minimum;
static uint32_t CntStart;
static uint32_t CntEnd;

static volatile uint32_t Worker_Stop;
static volatile uint32_t Worker_Count;
static volatile uint32_t Yield_Active;
static volatile uint32_t Yield_Rounds;
static volatile uint32_t Yield_Start;

static sem_t Bench_Sem;
static mqd_t Bench_Mq;
static mqd_t Bench_Bmq;
static mqd_t Bench_Isr_Mq;
static struct wdog_s Bench_Wd;

/****************************************************************************
 * Helpers
 ****************************************************************************/

static void ntx_puts(const char *Text)
{
  while (*Text != '\0')
    {
      up_putc(*Text++);
    }
}

static void ntx_report(const char *Name)
{
  char Buffer[80];

  snprintf(Buffer, sizeof(Buffer), "%-34s : %u / %u / %u\r\n", Name,
           (unsigned int)(Total / BENCH_ROUNDS),
           (unsigned int)Maximum, (unsigned int)Minimum);
  ntx_puts(Buffer);
}

static void ntx_stats_reset(void)
{
  Total = 0;
  Maximum = 0;
  Minimum = 0xFFFFFFFFU;
}

static void ntx_stats_record_diff(uint32_t Diff)
{
  Total += Diff;
  if (Diff > Maximum)
    {
      Maximum = Diff;
    }

  if (Diff < Minimum)
    {
      Minimum = Diff;
    }
}

static void ntx_stats_record(void)
{
  ntx_stats_record_diff(CntEnd - CntStart);
}

/****************************************************************************
 * Yield
 ****************************************************************************/

static int ntx_yield_worker(int argc, char *argv[])
{
  (void)argc;
  (void)argv;

  while (Worker_Stop == 0)
    {
      sched_yield();

      if (Yield_Active != 0 && Yield_Rounds < BENCH_ROUNDS)
        {
          ntx_stats_record_diff(NTX_TEST_CNT_READ() - Yield_Start);
          Yield_Rounds++;
        }
    }

  return 0;
}

static void ntx_bench_yield(void)
{
  uint32_t Count;

  Worker_Stop = 0;
  Yield_Active = 0;
  Yield_Rounds = 0;
  task_create("ntxy", BENCH_PRIO, BENCH_STACK, ntx_yield_worker, NULL);

  /* Let the worker reach its first sched_yield() and return to us. */

  sched_yield();

  ntx_stats_reset();
  Yield_Active = 1;
  for (Count = 0; Count < BENCH_ROUNDS; Count++)
    {
      Yield_Start = NTX_TEST_CNT_READ();
      sched_yield();
    }

  while (Yield_Rounds < BENCH_ROUNDS)
    {
      sched_yield();
    }

  Yield_Active = 0;
  Worker_Stop = 1;
  sched_yield();
  ntx_report("Yield");
}

/****************************************************************************
 * Semaphore
 ****************************************************************************/

static void ntx_bench_semaphore(void)
{
  uint32_t Count;

  nxsem_init(&Bench_Sem, 0, 0);

  ntx_stats_reset();
  for (Count = 0; Count < BENCH_ROUNDS; Count++)
    {
      CntStart = NTX_TEST_CNT_READ();
      nxsem_post(&Bench_Sem);
      nxsem_wait(&Bench_Sem);
      CntEnd = NTX_TEST_CNT_READ();
      ntx_stats_record();
    }

  ntx_report("Semaphore");
}

/****************************************************************************
 * Message queue
 ****************************************************************************/

static void ntx_bench_message_queue(void)
{
  uint32_t Count;
  uint32_t Value;

  ntx_stats_reset();
  for (Count = 0; Count < BENCH_ROUNDS; Count++)
    {
      CntStart = NTX_TEST_CNT_READ();
      nxmq_send(Bench_Mq, (char *)&Count, sizeof(Count), 0);
      nxmq_receive(Bench_Mq, (char *)&Value, sizeof(Value), NULL);
      CntEnd = NTX_TEST_CNT_READ();
      ntx_stats_record();
    }

  ntx_report("Message queue");
}

/****************************************************************************
 * Blocking message queue
 ****************************************************************************/

static int ntx_bmq_worker(int argc, char *argv[])
{
  mqd_t Mq;
  uint32_t Value;
  int Ret;

  (void)argc;
  (void)argv;

  /* A new task has its own descriptor table (FDCLONE_DISABLE), so it must
   * open the named queue itself; the descriptor number is process-local. */

  Mq = nxmq_open("/ntxbmq", O_RDWR, 0666);
  if (Mq < 0)
    {
      return 0;
    }

  while (Worker_Stop == 0)
    {
      Ret = nxmq_receive(Mq, (char *)&Value, sizeof(Value), NULL);
      if (Ret >= 0)
        {
          Worker_Count++;
        }
    }

  return 0;
}

static void ntx_bench_blocking_mq(void)
{
  uint32_t Count;
  uint32_t Value = 0x5a5a5a5aU;

  Worker_Stop = 0;
  Worker_Count = 0;
  task_create("ntxb", BENCH_PRIO, BENCH_STACK, ntx_bmq_worker, NULL);

  ntx_stats_reset();
  for (Count = 0; Count < BENCH_ROUNDS; Count++)
    {
      while (Worker_Count != Count)
        {
          sched_yield();
        }

      CntStart = NTX_TEST_CNT_READ();
      nxmq_send(Bench_Bmq, (char *)&Value, sizeof(Value), 0);
      CntEnd = NTX_TEST_CNT_READ();
      ntx_stats_record();
    }

  Worker_Stop = 1;
  sched_yield();
  ntx_report("Blocking message queue");
}

/****************************************************************************
 * Alarm
 ****************************************************************************/

static void ntx_alarm_cb(wdparm_t Arg)
{
  (void)Arg;
}

static void ntx_bench_alarm(void)
{
  uint32_t Count;

  ntx_stats_reset();
  for (Count = 0; Count < BENCH_ROUNDS; Count++)
    {
      CntStart = NTX_TEST_CNT_READ();
      wd_start(&Bench_Wd, 1, ntx_alarm_cb, 0);
      wd_cancel(&Bench_Wd);
      CntEnd = NTX_TEST_CNT_READ();
      ntx_stats_record();
    }

  ntx_report("Alarm");
}

/****************************************************************************
 * Memory
 ****************************************************************************/

static void ntx_bench_memory(void)
{
  uint32_t Count;
  void *Block;

  ntx_stats_reset();
  for (Count = 0; Count < BENCH_ROUNDS; Count++)
    {
      CntStart = NTX_TEST_CNT_READ();
      Block = malloc(32);
      free(Block);
      CntEnd = NTX_TEST_CNT_READ();
      ntx_stats_record();
    }

  ntx_report("Memory allocation/free pair");
}

/****************************************************************************
 * ISR rows
 ****************************************************************************/

#define BENCH_ISR_QTY 3U

static const char *g_isr_name[BENCH_ISR_QTY];
static uint32_t g_isr_avg[BENCH_ISR_QTY];
static uint32_t g_isr_max[BENCH_ISR_QTY];
static uint32_t g_isr_min[BENCH_ISR_QTY];
static uint32_t g_isr_count;

static volatile uint32_t Isr_Stamp;
static volatile uint32_t Isr_Seen;
static volatile uint32_t Isr_Flag;
static volatile uint32_t Isr_Mode;
static sem_t Isr_Sem;

static void ntx_bench_vector(void)
{
  ntx_irq_entry(NTX_TEST_INT_VCT);
}

static int ntx_bench_irq(int irq, FAR void *context, FAR void *arg)
{
  (void)irq;
  (void)context;
  (void)arg;
  uint32_t Value = 0;

  NTX_TEST_INT_CLEAR();
  Isr_Stamp = NTX_TEST_CNT_READ();

  if (Isr_Mode == 1)
    {
      nxsem_post(&Isr_Sem);
    }
  else if (Isr_Mode == 2)
    {
      nxmq_send(Bench_Isr_Mq, (char *)&Value, sizeof(Value), 0);
    }
  else if (Isr_Mode == 3)
    {
      /* RVM activation relay: the stamp above is the whole handler.  The
       * row measures the bare interrupt relay - vector delivery, vector
       * thread round trip and resume of the interrupted spinning thread -
       * with no NuttX kernel object in the path.  It is the baseline the
       * other ISR rows should be read against. */

      Isr_Flag = 1;
    }

  return OK;
}

static int ntx_isr_sem_worker(int argc, char *argv[])
{
  (void)argc;
  (void)argv;

  for (; ; )
    {
      nxsem_wait(&Isr_Sem);
      Isr_Seen = NTX_TEST_CNT_READ();
    }

  return 0;
}

static int ntx_isr_mq_worker(int argc, char *argv[])
{
  mqd_t Mq;
  uint32_t Value;

  (void)argc;
  (void)argv;

  Mq = nxmq_open("/ntxisr", O_RDWR, 0666);
  if (Mq < 0)
    {
      return 0;
    }

  for (; ; )
    {
      if (nxmq_receive(Mq, (char *)&Value, sizeof(Value), NULL) >= 0)
        {
          Isr_Seen = NTX_TEST_CNT_READ();
        }
    }

  return 0;
}

static void ntx_isr_store(const char *Name)
{
  uint32_t Index = g_isr_count;

  if (Index < BENCH_ISR_QTY)
    {
      g_isr_name[Index] = Name;
      g_isr_avg[Index] = (uint32_t)(Total / BENCH_ROUNDS);
      g_isr_max[Index] = Maximum;
      g_isr_min[Index] = Minimum;
      g_isr_count = Index + 1;
    }
}

/* Relay row: this thread spins, gets preempted by the vector thread and
 * reads the cycle counter right after the monitor resumes it.  The delta
 * covers the complete RVM interrupt relay without any OS primitive.
 */

static void ntx_isr_relay_measure(void)
{
  uint32_t Count;

  ntx_stats_reset();
  for (Count = 0; Count < BENCH_ROUNDS; Count++)
    {
      while (Isr_Flag == 0)
        {
        }

      ntx_stats_record_diff(NTX_TEST_CNT_READ() - Isr_Stamp);
      Isr_Flag = 0;
    }
}

static void ntx_isr_measure(void)
{
  uint32_t Count;
  uint32_t Last = 0;

  ntx_stats_reset();
  for (Count = 0; Count < BENCH_ROUNDS; Count++)
    {
      while (Isr_Seen == Last)
        {
        }

      Last = Isr_Seen;
      ntx_stats_record_diff(Isr_Seen - Isr_Stamp);
    }
}

static void ntx_bench_isr(void)
{
  uint32_t Count;

  irq_attach(NTX_TEST_INT_VCT, ntx_bench_irq, NULL);
  NTX_Test_Int_Init(ntx_bench_vector);

  nxsem_init(&Isr_Sem, 0, 0);
  Isr_Mode = 1;
  task_create("ntxsi", BENCH_WAKE_PRIO, BENCH_STACK, ntx_isr_sem_worker,
              NULL);
  sched_yield();
  NTX_Test_Int_Start();

  Isr_Seen = 0;
  ntx_isr_measure();
  ntx_isr_store("ISR Semaphore");

  Isr_Mode = 2;
  task_create("ntxsm", BENCH_WAKE_PRIO, BENCH_STACK, ntx_isr_mq_worker,
              NULL);
  sched_yield();

  Isr_Seen = 0;
  ntx_isr_measure();
  ntx_isr_store("ISR Message queue");

  Isr_Mode = 3;
  Isr_Flag = 0;
  ntx_isr_relay_measure();
  ntx_isr_store("ISR RVM activation relay");

  NTX_Test_Int_Disable();

  for (Count = 0; Count < g_isr_count; Count++)
    {
      char Buffer[80];

      snprintf(Buffer, sizeof(Buffer), "%-34s : %u / %u / %u\r\n",
               g_isr_name[Count], (unsigned int)g_isr_avg[Count],
               (unsigned int)g_isr_max[Count],
               (unsigned int)g_isr_min[Count]);
      ntx_puts(Buffer);
    }

  ntx_puts("Benchmark complete\r\n");
}

/****************************************************************************
 * Public entry
 ****************************************************************************/

int ntx_main(int argc, char *argv[])
{
  struct mq_attr Attr;

  (void)argc;
  (void)argv;

  ntx_puts("\r\nNTX: NuttX-on-RVM benchmark\r\n");
  ntx_puts("====================================================\r\n");

  NTX_Test_Timer_Init();

  memset(&Attr, 0, sizeof(Attr));
  Attr.mq_maxmsg = 8;
  Attr.mq_msgsize = sizeof(uint32_t);
  Bench_Mq = nxmq_open("/ntxbench", O_CREAT | O_RDWR, 0666, &Attr);
  Bench_Bmq = nxmq_open("/ntxbmq", O_CREAT | O_RDWR, 0666, &Attr);
  Attr.mq_maxmsg = 1;
  Bench_Isr_Mq = nxmq_open("/ntxisr", O_CREAT | O_RDWR, 0666, &Attr);
  if (Bench_Mq < 0 || Bench_Bmq < 0 || Bench_Isr_Mq < 0)
    {
      ntx_puts("NTX: mqueue open failed\r\n");
      return 1;
    }

  ntx_puts("Test (number in CPU cycles)        : AVG / MAX / MIN\r\n");

  ntx_bench_yield();
  ntx_bench_semaphore();
  ntx_bench_message_queue();
  ntx_bench_blocking_mq();
  ntx_bench_alarm();
  ntx_bench_memory();
  ntx_bench_isr();

  for (; ; )
    {
    }

  return 0;
}
