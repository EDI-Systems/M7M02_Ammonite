//----------------------------------------------------
// Copyright (c) 2026, Evo-Devo Instrum. All rights reserved.
//
// This file is NOT part of the DjyOS distribution.  It belongs to the
// DjyOS-on-RVM port and is released under the Unlicense; refer to the
// LICENSE file for details.  The DjyOS kernel itself is copyright the
// DjyOS Open Source Development team and is not modified by this file.
//----------------------------------------------------
//Module: Hardware interrupt module (RVM guest)
//Author:  Evo-Devo Instrum.
//Version: V1.0.0
//Description: Replaces bsp/arch/arm/cortex-m/armv7-m/cortex-m3/src/
//             int_hard.c for ARMv7(E)-M systems running as an
//             unprivileged guest of the RVM hypervisor.  The interrupt
//             engines are kept verbatim from the original; everything
//             that touched the NVIC is replaced, because the guest has
//             no right to touch it and the RVM hypervisor owns the
//             physical delivery.  Vectors arrive as plain function
//             calls on the hypervisor's vector thread, so the original
//             assembly entries (__start_asyn_signal/__start_real) and
//             the fg_vect_table are replaced by the per-vector
//             trampolines at the bottom of this file.
//             The deferred context switch handler completes the
//             machinery that cpus.s defers to: the hypervisor calls it
//             on its vector thread with the interrupted user context
//             available through RVM_REG, and it performs the same
//             register block transfer as the original
//             __asm_switch_context_int exception return did.
//Notes: Porting sensitive - the mask wrappers map the kernel's critical
//       sections onto the RVM virtual interrupt mask, and no NVIC
//       register may be touched from here.
//Revision history:
//1. Date: 2026-10-29
//   Author:  Evo-Devo Instrum.
//   Version: V1.0.0
//   Change: Initial version for the RVM guest.
//----------------------------------------------------

#include "rvm.h"
#include "int.h"
#include "int_hard.h"
#include "critical.h"
#include "djyos.h"

extern struct IntLine *tg_pIntLineTable[];
extern struct IntMasterCtrl tg_int_global;
extern void __DJY_ScheduleAsynSignal(void);
extern void djy_dbg_puts(const char *str);
extern void djy_dbg_putu32(u32 val);
void __DJY_EventReady(struct EventECB *event_ready);

/* The deferred switch handshake with cpus.s - latched by the switch
 * primitives, consumed by Djy_Ctx_Handler below.  A null from-vm
 * means the outgoing stack is abandoned (event completion switch).
 * The flag is the single-consumption guard of the RT-Thread interrupt
 * switch pattern: the handler skips a notification that carries no
 * armed request, and no latch is ever consumed twice. */
struct ThreadVm *djy_from_vm = NULL;
struct ThreadVm *djy_to_vm = NULL;
volatile u32 djy_switch_flag = 0;

/* Diagnostic counter: how many virtual vectors this guest has taken. */
volatile u32 djy_vct_entry_count = 0;

/* The recycler slot of __asm_reset_switch: the new vm waits here while
 * the old thread's frame is rebuilt, because neither the old stack
 * (exactly the area the rebuild writes) nor any scratch register (a
 * vector delivered mid-rebuild restores its own r0-r3/r12) can carry
 * it across the rebuild call. */
volatile u32 djy_rs_new_vm = 0;

//----Atomic state query-------------------------------------------------------
//The snapshots are the RVM mask flag: nonzero means the corresponding
//tier was already masked when the Start call was made.
bool_t Int_IsLowAtom(atom_low_t AtomStatus)
{
    if(AtomStatus != 0)
        return true;
    else
        return false;
}

bool_t Int_IsHighAtom(atom_high_t AtomStatus)
{
    if(AtomStatus != 0)
        return true;
    else
        return false;
}

//----Switching functions: all mapped onto the RVM virtual interrupt mask------

void Int_ContactAsynSignal(void)
{
    RVM_Virt_Int_Unmask();
}

void Int_CutAsynSignal(void)
{
    RVM_Virt_Int_Mask();
}

void Int_ContactTrunk(void)
{
    RVM_Virt_Int_Unmask();
}

void Int_CutTrunk(void)
{
    RVM_Virt_Int_Mask();
}

//----Interrupt line control: the NVIC is owned by the hypervisor, so every----
//----entry below is a harmless no-op------------------------------------------

