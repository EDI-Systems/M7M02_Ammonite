/******************************************************************************
Filename    : RVM.h
Author      : pry
Date        : 01/04/2017
Licence     : The Unlicense; see LICENSE for details.
Description : The header of the RVM user-level library. This header defines the
              error codes, operation flags and system call numbers in a generic way.
******************************************************************************/

#ifndef __RVM_H__
#define __RVM_H__
/* Defines *******************************************************************/

/* Errors ********************************************************************/
/* The base of capability table error */
#define RVM_ERR_CPT                     (0)
/* The capability is empty */
#define RVM_ERR_CPT_NULL                ((-1)+RVM_ERR_CPT)
/* The capability type is wrong */
#define RVM_ERR_CPT_TYPE                ((-2)+RVM_ERR_CPT)
/* The range of the capability is wrong */
#define RVM_ERR_CPT_RANGE               ((-3)+RVM_ERR_CPT)
/* The kernel object table operation of the capability is wrong */
#define RVM_ERR_CPT_KOTBL               ((-4)+RVM_ERR_CPT)
/* The capability number already exists, or not exist, depending on the function */
#define RVM_ERR_CPT_EXIST               ((-5)+RVM_ERR_CPT)
/* When freezing capabilities, the refcnt is not zero, or when delegating, refcnt overflowed */
#define RVM_ERR_CPT_REFCNT              ((-6)+RVM_ERR_CPT)
/* The flags of the fine-grain-controlled capability is not correct */
#define RVM_ERR_CPT_FLAG                ((-7)+RVM_ERR_CPT)
/* Something involved is not quiescent */
#define RVM_ERR_CPT_QUIE                ((-8)+RVM_ERR_CPT)
/* Something involved is frozen */
#define RVM_ERR_CPT_FROZEN              ((-9)+RVM_ERR_CPT)
/* The capability is a root one and therefore cannot use removal */
#define RVM_ERR_CPT_ROOT                ((-10)+RVM_ERR_CPT)

/* The base of page table errors */
#define RVM_ERR_PGT                     (-100)
/* Incorrect address */
#define RVM_ERR_PGT_ADDR                ((-1)+RVM_ERR_PGT)
/* Mapping failure */
#define RVM_ERR_PGT_MAP                 ((-2)+RVM_ERR_PGT)
/* Hardware restrictions */
#define RVM_ERR_PGT_HW                  ((-3)+RVM_ERR_PGT)
/* Wrong permissions */
#define RVM_ERR_PGT_PERM                ((-4)+RVM_ERR_PGT)

/* The base of process/thread errors */
#define RVM_ERR_PTH                     (-200)
/* Incorrect address */
#define RVM_ERR_PTH_PGT                 ((-1)+RVM_ERR_PTH)
/* Conflicting operations happening at the same time */
#define RVM_ERR_PTH_CONFLICT            ((-2)+RVM_ERR_PTH)
/* Thread ID error */
#define RVM_ERR_PTH_TID                 ((-3)+RVM_ERR_PTH)
/* Thread state is invalid or we have failed the CAS */
#define RVM_ERR_PTH_INVSTATE            ((-4)+RVM_ERR_PTH)
/* The thread priority is not correct */
#define RVM_ERR_PTH_PRIO                ((-5)+RVM_ERR_PTH)
/* The reference count not correct */
#define RVM_ERR_PTH_REFCNT              ((-6)+RVM_ERR_PTH)
/* There are no notifications sent to this thread */
#define RVM_ERR_PTH_NOTIF               ((-7)+RVM_ERR_PTH)
/* The time is full for this thread */
#define RVM_ERR_PTH_OVERFLOW            ((-8)+RVM_ERR_PTH)
/* Cannot conduct operation because target have a fault */
#define RVM_ERR_PTH_FAULT               ((-9)+RVM_ERR_PTH)

