/******************************************************************************
Filename    : guest_cmx.h
Author      : pry
Date        : 01/04/2017
Licence     : LGPL v3+; see COPYING for details.
Description : The header of platform-specific part of RMP for RVM.
******************************************************************************/

/* Begin Typedefs ************************************************************/
#ifndef __GUEST_CMX__
#define __GUEST_CMX__

#ifndef __PTR_T__
#define __PTR_T__
/* The typedef for the pointers - This is the raw style. Pointers must be unsigned */
typedef unsigned int ptr_t;
#endif

#ifndef __CNT_T__
#define __CNT_T__
/* The typedef for the count variables */
typedef signed int cnt_t;
#endif

#ifndef __RET_T__
#define __RET_T__
/* The type for process return value */
typedef signed int ret_t;
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
#define RMP_INT_STACK_ADDR           (((ptr_t)&__initial_sp)+0x400-64*sizeof(ptr_t))
    
/* Generic page table flags */
#define RME_PGTBL_ORDER(SIZE,NUM)    (((SIZE)<<(sizeof(ptr_t)*4))|(NUM))
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

/* We used structs in the header */
struct RVM_Param
{
    ptr_t Number;
    ptr_t Param[4];
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
    unsigned char Flags[8];
};

/* The image structure */
struct RVM_Image
{
    /* The magic number;always equal to VIRT(0x56495254) */
    const ptr_t Magic;
    /* The name is always 16 bytes long. This is fixed and is not changeable */
    char Name[16];

    /* The entry, stack and stack size of the virtual machines */
    void* User_Entry;
    void* User_Stack;
    ptr_t User_Size;
    void* Int_Entry;
    void* Int_Stack;
    ptr_t Int_Size;
    
    /* The parameter space and register set space */
    struct RVM_Param* Param;
    struct RVM_Regs* Regs;
    
    /* The priority and timeslices */
    ptr_t Prio;
    ptr_t Slices;
    
    /* The console space and size */
    ptr_t Console_Size;
    void* Console_Buf;
    
    /* The interrupt space */
    ptr_t Int_Num;
    ptr_t* Int_Flags;

    /* The number of page tables in this image, and where are they stored */
    ptr_t Pgtbl_Num;
    const struct RVM_Hdr_Pgtbl* Pgtbl;
    
    /* The number of kernel capabilities in this image, and their list */
    ptr_t Kcap_Num;
    const ptr_t* Kcap;

    /* Is there any other images? If there is, here is a pointer to the start
     * of the next one. This is a constant pointer to a constant pointer to a 
     * constant structure */
    const struct RVM_Image* const * const Next_Image;
};

#include "guest_cmx_conf.h"
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
static ret_t RVM_Hypercall(ptr_t Number, ptr_t Param1, ptr_t Param2, ptr_t Param3, ptr_t Param4);
static ret_t RVM_Get_Int(void);
static void RVM_Int(void);
static void RVM_Wait_Int(void);
static ptr_t RVM_Int_Enable;
#endif

/* The debug console buffer and pointer */
EXTERN char RVM_Console[RVM_DEBUG_MAX_STR];

/* The initial stack pointer as defined in assembly */
extern ptr_t RVM_User_Stack;
extern ptr_t RVM_Int_Stack;
/* The hypercall parameter& structure */
EXTERN struct RVM_Param RVM_Param;
EXTERN struct RVM_Regs RVM_Regs;

/* The interrupt handler registration table */
EXTERN ptr_t RVM_Vect[RVM_MAX_INTVECT];
/* The interrupt flag registration table */
EXTERN ptr_t RVM_Flag[RVM_VECT_BITMAP];
/*****************************************************************************/

/* End Public Global Variables ***********************************************/

/* Public C Function Prototypes **********************************************/
/*****************************************************************************/
extern ptr_t RVM_Fetch_And(ptr_t* Ptr, ptr_t Operand);
extern ptr_t RVM_Fetch_Or(ptr_t* Ptr, ptr_t Operand);
extern void RVM_Int_Rcv(void);
extern ptr_t RVM_MSB_Get(ptr_t Val);
extern void _RVM_Yield(void);
extern void _RVM_Hypercall(void);
extern void _RVM_Entry(void);
extern ret_t _RVM_Kern(cnt_t Capid, ptr_t Func_ID, ptr_t Param1, ptr_t Param2);

/* Hypercalls */
EXTERN void RVM_Init(void);
EXTERN ret_t RVM_Vect_Init(ptr_t Num, void* Handler);
EXTERN void RVM_Disable_Int(void);
EXTERN void RVM_Enable_Int(void);
EXTERN ret_t RVM_Reg_Int(ptr_t Vect_Num, ptr_t Int_Num);
EXTERN ret_t RVM_Del_Int(cnt_t Int_ID);
EXTERN ret_t RVM_Reg_Evt(ptr_t Int_Num, ptr_t VMID);
EXTERN ret_t RVM_Del_Evt(cnt_t Evt_ID);
EXTERN ret_t RVM_Send_Evt(ptr_t Evt_ID);
EXTERN ret_t RVM_Query_Evt(ptr_t VMID);
EXTERN ret_t RVM_Query(char* Name);
EXTERN ret_t RVM_Tim_Prog(ptr_t Period);
EXTERN ret_t RVM_Print(void);
EXTERN void RVM_Yield(void);
EXTERN ret_t RVM_HW_Int_Enable(ptr_t Int_ID);
EXTERN ret_t RVM_HW_Int_Disable(ptr_t Int_ID);
EXTERN ret_t RVM_HW_Int_Prio(ptr_t Int_ID, ptr_t Prio);

#ifdef __HDR_PRIVATE_MEMBERS__
const struct RVM_Image RVM_Img=
{
    RVM_MAGIC                                                                  /* const ptr_t Magic */,
    RVM_VM_NAME                                                                /* char Name[16] */,
    _RVM_Entry                                                                 /* void* User_Entry */,
    &RVM_User_Stack                                                            /* void* User_Stack */,
    RVM_USER_STACK_SIZE                                                        /* ptr_t User_Size */,
    RVM_Int                                                                    /* void* Int_Entry */,
    &RVM_Int_Stack                                                             /* void* Int_Stack */,
    RVM_INT_STACK_SIZE                                                         /* ptr_t Int_Size */,
    
    &RVM_Param                                                                 /* struct RVM_Param* Param */,
    &RVM_Regs                                                                  /* struct RVM_Regs* Regs */,
    
    RVM_VM_PRIO                                                                /* ptr_t Prio */,
    RVM_VM_SLICES                                                              /* ptr_t Slices */,
    
    RVM_DEBUG_MAX_STR                                                          /* ptr_t Console_Size */,
    RVM_Console                                                                /* void* Console_Buf */,
    
    RVM_MAX_INTVECT                                                            /* ptr_t Int_Num */,
    RVM_Flag                                                                   /* ptr_t* Int_Flags */,
    
    RVM_PGTBL_NUM                                                              /* ptr_t Pgtbl_Num */,
    RVM_Pgtbl                                                                  /* const struct RVM_Hdr_Pgtbl* Pgtbl */,
    
    RVM_KCAP_NUM                                                               /* ptr_t Kcap_Num */,
#if(RVM_KCAP_NUM!=0)
    RVM_Kcap                                                                   /* const ptr_t* Kcap */,
#else
    0                                                                          /* const ptr_t* Kcap */,
#endif
    
    RVM_NEXT_IMAGE                                                             /* const struct RVM_Image* const * const Next_Image; */
};
#endif
/*****************************************************************************/
#endif /* __GUEST_CMX__ */
/* End Public C Function Prototypes ******************************************/

/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
