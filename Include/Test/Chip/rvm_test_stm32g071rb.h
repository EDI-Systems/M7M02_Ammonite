/******************************************************************************
Filename    : rvm_test_stm32g071rb.h
Author      : ryp.
Date        : 27/07/2024
License     : Unlicense; see COPYING for details.
Description : The test file for STM32G071RB.

GCC 9.3.1 -Os (SysTick turned on)
    ___   __  ___ ___
   / _ \ /  |/  // _ \       Simple real-time kernel
  / , _// /|_/ // ___/       Standard benchmark test
 /_/|_|/_/  /_//_/
====================================================
Test (number in CPU cycles)        : AVG / MAX / MIN
Yield (self, one-way)              : 552 / 889 / 552
Signal (self)                      : 838 / 1174 / 838
Invocation (round-trip)            : 983 / 1322 / 983
Vector                             : 638 / 956 / 638
Signal (intra)                     : 823 / 1157 / 822
Signal (inter)                     : 916 / 1252 / 916
Yield (inter, one-way)             : 854 / 1190 / 854

ARMCC 6.23 -Oz (SysTick turned on)
    ___   __  ___ ___
   / _ \ /  |/  // _ \       Simple real-time kernel
  / , _// /|_/ // ___/       Standard benchmark test
 /_/|_|/_/  /_//_/
====================================================
Test (number in CPU cycles)        : AVG / MAX / MIN
Yield (self, one-way)              : 521 / 830 / 521
Signal (self)                      : 844 / 1153 / 844
Invocation (round-trip)            : 1000 / 1328 / 1000
Vector                             : 582 / 891 / 582
Signal (intra)                     : 774 / 1083 / 774
Signal (inter)                     : 874 / 1183 / 874
Yield (inter, one-way)             : 805 / 1114 / 805
******************************************************************************/

/* Kernel Vector Handler *****************************************************/
#ifdef RME_TEST_TIM3_HANDLER
/* Using shared memory */
#define TARGET                  *((volatile rme_u16_t*)(RME_DATA_SHARED_SHARED1_BASE))
#define START                   *((volatile rme_u16_t*)(RME_DATA_SHARED_SHARED1_BASE+RME_WORD_BYTE))

/* Hardware definitions so we don't rely on STM32 HAL */
#define TIM3_SR                *((volatile rme_u32_t*)(0x40000410U))
#define TIM2_CNT                *((volatile rme_u32_t*)(0x40000024U))
#define RME_CNT_READ()          ((rme_u16_t)(TIM2_CNT))

    /* Clear timer flag */
    TIM3_SR=0U;
    
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
#define RVM_CNT_READ()          ((rvm_tim_t)(TIM2_CNT))
/* Vector signal endpoint */
#define SIG_VCT                 VCT_TIM3

/* Hardware definitions so we don't rely on STM32 HAL */
#define TIM2_BASE               (0x40000000U)
#define TIM3_BASE              (0x40000400U)
#define RCC_BASE                (0x40021000U)

#define RCC_APBENR1             *((volatile rvm_u32_t*)(RCC_BASE+0x3CU))
#define RCC_APBENR1             *((volatile rvm_u32_t*)(RCC_BASE+0x3CU))
#define RCC_APBENR1_TIM2        (1U<<0)
#define RCC_APBENR1_TIM3        (1U<<1)

#define TIM2_CR1                *((volatile rvm_u32_t*)(TIM2_BASE+0x00U))
#define TIM2_ARR                *((volatile rvm_u32_t*)(TIM2_BASE+0x2CU))
#define TIM2_PSC                *((volatile rvm_u32_t*)(TIM2_BASE+0x28U))
#define TIM2_CNT                *((volatile rvm_u32_t*)(TIM2_BASE+0x24U))

#define TIM3_CR1               *((volatile rvm_u32_t*)(TIM3_BASE+0x00U))
#define TIM3_ARR               *((volatile rvm_u32_t*)(TIM3_BASE+0x2CU))
#define TIM3_PSC               *((volatile rvm_u32_t*)(TIM3_BASE+0x28U))
#define TIM3_DIER              *((volatile rvm_u32_t*)(TIM3_BASE+0x0CU))
#define TIM3_SR                *((volatile rvm_u32_t*)(TIM3_BASE+0x10U))
    
#define TIM_CR1_CEN             (1U<<0)
#define TIM_COUNTERMODE_UP      (0U)
#define TIM_COUNTERMODE_DOWN    (1U<<4)
#define TIM_CLOCKDIVISION_DIV1  (0U)

/* Timer initialization */
#define RVM_TIM_INIT() \
do \
{ \
    /* TIM2 clock = CPU clock */ \
    TIM2_CR1=TIM_COUNTERMODE_UP|TIM_CLOCKDIVISION_DIV1; \
    TIM2_ARR=(unsigned int)(-1); \
    TIM2_PSC=0U; \
    RCC_APBENR1|=RCC_APBENR1_TIM2; \
    TIM2_CR1|=TIM_CR1_CEN; \
} \
while(0)

/* Interrupt source initialization */
#define RVM_INT_INIT() \
do \
{ \
    /* Set the priority of the physical interrupt and enable it */ \
    RVM_ASSERT(RVM_A6M_Int_Local_Mod(KFN_INT_LOCAL_MOD,16U, \
                                     RVM_A6M_KFN_INT_LOCAL_MOD_STATE_SET,1U)==0); \
    RVM_ASSERT(RVM_A6M_Int_Local_Mod(KFN_INT_LOCAL_MOD,16U, \
                                     RVM_A6M_KFN_INT_LOCAL_MOD_PRIO_SET,0xFFU)==0); \
    \
    /* TIM3 clock = CPU clock */ \
    TIM3_CR1=TIM_COUNTERMODE_DOWN|TIM_CLOCKDIVISION_DIV1; \
    TIM3_ARR=12800U; \
    TIM3_PSC=0U; \
    RCC_APBENR1|=RCC_APBENR1_TIM3; \
    TIM3_SR&=~0x01U; \
    TIM3_DIER|=0x01U; \
    TIM3_CR1|=TIM_CR1_CEN; \
} \
while(0)
    
/* Interrupt source disabling */
#define RVM_INT_DISABLE() \
do \
{ \
    TIM3_CR1=0U; \
    TIM3_SR=0U; \
    TIM3_DIER=0U; \
    \
    /* Set the priority of the physical interrupt and enable it */ \
    RVM_ASSERT(RVM_A6M_Int_Local_Mod(KFN_INT_LOCAL_MOD,16U, \
                                     RVM_A6M_KFN_INT_LOCAL_MOD_STATE_SET,0U)==0); \
} \
while(0)
#endif
/* End Native Include ********************************************************/

/* Native Putchar ************************************************************/
#ifdef RVM_TEST_NATIVE_PUTCHAR
/* Hardware definitions so we don't rely on STM32 HAL */
#define USART2_ISR              *((volatile rvm_u32_t*)(0x4000441CU))
#define USART2_TDR              *((volatile rvm_u32_t*)(0x40004428U))

    USART2_TDR=(rvm_ptr_t)(Char);
    while((USART2_ISR&0x80U)==0U);
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

