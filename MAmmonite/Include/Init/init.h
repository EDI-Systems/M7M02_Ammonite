/******************************************************************************
Filename    : init.h
Author      : pry
Date        : 23/09/2017
Licence     : LGPL v3+; see COPYING for details.
Description : The header of microcontroller user-level library.
******************************************************************************/

/* Defines *******************************************************************/
#ifdef __HDR_DEFS__
#ifndef __INIT_H_DEFS__
#define __INIT_H_DEFS__
/*****************************************************************************/
/* Priority of threads */
#define RVM_GUARD_PRIO           (RVM_MAX_PREEMPT_PRIO-1)
#define RVM_TIMD_PRIO            (RVM_VMD_PRIO)
#define RVM_VMMD_PRIO            (RVM_VMD_PRIO)
#define RVM_INTD_PRIO            (RVM_VMD_PRIO)
#define RVM_VINT_PRIO            (RVM_VMD_PRIO-1)
#define RVM_USER_PRIO            (2)
#define RVM_INIT_PRIO            (1)
#define RVM_WAIT_PRIO            (0)

/* Size of bitmap */
#define RVM_PRIO_BITMAP          ((RVM_MAX_PREEMPT_VPRIO-1)/RVM_WORD_SIZE+1)
#define RVM_VECT_BITMAP(X)       ((X-1)/RVM_WORD_SIZE+1)

/* States of virtual machines */
#define RVM_VM_STATE(X)          ((X)&0xFF)
#define RVM_VM_FLAG(X)           ((X)&~0xFF)
#define RVM_VM_STATE_SET(X,S)    ((X)=(RVM_VM_FLAG(X)|(S)))

/* The virtual machine is running */
#define RVM_VM_RUNNING           (0)
/* The virtual machine is temporarily suspended and is waiting for an interrupt */
#define RVM_VM_WAITEVT           (1)

/* The virtual machine have its interrupt enabled */
#define RVM_VM_INTENA            (1<<(sizeof(ptr_t)<<2))
/* The virtual machine have finished its booting */
#define RVM_VM_BOOTDONE          (RVM_VM_INTENA<<1)
    
/* The timer wheel size */
#define RVM_WHEEL_SIZE           32
#define RVM_DLY2VM(X)            ((struct RVM_Virt*)(((ptr_t)(X))-sizeof(struct RVM_List)))
/*****************************************************************************/
/* __INIT_H_DEFS__ */
#endif
/* __HDR_DEFS__ */
#endif
/* End Defines ***************************************************************/

/* Structs *******************************************************************/
#ifdef __HDR_STRUCTS__
#ifndef __INIT_H_STRUCTS__
#define __INIT_H_STRUCTS__
/* We used structs in the header */

/* Use defines in these headers */
#define __HDR_DEFS__
#undef __HDR_DEFS__
/*****************************************************************************/
/* The image structure */
struct RVM_Image
{
    /* The magic number;always equal to VIRT(0x56495254) */
    const ptr_t Magic;
    /* The name is always 16 bytes long. This is fixed and is not changeable */
    s8 Name[16];

    /* The entry, stack and stack size of the virtual machines */
    void* User_Entry;
    void* User_Stack;
    ptr_t User_Size;
    void* Int_Entry;
    void* Int_Stack;
    ptr_t Int_Size;
    
    /* The parameter space and register set space */
    struct RVM_Param* Param;
    struct RVM_Regs* Regs;
    
    /* The priority and timeslices */
    ptr_t Prio;
    ptr_t Slices;
    
    /* The console space and size */
    ptr_t Console_Size;
    void* Console_Buf;
    
    /* The interrupt space */
    ptr_t Int_Num;
    ptr_t* Int_Flags;

    /* The number of page tables in this image, and where are they stored */
    ptr_t Pgtbl_Num;
    const struct RVM_Hdr_Pgtbl* Pgtbl;
    
    /* The number of kernel capabilities in this image, and their list */
    ptr_t Kcap_Num;
    const ptr_t* Kcap;

    /* Is there any other images? If there is, here is a pointer to the start
     * of the next one. This is a constant pointer to a constant pointer to a 
     * constant structure */
    const struct RVM_Image* const * const Next_Image;
};

/* The capability structure for one VM */
struct RVM_Cap
{
    cid_t Proc;
    cid_t Captbl;
    cid_t Pgtbl;
    cid_t Int_Thd;
    cid_t User_Thd;
    cid_t Int_Sig;
    /* Thread TIDs */
    tid_t Int_TID;
    tid_t User_TID;
};

