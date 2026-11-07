/* SPDX-License-Identifier: Unlicense */

/**
 * @file
 *
 * @ingroup RTEMSScoreCPUARM
 *
 * @brief RTE RVM context switch implementation.
 *
 * The guest is unprivileged: it cannot enter PendSV and it cannot program
 * the exception return itself.  The thread level switch therefore builds
 * the same masqueraded exception frame as the uC/OS-II RVM port, pends the
 * switch and lets the RVM vector thread rebuild RVM_REG from the heir's
 * frame.  The real exception return performed by the monitor is also the
 * only way to restore EPSR ICI/IT state.
 *
 * Frame layout, from the stack pointer stored in Context_Control::register_sp
 * upwards:
 *
 *   Number, Param[0..3]                 5 words
 *   R4..R11, EXC_RETURN                 9 words
 *   S16..S31                           16 words, extended frame only
 *   R0..R3, R12, LR, PC, xPSR           8 words, basic hardware frame
 *   S0..S15, FPSCR, [pad]              18 words, extended frame only
 *
 * The basic frame is 22 words, the extended frame is 56 words.  Frames are
 * laid down 8 byte aligned; the alignment pad is the 18th word of the
 * extended hardware area.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include <rtems/score/cpu.h>
#include <rtems/score/percpu.h>
#include <rtems/score/tls.h>

#include "rvm.h"

#include "rte_config.h"
#include "rte_rvm_context.h"

#define RTE_FRAME_PARAMS 5U
#define RTE_FRAME_GP 9U
#define RTE_FRAME_FP16 16U
#define RTE_FRAME_BASIC 8U
#define RTE_FRAME_FP_AREA 18U

#define RTE_FRAME_BASIC_WORDS \
  ( RTE_FRAME_PARAMS + RTE_FRAME_GP + RTE_FRAME_BASIC )
#define RTE_FRAME_EXT_WORDS \
  ( RTE_FRAME_PARAMS + RTE_FRAME_GP + RTE_FRAME_FP16 + RTE_FRAME_BASIC + \
    RTE_FRAME_FP_AREA )

#define RTE_FRAME_BASIC_HW ( RTE_FRAME_PARAMS + RTE_FRAME_GP )
#define RTE_FRAME_EXT_HW \
  ( RTE_FRAME_PARAMS + RTE_FRAME_GP + RTE_FRAME_FP16 )

#define RTE_EXC_RETURN_BASIC 0xFFFFFFFDU
#define RTE_EXC_RETURN_EXT 0xFFFFFFEDU

_Static_assert(
  RTE_FRAME_BASIC_WORDS == 22,
  "basic RVM frame size"
);
_Static_assert(
  RTE_FRAME_EXT_WORDS == 56,
  "extended RVM frame size"
);
_Static_assert(
  offsetof( Context_Control, register_sp ) == 36,
  "Context_Control::register_sp offset used by the assembly switch"
);
_Static_assert(
  offsetof( Context_Control, isr_nest_level ) == 40,
  "Context_Control::isr_nest_level offset"
);

Context_Control *rte_pending_heir;

static bool rte_isr_captured;

extern void _RVM_CPU_Context_switch_yield(
  Context_Control *executing,
  Context_Control *heir,
  const void *usr_params
);

extern void _RVM_CPU_Context_switch_swt(
  Context_Control *executing,
  Context_Control *heir,
  const void *usr_params
);

#define RTE_XPSR_ICI_IT_MASK 0x0600FC00U

#if defined(RTE_FAST_PATH) && (RTE_FAST_PATH != 0)
#define RTE_USE_FAST_PATH 1
#else
#define RTE_USE_FAST_PATH 0
#endif

static bool rte_in_vector_stack( void )
{
  uintptr_t sp;

  __asm__ volatile ( "mov %0, sp" : "=r" ( sp ) );

  return sp >= RTE_VECTOR_STACK_BASE
    && sp < RTE_VECTOR_STACK_BASE + RTE_VECTOR_STACK_SIZE;
}

static bool rte_exc_return_is_valid( uint32_t exc )
{
  /*
   * 0xFFFFFFFD (basic) and 0xFFFFFFED (extended) have bits 0..4 as the
   * variable part and all other bits set.
   */
  return ( exc | 0x1EU ) == 0xFFFFFFFFU;
}

