/******************************************************************************
Filename    : rme_boot.h
Author      : The RVM project generator.
Date        : 15/11/2026 04:05:58
License     : Unlicense; see COPYING for details.
Description : The boot-time initialization file header.
******************************************************************************/

/* Define ********************************************************************/
/* Capability table maximum capacity */
#define RME_MAIN_CPT_SIZE                               (128U)
/* Boot-time capability table */
#define RME_BOOT_CPT_OBJ                                (RME_A7M_CPT)

/* Vector endpoint capability tables */
#define RME_MAIN_VCT_0                                  (7U)

/* Vector endpoints */
#define RVM_VCT_TIM4                                    (0U)

/* Receive endpoint capability tables - created by RVM later */
#define RME_BOOT_SIG_0                                  (17U)

/* Receive endpoints - created by RVM later */
#define RME_RCV_RECV1_PRC_PROC1                         (RME_CID(RME_BOOT_SIG_0,0U))
#define RME_RCV_RECV2_PRC_PROC1                         (RME_CID(RME_BOOT_SIG_0,1U))
#define RME_RCV_RECV1_PRC_PROC2                         (RME_CID(RME_BOOT_SIG_0,2U))
#define RME_RCV_RECV2_PRC_PROC2                         (RME_CID(RME_BOOT_SIG_0,3U))

/* Code memory blocks */
#define RME_PROC1_CODE_CODE1_BASE                       (0x80E0000U)
#define RME_PROC1_CODE_CODE1_SIZE                       (0x2000U)
#define RME_PROC2_CODE_CODE1_BASE                       (0x80E2000U)
#define RME_PROC2_CODE_CODE1_SIZE                       (0x2000U)
#define RME_VIRT_FRT_CODE_CODE1_BASE                    (0x80A0000U)
#define RME_VIRT_FRT_CODE_CODE1_SIZE                    (0x8000U)
#define RME_VIRT_UO3_CODE_CODE1_BASE                    (0x80A8000U)
#define RME_VIRT_UO3_CODE_CODE1_SIZE                    (0x8000U)
#define RME_VIRT_RTT_CODE_CODE1_BASE                    (0x80B0000U)
#define RME_VIRT_RTT_CODE_CODE1_SIZE                    (0x8000U)
#define RME_VIRT_UO2_CODE_CODE1_BASE                    (0x80B8000U)
#define RME_VIRT_UO2_CODE_CODE1_SIZE                    (0x8000U)
#define RME_VIRT_CHB_CODE_CODE1_BASE                    (0x80C0000U)
#define RME_VIRT_CHB_CODE_CODE1_SIZE                    (0x8000U)
#define RME_VIRT_RMP_CODE_CODE1_BASE                    (0x80C8000U)
#define RME_VIRT_RMP_CODE_CODE1_SIZE                    (0x8000U)
#define RME_VIRT_KLT_CODE_CODE1_BASE                    (0x80D0000U)
#define RME_VIRT_KLT_CODE_CODE1_SIZE                    (0x8000U)
#define RME_VIRT_DJY_CODE_CODE1_BASE                    (0x80D8000U)
#define RME_VIRT_DJY_CODE_CODE1_SIZE                    (0x8000U)
#define RME_VIRT_NTX_CODE_CODE1_BASE                    (0x8090000U)
#define RME_VIRT_NTX_CODE_CODE1_SIZE                    (0x10000U)
#define RME_VIRT_ZPR_CODE_CODE1_BASE                    (0x8030000U)
#define RME_VIRT_ZPR_CODE_CODE1_SIZE                    (0x30000U)
#define RME_VIRT_RTE_CODE_CODE1_BASE                    (0x8060000U)
#define RME_VIRT_RTE_CODE_CODE1_SIZE                    (0x30000U)

/* Data memory blocks */
#define RME_PROC1_DATA_DATA1_BASE                       (0x24076000U)
#define RME_PROC1_DATA_DATA1_SIZE                       (0x2000U)
#define RME_PROC2_DATA_DATA1_BASE                       (0x24078000U)
#define RME_PROC2_DATA_DATA1_SIZE                       (0x2000U)
#define RME_VIRT_FRT_DATA_DATA1_BASE                    (0x24018000U)
#define RME_VIRT_FRT_DATA_DATA1_SIZE                    (0x8000U)
#define RME_VIRT_UO3_DATA_DATA1_BASE                    (0x2404C000U)
#define RME_VIRT_UO3_DATA_DATA1_SIZE                    (0x4000U)
#define RME_VIRT_RTT_DATA_DATA1_BASE                    (0x2406E000U)
#define RME_VIRT_RTT_DATA_DATA1_SIZE                    (0x4000U)
#define RME_VIRT_UO2_DATA_DATA1_BASE                    (0x24072000U)
#define RME_VIRT_UO2_DATA_DATA1_SIZE                    (0x4000U)
#define RME_VIRT_CHB_DATA_DATA1_BASE                    (0x24044000U)
#define RME_VIRT_CHB_DATA_DATA1_SIZE                    (0x8000U)
#define RME_VIRT_RMP_DATA_DATA1_BASE                    (0x24060000U)
#define RME_VIRT_RMP_DATA_DATA1_SIZE                    (0x8000U)
#define RME_VIRT_KLT_DATA_DATA1_BASE                    (0x24068000U)
#define RME_VIRT_KLT_DATA_DATA1_SIZE                    (0x6000U)
#define RME_VIRT_DJY_DATA_DATA1_BASE                    (0x24034000U)
#define RME_VIRT_DJY_DATA_DATA1_SIZE                    (0x10000U)
#define RME_VIRT_NTX_DATA_DATA1_BASE                    (0x24050000U)
#define RME_VIRT_NTX_DATA_DATA1_SIZE                    (0x10000U)
#define RME_VIRT_ZPR_DATA_DATA1_BASE                    (0x24000000U)
#define RME_VIRT_ZPR_DATA_DATA1_SIZE                    (0x18000U)
#define RME_VIRT_RTE_DATA_DATA1_BASE                    (0x24020000U)
#define RME_VIRT_RTE_DATA_DATA1_SIZE                    (0x14000U)
#define RME_DATA_SHARED_SHARED1_BASE                    (0x2407A000U)
#define RME_DATA_SHARED_SHARED1_SIZE                    (0x400U)

/* Device memory blocks */
#define RME_DEVICE_SHARED_DEVICE1_BASE                  (0x40000000U)
#define RME_DEVICE_SHARED_DEVICE1_SIZE                  (0x20000000U)
/* End Define ****************************************************************/

/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/

