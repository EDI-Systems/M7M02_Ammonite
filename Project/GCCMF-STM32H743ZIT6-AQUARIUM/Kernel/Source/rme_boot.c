/******************************************************************************
Filename    : rme_boot.c
Author      : The RVM project generator.
Date        : 15/11/2026 04:05:58
License     : Unlicense; see COPYING for details.
Description : The boot-time initialization file.
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

/* Private Variable **********************************************************/
static struct RME_Cap_Sig* Tim4_Vct_Sig;
/* End Private Variable ******************************************************/

/* Private Function **********************************************************/
RME_EXTERN rme_ptr_t RME_Vct_Tim4_Handler(struct RME_Reg_Struct* Reg);
RME_EXTERN rme_ptr_t RME_Spurious_Handler(struct RME_Reg_Struct* Reg,
                                          rme_ptr_t Vct_Num);
/* End Private Function ******************************************************/

/* Public Function ***********************************************************/
rme_ptr_t RME_Boot_Vct_Init(struct RME_Cap_Cpt* Cpt,
                            rme_ptr_t Cap_Front,
                            rme_ptr_t Kom_Front);
rme_ptr_t RME_Boot_Vct_Handler(struct RME_Reg_Struct* Reg,
                               rme_ptr_t Vct_Num);
/* End Public Function *******************************************************/

/* Begin Function:RME_Boot_Vct_Init *******************************************
Description : Initialize all the vector endpoints at boot-time.
Input       : rme_ptr_t Cap_Front - The current capability table frontier.
            : rme_ptr_t Kom_Front - The current kernel absolute memory frontier.
Output      : None.
Return      : rme_ptr_t - The kernel memory frontier after booting is all done.
******************************************************************************/
rme_ptr_t RME_Boot_Vct_Init(struct RME_Cap_Cpt* Cpt,
                            rme_ptr_t Cap_Front,
                            rme_ptr_t Kom_Front)
{
    rme_ptr_t Cur_Addr;
    struct RME_Cap_Cpt* Sig_Cpt;

    /* The address here shall match what is in the generator */
    RME_ASSERT(Cap_Front==7U);
    RME_ASSERT(Kom_Front==0x2000B770U);

    Cur_Addr=Kom_Front;
    /* Create all the vector capability tables first */
    RME_ASSERT(_RME_Cpt_Boot_Crt(Cpt, RME_BOOT_INIT_CPT, RME_MAIN_VCT_0, Cur_Addr, 1U)==0U);
    Cur_Addr+=RME_KOM_ROUND(RME_CPT_SIZE(1U));

    /* Then all the vectors */
    Sig_Cpt=&(RME_CAP_GETOBJ(Cpt,struct RME_Cap_Cpt*)[RME_MAIN_VCT_0]);
    Tim4_Vct_Sig=&(RME_CAP_GETOBJ(Sig_Cpt,struct RME_Cap_Sig*)[RVM_VCT_TIM4]);
    RME_ASSERT(_RME_Sig_Boot_Crt(Cpt, RME_MAIN_VCT_0, RVM_VCT_TIM4)==0U);

    return Cur_Addr;
}
/* End Function:RME_Boot_Vct_Init ********************************************/

/* Begin Function:RME_Boot_Vct_Handler ****************************************
Description : The interrupt handler entry for all the vectors.
Input       : struct RME_Reg_Struct* Reg - The register set.
            : rme_ptr_t Vct_Num - The vector number.
Output      : None.
Return      : rme_ptr_t - Whether to send to the generic vector endpoint.
******************************************************************************/
rme_ptr_t RME_Boot_Vct_Handler(struct RME_Reg_Struct* Reg,
                               rme_ptr_t Vct_Num)
{
    rme_ptr_t Send;
    struct RME_Cap_Sig* Endp;

    Send=0U;

    switch(Vct_Num)
    {
        /* Tim4 */
        case 30U: 
        {
            Send=RME_Vct_Tim4_Handler(Reg);
            Endp=Tim4_Vct_Sig;
            break;
        }
        /* Spurious */
        default: 
        {
            Send=RME_Spurious_Handler(Reg,Vct_Num);
            Endp=RME_NULL;
            break;
        }
    }

    if(Endp==RME_NULL)
    {
        if(Send==RME_RVM_VCT_SIG_NONE)
	         return 0U;
        else
	         return 1U;
    }

    switch(Send)
    {
        case RME_RVM_VCT_SIG_ALL:
        {
            _RME_Kern_Snd(Endp,1U);
            return 1U;
        }
        case RME_RVM_VCT_SIG_SELF:
        {
            _RME_Kern_Snd(Endp,1U);
            _RME_Kern_High(Reg,RME_CPU_LOCAL());
            return 0U;
        }
        case RME_RVM_VCT_SIG_INIT: return 1U;
        default: break;
    }

    return 0U;
}
/* End Function:RME_Boot_Vct_Handler *****************************************/

/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/

