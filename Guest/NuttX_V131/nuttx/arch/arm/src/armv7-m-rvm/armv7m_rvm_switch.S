/****************************************************************************
 * arch/arm/src/armv7-m-rvm/armv7m_rvm_switch.S
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.  The
 * ASF licenses this file to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance with the
 * License.  You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  See the
 * License for the specific language governing permissions and limitations
 * under the License.
 *
 ****************************************************************************/

/****************************************************************************
 * Task level context switch for the RVM vector-thread monitor.
 *
 * RVM_Virt_Yield() only posts the context-switch flag to the vector thread;
 * it does not capture the caller's registers.  The guest must therefore
 * masquerade its own context as an exception frame, exactly as the Zephyr
 * and RTEMS guests do, and let the monitor's real exception return resume
 * it from that frame.
 *
 * Frame layout, from the saved stack pointer upwards:
 *
 *   Number, Param[0..3]                  5 words
 *   R4..R11, EXC_RETURN                  9 words
 *   S16..S31                            16 words, extended only
 *   R0..R3, R12, LR, PC, xPSR            8 words
 *   S0..S15, FPSCR, [pad]               18 words, extended only
 *
 * The resume PC is ntx_ctx_exit; the hardware frame's LR is the return
 * address of the C caller, so the task continues after ntx_context_switch()
 * when the monitor resumes it.  CONTROL.FPCA selects the frame type at the
 * moment of the switch, so a thread that never touches the FPU never pays
 * for the extended frame.
 ****************************************************************************/

#include <nuttx/config.h>

	.syntax	unified
	.thumb
	.arch	armv7-m

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

