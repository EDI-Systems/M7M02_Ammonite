/*
    ChibiOS 21.11 LTS default application for the RVM hypervisor.
    Copyright (C) 2026 Evo-Devo Instrum. All rights reserved.

    This file is NOT part of the ChibiOS distribution.  It belongs to the
    ChibiOS-on-RVM test suite and is released under the Unlicense; refer to
    the LICENSE file for details.
*/

#include "ch.h"
#include "rvm.h"

static THD_WORKING_AREA(waBlink, 512);

static void chb_puts(const char *s) {
  while (*s != '\0') {
    RVM_Hyp_Putchar(*s++);
  }
}

static void blink_thread(void *arg) {
  (void)arg;

  chb_puts("\r\nCHB: ChibiOS-on-RVM main thread\r\n");
  while (true) {
    RVM_Hyp_Putchar('.');
    chThdSleepMilliseconds(500);
  }
}

int main(void) {
  chSysInit();

  (void)chThdCreateStatic(waBlink, sizeof(waBlink), NORMALPRIO,
                          blink_thread, NULL);

  while (true) {
    chThdSleepMilliseconds(1000);
  }
}
