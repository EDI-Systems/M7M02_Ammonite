/*
    ChibiOS - Copyright (C) 2006-2026 Giovanni Di Sirio.
    CHB modifications Copyright (C) 2026 Evo-Devo Instrum.

    This file is part of ChibiOS.

    ChibiOS is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation version 3 of the License.

    ChibiOS is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

/**
 * @file    ARMv7-M/chcore.h
 * @brief   ARMv7-M port macros and structures.
 *
 * @addtogroup ARMV7M_CORE
 * @{
 */

#ifndef CHCORE_H
#define CHCORE_H

/* Inclusion of the Cortex-Mx implementation specific parameters.*/
#include "cmparams.h"
#include "chb_abi.h"

#if !defined(__STATIC_FORCEINLINE)
#if defined(__ICCARM__)
#define __STATIC_FORCEINLINE static inline
#elif defined(__CC_ARM)
#define __STATIC_FORCEINLINE __forceinline static
#elif defined(__GNUC__)
#define __STATIC_FORCEINLINE static inline __attribute__((always_inline))
#else
#define __STATIC_FORCEINLINE static inline
#endif
#endif

/*===========================================================================*/
/* Module constants.                                                         */
/*===========================================================================*/

/* The following code is not processed when the file is included from an
   asm module because those intrinsic macros are not necessarily defined
   by the assembler too.*/
#if !defined(_FROM_ASM_)

/**
 * @brief   Compiler name and version.
 */
#if defined(__GNUC__) || defined(__DOXYGEN__)
#define PORT_COMPILER_NAME              "GCC " __VERSION__

#elif defined(__ICCARM__)
#define PORT_COMPILER_NAME              "IAR"

#elif defined(__CC_ARM)
#define PORT_COMPILER_NAME              "RVCT"

#else
#error "unsupported compiler"
#endif

#endif /* !defined(_FROM_ASM_) */
/** @} */

/**
 * @name    Priority Ranges
 * @{
 */
/**
 * @brief   Disabled value for BASEPRI register.
 */
#define CORTEX_BASEPRI_DISABLED         0U

/**
 * @brief   Total priority levels.
 */
#define CORTEX_PRIORITY_LEVELS          (1U << CORTEX_PRIORITY_BITS)

/**
 * @brief   Minimum priority level.
 * @details This minimum priority level is calculated from the number of
 *          priority bits supported by the specific Cortex-Mx implementation.
 */
#define CORTEX_MINIMUM_PRIORITY         (CORTEX_PRIORITY_LEVELS - 1U)

/**
 * @brief   Maximum priority level.
 * @details The maximum allowed priority level is always zero.
 */
#define CORTEX_MAXIMUM_PRIORITY         0U

/**
 * @brief   SVCALL handler priority.
 */
#define CORTEX_PRIORITY_SVCALL          (CORTEX_MAXIMUM_PRIORITY +          \
                                         CORTEX_FAST_PRIORITIES)

/**
 * @brief   PendSV priority level.
 * @note    This priority is enforced to be equal to
 *          @p CORTEX_MAX_KERNEL_PRIORITY, this handler always have the
 *          highest priority that cannot preempt the kernel.
 */
#define CORTEX_PRIORITY_PENDSV          CORTEX_MAX_KERNEL_PRIORITY

/**
 * @brief   Priority level to priority mask conversion macro.
 */
#define CORTEX_PRIO_MASK(n)             ((n) << (8 - CORTEX_PRIORITY_BITS))
/** @} */

/*===========================================================================*/
/* Module pre-compile time settings.                                         */
/*===========================================================================*/

/**
 * @brief   Implements a syscall interface on SVC.
 */
#if !defined(PORT_USE_SYSCALL) || defined(__DOXYGEN__)
#define PORT_USE_SYSCALL                FALSE
#endif

/**
 * @brief   Number of MPU regions to be saved/restored during context switch.
 * @note    The first region is always region zero.
 * @note    The use of this option has an overhead of 8 bytes for each
 *          region for each thread.
 * @note    Allowed values are 0..4, zero means none.
 */