/* Build the hardware part of the masqueraded frame.
 *
 * On entry SP points at two protected words: the caller's xcptcontext pointer
 * at SP+4 and the real XPSR at SP.  sz is the hardware frame size in words
 * (8 basic, 9 basic with an alignment pad, 26 or 27 extended).  The two
 * protected words become the frame's PC and xPSR slots; R0..R2 are left in
 * the hardware frame so that the routine arguments can be recovered after
 * the rest of the frame has been pushed.
 */

	.macro	NTX_EXC_PUSH sz, xpsr, exc
	ldr		r0, [sp, #4]
	.if	(((\sz) - 8) != 0)
	sub		sp, #4 * ((\sz) - 8)
	.endif
	push	{r0-r3, r12, lr}
	ldr		r0, [sp, #4 * ((\sz) - 2)]
	ldr		r1, =\xpsr
	orrs	r0, r1
	str		r0, [sp, #4 * 7]
	ldr		r0, =ntx_ctx_exit
	ldr		r1, =0xfffffffe
	ands	r0, r1
	str		r0, [sp, #4 * 6]
	ldr		r1, =\exc
	mov		lr, r1
	.endm

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/****************************************************************************
 * Name: ntx_context_switch
 *
 * Description:
 *   Save the current register context on the current stack in RVM frame
 *   format and request a context switch.
 *
 * Input Parameters:
 *   r0 - Pointer to the current thread's struct xcptcontext.  rvm_sp is
 *        the first word of that structure and receives the saved frame.
 *   r1 - Pointer to the five hypercall parameters to store in the frame.
 *
 * Returned Value:
 *   None.  The function appears to return when the monitor resumes this
 *   thread; the actual return is through ntx_ctx_exit.
 *
 ****************************************************************************/

	.text
	.align	2
	.global	ntx_context_switch
	.type	ntx_context_switch, %function
	.thumb_func

ntx_context_switch:
	push	{r0}			/* Protect the xcptcontext pointer */
	mrs		r0, xpsr
	push	{r0}			/* Protect the real XPSR */

#ifdef CONFIG_ARCH_FPU
	mrs		r0, control
	tst		r0, #4			/* CONTROL.FPCA selects the frame type */
	bne		1f
#endif

	/* Basic frame */

	mov		r0, sp
	tst		r0, #7
	bne		2f
	NTX_EXC_PUSH 8, 0x01000000, 0xfffffffd
	movs	r12, #36
	b		3f

2:
	NTX_EXC_PUSH 9, 0x01000200, 0xfffffffd
	movs	r12, #36
	b		3f

1:
#ifdef CONFIG_ARCH_FPU
	/* Extended frame */

	mov		r0, sp
	tst		r0, #7
	bne		4f
	NTX_EXC_PUSH 26, 0x01000000, 0xffffffed
	b		5f

4:
	NTX_EXC_PUSH 27, 0x01000200, 0xffffffed

5:
	add		r0, sp, #4 * 8
	vstmia	r0!, {s0-s15}
	vmrs	r1, fpscr
	str		r1, [r0]
	vpush	{s16-s31}
	movs	r12, #100
	b		3f
#endif

3:
	push	{r4-r11, lr}
	add		r3, sp, r12
	ldr		r4, [r3]		/* xcptcontext pointer */
	ldr		r6, [r3, #4]	/* Hypercall parameters */
	ldmia	r6, {r7-r11}
	push	{r7-r11}		/* Number, Param[0..3] */
	str		sp, [r4]		/* xcp->rvm_sp = frame base */

	bl		RVM_Virt_Yield
	bl		RVM_Virt_Int_Unmask

	/* The monitor resumes at ntx_ctx_exit, not here. */

	b		.

/****************************************************************************
 * Name: ntx_ctx_exit
 *
 * Description:
 *   Resume point of a thread that was switched out by ntx_context_switch.
 *   The monitor has restored LR from the frame, so this returns to the C
 *   caller of ntx_context_switch with the stack pointer restored.
 *
 ****************************************************************************/

	.thumb_func
ntx_ctx_exit:
	bx		lr

	.size	ntx_context_switch, . - ntx_context_switch

#ifdef CONFIG_ARMV7M_RVM_FAST_PATH

/****************************************************************************
 * Direct task-to-task switch.
 *
 * Entered only when the current XPSR has no ICI/IT continuation and the
 * heir's frame is a clean task level frame.  The current frame is built in
 * the same masqueraded layout, but the heir is resumed in place instead of
 * going through the vector thread and the monitor.
 ****************************************************************************/

/* See whether the frame holds an alignment padding word: bit 9 of the
 * stacked XPSR is used to mark it.
 */

	.macro	NTX_ALIGN_POP label
	ldr		r0, [sp, #4 * 7]
	tst		r0, #0x00000200
	bne		\label
	.endm

/* Manually unstack an exception frame of sz words.  The frame is transformed
 * in place so that PC and SP are restored as the last step; dirty ICI/IT
 * bits are rejected here as a defence-in-depth check.
 */

	.macro	NTX_EXC_POP sz
	ldr		lr, [sp, #4 * 5]
	pop		{r0}
	ldr		r1, [sp, #4 * 6]
	ldr		r12, =0x0600fc00
	tst		r1, r12
	bne		ntx_fast_err
	and		r1, #0xfffffdff
	ldr		r2, [sp, #4 * 5]
	orr		r2, #0x00000001
	str		r1, [sp, #4 * ((\sz) - 4)]
	str		r0, [sp, #4 * ((\sz) - 3)]
	str		r2, [sp, #4 * ((\sz) - 2)]
	pop		{r1-r3, r12}
	.if	(((\sz) - 8) != 0)
	add		sp, #4 * ((\sz) - 8)
	.endif
	pop		{r0}
	msr		xpsr_nzcvq, r0
	pop		{r0, pc}
	.endm

	.text
	.align	2
	.global	ntx_fast_switch
	.type	ntx_fast_switch, %function
	.thumb_func

/* void ntx_fast_switch(struct xcptcontext *from, struct xcptcontext *to,
 *                      const void *params);
 */

ntx_fast_switch:
	push	{r0}			/* Protect the from pointer */
	mrs		r0, xpsr
	push	{r0}			/* Protect the real XPSR */

	mrs		r0, control
	tst		r0, #4
	bne		1f

	/* Basic frame */

	mov		r0, sp
	tst		r0, #7
	bne		2f
	NTX_EXC_PUSH 8, 0x01000000, 0xfffffffd
	movs	r12, #36
	b		3f

2:
	NTX_EXC_PUSH 9, 0x01000200, 0xfffffffd
	movs	r12, #36
	b		3f

1:
	/* Extended frame */

	mov		r0, sp
	tst		r0, #7
	bne		4f
	NTX_EXC_PUSH 26, 0x01000000, 0xffffffed
	b		5f

4:
	NTX_EXC_PUSH 27, 0x01000200, 0xffffffed

5:
	add		r0, sp, #4 * 8
	vstmia	r0!, {s0-s15}
	vmrs	r1, fpscr
	str		r1, [r0]
	vpush	{s16-s31}
	movs	r12, #100
	b		3f

3:
	push	{r4-r11, lr}
	add		r3, sp, r12
	ldr		r4, [r3]		/* from xcptcontext */
	ldr		r5, [r3, #4]	/* to xcptcontext */
	ldr		r6, [r3, #8]	/* hypercall parameters */
	ldmia	r6, {r7-r11}
	push	{r7-r11}
	str		sp, [r4]		/* from->rvm_sp = frame base */

	/* Switch to the heir's frame and select its virtual mask level. */

	ldr		sp, [r5]
	ldr		r0, [r5, #4]	/* heir nest level */
	cbz		r0, 6f
	bl		RVM_Virt_Int_Mask
	b		7f

6:
	bl		RVM_Virt_Int_Unmask

7:
	ldmia	sp, {r7-r11}	/* heir hypercall parameters */
	stmia	r6, {r7-r11}	/* RVM_STATE->Usr */
	add		sp, #4 * 5
	pop		{r4-r11, lr}
	tst		lr, #0x00000010
	bne		8f

	/* Extended frame: pop S16-S31 first, then reload S0-S15/FPSCR. */

	vpop	{s16-s31}
	add		r0, sp, #4 * (8 + 16)
	ldr		r1, [r0]
	vmsr	fpscr, r1
	vldmdb	r0!, {s0-s15}
	NTX_ALIGN_POP 10f
	NTX_EXC_POP 26

10:
	NTX_EXC_POP 27

8:
	NTX_ALIGN_POP 11f
	NTX_EXC_POP 8

11:
	NTX_EXC_POP 9

ntx_fast_err:
	b		ntx_fast_err

	.size	ntx_fast_switch, . - ntx_fast_switch

#endif /* CONFIG_ARMV7M_RVM_FAST_PATH */

	.ltorg
	.end
