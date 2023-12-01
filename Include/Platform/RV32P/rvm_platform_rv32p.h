/******************************************************************************
Filename    : rvm_platform_rv32p.h
Author      : pry
Date        : 25/06/2017
Licence     : The Unlicense; see LICENSE for details.
Description : The header of the platform dependent part.
******************************************************************************/

/* Define ********************************************************************/
#ifdef __HDR_DEF__
#ifndef __RVM_PLATFORM_RV32P_DEF__
#define __RVM_PLATFORM_RV32P_DEF__
/*****************************************************************************/
/* Definitions of basic types */
/* Basic Types ***************************************************************/
#ifndef __RVM_S32_T__
#define __RVM_S32_T__
typedef signed int rvm_s32_t;
#endif

#ifndef __RVM_S16_T__
#define __RVM_S16_T__
typedef signed short rvm_s16_t;
#endif

#ifndef __RVM_S8_T__
#define __RVM_S8_T__
typedef signed char rvm_s8_t;
#endif

#ifndef __RVM_U32_T__
#define __RVM_U32_T__
typedef unsigned int rvm_u32_t;
#endif

#ifndef __RVM_U16_T__
#define __RVM_U16_T__
typedef unsigned short rvm_u16_t;
#endif

#ifndef __RVM_U8_T__
#define __RVM_U8_T__
typedef unsigned char rvm_u8_t;
#endif
/* End Basic Types ***********************************************************/
/* EXTERN keyword definition */
#define EXTERN                                      extern
/* The order of bits in one CPU machine word */
#define RVM_WORD_ORDER                              (5U)
/* Get most significant bit */
#define RVM_MSB_GET(VAL)                            _RVM_MSB_Generic(VAL)
/* FPU type definitions - keep in accordance with kernel */
#define RVM_RV32P_ATTR_NONE                         (0U)
#define RVM_RV32P_ATTR_RVF                          (1U<<0)
#define RVM_RV32P_ATTR_RVD                          (1U<<1)

/* Platform-specific includes */
#include "rvm_platform_rv32p_conf.h"

/* Thread size */
#define RVM_HYP_RAW_SIZE                            ((21U)*sizeof(rvm_ptr_t))
/* Invocation size */
#define RVM_INV_RAW_SIZE                            ((9U)*sizeof(rvm_ptr_t))
/* Normal page directory size */
#define RVM_PGT_RAW_SIZE_NOM(NUM_ORDER)             ((4U+RVM_POW2(NUM_ORDER))*sizeof(rvm_ptr_t))
/* Top-level page directory size */
#define RVM_PGT_RAW_SIZE_TOP(NUM_ORDER)             ((1U+2U*RVM_RV32P_REGION_NUM)*sizeof(rvm_ptr_t)+RVM_PGT_RAW_SIZE_NOM(NUM_ORDER))

#define RVM_RV32P_REG(X)                            (*((volatile rvm_ptr_t*)(X)))
#define RVM_RV32P_REGB(X)                           (*((volatile rvm_u8_t*)(X)))

/* Fault reasons */
/* Instruction misaligned */
#define RVM_RV32P_MCAUSE_IMALIGN                    (0x00000000U)
/* Instruction access fault */
#define RVM_RV32P_MCAUSE_IACCFLT                    (0x00000001U)
/* Undefined instruction */
#define RVM_RV32P_MCAUSE_IUNDEF                     (0x00000002U)
/* Breakpoint */
#define RVM_RV32P_MCAUSE_IBRKPT                     (0x00000003U)
/* Data load misaligned */
#define RVM_RV32P_MCAUSE_LALIGN                     (0x00000004U)
/* Data load access fault */
#define RVM_RV32P_MCAUSE_LACCFLT                    (0x00000005U)
/* Data store misaligned */
#define RVM_RV32P_MCAUSE_SALIGN                     (0x00000006U)
/* Data store access fault */
#define RVM_RV32P_MCAUSE_SACCFLT                    (0x00000007U)
/* System calls from user/supervisor/hypervisor/machine mode */
#define RVM_RV32P_MCAUSE_U_ECALL                    (0x00000008U)
#define RVM_RV32P_MCAUSE_S_ECALL                    (0x00000009U)
#define RVM_RV32P_MCAUSE_H_ECALL                    (0x0000000AU)
#define RVM_RV32P_MCAUSE_M_ECALL                    (0x0000000BU)
/* Instruction page fault */
#define RVM_RV32P_MCAUSE_IPGFLT                     (0x0000000CU)
/* Load page fault */
#define RVM_RV32P_MCAUSE_LPGFLT                     (0x0000000DU)
/* Store page fault */
#define RVM_RV32P_MCAUSE_SPGFLT                     (0x0000000FU)

/* Platform-specific kernel function macros **********************************/
/* Page table entry mode which property to get */
#define RVM_RV32P_KFN_PGT_ENTRY_MOD_FLAG_GET            (0U)
#define RVM_RV32P_KFN_PGT_ENTRY_MOD_SZORD_GET           (1U)
#define RVM_RV32P_KFN_PGT_ENTRY_MOD_NUMORD_GET          (2U)
/* Interrupt source configuration */
#define RVM_RV32P_KFN_INT_LOCAL_MOD_STATE_GET           (0U)
#define RVM_RV32P_KFN_INT_LOCAL_MOD_STATE_SET           (1U)
#define RVM_RV32P_KFN_INT_LOCAL_MOD_PRIO_GET            (2U)
#define RVM_RV32P_KFN_INT_LOCAL_MOD_PRIO_SET            (3U)
/* Prefetcher modification */
#define RVM_RV32P_KFN_PRFTH_MOD_STATE_GET               (0U)
#define RVM_RV32P_KFN_PRFTH_MOD_STATE_SET               (1U)
/* Prefetcher state */
#define RVM_RV32P_KFN_PRFTH_STATE_DISABLE               (0U)
#define RVM_RV32P_KFN_PRFTH_STATE_ENABLE                (1U)

