/******************************************************************************
Filename    : rvm_syslib.h
Author      : pry
Date        : 25/06/2017
Licence     : The Unlicense; see LICENSE for details.
Description : System user-level library header.
******************************************************************************/

/* Define ********************************************************************/
#ifdef __HDR_DEF__
#ifndef __RVM_SYSLIB_DEF__
#define __RVM_SYSLIB_DEF__
/*****************************************************************************/
/* Generic Macro *************************************************************/
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
/* End Generic Macro *********************************************************/

/* Debug Macro ***************************************************************/
/* Maximum logging length */
#define RVM_DBGLOG_MAX                              (255U)

#if(RVM_DBGLOG_ENABLE!=0U)
/* Debug prints */
#define RVM_DBG_I(INT)                              RVM_Int_Print((rvm_cnt_t)(INT))
#define RVM_DBG_H(HEX)                              RVM_Hex_Print((rvm_ptr_t)(HEX))
#define RVM_DBG_S(STR)                              RVM_Str_Print((const rvm_s8_t*)(STR))
#else
#define RVM_DBG_I(INT)                              while(0)
#define RVM_DBG_H(HEX)                              while(0)
#define RVM_DBG_S(STR)                              while(0)
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

/* Reboot macro */
#ifndef RVM_VIRT_LIB_ENABLE
/* Monitor */
#define RVM_REBOOT_OP()                             RVM_Kfn_Act(RVM_BOOT_INIT_KFN,RVM_KFN_SYS_REBOOT,0U,0U,0U)
#else
#if(RVM_VIRT_LIB_ENABLE==0U)
/* Native process */
#define RVM_REBOOT_OP()                             ((void(*)(void))RVM_NULL)()
#else
/* Virtual machine */
#define RVM_REBOOT_OP()                             RVM_Hyp_Reboot()
#endif
#endif

