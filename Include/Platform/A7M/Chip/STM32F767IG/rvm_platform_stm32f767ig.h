/******************************************************************************
Filename   : rvm_platform_stm32f767ig.h
Author     : pry
Date       : 26/06/2017
Licence    : The Unlicense; see LICENSE for details.
Description: The configuration file for STM32F767IG. The values listed here should
             be in accordance with that in the kernel.
******************************************************************************/

/* Define ********************************************************************/
/* Debugging *****************************************************************/
#define RVM_ASSERT_ENABLE                               (0U)
#define RVM_DBGLOG_ENABLE                               (1U)
/* Kernel configuration - align with the kernel ******************************/
/* Are we using raw memory mappings? */
#define RVM_PGT_RAW_ENABLE                              (0U)
/* Kernel object virtual memory base */
#define RVM_KOM_VA_BASE                                 (0x20003000U)
/* Kernel object virtual memory size */
#define RVM_KOM_VA_SIZE                                 (0xD000U)
/* Kernel memory allocation granularity order */
#define RVM_KOM_SLOT_ORDER                              (4U)
/* The maximum number of preemption priorities */
#define RVM_PREEMPT_PRIO_NUM                            (32U)

/* Size of initial capability table */
#define RVM_INIT_CPT_SIZE                               (52U)
/* Initial kernel object frontier limit */
#define RVM_CPT_BOOT_FRONT                              (9U)
#define RVM_KOM_BOOT_FRONT                              (0x400U)
/* Post-boot kernel object frontier limit */
#define RVM_CPT_DONE_FRONT                              (18U)
#define RVM_KOM_DONE_FRONT                              (0x1200U)

/* Init process's first thread's entry point address */
#define RVM_A7M_INIT_ENTRY                              (0x08004000U|0x01U)
/* Init process's first thread's stack address */
#define RVM_A7M_INIT_STACK                              (0x2000FFF0U)
/* Number of MPU regions available */
#define RVM_A7M_REGION_NUM                              (8U)
/* What is the FPU type? */
#define RVM_COP_NUM                                     (3U)
#define RVM_A7M_COP_FPV4_SP                             (1U)
#define RVM_A7M_COP_FPV5_SP                             (1U)
#define RVM_A7M_COP_FPV5_DP                             (1U)

/* Monitor configuration *****************************************************/
/* Sleep when idle */
#define RVM_IDLE_SLEEP_ENABLE                           (1U)

/* The maximum number of VM preemption priorities */
#define RVM_PREEMPT_VPRIO_NUM                           (32U)

/* Physical vector number, flag area base and size */
#define RVM_PHYS_VCT_NUM                                (110U)
#define RVM_PHYS_VCTF_BASE                              (0x20007C00U)
#define RVM_PHYS_VCTF_SIZE                              (0x200U)
/* Virtual event number, flag area base and size */
#define RVM_VIRT_EVT_NUM                                (10U)
#define RVM_VIRT_EVTF_BASE                              (0x20007E00U)
#define RVM_VIRT_EVTF_SIZE                              (0x200U)
/* Maximum number of mappings allowed */
#define RVM_VIRT_MAP_NUM                                (128U)

/* Daemon process stack address and size, in bytes */
#define RVM_SFTD_STACK_BASE                             (0x20000000U)
#define RVM_SFTD_STACK_SIZE                             (1024U)
#define RVM_VMMD_STACK_BASE                             (0x20000000U)
#define RVM_VMMD_STACK_SIZE                             (1024U)

#define RVM_A7M_USART1_ISR                              RVM_A7M_REG(0x4001101CU)
#define RVM_A7M_USART1_TDR                              RVM_A7M_REG(0x40011028U)

#if(RVM_DBGLOG_ENABLE!=0U)
/* Print characters to console */
#define RVM_A7M_PUTCHAR(CHAR) \
do \
{ \
    RVM_A7M_USART1_TDR=(rvm_ptr_t)(CHAR); \
    while((RVM_A7M_USART1_ISR&0x40U)==0U); \
} \
while(0)
#endif
/* End Define ****************************************************************/

/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/

