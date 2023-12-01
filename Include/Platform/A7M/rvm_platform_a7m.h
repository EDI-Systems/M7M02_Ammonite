/******************************************************************************
Filename    : rvm_platform_a7m.h
Author      : pry
Date        : 25/06/2017
Licence     : The Unlicense; see LICENSE for details.
Description : The header of the platform dependent part.
******************************************************************************/

/* Define ********************************************************************/
#ifdef __HDR_DEF__
#ifndef __RVM_PLATFORM_A7M_DEF__
#define __RVM_PLATFORM_A7M_DEF__
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
#define RVM_MSB_GET(VAL)                            __RVM_A7M_MSB_Get(VAL)
/* FPU type definitions - keep in accordance with kernel */
#define RVM_A7M_ATTR_NONE                           (0U)
#define RVM_A7M_ATTR_FPV4_SP                        (1U<<0)
#define RVM_A7M_ATTR_FPV5_SP                        (1U<<1)
#define RVM_A7M_ATTR_FPV5_DP                        (1U<<2)

/* Platform-specific includes */
#include "rvm_platform_a7m_conf.h"

/* Thread size */
#define RVM_HYP_RAW_SIZE                            ((21U)*sizeof(rvm_ptr_t))
/* Invocation size */
#define RVM_INV_RAW_SIZE                            ((9U)*sizeof(rvm_ptr_t))
/* Normal page directory size */
#define RVM_PGT_RAW_SIZE_NOM(NUM_ORDER)             ((4U+RVM_POW2(NUM_ORDER))*sizeof(rvm_ptr_t))
/* Top-level page directory size */
#define RVM_PGT_RAW_SIZE_TOP(NUM_ORDER)             ((1U+2U*RVM_A7M_REGION_NUM)*sizeof(rvm_ptr_t)+RVM_PGT_RAW_SIZE_NOM(NUM_ORDER))

#define RVM_A7M_REG(X)                              (*((volatile rvm_ptr_t*)(X)))
#define RVM_A7M_REGB(X)                             (*((volatile rvm_u8_t*)(X)))

/* Fault reasons */
/* Debug event has occurred. The Debug Fault Status Register has been updated */
#define RVM_A7M_HFSR_DEBUGEVT                       (1U<<31)
/* Processor has escalated a configurable-priority exception to HardFault */
#define RVM_A7M_HFSR_FORCED                         (1U<<30)
/* Vector table read fault has occurred */
#define RVM_A7M_HFSR_VECTTBL                        (1U<<1)
/* Divide by zero */
#define RVM_A7M_UFSR_DIVBYZERO                      (1U<<25)
/* Unaligned load/store access */
#define RVM_A7M_UFSR_UNALIGNED                      (1U<<24)
/* No such coprocessor */
#define RVM_A7M_UFSR_NOCP                           (1U<<19)
/* Invalid vector return LR or PC value */
#define RVM_A7M_UFSR_INVPC                          (1U<<18)
/* Attempt to enter an invalid instruction set (ARM) state */
#define RVM_A7M_UFSR_INVSTATE                       (1U<<17)
/* Invalid IT instruction or related instructions */
#define RVM_A7M_UFSR_UNDEFINSTR                     (1U<<16)
/* The Bus Fault Address Register (BFAR) is valid */
#define RVM_A7M_BFSR_BFARVALID                      (1U<<15)
/* The bus fault happened during FP lazy stacking */
#define RVM_A7M_BFSR_LSPERR                         (1U<<13)
/* A derived bus fault has occurred on exception entry */
#define RVM_A7M_BFSR_STKERR                         (1U<<12)
/* A derived bus fault has occurred on exception return */
#define RVM_A7M_BFSR_UNSTKERR                       (1U<<11)
/* Imprecise data access error has occurred */
#define RVM_A7M_BFSR_IMPRECISERR                    (1U<<10)
/* Precise data access error has occurred, BFAR updated */
#define RVM_A7M_BFSR_PRECISERR                      (1U<<9)
/* A bus fault on an instruction prefetch has occurred. The 
 * fault is signaled only if the instruction is issued */
