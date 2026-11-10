/* SPDX-License-Identifier: Unlicense */

#include <stddef.h>
#include <string.h>

#include <zephyr/kernel.h>
#include <zephyr/init.h>

#include "rvm.h"
#include "zpr_config.h"
#include "zpr_abi.h"

/*
 * This layout must stay in sync with the RVM fields in
 * include/zephyr/arch/arm/thread.h.  register_sp is at byte offset 36,
 * which is what the shared RVM slow-path assembly expects.
 */
typedef struct zpr_ctx {
	uint32_t r4;
	uint32_t r5;
	uint32_t r6;
	uint32_t r7;
	uint32_t r8;
	uint32_t r9;
	uint32_t r10;
	uint32_t r11;
	void *lr;
	void *sp;
	uint32_t nest;
	void *owner;
} zpr_ctx_t;

extern void z_thread_entry(k_thread_entry_t entry, void *p1, void *p2,
			   void *p3);
extern void _RVM_CPU_Context_switch_yield(void *executing, void *heir,
					  void *usr_params);
extern void _RVM_CPU_Context_switch_swt(void *executing, void *heir,
					void *usr_params);
extern void zpr_irq_restore_nest(unsigned int level);
extern void z_reschedule_irqlock(uint32_t key);
extern unsigned int zpr_irq_get_nest(void);

zpr_ctx_t *zpr_pending_heir;
static uint32_t zpr_usr_params[5];
static bool zpr_isr_captured;

_Static_assert(offsetof(zpr_ctx_t, sp) == ZPR_CTX_SP_OFFSET,
	       "zpr_ctx_t SP offset must match the RVM assembly");
_Static_assert(offsetof(zpr_ctx_t, nest) == ZPR_CTX_NEST_OFFSET,
	       "zpr_ctx_t nest offset must match the RVM assembly");
_Static_assert(offsetof(struct _thread_arch, rvm_sp) -
	       offsetof(struct _thread_arch, rvm_r4) == ZPR_CTX_SP_OFFSET,
	       "thread RVM SP offset must match zpr_ctx_t");
_Static_assert(offsetof(struct _thread_arch, rvm_isr_nest) -
	       offsetof(struct _thread_arch, rvm_r4) == ZPR_CTX_NEST_OFFSET,
	       "thread RVM nest offset must match zpr_ctx_t");
_Static_assert(ZPR_BASIC_FRAME_WORDS ==
	       ZPR_FRAME_PARAMS + ZPR_FRAME_GP + ZPR_FRAME_BASIC,
	       "basic RVM frame composition");
_Static_assert(ZPR_EXT_FRAME_WORDS ==
	       ZPR_FRAME_PARAMS + ZPR_FRAME_GP + ZPR_FRAME_FP16 +
	       ZPR_FRAME_BASIC + ZPR_FRAME_FP_AREA,
	       "extended RVM frame composition");
_Static_assert(ZPR_BASIC_HW_WORD == ZPR_FRAME_PARAMS + ZPR_FRAME_GP,
	       "basic hardware frame offset");
_Static_assert(ZPR_EXT_HW_WORD ==
	       ZPR_FRAME_PARAMS + ZPR_FRAME_GP + ZPR_FRAME_FP16,
	       "extended hardware frame offset");
#if IS_ENABLED(CONFIG_FPU) && (RVM_COP_NUM == 0U)
#error "CONFIG_FPU requires an RVM coprocessor context (RVM_COP_NUM != 0)"
#endif

static bool zpr_exc_return_is_valid(uint32_t exc)
{
	switch (exc) {
	case 0xFFFFFFF1U:
	case 0xFFFFFFF9U:
	case 0xFFFFFFFDU:
	case 0xFFFFFFE1U:
	case 0xFFFFFFE9U:
	case 0xFFFFFFEDU:
		return true;
	default:
		return false;
	}
}

static void zpr_ctx_fail(void)
{
	static const char msg[] = "\r\nZPR ctx bad frame\r\n";
	size_t index;

	for (index = 0U; index < sizeof(msg) - 1U; ++index) {
		RVM_Hyp_Putchar(msg[index]);
	}

	for (;;) {
	}
}

