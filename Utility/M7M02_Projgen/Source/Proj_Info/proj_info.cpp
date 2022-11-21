/******************************************************************************
Filename    : rme_recv.cpp
Author      : pry
Date        : 16/07/2019
Licence     : LGPL v3+; see COPYING for details.
Description : The receive endpoint class.
******************************************************************************/

/* Includes ******************************************************************/
#include "string"
#include "memory"
#include "vector"
#include "algorithm"
#include "stdexcept"

extern "C"
{
#include "xml.h"
}

#define __HDR_DEFS__
#include "rvm_gen.hpp"
#include "Proj_Info/proj_info.hpp"
#undef __HDR_DEFS__

#define __HDR_CLASSES__
#include "rvm_gen.hpp"
#include "Proj_Info/proj_info.hpp"
#include "Proj_Info/Kernel/kernel.hpp"
#include "Proj_Info/Monitor/monitor.hpp"
#include "Proj_Info/Process/process.hpp"
#include "Proj_Info/Process/Virtual/virtual.hpp"
#include "Mem_Info/mem_info.hpp"
#undef __HDR_CLASSES__
/* End Includes **************************************************************/
namespace RVM_GEN
{
/* Begin Function:Proj_Info::Proj_Info ****************************************
Description : Constructor for project information class.
Input       : xml_node_t* Root - The node containing the whole project.
Output      : None.
Return      : None.
******************************************************************************/
/* void */ Proj_Info::Proj_Info(xml_node_t* Root)
{
    xml_node_t* Temp;
    xml_node_t* Trunk;
    xml_node_t* Mem_Type;
    std::unique_ptr<std::string> Str;
    std::unique_ptr<class Mem> Mem_Block;

    try
    {
        /* Name */
        this->Name=Main::XML_Get_String(Root,"Name","DXXXX","DXXXX");
        /* Version */
        this->Version=Main::XML_Get_String(Root,"Version","DXXXX","DXXXX");
        /* Chip */
        if((XML_Child(Root,(xml_s8_t*)"Chip",&Temp)<0)||(Temp==0))
            Main::Error("T1600: 'Property' section is missing.");
        this->Chip=std::make_unique<class Chip>(Temp);
        /* Extra memory segments - not necessarily exist */
        Parse_Trunk<class Mem_Info,class Mem_Info>(Root,"Extmem",this->Extmem,"DXXXX","DXXXX");
        /* Shared memory segments - not necessarily exist */
        Parse_Trunk<class Mem_Info,class Mem_Info>(Root,"Shmem",this->Shmem,"DXXXX","DXXXX");
        /* Kernel-related info */
        if((XML_Child(Root,(xml_s8_t*)"Kernel",&Temp)<0)||(Temp==0))
            Main::Error("T1600: 'Property' section is missing.");
        this->Kernel=std::make_unique<class Kernel>(Temp);
        /* Processes & VMs */
        Parse_Trunk<class Process,class Process>(Root,"Process",this->Process,"DXXXX","DXXXX");
        Parse_Trunk<class Process,class Virtual>(Root,"Virtual",this->Process,"DXXXX","DXXXX");
    }
    catch(std::exception& Exc)
    {
        if(this->Name!="")
            Main::Error(std::string("Project: ")+this->Name+"\n"+Exc.what());
        else
            Main::Error(std::string("Project: ")+"Unknown"+"\n"+Exc.what());
    }
}
/* End Function:Proj_Info::Proj_Info *****************************************/
}
/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