/* Assert macro */
#if(RVM_ASSERT_ENABLE!=0U)
#define RVM_ASSERT(X) \
do \
{ \
    if(!(X)) \
    { \
        RVM_LOG_OP(__FILE__,__LINE__,__DATE__,__TIME__); \
        RVM_REBOOT_OP(); \
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
/* End Debug Macro ***********************************************************/

/* System Call Helper ********************************************************/
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
/* End System Call Helper ****************************************************/

/* System Call Error *********************************************************/
/* Capability table **********************************************************/
#define RVM_ERR_CPT                                 (0)
/* The capability is empty */
#define RVM_ERR_CPT_NULL                            ((-1)+RVM_ERR_CPT)
/* The capability type is wrong */
#define RVM_ERR_CPT_TYPE                            ((-2)+RVM_ERR_CPT)
/* The range of the capability is wrong */
#define RVM_ERR_CPT_RANGE                           ((-3)+RVM_ERR_CPT)
/* The kernel object table operation of the capability is wrong */
#define RVM_ERR_CPT_KOTBL                           ((-4)+RVM_ERR_CPT)
/* The capability number already exists, or not exist, depending on the function */
#define RVM_ERR_CPT_EXIST                           ((-5)+RVM_ERR_CPT)
/* When freezing capabilities, the refcnt is not zero, or when delegating, refcnt overflowed */
#define RVM_ERR_CPT_REFCNT                          ((-6)+RVM_ERR_CPT)
/* The flags of the fine-grain-controlled capability is not correct */
#define RVM_ERR_CPT_FLAG                            ((-7)+RVM_ERR_CPT)
/* Something involved is not quiescent */
#define RVM_ERR_CPT_QUIE                            ((-8)+RVM_ERR_CPT)
/* Something involved is frozen */
#define RVM_ERR_CPT_FROZEN                          ((-9)+RVM_ERR_CPT)
/* The capability is a root one and therefore cannot use removal */
#define RVM_ERR_CPT_ROOT                            ((-10)+RVM_ERR_CPT)

/* Page table ****************************************************************/
#define RVM_ERR_PGT                                 (-100)
/* Incorrect address */
#define RVM_ERR_PGT_ADDR                            ((-1)+RVM_ERR_PGT)
/* Mapping failure */
#define RVM_ERR_PGT_MAP                             ((-2)+RVM_ERR_PGT)
/* Hardware restrictions */
#define RVM_ERR_PGT_HW                              ((-3)+RVM_ERR_PGT)
/* Wrong permissions */
#define RVM_ERR_PGT_PERM                            ((-4)+RVM_ERR_PGT)

/* Process/thread ************************************************************/
#define RVM_ERR_PTH                                 (-200)
/* Incorrect address */
#define RVM_ERR_PTH_PGT                             ((-1)+RVM_ERR_PTH)
/* Conflicting operations happening at the same time */
#define RVM_ERR_PTH_CONFLICT                        ((-2)+RVM_ERR_PTH)
/* Thread ID error */
#define RVM_ERR_PTH_TID                             ((-3)+RVM_ERR_PTH)
/* Thread state is invalid or we have failed the CAS */
#define RVM_ERR_PTH_INVSTATE                        ((-4)+RVM_ERR_PTH)
/* The thread priority is not correct */
#define RVM_ERR_PTH_PRIO                            ((-5)+RVM_ERR_PTH)
/* The reference count not correct */
#define RVM_ERR_PTH_REFCNT                          ((-6)+RVM_ERR_PTH)
/* There are no notifications sent to this thread */
#define RVM_ERR_PTH_NOTIF                           ((-7)+RVM_ERR_PTH)
/* The time is full for this thread */
#define RVM_ERR_PTH_OVERFLOW                        ((-8)+RVM_ERR_PTH)
/* Cannot conduct operation because target have a fault */
#define RVM_ERR_PTH_FAULT                           ((-9)+RVM_ERR_PTH)

/* Signal/invocation *********************************************************/
#define RVM_ERR_SIV                                 (-300)
/* This invocation capability/signal endpoint is already active, or wrong option on receive */
#define RVM_ERR_SIV_ACT                             ((-1)+RVM_ERR_SIV)
/* This invocation succeeded, but a fault happened in the thread, and we forced a invocation return */
#define RVM_ERR_SIV_FAULT                           ((-2)+RVM_ERR_SIV)
/* The thread's signal counter is full */
#define RVM_ERR_SIV_FULL                            ((-3)+RVM_ERR_SIV)
/* The thread's invocation stack is empty, cannot return */
#define RVM_ERR_SIV_EMPTY                           ((-4)+RVM_ERR_SIV)
/* The signal capability is operated on by two threads simutaneously */
#define RVM_ERR_SIV_CONFLICT                        ((-5)+RVM_ERR_SIV)
/* The signal receive system call ended with a result of free(forced unblock) */
#define RVM_ERR_SIV_FREE                            ((-6)+RVM_ERR_SIV)
/* The signal receive failed because we are the boot-time thread */
#define RVM_ERR_SIV_BOOT                            ((-7)+RVM_ERR_SIV)
/* End System Call Error *****************************************************/

/* System Call Flag **********************************************************/
/* Capability table **********************************************************/
/* This cap to captbl allows creating kernel objects in the captbl */
#define RVM_CPT_FLAG_CRT                            RVM_POW2(0U)
/* This cap to captbl allows deleting kernel objects in the captbl */
#define RVM_CPT_FLAG_DEL                            RVM_POW2(1U)
/* This cap to captbl allows freezing kernel objects in the captbl */
#define RVM_CPT_FLAG_FRZ                            RVM_POW2(2U)
/* This cap to captbl allows delegating kernel objects in it */
#define RVM_CPT_FLAG_ADD_SRC                        RVM_POW2(3U)
/* This cap to captbl allows receiving delegated kernel objects to it */
#define RVM_CPT_FLAG_ADD_DST                        RVM_POW2(4U)  
/* This cap to captbl allows removal operation in kernel objects(captbls) in the captbl */
#define RVM_CPT_FLAG_REM                            RVM_POW2(5U)
/* This cap to captbl allows itself to be used in process creation */
#define RVM_CPT_FLAG_PRC_CRT                        RVM_POW2(6U)
/* This cap to captbl allows itself to be used in process capability table replacement */
#define RVM_CPT_FLAG_PRC_CPT                        RVM_POW2(7U)
/* This cap to captbl allows all operations */
#define RVM_CPT_FLAG_ALL                            (RVM_CPT_FLAG_CRT|RVM_CPT_FLAG_DEL|RVM_CPT_FLAG_FRZ| \
                                                     RVM_CPT_FLAG_ADD_SRC|RVM_CPT_FLAG_ADD_DST|RVM_CPT_FLAG_REM| \
                                                     RVM_CPT_FLAG_PRC_CRT|RVM_CPT_FLAG_PRC_CPT)

/* Page table ****************************************************************/
/* This cap to pgtbl allows delegating pages in it */
#define RVM_PGT_FLAG_ADD_SRC                        RVM_POW2(0U)
/* This cap to pgtbl allows receiving delegated pages to it */
#define RVM_PGT_FLAG_ADD_DST                        RVM_POW2(1U)
/* This cap to pgtbl allows removal of pages in it */
#define RVM_PGT_FLAG_REM                            RVM_POW2(2U) 
/* This cap to pgtbl allows to be mapped into other page tables as a child
 * or destructed from other page tables as a child */
#define RVM_PGT_FLAG_CHILD                          RVM_POW2(3U)
/* This cap to pgtbl allows accepting lower page table mappings */
#define RVM_PGT_FLAG_CON_PARENT                     RVM_POW2(4U)
/* This cap to pgtbl allows its lower level page table mappings to be destructed */
#define RVM_PGT_FLAG_DES_PARENT                     RVM_POW2(5U)
/* This cap to pgtbl allows itself to be used in process creation */
#define RVM_PGT_FLAG_PRC_CRT                        RVM_POW2(6U)
/* This cap to pgtbl allows itself to be used in process page table replacement */
#define RVM_PGT_FLAG_PRC_PGT                        RVM_POW2(7U)
/* This cap to pgtbl allows all operations */
#define RVM_PGT_FLAG_ALL                            (RVM_PGT_FLAG_ADD_SRC|RVM_PGT_FLAG_ADD_DST|RVM_PGT_FLAG_REM| \
                                                     RVM_PGT_FLAG_CHILD|RVM_PGT_FLAG_CON_PARENT|RVM_PGT_FLAG_DES_PARENT| \
                                                     RVM_PGT_FLAG_PRC_CRT|RVM_PGT_FLAG_PRC_PGT)

