/******************************************************************************
Filename    : syssvc.h
Author      : pry
Date        : 25/06/2017
Licence     : LGPL v3+; see COPYING for details.
Description : The header of MPU-based user level low-level library.
******************************************************************************/

/* Defines *******************************************************************/
#ifdef __HDR_DEFS__
#ifndef __SYSSVC_H_DEFS__
#define __SYSSVC_H_DEFS__
/*****************************************************************************/
/* Generic definitions */
#define RVM_TRUE                            1
#define RVM_FALSE                           0
#define RVM_NULL                            0
#define RVM_EXIST                           1
#define RVM_EMPTY                           0

/* Debug string length */
#define RVM_USER_DEBUG_MAX_STR              128
/* Magic number for RVM interrupt header */
#define RVM_INT_MAGIC                      (0x49535953)
/* Magic number for RTD image header */
#define RVM_VM_MAGIC                       (0x56495254)

/* Assertion */
#define RVM_PRINTU_I(INT)                  RVM_Print_Int((INT))
#define RVM_PRINTU_U(UINT)                 RVM_Print_Uint((UINT))
#define RVM_PRINTU_S(STR)                  RVM_Print_String((s8*)(STR))

#define RVM_PRINTU_SIS(STR1,INT,STR2) \
do \
{ \
    RVM_PRINTU_S(STR1); \
    RVM_PRINTU_I(INT); \
    RVM_PRINTU_S(STR2); \
} \
while(0)
    
#define RVM_PRINTU_SUS(STR1,UINT,STR2) \
do \
{ \
    RVM_PRINTU_S(STR1); \
    RVM_PRINTU_U(UINT); \
    RVM_PRINTU_S(STR2); \
} \
while(0)
    
#define RVM_PRINTU_SISUS(STR1,INT,STR2,UINT,STR3) \
do \
{ \
    RVM_PRINTU_S(STR1); \
    RVM_PRINTU_I(INT); \
    RVM_PRINTU_S(STR2); \
    RVM_PRINTU_U(UINT); \
    RVM_PRINTU_S(STR3); \
} \
while(0)
    
#if(RVM_DEBUG_LOG==RVM_TRUE)
#define RVM_LOG_I(INT)                           RVM_PRINTU_I(INT)
#define RVM_LOG_U(UINT)                          RVM_PRINTU_U(UINT)
#define RVM_LOG_S(STR)                           RVM_PRINTU_S(STR)
#define RVM_LOG_SIS(STR1,INT,STR2)               RVM_PRINTU_SIS(STR1,INT,STR2)
#define RVM_LOG_SUS(STR1,UINT,STR2)              RVM_PRINTU_SUS(STR1,UINT,STR2)
#define RVM_LOG_SISUS(STR1,INT,STR2,UINT,STR3)   RVM_PRINTU_SISUS(STR1,INT,STR2,UINT,STR3)
#else
#define RVM_LOG_I(INT)                           while(0)
#define RVM_LOG_U(UINT)                          while(0)
#define RVM_LOG_S(STR)                           while(0)
#define RVM_LOG_SIS(STR1,INT,STR2)               while(0)
#define RVM_LOG_SUS(STR1,UINT,STR2)              while(0)
#define RVM_LOG_SISUS(STR1,INT,STR2,UINT,STR3)   while(0)
#endif