static bool rte_frame_is_executable( const uint32_t *sp )
{
  uint32_t exc;
  uint32_t hw;
  uint32_t pc;
  uint32_t xpsr;

  if ( (uintptr_t) sp < DATA_DATA1_BASE
    || (uintptr_t) sp >= RVM_VIRT_REG_BASE ) {
    return false;
  }

  exc = sp[13];

  if ( !rte_exc_return_is_valid( exc ) ) {
    return false;
  }

  hw = ( exc & 0x10U ) == 0U ? RTE_FRAME_EXT_HW : RTE_FRAME_BASIC_HW;
  pc = sp[hw + 6U];
  xpsr = sp[hw + 7U];

  return pc >= CODE_CODE1_BASE
    && pc < CODE_CODE1_BASE + CODE_CODE1_SIZE
    && ( xpsr & 0x01000000U ) != 0U;
}

#if RTE_USE_FAST_PATH

static bool rte_frame_is_dirty( const Context_Control *heir )
{
  const uint32_t *sp;
  uint32_t exc;
  uint32_t off;

  sp = (const uint32_t *) heir->register_sp;

  if ( !rte_frame_is_executable( sp ) ) {
    return true;
  }

  exc = sp[13];
  off = ( exc & 0x10U ) == 0U ? 37U : 21U;

  return ( sp[off] & RTE_XPSR_ICI_IT_MASK ) != 0U;
}

static bool rte_xpsr_is_dirty( void )
{
  uint32_t xpsr;

  __asm__ volatile ( "mrs %0, xpsr" : "=r" ( xpsr ) );

  return ( xpsr & RTE_XPSR_ICI_IT_MASK ) != 0U;
}

#endif /* RTE_USE_FAST_PATH */

static void rte_rvm_ctx_fail( void )
{
  static const char msg[] = "\r\nRTE ctx bad frame\r\n";
  size_t i;

  for ( i = 0; i < sizeof( msg ) - 1U; ++i ) {
    RVM_Hyp_Putchar( msg[i] );
  }

  for (;;) {
    /* The serial marker identifies the corrupt frame. */
  }
}