/* Kernel memory *************************************************************/
/* This cap to kernel memory allows creation of captbl */
#define RVM_KOM_FLAG_CPT                            RVM_POW2(0U)
/* This cap to kernel memory allows creation of pgtbl */
#define RVM_KOM_FLAG_PGT                            RVM_POW2(1U)
/* This cap to kernel memory allows creation of thread */
#define RVM_KOM_FLAG_THD                            RVM_POW2(2U)
/* This cap to kernel memory allows creation of invocation */
#define RVM_KOM_FLAG_INV                            RVM_POW2(3U)
/* This cap to kernel memory allows all operations */
#define RVM_KOM_FLAG_ALL                            (RVM_KOM_FLAG_CPT|RVM_KOM_FLAG_PGT| \
                                                     RVM_KOM_FLAG_THD|RVM_KOM_FLAG_INV)

/* Process *******************************************************************/
/* This cap to process allows creating invocation stubs in it */
#define RVM_PRC_FLAG_INV                            RVM_POW2(0U)
/* This cap to process allows creating threads in it */
#define RVM_PRC_FLAG_THD                            RVM_POW2(1U)
/* This cap to process allows changing its capability table */
#define RVM_PRC_FLAG_CPT                            RVM_POW2(2U)
/* This cap to process allows changing its page table */
#define RVM_PRC_FLAG_PGT                            RVM_POW2(3U)
/* This cap to process allows all operations */
#define RVM_PRC_FLAG_ALL                            (RVM_PRC_FLAG_INV|RVM_PRC_FLAG_THD| \
                                                     RVM_PRC_FLAG_CPT|RVM_PRC_FLAG_PGT)

/* Thread ********************************************************************/
/* This cap to thread allows setting its execution parameters */
#define RVM_THD_FLAG_EXEC_SET                       RVM_POW2(0U)
/* This cap to thread allows setting its scheduling parameters */
#define RVM_THD_FLAG_SCHED_CHILD                    RVM_POW2(1U)
/* This cap to thread allows registering it as a scheduler */
#define RVM_THD_FLAG_SCHED_PARENT                   RVM_POW2(2U)
/* This cap to thread allows changing its priority level */
#define RVM_THD_FLAG_SCHED_PRIO                     RVM_POW2(3U)
/* This cap to thread allows freeing the thread from the core */
#define RVM_THD_FLAG_SCHED_FREE                     RVM_POW2(4U)
/* This cap to thread allows getting scheduling notifications */
#define RVM_THD_FLAG_SCHED_RCV                      RVM_POW2(5U)
/* This cap to thread allows acting as a source for time transfer */
#define RVM_THD_FLAG_XFER_SRC                       RVM_POW2(6U)
/* This cap to thread allows acting as a destination for time transfer */
#define RVM_THD_FLAG_XFER_DST                       RVM_POW2(7U)
/* This cap to thread allows switching to it */
#define RVM_THD_FLAG_SWT                            RVM_POW2(8U)
/* This cap to thread allows all operations */
#define RVM_THD_FLAG_ALL                            (RVM_THD_FLAG_EXEC_SET|RVM_THD_FLAG_SCHED_CHILD| \
                                                     RVM_THD_FLAG_SCHED_PARENT|RVM_THD_FLAG_SCHED_PRIO| \
                                                     RVM_THD_FLAG_SCHED_FREE|RVM_THD_FLAG_SCHED_RCV| \
                                                     RVM_THD_FLAG_XFER_SRC|RVM_THD_FLAG_XFER_DST|RVM_THD_FLAG_SWT)
                                                     
