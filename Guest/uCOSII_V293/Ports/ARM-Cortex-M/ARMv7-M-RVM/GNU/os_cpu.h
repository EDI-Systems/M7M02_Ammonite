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
*                                         ARMv7-M Port - os_cpu.h
*
* File      : os_cpu.h
* Version   : V1.00.00
*********************************************************************************************************
*/

/*
*********************************************************************************************************
* Note(s)  : (1) This file declares the RVM porting layer of uC/OS-II for ARMv7-M systems running
*                as an unprivileged guest of the RVM hypervisor.  The context switches do not use
*                the PendSV exception of the official ports - the guest cannot trigger it - but go
*                through the RVM virtual yield hypercall instead, with the context switch itself
*                performed by the handler that is registered through RVM_Virt_Ctx_Reg().
*
*            (2) uC/OS-II does not use the uC/CPU abstraction layer: this header defines all the
*                types the kernel expects from the port, the critical section macros are mapped
*                onto the RVM virtual interrupt mask, and the port is fully self-contained.
*
*            (3) The EPSR ICI/IT bits of a frame that is being switched to can only be restored
*                by a real exception return; the task level fast path cannot restore them.  The
*                C level OSCtxSw() filter checks the target frame and routes dirty frames onto
*                the interrupt driven slow path.
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
#define  OS_TASK_SW()     OSCtxSw()            /* Filtered fast yield (see os_cpu_c.c).              */
#else
#define  OS_TASK_SW()     RVM_Virt_Yield()     /* Yield through the hypervisor.                      */
#endif

/*
*********************************************************************************************************
*                                          DATA TYPES
*********************************************************************************************************
*/

typedef  unsigned  char       BOOLEAN;
typedef  unsigned  char       INT8U;
typedef  signed    char       INT8S;
typedef  unsigned  short      INT16U;
typedef  signed    short      INT16S;
typedef  unsigned  int        INT32U;
typedef  signed    int        INT32S;
typedef  float                 FP32;
typedef  double                FP64;

typedef  unsigned  int        OS_STK;
typedef  unsigned  int        OS_CPU_SR;

/*
*********************************************************************************************************
*                                      CRITICAL SECTION MACROS
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
void  OSCtxSw_Swt          (void);

                                                   /* See OS_CPU_C.C.                                    */
void  OSCtxSw              (void);
void  OSCtxSw_Err          (void);
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

#endif
