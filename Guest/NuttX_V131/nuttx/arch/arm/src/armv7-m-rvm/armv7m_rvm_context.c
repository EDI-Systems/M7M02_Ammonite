/****************************************************************************
 * arch/arm/src/armv7-m-rvm/armv7m_rvm_context.c
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * RVM vector-thread context handling for the NTX guest.
 *
 * A task level switch cannot simply ask the monitor to save the caller:
 * RVM_Virt_Yield() only posts the context switch flag to the vector thread.
 * ntx_context_switch() (armv7m_rvm_switch.S) therefore masquerades the
 * current context as an RVM exception frame on the current stack and stores
 * its address in the TCB.  The monitor performs the physical switch when
 * the vector thread has loaded the heir into RVM_REG.
 *
 * A switch requested while the vector thread is running an interrupt is
 * different: RVM_REG already describes the interrupted user context, and
 * the hardware frame is on its stack.  That state is copied into the TCB
 * below the hardware frame and loaded the same way.
 *
 * Frame layout, from rvm_sp upwards:
 *
 *   Number, Param[0..3]                 5 words
 *   R4..R11, EXC_RETURN                 9 words
 *   S16..S31                           16 words, extended only
 *   R0..R3, R12, LR, PC, xPSR           8 words
 *   S0..S15, FPSCR, [pad]              18 words, extended only
 ****************************************************************************/

#include <nuttx/config.h>

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include <nuttx/arch.h>
#include <nuttx/irq.h>
#include <nuttx/sched.h>

#include "sched/sched.h"
#include "arm_internal.h"
#include "armv7m_rvm_abi.h"
#include "armv7m_rvm_internal.h"

#include "rvm.h"

#if RVM_COP_NUM != 0
#  define NTX_HAS_COP 1
#else
#  define NTX_HAS_COP 0
#endif

/* armv7m_rvm_switch.S addresses these fields by fixed offset. */

_Static_assert(offsetof(struct xcptcontext, rvm_sp) == 0,
               "rvm_sp must be the first xcptcontext field");
_Static_assert(offsetof(struct xcptcontext, rvm_nest) == 4,
               "rvm_nest must be the second xcptcontext field");
_Static_assert(offsetof(struct xcptcontext, rvm_sw) == 8,
               "rvm_sw must be the third xcptcontext field");

extern char __RVM_Code_Start[];
extern char __RVM_Code_End[];

/* Virtual interrupt nesting of the running user thread.  Zero means the RVM
 * virtual interrupt mask is open.
 */

volatile uint32_t g_ntx_irqnest = 1;

/* Nesting used by up_irq_save()/up_irq_restore() while the vector thread is
 * executing an interrupt handler.  It never touches the RVM mask: the
 * vector thread is already running at monitor priority.
 */

static volatile uint32_t g_ntx_vctnest;

static struct tcb_s *g_ntx_pending_to;
static volatile bool g_ntx_pending;

/* The task the monitor is actually running.  This is not always the head of
 * the ready-to-run list: NuttX moves the head before up_switch_context() is
 * called, while the physical switch still has to happen.  An interrupt that
 * arrives in that window must not capture the scheduler head (it is not the
 * context described by RVM_REG) and must not overwrite the frame that
 * ntx_context_switch() already saved.
 */

static struct tcb_s *g_ntx_running;

/* Native view handed to irq_dispatch() while a virtual interrupt runs. */

static uint32_t g_ntx_irq_regs[XCPTCONTEXT_REGS];

extern FAR struct tcb_s *g_running_tasks[CONFIG_SMP_NCPUS];


/****************************************************************************
 * Private Functions
 ****************************************************************************/

static bool ntx_exc_valid(uint32_t exc)
{
  switch (exc)
    {
      case 0xfffffffd: /* Thread mode, PSP, basic */
      case 0xffffffed: /* Thread mode, PSP, extended */
        return true;
      default:
        return false;
    }
}

static inline bool ntx_is_extended(uint32_t exc)
{
  return (exc & 0x10U) == 0U;
}

static inline uint32_t ntx_hw_word(uint32_t exc)
{
  return ntx_is_extended(exc) ? NTX_RVM_EXT_HW : NTX_RVM_BASIC_HW;
}

static inline uint32_t ntx_frame_words(uint32_t exc)
{
  return ntx_is_extended(exc) ? NTX_RVM_EXT_WORDS : NTX_RVM_BASIC_WORDS;
}

/* A corrupted frame is fatal for the guest.  The monitor recovers the VM
 * after the spin, so report enough to identify it from the console.
 */

