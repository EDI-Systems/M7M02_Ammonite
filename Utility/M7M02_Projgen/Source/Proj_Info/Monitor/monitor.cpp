/******************************************************************************
Filename    : monitor.cpp
Author      : pry
Date        : 16/07/2019
Licence     : LGPL v3+; see COPYING for details.
Description : The virtual machine monitor class.
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
#include "Proj_Info/Monitor/monitor.hpp"
#undef __HDR_DEFS__

#define __HDR_CLASSES__
#include "rvm_gen.hpp"
#include "Proj_Info/Monitor/monitor.hpp"
#undef __HDR_CLASSES__
/* End Includes **************************************************************/
namespace RVM_GEN
{
/* Begin Function:Monitor::Monitor ********************************************
Description : Constructor for RVM class.
Input       : xml_node_t* Root - The node containing the whole project.
Output      : None.
Return      : None.
******************************************************************************/
/* void */ Monitor::Monitor(xml_node_t* Root)
{
    try
    {
        /* Code size */
        this->Code_Size=Main::XML_Get_Number(Root,"Code_Size","DXXXX","DXXXX");
        /* Data size */
        this->Data_Size=Main::XML_Get_Number(Root,"Data_Size","DXXXX","DXXXX");
        /* Stack size */
        this->Stack_Size=Main::XML_Get_Number(Root,"Stack_Size","DXXXX","DXXXX");
        /* Virtual machine priorities */
        this->Virt_Prio=Main::XML_Get_Number(Root,"Virt_Prio","DXXXX","DXXXX");
        /* Virtual machine events */
        this->Virt_Evt=Main::XML_Get_Number(Root,"Virt_Evt","DXXXX","DXXXX");
        /* Virtual machine mappings */
        this->Virt_Map=Main::XML_Get_Number(Root,"Virt_Map","DXXXX","DXXXX");

        /* Build */
        this->Build=Main::XML_Get_String(Root,"Build","DXXXX","DXXXX");
        /* Toolchain */
        this->Toolchain=Main::XML_Get_String(Root,"Toolchain","DXXXX","DXXXX");
        /* Optimization */
        this->Optimization=Main::XML_Get_String(Root,"Optimization","DXXXX","DXXXX");
        /* Monitor_Root */
        this->Monitor_Root=Main::XML_Get_String(Root,"Monitor_Root","DXXXX","DXXXX");
        /* Project_Output */
        this->Project_Output=Main::XML_Get_String(Root,"Project_Output","DXXXX","DXXXX");
        /* Project_Overwrite */
        this->Project_Overwrite=Main::XML_Get_Yesno(Root,"Project_Overwrite","DXXXX","DXXXX");
        /* Linker_Output */
        this->Linker_Output=Main::XML_Get_String(Root,"Linker_Output","DXXXX","DXXXX");
        /* Config_Header_Output */
        this->Config_Header_Output=Main::XML_Get_String(Root,"Config_Header_Output","DXXXX","DXXXX");
        /* Boot_Header_Output */
        this->Boot_Header_Output=Main::XML_Get_String(Root,"Boot_Header_Output","DXXXX","DXXXX");
        /* Boot_Source_Output */
        this->Boot_Source_Output=Main::XML_Get_String(Root,"Boot_Source_Output","DXXXX","DXXXX");
        /* Init_Source_Output */
        this->Init_Source_Output=Main::XML_Get_String(Root,"Init_Source_Output","DXXXX","DXXXX");
        /* Init_Source_Output */
        this->Init_Source_Overwrite=Main::XML_Get_Yesno(Root,"Init_Source_Overwrite","DXXXX","DXXXX");
    }
    catch(std::exception& Exc)
    {
        Main::Error(std::string("Monitor:\n")+Exc.what());
    }
}
/* End Function:Monitor::Monitor *********************************************/
}
/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
