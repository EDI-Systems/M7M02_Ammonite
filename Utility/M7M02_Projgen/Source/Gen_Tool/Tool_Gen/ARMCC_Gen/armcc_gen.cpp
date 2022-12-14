/******************************************************************************
Filename    : armcc_gen.cpp
Author      : pry
Date        : 12/07/2019
Licence     : LGPL v3+; see COPYING for details.
Description : This toolset is for ARMCC, both v5 and v6; generates ARMCC linker scripts.
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
#include "Gen_Tool/Tool_Gen/tool_gen.hpp"
#include "Gen_Tool/Tool_Gen/ARMCC_Gen/armcc_gen.hpp"
#undef __HDR_DEFS__

#define __HDR_CLASSES__
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
#undef __HDR_STRUCTS__
/* End Includes **************************************************************/
namespace RVM_GEN
{
/* Begin Function:ARMCC_Gen::ARMCC_Gen ****************************************
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

/* Begin Function:ARMCC_Gen::Suffix *******************************************
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
        Main::Error("AXXXX: File type not recognized.");
}
/* End Function:ARMCC_Gen::Suffix ********************************************/

/* Begin Function:ARMCC_Gen::Kernel_Linker ************************************
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
    List->push_back("; Begin Segment:KERNEL ********************************************************");
    List->push_back("; Description : The kernel segment, this is where the kernel of the operating");
    List->push_back(";               system is located at. This segment also contains the .bss(ZI)");
    List->push_back(";               for processes.");
    List->push_back("; *****************************************************************************");
    List->push_back(std::string("KERNEL 0x")+Main::Hex(this->Proj->Kernel->Code_Base)+" 0x"+Main::Hex(this->Proj->Kernel->Code_Size));
    List->push_back("{");
    List->push_back("    ; Kernel code segment");
    List->push_back(std::string("    KERNEL_CODE 0x")+Main::Hex(this->Proj->Kernel->Code_Base)+" 0x"+Main::Hex(this->Proj->Kernel->Code_Size));
    List->push_back("    {");
    List->push_back("        ; The reset vectors");
    List->push_back("        *.o                            (RESET, +First)");
    List->push_back("        ; The lib code copying code");
    List->push_back("        *                              (InRoot$$Sections)");
    List->push_back("        ; The ARM C library code - all of them is here");
    List->push_back("        *armlib*                       (+RO)");
    List->push_back("        ; The kernel code section");
    List->push_back("        .ANY                           (+RO)");
    List->push_back("    }");
    List->push_back("");
    List->push_back("    ; Kernel stack segment");
    List->push_back(std::string("    KERNEL_STACK 0x")+Main::Hex(this->Proj->Kernel->Stack_Base)+" 0x"+Main::Hex(this->Proj->Kernel->Stack_Size));
    List->push_back("    {");
    List->push_back("        rme_platform_*_asm.o             (HEAP)");
    List->push_back("        rme_platform_*_asm.o             (STACK)");
    List->push_back("    }");
    List->push_back("");
    List->push_back("    ; Initial kernel data segment");
    List->push_back(std::string("    KERNEL_INIT 0x")+Main::Hex(this->Proj->Kernel->Data_Base)+" 0x"+Main::Hex(this->Proj->Kernel->Data_Size));
    List->push_back("    {");
    List->push_back("        .ANY                           (+RW +ZI)");
    List->push_back("    }");
    List->push_back("");
    List->push_back("    ; Dynamically managed kernel data segment");
    List->push_back(std::string("    KERNEL_DATA 0x")+Main::Hex(this->Proj->Kernel->Kmem_Base)+" EMPTY 0x"+Main::Hex(this->Proj->Kernel->Kmem_Size));
    List->push_back("    {");
    List->push_back("");
    List->push_back("    }");
    List->push_back("}");
    List->push_back("; End Segment:KERNEL **********************************************************");
    List->push_back("");
    List->push_back("; Begin Segment:MONITOR *******************************************************");
    List->push_back("; Description : The .text segment of process MONITOR, which is the first process");
    List->push_back(";               in the system, and should act as a memory manager itself.");
    List->push_back("; *****************************************************************************");
    List->push_back(std::string("MONITOR 0x")+Main::Hex(this->Proj->Monitor->Code_Base)+" 0x"+Main::Hex(this->Proj->Monitor->Code_Size));
    List->push_back("{");
    List->push_back("    ; Init process code segment");
    List->push_back(std::string("    MONITOR_CODE 0x")+Main::Hex(this->Proj->Monitor->Code_Base)+" 0x"+Main::Hex(this->Proj->Monitor->Code_Size));
    List->push_back("    {");
    List->push_back("        RVM.o                          (+RO)");
    List->push_back("    }");
    List->push_back("}");
    List->push_back("; End Segment:MONITOR *********************************************************");
    List->push_back("");

    for(const class Process* Proc:this->Proj->Monitor->Process)
    {
        List->push_back("; Begin Segment:Proc **********************************************************");
        List->push_back("; Description : The .text segment of the virtual machines. All virtual machines");
        List->push_back("; needs to be placed here; If you don't want to convert them to C and compile to");
        List->push_back("; a whole image, flash them to this address.");
        List->push_back("; *****************************************************************************");
        List->push_back(Proc->Name_Upper+" 0x"+Main::Hex(Proc->Code_Base)+" 0x"+Main::Hex(Proc->Code_Size));
        List->push_back("{");
        List->push_back("    ; Process code segment");
        List->push_back(std::string("    ")+Proc->Name_Upper+"_CODE 0x"+Main::Hex(Proc->Code_Base)+" 0x"+Main::Hex(Proc->Code_Size));
        List->push_back("    {");
        List->push_back(std::string("        ")+Proc->Name_Lower+".o                         (+RO)");
        List->push_back("    }");
        List->push_back("}");
        List->push_back("; End Segment:Proc ************************************************************");
        List->push_back("");
    }

    List->push_back("; End Of File *****************************************************************");
    List->push_back("");
    List->push_back("; Copyright (C) Evo-Devo Instrum. All rights reserved *************************");
    List->push_back("");
}
/* End Function:ARMCC_Gen::Kernel_Linker *************************************/

/* Begin Function:ARMCC_Gen::Monitor_Linker ***********************************
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
    List->push_back("; Begin Segment:MONITOR *******************************************************");
    List->push_back("; Description : The init segment, which contains the core of the user-level library.");
    List->push_back("; *****************************************************************************");
    List->push_back(std::string("MONITOR 0x")+Main::Hex(this->Proj->Monitor->Code_Base)+" 0x"+Main::Hex(this->Proj->Monitor->Code_Size));
    List->push_back("{");
    List->push_back("    ; The code segment of the monitor process");
    List->push_back(std::string("    MONITOR_CODE 0x")+Main::Hex(this->Proj->Monitor->Code_Base)+" 0x"+Main::Hex(this->Proj->Monitor->Code_Size));
    List->push_back("    {");
    List->push_back("        ; The entry will be the first instruction");
    List->push_back("        *.o                    (ARCH, +First)");
    List->push_back("        ; The lib code copying code");
    List->push_back("        *                      (InRoot$$Sections)");
    List->push_back("        ; The init code section");
    List->push_back("        .ANY                   (+RO)");
    List->push_back("    }");
    List->push_back("");
    List->push_back("    ; The data section of the monitor process");
    List->push_back(std::string("    MONITOR_DATA 0x")+Main::Hex(this->Proj->Monitor->Data_Base)+" 0x"+Main::Hex(this->Proj->Monitor->Data_Size));
    List->push_back("    {");
    List->push_back("        .ANY                   (+RW +ZI)");
    List->push_back("    }");
    List->push_back("}");
    List->push_back("; End Segment:INIT ************************************************************");
    List->push_back("");
    List->push_back("; End Of File *****************************************************************");
    List->push_back("");
    List->push_back("; Copyright (C) Evo-Devo Instrum. All rights reserved *************************");
    List->push_back("");
}
/* End Function:ARMCC_Gen::Monitor_Linker ************************************/

/* Begin Function:ARMCC_Gen::Process_Linker ***********************************
Description : Generate the process linker script.
Input       : std::unique_ptr<std::vector<std::string>>& List - The file.
              const class Process* Proc - The process to generate for.
Output      : std::unique_ptr<std::vector<std::string>>& List - The updated file.
Return      : None.
******************************************************************************/
void ARMCC_Gen::Process_Linker(std::unique_ptr<std::vector<std::string>>& List,
                               const class Process* Proc)
{
    ptr_t Header_Size;
    ptr_t Real_Code_Size;
    std::string Tool_Lower;

    Tool_Lower=Proc->Toolchain;
    Main::Lower(Tool_Lower);
    Header_Size=Proc->Code_Front-Proc->Code_Base;
    Real_Code_Size=Proc->Code_Size-Header_Size;

    List->push_back(";******************************************************************************");
    List->push_back(std::string(";Filename    : proc_")+Proc->Name_Lower+"_"+this->Chip->Name_Lower+".sct");
    List->push_back(std::string(";Author      : ")+CODE_AUTHOR);
    List->push_back(std::string(";Date        : ")+Main::Time);
    List->push_back(std::string(";Licence     : ")+CODE_LICENSE);
    List->push_back(";Description : The scatter file for ARMv7-M layout. This file is intended");
    List->push_back(std::string(";              to be used with ")+this->Chip->Name+".");
    List->push_back(";******************************************************************************");
    List->push_back("");
    List->push_back("; Begin Segment:DESC **********************************************************");
    List->push_back("; Description : The process header segment that describe process info.");
    List->push_back("; *****************************************************************************");
    List->push_back(std::string("DESC 0x")+Main::Hex(Proc->Code_Base)+" 0x"+Main::Hex(Header_Size));
    List->push_back("{");
    List->push_back("    ; The header segment of the process");
    List->push_back(std::string("    PROC_DESC 0x")+Main::Hex(Proc->Code_Base)+" 0x"+Main::Hex(Header_Size));
    List->push_back("    {");
    List->push_back("        ; Process header");
    List->push_back(std::string("        proc_")+Proc->Name_Lower+"_desc.o         (+RO)");
    List->push_back("    }");
    List->push_back("}");
    List->push_back("; End Segment:DESC ************************************************************");
    List->push_back("");
    List->push_back("; Begin Segment:PROC **********************************************************");
    List->push_back("; Description : The process segment, where the process executable is located at.");
    List->push_back("; *****************************************************************************");
    List->push_back(std::string("PROC 0x")+Main::Hex(Proc->Code_Front)+" 0x"+Main::Hex(Real_Code_Size));
    List->push_back("{");
    List->push_back("    ; The code segment of the process");
    List->push_back(std::string("    PROC_CODE 0x")+Main::Hex(Proc->Code_Front)+" 0x"+Main::Hex(Real_Code_Size));
    List->push_back("    {");
    List->push_back("        ; Entry point assembly");
    List->push_back(std::string("        rvm_guest_")+this->Plat->Name_Lower+"_"+Tool_Lower+".o     (RESET, +First)");
    List->push_back("        ; The lib code copying code");
    List->push_back("        *                      (InRoot$$Sections)");
    List->push_back("        ; The init code section");
    List->push_back("        .ANY                   (+RO)");
    List->push_back("    }");
    List->push_back("");
    List->push_back("    ; The data section of the process");
    List->push_back(std::string("    PROC_DATA 0x")+Main::Hex(Proc->Data_Base)+" 0x"+Main::Hex(Proc->Data_Size));
    List->push_back("    {");
    List->push_back("        .ANY                   (+RW +ZI)");
    List->push_back("    }");
    List->push_back("}");
    List->push_back("; End Segment:PROC ************************************************************");
    List->push_back("");
    List->push_back("; End Of File *****************************************************************");
    List->push_back("");
    List->push_back("; Copyright (C) Evo-Devo Instrum. All rights reserved *************************");
    List->push_back("");
}
/* End Function:ARMCC_Gen::Process_Linker ************************************/
}
/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
