/******************************************************************************
Filename    : rvm_hyper.h
Author      : pry
Date        : 18/10/2017
Licence     : The Unlicense; see LICENSE for details.
Description : The header of microcontroller user-level library.
******************************************************************************/

/* Defines *******************************************************************/
#ifdef __HDR_DEFS__
#ifndef __RVM_HYPER_H_DEFS__
#define __RVM_HYPER_H_DEFS__
/*****************************************************************************/
/* Hypercalls */
/* Print character */
#define RVM_HYP_PUTCHAR             (0U)
/* Enable interrupts */
#define RVM_HYP_INT_ENA             (1U)
/* Disable interrupts */
#define RVM_HYP_INT_DIS             (2U)
/* Register a physical vector */
#define RVM_HYP_VECT_PHYS           (3U)
/* Register a event */
#define RVM_HYP_VECT_EVT            (4U)
/* Delete a virtual vector mapping */
#define RVM_HYP_VECT_DEL            (5U)
/* Lockdown current virtual vector mapping */
#define RVM_HYP_VECT_LOCK           (6U)
/* Wait for an virtual vector to come */
#define RVM_HYP_VECT_WAIT           (7U)
/* Add a event source to send to */
#define RVM_HYP_EVT_ADD             (8U)
/* Delete a event source to send to */
#define RVM_HYP_EVT_DEL             (9U)
/* Send to an event */
#define RVM_HYP_EVT_SEND            (10U)
/* Start and feed watchdog */
#define RVM_HYP_WDOG_FEED           (11U)

/* Error codes */
/* The state is wrong */
#define RVM_ERR_STATE               (-1)
/* The number is overrange */
#define RVM_ERR_RANGE               (-2)
/* The error is address related */
#define RVM_ERR_ADDR                (-3)
/* The error is physical vector related */
#define RVM_ERR_PHYS                (-4)
/* The error is virtual vector related */
#define RVM_ERR_VIRT                (-5)
/* The error is event related */
#define RVM_ERR_EVT                 (-6)
/* The error is mapping block related */
#define RVM_ERR_MAP                 (-7)

/* Size of bitmap */
#define RVM_VPRIO_BITMAP            ((RVM_PREEMPT_VPRIO_NUM+RVM_WORD_SIZE-1U)/RVM_WORD_SIZE)
#define RVM_EVTCAP_BITMAP           ((RVM_VIRT_EVENT_NUM+RVM_WORD_SIZE-1U)/RVM_WORD_SIZE)
#define RVM_VCTF_BITMAP(X)          (((X)+RVM_WORD_SIZE-1U)/RVM_WORD_SIZE)

/* States of virtual machines */
#define RVM_VM_STATE(X)             ((X)&0xFFU)
#define RVM_VM_FLAG(X)              ((X)&~0xFFU)
#define RVM_VM_STATE_SET(X,S)       ((X)=(RVM_VM_FLAG(X)|(S)))

/* The virtual machine is running */
#define RVM_VM_RUNNING              (0U)
/* The virtual machine is temporarily suspended and is waiting for an interrupt */
#define RVM_VM_WAITING              (1U)
/* The virtual machine have its interrupt enabled */
#define RVM_VM_INTENA               RVM_POW2(sizeof(rvm_ptr_t)*4U)
/* The virtual machine have finished its booting */
#define RVM_VM_BOOTDONE             (RVM_VM_INTENA<<1)
/* The virtual machine have its virtual vector mappings locked down */
#define RVM_VM_VECTLOCK             (RVM_VM_BOOTDONE<<1)
/* The watchdog for this virtual machine is enabled */
#define RVM_VM_WDOGENA              (RVM_VM_VECTLOCK<<1)
    
/* The timer wheel size */
#define RVM_WHEEL_SIZE              (32U)
#define RVM_DST2MAP(X)              ((struct RVM_Map_Struct*)(((rvm_ptr_t)(X))-sizeof(struct RVM_List)))
#define RVM_DLY2VM(X)               ((struct RVM_Virt_Struct*)(((rvm_ptr_t)(X))-sizeof(struct RVM_List)))

/* Virtual machine thread ID identification */
#define RVM_VIRT_THDID_MARKER       ((rvm_tid_t)RVM_POW2(16U))

/* Communication flag set selection */
#define RVM_FLAG_SET(B, S, N)       ((volatile struct RVM_Flag*)((B)+((S)>>1)*(N)))