/* The base of signal/invocation errors */
#define RVM_ERR_SIV                     (-300)
/* This invocation capability/signal endpoint is already active, or wrong option on receive */
#define RVM_ERR_SIV_ACT                 ((-1)+RVM_ERR_SIV)
/* This invocation succeeded, but a fault happened in the thread, and we forced a invocation return */
#define RVM_ERR_SIV_FAULT               ((-2)+RVM_ERR_SIV)
/* The thread's signal counter is full */
#define RVM_ERR_SIV_FULL                ((-3)+RVM_ERR_SIV)
/* The thread's invocation stack is empty, cannot return */
#define RVM_ERR_SIV_EMPTY               ((-4)+RVM_ERR_SIV)
/* The signal capability is operated on by two threads simutaneously */
#define RVM_ERR_SIV_CONFLICT            ((-5)+RVM_ERR_SIV)
/* The signal receive system call ended with a result of free(forced unblock) */
#define RVM_ERR_SIV_FREE                ((-6)+RVM_ERR_SIV)
/* The signal receive failed because we are the boot-time thread */
#define RVM_ERR_SIV_BOOT                ((-7)+RVM_ERR_SIV)
/* End Errors ****************************************************************/

/* Operation Flags ***********************************************************/
/* Cpt */
/* This cap to captbl allows creating kernel objects in the captbl */
#define RVM_CPT_FLAG_CRT                (1U<<0)
/* This cap to captbl allows deleting kernel objects in the captbl */
#define RVM_CPT_FLAG_DEL                (1U<<1)
/* This cap to captbl allows freezing kernel objects in the captbl */
#define RVM_CPT_FLAG_FRZ                (1U<<2)
/* This cap to captbl allows delegating kernel objects in it */
#define RVM_CPT_FLAG_ADD_SRC            (1U<<3)
/* This cap to captbl allows receiving delegated kernel objects to it */
#define RVM_CPT_FLAG_ADD_DST            (1U<<4)  
/* This cap to captbl allows removal operation in kernel objects(captbls) in the captbl */
#define RVM_CPT_FLAG_REM                (1U<<5)
/* This cap to captbl allows itself to be used in process creation */
#define RVM_CPT_FLAG_PRC_CRT            (1U<<6)
/* This cap to captbl allows itself to be used in process capability table replacement */
#define RVM_CPT_FLAG_PRC_CPT            (1U<<7)
/* This cap to captbl allows all operations */
#define RVM_CPT_FLAG_ALL                (RVM_CPT_FLAG_CRT|RVM_CPT_FLAG_DEL|RVM_CPT_FLAG_FRZ| \
                                         RVM_CPT_FLAG_ADD_SRC|RVM_CPT_FLAG_ADD_DST|RVM_CPT_FLAG_REM| \
                                         RVM_CPT_FLAG_PRC_CRT|RVM_CPT_FLAG_PRC_CPT)

/* Page table */
/* This cap to pgtbl allows delegating pages in it */
#define RVM_PGT_FLAG_ADD_SRC            (1U<<0)
/* This cap to pgtbl allows receiving delegated pages to it */
#define RVM_PGT_FLAG_ADD_DST            (1U<<1)
/* This cap to pgtbl allows removal of pages in it */
#define RVM_PGT_FLAG_REM                (1U<<2) 
/* This cap to pgtbl allows to be mapped into other page tables as a child
 * or destructed from other page tables as a child */
#define RVM_PGT_FLAG_CHILD              (1U<<3)
/* This cap to pgtbl allows accepting lower page table mappings */
#define RVM_PGT_FLAG_CON_PARENT         (1U<<4)
/* This cap to pgtbl allows its lower level page table mappings to be destructed */
#define RVM_PGT_FLAG_DES_PARENT         (1U<<5)
/* This cap to pgtbl allows itself to be used in process creation */
#define RVM_PGT_FLAG_PRC_CRT            (1U<<6)
/* This cap to pgtbl allows itself to be used in process page table replacement */
#define RVM_PGT_FLAG_PRC_PGT            (1U<<7)
/* This cap to pgtbl allows all operations */
#define RVM_PGT_FLAG_ALL                (RVM_PGT_FLAG_ADD_SRC|RVM_PGT_FLAG_ADD_DST|RVM_PGT_FLAG_REM| \
                                         RVM_PGT_FLAG_CHILD|RVM_PGT_FLAG_CON_PARENT|RVM_PGT_FLAG_DES_PARENT| \
                                         RVM_PGT_FLAG_PRC_CRT|RVM_PGT_FLAG_PRC_PGT)

