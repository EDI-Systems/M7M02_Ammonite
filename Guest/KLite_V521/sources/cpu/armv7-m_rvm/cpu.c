/*
 * KLite V5.2.1 RVM port - Copyright (C) 2026 Evo-Devo Instrum.
 * All rights reserved.
 *
 * This file is NOT part of the KLite distribution.  It belongs to the
 * KLite-on-RVM port and is released under the Unlicense; refer to the
 * LICENSE file for details.  The KLite kernel itself is copyright
 * jiangxiaogang and is not modified by this file.
 *
 * The porting layer of KLite for ARMv7-M systems running as an
 * unprivileged guest of the RVM hypervisor.  The kernel is used exactly
 * as the official Cortex-M ports use it: cpu_contex_switch() only marks
 * a pended switch, and the real switch happens when the outermost
 * cpu_leave_critical() consumes the pend - on this hypervisor through
 * the fast masquerade path in cpu_asm_gcc.s, or through the deferred
 * context handler when the target frame carries EPSR ICI/IT state that
 * only a real exception return can restore.  The official ARM9 port
 * plays the same game with a software interrupt at leave_critical, and
 * so does the RMP kernel at its scheduler unlock.
 *
 * Only the official port contract is used: the kernel.h API, the two
 * scheduler globals below, and the fact that 'stack' is the first member
 * of struct tcb - the same contract the official PendSV handler relies
 * on.  Nothing in the KLite sources is modified or included.
 */

#include <string.h>
#include "rvm.h"
#include "kernel.h"

/* The scheduler globals - declared in the kernel's internal.h, referenced
 * by name exactly like the official PendSV handler does.  The tcb type
 * stays incomplete: the port only ever holds pointers to it. */
extern struct tcb *sched_tcb_now;
extern struct tcb *sched_tcb_next;

/* The fast-path switch and its dirty frame trap, in cpu_asm_gcc.s. */
extern void cpu_contex_swt(void);
extern void klt_yield_err(void);

/* The deferred context switch handler, defined at the bottom of this
 * file and registered with the hypervisor in cpu_sys_start(). */
void klt_ctx_handler(void);

/* The pended switch flag.  Set by cpu_contex_switch() in any context,
 * consumed by klt_yield() at the outermost cpu_leave_critical() on the
 * thread context, or by klt_ctx_handler() when the deferred path runs. */
static volatile uint32_t m_switch_pend;

/* The kernel's critical section nesting.  Every context freeze of this
 * port happens at a nesting of zero: the fast path masquerades inside
 * klt_yield() which the outermost leave_critical() calls after clearing
 * the count, and the deferred path freezes at the unmask hypercall
 * boundary or at an arbitrary unmasked instruction boundary.  The
 * official void/void recursive critical section API therefore stays
 * consistent without per-thread counters. */
static uint32_t m_lock_nesting;

/* The idle action of the application.  cpu_sys_sleep runs inside the
 * kernel's critical section with the virtual interrupt mask held, so the
 * park hook must be called between an unmask/mask bracket - parking with
 * the mask held would never be woken, because the vector thread's timer
 * and context processing is gated on the mask.  NULL keeps the busy idle
 * of the benchmark. */
static void (*m_sleep_hook)(void);

void klt_set_idle_hook(void (*hook)(void))
{
	m_sleep_hook = hook;
}

/* Word offsets into a saved thread frame, counted up from the saved SP
 * that the first member of the tcb holds.  The layout is created by
 * cpu_contex_init() below, by klt_ctx_handler() and by the fast path in
 * cpu_asm_gcc.s, and is identical for all three:
 *   word  0 -  4 : the five hypercall parameter words
 *   word  5 - 12 : R4 - R11
 *   word 13      : the saved EXC_RETURN (bit 4 set means basic frame)
 *   word 14 - 20 : R0 - R3, R12, LR, PC            } basic frame
 *   word 21      : XPSR                            }
 *   word 22      : [PAD], only when XPSR bit 9 is set - the same place a
 *                  real exception entry puts it, so XPSR never moves. */
#define KLT_OFFSET_TO_XPSR					(13U+8U)
#define KLT_XPSR_ICI_IT_MASK					(0x0600FC00UL)

/* Check whether the frame of the pended target carries EPSR ICI/IT state
 * that only a real exception return can restore.  The target is the tcb
 * the kernel has already picked into sched_tcb_next - unlike FreeRTOS,
 * KLite's scheduler selects before calling the port, so no peek of the
 * ready lists is needed. */
