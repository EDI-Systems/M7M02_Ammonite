/******************************************************************************
Filename    : rvm_platform_a6m.h
Author      : pry
Date        : 25/06/2017
Licence     : The Unlicense; see LICENSE for details.
Description : The header of the platform dependent part.
******************************************************************************/

/* Define ********************************************************************/
#ifdef __HDR_DEF__
#ifndef __RVM_PLATFORM_A6M_DEF__
#define __RVM_PLATFORM_A6M_DEF__
/*****************************************************************************/
/* Basic Type ****************************************************************/
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
/* End Basic Type ************************************************************/

/* Extended Type *************************************************************/
#ifndef __RVM_CID_T__
#define __RVM_CID_T__
/* Capability ID */
typedef rvm_s32_t rvm_cid_t;
#endif

#ifndef __RVM_TID_T__
#define __RVM_TID_T__
/* Thread ID */
typedef rvm_s32_t rvm_tid_t;
#endif

#ifndef __RVM_PTR_T__
#define __RVM_PTR_T__
/* Pointer */
typedef rvm_u32_t rvm_ptr_t;
#endif

#ifndef __RVM_CNT_T__
#define __RVM_CNT_T__
/* Counter */
typedef rvm_s32_t rvm_cnt_t;
#endif

#ifndef __RVM_RET_T__
#define __RVM_RET_T__
/* Return value */
typedef rvm_s32_t rvm_ret_t;
#endif
/* End Extended Type *********************************************************/

/* System Macro **************************************************************/
/* Compiler "extern" keyword setting */
#define RVM_EXTERN                              extern
/* The order of bits in one CPU machine word */
#define RVM_WORD_ORDER                          (5U)
/* Get most significant bit */
#define RVM_MSB_GET(VAL)                        _RVM_MSB_Generic(VAL)

/* Platform-specific includes */
#include "rvm_platform_a6m_conf.h"

/* Thread size */
#define RVM_HYP_RAW_SIZE                        ((22U)*RVM_WORD_BYTE)
/* Invocation size - ARMv6-M has fixed LR */
#define RVM_INV_RAW_SIZE                        ((8U)*RVM_WORD_BYTE)
/* Normal page directory size */
#define RVM_PGT_RAW_SIZE_NOM(NMORD)             ((4U+RVM_POW2(NMORD))*RVM_WORD_BYTE)
/* Top-level page directory size - ARMv6-M are all static, no static field needed */
#define RVM_PGT_RAW_SIZE_TOP(NMORD)             ((2U*RVM_A6M_REGION_NUM)*RVM_WORD_BYTE+RVM_PGT_RAW_SIZE_NOM(NMORD))
/* End System Macro **********************************************************/

/* ARMv6-M Macro *************************************************************/
/* Generic *******************************************************************/
/* Register access */
#define RVM_A6M_REG(X)                          (*((volatile rvm_ptr_t*)(X)))
#define RVM_A6M_REGB(X)                         (*((volatile rvm_u8_t*)(X)))

/* Thread context attribute definitions - keep in accordance with kernel */
#define RVM_A6M_ATTR_NONE                       (0U)

