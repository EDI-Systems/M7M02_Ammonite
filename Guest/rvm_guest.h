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
#define RVM_NULL                                    ((void*)0U)
#define RVM_EXIST                                   (1U)
#define RVM_EMPTY                                   (0U)

/* Debug string length */
#define RVM_USER_DEBUG_MAX_STR                      (256U)
/* Magic number for native processes */
#define RVM_MAGIC_NATIVE                            (0x49535953U)
/* Magic number for virtual machine processes */
#define RVM_MAGIC_VIRTUAL                           (0x56495254U)

/* Bit mask/address operations */
#define RVM_ALLBITS                                 (~((rvm_ptr_t)0U))
/* Word size settings */
#define RVM_WORD_SIZE                               RVM_POW2(RVM_WORD_ORDER)
#define RVM_WORD_BYTE                               (RVM_WORD_SIZE>>3)
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
#define RVM_POW2(POW)                               (((rvm_ptr_t)1U)<<(POW))

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
#define RVM_KFN_FLAG(HIGH, LOW)                     (((HIGH)<<(sizeof(rvm_ptr_t)*4U))|(LOW))
/* Kernel memory */
#define RVM_KOM_FLAG(HIGH, LOW)                     ((((HIGH)>>(sizeof(rvm_ptr_t)*4U))<<(sizeof(rvm_ptr_t)*4U))| \
                                                    ((LOW)>>(sizeof(rvm_ptr_t)*4U)))
#define RVM_KOM_SVC(HIGH, SVC)                      (((((HIGH)>>6)<<(sizeof(rvm_ptr_t)*4U+6U))>>(sizeof(rvm_ptr_t)*4U))|(SVC))
#define RVM_KOM_CAPID(LOW, FLAG)                    (((((LOW)>>6)<<(sizeof(rvm_ptr_t)*4U+6U))>>(sizeof(rvm_ptr_t)*4U))|(FLAG))
/* Page table */
#define RVM_PGT_SVC(NUM_ORDER, SVC)                 (((NUM_ORDER)<<(sizeof(rvm_ptr_t)<<1))|(SVC))
#define RVM_PGT_FLAG(HIGH,LOW, FLAG)                (((HIGH)<<(sizeof(rvm_ptr_t)*4U+4U))|((LOW)<<8)|(FLAG))
/* Page table size and number order */
#define RVM_PGT(SIZE, NUM)                          (((SIZE)<<(sizeof(rvm_ptr_t)<<2))|(NUM))
#define RVM_PGT_SIZE(X)                             ((X)>>(sizeof(rvm_ptr_t)<<2))
#define RVM_PGT_NUM(X)                              ((X)&(RVM_ALLBITS>>(sizeof(rvm_ptr_t)<<2)))
/* Thread time delegation */
/* Init thread infinite time marker */
#define RVM_THD_INIT_TIME                           (RVM_ALLBITS>>1)
/* Other thread infinite time marker */
#define RVM_THD_INF_TIME                            (RVM_THD_INIT_TIME-1U)
/* Thread time upper limit - always ths infinite time */
#define RVM_THD_MAX_TIME                            (RVM_THD_INF_TIME)
/* Sched rcv return value's fault flag */
#define RVM_THD_EXC_FLAG                            ((rvm_tid_t)RVM_POW2(sizeof(rvm_ptr_t)*8U-2U))
    
/* Size of kernel objects that are architecture agnostic */
/* Capability table */
#define RVM_CPT_WORD_SIZE(NUM)                      (((rvm_ptr_t)(NUM))<<3)