bool_t Int_ClearLine(ufast_t ufl_line)
{
    (void)ufl_line;
    return true;
}

bool_t Int_TapLine(ufast_t ufl_line)
{
    (void)ufl_line;
    return false;               /* no software trigger from the guest */
}

bool_t Int_ContactLine(ufast_t ufl_line)
{
    (void)ufl_line;
    return true;
}

bool_t Int_CutLine(ufast_t ufl_line)
{
    (void)ufl_line;
    return true;
}

//----Real-time interrupt engine: verbatim from the original-------------------

void __Int_EngineReal(ufast_t ufl_line)
{
    struct IntLine *ptIntLine;
    tg_int_global.nest_real++;

    ptIntLine = tg_pIntLineTable[ufl_line];
    if(ptIntLine->enable_nest == false)
    {
        Int_CutTrunk();
    }

    if(ptIntLine->ISR != NULL)
        ptIntLine->ISR(ptIntLine->para);

    if(ptIntLine->enable_nest == false)
    {
        Int_ContactTrunk();
    }
    tg_int_global.nest_real--;
}

//----Asynchronous signal engine: verbatim from the original-------------------

void __Int_EngineAsynSignal(ufast_t ufl_line)
{
    struct EventECB *event;
    struct IntLine *ptIntLine;
    u32 isr_result;

    g_bScheduleEnable = false;
    ptIntLine = tg_pIntLineTable[ufl_line];

    tg_int_global.nest_asyn_signal = 1;
    if(ptIntLine->clear_type == CN_INT_CLEAR_AUTO)
        Int_ClearLine(ufl_line);
    if(ptIntLine->ISR != NULL)
        isr_result = ptIntLine->ISR(ptIntLine->para);
    else
    {
        if(ptIntLine->clear_type == CN_INT_CLEAR_USER)
            Int_ClearLine(ufl_line);
        isr_result = 0;
    }
    event = ptIntLine->sync_event;
    if(event != NULL)
    {
        event->event_result = isr_result;
        __DJY_EventReady(event);
        ptIntLine->sync_event = NULL;
    }
    if(ptIntLine->my_evtt_id != CN_EVTT_ID_INVALID)
    {
        DJY_EventPop(ptIntLine->my_evtt_id,
                        NULL, 0, (ptu32_t)isr_result, (ptu32_t)ufl_line, 0);
    }
    tg_int_global.nest_asyn_signal = 0;

    if(g_ptEventReady != g_ptEventRunning)
        __DJY_ScheduleAsynSignal();
    g_bScheduleEnable = true;
    return;
}

//----Deferred context switch handler------------------------------------------
//The hypervisor calls this on its vector thread when a context switch
//has been pended by __asm_switch_context_int (or a dirty-frame
//deferral) and the interrupts are being re-enabled.  It transfers the
//interrupted user context from RVM_REG onto the from-thread's stack -
//building the same 22 word port frame the fast paths build - and loads
//the to-thread's frame into RVM_REG.  Returning re-enters the new
//thread through the kernel's real exception return, which is also the
//only way to restore EPSR ICI/IT state; that is exactly why the dirty
//filter in cpus.s routes here.
/* Diagnostic counters: how many deferred switches were requested from
 * interrupt level, and how many the handler actually performed. */
volatile u32 djy_ctx_runs = 0;
volatile u32 djy_sci_calls = 0;
volatile u32 djy_bad_frames = 0;
volatile u32 djy_defer_hits = 0;
volatile u32 djy_unalign_hits = 0;
volatile u32 djy_turnto_lr = 0;
volatile u32 djy_turnto_vm = 0;
volatile u32 djy_bad_dbg2[4] = {0U, 0U, 0U, 0U};
volatile u32 djy_bad_dbg[4] = {0U, 0U, 0U, 0U};
/* Saves whose suspended context did not live on the charged thread's own
 * stack: the invariant every switch relies on (the from-thread owns the
 * interrupted context) was violated.  Recorded, never expected. */
volatile u32 djy_foreign_saves = 0;
volatile u32 djy_foreign_dbg[4] = {0U, 0U, 0U, 0U};
/* First-switch forensics: {to_vm, to->stack, frame PC, frame XPSR}. */
volatile u32 djy_ctx_dbg[4] = {0U, 0U, 0U, 0U};