/* Assert macro */
#define RVM_ASSERT(X) \
do \
{ \
    if((X)==0) \
    { \
        RVM_PRINTU_S("\r\n***\r\nUser-level library panic - not syncing:\r\n"); \
        RVM_PRINTU_S(__FILE__); \
        RVM_PRINTU_S(", Line "); \
        RVM_PRINTU_I(__LINE__); \
        RVM_PRINTU_S("\r\n"); \
        RVM_PRINTU_S(__DATE__); \
        RVM_PRINTU_S(", "); \
        RVM_PRINTU_S(__TIME__); \
        RVM_PRINTU_S("\r\n"); \
        while(1); \
    } \
} \
while(0)
/* Word size settings */
#define RVM_WORD_SIZE                       (((ptr_t)1)<<RVM_WORD_ORDER)
#define RVM_WORD_MASK                       (~(((ptr_t)(-1))<<(RVM_WORD_ORDER-1)))
/* Bit mask/address operations */
#define RVM_ALLBITS                         ((ptr_t)(-1))
/* Apply this mask to keep START to MSB bits */
#define RVM_MASK_START(START)               ((RVM_ALLBITS)<<(START))
/* Apply this mask to keep LSB to END bits */
#define RVM_MASK_END(END)                   ((RVM_ALLBITS)>>(RVM_WORD_SIZE-1-(END)))
/* Apply this mask to keep START to END bits, START < END */
#define RVM_MASK(START,END)                 ((RVM_MASK_START(START))&(RVM_MASK_END(END)))
/* Round the number down & up to a power of 2, or get the power of 2 */
#define RVM_ROUND_DOWN(NUM,POW)             ((NUM)&(RVM_MASK_START(POW)))
#define RVM_ROUND_UP(NUM,POW)               RVM_ROUND_DOWN((NUM)+RVM_MASK_END(POW-1),POW)
#define RVM_POW2(POW)                       (((ptr_t)1)<<(POW))

/* System service stub */
#define RVM_CAP_OP(OP,CAPID,ARG1,ARG2,ARG3) RVM_Svc(((OP)<<(sizeof(ptr_t)*4)|(CAPID)),ARG1,ARG2,ARG3)
#define RVM_PARAM_D_MASK                    ((RVM_ALLBITS)>>(sizeof(ptr_t)*4))
#define RVM_PARAM_Q_MASK                    ((RVM_ALLBITS)>>(sizeof(ptr_t)*6))
#define RVM_PARAM_O_MASK                    ((RVM_ALLBITS)>>(sizeof(ptr_t)*7))
/* The parameter passing - not to be confused with kernel macros. These macros just place the parameters */
#define RVM_PARAM_D1(X)                     (((X)&RVM_PARAM_D_MASK)<<(sizeof(ptr_t)*4))
#define RVM_PARAM_D0(X)                     ((X)&RVM_PARAM_D_MASK)

#define RVM_PARAM_Q3(X)                     (((X)&RVM_PARAM_Q_MASK)<<(sizeof(ptr_t)*6))
#define RVM_PARAM_Q2(X)                     (((X)&RVM_PARAM_Q_MASK)<<(sizeof(ptr_t)*4))
#define RVM_PARAM_Q1(X)                     (((X)&RVM_PARAM_Q_MASK)<<(sizeof(ptr_t)*2))
#define RVM_PARAM_Q0(X)                     ((X)&RVM_PARAM_Q_MASK)

#define RVM_PARAM_O7(X)                     (((X)&RVM_PARAM_O_MASK)<<(sizeof(ptr_t)*7))
#define RVM_PARAM_O6(X)                     (((X)&RVM_PARAM_O_MASK)<<(sizeof(ptr_t)*6))
#define RVM_PARAM_O5(X)                     (((X)&RVM_PARAM_O_MASK)<<(sizeof(ptr_t)*5))
#define RVM_PARAM_O4(X)                     (((X)&RVM_PARAM_O_MASK)<<(sizeof(ptr_t)*4))
#define RVM_PARAM_O3(X)                     (((X)&RVM_PARAM_O_MASK)<<(sizeof(ptr_t)*3))
#define RVM_PARAM_O2(X)                     (((X)&RVM_PARAM_O_MASK)<<(sizeof(ptr_t)*2))
#define RVM_PARAM_O1(X)                     (((X)&RVM_PARAM_O_MASK)<<(sizeof(ptr_t)*1))
#define RVM_PARAM_O0(X)                     ((X)&RVM_PARAM_O_MASK)

/* CID synthesis */
#define RME_CAPID_NULL                      (((cid_t)1)<<(sizeof(ptr_t)*4-1))
#define RVM_CAPID_2L                        (((cid_t)1)<<(sizeof(ptr_t)*2-1))
#define RVM_CAPID(X,Y)                      (((X)<<(sizeof(ptr_t)*2))|(Y)|RVM_CAPID_2L)

