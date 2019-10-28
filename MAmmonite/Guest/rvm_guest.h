/******************************************************************************
Filename    : rvm_guest.h
Author      : pry
Date        : 25/06/2017
Licence     : The Unlicense; see LICENSE for details.
Description : The header of guest user level low-level library.
******************************************************************************/

/* Defines *******************************************************************/
#ifndef __RVM_GUEST__
#define __RVM_GUEST__
/*****************************************************************************/
/* Generic definitions */
#define RVM_TRUE                                    (1)
#define RVM_FALSE                                   (0)
#define RVM_NULL                                    (0)
#define RVM_EXIST                                   (1)
#define RVM_EMPTY                                   (0)

/* Debug string length */
#define RVM_USER_DEBUG_MAX_STR                      (128)

/* Assertion */
#define RVM_PRINTU_I(INT)                           RVM_Print_Int((INT))
#define RVM_PRINTU_U(UINT)                          RVM_Print_Uint((UINT))
#define RVM_PRINTU_S(STR)                           RVM_Print_String((rvm_s8_t*)(STR))

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
#define RVM_LOG_I(INT)                              RVM_PRINTU_I(INT)
#define RVM_LOG_U(UINT)                             RVM_PRINTU_U(UINT)
#define RVM_LOG_S(STR)                              RVM_PRINTU_S(STR)
#define RVM_LOG_SIS(STR1,INT,STR2)                  RVM_PRINTU_SIS(STR1,INT,STR2)
#define RVM_LOG_SUS(STR1,UINT,STR2)                 RVM_PRINTU_SUS(STR1,UINT,STR2)
#define RVM_LOG_SISUS(STR1,INT,STR2,UINT,STR3)      RVM_PRINTU_SISUS(STR1,INT,STR2,UINT,STR3)
#else
#define RVM_LOG_I(INT)                              while(0)
#define RVM_LOG_U(UINT)                             while(0)
#define RVM_LOG_S(STR)                              while(0)
#define RVM_LOG_SIS(STR1,INT,STR2)                  while(0)
#define RVM_LOG_SUS(STR1,UINT,STR2)                 while(0)
#define RVM_LOG_SISUS(STR1,INT,STR2,UINT,STR3)      while(0)
#endif

