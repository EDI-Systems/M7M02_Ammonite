/******************************************************************************
Filename    : rvm_init.h
Author      : pry
Date        : 23/09/2017
Licence     : The Unlicense; see LICENSE for details.
Description : The header of microcontroller user-level library.
******************************************************************************/

/* Defines *******************************************************************/
#ifdef __HDR_DEFS__
#ifndef __RVM_INIT_H_DEFS__
#define __RVM_INIT_H_DEFS__
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
#define RVM_SFTD_PRIO            (RVM_PREEMPT_PRIO_NUM-1U)
#define RVM_TIMD_PRIO            (4U)
#define RVM_VMMD_PRIO            (4U)
#define RVM_VCTD_PRIO            (4U)
#define RVM_VECT_PRIO            (3U)
#define RVM_USER_PRIO            (2U)
#define RVM_INIT_PRIO            (1U)
#define RVM_WAIT_PRIO            (0U)
/*****************************************************************************/
/* __RVM_INIT_H_DEFS__ */
#endif
/* __HDR_DEFS__ */
#endif
/* End Defines ***************************************************************/

/* Structs *******************************************************************/
#ifdef __HDR_STRUCTS__
#ifndef __RVM_INIT_H_STRUCTS__
#define __RVM_INIT_H_STRUCTS__
/* We used structs in the header */

/* Use defines in these headers */
#define __HDR_DEFS__
#undef __HDR_DEFS__
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

/* Thread creation database entry */
struct RVM_Meta_Thd_Crt_Struct
{
    rvm_cid_t Main;
    rvm_cid_t Slot;
    rvm_cid_t Prc;
    rvm_ptr_t Prio;
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
struct RVM_Meta_Cpt_Kfunc_Struct
{
    rvm_cid_t Dst;
    rvm_cid_t Pos_Dst;
    rvm_ptr_t Start;
    rvm_ptr_t End;
};

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
    rvm_ptr_t Flags;
    rvm_ptr_t Pos_Src;
    rvm_ptr_t Index;
};
/*****************************************************************************/
/* __RVM_INIT_H_STRUCTS__ */
#endif
/* __HDR_STRUCTS__ */
#endif
/* End Structs ***************************************************************/

/* Private Global Variables **************************************************/
#if(!(defined __HDR_DEFS__||defined __HDR_STRUCTS__))
#ifndef __RVM_INIT_MEMBERS__
#define __RVM_INIT_MEMBERS__

/* In this way we can use the data structures and definitions in the headers */
#define __HDR_DEFS__

#undef __HDR_DEFS__

#define __HDR_STRUCTS__

#undef __HDR_STRUCTS__

/* If the header is not used in the public mode */
#ifndef __HDR_PUBLIC_MEMBERS__
/*****************************************************************************/
#if(RVM_BOOT_VEP_CRT_NUM!=0U)
EXTERN struct RVM_Meta_Main_Struct RVM_Meta_Vep_Main[];
EXTERN struct RVM_Meta_Rcv_Crt_Struct RVM_Meta_Vep_Crt[];
#endif

EXTERN struct RVM_Meta_Main_Struct RVM_Meta_Cpt_Main[];
EXTERN struct RVM_Meta_Cpt_Crt_Struct RVM_Meta_Cpt_Crt[];

EXTERN struct RVM_Meta_Main_Struct RVM_Meta_Pgt_Main[];
EXTERN struct RVM_Meta_Pgt_Crt_Struct RVM_Meta_Pgt_Crt[];
#if(RVM_BOOT_PGT_CON_NUM!=0U)
EXTERN struct RVM_Meta_Pgt_Con_Struct RVM_Meta_Pgt_Con[];
#endif
EXTERN struct RVM_Meta_Pgt_Add_Struct RVM_Meta_Pgt_Add[];

EXTERN struct RVM_Meta_Main_Struct RVM_Meta_Prc_Main[];
EXTERN struct RVM_Meta_Prc_Crt_Struct RVM_Meta_Prc_Crt[];

EXTERN struct RVM_Meta_Main_Struct RVM_Meta_Thd_Main[];
EXTERN struct RVM_Meta_Thd_Crt_Struct RVM_Meta_Thd_Crt[];