/* Flag synthesis */
/* Kernel function */
#define RVM_KERN_FLAG(HIGH,LOW)             (((HIGH)<<(sizeof(ptr_t)*4))|(LOW))
/* Kernel memory */
#define RVM_KMEM_FLAG(HIGH,LOW)             ((((HIGH)>>(sizeof(ptr_t)*4))<<(sizeof(ptr_t)*4))| \
                                            ((LOW)>>(sizeof(ptr_t)*4)))
#define RVM_KMEM_SVC(HIGH,SVC)              (((((HIGH)>>6)<<(sizeof(ptr_t)*4+6))>>(sizeof(ptr_t)*4))|(SVC))
#define RVM_KMEM_CAPID(LOW,FLAGS)           (((((LOW)>>6)<<(sizeof(ptr_t)*4+6))>>(sizeof(ptr_t)*4))|(FLAGS))
/* Page table */
#define RVM_PGTBL_SVC(NUM_ORDER,SVC)        (((NUM_ORDER)<<(sizeof(ptr_t)<<1))|(SVC))
#define RVM_PGTBL_FLAG(HIGH,LOW,FLAGS)      (((HIGH)<<(sizeof(ptr_t)*4+4))|((LOW)<<8)|(FLAGS))
/* Page table size and number order */
#define RVM_PGTBL(SIZE,NUM)                 (((SIZE)<<(sizeof(ptr_t)<<2))|(NUM))
#define RVM_PGTBL_SIZE(X)                   ((X)>>(sizeof(ptr_t)<<2))
#define RVM_PGTBL_NUM(X)                    ((X)&(RVM_ALLBITS>>(sizeof(ptr_t)<<2)))
/* Thread time delegation */
/* Init thread infinite time marker */
#define RVM_THD_INIT_TIME                   (RVM_ALLBITS>>1)
/* Other thread infinite time marker */
#define RVM_THD_INF_TIME                    (RVM_THD_INIT_TIME-1)
/* Thread time upper limit - always ths infinite time */
#define RVM_THD_MAX_TIME                    (RVM_THD_INF_TIME)
/* Sched rcv return value's fault flag */
#define RVM_THD_FAULT_FLAG                   (((ptr_t)1)<<(sizeof(ptr_t)*8-2))
    
/* Size of kernel objects */
/* Capability table */
#define RVM_CAPTBL_WORD_SIZE(NUM)            (((ptr_t)(NUM))<<3)
/* Process */
#define RVM_PROC_WORD_SIZE                   3
/* Thread */
#define RVM_THD_WORD_SIZE                    46
/* Signal */
#define RVM_SIG_WORD_SIZE                    3
/* Invocation */
#define RVM_INV_WORD_SIZE                    9

/* Rounded size of each object */
#define RVM_ROUNDED(X)                       RVM_ROUND_UP(((ptr_t)(X))*sizeof(ptr_t),RVM_KMEM_SLOT_ORDER)
/* Capability table */
#define RVM_CAPTBL_SIZE(NUM)                 RVM_ROUNDED(RVM_CAPTBL_WORD_SIZE(NUM))
/* Normal page directory */
#define RVM_PGTBL_SIZE_NOM(NUM_ORDER)        RVM_ROUNDED(RVM_PGTBL_WORD_SIZE_NOM(NUM_ORDER))
/* Top-level page directory */
#define RVM_PGTBL_SIZE_TOP(NUM_ORDER)        RVM_ROUNDED(RVM_PGTBL_WORD_SIZE_TOP(NUM_ORDER))
/* Process */
#define RVM_PROC_SIZE                        RVM_ROUNDED(RVM_PROC_WORD_SIZE)
/* Thread */
#define RVM_THD_SIZE                         RVM_ROUNDED(RVM_THD_WORD_SIZE)
/* Signal */                           
#define RVM_SIG_SIZE                         RVM_ROUNDED(RVM_SIG_WORD_SIZE)
/* Invocation */
#define RVM_INV_SIZE                         RVM_ROUNDED(RVM_INV_WORD_SIZE)

