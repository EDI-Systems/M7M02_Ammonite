/*
*********************************************************************************************************
*                                                 uC/CPU
*                                     CPU CONFIGURATION & PORT LAYER
*
*                               Copyright (C) 2026 Evo-Devo Instrum.
*                                       All rights reserved.
*
*                This file is NOT part of the uC/CPU distribution.  It belongs
*                to the uC/OS-III-on-RVM test suite and is released under the
*                Unlicense; refer to the LICENSE file for details.  The original
*                uC/OS-III and uC/CPU distributions are copyright Silicon
*                Laboratories Inc. and are not modified by this file.
*
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                           CPU CONFIGURATION
*                                     ARMv6-M - RVM Hypervisor Guest
*
* File      : cpu_cfg.h
* Version   : V1.00.00
*********************************************************************************************************
*/

/*
*********************************************************************************************************
* Note(s)  : (1) This file provides the uC/CPU configuration for ARMv6-M systems running as
*                unprivileged guests of the RVM hypervisor: only the differences against the
*                defaults of cpu_core.h are defined here.  TS_TMR_SIZE must stay CPU_WORD_SIZE_32,
*                and LEAD/TRAIL_ZEROS_ASM_PRESENT must stay undefined - cpu_core.h uses #ifndef
*                semantics, and defining them would drop the C implementations of the count
*                leading/trailing zeros helpers.  The timestamp, kernel profiling and interrupt
*                disable measurement facilities are all kept off.
*********************************************************************************************************
*/

#ifndef  CPU_CFG_H
#define  CPU_CFG_H


/*
*********************************************************************************************************
*                                             CPU MODULE CONFIGURATION
*********************************************************************************************************
*/

                                                                /* --------------------- NAMES ----------------------- */
#define  CPU_CFG_NAME_EN                            0u          /* Enable (1) or disable (0) the CPU host name.         */
#define  CPU_CFG_NAME_SIZE                          16u          /* Configure the size of the CPU host name.             */

                                                                /* ------------------ TIMESTAMP ----------------------- */
#define  CPU_CFG_TS_EN                              0u          /* Enable (1) or disable (0) the timestamp feature.     */
#define  CPU_CFG_TS_32_EN                           0u          /* Only uC/CPU supports 32-bit free-running counters.   */
#define  CPU_CFG_TS_64_EN                           0u          /* Only uC/CPU supports 64-bit free-running counters.   */
#define  CPU_CFG_TS_TMR_SIZE                        CPU_WORD_SIZE_32 /* Size of the CPU timestamp timer, in bits.    */

                                                                /* ------------- INTERRUPT DISABLE TIME --------------- */
#define  CPU_CFG_INT_DIS_MEAS_OVRHD_NBR             1u          /* Number of overhead measurements.                     */

                                                                /* --------------------- CACHE ------------------------ */
#define  CPU_CFG_CACHE_MGMT_EN                      0u          /* Enable (1) or disable (0) the cache management.      */

                                                                /* ------------------- NVIC (unused) ------------------ */
#define  CPU_CFG_NVIC_PRIO_BITS                     2u          /* The NVIC belongs to the hypervisor under RVM.        */


/*
*********************************************************************************************************
*                                             MODULE END
*********************************************************************************************************
*/

#endif
