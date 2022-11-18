/******************************************************************************
Filename    : rme_boot.c
Author      : The RME project generator.
Date        : 10/11/2019
License     : LGPL v3+; see COPYING for details.
Description : The boot-time initialization file.
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

/* Private Global Variables **************************************************/
static struct RME_Cap_Sig* TIM2_Vect_Sig;
/* End Private Global Variables **********************************************/

/* Private C Function Prototypes *********************************************/
extern rme_ptr_t RME_Vect_TIM2_User(rme_ptr_t Int_Num);
/* End Private C Function Prototypes *****************************************/

/* Public C Function Prototypes **********************************************/
rme_ptr_t RME_Boot_Vect_Init(struct RME_Cap_Captbl* Captbl, rme_ptr_t Cap_Front, rme_ptr_t Kmem_Front);
rme_ptr_t RME_Boot_Vect_Handler(rme_ptr_t Vect_Num);
/* End Public C Function Prototypes ******************************************/

/* Begin Function:RME_Boot_Vect_Init ******************************************
Description : Initialize all the vector endpoints at boot-time.
Input       : rme_ptr_t Cap_Front - The current capability table frontier.
            : rme_ptr_t Kmem_Front - The current kernel absolute memory frontier.
Output      : None.
Return      : rme_ptr_t - The kernel memory frontier after booting is all done.
******************************************************************************/
rme_ptr_t RME_Boot_Vect_Init(struct RME_Cap_Captbl* Captbl, rme_ptr_t Cap_Front, rme_ptr_t Kmem_Front)
{
    rme_ptr_t Cur_Addr;
    struct RME_Cap_Captbl* Sig_Captbl;

    /* The address here shall match what is in the generator */
    RME_ASSERT(Cap_Front==8);
    RME_ASSERT(Kmem_Front==0x10001320);

    Cur_Addr=Kmem_Front;
    /* Create all the vector capability tables first */
    RME_ASSERT(_RME_Captbl_Boot_Crt(Captbl, RME_BOOT_CAPTBL, RME_BOOT_CTVECT0, Cur_Addr, 1)==0);
    Cur_Addr+=RME_KOTBL_ROUND(RME_CAPTBL_SIZE(1));

    /* Then all the vectors */
    Sig_Captbl=&(RME_CAP_GETOBJ(Captbl,struct RME_Cap_Captbl*)[RME_BOOT_CTVECT0]);
    TIM2_Vect_Sig=&(RME_CAP_GETOBJ(Sig_Captbl,struct RME_Cap_Sig*)[RME_BOOT_VECT_TIM2]);
    RME_ASSERT(_RME_Sig_Boot_Crt(Captbl, RME_BOOT_CTVECT0, RME_BOOT_VECT_TIM2)==0);

    return Cur_Addr;
}
/* End Function:RME_Boot_Vect_Init *******************************************/

/* Begin Function:RME_Boot_Vect_Handler ***************************************
Description : The interrupt handler entry for all the vectors.
Input       : rme_ptr_t Vect_Num - The vector number.
Output      : None.
Return      : rme_ptr_t - The number of signals to send to the generic vector endpoint.
******************************************************************************/
rme_ptr_t RME_Boot_Vect_Handler(rme_ptr_t Vect_Num)
{
    rme_ptr_t Send_Num;

    Send_Num=0;

    switch(Vect_Num)
    {
        /* TIM2 */
        case 28:
        {
            extern rme_ptr_t RME_Vect_TIM2_User(rme_ptr_t Vect_Num);
            Send_Num=RME_Vect_TIM2_User(Vect_Num);
            _RME_Kern_Snd(TIM2_Vect_Sig);
            break;
        }
        default: break;
    }

    return Send_Num;
}
/* End Function:RME_Boot_Vect_Handler ****************************************/

/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/