struct RVM_Sched
{
    /* The state of the virtual machine */
    ptr_t State;
    /* The current number of timeslices */
    ptr_t Slices_Left;
    /* The timeout time value */
    ptr_t Timeout;
    /* The timer interrupt period, in hypervisor ticks */
    ptr_t Period;
};
    
/* The virtual machine structure */
struct RVM_Virt
{
    /* The head to insert into the database */
    struct RVM_List Head;
    struct RVM_List Delay;
    /* The actual virtual machine information structure in the image header */
    struct RVM_Image* Image;
    /* The capabilities */
    struct RVM_Cap Cap;
    /* The scheduling related information */
    struct RVM_Sched Sched;
    /* The events */
    struct RVM_List Evt_Head;
};
/*****************************************************************************/
/* __INIT_H_STRUCTS__ */
#endif
/* __HDR_STRUCTS__ */
#endif
/* End Structs ***************************************************************/

/* Private Global Variables **************************************************/
#if(!(defined __HDR_DEFS__||defined __HDR_STRUCTS__))
#ifndef __INIT_MEMBERS__
#define __INIT_MEMBERS__

/* In this way we can use the data structures and definitions in the headers */
#define __HDR_DEFS__

#undef __HDR_DEFS__

#define __HDR_STRUCTS__

#undef __HDR_STRUCTS__

/* If the header is not used in the public mode */
#ifndef __HDR_PUBLIC_MEMBERS__
/*****************************************************************************/
/* The TID counter */
static ptr_t RVM_TID_Inc;
/* The stacks for daemons */
static ptr_t RVM_GUARD_Stack[RVM_GUARD_STACK_SIZE/sizeof(ptr_t)];
static ptr_t RVM_TIMD_Stack[RVM_TIMD_STACK_SIZE/sizeof(ptr_t)];
static ptr_t RVM_VMMD_Stack[RVM_VMMD_STACK_SIZE/sizeof(ptr_t)];
static ptr_t RVM_INTD_Stack[RVM_INTD_STACK_SIZE/sizeof(ptr_t)];
/*****************************************************************************/
/* End Private Global Variables **********************************************/

/* Private C Function Prototypes *********************************************/ 
/*****************************************************************************/
/* Initialization */
static void RVM_Clear(void* Addr, ptr_t Size);
static void RVM_VM_Cnt(ptr_t* VM_Num, ptr_t* Pgtbl_Num);
static void RVM_Load_Image(struct RVM_Image** Image, ptr_t* Pgtbl_Bump, ptr_t* Kmem_Bump, ptr_t Count);
static void RVM_VM_Init(ptr_t* Kmem_Bump);
static void RVM_Daemon_Init(ptr_t* Kmem_Bump);

/* Interrupt delivery */
void RVM_Int_Send(struct RVM_Virt* Virt, ptr_t Int_Num);
static cnt_t RVM_Int_Get(struct RVM_Flag_Set* Set);
static void RVM_Int_Proc(cnt_t Vect_Num);

/* Daemons */
static void RVM_Guard_Daemon(void);
static void RVM_Timer_Daemon(void);
static void RVM_VMM_Daemon(void);
static void RVM_Interrupt_Daemon(void);
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
/* The internal virtual machine database */
__EXTERN__ struct RVM_Virt RVM_Virt_DB[RVM_MAX_VM_NUM];
/* The number of virtual machines */
__EXTERN__ ptr_t RVM_VM_Num;
/* The current virtual machine */
__EXTERN__ struct RVM_Virt* RVM_Cur_Virt;

/* Virtual machine run queue and bitmap */
__EXTERN__ ptr_t RVM_Bitmap[RVM_PRIO_BITMAP];
__EXTERN__ struct RVM_List RVM_Run[RVM_MAX_PREEMPT_VPRIO];
/* Virtual machine waiting queue */
__EXTERN__ struct RVM_List RVM_Wait;
/* Virtual machine free queue */
__EXTERN__ struct RVM_List RVM_Free;

/* Timestamp value */
__EXTERN__ ptr_t RVM_Tick;
/* Timer wheel - This system supports about 64 VMs maximum, thus we set the timer wheel at 32 */
__EXTERN__ struct RVM_List RVM_Wheel[RVM_WHEEL_SIZE];
/*****************************************************************************/

/* End Public Global Variables ***********************************************/

/* Public C Function Prototypes **********************************************/
/*****************************************************************************/
__EXTERN__ void _RVM_Wheel_Ins(struct RVM_Virt* Virt, ptr_t Period);
/*****************************************************************************/
/* Undefine "__EXTERN__" to avoid redefinition */
#undef __EXTERN__
/* __INIT_MEMBERS__ */
#endif
/* !(defined __HDR_DEFS__||defined __HDR_STRUCTS__) */
#endif
/* End Public C Function Prototypes ******************************************/

/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
