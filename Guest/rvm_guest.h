/******************************************************************************
Filename    : rvm_guest.h
Author      : pry
Date        : 25/06/2017
Licence     : The Unlicense; see LICENSE for details.
Description : The header of guest user level low-level library.
******************************************************************************/

/* Define ********************************************************************/
#ifndef __RVM_GUEST__
#define __RVM_GUEST__
/*****************************************************************************/
/* Generic definitions */
#define RVM_NULL                                    ((void*)0)
#define RVM_EXIST                                   (1U)
#define RVM_EMPTY                                   (0U)

/* Powers of 2 */
#define RVM_FIELD(VAL,POW)                          (((rvm_ptr_t)(VAL))<<(POW))
#define RVM_POW2(POW)                               RVM_FIELD(1U,POW)
/* Word size */
#define RVM_WORD_BIT                                RVM_POW2(RVM_WORD_ORDER)
#define RVM_WORD_BYTE                               (RVM_WORD_BIT>>3)
#define RVM_WORD_BIT_O1                             (RVM_WORD_BYTE)
#define RVM_WORD_BIT_O2                             (RVM_WORD_BYTE*2U)
#define RVM_WORD_BIT_O3                             (RVM_WORD_BYTE*3U)
#define RVM_WORD_BIT_O4                             (RVM_WORD_BYTE*4U)
#define RVM_WORD_BIT_O5                             (RVM_WORD_BYTE*5U)
#define RVM_WORD_BIT_O6                             (RVM_WORD_BYTE*6U)
#define RVM_WORD_BIT_O7                             (RVM_WORD_BYTE*7U)
#define RVM_WORD_BIT_Q1                             RVM_WORD_BIT_O2
#define RVM_WORD_BIT_Q2                             RVM_WORD_BIT_O4
#define RVM_WORD_BIT_Q3                             RVM_WORD_BIT_O6
#define RVM_WORD_BIT_D1                             RVM_WORD_BIT_Q2
/* Bit mask */
#define RVM_MASK_FULL                               (~((rvm_ptr_t)0U))
#define RVM_MASK_WORD                               (~(RVM_MASK_FULL<<(RVM_WORD_ORDER-1U)))
#define RVM_MASK_WORD_O                             RVM_MASK_END(RVM_WORD_BIT_O1-1U)
#define RVM_MASK_WORD_Q                             RVM_MASK_END(RVM_WORD_BIT_Q1-1U)
#define RVM_MASK_WORD_D                             RVM_MASK_END(RVM_WORD_BIT_D1-1U)
/* Apply this mask to keep BEGIN to MSB bits */
#define RVM_MASK_BEGIN(BEGIN)                       ((RVM_MASK_FULL)<<(BEGIN))
/* Apply this mask to keep LSB to END bits */
#define RVM_MASK_END(END)                           ((RVM_MASK_FULL)>>(RVM_WORD_BIT-1U-(END)))
/* Apply this mask to keep BEGIN to END bits, BEGIN < END */
#define RVM_MASK(BEGIN,END)                         ((RVM_MASK_BEGIN(BEGIN))&(RVM_MASK_END(END)))
/* Round the number down & up to a power of 2, or get the power of 2 */
#define RVM_ROUND_DOWN(NUM,POW)                     ((NUM)&(RVM_MASK_BEGIN(POW)))
#define RVM_ROUND_UP(NUM,POW)                       RVM_ROUND_DOWN((NUM)+RVM_MASK_END(POW-1U),POW)
/* Bitmap */
#define RVM_BITMAP_SET(BMP,POS)                     ((BMP)[(POS)>>RVM_WORD_ORDER]|=RVM_POW2((POS)&RVM_MASK_WORD))
#define RVM_BITMAP_CLR(BMP,POS)                     ((BMP)[(POS)>>RVM_WORD_ORDER]&=~RVM_POW2((POS)&RVM_MASK_WORD))
#define RVM_BITMAP_IS_SET(BMP,POS)                  (((BMP)[(POS)>>RVM_WORD_ORDER]&RVM_POW2((POS)&RVM_MASK_WORD))!=0U)