/* Register read/write */
#define RVM_RV32P_KFN_DEBUG_REG_MOD_GET                 (0U)
#define RVM_RV32P_KFN_DEBUG_REG_MOD_SET                 (1U<<16U)
/* General-purpose registers */
#define RVM_RV32P_KFN_DEBUG_REG_MOD_MSTATUS             (0U)
#define RVM_RV32P_KFN_DEBUG_REG_MOD_PC                  (1U)
#define RVM_RV32P_KFN_DEBUG_REG_MOD_X1_RA               (2U)
#define RVM_RV32P_KFN_DEBUG_REG_MOD_X2_SP               (3U)
#define RVM_RV32P_KFN_DEBUG_REG_MOD_X3_GP               (4U)
#define RVM_RV32P_KFN_DEBUG_REG_MOD_X4_TP               (5U)
#define RVM_RV32P_KFN_DEBUG_REG_MOD_X5_T0               (6U)
#define RVM_RV32P_KFN_DEBUG_REG_MOD_X6_T1               (7U)
#define RVM_RV32P_KFN_DEBUG_REG_MOD_X7_T2               (8U)
#define RVM_RV32P_KFN_DEBUG_REG_MOD_X8_S0_FP            (9U)
#define RVM_RV32P_KFN_DEBUG_REG_MOD_X9_S1               (10U)
#define RVM_RV32P_KFN_DEBUG_REG_MOD_X10_A0              (11U)
#define RVM_RV32P_KFN_DEBUG_REG_MOD_X11_A1              (12U)
#define RVM_RV32P_KFN_DEBUG_REG_MOD_X12_A2              (13U)
#define RVM_RV32P_KFN_DEBUG_REG_MOD_X13_A3              (14U)
#define RVM_RV32P_KFN_DEBUG_REG_MOD_X14_A4              (15U)
#define RVM_RV32P_KFN_DEBUG_REG_MOD_X15_A5              (16U)
#define RVM_RV32P_KFN_DEBUG_REG_MOD_X16_A6              (17U)
#define RVM_RV32P_KFN_DEBUG_REG_MOD_X17_A7              (18U)
#define RVM_RV32P_KFN_DEBUG_REG_MOD_X18_S2              (19U)
#define RVM_RV32P_KFN_DEBUG_REG_MOD_X19_S3              (20U)
#define RVM_RV32P_KFN_DEBUG_REG_MOD_X20_S4              (21U)
#define RVM_RV32P_KFN_DEBUG_REG_MOD_X21_S5              (22U)
#define RVM_RV32P_KFN_DEBUG_REG_MOD_X22_S6              (23U)
#define RVM_RV32P_KFN_DEBUG_REG_MOD_X23_S7              (24U)
#define RVM_RV32P_KFN_DEBUG_REG_MOD_X24_S8              (25U)
#define RVM_RV32P_KFN_DEBUG_REG_MOD_X25_S9              (26U)
#define RVM_RV32P_KFN_DEBUG_REG_MOD_X26_S10             (27U)
#define RVM_RV32P_KFN_DEBUG_REG_MOD_X27_S11             (28U)
#define RVM_RV32P_KFN_DEBUG_REG_MOD_X28_T3              (29U)
#define RVM_RV32P_KFN_DEBUG_REG_MOD_X29_T4              (30U)
#define RVM_RV32P_KFN_DEBUG_REG_MOD_X30_T5              (31U)
#define RVM_RV32P_KFN_DEBUG_REG_MOD_X31_T6              (32U)
/* FCSR & FPU registers */
#define RVM_RV32P_KFN_DEBUG_REG_MOD_FCSR                (33U)
#define RVM_RV32P_KFN_DEBUG_REG_MOD_F0                  (34U)
#define RVM_RV32P_KFN_DEBUG_REG_MOD_F1                  (35U)
#define RVM_RV32P_KFN_DEBUG_REG_MOD_F2                  (36U)
#define RVM_RV32P_KFN_DEBUG_REG_MOD_F3                  (37U)
#define RVM_RV32P_KFN_DEBUG_REG_MOD_F4                  (38U)
#define RVM_RV32P_KFN_DEBUG_REG_MOD_F5                  (39U)
#define RVM_RV32P_KFN_DEBUG_REG_MOD_F6                  (40U)
#define RVM_RV32P_KFN_DEBUG_REG_MOD_F7                  (41U)
#define RVM_RV32P_KFN_DEBUG_REG_MOD_F8                  (42U)
#define RVM_RV32P_KFN_DEBUG_REG_MOD_F9                  (43U)
#define RVM_RV32P_KFN_DEBUG_REG_MOD_F10                 (44U)
#define RVM_RV32P_KFN_DEBUG_REG_MOD_F11                 (45U)
#define RVM_RV32P_KFN_DEBUG_REG_MOD_F12                 (46U)
#define RVM_RV32P_KFN_DEBUG_REG_MOD_F13                 (47U)
#define RVM_RV32P_KFN_DEBUG_REG_MOD_F14                 (48U)
#define RVM_RV32P_KFN_DEBUG_REG_MOD_F15                 (49U)
#define RVM_RV32P_KFN_DEBUG_REG_MOD_F16                 (50U)
#define RVM_RV32P_KFN_DEBUG_REG_MOD_F17                 (51U)
#define RVM_RV32P_KFN_DEBUG_REG_MOD_F18                 (52U)
#define RVM_RV32P_KFN_DEBUG_REG_MOD_F19                 (53U)
#define RVM_RV32P_KFN_DEBUG_REG_MOD_F20                 (54U)
#define RVM_RV32P_KFN_DEBUG_REG_MOD_F21                 (55U)
#define RVM_RV32P_KFN_DEBUG_REG_MOD_F22                 (56U)
#define RVM_RV32P_KFN_DEBUG_REG_MOD_F23                 (57U)
#define RVM_RV32P_KFN_DEBUG_REG_MOD_F24                 (58U)
#define RVM_RV32P_KFN_DEBUG_REG_MOD_F25                 (59U)
#define RVM_RV32P_KFN_DEBUG_REG_MOD_F26                 (60U)
#define RVM_RV32P_KFN_DEBUG_REG_MOD_F27                 (61U)
#define RVM_RV32P_KFN_DEBUG_REG_MOD_F28                 (62U)
#define RVM_RV32P_KFN_DEBUG_REG_MOD_F29                 (63U)
#define RVM_RV32P_KFN_DEBUG_REG_MOD_F30                 (64U)
#define RVM_RV32P_KFN_DEBUG_REG_MOD_F31                 (65U)
/* Getters */
#define RVM_RV32P_KFN_DEBUG_REG_MOD_MSTATUS_GET         (RVM_RV32P_KFN_DEBUG_REG_MOD_MSTATUS)
#define RVM_RV32P_KFN_DEBUG_REG_MOD_PC_GET              (RVM_RV32P_KFN_DEBUG_REG_MOD_PC)
#define RVM_RV32P_KFN_DEBUG_REG_MOD_X1_RA_GET           (RVM_RV32P_KFN_DEBUG_REG_MOD_X1_RA)
#define RVM_RV32P_KFN_DEBUG_REG_MOD_X2_SP_GET           (RVM_RV32P_KFN_DEBUG_REG_MOD_X2_SP)
#define RVM_RV32P_KFN_DEBUG_REG_MOD_X3_GP_GET           (RVM_RV32P_KFN_DEBUG_REG_MOD_X3_GP)
#define RVM_RV32P_KFN_DEBUG_REG_MOD_X4_TP_GET           (RVM_RV32P_KFN_DEBUG_REG_MOD_X4_TP)
#define RVM_RV32P_KFN_DEBUG_REG_MOD_X5_T0_GET           (RVM_RV32P_KFN_DEBUG_REG_MOD_X5_T0)
#define RVM_RV32P_KFN_DEBUG_REG_MOD_X6_T1_GET           (RVM_RV32P_KFN_DEBUG_REG_MOD_X6_T1)
#define RVM_RV32P_KFN_DEBUG_REG_MOD_X7_T2_GET           (RVM_RV32P_KFN_DEBUG_REG_MOD_X7_T2)
#define RVM_RV32P_KFN_DEBUG_REG_MOD_X8_S0_FP_GET        (RVM_RV32P_KFN_DEBUG_REG_MOD_X8_S0_FP)
#define RVM_RV32P_KFN_DEBUG_REG_MOD_X9_S1_GET           (RVM_RV32P_KFN_DEBUG_REG_MOD_X9_S1)
#define RVM_RV32P_KFN_DEBUG_REG_MOD_X10_A0_GET          (RVM_RV32P_KFN_DEBUG_REG_MOD_X10_A0)
#define RVM_RV32P_KFN_DEBUG_REG_MOD_X11_A1_GET          (RVM_RV32P_KFN_DEBUG_REG_MOD_X11_A1)
#define RVM_RV32P_KFN_DEBUG_REG_MOD_X12_A2_GET          (RVM_RV32P_KFN_DEBUG_REG_MOD_X12_A2)
#define RVM_RV32P_KFN_DEBUG_REG_MOD_X13_A3_GET          (RVM_RV32P_KFN_DEBUG_REG_MOD_X13_A3)
#define RVM_RV32P_KFN_DEBUG_REG_MOD_X14_A4_GET          (RVM_RV32P_KFN_DEBUG_REG_MOD_X14_A4)
#define RVM_RV32P_KFN_DEBUG_REG_MOD_X15_A5_GET          (RVM_RV32P_KFN_DEBUG_REG_MOD_X15_A5)
#define RVM_RV32P_KFN_DEBUG_REG_MOD_X16_A6_GET          (RVM_RV32P_KFN_DEBUG_REG_MOD_X16_A6)
#define RVM_RV32P_KFN_DEBUG_REG_MOD_X17_A7_GET          (RVM_RV32P_KFN_DEBUG_REG_MOD_X17_A7)
#define RVM_RV32P_KFN_DEBUG_REG_MOD_X18_S2_GET          (RVM_RV32P_KFN_DEBUG_REG_MOD_X18_S2)
#define RVM_RV32P_KFN_DEBUG_REG_MOD_X19_S3_GET          (RVM_RV32P_KFN_DEBUG_REG_MOD_X19_S3)
#define RVM_RV32P_KFN_DEBUG_REG_MOD_X20_S4_GET          (RVM_RV32P_KFN_DEBUG_REG_MOD_X20_S4)
#define RVM_RV32P_KFN_DEBUG_REG_MOD_X21_S5_GET          (RVM_RV32P_KFN_DEBUG_REG_MOD_X21_S5)
#define RVM_RV32P_KFN_DEBUG_REG_MOD_X22_S6_GET          (RVM_RV32P_KFN_DEBUG_REG_MOD_X22_S6)
#define RVM_RV32P_KFN_DEBUG_REG_MOD_X23_S7_GET          (RVM_RV32P_KFN_DEBUG_REG_MOD_X23_S7)
#define RVM_RV32P_KFN_DEBUG_REG_MOD_X24_S8_GET          (RVM_RV32P_KFN_DEBUG_REG_MOD_X24_S8)
#define RVM_RV32P_KFN_DEBUG_REG_MOD_X25_S9_GET          (RVM_RV32P_KFN_DEBUG_REG_MOD_X25_S9)
#define RVM_RV32P_KFN_DEBUG_REG_MOD_X26_S10_GET         (RVM_RV32P_KFN_DEBUG_REG_MOD_X26_S10)
#define RVM_RV32P_KFN_DEBUG_REG_MOD_X27_S11_GET         (RVM_RV32P_KFN_DEBUG_REG_MOD_X27_S11)
#define RVM_RV32P_KFN_DEBUG_REG_MOD_X28_T3_GET          (RVM_RV32P_KFN_DEBUG_REG_MOD_X28_T3)
#define RVM_RV32P_KFN_DEBUG_REG_MOD_X29_T4_GET          (RVM_RV32P_KFN_DEBUG_REG_MOD_X29_T4)
#define RVM_RV32P_KFN_DEBUG_REG_MOD_X30_T5_GET          (RVM_RV32P_KFN_DEBUG_REG_MOD_X30_T5)
#define RVM_RV32P_KFN_DEBUG_REG_MOD_X31_T6_GET          (RVM_RV32P_KFN_DEBUG_REG_MOD_X31_T6)
#define RVM_RV32P_KFN_DEBUG_REG_MOD_FCSR_GET            (RVM_RV32P_KFN_DEBUG_REG_MOD_FCSR)
#define RVM_RV32P_KFN_DEBUG_REG_MOD_F0_GET              (RVM_RV32P_KFN_DEBUG_REG_MOD_F0)
#define RVM_RV32P_KFN_DEBUG_REG_MOD_F1_GET              (RVM_RV32P_KFN_DEBUG_REG_MOD_F1)
#define RVM_RV32P_KFN_DEBUG_REG_MOD_F2_GET              (RVM_RV32P_KFN_DEBUG_REG_MOD_F2)
#define RVM_RV32P_KFN_DEBUG_REG_MOD_F3_GET              (RVM_RV32P_KFN_DEBUG_REG_MOD_F3)
#define RVM_RV32P_KFN_DEBUG_REG_MOD_F4_GET              (RVM_RV32P_KFN_DEBUG_REG_MOD_F4)
#define RVM_RV32P_KFN_DEBUG_REG_MOD_F5_GET              (RVM_RV32P_KFN_DEBUG_REG_MOD_F5)
#define RVM_RV32P_KFN_DEBUG_REG_MOD_F6_GET              (RVM_RV32P_KFN_DEBUG_REG_MOD_F6)
#define RVM_RV32P_KFN_DEBUG_REG_MOD_F7_GET              (RVM_RV32P_KFN_DEBUG_REG_MOD_F7)
#define RVM_RV32P_KFN_DEBUG_REG_MOD_F8_GET              (RVM_RV32P_KFN_DEBUG_REG_MOD_F8)
#define RVM_RV32P_KFN_DEBUG_REG_MOD_F9_GET              (RVM_RV32P_KFN_DEBUG_REG_MOD_F9)
#define RVM_RV32P_KFN_DEBUG_REG_MOD_F10_GET             (RVM_RV32P_KFN_DEBUG_REG_MOD_F10)
#define RVM_RV32P_KFN_DEBUG_REG_MOD_F11_GET             (RVM_RV32P_KFN_DEBUG_REG_MOD_F11)
#define RVM_RV32P_KFN_DEBUG_REG_MOD_F12_GET             (RVM_RV32P_KFN_DEBUG_REG_MOD_F12)
#define RVM_RV32P_KFN_DEBUG_REG_MOD_F13_GET             (RVM_RV32P_KFN_DEBUG_REG_MOD_F13)
#define RVM_RV32P_KFN_DEBUG_REG_MOD_F14_GET             (RVM_RV32P_KFN_DEBUG_REG_MOD_F14)
#define RVM_RV32P_KFN_DEBUG_REG_MOD_F15_GET             (RVM_RV32P_KFN_DEBUG_REG_MOD_F15)
#define RVM_RV32P_KFN_DEBUG_REG_MOD_F16_GET             (RVM_RV32P_KFN_DEBUG_REG_MOD_F16)
#define RVM_RV32P_KFN_DEBUG_REG_MOD_F17_GET             (RVM_RV32P_KFN_DEBUG_REG_MOD_F17)
#define RVM_RV32P_KFN_DEBUG_REG_MOD_F18_GET             (RVM_RV32P_KFN_DEBUG_REG_MOD_F18)
#define RVM_RV32P_KFN_DEBUG_REG_MOD_F19_GET             (RVM_RV32P_KFN_DEBUG_REG_MOD_F19)
#define RVM_RV32P_KFN_DEBUG_REG_MOD_F20_GET             (RVM_RV32P_KFN_DEBUG_REG_MOD_F20)
#define RVM_RV32P_KFN_DEBUG_REG_MOD_F21_GET             (RVM_RV32P_KFN_DEBUG_REG_MOD_F21)
#define RVM_RV32P_KFN_DEBUG_REG_MOD_F22_GET             (RVM_RV32P_KFN_DEBUG_REG_MOD_F22)
#define RVM_RV32P_KFN_DEBUG_REG_MOD_F23_GET             (RVM_RV32P_KFN_DEBUG_REG_MOD_F23)
#define RVM_RV32P_KFN_DEBUG_REG_MOD_F24_GET             (RVM_RV32P_KFN_DEBUG_REG_MOD_F24)
#define RVM_RV32P_KFN_DEBUG_REG_MOD_F25_GET             (RVM_RV32P_KFN_DEBUG_REG_MOD_F25)
#define RVM_RV32P_KFN_DEBUG_REG_MOD_F26_GET             (RVM_RV32P_KFN_DEBUG_REG_MOD_F26)
#define RVM_RV32P_KFN_DEBUG_REG_MOD_F27_GET             (RVM_RV32P_KFN_DEBUG_REG_MOD_F27)
#define RVM_RV32P_KFN_DEBUG_REG_MOD_F28_GET             (RVM_RV32P_KFN_DEBUG_REG_MOD_F28)
#define RVM_RV32P_KFN_DEBUG_REG_MOD_F29_GET             (RVM_RV32P_KFN_DEBUG_REG_MOD_F29)
#define RVM_RV32P_KFN_DEBUG_REG_MOD_F30_GET             (RVM_RV32P_KFN_DEBUG_REG_MOD_F30)
#define RVM_RV32P_KFN_DEBUG_REG_MOD_F31_GET             (RVM_RV32P_KFN_DEBUG_REG_MOD_F31)
/* Setters */
#define RVM_RV32P_KFN_DEBUG_REG_MOD_MSTATUS_SET         (RVM_RV32P_KFN_DEBUG_REG_MOD_SET|RVM_RV32P_KFN_DEBUG_REG_MOD_MSTATUS)
#define RVM_RV32P_KFN_DEBUG_REG_MOD_PC_SET              (RVM_RV32P_KFN_DEBUG_REG_MOD_SET|RVM_RV32P_KFN_DEBUG_REG_MOD_PC)
#define RVM_RV32P_KFN_DEBUG_REG_MOD_X1_RA_SET           (RVM_RV32P_KFN_DEBUG_REG_MOD_SET|RVM_RV32P_KFN_DEBUG_REG_MOD_X1_RA)
#define RVM_RV32P_KFN_DEBUG_REG_MOD_X2_SP_SET           (RVM_RV32P_KFN_DEBUG_REG_MOD_SET|RVM_RV32P_KFN_DEBUG_REG_MOD_X2_SP)
#define RVM_RV32P_KFN_DEBUG_REG_MOD_X3_GP_SET           (RVM_RV32P_KFN_DEBUG_REG_MOD_SET|RVM_RV32P_KFN_DEBUG_REG_MOD_X3_GP)
#define RVM_RV32P_KFN_DEBUG_REG_MOD_X4_TP_SET           (RVM_RV32P_KFN_DEBUG_REG_MOD_SET|RVM_RV32P_KFN_DEBUG_REG_MOD_X4_TP)
#define RVM_RV32P_KFN_DEBUG_REG_MOD_X5_T0_SET           (RVM_RV32P_KFN_DEBUG_REG_MOD_SET|RVM_RV32P_KFN_DEBUG_REG_MOD_X5_T0)
#define RVM_RV32P_KFN_DEBUG_REG_MOD_X6_T1_SET           (RVM_RV32P_KFN_DEBUG_REG_MOD_SET|RVM_RV32P_KFN_DEBUG_REG_MOD_X6_T1)
#define RVM_RV32P_KFN_DEBUG_REG_MOD_X7_T2_SET           (RVM_RV32P_KFN_DEBUG_REG_MOD_SET|RVM_RV32P_KFN_DEBUG_REG_MOD_X7_T2)
#define RVM_RV32P_KFN_DEBUG_REG_MOD_X8_S0_FP_SET        (RVM_RV32P_KFN_DEBUG_REG_MOD_SET|RVM_RV32P_KFN_DEBUG_REG_MOD_X8_S0_FP)
#define RVM_RV32P_KFN_DEBUG_REG_MOD_X9_S1_SET           (RVM_RV32P_KFN_DEBUG_REG_MOD_SET|RVM_RV32P_KFN_DEBUG_REG_MOD_X9_S1)
#define RVM_RV32P_KFN_DEBUG_REG_MOD_X10_A0_SET          (RVM_RV32P_KFN_DEBUG_REG_MOD_SET|RVM_RV32P_KFN_DEBUG_REG_MOD_X10_A0)
#define RVM_RV32P_KFN_DEBUG_REG_MOD_X11_A1_SET          (RVM_RV32P_KFN_DEBUG_REG_MOD_SET|RVM_RV32P_KFN_DEBUG_REG_MOD_X11_A1)
#define RVM_RV32P_KFN_DEBUG_REG_MOD_X12_A2_SET          (RVM_RV32P_KFN_DEBUG_REG_MOD_SET|RVM_RV32P_KFN_DEBUG_REG_MOD_X12_A2)
#define RVM_RV32P_KFN_DEBUG_REG_MOD_X13_A3_SET          (RVM_RV32P_KFN_DEBUG_REG_MOD_SET|RVM_RV32P_KFN_DEBUG_REG_MOD_X13_A3)
#define RVM_RV32P_KFN_DEBUG_REG_MOD_X14_A4_SET          (RVM_RV32P_KFN_DEBUG_REG_MOD_SET|RVM_RV32P_KFN_DEBUG_REG_MOD_X14_A4)
#define RVM_RV32P_KFN_DEBUG_REG_MOD_X15_A5_SET          (RVM_RV32P_KFN_DEBUG_REG_MOD_SET|RVM_RV32P_KFN_DEBUG_REG_MOD_X15_A5)
#define RVM_RV32P_KFN_DEBUG_REG_MOD_X16_A6_SET          (RVM_RV32P_KFN_DEBUG_REG_MOD_SET|RVM_RV32P_KFN_DEBUG_REG_MOD_X16_A6)
#define RVM_RV32P_KFN_DEBUG_REG_MOD_X17_A7_SET          (RVM_RV32P_KFN_DEBUG_REG_MOD_SET|RVM_RV32P_KFN_DEBUG_REG_MOD_X17_A7)
#define RVM_RV32P_KFN_DEBUG_REG_MOD_X18_S2_SET          (RVM_RV32P_KFN_DEBUG_REG_MOD_SET|RVM_RV32P_KFN_DEBUG_REG_MOD_X18_S2)
#define RVM_RV32P_KFN_DEBUG_REG_MOD_X19_S3_SET          (RVM_RV32P_KFN_DEBUG_REG_MOD_SET|RVM_RV32P_KFN_DEBUG_REG_MOD_X19_S3)
#define RVM_RV32P_KFN_DEBUG_REG_MOD_X20_S4_SET          (RVM_RV32P_KFN_DEBUG_REG_MOD_SET|RVM_RV32P_KFN_DEBUG_REG_MOD_X20_S4)
#define RVM_RV32P_KFN_DEBUG_REG_MOD_X21_S5_SET          (RVM_RV32P_KFN_DEBUG_REG_MOD_SET|RVM_RV32P_KFN_DEBUG_REG_MOD_X21_S5)
#define RVM_RV32P_KFN_DEBUG_REG_MOD_X22_S6_SET          (RVM_RV32P_KFN_DEBUG_REG_MOD_SET|RVM_RV32P_KFN_DEBUG_REG_MOD_X22_S6)
#define RVM_RV32P_KFN_DEBUG_REG_MOD_X23_S7_SET          (RVM_RV32P_KFN_DEBUG_REG_MOD_SET|RVM_RV32P_KFN_DEBUG_REG_MOD_X23_S7)
#define RVM_RV32P_KFN_DEBUG_REG_MOD_X24_S8_SET          (RVM_RV32P_KFN_DEBUG_REG_MOD_SET|RVM_RV32P_KFN_DEBUG_REG_MOD_X24_S8)
#define RVM_RV32P_KFN_DEBUG_REG_MOD_X25_S9_SET          (RVM_RV32P_KFN_DEBUG_REG_MOD_SET|RVM_RV32P_KFN_DEBUG_REG_MOD_X25_S9)
#define RVM_RV32P_KFN_DEBUG_REG_MOD_X26_S10_SET         (RVM_RV32P_KFN_DEBUG_REG_MOD_SET|RVM_RV32P_KFN_DEBUG_REG_MOD_X26_S10)
#define RVM_RV32P_KFN_DEBUG_REG_MOD_X27_S11_SET         (RVM_RV32P_KFN_DEBUG_REG_MOD_SET|RVM_RV32P_KFN_DEBUG_REG_MOD_X27_S11)
#define RVM_RV32P_KFN_DEBUG_REG_MOD_X28_T3_SET          (RVM_RV32P_KFN_DEBUG_REG_MOD_SET|RVM_RV32P_KFN_DEBUG_REG_MOD_X28_T3)
#define RVM_RV32P_KFN_DEBUG_REG_MOD_X29_T4_SET          (RVM_RV32P_KFN_DEBUG_REG_MOD_SET|RVM_RV32P_KFN_DEBUG_REG_MOD_X29_T4)
#define RVM_RV32P_KFN_DEBUG_REG_MOD_X30_T5_SET          (RVM_RV32P_KFN_DEBUG_REG_MOD_SET|RVM_RV32P_KFN_DEBUG_REG_MOD_X30_T5)
#define RVM_RV32P_KFN_DEBUG_REG_MOD_X31_T6_SET          (RVM_RV32P_KFN_DEBUG_REG_MOD_SET|RVM_RV32P_KFN_DEBUG_REG_MOD_X31_T6)
#define RVM_RV32P_KFN_DEBUG_REG_MOD_FCSR_SET            (RVM_RV32P_KFN_DEBUG_REG_MOD_SET|RVM_RV32P_KFN_DEBUG_REG_MOD_FCSR)
#define RVM_RV32P_KFN_DEBUG_REG_MOD_F0_SET              (RVM_RV32P_KFN_DEBUG_REG_MOD_SET|RVM_RV32P_KFN_DEBUG_REG_MOD_F0)
#define RVM_RV32P_KFN_DEBUG_REG_MOD_F1_SET              (RVM_RV32P_KFN_DEBUG_REG_MOD_SET|RVM_RV32P_KFN_DEBUG_REG_MOD_F1)
#define RVM_RV32P_KFN_DEBUG_REG_MOD_F2_SET              (RVM_RV32P_KFN_DEBUG_REG_MOD_SET|RVM_RV32P_KFN_DEBUG_REG_MOD_F2)
#define RVM_RV32P_KFN_DEBUG_REG_MOD_F3_SET              (RVM_RV32P_KFN_DEBUG_REG_MOD_SET|RVM_RV32P_KFN_DEBUG_REG_MOD_F3)
#define RVM_RV32P_KFN_DEBUG_REG_MOD_F4_SET              (RVM_RV32P_KFN_DEBUG_REG_MOD_SET|RVM_RV32P_KFN_DEBUG_REG_MOD_F4)
#define RVM_RV32P_KFN_DEBUG_REG_MOD_F5_SET              (RVM_RV32P_KFN_DEBUG_REG_MOD_SET|RVM_RV32P_KFN_DEBUG_REG_MOD_F5)
#define RVM_RV32P_KFN_DEBUG_REG_MOD_F6_SET              (RVM_RV32P_KFN_DEBUG_REG_MOD_SET|RVM_RV32P_KFN_DEBUG_REG_MOD_F6)
#define RVM_RV32P_KFN_DEBUG_REG_MOD_F7_SET              (RVM_RV32P_KFN_DEBUG_REG_MOD_SET|RVM_RV32P_KFN_DEBUG_REG_MOD_F7)
#define RVM_RV32P_KFN_DEBUG_REG_MOD_F8_SET              (RVM_RV32P_KFN_DEBUG_REG_MOD_SET|RVM_RV32P_KFN_DEBUG_REG_MOD_F8)
#define RVM_RV32P_KFN_DEBUG_REG_MOD_F9_SET              (RVM_RV32P_KFN_DEBUG_REG_MOD_SET|RVM_RV32P_KFN_DEBUG_REG_MOD_F9)
#define RVM_RV32P_KFN_DEBUG_REG_MOD_F10_SET             (RVM_RV32P_KFN_DEBUG_REG_MOD_SET|RVM_RV32P_KFN_DEBUG_REG_MOD_F10)
#define RVM_RV32P_KFN_DEBUG_REG_MOD_F11_SET             (RVM_RV32P_KFN_DEBUG_REG_MOD_SET|RVM_RV32P_KFN_DEBUG_REG_MOD_F11)
#define RVM_RV32P_KFN_DEBUG_REG_MOD_F12_SET             (RVM_RV32P_KFN_DEBUG_REG_MOD_SET|RVM_RV32P_KFN_DEBUG_REG_MOD_F12)
#define RVM_RV32P_KFN_DEBUG_REG_MOD_F13_SET             (RVM_RV32P_KFN_DEBUG_REG_MOD_SET|RVM_RV32P_KFN_DEBUG_REG_MOD_F13)
#define RVM_RV32P_KFN_DEBUG_REG_MOD_F14_SET             (RVM_RV32P_KFN_DEBUG_REG_MOD_SET|RVM_RV32P_KFN_DEBUG_REG_MOD_F14)
#define RVM_RV32P_KFN_DEBUG_REG_MOD_F15_SET             (RVM_RV32P_KFN_DEBUG_REG_MOD_SET|RVM_RV32P_KFN_DEBUG_REG_MOD_F15)
#define RVM_RV32P_KFN_DEBUG_REG_MOD_F16_SET             (RVM_RV32P_KFN_DEBUG_REG_MOD_SET|RVM_RV32P_KFN_DEBUG_REG_MOD_F16)
#define RVM_RV32P_KFN_DEBUG_REG_MOD_F17_SET             (RVM_RV32P_KFN_DEBUG_REG_MOD_SET|RVM_RV32P_KFN_DEBUG_REG_MOD_F17)
#define RVM_RV32P_KFN_DEBUG_REG_MOD_F18_SET             (RVM_RV32P_KFN_DEBUG_REG_MOD_SET|RVM_RV32P_KFN_DEBUG_REG_MOD_F18)
#define RVM_RV32P_KFN_DEBUG_REG_MOD_F19_SET             (RVM_RV32P_KFN_DEBUG_REG_MOD_SET|RVM_RV32P_KFN_DEBUG_REG_MOD_F19)
#define RVM_RV32P_KFN_DEBUG_REG_MOD_F20_SET             (RVM_RV32P_KFN_DEBUG_REG_MOD_SET|RVM_RV32P_KFN_DEBUG_REG_MOD_F20)
#define RVM_RV32P_KFN_DEBUG_REG_MOD_F21_SET             (RVM_RV32P_KFN_DEBUG_REG_MOD_SET|RVM_RV32P_KFN_DEBUG_REG_MOD_F21)
#define RVM_RV32P_KFN_DEBUG_REG_MOD_F22_SET             (RVM_RV32P_KFN_DEBUG_REG_MOD_SET|RVM_RV32P_KFN_DEBUG_REG_MOD_F22)
#define RVM_RV32P_KFN_DEBUG_REG_MOD_F23_SET             (RVM_RV32P_KFN_DEBUG_REG_MOD_SET|RVM_RV32P_KFN_DEBUG_REG_MOD_F23)
#define RVM_RV32P_KFN_DEBUG_REG_MOD_F24_SET             (RVM_RV32P_KFN_DEBUG_REG_MOD_SET|RVM_RV32P_KFN_DEBUG_REG_MOD_F24)
#define RVM_RV32P_KFN_DEBUG_REG_MOD_F25_SET             (RVM_RV32P_KFN_DEBUG_REG_MOD_SET|RVM_RV32P_KFN_DEBUG_REG_MOD_F25)
#define RVM_RV32P_KFN_DEBUG_REG_MOD_F26_SET             (RVM_RV32P_KFN_DEBUG_REG_MOD_SET|RVM_RV32P_KFN_DEBUG_REG_MOD_F26)
#define RVM_RV32P_KFN_DEBUG_REG_MOD_F27_SET             (RVM_RV32P_KFN_DEBUG_REG_MOD_SET|RVM_RV32P_KFN_DEBUG_REG_MOD_F27)
#define RVM_RV32P_KFN_DEBUG_REG_MOD_F28_SET             (RVM_RV32P_KFN_DEBUG_REG_MOD_SET|RVM_RV32P_KFN_DEBUG_REG_MOD_F28)
#define RVM_RV32P_KFN_DEBUG_REG_MOD_F29_SET             (RVM_RV32P_KFN_DEBUG_REG_MOD_SET|RVM_RV32P_KFN_DEBUG_REG_MOD_F29)
#define RVM_RV32P_KFN_DEBUG_REG_MOD_F30_SET             (RVM_RV32P_KFN_DEBUG_REG_MOD_SET|RVM_RV32P_KFN_DEBUG_REG_MOD_F30)
#define RVM_RV32P_KFN_DEBUG_REG_MOD_F31_SET             (RVM_RV32P_KFN_DEBUG_REG_MOD_SET|RVM_RV32P_KFN_DEBUG_REG_MOD_F31)