/* Rounded size of each object */
#define RVM_KOM_ROUND(X)                            RVM_ROUND_UP((((rvm_ptr_t)(X))*sizeof(rvm_ptr_t)),RVM_KOM_SLOT_ORDER)
/* Capability table */
#define RVM_CPT_SIZE(NUM)                           RVM_KOM_ROUND(RVM_CPT_WORD_SIZE(NUM))
/* Normal page directory */
#define RVM_PGT_SIZE_NOM(NUM_ORDER)                 RVM_KOM_ROUND(RVM_PGT_WORD_SIZE_NOM(NUM_ORDER))
/* Top-level page directory */
#define RVM_PGT_SIZE_TOP(NUM_ORDER)                 RVM_KOM_ROUND(RVM_PGT_WORD_SIZE_TOP(NUM_ORDER))
/* Process */
#define RVM_PRC_SIZE                                RVM_KOM_ROUND(RVM_PRC_WORD_SIZE)
/* Thread */
#define RVM_THD_SIZE                                RVM_KOM_ROUND(RVM_THD_WORD_SIZE)
/* Signal */                           
#define RVM_SIG_SIZE                                RVM_KOM_ROUND(RVM_SIG_WORD_SIZE)
/* Invocation */
#define RVM_INV_SIZE                                RVM_KOM_ROUND(RVM_INV_WORD_SIZE)

/* Round the kernel object size to the entry slot size */
#define RVM_KOTBL_ROUND(X)                          RVM_ROUND_UP(X,RVM_KOM_SLOT_ORDER)

/* Initial capability layout - same across all architectures */
/* The capability table of the init process */
#define RVM_BOOT_CPT                                (0U)
/* The top-level page table of the init process - always full memory access */
#define RVM_BOOT_PGT                                (1U)
/* The init process */
#define RVM_BOOT_INIT_PRC                           (2U)
/* The init thread */
#define RVM_BOOT_INIT_THD                           (3U)
/* The initial kernel function capability */
#define RVM_BOOT_INIT_KFN                           (4U)
/* The initial kernel memory capability */
#define RVM_BOOT_INIT_KOM                           (5U)
/* The initial timer endpoint */
#define RVM_BOOT_INIT_TIM                           (6U)
/* The initial interrupt endpoint */
#define RVM_BOOT_INIT_VCT                           (7U)

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

#if(RVM_DEBUG_PRINT==1U)
/* Debugging */
#define RVM_DBG_I(INT)                              RVM_Int_Print((rvm_cnt_t)(INT))
#define RVM_DBG_H(UINT)                             RVM_Hex_Print((rvm_ptr_t)(UINT))
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
while(0U)
    
#define RVM_DBG_SHS(STR1,UINT,STR2) \
do \
{ \
    RVM_DBG_S(STR1); \
    RVM_DBG_H(UINT); \
    RVM_DBG_S(STR2); \
} \
while(0U)
    
#define RVM_DBG_SISHS(STR1,INT,STR2,UINT,STR3) \
do \
{ \
    RVM_DBG_S(STR1); \
    RVM_DBG_I(INT); \
    RVM_DBG_S(STR2); \
    RVM_DBG_H(UINT); \
    RVM_DBG_S(STR3); \
} \
while(0U)

#if(RVM_ASSERT_CORRECT==0U)
/* Assert macro - won't be optimized out */
#define RVM_ASSERT(X) \
do \
{ \
    if((X)==0) \
    { \
        RVM_DBG_S("\r\n***\r\nGuest panic - rebooting:\r\n"); \
        RVM_DBG_S(__FILE__); \
        RVM_DBG_S(", Line "); \
        RVM_DBG_I(__LINE__); \
        RVM_DBG_S("\r\n"); \
        RVM_DBG_S(__DATE__); \
        RVM_DBG_S(", "); \
        RVM_DBG_S(__TIME__); \
        RVM_DBG_S("\r\n"); \
        *((volatile rvm_ptr_t*)0U)=0U; \
        while(1U); \
    } \
} \
while(0U)
#else
#define RVM_ASSERT(X) \
do \
{ \
    if((X)==0) {} \
} \
while(0U)
#endif

/* Coprocessor */
#define RVM_COPROCESSOR_NONE                        (0U)

#ifdef RVM_VIRT_VCT_NUM
/* Register set space */
#define RVM_REG                                     ((volatile struct RVM_Thd_Reg*)RVM_VIRT_REG_BASE)
/* State block space */
#define RVM_STATE                                   ((volatile struct RVM_State*)RVM_VIRT_STATE_BASE)
/* Vector space (part of state block) */
#define RVM_VCTF                                    (&(RVM_STATE->Flag))

