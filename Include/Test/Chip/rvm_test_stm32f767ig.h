/******************************************************************************
Filename    : rvm_test_stm32f767ig.h
Author      : ryp.
Date        : 27/07/2024
License     : Unlicense; see COPYING for details.
Description : The test file for STM32F767IG.
******************************************************************************/

/* Kernel Vector Handler *****************************************************/
#ifdef RME_TEST_TIM4_HANDLER
/* Using shared memory */
#define TARGET                  *((volatile rme_u16_t*)(RME_DATA_SHARED_SHARED1_BASE))
#define START                   *((volatile rme_u16_t*)(RME_DATA_SHARED_SHARED1_BASE+RME_WORD_BYTE))

/* Hardware definitions so we don't rely on STM32 HAL */
#define TIM4_SR                 *((volatile rme_u32_t*)(0x40000810U))
#define TIM2_CNT                *((volatile rme_u32_t*)(0x40000024U))
#define RME_CNT_READ()          ((rme_u16_t)((TIM2_CNT)<<1))

    /* Clear timer flag */
    TIM4_SR=0U;
    
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
/* Using shared memory */
#define TARGET                  *((volatile rvm_u16_t*)(DATA_SHARED_SHARED1_BASE))
#define START                   *((volatile rvm_u16_t*)(DATA_SHARED_SHARED1_BASE+RVM_WORD_BYTE))

/* Counter read wrapper */
#define RVM_CNT_READ()          ((rvm_tim_t)((TIM2_CNT)<<1))
/* Vector signal endpoint */
#define SIG_VCT                 VCT_TIM4

/* Hardware definitions so we don't rely on STM32 HAL */
#define TIM2_BASE               (0x40000000U)
#define TIM4_BASE               (0x40000800U)
#define RCC_BASE                (0x40023800U)

#define RCC_APB1ENR             *((volatile rvm_u32_t*)(RCC_BASE+0x40U))
#define RCC_APB1ENR_TIM2        (1U<<0)
#define RCC_APB1ENR_TIM4        (1U<<2)

#define TIM2_CR1                *((volatile rvm_u32_t*)(TIM2_BASE+0x00U))
#define TIM2_ARR                *((volatile rvm_u32_t*)(TIM2_BASE+0x2CU))
#define TIM2_PSC                *((volatile rvm_u32_t*)(TIM2_BASE+0x28U))
#define TIM2_CNT                *((volatile rvm_u32_t*)(TIM2_BASE+0x24U))

#define TIM4_CR1                *((volatile rvm_u32_t*)(TIM4_BASE+0x00U))
#define TIM4_ARR                *((volatile rvm_u32_t*)(TIM4_BASE+0x2CU))
#define TIM4_PSC                *((volatile rvm_u32_t*)(TIM4_BASE+0x28U))
#define TIM4_DIER               *((volatile rvm_u32_t*)(TIM4_BASE+0x0CU))
#define TIM4_SR                 *((volatile rvm_u32_t*)(TIM4_BASE+0x10U))
    
#define TIM_CR1_CEN             (1U<<0)
#define TIM_COUNTERMODE_UP      (0U)
#define TIM_COUNTERMODE_DOWN    (1U<<4)
#define TIM_CLOCKDIVISION_DIV1  (0U)

/* Timer initialization */
#define RVM_TIM_INIT() \
do \
{ \
    /* TIM2 clock = 1/2 CPU clock */ \
    TIM2_CR1=TIM_COUNTERMODE_UP|TIM_CLOCKDIVISION_DIV1; \
    TIM2_ARR=(unsigned int)(-1); \
    TIM2_PSC=0U; \
    RCC_APB1ENR|=RCC_APB1ENR_TIM2; \
    TIM2_CR1|=TIM_CR1_CEN; \
} \
while(0)

/* Interrupt source initialization */
#define RVM_INT_INIT() \
do \
{ \
    /* Set the priority of the physical interrupt and enable it */ \
    RVM_ASSERT(RVM_A7M_Int_Local_Mod(KFN_INT_LOCAL_MOD,30U, \
                                     RVM_A7M_KFN_INT_LOCAL_MOD_STATE_SET,1U)==0); \
    RVM_ASSERT(RVM_A7M_Int_Local_Mod(KFN_INT_LOCAL_MOD,30U, \
                                     RVM_A7M_KFN_INT_LOCAL_MOD_PRIO_SET,0xFFU)==0); \
    \
    /* TIM4 clock = 1/2 CPU clock */ \
    TIM4_CR1=TIM_COUNTERMODE_DOWN|TIM_CLOCKDIVISION_DIV1; \
    TIM4_ARR=21600U; \
    TIM4_PSC=0U; \
    RCC_APB1ENR|=RCC_APB1ENR_TIM4; \
    TIM4_SR&=~0x01U; \
    TIM4_DIER|=0x01U; \
    TIM4_CR1|=TIM_CR1_CEN; \
} \
while(0)
    
/* Interrupt source disabling */
#define RVM_INT_DISABLE() \
do \
{ \
    TIM4_CR1=0U; \
    TIM4_SR=0U; \
    TIM4_DIER=0U; \
    \
    /* Set the priority of the physical interrupt and enable it */ \
    RVM_ASSERT(RVM_A7M_Int_Local_Mod(KFN_INT_LOCAL_MOD,30U, \
                                     RVM_A7M_KFN_INT_LOCAL_MOD_STATE_SET,0U)==0); \
} \
while(0)
#endif
/* End Native Include ********************************************************/

/* Native Putchar ************************************************************/
#ifdef RVM_TEST_NATIVE_PUTCHAR
/* Hardware definitions so we don't rely on STM32 HAL */
#define USART1_ISR              *((volatile rvm_u32_t*)(0x4001101CU))
#define USART1_TDR              *((volatile rvm_u32_t*)(0x40011028U))

    USART1_TDR=(rvm_ptr_t)(Char);
    while((USART1_ISR&0x40U)==0U);
#endif
/* End Native Putchar ********************************************************/

/* Virtual Include ***********************************************************/
#ifdef RVM_TEST_VIRTUAL_INC
/* Using shared memory */
#define TARGET                  *((volatile rvm_u16_t*)(DATA_SHARED_SHARED1_BASE))
#define START                   *((volatile rvm_u16_t*)(DATA_SHARED_SHARED1_BASE+RVM_WORD_BYTE))
#endif
/* End Virtual Include *******************************************************/

/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/