/* Header branch address extraction */
#define RVM_DESC_ENTRY(B, X)        (((const struct RVM_Desc*)(B))->Entry[X])
#define RVM_DESC_MAGIC(B)           (((const struct RVM_Desc*)(B))->Magic)
#define RVM_DESC_NUM(B)             (((const struct RVM_Desc*)(B))->Number)
#define RVM_DESC_STUB(B)            RVM_DESC_ENTRY(B, RVM_DESC_NUM(B)-1U)
/*****************************************************************************/
/* __RVM_HYPER_H_DEFS__ */
#endif
/* __HDR_DEFS__ */
#endif
/* End Defines ***************************************************************/

/* Structs *******************************************************************/
#ifdef __HDR_STRUCTS__
#ifndef __RVM_HYPER_H_STRUCTS__
#define __RVM_HYPER_H_STRUCTS__

/* Use defines in these headers */
#define __HDR_DEFS__
#undef __HDR_DEFS__
/*****************************************************************************/
#if(RVM_PREEMPT_VPRIO_NUM!=0U)
/* Parameter */
struct RVM_Param
{
    rvm_ptr_t Number;
    rvm_ptr_t Param[4];
};

/* Interrupt flags */
struct RVM_Vctf
{
    rvm_ptr_t Timer;
    rvm_ptr_t Ctxsw;
    rvm_ptr_t Vect[1];
};

/* State block */
struct RVM_State
{
    /* Indicate that the vector thread is now active */
    rvm_ptr_t Vect_Act;
    /* Parameter area, one for vector, another for user */
    struct RVM_Param Vect;
    struct RVM_Param User;
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
    volatile struct RVM_Virt_Struct* Virt;
    /* The virtual machine vector to send to */
    rvm_ptr_t Vect_Num;
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
    rvm_ptr_t Vect_Num;

    /* Register base */
    volatile struct RVM_Regs* Reg_Base;
    /* Parameter base & size */
    volatile struct RVM_State* State_Base;
    rvm_ptr_t State_Size;
    /* Descriptor header base address */
    rvm_ptr_t Desc_Base;
    /* Vector signal endpoint capability */
    rvm_cid_t Vect_Sig_Cap;
    
    /* Vector thread capability */
    rvm_cid_t Vect_Thd_Cap;
    /* Vector thread stack base */
    rvm_ptr_t Vect_Stack_Base;
    /* Vector thread size */
    rvm_ptr_t Vect_Stack_Size;
    
    /* User thread capability */
    rvm_cid_t User_Thd_Cap;
    /* User thread stack base */
    rvm_ptr_t User_Stack_Base;
    /* User thread size */
    rvm_ptr_t User_Stack_Size;
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
    struct RVM_List Vect_Head;
    /* ROM database */
    const struct RVM_Vmap_Struct* Map;
    /* Event source send capability table */
    rvm_ptr_t Evt_Cap[RVM_EVTCAP_BITMAP];
};
#endif

/* Interrupt flags */
struct RVM_Flag
{
    rvm_ptr_t Lock;
    rvm_ptr_t Group;
    rvm_ptr_t Flags[1024];
};

/* Process descriptor header */
struct RVM_Desc
{
    rvm_ptr_t Magic;
    rvm_ptr_t Number;
    rvm_ptr_t Entry[1024];
};
/*****************************************************************************/
/* __RVM_HYPER_H_STRUCTS__ */
#endif
/* __HDR_STRUCTS__ */
#endif
/* End Structs ***************************************************************/

/* Private Global Variables **************************************************/
#if(!(defined __HDR_DEFS__||defined __HDR_STRUCTS__))
#ifndef __RVM_HYPER_MEMBERS__
#define __RVM_HYPER_MEMBERS__

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
#if(RVM_PREEMPT_VPRIO_NUM!=0U)
/* Helper functions */
static void _RVM_Run_Ins(volatile struct RVM_Virt_Struct* Virt);
static void _RVM_Run_Del(volatile struct RVM_Virt_Struct* Virt);
static volatile struct RVM_Virt_Struct* _RVM_Run_High(void);

static void _RVM_Virt_Switch(volatile struct RVM_Virt_Struct* From, 
                             volatile struct RVM_Virt_Struct* To);
static void _RVM_Virt_Vect_Send(volatile struct RVM_List* Array,
                                rvm_ptr_t Num);
