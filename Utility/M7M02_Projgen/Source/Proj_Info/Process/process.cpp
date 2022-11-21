/******************************************************************************
Filename    : process.cpp
Author      : pry
Date        : 16/07/2019
Licence     : LGPL v3+; see COPYING for details.
Description : The process class.
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
#include "Mem_Info/mem_info.hpp"
#include "Proj_Info/Process/process.hpp"
#undef __HDR_DEFS__

#define __HDR_CLASSES__
#include "rvm_gen.hpp"
#include "Proj_Info/Process/process.hpp"
#include "Proj_Info/Process/Thread/thread.hpp"
#include "Proj_Info/Process/Invocation/invocation.hpp"
#include "Proj_Info/Process/Port/port.hpp"
#include "Proj_Info/Process/Receive/receive.hpp"
#include "Proj_Info/Process/Send/send.hpp"
#include "Proj_Info/Process/Kfunc/kfunc.hpp"
#include "Mem_Info/mem_info.hpp"
#include "Vect_Info/vect_info.hpp"
#undef __HDR_CLASSES__
/* End Includes **************************************************************/
namespace RVM_GEN
{
/* Begin Function:Process::Process ********************************************
Description : Constructor for process class.
Input       : xml_node_t* Root - The node containing the process information.
              ptr_t Type - The process type.
Output      : None.
Return      : None.
******************************************************************************/
/* void */ Process::Process(xml_node_t* Root, ptr_t Type)
{
    try
    {
        /* The type of this process is by default a native process */
        this->Type=Type;
        /* Name */
        this->Name=Main::XML_Get_String(Root,"Name","DXXXX","DXXXX");
        /* Extra_Captbl */
        this->Extra_Captbl=Main::XML_Get_Number(Root,"Extra_Captbl","DXXXX","DXXXX");

        /* Build */
        this->Build=Main::XML_Get_String(Root,"Build","DXXXX","DXXXX");
        /* Toolchain */
        this->Toolchain=Main::XML_Get_String(Root,"Toolchain","DXXXX","DXXXX");
        /* Optimization */
        this->Optimization=Main::XML_Get_String(Root,"Optimization","DXXXX","DXXXX");
        /* Project_Output */
        this->Project_Output=Main::XML_Get_String(Root,"Project_Output","DXXXX","DXXXX");
        /* Project_Overwrite */
        this->Project_Overwrite=Main::XML_Get_Yesno(Root,"Project_Overwrite","DXXXX","DXXXX");
        /* Linker_Output */
        this->Linker_Output=Main::XML_Get_String(Root,"Linker_Output","DXXXX","DXXXX");
        /* Init_Source_Output */
        this->Init_Source_Output=Main::XML_Get_String(Root,"Init_Source_Output","DXXXX","DXXXX");
        /* Init_Source_Output */
        this->Init_Source_Overwrite=Main::XML_Get_Yesno(Root,"Init_Source_Overwrite","DXXXX","DXXXX");

        /* Memory */
        Parse_Trunk_Param<class Mem_Info,class Mem_Info,ptr_t>(Root,"Memory",this->Memory,MEM_DECL,"DXXXX","DXXXX");
        /* Shmem */
        Parse_Trunk_Param<class Mem_Info,class Mem_Info,ptr_t>(Root,"Shmem",this->Shmem,MEM_REF,"DXXXX","DXXXX");
        /* Send */
        Parse_Trunk<class Send,class Send>(Root,"Send",this->Send,"DXXXX","DXXXX");
        /* These are present only if the process is native */
        if(Type==PROC_NATIVE)
        {
            /* Thread */
            Parse_Trunk<class Thread,class Thread>(Root,"Thread",this->Thread,"DXXXX","DXXXX");
            /* Invocation */
            Parse_Trunk<class Invocation,class Invocation>(Root,"Invocation",this->Invocation,"DXXXX","DXXXX");
            /* Port */
            Parse_Trunk<class Port,class Port>(Root,"Port",this->Port,"DXXXX","DXXXX");
            /* Receive */
            Parse_Trunk<class Receive,class Receive>(Root,"Receive",this->Receive,"DXXXX","DXXXX");
            /* Vector */
            Parse_Trunk<class Vect_Info,class Vect_Info>(Root,"Vector",this->Vector,"DXXXX","DXXXX");
            /* Kfunc */
            Parse_Trunk<class Kfunc,class Kfunc>(Root,"Kfunc",this->Kfunc,"DXXXX","DXXXX");
        }
    }
    catch(std::exception& Exc)
    {
        if(this->Name!="")
            Main::Error(std::string("Process: ")+this->Name+"\n"+Exc.what());
        else
            Main::Error(std::string("Process: ")+"Unknown"+"\n"+Exc.what());
    }
}
/* End Function:Process::Process *********************************************/
}
/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
