/******************************************************************************
Filename    : rvm_guest_a6m.h
Author      : pry
Date        : 01/04/2017
Licence     : The Unlicense; see LICENSE for details.
Description : The header of platform-specific part of RMP for RVM.
******************************************************************************/

/* Begin Typedefs ************************************************************/
#ifndef __RVM_GUEST_A6M__
#define __RVM_GUEST_A6M__

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
/* End Typedefs **************************************************************/

/* Begin Defines *************************************************************/
/* System macros *************************************************************/
/* Compiler "extern" keyword setting */
#define EXTERN                                      extern
/* The order of bits in one CPU machine word */
#define RVM_WORD_ORDER                              (5U)

/* Detect floating-point coprocessor existence */
#define RVM_COPROCESSOR_TYPE                        RVM_COPROCESSOR_NONE

/* Thread size */
#define RVM_THD_WORD_SIZE                           (31U)
/* Invocation size */
#define RVM_INV_WORD_SIZE                           (9U)
/* Normal page directory size */
#define RVM_PGT_WORD_SIZE_NOM(NUM_ORDER)            (5U+RVM_POW2(NUM_ORDER))
/* Top-level page directory size */
#define RVM_PGT_WORD_SIZE_TOP(NUM_ORDER)            (2U*RVM_A6M_REGION_NUM+RVM_PGT_WORD_SIZE_NOM(NUM_ORDER))

