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

 * Change Logs:
 * Date           Author       Notes
 * 2026-10-26     Evo-Devo     first version for the ARMv7-M RVM guest.
 * 2026-10-28     Evo-Devo     restore the dirty frame filter retired by
 *                             the faulty ARMv6-M-copied macros; clear the
 *                             switch request flag at the first startup.
 *
 * This file implements the C part of the RT-Thread porting layer for
 * ARMv7-M systems with the FPv4-SP coprocessor running as an unprivileged
 * guest of the RVM hypervisor.
 */

#include <rtthread.h>
#include "rvm.h"

#if(RVM_COP_NUM!=0U)
/* The coprocessor register set of the hypervisor - the S16-S31 half of
 * the FPv4-SP coprocessor, reachable through the RVM_REG walking pointer.
 * The S0-S15 half and the FPSCR travel in the extended part of the
 * hardware exception frames that the hypervisor and this port exchange. */
#define RTT_CPU_FPU                     ((volatile struct RVM_A7M_Cop_Struct*)(RVM_REG->Cop))
#endif

/* Frame word offsets from the saved stack pointer - shared by the dirty
 * frame filter below, exactly like on the proven FreeRTOS port. */
#define RTT_OFFSET_TO_LR_EXC                        (13u)
#define RTT_OFFSET_TO_XPSR_BASIC                    (21u)
#define RTT_OFFSET_TO_XPSR_EXTENDED                 (37u)
#define RTT_XPSR_ICI_IT_MASK                        (0x0600FC00UL)
#define RTT_EXC_RETURN_EXTENDED_MASK                (0x00000010UL)

/*
 * The stack frame that the port maintains, from the stack pointer stored
 * in the thread control block upwards:
 *
 *   - the five RVM hypercall parameter words (the number and the four
 *     parameters), which must survive across a context switch because a
 *     thread may be preempted in the middle of a hypercall;
 *   - R4-R11 and the interrupted LR (the saved EXC_RETURN);
 *   - the S16-S31 block, only when the frame is extended;
 *   - a hardware exception frame - basic, or extended with S0-S15/FPSCR -
 *     with one padding word when the stack pointer was not 8 byte
 *     aligned at entry, marked by XPSR bit 9 exactly like the hardware.
 *
 * The frame is built in three places that must stay in sync: by this file
 * for the freshly initialized threads, by rt_hw_context_switch_swt() in
 * context_gcc.s for the scheduler level switches, and by PendSV_Handler()
 * below for the interrupt driven switches.
 */
struct exception_stack_frame
{
    rt_uint32_t r0;
    rt_uint32_t r1;
    rt_uint32_t r2;
    rt_uint32_t r3;
    rt_uint32_t r12;
    rt_uint32_t lr;
    rt_uint32_t pc;
    rt_uint32_t psr;
};

struct stack_frame
{
    /* the RVM hypercall parameters */
    rt_uint32_t hypercall_number;
    rt_uint32_t hypercall_param[4];

    /* r4 ~ r7 low register */
    rt_uint32_t r4;
    rt_uint32_t r5;
    rt_uint32_t r6;
    rt_uint32_t r7;

    /* r8 ~ r11 high register */
    rt_uint32_t r8;
    rt_uint32_t r9;
    rt_uint32_t r10;
    rt_uint32_t r11;

    /* the interrupted LR, always the EXC_RETURN */
    rt_uint32_t exception_return;

    struct exception_stack_frame exception_stack_frame;
};

/* flag in interrupt handling */
rt_uint32_t rt_interrupt_from_thread, rt_interrupt_to_thread;
rt_uint32_t rt_thread_switch_interrupt_flag;

/*
 * This function will initialize thread stack
 *
 * @param tentry the entry of thread
 * @param parameter the parameter of entry
 * @param stack_addr the beginning stack address
 * @param texit the function will be called when thread exit
 *
 * @return stack address
 */
