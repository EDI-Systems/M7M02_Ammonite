/******************************************************************************
Filename    : syssvc.h
Author      : pry
Date        : 25/06/2017
Licence     : The Unlicense; see LICENSE for details.
Description : The header of MPU-based user level low-level library.
******************************************************************************/

/* Defines *******************************************************************/
#ifdef __HDR_DEFS__
#ifndef __RVM_SYSSVC_H_DEFS__
#define __RVM_SYSSVC_H_DEFS__
/*****************************************************************************/
/* Generic definitions */
#define RVM_NULL                                    (0U)
#define RVM_EXIST                                   (1U)
#define RVM_EMPTY                                   (0U)

/* Debug string length */
#define RVM_USER_DEBUG_MAX_STR                      (256U)
/* Magic number for native processes */
#define RVM_MAGIC_NATIVE                            (0x49535953U)
/* Magic number for virtual machine processes */
#define RVM_MAGIC_VIRTUAL                           (0x56495254U)

#if(RVM_DEBUG_PRINT==1U)
/* Debug prints */
#define RVM_DBG_I(INT)                              RVM_Int_Print(INT)
#define RVM_DBG_H(UINT)                             RVM_Hex_Print(UINT)
#define RVM_DBG_S(STR)                              RVM_Str_Print((rvm_s8_t*)(STR))
#else
#define RVM_DBG_I(INT)                              while(0U)
#define RVM_DBG_H(UINT)                             while(0U)
#define RVM_DBG_S(STR)                              while(0U)
#endif

#define RVM_DBG_SIS(STR1,INT,STR2) \
do \
{ \
    RVM_DBG_S(STR1); \
    RVM_DBG_I(INT); \
    RVM_DBG_S(STR2); \
} \
while(0)
    
#define RVM_DBG_SHS(STR1,UINT,STR2) \
do \
{ \
    RVM_DBG_S(STR1); \
    RVM_DBG_H(UINT); \
    RVM_DBG_S(STR2); \
} \
while(0)
    
#define RVM_DBG_SISHS(STR1,INT,STR2,UINT,STR3) \
do \
{ \
    RVM_DBG_S(STR1); \
    RVM_DBG_I(INT); \
    RVM_DBG_S(STR2); \
    RVM_DBG_H(UINT); \
    RVM_DBG_S(STR3); \
} \
while(0)

#if(RVM_ASSERT_CORRECT==0U)
/* Assert macro - used only in internal development */
#define RVM_ASSERT(X) \
do \
{ \
    if((X)==0) \
    { \
        RVM_DBG_S("\r\n***\r\nUser-level library panic - not syncing:\r\n"); \
        RVM_DBG_S(__FILE__); \
        RVM_DBG_S(", Line "); \
        RVM_DBG_I(__LINE__); \
        RVM_DBG_S("\r\n"); \
        RVM_DBG_S(__DATE__); \
        RVM_DBG_S(", "); \
        RVM_DBG_S(__TIME__); \
        RVM_DBG_S("\r\n"); \
        RVM_Kern_Act(RVM_BOOT_INIT_KERN,RVM_KERN_SYS_REBOOT,0,0,0); \
        while(1); \
    } \
} \
while(0)
#else
#define RVM_ASSERT(X) \
do \
{ \
    if((X)==0) {} \
} \
while(0)
#endif

