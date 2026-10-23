/*
*********************************************************************************************************
*                                               uC/OS-III
*                                         The Real-Time Kernel
*
*                                       RVM Hypervisor Guest Port
*
*                               Copyright (C) 2026 Evo-Devo Instrum.
*                                       All rights reserved.
*
*                This file is NOT part of the uC/OS-III distribution.  It belongs
*                to the uC/OS-III-on-RVM test suite and is released under the
*                Unlicense; refer to the LICENSE file for details.  The uC/OS-III
*                kernel itself is copyright Silicon Laboratories Inc. and is not
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
*                                       TIMESTAMP CONFIGURATION
*
* Note(s) : (1) OS_TS_GET() is generally defined as CPU_TS_Get32() to allow CPU timestamp timer to be of
*               any data type size.
*
*           (2) For architectures that provide 32-bit or higher precision free running counters
*               (i.e. cycle count registers):
*
*               (a) OS_TS_GET() may be defined as CPU_TS_TmrRd() to improve performance when retrieving
*                   the timestamp.  You would use CPU_TS_TmrRd() if this function returned the value of
*                   a 32-bit free running timer 0x00000000 to 0xFFFFFFFF then roll over to 0x00000000.
*
*               (b) CPU_TS_TmrRd() MUST be configured to be greater or equal to 32-bits to avoid
*                   truncation of TS.
*
*               (c) The Timer must be an up counter.
*********************************************************************************************************
*/

#if      OS_CFG_TS_EN == 1u
#define  OS_TS_GET()               (CPU_TS)CPU_TS_TmrRd()   /* See Note #2a.                           */
#else
#define  OS_TS_GET()               (CPU_TS)0u
#endif

#if (CPU_CFG_TS_32_EN    > 0u) && \
    (CPU_CFG_TS_TMR_SIZE < CPU_WORD_SIZE_32)
                                                  /* see Note #2b.                                     */
#error  "cpu_cfg.h, CPU_CFG_TS_TMR_SIZE MUST be >= CPU_WORD_SIZE_32"
#endif


/*
*********************************************************************************************************
*                                         FUNCTION PROTOTYPES
*********************************************************************************************************
*/
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
