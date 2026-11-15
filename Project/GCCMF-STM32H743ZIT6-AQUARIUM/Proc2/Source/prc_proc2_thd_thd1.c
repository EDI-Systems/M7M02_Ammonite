/******************************************************************************
Filename    : prc_proc2_thd_thd1.c
Author      : The RVM project generator.
Date        : 14/11/2026 20:46:02
License     : Unlicense; see COPYING for details.
Description : The user stub file for thread 'Thd1'.
******************************************************************************/

/* Include *******************************************************************/
#include "rvm.h"
/* End Include ***************************************************************/

/* Private Function **********************************************************/
void Thd_Thd1(rvm_ret_t Param);
/* End Private Function ******************************************************/

/* Begin Function:Thd_Thd1 ****************************************************
Description : The function body for thread.
Input       : rvm_ret_t Param - The parameter supplied by the OS.
Output      : None.
Return      : None.
******************************************************************************/
void Thd_Thd1(rvm_ret_t Param)
{
    /* Add your code here */
    RVM_ASSERT(RVM_Sig_Rcv(RCV_RECV1,RVM_RCV_BM)>=0);
    while(1);
}
/* End Function:Thd_Thd1 *****************************************************/

/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/


