/*
 * RT-Thread V4.1.1 RVM port - Copyright (C) 2026 Evo-Devo Instrum.
 * All rights reserved
 *
 * This file is NOT part of the RT-Thread distribution.  It belongs to the
 * RT-Thread-on-RVM test suite and is released under the Unlicense; refer
 * to the LICENSE file for details.  The RT-Thread kernel itself is
 * copyright RT-Thread Development Team and is not modified by this file.
 *
 * RT-Thread is free software; you can redistribute it and/or modify it
 * under the terms of the Apache License 2.0 as published by The Apache
 * Software Foundation.
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

/* The alignment of the data structures */
#define RT_ALIGN_SIZE                  8

// <o>the maximum level of thread priority<1-256>
//  <i>Default: 32
#define RT_THREAD_PRIORITY_MAX         32

// <o>the frequency of kernel tick<1-1000>
//  <i>Default: 1000  (1ms)
#define RT_TICK_PER_SECOND             1000

/* Using the small memory first-fit allocator - the recommended heap
 * implementation for the small RAM systems */
#define RT_USING_SMALL_MEM
#define RT_USING_SMALL_MEM_AS_HEAP
#define RT_USING_HEAP

/* The IPC objects that the applications use */
#define RT_USING_SEMAPHORE
#define RT_USING_MAILBOX
#define RT_USING_MESSAGEQUEUE

/* The heap that the board initialization carves the allocations from */
#define RT_HEAP_SIZE                   8192

/* The console - the board support routes it through the RVM debug channel,
 * and the port's error trap prints through rt_kprintf, which the headers
 * compile into an empty inline function without this switch. */
#define RT_USING_CONSOLE
#define RT_CONSOLEBUF_SIZE             128
#define RT_KPRINTF_USING_LONGLONG

/* The idle thread stack.  NOTE: RT-Thread V4.1.1's idle.c sizes its stack
 * with IDLE_THREAD_STACK_SIZE, NOT with RT_THREAD_IDLE_STACK_SIZE - the
 * usual knob is silently ignored, and the default 256 bytes are far too
 * small for the idle work on this port: the defunct thread reclamation
 * (rt_thread_idle_excute -> rt_thread_detach -> rt_free) overflows it and
 * corrupts whatever sits below.  The RV32 switch frames are also much
 * wider than the ARM ones - 37 integer words plus the 33 word FPU
 * transient of the fast path, 280 bytes in total - so the stacks carry a
 * healthy margin over the ARM ports. */
#define IDLE_THREAD_STACK_SIZE         1024
#define RT_THREAD_IDLE_STACK_SIZE      1024

/* The main thread - the application's main() runs on it */
#define RT_USING_USER_MAIN
#define RT_MAIN_THREAD_STACK_SIZE      1024

#endif
