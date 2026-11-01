/*
 * KLite V5.2.1 RVM port - Copyright (C) 2026 Evo-Devo Instrum.
 * All rights reserved.
 *
 * This file is NOT part of the KLite distribution.  It belongs to the
 * KLite-on-RVM test suite and is released under the Unlicense; refer to
 * the LICENSE file for details.  The KLite kernel itself is copyright
 * jiangxiaogang and is not modified by this file.
 *
 * The plain application of the port - the generator copies this file
 * into the workspace as main.c when the project is generated without
 * the benchmark mode.  It boots the kernel through the official example
 * pattern (an idle thread running kernel_idle, plus one application
 * thread) and parks the idle path in the hypervisor; no benchmark
 * timer, no interrupt source and no test header is involved.
 */

#include "rvm.h"
#include "kernel.h"

extern void klt_set_idle_hook(void (*hook)(void));

/* The kernel heap - carved out of the virtual machine's data section. */
static uint8_t m_heap[16*1024] __attribute__((aligned(8)));

/*
 * IDLE PARK HOOK
 *
 * The RVM user manual requires the guest's idle path to park in the
 * hypervisor while there is nothing to run, so that the core is not spun
 * away.  RVM_Hyp_Vct_Wait() blocks the user thread until the next virtual
 * interrupt is delivered and resumes it right here.  The port calls the
 * hook from cpu_sys_sleep(), which the kernel's idle thread enters with
 * the virtual interrupt mask held - the port opens the mask around the
 * hook, see the note in cpu.c.
 */
static void klt_idle_park(void)
{
	RVM_Hyp_Vct_Wait();
}

/*
 * The idle thread entry - the official pattern: kernel_idle() never
 * returns, it cleans up dead threads and parks through cpu_sys_sleep().
 */
static void klt_idle_entry(void *arg)
{
	(void)arg;
	kernel_idle();
}

/*
 * The application thread - replace the body with your own code.
 */
static void klt_app_entry(void *arg)
{
	(void)arg;
	while(1)
	{
		thread_sleep(1000);
	}
}

int main(void)
{
	/* The idle policy belongs to the application; the port only calls
	 * the hook back from its idle path.  The benchmark does not
	 * register any hook and keeps its idle path busy. */
	klt_set_idle_hook(klt_idle_park);

	kernel_init(m_heap, sizeof(m_heap));
	thread_create(klt_idle_entry, NULL, 0);
	thread_create(klt_app_entry, NULL, 0);
	kernel_start();

	/* kernel_start() releases the boot critical section and switches
	 * into the first thread; the startup context is abandoned inside
	 * that switch.  If control ever falls through to here anyway - a
	 * fault-recovery return resurrecting the startup context - block
	 * in the hypervisor instead of spinning the user thread. */
	while(1)
	{
		RVM_Virt_Yield();
	}
}
