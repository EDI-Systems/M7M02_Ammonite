/******************************************************************************
Filename    : proc_virt1.h
Author      : The RME project generator.
Date        : 10/11/2019
License     : LGPL v3+; see COPYING for details.
Description : The user kernel object header.
******************************************************************************/

/* Defines *******************************************************************/
#ifndef __PROC_VIRT1_H__
#define __PROC_VIRT1_H__
/* Ports */

/* Receive endpoints */

/* Send endpoints */
#define SEND_RECV1                                      (2)

/* Vector endpoints */

/* Kernel function */
#define KERN_KFUNC1                                     (3)

/* Code memory blocks */
#define CODE_MAIN_CODE_BASE                             (0x8030000)
#define CODE_MAIN_CODE_SIZE                             (0x8000)

/* Data memory blocks */
#define DATA_MAIN_DATA_BASE                             (0x20004000)
#define DATA_MAIN_DATA_SIZE                             (0x2000)
#define SDATA_SHARED1_BASE                              (0x10008000)
#define SDATA_SHARED1_SIZE                              (0x1000)

/* Device memory blocks */

/* Virtual vector total number */
#define RVM_VIRT_VECT_NUM                               (100)
/* Virtual vector bitmap base address */
#define RVM_VIRT_VCTF_BASE                              (0x20005700)
/* Virtual register base address */
#define RVM_VIRT_REG_BASE                               (0x20005400)
/* Hypercall parameter base address */
#define RVM_VIRT_PARAM_BASE                             (0x20005600)
/* The number of MPU regions */
#define RVM_MPU_REGIONS                                 (8)
/* The kernel memory allocation granularity order */
#define RVM_KMEM_SLOT_ORDER                             (4)
#endif /* __PROC_VIRT1_H__ */
/* End Defines ***************************************************************/

/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/