/* The TLS masks */ 
#define RVM_TLS_MASK_128B                    RVM_ROUND_DOWN(RVM_ALLBITS,7)
#define RVM_TLS_MASK_256B                    RVM_ROUND_DOWN(RVM_ALLBITS,8)
#define RVM_TLS_MASK_512B                    RVM_ROUND_DOWN(RVM_ALLBITS,9)
#define RVM_TLS_MASK_1KB                     RVM_ROUND_DOWN(RVM_ALLBITS,10)
#define RVM_TLS_MASK_2KB                     RVM_ROUND_DOWN(RVM_ALLBITS,11)
#define RVM_TLS_MASK_4KB                     RVM_ROUND_DOWN(RVM_ALLBITS,12)
#define RVM_TLS_MASK_8KB                     RVM_ROUND_DOWN(RVM_ALLBITS,13)
#define RVM_TLS_MASK_16KB                    RVM_ROUND_DOWN(RVM_ALLBITS,14)
#define RVM_TLS_MASK_32KB                    RVM_ROUND_DOWN(RVM_ALLBITS,15)
#define RVM_TLS_MASK_64KB                    RVM_ROUND_DOWN(RVM_ALLBITS,16)

/* Initial capability layout - same across all architectures */
/* The capability table of the init process */
#define RVM_BOOT_CAPTBL                      0
/* The top-level page table of the init process - always full memory access */
#define RVM_BOOT_PGTBL                       1
/* The init process */
#define RVM_BOOT_INIT_PROC                   2
/* The init thread */
#define RVM_BOOT_INIT_THD                    3
/* The initial kernel function capability */
#define RVM_BOOT_INIT_KERN                   4
/* The initial kernel memory capability */
#define RVM_BOOT_INIT_KMEM                   5
/* The initial timer endpoint */
#define RVM_BOOT_INIT_TIMER                  6
/* The initial interrupt endpoint */
#define RVM_BOOT_INIT_INT                    7
/* This is the capability table containing all the VM process/capability table capabilities */
#define RVM_VIRT_TBL_CAPPROC                 8
/* This is the capability table containing all the VM thread/endpoing capabilities */
#define RVM_VIRT_TBL_THDSIG                  9
/* This is the capability table for virtual machine page tables */
#define RVM_VIRT_TBL_PGTBL                   10
/* These are the capabilities for the guard thread - guard thread does not have an endpoint */
#define RVM_VMM_GUARD_THD                    11
#define RVM_VMM_GUARD_SIG                    12
/* These are the capabilities for the timer thread - timer thread does not have an endpoint */
#define RVM_VMM_TIMD_THD                     13
/* These are the capabilities for the real-time daemon */
#define RVM_VMM_VMMD_THD                     14
#define RVM_VMM_VMMD_SIG                     15
/* These are the capabilities for the interrupt daemon */
#define RVM_VMM_INTD_THD                     16

/* Helper capability definitions */
/* The capability to its capability table */
#define RVM_VIRT_CAPTBL(X)                  RVM_CAPID(RVM_VIRT_TBL_CAPPROC,((X)*2))
/* The capability to its process */
#define RVM_VIRT_PROC(X)                    RVM_CAPID(RVM_VIRT_TBL_CAPPROC,((X)*2+1))
/* The capability to its user thread */
#define RVM_VIRT_USERTHD(X)                 RVM_CAPID(RVM_VIRT_TBL_THDSIG,((X)*3))
/* The capability to its interrupt thread */
#define RVM_VIRT_INTTHD(X)                  RVM_CAPID(RVM_VIRT_TBL_THDSIG,((X)*3)+1)
/* The capability to its interrupt thread signal endpoint */
#define RVM_VIRT_INTSIG(X)                  RVM_CAPID(RVM_VIRT_TBL_THDSIG,((X)*3)+2)
/*****************************************************************************/
/* __SYSSVC_H_DEFS__ */
#endif
/* __HDR_DEFS__ */
#endif
/* End Defines ***************************************************************/

/* Structs *******************************************************************/
#ifdef __HDR_STRUCTS__
#ifndef __SYSSVC_H_STRUCTS__
#define __SYSSVC_H_STRUCTS__
/* We used structs in the header */

