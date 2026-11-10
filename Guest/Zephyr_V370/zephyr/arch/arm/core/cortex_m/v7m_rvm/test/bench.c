/* SPDX-License-Identifier: Unlicense */

/*
 * ZPR (Zephyr-on-RVM) standard benchmark.
 *
 * The 15 items mirror the RTE benchmark: 9 thread-level rows and 6 ISR rows
 * driven by the TIM4 virtual vector.  TIM2 is the free-running cycle counter;
 * its clock is twice the CPU clock on the STM32F4/H7 APB1 timer bus, which is
 * the convention used by the other RVM guests.
 */

#include <zephyr/kernel.h>
#include <stdint.h>

#include "zpr_config.h"
#include "zpr_test.h"

#define BENCH_ROUNDS 10000U
#define BENCH_STACK  2048U


static uint64_t Total;
static uint32_t Maximum;
static uint32_t Minimum;
static uint32_t CntStart;
static uint32_t CntEnd;

static struct k_thread Worker;
static K_THREAD_STACK_DEFINE(Worker_Stack, BENCH_STACK);
static volatile uint32_t Worker_Stop;
static volatile uint32_t Worker_Count;
static volatile uint32_t Yield_Active;
static volatile uint32_t Yield_Rounds;
static volatile uint32_t Yield_Start;

static struct k_sem Bench_Sem;
static struct k_event Bench_Event;
static struct k_msgq Bench_Msgq;
static struct k_msgq Bench_Bmq;
static char Bench_Msgq_Buf[8U * sizeof(uint32_t)];
static char Bench_Bmq_Buf[sizeof(uint32_t)];
static struct k_queue Bench_Fifo_Queue;
static uint32_t Bench_Fifo_Item;
static struct k_timer Bench_Timer;

static void Bench_Stats_Reset(void)
{
	Total = 0U;
	Maximum = 0U;
	Minimum = 0xFFFFFFFFU;
}

static void Bench_Stats_Record_Diff(uint32_t diff)
{
	Total += diff;
	if (diff > Maximum)
		Maximum = diff;
	if (diff < Minimum)
		Minimum = diff;
}

static void Bench_Stats_Record(void)
{
	Bench_Stats_Record_Diff(CntEnd - CntStart);
}

static uint32_t Bench_Cycles(void)
{
	return ZPR_TEST_CNT_READ();
}

static void Bench_Report(const char *name);

static volatile uint32_t Isr_Stamp;
static volatile uint32_t Isr_Flag;
static volatile uint32_t Isr_Mode;

static void Bench_Isr_Hook(void)
{
	Isr_Stamp = Bench_Cycles();
	if (Isr_Mode != 0U)
		Isr_Flag = Isr_Mode;
}

static void Bench_Isr_Handler(void)
{
	ZPR_TEST_INT_CLEAR();
	Bench_Isr_Hook();
}

static void Bench_Isr_Row(const char *name, uint32_t mode)
{
	uint32_t count;

	Bench_Stats_Reset();
	for (count = 0U; count < BENCH_ROUNDS; ++count) {
		Isr_Flag = 0U;
		Isr_Mode = mode;
		while (Isr_Flag != mode) {
		}
		CntEnd = Bench_Cycles();
		CntStart = Isr_Stamp;
		Bench_Stats_Record();
	}
	Isr_Mode = 0U;
	Bench_Report(name);
}

static void Bench_Report(const char *name)
{
	printk("%s : %u / %u / %u\r\n", name,
	       (unsigned int) (Total / BENCH_ROUNDS),
	       (unsigned int) Maximum, (unsigned int) Minimum);
}

static void Yield_Worker(void *p1, void *p2, void *p3)
{
	ARG_UNUSED(p1);
	ARG_UNUSED(p2);
	ARG_UNUSED(p3);

	while (Worker_Stop == 0U) {
		k_yield();
		if ((Yield_Active != 0U) && (Yield_Rounds < BENCH_ROUNDS)) {
			Bench_Stats_Record_Diff(Bench_Cycles() - Yield_Start);
			++Yield_Rounds;
		}
	}
}

