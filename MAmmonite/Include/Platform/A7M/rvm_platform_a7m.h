/******************************************************************************
Filename    : rvm_platform_cmx.h
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
#define EXTERN                              extern
/* The order of bits in one CPU machine word */
#define RVM_WORD_ORDER                      5
/* The stack safe redundancy size in words - set to 0x20 words */
#define RVM_STACK_SAFE_SIZE                 0x20
/* FPU type definitions - keep in accordance with kernel */
#define RVM_A7M_FPU_NONE                    0
#define RVM_A7M_FPU_VFPV4                   1
#define RVM_A7M_FPU_FPV5_SP                 2
#define RVM_A7M_FPU_FPV5_DP                 3

/* The size of page tables on this platform, in machine words */
/* Normal page directory */
#define RVM_PGTBL_WORD_SIZE_NOM(NUM_ORDER)   ((((rvm_ptr_t)1)<<(NUM_ORDER))+5)
/* Top-level page directory */
#define RVM_PGTBL_WORD_SIZE_TOP(NUM_ORDER)   (RVM_PGTBL_SIZE_NOM(NUM_ORDER)+11)

/* Kernel functions standard to Cortex-M, interrupt management and power */
#define RVM_A7M_KERN_INT(X)                  (X)
#define RVM_A7M_INT_OP                       0
#define RVM_A7M_INT_ENABLE                   1
#define RVM_A7M_INT_DISABLE                  0
#define RVM_A7M_INT_PRIO                     1
#define RVM_A7M_KERN_PWR                     240

/* Platform-specific includes */
#include "Platform/A7M/rvm_platform_a7m_conf.h"

/* The boot-time allocation frontier */
#define RVM_KMEM_BOOT_FRONTIER               RVM_A7M_KMEM_BOOT_FRONTIER
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

struct RVM_Flags
{
    struct RVM_Flag_Set Set0;
    struct RVM_Flag_Set Set1;
};

/* The page table layout is statically decided, the first one being the top-level */
struct RVM_Hdr_Pgtbl
{
    /* Its parent's position */
    rvm_ptr_t Parent;
    /* The start address of the mapping */
    rvm_ptr_t Addr;
    /* The size order and number order, combined into one word */
    rvm_ptr_t Order;
    /* The initially mapped pages' data. The only thing stored here
     * is the RVM standard flags */
    rvm_u8_t Flags[8];
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
static void __RVM_Pgtbl_Map(struct RVM_Hdr_Pgtbl* Pgtbl, rvm_ptr_t Pos, rvm_cid_t Pgtbl_Cap);
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
/* Invocation */
EXTERN rvm_ret_t RVM_Inv_Act(rvm_cid_t Cap_Inv, rvm_ptr_t Param, rvm_ptr_t* Retval);
EXTERN rvm_ret_t RVM_Inv_Ret(rvm_ptr_t Retval);
/* Character printing */
__EXTERN__ rvm_ptr_t RVM_Putchar(char Char);
/* Stack operations */
__EXTERN__ rvm_ptr_t RVM_Stack_Init(rvm_ptr_t Stack, rvm_ptr_t Size);
/* Check the page table to see if the access is allowed */
__EXTERN__ rvm_ret_t _RVM_Pgtbl_Check(const struct RVM_Hdr_Pgtbl* Pgtbl, rvm_ptr_t Pgtbl_Num, void* Addr, rvm_ptr_t Size);
/* Setup the page table */
__EXTERN__ void _RVM_Pgtbl_Setup(struct RVM_Hdr_Pgtbl* Pgtbl, rvm_ptr_t Pos, 
                                 rvm_cid_t Cap_Captbl, rvm_ptr_t* Cap_Bump,
                                 rvm_cid_t Cap_Kmem, rvm_ptr_t* Kmem_Bump);
/* Idle function */
__EXTERN__ void RVM_Idle(void);
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
