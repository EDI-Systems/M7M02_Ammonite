/******************************************************************************
Filename    : rvm_platform_a7m.h
Author      : pry
Date        : 25/06/2017
Licence     : The Unlicense; see LICENSE for details.
Description : The header of the platform dependent part.
******************************************************************************/

/* Defines *******************************************************************/
#ifdef __HDR_DEFS__
#ifndef __RVM_PLATFORM_A7M_H_DEFS__
#define __RVM_PLATFORM_A7M_H_DEFS__
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
#define EXTERN                                  extern
/* The order of bits in one CPU machine word */
#define RVM_WORD_ORDER                          (5)
/* FPU type definitions - keep in accordance with kernel */
#define RVM_A7M_FPU_NONE                        (0)
#define RVM_A7M_FPU_FPV4                        (1)
#define RVM_A7M_FPU_FPV5_SP                     (2)
#define RVM_A7M_FPU_FPV5_DP                     (3)

/* The size of page tables on this platform, in machine words */
/* Normal page directory */
#define RVM_PGTBL_WORD_SIZE_NOM(NUM_ORDER)      ((((rvm_ptr_t)1)<<(NUM_ORDER))+5)
/* Top-level page directory */
#define RVM_PGTBL_WORD_SIZE_TOP(NUM_ORDER)      (RVM_PGTBL_SIZE_NOM(NUM_ORDER)+5+1+2*RVM_A7M_MPU_REGIONS)

#define RVM_A7M_REG(X)                          (*((volatile rvm_ptr_t*)(X)))
#define RVM_A7M_REGB(X)                         (*((volatile rvm_u8_t*)(X)))

/* Platform-specific includes */
#include "Platform/A7M/rvm_platform_a7m_conf.h"

/* The number of vectors for this particular chip */
#define RVM_VECT_NUM                            (RVM_A7M_VECT_NUM)

/* The boot-time kernel capability allocation frontier */
#define RVM_CAP_BOOT_FRONTIER                   RVM_A7M_CAP_BOOT_FRONTIER
/* The boot-time kernel memory allocation frontier */
#define RVM_KMEM_BOOT_FRONTIER                  RVM_A7M_KMEM_BOOT_FRONTIER

/* Fault reasons */
/* Debug event has occurred. The Debug Fault Status Register has been updated */
#define RVM_A7M_HFSR_DEBUGEVT                   (1U<<31)
/* Processor has escalated a configurable-priority exception to HardFault */
#define RVM_A7M_HFSR_FORCED                     (1<<30)
/* Vector table read fault has occurred */
#define RVM_A7M_HFSR_VECTTBL                    (1<<1)
/* Divide by zero */
#define RVM_A7M_UFSR_DIVBYZERO                  (1<<25)
/* Unaligned load/store access */
#define RVM_A7M_UFSR_UNALIGNED                  (1<<24)
/* No such coprocessor */
#define RVM_A7M_UFSR_NOCP                       (1<<19)
/* Invalid vector return LR or PC value */
#define RVM_A7M_UFSR_INVPC                      (1<<18)
/* Attempt to enter an invalid instruction set (ARM) state */
#define RVM_A7M_UFSR_INVSTATE                   (1<<17)
/* Invalid IT instruction or related instructions */
#define RVM_A7M_UFSR_UNDEFINSTR                 (1<<16)
/* The Bus Fault Address Register (BFAR) is valid */
#define RVM_A7M_BFSR_BFARVALID                  (1<<15)
/* The bus fault happened during FP lazy stacking */
#define RVM_A7M_BFSR_LSPERR                     (1<<13)
/* A derived bus fault has occurred on exception entry */
#define RVM_A7M_BFSR_STKERR                     (1<<12)
/* A derived bus fault has occurred on exception return */
#define RVM_A7M_BFSR_UNSTKERR                   (1<<11)
/* Imprecise data access error has occurred */
#define RVM_A7M_BFSR_IMPRECISERR                (1<<10)
/* Precise data access error has occurred, BFAR updated */
#define RVM_A7M_BFSR_PRECISERR                  (1<<9)
/* A bus fault on an instruction prefetch has occurred. The 
 * fault is signaled only if the instruction is issued */
#define RVM_A7M_BFSR_IBUSERR                    (1<<8)
/* The Memory Mnagement Fault Address Register have valid contents */
#define RVM_A7M_MFSR_MMARVALID                  (1<<7)
/* A MemManage fault occurred during FP lazy state preservation */
#define RVM_A7M_MFSR_MLSPERR                    (1<<5)
/* A derived MemManage fault occurred on exception entry */
#define RVM_A7M_MFSR_MSTKERR                    (1<<4)
/* A derived MemManage fault occurred on exception return */
#define RVM_A7M_MFSR_MUNSTKERR                  (1<<3)
/* Data access violation. The MMFAR shows the data address that
 * the load or store tried to access */