/* Use defines in these headers */
#define __HDR_DEFS__
#undef __HDR_DEFS__
/*****************************************************************************/
/* Doubly-linked list */
struct RVM_List
{
    struct RVM_List* Prev;
    struct RVM_List* Next;
};
/*****************************************************************************/
/* __SYSSVC_H_STRUCTS__ */
#endif
/* __HDR_STRUCTS__ */
#endif
/* End Structs ***************************************************************/

/* Private Global Variables **************************************************/
#if(!(defined __HDR_DEFS__||defined __HDR_STRUCTS__))
#ifndef __SYSSVC_MEMBERS__
#define __SYSSVC_MEMBERS__

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
/* Doubly linked list */
__EXTERN__ void RVM_List_Crt(volatile struct RVM_List* Head);
__EXTERN__ void RVM_List_Del(volatile struct RVM_List* Prev,volatile struct RVM_List* Next);
__EXTERN__ void RVM_List_Ins(volatile struct RVM_List* New,
                             volatile struct RVM_List* Prev,
                             volatile struct RVM_List* Next);
/* Capability table operations */
__EXTERN__ ret_t RVM_Captbl_Crt(cid_t Cap_Captbl_Crt, cid_t Cap_Kmem, 
                                cid_t Cap_Captbl, ptr_t Vaddr, ptr_t Entry_Num);
__EXTERN__ ret_t RVM_Captbl_Del(cid_t Cap_Captbl_Del, cid_t Cap_Del);
__EXTERN__ ret_t RVM_Captbl_Frz(cid_t Cap_Captbl_Frz, cid_t Cap_Frz);
__EXTERN__ ret_t RVM_Captbl_Add(cid_t Cap_Captbl_Dst, cid_t Cap_Dst, 
                                cid_t Cap_Captbl_Src, cid_t Cap_Src, ptr_t Flags);
__EXTERN__ ret_t RVM_Captbl_Pgtbl(cid_t Cap_Captbl_Dst, cid_t Cap_Dst, 
                                  cid_t Cap_Captbl_Src, cid_t Cap_Src,
                                  ptr_t Start, ptr_t End, ptr_t Flags);
__EXTERN__ ret_t RVM_Captbl_Kern(cid_t Cap_Captbl_Dst, cid_t Cap_Dst, 
                                 cid_t Cap_Captbl_Src, cid_t Cap_Src,
                                 ptr_t Start, ptr_t End);
__EXTERN__ ret_t RVM_Captbl_Kmem(cid_t Cap_Captbl_Dst, cid_t Cap_Dst, 
                                 cid_t Cap_Captbl_Src, cid_t Cap_Src,
                                 ptr_t Start, ptr_t End, ptr_t Flags);
__EXTERN__ ret_t RVM_Captbl_Rem(cid_t Cap_Captbl_Rem, cid_t Cap_Rem);
/* Kernel function operations */
__EXTERN__ ret_t RVM_Kern_Act(cid_t Cap_Kern, ptr_t Func_ID, ptr_t Sub_ID, ptr_t Param1, ptr_t Param2);
/* Page table operations */
__EXTERN__ ret_t RVM_Pgtbl_Crt(cid_t Cap_Captbl, cid_t Cap_Kmem, cid_t Cap_Pgtbl, 
                               ptr_t Vaddr, ptr_t Start_Addr, ptr_t Top_Flag,
                               ptr_t Size_Order, ptr_t Num_Order);
__EXTERN__ ret_t RVM_Pgtbl_Del(cid_t Cap_Captbl, cid_t Cap_Pgtbl);
__EXTERN__ ret_t RVM_Pgtbl_Add(cid_t Cap_Pgtbl_Dst, ptr_t Pos_Dst, ptr_t Flags_Dst,
                               cid_t Cap_Pgtbl_Src, ptr_t Pos_Src, ptr_t Index);