/* Magic number for native processes */
#define RVM_MAGIC_NATIVE                            (0x49535953U)
/* Magic number for virtual machine processes */
#define RVM_MAGIC_VIRTUAL                           (0x56495254U)

/* System service stub */
#define RVM_SVC(SVC,CID,ARG1,ARG2,ARG3)             RVM_Svc(RVM_FIELD(SVC,RVM_WORD_BIT_D1)|((rvm_ptr_t)(CID)), \
                                                            ((rvm_ptr_t)(ARG1)),((rvm_ptr_t)(ARG2)),((rvm_ptr_t)(ARG3)))
/* User-level parameter marshalling macros - reverse of the kernel */
#define RVM_PARAM_D1(X)                             RVM_FIELD(((rvm_ptr_t)(X))&RVM_MASK_WORD_D,RVM_WORD_BIT_D1)
#define RVM_PARAM_D0(X)                             (((rvm_ptr_t)(X))&RVM_MASK_WORD_D)

#define RVM_PARAM_Q3(X)                             RVM_FIELD(((rvm_ptr_t)(X))&RVM_MASK_WORD_Q,RVM_WORD_BIT_Q3)
#define RVM_PARAM_Q2(X)                             RVM_FIELD(((rvm_ptr_t)(X))&RVM_MASK_WORD_Q,RVM_WORD_BIT_Q2)
#define RVM_PARAM_Q1(X)                             RVM_FIELD(((rvm_ptr_t)(X))&RVM_MASK_WORD_Q,RVM_WORD_BIT_Q1)
#define RVM_PARAM_Q0(X)                             (((rvm_ptr_t)(X))&RVM_MASK_WORD_Q)

#define RVM_PARAM_O7(X)                             RVM_FIELD(((rvm_ptr_t)(X))&RVM_MASK_WORD_O,RVM_WORD_BIT_O7)
#define RVM_PARAM_O6(X)                             RVM_FIELD(((rvm_ptr_t)(X))&RVM_MASK_WORD_O,RVM_WORD_BIT_O6)
#define RVM_PARAM_O5(X)                             RVM_FIELD(((rvm_ptr_t)(X))&RVM_MASK_WORD_O,RVM_WORD_BIT_O5)
#define RVM_PARAM_O4(X)                             RVM_FIELD(((rvm_ptr_t)(X))&RVM_MASK_WORD_O,RVM_WORD_BIT_O4)
#define RVM_PARAM_O3(X)                             RVM_FIELD(((rvm_ptr_t)(X))&RVM_MASK_WORD_O,RVM_WORD_BIT_O3)
#define RVM_PARAM_O2(X)                             RVM_FIELD(((rvm_ptr_t)(X))&RVM_MASK_WORD_O,RVM_WORD_BIT_O2)
#define RVM_PARAM_O1(X)                             RVM_FIELD(((rvm_ptr_t)(X))&RVM_MASK_WORD_O,RVM_WORD_BIT_O1)
#define RVM_PARAM_O0(X)                             (((rvm_ptr_t)(X))&RVM_MASK_WORD_O)

/* CID synthesis */
#define RVM_CID_NULL                                ((rvm_cid_t)RVM_POW2(RVM_WORD_BIT_D1-1U))
#define RVM_CID_2L                                  RVM_POW2(RVM_WORD_BIT_Q1-1U)
#define RVM_CID(X,Y)                                ((rvm_cid_t)(RVM_FIELD(X,RVM_WORD_BIT_Q1)|((rvm_ptr_t)(Y))|RVM_CID_2L))