/* Powers of 2 */
#define RVM_POW2(POW)                               (((rvm_ptr_t)1U)<<(POW))
/* Bit mask/address operations */
#define RVM_ALLBITS                                 (~((rvm_ptr_t)0U))
/* Word size settings */
#define RVM_WORD_SIZE                               RVM_POW2(RVM_WORD_ORDER)
#define RVM_WORD_MASK                               (~(RVM_ALLBITS<<(RVM_WORD_ORDER-1U)))
/* Apply this mask to keep START to MSB bits */
#define RVM_MASK_START(START)                       ((RVM_ALLBITS)<<(START))
/* Apply this mask to keep LSB to END bits */
#define RVM_MASK_END(END)                           ((RVM_ALLBITS)>>(RVM_WORD_SIZE-1U-(END)))
/* Apply this mask to keep START to END bits, START < END */
#define RVM_MASK(START,END)                         ((RVM_MASK_START(START))&(RVM_MASK_END(END)))
/* Round the number down & up to a power of 2, or get the power of 2 */
#define RVM_ROUND_DOWN(NUM,POW)                     ((NUM)&(RVM_MASK_START(POW)))
#define RVM_ROUND_UP(NUM,POW)                       RVM_ROUND_DOWN((NUM)+RVM_MASK_END(POW-1U),POW)

/* System service stub */
#define RVM_CAP_OP(OP,CAPID,ARG1,ARG2,ARG3)         RVM_Svc(((OP)<<(sizeof(rvm_ptr_t)*4U))|((rvm_ptr_t)(CAPID)), \
                                                            ((rvm_ptr_t)(ARG1)),((rvm_ptr_t)(ARG2)),((rvm_ptr_t)(ARG3)))
#define RVM_PARAM_D_MASK                            ((RVM_ALLBITS)>>(sizeof(rvm_ptr_t)*4U))
#define RVM_PARAM_Q_MASK                            ((RVM_ALLBITS)>>(sizeof(rvm_ptr_t)*6U))
#define RVM_PARAM_O_MASK                            ((RVM_ALLBITS)>>(sizeof(rvm_ptr_t)*7U))
/* The parameter passing - not to be confused with kernel macros. These macros just place the parameters */
#define RVM_PARAM_D1(X)                             ((((rvm_ptr_t)(X))&RVM_PARAM_D_MASK)<<(sizeof(rvm_ptr_t)*4U))
#define RVM_PARAM_D0(X)                             (((rvm_ptr_t)(X))&RVM_PARAM_D_MASK)

#define RVM_PARAM_Q3(X)                             ((((rvm_ptr_t)(X))&RVM_PARAM_Q_MASK)<<(sizeof(rvm_ptr_t)*6U))
#define RVM_PARAM_Q2(X)                             ((((rvm_ptr_t)(X))&RVM_PARAM_Q_MASK)<<(sizeof(rvm_ptr_t)*4U))
#define RVM_PARAM_Q1(X)                             ((((rvm_ptr_t)(X))&RVM_PARAM_Q_MASK)<<(sizeof(rvm_ptr_t)*2U))
#define RVM_PARAM_Q0(X)                             (((rvm_ptr_t)(X))&RVM_PARAM_Q_MASK)

#define RVM_PARAM_O7(X)                             ((((rvm_ptr_t)(X))&RVM_PARAM_O_MASK)<<(sizeof(rvm_ptr_t)*7U))
#define RVM_PARAM_O6(X)                             ((((rvm_ptr_t)(X))&RVM_PARAM_O_MASK)<<(sizeof(rvm_ptr_t)*6U))
#define RVM_PARAM_O5(X)                             ((((rvm_ptr_t)(X))&RVM_PARAM_O_MASK)<<(sizeof(rvm_ptr_t)*5U))
#define RVM_PARAM_O4(X)                             ((((rvm_ptr_t)(X))&RVM_PARAM_O_MASK)<<(sizeof(rvm_ptr_t)*4U))
#define RVM_PARAM_O3(X)                             ((((rvm_ptr_t)(X))&RVM_PARAM_O_MASK)<<(sizeof(rvm_ptr_t)*3U))
#define RVM_PARAM_O2(X)                             ((((rvm_ptr_t)(X))&RVM_PARAM_O_MASK)<<(sizeof(rvm_ptr_t)*2U))
#define RVM_PARAM_O1(X)                             ((((rvm_ptr_t)(X))&RVM_PARAM_O_MASK)<<(sizeof(rvm_ptr_t)*1U))
#define RVM_PARAM_O0(X)                             (((rvm_ptr_t)(X))&RVM_PARAM_O_MASK)