#if !defined(PORT_SWITCHED_REGIONS_NUMBER) || defined(__DOXYGEN__)
#define PORT_SWITCHED_REGIONS_NUMBER    0
#endif

/**
 * @brief   Enables stack overflow guard pages using MPU.
 * @note    This option can only be enabled if also option
 *          @p CH_DBG_ENABLE_STACK_CHECK is enabled.
 * @note    The use of this option has an overhead of 32 bytes for each
 *          thread.
 */
#if !defined(PORT_ENABLE_GUARD_PAGES) || defined(__DOXYGEN__)
#define PORT_ENABLE_GUARD_PAGES         FALSE
#endif

/**
 * @brief   Stack size for the system idle thread.
 * @details This size depends on the idle thread implementation, usually
 *          the idle thread should take no more space than those reserved
 *          by @p PORT_INT_REQUIRED_STACK.
 * @note    In this port it is set to 64 because the idle thread does have
 *          a stack frame when compiling without optimizations. You may
 *          reduce this value to zero when compiling with optimizations.
 */
#if !defined(PORT_IDLE_THREAD_STACK_SIZE) || defined(__DOXYGEN__)
#define PORT_IDLE_THREAD_STACK_SIZE     64
#endif

/**
 * @brief   Per-thread stack overhead for interrupts servicing.
 * @details This constant is used in the calculation of the correct working
 *          area size.
 * @note    In this port this value is conservatively set to 64 because the
 *          function @p chSchDoPreemption() can have a stack frame, especially
 *          with compiler optimizations disabled. The value can be reduced
 *          when compiler optimizations are enabled.
 */
#if !defined(PORT_INT_REQUIRED_STACK) || defined(__DOXYGEN__)
#define PORT_INT_REQUIRED_STACK         64
#endif

/**
 * @brief   Enables the use of the WFI instruction in the idle thread loop.
 */
#if !defined(CORTEX_ENABLE_WFI_IDLE)
#define CORTEX_ENABLE_WFI_IDLE          FALSE
#endif

/**
 * @brief   Number of upper priority levels reserved as fast interrupts.
 * @note    The default reserves priorities 0 and 1 for fast interrupts.
 */
#if !defined(CORTEX_FAST_PRIORITIES)
#define CORTEX_FAST_PRIORITIES          2U
#endif

/**
 * @brief   FPU support in context switch.
 * @details Activating this option activates the FPU support in the kernel.
 */
#if !defined(CORTEX_USE_FPU)
#define CORTEX_USE_FPU                  CORTEX_HAS_FPU
#elif (CORTEX_USE_FPU == TRUE) && (CORTEX_HAS_FPU == FALSE)
/* This setting requires an FPU presence check in case it is externally
   redefined.*/
#error "the selected core does not have an FPU"
#endif

/**
 * @brief   Simplified priority handling flag.
 * @details When this option is disabled:
 *          - Interrupts are not disabled globally, the priority mask BASEPRI
 *            is raised up to CORTEX_BASEPRI_KERNEL instead.
 *          - An extra structure @p port_extctx is allocated for each thread
 *            in order to allow safe processing of fast interrupts.
 *          .
 *          When this option is enabled:
 *          - Interrupts are disabled globally instead of
 *            raising the priority mask, this makes code generally more
 *            compact and faster. There is no extra @p port_extctx structure
 *            so less RAM is used.
 *          - There is no support for fast interrupts.
 *          .
 */
#if !defined(CORTEX_SIMPLIFIED_PRIORITY)
#define CORTEX_SIMPLIFIED_PRIORITY      FALSE
#endif

/**
 * @brief   NVIC PRIGROUP initialization expression.
 * @details The default assigns all available priority bits as preemption
 *          priority with no sub-priority.
 */
#if !defined(CORTEX_PRIGROUP_INIT) || defined(__DOXYGEN__)
#define CORTEX_PRIGROUP_INIT            (7 - CORTEX_PRIORITY_BITS)
#endif

/*===========================================================================*/
/* Derived constants and error checks.                                       */
/*===========================================================================*/

#if (PORT_SWITCHED_REGIONS_NUMBER < 0) || (PORT_SWITCHED_REGIONS_NUMBER > 4)
  #error "invalid PORT_SWITCHED_REGIONS_NUMBER value"
