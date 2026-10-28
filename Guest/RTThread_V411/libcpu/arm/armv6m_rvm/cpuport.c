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
 * 2026-10-26     Evo-Devo     first version for the ARMv6-M RVM guest.
 * 2026-10-28     Evo-Devo     clear the switch request flag at the first
 *                             startup, like the ARMv7-M port.
 *
 * This file implements the C part of the RT-Thread porting layer for
 * ARMv6-M systems running as an unprivileged guest of the RVM hypervisor:
 * the thread stack initialization, the virtual interrupt masking, and the
 * context switch handler that the hypervisor calls for the interrupt
 * driven switches.
 */

#include <rtthread.h>
#include "rvm.h"

/*
 * The stack frame that the port maintains, from the stack pointer stored
 * in the thread control block upwards:
 *
 *   - the five RVM hypercall parameter words (the number and the four
 *     parameters), which must survive across a context switch because a
 *     thread may be preempted in the middle of a hypercall;
 *   - R4 - R11 and the interrupted LR;
 *   - a basic exception frame, with one padding word below XPSR when the
 *     stack pointer was not 8 byte aligned, marked by XPSR bit 9 exactly
 *     like the hardware does.
 *
 * The frame is built in three places that must stay in sync: by this file
 * for the freshly initialized threads, by rt_hw_context_switch() in
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



/**
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
    unsigned long       i;

    stk  = stack_addr + sizeof(rt_uint32_t);
    stk  = (rt_uint8_t *)RT_ALIGN_DOWN((rt_uint32_t)stk, 8);
    stk -= sizeof(struct stack_frame);

    stack_frame = (struct stack_frame *)stk;

    /* init all register */
    for (i = 0; i < sizeof(struct stack_frame) / sizeof(rt_uint32_t); i ++)
    {
        ((rt_uint32_t *)stack_frame)[i] = 0xdeadbeef;
    }

    stack_frame->exception_stack_frame.r0  = (unsigned long)parameter; /* r0 : argument */
    stack_frame->exception_stack_frame.r1  = 0;                        /* r1 */
    stack_frame->exception_stack_frame.r2  = 0;                        /* r2 */
    stack_frame->exception_stack_frame.r3  = 0;                        /* r3 */
    stack_frame->exception_stack_frame.r12 = 0;                        /* r12 */
    stack_frame->exception_stack_frame.lr  = (unsigned long)texit;     /* lr */
    stack_frame->exception_stack_frame.pc  = (unsigned long)tentry;    /* entry point, pc */
    stack_frame->exception_stack_frame.psr = 0x01000000L;              /* PSR */

    stack_frame->exception_return = 0xFFFFFFFDL;                       /* EXC_RETURN */

    stack_frame->hypercall_number = 0;                                 /* clean hypercall state */
    stack_frame->hypercall_param[0] = 0;
    stack_frame->hypercall_param[1] = 0;
    stack_frame->hypercall_param[2] = 0;
    stack_frame->hypercall_param[3] = 0;

    /* return task's current stack address */
    return stk;
}

/*
 * The virtual interrupt masking.  The RVM virtual interrupt mask is a plain
 * flag that cannot be snapshot through the PRIMASK, so the official
 * MRS/CPSID pair is replaced by a read of the flag followed by an
 * idempotent mask, and the MSR by a restore of the snapshot - nested
 * sections are balanced by the snapshot, not by a counter.  Writing back
 * "unmasked" must take the full unmask path, or the interrupts that were
 * pended while masked would starve on a bare flag write.
 */
rt_base_t rt_hw_interrupt_disable(void)
{
    rt_base_t level;

    level = (rt_base_t)RVM_Int_Mask;             /* the flag seen on entry */
    if (level == 0)
    {
        RVM_Virt_Int_Mask();
    }

    return level;
}

void rt_hw_interrupt_enable(rt_base_t level)
{
    if (level == 0)
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
 * interrupt context through rt_hw_context_switch_interrupt(), the
 * hypervisor saves the interrupted context and calls this function with
 * the register block made available through RVM_REG, after the interrupt
 * handler has returned.  The saved registers are moved onto the stack of
 * the thread that is being switched out - the thread whose stack pointer
 * location rt_hw_context_switch_interrupt() latched - and the registers of
 * the thread being switched in are loaded into the register block.
 * Returning re-enters the new thread exactly where it was interrupted.
 *
 * The scheduler level switches do not pass through here: they take the
 * fast path in rt_hw_context_switch() instead, but the frame layout is
 * identical, so the two paths can exchange threads freely.
 */
void PendSV_Handler(void)
{
    rt_uint32_t *sp;

    /* Skip the switch when no request is pending, like the official PendSV
     * handler does - at the very first dispatch the from-thread global is
     * still zero, and writing a context block through it would fault. */
    if (rt_thread_switch_interrupt_flag == 0U)
    {
        return;
    }

    /* save the current context onto the from thread's stack - the same
     * sequence the official PendSV handler performs.  A null from thread
     * means no thread has ever been switched out yet, so there is nothing
     * to save - exactly like the official handler's early check. */
    if (rt_interrupt_from_thread != 0U)
    {
        sp = (rt_uint32_t *)RVM_REG->Reg.SP;

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
 * reset CPU
 *
 * The guest cannot write the AIRCR, so the reset goes through the
 * hypervisor.
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
