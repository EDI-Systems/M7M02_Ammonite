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
/* Enable interrupts */
#define RVM_HYP_ENAINT              (0)
/* Disable interrupts */
#define RVM_HYP_DISINT              (1)
/* Register a physical vector */
#define RVM_HYP_REGPHYS             (2)
/* Register a event */
#define RVM_HYP_REGEVT              (3)
/* Delete a virtual vector mapping */
#define RVM_HYP_DELVECT             (4)
/* Lockdown current virtual vector mapping */
#define RVM_HYP_LOCKVECT            (5)
/* Wait for an virtual vector to come */
#define RVM_HYP_WAITVECT            (6)
/* Send to an event */
#define RVM_HYP_SENDEVT             (7)
/* Start and feed watchdog */
#define RVM_HYP_FEEDWDOG            (8)

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
#define RVM_PRIO_BITMAP             ((RVM_MAX_PREEMPT_VPRIO-1)/RVM_WORD_SIZE+1)

/* States of virtual machines */
#define RVM_VM_STATE(X)             ((X)&0xFF)
#define RVM_VM_FLAG(X)              ((X)&~0xFF)
#define RVM_VM_STATE_SET(X,S)       ((X)=(RVM_VM_FLAG(X)|(S)))

/* The virtual machine is running */
#define RVM_VM_RUNNING              (0)
/* The virtual machine is temporarily suspended and is waiting for an interrupt */
#define RVM_VM_WAITING              (1)
/* The virtual machine have its interrupt enabled */
#define RVM_VM_INTENA               (1<<(sizeof(rvm_ptr_t)*4))
/* The virtual machine have finished its booting */
#define RVM_VM_BOOTDONE             (RVM_VM_INTENA<<1)
/* The virtual machine have its virtual vector mappings locked down */
#define RVM_VM_LOCKVECT             (RVM_VM_BOOTDONE<<1)
/* The watchdog for this virtual machine is enabled */
#define RVM_VM_WDOGENA              (RVM_VM_LOCKVECT<<1)
    
/* The timer wheel size */
#define RVM_WHEEL_SIZE              (32)
#define RVM_DST2MAP(X)              ((struct RVM_Map_Struct*)(((rvm_ptr_t)(X))-sizeof(struct RVM_List)))
#define RVM_DLY2VM(X)               ((struct RVM_Virt*)(((rvm_ptr_t)(X))-sizeof(struct RVM_List)))

/* Virtual machine thread ID identification */
#define RVM_VIRT_THDID_MARKER       RVM_POW2(16)
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
#if(RVM_MAX_PREEMPT_VPRIO!=0)
/* The parameter structure */
struct RVM_Param
{
    rvm_ptr_t Number;
    rvm_ptr_t Param[4];
};

struct RVM_Param_Area
{
    rvm_ptr_t Vect_Active;
    struct RVM_Param Vect;
    struct RVM_Param User;
};

/* The interrupt structure */
struct RVM_Vect_Flag
{
    rvm_ptr_t Timer;
    rvm_ptr_t Ctxsw;
    rvm_ptr_t Vect[1];
};

/* The mapping structure */
struct RVM_Map_Struct
{
    /* The head to insert into the source */
    struct RVM_List Src_Head;
    /* The head to insert into the receiver */
    struct RVM_List Dst_Head;
    /* The pointer to the virtual machine to send to */
    struct RVM_Virt* Virt;
    /* The virtual machine vector to send to */
    rvm_ptr_t Vect_Num;
};

struct RVM_Sched
{
    /* The state of the virtual machine */
    rvm_ptr_t State;
    /* The current number of timeslices left for context switch */
    rvm_ptr_t Slices_Left;
    /* The current number of timeslices left until watchdog timeout */
    rvm_ptr_t Watchdog_Left;
    /* The timer interrupt period timeout time value */
    rvm_ptr_t Period_Timeout;
};

/* The virtual machine structure */
struct RVM_Virt
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
    struct RVM_Virt_Map* Map;
};

/* Static virtual machine database */
struct RVM_Virt_Map
{
    /* Name of the VM */
    rvm_s8_t* Name;

    /* The priority for this virtual machine */
    rvm_ptr_t Prio;
    /* The timeslice for this virtual machine, in hypervisor ticks */
    rvm_ptr_t Slices;
    /* The timer interrupt period, in hypervisor ticks */
    rvm_ptr_t Period;
    /* The watchdog timeout time, in hypervisor ticks */
    rvm_ptr_t Watchdog;
    /* The number of virtual vectors */
    rvm_ptr_t Vect_Num;