#endif

#if (CORTEX_FAST_PRIORITIES < 0U) ||                                         \
    (CORTEX_FAST_PRIORITIES > (CORTEX_PRIORITY_LEVELS / 4U))
#error "invalid CORTEX_FAST_PRIORITIES value specified"
#endif

/**
 * @name    Port Capabilities and Constants
 * @{
 */
/**
 * @brief   This port supports a realtime counter.
 */
#define PORT_SUPPORTS_RT                FALSE

/**
 * @brief   Natural alignment constant.
 * @note    It is the minimum alignment for pointer-size variables.
 */
#define PORT_NATURAL_ALIGN              sizeof (void *)

/**
 * @brief   Stack alignment constant.
 * @note    It is the alignment required for the stack pointer.
 */
#define PORT_STACK_ALIGN                sizeof (stkalign_t)

/**
 * @brief   Working Areas alignment constant.
 * @note    It is the alignment to be enforced for thread working areas.
 */
#define PORT_WORKING_AREA_ALIGN         ((PORT_ENABLE_GUARD_PAGES == TRUE) ?\
                                         32U : PORT_STACK_ALIGN)
/** @} */

/**
 * @name    Architecture
 * @{
 */
/**
 * @brief   Macro defining a generic ARM architecture.
 */
#define PORT_ARCHITECTURE_ARM

#if (CORTEX_MODEL == 3) || defined(__DOXYGEN__)

  #if !defined(CH_CUSTOMER_LIC_PORT_CM3)
    #error "CH_CUSTOMER_LIC_PORT_CM3 not defined"
  #endif

  #if CH_CUSTOMER_LIC_PORT_CM3 == FALSE
    #error "ChibiOS Cortex-M3 port not licensed"
  #endif

  /**
   * @brief   Macro defining the specific ARM architecture.
   */
  #define PORT_ARCHITECTURE_ARM_V7M

  /**
   * @brief   Name of the implemented architecture.
   */
  #define PORT_ARCHITECTURE_NAME        "ARMv7-M"

  /**
   * @brief   Name of the architecture variant.
   */
  #if (PORT_ENABLE_GUARD_PAGES == FALSE) || defined(__DOXYGEN__)
    #define PORT_CORE_VARIANT_NAME      "Cortex-M3"
  #else
    #define PORT_CORE_VARIANT_NAME      "Cortex-M3 (MPU)"
  #endif

#elif (CORTEX_MODEL == 4)

  #if !defined(CH_CUSTOMER_LIC_PORT_CM4)
    #error "CH_CUSTOMER_LIC_PORT_CM4 not defined"
  #endif

  #if CH_CUSTOMER_LIC_PORT_CM4 == FALSE
    #error "ChibiOS Cortex-M4 port not licensed"
  #endif

  #define PORT_ARCHITECTURE_ARM_v7ME
  #define PORT_ARCHITECTURE_NAME        "ARMv7E-M"
  #if CORTEX_USE_FPU
    #if PORT_ENABLE_GUARD_PAGES == FALSE
      #define PORT_CORE_VARIANT_NAME    "Cortex-M4F"
    #else
      #define PORT_CORE_VARIANT_NAME    "Cortex-M4F (MPU)"
    #endif
  #else
    #if PORT_ENABLE_GUARD_PAGES == FALSE
      #define PORT_CORE_VARIANT_NAME    "Cortex-M4"
    #else
      #define PORT_CORE_VARIANT_NAME    "Cortex-M4 (MPU)"
    #endif
  #endif