/* Flag synthesis */
/* Kernel function */
#define RVM_KFN_FLAG(HIGH,LOW)                      (RVM_FIELD(HIGH,RVM_WORD_BIT_D1)|(LOW))
/* Kernel memory */
#define RVM_KOM_FLAG(HIGH,LOW)                      (((HIGH)&RVM_MASK_BEGIN(RVM_WORD_BIT_D1))|((LOW)>>(RVM_WORD_BIT_D1)))
#define RVM_KOM_SVC(HIGH,SVC)                       (((HIGH)&RVM_MASK_BEGIN(6U))|(SVC))
#define RVM_KOM_CID(LOW,FLAG)                       (((LOW)&RVM_MASK_BEGIN(6U))|(FLAG))
/* Page table */
#define RVM_PGT_SVC(NUM_ORDER,SVC)                  (RVM_FIELD(NUM_ORDER,RVM_WORD_BIT_Q1)|(SVC))
#define RVM_PGT_FLAG(HIGH,LOW,FLAG)                 (RVM_FIELD(HIGH,RVM_WORD_BIT_D1+4U)|((LOW)<<8)|(FLAG))
/* Page table size and number order */
#define RVM_PGT(SIZE,NUM)                           (RVM_FIELD(SIZE,RVM_WORD_BIT_D1)|(NUM))
#define RVM_PGT_SIZE(X)                             ((X)>>RVM_WORD_BIT_D1)
#define RVM_PGT_NUM(X)                              ((X)&RVM_MASK_WORD_D)
/* Thread time delegation */
/* Init thread infinite time marker */
#define RVM_THD_INIT_TIME                           (RVM_MASK_FULL>>1)
/* Other thread infinite time marker */
#define RVM_THD_INF_TIME                            (RVM_THD_INIT_TIME-1U)
/* Thread time upper limit - always ths infinite time */
#define RVM_THD_MAX_TIME                            (RVM_THD_INF_TIME)
/* Sched rcv return value's fault flag */
#define RVM_THD_EXCPEND_FLAG                        RVM_POW2(RVM_WORD_BIT-2U)
/* Sched rcv return value's timeout flag */
#define RVM_THD_TIMEOUT_FLAG                        RVM_POW2(RVM_WORD_BIT-3U)
/* Thread creation */
#define RVM_THD_SVC(ATTR,IS_HYP,SVC)                (RVM_FIELD(ATTR,7U)|RVM_FIELD(IS_HYP,6U)|(SVC))

/* Initial capability layout - same across all architectures */
/* The capability table of the init process */
#define RVM_BOOT_INIT_CPT                           (0U)
/* The top-level page table of the init process - always full memory access */
#define RVM_BOOT_INIT_PGT                           (1U)
/* The init process */
#define RVM_BOOT_INIT_PRC                           (2U)
/* The init thread */
#define RVM_BOOT_INIT_THD                           (3U)
/* The initial kernel function capability */
#define RVM_BOOT_INIT_KFN                           (4U)
/* The initial kernel memory capability */
#define RVM_BOOT_INIT_KOM                           (5U)
/* The initial timer/interrupt endpoint */
#define RVM_BOOT_INIT_VCT                           (6U)

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

/* Debug string length */
#define RVM_DBGLOG_MAX                              (255U)

#if(RVM_DBGLOG_ENABLE!=0U)
/* Debugging */
#define RVM_DBG_I(INT)                              RVM_Int_Print((rvm_cnt_t)(INT))
#define RVM_DBG_H(HEX)                              RVM_Hex_Print((rvm_ptr_t)(HEX))
#define RVM_DBG_S(STR)                              RVM_Str_Print((const rvm_s8_t*)(STR))
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
    
#define RVM_DBG_SHS(STR1,HEX,STR2) \
do \
{ \
    RVM_DBG_S(STR1); \
    RVM_DBG_H(HEX); \
    RVM_DBG_S(STR2); \
} \
while(0U)
    
#define RVM_DBG_SISHS(STR1,INT,STR2,HEX,STR3) \
do \
{ \
    RVM_DBG_S(STR1); \
    RVM_DBG_I(INT); \
    RVM_DBG_S(STR2); \
    RVM_DBG_H(HEX); \
    RVM_DBG_S(STR3); \
} \
while(0U)

/* Logging macro */
#ifndef RVM_LOG
#define RVM_LOG_OP(F,L,D,T)         RVM_Log(F,L,D,T)
#else
#define RVM_LOG_OP(F,L,D,T)         RVM_LOG(F,L,D,T)
#endif

/* Assert macro - used only in internal development */
#if(RVM_ASSERT_ENABLE!=0U)
#define RVM_ASSERT(X) \
do \
{ \
    if(!(X)) \
    { \
        RVM_LOG_OP(__FILE__,__LINE__,__DATE__,__TIME__); \
        while(1); \
    } \
} \
while(0)
#else
#define RVM_ASSERT(X) \
do \
{ \
    RVM_USE(X); \
} \
while(0)
#endif
    
