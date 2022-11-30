/******************************************************************************
Filename    : build_gen.cpp
Author      : pry
Date        : 12/07/2019
Licence     : LGPL v3+; see COPYING for details.
Description : Generic buildsystem generator port.
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
#undef __HDR_DEFS__

#define __HDR_CLASSES__
#include "rvm_gen.hpp"
#include "Gen_Tool/Build_Gen/build_gen.hpp"
#undef __HDR_STRUCTS__
/* End Includes **************************************************************/
namespace RVM_GEN
{
/* Begin Function:Build_Gen::Build_Gen ****************************************
Description : Generator for the toolchain.
Input       : class Proj_Info* Proj - The project information.
              class Plat_Info* Plat - The platform information.
              class Chip_Info* Chip - The chip information.
Output      : None.
Return      : None.
******************************************************************************/
/* void */ Build_Gen::Build_Gen(const std::string& Name,
                                class Proj_Info* Proj, class Plat_Info* Plat, class Chip_Info* Chip)
{
    try
    {
        this->Name=Name;
        this->Proj=Proj;
        this->Plat=Plat;
        this->Chip=Chip;
    }
    catch(std::exception& Exc)
    {
        Main::Error(std::string("Buildsystem generator:\n")+Exc.what());
    }
}
/* End Function:Build_Gen::Build_Gen *****************************************/
}
/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
