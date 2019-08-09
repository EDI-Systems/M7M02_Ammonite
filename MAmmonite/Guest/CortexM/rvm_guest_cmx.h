/******************************************************************************
Filename    : rvm_guest_cmx.h
Author      : pry
Date        : 01/04/2017
Licence     : The Unlicense; see LICENSE for details.
Description : The header of platform-specific part of RMP for RVM.
******************************************************************************/

/* Begin Typedefs ************************************************************/
#ifndef __RVM_GUEST_CMX__
#define __RVM_GUEST_CMX__

#ifndef __RVM_PTR_T__
#define __RVM_PTR_T__
/* The typedef for the pointers - This is the raw style. Pointers must be unsigned */
typedef unsigned int rvm_ptr_t;
#endif

#ifndef __RVM_CNT_T__
#define __RVM_CNT_T__
/* The typedef for the count variables */
typedef signed int rvm_cnt_t;
#endif

#ifndef __RVM_RET_T__
#define __RVM_RET_T__
/* The type for process return value */
typedef signed int rvm_ret_t;
#endif
/* End Typedefs **************************************************************/

/* Begin Defines *************************************************************/
/* System macros *************************************************************/
/* Compiler "extern" keyword setting */
#define EXTERN                   extern
/* The order of bits in one CPU machine word */
#define RVM_WORD_ORDER           5
/* End System macros *********************************************************/

/* Hypervisor macros *********************************************************/
#define RVM_MAGIC                0x56495254
#define RVM_VECT_BITMAP          (((RVM_MAX_INTVECT-1)>>RVM_WORD_ORDER)+1)

/* Hypercalls */
/* Enable interrupts */
#define RVM_HYP_ENAINT           0
/* Disable interrupts */
#define RVM_HYP_DISINT           1
/* Register a vector */
#define RVM_HYP_REGINT           2
/* Delete a vector */
#define RVM_HYP_DELINT           3
/* Register a event */
#define RVM_HYP_REGEVT           4
/* Delete a event */
#define RVM_HYP_DELEVT           5
/* Wait for an interrupt to come */
#define RVM_HYP_WAITEVT          6
/* Trigger a vector */
#define RVM_HYP_SENDEVT          7
/* Returning from a vector */
#define RVM_HYP_QUERYEVT         8
/* Query the VM's ID given its name */
#define RVM_HYP_QUERYVM          9
/* Send to another VM */
#define RVM_HYP_TIMPROG          10
/* Print to the console */
#define RVM_HYP_PRINT            11

/* System stack size when entering the system - do not need to be changed in most cases */
#define RMP_INT_STACK_ADDR           (((rvm_ptr_t)&__initial_sp)+0x400-64*sizeof(rvm_ptr_t))
    
/* Generic page table flags */
#define RME_PGTBL_ORDER(SIZE,NUM)    (((SIZE)<<(sizeof(rvm_ptr_t)*4))|(NUM))
#define RME_PGTBL_READ               (1<<0)
#define RME_PGTBL_WRITE              (1<<1)
#define RME_PGTBL_EXECUTE            (1<<2)
#define RME_PGTBL_CACHEABLE          (1<<3)
#define RME_PGTBL_BUFFERABLE         (1<<4)
#define RME_PGTBL_STATIC             (1<<5)

#define RME_PGTBL_ALL_PERM           (RME_PGTBL_READ|RME_PGTBL_WRITE|RME_PGTBL_EXECUTE| \
                                      RME_PGTBL_CACHEABLE|RME_PGTBL_BUFFERABLE|RME_PGTBL_STATIC)
                                        
/* Generic page size order definitions */
#define RME_PGTBL_SIZE_2B            (1)
#define RME_PGTBL_SIZE_4B            (2)
#define RME_PGTBL_SIZE_8B            (3)
#define RME_PGTBL_SIZE_16B           (4)
#define RME_PGTBL_SIZE_32B           (5)
#define RME_PGTBL_SIZE_64B           (6)
#define RME_PGTBL_SIZE_128B          (7)
#define RME_PGTBL_SIZE_256B          (8)
#define RME_PGTBL_SIZE_512B          (9)
#define RME_PGTBL_SIZE_1K            (10)
#define RME_PGTBL_SIZE_2K            (11)
#define RME_PGTBL_SIZE_4K            (12)
#define RME_PGTBL_SIZE_8K            (13)
#define RME_PGTBL_SIZE_16K           (14)
#define RME_PGTBL_SIZE_32K           (15)
#define RME_PGTBL_SIZE_64K           (16)
#define RME_PGTBL_SIZE_128K          (17)
#define RME_PGTBL_SIZE_256K          (18)
#define RME_PGTBL_SIZE_512K          (19)
#define RME_PGTBL_SIZE_1M            (20)
#define RME_PGTBL_SIZE_2M            (21)
#define RME_PGTBL_SIZE_4M            (22)
#define RME_PGTBL_SIZE_8M            (23)
#define RME_PGTBL_SIZE_16M           (24)
#define RME_PGTBL_SIZE_32M           (25)
#define RME_PGTBL_SIZE_64M           (26)
#define RME_PGTBL_SIZE_128M          (27)
#define RME_PGTBL_SIZE_256M          (28)
#define RME_PGTBL_SIZE_512M          (29)
#define RME_PGTBL_SIZE_1G            (30)
#define RME_PGTBL_SIZE_2G            (31)
#define RME_PGTBL_SIZE_4G            (32)

