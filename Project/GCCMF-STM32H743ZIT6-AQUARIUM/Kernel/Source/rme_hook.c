/******************************************************************************
Filename    : rme_hook.c
Author      : The RVM project generator.
Date        : 14/11/2026 20:46:02
License     : Unlicense; see COPYING for details.
Description : The user initialization hook file.
******************************************************************************/

/* Include *******************************************************************/
#define __HDR_DEF__
#include "Platform/A7M/rme_platform_a7m.h"
#include "Kernel/rme_kernel.h"
#undef __HDR_DEF__

#define __HDR_STRUCT__
#include "Platform/A7M/rme_platform_a7m.h"
#include "Kernel/rme_kernel.h"
#undef __HDR_STRUCT__

#define __HDR_PUBLIC__
#include "Platform/A7M/rme_platform_a7m.h"
#include "Kernel/rme_kernel.h"
#undef __HDR_PUBLIC__

#include "rme_boot.h"
/* End Include ***************************************************************/

/* Public Function ***********************************************************/
void RME_Boot_Pre_Init(void);
void RME_Boot_Post_Init(void);
void RME_Reboot_Failsafe(void);
rme_ptr_t RME_Spurious_Handler(struct RME_Reg_Struct* Reg,
                               rme_ptr_t Vct_Num);
rme_ret_t RME_Hook_Kfn_Handler(rme_ptr_t Func_ID,
                               rme_ptr_t Sub_ID,
                               rme_ptr_t Param1,
                               rme_ptr_t Param2);
/* End Public Function *******************************************************/

/* Begin Function:RME_Boot_Pre_Init *******************************************
Description : Initialize critical hardware before any initialization takes place.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
void RME_Boot_Pre_Init(void)
{
    /* Add code here */
}
/* End Function:RME_Boot_Pre_Init ********************************************/

/* Begin Function:RME_Boot_Post_Init ******************************************
Description : Initialize other hardware after all initialization took place.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
void RME_Boot_Post_Init(void)
{
    /* Add code here */
}
/* End Function:RME_Boot_Post_Init *******************************************/

/* Begin Function:RME_Reboot_Failsafe *****************************************
Description : User-modifiable pre-rebooting failsafe sequence.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
void RME_Reboot_Failsafe(void)
{
    /* Add code here */
}
/* End Function:RME_Reboot_Failsafe ******************************************/

/* Begin Function:RME_Spurious_Handler ****************************************
Description : User-modifiable spurious interrupt handler.
Input       : struct RME_Reg_Struct* Reg - The register set.
            : rme_ptr_t Vct_Num - The vector number.
Output      : None.
Return      : rme_ptr_t - Decides what endpoints to send to (see manual).
******************************************************************************/
rme_ptr_t RME_Spurious_Handler(struct RME_Reg_Struct* Reg,
                               rme_ptr_t Vct_Num)
{
    /* Add code here */
    return RME_RVM_VCT_SIG_NONE;
}
/* End Function:RME_Spurious_Handler *****************************************/

/* Begin Function:RME_Hook_Kfn_Handler ****************************************
Description : User-modifiable kernel function handler hook.
Input       : struct RME_Reg_Struct* Reg - The register set.
            : rme_ptr_t Vct_Num - The vector number.
            : rme_ptr_t Func_ID - The function ID.
            : rme_ptr_t Sub_ID - The subfunction ID.
            : rme_ptr_t Param1 - The first parameter.
            : rme_ptr_t Param2 - The second parameter.
Output      : None.
Return      : rme_ret_t - The return value.
******************************************************************************/
rme_ret_t RME_Hook_Kfn_Handler(rme_ptr_t Func_ID,
                               rme_ptr_t Sub_ID,
                               rme_ptr_t Param1,
                               rme_ptr_t Param2)
{
    /* Add code here */
    return RME_ERR_KFN_FAIL;
}
/* End Function:RME_Hook_Kfn_Handler *****************************************/

/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/