static void ntx_ctx_fail(void)
{
  static const char msg[] = "\r\nNTX: bad RVM context frame\r\n";
  size_t i;

  for (i = 0; i < sizeof(msg) - 1; i++)
    {
      up_putc(msg[i]);
    }

  for (; ; )
    {
    }
}

/* Check that a frame is inside the thread's stack and can be resumed. */

static bool ntx_frame_ok(struct tcb_s *tcb, const uint32_t *frame)
{
  uint32_t base = (uint32_t)(uintptr_t)tcb->stack_base_ptr;
  uint32_t top = base + tcb->adj_stack_size;
  uint32_t exc;
  uint32_t hw;
  uint32_t pc;

  if (frame == NULL || (uint32_t)(uintptr_t)frame < base)
    {
      return false;
    }

  exc = frame[13];
  if (!ntx_exc_valid(exc))
    {
      return false;
    }

  if ((uint32_t)(uintptr_t)(frame + ntx_frame_words(exc)) > top)
    {
      return false;
    }

  hw = ntx_hw_word(exc);
  pc = frame[hw + 6];
  if ((uintptr_t)pc < (uintptr_t)__RVM_Code_Start ||
      (uintptr_t)pc >= (uintptr_t)__RVM_Code_End)
    {
      return false;
    }

  return (frame[hw + 7] & NTX_XPSR_THUMB) != 0;
}

#ifdef CONFIG_ARMV7M_RVM_FAST_PATH

/* The direct switch may only run when the current EPSR carries no ICI/IT
 * continuation, because the manual unstack cannot restore them.
 */

static bool ntx_xpsr_clean(void)
{
  uint32_t xpsr;

  __asm__ volatile ("mrs %0, xpsr" : "=r" (xpsr));
  return (xpsr & NTX_XPSR_ICI_IT_MASK) == 0;
}

static bool ntx_heir_fast(struct tcb_s *tcb)
{
  uint32_t *frame = (uint32_t *)(uintptr_t)tcb->xcp.rvm_sp;
  uint32_t exc;
  uint32_t hw;

  if (tcb->xcp.rvm_sw == 0)
    {
      return false;
    }

  if (!ntx_frame_ok(tcb, frame))
    {
      return false;
    }

  exc = frame[13];
  hw = ntx_hw_word(exc);
  return (frame[hw + 7] & NTX_XPSR_ICI_IT_MASK) == 0;
}

#endif

/* Copy the interrupted user context described by RVM_REG into a frame that
 * starts below the monitor's hardware frame.  The hardware frame and the FP
 * area stay where the monitor put them, on the thread's stack.
 */

static bool ntx_capture_vector(struct tcb_s *tcb)
{
  uint32_t exc = RVM_REG->Reg.LR;
  uint32_t hwaddr = (uint32_t)(uintptr_t)RVM_REG->Reg.SP;
  uint32_t base = (uint32_t)(uintptr_t)tcb->stack_base_ptr;
  uint32_t top = base + tcb->adj_stack_size;
  uint32_t reserve;
  uint32_t words;
  uint32_t *hw;
  uint32_t *frame;
  bool extended;

  if (!ntx_exc_valid(exc))
    {
      return false;
    }

  extended = ntx_is_extended(exc);
  words = ntx_frame_words(exc);
  reserve = words * sizeof(uint32_t);

  /* The frame base lies below the hardware frame.  Reject a hardware frame
   * that could not possibly belong to this thread before touching memory.
   */

  if (hwaddr < base + reserve || hwaddr > top)
    {
      return false;
    }

  hw = (uint32_t *)(uintptr_t)hwaddr;
  frame = hw - ntx_hw_word(exc);

  frame[0] = RVM_STATE->Usr.Number;
  frame[1] = RVM_STATE->Usr.Param[0];
  frame[2] = RVM_STATE->Usr.Param[1];
  frame[3] = RVM_STATE->Usr.Param[2];
  frame[4] = RVM_STATE->Usr.Param[3];
  frame[5] = RVM_REG->Reg.R4;
  frame[6] = RVM_REG->Reg.R5;
  frame[7] = RVM_REG->Reg.R6;
  frame[8] = RVM_REG->Reg.R7;
  frame[9] = RVM_REG->Reg.R8;
  frame[10] = RVM_REG->Reg.R9;
  frame[11] = RVM_REG->Reg.R10;
  frame[12] = RVM_REG->Reg.R11;
  frame[13] = exc;

  if (extended)
    {
#if NTX_HAS_COP
      volatile struct RVM_A7M_Cop_Struct *cop =
        (volatile struct RVM_A7M_Cop_Struct *)(uintptr_t)RVM_REG->Cop;

      frame[14] = cop->S16;
      frame[15] = cop->S17;
      frame[16] = cop->S18;
      frame[17] = cop->S19;
      frame[18] = cop->S20;
      frame[19] = cop->S21;
      frame[20] = cop->S22;
      frame[21] = cop->S23;
      frame[22] = cop->S24;
      frame[23] = cop->S25;
      frame[24] = cop->S26;
      frame[25] = cop->S27;
      frame[26] = cop->S28;
      frame[27] = cop->S29;
      frame[28] = cop->S30;
      frame[29] = cop->S31;
#endif
    }

  tcb->xcp.rvm_sp = (uint32_t)(uintptr_t)frame;
  tcb->xcp.rvm_nest = g_ntx_irqnest;
  tcb->xcp.rvm_sw = 0;

  return ntx_frame_ok(tcb, frame);
}

