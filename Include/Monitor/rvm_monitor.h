/******************************************************************************
Filename    : rvm_monitor.h
Author      : pry
Date        : 25/06/2017
Licence     : The Unlicense; see LICENSE for details.
Description : Virtual machine monitor header.
******************************************************************************/

/* Define ********************************************************************/
#ifdef __HDR_DEF__
#ifndef __RVM_MONITOR_DEF__
#define __RVM_MONITOR_DEF__
/*****************************************************************************/
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
static void _RVM_Virt_Recover(rvm_s8_t* Daemon);

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
#if(RVM_DBGLOG_ENABLE!=0U)
/* Debugging */
__RVM_EXTERN__ void RVM_Putchar(char Char);
#endif
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
