/******************************************************************************
Filename    : hyper.h
Author      : pry
Date        : 18/10/2017
Licence     : LGPL v3+; see COPYING for details.
Description : The header of microcontroller user-level library.
******************************************************************************/

/* Defines *******************************************************************/
#ifdef __HDR_DEFS__
#ifndef __HYPER_H_DEFS__
#define __HYPER_H_DEFS__
/*****************************************************************************/
/* Hypercalls */
/* Enable interrupts */
#define RVM_HYP_ENAINT           0
/* Disable interrupts */
#define RVM_HYP_DISINT           1
/* Register a vector */
#define RVM_HYP_REGEVT           2
/* Delete a vector */
#define RVM_HYP_DELEVT           3
/* Wait for an interrupt to come */
#define RVM_HYP_WAITEVT          4
/* Trigger a vector */
#define RVM_HYP_SENDEVT          5
/* Returning from a vector */
#define RVM_HYP_QUERYEVT         6
/* Query the VM's ID given its name */
#define RVM_HYP_QUERY            7
/* Send to another VM */
#define RVM_HYP_TIMPROG          8
/* Print to the console */
#define RVM_HYP_PRINT            9

/* Error codes */
/* The state is wrong */
#define RVM_ERR_STATE            (-1)
/* The number is overrange */
#define RVM_ERR_RANGE            (-2)
/* The error is address related */
#define RVM_ERR_ADDR             (-3)
/* The error is event related */
#define RVM_ERR_EVENT            (-4)

/* Get VM ID or event ID from their address */
#define RVM_VMID(ADDR)           ((((ptr_t)(ADDR))-((ptr_t)RVM_Virt_DB))/sizeof(struct RVM_Virt))
#define RVM_EVTID(ADDR)          ((((ptr_t)(ADDR))-((ptr_t)RVM_Evt_DB))/sizeof(struct RVM_Evt))
    
/* Event states */
#define RVM_EVT_FREE             0
#define RVM_EVT_USED             1
/*****************************************************************************/
/* __HYPER_H_DEFS__ */
#endif
/* __HDR_DEFS__ */
#endif
/* End Defines ***************************************************************/

/* Structs *******************************************************************/
#ifdef __HDR_STRUCTS__
#ifndef __HYPER_H_STRUCTS__
#define __HYPER_H_STRUCTS__

/* Use defines in these headers */
#define __HDR_DEFS__
#undef __HDR_DEFS__
/*****************************************************************************/
struct RVM_Evt
{
    struct RVM_List Head;
    ptr_t State;
    ptr_t Int_Num;
    ptr_t Snd_VM_ID;
    ptr_t Rcv_VM_ID;
};
/* The parameter structure */
struct RVM_Param
{
    ptr_t Number;
    ptr_t Param[4];
};
/*****************************************************************************/
/* __HYPER_H_STRUCTS__ */
#endif
/* __HDR_STRUCTS__ */
#endif
/* End Structs ***************************************************************/

/* Private Global Variables **************************************************/
#if(!(defined __HDR_DEFS__||defined __HDR_STRUCTS__))
#ifndef __HYPER_MEMBERS__
#define __HYPER_MEMBERS__

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
__EXTERN__ struct RVM_List RVM_Evt_Free;
__EXTERN__ struct RVM_Evt RVM_Evt_DB[RVM_MAX_EVT_NUM];
/*****************************************************************************/

/* End Public Global Variables ***********************************************/

/* Public C Function Prototypes **********************************************/
/*****************************************************************************/
/* Helper functions */
__EXTERN__ ret_t _RVM_Strncmp(const s8* Str1, const s8* Str2, ptr_t Num);
__EXTERN__ void _RVM_Set_Rdy(struct RVM_Virt* Virt);
__EXTERN__ void _RVM_Clr_Rdy(struct RVM_Virt* Virt);
__EXTERN__ struct RVM_Virt* _RVM_Get_High_Rdy(void);
__EXTERN__ void _RVM_Virt_Switch(struct RVM_Virt* From, struct RVM_Virt* To);
__EXTERN__ ret_t _RVM_Check_Int_Pend(struct RVM_Virt* Virt);
__EXTERN__ void _RVM_Set_Int_Flag(struct RVM_Virt* Virt, ptr_t Int_Num);
__EXTERN__ void _RVM_Clr_Int_Flag(struct RVM_Virt* Virt, ptr_t Int_Num);

__EXTERN__ ret_t RVM_Hyp_Enable_Int(void);
__EXTERN__ ret_t RVM_Hyp_Disable_Int(void);
__EXTERN__ ret_t RVM_Hyp_Reg_Evt(ptr_t Int_Num, ptr_t VMID);
__EXTERN__ ret_t RVM_Hyp_Del_Evt(cnt_t Evt_ID);
__EXTERN__ ret_t RVM_Hyp_Wait_Evt(void);
__EXTERN__ ret_t RVM_Hyp_Send_Evt(ptr_t Evt_ID);
__EXTERN__ ret_t RVM_Hyp_Query_Evt(ptr_t VMID);
__EXTERN__ ret_t RVM_Hyp_Query(s8* Name);
__EXTERN__ ret_t RVM_Hyp_Tim_Prog(ptr_t Period);
__EXTERN__ ret_t RVM_Hyp_Print(void);
/*****************************************************************************/
/* Undefine "__EXTERN__" to avoid redefinition */
#undef __EXTERN__
/* __HYPER_MEMBERS__ */
#endif
/* !(defined __HDR_DEFS__||defined __HDR_STRUCTS__) */
#endif
/* End Public C Function Prototypes ******************************************/

/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