/* CID synthesis */
#define RME_CAPID_NULL                              (1<<(sizeof(rvm_ptr_t)*4U-1U))
#define RVM_CAPID_2L                                (1<<(sizeof(rvm_ptr_t)*2U-1U))
#define RVM_CAPID(X,Y)                              (((X)<<(sizeof(rvm_ptr_t)*2U))|(Y)|RVM_CAPID_2L)

/* Flag synthesis */
/* Kernel function */
#define RVM_KERN_FLAG(HIGH,LOW)                     (((HIGH)<<(sizeof(rvm_ptr_t)*4U))|(LOW))
/* Kernel memory */
#define RVM_KMEM_FLAG(HIGH,LOW)                     ((((HIGH)>>(sizeof(rvm_ptr_t)*4U))<<(sizeof(rvm_ptr_t)*4U))| \
                                                    ((LOW)>>(sizeof(rvm_ptr_t)*4U)))
#define RVM_KMEM_SVC(HIGH,SVC)                      (((((HIGH)>>6)<<(sizeof(rvm_ptr_t)*4U+6U))>>(sizeof(rvm_ptr_t)*4U))|(SVC))
#define RVM_KMEM_CAPID(LOW,FLAGS)                   (((((LOW)>>6)<<(sizeof(rvm_ptr_t)*4U+6U))>>(sizeof(rvm_ptr_t)*4U))|(FLAGS))
/* Page table */
#define RVM_PGTBL_SVC(NUM_ORDER,SVC)                (((NUM_ORDER)<<(sizeof(rvm_ptr_t)<<1))|(SVC))
#define RVM_PGTBL_FLAG(HIGH,LOW,FLAGS)              (((HIGH)<<(sizeof(rvm_ptr_t)*4U+4U))|((LOW)<<8)|(FLAGS))
/* Page table size and number order */
#define RVM_PGTBL(SIZE,NUM)                         (((SIZE)<<(sizeof(rvm_ptr_t)<<2))|(NUM))
#define RVM_PGTBL_SIZE(X)                           ((X)>>(sizeof(rvm_ptr_t)<<2))
#define RVM_PGTBL_NUM(X)                            ((X)&(RVM_ALLBITS>>(sizeof(rvm_ptr_t)<<2)))
/* Thread time delegation */
/* Init thread infinite time marker */
#define RVM_THD_INIT_TIME                           (RVM_ALLBITS>>1)
/* Other thread infinite time marker */
#define RVM_THD_INF_TIME                            (RVM_THD_INIT_TIME-1U)
/* Thread time upper limit - always ths infinite time */
#define RVM_THD_MAX_TIME                            (RVM_THD_INF_TIME)
/* Sched rcv return value's fault flag */
#define RVM_THD_FAULT_FLAG                          ((rvm_tid_t)RVM_POW2(sizeof(rvm_ptr_t)*8U-2U))
    
/* Size of kernel objects */
/* Capability table */
#define RVM_CAPTBL_WORD_SIZE(NUM)                   (((rvm_ptr_t)(NUM))<<3)