/* Coverage marker enabling */
#ifdef RVM_COV_LINE_NUM
#define RVM_COV_WORD_NUM                            (RVM_ROUND_UP(RVM_COV_LINE_NUM,RVM_WORD_ORDER)>>RVM_WORD_ORDER)
#define RVM_COV_MARKER()                            RVM_BITMAP_SET(RVM_Cov,__LINE__)
#else
#define RVM_COV_MARKER()
#endif

/* Size of kernel objects that are architecture agnostic */
/* Capability table */
#define RVM_CPT_RAW_SIZE(NUM)                       ((((rvm_ptr_t)(NUM))<<3)*RVM_WORD_BYTE)
/* Rounded size of each object */
#define RVM_KOM_ROUND(X)                            RVM_ROUND_UP(((rvm_ptr_t)(X)),RVM_KOM_SLOT_ORDER)
/* Capability table */
#define RVM_CPT_SIZE(NUM)                           RVM_KOM_ROUND(RVM_CPT_RAW_SIZE(NUM))
/* Normal page directory */
#define RVM_PGT_SIZE_NOM(NUM_ORDER)                 RVM_KOM_ROUND(RVM_PGT_RAW_SIZE_NOM(NUM_ORDER))
/* Top-level page directory */
#define RVM_PGT_SIZE_TOP(NUM_ORDER)                 RVM_KOM_ROUND(RVM_PGT_RAW_SIZE_TOP(NUM_ORDER))
/* Thread */
#if(RVM_COP_NUM!=0U)
#define RVM_REG_RAW_SIZE(X)                         (sizeof(struct RVM_Thd_Reg)-RVM_WORD_BYTE+RVM_Thd_Cop_Size(X))
#else
#define RVM_REG_RAW_SIZE(X)                         (sizeof(struct RVM_Thd_Reg)-RVM_WORD_BYTE)
#endif
#define RVM_HYP_SIZE                                RVM_KOM_ROUND(RVM_HYP_RAW_SIZE)
#define RVM_REG_SIZE(X)                             RVM_KOM_ROUND(RVM_REG_RAW_SIZE(X))
#define RVM_THD_SIZE(X)                             RVM_KOM_ROUND(RVM_HYP_RAW_SIZE+RVM_REG_RAW_SIZE(X))
/* Invocation */
#define RVM_INV_SIZE                                RVM_KOM_ROUND(RVM_INV_RAW_SIZE)

#if(RVM_VIRT_LIB_ENABLE!=0U)
/* Register set space - might be changed by hypervisor */
#define RVM_REG                                     ((volatile struct RVM_Thd_Reg*)RVM_VIRT_REG_BASE)
/* State block space - might be changed by hypervisor */
#define RVM_STATE                                   ((volatile struct RVM_State*)RVM_VIRT_STATE_BASE)
/* Parameter block space (part of state block) */
#define RVM_PARAM                                   (&(RVM_STATE->Usr))
/* Vector space (part of state block) */
#define RVM_VCTF                                    (&(RVM_STATE->Flag))
/* Vector space words */
#define RVM_VCTF_WORD_SIZE                          (RVM_ROUND_UP(RVM_VIRT_VCT_NUM, RVM_WORD_ORDER-3U)/RVM_WORD_BYTE)

/* Virtualization signal endpoints */
/* Hypervisor signal endpoint */
#define RVM_SIG_HYP                                 (0U)
/* Vector signal endpoint */
#define RVM_SIG_VCT                                 (1U)