#define RVM_A7M_BFSR_IBUSERR                        (1U<<8)
/* The Memory Mnagement Fault Address Register have valid contents */
#define RVM_A7M_MFSR_MMARVALID                      (1U<<7)
/* A MemManage fault occurred during FP lazy state preservation */
#define RVM_A7M_MFSR_MLSPERR                        (1U<<5)
/* A derived MemManage fault occurred on exception entry */
#define RVM_A7M_MFSR_MSTKERR                        (1U<<4)
/* A derived MemManage fault occurred on exception return */
#define RVM_A7M_MFSR_MUNSTKERR                      (1U<<3)
/* Data access violation. The MMFAR shows the data address that
 * the load or store tried to access */
#define RVM_A7M_MFSR_DACCVIOL                       (1U<<1)
/* MPU or Execute Never (XN) default memory map access violation on an
 * instruction fetch has occurred. The fault is signalled only if the
 * instruction is issued */
#define RVM_A7M_MFSR_IACCVIOL                       (1U<<0)

/* Platform-specific kernel function macros **********************************/
/* Page table entry mode which property to get */
#define RVM_A7M_KFN_PGT_ENTRY_MOD_FLAG_GET          (0U)
#define RVM_A7M_KFN_PGT_ENTRY_MOD_SZORD_GET         (1U)
#define RVM_A7M_KFN_PGT_ENTRY_MOD_NUMORD_GET        (2U)
/* Interrupt source configuration */
#define RVM_A7M_KFN_INT_LOCAL_MOD_STATE_GET         (0U)
#define RVM_A7M_KFN_INT_LOCAL_MOD_STATE_SET         (1U)
#define RVM_A7M_KFN_INT_LOCAL_MOD_PRIO_GET          (2U)
#define RVM_A7M_KFN_INT_LOCAL_MOD_PRIO_SET          (3U)
/* Cache identifier */
#define RVM_A7M_KFN_CACHE_ICACHE                    (0U)
#define RVM_A7M_KFN_CACHE_DCACHE                    (1U)
#define RVM_A7M_KFN_CACHE_BTAC                      (2U)
/* Cache maintenance */
#define RVM_A7M_KFN_CACHE_CLEAN_ALL                 (0U)
#define RVM_A7M_KFN_CACHE_CLEAN_ADDR                (1U)
#define RVM_A7M_KFN_CACHE_CLEAN_SET                 (2U)
#define RVM_A7M_KFN_CACHE_CLEAN_WAY                 (3U)
#define RVM_A7M_KFN_CACHE_CLEAN_SETWAY              (4U)
#define RVM_A7M_KFN_CACHE_INV_ALL                   (5U)
#define RVM_A7M_KFN_CACHE_INV_ADDR                  (6U)
#define RVM_A7M_KFN_CACHE_INV_SET                   (7U)
#define RVM_A7M_KFN_CACHE_INV_WAY                   (8U)
#define RVM_A7M_KFN_CACHE_INV_SETWAY                (9U)
#define RVM_A7M_KFN_CACHE_CLEAN_INV_ALL             (10U)
#define RVM_A7M_KFN_CACHE_CLEAN_INV_ADDR            (11U)
#define RVM_A7M_KFN_CACHE_CLEAN_INV_SET             (12U)
#define RVM_A7M_KFN_CACHE_CLEAN_INV_WAY             (13U)
#define RVM_A7M_KFN_CACHE_CLEAN_INV_SETWAY          (14U)
/* CPU feature support */
#define RVM_A7M_KFN_CPU_FUNC_CPUID                  (0U)
#define RVM_A7M_KFN_CPU_FUNC_ID_PFR0                (1U)
#define RVM_A7M_KFN_CPU_FUNC_ID_PFR1                (2U)
#define RVM_A7M_KFN_CPU_FUNC_ID_DFR0                (3U)
#define RVM_A7M_KFN_CPU_FUNC_ID_AFR0                (4U)
#define RVM_A7M_KFN_CPU_FUNC_ID_MMFR0               (5U)
#define RVM_A7M_KFN_CPU_FUNC_ID_MMFR1               (6U)
#define RVM_A7M_KFN_CPU_FUNC_ID_MMFR2               (7U)
#define RVM_A7M_KFN_CPU_FUNC_ID_MMFR3               (8U)
#define RVM_A7M_KFN_CPU_FUNC_ID_ISAR0               (9U)
#define RVM_A7M_KFN_CPU_FUNC_ID_ISAR1               (10U)
#define RVM_A7M_KFN_CPU_FUNC_ID_ISAR2               (11U)
#define RVM_A7M_KFN_CPU_FUNC_ID_ISAR3               (12U)
#define RVM_A7M_KFN_CPU_FUNC_ID_ISAR4               (13U)
#define RVM_A7M_KFN_CPU_FUNC_ID_ISAR5               (14U)
#define RVM_A7M_KFN_CPU_FUNC_CLIDR                  (15U)
#define RVM_A7M_KFN_CPU_FUNC_CTR                    (16U)
#define RVM_A7M_KFN_CPU_FUNC_ICACHE_CCSIDR          (17U)
#define RVM_A7M_KFN_CPU_FUNC_DCACHE_CCSIDR          (18U)
#define RVM_A7M_KFN_CPU_FUNC_MPU_TYPE               (19U)
#define RVM_A7M_KFN_CPU_FUNC_MVFR0                  (20U)
#define RVM_A7M_KFN_CPU_FUNC_MVFR1                  (21U)
#define RVM_A7M_KFN_CPU_FUNC_MVFR2                  (22U)
#define RVM_A7M_KFN_CPU_FUNC_PID0                   (23U)
#define RVM_A7M_KFN_CPU_FUNC_PID1                   (24U)
#define RVM_A7M_KFN_CPU_FUNC_PID2                   (25U)
#define RVM_A7M_KFN_CPU_FUNC_PID3                   (26U)
#define RVM_A7M_KFN_CPU_FUNC_PID4                   (27U)
#define RVM_A7M_KFN_CPU_FUNC_PID5                   (28U)
#define RVM_A7M_KFN_CPU_FUNC_PID6                   (29U)
#define RVM_A7M_KFN_CPU_FUNC_PID7                   (30U)
#define RVM_A7M_KFN_CPU_FUNC_CID0                   (31U)
#define RVM_A7M_KFN_CPU_FUNC_CID1                   (32U)
#define RVM_A7M_KFN_CPU_FUNC_CID2                   (33U)
#define RVM_A7M_KFN_CPU_FUNC_CID3                   (34U)
/* Perfomance counters */
#define RVM_A7M_KFN_PERF_CYCLE_CYCCNT               (0U)
/* Performance counter state operations */
#define RVM_A7M_KFN_PERF_STATE_GET                  (0U)
#define RVM_A7M_KFN_PERF_STATE_SET                  (1U)
/* Performance counter states */
#define RVM_A7M_KFN_PERF_STATE_DISABLE              (0U)
#define RVM_A7M_KFN_PERF_STATE_ENABLE               (1U)
/* Performance counter value operations */
#define RVM_A7M_KFN_PERF_VAL_GET                    (0U)
#define RVM_A7M_KFN_PERF_VAL_SET                    (1U)
/* Register read/write */
#define RVM_A7M_KFN_DEBUG_REG_MOD_GET               (0U)
#define RVM_A7M_KFN_DEBUG_REG_MOD_SET               (1U<<16)
/* General-purpose registers */
#define RVM_A7M_KFN_DEBUG_REG_MOD_SP                (0U)
#define RVM_A7M_KFN_DEBUG_REG_MOD_R4                (1U)
#define RVM_A7M_KFN_DEBUG_REG_MOD_R5                (2U)
#define RVM_A7M_KFN_DEBUG_REG_MOD_R6                (3U)
#define RVM_A7M_KFN_DEBUG_REG_MOD_R7                (4U)
#define RVM_A7M_KFN_DEBUG_REG_MOD_R8                (5U)
#define RVM_A7M_KFN_DEBUG_REG_MOD_R9                (6U)
#define RVM_A7M_KFN_DEBUG_REG_MOD_R10               (7U)
#define RVM_A7M_KFN_DEBUG_REG_MOD_R11               (8U)
#define RVM_A7M_KFN_DEBUG_REG_MOD_LR                (9U)
/* FPU registers */
#define RVM_A7M_KFN_DEBUG_REG_MOD_S16               (10U)
#define RVM_A7M_KFN_DEBUG_REG_MOD_S17               (11U)
#define RVM_A7M_KFN_DEBUG_REG_MOD_S18               (12U)
#define RVM_A7M_KFN_DEBUG_REG_MOD_S19               (13U)
#define RVM_A7M_KFN_DEBUG_REG_MOD_S20               (14U)
#define RVM_A7M_KFN_DEBUG_REG_MOD_S21               (15U)
#define RVM_A7M_KFN_DEBUG_REG_MOD_S22               (16U)
#define RVM_A7M_KFN_DEBUG_REG_MOD_S23               (17U)
#define RVM_A7M_KFN_DEBUG_REG_MOD_S24               (18U)
#define RVM_A7M_KFN_DEBUG_REG_MOD_S25               (19U)
#define RVM_A7M_KFN_DEBUG_REG_MOD_S26               (20U)
#define RVM_A7M_KFN_DEBUG_REG_MOD_S27               (21U)
#define RVM_A7M_KFN_DEBUG_REG_MOD_S28               (22U)
#define RVM_A7M_KFN_DEBUG_REG_MOD_S29               (23U)
#define RVM_A7M_KFN_DEBUG_REG_MOD_S30               (24U)
#define RVM_A7M_KFN_DEBUG_REG_MOD_S31               (25U)
/* Getters */
#define RVM_A7M_KFN_DEBUG_REG_MOD_SP_GET            (RVM_A7M_KFN_DEBUG_REG_MOD_SP)
#define RVM_A7M_KFN_DEBUG_REG_MOD_R4_GET            (RVM_A7M_KFN_DEBUG_REG_MOD_R4)
#define RVM_A7M_KFN_DEBUG_REG_MOD_R5_GET            (RVM_A7M_KFN_DEBUG_REG_MOD_R5)
#define RVM_A7M_KFN_DEBUG_REG_MOD_R6_GET            (RVM_A7M_KFN_DEBUG_REG_MOD_R6)
#define RVM_A7M_KFN_DEBUG_REG_MOD_R7_GET            (RVM_A7M_KFN_DEBUG_REG_MOD_R7)
#define RVM_A7M_KFN_DEBUG_REG_MOD_R8_GET            (RVM_A7M_KFN_DEBUG_REG_MOD_R8)
#define RVM_A7M_KFN_DEBUG_REG_MOD_R9_GET            (RVM_A7M_KFN_DEBUG_REG_MOD_R9)
#define RVM_A7M_KFN_DEBUG_REG_MOD_R10_GET           (RVM_A7M_KFN_DEBUG_REG_MOD_R10)
#define RVM_A7M_KFN_DEBUG_REG_MOD_R11_GET           (RVM_A7M_KFN_DEBUG_REG_MOD_R11)
#define RVM_A7M_KFN_DEBUG_REG_MOD_LR_GET            (RVM_A7M_KFN_DEBUG_REG_MOD_LR)
#define RVM_A7M_KFN_DEBUG_REG_MOD_S16_GET           (RVM_A7M_KFN_DEBUG_REG_MOD_S16)
#define RVM_A7M_KFN_DEBUG_REG_MOD_S17_GET           (RVM_A7M_KFN_DEBUG_REG_MOD_S17)
#define RVM_A7M_KFN_DEBUG_REG_MOD_S18_GET           (RVM_A7M_KFN_DEBUG_REG_MOD_S18)
#define RVM_A7M_KFN_DEBUG_REG_MOD_S19_GET           (RVM_A7M_KFN_DEBUG_REG_MOD_S19)
#define RVM_A7M_KFN_DEBUG_REG_MOD_S20_GET           (RVM_A7M_KFN_DEBUG_REG_MOD_S20)
#define RVM_A7M_KFN_DEBUG_REG_MOD_S21_GET           (RVM_A7M_KFN_DEBUG_REG_MOD_S21)
#define RVM_A7M_KFN_DEBUG_REG_MOD_S22_GET           (RVM_A7M_KFN_DEBUG_REG_MOD_S22)
#define RVM_A7M_KFN_DEBUG_REG_MOD_S23_GET           (RVM_A7M_KFN_DEBUG_REG_MOD_S23)
#define RVM_A7M_KFN_DEBUG_REG_MOD_S24_GET           (RVM_A7M_KFN_DEBUG_REG_MOD_S24)
#define RVM_A7M_KFN_DEBUG_REG_MOD_S25_GET           (RVM_A7M_KFN_DEBUG_REG_MOD_S25)
#define RVM_A7M_KFN_DEBUG_REG_MOD_S26_GET           (RVM_A7M_KFN_DEBUG_REG_MOD_S26)
#define RVM_A7M_KFN_DEBUG_REG_MOD_S27_GET           (RVM_A7M_KFN_DEBUG_REG_MOD_S27)
#define RVM_A7M_KFN_DEBUG_REG_MOD_S28_GET           (RVM_A7M_KFN_DEBUG_REG_MOD_S28)
#define RVM_A7M_KFN_DEBUG_REG_MOD_S29_GET           (RVM_A7M_KFN_DEBUG_REG_MOD_S29)
#define RVM_A7M_KFN_DEBUG_REG_MOD_S30_GET           (RVM_A7M_KFN_DEBUG_REG_MOD_S30)
#define RVM_A7M_KFN_DEBUG_REG_MOD_S31_GET           (RVM_A7M_KFN_DEBUG_REG_MOD_S31)
/* Setters */
#define RVM_A7M_KFN_DEBUG_REG_MOD_SP_SET            (RVM_A7M_KFN_DEBUG_REG_MOD_SET|RVM_A7M_KFN_DEBUG_REG_MOD_SP)
#define RVM_A7M_KFN_DEBUG_REG_MOD_R4_SET            (RVM_A7M_KFN_DEBUG_REG_MOD_SET|RVM_A7M_KFN_DEBUG_REG_MOD_R4)
#define RVM_A7M_KFN_DEBUG_REG_MOD_R5_SET            (RVM_A7M_KFN_DEBUG_REG_MOD_SET|RVM_A7M_KFN_DEBUG_REG_MOD_R5)
#define RVM_A7M_KFN_DEBUG_REG_MOD_R6_SET            (RVM_A7M_KFN_DEBUG_REG_MOD_SET|RVM_A7M_KFN_DEBUG_REG_MOD_R6)
#define RVM_A7M_KFN_DEBUG_REG_MOD_R7_SET            (RVM_A7M_KFN_DEBUG_REG_MOD_SET|RVM_A7M_KFN_DEBUG_REG_MOD_R7)
#define RVM_A7M_KFN_DEBUG_REG_MOD_R8_SET            (RVM_A7M_KFN_DEBUG_REG_MOD_SET|RVM_A7M_KFN_DEBUG_REG_MOD_R8)
#define RVM_A7M_KFN_DEBUG_REG_MOD_R9_SET            (RVM_A7M_KFN_DEBUG_REG_MOD_SET|RVM_A7M_KFN_DEBUG_REG_MOD_R9)
#define RVM_A7M_KFN_DEBUG_REG_MOD_R10_SET           (RVM_A7M_KFN_DEBUG_REG_MOD_SET|RVM_A7M_KFN_DEBUG_REG_MOD_R10)
#define RVM_A7M_KFN_DEBUG_REG_MOD_R11_SET           (RVM_A7M_KFN_DEBUG_REG_MOD_SET|RVM_A7M_KFN_DEBUG_REG_MOD_R11)
#define RVM_A7M_KFN_DEBUG_REG_MOD_LR_SET            (RVM_A7M_KFN_DEBUG_REG_MOD_SET|RVM_A7M_KFN_DEBUG_REG_MOD_LR)
#define RVM_A7M_KFN_DEBUG_REG_MOD_S16_SET           (RVM_A7M_KFN_DEBUG_REG_MOD_SET|RVM_A7M_KFN_DEBUG_REG_MOD_S16)
#define RVM_A7M_KFN_DEBUG_REG_MOD_S17_SET           (RVM_A7M_KFN_DEBUG_REG_MOD_SET|RVM_A7M_KFN_DEBUG_REG_MOD_S17)
#define RVM_A7M_KFN_DEBUG_REG_MOD_S18_SET           (RVM_A7M_KFN_DEBUG_REG_MOD_SET|RVM_A7M_KFN_DEBUG_REG_MOD_S18)
#define RVM_A7M_KFN_DEBUG_REG_MOD_S19_SET           (RVM_A7M_KFN_DEBUG_REG_MOD_SET|RVM_A7M_KFN_DEBUG_REG_MOD_S19)
#define RVM_A7M_KFN_DEBUG_REG_MOD_S20_SET           (RVM_A7M_KFN_DEBUG_REG_MOD_SET|RVM_A7M_KFN_DEBUG_REG_MOD_S20)
#define RVM_A7M_KFN_DEBUG_REG_MOD_S21_SET           (RVM_A7M_KFN_DEBUG_REG_MOD_SET|RVM_A7M_KFN_DEBUG_REG_MOD_S21)
#define RVM_A7M_KFN_DEBUG_REG_MOD_S22_SET           (RVM_A7M_KFN_DEBUG_REG_MOD_SET|RVM_A7M_KFN_DEBUG_REG_MOD_S22)
#define RVM_A7M_KFN_DEBUG_REG_MOD_S23_SET           (RVM_A7M_KFN_DEBUG_REG_MOD_SET|RVM_A7M_KFN_DEBUG_REG_MOD_S23)
#define RVM_A7M_KFN_DEBUG_REG_MOD_S24_SET           (RVM_A7M_KFN_DEBUG_REG_MOD_SET|RVM_A7M_KFN_DEBUG_REG_MOD_S24)
#define RVM_A7M_KFN_DEBUG_REG_MOD_S25_SET           (RVM_A7M_KFN_DEBUG_REG_MOD_SET|RVM_A7M_KFN_DEBUG_REG_MOD_S25)
#define RVM_A7M_KFN_DEBUG_REG_MOD_S26_SET           (RVM_A7M_KFN_DEBUG_REG_MOD_SET|RVM_A7M_KFN_DEBUG_REG_MOD_S26)
#define RVM_A7M_KFN_DEBUG_REG_MOD_S27_SET           (RVM_A7M_KFN_DEBUG_REG_MOD_SET|RVM_A7M_KFN_DEBUG_REG_MOD_S27)
#define RVM_A7M_KFN_DEBUG_REG_MOD_S28_SET           (RVM_A7M_KFN_DEBUG_REG_MOD_SET|RVM_A7M_KFN_DEBUG_REG_MOD_S28)
#define RVM_A7M_KFN_DEBUG_REG_MOD_S29_SET           (RVM_A7M_KFN_DEBUG_REG_MOD_SET|RVM_A7M_KFN_DEBUG_REG_MOD_S29)
#define RVM_A7M_KFN_DEBUG_REG_MOD_S30_SET           (RVM_A7M_KFN_DEBUG_REG_MOD_SET|RVM_A7M_KFN_DEBUG_REG_MOD_S30)
#define RVM_A7M_KFN_DEBUG_REG_MOD_S31_SET           (RVM_A7M_KFN_DEBUG_REG_MOD_SET|RVM_A7M_KFN_DEBUG_REG_MOD_S31)
/* Invocation register read/write */
#define RVM_A7M_KFN_DEBUG_INV_MOD_SP_GET            (0U)
#define RVM_A7M_KFN_DEBUG_INV_MOD_SP_SET            (1U)
#define RVM_A7M_KFN_DEBUG_INV_MOD_LR_GET            (2U)
#define RVM_A7M_KFN_DEBUG_INV_MOD_LR_SET            (3U)
/* Error register read */
#define RVM_A7M_KFN_DEBUG_EXC_CAUSE_GET             (0U)
#define RVM_A7M_KFN_DEBUG_EXC_ADDR_GET              (1U)
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
/* __RVM_PLATFORM_A7M_DEF__ */
#endif
/* __HDR_DEF__ */
#endif
/* End Define ****************************************************************/