/* Assert macro */
#define RVM_ASSERT(X) \
do \
{ \
    if((X)==0) \
    { \
        RVM_PRINTU_S("\r\n***\r\nGuest panic - rebooting:\r\n"); \
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
#define RVM_WORD_SIZE                               RVM_POW2(RVM_WORD_ORDER)
#define RVM_WORD_MASK                               (~(RVM_ALLBITS<<(RVM_WORD_ORDER-1)))
/* Bit mask/address operations */
#define RVM_ALLBITS                                 ((rvm_ptr_t)(-1))
/* Apply this mask to keep START to MSB bits */
#define RVM_MASK_START(START)                       ((RVM_ALLBITS)<<(START))
/* Apply this mask to keep LSB to END bits */
#define RVM_MASK_END(END)                           ((RVM_ALLBITS)>>(RVM_WORD_SIZE-1-(END)))
/* Apply this mask to keep START to END bits, START < END */
#define RVM_MASK(START,END)                         ((RVM_MASK_START(START))&(RVM_MASK_END(END)))
/* Round the number down & up to a power of 2, or get the power of 2 */
#define RVM_ROUND_DOWN(NUM,POW)                     ((NUM)&(RVM_MASK_START(POW)))
#define RVM_ROUND_UP(NUM,POW)                       RVM_ROUND_DOWN((NUM)+RVM_MASK_END(POW-1),POW)
#define RVM_POW2(POW)                               (((rvm_ptr_t)1)<<(POW))

/* System service stub */
#define RVM_CAP_OP(OP,CAPID,ARG1,ARG2,ARG3)         RVM_Svc(((OP)<<(sizeof(rvm_ptr_t)*4)|(CAPID)),ARG1,ARG2,ARG3)
#define RVM_PARAM_D_MASK                            ((RVM_ALLBITS)>>(sizeof(rvm_ptr_t)*4))
#define RVM_PARAM_Q_MASK                            ((RVM_ALLBITS)>>(sizeof(rvm_ptr_t)*6))
#define RVM_PARAM_O_MASK                            ((RVM_ALLBITS)>>(sizeof(rvm_ptr_t)*7))
/* The parameter passing - not to be confused with kernel macros. These macros just place the parameters */
#define RVM_PARAM_D1(X)                             (((X)&RVM_PARAM_D_MASK)<<(sizeof(rvm_ptr_t)*4))
#define RVM_PARAM_D0(X)                             ((X)&RVM_PARAM_D_MASK)

#define RVM_PARAM_Q3(X)                             (((X)&RVM_PARAM_Q_MASK)<<(sizeof(rvm_ptr_t)*6))
#define RVM_PARAM_Q2(X)                             (((X)&RVM_PARAM_Q_MASK)<<(sizeof(rvm_ptr_t)*4))
#define RVM_PARAM_Q1(X)                             (((X)&RVM_PARAM_Q_MASK)<<(sizeof(rvm_ptr_t)*2))
#define RVM_PARAM_Q0(X)                             ((X)&RVM_PARAM_Q_MASK)

#define RVM_PARAM_O7(X)                             (((X)&RVM_PARAM_O_MASK)<<(sizeof(rvm_ptr_t)*7))
#define RVM_PARAM_O6(X)                             (((X)&RVM_PARAM_O_MASK)<<(sizeof(rvm_ptr_t)*6))
#define RVM_PARAM_O5(X)                             (((X)&RVM_PARAM_O_MASK)<<(sizeof(rvm_ptr_t)*5))
#define RVM_PARAM_O4(X)                             (((X)&RVM_PARAM_O_MASK)<<(sizeof(rvm_ptr_t)*4))
#define RVM_PARAM_O3(X)                             (((X)&RVM_PARAM_O_MASK)<<(sizeof(rvm_ptr_t)*3))
#define RVM_PARAM_O2(X)                             (((X)&RVM_PARAM_O_MASK)<<(sizeof(rvm_ptr_t)*2))
#define RVM_PARAM_O1(X)                             (((X)&RVM_PARAM_O_MASK)<<(sizeof(rvm_ptr_t)*1))
#define RVM_PARAM_O0(X)                             ((X)&RVM_PARAM_O_MASK)

/* CID synthesis */
#define RME_CAPID_NULL                              (1<<(sizeof(rvm_ptr_t)*4-1))
#define RVM_CAPID_2L                                (1<<(sizeof(rvm_ptr_t)*2-1))
#define RVM_CAPID(X,Y)                              (((X)<<(sizeof(rvm_ptr_t)*2))|(Y)|RVM_CAPID_2L)

/* Flag synthesis */
/* Kernel function */
#define RVM_KERN_FLAG(HIGH,LOW)                     (((HIGH)<<(sizeof(rvm_ptr_t)*4))|(LOW))
/* Kernel memory */
#define RVM_KMEM_FLAG(HIGH,LOW)                     ((((HIGH)>>(sizeof(rvm_ptr_t)*4))<<(sizeof(rvm_ptr_t)*4))| \
                                                    ((LOW)>>(sizeof(rvm_ptr_t)*4)))
#define RVM_KMEM_SVC(HIGH,SVC)                      (((((HIGH)>>6)<<(sizeof(rvm_ptr_t)*4+6))>>(sizeof(rvm_ptr_t)*4))|(SVC))
#define RVM_KMEM_CAPID(LOW,FLAGS)                   (((((LOW)>>6)<<(sizeof(rvm_ptr_t)*4+6))>>(sizeof(rvm_ptr_t)*4))|(FLAGS))
/* Page table */
#define RVM_PGTBL_SVC(NUM_ORDER,SVC)                (((NUM_ORDER)<<(sizeof(rvm_ptr_t)<<1))|(SVC))
#define RVM_PGTBL_FLAG(HIGH,LOW,FLAGS)              (((HIGH)<<(sizeof(rvm_ptr_t)*4+4))|((LOW)<<8)|(FLAGS))
/* Page table size and number order */
#define RVM_PGTBL(SIZE,NUM)                         (((SIZE)<<(sizeof(rvm_ptr_t)<<2))|(NUM))
#define RVM_PGTBL_SIZE(X)                           ((X)>>(sizeof(rvm_ptr_t)<<2))
#define RVM_PGTBL_NUM(X)                            ((X)&(RVM_ALLBITS>>(sizeof(rvm_ptr_t)<<2)))
/* Thread time delegation */
/* Init thread infinite time marker */
#define RVM_THD_INIT_TIME                           (RVM_ALLBITS>>1)
/* Other thread infinite time marker */
#define RVM_THD_INF_TIME                            (RVM_THD_INIT_TIME-1)
/* Thread time upper limit - always ths infinite time */
#define RVM_THD_MAX_TIME                            (RVM_THD_INF_TIME)
/* Sched rcv return value's fault flag */
#define RVM_THD_FAULT_FLAG                          RVM_POW2(sizeof(rvm_ptr_t)*8-2)
    
/* Size of kernel objects that are architecture agnostic */
/* Capability table */
#define RVM_CAPTBL_WORD_SIZE(NUM)                   (((rvm_ptr_t)(NUM))<<3)
/* Invocation */
#define RVM_INV_WORD_SIZE                           (9)

/* Rounded size of each object */
#define RVM_ROUNDED(X)                              RVM_ROUND_UP(((rvm_ptr_t)(X))*sizeof(rvm_ptr_t),RVM_KMEM_SLOT_ORDER)
/* Capability table */
#define RVM_CAPTBL_SIZE(NUM)                        RVM_ROUNDED(RVM_CAPTBL_WORD_SIZE(NUM))
/* Normal page directory */
#define RVM_PGTBL_SIZE_NOM(NUM_ORDER)               RVM_ROUNDED(RVM_PGTBL_WORD_SIZE_NOM(NUM_ORDER))
/* Top-level page directory */
#define RVM_PGTBL_SIZE_TOP(NUM_ORDER)               RVM_ROUNDED(RVM_PGTBL_WORD_SIZE_TOP(NUM_ORDER))
/* Process */
#define RVM_PROC_SIZE                               RVM_ROUNDED(RVM_PROC_WORD_SIZE)
/* Thread */
#define RVM_THD_SIZE                                RVM_ROUNDED(RVM_THD_WORD_SIZE)
/* Signal */                           
#define RVM_SIG_SIZE                                RVM_ROUNDED(RVM_SIG_WORD_SIZE)
/* Invocation */
#define RVM_INV_SIZE                                RVM_ROUNDED(RVM_INV_WORD_SIZE)

/* Round the kernel object size to the entry slot size */
#define RVM_KOTBL_ROUND(X)                          RVM_ROUND_UP(X,RVM_KMEM_SLOT_ORDER)

/* Initial capability layout - same across all architectures */
/* The capability table of the init process */
#define RVM_BOOT_CAPTBL                             (0)
/* The top-level page table of the init process - always full memory access */
#define RVM_BOOT_PGTBL                              (1)
/* The init process */
#define RVM_BOOT_INIT_PROC                          (2)
/* The init thread */
#define RVM_BOOT_INIT_THD                           (3)
/* The initial kernel function capability */
#define RVM_BOOT_INIT_KERN                          (4)
/* The initial kernel memory capability */
#define RVM_BOOT_INIT_KMEM                          (5)
/* The initial timer endpoint */
#define RVM_BOOT_INIT_TIMER                         (6)
/* The initial interrupt endpoint */
#define RVM_BOOT_INIT_VECT                          (7)

/* Error codes */
/* The state is wrong */
#define RVM_ERR_STATE                               (-1)
/* The number is overrange */
#define RVM_ERR_RANGE                               (-2)
/* The error is address related */
#define RVM_ERR_ADDR                                (-3)
/* The error is physical vector related */
#define RVM_ERR_PHYS                                (-4)
/* The error is virtual vector related */
#define RVM_ERR_VIRT                                (-5)
/* The error is event related */
#define RVM_ERR_EVT                                 (-6)
/* The error is mapping block related */
#define RVM_ERR_MAP                                 (-7)
/*****************************************************************************/
#include "rvm_guest_conf.h"

#ifdef RVM_VIRT_VECT_NUM
/* Register space */
#define RVM_REGS                                    ((volatile struct RVM_Regs*)RVM_VIRT_REG_BASE)
/* Parameter space */
#define RVM_PARAM                                   ((volatile struct RVM_Param_Area*)RVM_VIRT_PARAM_BASE)
/* Vector space */
#define RVM_VECT_FLAG                               ((volatile struct RVM_Vect_Flag*)RVM_VIRT_VCTF_BASE)
#define RVM_VIRT_VCTF_WORDS                         (RVM_ROUND_UP(RVM_VIRT_VECT_NUM,RVM_WORD_ORDER)>>RVM_WORD_ORDER)

/* Virtualization signal endpoints */
/* Vector signal endpoint */
#define RVM_SIG_VECT                                (1)
/* Hypervisor signal endpoint */
#define RVM_SIG_HYP                                 (0)

/* Hypercalls */
/* Enable interrupts */
#define RVM_HYP_ENAINT                              (0)
/* Disable interrupts */
#define RVM_HYP_DISINT                              (1)
/* Register a physical vector */
#define RVM_HYP_REGPHYS                             (2)
/* Register a event */
#define RVM_HYP_REGEVT                              (3)
/* Delete a event */
#define RVM_HYP_DELVECT                             (4)
/* Delete a event */
#define RVM_HYP_LOCKVECT                            (5)
/* Wait for an virtual vector to come */
#define RVM_HYP_WAITVECT                            (6)
/* Send to an event */
#define RVM_HYP_SENDEVT                             (7)
/* Start and feed watchdog */
#define RVM_HYP_FEEDWDOG                            (8)
#endif
/* End Defines ***************************************************************/

/* Structs *******************************************************************/
/*****************************************************************************/
/* Doubly-linked list */
struct RVM_List
{
    struct RVM_List* Prev;
    struct RVM_List* Next;
};

#ifdef RVM_VIRT_VECT_NUM
/* Parameter */
struct RVM_Param
{
    rvm_ptr_t Number;
    rvm_ptr_t Param[4];
};
/* Parameter area, one for vector, another for ser thread */
struct RVM_Param_Area
{
    rvm_ptr_t Vect_Active;
    struct RVM_Param Vect;
    struct RVM_Param User;
};
/* Interrupt flags */
struct RVM_Vect_Flag
{
    rvm_ptr_t Timer;
    rvm_ptr_t Ctxsw;
    rvm_ptr_t Vect[1];
};
/* Interrupt handlers */
struct RVM_Vect_Handler
{
    void* Timer;
    void* Ctxsw;
    void* Vect[RVM_VIRT_VECT_NUM];
};
#endif
/*****************************************************************************/
/* End Structs ***************************************************************/

/* Private Global Variables **************************************************/
#ifdef __HDR_PRIVATE_MEMBERS__
#undef EXTERN
#define EXTERN
/*****************************************************************************/

/*****************************************************************************/
/* End Private Global Variables **********************************************/

/* Private C Function Prototypes *********************************************/ 
/*****************************************************************************/
#ifdef RVM_VIRT_VECT_NUM
static struct RVM_Vect_Handler RVM_Vect;
static rvm_ptr_t RVM_Int_Enable;
static rvm_ptr_t RVM_Vect_Pend;
static rvm_ptr_t RVM_Vect_Active;
#endif
/*****************************************************************************/
#endif /* __HDR_PRIVATE_MEMBERS__ */
/* End Private C Function Prototypes *****************************************/

/* Public Global Variables ***************************************************/
/*****************************************************************************/

/*****************************************************************************/
/* End Public Global Variables ***********************************************/

/* Public C Function Prototypes **********************************************/
/*****************************************************************************/
/* Memory clearing */
EXTERN void RVM_Clear(void* Addr, rvm_ptr_t Size);
/* Doubly linked list */
EXTERN void RVM_List_Crt(volatile struct RVM_List* Head);
EXTERN void RVM_List_Del(volatile struct RVM_List* Prev, volatile struct RVM_List* Next);
EXTERN void RVM_List_Ins(volatile struct RVM_List* New,
                         volatile struct RVM_List* Prev,
                         volatile struct RVM_List* Next);
/* Capability table operations */
EXTERN rvm_ret_t RVM_Captbl_Crt(rvm_cid_t Cap_Captbl_Crt, rvm_cid_t Cap_Kmem, 
                                rvm_cid_t Cap_Captbl, rvm_ptr_t Raddr, rvm_ptr_t Entry_Num);
EXTERN rvm_ret_t RVM_Captbl_Del(rvm_cid_t Cap_Captbl_Del, rvm_cid_t Cap_Del);
EXTERN rvm_ret_t RVM_Captbl_Frz(rvm_cid_t Cap_Captbl_Frz, rvm_cid_t Cap_Frz);
EXTERN rvm_ret_t RVM_Captbl_Add(rvm_cid_t Cap_Captbl_Dst, rvm_cid_t Cap_Dst, 
                                rvm_cid_t Cap_Captbl_Src, rvm_cid_t Cap_Src, rvm_ptr_t Flags);
EXTERN rvm_ret_t RVM_Captbl_Pgtbl(rvm_cid_t Cap_Captbl_Dst, rvm_cid_t Cap_Dst, 
                                  rvm_cid_t Cap_Captbl_Src, rvm_cid_t Cap_Src,
                                  rvm_ptr_t Start, rvm_ptr_t End, rvm_ptr_t Flags);
EXTERN rvm_ret_t RVM_Captbl_Kern(rvm_cid_t Cap_Captbl_Dst, rvm_cid_t Cap_Dst, 
                                 rvm_cid_t Cap_Captbl_Src, rvm_cid_t Cap_Src,
                                 rvm_ptr_t Start, rvm_ptr_t End);
EXTERN rvm_ret_t RVM_Captbl_Kmem(rvm_cid_t Cap_Captbl_Dst, rvm_cid_t Cap_Dst, 
                                 rvm_cid_t Cap_Captbl_Src, rvm_cid_t Cap_Src,
                                 rvm_ptr_t Start, rvm_ptr_t End, rvm_ptr_t Flags);
EXTERN rvm_ret_t RVM_Captbl_Rem(rvm_cid_t Cap_Captbl_Rem, rvm_cid_t Cap_Rem);
/* Kernel function operations */
EXTERN rvm_ret_t RVM_Kern_Act(rvm_cid_t Cap_Kern, rvm_ptr_t Func_ID, rvm_ptr_t Sub_ID, rvm_ptr_t Param1, rvm_ptr_t Param2);
/* Page table operations */
EXTERN rvm_ret_t RVM_Pgtbl_Crt(rvm_cid_t Cap_Captbl, rvm_cid_t Cap_Kmem, rvm_cid_t Cap_Pgtbl, 
                               rvm_ptr_t Raddr, rvm_ptr_t Base_Addr, rvm_ptr_t Top_Flag,
                               rvm_ptr_t Size_Order, rvm_ptr_t Num_Order);
EXTERN rvm_ret_t RVM_Pgtbl_Del(rvm_cid_t Cap_Captbl, rvm_cid_t Cap_Pgtbl);
EXTERN rvm_ret_t RVM_Pgtbl_Add(rvm_cid_t Cap_Pgtbl_Dst, rvm_ptr_t Pos_Dst, rvm_ptr_t Flags_Dst,
                               rvm_cid_t Cap_Pgtbl_Src, rvm_ptr_t Pos_Src, rvm_ptr_t Index);
EXTERN rvm_ret_t RVM_Pgtbl_Rem(rvm_cid_t Cap_Pgtbl, rvm_ptr_t Pos);
EXTERN rvm_ret_t RVM_Pgtbl_Con(rvm_cid_t Cap_Pgtbl_Parent, rvm_ptr_t Pos, rvm_cid_t Cap_Pgtbl_Child, rvm_ptr_t Flags_Child);
EXTERN rvm_ret_t RVM_Pgtbl_Des(rvm_cid_t Cap_Pgtbl_Parent, rvm_ptr_t Pos, rvm_cid_t Cap_Pgtbl_Child);
/* Process operations */
EXTERN rvm_ret_t RVM_Proc_Crt(rvm_cid_t Cap_Captbl_Crt, rvm_cid_t Cap_Proc,
                              rvm_cid_t Cap_Captbl, rvm_cid_t Cap_Pgtbl);
EXTERN rvm_ret_t RVM_Proc_Del(rvm_cid_t Cap_Captbl, rvm_cid_t Cap_Proc);
EXTERN rvm_ret_t RVM_Proc_Cpt(rvm_cid_t Cap_Proc, rvm_cid_t Cap_Captbl);
EXTERN rvm_ret_t RVM_Proc_Pgt(rvm_cid_t Cap_Proc, rvm_cid_t Cap_Pgtbl);
/* Thread operations */
EXTERN rvm_ret_t RVM_Thd_Crt(rvm_cid_t Cap_Captbl, rvm_cid_t Cap_Kmem, rvm_cid_t Cap_Thd,
                             rvm_cid_t Cap_Proc, rvm_ptr_t Max_Prio, rvm_ptr_t Raddr);
EXTERN rvm_ret_t RVM_Thd_Del(rvm_cid_t Cap_Captbl, rvm_cid_t Cap_Thd);
EXTERN rvm_ret_t RVM_Thd_Exec_Set(rvm_cid_t Cap_Thd, rvm_ptr_t Entry, rvm_ptr_t Stack, rvm_ptr_t Param);
EXTERN rvm_ret_t RVM_Thd_Hyp_Set(rvm_cid_t Cap_Thd, rvm_ptr_t Kaddr);
EXTERN rvm_ret_t RVM_Thd_Sched_Bind(rvm_cid_t Cap_Thd, rvm_cid_t Cap_Thd_Sched,
                                    rvm_cid_t Cap_Sig, rvm_tid_t TID, rvm_ptr_t Prio);
EXTERN rvm_ret_t RVM_Thd_Sched_Prio(rvm_cid_t Cap_Thd, rvm_ptr_t Prio);
EXTERN rvm_ret_t RVM_Thd_Sched_Free(rvm_cid_t Cap_Thd);
EXTERN rvm_ret_t RVM_Thd_Time_Xfer(rvm_cid_t Cap_Thd_Dst, rvm_cid_t Cap_Thd_Src, rvm_ptr_t Time);
EXTERN rvm_ret_t RVM_Thd_Swt(rvm_cid_t Cap_Thd, rvm_ptr_t Full_Yield);
/* Signal operations */
EXTERN rvm_ret_t RVM_Sig_Crt(rvm_cid_t Cap_Captbl, rvm_cid_t Cap_Sig);
EXTERN rvm_ret_t RVM_Sig_Del(rvm_cid_t Cap_Captbl, rvm_cid_t Cap_Sig);
EXTERN rvm_ret_t RVM_Sig_Snd(rvm_cid_t Cap_Sig);
EXTERN rvm_ret_t RVM_Sig_Rcv(rvm_cid_t Cap_Sig, rvm_ptr_t Option);
/* Invocation operations */
EXTERN rvm_ret_t RVM_Inv_Crt(rvm_cid_t Cap_Captbl, rvm_cid_t Cap_Kmem, 
                             rvm_cid_t Cap_Inv, rvm_cid_t Cap_Proc, rvm_ptr_t Raddr);
EXTERN rvm_ret_t RVM_Inv_Del(rvm_cid_t Cap_Captbl, rvm_cid_t Cap_Inv);
EXTERN rvm_ret_t RVM_Inv_Set(rvm_cid_t Cap_Inv, rvm_ptr_t Entry, rvm_ptr_t Stack, rvm_ptr_t Fault_Ret_Flag);

/* Debugging helpers */
EXTERN rvm_ret_t RVM_Print_Int(rvm_ret_t Int);
EXTERN rvm_ret_t RVM_Print_Uint(rvm_ptr_t Uint);
EXTERN rvm_ret_t RVM_Print_String(rvm_s8_t* String);

/* Process related */
EXTERN rvm_ret_t RVM_Proc_Send_Evt(rvm_ptr_t Evt_Num);

/* Virtual machine related - not used in normal processes */
#ifdef RVM_VIRT_VECT_NUM
EXTERN void RVM_Virt_Init(void);
EXTERN rvm_ret_t RVM_Virt_Reg_Vect(rvm_ptr_t Vect_Num, void* Vect);
EXTERN void RVM_Virt_Reg_Timer(void* Timer);
EXTERN void RVM_Virt_Reg_Ctxsw(void* Ctxsw);
EXTERN void RVM_Virt_Mask_Int(void);
EXTERN void RVM_Virt_Unmask_Int(void);
EXTERN void RVM_Virt_Yield(void);

/* Hypercall related - not used in normal processes */
EXTERN rvm_ret_t RVM_Hyp(rvm_ptr_t Number, rvm_ptr_t Param1, rvm_ptr_t Param2, rvm_ptr_t Param3, rvm_ptr_t Param4);
EXTERN void RVM_Hyp_Ena_Int(void);
EXTERN void RVM_Hyp_Dis_Int(void);
EXTERN rvm_ret_t RVM_Hyp_Reg_Phys(rvm_ptr_t Phys_Num, rvm_ptr_t Vect_Num);
EXTERN rvm_ret_t RVM_Hyp_Reg_Evt(rvm_ptr_t Evt_Num, rvm_ptr_t Vect_Num);
EXTERN rvm_ret_t RVM_Hyp_Del_Vect(rvm_ptr_t Vect_Num);
EXTERN rvm_ret_t RVM_Hyp_Wait_Vect(void);
EXTERN rvm_ret_t RVM_Hyp_Send_Evt(rvm_ptr_t Evt_Num);
EXTERN rvm_ret_t RVM_Hyp_Feed_Wdog(void);

/* Interrupt processing related - not used in normal processes */
EXTERN rvm_ret_t RVM_Get_Vect(void);
EXTERN void RVM_Vect_Loop(void);
#endif
/*****************************************************************************/
/* __RVM_GUEST__ */
#endif
/* End Public C Function Prototypes ******************************************/

/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
