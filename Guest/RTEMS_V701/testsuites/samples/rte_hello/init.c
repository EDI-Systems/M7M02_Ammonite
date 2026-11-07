/* SPDX-License-Identifier: Unlicense */

/*
 * Minimal RTE guest application for the RVM ARMv7-M port.
 *
 * Phase 1: one Init task, no clock driver, console output through the RVM
 * debug console.  The task must not return or block, otherwise RTEMS would
 * switch to the idle task and exercise the context switch machinery that is
 * implemented in Phase 3.
 */

#include <rtems.h>
#include <stdio.h>

rtems_task Init(rtems_task_argument);

rtems_task Init(rtems_task_argument ignored)
{
  (void) ignored;

  printf("\nRTE on RVM: Hello World\n");

  for (;;) {
    /* Keep the Init task running; no context switch in Phase 1. */
  }
}

#define CONFIGURE_APPLICATION_DOES_NOT_NEED_CLOCK_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER

#define CONFIGURE_MAXIMUM_TASKS 1
#define CONFIGURE_RTEMS_INIT_TASKS_TABLE
#define CONFIGURE_INIT_TASK_STACK_SIZE 8192

#define CONFIGURE_INIT
#include <rtems/confdefs.h>
