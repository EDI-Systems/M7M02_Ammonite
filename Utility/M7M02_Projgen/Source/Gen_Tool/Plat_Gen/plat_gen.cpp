/******************************************************************************
Filename    : plat_gen.cpp
Author      : pry
Date        : 12/07/2019
Licence     : LGPL v3+; see COPYING for details.
Description : Generic platform generator port.
******************************************************************************/

/* Includes ******************************************************************/
extern "C"
{
#include "xml.h"
}

#include "map"
#include "string"
#include "memory"
#include "vector"
#include "stdexcept"
#include "algorithm"

#define __HDR_DEFS__
#include "rvm_gen.hpp"
#include "Proj_Info/proj_info.hpp"
#include "Plat_Info/plat_info.hpp"
#include "Chip_Info/chip_info.hpp"
#include "Proj_Info/Kernel/kernel.hpp"
#include "Gen_Tool/Plat_Gen/plat_gen.hpp"
#undef __HDR_DEFS__

#define __HDR_CLASSES__
#include "rvm_gen.hpp"
#include "Proj_Info/proj_info.hpp"
#include "Proj_Info/Kobj/kobj.hpp"
#include "Plat_Info/plat_info.hpp"
#include "Chip_Info/chip_info.hpp"
#include "Proj_Info/Kernel/kernel.hpp"
#include "Gen_Tool/Plat_Gen/plat_gen.hpp"
#undef __HDR_STRUCTS__
/* End Includes **************************************************************/
namespace RVM_GEN
{
/* Begin Function:Plat_Gen::Plat_Gen ******************************************
Description : Generator for the platform.
Input       : const std::string& Name - The platform name.
              class Proj_Info* Proj - The project information.
              class Plat_Info* Plat - The platform information.
              class Chip_Info* Chip - The chip information.
Output      : None.
Return      : None.
******************************************************************************/
/* void */ Plat_Gen::Plat_Gen(const std::string& Name,
                              class Proj_Info* Proj, class Plat_Info* Plat, class Chip_Info* Chip)
{
    try
    {
        this->Name=Name;
        Name_Gen(this);
        this->Proj=Proj;
        this->Plat=Plat;
        this->Chip=Chip;
    }
    catch(std::exception& Exc)
    {
        Main::Error(std::string("Platform generator:\n")+Exc.what());
    }
}
/* End Function:Plat_Gen::Plat_Gen *******************************************/

/* Begin Function:Plat_Gen::Size_Captbl **************************************
Description : Query the size of capability table given the number of slots.
Input       : ptr_t Slot - The number of slots.
Output      : None.
Return      : ptr_t - The size in bytes.
******************************************************************************/
ptr_t Plat_Gen::Size_Captbl(ptr_t Slot)
{
    return ROUND_UP_POW2(this->Plat->Wordlength*Slot,this->Proj->Kernel->Kmem_Order);
}
/* End Function:Plat_Gen::Size_Captbl ****************************************/

/* Begin Function:Plat_Gen::Size_Pgtbl ****************************************
Description : Query the size of page table given the parameters.
Input       : ptr_t Num_Order - The number order.
              ptr_t Is_Top - Whether this is a top-level.
Output      : None.
Return      : ptr_t - The size in bytes.
******************************************************************************/
ptr_t Plat_Gen::Size_Pgtbl(ptr_t Num_Order, ptr_t Is_Top)
{
    return ROUND_UP_POW2(this->Raw_Pgtbl(Num_Order,Is_Top),this->Proj->Kernel->Kmem_Order);
}
/* End Function:Plat_Gen::Size_Pgtbl *****************************************/

/* Begin Function:Plat_Gen::Size_Thread ***************************************
Description : Query the size of thread.
Input       : None.
Output      : None.
Return      : ptr_t - The size in bytes.
******************************************************************************/
ptr_t Plat_Gen::Size_Thread(void)
{
    return ROUND_UP_POW2(this->Raw_Thread(),this->Proj->Kernel->Kmem_Order);
}
/* End Function:Plat_Gen::Size_Thread ****************************************/

/* Begin Function:Plat_Gen::Size_Invocation ***********************************
Description : Query the size of invocation.
Input       : None.
Output      : None.
Return      : ptr_t - The size in bytes.
******************************************************************************/
ptr_t Plat_Gen::Size_Invocation(void)
{
    return ROUND_UP_POW2(this->Raw_Invocation(),this->Proj->Kernel->Kmem_Order);
}
/* End Function:Plat_Gen::Size_Invocation ************************************/

/* Begin Function:Plat_Gen::Size_Register *************************************
Description : Query the size of the register set.
Input       : None.
Output      : None.
Return      : ptr_t - The size in bytes.
******************************************************************************/
ptr_t Plat_Gen::Size_Register(void)
{
    return ROUND_UP_POW2(this->Raw_Register(),this->Proj->Kernel->Kmem_Order);
}
/* End Function:Plat_Gen::Size_Register **************************************/
}
/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
