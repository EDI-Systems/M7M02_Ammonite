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
 * The board support of the DjyOS-on-RVM benchmark: the two timers the
 * tests measure with (TIM2 the cycle counter, TIM4 the interrupt
 * source) and the boot sequence that registers the benchmark's event
 * types before the scheduler starts.  The glue the kernel itself needs
 * lives in the port's board.c, so this file - and the chip test header
 * it includes - is compiled into benchmark builds only.
 */

#include "rvm.h"
#include "djy_test.h"
#include "djyos.h"
#include "int.h"

/* The port's register-transfer helpers and boot sequence. */
extern void djy_dbg_puts(const char *str);
extern bool_t djy_attach_phys(rvm_ptr_t phys_num, rvm_ptr_t vct_num);
extern void djy_arm_handlers(void);
extern void djy_port_boot(void);
extern void djy_port_start(void);

/* Benchmark entry, in djy_benchmark.c. */
extern void djy_bench_boot(void);
extern u32 djy_bench_isr(ptu32_t para);

/*
 * The board glue for the RVM interrupt source: connect the ISR to the
 * DjyOS line (async signal, auto clear type), then map the physical
 * timer vector through the hypervisor.  Called by the benchmark right
 * before the interrupt-driven phases.
 */
void djy_bench_int_init(void)
{
    /* Register the line first: the kernel allocates the struct IntLine
     * on the heap and fills tg_pIntLineTable[] - without it every
     * Int_* call below silently no-ops and the first vector delivery
     * dereferences a NULL slot. */
    if(Int_Register(DJY_VCT_TIM4) == false)
    {
        djy_dbg_puts("djy: failed to register the interrupt line.\r\n");
        while(1);
    }
    Int_IsrConnect(DJY_VCT_TIM4, djy_bench_isr);
    Int_SetIsrPara(DJY_VCT_TIM4, 0U);
    Int_SetClearType(DJY_VCT_TIM4, CN_INT_CLEAR_USER);
    Int_EnableLine(DJY_VCT_TIM4);

    if(djy_attach_phys(DJY_PHYS_TIM4, DJY_VCT_TIM4) == false)
    {
        djy_dbg_puts("djy: failed to attach the interrupt source.\r\n");
        while(1);
    }

    /* Re-arm right before the source starts: a late vector-thread init
     * must not be able to leave the line unregistered. */
    djy_arm_handlers();

    /* TIM4 - the interrupt source, at the chip header defined rate. */
    DJY_RCC_APB1ENR |= DJY_RCC_APB1ENR_TIM4;
    DJY_TIM4_PSC = 0U;
    DJY_TIM4_ARR = DJY_TIM4_RELOAD;
    DJY_TIM4_SR = 0U;
    DJY_TIM4_DIER = DJY_TIM4_DIER_UIE;
    DJY_TIM4_CR1 = DJY_TIM4_CR1_CEN;
}

/*
 * djy_main - the benchmark's VM entry: run the port's boot sequence,
 * start the timing counter, register the event types and start the
 * multi event scheduling.  The startup context is abandoned inside
 * __DJY_StartOs (through djy_port_start).
 */
void djy_main(void)
{
    djy_port_boot();

    /* TIM2 - free running benchmark counter. */
    DJY_RCC_APB1ENR |= DJY_RCC_APB1ENR_TIM2;
    DJY_TIM2_PSC = 0U;
    DJY_TIM2_ARR = 0xFFFFFFFFUL;
    DJY_TIM2_CR1 = 1UL;

    djy_dbg_puts("[7]");

    djy_bench_boot();       /* register the event types */
    djy_dbg_puts("[b0]");

    djy_port_start();       /* never returns */
}

/* End Of File ***************************************************************/