/* Hypercalls */
/* No operation - operation not ready */
#define RVM_HYP_INVALID                             (0U)
/* Print character */
#define RVM_HYP_PUTCHAR                             (1U)
/* Enable interrupts */
#define RVM_HYP_INT_ENA                             (2U)
/* Disable interrupts */
#define RVM_HYP_INT_DIS                             (3U)
/* Register a physical vector */
#define RVM_HYP_VCT_PHYS                            (4U)
/* Register a event */
#define RVM_HYP_VCT_EVT                             (5U)
/* Delete a virtual vector mapping */
#define RVM_HYP_VCT_DEL                             (6U)
/* Lockdown current virtual vector mapping */
#define RVM_HYP_VCT_LCK                             (7U)
/* Wait for an virtual vector to come */
#define RVM_HYP_VCT_WAIT                            (8U)
/* Add a event source to send to */
#define RVM_HYP_EVT_ADD                             (9U)
/* Delete a event source to send to */
#define RVM_HYP_EVT_DEL                             (10U)
/* Send to an event */
#define RVM_HYP_EVT_SND                             (11U)
/* Start and clear watchdog */
#define RVM_HYP_WDG_CLR                             (12U)
#endif
/* End Define ****************************************************************/

/* Struct ********************************************************************/
/*****************************************************************************/
/* Doubly-linked list */
struct RVM_List
{
    struct RVM_List* Next;
    struct RVM_List* Prev;
};

#if(RVM_VIRT_LIB_ENABLE!=0U)
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
    /* Size is calculated depending on the actual number of interrupts.
     * This field will be aligned to at least a machine word. */
    rvm_ptr_t Vct[1];
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
    struct RVM_Exc_Struct Exc;
    rvm_ptr_t Cop[1];
};
/*****************************************************************************/
/* End Struct ****************************************************************/

/* Private Variable **********************************************************/
#ifdef __HDR_PRIVATE__
#undef RVM_EXTERN
#define RVM_EXTERN
/*****************************************************************************/
#if(RVM_VIRT_LIB_ENABLE!=0U)
static volatile struct RVM_Handler_Struct RVM_Handler;
static volatile rvm_ptr_t RVM_Int_Mask;
static volatile rvm_ptr_t RVM_Vct_Pend;
/* Paramater space location provided for assembly convenience */
volatile struct RVM_Param* const RVM_Usr_Param=RVM_PARAM;
#endif
/*****************************************************************************/
/* End Private Variable ******************************************************/

/* Private Function **********************************************************/ 
/*****************************************************************************/

/*****************************************************************************/
#endif /* __HDR_PRIVATE__ */
/* End Private Function ******************************************************/

/* Public Variable ***********************************************************/
/*****************************************************************************/

/*****************************************************************************/
/* End Public Variable *******************************************************/

/* Public Function ***********************************************************/
/*****************************************************************************/
/* Debugging helper */
#if(RVM_DBGLOG_ENABLE!=0U)
RVM_EXTERN rvm_ret_t RVM_Int_Print(rvm_ret_t Int);
RVM_EXTERN rvm_ret_t RVM_Hex_Print(rvm_ptr_t Hex);
RVM_EXTERN rvm_ret_t RVM_Str_Print(const rvm_s8_t* String);
#endif

/* Default logging */
#ifndef RVM_LOG
RVM_EXTERN void RVM_Log(const char* File,
                        long Line,
                        const char* Date,
                        const char* Time);
#endif

/* Coverage test - internal use */
#ifdef RVM_COV_LINE_NUM
RVM_EXTERN void RVM_Cov_Print(void);
#endif

/* Memory clearing */
RVM_EXTERN void RVM_Clear(volatile void* Addr,
                          rvm_ptr_t Size);

/* Doubly linked list */
RVM_EXTERN void RVM_List_Crt(struct RVM_List* Head);
RVM_EXTERN void RVM_List_Del(struct RVM_List* Prev,
                             struct RVM_List* Next);
RVM_EXTERN void RVM_List_Ins(struct RVM_List* New,
                             struct RVM_List* Prev,
                             struct RVM_List* Next);
                         
/* Capability table operation */
#if(RVM_VIRT_LIB_ENABLE==0U)
RVM_EXTERN rvm_ret_t RVM_Cpt_Crt(rvm_cid_t Cap_Cpt_Crt,
                                 rvm_cid_t Cap_Kom, 
                                 rvm_cid_t Cap_Cpt,
                                 rvm_ptr_t Raddr,
                                 rvm_ptr_t Entry_Num);
RVM_EXTERN rvm_ret_t RVM_Cpt_Del(rvm_cid_t Cap_Cpt_Del,
                                 rvm_cid_t Cap_Del);