/* Struct ********************************************************************/
#ifdef __HDR_STRUCT__
#ifndef __RVM_PLATFORM_A7M_STRUCT__
#define __RVM_PLATFORM_A7M_STRUCT__
/* We used structs in the header */

/* Use defines in these headers */
#define __HDR_DEF__
#undef __HDR_DEF__
/*****************************************************************************/
struct RVM_A7M_Stack
{
    /* Normal registers */
    rvm_ptr_t R0;
    rvm_ptr_t R1;
    rvm_ptr_t R2;
    rvm_ptr_t R3;
    rvm_ptr_t R12;
    rvm_ptr_t LR;
    rvm_ptr_t PC;
    rvm_ptr_t XPSR;
    /* FPU registers follow - no matter they are used or not, we reserve the space
     * in stack creation */
    rvm_ptr_t S0;
    rvm_ptr_t S1;
    rvm_ptr_t S2;
    rvm_ptr_t S3;
    rvm_ptr_t S4;
    rvm_ptr_t S5;
    rvm_ptr_t S6;
    rvm_ptr_t S7;
    rvm_ptr_t S8;
    rvm_ptr_t S9;
    rvm_ptr_t S10;
    rvm_ptr_t S11;
    rvm_ptr_t S12;
    rvm_ptr_t S13;
    rvm_ptr_t S14;
    rvm_ptr_t S15;
};