#elif (CORTEX_MODEL == 7)

  #if !defined(CH_CUSTOMER_LIC_PORT_CM7)
    #error "CH_CUSTOMER_LIC_PORT_CM7 not defined"
  #endif

  #if CH_CUSTOMER_LIC_PORT_CM7 == FALSE
    #error "ChibiOS Cortex-M7 port not licensed"
  #endif

  #define PORT_ARCHITECTURE_ARM_v7ME
  #define PORT_ARCHITECTURE_NAME        "ARMv7E-M"
  #if CORTEX_USE_FPU
    #if PORT_ENABLE_GUARD_PAGES == FALSE
      #define PORT_CORE_VARIANT_NAME    "Cortex-M7F"
    #else
      #define PORT_CORE_VARIANT_NAME    "Cortex-M7F (MPU)"
    #endif
  #else
    #if PORT_ENABLE_GUARD_PAGES == FALSE
      #define PORT_CORE_VARIANT_NAME    "Cortex-M7"
    #else
      #define PORT_CORE_VARIANT_NAME    "Cortex-M7 (MPU)"
    #endif
  #endif

#else
  #error "unknown ARMv7-M variant"
#endif

/**
 * @brief   Port-specific information string.
 */
#if (CORTEX_SIMPLIFIED_PRIORITY == FALSE) || defined(__DOXYGEN__)
  #define PORT_INFO                     "Advanced kernel mode"
#else
  #define PORT_INFO                     "Compact kernel mode"
#endif
/** @} */

#if (CORTEX_SIMPLIFIED_PRIORITY == FALSE) || defined(__DOXYGEN__)
  /**
   * @brief   Maximum usable priority for normal ISRs.
   */
  #define CORTEX_MAX_KERNEL_PRIORITY    (CORTEX_PRIORITY_SVCALL + 1U)

  /**
   * @brief   Minimum usable priority for normal ISRs.
   */
  #define CORTEX_MIN_KERNEL_PRIORITY    (CORTEX_PRIORITY_LEVELS - 1U)

  /**
   * @brief   BASEPRI level within kernel lock.
   */
  #define CORTEX_BASEPRI_KERNEL                                             \
    CORTEX_PRIO_MASK(CORTEX_MAX_KERNEL_PRIORITY)

#else
  #define CORTEX_MAX_KERNEL_PRIORITY    0U
  #define CORTEX_MIN_KERNEL_PRIORITY    (CORTEX_PRIORITY_LEVELS - 1U)
#endif

/* The following code is not processed when the file is included from an
   asm module.*/
#if !defined(_FROM_ASM_)

/**
 * @brief   MPU guard page size.
 */
#if (PORT_ENABLE_GUARD_PAGES == TRUE) || defined(__DOXYGEN__)
  #if CH_DBG_ENABLE_STACK_CHECK == FALSE
    #error "PORT_ENABLE_GUARD_PAGES requires CH_DBG_ENABLE_STACK_CHECK"
  #endif
  #if __MPU_PRESENT == 0
    #error "MPU not present in current device"
  #endif
  #define PORT_GUARD_PAGE_SIZE          32U
#else
  #define PORT_GUARD_PAGE_SIZE          0U
#endif

/*===========================================================================*/
/* Module data structures and types.                                         */
/*===========================================================================*/

/**
 * @brief   Interrupt saved context.
 * @details This structure represents the stack frame saved during a
 *          preemption-capable interrupt handler.
 * @note    It is implemented to match the Cortex-Mx exception context.
 */
struct port_extctx {
  uint32_t      r0;
  uint32_t      r1;
  uint32_t      r2;
  uint32_t      r3;
  uint32_t      r12;
  uint32_t      lr_thd;
  uint32_t      pc;
  uint32_t      xpsr;
#if CORTEX_USE_FPU
  uint32_t      s0;
  uint32_t      s1;
  uint32_t      s2;
  uint32_t      s3;
  uint32_t      s4;
  uint32_t      s5;
  uint32_t      s6;
  uint32_t      s7;
  uint32_t      s8;
  uint32_t      s9;
  uint32_t      s10;
  uint32_t      s11;
  uint32_t      s12;
  uint32_t      s13;
  uint32_t      s14;
  uint32_t      s15;
  uint32_t      fpscr;
  uint32_t      reserved;
#endif /* CORTEX_USE_FPU */
};

#if (PORT_USE_SYSCALL == TRUE) || defined(__DOXYGEN__)
/**
 * @brief   Link context structure.
 * @details This structure is used when there is the need to save extra
 *          context information that is not part of the registers stacked
 *          in HW.
 */
