/*
    CHB port for the RVM hypervisor.
    Copyright (C) 2026 Evo-Devo Instrum. All rights reserved.

    This file is NOT part of the ChibiOS distribution.  It belongs to the
    ChibiOS-on-RVM guest port and is released under the Unlicense; refer to
    the LICENSE file for details.  The ChibiOS kernel itself is copyright
    Giovanni Di Sirio and is not modified by this file.
*/

/**
 * @file    ARMv7-M-RVM/chcore.c
 * @brief   ChibiOS ARMv7-M port for the RVM hypervisor.
 *
 * @addtogroup ARMV7M_RVM_CORE
 * @{
 */

#include <stdbool.h>
#include <stdint.h>

#include "ch.h"
#include "rvm.h"
#include "chb_abi.h"

/*===========================================================================*/
/* Module local variables.                                                   */
/*===========================================================================*/

static volatile uint32_t chb_nest;
static thread_t *chb_pending_ntp;
static thread_t *chb_pending_otp;
static thread_t *chb_fast_otp;
static thread_t *chb_fast_ntp;

/*===========================================================================*/
/* Module local functions.                                                   */
/*===========================================================================*/

static void chb_tick_handler(void);
static void chb_ctx_handler(void);

#if RVM_COP_NUM == 0
static void chb_ctx_fail(void) {
  static const char msg[] = "\r\nCHB ctx bad frame\r\n";
  uint32_t i;

  for (i = 0U; i < sizeof(msg) - 1U; i++) {
    RVM_Hyp_Putchar(msg[i]);
  }
  for (;;) {
  }
}
#endif

#if CHB_FAST_PATH
static bool chb_xpsr_is_dirty(void) {
  uint32_t xpsr;

  xpsr = chb_get_xpsr();
  return (xpsr & CHB_XPSR_ICI_IT_MASK) != 0U;
}

static bool chb_frame_is_clean(thread_t *tp) {
  uint32_t *sp;
  uint32_t exc;
  uint32_t off;

  if ((tp == NULL) || (tp->ctx.sp == NULL)) {
    return false;
  }

  sp = (uint32_t *)(void *)tp->ctx.sp;
  exc = sp[13];
  if ((exc != CHB_EXC_RETURN_BASIC) && (exc != CHB_EXC_RETURN_EXT)) {
    return false;
  }

  off = ((exc & 0x10U) == 0U) ? 37U : 21U;
  if ((sp[off] & CHB_XPSR_ICI_IT_MASK) != 0U) {
    return false;
  }
  if ((sp[off] & CHB_XPSR_THUMB) == 0U) {
    return false;
  }

#if RVM_COP_NUM == 0
  if ((exc & 0x10U) == 0U) {
    return false;
  }
#endif

  return true;
}
#endif

static void chb_ctx_switch(thread_t *ntp, thread_t *otp) {
  uint32_t *sp;
  uint32_t *base;
  uint32_t exc;

  /* The monitor stacked a hardware exception frame at RVM_REG->Reg.SP.
   * For an extended frame the monitor has already placed S0-S15/FPSCR
   * above the hardware frame; save the S16-S31 block below it first. */
  sp = (uint32_t *)(void *)RVM_REG->Reg.SP;
  exc = RVM_REG->Reg.LR;

#if RVM_COP_NUM != 0U
  if ((exc & 0x10U) == 0U) {
    volatile struct RVM_A7M_Cop_Struct *cop =
      (volatile struct RVM_A7M_Cop_Struct *) RVM_REG->Cop;

    sp -= CHB_FRAME_FP16;
    sp[0] = cop->S16;
    sp[1] = cop->S17;
    sp[2] = cop->S18;
    sp[3] = cop->S19;
    sp[4] = cop->S20;
    sp[5] = cop->S21;
    sp[6] = cop->S22;
    sp[7] = cop->S23;
    sp[8] = cop->S24;
    sp[9] = cop->S25;
    sp[10] = cop->S26;
    sp[11] = cop->S27;
    sp[12] = cop->S28;
    sp[13] = cop->S29;
    sp[14] = cop->S30;
    sp[15] = cop->S31;
  }
#else
  if ((exc & 0x10U) == 0U) {
    chb_ctx_fail();
  }
#endif

  sp -= CHB_FRAME_PARAMS + CHB_FRAME_GP;
  base = sp;

  base[0] = RVM_STATE->Usr.Number;
  base[1] = RVM_STATE->Usr.Param[0];
  base[2] = RVM_STATE->Usr.Param[1];
  base[3] = RVM_STATE->Usr.Param[2];
  base[4] = RVM_STATE->Usr.Param[3];
  base[5] = RVM_REG->Reg.R4;
  base[6] = RVM_REG->Reg.R5;
  base[7] = RVM_REG->Reg.R6;
  base[8] = RVM_REG->Reg.R7;
  base[9] = RVM_REG->Reg.R8;
  base[10] = RVM_REG->Reg.R9;
  base[11] = RVM_REG->Reg.R10;
  base[12] = RVM_REG->Reg.R11;
  base[13] = exc;

  if (otp != NULL) {
    otp->ctx.sp = (struct port_intctx *)(void *)base;
  }

  base = (uint32_t *)(void *)ntp->ctx.sp;
  exc = base[13];
  RVM_STATE->Usr.Number = base[0];
  RVM_STATE->Usr.Param[0] = base[1];
  RVM_STATE->Usr.Param[1] = base[2];
  RVM_STATE->Usr.Param[2] = base[3];
  RVM_STATE->Usr.Param[3] = base[4];
  RVM_REG->Reg.R4 = base[5];
  RVM_REG->Reg.R5 = base[6];
  RVM_REG->Reg.R6 = base[7];
  RVM_REG->Reg.R7 = base[8];
  RVM_REG->Reg.R8 = base[9];
  RVM_REG->Reg.R9 = base[10];
  RVM_REG->Reg.R10 = base[11];
  RVM_REG->Reg.R11 = base[12];
  RVM_REG->Reg.LR = exc;

#if RVM_COP_NUM != 0U
  if ((exc & 0x10U) == 0U) {
    volatile struct RVM_A7M_Cop_Struct *cop =
      (volatile struct RVM_A7M_Cop_Struct *) RVM_REG->Cop;

    cop->S16 = base[14];
    cop->S17 = base[15];
    cop->S18 = base[16];
    cop->S19 = base[17];
    cop->S20 = base[18];
    cop->S21 = base[19];
    cop->S22 = base[20];
    cop->S23 = base[21];
    cop->S24 = base[22];
    cop->S25 = base[23];
    cop->S26 = base[24];
    cop->S27 = base[25];
    cop->S28 = base[26];
    cop->S29 = base[27];
    cop->S30 = base[28];
    cop->S31 = base[29];
    RVM_REG->Reg.SP = (rvm_ptr_t)(base + CHB_FRAME_EXT_HW);
  }
  else
#else
  if ((exc & 0x10U) == 0U) {
    chb_ctx_fail();
  }
#endif
  {
    RVM_REG->Reg.SP = (rvm_ptr_t)(base + CHB_FRAME_PARAMS + CHB_FRAME_GP);
  }
}