static uint32_t klt_frame_dirty(void)
{
	uint32_t *sp;
	sp = *(uint32_t **)sched_tcb_next;	/* 'stack' - the first tcb member. */
	return sp[KLT_OFFSET_TO_XPSR] & KLT_XPSR_ICI_IT_MASK;
}

/* The fast-path switch dispatcher - the local equivalent of the RMP
 * thread-level yield.  Entered with the mask held (the outermost
 * cpu_leave_critical() calls it right after clearing the nesting), and
 * every branch ends with the mask open, so the caller's trailing unmask
 * is a harmless repeat.  Only the self-switch and the fast path consume
 * the pend here; the vector-thread and the dirty-frame branch leave it
 * armed, because their switch is executed by klt_ctx_handler() at the
 * vector loop's tail - consuming the pend here would make that handler
 * find nothing pended and skip the switch altogether. */
static void klt_yield(void)
{
	if(RVM_STATE->Vct_Act != 0)
	{
		/* Never fast-switch from the vector thread: the request is
		 * already armed there by cpu_contex_switch() as a pended
		 * context flag, and klt_ctx_handler() runs it at the tail of
		 * this very vector loop iteration. */
		RVM_Virt_Int_Unmask();
		return;
	}
	RVM_Virt_Int_Mask();			/* Idempotent - we are already masked. */
	if(sched_tcb_next == sched_tcb_now)
	{
		m_switch_pend = 0;			/* Switching to self - consume, run on. */
		RVM_Virt_Int_Unmask();
		return;
	}
	if(klt_frame_dirty() != 0)
	{
		/* Dirty ICI/IT: pend the switch and arm it at the unmask
		 * hypercall boundary - no unmasked window in between, and
		 * the real exception return of the slow path restores what
		 * manual unstacking cannot. */
		RVM_Virt_Yield();
		RVM_Virt_Int_Unmask();
		return;
	}
	m_switch_pend = 0;				/* The fast switch consumes the request. */
	cpu_contex_swt();			/* Unmasks at its coherent boundary. */
}

void cpu_enter_critical(void)
{
	RVM_Virt_Int_Mask();
	m_lock_nesting++;
}

void cpu_leave_critical(void)
{
	m_lock_nesting--;
	if(m_lock_nesting == 0)
	{
		if(m_switch_pend != 0)
		{
			klt_yield();
		}
		RVM_Virt_Int_Unmask();
	}
}

void cpu_contex_switch(void)
{
	m_switch_pend = 1;
	if(RVM_STATE->Vct_Act != 0)
	{
		/* Called by the kernel from a vector handler: pend the
		 * context flag so the switch runs at this vector loop
		 * iteration's tail, after all vectors are drained. */
		RVM_Virt_Yield();
	}
}

void cpu_sys_init(void)
{
	/* A guest owns no NVIC - the hypervisor owns the physical delivery,
	 * and the port registers its handlers in cpu_sys_start(). */
}

/* The kernel tick - one delivery of the RVM virtual timer is one tick,
 * exactly the SysTick handler shape of the official ports. */
static void klt_tick(void)
{
	kernel_tick(1);
}

void cpu_sys_start(void)
{
	/* The deferred context switch handler and the kernel tick - the RVM
	 * virtual timer channel, the same source the other RVM guests use.
	 * The interrupts of the virtual machine are enabled here, with the
	 * kernel's boot critical section still held: nothing can be
	 * delivered until the outermost leave_critical() in kernel_start()
	 * opens the mask and switches into the first thread. */
	RVM_Virt_Tim_Reg(klt_tick);
	RVM_Virt_Ctx_Reg(klt_ctx_handler);
	RVM_Hyp_Int_Ena();
}

void cpu_sys_sleep(uint32_t time)
{
	(void)time;
	if(m_sleep_hook != NULL)
	{
		/* Inside the kernel's critical section: open the mask for
		 * the park and take it back afterwards, so the kernel's
		 * nesting still balances around this call. */
		RVM_Virt_Int_Unmask();
		m_sleep_hook();
		RVM_Virt_Int_Mask();
	}
}

/* The deferred context switch handler, running on the hypervisor's vector
 * thread at the tail of its loop: it transfers the interrupted user
 * context between RVM_REG/RVM_STATE and the thread frames, and the
 * hypervisor's real exception return restores the stacked PC/XPSR - the
 * only path that can restore EPSR ICI/IT state. */
