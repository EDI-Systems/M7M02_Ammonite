/*
*********************************************************************************************************
*                                                uC/OS-II
*                                          The Real-Time Kernel
*
*                               Copyright (C) 2026 Evo-Devo Instrum.
*                                       All rights reserved.
*
*                This file is NOT part of the uC/OS-II distribution.  It belongs
*                to the uC/OS-II-on-RVM test suite and is released under the
*                Unlicense; refer to the LICENSE file for details.  The original
*                uC/OS-II kernel is copyright Weston Embedded and is not
*                modified by this file.
*
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                         ARMv6-M Port - os_cpu.h
*
* File      : os_cpu.h
* Version   : V1.00.00
*********************************************************************************************************
*/

/*
*********************************************************************************************************
* Note(s)  : (1) This file declares the RVM porting layer of uC/OS-II for ARMv6-M systems running
*                as an unprivileged guest of the RVM hypervisor.  The context switches do not use
*                the PendSV exception of the official ports - the guest cannot trigger it - but go
*                through the RVM virtual yield hypercall instead, with the context switch itself
*                performed by the handler that is registered through RVM_Virt_Ctx_Reg().
*
*            (2) uC/OS-II does not use the uC/CPU abstraction layer: this header defines all the
*                types the kernel expects from the port, the critical section macros are mapped
*                onto the RVM virtual interrupt mask, and the port is fully self-contained.
*********************************************************************************************************
*/

#ifndef  OS_CPU_RVM_H
#define  OS_CPU_RVM_H

#ifdef   OS_CPU_GLOBALS
#define  OS_CPU_EXT
#else
#define  OS_CPU_EXT  extern
#endif

#ifdef __cplusplus
extern  "C" {
#endif

/*
*********************************************************************************************************
*                                           INCLUDE FILES
*********************************************************************************************************
*/

#include  <stdint.h>
#include  "rvm.h"

/*
*********************************************************************************************************
*                                        DEFINES AND MACROS
*********************************************************************************************************
*/

#define  OS_STK_GROWTH    1u                   /* Stack grows from HIGH to LOW memory on ARM        */

#define  OS_CRITICAL_METHOD  3u                /* See Note #2                                        */

#ifndef   OS_CFG_RVM_FAST_YIELD_EN
#error    "OS_CFG_RVM_FAST_YIELD_EN           not #define'd in 'os_cfg.h'"
#endif

#if OS_CFG_RVM_FAST_YIELD_EN != 0
#define  OS_TASK_SW()     OSCtxSw()            /* Fast yield on the task stacks.                     */
#else
#define  OS_TASK_SW()     RVM_Virt_Yield()     /* Yield through the hypervisor.                      */
#endif

/*
*********************************************************************************************************
*                                          DATA TYPES
*
* Note(s)  : (1) The uC/OS-II kernel expects the port to define its own sized integer types.
*                These are NOT the uC/CPU types - uC/OS-II V2.93 is self-contained.
*********************************************************************************************************
*/

typedef  unsigned  char       BOOLEAN;         /* Boolean                                    */
typedef  unsigned  char       INT8U;           /* Unsigned  8 bit quantity                   */
typedef  signed    char       INT8S;           /* Signed    8 bit quantity                   */
typedef  unsigned  short      INT16U;          /* Unsigned 16 bit quantity                   */
typedef  signed    short      INT16S;          /* Signed   16 bit quantity                   */
typedef  unsigned  int        INT32U;          /* Unsigned 32 bit quantity                   */
typedef  signed    int        INT32S;          /* Signed   32 bit quantity                   */
typedef  float                 FP32;           /* Single precision floating point            */
typedef  double                FP64;           /* Double precision floating point            */

typedef  unsigned  int        OS_STK;          /* Each stack entry is 32-bit wide            */
typedef  unsigned  int        OS_CPU_SR;       /* The size of the status register            */

/*
*********************************************************************************************************
*                                      CRITICAL SECTION MACROS
*
* Note(s)  : (1) The RVM virtual interrupt mask is a flat flag, not a nested counter; the snapshot
*                is taken on entry and restored on exit, so nesting is balanced by the snapshot
*                rather than by a counter.  Writing back "unmasked" must go through the full
*                unmask path or pending virtual interrupts would starve.
*
*            (2) OS_CRITICAL_METHOD 3 means the kernel declares a local `cpu_sr` variable and the
*                macros save/restore the interrupt state through it.
*********************************************************************************************************
*/

#define  OS_ENTER_CRITICAL()  do { cpu_sr = OS_CPU_SR_Save();    } while (0)
#define  OS_EXIT_CRITICAL()   do {        OS_CPU_SR_Restore(cpu_sr); } while (0)

/*
*********************************************************************************************************
*                                       FUNCTION PROTOTYPES
*********************************************************************************************************
*/

                                                   /* See OS_CPU_A.S.                                    */
void  OSStartHighRdy       (void);

                                                   /* See OS_CPU_C.C.                                    */
void  OSCtxSw              (void);
void  OSIntCtxSw           (void);
void  OS_Ctx_Handler       (void);
void  OS_Tick_Handler      (void);
void  OS_CPU_Port_Init     (void);

OS_CPU_SR  OS_CPU_SR_Save  (void);
void       OS_CPU_SR_Restore(OS_CPU_SR  cpu_sr);

/*
*********************************************************************************************************
*                                   EXTERNAL C LANGUAGE LINKAGE END
*********************************************************************************************************
*/

#ifdef __cplusplus
}
#endif

/*
*********************************************************************************************************
*                                             MODULE END
*********************************************************************************************************
*/

#endif
