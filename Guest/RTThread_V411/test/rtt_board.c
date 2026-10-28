/*
 * RT-Thread V4.1.1 board support for the RVM virtual machines.
 * Copyright (C) 2026 Evo-Devo Instrum. All rights reserved.
 *
 * This file is NOT part of the RT-Thread distribution.  It belongs to the
 * RT-Thread-on-RVM test suite and is released under the Unlicense; refer
 * to the LICENSE file for details.  The RT-Thread kernel itself is
 * copyright RT-Thread Development Team and is not modified by this file.
 *
 * This file plays the role of the board support package of an official
 * RT-Thread BSP:  it hooks the kernel tick and the context switch handler
 * up to the RVM hypervisor, initializes the heap, and routes the console
 * output through the RVM debug channel.  It is compiled into every
 * RT-Thread guest, benchmark or not.
 */

#include <rtthread.h>
#include "rvm.h"
#include "rtt_layout_knob.h"

/*-----------------------------------------------------------*/

/* The context switch handler of the port, implemented in cpuport.c - the
 * board hooks it up to the hypervisor below, playing the role that the
 * PendSV vector plays on the official ports. */
extern void PendSV_Handler(void);

/*-----------------------------------------------------------*/

/* The heap that the allocations are carved from - the RVM virtual machine
 * has no linker-exported heap boundary symbols, so the heap is a plain
 * static array sized by the rtconfig.h. */
#define RT_HEAP_SIZE_IF_NOT_DEFINED 4096
#if !defined(RT_HEAP_SIZE)
#define RT_HEAP_SIZE RT_HEAP_SIZE_IF_NOT_DEFINED
#endif
static rt_uint8_t rt_heap[RT_HEAP_SIZE];

/*-----------------------------------------------------------*/

/*
 * The kernel tick.  The RVM hypervisor delivers the tick on its virtual
 * timer channel; the handler is registered in rt_hw_board_init() below
 * and carries the standard RT-Thread tick protocol.
 */
void SysTick_Handler(void)
{
    /* enter interrupt */
    rt_interrupt_enter();

    rt_tick_increase();

    /* leave interrupt */
    rt_interrupt_leave();
}

/**
 * This function will initial STM32 board.
 */
void rt_hw_board_init(void)
{
    /* Layout knob echo - see the chip header; the string length is the knob. */
    rt_kprintf("LK:%s|\n", RTT_LAYOUT_KNOB);

    /* Hook the tick handler up to the hypervisor's virtual timer channel,
     * and the context switch handler up to the hypervisor's context switch
     * channel - the RVM equivalent of the SysTick and PendSV vectors of
     * the official ports. */
    RVM_Virt_Tim_Reg(SysTick_Handler);
    RVM_Virt_Ctx_Reg(PendSV_Handler);

#ifdef RT_USING_HEAP
    rt_system_heap_init(rt_heap, rt_heap + sizeof(rt_heap));
#endif
}

/**
 * This function will put string to the console - the RVM debug channel.
 *
 * @param str the string to be printed.
 */
void rt_hw_console_output(const char *str)
{
    while (*str != '\0')
    {
        RVM_Hyp_Putchar(*str++);
    }
}
