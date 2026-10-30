/*
 * RT-Thread V4.1.1 RVM port - Copyright (C) 2026 Evo-Devo Instrum.
 * All rights reserved
 *
 * This file is NOT part of the RT-Thread distribution.  It belongs to the
 * RT-Thread-on-RVM test suite and is released under the Unlicense; refer
 * to the LICENSE file for details.  The RT-Thread kernel itself is
 * copyright RT-Thread Development Team and is not modified by this file.
 *
 * The default configuration of the RT-Thread-on-RVM guests: the small
 * memory allocator, the main thread, and nothing else.  This template is
 * copied into each generated virtual machine as its "rtconfig.h"; the
 * copy belongs to the project and can be edited freely - it is never
 * replaced unless the regeneration is forced.
 */

#ifndef __RTCONFIG_H__
#define __RTCONFIG_H__

// <o>the max length of object name<2-16>
//  <i>Default: 8
#define RT_NAME_MAX                    8

/* The IAR and the ARM compilers do not reach the GCC branch of rtdef.h
 * where the builtin va_list fallback lives (armclang defines
 * __ARMCC_VERSION and takes the ARM Compiler branch first), and rtdef.h
 * only pulls <stdarg.h> under this macro - so every toolchain except
 * plain GCC needs the libc path here.  The proven GCC configurations
 * keep the builtin path untouched. */
#if !defined(__GNUC__) || defined(__ARMCC_VERSION)
#define RT_USING_LIBC
#endif

// <o>Alignment size for CPU architecture data access
//  <i>Default: 8 (the AAPCS stack alignment)
#define RT_ALIGN_SIZE                  8

// <o>the maximum level of thread priority<1-256>
//  <i>Default: 32
#define RT_THREAD_PRIORITY_MAX         32

// <o>the frequency of kernel tick<1-1000>
//  <i>Default: 1000  (1ms)
#define RT_TICK_PER_SECOND             1000

// <c1>Using the small memory first-fit allocator - the recommended heap
//  <i>implementation for the small RAM systems
#define RT_USING_SMALL_MEM
// </c>
// <c1>Use the small memory allocator as the heap
#define RT_USING_SMALL_MEM_AS_HEAP
// </c>
// <c1>Using heap
#define RT_USING_HEAP
// </c>

// <c1>Using semaphore
#define RT_USING_SEMAPHORE
// </c>
// <c1>Using mailbox
#define RT_USING_MAILBOX
// </c>
// <c1>Using message queue
#define RT_USING_MESSAGEQUEUE
// </c>

// <c1>Using the idle hook - the application registers the hypervisor
//  <i>parking hook, so the idle thread must be able to carry hooks
#define RT_USING_HOOK
#define RT_USING_IDLE_HOOK
// </c>

// <o>the heap size of the RVM guest<128-8192>
//  <i>Default: 6144 - the board initialization carves the allocations from it
#define RT_HEAP_SIZE                   6144

// <c1>Using console - the board support routes it through the RVM debug
//  <i>channel; the port's error trap prints through rt_kprintf, which the
//  <i>headers compile into an empty inline function without this switch.
#define RT_USING_CONSOLE
// </c>
// <o>the buffer size of console<1-1024>
//  <i>Default: 128
#define RT_CONSOLEBUF_SIZE             128
// <c1>rt_kprintf supports long long
#define RT_KPRINTF_USING_LONGLONG
// </c>

/* The idle thread stack.  NOTE: RT-Thread V4.1.1's idle.c sizes its stack
 * with IDLE_THREAD_STACK_SIZE, NOT with RT_THREAD_IDLE_STACK_SIZE - the
 * usual knob is silently ignored, and the default 256 bytes are far too
 * small for the idle work on this port: the defunct thread reclamation
 * (rt_thread_idle_excute -> rt_thread_detach -> rt_free) overflows it and
 * corrupts whatever sits below, which included the freed main thread's
 * heap block and, through it, kernel list nodes. */
// <o>the stack size of idle thread<256-4096>
//  <i>Default: 512 (both knobs are kept in sync; only this one counts)
#define IDLE_THREAD_STACK_SIZE         512
#define RT_THREAD_IDLE_STACK_SIZE      512

// <c1>Using user main
#define RT_USING_USER_MAIN
// </c>
// <o>the stack size of main thread<512-4096>
//  <i>Default: 1024 (512 overflows in the rt_kprintf/vsnprintf chain)
#define RT_MAIN_THREAD_STACK_SIZE      1024

#endif