struct RVM_Reg_Struct
{
    rvm_ptr_t SP;
    rvm_ptr_t R4;
    rvm_ptr_t R5;
    rvm_ptr_t R6;
    rvm_ptr_t R7;
    rvm_ptr_t R8;
    rvm_ptr_t R9;
    rvm_ptr_t R10;
    rvm_ptr_t R11;
    rvm_ptr_t LR;
};

/* The coprocessor register set structure. In Cortex-M, if there is a 
 * single-precision FPU, then the FPU S0-S15 is automatically pushed */
struct RVM_A7M_Cop_Struct
{
    rvm_ptr_t S16;
    rvm_ptr_t S17;
    rvm_ptr_t S18;
    rvm_ptr_t S19;
    rvm_ptr_t S20;
    rvm_ptr_t S21;
    rvm_ptr_t S22;
    rvm_ptr_t S23;
    rvm_ptr_t S24;
    rvm_ptr_t S25;
    rvm_ptr_t S26;
    rvm_ptr_t S27;
    rvm_ptr_t S28;
    rvm_ptr_t S29;
    rvm_ptr_t S30;
    rvm_ptr_t S31;
};

struct RVM_Exc_Struct
{
    rvm_ptr_t Cause;
    rvm_ptr_t Addr;
};
/*****************************************************************************/
/* __RVM_PLATFORM_A7M_STRUCT__ */
#endif
/* __HDR_STRUCT__ */
#endif
/* End Struct ****************************************************************/

