/*
    CHB port for the RVM hypervisor.
    Copyright (C) 2026 Evo-Devo Instrum. All rights reserved.

    This file is NOT part of the ChibiOS distribution.  It belongs to the
    ChibiOS-on-RVM guest port and is released under the Unlicense; refer to
    the LICENSE file for details.  The ChibiOS kernel itself is copyright
    Giovanni Di Sirio and is not modified by this file.
*/

/**
 * @file    RV32P-RVM/chcore.h
 * @brief   ChibiOS RV32P port for the RVM hypervisor.
 *
 * @addtogroup RV32P_RVM_CORE
 * @{
 */

#ifndef CHCORE_H
#define CHCORE_H

#if !defined(_FROM_ASM_)
#include "rvm.h"
#endif

#if !defined(CHB_FAST_PATH)
#define CHB_FAST_PATH                   1
#endif

#if !defined(CHB_USE_FPU)
#define CHB_USE_FPU                     0
#endif
#if CHB_USE_FPU
#define CHB_CFG_USE_FPU                 1U
#else
#define CHB_CFG_USE_FPU                 0U
#endif

#if defined(RVM_RV32P_COP_RVD) && (RVM_RV32P_COP_RVD != 0U)
#define CHB_RV_USE_RVD                  1U
#else
#define CHB_RV_USE_RVD                  0U
#endif

#if defined(RVM_RV32P_COP_RVF) && (RVM_RV32P_COP_RVF != 0U)
#define CHB_RV_USE_RVF                  1U
#else
#define CHB_RV_USE_RVF                  0U
#endif

/* The per-VM coprocessor grant in the RVM headers is authoritative.  The
 * generated CHB_USE_FPU is derived from the same .rvp selection, but using
 * the RVM macros here keeps the frame width tied to what the monitor will
 * actually save and restore. */
#if defined(RVM_COP_NUM) && (RVM_COP_NUM != 0U) &&                        \
    ((CHB_RV_USE_RVF != 0U) || (CHB_RV_USE_RVD != 0U))
#undef CHB_USE_FPU
#define CHB_USE_FPU                     1U
#else
#undef CHB_USE_FPU
#define CHB_USE_FPU                     0U
#endif

#if (CHB_USE_FPU != CHB_CFG_USE_FPU)
#error "CHB_USE_FPU in chb_config.h disagrees with the RVM coprocessor grant"
#endif

#if !CHB_USE_FPU
#define CHB_RV_FPU_WORDS                0U
#elif CHB_RV_USE_RVD
#define CHB_RV_FPU_WORDS                65U
#else
#define CHB_RV_FPU_WORDS                33U
#endif

#if CHB_FAST_PATH && CHB_RV_USE_RVD
/* The direct switch implements the 33-word RVF save sequence only. */
#undef CHB_FAST_PATH
#define CHB_FAST_PATH                   0U
#endif

#if !defined(__STATIC_FORCEINLINE)
#define __STATIC_FORCEINLINE static inline __attribute__((always_inline))
#endif

/*===========================================================================*/
/* Module constants.                                                         */
/*===========================================================================*/

#define PORT_COMPILER_NAME              "GCC"
#define PORT_ARCHITECTURE_NAME          "RV32P"
#define PORT_CORE_VARIANT_NAME          "RV32IMF"
#define PORT_INFO                       "RVM guest"

#define PORT_CORES_NUMBER               1U
#if !defined(PORT_IDLE_THREAD_STACK_SIZE)
#define PORT_IDLE_THREAD_STACK_SIZE     512U
#endif
#if !defined(PORT_INT_REQUIRED_STACK)
#define PORT_INT_REQUIRED_STACK         128U
#endif
#define PORT_SUPPORTS_RT                FALSE
#define PORT_NATURAL_ALIGN              4U
#define PORT_STACK_ALIGN                16U
#define PORT_WORKING_AREA_ALIGN         16U

#define PORT_IRQ_IS_VALID_PRIORITY(n)   ((n) >= 0)
#define PORT_IRQ_IS_VALID_KERNEL_PRIORITY(n) ((n) >= 0)

/* Frame, from the lowest address upwards:
 *   Number, Param[0..3]       5 words
 *   mstatus                   1 word
 *   F0..F31, FCSR             33 words (RVF) or 65 words (RVD),
 *                             only when FS is dirty
 *   PC, X1, X3..X31           31 words (X2 is the frame pointer)
 * New threads have only the 37-word basic frame; the first FPU instruction
 * turns a later save into the extended frame.
 */
