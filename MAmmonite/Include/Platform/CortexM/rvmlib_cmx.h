/******************************************************************************
Filename    : rvmlib_cmx.h
Author      : pry
Date        : 25/06/2017
Licence     : LGPL v3+; see COPYING for details.
Description : The header of the platform dependent part.
******************************************************************************/

/* Defines *******************************************************************/
#ifdef __HDR_DEFS__
#ifndef __RVMLIB_CMX_H_DEFS__
#define __RVMLIB_CMX_H_DEFS__
/*****************************************************************************/
/* Definitions of basic types */
/* Basic Types ***************************************************************/
#ifndef __S32__
#define __S32__
typedef signed int  s32;
#endif

#ifndef __S16__
#define __S16__
typedef signed short s16;
#endif

#ifndef __S8__
#define __S8__
typedef signed char  s8;
#endif

#ifndef __U64__
#define __U64__
typedef unsigned long long u64;
#endif

#ifndef __U32__
#define __U32__
typedef unsigned int  u32;
#endif

#ifndef __U16__
#define __U16__
typedef unsigned short u16;
#endif

#ifndef __U8__
#define __U8__
typedef unsigned char  u8;
#endif
/* End Basic Types ***********************************************************/
/* EXTERN keyword definition */
#define EXTERN                              extern
/* The order of bits in one CPU machine word */
#define RVM_WORD_ORDER           5
/* The stack safe redundancy size in words - set to 0x20 words */
#define RVM_STACK_SAFE_SIZE                 0x20
/* FPU type definitions - keep in accordance with kernel */
#define RVM_CMX_FPU_NONE                    0
#define RVM_CMX_FPU_VFPV4                   1
#define RVM_CMX_FPU_FPV5_SP                 2
#define RVM_CMX_FPU_FPV5_DP                 3

/* The size of page tables on this platform, in machine words */
/* Normal page directory */
#define RVM_PGTBL_WORD_SIZE_NOM(NUM_ORDER)   ((((ptr_t)1)<<(NUM_ORDER))+5)
/* Top-level page directory */
#define RVM_PGTBL_WORD_SIZE_TOP(NUM_ORDER)   (RVM_PGTBL_SIZE_NOM(NUM_ORDER)+11)

/* Kernel functions standard to Cortex-M, interrupt management and power */
#define RVM_CMX_KERN_INT(X)                  (X)
#define RVM_CMX_INT_OP                       0
#define RVM_CMX_INT_ENABLE                   1
#define RVM_CMX_INT_DISABLE                  0
#define RVM_CMX_INT_PRIO                     1
#define RVM_CMX_KERN_PWR                     240

/* Platform-specific includes */
#include "Platform\CortexM\rvmlib_cmx_conf.h"

/* The boot-time allocation frontier */
#define RVM_KMEM_BOOT_FRONTIER               RVM_CMX_KMEM_BOOT_FRONTIER

/* Maximum time allowed to set as period */
#define RVM_MAX_PERIOD                       10000
/* Minimum time allowed to set as period */
#define RVM_MIN_PERIOD                       1
/* The default value for timer interrupt frequency for a VM */
#define RVM_DEF_PERIOD                       1
/* Begin Extended Types ******************************************************/
#ifndef __PID_T__
#define __PID_T__
/* The typedef for the Process ID */
typedef s32 pid_t;
#endif

#ifndef __TID_T__
#define __TID_T__
/* The typedef for the Thread ID */
typedef s32 tid_t;
#endif

#ifndef __PTR_T__
#define __PTR_T__
/* The typedef for the pointers - This is the raw style. Pointers must be unsigned */
typedef u32 ptr_t;
#endif

#ifndef __CNT_T__
#define __CNT_T__
/* The typedef for the count variables */
typedef s32 cnt_t;
#endif

#ifndef __CID_T__
#define __CID_T__
/* The typedef for capability ID */
typedef s32 cid_t;
#endif

#ifndef __RET_T__
#define __RET_T__
/* The type for process return value */
typedef s32 ret_t;
#endif
/* End Extended Types ********************************************************/
/*****************************************************************************/
/* __RVMLIB_CMX_H_DEFS__ */
#endif
/* __HDR_DEFS__ */
#endif
/* End Defines ***************************************************************/

/* Structs *******************************************************************/
#ifdef __HDR_STRUCTS__
#ifndef __RVMLIB_CMX_H_STRUCTS__
#define __RVMLIB_CMX_H_STRUCTS__
/* We used structs in the header */

