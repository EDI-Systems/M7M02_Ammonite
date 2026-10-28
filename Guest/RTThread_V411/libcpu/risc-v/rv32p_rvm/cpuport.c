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
 * 2026-10-27     Evo-Devo     first version for the RV32P RVM guest.
 *
 * This file implements the C part of the RT-Thread porting layer for
 * RV32 systems running as an unprivileged guest of the RVM hypervisor:
 * the thread stack initialization, the virtual interrupt masking, the
 * scheduler level switch entry, and the context switch handler that the
 * hypervisor calls for the interrupt driven switches.
 */

#include <rtthread.h>
#include "rvm.h"

#if(RVM_COP_NUM!=0U)
/* The mstatus.FS[1] bit - the bit that marks a stack frame as carrying
 * live coprocessor context (see the RISC-V privileged spec, mstatus.FS);
 * the initial (01) and off (00) states do not carry any FPU state. */
#define RTT_FPU_CONTEXT_DIRTY_BIT       (1UL<<14UL)

/* The coprocessor register save area that the hypervisor latches for
 * the interrupted thread; the F0..F31 fields are laid out contiguously,
 * so they are copied through a single walking pointer. */
#define RTT_FPU_CONTEXT                 ((volatile struct RVM_RV32P_Cop_Struct*)(RVM_REG->Cop))
#endif

/* flag in interrupt handling */
rt_ubase_t rt_interrupt_from_thread = 0;
rt_ubase_t rt_interrupt_to_thread = 0;
rt_uint32_t rt_thread_switch_interrupt_flag = 0;

/* The gp value for the user land, from the linker script. */
extern rt_ubase_t __RVM_Global;

/*
 * void rt_hw_context_switch_swt(rt_ubase_t from, rt_ubase_t to);  -  the
 * raw fast-path switch body in context_gcc.s, entered with the switch
 * pair latched into the port globals and the virtual interrupts masked
 * by the scheduler's critical section.
 */
extern void rt_hw_context_switch_swt(rt_ubase_t from, rt_ubase_t to);

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
    rt_ubase_t *stk;
    rt_uint32_t i;

    stk  = (rt_ubase_t *)stack_addr + 1;
    stk  = (rt_ubase_t *)RT_ALIGN_DOWN((rt_ubase_t)stk, 4);
    stk -= 37;

    /* the frame, from the stack pointer upwards:  the five hypercall
     * parameter words, the mstatus, and the integer registers - the
     * same layout that the fast path in context_gcc.s and
     * PendSV_Handler() below build */
    for (i = 0U; i < 37U; i++)
    {
        stk[i] = 0xdeadbeef;
    }

    stk[36] = 0x31313131U;              /* x31/t6 */
    stk[35] = 0x30303030U;              /* x30/t5 */
    stk[34] = 0x29292929U;              /* x29/t4 */
    stk[33] = 0x28282828U;              /* x28/t3 */
    stk[32] = 0x27272727U;              /* x27/s11 */
    stk[31] = 0x26262626U;              /* x26/s10 */
    stk[30] = 0x25252525U;              /* x25/s9 */
    stk[29] = 0x24242424U;              /* x24/s8 */
    stk[28] = 0x23232323U;              /* x23/s7 */
    stk[27] = 0x22222222U;              /* x22/s6 */
    stk[26] = 0x21212121U;              /* x21/s5 */
    stk[25] = 0x20202020U;              /* x20/s4 */
    stk[24] = 0x19191919U;              /* x19/s3 */
    stk[23] = 0x18181818U;              /* x18/s2 */
    stk[22] = 0x17171717U;              /* x17/a7 */
    stk[21] = 0x16161616U;              /* x16/a6 */
    stk[20] = 0x15151515U;              /* x15/a5 */
    stk[19] = 0x14141414U;              /* x14/a4 */
    stk[18] = 0x13131313U;              /* x13/a3 */
    stk[17] = 0x12121212U;              /* x12/a2 */
    stk[16] = 0x11111111U;              /* x11/a1 */
    stk[15] = (rt_ubase_t)parameter;    /* x10/a0 */
    stk[14] = 0x09090909U;              /* x9/s1 */
    stk[13] = 0x08080808U;              /* x8/s0 */
    stk[12] = 0x07070707U;              /* x7/t2 */
    stk[11] = 0x06060606U;              /* x6/t1 */
    stk[10] = 0x05050505U;              /* x5/t0 */
    stk[9]  = 0x04040404U;              /* x4/tp - trashed on resume */
    stk[8]  = __RVM_Global;             /* x3/gp - the user global pointer */
    stk[7]  = (rt_ubase_t)texit;        /* x1/ra - the thread exit */
    stk[6]  = (rt_ubase_t)tentry;       /* the resume pc */