/* Copy the monitor's current view into a local frame.  Used to build the
 * native register array passed to irq_dispatch().
 */

static void ntx_peek_vector(uint32_t *frame)
{
  uint32_t exc = RVM_REG->Reg.LR;
  uint32_t *hw = (uint32_t *)(uintptr_t)RVM_REG->Reg.SP;
  uint32_t hwoff = ntx_hw_word(exc);

  frame[0] = RVM_STATE->Usr.Number;
  frame[1] = RVM_STATE->Usr.Param[0];
  frame[2] = RVM_STATE->Usr.Param[1];
  frame[3] = RVM_STATE->Usr.Param[2];
  frame[4] = RVM_STATE->Usr.Param[3];
  frame[5] = RVM_REG->Reg.R4;
  frame[6] = RVM_REG->Reg.R5;
  frame[7] = RVM_REG->Reg.R6;
  frame[8] = RVM_REG->Reg.R7;
  frame[9] = RVM_REG->Reg.R8;
  frame[10] = RVM_REG->Reg.R9;
  frame[11] = RVM_REG->Reg.R10;
  frame[12] = RVM_REG->Reg.R11;
  frame[13] = exc;

  memcpy(&frame[hwoff], hw, NTX_RVM_HW * sizeof(uint32_t));

  if (ntx_is_extended(exc))
    {
#if NTX_HAS_COP
      volatile struct RVM_A7M_Cop_Struct *cop =
        (volatile struct RVM_A7M_Cop_Struct *)(uintptr_t)RVM_REG->Cop;

      frame[14] = cop->S16;
      frame[15] = cop->S17;
      frame[16] = cop->S18;
      frame[17] = cop->S19;
      frame[18] = cop->S20;
      frame[19] = cop->S21;
      frame[20] = cop->S22;
      frame[21] = cop->S23;
      frame[22] = cop->S24;
      frame[23] = cop->S25;
      frame[24] = cop->S26;
      frame[25] = cop->S27;
      frame[26] = cop->S28;
      frame[27] = cop->S29;
      frame[28] = cop->S30;
      frame[29] = cop->S31;
#endif
      memcpy(&frame[NTX_RVM_EXT_HW + NTX_RVM_HW],
             &hw[NTX_RVM_HW], NTX_RVM_FP_AREA * sizeof(uint32_t));
    }
}

static void ntx_frame_to_native(const uint32_t *frame, uint32_t *native)
{
  uint32_t exc = frame[13];
  bool extended = ntx_is_extended(exc);
  uint32_t hwoff = ntx_hw_word(exc);
  const uint32_t *hw = &frame[hwoff];

  memset(native, 0, XCPTCONTEXT_SIZE);

  native[REG_SP] = (uint32_t)(uintptr_t)(frame + ntx_frame_words(exc));
  native[REG_BASEPRI] = g_ntx_irqnest;
  native[REG_R4] = frame[5];
  native[REG_R5] = frame[6];
  native[REG_R6] = frame[7];
  native[REG_R7] = frame[8];
  native[REG_R8] = frame[9];
  native[REG_R9] = frame[10];
  native[REG_R10] = frame[11];
  native[REG_R11] = frame[12];
  native[REG_CONTROL] = 0;
  native[REG_EXC_RETURN] = exc;
  memcpy(&native[REG_R0], hw, NTX_RVM_HW * sizeof(uint32_t));

#if NTX_HAS_COP && defined(CONFIG_ARCH_FPU)
  if (extended)
    {
      memcpy(&native[REG_S16], &frame[14],
             NTX_RVM_FP16 * sizeof(uint32_t));
      memcpy(&native[REG_S0], &frame[hwoff + NTX_RVM_HW],
             NTX_RVM_FP_AREA * sizeof(uint32_t));
    }
#endif
}

