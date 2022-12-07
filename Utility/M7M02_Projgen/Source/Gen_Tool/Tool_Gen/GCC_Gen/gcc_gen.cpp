/******************************************************************************
Filename    : gcc_gen.cpp
Author      : pry
Date        : 12/07/2019
Licence     : LGPL v3+; see COPYING for details.
Description : This toolset is for GCC; generates GCC linker scripts.
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
#include "Gen_Tool/Tool_Gen/tool_gen.hpp"
#include "Gen_Tool/Tool_Gen/GCC_Gen/gcc_gen.hpp"
#undef __HDR_DEFS__

#define __HDR_CLASSES__
#include "rvm_gen.hpp"
#include "Gen_Tool/Tool_Gen/tool_gen.hpp"
#include "Gen_Tool/Tool_Gen/GCC_Gen/gcc_gen.hpp"
#undef __HDR_STRUCTS__
/* End Includes **************************************************************/
namespace RVM_GEN
{
/* Begin Function:GCC_Gen::GCC_Gen ********************************************
Description : Generator for the ARMv7-M platform.
Input       : class Proj_Info* Proj - The project information.
              class Plat_Info* Plat - The platform information.
              class Chip_Info* Chip - The chip information.
Output      : None.
Return      : None.
******************************************************************************/
/* void */ GCC_Gen::GCC_Gen(class Proj_Info* Proj, class Plat_Info* Plat, class Chip_Info* Chip):
Tool_Gen("GCC", Proj, Plat, Chip)
{
    try
    {

    }
    catch(std::exception& Exc)
    {
        Main::Error(std::string("GCC generator:\n")+Exc.what());
    }
}
/* End Function:GCC_Gen::GCC_Gen *********************************************/

/* Begin Function:GCC_Gen::Suffix *********************************************
Description : Returns suffix for a given type of file.
Input       : ptr_t Type - The file type.
Output      : None.
Return      : std::string - The file suffix.
******************************************************************************/
std::string GCC_Gen::Suffix(ptr_t Type)
{
    if(Type==TOOL_LINKER)
        return ".ld";
    else if(Type==TOOL_ASSEMBLER)
        return ".s";
    else
        Main::Error("AXXXX: File type not recognized.");
}
/* End Function:GCC_Gen::Suffix **********************************************/

/* Begin Function:GCC_Gen::Kernel_Linker **************************************
Description : Generate the RME linker script.
Input       : std::unique_ptr<std::vector<std::string>>& List - The file.
Output      : std::unique_ptr<std::vector<std::string>>& List - The updated file.
Return      : None.
******************************************************************************/
void GCC_Gen::Kernel_Linker(std::unique_ptr<std::vector<std::string>>& List)
{

}
/* End Function:GCC_Gen::Kernel_Linker ***************************************/
}
/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
