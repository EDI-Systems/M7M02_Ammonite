/*
    CHB port for the RVM hypervisor.
    Copyright (C) 2026 Evo-Devo Instrum. All rights reserved.

    This file is NOT part of the ChibiOS distribution.  It belongs to the
    ChibiOS-on-RVM guest port and is released under the Unlicense; refer to
    the LICENSE file for details.  The ChibiOS kernel itself is copyright
    Giovanni Di Sirio and is not modified by this file.
*/

/**
 * @file    RV32P-RVM/chcore.c
 * @brief   ChibiOS RV32P port for the RVM hypervisor.
 *
 * @addtogroup RV32P_RVM_CORE
 * @{
 */

#include <stdbool.h>
#include <stdint.h>

#include "ch.h"
#include "rvm.h"

extern uint32_t __RVM_Global;

static volatile uint32_t chb_nest;
static thread_t *chb_pending_ntp;
static thread_t *chb_pending_otp;
#if CHB_FAST_PATH
static thread_t *chb_fast_otp;
static thread_t *chb_fast_ntp;
#endif

static void chb_tick_handler(void);
static void chb_ctx_handler(void);

#if CHB_USE_FPU
static void chb_fpu_save(uint32_t *sp) {
  volatile struct RVM_RV32P_Cop_Struct *cop =
    (volatile struct RVM_RV32P_Cop_Struct *) RVM_REG->Cop;
  uint32_t i;

#if CHB_RV_USE_RVD
  for (i = 0U; i < 32U; i++) {
    sp[2U * i] = cop->F[i][0];
    sp[2U * i + 1U] = cop->F[i][1];
  }
  sp[CHB_RV_FPU_WORDS - 1U] = cop->FCSR;
#else
  for (i = 0U; i < 32U; i++) {
    sp[i] = ((volatile rvm_ptr_t *)(void *)cop)[1U + i];
  }
  sp[CHB_RV_FPU_WORDS - 1U] = cop->FCSR;
#endif
}

static void chb_fpu_load(const uint32_t *sp) {
  volatile struct RVM_RV32P_Cop_Struct *cop =
    (volatile struct RVM_RV32P_Cop_Struct *) RVM_REG->Cop;
  uint32_t i;

#if CHB_RV_USE_RVD
  for (i = 0U; i < 32U; i++) {
    cop->F[i][0] = sp[2U * i];
    cop->F[i][1] = sp[2U * i + 1U];
  }
  cop->FCSR = sp[CHB_RV_FPU_WORDS - 1U];
#else
  for (i = 0U; i < 32U; i++) {
    ((volatile rvm_ptr_t *)(void *)cop)[1U + i] = sp[i];
  }
  cop->FCSR = sp[CHB_RV_FPU_WORDS - 1U];
#endif
}
#endif

#if CHB_FAST_PATH
static bool chb_frame_is_valid(thread_t *tp) {
  uint32_t *sp;
  uint32_t mstatus;
  uint32_t pc;

  if ((tp == NULL) || (tp->ctx.sp == NULL)) {
    return false;
  }

  sp = (uint32_t *)(void *)tp->ctx.sp;
  mstatus = sp[CHB_RV_PARAMS];

  if ((mstatus & CHB_RV_MSTATUS_FS_DIRTY) != 0U) {
#if CHB_USE_FPU
    sp += CHB_RV_FPU_WORDS;
#else
    return false;
#endif
  }
  sp += CHB_RV_PARAMS + CHB_RV_MSTATUS;

  pc = sp[0];
  return (pc != 0U) && ((pc & 1U) == 0U);
}
#endif


