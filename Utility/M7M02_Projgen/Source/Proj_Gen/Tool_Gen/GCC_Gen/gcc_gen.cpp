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
#include "Proj_Gen/Tool_Gen/tool_gen.hpp"
#include "Proj_Gen/Tool_Gen/GCC_Gen/gcc_gen.hpp"
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

#include "Proj_Gen/proj_gen.hpp"
#include "Proj_Gen/Tool_Gen/tool_gen.hpp"
#include "Proj_Gen/Tool_Gen/GCC_Gen/gcc_gen.hpp"
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

/* Function:GCC_Gen::Compat_Symbol ********************************************
Description : Generate compatibility symbols, which is the same across all scripts.
Input       : std::unique_ptr<std::vector<std::string>>& List - The file.
              std::string Prefix - The prefix to append to all compatibility symbols.
Output      : None.
Return      : None.
******************************************************************************/
void GCC_Gen::Compat_Symbol(std::unique_ptr<std::vector<std::string>>& List, std::string Prefix)
{
    List->push_back("/* Compatibility symbols for various architectures */");
    List->push_back("PROVIDE(__stack = __"+Prefix+"_Stack);");
    List->push_back("PROVIDE(__initial_sp = __"+Prefix+"_Stack);");
    List->push_back("PROVIDE(__initial_sp$ = __"+Prefix+"_Stack);");
    List->push_back("PROVIDE(__global_pointer = __"+Prefix+"_Global);");
    List->push_back("PROVIDE(__global_pointer$ = __"+Prefix+"_Global);");
    List->push_back("PROVIDE(__start = __"+Prefix+"_Entry);");
    List->push_back("PROVIDE(_stext = __"+Prefix+"_Code_Start);");
    List->push_back("PROVIDE(_etext = __"+Prefix+"_Code_End);");
    List->push_back("PROVIDE(_sidata = __"+Prefix+"_Data_Load);");
    List->push_back("PROVIDE(_sdata = __"+Prefix+"_Data_Start);");
    List->push_back("PROVIDE(_edata = __"+Prefix+"_Data_End);");
    List->push_back("PROVIDE(_sbss = __"+Prefix+"_Zero_Start);");
    List->push_back("PROVIDE(_ebss = __"+Prefix+"_Zero_End);");
    List->push_back("PROVIDE(_snoinit = __"+Prefix+"_Noinit_Start);");
    List->push_back("PROVIDE(_enoinit = __"+Prefix+"_Noinit_End);");
    List->push_back("PROVIDE(__text_start__ = __"+Prefix+"_Code_Start);");
    List->push_back("PROVIDE(__text_end__ = __"+Prefix+"_Code_End);");
    List->push_back("PROVIDE(__data_load__ = __"+Prefix+"_Data_Load);");
    List->push_back("PROVIDE(__data_start__ = __"+Prefix+"_Data_Start);");
    List->push_back("PROVIDE(__data_end__ = __"+Prefix+"_Data_End);");
    List->push_back("PROVIDE(__bss_start__ = __"+Prefix+"_Zero_Start);");
    List->push_back("PROVIDE(__bss_end__ = __"+Prefix+"_Zero_End);");
    List->push_back("PROVIDE(__noinit_start__ = __"+Prefix+"_Noinit_Start);");
    List->push_back("PROVIDE(__noinit_end__ = __"+Prefix+"_Noinit_End);");
    List->push_back("PROVIDE(end = __"+Prefix+"_Noinit_End);");
    List->push_back("PROVIDE(_end = __"+Prefix+"_Noinit_End);");
    List->push_back("PROVIDE(__end = __"+Prefix+"_Noinit_End);");
    List->push_back("PROVIDE(__end__ = __"+Prefix+"_Noinit_End);");
}
/* End Function:GCC_Gen::Compat_Symbol ***************************************/

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
    List->push_back("/* Section *******************************************************************/");
    List->push_back("/* Kernel code segment */");
    List->push_back(".KERNEL_CODE : ALIGN(4)");
    List->push_back("{");
    List->push_back("    __RME_Code_Start = .;");
    /* ARM Cortex-M processors require that the vector table be linked upfront, whereas other processors
     * require entry to be linked first. This can change in the future when new architecture is added. */
    if((this->Chip->Platform=="A6M")||(this->Chip->Platform=="A7M")||(this->Chip->Platform=="A8M"))
    {
        List->push_back("    KEEP(*_gcc.o(.text.rme_vector))");
    }
    else
    {
        List->push_back("    KEEP(*_gcc.o(.text.rme_entry))");
        List->push_back("    KEEP(*_gcc.o(.text.rme_vector))");
    }
    List->push_back("    *(.text .text.*)");
    List->push_back("    *(.srodata .srodata.* .sconstdata .sconstdata.*)");
    List->push_back("    *(.rodata .rodata.* .constdata .constdata.*)");
    List->push_back("    . = ALIGN(4);");
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
    List->push_back("    *(.sdata_begin .sdata_begin.*)");
    List->push_back("    *(.sdata .sdata.*)");
    List->push_back("    *(.sdata2.*)");
    List->push_back("    *(.sdata_end .sdata_end.*)");
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
    List->push_back("    *(.sbss_begin .sbss_begin.*)");
    List->push_back("    *(.sbss .sbss.*)");
    List->push_back("    *(.sbss2.*)");
    List->push_back("    *(SCOMMON)");
    List->push_back("    *(.sbss_end .sbss_end.*)");
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
    List->push_back("    *(.snoinit .snoinit.*)");
    List->push_back("    *(.noinit .noinit.*)");
    List->push_back("    . = ALIGN(4);");
    List->push_back("    __RME_Noinit_End = .;");
    List->push_back("} > KDATA");
    List->push_back("");
    this->Compat_Symbol(List, "RME");
    List->push_back("/* End Section ***************************************************************/");
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
    List->push_back(std::string("    DATA (rw) : ORIGIN = 0x")+Main::Hex(this->Proj->Monitor->Data_Base)+" , LENGTH = 0x" + Main::Hex(this->Proj->Monitor->Data_Size));
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
    List->push_back("    KEEP(*_gcc.o(.text.rvm_entry))");
    List->push_back("    *(.text .text.*)");
    List->push_back("    *(.srodata .srodata.* .sconstdata .sconstdata.*)");
    List->push_back("    *(.rodata .rodata.* .constdata .constdata.*)");
    List->push_back("    . = ALIGN(4);");
    List->push_back("    __RVM_Code_End = .;");
    List->push_back("} > CODE");
    List->push_back("");
    List->push_back("/* Monitor data segment */");
    List->push_back("__RVM_Data_Load = LOADADDR(.MONITOR_DATA);");
    List->push_back(".MONITOR_DATA : ALIGN(4)");
    List->push_back("{");
    List->push_back("    FILL(0xFF)");
    List->push_back("    __RVM_Data_Start = .;");
    List->push_back("    __RVM_Global = . + 0x800;");
    List->push_back("    *(.sdata_begin .sdata_begin.*)");
    List->push_back("    *(.sdata .sdata.*)");
    List->push_back("    *(.sdata2.*)");
    List->push_back("    *(.sdata_end .sdata_end.*)");
    List->push_back("    *(.data_begin .data_begin.*)");
    List->push_back("    *(.data .data.*)");
    List->push_back("    *(.data_end .data_end.*)");
    List->push_back("    . = ALIGN(4);");
    List->push_back("    __RVM_Data_End = .;");
    List->push_back("  } > DATA AT > CODE");
    List->push_back("");
    List->push_back("/* Monitor bss segment */");
    List->push_back(".MONITOR_ZERO : ALIGN(4)");
    List->push_back("{");
    List->push_back("    __RVM_Zero_Start = .;");
    List->push_back("    *(.sbss_begin .sbss_begin.*)");
    List->push_back("    *(.sbss .sbss.*)");
    List->push_back("    *(.sbss2.*)");
    List->push_back("    *(SCOMMON)");
    List->push_back("    *(.sbss_end .sbss_end.*)");
    List->push_back("    *(.bss_begin .bss_begin.*)");
    List->push_back("    *(.bss .bss.*)");
    List->push_back("    *(COMMON)");
    List->push_back("    *(.bss_end .bss_end.*)");
    List->push_back("    . = ALIGN(4);");
    List->push_back("    __RVM_Zero_End = .;");
    List->push_back("} > DATA");
    List->push_back("");
    List->push_back("/* Monitor noinit segment */");
    List->push_back(".MONITOR_NOINIT (NOLOAD) : ALIGN(4)");
    List->push_back("{");
    List->push_back("    __RVM_Noinit_Start = .;");
    List->push_back("    *(.snoinit .snoinit.*)");
    List->push_back("    *(.noinit .noinit.*)");
    List->push_back("    . = ALIGN(4);");
    List->push_back("    __RVM_Noinit_End = .;");
    List->push_back("} > DATA");
    List->push_back("}");
    List->push_back("");
    this->Compat_Symbol(List, "RVM");
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
    List->push_back(std::string("    DATA (rw) : ORIGIN = 0x")+Main::Hex(Prc->Data_Base) + " , LENGTH = 0x" + Main::Hex(Prc->Data_Size));
    List->push_back("    /* Process stack segment */");
    if(Prc->Type==PROCESS_NATIVE)
        List->push_back(std::string("    STK (rw) : ORIGIN = 0x")+Main::Hex(Prc->Thread[0]->Stack_Base)+" , LENGTH = 0x"+Main::Hex(Prc->Thread[0]->Stack_Size));
    else
    {
        ASSERT((Prc->Thread[0]->Name=="Vct")&&(Prc->Thread[1]->Name=="Usr"));
        List->push_back(std::string("    STK (rw) : ORIGIN = 0x")+Main::Hex(Prc->Thread[1]->Stack_Base)+" , LENGTH = 0x"+Main::Hex(Prc->Thread[1]->Stack_Size));
    }
    List->push_back("    /* Process descriptor */");
    List->push_back(std::string("    DESC (r) : ORIGIN = 0x")+Main::Hex(Prc->Code_Base)+" , LENGTH = 0x"+Main::Hex(Header_Size));
    List->push_back("    /* Process code segment */");
    List->push_back(std::string("    CODE (rx) : ORIGIN = 0x")+Main::Hex(Prc->Code_Front)+" , LENGTH = 0x"+Main::Hex(Real_Code_Size));
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
    List->push_back("/* Process descriptor segment */");
    List->push_back(".PROCESS_DESC : ALIGN(4)");
    List->push_back("{");
    List->push_back(std::string("    KEEP(*prc_")+Prc->Name_Lower+"_desc.o(.srodata .srodata.* .sconstdata .sconstdata.*))");
    List->push_back(std::string("    KEEP(*prc_")+Prc->Name_Lower+"_desc.o(.rodata .rodata.* .constdata .constdata.*))");
    List->push_back("} > DESC");
    List->push_back("");
    List->push_back("/* Process code, srodata and rodata segment */");
    List->push_back(".PROCESS_CODE : ALIGN(4)");
    List->push_back("{");
    List->push_back("    __RVM_Code_Start = .;");
    List->push_back("    KEEP(*_gcc.o(.text.rvm_entry))");
    List->push_back("    *(.text .text.*)");
    List->push_back("    *(.srodata .srodata.* .sconstdata .sconstdata.*)");
    List->push_back("    *(.rodata .rodata.* .constdata .constdata.*)");
    List->push_back("    . = ALIGN(4);");
    List->push_back("    __RVM_Code_End = .;");
    List->push_back("} > CODE");
    List->push_back("");
    List->push_back("/* Process sdata & data segment */");
    List->push_back("__RVM_Data_Load = LOADADDR(.PROCESS_DATA);");
    List->push_back(".PROCESS_DATA : ALIGN(4)");
    List->push_back("{");
    List->push_back("    FILL(0xFF)");
    List->push_back("    __RVM_Data_Start = .;");
    List->push_back("    __RVM_Global = . + 0x800;");
    List->push_back("    *(.sdata_begin .sdata_begin.*)");
    List->push_back("    *(.sdata .sdata.*)");
    List->push_back("    *(.sdata2.*)");
    List->push_back("    *(.sdata_end .sdata_end.*)");
    List->push_back("    *(.data_begin .data_begin.*)");
    List->push_back("    *(.data .data.*)");
    List->push_back("    *(.data_end .data_end.*)");
    List->push_back("    . = ALIGN(4);");
    List->push_back("    __RVM_Data_End = .;");
    List->push_back("  } > DATA AT > CODE");
    List->push_back("");
    List->push_back("/* Process sbss & bss segment */");
    List->push_back(".PROCESS_ZERO : ALIGN(4)");
    List->push_back("{");
    List->push_back("    __RVM_Zero_Start = .;");
    List->push_back("    *(.sbss_begin .sbss_begin.*)");
    List->push_back("    *(.sbss .sbss.*)");
    List->push_back("    *(.sbss2.*)");
    List->push_back("    *(SCOMMON)");
    List->push_back("    *(.sbss_end .sbss_end.*)");
    List->push_back("    *(.bss_begin .bss_begin.*)");
    List->push_back("    *(.bss .bss.*)");
    List->push_back("    *(COMMON)");
    List->push_back("    *(.bss_end .bss_end.*)");
    List->push_back("    . = ALIGN(4);");
    List->push_back("    __RVM_Zero_End = .;");
    List->push_back("} > DATA");
    List->push_back("");
    List->push_back("/* Process snoinit & noinit segment */");
    List->push_back(".PROCESS_NOINIT (NOLOAD) : ALIGN(4)");
    List->push_back("{");
    List->push_back("    __RVM_Noinit_Start = .;");
    List->push_back("    *(.snoinit .snoinit.*)");
    List->push_back("    *(.noinit .noinit.*)");
    List->push_back("    . = ALIGN(4);");
    List->push_back("    __RVM_Noinit_End = .;");
    List->push_back("} > DATA");
    List->push_back("}");
    List->push_back("");
    this->Compat_Symbol(List, "RVM");
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