/* Invocation register read/write */
#define RVM_RV32P_KFN_DEBUG_INV_MOD_SP_GET              (0U)
#define RVM_RV32P_KFN_DEBUG_INV_MOD_SP_SET              (1U)
#define RVM_RV32P_KFN_DEBUG_INV_MOD_PC_GET              (2U)
#define RVM_RV32P_KFN_DEBUG_INV_MOD_PC_SET              (3U)

/* Exception register read */
#define RVM_RV32P_KFN_DEBUG_EXC_CAUSE_GET               (0U)
#define RVM_RV32P_KFN_DEBUG_EXC_ADDR_GET                (1U)
#define RVM_RV32P_KFN_DEBUG_EXC_VALUE_GET               (2U)
/* Extended Types ************************************************************/
#ifndef __RVM_TID_T__
#define __RVM_TID_T__
/* The typedef for the Thread ID */
typedef rvm_s32_t rvm_tid_t;
#endif

#ifndef __RVM_PTR_T__
#define __RVM_PTR_T__
/* The typedef for the pointers - This is the raw style. Pointers must be unsigned */
typedef rvm_u32_t rvm_ptr_t;
#endif

#ifndef __RVM_CNT_T__
#define __RVM_CNT_T__
/* The typedef for the count variables */
typedef rvm_s32_t rvm_cnt_t;
#endif