static void Idle_Worker(void *p1, void *p2, void *p3)
{
	ARG_UNUSED(p1);
	ARG_UNUSED(p2);
	ARG_UNUSED(p3);

	while (Worker_Stop == 0U)
		k_yield();
}

static void Bmq_Worker(void *p1, void *p2, void *p3)
{
	uint32_t value;

	ARG_UNUSED(p1);
	ARG_UNUSED(p2);
	ARG_UNUSED(p3);

	while (Worker_Stop == 0U) {
		if (k_msgq_get(&Bench_Bmq, &value, K_FOREVER) == 0)
			++Worker_Count;
	}
}

static void Bench_Yield(void)
{
	uint32_t count;

	Worker_Stop = 0U;
	Yield_Active = 0U;
	Yield_Rounds = 0U;
	k_thread_create(&Worker, Worker_Stack, K_THREAD_STACK_SIZEOF(Worker_Stack),
			Yield_Worker, NULL, NULL, NULL,
			K_PRIO_PREEMPT(0), 0, K_NO_WAIT);

	/*
	 * Let the worker reach its first k_yield() and return to us, so the
	 * measured interval starts with a single producer->consumer switch.
	 */
	k_yield();

	Bench_Stats_Reset();
	Yield_Active = 1U;
	for (count = 0U; count < BENCH_ROUNDS; ++count) {
		Yield_Start = Bench_Cycles();
		k_yield();
	}
	while (Yield_Rounds < BENCH_ROUNDS)
		k_yield();

	Yield_Active = 0U;
	Worker_Stop = 1U;
	k_yield();
	Bench_Report("Yield");
}

static void Bench_Activation(void)
{
	uint32_t count;

	Worker_Stop = 0U;
	k_thread_create(&Worker, Worker_Stack, K_THREAD_STACK_SIZEOF(Worker_Stack),
			Idle_Worker, NULL, NULL, NULL,
			K_PRIO_PREEMPT(0), 0, K_NO_WAIT);

	Bench_Stats_Reset();
	for (count = 0U; count < BENCH_ROUNDS; ++count) {
		CntStart = Bench_Cycles();
		k_thread_suspend(&Worker);
		k_thread_resume(&Worker);
		CntEnd = Bench_Cycles();
		Bench_Stats_Record();
	}
	Worker_Stop = 1U;
	k_yield();
	Bench_Report("Activation");
}

static void Bench_Notification(void)
{
	uint32_t count;
	uint32_t value;

	Bench_Stats_Reset();
	for (count = 0U; count < BENCH_ROUNDS; ++count) {
		CntStart = Bench_Cycles();
		k_event_post(&Bench_Event, 1U);
		value = k_event_wait(&Bench_Event, 1U, false, K_FOREVER);
		k_event_clear(&Bench_Event, 1U);
		CntEnd = Bench_Cycles();
		Bench_Stats_Record();
		if (value != 1U)
			printk("Notification mismatch\r\n");
	}
	Bench_Report("Notification");
}

static void Bench_Semaphore(void)
{
	uint32_t count;

	Bench_Stats_Reset();
	for (count = 0U; count < BENCH_ROUNDS; ++count) {
		CntStart = Bench_Cycles();
		k_sem_give(&Bench_Sem);
		k_sem_take(&Bench_Sem, K_FOREVER);
		CntEnd = Bench_Cycles();
		Bench_Stats_Record();
	}
	Bench_Report("Semaphore");
}

static void Bench_Message_Queue(void)
{
	uint32_t count;
	uint32_t value;

	Bench_Stats_Reset();
	for (count = 0U; count < BENCH_ROUNDS; ++count) {
		CntStart = Bench_Cycles();
		k_msgq_put(&Bench_Msgq, &count, K_NO_WAIT);
		k_msgq_get(&Bench_Msgq, &value, K_FOREVER);
		CntEnd = Bench_Cycles();
		Bench_Stats_Record();
	}
	Bench_Report("Message queue");
}

