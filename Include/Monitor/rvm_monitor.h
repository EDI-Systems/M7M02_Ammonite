/******************************************************************************
Filename    : rvm_monitor.h
Author      : pry
Date        : 25/06/2017
Licence     : The Unlicense; see LICENSE for details.
Description : The header of MPU-based user level low-level library.
******************************************************************************/

/* Define ********************************************************************/
#ifdef __HDR_DEF__
#ifndef __RVM_MONITOR_DEF__
#define __RVM_MONITOR_DEF__
/*****************************************************************************/
/* Generic definitions */
#define RVM_NULL                                    (0U)
#define RVM_EXIST                                   (1U)
#define RVM_EMPTY                                   (0U)

/* Power of 2 */
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

/* Maximum logging length */
#define RVM_DBGLOG_MAX                              (255U)

#if(RVM_DBGLOG_ENABLE!=0U)
/* Debug prints */
#define RVM_DBG_I(INT)                              RVM_Int_Print((rvm_cnt_t)(INT))
#define RVM_DBG_H(HEX)                              RVM_Hex_Print((rvm_ptr_t)(HEX))
#define RVM_DBG_S(STR)                              RVM_Str_Print((const rvm_s8_t*)(STR))
#else
#define RVM_DBG_I(INT)                              while(0U)
#define RVM_DBG_H(HEX)                              while(0U)
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
    
#define RVM_DBG_SHS(STR1,HEX,STR2) \
do \
{ \
    RVM_DBG_S(STR1); \
    RVM_DBG_H(HEX); \
    RVM_DBG_S(STR2); \
} \
while(0)
    
#define RVM_DBG_SISHS(STR1,INT,STR2,HEX,STR3) \
do \
{ \
    RVM_DBG_S(STR1); \
    RVM_DBG_I(INT); \
    RVM_DBG_S(STR2); \
    RVM_DBG_H(HEX); \
    RVM_DBG_S(STR3); \
} \
while(0)

/* Logging macro */
#ifndef RVM_LOG
#define RVM_LOG_OP(F,L,D,T)                         RVM_Log(F,L,D,T)
#else
#define RVM_LOG_OP(F,L,D,T)                         RVM_LOG(F,L,D,T)
#endif