/* Signal *********************************************************************/
/* This cap to signal endpoint allows sending to it */
#define RVM_SIG_FLAG_SND                            RVM_POW2(0U)
/* This cap to signal endpoint allows blocking single receive on it */
#define RVM_SIG_FLAG_RCV_BS                         RVM_POW2(1U)
/* This cap to signal endpoint allows blocking multi receive on it */
#define RVM_SIG_FLAG_RCV_BM                         RVM_POW2(2U)
/* This cap to signal endpoint allows non-blocking single receive on it */
#define RVM_SIG_FLAG_RCV_NS                         RVM_POW2(3U)
/* This cap to signal endpoint allows non-blocking multi receive on it */
#define RVM_SIG_FLAG_RCV_NM                         RVM_POW2(4U)
/* This cap to signal endpoint allows all receiving operations */
#define RVM_SIG_FLAG_RCV                            (RVM_SIG_FLAG_RCV_BS|RVM_SIG_FLAG_RCV_BM| \
                                                     RVM_SIG_FLAG_RCV_NS|RVM_SIG_FLAG_RCV_NM)
/* This cap to signal endpoint allows sending scheduler notification to it */
#define RVM_SIG_FLAG_SCHED                          RVM_POW2(5U)
/* This cap to signal endpoint allows all operations */
#define RVM_SIG_FLAG_ALL                            (RVM_SIG_FLAG_SND|RVM_SIG_FLAG_RCV|RVM_SIG_FLAG_SCHED)

/* Invocation ****************************************************************/
/* This cap to invocation allows setting parameters for it */
#define RVM_INV_FLAG_SET                            RVM_POW2(0U)
/* This cap to invocation allows activating it */
#define RVM_INV_FLAG_ACT                            RVM_POW2(1U)
/* The return operation does not need a flag, nor does it need a capability */
/* This cap to invocation allows all operations */
#define RVM_INV_FLAG_ALL                            (RVM_INV_FLAG_SET|RVM_INV_FLAG_ACT)
/* End Operation Flag ********************************************************/

/* Page Table Special Flag ***************************************************/
/* Generic *******************************************************************/
/* This page allows to be read */
#define RVM_PGT_READ                                RVM_POW2(0U)
/* This page allows to be written */
#define RVM_PGT_WRITE                               RVM_POW2(1U)
/* This page allows execution */
#define RVM_PGT_EXECUTE                             RVM_POW2(2U)
/* This page is cacheable */
#define RVM_PGT_CACHE                               RVM_POW2(3U)
/* This page is bufferable (write-back can be used instead of write-through) */
#define RVM_PGT_BUFFER                              RVM_POW2(4U)
/* This page is pinned in TLB */
#define RVM_PGT_STATIC                              RVM_POW2(5U)
/* All the permissions are set */
#define RVM_PGT_ALL_PERM                            (RVM_PGT_READ|RVM_PGT_WRITE|RVM_PGT_EXECUTE| \
                                                     RVM_PGT_CACHE|RVM_PGT_BUFFER|RVM_PGT_STATIC)

