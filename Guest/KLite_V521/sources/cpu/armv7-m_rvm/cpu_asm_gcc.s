/*
 * KLite V5.2.1 RVM port - Copyright (C) 2026 Evo-Devo Instrum.
 * All rights reserved.
 *
 * This file is NOT part of the KLite distribution.  It belongs to the
 * KLite-on-RVM port and is released under the Unlicense; refer to the
 * LICENSE file for details.  The KLite kernel itself is copyright
 * jiangxiaogang and is not modified by this file.
 *
 * The assembly part of the porting layer for ARMv7-M systems running as
 * an unprivileged guest of the RVM hypervisor: the fast-path context
 * switch.  It is called by klt_yield() in cpu.c with the RVM virtual
 * interrupts already masked, and it unmasks them itself once the new
 * stack has been loaded - the only coherent boundary for the unmask,
 * because its re-delivery drains the vectors that piled up while both
 * stacks were being rewritten.
 */

/*-----------------------------------------------------------*/

/* Symbols provided by the scheduler, the RVM paravirtualization library
 * and the port itself. */
	.extern		RVM_Usr_Param		/* The user hypercall parameter block. */
	.extern		RVM_Virt_Int_Unmask	/* Unmask the RVM virtual interrupts. */
	.extern		sched_tcb_now		/* The running tcb - 'stack' is its first member. */
	.extern		sched_tcb_next		/* The tcb to switch to, picked by the kernel. */
	.extern		klt_yield_err		/* The dirty ICI/IT trap in cpu.c. */

/* Entry point implemented in this file, called by klt_yield() in cpu.c
 * when the pended switch target holds a clean ICI/IT state.  A NULL
 * sched_tcb_now means the first switch of the kernel: nothing is saved
 * and the startup context is abandoned on its own stack. */
	.global		cpu_contex_swt

/*-----------------------------------------------------------*/

/*
 * cpu_contex_swt switches the CPU from one thread to another without a
 * trap through the hypervisor: the scheduler has already picked
 * sched_tcb_next, so the switch runs directly on the thread stacks.  The
 * routine masquerades the current context as if it were stacked by an
 * exception entry, so the very same frame can also be recovered by the
 * interrupt driven switches through klt_ctx_handler() in cpu.c.
 *
 * The exception frame built here, counted up from the saved SP:
 *
 *   word  0 -  4 : the five hypercall parameter words
 *   word  5 - 12 : R4 - R11
 *   word 13      : the saved EXC_RETURN (0xFFFFFFFD - always basic here)
 *   word 14 - 20 : R0 - R3, R12, LR, PC
 *   word 21      : XPSR
 *   word 22      : [PAD], only when XPSR bit 9 is set
 *
 * The PC pushed in the frame is the exit label below, and the LR is the
 * return address into klt_yield(), so a thread saved here and later
 * resumed through the interrupt path continues exactly where the fast
 * path would have returned to.
 *
 * The guest is compiled soft-float, so all frames are basic frames; an
 * FPU-capable build is stopped at compile time instead of silently
 * corrupting the S registers across a switch.
 *
 * The EPSR ICI/IT bits of the frame being unstacked are checked on the
 * fly: a frame holding interrupted IT block or LDM/STM continuation
 * state can only be restored by a real exception return, which the
 * manual unstacking below cannot perform.  Such frames are filtered out
 * by klt_frame_dirty() beforehand, so hitting klt_yield_err() here
 * indicates an internal port error.
 */

#if defined(__ARM_FP)
#error "The KLite RVM port builds integer-only frames: compile the guest soft-float."
#endif

	.syntax		unified
	.arch		armv7-m
	.thumb

/* See whether the stack pointer is 8 byte aligned.  TST SP is
 * unpredictable on ARMv7-M, so SP has to go through a register first.
 * Two words (the protected R0 and XPSR) have been pushed above the
 * tested value at entry, so the polarity is NE. */
	.macro	KLT_ALIGN_PUSH label
	MOV		R0, SP
	TST		R0, #0x00000007
	BNE		\label
	.endm