#if(RVM_COP_NUM!=0U)
    stk[5]  = 0x00002088U;              /* mstatus: U-mode, FS=initial - the
                                         * first floating point instruction
                                         * marks the state dirty and the
                                         * context is lazily saved from
                                         * there on */
#else
    stk[5]  = 0x00000088U;              /* mstatus: U-mode, FS=off */
#endif
    stk[4]  = 0U;                       /* the hypercall parameters */
    stk[3]  = 0U;
    stk[2]  = 0U;
    stk[1]  = 0U;
    stk[0]  = 0U;

    return (rt_uint8_t *)stk;
}

/*
 * void rt_hw_context_switch_interrupt(rt_ubase_t from, rt_ubase_t to);
 *
 * Called by rt_schedule() when a switch is requested from an interrupt
 * context.  Like the official ports, only the switch bookkeeping is
 * done here: the from/to stack pointer slots are latched and the flag
 * is raised, and the switch itself is deferred through the RVM yield
 * hypercall, which on the vector thread only marks the context switch
 * pending.  The actual switch happens when the vector thread's dispatch
 * loop calls PendSV_Handler() after the interrupt handler returns.
 */
void rt_hw_context_switch_interrupt(rt_ubase_t from, rt_ubase_t to)
{
    if (rt_thread_switch_interrupt_flag == 0U)
    {
        rt_interrupt_from_thread = from;
    }

    rt_interrupt_to_thread = to;
    rt_thread_switch_interrupt_flag = 1U;

    RVM_Virt_Yield();
}

/*
 * void rt_hw_context_switch(rt_ubase_t from, rt_ubase_t to);
 *
 * Called by rt_schedule() from thread context, with the virtual
 * interrupts already masked by the scheduler's critical section.
 * rt_schedule() has already selected the to-thread, so the switch is
 * performed directly on the thread stacks by the fast path in
 * context_gcc.s.  The RISC-V machine state has no partially restorable
 * fields - the whole pc/mstatus pair travels in the frame - so, unlike
 * the ARM ports, every frame can take the fast path.
 */
void rt_hw_context_switch(rt_ubase_t from, rt_ubase_t to)
{
    rt_interrupt_from_thread = from;
    rt_interrupt_to_thread = to;

    /* Cancel any interrupt request that is still pending: this switch
     * supersedes it, and PendSV_Handler() must never replay it later
     * against this already-executed pair.  The whole switch runs under
     * the caller's mask, so the cancellation is atomic. */
    rt_thread_switch_interrupt_flag = 0U;

    rt_hw_context_switch_swt(from, to);
}

/*
 * rt_base_t rt_hw_interrupt_disable(void);  and the enable counterpart -
 * the RVM virtual interrupt mask with snapshot semantics, exactly like
 * the MIE bit of the official ports.
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
 * the board initialization.  It plays the role that the trap handler
 * plays on the official ports: when a switch has been requested from an
 * interrupt context through rt_hw_context_switch_interrupt(), the
 * hypervisor saves the interrupted context and calls this function with
 * the register block made available through RVM_REG, after the interrupt
 * handler has returned.  The saved registers are moved onto the stack of
 * the thread that is being switched out - the thread whose stack pointer
 * slot the request latched - and the registers of the thread being
 * switched in are loaded into the register block.  Returning re-enters
 * the new thread exactly where it was interrupted.
 */
