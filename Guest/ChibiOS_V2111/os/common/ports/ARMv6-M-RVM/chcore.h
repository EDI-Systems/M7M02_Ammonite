/*
    ChibiOS - Copyright (C) 2006-2026 Giovanni Di Sirio.

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
 * @file    ARMv6-M-RVM/chcore.h
 * @brief   ARMv6-M port macros and structures.
 *
 * @addtogroup ARMV6M_RVM_CORE
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
 * @name    Port Capabilities and Constants
 * @{
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
#define PORT_WORKING_AREA_ALIGN         PORT_STACK_ALIGN
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
#define CORTEX_MINIMUM_PRIORITY         (CORTEX_PRIORITY_LEVELS - 1)

/**
 * @brief   Maximum priority level.
 * @details The maximum allowed priority level is always zero.
 */
#define CORTEX_MAXIMUM_PRIORITY         0U

/**
 * @brief   PendSV priority level.
 * @note    This priority is enforced to be equal to @p 0,
 *          this handler always has the highest priority that cannot preempt
 *          the kernel.
 */
#define CORTEX_PRIORITY_PENDSV          0

/**
 * @brief   Priority level to priority mask conversion macro.
 */
#define CORTEX_PRIO_MASK(n)                                                 \
  ((n) << (8U - (unsigned)CORTEX_PRIORITY_BITS))
/** @} */

/*===========================================================================*/
/* Module pre-compile time settings.                                         */
/*===========================================================================*/

/**
 * @brief   Stack size for the system idle thread.
 * @details This size depends on the idle thread implementation, usually
 *          the idle thread should take no more space than those reserved
 *          by @p PORT_INT_REQUIRED_STACK.
 * @note    In this port it is set to 16 because the idle thread does have
 *          a stack frame when compiling without optimizations. You may
 *          reduce this value to zero when compiling with optimizations.
 */
#if !defined(PORT_IDLE_THREAD_STACK_SIZE)
#define PORT_IDLE_THREAD_STACK_SIZE     16
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
#if !defined(PORT_INT_REQUIRED_STACK)
#define PORT_INT_REQUIRED_STACK         64
#endif

/**
 * @brief   Enables the use of the WFI instruction in the idle thread loop.
 */
#if !defined(CORTEX_ENABLE_WFI_IDLE)
#define CORTEX_ENABLE_WFI_IDLE          FALSE
#endif

/**
 * @brief   Alternate preemption method.
 * @details Activating this option will make the Kernel use the PendSV
 *          handler for preemption instead of the NMI handler.
 */
#ifndef CORTEX_ALTERNATE_SWITCH
#define CORTEX_ALTERNATE_SWITCH         FALSE
#endif

/*===========================================================================*/
/* Derived constants and error checks.                                       */
/*===========================================================================*/

#if !defined(CH_CUSTOMER_LIC_PORT_CM0)
  #error "CH_CUSTOMER_LIC_PORT_CM0 not defined"
#endif

#if CH_CUSTOMER_LIC_PORT_CM0 == FALSE
  #error "ChibiOS Cortex-M0 port not licensed"
#endif

/* The upstream GCC version diagnostic for Cortex-M0 code generation is
   dropped: it uses #warning, which the IAR assembler preprocessor does not
   implement, and the supported GCC versions here are outside its range. */

/**
 * @name    Architecture
 * @{
 */
/**
 * @brief   Macro defining a generic ARM architecture.
 */
#define PORT_ARCHITECTURE_ARM

/**
 * @brief   Macro defining the specific ARM architecture.
 */
#define PORT_ARCHITECTURE_ARM_V6M

/**
 * @brief   Name of the implemented architecture.
 */
#define PORT_ARCHITECTURE_NAME          "ARMv6-M"

/**
 * @brief   Name of the architecture variant.
 * @note    The RVM guest does not include any vendor CMSIS; the STM32G0
 *          devices are all Cortex-M0+.
 */
#define PORT_CORE_VARIANT_NAME          "Cortex-M0+"

/**
 * @brief   Port-specific information string.
 */
#define PORT_INFO                     "RVM hypervisor guest"
/** @} */

/**
 * @brief   Maximum usable priority for normal ISRs.
 */
#if (CORTEX_ALTERNATE_SWITCH == TRUE) || defined(__DOXYGEN__)
  #define CORTEX_MAX_KERNEL_PRIORITY    1
#else
  #define CORTEX_MAX_KERNEL_PRIORITY    0
#endif

/*===========================================================================*/
/* Module data structures and types.                                         */
/*===========================================================================*/

/* The following code is not processed when the file is included from an
   asm module.*/
#if !defined(_FROM_ASM_)

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
};

/**
 * @brief   System saved context.
 * @details This structure is the masqueraded RVM exception frame that the
 *          guest builds for a task-level switch and that the monitor
 *          stacks for a vector-thread switch.  ARMv6-M frames are always
 *          basic frames.
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
};

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
  (((n) >= CORTEX_MAX_KERNEL_PRIORITY) && ((n) < CORTEX_PRIORITY_LEVELS))

/**
 * @brief   Optimized thread function declaration macro.
 */