/* Rounded size of each object */
#define RVM_KOBJ_ROUND(X)                           RVM_ROUND_UP((((rvm_ptr_t)(X))*sizeof(rvm_ptr_t)),RVM_KMEM_SLOT_ORDER)
/* Capability table */
#define RVM_CAPTBL_SIZE(NUM)                        RVM_KOBJ_ROUND(RVM_CAPTBL_WORD_SIZE(NUM))
/* Normal page directory */
#define RVM_PGTBL_SIZE_NOM(NUM_ORDER)               RVM_KOBJ_ROUND(RVM_PGTBL_WORD_SIZE_NOM(NUM_ORDER))
/* Top-level page directory */
#define RVM_PGTBL_SIZE_TOP(NUM_ORDER)               RVM_KOBJ_ROUND(RVM_PGTBL_WORD_SIZE_TOP(NUM_ORDER))
/* Process */
#define RVM_PROC_SIZE                               RVM_KOBJ_ROUND(RVM_PROC_WORD_SIZE)
/* Thread */
#define RVM_THD_SIZE                                RVM_KOBJ_ROUND(RVM_THD_WORD_SIZE)
/* Signal */                           
#define RVM_SIG_SIZE                                RVM_KOBJ_ROUND(RVM_SIG_WORD_SIZE)
/* Invocation */
#define RVM_INV_SIZE                                RVM_KOBJ_ROUND(RVM_INV_WORD_SIZE)

/* The TLS masks */ 
#define RVM_TLS_MASK_128B                           RVM_ROUND_DOWN(RVM_ALLBITS, 7U)
#define RVM_TLS_MASK_256B                           RVM_ROUND_DOWN(RVM_ALLBITS, 8U)
#define RVM_TLS_MASK_512B                           RVM_ROUND_DOWN(RVM_ALLBITS, 9U)
#define RVM_TLS_MASK_1KB                            RVM_ROUND_DOWN(RVM_ALLBITS, 10U)
#define RVM_TLS_MASK_2KB                            RVM_ROUND_DOWN(RVM_ALLBITS, 11U)
#define RVM_TLS_MASK_4KB                            RVM_ROUND_DOWN(RVM_ALLBITS, 12U)
#define RVM_TLS_MASK_8KB                            RVM_ROUND_DOWN(RVM_ALLBITS, 13U)
#define RVM_TLS_MASK_16KB                           RVM_ROUND_DOWN(RVM_ALLBITS, 14U)
#define RVM_TLS_MASK_32KB                           RVM_ROUND_DOWN(RVM_ALLBITS, 15U)
#define RVM_TLS_MASK_64KB                           RVM_ROUND_DOWN(RVM_ALLBITS, 16U)

/* Initial capability layout - same across all architectures */
/* The capability table of the init process */
#define RVM_BOOT_CAPTBL                             (0U)
/* The top-level page table of the init process - always full memory access */
#define RVM_BOOT_PGTBL                              (1U)
/* The init process */
#define RVM_BOOT_INIT_PROC                          (2U)
/* The init thread */
#define RVM_BOOT_INIT_THD                           (3U)
/* The initial kernel function capability */
#define RVM_BOOT_INIT_KERN                          (4U)
/* The initial kernel memory capability */
#define RVM_BOOT_INIT_KMEM                          (5U)
/* The initial timer endpoint */
#define RVM_BOOT_INIT_TIMER                         (6U)
/* The initial interrupt endpoint */
#define RVM_BOOT_INIT_VECT                          (7U)

/* Helper capability definitions */
/* The capability to its capability table */
#define RVM_VIRT_CAPTBL(X)                          RVM_CAPID(RVM_VIRT_TBL_CAPPROC,((X)*2U))
/* The capability to its process */
#define RVM_VIRT_PROC(X)                            RVM_CAPID(RVM_VIRT_TBL_CAPPROC,((X)*2U+1U))
/* The capability to its user thread */
#define RVM_VIRT_USERTHD(X)                         RVM_CAPID(RVM_VIRT_TBL_THDSIG,((X)*3U))
/* The capability to its interrupt thread */
#define RVM_VIRT_INTTHD(X)                          RVM_CAPID(RVM_VIRT_TBL_THDSIG,((X)*3U)+1U)
/* The capability to its interrupt thread signal endpoint */
#define RVM_VIRT_INTSIG(X)                          RVM_CAPID(RVM_VIRT_TBL_THDSIG,((X)*3U)+2U)
/*****************************************************************************/
/* __RVM_SYSSVC_H_DEFS__ */
#endif
/* __HDR_DEFS__ */
#endif
/* End Defines ***************************************************************/