#ifndef __RVM_CID_T__
#define __RVM_CID_T__
/* The typedef for capability ID */
typedef rvm_s32_t rvm_cid_t;
#endif

#ifndef __RVM_RET_T__
#define __RVM_RET_T__
/* The type for process return value */
typedef rvm_s32_t rvm_ret_t;
#endif
/* End Extended Types ********************************************************/
/*****************************************************************************/
/* __RVM_PLATFORM_RV32P_DEF__ */
#endif
/* __HDR_DEF__ */
#endif
/* End Define ****************************************************************/

/* Struct ********************************************************************/
#ifdef __HDR_STRUCT__
#ifndef __RVM_PLATFORM_RV32P_STRUCT__
#define __RVM_PLATFORM_RV32P_STRUCT__
/* We used structs in the header */

/* Use defines in these headers */
#define __HDR_DEF__
#undef __HDR_DEF__
/*****************************************************************************/
struct RVM_RV32P_Stack
{
    /* Only PC is placed onto stack */
    rvm_ptr_t PC;
};

/* The register set struct */
struct RVM_Reg_Struct
{
    rvm_ptr_t MSTATUS;
    rvm_ptr_t PC;
    rvm_ptr_t X1_RA;
    rvm_ptr_t X2_SP;
    rvm_ptr_t X3_GP;
    rvm_ptr_t X4_TP;
    rvm_ptr_t X5_T0;
    rvm_ptr_t X6_T1;
    rvm_ptr_t X7_T2;
    rvm_ptr_t X8_S0_FP;
    rvm_ptr_t X9_S1;
    rvm_ptr_t X10_A0;
    rvm_ptr_t X11_A1;
    rvm_ptr_t X12_A2;
    rvm_ptr_t X13_A3;
    rvm_ptr_t X14_A4;
    rvm_ptr_t X15_A5;
    rvm_ptr_t X16_A6;
    rvm_ptr_t X17_A7;
    rvm_ptr_t X18_S2;
    rvm_ptr_t X19_S3;
    rvm_ptr_t X20_S4;
    rvm_ptr_t X21_S5;
    rvm_ptr_t X22_S6;
    rvm_ptr_t X23_S7;
    rvm_ptr_t X24_S8;
    rvm_ptr_t X25_S9;
    rvm_ptr_t X26_S10;
    rvm_ptr_t X27_S11;
    rvm_ptr_t X28_T3;
    rvm_ptr_t X29_T4;
    rvm_ptr_t X30_T5;
    rvm_ptr_t X31_T6;
};