#define RVM_A7M_MFSR_DACCVIOL                   (1<<1)
/* MPU or Execute Never (XN) default memory map access violation on an
 * instruction fetch has occurred. The fault is signalled only if the
 * instruction is issued */
#define RVM_A7M_MFSR_IACCVIOL                   (1<<0)

/* Platform-specific kernel function macros **********************************/
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
/* Begin Extended Types ******************************************************/
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
/* __RVM_PLATFORM_A7M_H_DEFS__ */
#endif
/* __HDR_DEFS__ */
#endif
/* End Defines ***************************************************************/

/* Structs *******************************************************************/
#ifdef __HDR_STRUCTS__
#ifndef __RVM_PLATFORM_A7M_H_STRUCTS__
#define __RVM_PLATFORM_A7M_H_STRUCTS__
/* We used structs in the header */

/* Use defines in these headers */
#define __HDR_DEFS__
#undef __HDR_DEFS__
/*****************************************************************************/
struct RVM_A7M_Ret_Stack
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

/* Interrupt flags */
struct RVM_Flag_Set
{
    rvm_ptr_t Lock;
    rvm_ptr_t Group;
    rvm_ptr_t Flags[32];
};

struct RVM_Phys_Flags
{
    struct RVM_Flag_Set Set0;
    struct RVM_Flag_Set Set1;
};
/*****************************************************************************/
/* __RVM_PLATFORM_A7M_H_STRUCTS__ */
#endif
/* __HDR_STRUCTS__ */
#endif
/* End Structs ***************************************************************/

/* Private Global Variables **************************************************/
#if(!(defined __HDR_DEFS__||defined __HDR_STRUCTS__))
#ifndef __RVM_PLATFORM_A7M_MEMBERS__
#define __RVM_PLATFORM_A7M_MEMBERS__

/* In this way we can use the data structures and definitions in the headers */
#define __HDR_DEFS__

#undef __HDR_DEFS__

#define __HDR_STRUCTS__

#undef __HDR_STRUCTS__

/* If the header is not used in the public mode */
#ifndef __HDR_PUBLIC_MEMBERS__
/*****************************************************************************/

/*****************************************************************************/
/* End Private Global Variables **********************************************/

/* Private C Function Prototypes *********************************************/ 
/*****************************************************************************/

/*****************************************************************************/
#define __EXTERN__
/* End Private C Function Prototypes *****************************************/

/* Public Global Variables ***************************************************/
/* __HDR_PUBLIC_MEMBERS__ */
#else
#define __EXTERN__ EXTERN 
/* __HDR_PUBLIC_MEMBERS__ */
#endif

/*****************************************************************************/

/*****************************************************************************/

/* End Public Global Variables ***********************************************/

/* Public C Function Prototypes **********************************************/
/*****************************************************************************/
/* Stubs in assembly */
EXTERN void _RVM_Entry(void);
EXTERN void _RVM_Jmp_Stub(void);
EXTERN rvm_ptr_t _RVM_MSB_Get(rvm_ptr_t Val); 
EXTERN rvm_ret_t RVM_Svc(rvm_ptr_t Op_Capid, rvm_ptr_t Arg1, rvm_ptr_t Arg2, rvm_ptr_t Arg3);
EXTERN rvm_ret_t RVM_A7M_Svc_Kern(rvm_ptr_t Op_Capid, rvm_ptr_t ID, rvm_ptr_t* Args);
/* Thread scheduler notifications */
EXTERN rvm_ret_t RVM_Thd_Sched_Rcv(rvm_cid_t Cap_Thd, rvm_ptr_t* Fault);
/* Invocation */
EXTERN rvm_ret_t RVM_Inv_Act(rvm_cid_t Cap_Inv, rvm_ptr_t Param, rvm_ptr_t* Retval);
EXTERN rvm_ret_t RVM_Inv_Ret(rvm_ptr_t Retval);
/* Character printing */
__EXTERN__ rvm_ptr_t RVM_Putchar(char Char);
/* Stack operations */
__EXTERN__ rvm_ptr_t RVM_Stack_Init(rvm_ptr_t Stack_Base, rvm_ptr_t Stack_Size,
                                    rvm_ptr_t Entry_Addr, rvm_ptr_t Stub_Addr);
/* Idle function */
__EXTERN__ void RVM_Idle(void);
/* Print fault */
__EXTERN__ void RVM_Thd_Print_Fault(rvm_ptr_t Fault);
/* Print registers */
__EXTERN__ rvm_ret_t RVM_Thd_Print_Regs(rvm_cid_t Cap_Thd);
/*****************************************************************************/
/* Undefine "__EXTERN__" to avoid redefinition */
#undef __EXTERN__
/* __RVM_PLATFORM_A7M_MEMBERS__ */
#endif
/* !(defined __HDR_DEFS__||defined __HDR_STRUCTS__) */
#endif
/* End Public C Function Prototypes ******************************************/

/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
