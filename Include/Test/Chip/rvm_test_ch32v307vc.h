/******************************************************************************
Filename    : rvm_test_ch32v307vc.h
Author      : ryp.
Date        : 27/07/2024
License     : Unlicense; see COPYING for details.
Description : The test file for CH32V307VC.

GCC 13.2.1 -O3 (SysTick turned on, w/FPU context)
    ___  _   __ __  ___
   / _ \| | / //  |/  /    Feather-weight hypervisor
  / , _/| |/ // /|_/ /      Standard benchmark test
 /_/|_| |___//_/  /_/
====================================================
Test (number in CPU cycles)        : AVG / MAX / MIN
Yield (self, one-way)              : 362 / 373 / 362
Signal (self)                      : 628 / 657 / 628
Invocation (round-trip)            : 746 / 787 / 746
Vector                             : 612 / 656 / 521
Signal (intra)                     : 789 / 810 / 789
Signal (inter)                     : 812 / 831 / 812
Yield (inter, one-way)             : 748 / 769 / 748
******************************************************************************/

/* Kernel Vector Handler *****************************************************/
#ifdef RME_TEST_TIM4_HANDLER
/* Using backup data register - not enough regions available */
#define TARGET                  *((volatile rme_u16_t*)(0x40006C04U))
#define START                   *((volatile rme_u16_t*)(0x40006C08U))

/* Hardware definitions so we don't rely on WCH HAL */
#define TIM4_INTFR              *((volatile rme_u16_t*)(0x40000810U))
#define TIM1_CNT                *((volatile rme_u16_t*)(0x40012C24U))
#define RME_CNT_READ()          (TIM1_CNT)

    /* Clear timer flag */
    TIM4_INTFR=0U;
    
    /* Where to send this depends on what we are testing - kernel or VM? */
    START=RME_CNT_READ();
    if(TARGET==0U)
        return RME_RVM_VCT_SIG_SELF;
    else
        return RME_RVM_VCT_SIG_INIT;
#endif
/* End Kernel Vector Handler *************************************************/

/* Native Include ************************************************************/
#ifdef RVM_TEST_NATIVE_INC
/* Using backup data register - not enough regions available */
#define TARGET                  *((volatile rvm_u16_t*)(0x40006C04U))
#define START                   *((volatile rvm_u16_t*)(0x40006C08U))

/* Counter read wrapper */
#define RVM_CNT_READ()          TIM1_CNT
/* Vector signal endpoint */
#define SIG_VCT                 VCT_TIM4

/* Hardware definitions so we don't rely on WCH HAL */
#define TIM1_BASE               (0x40012C00U)
#define TIM4_BASE               (0x40000800U)
#define RCC_BASE                (0x40021000U)

#define RCC_APB2PCENR           *((volatile rvm_u32_t*)(RCC_BASE+0x18U))
#define RCC_APB2PCENR_TIM1      (1U<<11)

#define RCC_APB1PCENR           *((volatile rvm_u32_t*)(RCC_BASE+0x1CU))
#define RCC_APB1PCENR_TIM4      (1U<<2)
#define RCC_APB1PCENR_BKP       (1U<<27)
#define RCC_APB1PCENR_PWR       (1U<<28)

#define PWR_CTLR                *((volatile rvm_u32_t*)(0x40007000))
#define PWR_CTLR_DBP            (1U<<8)

#define TIM1_CTLR1              *((volatile rvm_u16_t*)(TIM1_BASE+0x00U))
#define TIM1_SMCFGR             *((volatile rvm_u16_t*)(TIM1_BASE+0x08U))
#define TIM1_CNT                *((volatile rvm_u16_t*)(TIM1_BASE+0x24U))

#define TIM4_CTLR1              *((volatile rvm_u16_t*)(TIM4_BASE+0x00U))
#define TIM4_ATRLR              *((volatile rvm_u16_t*)(TIM4_BASE+0x2CU))
#define TIM4_PSC                *((volatile rvm_u16_t*)(TIM4_BASE+0x28U))
#define TIM4_DMAINTENR          *((volatile rvm_u16_t*)(TIM4_BASE+0x0CU))
#define TIM4_INTFR              *((volatile rvm_u16_t*)(TIM4_BASE+0x10U))
    
