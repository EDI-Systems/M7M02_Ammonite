/******************************************************************************
Filename    : rvm_boot.h
Author      : The RME project generator.
Date        : 10/11/2019
License     : LGPL v3+; see COPYING for details.
Description : The boot-time initialization file header.
******************************************************************************/

/* Defines *******************************************************************/
/* Vector endpoint capability tables */
/* Vector capability table capability tables */
#define RVM_BOOT_CTVECT0                                (8)

/* Vectors */
#define RVM_BOOT_VECT_TIM2                              (RVM_CAPID(RVM_BOOT_CTVECT0,0))

/* Virtual machine endpoint capability tables */
#define RVM_BOOT_CTVIRTEP0                              (15)

/* Process capability table capability tables */
#define RVM_BOOT_CTCAPTBL0                              (16)

/* Process capability tables */
#define RVM_CAPTBL_PROC1                                (RVM_CAPID(RVM_BOOT_CTCAPTBL0,0))
#define RVM_CAPTBL_PROC2                                (RVM_CAPID(RVM_BOOT_CTCAPTBL0,1))
#define RVM_CAPTBL_VIRT1                                (RVM_CAPID(RVM_BOOT_CTCAPTBL0,2))

/* Process page table capability tables */
#define RVM_BOOT_CTPGTBL0                               (17)

/* Process page tables */
#define RVM_PGTBL_PROC1_N0                              (RVM_CAPID(RVM_BOOT_CTPGTBL0,0))
#define RVM_PGTBL_PROC1_N1                              (RVM_CAPID(RVM_BOOT_CTPGTBL0,1))
#define RVM_PGTBL_PROC1_N2                              (RVM_CAPID(RVM_BOOT_CTPGTBL0,2))
#define RVM_PGTBL_PROC1_N3                              (RVM_CAPID(RVM_BOOT_CTPGTBL0,3))
#define RVM_PGTBL_PROC1_N4                              (RVM_CAPID(RVM_BOOT_CTPGTBL0,4))
#define RVM_PGTBL_PROC1_N5                              (RVM_CAPID(RVM_BOOT_CTPGTBL0,5))
#define RVM_PGTBL_PROC1_N6                              (RVM_CAPID(RVM_BOOT_CTPGTBL0,6))
#define RVM_PGTBL_PROC1_N7                              (RVM_CAPID(RVM_BOOT_CTPGTBL0,7))
#define RVM_PGTBL_PROC2_N0                              (RVM_CAPID(RVM_BOOT_CTPGTBL0,8))
#define RVM_PGTBL_PROC2_N1                              (RVM_CAPID(RVM_BOOT_CTPGTBL0,9))
#define RVM_PGTBL_PROC2_N2                              (RVM_CAPID(RVM_BOOT_CTPGTBL0,10))
#define RVM_PGTBL_PROC2_N3                              (RVM_CAPID(RVM_BOOT_CTPGTBL0,11))
#define RVM_PGTBL_VIRT1_N0                              (RVM_CAPID(RVM_BOOT_CTPGTBL0,12))
#define RVM_PGTBL_VIRT1_N1                              (RVM_CAPID(RVM_BOOT_CTPGTBL0,13))
#define RVM_PGTBL_VIRT1_N2                              (RVM_CAPID(RVM_BOOT_CTPGTBL0,14))
#define RVM_PGTBL_VIRT1_N3                              (RVM_CAPID(RVM_BOOT_CTPGTBL0,15))

/* Process capability tables */
#define RVM_BOOT_CTPROC0                                (18)

/* Processes */
#define RVM_PROC_PROC1                                  (RVM_CAPID(RVM_BOOT_CTPROC0,0))
#define RVM_PROC_PROC2                                  (RVM_CAPID(RVM_BOOT_CTPROC0,1))
#define RVM_PROC_VIRT1                                  (RVM_CAPID(RVM_BOOT_CTPROC0,2))

/* Thread capability tables */
#define RVM_BOOT_CTTHD0                                 (19)

/* Threads */
#define RVM_PROC_PROC1_THD_THD1                         (RVM_CAPID(RVM_BOOT_CTTHD0,0))
#define RVM_PROC_PROC2_THD_THD1                         (RVM_CAPID(RVM_BOOT_CTTHD0,1))
#define RVM_PROC_VIRT1_THD_VECT                         (RVM_CAPID(RVM_BOOT_CTTHD0,2))
#define RVM_PROC_VIRT1_THD_USER                         (RVM_CAPID(RVM_BOOT_CTTHD0,3))

/* Invocation capability tables */
#define RVM_BOOT_CTINV0                                 (20)

/* Invocations */
#define RVM_PROC_PROC2_INV_INV1                         (RVM_CAPID(RVM_BOOT_CTINV0,0))

/* Receive endpoint capability tables */
#define RVM_BOOT_CTRECV0                                (21)

/* Receive endpoints */
#define RVM_PROC_PROC1_RECV_RECV1                       (RVM_CAPID(RVM_BOOT_CTRECV0,0))
#define RVM_PROC_PROC2_RECV_RECV1                       (RVM_CAPID(RVM_BOOT_CTRECV0,1))

/* Capability table frontier */
#define RVM_BOOT_CAP_FRONTIER                           (22)
/* Kernel memory frontier */
#define RVM_BOOT_KMEM_FRONTIER                          (0x1890)
/* End Defines ***************************************************************/

/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/

