/******************************************************************************
Filename    : plat_gen.cpp
Author      : pry
Date        : 12/07/2019
Licence     : LGPL v3+; see COPYING for details.
Description : Generic platform generator port.
******************************************************************************/

/* Include *******************************************************************/
extern "C"
{
#include "xml.h"
}

#include "set"
#include "map"
#include "string"
#include "memory"
#include "vector"
#include "stdexcept"
#include "algorithm"

#define __HDR_DEF__
#include "rvm_gen.hpp"
#include "Proj_Info/proj_info.hpp"
#include "Plat_Info/plat_info.hpp"
#include "Chip_Info/chip_info.hpp"
#include "Proj_Info/Kernel/kernel.hpp"
#include "Proj_Gen/Plat_Gen/plat_gen.hpp"
#undef __HDR_DEF__

#define __HDR_CLASS__
#include "rvm_gen.hpp"
#include "Proj_Info/proj_info.hpp"
#include "Proj_Info/Kobj/kobj.hpp"
#include "Plat_Info/plat_info.hpp"
#include "Chip_Info/chip_info.hpp"
#include "Proj_Info/Kernel/kernel.hpp"
#include "Proj_Gen/Plat_Gen/plat_gen.hpp"
#undef __HDR_CLASS__
/* End Include ***************************************************************/
namespace RVM_GEN
{
/* Function:Plat_Gen::Plat_Gen ************************************************
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

/* Function:Plat_Gen::Pow2_Align **********************************************
Description : Get the power-of-2 alignment status for a memory address.
Input       : ptr_t Base - The address.
Output      : None.
Return      : ptr_t - The maximum alignment order.
******************************************************************************/
ptr_t Plat_Gen::Pow2_Align(ptr_t Base)
{
    ptr_t Align;

    for(Align=0;Align<this->Plat->Wordlength;Align++)
    {
        if(((Base>>Align)&0x01)!=0)
            break;
    }

    return Align;
}
/* End Function:Plat_Gen::Pow2_Align *****************************************/

/* Function:Plat_Gen::Pow2_Contain ********************************************
Description : Get the power-of-2 containment status for a memory size.
Input       : ptr_t Base - The address.
Output      : None.
Return      : ptr_t - The maximum containable order.
******************************************************************************/
ptr_t Plat_Gen::Pow2_Contain(ptr_t Size)
{
    ptr_t Contain;

    for(Contain=0;Contain<this->Plat->Wordlength;Contain++)
    {
        if(Size<POW2(Contain))
            break;
    }

    return Contain-1;
}
/* End Function:Plat_Gen::Pow2_Contain ***************************************/

/* Function:Plat_Gen::Pow2_Box ************************************************
Description : Get the power-of-2 box this memory range lies in. Useful for some
              architectures.
Input       : ptr_t Begin - The inclusive begin address.
              ptr_t End - The exclusive end address.
Output      : None.
Return      : ptr_t - The total order needed to contain the memory range.
******************************************************************************/
ptr_t Plat_Gen::Pow2_Box(ptr_t Begin, ptr_t End)
{
    ptr_t Total_Order;

    /* Which power-of-2 box is this in? */
    Total_Order=0;
    while(1)
    {
        /* No bigger than 32 is ever possible */
        if(Total_Order>=this->Plat->Wordlength)
            break;
        if(End<=(ROUND_DOWN_POW2(Begin, Total_Order)+POW2(Total_Order)))
            break;
        Total_Order++;
    }

    return Total_Order;
}
/* End Function:Plat_Gen::Pow2_Box *******************************************/

/* Function:Plat_Gen::Size_Cpt ************************************************
Description : Query the size of capability table given the number of slots.
Input       : ptr_t Slot - The number of slots.
Output      : None.
Return      : ptr_t - The size in bytes.
******************************************************************************/
ptr_t Plat_Gen::Size_Cpt(ptr_t Slot)
{
    return ROUND_UP_POW2(this->Plat->Wordlength*Slot,this->Proj->Kernel->Kom_Order);
}
/* End Function:Plat_Gen::Size_Cpt *******************************************/

/* Function:Plat_Gen::Size_Pgt ************************************************
Description : Query the size of page table given the parameters.
Input       : ptr_t Num_Order - The number order.
              ptr_t Is_Top - Whether this is a top-level.
Output      : None.
Return      : ptr_t - The size in bytes.
******************************************************************************/
ptr_t Plat_Gen::Size_Pgt(ptr_t Num_Order, ptr_t Is_Top)
{
    return ROUND_UP_POW2(this->Raw_Pgt(Num_Order,Is_Top),this->Proj->Kernel->Kom_Order);
}
/* End Function:Plat_Gen::Size_Pgt *******************************************/

/* Function:Plat_Gen::Size_Thread *********************************************
Description : Query the size of thread.
Input       : const std::vector<std::string>& Coprocessor - The coprocessor list.
              ptr_t Is_Hyp - Whether the thread is hypervisor managed.
Output      : None.
Return      : ptr_t - The size in bytes.
******************************************************************************/
ptr_t Plat_Gen::Size_Thread(const std::vector<std::string>& Coprocessor,
		                    ptr_t Is_Hyp)
{
	if(Is_Hyp!=0)
		return ROUND_UP_POW2(this->Raw_Thread(),this->Proj->Kernel->Kom_Order);
	else
		return ROUND_UP_POW2(this->Raw_Thread()+this->Raw_Register(Coprocessor),this->Proj->Kernel->Kom_Order);
}
/* End Function:Plat_Gen::Size_Thread ****************************************/

/* Function:Plat_Gen::Size_Invocation *****************************************
Description : Query the size of invocation.
Input       : None.
Output      : None.
Return      : ptr_t - The size in bytes.
******************************************************************************/
ptr_t Plat_Gen::Size_Invocation(void)
{
    return ROUND_UP_POW2(this->Raw_Invocation(),this->Proj->Kernel->Kom_Order);
}
/* End Function:Plat_Gen::Size_Invocation ************************************/

/* Function:Plat_Gen::Size_Register *******************************************
Description : Query the size of the register set.
Input       : const std::vector<std::string>& Coprocessor - The coprocessor list.
Output      : None.
Return      : ptr_t - The size in bytes.
******************************************************************************/
ptr_t Plat_Gen::Size_Register(const std::vector<std::string>& Coprocessor)
{
    return ROUND_UP_POW2(this->Raw_Register(Coprocessor),this->Proj->Kernel->Kom_Order);
}
/* End Function:Plat_Gen::Size_Register **************************************/
}
/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