/* Handler *******************************************************************/
/* Debug event has occurred. The Debug Fault Status Register has been updated */
#define RVM_A6M_HFSR_DEBUGEVT                   RVM_POW2(31U)
/* Processor has escalated a configurable-priority exception to HardFault */
#define RVM_A6M_HFSR_FORCED                     RVM_POW2(30U)
/* Vector table read fault has occurred */
#define RVM_A6M_HFSR_VECTTBL                    RVM_POW2(1U)
/* Divide by zero */
#define RVM_A6M_UFSR_DIVBYZERO                  RVM_POW2(25U)
/* Unaligned load/store access */
#define RVM_A6M_UFSR_UNALIGNED                  RVM_POW2(24U)
/* No such coprocessor */
#define RVM_A6M_UFSR_NOCP                       RVM_POW2(19U)
/* Invalid vector return LR or PC value */
#define RVM_A6M_UFSR_INVPC                      RVM_POW2(18U)
/* Attempt to enter an invalid instruction set (ARM) state */
#define RVM_A6M_UFSR_INVSTATE                   RVM_POW2(17U)
/* Invalid IT instruction or related instructions */
#define RVM_A6M_UFSR_UNDEFINSTR                 RVM_POW2(16U)
/* The Bus Fault Address Register (BFAR) is valid */
#define RVM_A6M_BFSR_BFARVALID                  RVM_POW2(15U)
/* The bus fault happened during FP lazy stacking */
#define RVM_A6M_BFSR_LSPERR                     RVM_POW2(13U)
/* A derived bus fault has occurred on exception entry */
#define RVM_A6M_BFSR_STKERR                     RVM_POW2(12U)
/* A derived bus fault has occurred on exception return */
#define RVM_A6M_BFSR_UNSTKERR                   RVM_POW2(11U)
/* Imprecise data access error has occurred */
#define RVM_A6M_BFSR_IMPRECISERR                RVM_POW2(10U)
/* Precise data access error has occurred, BFAR updated */
#define RVM_A6M_BFSR_PRECISERR                  RVM_POW2(9U)
/* A bus fault on an instruction prefetch has occurred. The 
 * fault is signaled only if the instruction is issued */
#define RVM_A6M_BFSR_IBUSERR                    RVM_POW2(8U)
/* The Memory Mnagement Fault Address Register have valid contents */
#define RVM_A6M_MFSR_MMARVALID                  RVM_POW2(7U)
/* A MemManage fault occurred during FP lazy state preservation */
#define RVM_A6M_MFSR_MLSPERR                    RVM_POW2(5U)
/* A derived MemManage fault occurred on exception entry */
#define RVM_A6M_MFSR_MSTKERR                    RVM_POW2(4U)
/* A derived MemManage fault occurred on exception return */
#define RVM_A6M_MFSR_MUNSTKERR                  RVM_POW2(3U)
/* Data access violation. The MMFAR shows the data address that
 * the load or store tried to access */
#define RVM_A6M_MFSR_DACCVIOL                   RVM_POW2(1U)
/* MPU or Execute Never (XN) default memory map access violation on an
 * instruction fetch has occurred. The fault is signalled only if the
 * instruction is issued */
#define RVM_A6M_MFSR_IACCVIOL                   RVM_POW2(0U)