/* Structs *******************************************************************/
#ifdef __HDR_STRUCTS__
#ifndef __RVM_SYSSVC_H_STRUCTS__
#define __RVM_SYSSVC_H_STRUCTS__
/* We used structs in the header */

/* Use defines in these headers */
#define __HDR_DEFS__
#undef __HDR_DEFS__
/*****************************************************************************/
/* Doubly-linked list */
struct RVM_List
{
    volatile struct RVM_List* Prev;
    volatile struct RVM_List* Next;
};
/*****************************************************************************/
/* __RVM_SYSSVC_H_STRUCTS__ */
#endif
/* __HDR_STRUCTS__ */
#endif
/* End Structs ***************************************************************/

/* Private Global Variables **************************************************/
#if(!(defined __HDR_DEFS__||defined __HDR_STRUCTS__))
#ifndef __RVM_SYSSVC_MEMBERS__
#define __RVM_SYSSVC_MEMBERS__

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
__EXTERN__ void RVM_List_Del(volatile struct RVM_List* Prev,
                             volatile struct RVM_List* Next);
__EXTERN__ void RVM_List_Ins(volatile struct RVM_List* New,
                             volatile struct RVM_List* Prev,
                             volatile struct RVM_List* Next);
                             
/* Capability table operations */
__EXTERN__ rvm_ret_t RVM_Captbl_Crt(rvm_cid_t Cap_Captbl_Crt,
                                    rvm_cid_t Cap_Kmem, 
                                    rvm_cid_t Cap_Captbl,
                                    rvm_ptr_t Raddr,
                                    rvm_ptr_t Entry_Num);
__EXTERN__ rvm_ret_t RVM_Captbl_Del(rvm_cid_t Cap_Captbl_Del,
                                    rvm_cid_t Cap_Del);
__EXTERN__ rvm_ret_t RVM_Captbl_Frz(rvm_cid_t Cap_Captbl_Frz,
                                    rvm_cid_t Cap_Frz);
__EXTERN__ rvm_ret_t RVM_Captbl_Add(rvm_cid_t Cap_Captbl_Dst,
                                    rvm_cid_t Cap_Dst, 
                                    rvm_cid_t Cap_Captbl_Src,
                                    rvm_cid_t Cap_Src,
                                    rvm_ptr_t Flags);
__EXTERN__ rvm_ret_t RVM_Captbl_Pgtbl(rvm_cid_t Cap_Captbl_Dst,
                                      rvm_cid_t Cap_Dst, 
                                      rvm_cid_t Cap_Captbl_Src,
                                      rvm_cid_t Cap_Src,
                                      rvm_ptr_t Start,
                                      rvm_ptr_t End,
                                      rvm_ptr_t Flags);
__EXTERN__ rvm_ret_t RVM_Captbl_Kern(rvm_cid_t Cap_Captbl_Dst,
                                     rvm_cid_t Cap_Dst, 
                                     rvm_cid_t Cap_Captbl_Src,
                                     rvm_cid_t Cap_Src,
                                     rvm_ptr_t Start,
                                     rvm_ptr_t End);
__EXTERN__ rvm_ret_t RVM_Captbl_Kmem(rvm_cid_t Cap_Captbl_Dst,
                                     rvm_cid_t Cap_Dst, 
                                     rvm_cid_t Cap_Captbl_Src,
                                     rvm_cid_t Cap_Src,
                                     rvm_ptr_t Start,
                                     rvm_ptr_t End,
                                     rvm_ptr_t Flags);
__EXTERN__ rvm_ret_t RVM_Captbl_Rem(rvm_cid_t Cap_Captbl_Rem,
                                    rvm_cid_t Cap_Rem);
                                    
/* Kernel function operations */
__EXTERN__ rvm_ret_t RVM_Kern_Act(rvm_cid_t Cap_Kern,
                                  rvm_ptr_t Func_ID,
                                  rvm_ptr_t Sub_ID,
                                  rvm_ptr_t Param1,
                                  rvm_ptr_t Param2);
                                  