/* Size order ****************************************************************/
#define RVM_PGT_SIZE_2B                             (1U)
#define RVM_PGT_SIZE_4B                             (2U)
#define RVM_PGT_SIZE_8B                             (3U)
#define RVM_PGT_SIZE_16B                            (4U)
#define RVM_PGT_SIZE_32B                            (5U)
#define RVM_PGT_SIZE_64B                            (6U)
#define RVM_PGT_SIZE_128B                           (7U)
#define RVM_PGT_SIZE_256B                           (8U)
#define RVM_PGT_SIZE_512B                           (9U)
#define RVM_PGT_SIZE_1K                             (10U)
#define RVM_PGT_SIZE_2K                             (11U)
#define RVM_PGT_SIZE_4K                             (12U)
#define RVM_PGT_SIZE_8K                             (13U)
#define RVM_PGT_SIZE_16K                            (14U)
#define RVM_PGT_SIZE_32K                            (15U)
#define RVM_PGT_SIZE_64K                            (16U)
#define RVM_PGT_SIZE_128K                           (17U)
#define RVM_PGT_SIZE_256K                           (18U)
#define RVM_PGT_SIZE_512K                           (19U)
#define RVM_PGT_SIZE_1M                             (20U)
#define RVM_PGT_SIZE_2M                             (21U)
#define RVM_PGT_SIZE_4M                             (22U)
#define RVM_PGT_SIZE_8M                             (23U)
#define RVM_PGT_SIZE_16M                            (24U)
#define RVM_PGT_SIZE_32M                            (25U)
#define RVM_PGT_SIZE_64M                            (26U)
#define RVM_PGT_SIZE_128M                           (27U)
#define RVM_PGT_SIZE_256M                           (28U)
#define RVM_PGT_SIZE_512M                           (29U)
#define RVM_PGT_SIZE_1G                             (30U)
#define RVM_PGT_SIZE_2G                             (31U)
#define RVM_PGT_SIZE_4G                             (32U)
#define RVM_PGT_SIZE_8G                             (33U)
#define RVM_PGT_SIZE_16G                            (34U)
#define RVM_PGT_SIZE_32G                            (35U)
#define RVM_PGT_SIZE_64G                            (36U)
#define RVM_PGT_SIZE_128G                           (37U)
#define RVM_PGT_SIZE_256G                           (38U)
#define RVM_PGT_SIZE_512G                           (39U)
#define RVM_PGT_SIZE_1T                             (40U)
#define RVM_PGT_SIZE_2T                             (41U)
#define RVM_PGT_SIZE_4T                             (42U)
#define RVM_PGT_SIZE_8T                             (43U)
#define RVM_PGT_SIZE_16T                            (44U)
#define RVM_PGT_SIZE_32T                            (45U)
#define RVM_PGT_SIZE_64T                            (46U)
#define RVM_PGT_SIZE_128T                           (47U)
#define RVM_PGT_SIZE_256T                           (48U)
#define RVM_PGT_SIZE_512T                           (49U)
#define RVM_PGT_SIZE_1P                             (50U)
#define RVM_PGT_SIZE_2P                             (51U)
#define RVM_PGT_SIZE_4P                             (52U)
#define RVM_PGT_SIZE_8P                             (53U)
#define RVM_PGT_SIZE_16P                            (54U)
#define RVM_PGT_SIZE_32P                            (55U)
#define RVM_PGT_SIZE_64P                            (56U)
#define RVM_PGT_SIZE_128P                           (57U)
#define RVM_PGT_SIZE_256P                           (58U)
#define RVM_PGT_SIZE_512P                           (59U)
#define RVM_PGT_SIZE_1E                             (60U)
#define RVM_PGT_SIZE_2E                             (61U)
#define RVM_PGT_SIZE_4E                             (62U)
#define RVM_PGT_SIZE_8E                             (63U)
#define RVM_PGT_SIZE_16E                            (64U)
#define RVM_PGT_SIZE_32E                            (65U)
#define RVM_PGT_SIZE_64E                            (66U)
#define RVM_PGT_SIZE_128E                           (67U)
#define RVM_PGT_SIZE_256E                           (68U)
#define RVM_PGT_SIZE_512E                           (69U)
#define RVM_PGT_SIZE_1Z                             (70U)

/* Entry number order ********************************************************/
#define RVM_PGT_NUM_1                               (0U)
#define RVM_PGT_NUM_2                               (1U)
#define RVM_PGT_NUM_4                               (2U)
#define RVM_PGT_NUM_8                               (3U)
#define RVM_PGT_NUM_16                              (4U)
#define RVM_PGT_NUM_32                              (5U)
#define RVM_PGT_NUM_64                              (6U)
#define RVM_PGT_NUM_128                             (7U)
#define RVM_PGT_NUM_256                             (8U)
#define RVM_PGT_NUM_512                             (9U)
#define RVM_PGT_NUM_1K                              (10U)
#define RVM_PGT_NUM_2K                              (11U)
#define RVM_PGT_NUM_4K                              (12U)
#define RVM_PGT_NUM_8K                              (13U)
#define RVM_PGT_NUM_16K                             (14U)
#define RVM_PGT_NUM_32K                             (15U)
#define RVM_PGT_NUM_64K                             (16U)
#define RVM_PGT_NUM_128K                            (17U)
#define RVM_PGT_NUM_256K                            (18U)
#define RVM_PGT_NUM_512K                            (19U)
#define RVM_PGT_NUM_1M                              (20U)
#define RVM_PGT_NUM_2M                              (21U)
#define RVM_PGT_NUM_4M                              (22U)
/* End Page Table Special Flag ***********************************************/

/* Receive Special Flag ******************************************************/
#define RVM_RCV_BS                                  (0U)
#define RVM_RCV_BM                                  (1U)
#define RVM_RCV_NS                                  (2U)
#define RVM_RCV_NM                                  (3U)
/* End Receive Special Flag **************************************************/