rt_uint8_t *rt_hw_stack_init(void       *tentry,
                             void       *parameter,
                             rt_uint8_t *stack_addr,
                             void       *texit)
{
    struct stack_frame *stack_frame;
    rt_uint8_t         *stk;
    rt_uint32_t        *words;
    rt_uint32_t         i;

    stk  = stack_addr + sizeof(rt_uint32_t);
    stk  = (rt_uint8_t *)RT_ALIGN_DOWN((rt_uint32_t)stk, 8);
    stk -= sizeof(struct stack_frame);

    stack_frame = (struct stack_frame *)stk;
    words = (rt_uint32_t *)stack_frame;

    /* build a basic hardware exception frame - the first thread starts
     * without any coprocessor context, and the EXC_RETURN in the frame
     * marks it exactly like the hardware would */
    for (i = 0U; i < sizeof(struct stack_frame) / sizeof(rt_uint32_t); i++)
    {
        words[i] = 0xDEADBEEFU;
    }

    stack_frame->exception_stack_frame.psr = 0x01000000L;
    stack_frame->exception_stack_frame.pc = (rt_uint32_t)tentry;
    stack_frame->exception_stack_frame.lr = (rt_uint32_t)texit;
    stack_frame->exception_stack_frame.r0 = (rt_uint32_t)parameter;

    stack_frame->exception_return = 0xFFFFFFFDL;

    return stk;
}

/*
 * void rt_hw_context_switch_swt(void);  -  the raw fast-path switch body in
 * context_gcc.s, entered with the switch pair latched into the port
 * globals and the virtual interrupts masked.
 */
extern void rt_hw_context_switch_swt(void);

/*
 * The fast-path dirty frame trap.  The manual unstacking in context_gcc.s
 * jumps here when it meets a frame whose EPSR ICI/IT bits are set - the
 * filter in rt_hw_context_switch() below should have routed such frames
 * onto the slow path, so reaching this function means the filter itself
 * is broken.  Trap loudly instead of corrupting the program flow.
 */
void rt_hw_context_switch_err(void)
{
    RVM_Virt_Int_Mask();
    rt_kprintf("rtt: A7M RVM port - dirty XPSR ICI/IT on the fast path!\r\n");
    while (1);
}

/*
 * Check whether the frame of the given thread carries EPSR ICI/IT state
 * that only a real exception return can restore.
 */
static rt_bool_t rt_hw_frame_dirty(rt_uint32_t to)
{
    rt_uint32_t *frame;
    rt_uint32_t xpsr;

    frame = (rt_uint32_t *)(*(rt_uint32_t *)to);

    /* The dirty frame filter.  The temporary all-slow detour that shipped
     * while the fast path's macros were still the faulty ARMv6-M copies
     * (inverted alignment polarity, prologue-depth reload offsets) is gone:
     * those masqueraded frames were born with a garbage XPSR built from the
     * interrupted thread's R0, which tripped the dirty frame trap below -
     * the fast path itself was never the leak. */
    if ((frame[RTT_OFFSET_TO_LR_EXC] & RTT_EXC_RETURN_EXTENDED_MASK) == 0UL)
    {
        /* extended frame - the S16-S31 block sits between the saved
         * EXC_RETURN and the hardware exception frame, whose XPSR is the
         * last word below the S0-S15/FPSCR area */
        xpsr = frame[RTT_OFFSET_TO_XPSR_EXTENDED];
    }
    else
    {
        /* basic frame */
        xpsr = frame[RTT_OFFSET_TO_XPSR_BASIC];
    }

    return ((xpsr & RTT_XPSR_ICI_IT_MASK) != 0UL) ? RT_TRUE : RT_FALSE;
}

/*
 * void rt_hw_context_switch(rt_uint32 from, rt_uint32 to);
 *
 * Called by rt_schedule() from thread context, with the virtual interrupts
 * already masked by the scheduler's critical section.  The switch pair is
 * latched into the port globals unconditionally - unlike the official
 * ports, whose PendSV path can keep an already latched pair, the fast
 * path here consumes the pair on every switch, and rt_hw_context_switch_to()
 * leaves the from-thread global at zero while raising the flag.  The
 * switch itself takes one of two shapes:  frames whose EPSR ICI/IT bits
 * need a real exception return are pended onto the interrupt driven path
 * through the yield hypercall, everything else is switched directly on
 * the thread stacks by rt_hw_context_switch_swt() in context_gcc.s.
 */
void rt_hw_context_switch(rt_ubase_t from, rt_ubase_t to)
{
    rt_interrupt_from_thread = from;
    rt_interrupt_to_thread = to;

    if (rt_hw_frame_dirty(to) != RT_FALSE)
    {
        /* dirty ICI/IT - the slow path, whose real exception return
         * restores them; the pended switch fires at the unmasking
         * hypercall boundary, with no unmasked window in between.  The
         * flag makes PendSV_Handler() execute the latched pair. */
        rt_thread_switch_interrupt_flag = 1U;
        RVM_Virt_Yield();
        RVM_Virt_Int_Unmask();
    }
    else
    {
        /* clean frame - switch on the thread stacks; the switch itself
         * cancels the flag, because it supersedes any request that would
         * have consumed it */
        rt_hw_context_switch_swt();
    }
}