/* Private Variable **********************************************************/
#if(!(defined __HDR_DEF__||defined __HDR_STRUCT__))
#ifndef __RVM_PLATFORM_A7M_MEMBER__
#define __RVM_PLATFORM_A7M_MEMBER__

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
EXTERN rvm_ptr_t __RVM_A7M_MSB_Get(rvm_ptr_t Value); 

/* System call */
EXTERN rvm_ret_t RVM_Svc(rvm_ptr_t Op_Cid,
                         rvm_ptr_t Param1,
                         rvm_ptr_t Param2,
                         rvm_ptr_t Param3);
EXTERN rvm_ret_t RVM_A7M_Svc_Kfn(rvm_ptr_t Op_Cid,
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
/* Stack operation */
__EXTERN__ rvm_ptr_t RVM_Stack_Init(rvm_ptr_t Stack,
                                    rvm_ptr_t Size,
                                    rvm_ptr_t* Entry,
                                    rvm_ptr_t Stub);
/* Idle function */
__EXTERN__ void RVM_Idle(void);
/* Coprocessor context size */
__EXTERN__ rvm_ptr_t RVM_Thd_Cop_Size(rvm_ptr_t Attr);
/* Kernel function activation */
__EXTERN__ rvm_ret_t RVM_A7M_Kfn_Act(rvm_cid_t Cap_Kern,
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
/* __RVM_PLATFORM_A7M_MEMBER__ */
#endif
/* !(defined __HDR_DEF__||defined __HDR_STRUCT__) */
#endif
/* End Public Function *******************************************************/

/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
