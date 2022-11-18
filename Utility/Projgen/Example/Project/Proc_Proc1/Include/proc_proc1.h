/******************************************************************************
Filename    : proc_proc1.h
Author      : The RME project generator.
Date        : 10/11/2019
License     : LGPL v3+; see COPYING for details.
Description : The user kernel object header.
******************************************************************************/

/* Defines *******************************************************************/
#ifndef __PROC_PROC1_H__
#define __PROC_PROC1_H__
/* Ports */
#define PORT_INV1                                       (1)

/* Receive endpoints */
#define RECV_RECV1                                      (2)

/* Send endpoints */

/* Vector endpoints */

/* Kernel function */
#define KERN_KFUNC1                                     (3)

/* Code memory blocks */
#define CODE_MAIN_CODE_BASE                             (0x8020000)
#define CODE_MAIN_CODE_SIZE                             (0x8000)

/* Data memory blocks */
#define DATA_MAIN_DATA_BASE                             (0x20000000)
#define DATA_MAIN_DATA_SIZE                             (0x4000)
#define DATA_EXTRA_DATA_BASE                            (0x10007000)
#define DATA_EXTRA_DATA_SIZE                            (0x1000)
#define SDATA_SHARED1_BASE                              (0x10008000)
#define SDATA_SHARED1_SIZE                              (0x1000)

/* Device memory blocks */
#define DEVICE_MAIN_DEVICE_BASE                         (0xF0000000)
#define DEVICE_MAIN_DEVICE_SIZE                         (0x10000)

/* The number of MPU regions */
#define RVM_MPU_REGIONS                                 (8)
/* The kernel memory allocation granularity order */
#define RVM_KMEM_SLOT_ORDER                             (4)
#endif /* __PROC_PROC1_H__ */
/* End Defines ***************************************************************/

/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/