/* Generic page table entry number definitions */
#define RME_PGTBL_NUM_2             (1)
#define RME_PGTBL_NUM_4             (2)
#define RME_PGTBL_NUM_8             (3)
#define RME_PGTBL_NUM_16            (4)
#define RME_PGTBL_NUM_32            (5)
#define RME_PGTBL_NUM_64            (6)
#define RME_PGTBL_NUM_128           (7)
#define RME_PGTBL_NUM_256           (8)
#define RME_PGTBL_NUM_512           (9)
#define RME_PGTBL_NUM_1K            (10)
#define RME_PGTBL_NUM_2K            (11)
#define RME_PGTBL_NUM_4K            (12)
#define RME_PGTBL_NUM_8K            (13)
#define RME_PGTBL_NUM_16K           (14)
#define RME_PGTBL_NUM_32K           (15)
#define RME_PGTBL_NUM_64K           (16)
#define RME_PGTBL_NUM_128K          (17)
#define RME_PGTBL_NUM_256K          (18)
#define RME_PGTBL_NUM_512K          (19)
#define RME_PGTBL_NUM_1M            (20)
#define RME_PGTBL_NUM_2M            (21)
#define RME_PGTBL_NUM_4M            (22)

/* Kernel functions standard to Cortex-M, interrupt management and power.
 * We do not use subfunction numbers for Cortex-M */
#define RVM_CMX_KERN_INT(X)         (X)
#define RVM_CMX_INT_OP              0
#define RVM_CMX_INT_ENABLE          1
#define RVM_CMX_INT_DISABLE         0
#define RVM_CMX_INT_PRIO            1
/* End Hypervisor macros *****************************************************/
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

/* We used structs in the header */
struct RVM_Param
{
    rvm_ptr_t Number;
    rvm_ptr_t Param[4];
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
    unsigned char Flags[8];
};

/* The image structure */
struct RVM_Image
{
    /* The magic number;always equal to VIRT(0x56495254) */
    const rvm_ptr_t Magic;
    /* The name is always 16 bytes long. This is fixed and is not changeable */
    char Name[16];

    /* The entry, stack and stack size of the virtual machines */
    void* User_Entry;
    void* User_Stack;
    rvm_ptr_t User_Size;
    void* Int_Entry;
    void* Int_Stack;
    rvm_ptr_t Int_Size;
    
    /* The parameter space and register set space */
    struct RVM_Param* Param;
    struct RVM_Regs* Regs;
    
    /* The priority and timeslices */
    rvm_ptr_t Prio;
    rvm_ptr_t Slices;
    
    /* The console space and size */
    rvm_ptr_t Console_Size;
    void* Console_Buf;
    
    /* The interrupt space */
    rvm_ptr_t Int_Num;
    rvm_ptr_t* Int_Flags;

    /* The number of page tables in this image, and where are they stored */
    rvm_ptr_t Pgtbl_Num;
    const struct RVM_Hdr_Pgtbl* Pgtbl;
    
    /* The number of kernel capabilities in this image, and their list */
    rvm_ptr_t Kcap_Num;
    const rvm_ptr_t* Kcap;

    /* Is there any other images? If there is, here is a pointer to the start
     * of the next one. This is a constant pointer to a constant pointer to a 
     * constant structure */
    const struct RVM_Image* const * const Next_Image;
};

#include "rvm_guest_cmx_conf.h"
/*****************************************************************************/
/* End Structs ***************************************************************/

/* Private C Function Prototypes *********************************************/ 
/*****************************************************************************/

/*****************************************************************************/
/* End Private C Function Prototypes *****************************************/

/*****************************************************************************/
#ifdef __HDR_PRIVATE_MEMBERS__
#undef EXTERN
#define EXTERN
static rvm_ret_t RVM_Hypercall(rvm_ptr_t Number, rvm_ptr_t Param1, rvm_ptr_t Param2, rvm_ptr_t Param3, rvm_ptr_t Param4);
static rvm_ret_t RVM_Get_Int(void);
static void RVM_Int(void);
static rvm_ptr_t RVM_Int_Enable;
static rvm_ptr_t RVM_Int_Pend;
static rvm_ptr_t RVM_Int_Active;
#endif

/* The debug console buffer and pointer */
EXTERN char RVM_Console[RVM_DEBUG_MAX_STR];

/* The initial stack pointer as defined in assembly */
extern rvm_ptr_t RVM_User_Stack;
extern rvm_ptr_t RVM_Int_Stack;
/* The hypercall parameter& structure */
EXTERN struct RVM_Param RVM_Param;
EXTERN struct RVM_Regs RVM_Regs;