void klt_ctx_handler(void)
{
	uint32_t *sp;

	if(m_switch_pend == 0)
	{
		return;				/* Not ours - nothing pended. */
	}
	m_switch_pend = 0;
	if(sched_tcb_next == sched_tcb_now && sched_tcb_now != NULL)
	{
		return;				/* Switching to self - the
						 * interrupted context runs on. */
	}
	sp = (uint32_t *)(RVM_REG->Reg.SP);
	if(sched_tcb_now != NULL)
	{
		/* Save down from the interrupted stack pointer: the params,
		 * the GP registers and the saved EXC_RETURN land below the
		 * hardware frame the hypervisor already stacked. */
		*(--sp) = RVM_REG->Reg.LR;
		*(--sp) = RVM_REG->Reg.R11;
		*(--sp) = RVM_REG->Reg.R10;
		*(--sp) = RVM_REG->Reg.R9;
		*(--sp) = RVM_REG->Reg.R8;
		*(--sp) = RVM_REG->Reg.R7;
		*(--sp) = RVM_REG->Reg.R6;
		*(--sp) = RVM_REG->Reg.R5;
		*(--sp) = RVM_REG->Reg.R4;
		*(--sp) = RVM_STATE->Usr.Param[3];
		*(--sp) = RVM_STATE->Usr.Param[2];
		*(--sp) = RVM_STATE->Usr.Param[1];
		*(--sp) = RVM_STATE->Usr.Param[0];
		*(--sp) = RVM_STATE->Usr.Number;
		*(uint32_t *)sched_tcb_now = (uint32_t)sp;
	}
	sched_tcb_now = sched_tcb_next;
	sp = *(uint32_t **)sched_tcb_now;
	RVM_STATE->Usr.Number = *(sp++);
	RVM_STATE->Usr.Param[0] = *(sp++);
	RVM_STATE->Usr.Param[1] = *(sp++);
	RVM_STATE->Usr.Param[2] = *(sp++);
	RVM_STATE->Usr.Param[3] = *(sp++);
	RVM_REG->Reg.R4 = *(sp++);
	RVM_REG->Reg.R5 = *(sp++);
	RVM_REG->Reg.R6 = *(sp++);
	RVM_REG->Reg.R7 = *(sp++);
	RVM_REG->Reg.R8 = *(sp++);
	RVM_REG->Reg.R9 = *(sp++);
	RVM_REG->Reg.R10 = *(sp++);
	RVM_REG->Reg.R11 = *(sp++);
	RVM_REG->Reg.LR = *(sp++);
	RVM_REG->Reg.SP = (rvm_ptr_t)sp;
}

void *cpu_contex_init(uint8_t *stack_base, uint32_t stack_size, void *entry, void *arg)
{
	uint32_t *sp = (uint32_t *)((uint32_t)(stack_base + stack_size) & 0xFFFFFFF8);
	memset(stack_base, 0xCC, stack_size);
	*(--sp) = 0x01000000;			/* XPSR - clean, no padding word. */
	*(--sp) = ((uint32_t)entry) | 1U;	/* PC - the Thumb bit set, the
						 * frame feeds a real exception
						 * return on the first entry. */
	*(--sp) = (uint32_t)thread_exit;	/* R14(LR) */
	*(--sp) = 0;				/* R12 */
	*(--sp) = 0;				/* R3 */
	*(--sp) = 0;				/* R2 */
	*(--sp) = 0;				/* R1 */
	*(--sp) = (uint32_t)arg;		/* R0 */
	*(--sp) = 0xFFFFFFFD;			/* LR_EXC - basic frame, the
						 * thread stack, PSP. */
	*(--sp) = 0;				/* R11 */
	*(--sp) = 0;				/* R10 */
	*(--sp) = 0;				/* R9 */
	*(--sp) = 0;				/* R8 */
	*(--sp) = 0;				/* R7 */
	*(--sp) = 0;				/* R6 */
	*(--sp) = 0;				/* R5 */
	*(--sp) = 0;				/* R4 */
	*(--sp) = 0;				/* Param[3] */
	*(--sp) = 0;				/* Param[2] */
	*(--sp) = 0;				/* Param[1] */
	*(--sp) = 0;				/* Param[0] */
	*(--sp) = 0;				/* Number */
	return sp;
}

/* The fast-path dirty frame trap.  The manual unstacking in cpu_asm_gcc.s
 * jumps here when it meets a frame whose EPSR ICI/IT bits are set - the
 * filter in klt_yield() above should have routed such frames onto the
 * slow path, so reaching this function means the filter itself is
 * broken.  Trap loudly instead of corrupting the program flow. */
void klt_yield_err(void)
{
	RVM_Virt_Int_Mask();
	RVM_DBG_S("klt: dirty XPSR ICI/IT on the fast path!\r\n");
	while(1);
}