void djy_ctx_probe(void);
void djy_ctx_probe(void)
{
    djy_dbg_puts("[CTX-PROBE]");
    while(1);
}

void Djy_Ctx_Handler(void)
{
    u32 *sp;
    struct ThreadVm *from = djy_from_vm;
    struct ThreadVm *to = djy_to_vm;

    if(djy_switch_flag == 0U)
        return;                 /* spurious - no armed switch request */

    if(to == NULL)
    {
        djy_switch_flag = 0U;   /* armed but empty - clear and leave */
        return;
    }
    djy_switch_flag = 0U;

    /* Validate the target frame before touching anything: a frame that
     * does not look like this port's 22 word layout (PC outside the
     * guest image, Thumb bit clear, or the alignment pad marker set)
     * belongs to a context this handler cannot resume.  Dropping the
     * switch keeps the current context running and records the event
     * instead of faulting on an invalid exception return. */
    {
        u32 *f = (u32 *)to->stack;
        u32 pc = f[20];
        u32 xs = f[21];

        if((pc < 0x08050000U) || (pc >= 0x08060000U) ||
           ((xs & 0x01000000U) == 0U) || ((xs & 0x00000200U) != 0U))
        {
            djy_bad_frames++;
            {
                extern struct EventECB *g_ptEventRunning;
                extern struct EventECB *g_ptEventReady;
                if(djy_bad_dbg2[0] == 0U)
                {
                    djy_bad_dbg2[0] = djy_turnto_lr;
                    djy_bad_dbg2[1] = djy_turnto_vm;
                    djy_bad_dbg2[2] = (u32)g_ptEventRunning;
                    djy_bad_dbg2[3] = (u32)g_ptEventReady;
                }
            }
            if(djy_bad_dbg[0] == 0U)
            {
                djy_bad_dbg[0] = (u32)to;
                djy_bad_dbg[1] = (u32)from;
                djy_bad_dbg[2] = pc;
                djy_bad_dbg[3] = xs;
            }
            {
                extern void djy_dbg_putu32(u32 v);
                extern struct EventECB *g_ptEventRunning;
                djy_dbg_puts(" [BADFRAME ev=");
                djy_dbg_putu32(g_ptEventRunning->event_id);
                djy_dbg_puts(" et=");
                djy_dbg_putu32(g_ptEventRunning->evtt_id);
                djy_dbg_puts(" from=");
                djy_dbg_putu32((u32)from);
                djy_dbg_puts(" to=");
                djy_dbg_putu32((u32)to);
                djy_dbg_puts(" pc=");
                djy_dbg_putu32(pc);
                djy_dbg_puts(" st=");
                djy_dbg_putu32((u32)to->stack);
                djy_dbg_puts(" top=");
                djy_dbg_putu32((u32)to->stack_top);
                djy_dbg_puts(" ss=");
                djy_dbg_putu32(to->stack_size);
                djy_dbg_puts(" w[st,top,used,next]=");
                djy_dbg_putu32((u32)to->stack);
                djy_dbg_putu32((u32)to->stack_top);
                djy_dbg_putu32((u32)to->stack_used);
                djy_dbg_putu32((u32)to->next);
                djy_dbg_puts(" wf=");
                {
                    extern struct ThreadVm *g_ptWaitFreeVm;
                    djy_dbg_putu32((u32)g_ptWaitFreeVm);
                }
                djy_dbg_puts(" rvmSP=");
                djy_dbg_putu32((u32)RVM_REG->Reg.SP);
                djy_dbg_puts("] ");
            }
            return;
        }
    }
    djy_ctx_runs++;

    if(djy_ctx_dbg[0] == 0U)
    {
        u32 *f = (u32 *)to->stack;
        djy_ctx_dbg[1] = (u32)to->stack;
        djy_ctx_dbg[2] = f[20];     /* the resume PC */
        djy_ctx_dbg[3] = f[14];     /* R0 - the event routine */
        djy_ctx_dbg[0] = (u32)to;
    }

    if(from != NULL)
    {
        u32 spv = (u32)RVM_REG->Reg.SP;
        u32 in_from;
        u32 in_to;

        /* The suspended context must live on the stack of a thread of
         * this kernel - normally the from-thread's, and in the one
         * legitimate hybrid case (a switch already loaded the target's
         * SP and the delivery landed inside its unmask) the to-thread's.
         * Anywhere else means the switch was requested by a context that
         * is no thread at all, i.e. the startup container; filing such a
         * frame under a thread poisons that thread's saved context and
         * only faults much later, behind an unrelated operation. */
        in_from = (spv >= ((u32)from + (u32)sizeof(struct ThreadVm))) &&
                  (spv <= (u32)from->stack_top);
        in_to = (to != NULL) &&
                (spv >= ((u32)to + (u32)sizeof(struct ThreadVm))) &&
                (spv <= (u32)to->stack_top);
        if((in_from == 0U) && (in_to == 0U))
        {
            djy_foreign_saves++;
            if(djy_foreign_dbg[0] == 0U)
            {
                extern void djy_dbg_putu32(u32 v);
                djy_foreign_dbg[0] = (u32)from;
                djy_foreign_dbg[1] = (u32)to;
                djy_foreign_dbg[2] = spv;
                djy_foreign_dbg[3] = (u32)RVM_REG->Reg.LR;
                djy_dbg_puts(" [FSAVE from=");
                djy_dbg_putu32((u32)from);
                djy_dbg_puts(" to=");
                djy_dbg_putu32((u32)to);
                djy_dbg_puts(" sp=");
                djy_dbg_putu32(spv);
                djy_dbg_puts(" lr=");
                djy_dbg_putu32((u32)RVM_REG->Reg.LR);
                djy_dbg_puts(" num=");
                djy_dbg_putu32((u32)RVM_STATE->Usr.Number);
                djy_dbg_puts("] ");
            }
            from = NULL;            /* never charge it to a thread */
        }
    }

    if(from != NULL)
    {
        sp = (u32 *)RVM_REG->Reg.SP;

        sp -= 1U;                           /* the saved EXC_RETURN */
        *sp = RVM_REG->Reg.LR;
        sp -= 8U;                           /* R4-R11 */
        sp[0] = RVM_REG->Reg.R4;
        sp[1] = RVM_REG->Reg.R5;
        sp[2] = RVM_REG->Reg.R6;
        sp[3] = RVM_REG->Reg.R7;
        sp[4] = RVM_REG->Reg.R8;
        sp[5] = RVM_REG->Reg.R9;
        sp[6] = RVM_REG->Reg.R10;
        sp[7] = RVM_REG->Reg.R11;
        sp -= 5U;                           /* the hypercall parameters */
        sp[0] = RVM_STATE->Usr.Number;
        sp[1] = RVM_STATE->Usr.Param[0];
        sp[2] = RVM_STATE->Usr.Param[1];
        sp[3] = RVM_STATE->Usr.Param[2];
        sp[4] = RVM_STATE->Usr.Param[3];

        from->stack = sp;
    }

    sp = (u32 *)to->stack;

    RVM_STATE->Usr.Number = sp[0];          /* the hypercall parameters */
    RVM_STATE->Usr.Param[0] = sp[1];
    RVM_STATE->Usr.Param[1] = sp[2];
    RVM_STATE->Usr.Param[2] = sp[3];
    RVM_STATE->Usr.Param[3] = sp[4];
    sp += 5U;

    RVM_REG->Reg.R4 = sp[0];                /* R4-R11 */
    RVM_REG->Reg.R5 = sp[1];
    RVM_REG->Reg.R6 = sp[2];
    RVM_REG->Reg.R7 = sp[3];
    RVM_REG->Reg.R8 = sp[4];
    RVM_REG->Reg.R9 = sp[5];
    RVM_REG->Reg.R10 = sp[6];
    RVM_REG->Reg.R11 = sp[7];
    sp += 8U;

    RVM_REG->Reg.LR = *sp;                  /* the saved EXC_RETURN */
    sp += 1U;

    RVM_REG->Reg.SP = (rvm_ptr_t)sp;        /* the hardware frame stays */

    djy_from_vm = NULL;
    djy_to_vm = NULL;
}

