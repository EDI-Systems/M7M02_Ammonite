/*
*********************************************************************************************************
*                                               uC/OS-II
*                                         The Real-Time Kernel
*
*                                       RVM Hypervisor Guest Port
*
*                               Copyright (C) 2026 Evo-Devo Instrum.
*                                       All rights reserved.
*
*                This file is NOT part of the uC/OS-II distribution.  It belongs
*                to the uC/OS-II-on-RVM test suite and is released under the
*                Unlicense; refer to the LICENSE file for details.  The uC/OS-II
*                kernel itself is copyright Weston Embedded and is not
*                modified by this file.
*
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                            RISC-V Port - os_cpu.h
*
* File      : os_cpu.h
* Version   : V1.00.00
*********************************************************************************************************
*/


#ifndef _OS_CPU_H
#define _OS_CPU_H

#include  <rvm.h>                                              /* RVM paravirtualization interface.                      */

#ifdef  OS_CPU_GLOBALS
#define OS_CPU_EXT
#else
#define OS_CPU_EXT  extern
#endif


/*
*********************************************************************************************************
*                                     EXTERNAL C LANGUAGE LINKAGE
*
* Note(s) : (1) C++ compilers MUST 'extern'ally declare ALL C function prototypes & variable/object
*               declarations for correct C language linkage.
*********************************************************************************************************
*/

#ifdef __cplusplus
extern  "C" {                                    /* See Note #1.                                       */
#endif


/*
*********************************************************************************************************
*                                               MACROS
*
* Note(s): OS_TASK_SW()  invokes the task level context switch.
*
*          (1) Under RVM the switch is performed directly on the task stack by the assembly
*              fast path in os_cpu_a.s.  Three flavours of OSCtxSw() exist - without a
*              coprocessor, with the single-precision FPU and with the double-precision
*              FPU - and the hypervisor's coprocessor configuration selects between them:
*              the floating point registers are always pushed as a whole, because the FS
*              state cannot be probed from user mode.
*********************************************************************************************************
*/

#ifndef  OS_CFG_RVM_FAST_YIELD_EN
#error   "OS_CFG_RVM_FAST_YIELD_EN              not #define'd in 'os_cfg.h'"
#endif

#if OS_CFG_RVM_FAST_YIELD_EN != 0
#if RVM_RV32P_COP_RVD != 0
#define  OS_TASK_SW()         OSCtxSwRVD()
#elif RVM_RV32P_COP_RVF != 0
#define  OS_TASK_SW()         OSCtxSwRVF()
#else
#define  OS_TASK_SW()         OSCtxSw()
#endif
#else
#define  OS_TASK_SW()         RVM_Virt_Yield()
#endif


/*
*********************************************************************************************************
*                                        DEFINES AND MACROS
*********************************************************************************************************
*/

#define  OS_STK_GROWTH    1u                   /* Stack grows from HIGH to LOW memory              */

#define  OS_CRITICAL_METHOD  3u

/*
*********************************************************************************************************
*                                          DATA TYPES
*
* Note(s)  : (1) uC/OS-II does not use the uC/CPU abstraction layer: the port defines all the
*                sized integer types the kernel expects.
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
*
* Note(s)  : (1) The RVM virtual interrupt mask is a flat flag; the snapshot is taken on entry
*                and restored on exit, so nesting is balanced by the snapshot.
*********************************************************************************************************
*/

#define  OS_ENTER_CRITICAL()  do { cpu_sr = OS_CPU_SR_Save();    } while (0)
#define  OS_EXIT_CRITICAL()   do {        OS_CPU_SR_Restore(cpu_sr); } while (0)


/*
*********************************************************************************************************
*                                         FUNCTION PROTOTYPES
*********************************************************************************************************
*/
                                                  /* See OS_CPU_C.C                                    */
OS_CPU_SR  OS_CPU_SR_Save  (void);
void       OS_CPU_SR_Restore(OS_CPU_SR  cpu_sr);

                                                  /* See OS_CPU_A.S                                    */
void  OSCtxSw       (void);
void  OSCtxSwRVF    (void);
void  OSCtxSwRVD    (void);
void  OSIntCtxSw    (void);
void  OSStartHighRdy(void);
                                                  /* See OS_CPU_C.C                                    */
void  OS_Ctx_Handler(void);
void  OS_Tick_Handler(void);
void  OS_CPU_Port_Init(void);


/*
*********************************************************************************************************
*                                   EXTERNAL C LANGUAGE LINKAGE END
*********************************************************************************************************
*/

#ifdef __cplusplus
}                                                 /* End of 'extern'al C lang linkage.                 */
#endif


/*
*********************************************************************************************************
*                                             MODULE END
*********************************************************************************************************
*/

#endif
