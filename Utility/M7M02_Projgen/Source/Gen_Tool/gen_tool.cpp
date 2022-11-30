/******************************************************************************
Filename    : gen_tool.cpp
Author      : pry
Date        : 16/07/2019
Licence     : LGPL v3+; see COPYING for details.
Description : The memory block class. This contains the memory block information.
******************************************************************************/

/* Includes ******************************************************************/
#include "map"
#include "string"
#include "memory"
#include "vector"
#include "stdexcept"
#include "algorithm"

extern "C"
{
#include "xml.h"
}

#define __HDR_DEFS__
#include "rvm_gen.hpp"
#include "Gen_Tool/gen_tool.hpp"
#undef __HDR_DEFS__

#define __HDR_CLASSES__
#include "rvm_gen.hpp"
#include "Gen_Tool/gen_tool.hpp"

#include "Gen_Tool/Plat_Gen/plat_gen.hpp"
#include "Gen_Tool/Plat_Gen/A7M_Gen/a7m_gen.hpp"

#include "Gen_Tool/Build_Gen/build_gen.hpp"
#include "Gen_Tool/Build_Gen/Keil_Gen/keil_gen.hpp"
#include "Gen_Tool/Build_Gen/Makefile_Gen/makefile_gen.hpp"

#include "Gen_Tool/Tool_Gen/tool_gen.hpp"
#include "Gen_Tool/Tool_Gen/ARMCC_Gen/armcc_gen.hpp"
#include "Gen_Tool/Tool_Gen/GCC_Gen/gcc_gen.hpp"

#include "Gen_Tool/Guest_Gen/guest_gen.hpp"
#include "Gen_Tool/Guest_Gen/RMP_Gen/rmp_gen.hpp"
#undef __HDR_CLASSES__
/* End Includes **************************************************************/
namespace RVM_GEN
{
/* Begin Function:Gen_Tool::Gen_Tool ******************************************
Description : Constructor for generation tool class.
Input       : const std::string& Name - The name of the platform.
Input       : class Proj_Info* Proj - The project information.
              class Plat_Info* Plat - The platform information.
              class Chip_Info* Chip - The chip information.
Output      : None.
Return      : None.
******************************************************************************/
/* void */ Gen_Tool::Gen_Tool(const std::string& Name,
                              class Proj_Info* Proj, class Plat_Info* Plat, class Chip_Info* Chip)
{
    try
    {
        if(Name=="A7M")
            this->Plat=std::make_unique<class A7M_Gen>(Proj,Plat,Chip);
        else
            Main::Error("PXXXX: Platform generator for '"+Name+"' is not found.");
    }
    catch(std::exception& Exc)
    {
        Main::Error(std::string("Tool:\n")+Exc.what());
    }
}
/* End Function:Gen_Tool::Gen_Tool *******************************************/

/* Begin Function:Gen_Tool::Build_Load *****************************************
Description : Load one of the buildsystems.
Input       : const std::string& Name - The name of the buildsystem.
Output      : None.
Return      : None.
******************************************************************************/
void  Gen_Tool::Build_Load(const std::string& Name)
{
    std::unique_ptr<class Build_Gen> Build;

    try
    {
        if(this->Build_Map.find(Name)!=this->Build_Map.end())
            return;

        if(Name=="Keil")
            Build=std::make_unique<class Keil_Gen>(this->Plat->Proj,this->Plat->Plat,this->Plat->Chip);
        else if(Name=="Makefile")
            Build=std::make_unique<class Makefile_Gen>(this->Plat->Proj,this->Plat->Plat,this->Plat->Chip);
        else
            Main::Error("PXXXX: Buildsystem generator for '"+Name+"' is not found.");

        this->Build_Map.insert(std::make_pair(Name,Build.get()));
        this->Build.push_back(std::move(Build));
    }
    catch(std::exception& Exc)
    {
        Main::Error(std::string("Tool:\n")+Exc.what());
    }
}
/* End Function:Gen_Tool::Build_Load *****************************************/

/* Begin Function:Gen_Tool::Tool_Load *****************************************
Description : Load one of the build tools.
Input       : const std::string& Name - The name of the toolchain.
Output      : None.
Return      : None.
******************************************************************************/
void  Gen_Tool::Tool_Load(const std::string& Name)
{
    std::unique_ptr<class Tool_Gen> Tool;

    try
    {
        if(this->Tool_Map.find(Name)!=this->Tool_Map.end())
            return;

        if(Name=="ARMCC")
            Tool=std::make_unique<class ARMCC_Gen>(this->Plat->Proj,this->Plat->Plat,this->Plat->Chip);
        else if(Name=="GCC")
            Tool=std::make_unique<class GCC_Gen>(this->Plat->Proj,this->Plat->Plat,this->Plat->Chip);
        else
            Main::Error("PXXXX: Toolchain generator for '"+Name+"' is not found.");

        this->Tool_Map.insert(std::make_pair(Name,Tool.get()));
        this->Tool.push_back(std::move(Tool));
    }
    catch(std::exception& Exc)
    {
        Main::Error(std::string("Tool:\n")+Exc.what());
    }
}
/* End Function:Gen_Tool::Tool_Load ******************************************/

/* Begin Function:Gen_Tool::Guest_Load ****************************************
Description : Load one of the guest OSes.
Input       : const std::string& Name - The name of the guest OS.
Output      : None.
Return      : None.
******************************************************************************/
void  Gen_Tool::Guest_Load(const std::string& Name)
{
    std::unique_ptr<class Guest_Gen> Guest;

    try
    {
        if(this->Guest_Map.find(Name)!=this->Guest_Map.end())
            return;

        if(Name=="RMP")
            Guest=std::make_unique<class RMP_Gen>(this->Plat->Proj,this->Plat->Plat,this->Plat->Chip);
        else
            Main::Error("PXXXX: Guest generator for '"+Name+"' is not found.");

        this->Guest_Map.insert(std::make_pair(Name,Guest.get()));
        this->Guest.push_back(std::move(Guest));
    }
    catch(std::exception& Exc)
    {
        Main::Error(std::string("Tool:\n")+Exc.what());
    }
}
/* End Function:Gen_Tool::Guest_Load *****************************************/
}
/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
