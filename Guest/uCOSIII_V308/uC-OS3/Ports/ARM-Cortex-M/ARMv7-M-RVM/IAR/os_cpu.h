/*
*********************************************************************************************************
*                                               uC/OS-III
*                                          The Real-Time Kernel
*
*                               Copyright (C) 2026 Evo-Devo Instrum.
*                                       All rights reserved.
*
*                This file is NOT part of the uC/OS-III distribution.  It belongs
*                to the uC/OS-III-on-RVM test suite and is released under the
*                Unlicense; refer to the LICENSE file for details.  The original
*                uC/OS-III and uC/CPU distributions are copyright Silicon
*                Laboratories Inc. and are not modified by this file.
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
* Note(s)  : (1) This file declares the RVM porting layer of uC/OS-III for ARMv7-M systems running
*                as an unprivileged guest of the RVM hypervisor.  The context switches do not use
*                the PendSV exception of the official ports - the guest cannot trigger it - but go
*                through the RVM virtual yield hypercall instead, with the context switch itself
*                performed by the handler that is registered through RVM_Virt_Ctx_Reg().
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
extern  "C" {                                    /* See Note #1.                                       */
#endif

/*
*********************************************************************************************************
*                                               MACROS
*********************************************************************************************************
*/

#ifndef  OS_CFG_RVM_FAST_YIELD_EN
#error   "OS_CFG_RVM_FAST_YIELD_EN              not #define'd in 'os_cfg.h'"
#endif

#if OS_CFG_RVM_FAST_YIELD_EN != 0
#define  OS_TASK_SW()               OSCtxSw()                  /* Fast yield on the task stacks.                      */
#else
#define  OS_TASK_SW()               RVM_Virt_Yield()           /* Yield through the hypervisor - traps every switch.  */
#endif

#define  OS_TASK_SW_SYNC()          __asm volatile ("isb")

/*
*********************************************************************************************************
*                                        TIMESTAMP CONFIGURATION
*
* Note(s) : (1) The uC/CPU timestamp timer is disabled under RVM (the guest cannot reach the DWT),
*               and the benchmark measures with its own timer instead.  OS_TS_GET() therefore
*               always returns zero.
*********************************************************************************************************
*/

#define  OS_TS_GET()                ((CPU_TS)0u)


/*
*********************************************************************************************************
*                                         FUNCTION PROTOTYPES
*********************************************************************************************************
*/

                                                   /* See OS_CPU_A.S                                      */
void  OSStartHighRdy       (void);
void  OSCtxSw_Swt          (void);

                                                   /* See OS_CPU_C.C                                      */
void  OSCtxSw              (void);
void  OSCtxSw_Err          (void);
void  OSIntCtxSw           (void);
void  OS_Ctx_Handler       (void);
void  OS_Tick_Handler      (void);
void  OS_CPU_Port_Init     (void);

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
