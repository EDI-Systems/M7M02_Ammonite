/******************************************************************************
Filename    : proj_info.cpp
Author      : pry
Date        : 16/07/2019
Licence     : LGPL v3+; see COPYING for details.
Description : The project information main class.
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
#include "algorithm"
#include "stdexcept"

#define __HDR_DEF__
#include "rvm_gen.hpp"
#include "Proj_Info/proj_info.hpp"
#include "Proj_Info/Process/process.hpp"
#include "Mem_Info/mem_info.hpp"
#undef __HDR_DEF__

#define __HDR_CLASS__
#include "rvm_gen.hpp"
#include "Proj_Info/proj_info.hpp"
#include "Proj_Info/Chip/chip.hpp"
#include "Proj_Info/Kernel/kernel.hpp"
#include "Proj_Info/Monitor/monitor.hpp"
#include "Proj_Info/Kobj/kobj.hpp"
#include "Proj_Info/Process/process.hpp"
#include "Proj_Info/Process/Shmem/shmem.hpp"
#include "Proj_Info/Process/Captbl/captbl.hpp"
#include "Proj_Info/Process/Pgtbl/pgtbl.hpp"
#include "Proj_Info/Process/Thread/thread.hpp"
#include "Proj_Info/Process/Invocation/invocation.hpp"
#include "Proj_Info/Process/Port/port.hpp"
#include "Proj_Info/Process/Receive/receive.hpp"
#include "Proj_Info/Process/Send/send.hpp"
#include "Proj_Info/Process/Kfunc/kfunc.hpp"
#include "Proj_Info/Process/Native/native.hpp"
#include "Proj_Info/Process/Virtual/virtual.hpp"
#include "Mem_Info/mem_info.hpp"
#include "Vect_Info/vect_info.hpp"
#undef __HDR_CLASS__
/* End Include ***************************************************************/
namespace RVM_GEN
{
/* Function:Proj_Info::Proj_Info **********************************************
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
        /* Assert_Enable */
        this->Assert_Enable=Main::XML_Get_Yesno(Root,"Assert_Enable","DXXXX","DXXXX");
        /* Debug_Log_Enable */
        this->Debug_Log_Enable=Main::XML_Get_Yesno(Root,"Debug_Log_Enable","DXXXX","DXXXX");
        /* Pgtbl_Raw_Enable */
        this->Pgtbl_Raw_Enable=Main::XML_Get_Yesno(Root,"Pgtbl_Raw_Enable","DXXXX","DXXXX");
        /* Buildsystem */
        this->Buildsystem=Main::XML_Get_String(Root,"Buildsystem","DXXXX","DXXXX");
        /* Workspace_Overwrite */
        this->Workspace_Overwrite=Main::XML_Get_Yesno(Root,"Workspace_Overwrite","DXXXX","DXXXX");

        /* Chip */
        if((XML_Child(Root,(xml_s8_t*)"Chip",&Temp)<0)||(Temp==0))
            Main::Error("XXXXX: 'Property' section is missing.");
        this->Chip=std::make_unique<class Chip>(Temp);

        /* Extra memory segments - not necessarily exist */
        Trunk_Parse_Param<class Mem_Info,class Mem_Info,ptr_t>(Root,"Extmem",this->Extmem,MEM_PHYS,"DXXXX","DXXXX");
        /* Shared memory segments - not necessarily exist */
        Trunk_Parse_Param<class Mem_Info,class Mem_Info,ptr_t>(Root,"Shmem",this->Shmem,MEM_SSEG,"DXXXX","DXXXX");

        /* Kernel-related info */
        if((XML_Child(Root,(xml_s8_t*)"Kernel",&Temp)<0)||(Temp==0))
            Main::Error("XXXXX: 'Kernel' section is missing.");
        this->Kernel=std::make_unique<class Kernel>(Temp);

        /* VMM-related info */
        if((XML_Child(Root,(xml_s8_t*)"Monitor",&Temp)<0)||(Temp==0))
            Main::Error("XXXXX: 'Monitor' section is missing.");
        this->Monitor=std::make_unique<class Monitor>(Temp,
                                                      this->Kernel->Code_Base+this->Kernel->Code_Size,
                                                      this->Kernel->Data_Base+this->Kernel->Data_Size);

        /* Natives & VMs */
        Trunk_Parse<class Process,class Native>(Root,"Native",this->Process,"DXXXX","DXXXX");
        Trunk_Parse<class Process,class Virtual>(Root,"Virtual",this->Process,"DXXXX","DXXXX");
        for(std::unique_ptr<class Process>& Prc:this->Process)
        {
            if(Prc->Type==PROCESS_NATIVE)
                this->Native.push_back(static_cast<class Native*>(Prc.get()));
            else
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

/* Function:Proj_Info::Check **************************************************
Description : Check whether the project configuration makes sense.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
void Proj_Info::Check(void)
{
    try
    {
        /* Check chip configs */
        this->Chip->Check();

        /* External memory checks */
        for(std::unique_ptr<class Mem_Info>& Mem:this->Extmem)
            Mem->Check();

        /* Shared memory checks */
        for(std::unique_ptr<class Mem_Info>& Mem:this->Shmem)
        {
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
            Main::Error("XXXXX: The project contains no process.");

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

/* Function:Proj_Info::Flag_Alloc *********************************************
Description : Allocate the memory for vector/event flags. Flags are always like
              struct Flag
              {
                    ptr_t Lock;
                    ptr_t Fast;
                    ptr_t Group;
                    ptr_t Flag[1];
              };
              Each bit corresponds to a single vector. The whole struct will
              repeat twice so we can support CPUs with no atomics at all.
              The "Fast" entry is unused in the event flags.
Input       : ptr_t Source - The source number.
              ptr_t Wordlength - The processor wordlength.
              ptr_t Kom_Order - The kernel memory alignment order of 2.
Output      : None.
Return      : ptr_t - The vector flag sector size.
******************************************************************************/
ptr_t Proj_Info::Flag_Alloc(ptr_t Source, ptr_t Wordlength, ptr_t Kom_Order)
{
    ptr_t Raw;

    Raw=(ROUND_DIV(Source,Wordlength)+3)*(Wordlength/8);

    /* Repeat the struct twice */
    return ROUND_UP_POW2(Raw,Kom_Order)*2;
}
/* End Function:Proj_Info::Flag_Alloc ****************************************/
}
/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