/* Load the heir's frame into RVM_REG.  The monitor resumes it with a real
 * exception return, which restores the FP area and honours the XPSR
 * alignment pad bit exactly like hardware.
 */

static void ntx_frame_load(struct tcb_s *tcb)
{
  uint32_t *frame = (uint32_t *)(uintptr_t)tcb->xcp.rvm_sp;
  uint32_t exc;
  uint32_t hw;
  bool extended;

  if (!ntx_frame_ok(tcb, frame))
    {
      ntx_ctx_fail();
    }

  exc = frame[13];
  extended = ntx_is_extended(exc);
  hw = ntx_hw_word(exc);

  RVM_STATE->Usr.Number = frame[0];
  RVM_STATE->Usr.Param[0] = frame[1];
  RVM_STATE->Usr.Param[1] = frame[2];
  RVM_STATE->Usr.Param[2] = frame[3];
  RVM_STATE->Usr.Param[3] = frame[4];
  RVM_REG->Reg.R4 = frame[5];
  RVM_REG->Reg.R5 = frame[6];
  RVM_REG->Reg.R6 = frame[7];
  RVM_REG->Reg.R7 = frame[8];
  RVM_REG->Reg.R8 = frame[9];
  RVM_REG->Reg.R9 = frame[10];
  RVM_REG->Reg.R10 = frame[11];
  RVM_REG->Reg.R11 = frame[12];
  RVM_REG->Reg.LR = exc;

  if (extended)
    {
#if NTX_HAS_COP
      volatile struct RVM_A7M_Cop_Struct *cop =
        (volatile struct RVM_A7M_Cop_Struct *)(uintptr_t)RVM_REG->Cop;

      cop->S16 = frame[14];
      cop->S17 = frame[15];
      cop->S18 = frame[16];
      cop->S19 = frame[17];
      cop->S20 = frame[18];
      cop->S21 = frame[19];
      cop->S22 = frame[20];
      cop->S23 = frame[21];
      cop->S24 = frame[22];
      cop->S25 = frame[23];
      cop->S26 = frame[24];
      cop->S27 = frame[25];
      cop->S28 = frame[26];
      cop->S29 = frame[27];
      cop->S30 = frame[28];
      cop->S31 = frame[29];
#endif
    }

  RVM_REG->Reg.SP = (rvm_ptr_t)(uintptr_t)&frame[hw];
}

/****************************************************************************
 * Public Functions
 ****************************************************************************/

static void ntx_vector_switch(struct tcb_s *from, struct tcb_s *head,
                              struct tcb_s *to)
{
  if (from == NULL)
    {
      from = head;
    }

  if (g_ntx_pending)
    {
      /* A task level switch is in flight.  Its frame is the one that must
       * be resumed; only the final heir can still change.
       */

      if (to != head)
        {
          nxsched_switch_context(head, to);
          g_ntx_pending_to = to;
        }

      return;
    }

  if (to == from)
    {
      return;
    }

  if (!ntx_capture_vector(from))
    {
      ntx_ctx_fail();
    }

  nxsched_switch_context(head, to);

  g_ntx_pending_to = to;
  g_ntx_pending = true;
  RVM_Virt_Yield();
}

void ntx_timer_handler(void)
{
  struct tcb_s *from = g_ntx_running;
  struct tcb_s *head = this_task();

  nxsched_process_timer();
  ntx_vector_switch(from, head, this_task());
}

void ntx_irq_entry(int irq)
{
  struct tcb_s *from = g_ntx_running;
  struct tcb_s *head = this_task();
  uint32_t frame[NTX_RVM_FRAME_MAX];

  if (!ntx_exc_valid(RVM_REG->Reg.LR))
    {
      ntx_ctx_fail();
    }

  ntx_peek_vector(frame);
  ntx_frame_to_native(frame, g_ntx_irq_regs);

  irq_dispatch(irq, g_ntx_irq_regs);
  ntx_vector_switch(from, head, this_task());
}