RVM_EXTERN rvm_ret_t RVM_Cpt_Frz(rvm_cid_t Cap_Cpt_Frz,
                                 rvm_cid_t Cap_Frz);
RVM_EXTERN rvm_ret_t RVM_Cpt_Add(rvm_cid_t Cap_Cpt_Dst,
                                 rvm_cid_t Cap_Dst, 
                                 rvm_cid_t Cap_Cpt_Src,
                                 rvm_cid_t Cap_Src,
                                 rvm_ptr_t Flag);
RVM_EXTERN rvm_ret_t RVM_Cpt_Pgt(rvm_cid_t Cap_Cpt_Dst,
                                 rvm_cid_t Cap_Dst, 
                                 rvm_cid_t Cap_Cpt_Src,
                                 rvm_cid_t Cap_Src,
                                 rvm_ptr_t Begin,
                                 rvm_ptr_t End,
                                 rvm_ptr_t Flag);
RVM_EXTERN rvm_ret_t RVM_Cpt_Kfn(rvm_cid_t Cap_Cpt_Dst,
                                 rvm_cid_t Cap_Dst, 
                                 rvm_cid_t Cap_Cpt_Src,
                                 rvm_cid_t Cap_Src,
                                 rvm_ptr_t Begin,
                                 rvm_ptr_t End);
RVM_EXTERN rvm_ret_t RVM_Cpt_Kom(rvm_cid_t Cap_Cpt_Dst,
                                 rvm_cid_t Cap_Dst, 
                                 rvm_cid_t Cap_Cpt_Src,
                                 rvm_cid_t Cap_Src,
                                 rvm_ptr_t Begin,
                                 rvm_ptr_t End,
                                 rvm_ptr_t Flag);
RVM_EXTERN rvm_ret_t RVM_Cpt_Rem(rvm_cid_t Cap_Cpt_Rem,
                                 rvm_cid_t Cap_Rem);
                              
/* Page table operation */
RVM_EXTERN rvm_ret_t RVM_Pgt_Crt(rvm_cid_t Cap_Cpt,
                                 rvm_cid_t Cap_Kom,
                                 rvm_cid_t Cap_Pgt, 
                                 rvm_ptr_t Raddr,
                                 rvm_ptr_t Base_Addr,
                                 rvm_ptr_t Is_Top,
                                 rvm_ptr_t Size_Order,
                                 rvm_ptr_t Num_Order);
RVM_EXTERN rvm_ret_t RVM_Pgt_Del(rvm_cid_t Cap_Cpt,
                                 rvm_cid_t Cap_Pgt);
RVM_EXTERN rvm_ret_t RVM_Pgt_Add(rvm_cid_t Cap_Pgt_Dst,
                                 rvm_ptr_t Pos_Dst,
                                 rvm_ptr_t Flag_Dst,
                                 rvm_cid_t Cap_Pgt_Src,
                                 rvm_ptr_t Pos_Src,
                                 rvm_ptr_t Index);
RVM_EXTERN rvm_ret_t RVM_Pgt_Rem(rvm_cid_t Cap_Pgt,
                                 rvm_ptr_t Pos);
RVM_EXTERN rvm_ret_t RVM_Pgt_Con(rvm_cid_t Cap_Pgt_Parent,
                                 rvm_ptr_t Pos,
                                 rvm_cid_t Cap_Pgt_Child,
                                 rvm_ptr_t Flag_Child);
RVM_EXTERN rvm_ret_t RVM_Pgt_Des(rvm_cid_t Cap_Pgt_Parent,
                                 rvm_ptr_t Pos,
                                 rvm_cid_t Cap_Pgt_Child);
                               
/* Process operation */
RVM_EXTERN rvm_ret_t RVM_Prc_Crt(rvm_cid_t Cap_Cpt_Crt,
                                 rvm_cid_t Cap_Prc,
                                 rvm_cid_t Cap_Cpt,
                                 rvm_cid_t Cap_Pgt);
RVM_EXTERN rvm_ret_t RVM_Prc_Del(rvm_cid_t Cap_Cpt,
                                 rvm_cid_t Cap_Prc);
