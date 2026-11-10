/*
 * DjyOS V1.1.0 RVM port - Copyright (C) 2026 Evo-Devo Instrum.
 * All rights reserved
 *
 * This file is NOT part of the DjyOS distribution.  It belongs to the
 * DjyOS-on-RVM test suite and is released under the Unlicense; refer
 * to the LICENSE file for details.  The DjyOS kernel itself is
 * copyright DjyOS Open Source Development team and is not modified by
 * this file.
 *
 * The benchmark's VM entry: run the port's boot sequence, start the
 * timing counter through the chip test header, register the benchmark's
 * event types and start the scheduler.  Every chip-related register
 * access and the DjyOS interrupt-line wiring lives in
 * chip/djy_test_stm32h743zi_rvm.h, following the RMP test-header split.
 */

#include "djy_test.h"

extern void djy_port_boot(void);
extern void djy_port_start(void);

/* Benchmark entry, in djy_benchmark.c. */
extern void djy_bench_boot(void);

/*
 * djy_main - the benchmark's VM entry.  The startup context is abandoned
 * inside __DJY_StartOs (through djy_port_start).
 */
void djy_main(void)
{
    djy_port_boot();

    djy_bench_timer_init();

    djy_bench_boot();       /* register the event types */

    djy_port_start();       /* never returns */
}

/* End Of File ***************************************************************/
