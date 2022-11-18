/******************************************************************************
Filename    : rvm_user.c
Author      : The RME project generator.
Date        : 10/11/2019
License     : LGPL v3+; see COPYING for details.
Description : The user hook file.
******************************************************************************/

/* Includes ******************************************************************/
#include "rvm.h"

#define __HDR_DEFS__
#include "Platform/A7M/rvm_platform_a7m.h"
#include "Init/rvm_syssvc.h"
#include "Init/rvm_init.h"
#include "Init/rvm_hyper.h"
#undef __HDR_DEFS__

#define __HDR_STRUCTS__
#include "Platform/A7M/rvm_platform_a7m.h"
#include "Init/rvm_syssvc.h"
#include "Init/rvm_init.h"
#include "Init/rvm_hyper.h"
#undef __HDR_STRUCTS__

#define __HDR_PUBLIC_MEMBERS__
#include "Platform/A7M/rvm_platform_a7m.h"
#include "Init/rvm_syssvc.h"
#include "Init/rvm_init.h"
#include "Init/rvm_hyper.h"
#undef __HDR_PUBLIC_MEMBERS__

#include "rvm_boot.h"
/* End Includes **************************************************************/

/* Public C Function Prototypes **********************************************/
void RVM_Boot_Pre_Init(void);
void RVM_Boot_Post_Init(void);
/* End Public C Function Prototypes ******************************************/

/* Begin Function:RVM_Boot_Pre_Init *******************************************
Description : Initialize critical hardware before any kernel object creation takes place.
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
Description : Initialize hardware after all kernel object creation took place.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
void RVM_Boot_Post_Init(void)
{
    /* Add code here */
}
/* End Function:RVM_Boot_Post_Init *******************************************/

/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/

