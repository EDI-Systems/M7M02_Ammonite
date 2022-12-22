/******************************************************************************
Filename    : proj_info.cpp
Author      : pry
Date        : 16/07/2019
Licence     : LGPL v3+; see COPYING for details.
Description : The project information main class.
******************************************************************************/

/* Includes ******************************************************************/
#include "map"
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
#include "Proj_Info/Process/process.hpp"
#include "Mem_Info/mem_info.hpp"
#undef __HDR_DEFS__

#define __HDR_CLASSES__
#include "rvm_gen.hpp"
#include "Proj_Info/proj_info.hpp"
#include "Proj_Info/Chip/chip.hpp"
#include "Proj_Info/Kernel/kernel.hpp"
#include "Proj_Info/Monitor/monitor.hpp"
#include "Proj_Info/Kobj/kobj.hpp"
#include "Proj_Info/Process/process.hpp"
#include "Proj_Info/Process/Captbl/captbl.hpp"
#include "Proj_Info/Process/Pgtbl/pgtbl.hpp"
#include "Proj_Info/Process/Thread/thread.hpp"
#include "Proj_Info/Process/Invocation/invocation.hpp"
#include "Proj_Info/Process/Port/port.hpp"
#include "Proj_Info/Process/Receive/receive.hpp"
#include "Proj_Info/Process/Send/send.hpp"
#include "Proj_Info/Process/Kfunc/kfunc.hpp"
#include "Proj_Info/Process/Virtual/virtual.hpp"
#include "Mem_Info/mem_info.hpp"
#include "Vect_Info/vect_info.hpp"
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

    try
    {
        /* Name */
        this->Name=Main::XML_Get_String(Root,"Name","DXXXX","DXXXX");
        Name_Gen(this);
        /* Version */
        this->Version=Main::XML_Get_String(Root,"Version","DXXXX","DXXXX");
        /* Assert_Correct */
        this->Assert_Correct=Main::XML_Get_Yesno(Root,"Assert_Correct","DXXXX","DXXXX");
        /* Debug_Print */
        this->Debug_Print=Main::XML_Get_Yesno(Root,"Debug_Print","DXXXX","DXXXX");
        /* Buildsystem */
        this->Buildsystem=Main::XML_Get_String(Root,"Buildsystem","DXXXX","DXXXX");
        /* Workspace_Output */
        this->Workspace_Output=Main::XML_Get_String(Root,"Workspace_Output","DXXXX","DXXXX");
        /* Workspace_Output */
        this->Workspace_Overwrite=Main::XML_Get_Yesno(Root,"Workspace_Overwrite","DXXXX","DXXXX");

        /* Chip */
        if((XML_Child(Root,(xml_s8_t*)"Chip",&Temp)<0)||(Temp==0))
            Main::Error("T1600: 'Property' section is missing.");
        this->Chip=std::make_unique<class Chip>(Temp);

        /* Extra memory segments - not necessarily exist */
        Trunk_Parse_Param<class Mem_Info,class Mem_Info,ptr_t>(Root,"Extmem",this->Extmem,MEM_DECL,"DXXXX","DXXXX");
        /* Shared memory segments - not necessarily exist */
        Trunk_Parse_Param<class Mem_Info,class Mem_Info,ptr_t>(Root,"Shmem",this->Shmem,MEM_DECL,"DXXXX","DXXXX");

        /* Kernel-related info */
        if((XML_Child(Root,(xml_s8_t*)"Kernel",&Temp)<0)||(Temp==0))
            Main::Error("T1600: 'Kernel' section is missing.");
        this->Kernel=std::make_unique<class Kernel>(Temp);

        /* VMM-related info */
        if((XML_Child(Root,(xml_s8_t*)"Monitor",&Temp)<0)||(Temp==0))
            Main::Error("T1600: 'Monitor' section is missing.");
        this->Monitor=std::make_unique<class Monitor>(Temp,
                                                      this->Kernel->Code_Base+this->Kernel->Code_Size,
                                                      this->Kernel->Data_Base+this->Kernel->Data_Size);

        /* Processes & VMs */
        Trunk_Parse_Param<class Process,class Process,ptr_t>(Root,"Process",this->Process,PROCESS_NATIVE,"DXXXX","DXXXX");
        Trunk_Parse<class Process,class Virtual>(Root,"Virtual",this->Process,"DXXXX","DXXXX");
        for(std::unique_ptr<class Process>& Prc:this->Process)
        {
            if(Prc->Type==PROCESS_VIRTUAL)
                this->Virtual.push_back(static_cast<class Virtual*>(Prc.get()));
        }
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

/* Begin Function:Proj_Info::Check ********************************************
Description : Check whether the project configuration makes sense.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
void Proj_Info::Check(void)
{
    try
    {
        /* External memory must have start addresses allocated */
        for(std::unique_ptr<class Mem_Info>& Mem:this->Extmem)
        {
            if(Mem->Base==MEM_AUTO)
                Main::Error("PXXXX: Externally mounted memory must have a concrete base address.");
        }

        /* Shared memory must have names, and they cannot be the same */
        for(std::unique_ptr<class Mem_Info>& Mem:this->Shmem)
        {
            if(Mem->Name=="")
                Main::Error("PXXXX: Shared memory declaration must contain a name.");
            Mem->Check();
            switch(Mem->Type)
            {
                case MEM_CODE:this->Shmem_Code.push_back(Mem.get());break;
                case MEM_DATA:this->Shmem_Data.push_back(Mem.get());break;
                case MEM_DEVICE:this->Shmem_Device.push_back(Mem.get());break;
                default:ASSERT(0);
            }
        }
        Duplicate_Check<class Mem_Info,std::string>(this->Shmem,this->Shmem_Map,
                                                    [](std::unique_ptr<class Mem_Info>& Mem)->std::string{return Mem->Name;},
                                                    "PXXXX","name","Shmem");

        /* Make sure shared memory declarations do not overlap */
        Mem_Info::Overlap_Check(this->Shmem_Code,this->Shmem_Data,this->Shmem_Device,"Shared memory");

        /* Must at least have one process; then check each process */
        if(this->Process.empty())
            Main::Error("PXXXX: The project contains no process.");

        /* Check for duplicate process names */
        Duplicate_Check<class Process,std::string>(this->Process,this->Process_Map,
                                                   [](std::unique_ptr<class Process>& Prc)->std::string{return Prc->Name;},
                                                   "PXXXX","name","Process");

        /* Check individual projects */
        for(std::unique_ptr<class Process>& Prc:this->Process)
            Prc->Check();
    }
    catch(std::exception& Exc)
    {
        if(this->Name!="")
            Main::Error(std::string("Project: ")+this->Name+"\n"+Exc.what());
        else
            Main::Error(std::string("Project: ")+"Unknown"+"\n"+Exc.what());
    }
}
/* End Function:Proj_Info::Check *********************************************/

/* Begin Function:Proj_Info::Flag_Alloc ***************************************
Description : Allocate the memory for vector/event flags. Flags are always like
              struct Flag
              {
                    ptr_t Lock;
                    ptr_t Group;
                    ptr_t Flag[1];
              };
              Each bit corresponds to a single vector. The whole struct will repeat twice.
Input       : ptr_t Source - The source number.
              ptr_t Wordlength - The processor wordlength.
              ptr_t Kom_Order - The kernel memory alignment order of 2.
Output      : None.
Return      : ptr_t - The vector flag sector size.
******************************************************************************/
ptr_t Proj_Info::Flag_Alloc(ptr_t Source, ptr_t Wordlength, ptr_t Kom_Order)
{
    ptr_t Raw;

    Raw=(ROUND_DIV(Source,Wordlength)+2)*(Wordlength/8);
    return ROUND_UP_POW2(Raw,Kom_Order)*2;
}
/* End Function:Proj_Info::Flag_Alloc ****************************************/
}
/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
