/******************************************************************************
Filename    : rvm_boot.h
Author      : The RVM project generator.
Date        : 15/11/2026 04:05:58
License     : Unlicense; see COPYING for details.
Description : The boot-time initialization file header.
******************************************************************************/

/* Define ********************************************************************/
/* Vector endpoint capability tables */
#define RVM_MAIN_VCT_0                                  (7U)

/* Vector endpoints */
#define RVM_VCT_TIM4                                    (RVM_CID(RVM_MAIN_VCT_0, 0U))

/* 2 threads and 1 endpoint for RVM */

/* Virtual machine endpoint capability tables */
#define RVM_MAIN_VEP_0                                  (11U)

/* Process capability table capability tables */
#define RVM_MAIN_CPT_0                                  (12U)

/* Process capability tables */
#define RVM_CPT_PROC1                                   (RVM_CID(RVM_MAIN_CPT_0, 0U))
#define RVM_CPT_PROC2                                   (RVM_CID(RVM_MAIN_CPT_0, 1U))
#define RVM_CPT_VIRT_FRT                                (RVM_CID(RVM_MAIN_CPT_0, 2U))
#define RVM_CPT_VIRT_UO3                                (RVM_CID(RVM_MAIN_CPT_0, 3U))
#define RVM_CPT_VIRT_RTT                                (RVM_CID(RVM_MAIN_CPT_0, 4U))
#define RVM_CPT_VIRT_UO2                                (RVM_CID(RVM_MAIN_CPT_0, 5U))
#define RVM_CPT_VIRT_CHB                                (RVM_CID(RVM_MAIN_CPT_0, 6U))
#define RVM_CPT_VIRT_RMP                                (RVM_CID(RVM_MAIN_CPT_0, 7U))
#define RVM_CPT_VIRT_KLT                                (RVM_CID(RVM_MAIN_CPT_0, 8U))
#define RVM_CPT_VIRT_DJY                                (RVM_CID(RVM_MAIN_CPT_0, 9U))
#define RVM_CPT_VIRT_NTX                                (RVM_CID(RVM_MAIN_CPT_0, 10U))
#define RVM_CPT_VIRT_ZPR                                (RVM_CID(RVM_MAIN_CPT_0, 11U))
#define RVM_CPT_VIRT_RTE                                (RVM_CID(RVM_MAIN_CPT_0, 12U))

/* Process page table capability tables */
#define RVM_MAIN_PGT_0                                  (13U)