/* Virtualization signal endpoints */
/* Hypervisor signal endpoint */
#define RVM_SIG_HYP                                 (0U)
/* Vector signal endpoint */
#define RVM_SIG_VCT                                 (1U)

/* Hypercalls */
/* Print character */
#define RVM_HYP_PUTCHAR                             (0U)
/* Enable interrupts */
#define RVM_HYP_INT_ENA                             (1U)
/* Disable interrupts */
#define RVM_HYP_INT_DIS                             (2U)
/* Register a physical vector */
#define RVM_HYP_VCT_PHYS                            (3U)
/* Register a event */
#define RVM_HYP_VCT_EVT                             (4U)
/* Delete a virtual vector mapping */
#define RVM_HYP_VCT_DEL                             (5U)
/* Lockdown current virtual vector mapping */
#define RVM_HYP_VCT_LCK                             (6U)
/* Wait for an virtual vector to come */
#define RVM_HYP_VCT_WAIT                            (7U)
/* Add a event source to send to */
#define RVM_HYP_EVT_ADD                             (8U)
/* Delete a event source to send to */
#define RVM_HYP_EVT_DEL                             (9U)
/* Send to an event */
#define RVM_HYP_EVT_SND                             (10U)
/* Start and clear watchdog */
#define RVM_HYP_WDG_CLR                             (11U)
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

#ifdef RVM_VIRT_VCT_NUM
/* Parameter */
struct RVM_Param
{
    rvm_ptr_t Number;
    rvm_ptr_t Param[4];
};

/* Interrupt flags - Vct in bytes */
struct RVM_Vctf
{
    rvm_ptr_t Tim;
    rvm_ptr_t Ctx;
    rvm_u8_t Vct[16];
};

/* State block */
struct RVM_State
{
    /* Indicate that the vector thread is now active */
    rvm_ptr_t Vct_Act;
    /* Parameter area, one for vector, another for user */
    struct RVM_Param Vct;
    struct RVM_Param Usr;
    /* Interrupt flags */
    struct RVM_Vctf Flag;
};

/* Interrupt handlers */
struct RVM_Handler_Struct
{
    void (*Tim)(void);
    void (*Ctx)(void);
    void (*Vct[RVM_VIRT_VCT_NUM])(void);
};
#endif

struct RVM_Thd_Reg
{
    struct RVM_Reg_Struct Reg;
#if(RVM_COPROCESSOR_TYPE!=RVM_COPROCESSOR_NONE)
    struct RVM_Cop_Struct Cop;
#endif
    struct RVM_Exc_Struct Exc;
};
/*****************************************************************************/
/* End Structs ***************************************************************/

/* Private Global Variables **************************************************/
#ifdef __HDR_PRIVATE_MEMBERS__
#undef EXTERN
#define EXTERN
/*****************************************************************************/
#ifdef RVM_VIRT_VCT_NUM
static volatile struct RVM_Handler_Struct RVM_Handler;
static volatile rvm_ptr_t RVM_Int_Mask;
static volatile rvm_ptr_t RVM_Vct_Pend;
#endif
/*****************************************************************************/
/* End Private Global Variables **********************************************/

/* Private C Function Prototypes *********************************************/ 
/*****************************************************************************/

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
EXTERN void RVM_Clear(volatile void* Addr,
                      rvm_ptr_t Size);

/* Doubly linked list */
EXTERN void RVM_List_Crt(struct RVM_List* Head);
EXTERN void RVM_List_Del(struct RVM_List* Prev,
                         struct RVM_List* Next);
EXTERN void RVM_List_Ins(struct RVM_List* New,
                         struct RVM_List* Prev,
                         struct RVM_List* Next);
                         