RVM_EXTERN rvm_ret_t RVM_Prc_Cpt(rvm_cid_t Cap_Prc,
                                 rvm_cid_t Cap_Cpt);
RVM_EXTERN rvm_ret_t RVM_Prc_Pgt(rvm_cid_t Cap_Prc,
                                 rvm_cid_t Cap_Pgt);
                              
/* Thread operation */
RVM_EXTERN rvm_ret_t RVM_Thd_Crt(rvm_cid_t Cap_Cpt,
                                 rvm_cid_t Cap_Kom,
                                 rvm_cid_t Cap_Thd,
                                 rvm_cid_t Cap_Prc,
                                 rvm_ptr_t Prio_Max,
                                 rvm_ptr_t Raddr,
                                 rvm_ptr_t Attr);
RVM_EXTERN rvm_ret_t RVM_Hyp_Crt(rvm_cid_t Cap_Cpt,
                                 rvm_cid_t Cap_Kom,
                                 rvm_cid_t Cap_Thd,
                                 rvm_cid_t Cap_Prc,
                                 rvm_ptr_t Prio_Max,
                                 rvm_ptr_t Raddr,
                                 rvm_ptr_t Attr);
RVM_EXTERN rvm_ret_t RVM_Thd_Del(rvm_cid_t Cap_Cpt,
                                 rvm_cid_t Cap_Thd);
RVM_EXTERN rvm_ret_t RVM_Thd_Exec_Set(rvm_cid_t Cap_Thd,
                                      void* Entry,
                                      void* Stack,
                                      void* Param);
RVM_EXTERN rvm_ret_t RVM_Thd_Sched_Bind(rvm_cid_t Cap_Thd,
                                        rvm_cid_t Cap_Thd_Sched,
                                        rvm_cid_t Cap_Sig,
                                        rvm_tid_t TID,
                                        rvm_ptr_t Prio);
RVM_EXTERN rvm_ret_t RVM_Hyp_Sched_Bind(rvm_cid_t Cap_Thd,
                                        rvm_cid_t Cap_Thd_Sched,
                                        rvm_cid_t Cap_Sig,
                                        rvm_tid_t TID,
                                        rvm_ptr_t Prio,
                                        rvm_ptr_t Haddr);
RVM_EXTERN rvm_ret_t RVM_Thd_Sched_Prio(rvm_cid_t Cap_Thd,
                                        rvm_ptr_t Prio);
RVM_EXTERN rvm_ret_t RVM_Thd_Sched_Prio2(rvm_cid_t Cap_Thd0,
                                         rvm_ptr_t Prio0,
                                         rvm_cid_t Cap_Thd1,
                                         rvm_ptr_t Prio1);
RVM_EXTERN rvm_ret_t RVM_Thd_Sched_Prio3(rvm_cid_t Cap_Thd0,
                                         rvm_ptr_t Prio0,
                                         rvm_cid_t Cap_Thd1,
                                         rvm_ptr_t Prio1,
                                         rvm_cid_t Cap_Thd2,
                                         rvm_ptr_t Prio2);
RVM_EXTERN rvm_ret_t RVM_Thd_Sched_Free(rvm_cid_t Cap_Thd);
RVM_EXTERN rvm_ret_t RVM_Thd_Sched_Rcv(rvm_cid_t Cap_Thd);
RVM_EXTERN rvm_ret_t RVM_Thd_Time_Xfer(rvm_cid_t Cap_Thd_Dst,
                                       rvm_cid_t Cap_Thd_Src,
                                       rvm_ptr_t Time);
RVM_EXTERN rvm_ret_t RVM_Thd_Swt(rvm_cid_t Cap_Thd,
                                 rvm_ptr_t Is_Yield);
                             
/* Signal operation */
RVM_EXTERN rvm_ret_t RVM_Sig_Crt(rvm_cid_t Cap_Cpt,
                                 rvm_cid_t Cap_Sig);
RVM_EXTERN rvm_ret_t RVM_Sig_Del(rvm_cid_t Cap_Cpt,
                                 rvm_cid_t Cap_Sig);
#endif
RVM_EXTERN rvm_ret_t RVM_Sig_Snd(rvm_cid_t Cap_Sig);
RVM_EXTERN rvm_ret_t RVM_Sig_Rcv(rvm_cid_t Cap_Sig,
                                 rvm_ptr_t Option);