#if(RVM_COP_NUM!=0U)
#if(RVM_RV32P_COP_RVD==0U)
/* Single-precision register set */
struct RVM_RV32P_Cop_Struct
{
    rvm_ptr_t FCSR;
    rvm_ptr_t F0;
    rvm_ptr_t F1;
    rvm_ptr_t F2;
    rvm_ptr_t F3;
    rvm_ptr_t F4;
    rvm_ptr_t F5;
    rvm_ptr_t F6;
    rvm_ptr_t F7;
    rvm_ptr_t F8;
    rvm_ptr_t F9;
    rvm_ptr_t F10;
    rvm_ptr_t F11;
    rvm_ptr_t F12;
    rvm_ptr_t F13;
    rvm_ptr_t F14;
    rvm_ptr_t F15;
    rvm_ptr_t F16;
    rvm_ptr_t F17;
    rvm_ptr_t F18;
    rvm_ptr_t F19;
    rvm_ptr_t F20;
    rvm_ptr_t F21;
    rvm_ptr_t F22;
    rvm_ptr_t F23;
    rvm_ptr_t F24;
    rvm_ptr_t F25;
    rvm_ptr_t F26;
    rvm_ptr_t F27;
    rvm_ptr_t F28;
    rvm_ptr_t F29;
    rvm_ptr_t F30;
    rvm_ptr_t F31;
};
#else
/* Double-precision register set */
struct RVM_RV32P_Cop_Struct
{
    rvm_ptr_t FCSR;
    rvm_ptr_t F0[2];
    rvm_ptr_t F1[2];
    rvm_ptr_t F2[2];
    rvm_ptr_t F3[2];
    rvm_ptr_t F4[2];
    rvm_ptr_t F5[2];
    rvm_ptr_t F6[2];
    rvm_ptr_t F7[2];
    rvm_ptr_t F8[2];
    rvm_ptr_t F9[2];
    rvm_ptr_t F10[2];
    rvm_ptr_t F11[2];
    rvm_ptr_t F12[2];
    rvm_ptr_t F13[2];
    rvm_ptr_t F14[2];
    rvm_ptr_t F15[2];
    rvm_ptr_t F16[2];
    rvm_ptr_t F17[2];
    rvm_ptr_t F18[2];
    rvm_ptr_t F19[2];
    rvm_ptr_t F20[2];
    rvm_ptr_t F21[2];
    rvm_ptr_t F22[2];
    rvm_ptr_t F23[2];
    rvm_ptr_t F24[2];
    rvm_ptr_t F25[2];
    rvm_ptr_t F26[2];
    rvm_ptr_t F27[2];
    rvm_ptr_t F28[2];
    rvm_ptr_t F29[2];
    rvm_ptr_t F30[2];
    rvm_ptr_t F31[2];
};
#endif
#endif