/* The interrupt handler registration table */
EXTERN rvm_ptr_t RVM_Vect[RVM_MAX_INTVECT];
/* The interrupt flag registration table */
EXTERN rvm_ptr_t RVM_Flag[RVM_VECT_BITMAP];
/*****************************************************************************/

/* End Public Global Variables ***********************************************/

/* Public C Function Prototypes **********************************************/
/*****************************************************************************/
extern rvm_ptr_t RVM_Fetch_And(rvm_ptr_t* Ptr, rvm_ptr_t Operand);
extern rvm_ptr_t RVM_Fetch_Or(rvm_ptr_t* Ptr, rvm_ptr_t Operand);
extern void RVM_Int_Rcv(void);
extern rvm_ptr_t RVM_MSB_Get(rvm_ptr_t Val);
extern void _RVM_Yield(void);
extern void _RVM_Hypercall(void);
extern void _RVM_Entry(void);
extern rvm_ret_t _RVM_Kern(rvm_cnt_t Capid, rvm_ptr_t Func_ID, rvm_ptr_t Param1, rvm_ptr_t Param2);

/* Hypercalls */
EXTERN void RVM_Init(void);
EXTERN rvm_ret_t RVM_Vect_Init(rvm_ptr_t Num, void* Handler);
EXTERN void RVM_Disable_Int(void);
EXTERN void RVM_Enable_Int(void);
EXTERN void RVM_Mask_Int(void);
EXTERN void RVM_Unmask_Int(void);
EXTERN rvm_ret_t RVM_Reg_Int(rvm_ptr_t Vect_Num, rvm_ptr_t Int_Num);
EXTERN rvm_ret_t RVM_Del_Int(rvm_cnt_t Int_ID);
EXTERN rvm_ret_t RVM_Reg_Evt(rvm_ptr_t Int_Num, rvm_ptr_t VMID);
EXTERN rvm_ret_t RVM_Del_Evt(rvm_cnt_t Evt_ID);
EXTERN rvm_ret_t RVM_Send_Evt(rvm_ptr_t Evt_ID);
EXTERN rvm_ret_t RVM_Query_Evt(rvm_ptr_t VMID);
EXTERN rvm_ret_t RVM_Query(char* Name);
EXTERN rvm_ret_t RVM_Tim_Prog(rvm_ptr_t Period);
EXTERN rvm_ret_t RVM_Print(void);
EXTERN void RVM_Yield(void);
EXTERN rvm_ret_t RVM_HW_Int_Enable(rvm_ptr_t Int_ID);
EXTERN rvm_ret_t RVM_HW_Int_Disable(rvm_ptr_t Int_ID);
EXTERN rvm_ret_t RVM_HW_Int_Prio(rvm_ptr_t Int_ID, rvm_ptr_t Prio);

#ifdef __HDR_PRIVATE_MEMBERS__
const struct RVM_Image RVM_Img=
{
    RVM_MAGIC                                                                  /* const rvm_ptr_t Magic */,
    RVM_VM_NAME                                                                /* char Name[16] */,
    _RVM_Entry                                                                 /* void* User_Entry */,
    &RVM_User_Stack                                                            /* void* User_Stack */,
    RVM_USER_STACK_SIZE                                                        /* rvm_ptr_t User_Size */,
    RVM_Int                                                                    /* void* Int_Entry */,
    &RVM_Int_Stack                                                             /* void* Int_Stack */,
    RVM_INT_STACK_SIZE                                                         /* rvm_ptr_t Int_Size */,
    
    &RVM_Param                                                                 /* struct RVM_Param* Param */,
    &RVM_Regs                                                                  /* struct RVM_Regs* Regs */,
    
    RVM_VM_PRIO                                                                /* rvm_ptr_t Prio */,
    RVM_VM_SLICES                                                              /* rvm_ptr_t Slices */,
    
    RVM_DEBUG_MAX_STR                                                          /* rvm_ptr_t Console_Size */,
    RVM_Console                                                                /* void* Console_Buf */,
    
    RVM_MAX_INTVECT                                                            /* rvm_ptr_t Int_Num */,
    RVM_Flag                                                                   /* rvm_ptr_t* Int_Flags */,
    
    RVM_PGTBL_NUM                                                              /* rvm_ptr_t Pgtbl_Num */,
    RVM_Pgtbl                                                                  /* const struct RVM_Hdr_Pgtbl* Pgtbl */,
    
    RVM_KCAP_NUM                                                               /* rvm_ptr_t Kcap_Num */,
#if(RVM_KCAP_NUM!=0)
    RVM_Kcap                                                                   /* const rvm_ptr_t* Kcap */,
#else
    0                                                                          /* const rvm_ptr_t* Kcap */,
#endif
    
    RVM_NEXT_IMAGE                                                             /* const struct RVM_Image* const * const Next_Image; */
};
#endif
/*****************************************************************************/
#endif /* __RVM_GUEST_CMX__ */
/* End Public C Function Prototypes ******************************************/

/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
