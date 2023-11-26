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
#define RVM_MSB_GET(VAL)                            __RVM_RV32P_MSB_Get(VAL)
/* FPU type definitions - keep in accordance with kernel */
#define RVM_RV32P_ATTR_NONE                         (0U)
#define RVM_RV32P_ATTR_RVF                          (1U<<0)
#define RVM_RV32P_ATTR_RVFD                         (1U<<1)

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

/* Platform-specific kernel function macros **********************************/

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
/* Stubs in assembly */
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
__EXTERN__ rvm_ptr_t RVM_Stack_Init(rvm_ptr_t Stack_Base,
                                    rvm_ptr_t Stack_Size,
                                    rvm_ptr_t Entry_Addr,
                                    rvm_ptr_t Stub_Addr);
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

