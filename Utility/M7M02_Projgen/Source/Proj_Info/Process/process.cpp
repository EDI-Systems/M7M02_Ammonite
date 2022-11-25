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
        this->Buildsystem=Main::XML_Get_String(Root,"Buildsystem","DXXXX","DXXXX");
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
        Trunk_Parse_Param<class Mem_Info,class Mem_Info,ptr_t>(Root,"Memory",this->Memory,MEM_DECL,"DXXXX","DXXXX");
        /* Shmem */
        Trunk_Parse_Param<class Mem_Info,class Mem_Info,ptr_t>(Root,"Shmem",this->Shmem,MEM_REF,"DXXXX","DXXXX");
        /* Send */
        Trunk_Parse<class Send,class Send>(Root,"Send",this->Send,"DXXXX","DXXXX");
        /* These are present only if the process is native */
        if(Type==PROC_NATIVE)
        {
            /* Thread */
            Trunk_Parse<class Thread,class Thread>(Root,"Thread",this->Thread,"DXXXX","DXXXX");
            /* Invocation */
            Trunk_Parse<class Invocation,class Invocation>(Root,"Invocation",this->Invocation,"DXXXX","DXXXX");
            /* Port */
            Trunk_Parse<class Port,class Port>(Root,"Port",this->Port,"DXXXX","DXXXX");
            /* Receive */
            Trunk_Parse<class Receive,class Receive>(Root,"Receive",this->Receive,"DXXXX","DXXXX");
            /* Vector */
            Trunk_Parse<class Vect_Info,class Vect_Info>(Root,"Vector",this->Vector,"DXXXX","DXXXX");
            /* Kfunc */
            Trunk_Parse<class Kfunc,class Kfunc>(Root,"Kfunc",this->Kfunc,"DXXXX","DXXXX");
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

/* Begin Function:Process::Check **********************************************
Description : Check whether the process configuration makes sense.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
void Process::Check(void)
{
    try
    {
        /* Check memory layout */
        if(this->Memory.empty())
            Main::Error("PXXXX: The process contains no memory segments.");

        /* Classify memory - regular memory does not have name duplication requirements */
        for(std::unique_ptr<class Mem_Info>& Mem:this->Memory)
        {
            Mem->Check();
            switch(Mem->Type)
            {
                case MEM_CODE:this->Memory_Code.push_back(Mem.get());break;
                case MEM_DATA:this->Memory_Data.push_back(Mem.get());break;
                case MEM_DEVICE:this->Memory_Device.push_back(Mem.get());break;
                default:ASSERT(0);
            }
        }

        /* Every process must have at least one code and data segment, and they must be static.
         * The primary code segment allow RXS, the primary data segment must allow RWS */
        if(this->Memory_Code.empty())
            Main::Error("P0331: No primary code section exists.");
        if(this->Memory_Data.empty())
            Main::Error("P0332: No primary data section exists.");
        if((this->Memory_Code[0]->Attr&MEM_CODE_PRIME)!=MEM_CODE_PRIME)
            Main::Error("P0333: Primary code section does not have RXS attribute.");
        if((this->Memory_Data[0]->Attr&MEM_DATA_PRIME)!=MEM_DATA_PRIME)
            Main::Error("P0334: Primary data section does not have RWS attribute.");

        /* Make sure process memory declarations do not overlap */
        Mem_Info::Overlap_Check(this->Memory_Code,this->Memory_Data,this->Memory_Device,"Process memory");

        /* Check shared memory references */
        for(std::unique_ptr<class Mem_Info>& Mem:this->Shmem)
        {
            if(Mem->Name=="")
                Main::Error("Shared memory reference must contain a name.");
            Mem->Check();
        }
        Duplicate_Check<class Mem_Info,std::string>(this->Shmem,this->Shmem_Map,
                                                    [](std::unique_ptr<class Mem_Info>& Mem)->std::string{return Mem->Name;},
                                                    "PXXXX","name","Shmem");

        /* All normal processes shall have at least one thread */
        if((this->Type==PROC_NATIVE)&&(this->Thread.empty()))
            Main::Error("P0335: No thread exists in native process.");

        /* Check threads */
        Duplicate_Check<class Thread,std::string>(this->Thread,this->Thread_Map,
                                                  [](std::unique_ptr<class Thread>& Thd)->std::string{return Thd->Name;},
                                                  "PXXXX","name","Thread");

        /* Check invocations */
        Duplicate_Check<class Invocation,std::string>(this->Invocation,this->Invocation_Map,
                                                      [](std::unique_ptr<class Invocation>& Inv)->std::string{return Inv->Name;},
                                                      "PXXXX","name","Invocation");

        /* Check ports - both process and name must be the same for duplication */
        Duplicate_Check<class Port,std::string>(this->Port,this->Port_Map,
                                                [](std::unique_ptr<class Port>& Port)->std::string{return Port->Process+"_"+Port->Name;},
                                                "PXXXX","process/name pair","Port");

        /* Check receive endpoints */
        Duplicate_Check<class Receive,std::string>(this->Receive,this->Receive_Map,
                                                   [](std::unique_ptr<class Receive>& Recv)->std::string{return Recv->Name;},
                                                   "PXXXX","name","Receive");

        /* Check send endpoints - both process and name must be the same for duplication */
        Duplicate_Check<class Send,std::string>(this->Send,this->Send_Map,
                                                [](std::unique_ptr<class Send>& Send)->std::string{return Send->Process+"_"+Send->Name;},
                                                "PXXXX","process/name pair","Send");

        /* Check vectors - neither the name nor the number can be the same */
        Duplicate_Check<class Vect_Info,std::string>(this->Vector,this->Vector_Map,
                                                     [](std::unique_ptr<class Vect_Info>& Vect)->std::string{return Vect->Name;},
                                                     "PXXXX","name","Vector");
        Duplicate_Check<class Vect_Info,ptr_t>(this->Vector,this->Vector_Number_Map,
                                               [](std::unique_ptr<class Vect_Info>& Vect)->ptr_t{return Vect->Number;},
                                               "PXXXX","number","Vector");
    }
    catch(std::exception& Exc)
    {
        if(this->Name!="")
            Main::Error(std::string("Process: ")+this->Name+"\n"+Exc.what());
        else
            Main::Error(std::string("Process: ")+"Unknown"+"\n"+Exc.what());
    }
}
/* End Function:Process::Check *********************************************/
}
/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
