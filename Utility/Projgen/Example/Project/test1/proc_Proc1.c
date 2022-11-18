/******************************************************************************
Filename    : proc_Proc1.c
Author      : The RME project generator.
Date        : 10/09/2019
License     : LGPL v3+; see COPYING for details.
Description : The user stub file.
******************************************************************************/

/* Includes ******************************************************************/
#include "rvm.h"
#include "proc_Proc1.h"
#include "rvm_guest.h"
/* End Includes **************************************************************/

/* Private C Function Prototypes *********************************************/
/* Threads */
rvm_ret_t Thd_Thd1(rvm_ret_t Param);
/* Invocations */

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

/* End Private C Function Prototypes *****************************************/

/* Begin Function:Thd_Thd1 ****************************************************
Description : The function body for thread.
Input       : rvm_ret_t Param - The parameter, which is supplied by the caller.
Output      : None.
Return      : rvm_ret_t - Should never return.
******************************************************************************/
rvm_ret_t Thd_Thd1(rvm_ret_t Param)
{
    /* Add your code here - Threads shall never return */
    while(1){
    RVM_A7M_PUTCHAR('0');
    RVM_Proc_Send_Evt(0);
    RVM_A7M_PUTCHAR('x');
    RVM_Sig_Rcv(RECV_RECV1, RVM_RCV_BM);
    RVM_A7M_PUTCHAR('1');
    }
    while(1);
}
/* End Function:Thd_Thd1 *****************************************************/

/* Begin Function:main ********************************************************
Description : The function body for thread.
Input       : rvm_ret_t Param - The parameter, which is supplied by the caller.
Output      : None.
Return      : rvm_ret_t - Should never return.
******************************************************************************/
rvm_ret_t main(rvm_ret_t Param)
{
    RVM_Entry((rvm_ptr_t)Thd_Thd1, 0x2002f000, 0x1000, 0x1234);
}
/* End Function:main *********************************************************/

/* Begin Function:RVM_Putchar *************************************************
Description : The character printing function for debugging.
Input       : char Char - The character to print to console.
Output      : None.
Return      : None.
******************************************************************************/
void RVM_Putchar(char Char)
{
    /* Add your own code */
}
/* End Function:RVM_Putchar **************************************************/

/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/

