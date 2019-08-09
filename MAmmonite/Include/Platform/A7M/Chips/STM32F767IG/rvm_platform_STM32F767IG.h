/******************************************************************************
Filename   : rvm_platform_STM32F767IG.h
Author     : pry
Date       : 26/06/2017
Licence    : LGPL v3+; see COPYING for details.
Description: The configuration file for STM32F767IG. The values listed here should
             be in accordance with that in the kernel.
******************************************************************************/

/* Defines *******************************************************************/
/* The HAL library */
#include "stm32f7xx.h"
#include "core_cm7.h"
/* Kernel configurations - keep the same with the kernel *********************/
/* The virtual memory start address for the kernel objects */
#define RVM_KMEM_VA_START            0x20003000
/* The size of the kernel object virtual memory */
#define RVM_KMEM_SIZE                0xD000
/* The granularity of kernel memory allocation, order of 2 in bytes */
#define RVM_KMEM_SLOT_ORDER          4
/* The maximum number of preemption priority levels in the system.
 * This parameter must be divisible by the word length - 32 is usually sufficient */
#define RVM_MAX_PREEMPT_PRIO         32

/* Initial kernel object frontier limit */
#define RVM_A7M_KMEM_BOOT_FRONTIER   0x400
/* Number of MPU regions available */
#define RVM_A7M_MPU_REGIONS          8
/* Init process's first thread's entry point address */
#define RVM_A7M_INIT_ENTRY           (0x08004000|0x01)
/* Init process's first thread's stack address */
#define RVM_A7M_INIT_STACK           0x2001FFF0
/* What is the FPU type? */
#define RVM_A7M_FPU_TYPE             RVM_A7M_FPV5_DP
/* Syslib configurations *****************************************************/
/* The priority levels (assuming that the kernel have 32 levels):
 * 31      : Guard daemon - fixed
 * 30 - 5  : Handler threads
 * 4       : Timer daemon (adjustable from 29 to 4)
 *           Virtual machine monitor daemon
 *           Interrupt daemon
 * 3       : Active virtual machine - interrupt thread
 * 2       : Active virtual machine - user thread
 * 1       : Init daemon
 * 0       : Inactive virtual machine
 */
#define RVM_VMD_PRIO                 4
/* The user image header starting position */
#define RVM_IMAGE_HEADER_START       0x08020000

/* The stack size of the guard daemon process, in bytes */
#define RVM_GUARD_STACK_SIZE         2048
/* The stack size of the timer daemon process, in bytes */
#define RVM_TIMD_STACK_SIZE          2048
/* The stack size of the virtual machine monitor daemon process, in bytes */
#define RVM_VMMD_STACK_SIZE          2048
/* The stack size of the interrupt daemon process, in bytes */
#define RVM_INTD_STACK_SIZE          2048

/* Config settings - modifications not needed for most cases */
/* How many VMs are allowed? */
#define RVM_MAX_VM_NUM               16
/* How many different virtual machine priorities are allowed? */
#define RVM_MAX_PREEMPT_VPRIO        32

/* Shared interrupt flag region address - this will only appear on MPU-based systems */
#define RVM_INT_FLAG_ADDR            0x20010000
/* How many interrupts are there in the hardware */
#define RVM_INT_VECT_NUM             110
/* The maximum of interrupt mappings in the system */
#define RVM_INT_MAP_NUM              1024
/* The maximum number of event mappings in the system */
#define RVM_EVT_MAP_NUM              1024

/* Maximum time allowed to set as period */
#define RVM_MAX_PERIOD               10000
/* Minimum time allowed to set as period */
#define RVM_MIN_PERIOD               1
/* The default value for timer interrupt frequency for a VM */
#define RVM_DEF_PERIOD               1

/* Is debugging output enabled? */
#define RVM_DEBUG_LOG                RVM_TRUE
/* End Defines ***************************************************************/

/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