/* Use defines in these headers */
#define __HDR_DEFS__
#undef __HDR_DEFS__
/*****************************************************************************/
struct RVM_CMX_Ret_Stack
{
    /* Normal registers */
    ptr_t R0;
    ptr_t R1;
    ptr_t R2;
    ptr_t R3;
    ptr_t R12;
    ptr_t LR;
    ptr_t PC;
    ptr_t XPSR;
    /* FPU registers follow - no matter they are used or not, we reserve the space
     * in stack creation */
    ptr_t S0;
    ptr_t S1;
    ptr_t S2;
    ptr_t S3;
    ptr_t S4;
    ptr_t S5;
    ptr_t S6;
    ptr_t S7;
    ptr_t S8;
    ptr_t S9;
    ptr_t S10;
    ptr_t S11;
    ptr_t S12;
    ptr_t S13;
    ptr_t S14;
    ptr_t S15;
};

struct RVM_Reg_Struct
{
    ptr_t SP;
    ptr_t R4;
    ptr_t R5;
    ptr_t R6;
    ptr_t R7;
    ptr_t R8;
    ptr_t R9;
    ptr_t R10;
    ptr_t R11;
    ptr_t LR;
};

/* The coprocessor register set structure. In Cortex-M, if there is a 
 * single-precision FPU, then the FPU S0-S15 is automatically pushed */
struct RVM_Cop_Struct
{
    ptr_t S16;
    ptr_t S17;
    ptr_t S18;
    ptr_t S19;
    ptr_t S20;
    ptr_t S21;
    ptr_t S22;
    ptr_t S23;
    ptr_t S24;
    ptr_t S25;
    ptr_t S26;
    ptr_t S27;
    ptr_t S28;
    ptr_t S29;
    ptr_t S30;
    ptr_t S31;
};

struct RVM_Regs
{
    struct RVM_Reg_Struct Reg;
    struct RVM_Cop_Struct Cop_Reg;
};

/* Interrupt flags */
struct RVM_Flag_Set
{
    ptr_t Lock;
    ptr_t Group;
    ptr_t Flags[32];
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
    ptr_t Parent;
    /* The start address of the mapping */
    ptr_t Addr;
    /* The size order and number order, combined into one word */
    ptr_t Order;
    /* The initially mapped pages' data. The only thing stored here
     * is the RVM standard flags */
    u8 Flags[8];
};
/*****************************************************************************/
/* __RVMLIB_CMX_H_STRUCTS__ */
#endif
/* __HDR_STRUCTS__ */
#endif
/* End Structs ***************************************************************/

/* Private Global Variables **************************************************/
#if(!(defined __HDR_DEFS__||defined __HDR_STRUCTS__))
#ifndef __RVMLIB_CMX_MEMBERS__
#define __RVMLIB_CMX_MEMBERS__

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
static void __RVM_Pgtbl_Map(struct RVM_Hdr_Pgtbl* Pgtbl, ptr_t Pos, cid_t Pgtbl_Cap);
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
EXTERN ptr_t* _RVM_Get_TLS_Pos(ptr_t Mask);
EXTERN void _RVM_Thd_Stub(void);
EXTERN void _RVM_Inv_Stub(void);
EXTERN ptr_t _RVM_MSB_Get(ptr_t Val); 
EXTERN ret_t RVM_Svc(ptr_t Op_Capid, ptr_t Arg1, ptr_t Arg2, ptr_t Arg3);
EXTERN ret_t RVM_Svc_Thd_Exec_Get(ptr_t Op_Capid, ptr_t* Param1, ptr_t* Param2, ptr_t* Param3);
/* Character printing */
__EXTERN__ ptr_t _RVM_Putchar(char Char);
/* Stack operations */
__EXTERN__ ptr_t _RVM_Stack_Init(ptr_t Stack, ptr_t Size, ptr_t Stub,
                                 ptr_t Param1, ptr_t Param2, ptr_t Param3, ptr_t Param4);
/* Check the page table to see if the access is allowed */
__EXTERN__ ret_t _RVM_Pgtbl_Check(const struct RVM_Hdr_Pgtbl* Pgtbl, ptr_t Pgtbl_Num, void* Addr, ptr_t Size);
/* Setup the page table */
__EXTERN__ void _RVM_Pgtbl_Setup(struct RVM_Hdr_Pgtbl* Pgtbl, cid_t Cap_Captbl, ptr_t* Cap_Bump,
                                 cid_t Cap_Kmem, ptr_t* Kmem_Bump, ptr_t Pos);
/* Idle function */
__EXTERN__ void _RVM_Idle(void);
/*****************************************************************************/
/* Undefine "__EXTERN__" to avoid redefinition */
#undef __EXTERN__
/* __RVMLIB_CMX_MEMBERS__ */
#endif
/* !(defined __HDR_DEFS__||defined __HDR_STRUCTS__) */
#endif
/* End Public C Function Prototypes ******************************************/

/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
