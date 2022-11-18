/******************************************************************************
Filename    : proc_proc2.h
Author      : The RME project generator.
Date        : 10/11/2019
License     : LGPL v3+; see COPYING for details.
Description : The user kernel object header.
******************************************************************************/

/* Defines *******************************************************************/
#ifndef __PROC_PROC2_H__
#define __PROC_PROC2_H__
/* Ports */

/* Receive endpoints */
#define RECV_RECV1                                      (1)

/* Send endpoints */
#define SEND_RECV1                                      (2)

/* Vector endpoints */
#define VECT_TIM2                                       (3)

/* Kernel function */
#define KERN_KFUNC1                                     (4)

/* Code memory blocks */
#define ACODE_0X8028000_BASE                            (0x8028000)
#define ACODE_0X8028000_SIZE                            (0x8000)

/* Data memory blocks */
#define DATA_MAIN_DATA_BASE                             (0x20006000)
#define DATA_MAIN_DATA_SIZE                             (0x4000)
#define SDATA_SHARED1_BASE                              (0x10008000)
#define SDATA_SHARED1_SIZE                              (0x1000)

/* Device memory blocks */

/* The number of MPU regions */
#define RVM_MPU_REGIONS                                 (8)
/* The kernel memory allocation granularity order */
#define RVM_KMEM_SLOT_ORDER                             (4)
#endif /* __PROC_PROC2_H__ */
/* End Defines ***************************************************************/

/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/