/* ARMv7-M specific kernel function macros ***********************************/
/* Page table entry mode which property to get */
#define RVM_A6M_KFN_PGT_ENTRY_MOD_GET_FLAGS         (0U)
#define RVM_A6M_KFN_PGT_ENTRY_MOD_GET_SIZEORDER     (1U)
#define RVM_A6M_KFN_PGT_ENTRY_MOD_GET_NUMORDER      (2U)
/* Interrupt source configuration */
#define RVM_A6M_KFN_INT_LOCAL_MOD_GET_STATE         (0U)
#define RVM_A6M_KFN_INT_LOCAL_MOD_SET_STATE         (1U)
#define RVM_A6M_KFN_INT_LOCAL_MOD_GET_PRIO          (2U)
#define RVM_A6M_KFN_INT_LOCAL_MOD_SET_PRIO          (3U)
/* Prefetcher modification */
#define RVM_A6M_KFN_PRFTH_MOD_GET_STATE             (0U)
#define RVM_A6M_KFN_PRFTH_MOD_SET_STATE             (1U)
/* Prefetcher state */
#define RVM_A6M_KFN_PRFTH_STATE_DISABLE             (0U)
#define RVM_A6M_KFN_PRFTH_STATE_ENABLE              (1U)
/* CPU feature support */
#define RVM_A6M_KFN_CPU_FUNC_CPUID                  (0U)
#define RVM_A6M_KFN_CPU_FUNC_MPU_TYPE               (19U)
#define RVM_A6M_KFN_CPU_FUNC_PID0                   (23U)
#define RVM_A6M_KFN_CPU_FUNC_PID1                   (24U)
#define RVM_A6M_KFN_CPU_FUNC_PID2                   (25U)
#define RVM_A6M_KFN_CPU_FUNC_PID3                   (26U)
#define RVM_A6M_KFN_CPU_FUNC_PID4                   (27U)
#define RVM_A6M_KFN_CPU_FUNC_PID5                   (28U)
#define RVM_A6M_KFN_CPU_FUNC_PID6                   (29U)
#define RVM_A6M_KFN_CPU_FUNC_PID7                   (30U)
#define RVM_A6M_KFN_CPU_FUNC_CID0                   (31U)
#define RVM_A6M_KFN_CPU_FUNC_CID1                   (32U)
#define RVM_A6M_KFN_CPU_FUNC_CID2                   (33U)
#define RVM_A6M_KFN_CPU_FUNC_CID3                   (34U)
/* Perfomance counters */
#define RVM_A6M_KFN_PERF_CYCLE_CYCCNT               (0U)
/* Performance counter state operations */
#define RVM_A6M_KFN_PERF_STATE_GET                  (0U)
#define RVM_A6M_KFN_PERF_STATE_SET                  (1U)
/* Performance counter states */
#define RVM_A6M_KFN_PERF_STATE_DISABLE              (0U)
#define RVM_A6M_KFN_PERF_STATE_ENABLE               (1U)
/* Performance counter value operations */
#define RVM_A6M_KFN_PERF_VAL_GET                    (0U)
#define RVM_A6M_KFN_PERF_VAL_SET                    (1U)
/* Register read/write */
#define RVM_A6M_KFN_DEBUG_REG_MOD_SP_GET            (0U)
#define RVM_A6M_KFN_DEBUG_REG_MOD_SP_SET            (1U)
#define RVM_A6M_KFN_DEBUG_REG_MOD_R4_GET            (2U)
#define RVM_A6M_KFN_DEBUG_REG_MOD_R4_SET            (3U)
#define RVM_A6M_KFN_DEBUG_REG_MOD_R5_GET            (4U)
#define RVM_A6M_KFN_DEBUG_REG_MOD_R5_SET            (5U)
#define RVM_A6M_KFN_DEBUG_REG_MOD_R6_GET            (6U)
#define RVM_A6M_KFN_DEBUG_REG_MOD_R6_SET            (7U)
#define RVM_A6M_KFN_DEBUG_REG_MOD_R7_GET            (8U)
#define RVM_A6M_KFN_DEBUG_REG_MOD_R7_SET            (9U)
#define RVM_A6M_KFN_DEBUG_REG_MOD_R8_GET            (10U)
#define RVM_A6M_KFN_DEBUG_REG_MOD_R8_SET            (11U)
#define RVM_A6M_KFN_DEBUG_REG_MOD_R9_GET            (12U)
#define RVM_A6M_KFN_DEBUG_REG_MOD_R9_SET            (13U)
#define RVM_A6M_KFN_DEBUG_REG_MOD_R10_GET           (14U)
#define RVM_A6M_KFN_DEBUG_REG_MOD_R10_SET           (15U)
#define RVM_A6M_KFN_DEBUG_REG_MOD_R11_GET           (16U)
#define RVM_A6M_KFN_DEBUG_REG_MOD_R11_SET           (17U)
#define RVM_A6M_KFN_DEBUG_REG_MOD_LR_GET            (18U)
#define RVM_A6M_KFN_DEBUG_REG_MOD_LR_SET            (19U)
/* Invocation register read/write */
#define RVM_A6M_KFN_DEBUG_INV_MOD_SP_GET            (0U)
#define RVM_A6M_KFN_DEBUG_INV_MOD_SP_SET            (1U)
#define RVM_A6M_KFN_DEBUG_INV_MOD_LR_GET            (2U)
#define RVM_A6M_KFN_DEBUG_INV_MOD_LR_SET            (3U)
/* Error register read */
#define RVM_A6M_KFN_DEBUG_ERR_GET_CAUSE             (0U)
/* End Defines ***************************************************************/

/* Structs *******************************************************************/
struct RVM_Reg_Struct
{
    rvm_ptr_t SP;
    /* Second group pushed to stack */
    rvm_ptr_t R8;
    rvm_ptr_t R9;
    rvm_ptr_t R10;
    rvm_ptr_t R11;
    /* First group pushed to stack */
    rvm_ptr_t R4;
    rvm_ptr_t R5;
    rvm_ptr_t R6;
    rvm_ptr_t R7;
    rvm_ptr_t LR;
};

struct RVM_Exc_Struct
{
    rvm_ptr_t Cause;
    rvm_ptr_t Addr;
};
/* End Structs ***************************************************************/

/* Private C Function Prototypes *********************************************/
#ifdef __HDR_PRIVATE_MEMBERS__
#undef EXTERN
#define EXTERN
/*****************************************************************************/

/*****************************************************************************/
#endif
/* End Private C Function Prototypes *****************************************/

/* Public Global Variables ***************************************************/
/*****************************************************************************/

/*****************************************************************************/
/* End Public Global Variables ***********************************************/

/* Public C Function Prototypes **********************************************/
/*****************************************************************************/
EXTERN void RVM_Putchar(char Char);