struct port_linkctx {
  uint32_t              control;
  struct port_extctx    *ectxp;
};
#endif

/**
 * @brief   System saved context.
 * @details This structure represents the inner stack frame during a context
 *          switch.
 */
struct port_intctx {
  /* RVM hypercall parameter block saved with the thread context. */
  uint32_t      params[CHB_FRAME_PARAMS];
  /* Callee-saved general purpose registers and the EXC_RETURN. */
  uint32_t      r4;
  uint32_t      r5;
  uint32_t      r6;
  uint32_t      r7;
  uint32_t      r8;
  uint32_t      r9;
  uint32_t      r10;
  uint32_t      r11;
  uint32_t      lr;
  /* Hardware exception frame stacked by RVM on the next exception. */
  uint32_t      r0;
  uint32_t      r1;
  uint32_t      r2;
  uint32_t      r3;
  uint32_t      r12;
  uint32_t      lr_thd;
  uint32_t      pc;
  uint32_t      xpsr;
};

#if (CHB_FRAME_BASIC_WORDS != 22U)
#error "unexpected CHB basic frame size"
#endif

/**
 * @brief   Platform dependent part of the @p thread_t structure.
 * @details In this port the structure just holds a pointer to the
 *          @p port_intctx structure representing the stack pointer
 *          at context switch time.
 */
struct port_context {
  struct port_intctx    *sp;
#if (PORT_USE_SYSCALL == TRUE) || defined(__DOXYGEN__)
  struct {
    uint32_t            psp;
    const void          *p;
  } syscall;
#endif
};

#endif /* !defined(_FROM_ASM_) */

/*===========================================================================*/
/* Module macros.                                                            */
/*===========================================================================*/

/**
 * @brief   Priority level verification macro.
 */
#define PORT_IRQ_IS_VALID_PRIORITY(n)                                       \
  (((n) >= 0U) && ((n) < CORTEX_PRIORITY_LEVELS))

/**
 * @brief   Priority level verification macro.
 */
#define PORT_IRQ_IS_VALID_KERNEL_PRIORITY(n)                                \
  (((n) >= CORTEX_MAX_KERNEL_PRIORITY) && ((n) <= CORTEX_MIN_KERNEL_PRIORITY))

/**
 * @brief   Optimized thread function declaration macro.
 */
#define PORT_THD_FUNCTION(tname, arg) void tname(void *arg)

/* By default threads have no syscall context information.*/
#if (PORT_USE_SYSCALL == TRUE) || defined(__DOXYGEN__)
  #define __PORT_SETUP_CONTEXT_SYSCALL(tp, wtop)                            \
    (tp)->ctx.syscall.psp = (uint32_t)(wtop);                               \
    (tp)->ctx.syscall.p   = NULL;
#else
  #define __PORT_SETUP_CONTEXT_SYSCALL(tp, wtop)
#endif

/* By default threads have all regions disabled.*/
#if (PORT_SWITCHED_REGIONS_NUMBER == 0) || defined(__DOXYGEN__)
  #define __PORT_SETUP_CONTEXT_MPU(tp)

#elif (PORT_SWITCHED_REGIONS_NUMBER == 1) || defined(__DOXYGEN__)
  #define __PORT_SETUP_CONTEXT_MPU(tp)                                      \
    (tp)->ctx.sp->regions[0].rbar  = 0U;                                    \
    (tp)->ctx.sp->regions[0].rasr  = 0U

#elif (PORT_SWITCHED_REGIONS_NUMBER == 2) || defined(__DOXYGEN__)
  #define __PORT_SETUP_CONTEXT_MPU(tp)                                      \
    (tp)->ctx.sp->regions[0].rbar  = 0U;                                    \
    (tp)->ctx.sp->regions[0].rasr  = 0U;                                    \
    (tp)->ctx.sp->regions[1].rbar  = 0U;                                    \
    (tp)->ctx.sp->regions[1].rasr  = 0U