/* Assert macro - used only in internal development */
#if(RVM_ASSERT_ENABLE!=0U)
#define RVM_ASSERT(X) \
do \
{ \
    if(!(X)) \
    { \
        RVM_LOG_OP(__FILE__,__LINE__,__DATE__,__TIME__); \
        RVM_Kfn_Act(RVM_BOOT_INIT_KFN,RVM_KFN_SYS_REBOOT,0U,0U,0U); \
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
    
/* Size of kernel objects */
/* Rounded size of each object */
#define RVM_KOM_ROUND(X)                            RVM_ROUND_UP((rvm_ptr_t)(X),RVM_KOM_SLOT_ORDER)
/* Capability table raw size */
#define RVM_CPT_RAW_SIZE(NUM)                       (((rvm_ptr_t)(NUM))*(8U*RVM_WORD_BYTE))
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

/* The TLS masks */ 
#define RVM_TLS_MASK_128B                           RVM_ROUND_DOWN(RVM_MASK_FULL,7U)
#define RVM_TLS_MASK_256B                           RVM_ROUND_DOWN(RVM_MASK_FULL,8U)
#define RVM_TLS_MASK_512B                           RVM_ROUND_DOWN(RVM_MASK_FULL,9U)
#define RVM_TLS_MASK_1KB                            RVM_ROUND_DOWN(RVM_MASK_FULL,10U)
#define RVM_TLS_MASK_2KB                            RVM_ROUND_DOWN(RVM_MASK_FULL,11U)
#define RVM_TLS_MASK_4KB                            RVM_ROUND_DOWN(RVM_MASK_FULL,12U)
#define RVM_TLS_MASK_8KB                            RVM_ROUND_DOWN(RVM_MASK_FULL,13U)
#define RVM_TLS_MASK_16KB                           RVM_ROUND_DOWN(RVM_MASK_FULL,14U)
#define RVM_TLS_MASK_32KB                           RVM_ROUND_DOWN(RVM_MASK_FULL,15U)
#define RVM_TLS_MASK_64KB                           RVM_ROUND_DOWN(RVM_MASK_FULL,16U)

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
/* The initial interrupt endpoint */
#define RVM_BOOT_INIT_VCT                           (6U)

/* The priority levels (assuming that the kernel have N levels):
 * (N-1)   : Safety daemon - fixed
 * (N-2)-5 : Native processes
 * 4       : Timer daemon (adjustable from 29 to 4)
 *           Virtual machine monitor daemon
 *           Vector handling daemon
 * 3       : Active virtual machine - vector thread
 * 2       : Active virtual machine - user thread
 * 1       : Init daemon
 * 0       : Inactive virtual machine
 */
/* Priority of threads */
#define RVM_SFTD_PRIO                               (RVM_PREEMPT_PRIO_NUM-1U)
#define RVM_VMMD_PRIO                               (4U)
#define RVM_VVCT_PRIO                               (3U)
#define RVM_VUSR_PRIO                               (2U)
#define RVM_INIT_PRIO                               (1U)
#define RVM_WAIT_PRIO                               (0U)

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

/* Filter action */
#define RVM_FILTER_PASS                             (0)
#define RVM_FILTER_DROP                             (-1)

/* Size of bitmap */
#define RVM_VPRIO_BITMAP                            ((RVM_PREEMPT_VPRIO_NUM+RVM_WORD_BIT-1U)/RVM_WORD_BIT)
#define RVM_EVTCAP_BITMAP                           ((RVM_VIRT_EVT_NUM+RVM_WORD_BIT-1U)/RVM_WORD_BIT)

/* States of virtual machines */
#define RVM_VM_STATE(X)                             ((X)&0xFFU)
#define RVM_VM_FLAG(X)                              ((X)&~0xFFU)
#define RVM_VM_STATE_SET(X,S)                       ((X)=(RVM_VM_FLAG(X)|(S)))

/* The virtual machine is running */
#define RVM_VM_RUNNING                              (0U)
/* The virtual machine is temporarily suspended and is waiting for an interrupt */
#define RVM_VM_WAITING                              (1U)
/* The virtual machine have its interrupt enabled */
#define RVM_VM_INTENA                               RVM_POW2(RVM_WORD_BIT_D1)
/* The virtual machine have finished its booting */
#define RVM_VM_BOOTED                               (RVM_VM_INTENA<<1)
/* The virtual machine have its virtual vector mappings locked down */
#define RVM_VM_VCTLCK                               (RVM_VM_BOOTED<<1)
/* The watchdog for this virtual machine is enabled */
#define RVM_VM_WDGENA                               (RVM_VM_VCTLCK<<1)
    
/* The timer wheel size */
#define RVM_WHEEL_SIZE                              (32U)
#define RVM_DST2MAP(X)                              ((struct RVM_Map_Struct*)(((rvm_ptr_t)(X))-sizeof(struct RVM_List)))
#define RVM_DLY2VM(X)                               ((struct RVM_Virt_Struct*)(((rvm_ptr_t)(X))-sizeof(struct RVM_List)))

/* Virtual machine thread ID identification - TID is half word length */
#define RVM_VIRT_TID_FLAG                           RVM_POW2(RVM_WORD_BIT_D1-1U)
#define RVM_VIRT_TID(TID)                           ((rvm_tid_t)(((rvm_ptr_t)(TID))|RVM_VIRT_TID_FLAG))

/* Communication flag set selection */
#define RVM_FLAG_SET(B,S,N)                         ((volatile struct RVM_Flag*)((B)+((S)>>1)*(N)))

/* Header branch address extraction */
#define RVM_DESC_ENTRY(B, X)                        (((const struct RVM_Desc*)(B))->Entry[X])
#define RVM_DESC_MAGIC(B)                           (((const struct RVM_Desc*)(B))->Magic)
#define RVM_DESC_NUM(B)                             (((const struct RVM_Desc*)(B))->Number)
#define RVM_DESC_STUB(B)                            RVM_DESC_ENTRY(B,RVM_DESC_NUM(B)-1U)
/*****************************************************************************/
/* __RVM_MONITOR_DEF__ */
#endif
/* __HDR_DEF__ */
#endif
/* End Define ****************************************************************/

/* Struct ********************************************************************/
#ifdef __HDR_STRUCT__
#ifndef __RVM_MONITOR_STRUCT__
#define __RVM_MONITOR_STRUCT__
/* We used structs in the header */

/* Use defines in these headers */
#define __HDR_DEF__
#undef __HDR_DEF__
/*****************************************************************************/
/* Doubly-linked list */
struct RVM_List
{
    struct RVM_List* Next;
    struct RVM_List* Prev;
};

/* Main capability table creation struct */
struct RVM_Meta_Main_Struct
{
    rvm_cid_t Slot;
    rvm_ptr_t Size;
};

/* Capability table creation database entry */
struct RVM_Meta_Cpt_Crt_Struct
{
    rvm_cid_t Main;
    rvm_cid_t Slot;
    rvm_ptr_t Size;
};

#if(RVM_PGT_RAW_ENABLE==0U)
/* Page table creation database entry */
struct RVM_Meta_Pgt_Crt_Struct
{
    rvm_cid_t Main;
    rvm_cid_t Slot;
    rvm_ptr_t Base;
    rvm_ptr_t Is_Top;
    rvm_ptr_t Size_Order;
    rvm_ptr_t Num_Order;
};

/* Process creation database entry */
struct RVM_Meta_Prc_Crt_Struct
{
    rvm_cid_t Main;
    rvm_cid_t Slot;
    rvm_cid_t Cpt;
    rvm_cid_t Pgt;
};
#else
struct RVM_Meta_Prc_Crt_Struct
{
    rvm_cid_t Main;
    rvm_cid_t Slot;
    rvm_cid_t Cpt;
    rvm_ptr_t Raw;
};
#endif

/* Thread creation database entry */
struct RVM_Meta_Thd_Crt_Struct
{
    rvm_cid_t Main;
    rvm_cid_t Slot;
    rvm_cid_t Prc;
    rvm_ptr_t Prio;
    rvm_ptr_t Attr;
    rvm_ptr_t Is_Hyp;
};

/* Invocation creation database entry */
struct RVM_Meta_Inv_Crt_Struct
{
    rvm_cid_t Main;
    rvm_cid_t Slot;
    rvm_cid_t Prc;
};

/* Receive endpoint creation database entry */
struct RVM_Meta_Rcv_Crt_Struct
{
    rvm_cid_t Main;
    rvm_cid_t Slot;
};

/* VM capability table initialization database entry */
struct RVM_Meta_Vcap_Init_Struct
{
    rvm_cid_t Cpt;
    rvm_cid_t Vep_Main;
    rvm_cid_t Vep_Slot;
};

/* Capability table initialization database entry */
struct RVM_Meta_Cpt_Init_Struct
{
    rvm_cid_t Dst;
    rvm_cid_t Pos_Dst;
    rvm_cid_t Src;
    rvm_cid_t Pos_Src;
    rvm_ptr_t Flag;
};

/* Capability table kernel function initialization database entry */
struct RVM_Meta_Cpt_Kfn_Struct
{
    rvm_cid_t Dst;
    rvm_cid_t Pos_Dst;
    rvm_ptr_t Start;
    rvm_ptr_t End;
};

#if(RVM_PGT_RAW_ENABLE==0U)
/* Page table construct database entry */
struct RVM_Meta_Pgt_Con_Struct
{
    rvm_cid_t Parent;
    rvm_cid_t Child;
    rvm_ptr_t Position;
};

/* Page table mapping database entry */
struct RVM_Meta_Pgt_Add_Struct
{
    rvm_cid_t Pgt_Dst;
    rvm_ptr_t Pos_Dst;
    rvm_ptr_t Flag;
    rvm_ptr_t Pos_Src;
    rvm_ptr_t Index;
};
#endif

/* Thread initialization database entry */
struct RVM_Meta_Thd_Init_Struct
{
    rvm_cid_t Thd;
    rvm_ptr_t Flag;
    rvm_ptr_t Code_Base;
    rvm_ptr_t Desc_Slot;
    rvm_ptr_t Stack_Base;
    rvm_ptr_t Stack_Size;
    rvm_ptr_t Param;
    rvm_ptr_t Prio;
    rvm_ptr_t Reg_Base;
};

/* Invocation initialization database entry */
struct RVM_Meta_Inv_Init_Struct
{
    rvm_cid_t Inv;
    rvm_ptr_t Code_Base;
    rvm_ptr_t Desc_Slot;
    rvm_ptr_t Stack_Base;
    rvm_ptr_t Stack_Size;
};

#if(RVM_VIRT_NUM!=0U)
/* Parameter */
struct RVM_Param
{
    rvm_ptr_t Number;
    rvm_ptr_t Param[4];
};

/* Virtual interrupt flags */
struct RVM_Vctf
{
    rvm_ptr_t Tim;
    rvm_ptr_t Ctx;
    /* Actual number depends on configuration, 16 here for guaranteed
     * word alignment on even 128-bit hardware, which is unlikely. */
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

/* The stimulus source/destination mapping structure */
struct RVM_Map_Struct
{
    /* The head to insert into the source */
    struct RVM_List Src_Head;
    /* The head to insert into the receiver */
    struct RVM_List Dst_Head;
    /* The pointer to the virtual machine to send to */
    struct RVM_Virt_Struct* Virt;
    /* The virtual machine vector to send to */
    rvm_ptr_t Vct_Num;
};

/* Virtual machine scheduling data */
struct RVM_Sched
{
    /* The state of the virtual machine */
    rvm_ptr_t State;
    /* The current number of timeslices left for context switch */
    rvm_ptr_t Slice_Left;
    /* The current number of timeslices left until watchdog timeout */
    rvm_ptr_t Watchdog_Left;
    /* The timer interrupt period timeout time value */
    rvm_ptr_t Period_Timeout;
};

/* Fixed virtual machine mapping database entry */
struct RVM_Vmap_Struct
{
    /* Name of the VM */
    rvm_s8_t* Name;

    /* The priority for this virtual machine */
    rvm_ptr_t Prio;
    /* The timeslice for this virtual machine, in hypervisor ticks */
    rvm_ptr_t Slice;
    /* The timer interrupt period, in hypervisor ticks */
    rvm_ptr_t Period;
    /* The watchdog timeout time, in hypervisor ticks */
    rvm_ptr_t Watchdog;
    /* The number of virtual vectors */
    rvm_ptr_t Vct_Num;

    /* Register base - volatile, could be changed by the VM */
    volatile struct RVM_Thd_Reg* Reg_Base;
    /* Parameter base & size - volatile, could be changed by the VM */
    volatile struct RVM_State* State_Base;
    rvm_ptr_t State_Size;
    /* Descriptor header base address */
    rvm_ptr_t Desc_Base;
    /* Vector signal endpoint capability */
    rvm_cid_t Vct_Sig_Cap;
    
    /* Vector thread capability */
    rvm_cid_t Vct_Thd_Cap;
    /* Vector thread stack base */
    rvm_ptr_t Vct_Stack_Base;
    /* Vector thread size */
    rvm_ptr_t Vct_Stack_Size;
    
    /* User thread capability */
    rvm_cid_t Usr_Thd_Cap;
    /* User thread stack base */
    rvm_ptr_t Usr_Stack_Base;
    /* User thread size */
    rvm_ptr_t Usr_Stack_Size;
};

/* The virtual machine structure */
struct RVM_Virt_Struct
{
    /* The head to insert into the database */
    struct RVM_List Head;
    /* The head to go into the timer wheel */
    struct RVM_List Delay;
    /* The scheduling related information */
    struct RVM_Sched Sched;
    /* The vectors */
    struct RVM_List Vct_Head;
    /* ROM database */
    const struct RVM_Vmap_Struct* Map;
    /* Event source send capability table */
    rvm_ptr_t Evt_Cap[RVM_EVTCAP_BITMAP];
};
#endif

/* Interrupt flags between RME and RVM */
struct RVM_Flag
{
    /* Whether this group is locked */
    rvm_ptr_t Lock;
    /* Whether the fast interrupt group fired */
    rvm_ptr_t Fast;
    /* The slow interrupt group bitmap */
    rvm_ptr_t Group;
    rvm_ptr_t Flag[1024];
};

/* Process descriptor header */
struct RVM_Desc
{
    rvm_ptr_t Magic;
    rvm_ptr_t Number;
    rvm_ptr_t Entry[1024];
};

/* Thread register set */
struct RVM_Thd_Reg
{
    struct RVM_Reg_Struct Reg;
    struct RVM_Exc_Struct Exc;
    rvm_ptr_t Cop[1];
};
/*****************************************************************************/
/* __RVM_MONITOR_STRUCT__ */
#endif
/* __HDR_STRUCT__ */
#endif
/* End Struct ****************************************************************/

/* Private Variable **********************************************************/
#if(!(defined __HDR_DEF__||defined __HDR_STRUCT__))
#ifndef __RVM_MONITOR_MEMBER__
#define __RVM_MONITOR_MEMBER__

/* In this way we can use the data structures and definitions in the headers */
#define __HDR_DEF__

#undef __HDR_DEF__

#define __HDR_STRUCT__

#undef __HDR_STRUCT__

/* If the header is not used in the public mode */
#ifndef __HDR_PUBLIC__
/*****************************************************************************/
#ifdef RVM_COV_LINE_NUM
/* For monitor coverage use only */
static volatile rmp_ptr_t RVM_Cov[RVM_COV_WORD_NUM];
#endif

/* Virtual machine */
#if(RVM_VIRT_NUM!=0U)
RVM_EXTERN struct RVM_Virt_Struct RVM_Virt[];
RVM_EXTERN const struct RVM_Vmap_Struct RVM_Vmap[];
#endif
#if(RVM_BOOT_VEP_CRT_NUM!=0U)
RVM_EXTERN struct RVM_Meta_Main_Struct RVM_Meta_Vep_Main[];
RVM_EXTERN struct RVM_Meta_Rcv_Crt_Struct RVM_Meta_Vep_Crt[];
#endif
#if(RVM_BOOT_VCAP_INIT_NUM!=0U)
RVM_EXTERN struct RVM_Meta_Vcap_Init_Struct RVM_Meta_Vcap_Init[];
#endif

/* Captbl */
RVM_EXTERN struct RVM_Meta_Main_Struct RVM_Meta_Cpt_Main[];
RVM_EXTERN struct RVM_Meta_Cpt_Crt_Struct RVM_Meta_Cpt_Crt[];
#if(RVM_BOOT_INIT_CPT_INIT_NUM!=0U)
RVM_EXTERN struct RVM_Meta_Cpt_Init_Struct RVM_Meta_Cpt_Init[];
#endif
#if(RVM_BOOT_INIT_CPT_KFN_NUM!=0U)
RVM_EXTERN struct RVM_Meta_Cpt_Kfn_Struct RVM_Meta_Cpt_Kfn[];
#endif

#if(RVM_PGT_RAW_ENABLE==0U)
/* Page table */
RVM_EXTERN struct RVM_Meta_Main_Struct RVM_Meta_Pgt_Main[];
RVM_EXTERN struct RVM_Meta_Pgt_Crt_Struct RVM_Meta_Pgt_Crt[];
#if(RVM_BOOT_INIT_PGT_CON_NUM!=0U)
RVM_EXTERN struct RVM_Meta_Pgt_Con_Struct RVM_Meta_Pgt_Con[];
#endif
RVM_EXTERN struct RVM_Meta_Pgt_Add_Struct RVM_Meta_Pgt_Add[];
#endif

/* Process */
RVM_EXTERN struct RVM_Meta_Main_Struct RVM_Meta_Prc_Main[];
RVM_EXTERN struct RVM_Meta_Prc_Crt_Struct RVM_Meta_Prc_Crt[];

/* Thread */
RVM_EXTERN struct RVM_Meta_Main_Struct RVM_Meta_Thd_Main[];
RVM_EXTERN struct RVM_Meta_Thd_Crt_Struct RVM_Meta_Thd_Crt[];
RVM_EXTERN struct RVM_Meta_Thd_Init_Struct RVM_Meta_Thd_Init[];

/* Invocation */
#if(RVM_BOOT_INV_CRT_NUM!=0U)
RVM_EXTERN struct RVM_Meta_Main_Struct RVM_Meta_Inv_Main[];
RVM_EXTERN struct RVM_Meta_Inv_Crt_Struct RVM_Meta_Inv_Crt[];
#endif
#if(RVM_BOOT_INV_INIT_NUM!=0U)
RVM_EXTERN struct RVM_Meta_Inv_Init_Struct RVM_Meta_Inv_Init[];
#endif

/* Receive endpoint */
#if(RVM_BOOT_RCV_CRT_NUM!=0U)
RVM_EXTERN struct RVM_Meta_Main_Struct RVM_Meta_Rcv_Main[];
RVM_EXTERN struct RVM_Meta_Rcv_Crt_Struct RVM_Meta_Rcv_Crt[];
#endif

/* The capabilities */
static rvm_cid_t RVM_Sftd_Thd_Cap;
static rvm_cid_t RVM_Sftd_Sig_Cap;

#if(RVM_VIRT_NUM!=0U)
static rvm_cid_t RVM_Vmmd_Thd_Cap;
#endif

/* Everything below no longer volatile because we only have one thread VMMD for
 * all three daemons - HYPD, VCTD and TIMD. Their shared variables are free from
 * volatile now */
#if(RVM_VIRT_NUM!=0U)
/* Context switch trigger */
static rvm_ptr_t RVM_Switch;
/* Timestamp value */
static rvm_ptr_t RVM_Tick;
/* Timer wheel */
static struct RVM_List RVM_Wheel[RVM_WHEEL_SIZE];

/* The current virtual machine */
static struct RVM_Virt_Struct* RVM_Virt_Cur;
/* Virtual machine run queue and bitmap */
static rvm_ptr_t RVM_Bitmap[RVM_VPRIO_BITMAP];
static struct RVM_List RVM_Run[RVM_PREEMPT_VPRIO_NUM];

/* Event header */
static struct RVM_List RVM_Evt[RVM_VIRT_EVT_NUM];
/* Physical vector header */
static struct RVM_List RVM_Phys[RVM_PHYS_VCT_NUM];

/* Mapping database */
static struct RVM_List RVM_Map_Free;
static struct RVM_Map_Struct RVM_Map[RVM_VIRT_MAP_NUM];
#endif
/*****************************************************************************/
/* End Private Variable ******************************************************/

/* Private Function **********************************************************/ 
/*****************************************************************************/
/* Creation */
static rvm_ptr_t _RVM_Boot_Main_Crt(const struct RVM_Meta_Main_Struct* Meta,
                                    rvm_ptr_t Number,
                                    rvm_ptr_t Addr);
#if(RVM_BOOT_VEP_CRT_NUM!=0U)
static void _RVM_Boot_Vep_Crt(void);
#endif
static void _RVM_Boot_Cpt_Crt(void);
#if(RVM_PGT_RAW_ENABLE==0U)
static void _RVM_Boot_Pgt_Crt(void);
#endif
static void _RVM_Boot_Prc_Crt(void);
static void _RVM_Boot_Thd_Crt(void);
#if(RVM_BOOT_INV_CRT_NUM!=0U)
static void _RVM_Boot_Inv_Crt(void);
#endif
#if(RVM_BOOT_RCV_CRT_NUM!=0U)
static void _RVM_Boot_Rcv_Crt(void);
#endif
static void _RVM_Boot_Kobj_Crt(void);

/* Initialization */
#if(RVM_BOOT_VCAP_INIT_NUM!=0U)
static void _RVM_Boot_Vcap_Init(void);
#endif
#if(RVM_PGT_RAW_ENABLE==0U)
static void _RVM_Boot_Pgt_Init(void);
#endif
static void _RVM_Boot_Cpt_Init(void);
static void _RVM_Boot_Thd_Init(void);
#if(RVM_BOOT_INV_INIT_NUM!=0U)
static void _RVM_Boot_Inv_Init(void);
#endif
static void _RVM_Boot_Kobj_Init(void);

/* Boot hook */
RVM_EXTERN void RVM_Boot_Pre_Init(void);
RVM_EXTERN void RVM_Boot_Post_Init(void);
#if(RVM_VIRT_NUM!=0U)
static void _RVM_Virt_Crt(struct RVM_Virt_Struct* Virt,
                          const struct RVM_Vmap_Struct* Vmap,
                          rvm_ptr_t Virt_Num);
#endif
static void _RVM_Prc_Init(void);
static void _RVM_Daemon_Init(rvm_cid_t Cap_Base,
                             rvm_ptr_t Kom_Base);
/* VM management datastructure init */
#if(RVM_VIRT_NUM!=0U)
static void _RVM_Virt_Init(void);
#endif

/* Init daemon */
static void RVM_Init(void);

#if(RVM_VIRT_NUM!=0U)
/* Helper functions */
static void _RVM_Run_Ins(struct RVM_Virt_Struct* Virt);
static void _RVM_Run_Del(struct RVM_Virt_Struct* Virt);
static struct RVM_Virt_Struct* _RVM_Run_High(void);

static void _RVM_Virt_Switch(struct RVM_Virt_Struct* From, 
                             struct RVM_Virt_Struct* To);
static rvm_ret_t _RVM_Virt_Pend_Check(struct RVM_Virt_Struct* Virt);
static void _RVM_Virt_Vct_Snd(struct RVM_List* Array,
                              rvm_ptr_t Num);
static void _RVM_Virt_Recover(void);

static void _RVM_Wheel_Ins(struct RVM_Virt_Struct* Virt,
                           rvm_ptr_t Period);
static void _RVM_Tim_Snd(struct RVM_Virt_Struct* Virt);
static rvm_ptr_t _RVM_Flagset_Get(volatile struct RVM_Flag* Set);
static rvm_ptr_t _RVM_Vctset_Handle(volatile struct RVM_Flag* Set);
static void _RVM_Evtset_Handle(volatile struct RVM_Flag* Set);

/* Hypercalls */
#if(RVM_DBGLOG_ENABLE!=0U)
static rvm_ret_t _RVM_Hyp_Putchar(rvm_ptr_t Char);
#endif

static rvm_ret_t _RVM_Hyp_Int_Ena(void);
static rvm_ret_t _RVM_Hyp_Int_Dis(void);

static rvm_ret_t _RVM_Hyp_Vct_Phys(rvm_ptr_t Phys_Num,
                                   rvm_ptr_t Vct_Num);
static rvm_ret_t _RVM_Hyp_Vct_Evt(rvm_ptr_t Evt_Num,
                                  rvm_ptr_t Vct_Num);
static rvm_ret_t _RVM_Hyp_Vct_Del(rvm_ptr_t Vct_Num);
static rvm_ret_t _RVM_Hyp_Vct_Lck(void);
static rvm_ret_t _RVM_Hyp_Vct_Wait(void);

static rvm_ret_t _RVM_Hyp_Evt_Add(rvm_ptr_t Evt_Num);
static rvm_ret_t _RVM_Hyp_Evt_Del(rvm_ptr_t Evt_Num);
static rvm_ret_t _RVM_Hyp_Evt_Snd(rvm_ptr_t Evt_Num);

static rvm_ret_t _RVM_Hyp_Wdg_Clr(void);

/* Vector/event filter hooks - mandatory when there are VMs */
RVM_EXTERN rvm_ret_t RVM_Vct_Filter(rvm_ptr_t Vct_Num);
RVM_EXTERN rvm_ret_t RVM_Evt_Filter(rvm_ptr_t Evt_Num);

/* VM daemon - only present when there are VMs */
static void RVM_Vmmd(void);
#endif

/* Generic safety daemon - present regardless */
static void RVM_Sftd(void);
/*****************************************************************************/
#define __RVM_EXTERN__
/* End Private Function ******************************************************/

/* Public Variable ***********************************************************/
/* __HDR_PUBLIC__ */
#else
#define __RVM_EXTERN__ RVM_EXTERN 
/* __HDR_PUBLIC__ */
#endif

/*****************************************************************************/

/*****************************************************************************/

/* End Public Variable *******************************************************/

/* Public Function ***********************************************************/
/*****************************************************************************/
/* Debugging helper */
#if(RVM_DBGLOG_ENABLE!=0U)
__RVM_EXTERN__ rvm_ret_t RVM_Int_Print(rvm_ret_t Int);
__RVM_EXTERN__ rvm_ret_t RVM_Hex_Print(rvm_ptr_t Uint);
__RVM_EXTERN__ rvm_ret_t RVM_Str_Print(const rvm_s8_t* String);
#endif

/* Default logging */
#ifndef RVM_LOG
__RVM_EXTERN__ void RVM_Log(const char* File,
                            long Line,
                            const char* Date,
                            const char* Time);                        
#endif
                            
/* Coverage test - internal use */
#ifdef RVM_COV_LINE_NUM
__RVM_EXTERN__ void RVM_Cov_Print(void);
#endif

/* Bit manipulation */
__RVM_EXTERN__ rvm_ptr_t _RVM_MSB_Generic(rvm_ptr_t Value);
__RVM_EXTERN__ rvm_ptr_t _RVM_LSB_Generic(rvm_ptr_t Value);

/* Array helper */
__RVM_EXTERN__ void RVM_Clear(volatile void* Addr,
                              rvm_ptr_t Size);

/* Doubly linked list */
__RVM_EXTERN__ void RVM_List_Crt(struct RVM_List* Head);
__RVM_EXTERN__ void RVM_List_Del(struct RVM_List* Prev,
                                 struct RVM_List* Next);
__RVM_EXTERN__ void RVM_List_Ins(struct RVM_List* New,
                                 struct RVM_List* Prev,
                                 struct RVM_List* Next);
                             
/* Capability table operations */
__RVM_EXTERN__ rvm_ret_t RVM_Cpt_Crt(rvm_cid_t Cap_Cpt_Crt,
                                     rvm_cid_t Cap_Kom, 
                                     rvm_cid_t Cap_Cpt,
                                     rvm_ptr_t Raddr,
                                     rvm_ptr_t Entry_Num);
__RVM_EXTERN__ rvm_ret_t RVM_Cpt_Del(rvm_cid_t Cap_Cpt_Del,
                                     rvm_cid_t Cap_Del);
__RVM_EXTERN__ rvm_ret_t RVM_Cpt_Frz(rvm_cid_t Cap_Cpt_Frz,
                                     rvm_cid_t Cap_Frz);
__RVM_EXTERN__ rvm_ret_t RVM_Cpt_Add(rvm_cid_t Cap_Cpt_Dst,
                                     rvm_cid_t Cap_Dst, 
                                     rvm_cid_t Cap_Cpt_Src,
                                     rvm_cid_t Cap_Src,
                                     rvm_ptr_t Flag);
__RVM_EXTERN__ rvm_ret_t RVM_Cpt_Pgt(rvm_cid_t Cap_Cpt_Dst,
                                     rvm_cid_t Cap_Dst, 
                                     rvm_cid_t Cap_Cpt_Src,
                                     rvm_cid_t Cap_Src,
                                     rvm_ptr_t Begin,
                                     rvm_ptr_t End,
                                     rvm_ptr_t Flags);
__RVM_EXTERN__ rvm_ret_t RVM_Cpt_Kfn(rvm_cid_t Cap_Cpt_Dst,
                                     rvm_cid_t Cap_Dst, 
                                     rvm_cid_t Cap_Cpt_Src,
                                     rvm_cid_t Cap_Src,
                                     rvm_ptr_t Begin,
                                     rvm_ptr_t End);
__RVM_EXTERN__ rvm_ret_t RVM_Cpt_Kom(rvm_cid_t Cap_Cpt_Dst,
                                     rvm_cid_t Cap_Dst, 
                                     rvm_cid_t Cap_Cpt_Src,
                                     rvm_cid_t Cap_Src,
                                     rvm_ptr_t Begin,
                                     rvm_ptr_t End,
                                     rvm_ptr_t Flag);
__RVM_EXTERN__ rvm_ret_t RVM_Cpt_Rem(rvm_cid_t Cap_Cpt_Rem,
                                     rvm_cid_t Cap_Rem);
                                    
/* Kernel function operations */
__RVM_EXTERN__ rvm_ret_t RVM_Kfn_Act(rvm_cid_t Cap_Kfn,
                                     rvm_ptr_t Func_ID,
                                     rvm_ptr_t Sub_ID,
                                     rvm_ptr_t Param1,
                                     rvm_ptr_t Param2);

#if(RVM_PGT_RAW_ENABLE==0U)
/* Page table operations */
__RVM_EXTERN__ rvm_ret_t RVM_Pgt_Crt(rvm_cid_t Cap_Cpt,
                                     rvm_cid_t Cap_Kom,
                                     rvm_cid_t Cap_Pgt, 
                                     rvm_ptr_t Raddr,
                                     rvm_ptr_t Base_Addr,
                                     rvm_ptr_t Is_Top,
                                     rvm_ptr_t Size_Order,
                                     rvm_ptr_t Num_Order);
__RVM_EXTERN__ rvm_ret_t RVM_Pgt_Del(rvm_cid_t Cap_Cpt,
                                     rvm_cid_t Cap_Pgt);
__RVM_EXTERN__ rvm_ret_t RVM_Pgt_Add(rvm_cid_t Cap_Pgt_Dst,
                                     rvm_ptr_t Pos_Dst,
                                     rvm_ptr_t Flag_Dst,
                                     rvm_cid_t Cap_Pgt_Src,
                                     rvm_ptr_t Pos_Src,
                                     rvm_ptr_t Index);
__RVM_EXTERN__ rvm_ret_t RVM_Pgt_Rem(rvm_cid_t Cap_Pgt,
                                     rvm_ptr_t Pos);
__RVM_EXTERN__ rvm_ret_t RVM_Pgt_Con(rvm_cid_t Cap_Pgt_Parent,
                                     rvm_ptr_t Pos,
                                     rvm_cid_t Cap_Pgt_Child,
                                     rvm_ptr_t Flag_Child);
__RVM_EXTERN__ rvm_ret_t RVM_Pgt_Des(rvm_cid_t Cap_Pgt_Parent,
                                     rvm_ptr_t Pos,
                                     rvm_cid_t Cap_Pgt_Child);

/* Process operations */
__RVM_EXTERN__ rvm_ret_t RVM_Prc_Crt(rvm_cid_t Cap_Cpt_Crt,
                                     rvm_cid_t Cap_Prc,
                                     rvm_cid_t Cap_Cpt,
                                     rvm_cid_t Cap_Pgt);
#else
__RVM_EXTERN__ rvm_ret_t RVM_Prc_Crt(rvm_cid_t Cap_Cpt_Crt,
                                     rvm_cid_t Cap_Prc,
                                     rvm_cid_t Cap_Cpt,
                                     rvm_ptr_t Raw_Pgt);
#endif
__RVM_EXTERN__ rvm_ret_t RVM_Prc_Del(rvm_cid_t Cap_Cpt,
                                     rvm_cid_t Cap_Prc);
__RVM_EXTERN__ rvm_ret_t RVM_Prc_Cpt(rvm_cid_t Cap_Prc,
                                     rvm_cid_t Cap_Cpt);
#if(RVM_PGT_RAW_ENABLE==0U)
__RVM_EXTERN__ rvm_ret_t RVM_Prc_Pgt(rvm_cid_t Cap_Prc,
                                     rvm_cid_t Cap_Pgt);
#else
__RVM_EXTERN__ rvm_ret_t RVM_Prc_Pgt(rvm_cid_t Cap_Prc,
                                     rvm_ptr_t Raw_Pgt);
#endif
                                  
/* Thread operations */
__RVM_EXTERN__ rvm_ret_t RVM_Thd_Crt(rvm_cid_t Cap_Cpt,
                                     rvm_cid_t Cap_Kom,
                                     rvm_cid_t Cap_Thd,
                                     rvm_cid_t Cap_Prc,
                                     rvm_ptr_t Max_Prio,
                                     rvm_ptr_t Raddr,
                                     rvm_ptr_t Attr);
__RVM_EXTERN__ rvm_ret_t RVM_Hyp_Crt(rvm_cid_t Cap_Cpt,
                                     rvm_cid_t Cap_Kom,
                                     rvm_cid_t Cap_Thd,
                                     rvm_cid_t Cap_Prc,
                                     rvm_ptr_t Max_Prio,
                                     rvm_ptr_t Raddr,
                                     rvm_ptr_t Attr);
__RVM_EXTERN__ rvm_ret_t RVM_Thd_Del(rvm_cid_t Cap_Cpt,
                                     rvm_cid_t Cap_Thd);
__RVM_EXTERN__ rvm_ret_t RVM_Thd_Exec_Set(rvm_cid_t Cap_Thd,
                                          void* Entry,
                                          void* Stack,
                                          void* Param);
__RVM_EXTERN__ rvm_ret_t RVM_Thd_Sched_Bind(rvm_cid_t Cap_Thd,
                                            rvm_cid_t Cap_Thd_Sched,
                                            rvm_cid_t Cap_Sig,
                                            rvm_tid_t TID,
                                            rvm_ptr_t Prio);
__RVM_EXTERN__ rvm_ret_t RVM_Hyp_Sched_Bind(rvm_cid_t Cap_Thd,
                                            rvm_cid_t Cap_Thd_Sched,
                                            rvm_cid_t Cap_Sig,
                                            rvm_tid_t TID,
                                            rvm_ptr_t Prio,
                                            rvm_ptr_t Haddr);
__RVM_EXTERN__ rvm_ret_t RVM_Thd_Sched_Prio(rvm_cid_t Cap_Thd,
                                            rvm_ptr_t Prio);
__RVM_EXTERN__ rvm_ret_t RVM_Thd_Sched_Prio2(rvm_cid_t Cap_Thd0,
                                             rvm_ptr_t Prio0,
                                             rvm_cid_t Cap_Thd1,
                                             rvm_ptr_t Prio1);
__RVM_EXTERN__ rvm_ret_t RVM_Thd_Sched_Prio3(rvm_cid_t Cap_Thd0,
                                             rvm_ptr_t Prio0,
                                             rvm_cid_t Cap_Thd1,
                                             rvm_ptr_t Prio1,
                                             rvm_cid_t Cap_Thd2,
                                             rvm_ptr_t Prio2);
__RVM_EXTERN__ rvm_ret_t RVM_Thd_Sched_Free(rvm_cid_t Cap_Thd);
__RVM_EXTERN__ rvm_ret_t RVM_Thd_Sched_Rcv(rvm_cid_t Cap_Thd);
__RVM_EXTERN__ rvm_ret_t RVM_Thd_Time_Xfer(rvm_cid_t Cap_Thd_Dst,
                                           rvm_cid_t Cap_Thd_Src,
                                           rvm_ptr_t Time);
__RVM_EXTERN__ rvm_ret_t RVM_Thd_Swt(rvm_cid_t Cap_Thd,
                                     rvm_ptr_t Is_Yield);
                                 
/* Signal operations */
__RVM_EXTERN__ rvm_ret_t RVM_Sig_Crt(rvm_cid_t Cap_Cpt,
                                     rvm_cid_t Cap_Sig);
__RVM_EXTERN__ rvm_ret_t RVM_Sig_Del(rvm_cid_t Cap_Cpt,
                                     rvm_cid_t Cap_Sig);
__RVM_EXTERN__ rvm_ret_t RVM_Sig_Snd(rvm_cid_t Cap_Sig);
__RVM_EXTERN__ rvm_ret_t RVM_Sig_Rcv(rvm_cid_t Cap_Sig,
                                     rvm_ptr_t Option);
                                 
/* Invocation operations */
__RVM_EXTERN__ rvm_ret_t RVM_Inv_Crt(rvm_cid_t Cap_Cpt,
                                     rvm_cid_t Cap_Kom, 
                                     rvm_cid_t Cap_Inv,
                                     rvm_cid_t Cap_Prc,
                                     rvm_ptr_t Raddr);
__RVM_EXTERN__ rvm_ret_t RVM_Inv_Del(rvm_cid_t Cap_Cpt,
                                     rvm_cid_t Cap_Inv);
__RVM_EXTERN__ rvm_ret_t RVM_Inv_Set(rvm_cid_t Cap_Inv,
                                     rvm_ptr_t Entry,
                                     rvm_ptr_t Stack,
                                     rvm_ptr_t Is_Exc_Ret);
/*****************************************************************************/
/* Undefine "__RVM_EXTERN__" to avoid redefinition */
#undef __RVM_EXTERN__
/* __RVM_MONITOR_MEMBER__ */
#endif
/* !(defined __HDR_DEF__||defined __HDR_STRUCT__) */
#endif
/* End Public Function *******************************************************/

/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