/* System Call Number ********************************************************/
/* IPC activation ************************************************************/
/* Return from an invocation */
#define RVM_SVC_INV_RET                             (0U)
/* Activate the invocation */
#define RVM_SVC_INV_ACT                             (1U)
/* Send to a signal endpoint */
#define RVM_SVC_SIG_SND                             (2U)
/* Receive from a signal endpoint */
#define RVM_SVC_SIG_RCV                             (3U)
/* Kernel function ***********************************************************/
#define RVM_SVC_KFN                                 (4U)
/* System calls that may cause a context switch ******************************/
/* Free a thread from some core */
#define RVM_SVC_THD_SCHED_FREE                      (5U)
/* Set entry&stack */
#define RVM_SVC_THD_EXEC_SET                        (6U)
/* Changing thread priority */
#define RVM_SVC_THD_SCHED_PRIO                      (7U)
/* Transfer time to a thread */
#define RVM_SVC_THD_TIME_XFER                       (8U)
/* Switch to another thread */
#define RVM_SVC_THD_SWT                             (9U)
/* Capability table **********************************************************/
/* Create */
#define RVM_SVC_CPT_CRT                             (10U)
/* Delete */
#define RVM_SVC_CPT_DEL                             (11U)
/* Freeze */
#define RVM_SVC_CPT_FRZ                             (12U)
/* Add */
#define RVM_SVC_CPT_ADD                             (13U)
/* Remove */
#define RVM_SVC_CPT_REM                             (14U)
/* Page table ****************************************************************/
/* Create */
#define RVM_SVC_PGT_CRT                             (15U)
/* Delete */
#define RVM_SVC_PGT_DEL                             (16U)
/* Add */
#define RVM_SVC_PGT_ADD                             (17U)
/* Remove */
#define RVM_SVC_PGT_REM                             (18U)
/* Construction */
#define RVM_SVC_PGT_CON                             (19U)
/* Destruction */
#define RVM_SVC_PGT_DES                             (20U)
/* Process *******************************************************************/
/* Create */
#define RVM_SVC_PRC_CRT                             (21U)
/* Delete */
#define RVM_SVC_PRC_DEL                             (22U)
/* Change captbl */
#define RVM_SVC_PRC_CPT                             (23U)
/* Change pgtbl */ 
#define RVM_SVC_PRC_PGT                             (24U)
/* Thread ********************************************************************/
/* Create */
#define RVM_SVC_THD_CRT                             (25U)
/* Delete */
#define RVM_SVC_THD_DEL                             (26U)
/* Bind to the current processor */
#define RVM_SVC_THD_SCHED_BIND                      (27U)
/* Try to receive scheduling notifications */
#define RVM_SVC_THD_SCHED_RCV                       (28U)
/* Signal ********************************************************************/
/* Create */
#define RVM_SVC_SIG_CRT                             (29U)
/* Delete */
#define RVM_SVC_SIG_DEL                             (30U)
/* Invocation ****************************************************************/
/* Create */
#define RVM_SVC_INV_CRT                             (31U)
/* Delete */
#define RVM_SVC_INV_DEL                             (32U)
/* Set entry&stack */
#define RVM_SVC_INV_SET                             (33U)
/* End System Call Number ****************************************************/