/* Kernel memory */
/* This cap to kernel memory allows creation of captbl */
#define RVM_KOM_FLAG_CPT                (1U<<0)
/* This cap to kernel memory allows creation of pgtbl */
#define RVM_KOM_FLAG_PGT                (1U<<1)
/* This cap to kernel memory allows creation of thread */
#define RVM_KOM_FLAG_THD                (1U<<2)
/* This cap to kernel memory allows creation of invocation */
#define RVM_KOM_FLAG_INV                (1U<<3)
/* This cap to kernel memory allows all operations */
#define RVM_KOM_FLAG_ALL                (RVM_KOM_FLAG_CPT|RVM_KOM_FLAG_PGT| \
                                         RVM_KOM_FLAG_THD|RVM_KOM_FLAG_INV)

/* Process */
/* This cap to process allows creating invocation stubs in it */
#define RVM_PRC_FLAG_INV                (1U<<0)
/* This cap to process allows creating threads in it */
#define RVM_PRC_FLAG_THD                (1U<<1)
/* This cap to process allows changing its capability table */
#define RVM_PRC_FLAG_CPT                (1U<<2)
/* This cap to process allows changing its page table */
#define RVM_PRC_FLAG_PGT                (1U<<3)
/* This cap to process allows all operations */
#define RVM_PRC_FLAG_ALL                (RVM_PRC_FLAG_INV|RVM_PRC_FLAG_THD| \
                                         RVM_PRC_FLAG_CPT|RVM_PRC_FLAG_PGT)

/* Thread */
/* This cap to thread allows setting its execution parameters */
#define RVM_THD_FLAG_EXEC_SET           (1U<<0)
/* This cap to thread allows setting its scheduling parameters */
#define RVM_THD_FLAG_SCHED_CHILD        (1U<<1)
/* This cap to thread allows registering it as a scheduler */
#define RVM_THD_FLAG_SCHED_PARENT       (1U<<2)
/* This cap to thread allows changing its priority level */
#define RVM_THD_FLAG_SCHED_PRIO         (1U<<3)
/* This cap to thread allows freeing the thread from the core */
#define RVM_THD_FLAG_SCHED_FREE         (1U<<4)
/* This cap to thread allows getting scheduling notifications */
#define RVM_THD_FLAG_SCHED_RCV          (1U<<5)
/* This cap to thread allows acting as a source for time transfer */
#define RVM_THD_FLAG_XFER_SRC           (1U<<6)
/* This cap to thread allows acting as a destination for time transfer */
#define RVM_THD_FLAG_XFER_DST           (1U<<7)
/* This cap to thread allows switching to it */
#define RVM_THD_FLAG_SWT                (1U<<8)
/* This cap to thread allows all operations */
#define RVM_THD_FLAG_ALL                (RVM_THD_FLAG_EXEC_SET|RVM_THD_FLAG_SCHED_CHILD| \
                                         RVM_THD_FLAG_SCHED_PARENT|RVM_THD_FLAG_SCHED_PRIO|RVM_THD_FLAG_SCHED_FREE| \
                                         RVM_THD_FLAG_SCHED_RCV|RVM_THD_FLAG_XFER_SRC|RVM_THD_FLAG_XFER_DST|RVM_THD_FLAG_SWT)
/* Signal */
/* This cap to signal endpoint allows sending to it */
#define RVM_SIG_FLAG_SND                (1U<<0)
/* This cap to signal endpoint allows blocking single receive on it */
#define RVM_SIG_FLAG_RCV_BS             (1U<<1)
/* This cap to signal endpoint allows blocking multi receive on it */
#define RVM_SIG_FLAG_RCV_BM             (1U<<2)
/* This cap to signal endpoint allows non-blocking single receive on it */
#define RVM_SIG_FLAG_RCV_NS             (1U<<3)
/* This cap to signal endpoint allows non-blocking multi receive on it */
#define RVM_SIG_FLAG_RCV_NM             (1U<<4)
/* This cap to signal endpoint allows all receiving operations */
#define RVM_SIG_FLAG_RCV                (RVM_SIG_FLAG_RCV_BS|RVM_SIG_FLAG_RCV_BM| \
                                         RVM_SIG_FLAG_RCV_NS|RVM_SIG_FLAG_RCV_NM)
