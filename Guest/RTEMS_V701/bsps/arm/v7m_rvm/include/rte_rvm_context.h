/* SPDX-License-Identifier: Unlicense */

/**
 * @file
 *
 * @ingroup RTEMSBSPsARMV7MRVM
 *
 * @brief RTE RVM context switch glue.
 *
 * The RVM guest cannot enter the PendSV exception, so the RTEMS context
 * switch is split between the user thread and the RVM vector thread.  The
 * frame layout and the deferred dispatch follow the uC/OS-II RVM port.
 */

#ifndef LIBBSP_ARM_V7M_RVM_RTE_RVM_CONTEXT_H
#define LIBBSP_ARM_V7M_RVM_RTE_RVM_CONTEXT_H

#include <rtems/score/cpu.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief The heir selected by the RTEMS dispatcher on the vector thread.
 *
 * The ISR path records the interrupted thread's frame and then defers the
 * physical switch.  The vector thread's context handler consumes this
 * pointer after the interrupt handler returns.
 */
extern Context_Control *rte_pending_heir;

/**
 * @brief The RVM context switch handler (runs on the vector thread).
 */
void rte_rvm_ctx_handler( void );

/**
 * @brief Complete a deferred dispatch after the virtual ISR.
 *
 * Clears the per-ISR capture guard and yields to the vector thread when a
 * context switch was recorded by _CPU_Context_switch().
 */
void rte_rvm_dispatch_complete( void );

/**
 * @brief Save a context frame from the RVM register set.
 *
 * Used by the vector-thread path when the hardware already stacked the
 * interrupted thread's exception frame.
 */
void rte_rvm_capture_executing( Context_Control *executing );

#ifdef __cplusplus
}
#endif

#endif /* LIBBSP_ARM_V7M_RVM_RTE_RVM_CONTEXT_H */