void ntx_fullcontextrestore(void)
{
  struct tcb_s *to = this_task();

  /* up_exit() has already removed the current task and released its TCB.
   * There is no context to save: load the new head of the ready-to-run
   * list and let the monitor resume it.
   */

  if (to == NULL)
    {
      ntx_ctx_fail();
    }

  g_ntx_pending_to = to;
  g_ntx_pending = true;
  g_ntx_irqnest = 0;
  RVM_Virt_Int_Unmask();
  RVM_Virt_Yield();

  for (; ; )
    {
    }
}

void ntx_ctx_handler(void)
{
  struct tcb_s *to = g_ntx_pending_to;

  g_ntx_pending = false;
  g_ntx_pending_to = NULL;

  if (to == NULL)
    {
      return;
    }

  ntx_frame_load(to);

  g_running_tasks[0] = to;
  g_ntx_running = to;
  g_ntx_irqnest = to->xcp.rvm_nest;

  if (g_ntx_irqnest == 0)
    {
      RVM_Virt_Int_Unmask();
    }
  else
    {
      RVM_Virt_Int_Mask();
    }
}

void ntx_switch_context(struct tcb_s *tcb, struct tcb_s *rtcb)
{
  if (tcb == rtcb)
    {
      return;
    }

  if (ntx_in_interrupt())
    {
      /* The vector entry point compares the head of the ready-to-run list
       * with the interrupted task and performs the deferred switch.
       */

      return;
    }

  rtcb->xcp.rvm_nest = g_ntx_irqnest;
  rtcb->xcp.rvm_sw = 1;
  nxsched_switch_context(rtcb, tcb);

#ifdef CONFIG_ARMV7M_RVM_FAST_PATH
  /* Both sides must be free of EPSR ICI/IT continuations.  A frame captured
   * on the vector thread is never resumed directly.
   */

  if (ntx_xpsr_clean() && ntx_heir_fast(tcb))
    {
      g_running_tasks[0] = tcb;
      g_ntx_running = tcb;
      g_ntx_irqnest = tcb->xcp.rvm_nest;
      ntx_fast_switch(&rtcb->xcp, &tcb->xcp, (const void *)RVM_PARAM);

      /* Resumed by another switch; the globals describe this thread again. */

      return;
    }
#endif

  g_ntx_pending_to = tcb;
  g_ntx_pending = true;

  ntx_context_switch(&rtcb->xcp, (const void *)RVM_PARAM);

  /* Resumed here by ntx_ctx_exit with the scheduler state of this thread.
   * The context handler has already restored g_ntx_irqnest.
   */
}

bool ntx_in_interrupt(void)
{
  return RVM_STATE->Vct_Act != 0U;
}

irqstate_t ntx_irq_save(void)
{
  irqstate_t flags;

  if (ntx_in_interrupt())
    {
      return g_ntx_vctnest++;
    }

  flags = g_ntx_irqnest;
  if (g_ntx_irqnest == 0)
    {
      RVM_Virt_Int_Mask();
    }

  g_ntx_irqnest++;
  return flags;
}

void ntx_irq_restore(irqstate_t flags)
{
  if (ntx_in_interrupt())
    {
      g_ntx_vctnest = flags;
      return;
    }

  g_ntx_irqnest = flags;
  if (flags == 0)
    {
      RVM_Virt_Int_Unmask();
    }
  else
    {
      RVM_Virt_Int_Mask();
    }
}

void ntx_irq_disable(void)
{
  if (ntx_in_interrupt())
    {
      g_ntx_vctnest++;
      return;
    }

  if (g_ntx_irqnest == 0)
    {
      RVM_Virt_Int_Mask();
    }

  g_ntx_irqnest++;
}

void ntx_irq_enable(void)
{
  if (ntx_in_interrupt())
    {
      if (g_ntx_vctnest != 0)
        {
          g_ntx_vctnest--;
        }

      return;
    }

  if (g_ntx_irqnest != 0)
    {
      g_ntx_irqnest--;
      if (g_ntx_irqnest == 0)
        {
          RVM_Virt_Int_Unmask();
        }
    }
}

void ntx_vector_init(void)
{
  /* The boot context becomes the IDLE task.  Its logical nest level must be
   * zero, otherwise the level that protects this initialization would be
   * saved with the idle context and would keep the RVM mask closed forever.
   */

  g_ntx_irqnest = 0;
  g_ntx_running = this_task();
  RVM_Virt_Int_Mask();
  RVM_Virt_Tim_Reg(ntx_timer_handler);
  RVM_Virt_Ctx_Reg(ntx_ctx_handler);
  RVM_Hyp_Int_Ena();
  RVM_Virt_Int_Unmask();
}