/* Stack the remaining part of an exception frame of \sz words, with the
 * given XPSR bits and EXC_RETURN, and the exit label as the PC.  The two
 * words pushed at the entry double as the frame's XPSR/PC slots when the
 * frame is 8 words, and sit just above it otherwise - the alignment
 * padding word, when present, always lands above the XPSR, exactly where
 * a real exception entry would have put it. */
	.macro	KLT_EXC_PUSH sz, xpsr, exc_return
	LDR		R0, [SP, #4]		/* Load the real R0 value pushed at entry. */
	.if		((\sz-8)!=0)		/* Reserve the frame's upper words. */
	SUB		SP, #4*(\sz-8)
	.endif
	PUSH	{R0-R3, R12, LR}	/* Push the frame's GP registers. */
	LDR		R0, [SP, #4*(\sz-2)]/* Load the real XPSR value pushed at entry. */
	LDR		R1, =\xpsr
	ORR		R0, R1
	STR		R0, [SP, #4*7]		/* Merge in the flags and the padding mark. */
	LDR		R0, =klt_swt_exit	/* Push the resume PC with bit[0] cleared. */
	AND		R0, #0xFFFFFFFE
	STR		R0, [SP, #4*6]
	MOV		LR, #\exc_return	/* Make up the EXC_RETURN. */
	.endm

/* See whether the frame holds an alignment padding word: bit 9 of the
 * stacked XPSR is used to mark it. */
	.macro	KLT_ALIGN_POP label
	LDR		R0, [SP, #4*7]		/* Load up the XPSR. */
	TST		R0, #0x00000200
	BNE		\label
	.endm

/* Unstack an exception frame of \sz words.  The original sequence is
 * [PAD] XPSR PC LR R12 R3-R0, which is not ideal for a manual restoring,
 * because restoring PC and SP simultaneously must be the very last step.
 * Thus the frame is transformed in place first:
 * 1. With [PAD]:    [PAD] XPSR PC LR R12 R3-R0 -> PC R0 XPSR [EMPTY] R12 R3-R1 LR
 * 2. Without [PAD]: XPSR PC LR R12 R3-R0        -> PC R0 XPSR R12 R3-R1 LR
 * The transformation restores LR first so it cannot be overwritten, pops
 * R0, loads XPSR and PC and rearranges them at the end, pops R1-R3 and LR
 * while skipping the middle, and finally restores XPSR through R0, then
 * R0 and PC.  Nothing is ever placed below the current SP, because a
 * racing interrupt would clobber it. */
	.macro	KLT_EXC_POP sz
	LDR		LR, [SP, #4*5]		/* Restore the frame's LR. */
	POP		{R0}			/* Load R0/XPSR/PC into R0/R1/R2. */
	LDR		R1, [SP, #4*6]
	LDR		R12, =0x0600FC00	/* The EPSR ICI/IT field. */
	TST		R1, R12			/* Dirty ICI/IT cannot be restored here. */
	BNE		klt_yield_err		/* Defense in depth - see the header. */
	AND		R1, #0xFFFFFDFF		/* Clear XPSR[9]. */
	LDR		R2, [SP, #4*5]
	ORR		R2, #0x00000001		/* Set PC[0]. */
	STR		R1, [SP, #4*(\sz-4)]	/* Rearrange to H-PC-R0-XPSR-L. */
	STR		R0, [SP, #4*(\sz-3)]
	STR		R2, [SP, #4*(\sz-2)]
	POP		{R1-R3, R12}		/* Pop the GP registers. */
	.if		((\sz-8)!=0)		/* Skip the padding. */
	ADD		SP, #4*(\sz-8)
	.endif
	POP		{R0}			/* Pop the XPSR through R0. */
	MSR		XPSR_NZCVQ, R0
	POP		{R0, PC}		/* Pop R0 and PC. */
	.endm

/*
 * The fast-path switch entry point - see the header comment above.
 */
	.section	.text.cpu_contex_swt
	.align		3
	.thumb_func
cpu_contex_swt:				/* Entered with the virtual interrupts
					 * already masked by klt_yield(). */
	PUSH	{R0}			/* Protect R0 and XPSR. */
	MRS		R0, XPSR
	PUSH	{R0}

	KLT_ALIGN_PUSH klt_swt_basic_unaligned
klt_swt_basic_aligned:			/* Originally 8 byte aligned stack. */
	KLT_EXC_PUSH 8, 0x01000000, 0xFFFFFFFD
	B		klt_swt_stacked
klt_swt_basic_unaligned:		/* Originally unaligned stack. */
	KLT_EXC_PUSH 8+1, 0x01000200, 0xFFFFFFFD

klt_swt_stacked:
	PUSH	{R4-R11, LR}		/* Push GP registers. */

	LDR		R0, =RVM_Usr_Param	/* Push the hypercall parameters. */
	LDR		R0, [R0]
	LDMIA	R0, {R1-R5}
	PUSH	{R1-R5}

	LDR		R1, =sched_tcb_now	/* Save SP to the first tcb member. */
	LDR		R2, [R1]
	CBZ		R2, klt_swt_first
	STR		SP, [R2]
	LDR		R2, =sched_tcb_next	/* The kernel has already picked it. */
	LDR		R2, [R2]
	STR		R2, [R1]		/* sched_tcb_now = sched_tcb_next */
	LDR		SP, [R2]
	B		klt_swt_mask

klt_swt_first:				/* First switch: no current thread to
					 * save, the startup stack is abandoned. */
	LDR		R2, =sched_tcb_next
	LDR		R2, [R2]
	STR		R2, [R1]
	LDR		SP, [R2]

klt_swt_mask:
	BL		RVM_Virt_Int_Unmask	/* The coherent boundary - drains what
					 * piled up while both stacks moved. */

	LDR		R0, =RVM_Usr_Param	/* Pop the hypercall parameters. */
	LDR		R0, [R0]
	POP		{R1-R5}
	STMIA	R0, {R1-R5}
	POP		{R4-R11, LR}		/* Pop GP registers. */

	TST		LR, #0x00000010		/* LR.EXTENDED - impossible here, the
					 * port builds basic frames only. */
	BEQ		klt_yield_err

	KLT_ALIGN_POP klt_swt_pop_pad
klt_swt_pop_nopad:			/* Frame without alignment padding. */
	KLT_EXC_POP 8
	B		klt_swt_exit
klt_swt_pop_pad:			/* Frame with alignment padding. */
	KLT_EXC_POP 8+1

klt_swt_exit:				/* Exit location - also the resume PC. */
	BX		LR

/*-----------------------------------------------------------*/