    /* Register base */
    struct RVM_Regs* Reg_Base;
    /* Parameter base */
    struct RVM_Param_Area* Param_Base;
    /* Vector flag base */
    struct RVM_Vect_Flag* Vctf_Base;
    /* Stub entry code frontier */
    rvm_ptr_t Entry_Code_Front;
    /* Vector signal endpoint capability */
    rvm_cid_t Vect_Sig_Cap;
    
    /* Vector thread capability */
    rvm_cid_t Vect_Thd_Cap;
    /* Vector thread TID */
    rvm_tid_t Vect_TID;
    /* Vector thread entry */
    rvm_ptr_t Vect_Entry;
    /* Vector thread stack base */
    rvm_ptr_t Vect_Stack_Base;
    /* Vector thread size */
    rvm_ptr_t Vect_Stack_Size;
    
    /* User thread capability */
    rvm_cid_t User_Thd_Cap;
    /* User thread TID */
    rvm_tid_t User_TID;
    /* User thread entry */
    rvm_ptr_t User_Entry;
    /* User thread stack base */
    rvm_ptr_t User_Stack_Base;
    /* User thread size */
    rvm_ptr_t User_Stack_Size;
};
#endif
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
#if(RVM_MAX_PREEMPT_VPRIO!=0)
/* Helper functions */
static void _RVM_Set_Rdy(struct RVM_Virt* Virt);
static void _RVM_Clr_Rdy(struct RVM_Virt* Virt);
static struct RVM_Virt* _RVM_Get_High_Rdy(void);
static void _RVM_Virt_Switch(struct RVM_Virt* From, struct RVM_Virt* To);
static rvm_ret_t _RVM_Check_Vect_Pend(struct RVM_Virt* Virt);
static void _RVM_Set_Vect_Flag(struct RVM_Virt* Virt, rvm_ptr_t Vect_Num);
static void _RVM_Send_Virt_Vects(struct RVM_List* Array, rvm_ptr_t Num);
static void _RVM_Wheel_Ins(struct RVM_Virt* Virt, rvm_ptr_t Period);
static void _RVM_Timer_Send(struct RVM_Virt* Virt);
static void _RVM_Recover_Cur_Virt(void);
static rvm_ptr_t _RVM_Flagset_Get(struct RVM_Flag_Set* Set);

static rvm_ret_t RVM_Hyp_Ena_Int(void);
static rvm_ret_t RVM_Hyp_Dis_Int(void);
static rvm_ret_t RVM_Hyp_Reg_Phys(rvm_ptr_t Phys_Num, rvm_ptr_t Vect_Num);
static rvm_ret_t RVM_Hyp_Reg_Evt(rvm_ptr_t Evt_Num, rvm_ptr_t Vect_Num);
static rvm_ret_t RVM_Hyp_Del_Vect(rvm_ptr_t Vect_Num);
static rvm_ret_t RVM_Hyp_Wait_Vect(void);
static rvm_ret_t RVM_Hyp_Send_Evt(rvm_ptr_t Evt_Num);
static rvm_ret_t RVM_Hyp_Feed_Wdog(void);
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
#if(RVM_MAX_PREEMPT_VPRIO!=0)
/* Timestamp value */
__EXTERN__ rvm_ptr_t RVM_Tick;
/* Timer wheel - This system supports about 64 VMs maximum, thus we set the timer wheel at 32 */
__EXTERN__ struct RVM_List RVM_Wheel[RVM_WHEEL_SIZE];

/* The current virtual machine */
__EXTERN__ struct RVM_Virt* RVM_Cur_Virt;
/* Virtual machine run queue and bitmap */
__EXTERN__ rvm_ptr_t RVM_Bitmap[RVM_PRIO_BITMAP];
__EXTERN__ struct RVM_List RVM_Run[RVM_MAX_PREEMPT_VPRIO];

/* Event header */
__EXTERN__ struct RVM_List RVM_Evt[RVM_EVT_NUM];
/* Physical vector header */
__EXTERN__ struct RVM_List RVM_Phys[RVM_VECT_NUM];

/* Mapping database */
__EXTERN__ struct RVM_List RVM_Map_Free;
__EXTERN__ struct RVM_Map_Struct RVM_Map[RVM_MAP_NUM];
#endif
/*****************************************************************************/

/* End Public Global Variables ***********************************************/

/* Public C Function Prototypes **********************************************/
/*****************************************************************************/
#if(RVM_MAX_PREEMPT_VPRIO!=0)
/* Initializing functions */
__EXTERN__ void RVM_Virt_Init(void);
__EXTERN__ void RVM_Virt_Crt(struct RVM_Virt* Virt_DB, 
                             struct RVM_Virt_Map* Virt_Map, rvm_ptr_t Virt_Size);
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