/* Kernel Function ***********************************************************/
/* Page table entry mode which property to get */
#define RVM_A6M_KFN_PGT_ENTRY_MOD_FLAG_GET      (0U)
#define RVM_A6M_KFN_PGT_ENTRY_MOD_SZORD_GET     (1U)
#define RVM_A6M_KFN_PGT_ENTRY_MOD_NMORD_GET     (2U)
/* Interrupt source configuration */
#define RVM_A6M_KFN_INT_LOCAL_MOD_STATE_GET     (0U)
#define RVM_A6M_KFN_INT_LOCAL_MOD_STATE_SET     (1U)
#define RVM_A6M_KFN_INT_LOCAL_MOD_PRIO_GET      (2U)
#define RVM_A6M_KFN_INT_LOCAL_MOD_PRIO_SET      (3U)
/* Prefetcher modification */
#define RVM_A6M_KFN_PRFTH_MOD_STATE_GET         (0U)
#define RVM_A6M_KFN_PRFTH_MOD_STATE_SET         (1U)
/* Prefetcher state */
#define RVM_A6M_KFN_PRFTH_STATE_DISABLE         (0U)
#define RVM_A6M_KFN_PRFTH_STATE_ENABLE          (1U)
/* CPU feature support */
#define RVM_A6M_KFN_CPU_FUNC_CPUID              (0U)
#define RVM_A6M_KFN_CPU_FUNC_ID_PFR0            (1U)
#define RVM_A6M_KFN_CPU_FUNC_ID_PFR1            (2U)
#define RVM_A6M_KFN_CPU_FUNC_ID_DFR0            (3U)
#define RVM_A6M_KFN_CPU_FUNC_ID_AFR0            (4U)
#define RVM_A6M_KFN_CPU_FUNC_ID_MMFR0           (5U)
#define RVM_A6M_KFN_CPU_FUNC_ID_MMFR1           (6U)
#define RVM_A6M_KFN_CPU_FUNC_ID_MMFR2           (7U)
#define RVM_A6M_KFN_CPU_FUNC_ID_MMFR3           (8U)
#define RVM_A6M_KFN_CPU_FUNC_ID_ISAR0           (9U)
#define RVM_A6M_KFN_CPU_FUNC_ID_ISAR1           (10U)
#define RVM_A6M_KFN_CPU_FUNC_ID_ISAR2           (11U)
#define RVM_A6M_KFN_CPU_FUNC_ID_ISAR3           (12U)
#define RVM_A6M_KFN_CPU_FUNC_ID_ISAR4           (13U)
#define RVM_A6M_KFN_CPU_FUNC_ID_ISAR5           (14U)
#define RVM_A6M_KFN_CPU_FUNC_CLIDR              (15U)
#define RVM_A6M_KFN_CPU_FUNC_CTR                (16U)
#define RVM_A6M_KFN_CPU_FUNC_ICACHE_CCSIDR      (17U)
#define RVM_A6M_KFN_CPU_FUNC_DCACHE_CCSIDR      (18U)
#define RVM_A6M_KFN_CPU_FUNC_MPU_TYPE           (19U)
#define RVM_A6M_KFN_CPU_FUNC_MVFR0              (20U)
#define RVM_A6M_KFN_CPU_FUNC_MVFR1              (21U)
#define RVM_A6M_KFN_CPU_FUNC_MVFR2              (22U)
#define RVM_A6M_KFN_CPU_FUNC_PID0               (23U)
#define RVM_A6M_KFN_CPU_FUNC_PID1               (24U)
#define RVM_A6M_KFN_CPU_FUNC_PID2               (25U)
#define RVM_A6M_KFN_CPU_FUNC_PID3               (26U)
#define RVM_A6M_KFN_CPU_FUNC_PID4               (27U)
#define RVM_A6M_KFN_CPU_FUNC_PID5               (28U)
#define RVM_A6M_KFN_CPU_FUNC_PID6               (29U)
#define RVM_A6M_KFN_CPU_FUNC_PID7               (30U)
#define RVM_A6M_KFN_CPU_FUNC_CID0               (31U)
#define RVM_A6M_KFN_CPU_FUNC_CID1               (32U)
#define RVM_A6M_KFN_CPU_FUNC_CID2               (33U)
#define RVM_A6M_KFN_CPU_FUNC_CID3               (34U)
/* Perfomance counters */
#define RVM_A6M_KFN_PERF_CYCLE_CYCCNT           (0U)
/* Performance counter state operations */
#define RVM_A6M_KFN_PERF_STATE_GET              (0U)
#define RVM_A6M_KFN_PERF_STATE_SET              (1U)
/* Performance counter states */
#define RVM_A6M_KFN_PERF_STATE_DISABLE          (0U)
#define RVM_A6M_KFN_PERF_STATE_ENABLE           (1U)
/* Performance counter value operations */
#define RVM_A6M_KFN_PERF_VAL_GET                (0U)
#define RVM_A6M_KFN_PERF_VAL_SET                (1U)
/* Register read/write */
#define RVM_A6M_KFN_DEBUG_REG_MOD_GET           (0U)
#define RVM_A6M_KFN_DEBUG_REG_MOD_SET           RVM_POW2(16U)
/* General-purpose registers */
#define RVM_A6M_KFN_DEBUG_REG_MOD_SP            (0U)
#define RVM_A6M_KFN_DEBUG_REG_MOD_R8            (1U)
#define RVM_A6M_KFN_DEBUG_REG_MOD_R9            (2U)
#define RVM_A6M_KFN_DEBUG_REG_MOD_R10           (3U)
#define RVM_A6M_KFN_DEBUG_REG_MOD_R11           (4U)
#define RVM_A6M_KFN_DEBUG_REG_MOD_R4            (5U)
#define RVM_A6M_KFN_DEBUG_REG_MOD_R5            (6U)
#define RVM_A6M_KFN_DEBUG_REG_MOD_R6            (7U)
#define RVM_A6M_KFN_DEBUG_REG_MOD_R7            (8U)
#define RVM_A6M_KFN_DEBUG_REG_MOD_LR            (9U)
/* Getters */
#define RVM_A6M_KFN_DEBUG_REG_MOD_SP_GET        (RVM_A6M_KFN_DEBUG_REG_MOD_SP)
#define RVM_A6M_KFN_DEBUG_REG_MOD_R8_GET        (RVM_A6M_KFN_DEBUG_REG_MOD_R8)
#define RVM_A6M_KFN_DEBUG_REG_MOD_R9_GET        (RVM_A6M_KFN_DEBUG_REG_MOD_R9)
#define RVM_A6M_KFN_DEBUG_REG_MOD_R10_GET       (RVM_A6M_KFN_DEBUG_REG_MOD_R10)
#define RVM_A6M_KFN_DEBUG_REG_MOD_R11_GET       (RVM_A6M_KFN_DEBUG_REG_MOD_R11)
#define RVM_A6M_KFN_DEBUG_REG_MOD_R4_GET        (RVM_A6M_KFN_DEBUG_REG_MOD_R4)
#define RVM_A6M_KFN_DEBUG_REG_MOD_R5_GET        (RVM_A6M_KFN_DEBUG_REG_MOD_R5)
#define RVM_A6M_KFN_DEBUG_REG_MOD_R6_GET        (RVM_A6M_KFN_DEBUG_REG_MOD_R6)
#define RVM_A6M_KFN_DEBUG_REG_MOD_R7_GET        (RVM_A6M_KFN_DEBUG_REG_MOD_R7)
#define RVM_A6M_KFN_DEBUG_REG_MOD_LR_GET        (RVM_A6M_KFN_DEBUG_REG_MOD_LR)
/* Setters */
#define RVM_A6M_KFN_DEBUG_REG_MOD_SP_SET        (RVM_A6M_KFN_DEBUG_REG_MOD_SET|RVM_A6M_KFN_DEBUG_REG_MOD_SP)
#define RVM_A6M_KFN_DEBUG_REG_MOD_R8_SET        (RVM_A6M_KFN_DEBUG_REG_MOD_SET|RVM_A6M_KFN_DEBUG_REG_MOD_R8)
#define RVM_A6M_KFN_DEBUG_REG_MOD_R9_SET        (RVM_A6M_KFN_DEBUG_REG_MOD_SET|RVM_A6M_KFN_DEBUG_REG_MOD_R9)
#define RVM_A6M_KFN_DEBUG_REG_MOD_R10_SET       (RVM_A6M_KFN_DEBUG_REG_MOD_SET|RVM_A6M_KFN_DEBUG_REG_MOD_R10)
#define RVM_A6M_KFN_DEBUG_REG_MOD_R11_SET       (RVM_A6M_KFN_DEBUG_REG_MOD_SET|RVM_A6M_KFN_DEBUG_REG_MOD_R11)
#define RVM_A6M_KFN_DEBUG_REG_MOD_R4_SET        (RVM_A6M_KFN_DEBUG_REG_MOD_SET|RVM_A6M_KFN_DEBUG_REG_MOD_R4)
#define RVM_A6M_KFN_DEBUG_REG_MOD_R5_SET        (RVM_A6M_KFN_DEBUG_REG_MOD_SET|RVM_A6M_KFN_DEBUG_REG_MOD_R5)
#define RVM_A6M_KFN_DEBUG_REG_MOD_R6_SET        (RVM_A6M_KFN_DEBUG_REG_MOD_SET|RVM_A6M_KFN_DEBUG_REG_MOD_R6)
#define RVM_A6M_KFN_DEBUG_REG_MOD_R7_SET        (RVM_A6M_KFN_DEBUG_REG_MOD_SET|RVM_A6M_KFN_DEBUG_REG_MOD_R7)
#define RVM_A6M_KFN_DEBUG_REG_MOD_LR_SET        (RVM_A6M_KFN_DEBUG_REG_MOD_SET|RVM_A6M_KFN_DEBUG_REG_MOD_LR)
/* Invocation register read/write */
#define RVM_A6M_KFN_DEBUG_INV_MOD_SP_GET        (0U)
#define RVM_A6M_KFN_DEBUG_INV_MOD_SP_SET        (1U)
/* Error register read */
#define RVM_A6M_KFN_DEBUG_EXC_CAUSE_GET         (0U)
/* End ARMv6-M Macro *********************************************************/
/*****************************************************************************/
/* __RVM_PLATFORM_A6M_DEF__ */
#endif
/* __HDR_DEF__ */
#endif
/* End Define ****************************************************************/