/* Process page tables */
#define RVM_PGT_PROC1_0                                 (RVM_CID(RVM_MAIN_PGT_0, 0U))
#define RVM_PGT_PROC1_1                                 (RVM_CID(RVM_MAIN_PGT_0, 1U))
#define RVM_PGT_PROC1_2                                 (RVM_CID(RVM_MAIN_PGT_0, 2U))
#define RVM_PGT_PROC1_3                                 (RVM_CID(RVM_MAIN_PGT_0, 3U))
#define RVM_PGT_PROC2_0                                 (RVM_CID(RVM_MAIN_PGT_0, 4U))
#define RVM_PGT_PROC2_1                                 (RVM_CID(RVM_MAIN_PGT_0, 5U))
#define RVM_PGT_PROC2_2                                 (RVM_CID(RVM_MAIN_PGT_0, 6U))
#define RVM_PGT_PROC2_3                                 (RVM_CID(RVM_MAIN_PGT_0, 7U))
#define RVM_PGT_VIRT_FRT_0                              (RVM_CID(RVM_MAIN_PGT_0, 8U))
#define RVM_PGT_VIRT_FRT_1                              (RVM_CID(RVM_MAIN_PGT_0, 9U))
#define RVM_PGT_VIRT_FRT_2                              (RVM_CID(RVM_MAIN_PGT_0, 10U))
#define RVM_PGT_VIRT_FRT_3                              (RVM_CID(RVM_MAIN_PGT_0, 11U))
#define RVM_PGT_VIRT_FRT_4                              (RVM_CID(RVM_MAIN_PGT_0, 12U))
#define RVM_PGT_VIRT_UO3_0                              (RVM_CID(RVM_MAIN_PGT_0, 13U))
#define RVM_PGT_VIRT_UO3_1                              (RVM_CID(RVM_MAIN_PGT_0, 14U))
#define RVM_PGT_VIRT_UO3_2                              (RVM_CID(RVM_MAIN_PGT_0, 15U))
#define RVM_PGT_VIRT_UO3_3                              (RVM_CID(RVM_MAIN_PGT_0, 16U))
#define RVM_PGT_VIRT_UO3_4                              (RVM_CID(RVM_MAIN_PGT_0, 17U))
#define RVM_PGT_VIRT_RTT_0                              (RVM_CID(RVM_MAIN_PGT_0, 18U))
#define RVM_PGT_VIRT_RTT_1                              (RVM_CID(RVM_MAIN_PGT_0, 19U))
#define RVM_PGT_VIRT_RTT_2                              (RVM_CID(RVM_MAIN_PGT_0, 20U))
#define RVM_PGT_VIRT_RTT_3                              (RVM_CID(RVM_MAIN_PGT_0, 21U))
#define RVM_PGT_VIRT_RTT_4                              (RVM_CID(RVM_MAIN_PGT_0, 22U))
#define RVM_PGT_VIRT_RTT_5                              (RVM_CID(RVM_MAIN_PGT_0, 23U))
#define RVM_PGT_VIRT_UO2_0                              (RVM_CID(RVM_MAIN_PGT_0, 24U))
#define RVM_PGT_VIRT_UO2_1                              (RVM_CID(RVM_MAIN_PGT_0, 25U))
#define RVM_PGT_VIRT_UO2_2                              (RVM_CID(RVM_MAIN_PGT_0, 26U))
#define RVM_PGT_VIRT_UO2_3                              (RVM_CID(RVM_MAIN_PGT_0, 27U))
#define RVM_PGT_VIRT_UO2_4                              (RVM_CID(RVM_MAIN_PGT_0, 28U))
#define RVM_PGT_VIRT_CHB_0                              (RVM_CID(RVM_MAIN_PGT_0, 29U))
#define RVM_PGT_VIRT_CHB_1                              (RVM_CID(RVM_MAIN_PGT_0, 30U))
#define RVM_PGT_VIRT_CHB_2                              (RVM_CID(RVM_MAIN_PGT_0, 31U))
#define RVM_PGT_VIRT_CHB_3                              (RVM_CID(RVM_MAIN_PGT_0, 32U))
#define RVM_PGT_VIRT_CHB_4                              (RVM_CID(RVM_MAIN_PGT_0, 33U))
#define RVM_PGT_VIRT_CHB_5                              (RVM_CID(RVM_MAIN_PGT_0, 34U))
#define RVM_PGT_VIRT_RMP_0                              (RVM_CID(RVM_MAIN_PGT_0, 35U))
#define RVM_PGT_VIRT_RMP_1                              (RVM_CID(RVM_MAIN_PGT_0, 36U))
#define RVM_PGT_VIRT_RMP_2                              (RVM_CID(RVM_MAIN_PGT_0, 37U))
#define RVM_PGT_VIRT_RMP_3                              (RVM_CID(RVM_MAIN_PGT_0, 38U))
#define RVM_PGT_VIRT_KLT_0                              (RVM_CID(RVM_MAIN_PGT_0, 39U))
#define RVM_PGT_VIRT_KLT_1                              (RVM_CID(RVM_MAIN_PGT_0, 40U))
#define RVM_PGT_VIRT_KLT_2                              (RVM_CID(RVM_MAIN_PGT_0, 41U))
#define RVM_PGT_VIRT_KLT_3                              (RVM_CID(RVM_MAIN_PGT_0, 42U))
#define RVM_PGT_VIRT_KLT_4                              (RVM_CID(RVM_MAIN_PGT_0, 43U))
#define RVM_PGT_VIRT_DJY_0                              (RVM_CID(RVM_MAIN_PGT_0, 44U))
#define RVM_PGT_VIRT_DJY_1                              (RVM_CID(RVM_MAIN_PGT_0, 45U))
#define RVM_PGT_VIRT_DJY_2                              (RVM_CID(RVM_MAIN_PGT_0, 46U))
#define RVM_PGT_VIRT_DJY_3                              (RVM_CID(RVM_MAIN_PGT_0, 47U))
#define RVM_PGT_VIRT_DJY_4                              (RVM_CID(RVM_MAIN_PGT_0, 48U))
#define RVM_PGT_VIRT_DJY_5                              (RVM_CID(RVM_MAIN_PGT_0, 49U))
#define RVM_PGT_VIRT_NTX_0                              (RVM_CID(RVM_MAIN_PGT_0, 50U))
#define RVM_PGT_VIRT_NTX_1                              (RVM_CID(RVM_MAIN_PGT_0, 51U))
#define RVM_PGT_VIRT_NTX_2                              (RVM_CID(RVM_MAIN_PGT_0, 52U))
#define RVM_PGT_VIRT_NTX_3                              (RVM_CID(RVM_MAIN_PGT_0, 53U))
#define RVM_PGT_VIRT_ZPR_0                              (RVM_CID(RVM_MAIN_PGT_0, 54U))
#define RVM_PGT_VIRT_ZPR_1                              (RVM_CID(RVM_MAIN_PGT_0, 55U))
#define RVM_PGT_VIRT_ZPR_2                              (RVM_CID(RVM_MAIN_PGT_0, 56U))
#define RVM_PGT_VIRT_ZPR_3                              (RVM_CID(RVM_MAIN_PGT_0, 57U))
#define RVM_PGT_VIRT_ZPR_4                              (RVM_CID(RVM_MAIN_PGT_0, 58U))
#define RVM_PGT_VIRT_RTE_0                              (RVM_CID(RVM_MAIN_PGT_0, 59U))
#define RVM_PGT_VIRT_RTE_1                              (RVM_CID(RVM_MAIN_PGT_0, 60U))
#define RVM_PGT_VIRT_RTE_2                              (RVM_CID(RVM_MAIN_PGT_0, 61U))
#define RVM_PGT_VIRT_RTE_3                              (RVM_CID(RVM_MAIN_PGT_0, 62U))
#define RVM_PGT_VIRT_RTE_4                              (RVM_CID(RVM_MAIN_PGT_0, 63U))
#define RVM_PGT_VIRT_RTE_5                              (RVM_CID(RVM_MAIN_PGT_0, 64U))