#define TIM_CTLR1_CEN           (1U<<0)
#define TIM_COUNTERMODE_UP      (0U)
#define TIM_COUNTERMODE_DOWN    (1U<<4)
#define TIM_CKD_DIV1            (0U)
#define TIM_IT_UPDATE           (1U<<0)

/* Timer initialization */
#define RVM_TIM_INIT() \
do \
{ \
    /* TIM1 clock = CPU clock */ \
    RCC_APB2PCENR|=RCC_APB2PCENR_TIM1; \
    TIM1_CTLR1=TIM_COUNTERMODE_UP; \
    TIM1_SMCFGR=0U; \
    TIM1_CTLR1|=TIM_CTLR1_CEN; \
    /* Enable write to shared backup area as well */ \
    RCC_APB1PCENR|=RCC_APB1PCENR_BKP|RCC_APB1PCENR_PWR; \
    PWR_CTLR|=PWR_CTLR_DBP; \
} \
while(0)

/* Interrupt source initialization */
#define RVM_INT_INIT() \
do \
{ \
    /* Set the priority of the physical interrupt and enable it */ \
    RVM_ASSERT(RVM_RV32P_Int_Local_Mod(KFN_INT_LOCAL_MOD,46U, \
                                       RVM_RV32P_KFN_INT_LOCAL_MOD_STATE_SET,1U)==0); \
    RVM_ASSERT(RVM_RV32P_Int_Local_Mod(KFN_INT_LOCAL_MOD,46U, \
                                       RVM_RV32P_KFN_INT_LOCAL_MOD_PRIO_SET,0xFFU)==0); \
    \
    /* TIM4 clock = CPU clock */ \
    RCC_APB1PCENR|=RCC_APB1PCENR_TIM4; \
    TIM4_PSC=0U; \
    TIM4_ATRLR=14400U*4U; \
    TIM4_CTLR1=TIM_COUNTERMODE_DOWN|TIM_CKD_DIV1; \
    TIM4_INTFR=0U; \
    TIM4_DMAINTENR=TIM_IT_UPDATE; \
    TIM4_CTLR1|=TIM_CTLR1_CEN; \
} \
while(0)
    
/* Interrupt source disabling */
#define RVM_INT_DISABLE() \
do \
{ \
    TIM4_CTLR1=0U; \
    TIM4_INTFR=0U; \
    TIM4_DMAINTENR=0U; \
    \
    /* Disable interrupt */ \
    RVM_ASSERT(RVM_RV32P_Int_Local_Mod(KFN_INT_LOCAL_MOD,46U, \
                                       RVM_RV32P_KFN_INT_LOCAL_MOD_STATE_SET,0U)==0); \
} \
while(0)
#endif
/* End Native Include ********************************************************/

/* Native Putchar ************************************************************/
#ifdef RVM_TEST_NATIVE_PUTCHAR
/* Hardware definitions so we don't rely on WCH HAL */
#define USART1_STATR            *((volatile rvm_u32_t*)(0x40013800U))
#define USART1_STATR_TC         (0x00000040U)
#define USART1_DATAR            *((volatile rvm_u32_t*)(0x40013804U))

    while((USART1_STATR&USART1_STATR_TC)==0U);
    USART1_DATAR=(rvm_ptr_t)(Char);
#endif
/* End Native Putchar ********************************************************/

/* Virtual Include ***********************************************************/
#ifdef RVM_TEST_VIRTUAL_INC
/* Using backup data register - not enough regions available */
#define TARGET                  *((volatile rvm_u16_t*)(0x40006C04U))
#define START                   *((volatile rvm_u16_t*)(0x40006C08U))
#endif
/* End Virtual Include *******************************************************/

/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/

