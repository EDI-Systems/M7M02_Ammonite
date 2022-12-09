/******************************************************************************
Filename   : rvm_platform_stm32f405rg.h
Author     : pry
Date       : 26/06/2017
Licence    : The Unlicense; see LICENSE for details.
Description: The configuration file for STM32F405RG. The values listed here should
             be in accordance with that in the kernel.
******************************************************************************/

/* Defines *******************************************************************/
/* The generator must be enabled to use this */
/* Kernel configurations - keep the same with the kernel *********************/
/* The virtual memory start address for the kernel objects */
#define RVM_KMEM_VA_BASE                                (0x10002000U)
/* The size of the kernel object virtual memory */
#define RVM_KMEM_VA_SIZE                                (0x6000U)
/* The granularity of kernel memory allocation, order of 2 in bytes */
#define RVM_KMEM_SLOT_ORDER                             (4U)
/* The maximum number of preemption priority levels in the system.
 * This parameter must be divisible by the word length - 32 is usually sufficient */
#define RVM_PREEMPT_PRIO_NUM                            (32U)
/* Number of virtual priorities in the system */
#define RVM_PREEMPT_VPRIO_NUM                           (32U)

/* Physical vector number, flag area base and its size */
#define RVM_PHYS_VECT_NUM                               (82U)
#define RVM_PHYS_VECT_BASE                              (0x10007C00U)
#define RVM_PHYS_VECT_SIZE                              (0x200U)
/* Virtual event number, flag area base and its size */
#define RVM_VIRT_EVENT_NUM                              (10U)
#define RVM_VIRT_EVENT_BASE                             (0x10007E00U)
#define RVM_VIRT_EVENT_SIZE                             (0x200U)
/* Maximum number of mappings allowed */
#define RVM_VIRT_MAP_NUM                                (64U)

/* Initial kernel object capability limit */
#define RVM_CAP_BOOT_FRONTIER                           (9U)
/* Initial kernel object memory limit */
#define RVM_KMEM_BOOT_FRONTIER                          (0x400U)

/* Init process's first thread's entry point address */
#define RVM_A7M_INIT_ENTRY                              (0x08004000U|0x01U)
/* Init process's first thread's stack address */
#define RVM_A7M_INIT_STACK                              (0x1000FFF0U)
/* Number of MPU regions available */
#define RVM_A7M_MPU_REGIONS                             (8U)
/* What is the FPU type? */
#define RVM_A7M_FPU_TYPE                                (RVM_A7M_FPU_FPV4_SP)

/* Fixed info ****************************************************************/

/* Syslib configurations *****************************************************/
/* Stack redundancy */
#define RVM_STACK_SAFE_RDCY                             (0x10U)
/* Daemon process stack address and size, in bytes */
#define RVM_SFTD_STACK_BASE                             (0x20000000U)
#define RVM_SFTD_STACK_SIZE                             (1024U)
#define RVM_TIMD_STACK_BASE                             (0x20000000U)
#define RVM_TIMD_STACK_SIZE                             (1024U)
#define RVM_VMMD_STACK_BASE                             (0x20000000U)
#define RVM_VMMD_STACK_SIZE                             (1024U)
#define RVM_VCTD_STACK_BASE                             (0x20000000U)
#define RVM_VCTD_STACK_SIZE                             (1024U)

/* Is debugging output enabled? */
#define RVM_DEBUG_LOG                                   (1U)

#define RVM_A7M_USART1_SR                               RVM_A7M_REG(0x40011000U)
#define RVM_A7M_USART1_DR                               RVM_A7M_REG(0x40011004U)

/* Print characters to console */
#define RVM_A7M_PUTCHAR(CHAR) \
do \
{ \
    RVM_A7M_USART1_DR=(rvm_ptr_t)(CHAR); \
    while((RVM_A7M_USART1_SR&0x80U)==0U); \
} \
while(0U)
/* End Defines ***************************************************************/

/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/

