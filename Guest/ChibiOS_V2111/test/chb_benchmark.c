/*
    CHB benchmark entry point for the RVM hypervisor.
    Copyright (C) 2026 Evo-Devo Instrum. All rights reserved.

    This file is NOT part of the ChibiOS distribution.  It belongs to the
    ChibiOS-on-RVM test suite and is released under the Unlicense; refer to
    the LICENSE file for details.
*/

#include "ch.h"
#include "rvm.h"
#include "chb_test.h"

#define CHB_ROUNDS          10000U
#define CHB_MEM_BLOCKS      8U
#define CHB_MEM_BLOCK_SIZE  32U

typedef struct {
  uint32_t total;
  uint32_t max;
  uint32_t min;
} chb_stat_t;

static chb_stat_t yield_stat;
static chb_stat_t notify_stat;
static chb_stat_t sem_stat;
static chb_stat_t mq_stat;
static chb_stat_t mem_stat;
static chb_stat_t isr_notify_stat;
static chb_stat_t isr_sem_stat;
static chb_stat_t isr_mq_stat;
static chb_stat_t relay_stat;

static volatile uint32_t bench_start;
static volatile uint32_t bench_end;
static volatile uint32_t isr_start;
static volatile uint32_t isr_mode;
static volatile uint32_t isr_count;
static volatile uint32_t isr_armed;
static chb_stat_t *volatile isr_stat;
static thread_t *consumer_tp;
static semaphore_t ack_sem;
static semaphore_t test_sem;
static semaphore_t isr_ready_sem;
static semaphore_t isr_done_sem;
static mailbox_t test_mb;
static msg_t test_mb_buf[8];

#define CHB_ISR_NOTIFY  1U
#define CHB_ISR_SEM     2U
#define CHB_ISR_MQ      3U

/* Thread working area size in bytes; small-RAM devices may shrink this
 * from their chip test header (the switch cost numbers are unaffected). */
#if !defined(CHB_BENCH_WA_SIZE)
#define CHB_BENCH_WA_SIZE                 1024
#endif

static THD_WORKING_AREA(waYieldA, CHB_BENCH_WA_SIZE);
static THD_WORKING_AREA(waYieldB, CHB_BENCH_WA_SIZE);
static THD_WORKING_AREA(waProducer, CHB_BENCH_WA_SIZE);
static THD_WORKING_AREA(waConsumer, CHB_BENCH_WA_SIZE);
static THD_WORKING_AREA(waIsr, CHB_BENCH_WA_SIZE);
#if CHB_USE_FPU
static THD_WORKING_AREA(waFpuA, 1024);
static THD_WORKING_AREA(waFpuB, 1024);
static volatile uint32_t fpu_failures;
#endif

#if defined(__riscv)
static uint8_t heap_area[4096];
#else
static uint8_t heap_area[8192];
#endif
static memory_heap_t heap;

static void chb_putc(char c) {
  RVM_Hyp_Putchar(c);
}

static void chb_puts(const char *s) {
  while (*s != '\0') {
    chb_putc(*s++);
  }
}

static void chb_putu(uint32_t value) {
  char buf[11];
  uint32_t n = 0U;

  if (value == 0U) {
    chb_putc('0');
    return;
  }
  while (value != 0U) {
    buf[n++] = (char)('0' + (value % 10U));
    value /= 10U;
  }
  while (n != 0U) {
    chb_putc(buf[--n]);
  }
}

static void stat_reset(chb_stat_t *s) {
  s->total = 0U;
  s->max = 0U;
  s->min = 0xFFFFFFFFU;
}

static void stat_record(chb_stat_t *s, uint32_t value) {
  s->total += value;
  if (value > s->max) {
    s->max = value;
  }
  if (value < s->min) {
    s->min = value;
  }
}

#if CHB_USE_FPU
static void fpu_thread(void *arg) {
  float acc = 0.0f;

  (void)arg;
  for (uint32_t i = 0U; i < CHB_ROUNDS; i++) {
    acc += 1.0f;
    chThdYield();
  }
  if (acc != (float)CHB_ROUNDS) {
    fpu_failures++;
  }
}

static void fpu_context_test(void) {
  thread_t *a;
  thread_t *b;

  fpu_failures = 0U;
  a = chThdCreateStatic(waFpuA, sizeof(waFpuA), NORMALPRIO + 1U,
                        fpu_thread, NULL);
  b = chThdCreateStatic(waFpuB, sizeof(waFpuB), NORMALPRIO + 1U,
                        fpu_thread, NULL);
  (void)chThdWait(a);
  (void)chThdWait(b);
  chb_puts(fpu_failures == 0U ? "FPU context: PASS\r\n"
                               : "FPU context: FAIL\r\n");
}
#endif