struct RVM_Exc_Struct
{
    rvm_ptr_t Cause;
    rvm_ptr_t Addr;
    rvm_ptr_t Value;
};
/*****************************************************************************/
/* __RVM_PLATFORM_RV32P_STRUCT__ */
#endif
/* __HDR_STRUCT__ */
#endif
/* End Struct ****************************************************************/

/* Private Variable **********************************************************/
#if(!(defined __HDR_DEF__||defined __HDR_STRUCT__))
#ifndef __RVM_PLATFORM_RV32P_MEMBER__
#define __RVM_PLATFORM_RV32P_MEMBER__

/* In this way we can use the data structures and definitions in the headers */
#define __HDR_DEF__

#undef __HDR_DEF__

#define __HDR_STRUCT__

#undef __HDR_STRUCT__

/* If the header is not used in the public mode */
#ifndef __HDR_PUBLIC__
/*****************************************************************************/

/*****************************************************************************/
/* End Private Variable ******************************************************/

/* Private Function **********************************************************/ 
/*****************************************************************************/

/*****************************************************************************/
#define __EXTERN__
/* End Private Function ******************************************************/

/* Public Variable ***********************************************************/
/* __HDR_PUBLIC__ */
#else
#define __EXTERN__ EXTERN 
/* __HDR_PUBLIC__ */
#endif

