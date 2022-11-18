/******************************************************************************
Filename    : rmp_blank.c
Author      : pry 
Date        : 08/09/2019
Licence     : The Unlicense; see LICENSE for details.
Description : The blank project template file for RMP.
******************************************************************************/

/* Includes ******************************************************************/
#include "rmp.h"
#include "rvm.h"
#include "rvm_guest_a7m.h"
#include "rvm_guest.h"
/* End Includes **************************************************************/

#define RVM_A7M_REG(X)                          (*((volatile rvm_ptr_t*)(X)))
#define RVM_A7M_REGB(X)                         (*((volatile rvm_u8_t*)(X)))

#define RVM_A7M_USART1_TDR                              RVM_A7M_REG(0x40011000+0x28)
#define RVM_A7M_USART1_ISR                              RVM_A7M_REG(0x40011000+0x1C)

/* Print characters to console */
#define RVM_A7M_PUTCHAR(CHAR) \
do \
{ \
    RVM_A7M_USART1_TDR=(CHAR); \
    while((RVM_A7M_USART1_ISR&0x40)==0); \
} \
while(0)
/* Begin Function:RMP_Init ****************************************************
Description : The init thread hook functions.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
rmp_ptr_t Send;

void Handler(void)
{
    Send=1;
    RVM_A7M_PUTCHAR('5');
}

void RMP_Init_Hook(void)
{
    Send=0;
    RVM_Virt_Reg_Vect(0, Handler);
    RVM_Hyp_Reg_Evt(0, 0);
    RVM_A7M_PUTCHAR('6');
}


void RMP_Init_Idle(void)
{
    Send=1;
    while(1)
    {
        RVM_A7M_PUTCHAR('2');
        
        if(Send==1)
        {
            Send=0;
            RVM_Sig_Snd(SEND_RECV1);
        }
        
        RVM_Hyp_Wait_Vect();
        RVM_A7M_PUTCHAR('3');
    }
    return;
}
/* End Function:RMP_Init *****************************************************/

/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/