/* Struct ********************************************************************/
#ifdef __HDR_STRUCT__
#ifndef __RVM_PLATFORM_A6M_STRUCT__
#define __RVM_PLATFORM_A6M_STRUCT__
/* We used structs in the header */

/* Use defines in these headers */
#define __HDR_DEF__
#undef __HDR_DEF__
/*****************************************************************************/
struct RVM_A6M_Stack
{
    rvm_ptr_t R0;
    rvm_ptr_t R1;
    rvm_ptr_t R2;
    rvm_ptr_t R3;
    rvm_ptr_t R12;
    rvm_ptr_t LR;
    rvm_ptr_t PC;
    rvm_ptr_t XPSR;
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

struct RVM_Exc_Struct
{
    rvm_ptr_t Cause;
};
/*****************************************************************************/
/* __RVM_PLATFORM_A6M_STRUCT__ */
#endif
/* __HDR_STRUCT__ */
#endif
/* End Struct ****************************************************************/

/* Private Variable **********************************************************/
#if(!(defined __HDR_DEF__||defined __HDR_STRUCT__))
#ifndef __RVM_PLATFORM_A6M_MEMBER__
#define __RVM_PLATFORM_A6M_MEMBER__

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
#define __RVM_EXTERN__
/* End Private Function ******************************************************/

/* Public Variable ***********************************************************/
/* __HDR_PUBLIC__ */
#else
#define __RVM_EXTERN__ RVM_EXTERN 
/* __HDR_PUBLIC__ */
#endif

/*****************************************************************************/

/*****************************************************************************/

/* End Public Variable *******************************************************/

/* Public Function ***********************************************************/
/*****************************************************************************/
/* Assembly */
RVM_EXTERN void __RVM_Stub(void);

/* System call */
RVM_EXTERN rvm_ret_t RVM_Svc(rvm_ptr_t Op_Cid,
                             rvm_ptr_t Param1,
                             rvm_ptr_t Param2,
                             rvm_ptr_t Param3);
RVM_EXTERN rvm_ret_t RVM_A6M_Svc_Kfn(rvm_ptr_t Op_Cid,
                                     rvm_ptr_t ID,
                                     rvm_ptr_t* Param);
/* Invocation */
RVM_EXTERN rvm_ret_t RVM_Inv_Act(rvm_cid_t Cap_Inv,
                                 rvm_ptr_t Param,
                                 rvm_ptr_t* Retval);
RVM_EXTERN rvm_ret_t RVM_Inv_Ret(rvm_ptr_t Retval);
#if(RVM_DBGLOG_ENABLE!=0U)
/* Debugging */
__RVM_EXTERN__ rvm_ptr_t RVM_Putchar(char Char);
#endif
/* Stack operation */
__RVM_EXTERN__ rvm_ptr_t RVM_Stack_Init(rvm_ptr_t Stack,
                                        rvm_ptr_t Size,
                                        rvm_ptr_t* Entry,
                                        rvm_ptr_t Stub);
/* Idle function */
__RVM_EXTERN__ void RVM_Idle(void);
/* Kernel function activation */
__RVM_EXTERN__ rvm_ret_t RVM_A6M_Kfn_Act(rvm_cid_t Cap_Kern,
                                         rvm_ptr_t Func_ID,
                                         rvm_ptr_t Sub_ID,
                                         rvm_ptr_t* Param);
/* Print exception */
__RVM_EXTERN__ void RVM_Thd_Print_Exc(rvm_tid_t TID);
/* Print register */
__RVM_EXTERN__ rvm_ret_t RVM_Thd_Print_Reg(rvm_cid_t Cap_Thd);
/*****************************************************************************/
/* Undefine "__RVM_EXTERN__" to avoid redefinition */
#undef __RVM_EXTERN__
/* __RVM_PLATFORM_A6M_MEMBER__ */
#endif
/* !(defined __HDR_DEF__||defined __HDR_STRUCT__) */
#endif
/* End Public Function *******************************************************/

/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
