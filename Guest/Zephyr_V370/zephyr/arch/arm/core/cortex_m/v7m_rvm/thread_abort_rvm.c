/*
 * Copyright (c) 2016 Wind River Systems, Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @file
 * @brief RVM guest k_thread_abort() routine
 *
 * The native Cortex-M implementation pends PendSV and clears a pending
 * SVCall when a thread aborts itself from handler mode.  Neither exception
 * belongs to a guest: RVM owns the exception model and performs the actual
 * switch when z_swap() yields.  Keep the common scheduler teardown and let
 * the RVM context switch decide the next thread.
 */

#include <zephyr/kernel.h>
#include <ksched.h>
#include <zephyr/sys/__assert.h>

void z_impl_k_thread_abort(k_tid_t thread)
{
	SYS_PORT_TRACING_OBJ_FUNC_ENTER(k_thread, abort, thread);

	z_thread_abort(thread);

	__ASSERT_NO_MSG((thread->base.thread_state & _THREAD_DEAD) != 0);

	SYS_PORT_TRACING_OBJ_FUNC_EXIT(k_thread, abort, thread);
}