/* This cap to signal endpoint allows sending scheduler notification to it */
#define RVM_SIG_FLAG_SCHED              (1U<<5)
/* This cap to signal endpoint allows all operations */
#define RVM_SIG_FLAG_ALL                (RVM_SIG_FLAG_SND|RVM_SIG_FLAG_RCV|RVM_SIG_FLAG_SCHED)

/* Invocation */
/* This cap to invocation allows setting parameters for it */
#define RVM_INV_FLAG_SET                (1U<<0)
/* This cap to invocation allows activating it */
#define RVM_INV_FLAG_ACT                (1U<<1)
/* The return operation does not need a flag, nor does it need a capability */
/* This cap to invocation allows all operations */
#define RVM_INV_FLAG_ALL                (RVM_INV_FLAG_SET|RVM_INV_FLAG_ACT)
/* End Operation Flags *******************************************************/

/* Special Definitions *******************************************************/
/* Generic page table flags */
/* This page allows to be read */
#define RVM_PGT_READ                    (1U<<0)
/* This page allows to be written */
#define RVM_PGT_WRITE                   (1U<<1)
/* This page allows execution */
#define RVM_PGT_EXECUTE                 (1U<<2)
/* This page is cacheable */
#define RVM_PGT_CACHE                   (1U<<3)
/* This page is bufferable (write-back can be used instead of write-through) */
#define RVM_PGT_BUFFER                  (1U<<4)
/* This page is pinned in TLB */
#define RVM_PGT_STATIC                  (1U<<5)
/* All the permissions are set */
#define RVM_PGT_ALL_PERM                (RVM_PGT_READ|RVM_PGT_WRITE|RVM_PGT_EXECUTE| \
                                         RVM_PGT_CACHE|RVM_PGT_BUFFER|RVM_PGT_STATIC)
                                        
/* Generic page size order definitions */
#define RVM_PGT_SIZE_2B                 (1U)
#define RVM_PGT_SIZE_4B                 (2U)
#define RVM_PGT_SIZE_8B                 (3U)
#define RVM_PGT_SIZE_16B                (4U)
#define RVM_PGT_SIZE_32B                (5U)
#define RVM_PGT_SIZE_64B                (6U)
#define RVM_PGT_SIZE_128B               (7U)
#define RVM_PGT_SIZE_256B               (8U)
#define RVM_PGT_SIZE_512B               (9U)
#define RVM_PGT_SIZE_1K                 (10U)
#define RVM_PGT_SIZE_2K                 (11U)
#define RVM_PGT_SIZE_4K                 (12U)
#define RVM_PGT_SIZE_8K                 (13U)
#define RVM_PGT_SIZE_16K                (14U)
#define RVM_PGT_SIZE_32K                (15U)
#define RVM_PGT_SIZE_64K                (16U)
#define RVM_PGT_SIZE_128K               (17U)
#define RVM_PGT_SIZE_256K               (18U)
#define RVM_PGT_SIZE_512K               (19U)
#define RVM_PGT_SIZE_1M                 (20U)
#define RVM_PGT_SIZE_2M                 (21U)
#define RVM_PGT_SIZE_4M                 (22U)
#define RVM_PGT_SIZE_8M                 (23U)
#define RVM_PGT_SIZE_16M                (24U)
#define RVM_PGT_SIZE_32M                (25U)
#define RVM_PGT_SIZE_64M                (26U)
#define RVM_PGT_SIZE_128M               (27U)
#define RVM_PGT_SIZE_256M               (28U)
#define RVM_PGT_SIZE_512M               (29U)
#define RVM_PGT_SIZE_1G                 (30U)
#define RVM_PGT_SIZE_2G                 (31U)
#define RVM_PGT_SIZE_4G                 (32U)
#define RVM_PGT_SIZE_8G                 (33U)
#define RVM_PGT_SIZE_16G                (34U)
#define RVM_PGT_SIZE_32G                (35U)
#define RVM_PGT_SIZE_64G                (36U)
#define RVM_PGT_SIZE_128G               (37U)
#define RVM_PGT_SIZE_256G               (38U)
#define RVM_PGT_SIZE_512G               (39U)
#define RVM_PGT_SIZE_1T                 (40U)
#define RVM_PGT_SIZE_2T                 (41U)
#define RVM_PGT_SIZE_4T                 (42U)
#define RVM_PGT_SIZE_8T                 (43U)
#define RVM_PGT_SIZE_16T                (44U)
#define RVM_PGT_SIZE_32T                (45U)
#define RVM_PGT_SIZE_64T                (46U)
#define RVM_PGT_SIZE_128T               (47U)
#define RVM_PGT_SIZE_256T               (48U)
#define RVM_PGT_SIZE_512T               (49U)
#define RVM_PGT_SIZE_1P                 (50U)
#define RVM_PGT_SIZE_2P                 (51U)
#define RVM_PGT_SIZE_4P                 (52U)
#define RVM_PGT_SIZE_8P                 (53U)
#define RVM_PGT_SIZE_16P                (54U)
#define RVM_PGT_SIZE_32P                (55U)
#define RVM_PGT_SIZE_64P                (56U)
#define RVM_PGT_SIZE_128P               (57U)
#define RVM_PGT_SIZE_256P               (58U)
#define RVM_PGT_SIZE_512P               (59U)
#define RVM_PGT_SIZE_1E                 (60U)
#define RVM_PGT_SIZE_2E                 (61U)
#define RVM_PGT_SIZE_4E                 (62U)
#define RVM_PGT_SIZE_8E                 (63U)
#define RVM_PGT_SIZE_16E                (64U)
#define RVM_PGT_SIZE_32E                (65U)
#define RVM_PGT_SIZE_64E                (66U)
#define RVM_PGT_SIZE_128E               (67U)
#define RVM_PGT_SIZE_256E               (68U)
#define RVM_PGT_SIZE_512E               (69U)
#define RVM_PGT_SIZE_1Z                 (70U)