/* Capability table operations */
EXTERN rvm_ret_t RVM_Cpt_Crt(rvm_cid_t Cap_Cpt_Crt,
                             rvm_cid_t Cap_Kom, 
                             rvm_cid_t Cap_Cpt,
                             rvm_ptr_t Raddr,
                             rvm_ptr_t Entry_Num);
EXTERN rvm_ret_t RVM_Cpt_Del(rvm_cid_t Cap_Cpt_Del,
                             rvm_cid_t Cap_Del);
EXTERN rvm_ret_t RVM_Cpt_Frz(rvm_cid_t Cap_Cpt_Frz,
                             rvm_cid_t Cap_Frz);
EXTERN rvm_ret_t RVM_Cpt_Add(rvm_cid_t Cap_Cpt_Dst,
                             rvm_cid_t Cap_Dst, 
                             rvm_cid_t Cap_Cpt_Src,
                             rvm_cid_t Cap_Src,
                             rvm_ptr_t Flag);
EXTERN rvm_ret_t RVM_Cpt_Pgt(rvm_cid_t Cap_Cpt_Dst,
                             rvm_cid_t Cap_Dst, 
                             rvm_cid_t Cap_Cpt_Src,
                             rvm_cid_t Cap_Src,
                             rvm_ptr_t Start,
                             rvm_ptr_t End,
                             rvm_ptr_t Flag);
EXTERN rvm_ret_t RVM_Cpt_Kfn(rvm_cid_t Cap_Cpt_Dst,
                             rvm_cid_t Cap_Dst, 
                             rvm_cid_t Cap_Cpt_Src,
                             rvm_cid_t Cap_Src,
                             rvm_ptr_t Start,
                             rvm_ptr_t End);
EXTERN rvm_ret_t RVM_Cpt_Kom(rvm_cid_t Cap_Cpt_Dst,
                             rvm_cid_t Cap_Dst, 
                             rvm_cid_t Cap_Cpt_Src,
                             rvm_cid_t Cap_Src,
                             rvm_ptr_t Start,
                             rvm_ptr_t End,
                             rvm_ptr_t Flag);
EXTERN rvm_ret_t RVM_Cpt_Rem(rvm_cid_t Cap_Cpt_Rem,
                             rvm_cid_t Cap_Rem);
                                
/* Kernel function operations */
EXTERN rvm_ret_t RVM_Kfn_Act(rvm_cid_t Cap_Kfn,
                             rvm_ptr_t Func_ID,
                             rvm_ptr_t Sub_ID,
                             rvm_ptr_t Param1,
                             rvm_ptr_t Param2);
                              
/* Page table operations */
EXTERN rvm_ret_t RVM_Pgt_Crt(rvm_cid_t Cap_Cpt,
                             rvm_cid_t Cap_Kom,
                             rvm_cid_t Cap_Pgt, 
                             rvm_ptr_t Raddr,
                             rvm_ptr_t Base_Addr,
                             rvm_ptr_t Is_Top,
                             rvm_ptr_t Size_Order,
                             rvm_ptr_t Num_Order);
EXTERN rvm_ret_t RVM_Pgt_Del(rvm_cid_t Cap_Cpt,
                             rvm_cid_t Cap_Pgt);
EXTERN rvm_ret_t RVM_Pgt_Add(rvm_cid_t Cap_Pgt_Dst,
                             rvm_ptr_t Pos_Dst,
                             rvm_ptr_t Flag_Dst,
                             rvm_cid_t Cap_Pgt_Src,
                             rvm_ptr_t Pos_Src,
                             rvm_ptr_t Index);
EXTERN rvm_ret_t RVM_Pgt_Rem(rvm_cid_t Cap_Pgt,
                             rvm_ptr_t Pos);
EXTERN rvm_ret_t RVM_Pgt_Con(rvm_cid_t Cap_Pgt_Parent,
                             rvm_ptr_t Pos,
                             rvm_cid_t Cap_Pgt_Child,
                             rvm_ptr_t Flag_Child);
EXTERN rvm_ret_t RVM_Pgt_Des(rvm_cid_t Cap_Pgt_Parent,
                             rvm_ptr_t Pos,
                             rvm_cid_t Cap_Pgt_Child);
                               
