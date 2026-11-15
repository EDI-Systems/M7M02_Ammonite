/******************************************************************************
Filename    : prc_proc1.h
Author      : The RVM project generator.
Date        : 15/11/2026 04:05:58
License     : Unlicense; see COPYING for details.
Description : The user kernel object header.
******************************************************************************/

/* Define ********************************************************************/
#ifndef __PRC_PROC1__
#define __PRC_PROC1__

/* Process capability table frontier & size */
#define CPT_SIZE                                        (7U)
#define CPT_FRONT                                       (7U)

/* Ports */
#define PRT_INV1_PRC_PROC2                              (1U)

/* Receive endpoints */
#define RCV_RECV1                                       (2U)
#define RCV_RECV2                                       (3U)

/* Send endpoints */
#define SND_RECV1_PRC_PROC2                             (4U)
#define SND_RECV2_PRC_PROC2                             (5U)

/* Vector endpoints */
#define VCT_TIM4                                        (6U)

/* Kernel functions */

/* Code memory blocks */
#define CODE_CODE1_BASE                                 (0x80E0000U)
#define CODE_CODE1_SIZE                                 (0x2000U)

/* Data memory blocks */
#define DATA_DATA1_BASE                                 (0x24076000U)
#define DATA_DATA1_SIZE                                 (0x2000U)
#define DATA_SHARED_SHARED1_BASE                        (0x2407A000U)
#define DATA_SHARED_SHARED1_SIZE                        (0x400U)

/* Device memory blocks */
#define DEVICE_SHARED_DEVICE1_BASE                      (0x40000000U)
#define DEVICE_SHARED_DEVICE1_SIZE                      (0x20000000U)

/* Page table settings */
#define RVM_PGT_RAW_ENABLE                              (0U)
/* Total priority number */
#define RVM_PREEMPT_PRIO_NUM                            (32U)
/* Total VM priority number */
#define RVM_PREEMPT_VPRIO_NUM                           (32U)
/* The kernel memory allocation granularity order */
#define RVM_KOM_SLOT_ORDER                              (4U)

/* Virtual machine library disable */
#define RVM_VIRT_LIB_ENABLE                             (0U)

/* Debugging setting */
#define RVM_ASSERT_ENABLE                               (1U)
#define RVM_DBGLOG_ENABLE                               (1U)

/* Coprocessor option */
#define RVM_COP_NUM                                     (0U)
#define RVM_A7M_COP_FPV4_SP                             (0U)
#define RVM_A7M_COP_FPV5_SP                             (0U)
#define RVM_A7M_COP_FPV5_DP                             (0U)
#endif /* __PRC_PROC1__ */
/* End Define ****************************************************************/

/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/