/* Kernel function operation */
RVM_EXTERN rvm_ret_t RVM_Kfn_Act(rvm_cid_t Cap_Kfn,
                             rvm_ptr_t Func_ID,
                             rvm_ptr_t Sub_ID,
                             rvm_ptr_t Param1,
                             rvm_ptr_t Param2);

#if(RVM_VIRT_LIB_ENABLE==0U)
/* Invocation operation */
RVM_EXTERN rvm_ret_t RVM_Inv_Crt(rvm_cid_t Cap_Cpt,
                                 rvm_cid_t Cap_Kom, 
                                 rvm_cid_t Cap_Inv,
                                 rvm_cid_t Cap_Prc,
                                 rvm_ptr_t Raddr);
RVM_EXTERN rvm_ret_t RVM_Inv_Del(rvm_cid_t Cap_Cpt,
                                 rvm_cid_t Cap_Inv);
RVM_EXTERN rvm_ret_t RVM_Inv_Set(rvm_cid_t Cap_Inv,
                                 rvm_ptr_t Entry,
                                 rvm_ptr_t Stack,
                                 rvm_ptr_t Is_Exc_Ret);
                                 
/* Process event operation */
RVM_EXTERN rvm_ret_t RVM_Prc_Evt_Snd(rvm_ptr_t Evt_Num);
#else
/* Virtual machine library call */
RVM_EXTERN rvm_ret_t RVM_Virt_Vct_Reg(rvm_ptr_t Vct_Num,
                                      void (*Vct)(void));
RVM_EXTERN rvm_ret_t RVM_Virt_Vct_Trig(rvm_ptr_t Vct_Num);
RVM_EXTERN void RVM_Virt_Tim_Reg(void (*Tim)(void));
RVM_EXTERN void RVM_Virt_Ctx_Reg(void (*Ctx)(void));
RVM_EXTERN void RVM_Virt_Int_Mask(void);
RVM_EXTERN void RVM_Virt_Int_Unmask(void);
RVM_EXTERN void RVM_Virt_Yield(void);

/* Hypercall */
RVM_EXTERN rvm_ret_t RVM_Hyp(rvm_ptr_t Number,
                             rvm_ptr_t Param1,
                             rvm_ptr_t Param2,
                             rvm_ptr_t Param3,
                             rvm_ptr_t Param4);

RVM_EXTERN void RVM_Hyp_Putchar(char Char);

RVM_EXTERN void RVM_Hyp_Int_Ena(void);
RVM_EXTERN void RVM_Hyp_Int_Dis(void);

RVM_EXTERN rvm_ret_t RVM_Hyp_Vct_Phys(rvm_ptr_t Phys_Num,
                                      rvm_ptr_t Vct_Num);
RVM_EXTERN rvm_ret_t RVM_Hyp_Vct_Evt(rvm_ptr_t Evt_Num,
                                     rvm_ptr_t Vct_Num);
RVM_EXTERN rvm_ret_t RVM_Hyp_Vct_Del(rvm_ptr_t Vct_Num);
RVM_EXTERN rvm_ret_t RVM_Hyp_Vct_Lck(void);
RVM_EXTERN rvm_ret_t RVM_Hyp_Vct_Wait(void);

RVM_EXTERN rvm_ret_t RVM_Hyp_Evt_Add(rvm_ptr_t Evt_Num);
RVM_EXTERN rvm_ret_t RVM_Hyp_Evt_Del(rvm_ptr_t Evt_Num);
RVM_EXTERN rvm_ret_t RVM_Hyp_Evt_Snd(rvm_ptr_t Evt_Num);

RVM_EXTERN rvm_ret_t RVM_Hyp_Wdg_Clr(void);

/* Vct thread dedicated operation */
RVM_EXTERN void RVM_Virt_Init(void);
RVM_EXTERN rvm_ret_t RVM_Vct_Get(void);
RVM_EXTERN void RVM_Vct_Loop(void);
#endif
/*****************************************************************************/
/* __RVM_GUEST__ */
#endif
/* End Public Function *******************************************************/

/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