/* Page table operations */
__EXTERN__ rvm_ret_t RVM_Pgtbl_Crt(rvm_cid_t Cap_Captbl,
                                   rvm_cid_t Cap_Kmem,
                                   rvm_cid_t Cap_Pgtbl, 
                                   rvm_ptr_t Raddr,
                                   rvm_ptr_t Base_Addr,
                                   rvm_ptr_t Top_Flag,
                                   rvm_ptr_t Size_Order,
                                   rvm_ptr_t Num_Order);
__EXTERN__ rvm_ret_t RVM_Pgtbl_Del(rvm_cid_t Cap_Captbl,
                                   rvm_cid_t Cap_Pgtbl);
__EXTERN__ rvm_ret_t RVM_Pgtbl_Add(rvm_cid_t Cap_Pgtbl_Dst,
                                   rvm_ptr_t Pos_Dst,
                                   rvm_ptr_t Flags_Dst,
                                   rvm_cid_t Cap_Pgtbl_Src,
                                   rvm_ptr_t Pos_Src,
                                   rvm_ptr_t Index);
__EXTERN__ rvm_ret_t RVM_Pgtbl_Rem(rvm_cid_t Cap_Pgtbl,
                                   rvm_ptr_t Pos);
__EXTERN__ rvm_ret_t RVM_Pgtbl_Con(rvm_cid_t Cap_Pgtbl_Parent,
                                   rvm_ptr_t Pos,
                                   rvm_cid_t Cap_Pgtbl_Child,
                                   rvm_ptr_t Flags_Child);
__EXTERN__ rvm_ret_t RVM_Pgtbl_Des(rvm_cid_t Cap_Pgtbl_Parent,
                                   rvm_ptr_t Pos,
                                   rvm_cid_t Cap_Pgtbl_Child);
                                   
/* Process operations */
__EXTERN__ rvm_ret_t RVM_Proc_Crt(rvm_cid_t Cap_Captbl_Crt,
                                  rvm_cid_t Cap_Proc,
                                  rvm_cid_t Cap_Captbl,
                                  rvm_cid_t Cap_Pgtbl);
__EXTERN__ rvm_ret_t RVM_Proc_Del(rvm_cid_t Cap_Captbl,
                                  rvm_cid_t Cap_Proc);
__EXTERN__ rvm_ret_t RVM_Proc_Cpt(rvm_cid_t Cap_Proc,
                                  rvm_cid_t Cap_Captbl);
__EXTERN__ rvm_ret_t RVM_Proc_Pgt(rvm_cid_t Cap_Proc,
                                  rvm_cid_t Cap_Pgtbl);
                                  
/* Thread operations */
__EXTERN__ rvm_ret_t RVM_Thd_Crt(rvm_cid_t Cap_Captbl,
                                 rvm_cid_t Cap_Kmem,
                                 rvm_cid_t Cap_Thd,
                                 rvm_cid_t Cap_Proc,
                                 rvm_ptr_t Max_Prio,
                                 rvm_ptr_t Raddr);
__EXTERN__ rvm_ret_t RVM_Thd_Del(rvm_cid_t Cap_Captbl,
                                 rvm_cid_t Cap_Thd);
__EXTERN__ rvm_ret_t RVM_Thd_Exec_Set(rvm_cid_t Cap_Thd,
                                      rvm_ptr_t Entry,
                                      rvm_ptr_t Stack,
                                      rvm_ptr_t Param);
__EXTERN__ rvm_ret_t RVM_Thd_Hyp_Set(rvm_cid_t Cap_Thd,
                                     rvm_ptr_t Kaddr);
__EXTERN__ rvm_ret_t RVM_Thd_Hyp_Exec_Set(rvm_cid_t Cap_Thd,
                                          rvm_ptr_t Kaddr,
                                          rvm_ptr_t Entry,
                                          rvm_ptr_t Stack);