static void report(const char *name, const chb_stat_t *s) {

  const char *p;
  uint32_t n = 0U;

  for (p = name; *p != '\0'; p++) {
    chb_putc(*p);
    n++;
  }
  while (n < 34U) {
    chb_putc(' ');
    n++;
  }
  chb_puts(" : ");
  chb_putu(s->total / CHB_ROUNDS);
  chb_puts(" / ");
  chb_putu(s->max);
  chb_puts(" / ");
  chb_putu(s->min);
  chb_puts("\r\n");
}

static void yield_a(void *arg) {
  (void)arg;

  for (uint32_t i = 0U; i < CHB_ROUNDS; i++) {
    bench_start = CHB_TEST_CNT_READ();
    chThdYield();
  }
}

static void yield_b(void *arg) {
  (void)arg;

  for (uint32_t i = 0U; i < CHB_ROUNDS; i++) {
    chThdYield();
    bench_end = CHB_TEST_CNT_READ();
    stat_record(&yield_stat, (CHB_TEST_CNT_DIFF(bench_end, bench_start)) / 2U);
  }
}

static void bench_yield(void) {
  thread_t *a;
  thread_t *b;

  stat_reset(&yield_stat);
  a = chThdCreateStatic(waYieldA, sizeof(waYieldA), NORMALPRIO,
                        yield_a, NULL);
  b = chThdCreateStatic(waYieldB, sizeof(waYieldB), NORMALPRIO,
                        yield_b, NULL);
  (void)chThdWait(a);
  (void)chThdWait(b);
  report("Yield", &yield_stat);
}

static void producer_notify(void *arg) {
  (void)arg;

  for (uint32_t i = 0U; i < CHB_ROUNDS; i++) {
    bench_start = CHB_TEST_CNT_READ();
    chEvtSignal(consumer_tp, 1U);
    (void)chSemWait(&ack_sem);
  }
}

static void consumer_notify(void *arg) {
  (void)arg;

  for (uint32_t i = 0U; i < CHB_ROUNDS; i++) {
    (void)chEvtWaitOne(1U);
    bench_end = CHB_TEST_CNT_READ();
    stat_record(&notify_stat, CHB_TEST_CNT_DIFF(bench_end, bench_start));
    chSemSignal(&ack_sem);
  }
}

static void producer_sem(void *arg) {
  (void)arg;

  for (uint32_t i = 0U; i < CHB_ROUNDS; i++) {
    bench_start = CHB_TEST_CNT_READ();
    chSemSignal(&test_sem);
    (void)chSemWait(&ack_sem);
  }
}

static void consumer_sem(void *arg) {
  (void)arg;

  for (uint32_t i = 0U; i < CHB_ROUNDS; i++) {
    (void)chSemWait(&test_sem);
    bench_end = CHB_TEST_CNT_READ();
    stat_record(&sem_stat, CHB_TEST_CNT_DIFF(bench_end, bench_start));
    chSemSignal(&ack_sem);
  }
}

static void producer_mq(void *arg) {
  (void)arg;

  for (uint32_t i = 0U; i < CHB_ROUNDS; i++) {
    bench_start = CHB_TEST_CNT_READ();
    (void)chMBPostTimeout(&test_mb, (msg_t)1, TIME_INFINITE);
    (void)chSemWait(&ack_sem);
  }
}

static void consumer_mq(void *arg) {
  msg_t msg;

  for (uint32_t i = 0U; i < CHB_ROUNDS; i++) {
    (void)chMBFetchTimeout(&test_mb, &msg, TIME_INFINITE);
    bench_end = CHB_TEST_CNT_READ();
    stat_record(&mq_stat, CHB_TEST_CNT_DIFF(bench_end, bench_start));
    chSemSignal(&ack_sem);
  }
}

static void bench_thread_pair(void (*producer)(void *),
                              void (*consumer)(void *),
                              chb_stat_t *stat,
                              const char *name) {
  thread_t *p;
  thread_t *c;

  stat_reset(stat);
  chSemObjectInit(&ack_sem, 0U);
  c = chThdCreateStatic(waConsumer, sizeof(waConsumer), NORMALPRIO + 1U,
                        consumer, NULL);
  consumer_tp = c;
  p = chThdCreateStatic(waProducer, sizeof(waProducer), NORMALPRIO,
                        producer, NULL);
  (void)chThdWait(p);
  (void)chThdWait(c);
  report(name, stat);
}

static void bench_memory(void) {
  void *blocks[CHB_MEM_BLOCKS];
  uint32_t start;
  uint32_t end;

  chHeapObjectInit(&heap, heap_area, sizeof(heap_area));
  stat_reset(&mem_stat);

  for (uint32_t i = 0U; i < CHB_ROUNDS; i++) {
    start = CHB_TEST_CNT_READ();
    for (uint32_t j = 0U; j < CHB_MEM_BLOCKS; j++) {
      blocks[j] = chHeapAlloc(&heap, CHB_MEM_BLOCK_SIZE);
    }
    for (uint32_t j = 0U; j < CHB_MEM_BLOCKS; j++) {
      chHeapFree(blocks[j]);
    }
    end = CHB_TEST_CNT_READ();
    stat_record(&mem_stat, CHB_TEST_CNT_DIFF(end, start) / CHB_MEM_BLOCKS);
  }
  report("Memory allocation/free pair", &mem_stat);
}