/* Generic page table entry number definitions */
#define RVM_PGT_NUM_1                   (0U)
#define RVM_PGT_NUM_2                   (1U)
#define RVM_PGT_NUM_4                   (2U)
#define RVM_PGT_NUM_8                   (3U)
#define RVM_PGT_NUM_16                  (4U)
#define RVM_PGT_NUM_32                  (5U)
#define RVM_PGT_NUM_64                  (6U)
#define RVM_PGT_NUM_128                 (7U)
#define RVM_PGT_NUM_256                 (8U)
#define RVM_PGT_NUM_512                 (9U)
#define RVM_PGT_NUM_1K                  (10U)
#define RVM_PGT_NUM_2K                  (11U)
#define RVM_PGT_NUM_4K                  (12U)
#define RVM_PGT_NUM_8K                  (13U)
#define RVM_PGT_NUM_16K                 (14U)
#define RVM_PGT_NUM_32K                 (15U)
#define RVM_PGT_NUM_64K                 (16U)
#define RVM_PGT_NUM_128K                (17U)
#define RVM_PGT_NUM_256K                (18U)
#define RVM_PGT_NUM_512K                (19U)
#define RVM_PGT_NUM_1M                  (20U)
#define RVM_PGT_NUM_2M                  (21U)
#define RVM_PGT_NUM_4M                  (22U)

/* Receive options */
#define RVM_RCV_BS                      (0U)
#define RVM_RCV_BM                      (1U)
#define RVM_RCV_NS                      (2U)
#define RVM_RCV_NM                      (3U)
/* End Special Definitions ***************************************************/