/*
 * rt_base_t rt_hw_interrupt_disable(void);  and the enable counterpart -
 * the RVM virtual interrupt mask with snapshot semantics, exactly like
 * the PRIMASK of the official ports.
 */
rt_base_t rt_hw_interrupt_disable(void)
{
    rt_base_t level;

    level = RVM_Int_Mask;
    if (level == 0U)
    {
        RVM_Virt_Int_Mask();
    }

    return level;
}

void rt_hw_interrupt_enable(rt_base_t level)
{
    if (level == 0U)
    {
        RVM_Virt_Int_Unmask();
    }
    else
    {
        RVM_Virt_Int_Mask();
    }
}

/*
 * The RVM context switch handler, registered through RVM_Virt_Ctx_Reg() by
 * the board initialization.  It plays the role that the PendSV exception
 * plays on the official ports: when a switch has been requested from an
 * interrupt context through rt_hw_context_switch_interrupt(), or pended
 * from the scheduler level by the slow branch of rt_hw_context_switch(),
 * the hypervisor saves the interrupted context and calls this function
 * with the register block made available through RVM_REG.  The saved
 * registers are moved onto the stack of the thread that is being switched
 * out - the thread whose stack pointer location the request latched - and
 * the registers of the thread being switched in are loaded into the
 * register block.  Returning re-enters the new thread exactly where it
 * was interrupted.
 */
void PendSV_Handler(void)
{
    rt_uint32_t *sp;

    /* Skip the switch when no request is pending - a leftover flag from a
     * request that the fast path superseded must never replay a switch. */
    if (rt_thread_switch_interrupt_flag == 0U)
    {
        return;
    }

    /* save the current context onto the from thread's stack - the same
     * sequence the official PendSV handler performs.  A null from thread
     * means no thread has ever been switched out yet, so there is nothing
     * to save. */
    if (rt_interrupt_from_thread != 0U)
    {
        sp = (rt_uint32_t *)RVM_REG->Reg.SP;

#if(RVM_COP_NUM!=0U)
        if ((RVM_REG->Reg.LR & 0x10U) == 0U)
        {
            /* extended frame - stack the S16-S31 half of the coprocessor
             * below the hardware frame, in the same layout the interrupt
             * driven switches of the FreeRTOS and uC/OS ports build */
            *(--sp) = RTT_CPU_FPU->S31;
            *(--sp) = RTT_CPU_FPU->S30;
            *(--sp) = RTT_CPU_FPU->S29;
            *(--sp) = RTT_CPU_FPU->S28;
            *(--sp) = RTT_CPU_FPU->S27;
            *(--sp) = RTT_CPU_FPU->S26;
            *(--sp) = RTT_CPU_FPU->S25;
            *(--sp) = RTT_CPU_FPU->S24;
            *(--sp) = RTT_CPU_FPU->S23;
            *(--sp) = RTT_CPU_FPU->S22;
            *(--sp) = RTT_CPU_FPU->S21;
            *(--sp) = RTT_CPU_FPU->S20;
            *(--sp) = RTT_CPU_FPU->S19;
            *(--sp) = RTT_CPU_FPU->S18;
            *(--sp) = RTT_CPU_FPU->S17;
            *(--sp) = RTT_CPU_FPU->S16;
        }
#endif

        *(--sp) = RVM_REG->Reg.LR;                   /* the saved EXC_RETURN */
        *(--sp) = RVM_REG->Reg.R11;
        *(--sp) = RVM_REG->Reg.R10;
        *(--sp) = RVM_REG->Reg.R9;
        *(--sp) = RVM_REG->Reg.R8;
        *(--sp) = RVM_REG->Reg.R7;
        *(--sp) = RVM_REG->Reg.R6;
        *(--sp) = RVM_REG->Reg.R5;
        *(--sp) = RVM_REG->Reg.R4;

        *(--sp) = RVM_STATE->Usr.Param[3];           /* the hypercall parameters */
        *(--sp) = RVM_STATE->Usr.Param[2];
        *(--sp) = RVM_STATE->Usr.Param[1];
        *(--sp) = RVM_STATE->Usr.Param[0];
        *(--sp) = RVM_STATE->Usr.Number;

        /* write the new sp into the from thread's TCB slot */
        *(rt_uint32_t *)rt_interrupt_from_thread = (rt_uint32_t)sp;
    }

    /* clear rt_thread_switch_interrupt_flag to 0 */
    rt_thread_switch_interrupt_flag = 0;

    /* load the to thread's stack pointer from its TCB slot */
    sp = (rt_uint32_t *)*(rt_uint32_t *)rt_interrupt_to_thread;

    RVM_STATE->Usr.Number = *(sp++);             /* the hypercall parameters */
    RVM_STATE->Usr.Param[0] = *(sp++);
    RVM_STATE->Usr.Param[1] = *(sp++);
    RVM_STATE->Usr.Param[2] = *(sp++);
    RVM_STATE->Usr.Param[3] = *(sp++);

    RVM_REG->Reg.R4  = *(sp++);
    RVM_REG->Reg.R5  = *(sp++);
    RVM_REG->Reg.R6  = *(sp++);
    RVM_REG->Reg.R7  = *(sp++);
    RVM_REG->Reg.R8  = *(sp++);
    RVM_REG->Reg.R9  = *(sp++);
    RVM_REG->Reg.R10 = *(sp++);
    RVM_REG->Reg.R11 = *(sp++);
    RVM_REG->Reg.LR  = *(sp++);

#if(RVM_COP_NUM!=0U)
    if ((RVM_REG->Reg.LR & 0x10U) == 0U)
    {
        /* extended frame - recover the S16-S31 half of the coprocessor */
        RTT_CPU_FPU->S16 = *(sp++);
        RTT_CPU_FPU->S17 = *(sp++);
        RTT_CPU_FPU->S18 = *(sp++);
        RTT_CPU_FPU->S19 = *(sp++);
        RTT_CPU_FPU->S20 = *(sp++);
        RTT_CPU_FPU->S21 = *(sp++);
        RTT_CPU_FPU->S22 = *(sp++);
        RTT_CPU_FPU->S23 = *(sp++);
        RTT_CPU_FPU->S24 = *(sp++);
        RTT_CPU_FPU->S25 = *(sp++);
        RTT_CPU_FPU->S26 = *(sp++);
        RTT_CPU_FPU->S27 = *(sp++);
        RTT_CPU_FPU->S28 = *(sp++);
        RTT_CPU_FPU->S29 = *(sp++);
        RTT_CPU_FPU->S30 = *(sp++);
        RTT_CPU_FPU->S31 = *(sp++);
    }
#endif

    RVM_REG->Reg.SP  = (rvm_ptr_t)sp;            /* return to the new thread */
}

