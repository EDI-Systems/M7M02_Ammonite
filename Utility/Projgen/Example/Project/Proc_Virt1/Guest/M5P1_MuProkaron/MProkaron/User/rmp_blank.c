/******************************************************************************
Filename    : rmp_blank.c
Author      : pry 
Date        : 08/09/2019
Licence     : The Unlicense; see LICENSE for details.
Description : The blank project template file for RMP under RVM hypervisor.
******************************************************************************/

/* Includes ******************************************************************/
#include "rmp.h"
#include "rvm_guest.h"
/* End Includes **************************************************************/
#define RVM_A7M_REG(X)                          (*((volatile rvm_ptr_t*)(X)))
#define RVM_A7M_USART1_DR                               RVM_A7M_REG(0x40011000+0x04)
#define RVM_A7M_USART1_SR                               RVM_A7M_REG(0x40011000+0x00)

/* Print characters to console */
#define RVM_A7M_PUTCHAR(CHAR) \
do \
{ \
    RVM_A7M_USART1_DR=(CHAR); \
    while((RVM_A7M_USART1_SR&0x80)==0); \
} \
while(0)
    

volatile struct RMP_Thd Thd_1;

#define THD1_STACK        (&Stack_1[200])
rmp_ptr_t Stack_1[256];

/* Begin Function:RMP_Init ****************************************************
Description : The init thread hook functions.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
void Func_1(void)
{
    while(1)
    {
        RMP_Thd_Delay(10);
        RVM_A7M_PUTCHAR('s');
    }
}

void RMP_Init_Hook(void)
{

    RMP_Clear(&Thd_1,sizeof(struct RMP_Thd));
    RMP_Thd_Crt(&Thd_1, Func_1, THD1_STACK, (void*)0x1234, 1, 5);
}

void RMP_Init_Idle(void)
{
    RVM_Hyp_Wait_Vect();
}
/* End Function:RMP_Init *****************************************************/

/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/