/* Syystem Calls *************************************************************/
/* IPC activation ************************************************************/
/* Return from an invocation */
#define RVM_SVC_INV_RET                 (0U)
/* Activate the invocation */
#define RVM_SVC_INV_ACT                 (1U)
/* Send to a signal endpoint */
#define RVM_SVC_SIG_SND                 (2U)
/* Receive from a signal endpoint */
#define RVM_SVC_SIG_RCV                 (3U)
/* Kernel function calling ***************************************************/
#define RVM_SVC_KFN                     (4U)
/* The operations that may cause a context switch ****************************/
/* Changing thread priority */
#define RVM_SVC_THD_SCHED_PRIO          (5U)
/* Free a thread from some core */
#define RVM_SVC_THD_SCHED_FREE          (6U)
/* Transfer time to a thread */
#define RVM_SVC_THD_TIME_XFER           (7U)
/* Switch to another thread */
#define RVM_SVC_THD_SWT                 (8U)
/* Capability table operations ***********************************************/
/* Create */
#define RVM_SVC_CPT_CRT                 (9U)
/* Delete */
#define RVM_SVC_CPT_DEL                 (10U)
/* Freeze */
#define RVM_SVC_CPT_FRZ                 (11U)
/* Add */
#define RVM_SVC_CPT_ADD                 (12U)
/* Remove */
#define RVM_SVC_CPT_REM                 (13U)
/* Page table operations *****************************************************/
/* Create */
#define RVM_SVC_PGT_CRT                 (14U)
/* Delete */
#define RVM_SVC_PGT_DEL                 (15U)
/* Add */
#define RVM_SVC_PGT_ADD                 (16U)
/* Remove */
#define RVM_SVC_PGT_REM                 (17U)
/* Construction */
#define RVM_SVC_PGT_CON                 (18U)
/* Destruction */
#define RVM_SVC_PGT_DES                 (19U)
/* Process operations ********************************************************/
/* Create */
#define RVM_SVC_PRC_CRT                 (20U)
/* Delete */
#define RVM_SVC_PRC_DEL                 (21U)
/* Change captbl */
#define RVM_SVC_PRC_CPT                 (22U)
/* Change pgtbl */ 
#define RVM_SVC_PRC_PGT                 (23U)
/* Thread operations *********************************************************/
/* Create */
#define RVM_SVC_THD_CRT                 (24U)
/* Delete */
#define RVM_SVC_THD_DEL                 (25U)
/* Set entry&stack */
#define RVM_SVC_THD_EXEC_SET            (26U)
/* Bind to the current processor */
#define RVM_SVC_THD_SCHED_BIND          (27U)
/* Try to receive scheduling notifications */
#define RVM_SVC_THD_SCHED_RCV           (28U)
/* Signal operations *********************************************************/
/* Create */
#define RVM_SVC_SIG_CRT                 (29U)
/* Delete */
#define RVM_SVC_SIG_DEL                 (30U)
/* Invocation operations *****************************************************/
/* Create */
#define RVM_SVC_INV_CRT                 (31U)
/* Delete */
#define RVM_SVC_INV_DEL                 (32U)
/* Set entry&stack */
#define RVM_SVC_INV_SET                 (33U)
/* End System Calls **********************************************************/