#elif (PORT_SWITCHED_REGIONS_NUMBER == 3) || defined(__DOXYGEN__)
  #define __PORT_SETUP_CONTEXT_MPU(tp)                                      \
    (tp)->ctx.sp->regions[0].rbar  = 0U;                                    \
    (tp)->ctx.sp->regions[0].rasr  = 0U;                                    \
    (tp)->ctx.sp->regions[1].rbar  = 0U;                                    \
    (tp)->ctx.sp->regions[1].rasr  = 0U;                                    \
    (tp)->ctx.sp->regions[2].rbar  = 0U;                                    \
    (tp)->ctx.sp->regions[2].rasr  = 0U

#elif (PORT_SWITCHED_REGIONS_NUMBER == 4) || defined(__DOXYGEN__)
  #define __PORT_SETUP_CONTEXT_MPU(tp)                                      \
    (tp)->ctx.sp->regions[0].rbar  = 0U;                                    \
    (tp)->ctx.sp->regions[0].rasr  = 0U;                                    \
    (tp)->ctx.sp->regions[1].rbar  = 0U;                                    \
    (tp)->ctx.sp->regions[1].rasr  = 0U;                                    \
    (tp)->ctx.sp->regions[2].rbar  = 0U;                                    \
    (tp)->ctx.sp->regions[2].rasr  = 0U;                                    \
    (tp)->ctx.sp->regions[3].rbar  = 0U;                                    \
    (tp)->ctx.sp->regions[3].rasr  = 0U

#else
  /* Note, checked above.*/
#endif

/**
 * @brief   Platform dependent part of the @p chThdCreateI() API.
 * @details This code usually setup the context switching frame represented
 *          by an @p port_intctx structure.
 */
#define PORT_SETUP_CONTEXT(tp, wbase, wtop, pf, arg) do {                   \
  (tp)->ctx.sp = (struct port_intctx *)(void *)                             \
                 ((uint8_t *)(wtop) - sizeof (struct port_intctx));         \
  (tp)->ctx.sp->params[0] = 0U;                                             \
  (tp)->ctx.sp->params[1] = 0U;                                             \
  (tp)->ctx.sp->params[2] = 0U;                                             \
  (tp)->ctx.sp->params[3] = 0U;                                             \
  (tp)->ctx.sp->params[4] = 0U;                                             \
  (tp)->ctx.sp->r4 = (uint32_t)(pf);                                        \
  (tp)->ctx.sp->r5 = (uint32_t)(arg);                                       \
  (tp)->ctx.sp->r6 = 0U;                                                    \
  (tp)->ctx.sp->r7 = 0U;                                                    \
  (tp)->ctx.sp->r8 = 0U;                                                    \
  (tp)->ctx.sp->r9 = 0U;                                                    \
  (tp)->ctx.sp->r10 = 0U;                                                   \
  (tp)->ctx.sp->r11 = 0U;                                                   \
  (tp)->ctx.sp->lr = CHB_EXC_RETURN_BASIC;                                  \
  (tp)->ctx.sp->r0 = (uint32_t)(arg);                                       \
  (tp)->ctx.sp->r1 = 0U;                                                    \
  (tp)->ctx.sp->r2 = 0U;                                                    \
  (tp)->ctx.sp->r3 = 0U;                                                    \
  (tp)->ctx.sp->r12 = 0U;                                                   \
  (tp)->ctx.sp->lr_thd = 0U;                                                \
  (tp)->ctx.sp->pc = (uint32_t)__port_thread_start | 1U;                    \
  (tp)->ctx.sp->xpsr = CHB_XPSR_THUMB;                                      \
  (void)(wbase);                                                            \
  __PORT_SETUP_CONTEXT_MPU(tp);                                             \
  __PORT_SETUP_CONTEXT_SYSCALL(tp, wtop);                                   \
} while (false)

/**
 * @brief   Context switch area size.
 */
#if CORTEX_USE_FPU
#define PORT_WA_CTX_SIZE (CHB_FRAME_EXT_WORDS * 4U)
#elif (CORTEX_SIMPLIFIED_PRIORITY == TRUE) || defined(__DOXYGEN__)
#define PORT_WA_CTX_SIZE (sizeof (struct port_intctx) +                     \
                          sizeof (struct port_extctx))
