/******************************************************************************
Filename   : syslib_STM32F405RG.h
Author     : pry
Date       : 26/06/2017
Licence    : LGPL v3+; see COPYING for details.
Description: The configuration file for STM32F767IG. The values listed here should
             be in accordance with that in the kernel.
******************************************************************************/

/* Defines *******************************************************************/
/* Kernel configurations - keep the same with the kernel *********************/
/* The virtual memory start address for the kernel objects */
#define RME_KMEM_VA_START            0x10002000
/* The size of the kernel object virtual memory */
#define RME_KMEM_SIZE                0x6000
/* The granularity of kernel memory allocation, order of 2 in bytes */
#define RME_KMEM_SLOT_ORDER          4
/* Kernel stack size and address */
#define RME_KMEM_STACK_ADDR          0x10000FF0
/* The maximum number of preemption priority levels in the system.
 * This parameter must be divisible by the word length - 32 is usually sufficient */
#define RME_MAX_PREEMPT_PRIO         32

/* Initial kenel object frontier limit */
#define RME_CMX_KMEM_BOOT_FRONTIER   0x10003000
/* Number of MPU regions available */
#define RME_CMX_MPU_REGIONS          8

/* Init process's first thread's entry point address */
#define RME_CMX_INIT_ENTRY           (0x08004000|0x01)
/* Init process's first thread's stack address */
#define RME_CMX_INIT_STACK           0x1000FFF0
/* The first user process's image header position */
#define RME_CMX_FIRST_HEADER         0x08020000

/* What is the FPU type? */
#define RME_CMX_FPU_TYPE             RME_CMX_VFPV4
/* Syslib configurations *****************************************************/
/* The assert macro */
#define RME_ASSERT(X)                while((X)==0);
/* Config settings - modifications no needed for most cases */
/* The size of process-related capability table */
#define RME_CAPTBL_PROC_SIZE         64
/* The size of thread/signal capability table */
#define RME_CAPTBL_THD_SIG_SIZE      64
/* The size of invocation capability table */
#define RME_CAPTBL_INV_SIZE          64
/* End Defines ***************************************************************/

/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