/* Process capability tables */
#define RVM_MAIN_PRC_0                                  (14U)

/* Processes */
#define RVM_PRC_PROC1                                   (RVM_CID(RVM_MAIN_PRC_0, 0U))
#define RVM_PRC_PROC2                                   (RVM_CID(RVM_MAIN_PRC_0, 1U))
#define RVM_PRC_VIRT_FRT                                (RVM_CID(RVM_MAIN_PRC_0, 2U))
#define RVM_PRC_VIRT_UO3                                (RVM_CID(RVM_MAIN_PRC_0, 3U))
#define RVM_PRC_VIRT_RTT                                (RVM_CID(RVM_MAIN_PRC_0, 4U))
#define RVM_PRC_VIRT_UO2                                (RVM_CID(RVM_MAIN_PRC_0, 5U))
#define RVM_PRC_VIRT_CHB                                (RVM_CID(RVM_MAIN_PRC_0, 6U))
#define RVM_PRC_VIRT_RMP                                (RVM_CID(RVM_MAIN_PRC_0, 7U))
#define RVM_PRC_VIRT_KLT                                (RVM_CID(RVM_MAIN_PRC_0, 8U))
#define RVM_PRC_VIRT_DJY                                (RVM_CID(RVM_MAIN_PRC_0, 9U))
#define RVM_PRC_VIRT_NTX                                (RVM_CID(RVM_MAIN_PRC_0, 10U))
#define RVM_PRC_VIRT_ZPR                                (RVM_CID(RVM_MAIN_PRC_0, 11U))
#define RVM_PRC_VIRT_RTE                                (RVM_CID(RVM_MAIN_PRC_0, 12U))

/* Thread capability tables */
#define RVM_MAIN_THD_0                                  (15U)