/* Kernel Functions **********************************************************/
/* Page table operations *****************************************************/
/* Clear the whole TLB */
#define RVM_KFN_PGT_CACHE_CLR           (0xF000U)
/* Clear a single TLB line */
#define RVM_KFN_PGT_LINE_CLR            (0xF001U)
/* Set the ASID of a page table */
#define RVM_KFN_PGT_ASID_SET            (0xF002U)
/* Lock a page into the TLB */
#define RVM_KFN_PGT_TLB_LOCK            (0xF003U)
/* Query or modify the content of an entry */
#define RVM_KFN_PGT_ENTRY_MOD           (0xF004U)
/* Interrupt controller operations *******************************************/
/* Modify local interrupt controller */
#define RVM_KFN_INT_LOCAL_MOD           (0xF100U)
/* Modify global interrupt controller */
#define RVM_KFN_INT_GLOBAL_MOD          (0xF101U)
/* Trigger a local interrupt */
#define RVM_KFN_INT_LOCAL_TRIG          (0xF102U)
/* Trigger a local event */
#define RVM_KFN_EVT_LOCAL_TRIG          (0xF103U)
/* Cache operations **********************************************************/
/* Modify cache state */
#define RVM_KFN_CACHE_MOD               (0xF200U)
/* Configure cache */
#define RVM_KFN_CACHE_CONFIG            (0xF201U)
/* Invalidate cache */
#define RVM_KFN_CACHE_MAINT             (0xF202U)
/* Lock cache */
#define RVM_KFN_CACHE_LOCK              (0xF203U)
/* Modify prefetcher state */
#define RVM_KFN_PRFTH_MOD               (0xF204U)
/* Hot plug and pull operations **********************************************/
/* Modify physical CPU configuration */
#define RVM_KFN_HPNP_PCPU_MOD           (0xF300U)
/* Modify logical CPU configuration */
#define RVM_KFN_HPNP_LCPU_MOD           (0xF301U)
/* Modify physical memory configuration */
#define RVM_KFN_HPNP_PMEM_MOD           (0xF302U)
/* Power and frequency adjustment operations *********************************/
/* Put CPU into idle sleep mode */
#define RVM_KFN_IDLE_SLEEP              (0xF400U)
/* Reboot the whole system */
#define RVM_KFN_SYS_REBOOT              (0xF401U)
/* Shutdown the whole system */
#define RVM_KFN_SYS_SHUTDOWN            (0xF402U)
/* Modify voltage configuration */
#define RVM_KFN_VOLT_MOD                (0xF403U)
/* Modify frequency configuration */
#define RVM_KFN_FREQ_MOD                (0xF404U)
/* Modify power state */
#define RVM_KFN_PMOD_MOD                (0xF405U)
/* Modify safety lock state */
#define RVM_KFN_SAFETY_MOD              (0xF406U)
/* Performance monitoring operations *****************************************/
/* Query or modify CPU function configuration */
#define RVM_KFN_PERF_CPU_FUNC           (0xF500U)
/* Query or modify performance monitor configuration */
#define RVM_KFN_PERF_MON_MOD            (0xF501U)
/* Query or modify counting performance monitor register */
#define RVM_KFN_PERF_CNT_MOD            (0xF502U)
/* Query or modify clock cycle performance monitor register */
#define RVM_KFN_PERF_CYCLE_MOD          (0xF503U)
/* Query or modify data performance monitor register */
#define RVM_KFN_PERF_DATA_MOD           (0xF504U)
/* Query or modify physical monitor register */
#define RVM_KFN_PERF_PHYS_MOD           (0xF505U)
/* Query or modify cumulative monitor register */
#define RVM_KFN_PERF_CUMUL_MOD          (0xF506U)
/* Hardware virtualization operations ****************************************/
/* Create a virtual machine */
#define RVM_KFN_VM_CRT                  (0xF600U)
/* Delete a virtual machine */
#define RVM_KFN_VM_DEL                  (0xF601U)
/* Assign a user-level page table to the virtual machine */
#define RVM_KFN_VM_PGT                  (0xF602U)
/* Query or modify virtual machine state */
#define RVM_KFN_VM_MOD                  (0xF603U)
/* Create a virtual CPU */
#define RVM_KFN_VCPU_CRT                (0xF604U)
/* Bind a virtual CPU to a virtual machine */
#define RVM_KFN_VCPU_BIND               (0xF605U)
/* Free a virtual CPU from a virtual machine */
#define RVM_KFN_VCPU_FREE               (0xF606U)
/* Delete a virtual CPU */
#define RVM_KFN_VCPU_DEL                (0xF607U)
/* Query or modify virtual registers */
#define RVM_KFN_VCPU_MOD                (0xF608U)
/* Run the VCPU on this thread */
#define RVM_KFN_VCPU_RUN                (0xF609U)
/* Security monitor operations ***********************************************/
/* Create an enclave */
#define RVM_KFN_ECLV_CRT                (0xF700U)
/* Query or modify an enclave */
#define RVM_KFN_ECLV_MOD                (0xF701U)
/* Delete an enclave */
#define RVM_KFN_ECLV_DEL                (0xF702U)
/* Call into an enclave */
#define RVM_KFN_ECLV_ACT                (0xF703U)
/* Return from an enclave */
#define RVM_KFN_ECLV_RET                (0xF704U)
/* Debugging operations ******************************************************/
/* Debug printing - a single character or a series of characters */
#define RVM_KFN_DEBUG_PRINT             (0xF800U)
/* Modify thread register content */
#define RVM_KFN_DEBUG_REG_MOD           (0xF801U)
/* Modify thread invocation register content */
#define RVM_KFN_DEBUG_INV_MOD           (0xF802U)
/* Get thread error register content */
#define RVM_KFN_DEBUG_EXC_GET           (0xF803U)
/* Modify debug engine configuration */
#define RVM_KFN_DEBUG_MODE_MOD          (0xF804U)
/* Modify instruction breakpoint state */
#define RVM_KFN_DEBUG_IBP_MOD           (0xF805U)
/* Modify data breakpoint state */
#define RVM_KFN_DEBUG_DBP_MOD           (0xF806U)
/* End Kernel Functions ******************************************************/
/* End Defines ***************************************************************/

#endif /* __RVM_H__ */

/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
