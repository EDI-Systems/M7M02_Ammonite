/******************************************************************************
Filename    : guest_gen.cpp
Author      : pry
Date        : 12/07/2019
Licence     : LGPL v3+; see COPYING for details.
Description : Generic guest operating system generator port.
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
#include "Proj_Gen/proj_gen.hpp"
#include "Proj_Gen/Tool_Gen/tool_gen.hpp"
#include "Proj_Info/Kobj/kobj.hpp"
#include "Proj_Info/Process/process.hpp"
#include "Proj_Gen/Guest_Gen/guest_gen.hpp"
#undef __HDR_DEF__

#define __HDR_CLASS__
#include "rvm_gen.hpp"
#include "Proj_Gen/proj_gen.hpp"
#include "Proj_Gen/Tool_Gen/tool_gen.hpp"
#include "Proj_Info/Kobj/kobj.hpp"
#include "Proj_Info/Process/process.hpp"
#include "Proj_Gen/Guest_Gen/guest_gen.hpp"
#undef __HDR_CLASS__
/* End Include ***************************************************************/
namespace RVM_GEN
{
/* Function:Guest_Gen::Guest_Gen **********************************************
Description : Generator for the guest operating system.
Input       : const std::string& Name - The guest OS name.
              class Proj_Info* Proj - The project information.
              class Plat_Info* Plat - The platform information.
              class Chip_Info* Chip - The chip information.
Output      : None.
Return      : None.
******************************************************************************/
/* void */ Guest_Gen::Guest_Gen(const std::string& Name,
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
        Main::Error(std::string("Guest generator:\n")+Exc.what());
    }
}
/* End Function:Guest_Gen::Guest_Gen *****************************************/

/* Function:Guest_Gen::Process_Linker *****************************************
Description : Generate the linker script for a virtual process by default.
              Guest generators that own their linker script override this.
Input       : std::unique_ptr<std::vector<std::string>>& List - The lines.
              const class Process* Prc - The process to generate for.
              class Tool_Gen* Tool - The toolchain generator.
Output      : None.
Return      : None.
******************************************************************************/
void Guest_Gen::Process_Linker(std::unique_ptr<std::vector<std::string>>& List,
                               const class Process* Prc, class Tool_Gen* Tool)
{
    Main::Info("> Generating linker script.");
    Tool->Process_Linker(List, Prc);
    Proj_Gen::Line_Write(List, Prc->Linker_Output+Prc->Linker_Filename);
}
/* End Function:Guest_Gen::Process_Linker ************************************/

/* Function:Guest_Gen::Desc_Usr_Entry *****************************************
Description : The user thread entry placed in the virtual descriptor.
Input       : None.
Output      : None.
Return      : The entry symbol.
******************************************************************************/
std::string Guest_Gen::Desc_Usr_Entry(void)
{
    return "__RVM_Entry";
}
/* End Function:Guest_Gen::Desc_Usr_Entry ************************************/
}
/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