__EXTERN__ rvm_ret_t RVM_Thd_Sched_Bind(rvm_cid_t Cap_Thd,
                                        rvm_cid_t Cap_Thd_Sched,
                                        rvm_cid_t Cap_Sig,
                                        rvm_tid_t TID,
                                        rvm_ptr_t Prio);
__EXTERN__ rvm_ret_t RVM_Thd_Sched_Prio(rvm_cid_t Cap_Thd,
                                        rvm_ptr_t Prio);
__EXTERN__ rvm_ret_t RVM_Thd_Sched_Prio2(rvm_cid_t Cap_Thd0,
                                         rvm_ptr_t Prio0,
                                         rvm_cid_t Cap_Thd1,
                                         rvm_ptr_t Prio1);
__EXTERN__ rvm_ret_t RVM_Thd_Sched_Prio3(rvm_cid_t Cap_Thd0,
                                         rvm_ptr_t Prio0,
                                         rvm_cid_t Cap_Thd1,
                                         rvm_ptr_t Prio1,
                                         rvm_cid_t Cap_Thd2,
                                         rvm_ptr_t Prio2);
__EXTERN__ rvm_ret_t RVM_Thd_Sched_Free(rvm_cid_t Cap_Thd);
__EXTERN__ rvm_ret_t RVM_Thd_Time_Xfer(rvm_cid_t Cap_Thd_Dst,
                                       rvm_cid_t Cap_Thd_Src,
                                       rvm_ptr_t Time);
__EXTERN__ rvm_ret_t RVM_Thd_Swt(rvm_cid_t Cap_Thd,
                                 rvm_ptr_t Full_Yield);
                                 
/* Signal operations */
__EXTERN__ rvm_ret_t RVM_Sig_Crt(rvm_cid_t Cap_Captbl,
                                 rvm_cid_t Cap_Sig);
__EXTERN__ rvm_ret_t RVM_Sig_Del(rvm_cid_t Cap_Captbl,
                                 rvm_cid_t Cap_Sig);
__EXTERN__ rvm_ret_t RVM_Sig_Snd(rvm_cid_t Cap_Sig);
__EXTERN__ rvm_ret_t RVM_Sig_Rcv(rvm_cid_t Cap_Sig,
                                 rvm_ptr_t Option);
                                 
/* Invocation operations */
__EXTERN__ rvm_ret_t RVM_Inv_Crt(rvm_cid_t Cap_Captbl,
                                 rvm_cid_t Cap_Kmem, 
                                 rvm_cid_t Cap_Inv,
                                 rvm_cid_t Cap_Proc,
                                 rvm_ptr_t Raddr);
__EXTERN__ rvm_ret_t RVM_Inv_Del(rvm_cid_t Cap_Captbl,
                                 rvm_cid_t Cap_Inv);
__EXTERN__ rvm_ret_t RVM_Inv_Set(rvm_cid_t Cap_Inv,
                                 rvm_ptr_t Entry,
                                 rvm_ptr_t Stack,
                                 rvm_ptr_t Fault_Ret_Flag);
#if(RVM_DEBUG_PRINT==1U)
/* Debugging */
__EXTERN__ rvm_ret_t RVM_Int_Print(rvm_ret_t Int);
__EXTERN__ rvm_ret_t RVM_Hex_Print(rvm_ptr_t Uint);
__EXTERN__ rvm_ret_t RVM_Str_Print(rvm_s8_t* String);
#endif
/*****************************************************************************/
/* Undefine "__EXTERN__" to avoid redefinition */
#undef __EXTERN__
/* __RVM_SYSSVC_MEMBERS__ */
#endif
/* !(defined __HDR_DEFS__||defined __HDR_STRUCTS__) */
#endif
/* End Public C Function Prototypes ******************************************/

/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/