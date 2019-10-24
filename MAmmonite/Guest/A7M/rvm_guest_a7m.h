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
#define EXTERN                                  extern
/* The order of bits in one CPU machine word */
#define RVM_WORD_ORDER                          5

/* Size of kernel objects - architecture dependent */
/* Thread */
#define RVM_THD_WORD_SIZE                       46
/* Normal page directory */
#define RVM_PGTBL_WORD_SIZE_NOM(NUM_ORDER)      ((((rvm_ptr_t)1)<<(NUM_ORDER))+5)
/* Top-level page directory */
#define RVM_PGTBL_WORD_SIZE_TOP(NUM_ORDER)      (RVM_PGTBL_SIZE_NOM(NUM_ORDER)+5+1+2*RVM_MPU_REGIONS)
/* End System macros *********************************************************/
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

EXTERN void RVM_Entry(rvm_ptr_t Entry, rvm_ptr_t Stack_Base, rvm_ptr_t Stack_Size, rvm_ptr_t Param);
/*****************************************************************************/
#endif /* __RVM_GUEST_A7M__ */
/* End Public C Function Prototypes ******************************************/

/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/

