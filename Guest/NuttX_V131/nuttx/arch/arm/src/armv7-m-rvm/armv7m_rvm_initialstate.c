/****************************************************************************
 * arch/arm/src/armv7-m-rvm/armv7m_rvm_initialstate.c
 *
 * SPDX-License-Identifier: Apache-2.0
 ****************************************************************************/

#include <nuttx/config.h>

#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include <nuttx/arch.h>
#include <nuttx/sched.h>

#include "arm_internal.h"
#include "armv7m_rvm_abi.h"
#include "armv7m_rvm_internal.h"

/* The guest is entered on the monitor-provided stack region, and the boot
 * thread becomes the IDLE task.  Declare that region as the idle stack; it
 * is not part of the NuttX heap.
 */

extern char __RVM_Stack_Base[];
extern char __RVM_Stack_End[];

/****************************************************************************
 * Public Functions
 ****************************************************************************/

void up_initial_state(struct tcb_s *tcb)
{
  struct xcptcontext *xcp = &tcb->xcp;
  uint32_t *frame;
  uint32_t *hw;
  uint32_t top;

  memset(xcp, 0, sizeof(*xcp));

  if (tcb->pid == IDLE_PROCESS_ID)
    {
      tcb->stack_alloc_ptr = __RVM_Stack_Base;
      tcb->stack_base_ptr = __RVM_Stack_Base;
      tcb->adj_stack_size = (size_t)(__RVM_Stack_End - __RVM_Stack_Base);
    }

  /* Every thread starts with a basic frame.  There is no FP context before
   * the thread executes its first VFP instruction; that instruction sets
   * CONTROL.FPCA and the first switch then builds the extended frame.
   */

  top = ((uint32_t)tcb->stack_base_ptr + tcb->adj_stack_size) & ~7ul;
  frame = (uint32_t *)(uintptr_t)
          (top - NTX_RVM_BASIC_WORDS * sizeof(uint32_t));

  memset(frame, 0, NTX_RVM_BASIC_WORDS * sizeof(uint32_t));

  frame[13] = NTX_EXC_RETURN_BASIC;

  hw = &frame[NTX_RVM_BASIC_HW];
  hw[6] = (uint32_t)tcb->start | 1u;
  hw[7] = NTX_XPSR_THUMB;

  xcp->rvm_sp = (uint32_t)(uintptr_t)frame;
  xcp->rvm_nest = 0;
  xcp->rvm_sw = 1;
  xcp->regs = NULL;
}