/* Kernel Function Number ****************************************************/
/* Page table ****************************************************************/
/* Clear the whole TLB */
#define RVM_KFN_PGT_CACHE_CLR                       (0xF000U)
/* Clear a single TLB line */
#define RVM_KFN_PGT_LINE_CLR                        (0xF001U)
/* Set the ASID of a page table */
#define RVM_KFN_PGT_ASID_SET                        (0xF002U)
/* Lock a page into the TLB */
#define RVM_KFN_PGT_TLB_LOCK                        (0xF003U)
/* Query or modify the content of an entry */
#define RVM_KFN_PGT_ENTRY_MOD                       (0xF004U)
/* Interrupt controller ******************************************************/
/* Modify local interrupt controller */
#define RVM_KFN_INT_LOCAL_MOD                       (0xF100U)
/* Modify global interrupt controller */
#define RVM_KFN_INT_GLOBAL_MOD                      (0xF101U)
/* Trigger a local interrupt */
#define RVM_KFN_INT_LOCAL_TRIG                      (0xF102U)
/* Trigger a local event */
#define RVM_KFN_EVT_LOCAL_TRIG                      (0xF103U)
/* Cache maintenance *********************************************************/
/* Modify cache state */
#define RVM_KFN_CACHE_MOD                           (0xF200U)
/* Configure cache */
#define RVM_KFN_CACHE_CONFIG                        (0xF201U)
/* Invalidate cache */
#define RVM_KFN_CACHE_MAINT                         (0xF202U)
/* Lock cache */
#define RVM_KFN_CACHE_LOCK                          (0xF203U)
/* Modify prefetcher state */
#define RVM_KFN_PRFTH_MOD                           (0xF204U)
/* Hot plug ******************************************************************/
/* Modify physical CPU configuration */
#define RVM_KFN_HPNP_PCPU_MOD                       (0xF300U)
/* Modify logical CPU configuration */
#define RVM_KFN_HPNP_LCPU_MOD                       (0xF301U)
/* Modify physical memory configuration */
#define RVM_KFN_HPNP_PMEM_MOD                       (0xF302U)
/* Power and frequency adjustmentment ****************************************/
/* Put CPU into idle sleep mode */
#define RVM_KFN_IDLE_SLEEP                          (0xF400U)
/* Reboot the whole system */
#define RVM_KFN_SYS_REBOOT                          (0xF401U)
/* Shutdown the whole system */
#define RVM_KFN_SYS_SHUTDOWN                        (0xF402U)
/* Modify voltage configuration */
#define RVM_KFN_VOLT_MOD                            (0xF403U)
/* Modify frequency configuration */
#define RVM_KFN_FREQ_MOD                            (0xF404U)
/* Modify power state */
#define RVM_KFN_PMOD_MOD                            (0xF405U)
/* Modify safety lock state */
#define RVM_KFN_SAFETY_MOD                          (0xF406U)
/* Performance monitoring ****************************************************/
/* Query or modify CPU function configuration */
#define RVM_KFN_PERF_CPU_FUNC                       (0xF500U)
/* Query or modify performance monitor configuration */
#define RVM_KFN_PERF_MON_MOD                        (0xF501U)
/* Query or modify counting performance monitor register */
#define RVM_KFN_PERF_CNT_MOD                        (0xF502U)
/* Query or modify clock cycle performance monitor register */
#define RVM_KFN_PERF_CYCLE_MOD                      (0xF503U)
/* Query or modify data performance monitor register */
#define RVM_KFN_PERF_DATA_MOD                       (0xF504U)
/* Query or modify physical monitor register */
#define RVM_KFN_PERF_PHYS_MOD                       (0xF505U)
/* Query or modify cumulative monitor register */
#define RVM_KFN_PERF_CUMUL_MOD                      (0xF506U)
/* Hardware-assisted virtualization ******************************************/
/* Create a virtual machine */
#define RVM_KFN_VM_CRT                              (0xF600U)
/* Delete a virtual machine */
#define RVM_KFN_VM_DEL                              (0xF601U)
/* Assign a user-level page table to the virtual machine */
#define RVM_KFN_VM_PGT                              (0xF602U)
/* Query or modify virtual machine state */
#define RVM_KFN_VM_MOD                              (0xF603U)
/* Create a virtual CPU */
#define RVM_KFN_VCPU_CRT                            (0xF604U)
/* Bind a virtual CPU to a virtual machine */
#define RVM_KFN_VCPU_BIND                           (0xF605U)
/* Free a virtual CPU from a virtual machine */
#define RVM_KFN_VCPU_FREE                           (0xF606U)
/* Delete a virtual CPU */
#define RVM_KFN_VCPU_DEL                            (0xF607U)
/* Query or modify virtual registers */
#define RVM_KFN_VCPU_MOD                            (0xF608U)
/* Run the VCPU on this thread */
#define RVM_KFN_VCPU_RUN                            (0xF609U)
/* Security monitor **********************************************************/
/* Create an enclave */
#define RVM_KFN_ECLV_CRT                            (0xF700U)
/* Query or modify an enclave */
#define RVM_KFN_ECLV_MOD                            (0xF701U)
/* Delete an enclave */
#define RVM_KFN_ECLV_DEL                            (0xF702U)
/* Call into an enclave */
#define RVM_KFN_ECLV_ACT                            (0xF703U)
/* Return from an enclave */
#define RVM_KFN_ECLV_RET                            (0xF704U)
/* Debugging *****************************************************************/
/* Debug printing - a single character or a series of characters */
#define RVM_KFN_DEBUG_PRINT                         (0xF800U)
/* Modify thread register content */
#define RVM_KFN_DEBUG_REG_MOD                       (0xF801U)
/* Modify thread invocation register content */
#define RVM_KFN_DEBUG_INV_MOD                       (0xF802U)
/* Get thread error register content */
#define RVM_KFN_DEBUG_EXC_GET                       (0xF803U)
/* Modify debug engine configuration */
#define RVM_KFN_DEBUG_MODE_MOD                      (0xF804U)
/* Modify instruction breakpoint state */
#define RVM_KFN_DEBUG_IBP_MOD                       (0xF805U)
/* Modify data breakpoint state */
#define RVM_KFN_DEBUG_DBP_MOD                       (0xF806U)
/* End Kernel Function Number ************************************************/

/* Hypervisor Macro **********************************************************/
/* Native process magic number */
#define RVM_MAGIC_NATIVE                            (0x49535953U)
/* Virtual machine magic number */
#define RVM_MAGIC_VIRTUAL                           (0x56495254U)

/* Hypercall */
/* Spurious hypercall, ignore - This may originate from cases where
 * the hypercall was set up and the timer interrupt arrived before the
 * hypercall request is raised by the underlying virtual machine. */