static void chb_ctx_switch(thread_t *ntp, thread_t *otp) {
  uint32_t *sp;
  uint32_t mstatus;
  uint32_t *base;

  sp = (uint32_t *)(uintptr_t)RVM_REG->Reg.X2_SP;

  /* Integer frame, from the lowest address: PC, X1, X3..X31. */
  sp -= CHB_RV_INT_WORDS;
  sp[0] = RVM_REG->Reg.PC;
  sp[1] = RVM_REG->Reg.X1_RA;
  sp[2] = RVM_REG->Reg.X3_GP;
  sp[3] = RVM_REG->Reg.X4_TP;
  sp[4] = RVM_REG->Reg.X5_T0;
  sp[5] = RVM_REG->Reg.X6_T1;
  sp[6] = RVM_REG->Reg.X7_T2;
  sp[7] = RVM_REG->Reg.X8_S0_FP;
  sp[8] = RVM_REG->Reg.X9_S1;
  sp[9] = RVM_REG->Reg.X10_A0;
  sp[10] = RVM_REG->Reg.X11_A1;
  sp[11] = RVM_REG->Reg.X12_A2;
  sp[12] = RVM_REG->Reg.X13_A3;
  sp[13] = RVM_REG->Reg.X14_A4;
  sp[14] = RVM_REG->Reg.X15_A5;
  sp[15] = RVM_REG->Reg.X16_A6;
  sp[16] = RVM_REG->Reg.X17_A7;
  sp[17] = RVM_REG->Reg.X18_S2;
  sp[18] = RVM_REG->Reg.X19_S3;
  sp[19] = RVM_REG->Reg.X20_S4;
  sp[20] = RVM_REG->Reg.X21_S5;
  sp[21] = RVM_REG->Reg.X22_S6;
  sp[22] = RVM_REG->Reg.X23_S7;
  sp[23] = RVM_REG->Reg.X24_S8;
  sp[24] = RVM_REG->Reg.X25_S9;
  sp[25] = RVM_REG->Reg.X26_S10;
  sp[26] = RVM_REG->Reg.X27_S11;
  sp[27] = RVM_REG->Reg.X28_T3;
  sp[28] = RVM_REG->Reg.X29_T4;
  sp[29] = RVM_REG->Reg.X30_T5;
  sp[30] = RVM_REG->Reg.X31_T6;

  mstatus = RVM_REG->Reg.MSTATUS;

#if CHB_USE_FPU
  if ((mstatus & CHB_RV_MSTATUS_FS_DIRTY) != 0U) {
    sp -= CHB_RV_FPU_WORDS;
    chb_fpu_save(sp);
  }
#endif

  sp--;
  sp[0] = mstatus;
  sp -= CHB_RV_PARAMS;
  sp[0] = RVM_STATE->Usr.Number;
  sp[1] = RVM_STATE->Usr.Param[0];
  sp[2] = RVM_STATE->Usr.Param[1];
  sp[3] = RVM_STATE->Usr.Param[2];
  sp[4] = RVM_STATE->Usr.Param[3];

  if (otp != NULL) {
    otp->ctx.sp = (struct port_intctx *)(void *)sp;
  }

  base = (uint32_t *)(void *)ntp->ctx.sp;
  RVM_STATE->Usr.Number = base[0];
  RVM_STATE->Usr.Param[0] = base[1];
  RVM_STATE->Usr.Param[1] = base[2];
  RVM_STATE->Usr.Param[2] = base[3];
  RVM_STATE->Usr.Param[3] = base[4];

  base += CHB_RV_PARAMS;
  mstatus = base[0];
  base += CHB_RV_MSTATUS;

#if CHB_USE_FPU
  if ((mstatus & CHB_RV_MSTATUS_FS_DIRTY) != 0U) {
    chb_fpu_load(base);
    base += CHB_RV_FPU_WORDS;
  }
#endif

  RVM_REG->Reg.MSTATUS = mstatus;
  RVM_REG->Reg.PC = base[0];
  RVM_REG->Reg.X1_RA = base[1];
  RVM_REG->Reg.X3_GP = base[2];
  RVM_REG->Reg.X4_TP = base[3];
  RVM_REG->Reg.X5_T0 = base[4];
  RVM_REG->Reg.X6_T1 = base[5];
  RVM_REG->Reg.X7_T2 = base[6];
  RVM_REG->Reg.X8_S0_FP = base[7];
  RVM_REG->Reg.X9_S1 = base[8];
  RVM_REG->Reg.X10_A0 = base[9];
  RVM_REG->Reg.X11_A1 = base[10];
  RVM_REG->Reg.X12_A2 = base[11];
  RVM_REG->Reg.X13_A3 = base[12];
  RVM_REG->Reg.X14_A4 = base[13];
  RVM_REG->Reg.X15_A5 = base[14];
  RVM_REG->Reg.X16_A6 = base[15];
  RVM_REG->Reg.X17_A7 = base[16];
  RVM_REG->Reg.X18_S2 = base[17];
  RVM_REG->Reg.X19_S3 = base[18];
  RVM_REG->Reg.X20_S4 = base[19];
  RVM_REG->Reg.X21_S5 = base[20];
  RVM_REG->Reg.X22_S6 = base[21];
  RVM_REG->Reg.X23_S7 = base[22];
  RVM_REG->Reg.X24_S8 = base[23];
  RVM_REG->Reg.X25_S9 = base[24];
  RVM_REG->Reg.X26_S10 = base[25];
  RVM_REG->Reg.X27_S11 = base[26];
  RVM_REG->Reg.X28_T3 = base[27];
  RVM_REG->Reg.X29_T4 = base[28];
  RVM_REG->Reg.X30_T5 = base[29];
  RVM_REG->Reg.X31_T6 = base[30];
  RVM_REG->Reg.X2_SP = (rvm_ptr_t)(base + CHB_RV_INT_WORDS);
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

void chb_setup_context(thread_t *tp, void *wtop,
                       void (*pf)(void *), void *arg) {
  uint32_t *base;
  uint32_t *ir;
  uint32_t i;

  base = (uint32_t *)(void *)((uint8_t *)wtop - CHB_RV_FRAME_WORDS * 4U);
  for (i = 0U; i < CHB_RV_FRAME_WORDS; i++) {
    base[i] = 0U;
  }

  base[CHB_RV_PARAMS] = CHB_USE_FPU ? CHB_RV_MSTATUS_INIT
                                    : CHB_RV_MSTATUS_NO_FPU;
  ir = base + CHB_RV_PARAMS + CHB_RV_MSTATUS;
  ir[0] = (uint32_t)(uintptr_t)chb_thread_start;
  ir[1] = 0U;
  ir[2] = (uint32_t)(uintptr_t)&__RVM_Global;
  ir[9] = (uint32_t)(uintptr_t)arg;
  ir[10] = (uint32_t)(uintptr_t)pf;

  tp->ctx.sp = (struct port_intctx *)(void *)base;
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

#if CHB_FAST_PATH
void chb_fast_save(uint32_t *sp) {
  chb_fast_otp->ctx.sp = (struct port_intctx *)(void *)sp;
}

uint32_t *chb_fast_load(void) {
  return (uint32_t *)(void *)chb_fast_ntp->ctx.sp;
}
#endif

void chb_port_switch(thread_t *ntp, thread_t *otp) {
#if CHB_FAST_PATH
  if ((RVM_STATE->Vct_Act == 0U) && (ntp != otp) && (otp != NULL) &&
      chb_frame_is_valid(ntp)) {
    chb_fast_otp = otp;
    chb_fast_ntp = ntp;
    chb_fast_switch();
    return;
  }
#endif

  chb_pending_ntp = ntp;
  chb_pending_otp = otp;

  if (RVM_STATE->Vct_Act == 0U) {
    RVM_Virt_Yield();
    RVM_Virt_Int_Unmask();
  }
  else {
    RVM_Virt_Yield();
  }
}

void chb_thread_start(void *arg, void (*pf)(void *)) {
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
#if CHB_FAST_PATH
  chb_fast_otp = NULL;
  chb_fast_ntp = NULL;
#endif

  RVM_Virt_Int_Mask();
  RVM_Virt_Tim_Reg(chb_tick_handler);
  RVM_Virt_Ctx_Reg(chb_ctx_handler);
  RVM_Hyp_Int_Ena();
}

/** @} */