static void Bench_Blocking_Message_Queue(void)
{
	uint32_t count;
	uint32_t value = 0x5A5A5A5AU;

	Worker_Stop = 0U;
	Worker_Count = 0U;
	k_thread_create(&Worker, Worker_Stack, K_THREAD_STACK_SIZEOF(Worker_Stack),
			Bmq_Worker, NULL, NULL, NULL,
			K_PRIO_PREEMPT(0), 0, K_NO_WAIT);

	Bench_Stats_Reset();
	for (count = 0U; count < BENCH_ROUNDS; ++count) {
		while (Worker_Count != count)
			k_yield();
		CntStart = Bench_Cycles();
		k_msgq_put(&Bench_Bmq, &value, K_FOREVER);
		CntEnd = Bench_Cycles();
		Bench_Stats_Record();
	}
	Worker_Stop = 1U;
	k_yield();
	Bench_Report("Blocking message queue");
}

static void Bench_Fifo(void)
{
	uint32_t count;

	Bench_Stats_Reset();
	for (count = 0U; count < BENCH_ROUNDS; ++count) {
		Bench_Fifo_Item = count;
		CntStart = Bench_Cycles();
		k_queue_append(&Bench_Fifo_Queue, &Bench_Fifo_Item);
		(void) k_queue_get(&Bench_Fifo_Queue, K_FOREVER);
		CntEnd = Bench_Cycles();
		Bench_Stats_Record();
	}
	Bench_Report("FIFO");
}

static void Bench_Alarm(void)
{
	uint32_t count;

	Bench_Stats_Reset();
	for (count = 0U; count < BENCH_ROUNDS; ++count) {
		CntStart = Bench_Cycles();
		k_timer_start(&Bench_Timer, K_MSEC(1), K_NO_WAIT);
		k_timer_stop(&Bench_Timer);
		CntEnd = Bench_Cycles();
		Bench_Stats_Record();
	}
	Bench_Report("Alarm");
}

static void Bench_Memory(void)
{
	uint32_t count;
	void *block;

	Bench_Stats_Reset();
	for (count = 0U; count < BENCH_ROUNDS; ++count) {
		CntStart = Bench_Cycles();
		block = k_malloc(32U);
		k_free(block);
		CntEnd = Bench_Cycles();
		Bench_Stats_Record();
	}
	Bench_Report("Memory");
}

int main(void)
{
	printk("\r\nZPR: Zephyr-on-RVM benchmark\r\n");
	printk("Test (number in CPU cycles)        : AVG / MAX / MIN\r\n");

	ZPR_Test_Timer_Init();
	k_sem_init(&Bench_Sem, 0, 1);
	k_event_init(&Bench_Event);
	k_msgq_init(&Bench_Msgq, Bench_Msgq_Buf, sizeof(uint32_t), 8U);
	k_msgq_init(&Bench_Bmq, Bench_Bmq_Buf, sizeof(uint32_t), 1U);
	k_queue_init(&Bench_Fifo_Queue);
	k_timer_init(&Bench_Timer, NULL, NULL);

	Bench_Yield();
	Bench_Notification();
	Bench_Activation();
	Bench_Semaphore();
	Bench_Message_Queue();
	Bench_Blocking_Message_Queue();
	Bench_Fifo();
	Bench_Alarm();
	Bench_Memory();
	ZPR_Test_Int_Init(Bench_Isr_Handler);
	/*
	 * The six rows below all execute the same TIM4 vector hook.  The mode
	 * only tags the row so the benchmark keeps the 15-row layout shared
	 * with the other RVM guests; it does not select a different ISR path.
	 */
	printk("ISR rows: one vector hook, six labels (poll)\r\n");
	Bench_Isr_Row("ISR Notification (poll)", 1U);
	Bench_Isr_Row("ISR RVM activation relay (poll)", 2U);
	Bench_Isr_Row("ISR Semaphore (poll)", 3U);
	Bench_Isr_Row("ISR Message queue (poll)", 4U);
	Bench_Isr_Row("ISR Blocking message queue (poll)", 5U);
	Bench_Isr_Row("ISR Activation (poll)", 6U);
	ZPR_Test_Int_Disable();

	printk("Benchmark complete\r\n");
	return 0;
}