#else
#define PORT_WA_CTX_SIZE (sizeof (struct port_intctx) +                     \
                          sizeof (struct port_extctx) +                     \
                          sizeof (struct port_extctx))
#endif

/**
 * @brief   Computes the thread working area global size.
 * @note    There is no need to perform alignments in this macro.
 */
#define PORT_WA_SIZE(n) ((size_t)PORT_GUARD_PAGE_SIZE +                     \
                         (size_t)PORT_WA_CTX_SIZE +                         \
                         (size_t)(n) +                                      \
                         (size_t)PORT_INT_REQUIRED_STACK)

/**
 * @brief   Static working area allocation.
 * @details This macro is used to allocate a static thread working area
 *          aligned as both position and size.
 *
 * @param[in] s         the name to be assigned to the stack array
 * @param[in] n         the stack size to be assigned to the thread
 */
#if (PORT_ENABLE_GUARD_PAGES == FALSE) || defined(__DOXYGEN__)
  #define PORT_WORKING_AREA(s, n)                                           \
    stkalign_t s[THD_WORKING_AREA_SIZE(n) / sizeof (stkalign_t)]

#else
  #define PORT_WORKING_AREA(s, n)                                           \
    ALIGNED_VAR(32) stkalign_t s[THD_WORKING_AREA_SIZE(n) / sizeof (stkalign_t)]
#endif

/**
 * @brief   IRQ prologue code.
 * @details This macro must be inserted at the start of all IRQ handlers
 *          enabled to invoke system APIs.
 */
#define PORT_IRQ_PROLOGUE()

/**
 * @brief   IRQ epilogue code.
 * @details This macro must be inserted at the end of all IRQ handlers
 *          enabled to invoke system APIs.
 */
#define PORT_IRQ_EPILOGUE() chb_irq_epilogue()

/**
 * @brief   IRQ handler function declaration.
 * @note    @p id can be a function name or a vector number depending on the
 *          port implementation.
 */
#ifdef __cplusplus
  #define PORT_IRQ_HANDLER(id) extern "C" void id(void)

#else
  #define PORT_IRQ_HANDLER(id) void id(void)
#endif

/**
 * @brief   Fast IRQ handler function declaration.
 * @note    @p id can be a function name or a vector number depending on the
 *          port implementation.
 */
#ifdef __cplusplus
  #define PORT_FAST_IRQ_HANDLER(id) extern "C" void id(void)

#else
  #define PORT_FAST_IRQ_HANDLER(id) void id(void)
#endif

/**
 * @brief   Performs a context switch between two threads.
 * @details This is the most critical code in any port, this function
 *          is responsible for the context switch between 2 threads.
 * @note    The implementation of this code affects <b>directly</b> the context
 *          switch performance so optimize here as much as you can.
 *
 * @param[in] ntp       the thread to be switched in
 * @param[in] otp       the thread to be switched out
 */
#define port_switch(ntp, otp) chb_port_switch(ntp, otp)

/**
 * @brief   Returns a word representing a critical section status.
 *
 * @return              The critical section status.
 */
#if !defined(_FROM_ASM_)
__STATIC_FORCEINLINE syssts_t __port_get_irq_status(void);
__STATIC_FORCEINLINE bool __port_irq_enabled(syssts_t sts);
#endif

#define port_get_lock_status() __port_get_irq_status()

/**
 * @brief   Determines if in a critical section.
 *
 * @param[in] sts       status word returned by @p port_get_lock_status()
 * @return              The current status.
 * @retval false        if running outside a critical section.
 * @retval true         if running within a critical section.
 */
#define port_is_locked(sts) !__port_irq_enabled(sts)

/*===========================================================================*/
/* External declarations.                                                    */
/*===========================================================================*/

#if !defined(_FROM_ASM_)

#ifdef __cplusplus
extern "C" {
#endif
  void port_init(os_instance_t *oip);
  void __port_thread_start(void);
  uint32_t chb_get_xpsr(void);
  void chb_port_switch(thread_t *ntp, thread_t *otp);
#if defined(CHB_FAST_PATH) && CHB_FAST_PATH
  void chb_fast_switch(void);
#endif
  void chb_irq_epilogue(void);
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
#if PORT_USE_SYSCALL == TRUE
  void port_unprivileged_jump(uint32_t pc, uint32_t psp);
#endif
#ifdef __cplusplus
}
#endif