#if(RVM_BOOT_INV_CRT_NUM!=0U)
EXTERN struct RVM_Meta_Main_Struct RVM_Meta_Inv_Main[];
EXTERN struct RVM_Meta_Inv_Crt_Struct RVM_Meta_Inv_Crt[];
#endif

#if(RVM_BOOT_RCV_CRT_NUM!=0U)
EXTERN struct RVM_Meta_Main_Struct RVM_Meta_Rcv_Main[];
EXTERN struct RVM_Meta_Rcv_Crt_Struct RVM_Meta_Rcv_Crt[];
#endif

#if(RVM_BOOT_VCAP_INIT_NUM!=0U)
EXTERN struct RVM_Meta_Vcap_Init_Struct RVM_Meta_Vcap_Init[];
#endif

#if(RVM_BOOT_CPT_INIT_NUM!=0U)
EXTERN struct RVM_Meta_Cpt_Init_Struct RVM_Meta_Cpt_Init[];
#endif
#if(RVM_BOOT_CPT_KFUNC_NUM!=0U)
EXTERN struct RVM_Meta_Cpt_Kfunc_Struct RVM_Meta_Cpt_Kfunc[];
#endif

#if(RVM_VIRT_NUM!=0U)
EXTERN struct RVM_Virt_Struct RVM_Virt[];
EXTERN const struct RVM_Vmap_Struct RVM_Vmap[];
#endif
/*****************************************************************************/
/* End Private Global Variables **********************************************/

/* Private C Function Prototypes *********************************************/ 
/*****************************************************************************/
/* Creation */
static rvm_ptr_t RVM_Boot_Main_Crt(const struct RVM_Meta_Main_Struct* Meta,
                                   rvm_ptr_t Number,
                                   rvm_ptr_t Addr);
#if(RVM_BOOT_VEP_CRT_NUM!=0U)
static void RVM_Boot_Vep_Crt(void);
#endif
static void RVM_Boot_Cpt_Crt(void);
static void RVM_Boot_Pgt_Crt(void);
static void RVM_Boot_Prc_Crt(void);
static void RVM_Boot_Thd_Crt(void);
#if(RVM_BOOT_INV_CRT_NUM!=0U)
static void RVM_Boot_Inv_Crt(void);
#endif
#if(RVM_BOOT_RCV_CRT_NUM!=0U)
static void RVM_Boot_Rcv_Crt(void);
#endif

/* Initialization */
#if(RVM_BOOT_VCAP_INIT_NUM!=0U)
static void RVM_Boot_Vcap_Init(void);
#endif
static void RVM_Boot_Pgt_Init(void);
static void RVM_Boot_Cpt_Init(void);
EXTERN void RVM_Boot_Thd_Init(void);
EXTERN void RVM_Boot_Inv_Init(void);
    
static void RVM_Daemon_Init(rvm_cid_t Cap_Base,
                            rvm_ptr_t Kom_Base);
static void RVM_Prc_Init(void);

EXTERN void RVM_Boot_Pre_Init(void);
EXTERN void RVM_Boot_Post_Init(void);
/* Init daemon */
static void RVM_Init(void);
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
/* The capabilities */
__EXTERN__ rvm_cid_t RVM_Sftd_Thd_Cap;
__EXTERN__ rvm_cid_t RVM_Sftd_Sig_Cap;

#if(RVM_VIRT_NUM!=0U)
__EXTERN__ rvm_cid_t RVM_Timd_Thd_Cap;
__EXTERN__ rvm_cid_t RVM_Vmmd_Thd_Cap;
__EXTERN__ rvm_cid_t RVM_Vmmd_Sig_Cap;
__EXTERN__ rvm_cid_t RVM_Vctd_Thd_Cap;
#endif
/*****************************************************************************/

/* End Public Global Variables ***********************************************/

/* Public C Function Prototypes **********************************************/
/*****************************************************************************/
__EXTERN__ void RVM_Clear(volatile void* Addr,
                          rvm_ptr_t Size);
/*****************************************************************************/
/* Undefine "__EXTERN__" to avoid redefinition */
#undef __EXTERN__
/* __RVM_INIT_MEMBERS__ */
#endif
/* !(defined __HDR_DEFS__||defined __HDR_STRUCTS__) */
#endif
/* End Public C Function Prototypes ******************************************/

/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