static bool zpr_frame_is_executable(const uint32_t *sp)
{
	uint32_t exc;
	uint32_t hw;
	uint32_t pc;
	uint32_t xpsr;

	if ((uintptr_t) sp < DATA_DATA1_BASE
	    || (uintptr_t) sp >= RVM_VIRT_REG_BASE) {
		return false;
	}

	exc = sp[13];
	if (zpr_exc_return_is_valid(exc) == false) {
		return false;
	}

	hw = (exc & 0x10U) == 0U ? ZPR_EXT_HW_WORD : ZPR_BASIC_HW_WORD;
	pc = sp[hw + 6U];
	xpsr = sp[hw + 7U];

	return pc >= (ZPR_CODE_BASE + 0x20U) && pc < (ZPR_CODE_BASE + ZPR_CODE_SIZE)
		&& (xpsr & ZPR_XPSR_ICI_IT_MASK) == 0U;
}

static bool zpr_frame_is_dirty(const zpr_ctx_t *heir)
{
	const uint32_t *sp;
	uint32_t exc;
	uint32_t off;

	sp = (const uint32_t *) heir->sp;
	if (zpr_frame_is_executable(sp) == false) {
		return true;
	}

	exc = sp[13];
	off = (exc & 0x10U) == 0U ? 37U : 21U;

	return (sp[off] & ZPR_XPSR_ICI_IT_MASK) != 0U;
}

static bool zpr_xpsr_is_dirty(void)
{
	uint32_t xpsr;

	__asm__ volatile ("mrs %0, xpsr" : "=r" (xpsr));

	return (xpsr & ZPR_XPSR_ICI_IT_MASK) != 0U;
}

static bool zpr_capture_executing(zpr_ctx_t *executing)
{
	uint32_t *sp;
	uint32_t exc;
	uintptr_t reserve;

	sp = (uint32_t *) RVM_REG->Reg.SP;
	exc = RVM_REG->Reg.LR;

	if (zpr_exc_return_is_valid(exc) == false) {
		return false;
	}

	reserve = (ZPR_FRAME_PARAMS + ZPR_FRAME_GP) * 4U;
#if RVM_COP_NUM != 0U
	if ((exc & 0x10U) == 0U) {
		reserve += ZPR_FRAME_FP16 * 4U;
	}
#endif

	if ((uintptr_t) sp < (DATA_DATA1_BASE + reserve)
	    || (uintptr_t) sp >= RVM_VIRT_REG_BASE) {
		return false;
	}

#if RVM_COP_NUM != 0U
	if ((exc & 0x10U) == 0U) {
		volatile struct RVM_A7M_Cop_Struct *cop =
			(volatile struct RVM_A7M_Cop_Struct *) RVM_REG->Cop;

		*--sp = cop->S31;
		*--sp = cop->S30;
		*--sp = cop->S29;
		*--sp = cop->S28;
		*--sp = cop->S27;
		*--sp = cop->S26;
		*--sp = cop->S25;
		*--sp = cop->S24;
		*--sp = cop->S23;
		*--sp = cop->S22;
		*--sp = cop->S21;
		*--sp = cop->S20;
		*--sp = cop->S19;
		*--sp = cop->S18;
		*--sp = cop->S17;
		*--sp = cop->S16;
	}
#else
	if ((exc & 0x10U) == 0U) {
		return false;
	}
#endif

	*--sp = exc;
	*--sp = RVM_REG->Reg.R11;
	*--sp = RVM_REG->Reg.R10;
	*--sp = RVM_REG->Reg.R9;
	*--sp = RVM_REG->Reg.R8;
	*--sp = RVM_REG->Reg.R7;
	*--sp = RVM_REG->Reg.R6;
	*--sp = RVM_REG->Reg.R5;
	*--sp = RVM_REG->Reg.R4;

	*--sp = RVM_STATE->Usr.Param[3];
	*--sp = RVM_STATE->Usr.Param[2];
	*--sp = RVM_STATE->Usr.Param[1];
	*--sp = RVM_STATE->Usr.Param[0];
	*--sp = RVM_STATE->Usr.Number;

	executing->lr = (void *) 0;
	executing->sp = sp;
	executing->nest = zpr_irq_get_nest();
	executing->owner = _current;

	return true;
}