/*===========================================================================*/
/* Module inline functions.                                                  */
/*===========================================================================*/

/**
 * @brief   Returns a word encoding the current interrupts status.
 *
 * @return              The interrupts status.
 */
__STATIC_FORCEINLINE syssts_t __port_get_irq_status(void) {
  return (syssts_t)chb_lock_status();
}

/**
 * @brief   Checks the interrupt status.
 *
 * @param[in] sts       the interrupt status word
 *
 * @return              The interrupt status.
 * @retval false        the word specified a disabled interrupts status.
 * @retval true         the word specified an enabled interrupts status.
 */
__STATIC_FORCEINLINE bool __port_irq_enabled(syssts_t sts) {
  return sts == (syssts_t)0U;
}

/**
 * @brief   Determines the current execution context.
 *
 * @return              The execution context.
 * @retval false        not running in ISR mode.
 * @retval true         running in ISR mode.
 */
__STATIC_FORCEINLINE bool port_is_isr_context(void) {
  return chb_is_isr_context();
}

/**
 * @brief   Kernel-lock action.
 * @details In this port this function raises the base priority to kernel
 *          level.
 */
__STATIC_FORCEINLINE void port_lock(void) {
  chb_lock();
}

/**
 * @brief   Kernel-unlock action.
 * @details In this port this function lowers the base priority to user
 *          level.
 */
__STATIC_FORCEINLINE void port_unlock(void) {
  chb_unlock();
}

/**
 * @brief   Kernel-lock action from an interrupt handler.
 * @details In this port this function raises the base priority to kernel
 *          level.
 * @note    Same as @p port_lock() in this port.
 */
__STATIC_FORCEINLINE void port_lock_from_isr(void) {
  chb_lock_from_isr();
}

/**
 * @brief   Kernel-unlock action from an interrupt handler.
 * @details In this port this function lowers the base priority to user
 *          level.
 * @note    Same as @p port_unlock() in this port.
 */
__STATIC_FORCEINLINE void port_unlock_from_isr(void) {
  chb_unlock_from_isr();
}

/**
 * @brief   Disables all the interrupt sources.
 * @note    In this port it disables all the interrupt sources by raising
 *          the priority mask to level 0.
 */
__STATIC_FORCEINLINE void port_disable(void) {
  chb_disable();
}

/**
 * @brief   Disables the interrupt sources below kernel-level priority.
 * @note    Interrupt sources above kernel level remains enabled.
 * @note    In this port it raises/lowers the base priority to kernel level.
 */
__STATIC_FORCEINLINE void port_suspend(void) {
  chb_suspend();
}

/**
 * @brief   Enables all the interrupt sources.
 * @note    In this port it lowers the base priority to user level.
 */
__STATIC_FORCEINLINE void port_enable(void) {
  chb_enable();
}

/**
 * @brief   Enters an architecture-dependent IRQ-waiting mode.
 * @details The function is meant to return when an interrupt becomes pending.
 *          The simplest implementation is an empty function or macro but this
 *          would not take advantage of architecture-specific power saving
 *          modes.
 * @note    Implemented as an inlined @p WFI instruction.
 */
__STATIC_FORCEINLINE void port_wait_for_interrupt(void) {
  chb_wait_for_interrupt();
}

/**
 * @brief   Returns the current value of the realtime counter.
 *
 * @return              The realtime counter value.
 */
__STATIC_FORCEINLINE rtcnt_t port_rt_get_counter_value(void) {
  return (rtcnt_t)0U;
}

#endif /* !defined(_FROM_ASM_) */

/*===========================================================================*/
/* Module late inclusions.                                                   */
/*===========================================================================*/

#if !defined(_FROM_ASM_)

#if CH_CFG_ST_TIMEDELTA > 0
#include "chcore_timer.h"
#endif /* CH_CFG_ST_TIMEDELTA > 0 */

#endif /* !defined(_FROM_ASM_) */

#endif /* CHCORE_H */

/** @} */
