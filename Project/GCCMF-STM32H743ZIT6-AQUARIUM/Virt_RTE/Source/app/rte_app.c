/* Aquarium heartbeat for RTEMS on RVM: the Init task prints one line every
 * 1 s (15 ticks at the port's 10 Hz tick) and never exits; the idle task
 * parks the RVM user thread in the hypervisor, as the other RVM guests do. */
#include <rtems.h>
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

rtems_task Init(rtems_task_argument ignored);

rtems_task Init(rtems_task_argument ignored)
{
  int n = 0;

  (void) ignored;

  for (;;) {
    printf("\r\n[RTE] alive %d\r\n", n);
    n++;
    rtems_task_wake_after(10);
  }
}

#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER

#define CONFIGURE_MAXIMUM_TASKS 4
#define CONFIGURE_RTEMS_INIT_TASKS_TABLE
#define CONFIGURE_INIT_TASK_STACK_SIZE 8192
#define CONFIGURE_IDLE_TASK_BODY rte_idle_body
#define CONFIGURE_IDLE_TASK_STACK_SIZE 4096
#define CONFIGURE_MICROSECONDS_PER_TICK 100000

#define CONFIGURE_INIT
#include <rtems/confdefs.h>