/* Threads */
#define RVM_THD_THD1_PRC_PROC1                          (RVM_CID(RVM_MAIN_THD_0, 0U))
#define RVM_THD_THD2_PRC_PROC1                          (RVM_CID(RVM_MAIN_THD_0, 1U))
#define RVM_THD_THD1_PRC_PROC2                          (RVM_CID(RVM_MAIN_THD_0, 2U))
#define RVM_THD_THD2_PRC_PROC2                          (RVM_CID(RVM_MAIN_THD_0, 3U))
#define RVM_THD_VCT_PRC_VIRT_FRT                        (RVM_CID(RVM_MAIN_THD_0, 4U))
#define RVM_THD_USR_PRC_VIRT_FRT                        (RVM_CID(RVM_MAIN_THD_0, 5U))
#define RVM_THD_VCT_PRC_VIRT_UO3                        (RVM_CID(RVM_MAIN_THD_0, 6U))
#define RVM_THD_USR_PRC_VIRT_UO3                        (RVM_CID(RVM_MAIN_THD_0, 7U))
#define RVM_THD_VCT_PRC_VIRT_RTT                        (RVM_CID(RVM_MAIN_THD_0, 8U))
#define RVM_THD_USR_PRC_VIRT_RTT                        (RVM_CID(RVM_MAIN_THD_0, 9U))
#define RVM_THD_VCT_PRC_VIRT_UO2                        (RVM_CID(RVM_MAIN_THD_0, 10U))
#define RVM_THD_USR_PRC_VIRT_UO2                        (RVM_CID(RVM_MAIN_THD_0, 11U))
#define RVM_THD_VCT_PRC_VIRT_CHB                        (RVM_CID(RVM_MAIN_THD_0, 12U))
#define RVM_THD_USR_PRC_VIRT_CHB                        (RVM_CID(RVM_MAIN_THD_0, 13U))
#define RVM_THD_VCT_PRC_VIRT_RMP                        (RVM_CID(RVM_MAIN_THD_0, 14U))
#define RVM_THD_USR_PRC_VIRT_RMP                        (RVM_CID(RVM_MAIN_THD_0, 15U))
#define RVM_THD_VCT_PRC_VIRT_KLT                        (RVM_CID(RVM_MAIN_THD_0, 16U))
#define RVM_THD_USR_PRC_VIRT_KLT                        (RVM_CID(RVM_MAIN_THD_0, 17U))
#define RVM_THD_VCT_PRC_VIRT_DJY                        (RVM_CID(RVM_MAIN_THD_0, 18U))
#define RVM_THD_USR_PRC_VIRT_DJY                        (RVM_CID(RVM_MAIN_THD_0, 19U))
#define RVM_THD_VCT_PRC_VIRT_NTX                        (RVM_CID(RVM_MAIN_THD_0, 20U))
#define RVM_THD_USR_PRC_VIRT_NTX                        (RVM_CID(RVM_MAIN_THD_0, 21U))
#define RVM_THD_VCT_PRC_VIRT_ZPR                        (RVM_CID(RVM_MAIN_THD_0, 22U))
#define RVM_THD_USR_PRC_VIRT_ZPR                        (RVM_CID(RVM_MAIN_THD_0, 23U))
#define RVM_THD_VCT_PRC_VIRT_RTE                        (RVM_CID(RVM_MAIN_THD_0, 24U))
#define RVM_THD_USR_PRC_VIRT_RTE                        (RVM_CID(RVM_MAIN_THD_0, 25U))

/* Invocation capability tables */
#define RVM_MAIN_INV_0                                  (16U)

/* Invocations */
#define RVM_INV_INV1_PRC_PROC2                          (RVM_CID(RVM_MAIN_INV_0, 0U))

/* Receive endpoint capability tables */
#define RVM_MAIN_RCV_0                                  (17U)

/* Receive endpoints */
#define RVM_RCV_RECV1_PRC_PROC1                         (RVM_CID(RVM_MAIN_RCV_0, 0U))
#define RVM_RCV_RECV2_PRC_PROC1                         (RVM_CID(RVM_MAIN_RCV_0, 1U))
#define RVM_RCV_RECV1_PRC_PROC2                         (RVM_CID(RVM_MAIN_RCV_0, 2U))
#define RVM_RCV_RECV2_PRC_PROC2                         (RVM_CID(RVM_MAIN_RCV_0, 3U))

/* VM number */
#define RVM_VIRT_NUM                                    (11U)