/*****************************************************************************/

/*****************************************************************************/

/* End Public Variable *******************************************************/

/* Public Function ***********************************************************/
/*****************************************************************************/
/* Assembly */
EXTERN void __RVM_Stub(void);

/* System call */
EXTERN rvm_ret_t RVM_Svc(rvm_ptr_t Op_Cid,
                         rvm_ptr_t Arg1,
                         rvm_ptr_t Arg2,
                         rvm_ptr_t Arg3);
EXTERN rvm_ret_t RVM_RV32P_Svc_Kfn(rvm_ptr_t Op_Cid,
                                   rvm_ptr_t ID,
                                   rvm_ptr_t* Param);
/* Invocation */
EXTERN rvm_ret_t RVM_Inv_Act(rvm_cid_t Cap_Inv,
                             rvm_ptr_t Param,
                             rvm_ptr_t* Retval);
EXTERN rvm_ret_t RVM_Inv_Ret(rvm_ptr_t Retval);
#if(RVM_DEBUG_PRINT==1U)
/* Debugging */
__EXTERN__ rvm_ptr_t RVM_Putchar(char Char);
#endif
/* Stack operations */
__EXTERN__ rvm_ptr_t RVM_Stack_Init(rvm_ptr_t Stack,
                                    rvm_ptr_t Size,
                                    rvm_ptr_t* Entry,
                                    rvm_ptr_t Stub);
/* Idle function */
__EXTERN__ void RVM_Idle(void);
/* Coprocessor context size */
__EXTERN__ rvm_ptr_t RVM_Thd_Cop_Size(rvm_ptr_t Attr);
/* Kernel function activation */
__EXTERN__ rvm_ret_t RVM_RV32P_Kfn_Act(rvm_cid_t Cap_Kern,
                                       rvm_ptr_t Func_ID,
                                       rvm_ptr_t Sub_ID,
                                       rvm_ptr_t* Param);
/* Print exception */
__EXTERN__ void RVM_Thd_Print_Exc(rvm_tid_t TID);
/* Print registers */
__EXTERN__ rvm_ret_t RVM_Thd_Print_Reg(rvm_cid_t Cap_Thd);
/*****************************************************************************/
/* Undefine "__EXTERN__" to avoid redefinition */
#undef __EXTERN__
/* __RVM_PLATFORM_RV32P_MEMBER__ */
#endif
/* !(defined __HDR_DEF__||defined __HDR_STRUCT__) */
#endif
/* End Public Function *******************************************************/

/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/