//----Fast path dirty frame trap-----------------------------------------------
//Reaching here means the entry filters let a dirty ICI/IT frame onto
//the manual unstacking path, which cannot restore those bits.  Trap
//loudly instead of silently losing the state.
void djy_switch_err(void)
{
    RVM_Virt_Int_Mask();
    djy_dbg_puts("djy: A7M RVM port - dirty XPSR ICI/IT on the fast path!\r\n");
    while(1);
}

//----turnto NULL argument diagnostic trap-------------------------------------
//The scheduler passes a NULL vm to __asm_turnto_context only through a
//broken invariant; trap with the caller return address and the switch
//latch instead of faulting anonymously at address zero.
void djy_turnto_null(void);
void djy_turnto_null(void)
{
    extern void djy_dbg_putu32(u32 val);
    extern struct EventECB *g_ptEventReady;
    extern struct EventECB *g_ptEventRunning;
    RVM_Virt_Int_Mask();
    djy_dbg_puts("djy: turnto(NULL) caller=");
    djy_dbg_putu32((u32)__builtin_return_address(0));
    djy_dbg_puts(" sp=");
    djy_dbg_putu32((u32)__builtin_frame_address(0));
    djy_dbg_puts(" ready=");
    djy_dbg_putu32((u32)g_ptEventReady);
    djy_dbg_puts(" run=");
    djy_dbg_putu32((u32)g_ptEventRunning);
    djy_dbg_puts("\r\n");
    while(1);
}