/* Virtual endpoint frontiers & number */
#define RVM_BOOT_VEP_BEFORE                             (0x4B0U)
#define RVM_BOOT_VEP_AFTER                              (0x610U)
#define RVM_BOOT_VEP_MAIN_NUM                           (1U)
#define RVM_BOOT_VEP_CRT_NUM                            (11U)
#define RVM_BOOT_VCAP_INIT_NUM                          (11U)

/* Cpt frontiers & number */
#define RVM_BOOT_INIT_CPT_BEFORE                        (0x610U)
#define RVM_BOOT_INIT_CPT_AFTER                         (0x1010U)
#define RVM_BOOT_INIT_CPT_MAIN_NUM                      (1U)
#define RVM_BOOT_INIT_CPT_CRT_NUM                       (13U)
#define RVM_BOOT_INIT_CPT_INIT_NUM                      (32U)
#define RVM_BOOT_INIT_CPT_KFN_NUM                       (13U)

/* Pgt frontiers & number */
#define RVM_BOOT_INIT_PGT_BEFORE                        (0x1010U)
#define RVM_BOOT_INIT_PGT_AFTER                         (0x2920U)
#define RVM_BOOT_INIT_PGT_MAIN_NUM                      (1U)
#define RVM_BOOT_INIT_PGT_CRT_NUM                       (65U)
#define RVM_BOOT_INIT_PGT_CON_NUM                       (52U)
#define RVM_BOOT_INIT_PGT_ADD_NUM                       (77U)

/* Process frontiers & number */
#define RVM_BOOT_PRC_BEFORE                             (0x2920U)
#define RVM_BOOT_PRC_AFTER                              (0x2AC0U)
#define RVM_BOOT_PRC_MAIN_NUM                           (1U)
#define RVM_BOOT_PRC_CRT_NUM                            (13U)

/* Thread frontiers & number */
#define RVM_BOOT_THD_BEFORE                             (0x2AC0U)
#define RVM_BOOT_THD_AFTER                              (0x3A90U)
#define RVM_BOOT_THD_MAIN_NUM                           (1U)
#define RVM_BOOT_THD_CRT_NUM                            (26U)
#define RVM_BOOT_THD_INIT_NUM                           (26U)

/* Invocation frontiers & number */
#define RVM_BOOT_INV_BEFORE                             (0x3A90U)
#define RVM_BOOT_INV_AFTER                              (0x3AE0U)
#define RVM_BOOT_INV_MAIN_NUM                           (1U)
#define RVM_BOOT_INV_CRT_NUM                            (1U)
#define RVM_BOOT_INV_INIT_NUM                           (1U)

/* Receive endpoint frontiers & number */
#define RVM_BOOT_RCV_BEFORE                             (0x3AE0U)
#define RVM_BOOT_RCV_AFTER                              (0x3B60U)
#define RVM_BOOT_RCV_MAIN_NUM                           (1U)
#define RVM_BOOT_RCV_CRT_NUM                            (4U)