#define PORT_THD_FUNCTION(tname, arg) void tname(void *arg)

/**
 * @brief   Platform dependent part of the @p chThdCreateI() API.
 * @details This code usually setup the context switching frame represented
 *          by an @p port_intctx structure.
 */
#define PORT_SETUP_CONTEXT(tp, wbase, wtop, pf, arg) do {                    \
  (tp)->ctx.sp = (struct port_intctx *)(void *)                              \
                   ((uint8_t *)(wtop) - sizeof (struct port_intctx));        \
  (tp)->ctx.sp->params[0] = 0U;                                              \
  (tp)->ctx.sp->params[1] = 0U;                                              \
  (tp)->ctx.sp->params[2] = 0U;                                              \
  (tp)->ctx.sp->params[3] = 0U;                                              \
  (tp)->ctx.sp->params[4] = 0U;                                              \
  (tp)->ctx.sp->r4 = (uint32_t)(pf);                                         \
  (tp)->ctx.sp->r5 = (uint32_t)(arg);                                        \
  (tp)->ctx.sp->r6 = 0U;                                                     \
  (tp)->ctx.sp->r7 = 0U;                                                     \
  (tp)->ctx.sp->r8 = 0U;                                                     \
  (tp)->ctx.sp->r9 = 0U;                                                     \
  (tp)->ctx.sp->r10 = 0U;                                                    \
  (tp)->ctx.sp->r11 = 0U;                                                    \
  (tp)->ctx.sp->lr = CHB_EXC_RETURN_BASIC;                                   \
  (tp)->ctx.sp->r0 = (uint32_t)(arg);                                        \
  (tp)->ctx.sp->r1 = 0U;                                                     \
  (tp)->ctx.sp->r2 = 0U;                                                     \
  (tp)->ctx.sp->r3 = 0U;                                                     \
  (tp)->ctx.sp->r12 = 0U;                                                    \
  (tp)->ctx.sp->lr_thd = 0U;                                                 \
  (tp)->ctx.sp->pc = (uint32_t)__port_thread_start | 1U;                     \
  (tp)->ctx.sp->xpsr = CHB_XPSR_THUMB;                                       \
  (void)(wbase);                                                             \
} while (false)

/**
 * @brief   Computes the thread working area global size.
 * @note    There is no need to perform alignments in this macro.
 */
#define PORT_WA_SIZE(n) (sizeof (struct port_intctx) +                      \
                         sizeof (struct port_extctx) +                      \
                         ((size_t)(n)) + ((size_t)(PORT_INT_REQUIRED_STACK)))

/**
 * @brief   Static working area allocation.
 * @details This macro is used to allocate a static thread working area
 *          aligned as both position and size.
 *
 * @param[in] s         the name to be assigned to the stack array
 * @param[in] n         the stack size to be assigned to the thread
 */
#define PORT_WORKING_AREA(s, n)                                             \
  stkalign_t s[THD_WORKING_AREA_SIZE(n) / sizeof (stkalign_t)]

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

#ifdef __cplusplus
extern "C" {
#endif
  void port_init(os_instance_t *oip);
  void __port_thread_start(void);
#if CHB_FAST_PATH
  uint32_t chb_get_xpsr(void);
  void chb_fast_switch(void);
#endif
  void chb_port_switch(thread_t *ntp, thread_t *otp);
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
 * @details In this port the RVM virtual interrupt mask is raised.
 */
__STATIC_FORCEINLINE void port_lock(void) {
  chb_lock();
}

/**
 * @brief   Kernel-unlock action.
 */
__STATIC_FORCEINLINE void port_unlock(void) {
  chb_unlock();
}

/**
 * @brief   Kernel-lock action from an interrupt handler.
 */
__STATIC_FORCEINLINE void port_lock_from_isr(void) {
  chb_lock_from_isr();
}

/**
 * @brief   Kernel-unlock action from an interrupt handler.
 */
__STATIC_FORCEINLINE void port_unlock_from_isr(void) {
  chb_unlock_from_isr();
}

/**
 * @brief   Disables all the interrupt sources.
 */
__STATIC_FORCEINLINE void port_disable(void) {
  chb_disable();
}

/**
 * @brief   Disables the interrupt sources below kernel-level priority.
 */
__STATIC_FORCEINLINE void port_suspend(void) {
  chb_suspend();
}

/**
 * @brief   Enables all the interrupt sources.
 */
__STATIC_FORCEINLINE void port_enable(void) {
  chb_enable();
}

/**
 * @brief   Enters an architecture-dependent IRQ-waiting mode.
 * @details The RVM hypercall parks the VM until the next vector.
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

#endif /* CHCORE_H */

/** @} */