__EXTERN__ ret_t RVM_Pgtbl_Rem(cid_t Cap_Pgtbl, ptr_t Pos);
__EXTERN__ ret_t RVM_Pgtbl_Con(cid_t Cap_Pgtbl_Parent, ptr_t Pos, cid_t Cap_Pgtbl_Child, ptr_t Flags_Child);
__EXTERN__ ret_t RVM_Pgtbl_Des(cid_t Cap_Pgtbl, ptr_t Pos);
/* Process operations */
__EXTERN__ ret_t RVM_Proc_Crt(cid_t Cap_Captbl_Crt, cid_t Cap_Kmem, cid_t Cap_Proc,
                              cid_t Cap_Captbl, cid_t Cap_Pgtbl, ptr_t Vaddr);
__EXTERN__ ret_t RVM_Proc_Del(cid_t Cap_Captbl, cid_t Cap_Proc);
__EXTERN__ ret_t RVM_Proc_Cpt(cid_t Cap_Proc, cid_t Cap_Captbl);
__EXTERN__ ret_t RVM_Proc_Pgt(cid_t Cap_Proc, cid_t Cap_Pgtbl);
/* Thread operations */
__EXTERN__ ret_t RVM_Thd_Crt(cid_t Cap_Captbl, cid_t Cap_Kmem, cid_t Cap_Thd,
                             cid_t Cap_Proc, ptr_t Max_Prio, ptr_t Vaddr);
__EXTERN__ ret_t RVM_Thd_Del(cid_t Cap_Captbl, cid_t Cap_Thd);
__EXTERN__ ret_t RVM_Thd_Exec_Set(cid_t Cap_Thd, ptr_t Entry, ptr_t Stack, ptr_t Param);
__EXTERN__ ret_t RVM_Thd_Hyp_Set(cid_t Cap_Thd, ptr_t Kaddr);
__EXTERN__ ret_t RVM_Thd_Sched_Bind(cid_t Cap_Thd, cid_t Cap_Thd_Sched,
                                    cid_t Cap_Sig, tid_t TID, ptr_t Prio);
__EXTERN__ ret_t RVM_Thd_Sched_Rcv(cid_t Cap_Thd);
__EXTERN__ ret_t RVM_Thd_Sched_Prio(cid_t Cap_Thd, ptr_t Prio);
__EXTERN__ ret_t RVM_Thd_Sched_Free(cid_t Cap_Thd);
__EXTERN__ ret_t RVM_Thd_Time_Xfer(cid_t Cap_Thd_Dst, cid_t Cap_Thd_Src, ptr_t Time);
__EXTERN__ ret_t RVM_Thd_Swt(cid_t Cap_Thd, ptr_t Full_Yield);
/* Signal operations */
__EXTERN__ ret_t RVM_Sig_Crt(cid_t Cap_Captbl, cid_t Cap_Kmem, cid_t Cap_Sig, ptr_t Vaddr);
__EXTERN__ ret_t RVM_Sig_Del(cid_t Cap_Captbl, cid_t Cap_Sig);
__EXTERN__ ret_t RVM_Sig_Snd(cid_t Cap_Sig);
__EXTERN__ ret_t RVM_Sig_Rcv(cid_t Cap_Sig, ptr_t Option);
/* Invocation operations */
__EXTERN__ ret_t RVM_Inv_Crt(cid_t Cap_Captbl, cid_t Cap_Kmem, 
                             cid_t Cap_Inv, cid_t Cap_Proc, ptr_t Vaddr);
__EXTERN__ ret_t RVM_Inv_Del(cid_t Cap_Captbl, cid_t Cap_Inv);
__EXTERN__ ret_t RVM_Inv_Set(cid_t Cap_Inv, ptr_t Entry, ptr_t Stack, ptr_t Fault_Ret_Flag);

/* Debugging helpers */
__EXTERN__ cnt_t RVM_Print_Int(cnt_t Int);
__EXTERN__ cnt_t RVM_Print_Uint(ptr_t Uint);
__EXTERN__ cnt_t RVM_Print_String(s8* String);
/*****************************************************************************/
/* Undefine "__EXTERN__" to avoid redefinition */
#undef __EXTERN__
/* __SYSSVC_MEMBERS__ */
#endif
/* !(defined __HDR_DEFS__||defined __HDR_STRUCTS__) */
#endif
/* End Public C Function Prototypes ******************************************/

/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