/* Process operations */
EXTERN rvm_ret_t RVM_Prc_Crt(rvm_cid_t Cap_Cpt_Crt,
                             rvm_cid_t Cap_Prc,
                             rvm_cid_t Cap_Cpt,
                             rvm_cid_t Cap_Pgt);
EXTERN rvm_ret_t RVM_Prc_Del(rvm_cid_t Cap_Cpt,
                             rvm_cid_t Cap_Prc);
EXTERN rvm_ret_t RVM_Prc_Cpt(rvm_cid_t Cap_Prc,
                             rvm_cid_t Cap_Cpt);
EXTERN rvm_ret_t RVM_Prc_Pgt(rvm_cid_t Cap_Prc,
                             rvm_cid_t Cap_Pgt);
                              
/* Thread operations */
EXTERN rvm_ret_t RVM_Thd_Crt(rvm_cid_t Cap_Cpt,
                             rvm_cid_t Cap_Kom,
                             rvm_cid_t Cap_Thd,
                             rvm_cid_t Cap_Prc,
                             rvm_ptr_t Prio_Max,
                             rvm_ptr_t Raddr);
EXTERN rvm_ret_t RVM_Thd_Del(rvm_cid_t Cap_Cpt,
                             rvm_cid_t Cap_Thd);
EXTERN rvm_ret_t RVM_Thd_Exec_Set(rvm_cid_t Cap_Thd,
                                  rvm_ptr_t Entry,
                                  rvm_ptr_t Stack,
                                  rvm_ptr_t Param);
EXTERN rvm_ret_t RVM_Thd_Hyp_Set(rvm_cid_t Cap_Thd,
                                 rvm_ptr_t Kaddr);
EXTERN rvm_ret_t RVM_Thd_Hyp_Exec_Set(rvm_cid_t Cap_Thd,
                                      rvm_ptr_t Kaddr,
                                      rvm_ptr_t Entry,
                                      rvm_ptr_t Stack);
EXTERN rvm_ret_t RVM_Thd_Sched_Bind(rvm_cid_t Cap_Thd,
                                    rvm_cid_t Cap_Thd_Sched,
                                    rvm_cid_t Cap_Sig,
                                    rvm_tid_t TID,
                                    rvm_ptr_t Prio);
EXTERN rvm_ret_t RVM_Thd_Sched_Prio(rvm_cid_t Cap_Thd,
                                    rvm_ptr_t Prio);
EXTERN rvm_ret_t RVM_Thd_Sched_Prio2(rvm_cid_t Cap_Thd0,
                                     rvm_ptr_t Prio0,
                                     rvm_cid_t Cap_Thd1,
                                     rvm_ptr_t Prio1);
EXTERN rvm_ret_t RVM_Thd_Sched_Prio3(rvm_cid_t Cap_Thd0,
                                     rvm_ptr_t Prio0,
                                     rvm_cid_t Cap_Thd1,
                                     rvm_ptr_t Prio1,
                                     rvm_cid_t Cap_Thd2,
                                     rvm_ptr_t Prio2);
EXTERN rvm_ret_t RVM_Thd_Sched_Free(rvm_cid_t Cap_Thd);
EXTERN rvm_ret_t RVM_Thd_Sched_Rcv(rvm_cid_t Cap_Thd);
EXTERN rvm_ret_t RVM_Thd_Time_Xfer(rvm_cid_t Cap_Thd_Dst,
                                   rvm_cid_t Cap_Thd_Src,
                                   rvm_ptr_t Time);
EXTERN rvm_ret_t RVM_Thd_Swt(rvm_cid_t Cap_Thd,
                             rvm_ptr_t Is_Yield);
                             
/* Signal operations */
EXTERN rvm_ret_t RVM_Sig_Crt(rvm_cid_t Cap_Cpt,
                             rvm_cid_t Cap_Sig);
EXTERN rvm_ret_t RVM_Sig_Del(rvm_cid_t Cap_Cpt,
                             rvm_cid_t Cap_Sig);
