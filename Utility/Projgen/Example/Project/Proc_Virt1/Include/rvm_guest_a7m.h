/******************************************************************************
Filename    : rvm_guest_a7m.h
Author      : pry
Date        : 01/04/2017
Licence     : The Unlicense; see LICENSE for details.
Description : The header of platform-specific part of RMP for RVM.
******************************************************************************/

/* Begin Typedefs ************************************************************/
#ifndef __RVM_GUEST_A7M__
#define __RVM_GUEST_A7M__

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
#define RVM_WORD_ORDER                              (5)
/* Size of kernel objects - architecture dependent */
/* Thread */
#define RVM_THD_WORD_SIZE                           (46)
/* Normal page directory */
#define RVM_PGTBL_WORD_SIZE_NOM(NUM_ORDER)          ((((rvm_ptr_t)1)<<(NUM_ORDER))+5)
/* Top-level page directory */
#define RVM_PGTBL_WORD_SIZE_TOP(NUM_ORDER)          (RVM_PGTBL_SIZE_NOM(NUM_ORDER)+5+1+2*RVM_MPU_REGIONS)

/* ARMv7-M specific kernel function macros ***********************************/
/* Page table entry mode which property to get */
#define RVM_A7M_KERN_PGTBL_ENTRY_MOD_GET_FLAGS      (0)
#define RVM_A7M_KERN_PGTBL_ENTRY_MOD_GET_SIZEORDER  (1)
#define RVM_A7M_KERN_PGTBL_ENTRY_MOD_GET_NUMORDER   (2)
/* Interrupt source configuration */
#define RVM_A7M_KERN_INT_LOCAL_MOD_GET_STATE        (0)
#define RVM_A7M_KERN_INT_LOCAL_MOD_SET_STATE        (1)
#define RVM_A7M_KERN_INT_LOCAL_MOD_GET_PRIO         (2)
#define RVM_A7M_KERN_INT_LOCAL_MOD_SET_PRIO         (3)
/* Cache identifier */
#define RVM_A7M_KERN_CACHE_ICACHE                   (0)
#define RVM_A7M_KERN_CACHE_DCACHE                   (1)
#define RVM_A7M_KERN_CACHE_BTAC                     (2)
/* Cache modification */
#define RVM_A7M_KERN_CACHE_MOD_GET_STATE            (0)
#define RVM_A7M_KERN_CACHE_MOD_SET_STATE            (1)
/* Cache state */
#define RVM_A7M_KERN_CACHE_STATE_DISABLE            (0)
#define RVM_A7M_KERN_CACHE_STATE_ENABLE             (1)
/* Cache maintenance */
#define RVM_A7M_KERN_CACHE_CLEAN_ALL                (0)
#define RVM_A7M_KERN_CACHE_CLEAN_ADDR               (1)
#define RVM_A7M_KERN_CACHE_CLEAN_SET                (2)
#define RVM_A7M_KERN_CACHE_CLEAN_WAY                (3)
#define RVM_A7M_KERN_CACHE_CLEAN_SETWAY             (4)
#define RVM_A7M_KERN_CACHE_INV_ALL                  (5)
#define RVM_A7M_KERN_CACHE_INV_ADDR                 (6)
#define RVM_A7M_KERN_CACHE_INV_SET                  (7)
#define RVM_A7M_KERN_CACHE_INV_WAY                  (8)
#define RVM_A7M_KERN_CACHE_INV_SETWAY               (9)
#define RVM_A7M_KERN_CACHE_CLEAN_INV_ALL            (10)
#define RVM_A7M_KERN_CACHE_CLEAN_INV_ADDR           (11)
#define RVM_A7M_KERN_CACHE_CLEAN_INV_SET            (12)
#define RVM_A7M_KERN_CACHE_CLEAN_INV_WAY            (13)
#define RVM_A7M_KERN_CACHE_CLEAN_INV_SETWAY         (14)
/* Prefetcher modification */
#define RVM_A7M_KERN_PRFTH_MOD_GET_STATE            (0)
#define RVM_A7M_KERN_PRFTH_MOD_SET_STATE            (1)
/* Prefetcher state */
#define RVM_A7M_KERN_PRFTH_STATE_DISABLE            (0)
#define RVM_A7M_KERN_PRFTH_STATE_ENABLE             (1)
/* CPU feature support */
#define RVM_A7M_KERN_CPU_FUNC_CPUID                 (0)
#define RVM_A7M_KERN_CPU_FUNC_ID_PFR0               (1)
#define RVM_A7M_KERN_CPU_FUNC_ID_PFR1               (2)
#define RVM_A7M_KERN_CPU_FUNC_ID_DFR0               (3)
#define RVM_A7M_KERN_CPU_FUNC_ID_AFR0               (4)
#define RVM_A7M_KERN_CPU_FUNC_ID_MMFR0              (5)
#define RVM_A7M_KERN_CPU_FUNC_ID_MMFR1              (6)
#define RVM_A7M_KERN_CPU_FUNC_ID_MMFR2              (7)
#define RVM_A7M_KERN_CPU_FUNC_ID_MMFR3              (8)
#define RVM_A7M_KERN_CPU_FUNC_ID_ISAR0              (9)
#define RVM_A7M_KERN_CPU_FUNC_ID_ISAR1              (10)
#define RVM_A7M_KERN_CPU_FUNC_ID_ISAR2              (11)
#define RVM_A7M_KERN_CPU_FUNC_ID_ISAR3              (12)
#define RVM_A7M_KERN_CPU_FUNC_ID_ISAR4              (13)
#define RVM_A7M_KERN_CPU_FUNC_ID_ISAR5              (14)
#define RVM_A7M_KERN_CPU_FUNC_CLIDR                 (15)
#define RVM_A7M_KERN_CPU_FUNC_CTR                   (16)
#define RVM_A7M_KERN_CPU_FUNC_ICACHE_CCSIDR         (17)
#define RVM_A7M_KERN_CPU_FUNC_DCACHE_CCSIDR         (18)
#define RVM_A7M_KERN_CPU_FUNC_MPU_TYPE              (19)
#define RVM_A7M_KERN_CPU_FUNC_MVFR0                 (20)
#define RVM_A7M_KERN_CPU_FUNC_MVFR1                 (21)
#define RVM_A7M_KERN_CPU_FUNC_MVFR2                 (22)
#define RVM_A7M_KERN_CPU_FUNC_PID0                  (23)
#define RVM_A7M_KERN_CPU_FUNC_PID1                  (24)
#define RVM_A7M_KERN_CPU_FUNC_PID2                  (25)
#define RVM_A7M_KERN_CPU_FUNC_PID3                  (26)
#define RVM_A7M_KERN_CPU_FUNC_PID4                  (27)
#define RVM_A7M_KERN_CPU_FUNC_PID5                  (28)
#define RVM_A7M_KERN_CPU_FUNC_PID6                  (29)
#define RVM_A7M_KERN_CPU_FUNC_PID7                  (30)
#define RVM_A7M_KERN_CPU_FUNC_CID0                  (31)
#define RVM_A7M_KERN_CPU_FUNC_CID1                  (32)
#define RVM_A7M_KERN_CPU_FUNC_CID2                  (33)
#define RVM_A7M_KERN_CPU_FUNC_CID3                  (34)
/* Perfomance counters */
#define RVM_A7M_KERN_PERF_CYCLE_CYCCNT              (0)
/* Performance counter state operations */
#define RVM_A7M_KERN_PERF_STATE_GET                 (0)
#define RVM_A7M_KERN_PERF_STATE_SET                 (1)
/* Performance counter states */
#define RVM_A7M_KERN_PERF_STATE_DISABLE             (0)
#define RVM_A7M_KERN_PERF_STATE_ENABLE              (1)
/* Performance counter value operations */
#define RVM_A7M_KERN_PERF_VAL_GET                   (0)
#define RVM_A7M_KERN_PERF_VAL_SET                   (1)
/* Register read/write */
#define RVM_A7M_KERN_DEBUG_REG_MOD_SP_GET           (0)
#define RVM_A7M_KERN_DEBUG_REG_MOD_SP_SET           (1)
#define RVM_A7M_KERN_DEBUG_REG_MOD_R4_GET           (2)
#define RVM_A7M_KERN_DEBUG_REG_MOD_R4_SET           (3)
#define RVM_A7M_KERN_DEBUG_REG_MOD_R5_GET           (4)
#define RVM_A7M_KERN_DEBUG_REG_MOD_R5_SET           (5)
#define RVM_A7M_KERN_DEBUG_REG_MOD_R6_GET           (6)
#define RVM_A7M_KERN_DEBUG_REG_MOD_R6_SET           (7)
#define RVM_A7M_KERN_DEBUG_REG_MOD_R7_GET           (8)
#define RVM_A7M_KERN_DEBUG_REG_MOD_R7_SET           (9)
#define RVM_A7M_KERN_DEBUG_REG_MOD_R8_GET           (10)
#define RVM_A7M_KERN_DEBUG_REG_MOD_R8_SET           (11)
#define RVM_A7M_KERN_DEBUG_REG_MOD_R9_GET           (12)
#define RVM_A7M_KERN_DEBUG_REG_MOD_R9_SET           (13)
#define RVM_A7M_KERN_DEBUG_REG_MOD_R10_GET          (14)
#define RVM_A7M_KERN_DEBUG_REG_MOD_R10_SET          (15)
#define RVM_A7M_KERN_DEBUG_REG_MOD_R11_GET          (16)
#define RVM_A7M_KERN_DEBUG_REG_MOD_R11_SET          (17)
#define RVM_A7M_KERN_DEBUG_REG_MOD_LR_GET           (18)
#define RVM_A7M_KERN_DEBUG_REG_MOD_LR_SET           (19)
/* FPU register read/write */
#define RVM_A7M_KERN_DEBUG_REG_MOD_S16_GET          (20)
#define RVM_A7M_KERN_DEBUG_REG_MOD_S16_SET          (21)
#define RVM_A7M_KERN_DEBUG_REG_MOD_S17_GET          (22)
#define RVM_A7M_KERN_DEBUG_REG_MOD_S17_SET          (23)
#define RVM_A7M_KERN_DEBUG_REG_MOD_S18_GET          (24)
#define RVM_A7M_KERN_DEBUG_REG_MOD_S18_SET          (25)
#define RVM_A7M_KERN_DEBUG_REG_MOD_S19_GET          (26)
#define RVM_A7M_KERN_DEBUG_REG_MOD_S19_SET          (27)
#define RVM_A7M_KERN_DEBUG_REG_MOD_S20_GET          (28)
#define RVM_A7M_KERN_DEBUG_REG_MOD_S20_SET          (29)
#define RVM_A7M_KERN_DEBUG_REG_MOD_S21_GET          (30)
#define RVM_A7M_KERN_DEBUG_REG_MOD_S21_SET          (31)
#define RVM_A7M_KERN_DEBUG_REG_MOD_S22_GET          (32)
#define RVM_A7M_KERN_DEBUG_REG_MOD_S22_SET          (33)
#define RVM_A7M_KERN_DEBUG_REG_MOD_S23_GET          (34)
#define RVM_A7M_KERN_DEBUG_REG_MOD_S23_SET          (35)
#define RVM_A7M_KERN_DEBUG_REG_MOD_S24_GET          (36)
#define RVM_A7M_KERN_DEBUG_REG_MOD_S24_SET          (37)
#define RVM_A7M_KERN_DEBUG_REG_MOD_S25_GET          (38)
#define RVM_A7M_KERN_DEBUG_REG_MOD_S25_SET          (39)
#define RVM_A7M_KERN_DEBUG_REG_MOD_S26_GET          (40)
#define RVM_A7M_KERN_DEBUG_REG_MOD_S26_SET          (41)
#define RVM_A7M_KERN_DEBUG_REG_MOD_S27_GET          (42)
#define RVM_A7M_KERN_DEBUG_REG_MOD_S27_SET          (43)
#define RVM_A7M_KERN_DEBUG_REG_MOD_S28_GET          (44)
#define RVM_A7M_KERN_DEBUG_REG_MOD_S28_SET          (45)
#define RVM_A7M_KERN_DEBUG_REG_MOD_S29_GET          (46)
#define RVM_A7M_KERN_DEBUG_REG_MOD_S29_SET          (47)
#define RVM_A7M_KERN_DEBUG_REG_MOD_S30_GET          (48)
#define RVM_A7M_KERN_DEBUG_REG_MOD_S30_SET          (49)
#define RVM_A7M_KERN_DEBUG_REG_MOD_S31_GET          (50)
#define RVM_A7M_KERN_DEBUG_REG_MOD_S31_SET          (51)
/* Invocation register read/write */
#define RVM_A7M_KERN_DEBUG_INV_MOD_SP_GET           (0)
#define RVM_A7M_KERN_DEBUG_INV_MOD_SP_SET           (1)
#define RVM_A7M_KERN_DEBUG_INV_MOD_LR_GET           (2)
#define RVM_A7M_KERN_DEBUG_INV_MOD_LR_SET           (3)
/* End Defines ***************************************************************/

