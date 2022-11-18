/******************************************************************************
Filename    : proc_proc1.c
Author      : The RME project generator.
Date        : 23/10/2019
License     : LGPL v3+; see COPYING for details.
Description : The user stub file.
******************************************************************************/

/* Includes ******************************************************************/
#include "rvm.h"
#include "proc_proc1.h"
#include "rvm_guest.h"
/* End Includes **************************************************************/

/* Private C Function Prototypes *********************************************/
/* Threads */
rvm_ret_t Thd_Thd1(rvm_ret_t Param);
/* Invocations */

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
    RVM_Sig_Rcv(RECV_RECV1, RVM_RCV_BM);
    while(1);
}
/* End Function:Thd_Thd1 *****************************************************/

/* Begin Function:main ********************************************************
Description : The function body for thread.
Input       : None.
Output      : None.
Return      : rvm_ret_t - Should never return.
******************************************************************************/
rvm_ret_t main(void)
{
    RVM_Entry((rvm_ptr_t)Thd_Thd1, 0x20003e00, 0x200, 0x1234);
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