static void chb_tick_handler(void) {
  chb_lock_from_isr();
  chSysTimerHandlerI();
  if (chSchIsPreemptionRequired()) {
    chSchDoPreemption();
  }
  chb_unlock_from_isr();
}

static void chb_ctx_handler(void) {
  thread_t *ntp;
  thread_t *otp;

  ntp = chb_pending_ntp;
  otp = chb_pending_otp;
  chb_pending_ntp = NULL;
  chb_pending_otp = NULL;

  if (ntp != NULL) {
    chb_ctx_switch(ntp, otp);
  }
}

/*===========================================================================*/
/* Module exported functions.                                                */
/*===========================================================================*/

void chb_fast_save(uint32_t *sp) {
  chb_fast_otp->ctx.sp = (struct port_intctx *)(void *)sp;
}

uint32_t *chb_fast_load(void) {
  return (uint32_t *)(void *)chb_fast_ntp->ctx.sp;
}

void chb_lock(void) {
  if (chb_nest == 0U) {
    RVM_Virt_Int_Mask();
  }
  chb_nest++;
}

void chb_unlock(void) {
  if (chb_nest != 0U) {
    chb_nest--;
    if (chb_nest == 0U) {
      RVM_Virt_Int_Unmask();
    }
  }
}

void chb_lock_from_isr(void) {
  chb_lock();
}

void chb_unlock_from_isr(void) {
  chb_unlock();
}

void chb_disable(void) {
  RVM_Hyp_Int_Dis();
}

void chb_suspend(void) {
  RVM_Hyp_Int_Dis();
}

void chb_enable(void) {
  RVM_Hyp_Int_Ena();
}

bool chb_is_isr_context(void) {
  return (RVM_STATE->Vct_Act != 0U);
}

void chb_wait_for_interrupt(void) {
  (void)RVM_Hyp_Vct_Wait();
}

uint32_t chb_lock_status(void) {
  return chb_nest;
}

void chb_irq_epilogue(void) {
  chb_lock_from_isr();
  if (chSchIsPreemptionRequired()) {
    chSchDoPreemption();
  }
  chb_unlock_from_isr();
}

void chb_port_switch(thread_t *ntp, thread_t *otp) {
#if CHB_FAST_PATH
  if ((RVM_STATE->Vct_Act == 0U) && (ntp != otp)
      && chb_frame_is_clean(ntp) && !chb_xpsr_is_dirty()) {
    chb_fast_otp = otp;
    chb_fast_ntp = ntp;
    chb_fast_switch();
    return;
  }
#endif

  chb_pending_ntp = ntp;
  chb_pending_otp = otp;

  if (RVM_STATE->Vct_Act == 0U) {
    /* Task-level slow path.  ChibiOS has already marked the outgoing
     * thread as sleeping/ready, so this call must not return until the
     * monitor has actually switched away. */
    RVM_Virt_Yield();
    RVM_Virt_Int_Unmask();
  }
  else {
    /* Timer/interrupt handler on the vector thread.  The physical switch
     * is performed by the context handler after the handler returns. */
    RVM_Virt_Yield();
  }
}

void chb_thread_start(void (*pf)(void *), void *arg) {
  chb_nest = 0U;
  RVM_Virt_Int_Unmask();
  pf(arg);
  chThdExit(MSG_OK);
}

void port_init(os_instance_t *oip) {
  (void)oip;

  chb_nest = 1U;
  chb_pending_ntp = NULL;
  chb_pending_otp = NULL;
  chb_fast_otp = NULL;
  chb_fast_ntp = NULL;

  RVM_Virt_Int_Mask();
  RVM_Virt_Tim_Reg(chb_tick_handler);
  RVM_Virt_Ctx_Reg(chb_ctx_handler);
  RVM_Hyp_Int_Ena();
}

/** @} */
