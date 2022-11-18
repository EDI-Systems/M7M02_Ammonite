/******************************************************************************
Filename    : proc_virt1.c
Author      : The RME project generator.
Date        : 10/11/2019
License     : LGPL v3+; see COPYING for details.
Description : The user stub file.
******************************************************************************/

/* Includes ******************************************************************/
#include "rvm.h"
#include "proc_virt1.h"
#include "rvm_guest.h"
/* End Includes **************************************************************/

/* Private C Function Prototypes *********************************************/
/* Threads */
rvm_ret_t Thd_Vect(rvm_ret_t Param);
rvm_ret_t Thd_User(rvm_ret_t Param);
/* Invocations */

/* End Private C Function Prototypes *****************************************/

/* Begin Function:Thd_Vect ****************************************************
Description : The function body for thread.
Input       : rvm_ret_t Param - The parameter, which is supplied by the caller.
Output      : None.
Return      : rvm_ret_t - Should never return.
******************************************************************************/
rvm_ret_t Thd_Vect(rvm_ret_t Param)
{
    /* DO NOT MODIFY - ESSENTIAL FOR VIRTUAL MACHINE */
    RVM_Virt_Init();
    RVM_Vect_Loop();
    while(1);
}
/* End Function:Thd_Vect *****************************************************/

/* Begin Function:Thd_User ****************************************************
Description : The function body for thread.
Input       : rvm_ret_t Param - The parameter, which is supplied by the caller.
Output      : None.
Return      : rvm_ret_t - Should never return.
******************************************************************************/
rvm_ret_t Thd_User(rvm_ret_t Param)
{
    /* USELESS STUB - MAIN FUNCTION PROVIDED BY ORIGINAL OPERATING SYSTEM */
    while(1);
}
/* End Function:Thd_User *****************************************************/

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