#define RVM_HYP_SPURIOUS                            (0U)
/* Print character */
#define RVM_HYP_PUTCHAR                             (1U)
/* Reboot current virtual machine */
#define RVM_HYP_REBOOT                              (2U)
/* Enable interrupts */
#define RVM_HYP_INT_ENA                             (3U)
/* Disable interrupts */
#define RVM_HYP_INT_DIS                             (4U)
/* Register a physical vector */
#define RVM_HYP_VCT_PHYS                            (5U)
/* Register a event */
#define RVM_HYP_VCT_EVT                             (6U)
/* Delete a virtual vector mapping */
#define RVM_HYP_VCT_DEL                             (7U)
/* Lockdown current virtual vector mapping */
#define RVM_HYP_VCT_LCK                             (8U)
/* Wait for an virtual vector to come */
#define RVM_HYP_VCT_WAIT                            (9U)
/* Add a event source to send to */
#define RVM_HYP_EVT_ADD                             (10U)
/* Delete a event source to send to */
#define RVM_HYP_EVT_DEL                             (11U)
/* Send to an event */
#define RVM_HYP_EVT_SND                             (12U)
/* Start and clear watchdog */
#define RVM_HYP_WDG_CLR                             (13U)

/* Error code */
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
/* End Hypervisor Macro ******************************************************/
/*****************************************************************************/
/* __RVM_SYSLIB_DEF__ */
#endif
/* __HDR_DEF__ */
#endif
/* End Define ****************************************************************/

/* Struct ********************************************************************/
#ifdef __HDR_STRUCT__
#ifndef __RVM_SYSLIB_STRUCT__
#define __RVM_SYSLIB_STRUCT__
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

/* Parameter */
struct RVM_Param
{
    rvm_ptr_t Number;
    rvm_ptr_t Param[4];
};

/* Virtual interrupt flag */
struct RVM_Vctf
{
    /* Timer and context switch handler */
    rvm_ptr_t Tim;
    rvm_ptr_t Ctx;
    /* Actual number depends on configuration, 16 here for guaranteed
     * compiler word alignment on even 128-bit hardware, which is unlikely. 
     * Note that this is NOT to say that the actual number of interrupts
     * must be no more than 16; this is just a placeholder, and we may make
     * semantically out-of-bound accesses, just like the Cop[1] field. */
#ifdef RVM_VIRT_LIB_ENABLE
    /* Appear as whole words in user-level library for quicker traversal */
    rvm_ptr_t Vct[1];
#else
    /* Appear as independent chars in monitor for individual operation */
    rvm_u8_t Vct[16];
#endif
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

/* Thread register set */
struct RVM_Thd_Reg
{
    struct RVM_Reg_Struct Reg;
    struct RVM_Exc_Struct Exc;
    /* Just like the Vct[] field above, this expands as needed; also, this don't
     * when there are no coprocessors. See size reporting macro for details. */
    rvm_ptr_t Cop[1];
};
/*****************************************************************************/
/* __RVM_SYSLIB_STRUCT__ */
#endif
/* __HDR_STRUCT__ */
#endif
/* End Struct ****************************************************************/

/* Private Variable **********************************************************/
#if(!(defined __HDR_DEF__||defined __HDR_STRUCT__))
#ifndef __RVM_SYSLIB_MEMBER__
#define __RVM_SYSLIB_MEMBER__

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
/*****************************************************************************/
/* End Private Variable ******************************************************/

/* Private Function **********************************************************/ 
/*****************************************************************************/
/* External print in process main file or monitor */
#if(RVM_DBGLOG_ENABLE!=0U)
RVM_EXTERN void RVM_Putchar(char Char);
#endif
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
                                 
/* Capability table */
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
                                    
/* Kernel function */
__RVM_EXTERN__ rvm_ret_t RVM_Kfn_Act(rvm_cid_t Cap_Kfn,
                                     rvm_ptr_t Func_ID,
                                     rvm_ptr_t Sub_ID,
                                     rvm_ptr_t Param1,
                                     rvm_ptr_t Param2);

#if(RVM_PGT_RAW_ENABLE==0U)
/* Page table */
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

/* Process */
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
                                  
/* Thread */
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
                                 
/* Signal */
__RVM_EXTERN__ rvm_ret_t RVM_Sig_Crt(rvm_cid_t Cap_Cpt,
                                     rvm_cid_t Cap_Sig);
__RVM_EXTERN__ rvm_ret_t RVM_Sig_Del(rvm_cid_t Cap_Cpt,
                                     rvm_cid_t Cap_Sig);
__RVM_EXTERN__ rvm_ret_t RVM_Sig_Snd(rvm_cid_t Cap_Sig);
__RVM_EXTERN__ rvm_ret_t RVM_Sig_Rcv(rvm_cid_t Cap_Sig,
                                     rvm_ptr_t Option);
                                 
/* Invocation */
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
/* __RVM_SYSLIB_MEMBER__ */
#endif
/* !(defined __HDR_DEF__||defined __HDR_STRUCT__) */
#endif
/* End Public Function *******************************************************/

/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