void zpr_reschedule_from_vector(void)
{
	zpr_ctx_t *executing;
	struct k_thread *next;

	next = _kernel.ready_q.cache;
	if (next != _current && zpr_isr_captured == false) {
		executing = (zpr_ctx_t *) &_current->arch.rvm_r4;
		if (zpr_capture_executing(executing) == false) {
			zpr_ctx_fail();
		}

		zpr_isr_captured = true;
		zpr_pending_heir = (zpr_ctx_t *) &next->arch.rvm_r4;
		RVM_Virt_Yield();
		RVM_Virt_Int_Unmask();
	}
}

static void zpr_init_frame(struct k_thread *thread, char *stack_ptr,
			   k_thread_entry_t entry, void *p1, void *p2,
			   void *p3)
{
	uintptr_t end;
	uint32_t *sp;
	zpr_ctx_t *ctx;

	/*
	 * Every thread starts from a basic frame, including K_FP_REGS threads.
	 * No FP context exists before the thread executes its first VFP
	 * instruction; that instruction sets CONTROL.FPCA, and the next save
	 * builds the 56-word extended frame.  This matches native Zephyr and
	 * RMP and avoids reserving 136 extra bytes for a thread that may never
	 * use FP.
	 */
	end = ((uintptr_t) stack_ptr) & ~(uintptr_t) 7U;
	sp = (uint32_t *) (end - ZPR_BASIC_FRAME_WORDS * 4U);
	memset(sp, 0, ZPR_BASIC_FRAME_WORDS * 4U);

	/* hardware frame: R0-R3, R12, LR, PC, xPSR */
	sp[ZPR_BASIC_HW_WORD + 0U] = (uint32_t) entry;
	sp[ZPR_BASIC_HW_WORD + 1U] = (uint32_t) p1;
	sp[ZPR_BASIC_HW_WORD + 2U] = (uint32_t) p2;
	sp[ZPR_BASIC_HW_WORD + 3U] = (uint32_t) p3;
	sp[ZPR_BASIC_HW_WORD + 5U] = 0U;
	sp[ZPR_BASIC_HW_WORD + 6U] = ((uint32_t) z_thread_entry) | 1U;
	sp[ZPR_BASIC_HW_WORD + 7U] = 0x01000000U;

	/* EXC_RETURN is the last word of the RVM GP block. */
	sp[13] = ZPR_EXC_RETURN_BASIC;

	ctx = (zpr_ctx_t *) &thread->arch.rvm_r4;
	memset(ctx, 0, sizeof(*ctx));
	ctx->lr = (void *) 1U;
	ctx->sp = sp;
	ctx->nest = 0U;
	ctx->owner = thread;

	thread->callee_saved.psp = (uint32_t) sp;
	thread->arch.basepri = 0U;
}

void zpr_thread_init(struct k_thread *thread, char *stack_ptr,
		     k_thread_entry_t entry, void *p1, void *p2, void *p3)
{
	zpr_init_frame(thread, stack_ptr, entry, p1, p2, p3);
}

void zpr_switch_to_main(struct k_thread *thread, char *stack_ptr,
			k_thread_entry_t entry)
{
	zpr_init_frame(thread, stack_ptr, entry, NULL, NULL, NULL);

	/*
	 * Publish the main-thread state before opening the RVM interrupt
	 * gates.  This is the ZPR equivalent of arch_switch_to_main_thread():
	 * native Zephyr keeps interrupts disabled through z_cstart() and
	 * enables them only at this final switch.
	 */
	_current = thread;
	zpr_pending_heir = (zpr_ctx_t *) &thread->arch.rvm_r4;
	RVM_Hyp_Int_Ena();
	RVM_Virt_Int_Unmask();
	RVM_Virt_Yield();
	RVM_Virt_Int_Unmask();

	for (;;) {
	}
}