/* Code memory blocks */
#define RVM_PROC1_CODE_CODE1_BASE                       (0x80E0000U)
#define RVM_PROC1_CODE_CODE1_SIZE                       (0x2000U)
#define RVM_PROC2_CODE_CODE1_BASE                       (0x80E2000U)
#define RVM_PROC2_CODE_CODE1_SIZE                       (0x2000U)
#define RVM_VIRT_FRT_CODE_CODE1_BASE                    (0x80A0000U)
#define RVM_VIRT_FRT_CODE_CODE1_SIZE                    (0x8000U)
#define RVM_VIRT_UO3_CODE_CODE1_BASE                    (0x80A8000U)
#define RVM_VIRT_UO3_CODE_CODE1_SIZE                    (0x8000U)
#define RVM_VIRT_RTT_CODE_CODE1_BASE                    (0x80B0000U)
#define RVM_VIRT_RTT_CODE_CODE1_SIZE                    (0x8000U)
#define RVM_VIRT_UO2_CODE_CODE1_BASE                    (0x80B8000U)
#define RVM_VIRT_UO2_CODE_CODE1_SIZE                    (0x8000U)
#define RVM_VIRT_CHB_CODE_CODE1_BASE                    (0x80C0000U)
#define RVM_VIRT_CHB_CODE_CODE1_SIZE                    (0x8000U)
#define RVM_VIRT_RMP_CODE_CODE1_BASE                    (0x80C8000U)
#define RVM_VIRT_RMP_CODE_CODE1_SIZE                    (0x8000U)
#define RVM_VIRT_KLT_CODE_CODE1_BASE                    (0x80D0000U)
#define RVM_VIRT_KLT_CODE_CODE1_SIZE                    (0x8000U)
#define RVM_VIRT_DJY_CODE_CODE1_BASE                    (0x80D8000U)
#define RVM_VIRT_DJY_CODE_CODE1_SIZE                    (0x8000U)
#define RVM_VIRT_NTX_CODE_CODE1_BASE                    (0x8090000U)
#define RVM_VIRT_NTX_CODE_CODE1_SIZE                    (0x10000U)
#define RVM_VIRT_ZPR_CODE_CODE1_BASE                    (0x8030000U)
#define RVM_VIRT_ZPR_CODE_CODE1_SIZE                    (0x30000U)
#define RVM_VIRT_RTE_CODE_CODE1_BASE                    (0x8060000U)
#define RVM_VIRT_RTE_CODE_CODE1_SIZE                    (0x30000U)

/* Data memory blocks */
#define RVM_PROC1_DATA_DATA1_BASE                       (0x24076000U)
#define RVM_PROC1_DATA_DATA1_SIZE                       (0x2000U)
#define RVM_PROC2_DATA_DATA1_BASE                       (0x24078000U)
#define RVM_PROC2_DATA_DATA1_SIZE                       (0x2000U)
#define RVM_VIRT_FRT_DATA_DATA1_BASE                    (0x24018000U)
#define RVM_VIRT_FRT_DATA_DATA1_SIZE                    (0x8000U)
#define RVM_VIRT_UO3_DATA_DATA1_BASE                    (0x2404C000U)
#define RVM_VIRT_UO3_DATA_DATA1_SIZE                    (0x4000U)
#define RVM_VIRT_RTT_DATA_DATA1_BASE                    (0x2406E000U)
#define RVM_VIRT_RTT_DATA_DATA1_SIZE                    (0x4000U)
#define RVM_VIRT_UO2_DATA_DATA1_BASE                    (0x24072000U)
#define RVM_VIRT_UO2_DATA_DATA1_SIZE                    (0x4000U)
#define RVM_VIRT_CHB_DATA_DATA1_BASE                    (0x24044000U)
#define RVM_VIRT_CHB_DATA_DATA1_SIZE                    (0x8000U)
#define RVM_VIRT_RMP_DATA_DATA1_BASE                    (0x24060000U)
#define RVM_VIRT_RMP_DATA_DATA1_SIZE                    (0x8000U)
#define RVM_VIRT_KLT_DATA_DATA1_BASE                    (0x24068000U)
#define RVM_VIRT_KLT_DATA_DATA1_SIZE                    (0x6000U)
#define RVM_VIRT_DJY_DATA_DATA1_BASE                    (0x24034000U)
#define RVM_VIRT_DJY_DATA_DATA1_SIZE                    (0x10000U)
#define RVM_VIRT_NTX_DATA_DATA1_BASE                    (0x24050000U)
#define RVM_VIRT_NTX_DATA_DATA1_SIZE                    (0x10000U)
#define RVM_VIRT_ZPR_DATA_DATA1_BASE                    (0x24000000U)
#define RVM_VIRT_ZPR_DATA_DATA1_SIZE                    (0x18000U)
#define RVM_VIRT_RTE_DATA_DATA1_BASE                    (0x24020000U)
#define RVM_VIRT_RTE_DATA_DATA1_SIZE                    (0x14000U)
#define RVM_DATA_SHARED_SHARED1_BASE                    (0x2407A000U)
#define RVM_DATA_SHARED_SHARED1_SIZE                    (0x400U)

/* Device memory blocks */
#define RVM_DEVICE_SHARED_DEVICE1_BASE                  (0x40000000U)
#define RVM_DEVICE_SHARED_DEVICE1_SIZE                  (0x20000000U)
/* End Define ****************************************************************/

/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/

