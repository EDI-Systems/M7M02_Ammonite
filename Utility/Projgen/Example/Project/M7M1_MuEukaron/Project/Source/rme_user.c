/******************************************************************************
Filename    : rme_user.c
Author      : The RME project generator.
Date        : 10/11/2019
License     : LGPL v3+; see COPYING for details.
Description : The user hook file.
******************************************************************************/

/* Includes ******************************************************************/
#define __HDR_DEFS__
#include "Platform/A7M/rme_platform_a7m.h"
#include "Kernel/rme_kernel.h"
#undef __HDR_DEFS__

#define __HDR_STRUCTS__
#include "Platform/A7M/rme_platform_a7m.h"
#include "Kernel/rme_kernel.h"
#undef __HDR_STRUCTS__

#define __HDR_PUBLIC_MEMBERS__
#include "Platform/A7M/rme_platform_a7m.h"
#include "Kernel/rme_kernel.h"
#undef __HDR_PUBLIC_MEMBERS__

#include "rme_boot.h"
/* End Includes **************************************************************/

/* Public C Function Prototypes **********************************************/
void RME_Boot_Pre_Init(void);
void RME_Boot_Post_Init(void);
void RME_Reboot_Failsafe(void);
rme_ret_t RME_User_Kern_Func_Handler(rme_ptr_t Func_ID, rme_ptr_t Sub_ID, rme_ptr_t Param1, rme_ptr_t Param2);
rme_ptr_t RME_Vect_TIM2_User(rme_ptr_t Int_Num);
/* End Public C Function Prototypes ******************************************/

/* Begin Function:RME_Boot_Pre_Init *******************************************
Description : Initialize critical hardware before any kernel initialization takes place.
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
Description : Initialize hardware after all kernel initialization took place.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
void RME_Boot_Post_Init(void)
{
    /* Add code here */
}
/* End Function:RME_Boot_Post_Init *******************************************/

/* Begin Function:RME_User_Kern_Func_Handler **********************************
Description : User-modifiable kernel function handler.
Input       : rme_ptr_t Func_ID - The function ID.
            : rme_ptr_t Sub_ID - The subfunction ID.
            : rme_ptr_t Param1 - The first parameter.
            : rme_ptr_t Param2 - The second parameter.
Output      : None.
Return      : rme_ret_t - The return value.
******************************************************************************/
rme_ret_t RME_User_Kern_Func_Handler(rme_ptr_t Func_ID, rme_ptr_t Sub_ID, rme_ptr_t Param1, rme_ptr_t Param2)
{
    /* Add code here */
    return RME_ERR_KERN_OPFAIL;
}
/* End Function:RME_User_Kern_Func_Handler ***********************************/

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

/* Begin Function:RME_Vect_TIM2_User ******************************************
Description : The user top-half interrupt handler for vector.
Input       : rme_ptr_t Vect_Num - The vector number.
Output      : None.
Return      : rme_ptr_t - Return 0 to send to the RME_BOOT_INIT_VECT, return other values to skip.
******************************************************************************/
rme_ptr_t RME_Vect_TIM2_User(rme_ptr_t Vect_Num)

{
    /* Add code here */

    return 0;
}
/* End Function:RME_Vect_TIM2_User *******************************************/

/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/

