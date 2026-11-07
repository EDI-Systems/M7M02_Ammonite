/* SPDX-License-Identifier: Unlicense */

/*
 * RTE tick test for the RVM ARMv7-M port.
 *
 * The RVM monitor delivers its virtual timer to the guest vector thread; the
 * RTE clock driver turns each delivery into a RTEMS clock tick.  The Init task
 * polls the tick counter and prints every 100 ticks.  It must not block: the
 * RVM context switch path is implemented in Phase 3.
 */

#include <rtems.h>
#include <stdio.h>

rtems_task Init(rtems_task_argument);

rtems_task Init(rtems_task_argument ignored)
{
  rtems_interval last = 0;

  (void) ignored;

  printf("\nRTE tick test\n");

  for (;;) {
    rtems_interval ticks = rtems_clock_get_ticks_since_boot();

    if ((rtems_interval) (ticks - last) >= 10) {
      last = ticks;
      printf("tick %lu\n", (unsigned long) ticks);
    }
  }
}

#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER

#define CONFIGURE_MAXIMUM_TASKS 1
#define CONFIGURE_RTEMS_INIT_TASKS_TABLE
#define CONFIGURE_INIT_TASK_STACK_SIZE 8192
#define CONFIGURE_MICROSECONDS_PER_TICK 100000

#define CONFIGURE_INIT
#include <rtems/confdefs.h>