EXTERN rvm_ret_t RVM_Sig_Snd(rvm_cid_t Cap_Sig);
EXTERN rvm_ret_t RVM_Sig_Rcv(rvm_cid_t Cap_Sig,
                             rvm_ptr_t Option);
                             
/* Invocation operations */
EXTERN rvm_ret_t RVM_Inv_Crt(rvm_cid_t Cap_Cpt,
                             rvm_cid_t Cap_Kom, 
                             rvm_cid_t Cap_Inv,
                             rvm_cid_t Cap_Prc,
                             rvm_ptr_t Raddr);
EXTERN rvm_ret_t RVM_Inv_Del(rvm_cid_t Cap_Cpt,
                             rvm_cid_t Cap_Inv);
EXTERN rvm_ret_t RVM_Inv_Set(rvm_cid_t Cap_Inv,
                             rvm_ptr_t Entry,
                             rvm_ptr_t Stack,
                             rvm_ptr_t Is_Exc_Ret);

#if(RVM_DEBUG_PRINT==1U)
/* Debugging helpers */
EXTERN rvm_ret_t RVM_Int_Print(rvm_ret_t Int);
EXTERN rvm_ret_t RVM_Hex_Print(rvm_ptr_t Uint);
EXTERN rvm_ret_t RVM_Str_Print(rvm_s8_t* String);
#endif

/* Process related */
EXTERN rvm_ret_t RVM_Prc_Evt_Snd(rvm_ptr_t Evt_Num);

/* Virtual machine related - not used in normal processes */
#ifdef RVM_VIRT_VCT_NUM

/* Virtual machine operations */
EXTERN rvm_ret_t RVM_Virt_Vct_Reg(rvm_ptr_t Vect_Num,
                                  void (*Vct)(void));
EXTERN void RVM_Virt_Tim_Reg(void (*Tim)(void));
EXTERN void RVM_Virt_Ctx_Reg(void (*Ctx)(void));
EXTERN void RVM_Virt_Int_Mask(void);
EXTERN void RVM_Virt_Int_Unmask(void);
EXTERN void RVM_Virt_Yield(void);

/* Hypercalls */
EXTERN rvm_ret_t RVM_Hyp(rvm_ptr_t Number,
                         rvm_ptr_t Param1,
                         rvm_ptr_t Param2,
                         rvm_ptr_t Param3,
                         rvm_ptr_t Param4);

EXTERN void RVM_Hyp_Putchar(char Char);

EXTERN void RVM_Hyp_Int_Ena(void);
EXTERN void RVM_Hyp_Int_Dis(void);

EXTERN rvm_ret_t RVM_Hyp_Vct_Phys(rvm_ptr_t Phys_Num,
                                  rvm_ptr_t Vect_Num);
EXTERN rvm_ret_t RVM_Hyp_Vct_Evt(rvm_ptr_t Evt_Num,
                                 rvm_ptr_t Vect_Num);
EXTERN rvm_ret_t RVM_Hyp_Vct_Del(rvm_ptr_t Vect_Num);
EXTERN rvm_ret_t RVM_Hyp_Vct_Lck(void);
EXTERN rvm_ret_t RVM_Hyp_Vct_Wait(void);

EXTERN rvm_ret_t RVM_Hyp_Evt_Add(rvm_ptr_t Evt_Num);
EXTERN rvm_ret_t RVM_Hyp_Evt_Del(rvm_ptr_t Evt_Num);
EXTERN rvm_ret_t RVM_Hyp_Evt_Snd(rvm_ptr_t Evt_Num);

EXTERN rvm_ret_t RVM_Hyp_Wdg_Clr(void);

/* Vect thread dedicated */
EXTERN void RVM_Virt_Init(void);
EXTERN rvm_ret_t RVM_Vct_Get(void);
EXTERN void RVM_Vct_Loop(void);
#endif
/*****************************************************************************/
/* __RVM_GUEST__ */
#endif
/* End Public C Function Prototypes ******************************************/

/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
