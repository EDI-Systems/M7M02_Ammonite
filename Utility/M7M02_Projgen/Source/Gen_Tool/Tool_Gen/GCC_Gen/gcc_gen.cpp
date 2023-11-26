/******************************************************************************
Filename    : gcc_gen.cpp
Author      : pry
Date        : 12/07/2019
Licence     : LGPL v3+; see COPYING for details.
Description : This toolset is for GCC; generates GCC linker scripts.
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
#include "Gen_Tool/Tool_Gen/tool_gen.hpp"
#include "Gen_Tool/Tool_Gen/GCC_Gen/gcc_gen.hpp"
#include "Proj_Info/Process/process.hpp"
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
#include "Gen_Tool/Tool_Gen/GCC_Gen/gcc_gen.hpp"
#undef __HDR_CLASS__
/* End Include ***************************************************************/
namespace RVM_GEN
{
/* Function:GCC_Gen::GCC_Gen **************************************************
Description : Generator for the ARMv7-M platform.
Input       : class Proj_Info* Proj - The project information.
              class Plat_Info* Plat - The platform information.
              class Chip_Info* Chip - The chip information.
Output      : None.
Return      : None.
******************************************************************************/
/* void */ GCC_Gen::GCC_Gen(class Proj_Info* Proj, class Plat_Info* Plat, class Chip_Info* Chip):
Tool_Gen("GCC", Proj, Plat, Chip)
{
    try
    {

    }
    catch(std::exception& Exc)
    {
        Main::Error(std::string("GCC generator:\n")+Exc.what());
    }
}
/* End Function:GCC_Gen::GCC_Gen *********************************************/

/* Function:GCC_Gen::Suffix ***************************************************
Description : Returns suffix for a given type of file.
Input       : ptr_t Type - The file type.
Output      : None.
Return      : std::string - The file suffix.
******************************************************************************/
std::string GCC_Gen::Suffix(ptr_t Type)
{
    if(Type==TOOL_LINKER)
        return ".ld";
    else if(Type==TOOL_ASSEMBLER)
        return ".s";
    else
        Main::Error("XXXXX: File type not recognized.");
}
/* End Function:GCC_Gen::Suffix **********************************************/

/* Function:GCC_Gen::Kernel_Linker ********************************************
Description : Generate the RME linker script.
Input       : std::unique_ptr<std::vector<std::string>>& List - The file.
Output      : std::unique_ptr<std::vector<std::string>>& List - The updated file.
Return      : None.
******************************************************************************/
void GCC_Gen::Kernel_Linker(std::unique_ptr<std::vector<std::string>>& List)
{
    List->push_back("/******************************************************************************");
    List->push_back(std::string("Filename    : rme_platform_")+this->Chip->Name_Lower+".ld");
    List->push_back(std::string("Author      : ")+CODE_AUTHOR);
    List->push_back(std::string("Date        : ")+Main::Time);
    List->push_back(std::string("Licence     : ")+CODE_LICENSE);
    List->push_back("Description : The GCC linker script. This file is intended");
    List->push_back(std::string("              to be used with ") + this->Chip->Name + ", and the GNU toolchain.");
    List->push_back("******************************************************************************/");
    List->push_back("");
    List->push_back("/* Memory ********************************************************************/");
    List->push_back("MEMORY");
    List->push_back("{");
    List->push_back("    /* Statically allocated kernel data segment */");
    List->push_back(std::string("    KDATA (rw) : ORIGIN = 0x")+Main::Hex(this->Proj->Kernel->Data_Base)+" , LENGTH = 0x"+Main::Hex(this->Proj->Kernel->Data_Size));
    List->push_back("    /* Dynamically allocated kernel data segment */");
    List->push_back(std::string("    KMEM (rw) : ORIGIN = 0x")+Main::Hex(this->Proj->Kernel->Kom_Base)+" , LENGTH = 0x"+Main::Hex(this->Proj->Kernel->Kom_Size));
    List->push_back("    /* Kernel stack segment */");
    List->push_back(std::string("    KSTK (rw) : ORIGIN = 0x")+Main::Hex(this->Proj->Kernel->Stack_Base)+" , LENGTH = 0x"+Main::Hex(this->Proj->Kernel->Stack_Size));
    List->push_back("");
    List->push_back("    /* Kernel code segment */");
    List->push_back(std::string("    KCODE (rx) : ORIGIN = 0x")+Main::Hex(this->Proj->Kernel->Code_Base)+" , LENGTH = 0x"+Main::Hex(this->Proj->Kernel->Code_Size));
    List->push_back("    /* Monitor code segment */");
    List->push_back(std::string("    MCODE (rx) : ORIGIN = 0x")+Main::Hex(this->Proj->Monitor->Code_Base)+" , LENGTH = 0x"+Main::Hex(this->Proj->Kernel->Code_Size));
    for (const std::unique_ptr<class Process>& Prc : this->Proj->Process)
    {
        List->push_back(std::string("    /* ")+Prc->Name+" code segment */");
        List->push_back(std::string("    ")+Prc->Name_Upper+"_CODE (rx) : ORIGIN = 0x"+Main::Hex(Prc->Code_Base)+" , LENGTH = 0x"+Main::Hex(Prc->Code_Size));
    }
    List->push_back("}");
    List->push_back("/* End Memory ****************************************************************/");
    List->push_back("");
    List->push_back("/* Stack *********************************************************************/");
    List->push_back("__RME_Stack = ORIGIN(KSTK) + LENGTH(KSTK);");
    List->push_back("/* End Stack Definitions *****************************************************/");
    List->push_back("");
    List->push_back("/* Entry *********************************************************************/");
    List->push_back("ENTRY(__RME_Entry);");
    List->push_back("/* End Entry *****************************************************************/");
    List->push_back("");
    List->push_back("/* Process *******************************************************************/");
    List->push_back("SECTIONS");
    List->push_back("{");
    List->push_back("/* Monitor image */");
    List->push_back(".MONITOR_CODE : ALIGN(4)");
    List->push_back("{");
    List->push_back("    KEEP(*monitor_image.o(.text .text.*))");
    List->push_back("    KEEP(*monitor_image.o(.rodata .rodata.* .constdata .constdata.*))");
    List->push_back("} > MCODE");
    for(const std::unique_ptr<class Process>& Prc:this->Proj->Process)
    {
        List->push_back("");
        List->push_back("/* Process image */");
        List->push_back(std::string(".")+Prc->Name_Upper+"_CODE : ALIGN(4)");
        List->push_back("{");
        List->push_back(std::string("    KEEP(*")+Prc->Name_Lower+"_image.o(.text .text.*))");
        List->push_back(std::string("    KEEP(*")+Prc->Name_Lower+"_image.o(.rodata .rodata.* .constdata .constdata.*))");
        List->push_back(std::string("} > ")+Prc->Name_Upper+"_CODE");
    }
    List->push_back("/* End Process ***************************************************************/");
    List->push_back("");
    List->push_back("/* Kernel ********************************************************************/");
    List->push_back("/* Kernel code segment */");
    List->push_back(".KERNEL_CODE : ALIGN(4)");
    List->push_back("{");
    List->push_back("    __RME_Code_Start = .;");
    List->push_back("    KEEP(*_gcc.o(.text.rme_vector))");
    List->push_back("    KEEP(*_gcc.o(.text.rme_entry))");
    List->push_back("    *(.text .text.*)");
    List->push_back("    *(.rodata .rodata.* .constdata .constdata.*)");
    List->push_back("    __RME_Code_End = .;");
    List->push_back("} > KCODE");
    List->push_back("");
    List->push_back("/* Statically allocated kernel data segment */");
    List->push_back("__RME_Data_Load = LOADADDR(.KERNEL_DATA);");
    List->push_back(".KERNEL_DATA : ALIGN(4)");
    List->push_back("{");
    List->push_back("    FILL(0xFF)");
    List->push_back("    __RME_Data_Start = .;");
    List->push_back("    __RME_Global = . + 0x800;");
    List->push_back("    *(.data_begin .data_begin.*)");
    List->push_back("    *(.data .data.*)");
    List->push_back("    *(.data_end .data_end.*)");
    List->push_back("    . = ALIGN(4);");
    List->push_back("    __RME_Data_End = .;");
    List->push_back("} > KDATA AT > KCODE");
    List->push_back("");
    List->push_back("/* Statically allocated kernel bss segment */");
    List->push_back(".KERNEL_ZERO (NOLOAD) : ALIGN(4)");
    List->push_back("{");
    List->push_back("    __RME_Zero_Start = .;");
    List->push_back("    *(.bss_begin .bss_begin.*)");
    List->push_back("    *(.bss .bss.*)");
    List->push_back("    *(COMMON)");
    List->push_back("    *(.bss_end .bss_end.*)");
    List->push_back("    . = ALIGN(4);");
    List->push_back("    __RME_Zero_End = .;");
    List->push_back("} > KDATA");
    List->push_back("");
    List->push_back("/* Statically allocated kernel noinit segment */");
    List->push_back(".KERNEL_NOINIT (NOLOAD) : ALIGN(4)");
    List->push_back("{");
    List->push_back("    __RME_Noinit_Start = .;");
    List->push_back("    *(.noinit .noinit.*)");
    List->push_back("    . = ALIGN(4);");
    List->push_back("    __RME_Noinit_End = .;");
    List->push_back("} > KDATA");
    List->push_back("");
    List->push_back("/* Compatibility symbols for various architectures */");
    List->push_back("PROVIDE(__stack = __RME_Stack);");
    List->push_back("PROVIDE(__initial_sp = __RME_Stack);");
    List->push_back("PROVIDE(__initial_sp$ = __RME_Stack);");
    List->push_back("PROVIDE(__global_pointer = __RME_Global);");
    List->push_back("PROVIDE(__global_pointer$ = __RME_Global);");
    List->push_back("PROVIDE(__start = __RME_Entry);");
    List->push_back("PROVIDE(_stext = __RME_Code_Start);");
    List->push_back("PROVIDE(_etext = __RME_Code_End);");
    List->push_back("PROVIDE(_sidata = __RME_Data_Load);");
    List->push_back("PROVIDE(_sdata = __RME_Data_Start);");
    List->push_back("PROVIDE(_edata = __RME_Data_End);");
    List->push_back("PROVIDE(_sbss = __RME_Zero_Start);");
    List->push_back("PROVIDE(_ebss = __RME_Zero_End);");
    List->push_back("PROVIDE(_snoinit = __RME_Noinit_Start);");
    List->push_back("PROVIDE(_enoinit = __RME_Noinit_End);");
    List->push_back("PROVIDE(__text_start__ = __RME_Code_Start);");
    List->push_back("PROVIDE(__text_end__ = __RME_Code_End);");
    List->push_back("PROVIDE(__data_load__ = __RME_Data_Load);");
    List->push_back("PROVIDE(__data_start__ = __RME_Data_Start);");
    List->push_back("PROVIDE(__data_end__ = __RME_Data_End);");
    List->push_back("PROVIDE(__bss_start__ = __RME_Zero_Start);");
    List->push_back("PROVIDE(__bss_end__ = __RME_Zero_End);");
    List->push_back("PROVIDE(__noinit_start__ = __RME_Noinit_Start);");
    List->push_back("PROVIDE(__noinit_end__ = __RME_Noinit_End);");
    List->push_back("PROVIDE(end = __RME_Zero_End);");
    List->push_back("PROVIDE(_end = __RME_Zero_End);");
    List->push_back("PROVIDE(__end = __RME_Zero_End);");
    List->push_back("PROVIDE(__end__ = __RME_Zero_End);");
    List->push_back("/* End Section:.noinit *******************************************************/");
    List->push_back("}");
    List->push_back("/* End Of File ***************************************************************/");
    List->push_back("");
    List->push_back("/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/");
    List->push_back("");
}
/* End Function:GCC_Gen::Kernel_Linker ***************************************/

/* Function:GCC_Gen::Monitor_Linker *******************************************
Description : Generate the RVM linker script.
Input       : std::unique_ptr<std::vector<std::string>>& List - The file.
Output      : std::unique_ptr<std::vector<std::string>>& List - The updated file.
Return      : None.
******************************************************************************/
void GCC_Gen::Monitor_Linker(std::unique_ptr<std::vector<std::string>>& List)
{
    List->push_back("/******************************************************************************");
    List->push_back(std::string("Filename    : rvm_platform_") + this->Chip->Name_Lower + ".ld");
    List->push_back(std::string("Author      : ") + CODE_AUTHOR);
    List->push_back(std::string("Date        : ") + Main::Time);
    List->push_back(std::string("Licence     : ") + CODE_LICENSE);
    List->push_back("Description : The GCC linker script. This file is intended");
    List->push_back(std::string("              to be used with ") + this->Chip->Name + ", and the GNU toolchain.");
    List->push_back("******************************************************************************/");
    List->push_back("");
    List->push_back("/* Memory ********************************************************************/");
    List->push_back("MEMORY");
    List->push_back("{");
    List->push_back("    /* Monitor RAM BASE and SIZE*/");
    List->push_back(std::string("    DATA (rw) : ORIGIN =  0x")+Main::Hex(this->Proj->Monitor->Data_Base)+" , LENGTH = 0x" + Main::Hex(this->Proj->Monitor->Data_Size));
    List->push_back("    /* Monitor Stack BASE and SIZE*/");
    List->push_back(std::string("    STK (rw) : ORIGIN = 0x")+Main::Hex(this->Proj->Monitor->Init_Stack_Base)+" , LENGTH = 0x" + Main::Hex(this->Proj->Monitor->Init_Stack_Size));
    List->push_back("    /* Monitor flash segment */");
    List->push_back(std::string("    CODE (rx) : ORIGIN = 0x")+Main::Hex(this->Proj->Monitor->Code_Base)+" , LENGTH = 0x" + Main::Hex(this->Proj->Monitor->Code_Size));
    List->push_back("}");
    List->push_back("/* End Memory ****************************************************************/");
    List->push_back("");
    List->push_back("/* Stack *********************************************************************/");
    List->push_back("__RVM_Stack = ORIGIN(STK) + LENGTH(STK);");
    List->push_back("/* End Entry Point Definitions ***********************************************/");
    List->push_back("");
    List->push_back("/* Entry *********************************************************************/");
    List->push_back("ENTRY(__RVM_Entry)");
    List->push_back("/* End Entry *****************************************************************/");
    List->push_back("");
    List->push_back("/* Section *******************************************************************/");
    List->push_back("SECTIONS");
    List->push_back("{");
    List->push_back("/* Monitor code segment */");
    List->push_back(".MONITOR_CODE : ALIGN(4)");
    List->push_back("{");
    List->push_back("    __RVM_Code_Start = .;");
    List->push_back("    KEEP(*_gcc.o(.text.rvm_header))");
    List->push_back("    KEEP(*_gcc.o(.text.rvm_entry))");
    List->push_back("    *(.text .text.*)");
    List->push_back("    *(.rodata .rodata.* .constdata .constdata.*)");
    List->push_back("    . = ALIGN(4);");
    List->push_back("    __RVM_Code_End = .;");
    List->push_back("} > CODE");
    List->push_back("");
    List->push_back("/* Monitor data segment */");
    List->push_back("__RVM_Data_Load = LOADADDR(.MONITOR_DATA);");
    List->push_back(".MONITOR_DATA : ALIGN(4)");
    List->push_back("{");
    List->push_back("    __RVM_Data_Start = .;");
    List->push_back("    __RVM_Global = . + 0x800;");
    List->push_back("    *(.data_begin .data_begin.*)");
    List->push_back("    *(.data .data.*)");
    List->push_back("    *(.data_end .data_end.*)");
    List->push_back("    __RVM_Data_End = .;");
    List->push_back("    . = ALIGN(4);");
    List->push_back("  } > DATA AT > CODE");
    List->push_back("");
    List->push_back("/* Monitor bss segment */");
    List->push_back(".MONITOR_ZERO : ALIGN(4)");
    List->push_back("{");
    List->push_back("    __RVM_Zero_Start = .;");
    List->push_back("    *(.bss_begin .bss_begin.*)");
    List->push_back("    *(.bss .bss.*)");
    List->push_back("    *(COMMON)");
    List->push_back("    *(.bss_end .bss_end.*)");
    List->push_back("    __RVM_Zero_End = .;");
    List->push_back("    . = ALIGN(4);");
    List->push_back("} > DATA");
    List->push_back("");
    List->push_back("/* Monitor noinit segment */");
    List->push_back(".MONITOR_NOINIT (NOLOAD) : ALIGN(4)");
    List->push_back("{");
    List->push_back("    __RVM_Noinit_Start = .;");
    List->push_back("    *(.noinit .noinit.*)");
    List->push_back("    . = ALIGN(4);");
    List->push_back("    __RVM_Noinit_End = .;");
    List->push_back("} > DATA");
    List->push_back("}");
    List->push_back("");
    List->push_back("/* Compatibility symbols for various architectures */");
    List->push_back("PROVIDE(__stack = __RVM_Stack);");
    List->push_back("PROVIDE(__initial_sp = __RVM_Stack);");
    List->push_back("PROVIDE(__initial_sp$ = __RVM_Stack);");
    List->push_back("PROVIDE(__global_pointer = __RVM_Global);");
    List->push_back("PROVIDE(__global_pointer$ = __RVM_Global);");
    List->push_back("PROVIDE(__start = __RVM_Entry);");
    List->push_back("PROVIDE(_stext = __RVM_Code_Start);");
    List->push_back("PROVIDE(_etext = __RVM_Code_End);");
    List->push_back("PROVIDE(_sidata = __RVM_Data_Load);");
    List->push_back("PROVIDE(_sdata = __RVM_Data_Start);");
    List->push_back("PROVIDE(_edata = __RVM_Data_End);");
    List->push_back("PROVIDE(_sbss = __RVM_Zero_Start);");
    List->push_back("PROVIDE(_ebss = __RVM_Zero_End);");
    List->push_back("PROVIDE(_snoinit = __RVM_Noinit_Start);");
    List->push_back("PROVIDE(_enoinit = __RVM_Noinit_End);");
    List->push_back("PROVIDE(__text_start__ = __RVM_Code_Start);");
    List->push_back("PROVIDE(__text_end__ = __RVM_Code_End);");
    List->push_back("PROVIDE(__data_load__ = __RVM_Data_Load);");
    List->push_back("PROVIDE(__data_start__ = __RVM_Data_Start);");
    List->push_back("PROVIDE(__data_end__ = __RVM_Data_End);");
    List->push_back("PROVIDE(__bss_start__ = __RVM_Zero_Start);");
    List->push_back("PROVIDE(__bss_end__ = __RVM_Zero_End);");
    List->push_back("PROVIDE(__noinit_start__ = __RVM_Noinit_Start);");
    List->push_back("PROVIDE(__noinit_end__ = __RVM_Noinit_End);");
    List->push_back("PROVIDE(end = __RVM_Zero_End);");
    List->push_back("PROVIDE(_end = __RVM_Zero_End);");
    List->push_back("PROVIDE(__end = __RVM_Zero_End);");
    List->push_back("PROVIDE(__end__ = __RVM_Zero_End);");
    List->push_back("/* End Section ***************************************************************/");
    List->push_back("");
    List->push_back("/* End Of File ***************************************************************/");
    List->push_back("");
    List->push_back("/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/");
    List->push_back("");
}
/* End Function:GCC_Gen::Monitor_Linker **************************************/

/* Function:GCC_Gen::Process_Linker *******************************************
Description : Generate the process linker script.
Input       : std::unique_ptr<std::vector<std::string>>& List - The file.
              const class Process* Prc - The process to generate for.
Output      : std::unique_ptr<std::vector<std::string>>& List - The updated file.
Return      : None.
******************************************************************************/
void GCC_Gen::Process_Linker(std::unique_ptr<std::vector<std::string>>& List,
                             const class Process* Prc)
{
    ptr_t Header_Size;
    ptr_t Real_Code_Size;
    std::string Tool_Lower;

    Tool_Lower = Prc->Toolchain;
    Main::Lower(Tool_Lower);
    Header_Size = Prc->Code_Front - Prc->Code_Base;
    Real_Code_Size = Prc->Code_Size - Header_Size;
    List->push_back("/******************************************************************************");
    List->push_back(std::string("Filename    : proc_") + Prc->Name_Lower + "_" + this->Chip->Name_Lower + ".ld");
    List->push_back(std::string("Author      : ") + CODE_AUTHOR);
    List->push_back(std::string("Date        : ") + Main::Time);
    List->push_back(std::string("Licence     : ") + CODE_LICENSE);
    List->push_back("Description : The GCC linker script. This file is intended");
    List->push_back(std::string("              to be used with ") + this->Chip->Name + ", and the GNU toolchain.");
    List->push_back("******************************************************************************/");
    List->push_back("");
    List->push_back("/* Memory ********************************************************************/");
    List->push_back("MEMORY");
    List->push_back("{");
    List->push_back("    /* Process data segment */");
    List->push_back(std::string("    DATA (rw) : ORIGIN =  0x")+Main::Hex(Prc->Data_Base) + ",  LENGTH = 0x" + Main::Hex(Prc->Data_Size));
    List->push_back("    /* Process stack segment */");
    if(Prc->Type==PROCESS_NATIVE)
        List->push_back(std::string("    STK (rw) : ORIGIN =  0x")+Main::Hex(Prc->Thread[0]->Stack_Base)+" , LENGTH = 0x"+Main::Hex(Prc->Thread[0]->Stack_Size));
    else
    {
        ASSERT((Prc->Thread[0]->Name=="Vct")&&(Prc->Thread[1]->Name=="Usr"));
        List->push_back(std::string("    STK (rw) : ORIGIN =  0x")+Main::Hex(Prc->Thread[1]->Stack_Base)+" , LENGTH = 0x"+Main::Hex(Prc->Thread[1]->Stack_Size));
    }
    List->push_back("    /* Process descriptor */");
    List->push_back(std::string("    DESC   (r) : ORIGIN =  0x")+Main::Hex(Prc->Code_Base)+" , LENGTH = 0x"+Main::Hex(Header_Size));
    List->push_back("    /* Process code segment */");
    List->push_back(std::string("    CODE   (rx) : ORIGIN = 0x")+Main::Hex(Prc->Code_Front)+" , LENGTH = 0x"+Main::Hex(Real_Code_Size));
    List->push_back("}");
    List->push_back("/* End Memory ****************************************************************/");
    List->push_back("");
    List->push_back("/* Stack *********************************************************************/");
    List->push_back("_RVM_Stack = ORIGIN(STK) + LENGTH(STK);");
    List->push_back("/* End Entry Point Definitions ***********************************************/");
    List->push_back("");
    List->push_back("/* Entry *********************************************************************/");
    List->push_back("ENTRY(_RVM_Entry)");
    List->push_back("/* End Entry *****************************************************************/");
    List->push_back("");
    List->push_back("/* Section *******************************************************************/");
    List->push_back("SECTIONS");
    List->push_back("{");
    List->push_back("/* Process descriptor segment */");
    List->push_back(".PROCESS_DESC : ALIGN(4)");
    List->push_back("{");
    List->push_back(std::string("    KEEP(*prc_") + Prc->Name_Lower + "_desc.o(.rodata .rodata.* .constdata .constdata.*))");
    List->push_back("} > DESC");
    List->push_back("");
    List->push_back("/* Process code segment */");
    List->push_back(".PROCESS_CODE : ALIGN(4)");
    List->push_back("{");
    List->push_back("    _RVM_Code_Start = .;");
    List->push_back("    KEEP(*_gcc.o(.text.rvm_entry))");
    List->push_back("    *(.text .text.*)");
    List->push_back("    *(.rodata .rodata.* .constdata .constdata.*)");
    List->push_back("    . = ALIGN(4);");
    List->push_back("    _RVM_Code_End = .;");
    List->push_back("} > CODE");
    List->push_back("");
    List->push_back("/* Process data segment */");
    List->push_back("_RVM_Data_Load = LOADADDR(.PROCESS_DATA);");
    List->push_back(".PROCESS_DATA : ALIGN(4)");
    List->push_back("{");
    List->push_back("    _RVM_Data_Start = .;");
    List->push_back("    _RVM_Global = . + 0x800;");
    List->push_back("    *(.data_begin .data_begin.*)");
    List->push_back("    *(.data .data.*)");
    List->push_back("    *(.data_end .data_end.*)");
    List->push_back("    _RVM_Data_End = .;");
    List->push_back("    . = ALIGN(4);");
    List->push_back("  } > DATA AT > CODE");
    List->push_back("");
    List->push_back("/* Process bss segment */");
    List->push_back(".PROCESS_ZERO : ALIGN(4)");
    List->push_back("{");
    List->push_back("    _RVM_Zero_Start = .;");
    List->push_back("    *(.bss_begin .bss_begin.*)");
    List->push_back("    *(.bss .bss.*)");
    List->push_back("    *(COMMON)");
    List->push_back("    *(.bss_end .bss_end.*)");
    List->push_back("    _RVM_Zero_End = .;");
    List->push_back("    . = ALIGN(4);");
    List->push_back("} > DATA");
    List->push_back("");
    List->push_back("/* Process noinit segment */");
    List->push_back(".PROCESS_NOINIT (NOLOAD) : ALIGN(4)");
    List->push_back("{");
    List->push_back("    _RVM_Noinit_Start = .;");
    List->push_back("    *(.noinit .noinit.*)");
    List->push_back("    . = ALIGN(4);");
    List->push_back("    _RVM_Noinit_End = .;");
    List->push_back("} > DATA");
    List->push_back("}");
    List->push_back("");
    List->push_back("/* Compatibility symbols for various architectures */");
    List->push_back("PROVIDE(__stack = _RVM_Stack);");
    List->push_back("PROVIDE(__initial_sp = _RVM_Stack);");
    List->push_back("PROVIDE(__initial_sp$ = _RVM_Stack);");
    List->push_back("PROVIDE(__global_pointer = _RVM_Global);");
    List->push_back("PROVIDE(__global_pointer$ = _RVM_Global);");
    List->push_back("PROVIDE(__start = _RVM_Entry);");
    List->push_back("PROVIDE(_stext = _RVM_Code_Start);");
    List->push_back("PROVIDE(_etext = _RVM_Code_End);");
    List->push_back("PROVIDE(_sidata = _RVM_Data_Load);");
    List->push_back("PROVIDE(_sdata = _RVM_Data_Start);");
    List->push_back("PROVIDE(_edata = _RVM_Data_End);");
    List->push_back("PROVIDE(_sbss = _RVM_Zero_Start);");
    List->push_back("PROVIDE(_ebss = _RVM_Zero_End);");
    List->push_back("PROVIDE(_snoinit = _RVM_Noinit_Start);");
    List->push_back("PROVIDE(_enoinit = _RVM_Noinit_End);");
    List->push_back("PROVIDE(__text_start__ = _RVM_Code_Start);");
    List->push_back("PROVIDE(__text_end__ = _RVM_Code_End);");
    List->push_back("PROVIDE(__data_load__ = _RVM_Data_Load);");
    List->push_back("PROVIDE(__data_start__ = _RVM_Data_Start);");
    List->push_back("PROVIDE(__data_end__ = _RVM_Data_End);");
    List->push_back("PROVIDE(__bss_start__ = _RVM_Zero_Start);");
    List->push_back("PROVIDE(__bss_end__ = _RVM_Zero_End);");
    List->push_back("PROVIDE(__noinit_start__ = _RVM_Noinit_Start);");
    List->push_back("PROVIDE(__noinit_end__ = _RVM_Noinit_End);");
    List->push_back("PROVIDE(end = _RVM_Zero_End);");
    List->push_back("PROVIDE(_end = _RVM_Zero_End);");
    List->push_back("PROVIDE(__end = _RVM_Zero_End);");
    List->push_back("PROVIDE(__end__ = _RVM_Zero_End);");
    List->push_back("/* End Section ***************************************************************/");
    List->push_back("");
    List->push_back("/* End Of File ***************************************************************/");
    List->push_back("");
    List->push_back("/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/");
    List->push_back("");
}
/* End Function:GCC_Gen::Process_Linker **************************************/
}
/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
