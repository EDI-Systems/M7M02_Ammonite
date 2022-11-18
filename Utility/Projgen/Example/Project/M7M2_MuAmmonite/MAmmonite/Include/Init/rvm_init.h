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
 * 3       : Active virtual machine - interrupt thread
 * 2       : Active virtual machine - user thread
 * 1       : Init daemon
 * 0       : Inactive virtual machine
 */
/* Priority of threads */
#define RVM_SFTD_PRIO            (RVM_MAX_PREEMPT_PRIO-1)
#define RVM_TIMD_PRIO            (4)
#define RVM_VMMD_PRIO            (4)
#define RVM_VCTD_PRIO            (4)
#define RVM_VECT_PRIO            (3)
#define RVM_USER_PRIO            (2)
#define RVM_INIT_PRIO            (1)
#define RVM_WAIT_PRIO            (0)
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

/*****************************************************************************/
/* End Private Global Variables **********************************************/

/* Private C Function Prototypes *********************************************/ 
/*****************************************************************************/
/* Initialization */
static rvm_ptr_t RVM_Proc_Init(rvm_ptr_t Kmem_Bump);
static rvm_ptr_t RVM_Daemon_Init(rvm_ptr_t Cap_Front, rvm_ptr_t Kmem_Front);
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
__EXTERN__ rvm_ptr_t RVM_Sftd_Thd_Cap;
__EXTERN__ rvm_ptr_t RVM_Sftd_Sig_Cap;

#if(RVM_MAX_PREEMPT_VPRIO!=0)
__EXTERN__ rvm_ptr_t RVM_Timd_Thd_Cap;
__EXTERN__ rvm_ptr_t RVM_Vmmd_Thd_Cap;
__EXTERN__ rvm_ptr_t RVM_Vmmd_Sig_Cap;
__EXTERN__ rvm_ptr_t RVM_Vctd_Thd_Cap;
#endif
/*****************************************************************************/

/* End Public Global Variables ***********************************************/

/* Public C Function Prototypes **********************************************/
/*****************************************************************************/
__EXTERN__ void RVM_Clear(void* Addr, rvm_ptr_t Size);
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