void rte_rvm_capture_executing( Context_Control *executing )
{
  uint32_t *sp;
  uint32_t exc;

  sp = (uint32_t *) RVM_REG->Reg.SP;
  exc = RVM_REG->Reg.LR;

  if ( (uintptr_t) sp < DATA_DATA1_BASE
    || (uintptr_t) sp >= RVM_VIRT_REG_BASE
    || !rte_exc_return_is_valid( exc ) ) {
    return;
  }

#if RVM_COP_NUM != 0U
  if ( ( exc & 0x10U ) == 0U ) {
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

  executing->register_sp = sp;
  executing->register_lr = (void *) 0;
}

void rte_rvm_ctx_handler( void )
{
  Context_Control *heir;
  uint32_t *sp;
  uint32_t exc;

  heir = rte_pending_heir;

  if ( heir == NULL ) {
    return;
  }

  rte_pending_heir = NULL;

  sp = (uint32_t *) heir->register_sp;

  if ( !rte_frame_is_executable( sp ) ) {
    rte_rvm_ctx_fail();
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
  if ( ( exc & 0x10U ) == 0U ) {
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
#endif

  RVM_REG->Reg.SP = (rvm_ptr_t) sp;

  _Per_CPU_Get()->thread_dispatch_disable_level =
    ( (uintptr_t) heir->register_lr != 0U ) ? 1U : 0U;

  _RVM_CPU_ISR_Set_level( heir->isr_nest_level );

  rte_isr_captured = false;
}

void rte_rvm_dispatch_complete( void )
{
  rte_isr_captured = false;

  if ( rte_pending_heir != NULL ) {
    /*
     * Called from the vector thread: this only raises the RVM context flag,
     * RVM_Vct_Loop() invokes the context handler after this ISR returns.
     */
    RVM_Virt_Yield();
  }
}

void _CPU_Context_Initialize(
  Context_Control *context,
  void *stack_area_begin,
  size_t stack_area_size,
  uint32_t new_level,
  void (*entry_point)( void ),
  bool is_fp,
  void *tls_area
)
{
  uintptr_t end;
  uint32_t words;
  uint32_t hw;
  uint32_t exc;
  uint32_t *sp;

  end = ( (uintptr_t) stack_area_begin + stack_area_size ) & ~(uintptr_t) 7U;

  if ( is_fp ) {
    words = RTE_FRAME_EXT_WORDS;
    hw = RTE_FRAME_EXT_HW;
    exc = RTE_EXC_RETURN_EXT;
  } else {
    words = RTE_FRAME_BASIC_WORDS;
    hw = RTE_FRAME_BASIC_HW;
    exc = RTE_EXC_RETURN_BASIC;
  }

  sp = (uint32_t *) ( end - (uintptr_t) words * 4U );
  memset( sp, 0, (size_t) words * 4U );

  sp[13] = exc;
  sp[hw + 5U] = 0U;
  sp[hw + 6U] = (uint32_t) entry_point;
  sp[hw + 7U] = 0x01000000U;

  memset( context, 0, sizeof( *context ) );
  context->register_sp = sp;
  context->register_lr = (void *) 1;
  context->isr_nest_level = new_level;

  if ( tls_area != NULL ) {
    context->thread_id =
      (uint32_t) (uintptr_t) _TLS_Initialize_area( tls_area );
  }
}

void _CPU_Context_switch(
  Context_Control *executing,
  Context_Control *heir
)
{
  if ( rte_in_vector_stack() ) {
    /*
     * The vector thread is running the RTEMS dispatcher for an interrupt.
     * RVM_REG already holds the interrupted user context; defer the
     * physical switch to rte_rvm_ctx_handler().
     */
    if ( !rte_isr_captured ) {
      executing->isr_nest_level = _RVM_CPU_ISR_Get_level();
      rte_rvm_capture_executing( executing );
      rte_isr_captured = true;
      rte_pending_heir = heir;
      /*
       * The deferred switch can only resume one context before the ISR
       * returns.  If the scheduler selected another thread in the same
       * dispatch loop, leave it ready for the next thread level dispatch;
       * otherwise it would be marked executing but never actually run.
       */
      _Per_CPU_Get()->dispatch_necessary = 0;
      return;
    }

    /*
     * A second logical switch in the same ISR: the first heir is the one
     * which will physically run.  The remaining ready thread keeps its
     * scheduler state and is selected by the next dispatch.
     */
    return;
  }

  executing->isr_nest_level = _RVM_CPU_ISR_Get_level();
  executing->register_lr = (void *) 1;
  _RVM_CPU_ISR_Set_level( 1U );

#if RTE_USE_FAST_PATH
  /*
   * The heir can be resumed directly only when neither side carries
   * EPSR ICI/IT continuations and the heir is a fast/initial frame.  Dirty
   * frames still need the monitor's exception return through the Vct.
   */
  if ( heir != executing
    && rte_pending_heir == NULL
    && heir->register_lr != NULL
    && !rte_xpsr_is_dirty()
    && !rte_frame_is_dirty( heir ) ) {
    _RVM_CPU_Context_switch_swt(
      executing,
      heir,
      (const void *) &RVM_STATE->Usr
    );
    return;
  }
#endif

  /*
   * Builds the masqueraded frame on this stack, records the heir and
   * triggers the vector thread.  It returns only when this thread is
   * resumed by the context handler.
   */
  _RVM_CPU_Context_switch_yield(
    executing,
    heir,
    (const void *) &RVM_STATE->Usr
  );
}

RTEMS_NO_RETURN void _CPU_Context_restore(
  Context_Control *heir
)
{
  RVM_Hyp_Int_Ena();
  rte_pending_heir = heir;
  RVM_Virt_Yield();
  RVM_Virt_Int_Unmask();

  for (;;) {
    /* Resumed only through the context handler. */
  }
}

RTEMS_NO_RETURN void _ARMV7M_Start_multitasking(
  Context_Control *heir
)
{
  RVM_Hyp_Int_Ena();
  rte_pending_heir = heir;
  RVM_Virt_Yield();
  RVM_Virt_Int_Unmask();

  for (;;) {
    /* The boot context is abandoned; the heir starts through the handler. */
  }
}