int zpr_arch_swap(unsigned int key)
{
	zpr_ctx_t *executing;
	zpr_ctx_t *heir;
	struct k_thread *next;
	uint32_t *sp;

	next = _kernel.ready_q.cache;
	if (next == _current) {
		return _current->arch.swap_return_value;
	}

	executing = (zpr_ctx_t *) &_current->arch.rvm_r4;
	heir = (zpr_ctx_t *) &next->arch.rvm_r4;
	executing->nest = key;

	/*
	 * Fast path only for a clean current XPSR and a clean basic or
	 * extended frame.  Dirty frames must take the monitor's real
	 * exception return.  The saved PC is even by construction
	 * (ZPR_EXC_PUSH clears bit 0); Cortex-M stays in Thumb state and
	 * ZPR_EXC_POP sets bit 0 on resume, so an odd-PC test here would
	 * reject every normal frame.  A frame captured by the vector path
	 * has lr == NULL and must be resumed by the monitor, not here.
	 */
	sp = (uint32_t *) heir->sp;
#if ZPR_FAST_PATH
	if (sp != NULL
	    && heir->lr != NULL
	    && !zpr_frame_is_dirty(heir)
	    && !zpr_xpsr_is_dirty()) {
		_current = next;
		_RVM_CPU_Context_switch_swt(executing, heir, zpr_usr_params);
	} else {
		executing->lr = (void *) 1U;
		_RVM_CPU_Context_switch_yield(executing, heir, zpr_usr_params);
	}
#else
	executing->lr = (void *) 1U;
	_RVM_CPU_Context_switch_yield(executing, heir, zpr_usr_params);
#endif

	return _current->arch.swap_return_value;
}

void zpr_ctx_handler(void)
{
	zpr_ctx_t *heir;
	uint32_t *sp;
	uint32_t exc;

	heir = zpr_pending_heir;
	if (heir == NULL || heir->sp == NULL) {
		return;
	}

	zpr_pending_heir = NULL;
	sp = (uint32_t *) heir->sp;

	if (zpr_frame_is_executable(sp) == false) {
		zpr_ctx_fail();
	}

	RVM_STATE->Usr.Number = *sp++;
	RVM_STATE->Usr.Param[0] = *sp++;
	RVM_STATE->Usr.Param[1] = *sp++;
	RVM_STATE->Usr.Param[2] = *sp++;
	RVM_STATE->Usr.Param[3] = *sp++;

	RVM_REG->Reg.R4 = *sp++;
	RVM_REG->Reg.R5 = *sp++;
	RVM_REG->Reg.R6 = *sp++;
	RVM_REG->Reg.R7 = *sp++;
	RVM_REG->Reg.R8 = *sp++;
	RVM_REG->Reg.R9 = *sp++;
	RVM_REG->Reg.R10 = *sp++;
	RVM_REG->Reg.R11 = *sp++;
	RVM_REG->Reg.LR = *sp++;
	exc = RVM_REG->Reg.LR;

#if RVM_COP_NUM != 0U
	if ((exc & 0x10U) == 0U) {
		volatile struct RVM_A7M_Cop_Struct *cop =
			(volatile struct RVM_A7M_Cop_Struct *) RVM_REG->Cop;

		cop->S16 = *sp++;
		cop->S17 = *sp++;
		cop->S18 = *sp++;
		cop->S19 = *sp++;
		cop->S20 = *sp++;
		cop->S21 = *sp++;
		cop->S22 = *sp++;
		cop->S23 = *sp++;
		cop->S24 = *sp++;
		cop->S25 = *sp++;
		cop->S26 = *sp++;
		cop->S27 = *sp++;
		cop->S28 = *sp++;
		cop->S29 = *sp++;
		cop->S30 = *sp++;
		cop->S31 = *sp++;
	}
#else
	if ((exc & 0x10U) == 0U) {
		zpr_ctx_fail();
	}
#endif

	RVM_REG->Reg.SP = (rvm_ptr_t) sp;

	zpr_irq_restore_nest(heir->nest);
	_current = heir->owner;
	zpr_isr_captured = false;
}

static int zpr_ctx_init(void)
{
	RVM_Virt_Ctx_Reg(zpr_ctx_handler);
	return 0;
}

SYS_INIT(zpr_ctx_init, PRE_KERNEL_1, 0);

void zpr_cpu_idle(void)
{
	zpr_irq_restore_nest(0);
	if (_kernel.ready_q.cache != _current) {
		z_reschedule_irqlock(arch_irq_lock());
	} else {
		RVM_Hyp_Vct_Wait();
	}
}