EXTERN rvm_ret_t RVM_Svc(rvm_ptr_t Op_Capid,
                         rvm_ptr_t Arg1,
                         rvm_ptr_t Arg2,
                         rvm_ptr_t Arg3);
EXTERN rvm_ret_t RVM_A6M_Svc_Kfn(rvm_ptr_t Op_Capid,
                                 rvm_ptr_t ID,
                                 rvm_ptr_t* Param);
/* Invocation */
EXTERN rvm_ret_t RVM_Inv_Act(rvm_cid_t Cap_Inv,
                             rvm_ptr_t Param,
                             rvm_ptr_t* Retval);
EXTERN rvm_ret_t RVM_Inv_Ret(rvm_ptr_t Retval);

EXTERN rvm_ptr_t _RVM_MSB_Get(rvm_ptr_t Val);

EXTERN rvm_ret_t RVM_A6M_Kfn_Act(rvm_cid_t Cap_Kfn,
                                 rvm_ptr_t Func_ID,
                                 rvm_ptr_t Sub_ID,
                                 rvm_ptr_t* Param);
EXTERN rvm_ret_t RVM_A6M_Pgt_Entry_Mod(rvm_cid_t Cap_Kfn,
                                       rvm_cid_t Cap_Pgt,
                                       rvm_ptr_t Vaddr,
                                       rvm_ptr_t Type);
EXTERN rvm_ret_t RVM_A6M_Int_Local_Mod(rvm_cid_t Cap_Kfn,
                                       rvm_ptr_t Int_Num,
                                       rvm_ptr_t Operation,
                                       rvm_ptr_t Param);
EXTERN rvm_ret_t RVM_A6M_Int_Local_Trig(rvm_cid_t Cap_Kfn,
                                        rvm_ptr_t Int_Num);
EXTERN rvm_ret_t RVM_A6M_Prfth_Mod(rvm_cid_t Cap_Kfn,
                                   rvm_ptr_t Prfth_ID,
                                   rvm_ptr_t Operation,
                                   rvm_ptr_t Param);
EXTERN rvm_ret_t RVM_A6M_Perf_CPU_Func(rvm_cid_t Cap_Kfn,
                                       rvm_ptr_t Freg_ID,
                                       rvm_ptr_t* Content);
EXTERN rvm_ret_t RVM_A6M_Perf_Mon_Mod(rvm_cid_t Cap_Kfn,
                                      rvm_ptr_t Perf_ID,
                                      rvm_ptr_t Operation,
                                      rvm_ptr_t Param);
EXTERN rvm_ret_t RVM_A6M_Perf_Cycle_Mod(rvm_cid_t Cap_Kfn,
                                        rvm_ptr_t Cycle_ID, 
                                        rvm_ptr_t Operation,
                                        rvm_ptr_t Value,
                                        rvm_ptr_t* Content);
EXTERN rvm_ret_t RVM_A6M_Debug_Print(rvm_cid_t Cap_Kfn,
                                     char Char);
EXTERN rvm_ret_t RVM_A6M_Debug_Reg_Mod(rvm_cid_t Cap_Kfn,
                                       rvm_cid_t Cap_Thd, 
                                       rvm_ptr_t Operation,
                                       rvm_ptr_t Value,
                                       rvm_ptr_t* Content);
EXTERN rvm_ret_t RVM_A6M_Debug_Inv_Mod(rvm_cid_t Cap_Kfn,
                                       rvm_cid_t Cap_Thd, 
                                       rvm_ptr_t Layer,
                                       rvm_ptr_t Operation,
                                       rvm_ptr_t Value,
                                       rvm_ptr_t* Content);
EXTERN rvm_ret_t RVM_A6M_Debug_Exc_Get(rvm_cid_t Cap_Kfn,
                                       rvm_cid_t Cap_Thd,
                                       rvm_ptr_t Operation,
                                       rvm_ptr_t* Content);
/*****************************************************************************/
#endif /* __RVM_GUEST_A6M__ */
/* End Public C Function Prototypes ******************************************/

/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