#if defined(RT_USING_FINSH) && defined(MSH_USING_BUILT_IN_COMMANDS)
extern long list_thread(void);
#endif
extern rt_thread_t rt_current_thread;

/**
 * fault exception handling
 */
void rt_hw_hard_fault_exception(struct exception_stack_frame *contex)
{
    rt_kprintf("psr: 0x%08x\n", contex->psr);
    rt_kprintf(" pc: 0x%08x\n", contex->pc);
    rt_kprintf(" lr: 0x%08x\n", contex->lr);
    rt_kprintf("r12: 0x%08x\n", contex->r12);
    rt_kprintf("r03: 0x%08x\n", contex->r3);
    rt_kprintf("r02: 0x%08x\n", contex->r2);
    rt_kprintf("r01: 0x%08x\n", contex->r1);
    rt_kprintf("r00: 0x%08x\n", contex->r0);

    rt_kprintf("hard fault on thread: %s\n", rt_current_thread->name);

#if defined(RT_USING_FINSH) && defined(MSH_USING_BUILT_IN_COMMANDS)
    list_thread();
#endif

    while (1);
}

/**
 * reset CPU - the guest cannot write the AIRCR, so the reset goes
 * through the hypervisor.
 */
RT_WEAK void rt_hw_cpu_reset(void)
{
    RVM_Hyp_Reboot();
}

/*
 * rt_uint32_t rt_hw_interrupt_check(void);
 *
 * Reports whether the caller is running in an interrupt context.  The
 * physical interrupts are taken by the hypervisor and relayed to this
 * virtual machine as plain function calls on the vector thread, so the
 * processor is always in thread mode here - the authoritative indicator
 * is the RVM state block's Vct_Act flag, which the vector thread sets
 * while it is dispatching a virtual interrupt.  The value is a plain
 * boolean, unlike the IPSR exception number of the official ports.
 */
rt_uint32_t rt_hw_interrupt_check(void)
{
    return (RVM_STATE->Vct_Act != 0U) ? 1U : 0U;
}
