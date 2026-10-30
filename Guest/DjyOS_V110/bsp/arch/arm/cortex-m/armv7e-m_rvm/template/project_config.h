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
 * The project level configuration for the RVM guest: the minimal set
 * of kernel components (no shell, no stdio device file, no file
 * systems - the benchmark prints through the debug console hypercall
 * directly), with the limits sized for the benchmark workload.
 */
#ifndef __PROJECT_CONFIG_H__
#define __PROJECT_CONFIG_H__

/* ---- Kernel core (mirrors component_config_core.h defaults, kept here
 *      so that a single file describes the whole guest) ---- */
#define CFG_MODULE_ENABLE_KERNEL        true
#define CFG_INIT_STACK_SIZE             4096
#define CFG_EVENT_LIMIT                 32
#define CFG_EVENT_TYPE_LIMIT            24
#define CFG_MAINSTACK_LIMIT             2048
#define CFG_IDLESTACK_LIMIT             2048
#define CFG_IDLE_MONITOR_CYCLE          0
#define CFG_IDLE_WDT_RESET              false
#define CFG_OS_TINY                     false
#define CFG_IDLE_CHECK_STACK            false

/* ---- Stack paint: the kernel fills every thread stack with 'd' on
 *      creation and on pool recycle, so that __DJY_CheckStack can spot
 *      an overflow by scanning for the pattern.  Defining the switch
 *      below skips the 2KB repaint; the overflow check has nothing to
 *      scan then, so CFG_IDLE_CHECK_STACK must stay false. ---- */
#define CFG_STACK_PAINT_DISABLE

/* ---- Locks ---- */
#define CFG_LOCK_LIMIT                  20

/* ---- Heap: the static first-fit allocator of the kernel ---- */
#define CFG_DYNAMIC_MEM                 false
#define CFG_MEMPOOL_LIMIT               8

/* ---- Object and handle tables ---- */
#define CFG_OBJECT_LIMIT                16
#define CFG_HANDLE_LIMIT                8

/* ---- Black box: not ported (the Sftd daemon of the hypervisor
 *      already captures and recovers every guest fault) ---- */
#define CFG_MODULE_ENABLE_BLACK_BOX     false

#endif /* __PROJECT_CONFIG_H__ */
