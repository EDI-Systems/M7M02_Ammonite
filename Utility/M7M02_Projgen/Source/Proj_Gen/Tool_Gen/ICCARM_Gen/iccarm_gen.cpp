/******************************************************************************
Filename    : iccarm_gen.cpp
Author      : pry ; Cline
Date        : 21/10/2026
Licence     : LGPL v3+; see COPYING for details.
Description : This toolset is for IAR ICCARM; generates IAR ILINK linker
              configuration files. The layout mirrors ARMCC_Gen's scatter files
              exactly; only the syntax differs.
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
#include "Proj_Gen/Tool_Gen/tool_gen.hpp"
#include "Proj_Gen/Tool_Gen/ICCARM_Gen/iccarm_gen.hpp"
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

#include "Proj_Gen/proj_gen.hpp"
#include "Proj_Gen/Tool_Gen/tool_gen.hpp"
#include "Proj_Gen/Tool_Gen/ICCARM_Gen/iccarm_gen.hpp"
#undef __HDR_CLASS__
/* End Include ***************************************************************/
namespace RVM_GEN
{
/* Function:ICCARM_Gen::ICCARM_Gen ********************************************
Description : Generator for the IAR ICCARM toolchain.
Input       : class Proj_Info* Proj - The project information.
              class Plat_Info* Plat - The platform information.
              class Chip_Info* Chip - The chip information.
Output      : None.
Return      : None.
******************************************************************************/
/* void */ ICCARM_Gen::ICCARM_Gen(class Proj_Info* Proj, class Plat_Info* Plat, class Chip_Info* Chip):
Tool_Gen("ICCARM", Proj, Plat, Chip)
{
    try
    {

    }
    catch(std::exception& Exc)
    {
        Main::Error(std::string("ICCARM generator:\n")+Exc.what());
    }
}
/* End Function:ICCARM_Gen::ICCARM_Gen ***************************************/

/* Function:ICCARM_Gen::Suffix *************************************************
Description : Returns suffix for a given type of file.
Input       : ptr_t Type - The file type.
Output      : None.
Return      : std::string - The file suffix.
******************************************************************************/
std::string ICCARM_Gen::Suffix(ptr_t Type)
{
    if(Type==TOOL_LINKER)
        return ".icf";
    else if(Type==TOOL_ASSEMBLER)
        return ".s";
    else
        Main::Error("XXXXX: File type not recognized.");
}
/* End Function:ICCARM_Gen::Suffix *******************************************/

/* Function:ICCARM_Gen::Kernel_Linker ******************************************
Description : Generate the RME linker configuration file.
Input       : std::unique_ptr<std::vector<std::string>>& List - The file.
Output      : std::unique_ptr<std::vector<std::string>>& List - The updated file.
Return      : None.
******************************************************************************/
void ICCARM_Gen::Kernel_Linker(std::unique_ptr<std::vector<std::string>>& List)
{
    List->push_back("/* ****************************************************************************");
    List->push_back(std::string("Filename    : rme_platform_")+this->Chip->Name_Lower+".icf");
    List->push_back(std::string("Author      : ")+CODE_AUTHOR);
    List->push_back(std::string("Date        : ")+Main::Time);
    List->push_back(std::string("Licence     : ")+CODE_LICENSE);
    List->push_back("Description : The IAR ILINK configuration file for ARMv6-M layout.");
    List->push_back(std::string("              to be used with ")+this->Chip->Name+", and the IAR toolchain.");
    List->push_back("**************************************************************************** */");
    List->push_back("");
    List->push_back("define memory mem with size = 4G;");
    List->push_back("");
    List->push_back("/* Kernel code region */");
    List->push_back(std::string("define region KCODE = mem:[from 0x")+Main::Hex(this->Proj->Kernel->Code_Base)+
                    " to 0x"+Main::Hex(this->Proj->Kernel->Code_Base+this->Proj->Kernel->Code_Size-1)+"];");
    List->push_back("/* Statically allocated kernel data region */");
    List->push_back(std::string("define region KDATA = mem:[from 0x")+Main::Hex(this->Proj->Kernel->Data_Base)+
                    " to 0x"+Main::Hex(this->Proj->Kernel->Data_Base+this->Proj->Kernel->Data_Size-1)+"];");
    List->push_back("/* Dynamically allocated kernel data region - managed by the kernel itself */");
    List->push_back(std::string("define region KMEM = mem:[from 0x")+Main::Hex(this->Proj->Kernel->Kom_Base)+
                    " to 0x"+Main::Hex(this->Proj->Kernel->Kom_Base+this->Proj->Kernel->Kom_Size-1)+"];");
    List->push_back("/* Kernel stack - the initial stack pointer comes from here */");
    List->push_back(std::string("define block CSTACK with alignment = 8, size = 0x")+
                    Main::Hex(this->Proj->Kernel->Stack_Size)+" { };");
    List->push_back("");
    List->push_back("initialize by copy { readwrite };");
    List->push_back("");
    List->push_back("/* The vector table must be the first thing in the kernel ROM */");
    List->push_back(std::string("place at address mem:0x")+Main::Hex(this->Proj->Kernel->Code_Base)+
                    " { readonly section .rme_vector };");
    List->push_back("keep { readonly section .rme_vector };");
    List->push_back("/* All other readonly content goes to the kernel code region */");
    List->push_back("place in KCODE { readonly };");
    List->push_back("/* All readwrite content goes to the kernel data region */");
    List->push_back("place in KDATA { readwrite };");
    List->push_back("/* Kernel stack at its reserved physical location */");
    List->push_back(std::string("place at address mem:0x")+Main::Hex(this->Proj->Kernel->Stack_Base)+
                    " { block CSTACK };");
    /* See if the kernel project needs to generate a full image */
    if(this->Proj->Kernel->Full_Image!=0)
    {
        List->push_back("");
        List->push_back("/* Monitor image at its load address */");
        List->push_back(std::string("place at address mem:0x")+Main::Hex(this->Proj->Monitor->Code_Base)+
                        " { readonly section .rodata object monitor_image.o, readonly section .text object monitor_image.o };");
        List->push_back("keep { readonly section .rodata object monitor_image.o, readonly section .text object monitor_image.o };");
        for(const class Process* Prc:this->Proj->Monitor->Process)
        {
            List->push_back("");
            List->push_back(std::string("/* ")+Prc->Name+" image at its load address */");
            List->push_back(std::string("place at address mem:0x")+Main::Hex(Prc->Code_Base)+
                            " { readonly section .rodata object "+Prc->Name_Lower+"_image.o, readonly section .text object "+
                            Prc->Name_Lower+"_image.o };");
            List->push_back(std::string("keep { readonly section .rodata object ")+Prc->Name_Lower+
                            "_image.o, readonly section .text object "+Prc->Name_Lower+"_image.o };");
        }
    }
    List->push_back("");
    List->push_back("/* End Of File ************************************************************** */");
    List->push_back("");
    List->push_back("/* Copyright (C) Evo-Devo Instrum. All rights reserved *********************** */");
    List->push_back("");
}
/* End Function:ICCARM_Gen::Kernel_Linker ************************************/

/* Function:ICCARM_Gen::Monitor_Linker *****************************************
Description : Generate the RVM linker configuration file.
Input       : std::unique_ptr<std::vector<std::string>>& List - The file.
Output      : std::unique_ptr<std::vector<std::string>>& List - The updated file.
Return      : None.
******************************************************************************/
void ICCARM_Gen::Monitor_Linker(std::unique_ptr<std::vector<std::string>>& List)
{
    List->push_back("/* ****************************************************************************");
    List->push_back(std::string("Filename    : rvm_platform_")+this->Chip->Name_Lower+".icf");
    List->push_back(std::string("Author      : ")+CODE_AUTHOR);
    List->push_back(std::string("Date        : ")+Main::Time);
    List->push_back(std::string("Licence     : ")+CODE_LICENSE);
    List->push_back("Description : The IAR ILINK configuration file for ARMv6-M layout.");
    List->push_back(std::string("              to be used with ")+this->Chip->Name+", and the IAR toolchain.");
    List->push_back("**************************************************************************** */");
    List->push_back("");
    List->push_back("define memory mem with size = 4G;");
    List->push_back("");
    List->push_back("/* Monitor flash region */");
    List->push_back(std::string("define region CODE = mem:[from 0x")+Main::Hex(this->Proj->Monitor->Code_Base)+
                    " to 0x"+Main::Hex(this->Proj->Monitor->Code_Base+this->Proj->Monitor->Code_Size-1)+"];");
    List->push_back("/* Monitor data region */");
    List->push_back(std::string("define region DATA = mem:[from 0x")+Main::Hex(this->Proj->Monitor->Data_Base)+
                    " to 0x"+Main::Hex(this->Proj->Monitor->Data_Base+this->Proj->Monitor->Data_Size-1)+"];");
    List->push_back("/* Monitor booting stack */");
    List->push_back(std::string("define block CSTACK with alignment = 8, size = 0x")+
                    Main::Hex(this->Proj->Monitor->Init_Stack_Size)+" { };");
    List->push_back("");
    List->push_back("initialize by copy { readwrite };");
    List->push_back("");
    List->push_back("/* The entry will be the first instruction */");
    List->push_back(std::string("place at address mem:0x")+Main::Hex(this->Proj->Monitor->Code_Base)+
                    " { readonly section .rvm_entry };");
    List->push_back("keep { readonly section .rvm_entry };");
    List->push_back("place in CODE { readonly };");
    List->push_back("place in DATA { readwrite };");
    List->push_back(std::string("place at address mem:0x")+Main::Hex(this->Proj->Monitor->Init_Stack_Base)+
                    " { block CSTACK };");
    List->push_back("");
    List->push_back("/* End Of File ************************************************************** */");
    List->push_back("");
    List->push_back("/* Copyright (C) Evo-Devo Instrum. All rights reserved *********************** */");
    List->push_back("");
}
/* End Function:ICCARM_Gen::Monitor_Linker ***********************************/

/* Function:ICCARM_Gen::Process_Linker *****************************************
Description : Generate the process linker configuration file.
Input       : std::unique_ptr<std::vector<std::string>>& List - The file.
              const class Process* Prc - The process to generate for.
Output      : std::unique_ptr<std::vector<std::string>>& List - The updated file.
Return      : None.
******************************************************************************/
void ICCARM_Gen::Process_Linker(std::unique_ptr<std::vector<std::string>>& List,
                                const class Process* Prc)
{
    ptr_t Header_Size;
    ptr_t Real_Code_Size;
    ptr_t Stack_Base;
    ptr_t Stack_Size;

    Header_Size=Prc->Code_Front-Prc->Code_Base;
    Real_Code_Size=Prc->Code_Size-Header_Size;
    /* The booting thread's stack is reused as the C stack */
    if(Prc->Type==PROCESS_NATIVE)
    {
        Stack_Base=Prc->Thread[0]->Stack_Base;
        Stack_Size=Prc->Thread[0]->Stack_Size;
    }
    else
    {
        ASSERT((Prc->Thread[0]->Name=="Vct")&&(Prc->Thread[1]->Name=="Usr"));
        Stack_Base=Prc->Thread[1]->Stack_Base;
        Stack_Size=Prc->Thread[1]->Stack_Size;
    }

    List->push_back("/* ****************************************************************************");
    List->push_back(std::string("Filename    : proc_")+Prc->Name_Lower+"_"+this->Chip->Name_Lower+".icf");
    List->push_back(std::string("Author      : ")+CODE_AUTHOR);
    List->push_back(std::string("Date        : ")+Main::Time);
    List->push_back(std::string("Licence     : ")+CODE_LICENSE);
    List->push_back("Description : The IAR ILINK configuration file for ARMv6-M layout.");
    List->push_back(std::string("              to be used with ")+this->Chip->Name+", and the IAR toolchain.");
    List->push_back("**************************************************************************** */");
    List->push_back("");
    List->push_back("define memory mem with size = 4G;");
    List->push_back("");
    List->push_back("/* Process descriptor region */");
    List->push_back(std::string("define region DESC = mem:[from 0x")+Main::Hex(Prc->Code_Base)+
                    " to 0x"+Main::Hex(Prc->Code_Base+Header_Size-1)+"];");
    List->push_back("/* Process code region */");
    List->push_back(std::string("define region CODE = mem:[from 0x")+Main::Hex(Prc->Code_Front)+
                    " to 0x"+Main::Hex(Prc->Code_Front+Real_Code_Size-1)+"];");
    List->push_back("/* Process data region */");
    List->push_back(std::string("define region DATA = mem:[from 0x")+Main::Hex(Prc->Data_Base)+
                    " to 0x"+Main::Hex(Prc->Data_Base+Prc->Data_Size-1)+"];");
    List->push_back("/* Process stack - reusing the booting thread's stack */");
    List->push_back(std::string("define block CSTACK with alignment = 8, size = 0x")+
                    Main::Hex(Stack_Size)+" { };");
    List->push_back("");
    List->push_back("initialize by copy { readwrite };");
    List->push_back("");
    List->push_back("/* Process descriptor first, entry point right after the header */");
    List->push_back(std::string("place at address mem:0x")+Main::Hex(Prc->Code_Base)+
                    " { readonly section .rodata object prc_"+Prc->Name_Lower+"_desc.o };");
    List->push_back(std::string("keep { readonly section .rodata object prc_")+Prc->Name_Lower+"_desc.o };");
    List->push_back(std::string("place at address mem:0x")+Main::Hex(Prc->Code_Front)+
                    " { readonly section .rvm_entry };");
    List->push_back("keep { readonly section .rvm_entry };");
    List->push_back("place in CODE { readonly };");
    List->push_back("place in DATA { readwrite };");
    List->push_back(std::string("place at address mem:0x")+Main::Hex(Stack_Base)+" { block CSTACK };");
    List->push_back("");
    List->push_back("/* End Of File ************************************************************** */");
    List->push_back("");
    List->push_back("/* Copyright (C) Evo-Devo Instrum. All rights reserved *********************** */");
    List->push_back("");
}
/* End Function:ICCARM_Gen::Process_Linker ***********************************/
}
/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