#define CHB_RV_PARAMS                   5U
#define CHB_RV_MSTATUS                  1U
#define CHB_RV_INT_WORDS                31U
#define CHB_RV_FRAME_WORDS              (CHB_RV_PARAMS + CHB_RV_MSTATUS + \
                                         CHB_RV_INT_WORDS)
#define CHB_RV_FRAME_MAX                (CHB_RV_PARAMS + CHB_RV_MSTATUS + \
                                         CHB_RV_FPU_WORDS + CHB_RV_INT_WORDS)

#define CHB_RV_MSTATUS_INIT             0x2088U
#define CHB_RV_MSTATUS_NO_FPU           0x0088U
#define CHB_RV_MSTATUS_FS_DIRTY         0x4000U

/*===========================================================================*/
/* Module data structures and types.                                         */
/*===========================================================================*/

struct port_intctx {
  uint32_t data[1];
};

struct port_context {
  struct port_intctx *sp;
};

/*===========================================================================*/
/* Module macros.                                                            */
/*===========================================================================*/

#define PORT_STACK_ALIGN_BYTES          PORT_STACK_ALIGN

#define PORT_SETUP_CONTEXT(tp, wbase, wtop, pf, arg) do {                   \
  chb_setup_context((tp), (void *)(wtop),                                  \
                    (void (*)(void *))(pf), (void *)(arg));                \
  (void)(wbase);                                                           \
} while (false)

#if CHB_USE_FPU
#define PORT_WA_CTX_SIZE                (CHB_RV_FRAME_MAX * 4U)
#else
#define PORT_WA_CTX_SIZE                (CHB_RV_FRAME_WORDS * 4U)
#endif

#define PORT_WA_SIZE(n) ((size_t)PORT_WA_CTX_SIZE + (size_t)(n) +             \
                         (size_t)PORT_INT_REQUIRED_STACK)

#define PORT_WORKING_AREA(s, n)                                             \
  ALIGNED_VAR(PORT_WORKING_AREA_ALIGN)                                      \
  stkalign_t s[THD_WORKING_AREA_SIZE(n) / sizeof(stkalign_t)]

#define PORT_THD_FUNCTION(tname, arg) void tname(void *arg)

#define PORT_IRQ_PROLOGUE()
#define PORT_IRQ_EPILOGUE()             chb_irq_epilogue()
#define PORT_IRQ_HANDLER(id)            void id(void)
#define PORT_FAST_IRQ_HANDLER(id)       void id(void)

#define port_switch(ntp, otp)           chb_port_switch(ntp, otp)

#define port_get_lock_status()          chb_lock_status()
#define port_is_locked(sts)             ((sts) != 0U)

#define port_lock()                     chb_lock()
#define port_unlock()                   chb_unlock()
#define port_lock_from_isr()            chb_lock_from_isr()
#define port_unlock_from_isr()          chb_unlock_from_isr()
#define port_disable()                  chb_disable()
#define port_suspend()                  chb_suspend()
#define port_enable()                   chb_enable()
#define port_is_isr_context()           chb_is_isr_context()
#define port_wait_for_interrupt()       chb_wait_for_interrupt()

/*===========================================================================*/
/* External declarations.                                                    */
/*===========================================================================*/

#ifdef __cplusplus
extern "C" {
#endif
  void port_init(os_instance_t *oip);
  void chb_setup_context(thread_t *tp, void *wtop,
                         void (*pf)(void *), void *arg);
  void chb_thread_start(void *arg, void (*pf)(void *));
  void chb_port_switch(thread_t *ntp, thread_t *otp);
  void chb_irq_epilogue(void);
#if CHB_FAST_PATH
  void chb_fast_switch(void);
  void chb_fast_save(uint32_t *sp);
  uint32_t *chb_fast_load(void);
#endif
  void chb_lock(void);
  void chb_unlock(void);
  void chb_lock_from_isr(void);
  void chb_unlock_from_isr(void);
  void chb_disable(void);
  void chb_suspend(void);
  void chb_enable(void);
  bool chb_is_isr_context(void);
  void chb_wait_for_interrupt(void);
  uint32_t chb_lock_status(void);
#ifdef __cplusplus
}
#endif

#endif /* CHCORE_H */

/** @} */