static void _RVM_Virt_Cur_Recover(void);
                                
static rvm_ret_t _RVM_Vect_Pend_Check(volatile struct RVM_Virt_Struct* Virt);
static void _RVM_Vect_Flag_Set(volatile struct RVM_Virt_Struct* Virt,
                               rvm_ptr_t Vect_Num);

static void _RVM_Wheel_Ins(volatile struct RVM_Virt_Struct* Virt,
                           rvm_ptr_t Period);
static void _RVM_Timer_Send(volatile struct RVM_Virt_Struct* Virt);
static rvm_ptr_t _RVM_Flagset_Get(volatile struct RVM_Flag* Set);

#if(RVM_DEBUG_PRINT==1U)
static rvm_ret_t RVM_Hyp_Putchar(rvm_ptr_t Char);
#endif

static rvm_ret_t RVM_Hyp_Int_Ena(void);
static rvm_ret_t RVM_Hyp_Int_Dis(void);

static rvm_ret_t RVM_Hyp_Vect_Phys(rvm_ptr_t Phys_Num,
                                   rvm_ptr_t Vect_Num);
static rvm_ret_t RVM_Hyp_Vect_Evt(rvm_ptr_t Evt_Num,
                                  rvm_ptr_t Vect_Num);
static rvm_ret_t RVM_Hyp_Vect_Del(rvm_ptr_t Vect_Num);
static rvm_ret_t RVM_Hyp_Vect_Wait(void);
static rvm_ret_t RVM_Hyp_Vect_Lock(void);

static rvm_ret_t RVM_Hyp_Evt_Add(rvm_ptr_t Evt_Num);
static rvm_ret_t RVM_Hyp_Evt_Del(rvm_ptr_t Evt_Num);
static rvm_ret_t RVM_Hyp_Evt_Send(rvm_ptr_t Evt_Num);

static rvm_ret_t RVM_Hyp_Wdog_Feed(void);
#endif
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
#if(RVM_PREEMPT_VPRIO_NUM!=0U)
/* Timestamp value */
__EXTERN__ volatile rvm_ptr_t RVM_Tick;
/* Timer wheel - This system supports about 64 VMs maximum, thus we set the timer wheel at 32 */
__EXTERN__ volatile struct RVM_List RVM_Wheel[RVM_WHEEL_SIZE];

/* The current virtual machine */
__EXTERN__ volatile struct RVM_Virt_Struct* volatile RVM_Virt_Cur;
/* Virtual machine run queue and bitmap */
__EXTERN__ volatile rvm_ptr_t RVM_Bitmap[RVM_VPRIO_BITMAP];
__EXTERN__ volatile struct RVM_List RVM_Run[RVM_PREEMPT_VPRIO_NUM];

/* Event header */
__EXTERN__ volatile struct RVM_List RVM_Evt[RVM_VIRT_EVENT_NUM];
/* Physical vector header */
__EXTERN__ volatile struct RVM_List RVM_Phys[RVM_PHYS_VECT_NUM];

/* Mapping database */
__EXTERN__ volatile struct RVM_List RVM_Map_Free;
__EXTERN__ volatile struct RVM_Map_Struct RVM_Map[RVM_VIRT_MAP_NUM];
#endif
/*****************************************************************************/

/* End Public Global Variables ***********************************************/

/* Public C Function Prototypes **********************************************/
/*****************************************************************************/
#if(RVM_PREEMPT_VPRIO_NUM!=0U)
/* Initializing functions */
__EXTERN__ void RVM_Virt_Init(void);
__EXTERN__ void RVM_Virt_Crt(struct RVM_Virt_Struct* Virt,
                             const struct RVM_Vmap_Struct* Vmap,
                             rvm_ptr_t Virt_Num);
/* VM daemons */
__EXTERN__ void RVM_Timd(void);
__EXTERN__ void RVM_Vmmd(void);
__EXTERN__ void RVM_Vctd(void);
#endif

/* Generic daemons */
__EXTERN__ void RVM_Sftd(void);
/*****************************************************************************/
/* Undefine "__EXTERN__" to avoid redefinition */
#undef __EXTERN__
/* __RVM_HYPER_MEMBERS__ */
#endif
/* !(defined __HDR_DEFS__||defined __HDR_STRUCTS__) */
#endif
/* End Public C Function Prototypes ******************************************/

/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/