/******************************************************************************
Filename    : rvm_hook.c
Author      : The RVM project generator.
Date        : 14/11/2026 20:46:02
License     : Unlicense; see COPYING for details.
Description : The user initialization hook file.
******************************************************************************/

/* Include *******************************************************************/
#include "rvm_boot.h"

#define __HDR_DEF__
#include "Platform/A7M/rvm_platform_a7m.h"
#include "Syslib/rvm_syslib.h"
#include "Monitor/rvm_monitor.h"
#undef __HDR_DEF__

#define __HDR_STRUCT__
#include "Platform/A7M/rvm_platform_a7m.h"
#include "Syslib/rvm_syslib.h"
#include "Monitor/rvm_monitor.h"
#undef __HDR_STRUCT__

#define __HDR_PUBLIC__
#include "Platform/A7M/rvm_platform_a7m.h"
#include "Syslib/rvm_syslib.h"
#include "Monitor/rvm_monitor.h"
#undef __HDR_PUBLIC__
/* End Include ***************************************************************/

/* Public Function ***********************************************************/
void RVM_Boot_Pre_Init(void);
void RVM_Boot_Post_Init(void);
rvm_ret_t RVM_Vct_Filter(rvm_ptr_t Vct_Num);
rvm_ret_t RVM_Evt_Filter(rvm_ptr_t Evt_Num);
/* End Public Function *******************************************************/

/* Begin Function:RVM_Boot_Pre_Init *******************************************
Description : Initialize critical hardware before any initialization takes place.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
void RVM_Boot_Pre_Init(void)
{
    /* Add code here */
}
/* End Function:RVM_Boot_Pre_Init ********************************************/

/* Begin Function:RVM_Boot_Post_Init ******************************************
Description : Initialize other hardware after all initialization took place.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
void RVM_Boot_Post_Init(void)
{
    /* Add code here */
}
/* End Function:RVM_Boot_Post_Init *******************************************/

/* Begin Function:RVM_Vct_Filter **********************************************
Description : Filter all forwarded vector activations before they reach VMs.
Input       : rvm_ptr_t Vct_Num - The number of the activated vector.
Output      : None.
Return      : rvm_ret_t - If forwarding is needed, return RVM_FILTER_PASS (0);
                          if not, return RVM_FILTER_DROP (-1).
******************************************************************************/
rvm_ret_t RVM_Vct_Filter(rvm_ptr_t Vct_Num)
{
    /* Add code here */
    return RVM_FILTER_PASS;
}
/* End Function:RVM_Vct_Filter ***********************************************/

/* Begin Function:RVM_Evt_Filter **********************************************
Description : Filter all forwarded event activations before they reach VMs.
Input       : rvm_ptr_t Evt_Num - The number of the activated event.
Output      : None.
Return      : rvm_ret_t - If forwarding is needed, return RVM_FILTER_PASS (0);
                          if not, return RVM_FILTER_DROP (-1).
******************************************************************************/
rvm_ret_t RVM_Evt_Filter(rvm_ptr_t Evt_Num)
{
    /* Add code here */
    return RVM_FILTER_PASS;
}
/* End Function:RVM_Evt_Filter ***********************************************/

/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/