//----Vector dispatch trampolines----------------------------------------------
//The hypervisor dispatches vectors as void(void) calls; each trampoline
//recovers its DjyOS line number and enters the async signal engine.

static void djy_vct_enter(rvm_ptr_t line)
{
    djy_vct_entry_count++;
    __Int_EngineAsynSignal((ufast_t)line);
}

#define DJY_VCT_TRAMPOLINE(num)                 \
    static void djy_vct_##num(void)             \
    {                                           \
        djy_vct_enter(num);                     \
    }

DJY_VCT_TRAMPOLINE(0)
DJY_VCT_TRAMPOLINE(1)
DJY_VCT_TRAMPOLINE(2)
DJY_VCT_TRAMPOLINE(3)
DJY_VCT_TRAMPOLINE(4)
DJY_VCT_TRAMPOLINE(5)
DJY_VCT_TRAMPOLINE(6)
DJY_VCT_TRAMPOLINE(7)

static void (* const djy_vct_tab[8])(void) =
{
    djy_vct_0, djy_vct_1, djy_vct_2, djy_vct_3,
    djy_vct_4, djy_vct_5, djy_vct_6, djy_vct_7,
};

/* Which virtual vectors this port has attached so far. */
static u32 djy_vct_armed = 0U;

/*
 * Re-arm the hypervisor handler table.  The vector thread's entry calls
 * RVM_Virt_Init() on its very first run - which the hypervisor triggers
 * with the first interrupt, and that can land AFTER the board has
 * already registered everything - and the init wipes the whole table
 * (Tim, Ctx and every vector).  The idle event calls this again through
 * CleanWakeupEvent, and the benchmark calls it before enabling its
 * interrupt source, so the registrations survive that late wipe.
 */
void djy_port_arm(void)
{
    u32 i;

    RVM_Virt_Ctx_Reg(Djy_Ctx_Handler);
    for(i = 0U; i < 8U; i++)
    {
        if((djy_vct_armed & (1UL << i)) != 0U)
            RVM_Virt_Vct_Reg(i, djy_vct_tab[i], 0U);
    }
}

/*
 * Attach a physical interrupt to a DjyOS line: map the physical vector
 * to a virtual vector through the hypervisor, enable the physical
 * interrupt at the lowest priority (the guest cannot touch the NVIC
 * itself - the same two kernel function calls the official RVM test
 * suites use), and register the matching trampoline.  The caller has
 * already connected the ISR/evtt to the DjyOS line through the
 * kernel's Int_IsrConnect family.
 */
bool_t djy_attach_phys(rvm_ptr_t phys_num, rvm_ptr_t vct_num)
{
    if(vct_num >= 8U)
        return false;
    if(RVM_Hyp_Vct_Phys(phys_num, vct_num) != 0)
        return false;
    if(RVM_A7M_Int_Local_Mod(KFN_INT_LOCAL_MOD, phys_num,
                             RVM_A7M_KFN_INT_LOCAL_MOD_PRIO_SET,
                             0xFFU) != 0)
        return false;
    if(RVM_A7M_Int_Local_Mod(KFN_INT_LOCAL_MOD, phys_num,
                             RVM_A7M_KFN_INT_LOCAL_MOD_STATE_SET, 1U) != 0)
        return false;
    djy_vct_armed |= (1UL << vct_num);
    return (RVM_Virt_Vct_Reg(vct_num, djy_vct_tab[vct_num], 0U) == 0);
}