void PendSV_Handler(void)
{
#if(RVM_COP_NUM!=0U)
    volatile struct RVM_RV32P_Cop_Struct *cop;
    rt_ubase_t *fpu;
    rt_uint32_t i;
#endif
    rt_ubase_t *sp;
    rt_ubase_t mstatus;

    /* Skip the switch when no request is pending - a leftover flag from
     * a request that the fast path superseded must never replay a
     * switch. */
    if (rt_thread_switch_interrupt_flag == 0U)
    {
        return;
    }

    /* save the current context onto the from thread's stack.  A null
     * from thread means no thread has ever been switched out yet, so
     * there is nothing to save. */
    if (rt_interrupt_from_thread != 0U)
    {
        sp = (rt_ubase_t *)RVM_REG->Reg.X2_SP;
        sp -= 31U;                          /* the integer registers */
        sp[30] = RVM_REG->Reg.X31_T6;
        sp[29] = RVM_REG->Reg.X30_T5;
        sp[28] = RVM_REG->Reg.X29_T4;
        sp[27] = RVM_REG->Reg.X28_T3;
        sp[26] = RVM_REG->Reg.X27_S11;
        sp[25] = RVM_REG->Reg.X26_S10;
        sp[24] = RVM_REG->Reg.X25_S9;
        sp[23] = RVM_REG->Reg.X24_S8;
        sp[22] = RVM_REG->Reg.X23_S7;
        sp[21] = RVM_REG->Reg.X22_S6;
        sp[20] = RVM_REG->Reg.X21_S5;
        sp[19] = RVM_REG->Reg.X20_S4;
        sp[18] = RVM_REG->Reg.X19_S3;
        sp[17] = RVM_REG->Reg.X18_S2;
        sp[16] = RVM_REG->Reg.X17_A7;
        sp[15] = RVM_REG->Reg.X16_A6;
        sp[14] = RVM_REG->Reg.X15_A5;
        sp[13] = RVM_REG->Reg.X14_A4;
        sp[12] = RVM_REG->Reg.X13_A3;
        sp[11] = RVM_REG->Reg.X12_A2;
        sp[10] = RVM_REG->Reg.X11_A1;
        sp[9]  = RVM_REG->Reg.X10_A0;
        sp[8]  = RVM_REG->Reg.X9_S1;
        sp[7]  = RVM_REG->Reg.X8_S0_FP;
        sp[6]  = RVM_REG->Reg.X7_T2;
        sp[5]  = RVM_REG->Reg.X6_T1;
        sp[4]  = RVM_REG->Reg.X5_T0;
        sp[3]  = RVM_REG->Reg.X4_TP;
        sp[2]  = RVM_REG->Reg.X3_GP;
        sp[1]  = RVM_REG->Reg.X1_RA;
        sp[0]  = RVM_REG->Reg.PC;

        mstatus = RVM_REG->Reg.MSTATUS;     /* the FS field decides whether
                                             * the frame carries the FPU */
#if(RVM_COP_NUM!=0U)
        if ((mstatus & RTT_FPU_CONTEXT_DIRTY_BIT) != 0U)
        {
            sp -= 33U;                      /* the coprocessor block */
            cop = RTT_FPU_CONTEXT;
            fpu = (rt_ubase_t *)sp;
            for (i = 0U; i < 32U; i++)
            {
                fpu[i] = ((volatile rt_ubase_t *)cop)[i];
            }
            fpu[32] = cop->FCSR;
        }
#endif

        sp -= 1U;                           /* the saved mstatus */
        sp[0] = mstatus;

        sp -= 5U;                           /* the hypercall parameters */
        sp[4] = RVM_STATE->Usr.Param[3];
        sp[3] = RVM_STATE->Usr.Param[2];
        sp[2] = RVM_STATE->Usr.Param[1];
        sp[1] = RVM_STATE->Usr.Param[0];
        sp[0] = RVM_STATE->Usr.Number;

        /* write the new sp into the from thread's TCB slot */
        *(rt_ubase_t *)rt_interrupt_from_thread = (rt_ubase_t)sp;
    }

    /* clear rt_thread_switch_interrupt_flag to 0 */
    rt_thread_switch_interrupt_flag = 0U;

    /* load the to thread's stack pointer from its TCB slot */
    sp = (rt_ubase_t *)(*(rt_ubase_t *)rt_interrupt_to_thread);

    RVM_STATE->Usr.Number = sp[0];          /* the hypercall parameters */
    RVM_STATE->Usr.Param[0] = sp[1];
    RVM_STATE->Usr.Param[1] = sp[2];
    RVM_STATE->Usr.Param[2] = sp[3];
    RVM_STATE->Usr.Param[3] = sp[4];
    sp += 5U;

    mstatus = sp[0];                        /* the saved mstatus */
    sp += 1U;

#if(RVM_COP_NUM!=0U)
    if ((mstatus & RTT_FPU_CONTEXT_DIRTY_BIT) != 0U)
    {
        /* recover the coprocessor block */
        cop = RTT_FPU_CONTEXT;
        fpu = (rt_ubase_t *)sp;
        for (i = 0U; i < 32U; i++)
        {
            ((volatile rt_ubase_t *)cop)[i] = fpu[i];
        }
        cop->FCSR = (rvm_ptr_t)fpu[32];
        sp += 33U;
    }
#endif
    RVM_REG->Reg.MSTATUS = (rvm_ptr_t)mstatus;

    RVM_REG->Reg.PC = sp[0];                /* the integer registers */
    RVM_REG->Reg.X1_RA = sp[1];
    RVM_REG->Reg.X3_GP = sp[2];
    RVM_REG->Reg.X4_TP = sp[3];
    RVM_REG->Reg.X5_T0 = sp[4];
    RVM_REG->Reg.X6_T1 = sp[5];
    RVM_REG->Reg.X7_T2 = sp[6];
    RVM_REG->Reg.X8_S0_FP = sp[7];
    RVM_REG->Reg.X9_S1 = sp[8];
    RVM_REG->Reg.X10_A0 = sp[9];
    RVM_REG->Reg.X11_A1 = sp[10];
    RVM_REG->Reg.X12_A2 = sp[11];
    RVM_REG->Reg.X13_A3 = sp[12];
    RVM_REG->Reg.X14_A4 = sp[13];
    RVM_REG->Reg.X15_A5 = sp[14];
    RVM_REG->Reg.X16_A6 = sp[15];
    RVM_REG->Reg.X17_A7 = sp[16];
    RVM_REG->Reg.X18_S2 = sp[17];
    RVM_REG->Reg.X19_S3 = sp[18];
    RVM_REG->Reg.X20_S4 = sp[19];
    RVM_REG->Reg.X21_S5 = sp[20];
    RVM_REG->Reg.X22_S6 = sp[21];
    RVM_REG->Reg.X23_S7 = sp[22];
    RVM_REG->Reg.X24_S8 = sp[23];
    RVM_REG->Reg.X25_S9 = sp[24];
    RVM_REG->Reg.X26_S10 = sp[25];
    RVM_REG->Reg.X27_S11 = sp[26];
    RVM_REG->Reg.X28_T3 = sp[27];
    RVM_REG->Reg.X29_T4 = sp[28];
    RVM_REG->Reg.X30_T5 = sp[29];
    RVM_REG->Reg.X31_T6 = sp[30];
    RVM_REG->Reg.X2_SP = (rvm_ptr_t)(sp + 31U);
}

/**
 * reset CPU - the guest cannot reset the machine, so the reset goes
 * through the hypervisor.
 */
RT_WEAK void rt_hw_cpu_reset(void)
{
    RVM_Hyp_Reboot();
}

RT_WEAK void rt_hw_cpu_shutdown(void)
{
    rt_base_t level;

    rt_kprintf("shutdown...\n");

    level = rt_hw_interrupt_disable();
    while (level)
    {
        RT_ASSERT(0);
    }
}

/*
 * rt_uint32_t rt_hw_interrupt_check(void);
 *
 * Reports whether the caller is running in an interrupt context.  The
 * physical interrupts are taken by the hypervisor and relayed to this
 * virtual machine as plain function calls on the vector thread, so the
 * processor never sees a trap here - the authoritative indicator is the
 * RVM state block's Vct_Act flag, which the vector thread sets while it
 * is dispatching a virtual interrupt.
 */
rt_uint32_t rt_hw_interrupt_check(void)
{
    return (RVM_STATE->Vct_Act != 0U) ? 1U : 0U;
}