/* Structs *******************************************************************/
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
struct RVM_Cop_Struct
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

struct RVM_Regs
{
    struct RVM_Reg_Struct Reg;
    struct RVM_Cop_Struct Cop_Reg;
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
extern void RVM_Putchar(char Char);

extern rvm_ret_t RVM_Svc(rvm_ptr_t Op_Capid, rvm_ptr_t Arg1, rvm_ptr_t Arg2, rvm_ptr_t Arg3);
extern rvm_ret_t RVM_A7M_Svc_Kern(rvm_ptr_t Op_Capid, rvm_ptr_t ID, rvm_ptr_t* Args);
/* Invocation */
extern rvm_ret_t RVM_Inv_Act(rvm_cid_t Cap_Inv, rvm_ptr_t Param, rvm_ptr_t* Retval);
extern rvm_ret_t RVM_Inv_Ret(rvm_ptr_t Retval);
extern rvm_ret_t RVM_Thd_Sched_Rcv(rvm_cid_t Cap_Thd, rvm_ptr_t* Fault);

extern rvm_ptr_t RVM_Fetch_And(rvm_ptr_t* Ptr, rvm_ptr_t Operand);
extern rvm_ptr_t _RVM_MSB_Get(rvm_ptr_t Val);

extern void RVM_Entry(rvm_ptr_t Entry, rvm_ptr_t Stack_Base, rvm_ptr_t Stack_Size, rvm_ptr_t Param);

EXTERN rvm_ret_t RVM_A7M_Kern_Act(rvm_cid_t Cap_Kern, rvm_ptr_t Func_ID, rvm_ptr_t Sub_ID, rvm_ptr_t* Params);
EXTERN rvm_ret_t RVM_A7M_Pgtbl_Entry_Mod(rvm_cid_t Cap_Kern, rvm_cid_t Cap_Pgtbl, rvm_ptr_t Vaddr, rvm_ptr_t Type);
EXTERN rvm_ret_t RVM_A7M_Int_Local_Mod(rvm_cid_t Cap_Kern, rvm_ptr_t Int_Num, rvm_ptr_t Operation, rvm_ptr_t Param);
EXTERN rvm_ret_t RVM_A7M_Int_Local_Trig(rvm_cid_t Cap_Kern, rvm_ptr_t Int_Num);
EXTERN rvm_ret_t RVM_A7M_Cache_Mod(rvm_cid_t Cap_Kern, rvm_ptr_t Cache_ID, rvm_ptr_t Operation, rvm_ptr_t Param);
EXTERN rvm_ret_t RVM_A7M_Cache_Maint(rvm_cid_t Cap_Kern, rvm_ptr_t Cache_ID, rvm_ptr_t Operation, rvm_ptr_t Param);
EXTERN rvm_ret_t RVM_A7M_Prfth_Mod(rvm_cid_t Cap_Kern, rvm_ptr_t Prfth_ID, rvm_ptr_t Operation, rvm_ptr_t Param);
EXTERN rvm_ret_t RVM_A7M_Perf_CPU_Func(rvm_cid_t Cap_Kern, rvm_ptr_t Freg_ID, rvm_ptr_t* Content);
EXTERN rvm_ret_t RVM_A7M_Perf_Mon_Mod(rvm_cid_t Cap_Kern, rvm_ptr_t Perf_ID, rvm_ptr_t Operation, rvm_ptr_t Param);
EXTERN rvm_ret_t RVM_A7M_Perf_Cycle_Mod(rvm_cid_t Cap_Kern, rvm_ptr_t Cycle_ID, 
                                        rvm_ptr_t Operation, rvm_ptr_t Value, rvm_ptr_t* Content);
EXTERN rvm_ret_t RVM_A7M_Debug_Print(rvm_cid_t Cap_Kern, char Char);
EXTERN rvm_ret_t RVM_A7M_Debug_Reg_Mod(rvm_cid_t Cap_Kern, rvm_cid_t Cap_Thd, 
                                       rvm_ptr_t Operation, rvm_ptr_t Value, rvm_ptr_t* Content);
EXTERN rvm_ret_t RVM_A7M_Debug_Inv_Mod(rvm_cid_t Cap_Kern, rvm_cid_t Cap_Thd, 
                                       rvm_ptr_t Layer, rvm_ptr_t Operation, rvm_ptr_t Value, rvm_ptr_t* Content);
/*****************************************************************************/
#endif /* __RVM_GUEST_A7M__ */
/* End Public C Function Prototypes ******************************************/

/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/

