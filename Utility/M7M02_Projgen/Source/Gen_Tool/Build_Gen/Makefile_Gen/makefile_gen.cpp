/******************************************************************************
Filename    : makefile_gen.cpp
Author      : pry
Date        : 12/07/2019
Licence     : LGPL v3+; see COPYING for details.
Description : This toolset is for Keil uVision build system.
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
#include "Gen_Tool/Build_Gen/build_gen.hpp"
#include "Gen_Tool/Build_Gen/Makefile_Gen/makefile_gen.hpp"
#undef __HDR_DEFS__

#define __HDR_CLASSES__
#include "rvm_gen.hpp"
#include "Gen_Tool/Build_Gen/build_gen.hpp"
#include "Gen_Tool/Build_Gen/Makefile_Gen/makefile_gen.hpp"
#undef __HDR_STRUCTS__
/* End Includes **************************************************************/
namespace RVM_GEN
{
/* Begin Function:Makefile_Gen::Makefile_Gen **********************************
Description : Generator for the makefiles.
Input       : class Proj_Info* Proj - The project information.
              class Plat_Info* Plat - The platform information.
              class Chip_Info* Chip - The chip information.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
/* void */ Makefile_Gen::Makefile_Gen(class Proj_Info* Proj, class Plat_Info* Plat, class Chip_Info* Chip):
Build_Gen("Makefile", Proj, Plat, Chip)
{
    try
    {

    }
    catch(std::exception& Exc)
    {
        Main::Error(std::string("Makefile generator:\n")+Exc.what());
    }
}
/* End Function:Makefile_Gen::Makefile_Gen ***********************************/

/* Begin Function:Makefile_Gen::Suffix ****************************************
Description : Returns suffix for a given type of file.
Input       : ptr_t Type - The file type.
Output      : None.
Return      : std::string - The file suffix.
******************************************************************************/
std::string Makefile_Gen::Suffix(ptr_t Type)
{
    if(Type==BUILD_PROJECT)
        return "";
    else
        Main::Error("AXXXX: File type not recognized.");
}
/* End Function:Makefile_Gen::Suffix *****************************************/

/* Begin Function:Makefile_Gen::Kernel_Proj ***********************************
Description : Generate kernel project.
Input       : std::unique_ptr<std::vector<std::string>>& List - The file.
              const std::vector<std::string>& Include - The include file list.
              const std::vector<std::string>& Source - The source file list.
              const std::vector<std::string>& Source - The linker script file list.
Output      : std::unique_ptr<std::vector<std::string>>& List - The updated file.
Output      : None.
Return      : None.
******************************************************************************/
void Makefile_Gen::Kernel_Proj(std::unique_ptr<std::vector<std::string>>& List,
                               const std::vector<std::string>& Include,
                               const std::vector<std::string>& Source,
                               const std::vector<std::string>& Linker)
{

}
/* End Function:Makefile_Gen::Kernel_Proj ************************************/

/* Begin Function:Makefile_Gen::Monitor_Proj **********************************
Description : Generate monitor project.
Input       : std::unique_ptr<std::vector<std::string>>& List - The file.
              const std::vector<std::string>& Include - The include file list.
              const std::vector<std::string>& Source - The source file list.
              const std::vector<std::string>& Source - The linker script file list.
Output      : std::unique_ptr<std::vector<std::string>>& List - The updated file.
Output      : None.
Return      : None.
******************************************************************************/
void Makefile_Gen::Monitor_Proj(std::unique_ptr<std::vector<std::string>>& List,
                                const std::vector<std::string>& Include,
                                const std::vector<std::string>& Source,
                                const std::vector<std::string>& Linker)
{

}
/* End Function:Makefile_Gen::Monitor_Proj ***********************************/

/* Begin Function:Makefile_Gen::Process_Proj **********************************
Description : Generate process project.
Input       : std::unique_ptr<std::vector<std::string>>& List - The file.
              const std::vector<std::string>& Include - The include file list.
              const std::vector<std::string>& Source - The source file list.
              const std::vector<std::string>& Source - The linker script file list.
              const class Process* Prc - The process to generate for.
Output      : std::unique_ptr<std::vector<std::string>>& List - The updated file.
Output      : None.
Return      : None.
******************************************************************************/
void Makefile_Gen::Process_Proj(std::unique_ptr<std::vector<std::string>>& List,
                            const std::vector<std::string>& Include,
                            const std::vector<std::string>& Source,
                            const std::vector<std::string>& Linker,
                            const class Process* Prc)
{

}
/* End Function:Makefile_Gen::Process_Proj ***********************************/

/* Begin Function:Makefile_Gen::Workspace_Proj ********************************
Description : Generate workspace project.
Input       : std::unique_ptr<std::vector<std::string>>& List - The file.
              const std::vector<std::string>& Project - The project file list.
Output      : std::unique_ptr<std::vector<std::string>>& List - The updated file.
Output      : None.
Return      : None.
******************************************************************************/
void Makefile_Gen::Workspace_Proj(std::unique_ptr<std::vector<std::string>>& List,
                                  const std::vector<std::string>& Project)
{

}
/* End Function:Makefile_Gen::Workspace_Proj *********************************/
}
/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
