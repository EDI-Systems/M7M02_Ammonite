/******************************************************************************
Filename    : armcc_gen.cpp
Author      : pry
Date        : 12/07/2019
Licence     : LGPL v3+; see COPYING for details.
Description : This toolset is for ARMCC, both v5 and v6; generates ARMCC linker scripts.
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
#include "stdexcept"
#include "algorithm"

#define __HDR_DEF__
#include "rvm_gen.hpp"
#include "Proj_Info/Process/process.hpp"
#include "Gen_Tool/Tool_Gen/tool_gen.hpp"
#include "Gen_Tool/Tool_Gen/ARMCC_Gen/armcc_gen.hpp"
#undef __HDR_DEF__

#define __HDR_CLASS__
#include "rvm_gen.hpp"
#include "Plat_Info/plat_info.hpp"
#include "Chip_Info/chip_info.hpp"
#include "Proj_Info/proj_info.hpp"
#include "Proj_Info/Kobj/kobj.hpp"
#include "Proj_Info/Chip/chip.hpp"
#include "Proj_Info/Kernel/kernel.hpp"
#include "Proj_Info/Monitor/monitor.hpp"
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
#include "Conf_Info/conf_info.hpp"

#include "Gen_Tool/gen_tool.hpp"
#include "Gen_Tool/Tool_Gen/tool_gen.hpp"
#include "Gen_Tool/Tool_Gen/ARMCC_Gen/armcc_gen.hpp"
#undef __HDR_CLASS__
/* End Include ***************************************************************/
namespace RVM_GEN
{
/* Function:ARMCC_Gen::ARMCC_Gen **********************************************
Description : Generator for the ARMv7-M platform.
Input       : class Proj_Info* Proj - The project information.
              class Plat_Info* Plat - The platform information.
              class Chip_Info* Chip - The chip information.
Output      : None.
Return      : None.
******************************************************************************/
/* void */ ARMCC_Gen::ARMCC_Gen(class Proj_Info* Proj, class Plat_Info* Plat, class Chip_Info* Chip):
Tool_Gen("ARMCC", Proj, Plat, Chip)
{
    try
    {

    }
    catch(std::exception& Exc)
    {
        Main::Error(std::string("ARMCC generator:\n")+Exc.what());
    }
}
/* End Function:ARMCC_Gen::ARMCC_Gen *****************************************/

/* Function:ARMCC_Gen::Suffix *************************************************
Description : Returns suffix for a given type of file.
Input       : ptr_t Type - The file type.
Output      : None.
Return      : std::string - The file suffix.
******************************************************************************/
std::string ARMCC_Gen::Suffix(ptr_t Type)
{
    if(Type==TOOL_LINKER)
        return ".sct";
    else if(Type==TOOL_ASSEMBLER)
        return ".s";
    else
        Main::Error("XXXXX: File type not recognized.");
}
/* End Function:ARMCC_Gen::Suffix ********************************************/

/* Function:ARMCC_Gen::Kernel_Linker ******************************************
Description : Generate the RME linker script.
Input       : std::unique_ptr<std::vector<std::string>>& List - The file.
Output      : std::unique_ptr<std::vector<std::string>>& List - The updated file.
Return      : None.
******************************************************************************/
void ARMCC_Gen::Kernel_Linker(std::unique_ptr<std::vector<std::string>>& List)
{
    List->push_back(";******************************************************************************");
    List->push_back(std::string(";Filename    : rme_platform_")+this->Chip->Name_Lower+".sct");
    List->push_back(std::string(";Author      : ")+CODE_AUTHOR);
    List->push_back(std::string(";Date        : ")+Main::Time);
    List->push_back(std::string(";Licence     : ")+CODE_LICENSE);
    List->push_back(";Description : The scatter file for ARMv7-M layout. This file is intended");
    List->push_back(std::string(";              to be used with ")+this->Chip->Name+".");
    List->push_back(";******************************************************************************");
    List->push_back("");
    List->push_back(";Kernel ***********************************************************************");
    List->push_back(std::string("KERNEL 0x")+Main::Hex(this->Proj->Kernel->Code_Base)+" 0x"+Main::Hex(this->Proj->Kernel->Code_Size));
    List->push_back("{");
    List->push_back("    ;Kernel code segment");
    List->push_back(std::string("    KERNEL_CODE 0x")+Main::Hex(this->Proj->Kernel->Code_Base)+" 0x"+Main::Hex(this->Proj->Kernel->Code_Size));
    List->push_back("    {");
    List->push_back("        ;The vector table");
    List->push_back("        *.o                            (RME_VECTOR,+First)");
    List->push_back("        ;The entry");
    List->push_back("        *.o                            (RME_ENTRY)");
    List->push_back("        ;The lib code copying code");
    List->push_back("        *                              (InRoot$$Sections)");
    List->push_back("        ;The ARM C library code - all of them is here");
    List->push_back("        *armlib*                       (+RO)");
    List->push_back("        ;The kernel code section");
    List->push_back("        .ANY                           (+RO)");
    List->push_back("    }");
    List->push_back("");
    List->push_back("    ;Statically allocated kernel data segment");
    List->push_back(std::string("    KERNEL_INIT 0x")+Main::Hex(this->Proj->Kernel->Data_Base)+" 0x"+Main::Hex(this->Proj->Kernel->Data_Size));
    List->push_back("    {");
    List->push_back("        .ANY                           (+RW +ZI)");
    List->push_back("    }");
    List->push_back("");
    List->push_back("    ;Dynamically allocated kernel data segment");
    List->push_back(std::string("    KERNEL_DATA 0x")+Main::Hex(this->Proj->Kernel->Kom_Base)+" EMPTY 0x"+Main::Hex(this->Proj->Kernel->Kom_Size));
    List->push_back("    {");
    List->push_back("");
    List->push_back("    }");
    List->push_back("");
    List->push_back("    ;Kernel stack segment - required by armlib");
    List->push_back(std::string("    ARM_LIB_STACK 0x")+Main::Hex(this->Proj->Kernel->Stack_Base)+" EMPTY 0x"+Main::Hex(this->Proj->Kernel->Stack_Size));
    List->push_back("    {");
    List->push_back("");
    List->push_back("    }");
    List->push_back("}");
    List->push_back(";End Kernel *******************************************************************");
    List->push_back("");
    /* See if the kernel project needs to generate a full image */
    if(this->Proj->Kernel->Full_Image!=0)
    {
        List->push_back(";Monitor **********************************************************************");
        List->push_back(std::string("MONITOR 0x")+Main::Hex(this->Proj->Monitor->Code_Base)+" 0x"+Main::Hex(this->Proj->Monitor->Code_Size));
        List->push_back("{");
        List->push_back("    ;Monitor image");
        List->push_back(std::string("    MONITOR_CODE 0x")+Main::Hex(this->Proj->Monitor->Code_Base)+" 0x"+Main::Hex(this->Proj->Monitor->Code_Size));
        List->push_back("    {");
        List->push_back("        monitor_image.o                    (+RO)");
        List->push_back("    }");
        List->push_back("}");
        List->push_back(";End Monitor ******************************************************************");
        List->push_back("");
        for(const class Process* Prc:this->Proj->Monitor->Process)
        {
            List->push_back(";Process **********************************************************************");
            List->push_back(Prc->Name_Upper+" 0x"+Main::Hex(Prc->Code_Base)+" 0x"+Main::Hex(Prc->Code_Size));
            List->push_back("{");
            List->push_back("    ;Process image");
            List->push_back(std::string("    ")+Prc->Name_Upper+"_CODE 0x"+Main::Hex(Prc->Code_Base)+" 0x"+Main::Hex(Prc->Code_Size));
            List->push_back("    {");
            List->push_back(std::string("        ")+Prc->Name_Lower+"_image.o                   (+RO)");
            List->push_back("    }");
            List->push_back("}");
            List->push_back(";End Process ******************************************************************");
            List->push_back("");
        }
    }
    List->push_back(";End Of File ******************************************************************");
    List->push_back("");
    List->push_back(";Copyright (C) Evo-Devo Instrum. All rights reserved **************************");
    List->push_back("");
}
/* End Function:ARMCC_Gen::Kernel_Linker *************************************/

/* Function:ARMCC_Gen::Monitor_Linker *****************************************
Description : Generate the RVM linker script.
Input       : std::unique_ptr<std::vector<std::string>>& List - The file.
Output      : std::unique_ptr<std::vector<std::string>>& List - The updated file.
Return      : None.
******************************************************************************/
void ARMCC_Gen::Monitor_Linker(std::unique_ptr<std::vector<std::string>>& List)
{
    List->push_back(";******************************************************************************");
    List->push_back(std::string(";Filename    : rvm_platform_")+this->Chip->Name_Lower+".sct");
    List->push_back(std::string(";Author      : ")+CODE_AUTHOR);
    List->push_back(std::string(";Date        : ")+Main::Time);
    List->push_back(std::string(";Licence     : ")+CODE_LICENSE);
    List->push_back(";Description : The scatter file for ARMv7-M layout. This file is intended");
    List->push_back(std::string(";              to be used with ")+this->Chip->Name+".");
    List->push_back(";******************************************************************************");
    List->push_back("");
    List->push_back(";Monitor **********************************************************************");
    List->push_back(std::string("MONITOR 0x")+Main::Hex(this->Proj->Monitor->Code_Base)+" 0x"+Main::Hex(this->Proj->Monitor->Code_Size));
    List->push_back("{");
    List->push_back("    ;Monitor code segment");
    List->push_back(std::string("    MONITOR_CODE 0x")+Main::Hex(this->Proj->Monitor->Code_Base)+" 0x"+Main::Hex(this->Proj->Monitor->Code_Size));
    List->push_back("    {");
    List->push_back("        ;The entry will be the first instruction");
    List->push_back("        *.o                    (RVM_ENTRY, +First)");
    List->push_back("        ;The lib code copying code");
    List->push_back("        *                      (InRoot$$Sections)");
    List->push_back("        ;The monitor code section");
    List->push_back("        .ANY                   (+RO)");
    List->push_back("    }");
    List->push_back("");
    List->push_back("    ;Monitor data segment");
    List->push_back(std::string("    MONITOR_DATA 0x")+Main::Hex(this->Proj->Monitor->Data_Base)+" 0x"+Main::Hex(this->Proj->Monitor->Data_Size));
    List->push_back("    {");
    List->push_back("        .ANY                   (+RW +ZI)");
    List->push_back("    }");
    List->push_back("");
    List->push_back("    ;Monitor stack segment - required by armlib; reusing the init thread's");
    List->push_back(std::string("    ARM_LIB_STACK 0x")+Main::Hex(this->Proj->Monitor->Init_Stack_Base)+" EMPTY 0x"+Main::Hex(this->Proj->Monitor->Init_Stack_Size));
    List->push_back("    {");
    List->push_back("");
    List->push_back("    }");
    List->push_back("}");
    List->push_back(";End Monitor ******************************************************************");
    List->push_back("");
    List->push_back(";End Of File ******************************************************************");
    List->push_back("");
    List->push_back(";Copyright (C) Evo-Devo Instrum. All rights reserved **************************");
    List->push_back("");
}
/* End Function:ARMCC_Gen::Monitor_Linker ************************************/

/* Function:ARMCC_Gen::Process_Linker *****************************************
Description : Generate the process linker script.
Input       : std::unique_ptr<std::vector<std::string>>& List - The file.
              const class Process* Prc - The process to generate for.
Output      : std::unique_ptr<std::vector<std::string>>& List - The updated file.
Return      : None.
******************************************************************************/
void ARMCC_Gen::Process_Linker(std::unique_ptr<std::vector<std::string>>& List,
                               const class Process* Prc)
{
    ptr_t Header_Size;
    ptr_t Real_Code_Size;
    std::string Tool_Lower;

    Tool_Lower=Prc->Toolchain;
    Main::Lower(Tool_Lower);
    Header_Size=Prc->Code_Front-Prc->Code_Base;
    Real_Code_Size=Prc->Code_Size-Header_Size;

    List->push_back(";******************************************************************************");
    List->push_back(std::string(";Filename    : proc_")+Prc->Name_Lower+"_"+this->Chip->Name_Lower+".sct");
    List->push_back(std::string(";Author      : ")+CODE_AUTHOR);
    List->push_back(std::string(";Date        : ")+Main::Time);
    List->push_back(std::string(";Licence     : ")+CODE_LICENSE);
    List->push_back(";Description : The scatter file for ARMv7-M layout. This file is intended");
    List->push_back(std::string(";              to be used with ")+this->Chip->Name+".");
    List->push_back(";******************************************************************************");
    List->push_back("");
    List->push_back(";Descriptor *******************************************************************");
    List->push_back(std::string("DESC 0x")+Main::Hex(Prc->Code_Base)+" 0x"+Main::Hex(Header_Size));
    List->push_back("{");
    List->push_back("    ;Process header segment");
    List->push_back(std::string("    PROCESS_DESC 0x")+Main::Hex(Prc->Code_Base)+" 0x"+Main::Hex(Header_Size));
    List->push_back("    {");
    List->push_back("        ;Process header");
    List->push_back(std::string("        prc_")+Prc->Name_Lower+"_desc.o         (+RO)");
    List->push_back("    }");
    List->push_back("}");
    List->push_back(";End Descriptor ***************************************************************");
    List->push_back("");
    List->push_back(";Process **********************************************************************");
    List->push_back(std::string("PROCESS 0x")+Main::Hex(Prc->Code_Front)+" 0x"+Main::Hex(Real_Code_Size));
    List->push_back("{");
    List->push_back("    ;Process code segment");
    List->push_back(std::string("    PROCESS_CODE 0x")+Main::Hex(Prc->Code_Front)+" 0x"+Main::Hex(Real_Code_Size));
    List->push_back("    {");
    List->push_back("        ;Entry point assembly");
    List->push_back(std::string("        rvm_guest_")+this->Plat->Name_Lower+"_"+Tool_Lower+".o     (RVM_ENTRY, +First)");
    List->push_back("        ;The lib code copying code");
    List->push_back("        *                      (InRoot$$Sections)");
    List->push_back("        ;The other code sections");
    List->push_back("        .ANY                   (+RO)");
    List->push_back("    }");
    List->push_back("");
    List->push_back("    ;Process data segment");
    List->push_back(std::string("    PROCESS_DATA 0x")+Main::Hex(Prc->Data_Base)+" 0x"+Main::Hex(Prc->Data_Size));
    List->push_back("    {");
    List->push_back("        .ANY                   (+RW +ZI)");
    List->push_back("    }");
    List->push_back("");
    List->push_back("    ;Process stack segment - required by armlib; reusing the booting thread's");
    if(Prc->Type==PROCESS_NATIVE)
        List->push_back(std::string("    ARM_LIB_STACK 0x")+Main::Hex(Prc->Thread[0]->Stack_Base)+" EMPTY 0x"+Main::Hex(Prc->Thread[0]->Stack_Size));
    else
    {
        ASSERT((Prc->Thread[0]->Name=="Vct")&&(Prc->Thread[1]->Name=="Usr"));
        List->push_back(std::string("    ARM_LIB_STACK 0x")+Main::Hex(Prc->Thread[1]->Stack_Base)+" EMPTY 0x"+Main::Hex(Prc->Thread[1]->Stack_Size));
    }
    List->push_back("    {");
    List->push_back("");
    List->push_back("    }");
    List->push_back("}");
    List->push_back(";End Process ******************************************************************");
    List->push_back("");
    List->push_back(";End Of File ******************************************************************");
    List->push_back("");
    List->push_back(";Copyright (C) Evo-Devo Instrum. All rights reserved **************************");
    List->push_back("");
}
/* End Function:ARMCC_Gen::Process_Linker ************************************/
}
/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
