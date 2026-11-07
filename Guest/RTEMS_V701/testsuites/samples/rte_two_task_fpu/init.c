/* SPDX-License-Identifier: Unlicense */

/*
 * RTE multitask and hard-float test for the RVM ARMv7-M port.
 *
 * Two floating point tasks at different priorities carry eight doubles each
 * (d8-d15, the callee-saved FP registers).  The values are perturbed before
 * each block and checked against an arithmetic model after the wake-up.  A
 * lost or mixed FP context changes the checksum immediately.  The idle task
 * parks the RVM user thread in the hypervisor, as the other RVM guests do.
 */

#include <rtems.h>
#include <stdint.h>
#include <stdio.h>

#include "rvm.h"

static void *rte_idle_body(uintptr_t ignored)
{
  (void) ignored;

  for (;;) {
    RVM_Hyp_Vct_Wait();
  }

  return NULL;
}

static void fp_high(rtems_task_argument arg)
{
  double d0 = 1.0, d1 = 2.0, d2 = 3.0, d3 = 4.0;
  double d4 = 5.0, d5 = 6.0, d6 = 7.0, d7 = 8.0;
  uint32_t iter = 0;

  (void) arg;

  for (;;) {
    double sum =
      d0 * 1.0 + d1 * 2.0 + d2 * 3.0 + d3 * 4.0 +
      d4 * 5.0 + d5 * 6.0 + d6 * 7.0 + d7 * 8.0;
    int scaled = (int) (sum * 2.0);
    int expected = 408 + 36 * (int) iter;

    if (scaled != expected) {
      printf("FP FAIL H %lu %d %d\n",
        (unsigned long) iter, scaled, expected);
    } else if ((iter % 10U) == 0U) {
      printf("H %lu %d\n", (unsigned long) iter, scaled);
    }

    d0 += 0.5; d1 += 0.5; d2 += 0.5; d3 += 0.5;
    d4 += 0.5; d5 += 0.5; d6 += 0.5; d7 += 0.5;
    ++iter;

    rtems_task_wake_after(1);
  }
}

static void fp_low(rtems_task_argument arg)
{
  double d0 = 100.0, d1 = 200.0, d2 = 300.0, d3 = 400.0;
  double d4 = 500.0, d5 = 600.0, d6 = 700.0, d7 = 800.0;
  uint32_t iter = 0;

  (void) arg;

  for (;;) {
    double sum =
      d0 * 1.0 + d1 * 2.0 + d2 * 3.0 + d3 * 4.0 +
      d4 * 5.0 + d5 * 6.0 + d6 * 7.0 + d7 * 8.0;
    int scaled = (int) (sum * 4.0);
    int expected = 81600 + 36 * (int) iter;

    if (scaled != expected) {
      printf("FP FAIL L %lu %d %d\n",
        (unsigned long) iter, scaled, expected);
    } else if ((iter % 10U) == 0U) {
      printf("L %lu %d\n", (unsigned long) iter, scaled);
    }

    d0 += 0.25; d1 += 0.25; d2 += 0.25; d3 += 0.25;
    d4 += 0.25; d5 += 0.25; d6 += 0.25; d7 += 0.25;
    ++iter;

    rtems_task_wake_after(1);
  }
}

rtems_task Init(rtems_task_argument ignored);

rtems_task Init(rtems_task_argument ignored)
{
  rtems_id id;
  rtems_status_code sc;

  (void) ignored;

  printf("\nRTE multitask FP test\n");

  sc = rtems_task_create(
    rtems_build_name('F', 'P', 'H', 'I'),
    1,
    8192,
    RTEMS_DEFAULT_MODES,
    RTEMS_FLOATING_POINT,
    &id
  );
  if (sc != RTEMS_SUCCESSFUL) {
    printf("create H failed %d\n", (int) sc);
  } else {
    rtems_task_start(id, fp_high, 0);
  }

  sc = rtems_task_create(
    rtems_build_name('F', 'P', 'L', 'O'),
    2,
    8192,
    RTEMS_DEFAULT_MODES,
    RTEMS_FLOATING_POINT,
    &id
  );
  if (sc != RTEMS_SUCCESSFUL) {
    printf("create L failed %d\n", (int) sc);
  } else {
    rtems_task_start(id, fp_low, 0);
  }

  rtems_task_delete(RTEMS_SELF);
}

#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER

#define CONFIGURE_MAXIMUM_TASKS 8
#define CONFIGURE_RTEMS_INIT_TASKS_TABLE
#define CONFIGURE_INIT_TASK_STACK_SIZE 8192
#define CONFIGURE_IDLE_TASK_BODY rte_idle_body
#define CONFIGURE_IDLE_TASK_STACK_SIZE 4096
#define CONFIGURE_MICROSECONDS_PER_TICK 100000

#define CONFIGURE_INIT
#include <rtems/confdefs.h>