void chb_test_int_handler(void) {
  uint32_t now = CHB_TEST_CNT_READ();

  if ((isr_mode == 0U) || (isr_armed == 0U)) {
    PORT_IRQ_EPILOGUE();
    return;
  }

  if (isr_mode == CHB_ISR_NOTIFY) {
    uint16_t relay = (uint16_t)now - (uint16_t)CHB_TEST_KERNEL_STAMP();
    stat_record(&relay_stat, relay);
  }

  if (isr_count < CHB_ROUNDS) {
    isr_start = now;
    isr_count++;

    chSysLockFromISR();
    if (isr_mode == CHB_ISR_NOTIFY) {
      chEvtSignalI(consumer_tp, 1U);
    }
    else if (isr_mode == CHB_ISR_SEM) {
      chSemSignalI(&test_sem);
    }
    else {
      (void)chMBPostI(&test_mb, (msg_t)1);
    }
    chSysUnlockFromISR();
  }
  PORT_IRQ_EPILOGUE();
}

static void isr_consumer(void *arg) {
  msg_t msg;

  (void)arg;

  chSemSignal(&isr_ready_sem);
  for (uint32_t i = 0U; i < CHB_ROUNDS; i++) {
    if (isr_mode == CHB_ISR_NOTIFY) {
      (void)chEvtWaitOne(1U);
    }
    else if (isr_mode == CHB_ISR_SEM) {
      (void)chSemWait(&test_sem);
    }
    else {
      (void)chMBFetchTimeout(&test_mb, &msg, TIME_INFINITE);
    }
    bench_end = CHB_TEST_CNT_READ();
    stat_record(isr_stat, CHB_TEST_CNT_DIFF(bench_end, isr_start));
  }
  chSemSignal(&isr_done_sem);
}

static void bench_isr(uint32_t mode, chb_stat_t *stat, const char *name) {
  thread_t *c;

  isr_armed = 0U;
  isr_mode = mode;
  isr_stat = stat;
  isr_count = 0U;
  stat_reset(stat);
  chSemObjectInit(&isr_ready_sem, 0U);
  chSemObjectInit(&isr_done_sem, 0U);
  chSemObjectInit(&test_sem, 0U);
  chMBObjectInit(&test_mb, test_mb_buf, 8U);

  c = chThdCreateStatic(waIsr, sizeof(waIsr), NORMALPRIO + 1U,
                        isr_consumer, NULL);
  consumer_tp = c;
  (void)chSemWait(&isr_ready_sem);
  isr_count = 0U;
  isr_armed = 1U;
  (void)chSemWait(&isr_done_sem);
  isr_armed = 0U;
  (void)chThdWait(c);

  /* The row's statistics are complete but are not printed yet: every ISR
   * row stays in its caller-owned stat block and the whole interrupt phase
   * is printed as one block after chb_test_int_disable(), matching the
   * FRT/UO3/RTE/ZPR/NTX convention.  Printing row by row would emit dozens
   * of console hypercalls while the 10 kHz source is still firing. */
  (void)name;
  (void)stat;
}

int main(void) {
  chSysInit();

  chb_puts("\r\nCHB: ChibiOS-on-RVM benchmark\r\n");
  chb_puts("====================================================\r\n");
  chb_puts("Test (number in CPU cycles)        : AVG / MAX / MIN\r\n");

  chb_test_timer_init();

#if CHB_USE_FPU
  fpu_context_test();
#endif


  bench_yield();
  bench_thread_pair(producer_notify, consumer_notify, &notify_stat,
                    "Notification");
  chSemObjectInit(&test_sem, 0U);
  chMBObjectInit(&test_mb, test_mb_buf, 8U);
  bench_thread_pair(producer_sem, consumer_sem, &sem_stat,
                    "Semaphore");
  bench_thread_pair(producer_mq, consumer_mq, &mq_stat,
                    "Message queue");
  bench_memory();

  stat_reset(&relay_stat);
  chb_test_int_init(chb_test_int_handler);
  bench_isr(CHB_ISR_NOTIFY, &isr_notify_stat, "ISR Notification");
  bench_isr(CHB_ISR_SEM, &isr_sem_stat, "ISR Semaphore");
  bench_isr(CHB_ISR_MQ, &isr_mq_stat, "ISR Message queue");
  /* The interrupt source stops before anything is printed: no hypercall
   * bursts and no vector round trips can pollute the phase or the console. */
  chb_test_int_disable();

  report("ISR Notification", &isr_notify_stat);
  report("ISR Semaphore", &isr_sem_stat);
  report("ISR Message queue", &isr_mq_stat);
  report("ISR RVM activation relay", &relay_stat);

  chb_puts("Benchmark complete\r\n");
  for (;;) {
    chThdSleepMilliseconds(1000);
  }
}
