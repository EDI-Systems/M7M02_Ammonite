/******************************************************************************
Filename    : gcc_gen.cpp
Author      : pry
Date        : 12/07/2019
Licence     : LGPL v3+; see COPYING for details.
Description : This toolset is for GCC; generates GCC linker scripts.
******************************************************************************/

/* Includes ******************************************************************/
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

#define __HDR_DEFS__
#include "rvm_gen.hpp"
#include "Gen_Tool/Tool_Gen/tool_gen.hpp"
#include "Gen_Tool/Tool_Gen/GCC_Gen/gcc_gen.hpp"
#include "Proj_Info/Process/process.hpp"
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
#include "Gen_Tool/Tool_Gen/GCC_Gen/gcc_gen.hpp"
#undef __HDR_STRUCTS__
/* End Includes **************************************************************/
namespace RVM_GEN
{
/* Begin Function:GCC_Gen::GCC_Gen ********************************************
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

/* Begin Function:GCC_Gen::Suffix *********************************************
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

/* Begin Function:GCC_Gen::Kernel_Linker **************************************
Description : Generate the RME linker script.
Input       : std::unique_ptr<std::vector<std::string>>& List - The file.
Output      : std::unique_ptr<std::vector<std::string>>& List - The updated file.
Return      : None.
******************************************************************************/
void GCC_Gen::Kernel_Linker(std::unique_ptr<std::vector<std::string>>& List)
{
    List->push_back("/******************************************************************************");
    List->push_back(std::string("Filename    : rme_platform_") + this->Chip->Name_Lower + ".ld");
    List->push_back(std::string("Author      : ") + CODE_AUTHOR);
    List->push_back(std::string("Date        : ") + Main::Time);
    List->push_back(std::string("Licence     : ") + CODE_LICENSE);
    List->push_back("Description : The scatter file for ARMv7-M layout. This file is intended");
    List->push_back(std::string("              to be used with ") + this->Chip->Name + ".");
    List->push_back("******************************************************************************/");
    List->push_back("");
    List->push_back("/* Memory Definitions *********************************************************");
    List->push_back("Description : This section will define the memory layout of the system.");
    List->push_back("Component   : ORIGIN - Starting address of the memory region.");
    List->push_back("              LENGTH - Length of the region.");
    List->push_back(" *****************************************************************************/");
    List->push_back("MEMORY");
    List->push_back("{");
    List->push_back("    /* Kernel initalized segment */");
    List->push_back(std::string("   KIRAM   (xrw) : ORIGIN = 0x") + Main::Hex(this->Proj->Kernel->Data_Base) + ", LENGTH = 0x" + Main::Hex(this->Proj->Kernel->Data_Size));
    List->push_back("   /* Kernel memory segment for kernel object allocations */");
    List->push_back(std::string("   KORAM   (xrw) : ORIGIN = 0x") + Main::Hex(this->Proj->Kernel->Kom_Base) + ", LENGTH = 0x" + Main::Hex(this->Proj->Kernel->Kom_Size));
    List->push_back("    /* Kernel stack segment */");
    List->push_back(std::string("   KSRAM   (xrw) : ORIGIN = 0x") + Main::Hex(this->Proj->Kernel->Stack_Base) + ", LENGTH = 0x" + Main::Hex(this->Proj->Kernel->Stack_Size));
    List->push_back("");
    List->push_back("    /* Kernel flash segment */");
    List->push_back(std::string("   KFLASH   (rx) : ORIGIN = 0x") + Main::Hex(this->Proj->Kernel->Code_Base) + ", LENGTH = 0x" + Main::Hex(this->Proj->Kernel->Code_Size));
    List->push_back("   /* Init process flash segment */");
    List->push_back(std::string("   IFLASH   (rx) : ORIGIN = 0x") + Main::Hex(this->Proj->Monitor->Code_Base) + ", LENGTH = 0x" + Main::Hex(this->Proj->Kernel->Code_Size));
    List->push_back("   /* Virtual machine flash segment */");
    for (const std::unique_ptr<class Process>& Prc : this->Proj->Process)
    {
        List->push_back(std::string("     /*  ") + Prc->Name + "_FLASH segment */");
        List->push_back(std::string("   ") + Prc->Name + "_FLASH   (rx) : ORIGIN = 0x" + Main::Hex(Prc->Code_Base)+ ", LENGTH = 0x" + Main::Hex(Prc->Code_Size));
    }
    List->push_back("}");
    List->push_back("/* End Memory Definitions ****************************************************/");
    List->push_back("");

    List->push_back("/* Stack Definitions *********************************************************/");
    List->push_back("/* The '__stack' definition is required by crt0, do not remove it. */");
    List->push_back("__stack = ORIGIN(KSRAM) + LENGTH(KSRAM) - 0x100;");
    List->push_back("__initial_sp = __stack;");
    List->push_back("/* End Stack Definitions *****************************************************/");
    List->push_back("");

    List->push_back("/* Entry Point Definitions ***************************************************/");
    List->push_back("/* The entry point is informative, for debuggers and simulators,");
    List->push_back(" * since the Cortex-M vector points to it anyway. */");
    List->push_back("ENTRY(_start);");
    List->push_back("/* End Entry Point Definitions ***********************************************/");
    List->push_back("");

    List->push_back("SECTIONS");
    List->push_back("{");
    List->push_back("/* Begin Section:.init_code ***************************************************");
    List->push_back("Description : The program code for the init process, which goes to flash.");
    List->push_back("Location    : Flash");
    List->push_back("Component   : .text - The code segment.");
    List->push_back("              .rodata.* - The read-only data segment.");
    List->push_back("******************************************************************************/");
    List->push_back(".init_code : ALIGN(4)");
    List->push_back("{");
    List->push_back("    /* Place the init process here */");
    List->push_back("    KEEP(*monitor_image.o(.text .text.*))");
    List->push_back("    KEEP(*monitor_image.o(.rodata .rodata.* .constdata .constdata.*))");
    List->push_back("} > IFLASH");
    List->push_back("/* End Section:.init_text ****************************************************/");
    List->push_back("");
    for (const std::unique_ptr<class Process>& Prc : this->Proj->Process)
    {
        List->push_back(std::string("/* Begin Section:.") + Prc->Name+ "_code *****************************************************");
        List->push_back(std::string("Description : The program code for ") + Prc->Name + ", which goes to flash.");
        List->push_back(std::string("Location    : ") + Prc->Name + " Flash");
        List->push_back(std::string("Component   : .text - The ") + Prc->Name + " code segment.");
        List->push_back("              .rodata.* - The read-only data segment.");
        List->push_back("******************************************************************************/");
        List->push_back(std::string(".")+ Prc->Name + "_code : ALIGN(4)");
        List->push_back("{");
        List->push_back("    /* Place all the virtual machines here */");
        List->push_back(std::string("    KEEP(*") + Prc->Name + ".o(.text .text.*))");
        List->push_back(std::string("    KEEP(*") + Prc->Name + ".o(.rodata .rodata.* .constdata .constdata.*))");
        List->push_back(std::string("} > ") + Prc->Name + "_FLASH");
        List->push_back(std::string("/* End Section:.") + Prc->Name + "_code *****************************************************");
        List->push_back("");
    }
    List->push_back("/* Begin Section:.KERNEL_CODE ********************************************************");
    List->push_back("Description : The program code is stored in the .text section, which goes to FLASH.");
    List->push_back("Location    : Flash");
    List->push_back("Component   : .text - The code segment.");
    List->push_back("              .rodata.* - The read-only data segment.");
    List->push_back("******************************************************************************/");
    List->push_back(".KERNEL_CODE : ALIGN(4)");
    List->push_back("{");
    List->push_back("    /* the startup code */");
    List->push_back("    KEEP(*rme_platform_a7m_gcc.o(.text .text.*))");
    List->push_back("    /* All remaining code */");
    List->push_back("    *(.text .text.*)");
    List->push_back("    /* Read-only data (constants) */");
    List->push_back("    *(.rodata .rodata.* .constdata .constdata.*)");
    List->push_back("} > KFLASH");
    List->push_back("/* End Section:.KERNEL_CODE *********************************************************/");
    List->push_back("");

    List->push_back("/* Begin Section:.KERNEL_INIT ********************************************************");
    List->push_back("Description : The main initialized data section. The program executes knowing that");
    List->push_back("              the data is in the RAM but the loader puts the initial values in the");
    List->push_back("              FLASH (inidata). It is one task of the startup to copy the initial");
    List->push_back("              values from FLASH to RAM. The RME kernel does not really rely on the");
    List->push_back("              data section to be initialized because it never uses preinitialized");
    List->push_back("              global variables.");
    List->push_back("Location    : RAM");
    List->push_back("Component   : .data - The sections to put into the RAM.");
    List->push_back("******************************************************************************/");
    List->push_back("/* Used by the startup code to initialise the .data section */");
    List->push_back("_sidata = LOADADDR(.KERNEL_INIT);");
    List->push_back(".KERNEL_INIT : ALIGN(8192)");
    List->push_back("{");
    List->push_back("    FILL(0xFF)");
    List->push_back("    /* This is used by the startup code to initialise the .data section */");
    List->push_back("    _sdata = . ;        	/* STM specific definition */");
    List->push_back("    __data_start__ = . ;");
    List->push_back("    *(.data_begin .data_begin.*)");
    List->push_back("    *(.data .data.*)");
    List->push_back("    *(.data_end .data_end.*)");
    List->push_back("    . = ALIGN(4);");
    List->push_back("    /* This is used by the startup code to initialise the .data section */");
    List->push_back("    _edata = . ;        	/* STM specific definition */");
    List->push_back("    __data_end__ = . ;");
    List->push_back("} > KIRAM AT > KFLASH");
    List->push_back("/* End Section:.KERNEL_INIT *********************************************************/");
    List->push_back("");

    List->push_back("/* Begin Section:.KERNEL_DATA *********************************************************");
    List->push_back("Description : The initialised-to-0 data sections. NOLOAD is used to avoid");
    List->push_back("              the section .bss type changed to PROGBITS warning. This is the");
    List->push_back("              main region which is placed in RAM. Actually the RME does not");
    List->push_back("              initialize its bss because there is no such need.");
    List->push_back("Location    : RAM");
    List->push_back("Component   : .bss - The sections to put into the RAM, and initialized to 0.");
    List->push_back("******************************************************************************/");
    List->push_back(".KERNEL_DATA (NOLOAD) : ALIGN(4)");
    List->push_back("{");
    List->push_back("    __bss_start__ = .;     	/* standard newlib definition */");
    List->push_back("    _sbss = .;              /* STM specific definition */");
    List->push_back("    *(.bss_begin .bss_begin.*)");
    List->push_back("    *(.bss .bss.*)");
    List->push_back("    *(COMMON)");
    List->push_back("    *(.bss_end .bss_end.*)");
    List->push_back("    . = ALIGN(4);");
    List->push_back("");
    List->push_back("    __bss_end__ = .;        /* standard newlib definition */");
    List->push_back("    _ebss = . ;             /* STM specific definition */");
    List->push_back("} > KIRAM");
    List->push_back("/* End Section:.KERNEL_DATA **********************************************************/");
    List->push_back("");
    List->push_back("/* Begin Section:.noinit ******************************************************");
    List->push_back("Description : The uninitialised data sections. NOLOAD is used to avoid");
    List->push_back("              the section .noinit type changed to PROGBITS warning.");
    List->push_back("Location    : RAM");
    List->push_back("Component   : .noinit - The sections to put into the RAM, and not initialized.");
    List->push_back("******************************************************************************/");
    List->push_back(".noinit (NOLOAD) : ALIGN(4)");
    List->push_back("{");
    List->push_back("    _noinit = .;");
    List->push_back("    *(.noinit .noinit.*) ");
    List->push_back("    . = ALIGN(4) ;");
    List->push_back("    _end_noinit = .;");
    List->push_back("} > KIRAM");
    List->push_back("/* Mandatory to be word aligned, _sbrk assumes this */");
    List->push_back("PROVIDE ( end = _end_noinit ); /* was _ebss */");
    List->push_back("PROVIDE ( _end = _end_noinit );");
    List->push_back("PROVIDE ( __end = _end_noinit );");
    List->push_back("PROVIDE ( __end__ = _end_noinit );");
    List->push_back("/* End Section:.noinit *******************************************************/");
    List->push_back("");
    List->push_back("/* End Of File ***************************************************************/");
    List->push_back("}");
    List->push_back("/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/");
}
/* End Function:GCC_Gen::Kernel_Linker ***************************************/

/* Begin Function:GCC_Gen::Monitor_Linker *************************************
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
    List->push_back("Description : The scatter file for ARMv7-M layout. This file is intended");
    List->push_back(std::string("              to be used with ") + this->Chip->Name + ", and the GNU toolchain.");
    List->push_back("******************************************************************************/");
    List->push_back("");

    List->push_back("/* Memory Definitions *********************************************************");
    List->push_back("Description : This section will define the memory layout of the system.");
    List->push_back("Component   : ORIGIN - Starting address of the memory region.");
    List->push_back("              LENGTH - Length of the region.");
    List->push_back("******************************************************************************/");
    List->push_back("MEMORY");
    List->push_back("{");
    List->push_back("    /* Monitor RAM BASE and SIZE*/");
    List->push_back(std::string("   RAM   (xrw) : ORIGIN =  0x") + Main::Hex(this->Proj->Monitor->Data_Base) + ", LENGTH = 0x" + Main::Hex(this->Proj->Monitor->Data_Size));
    List->push_back("    /* Monitor Stack BASE and SIZE*/");
    List->push_back(std::string("   STACK_RAM   (xrw) : ORIGIN = 0x") + Main::Hex(this->Proj->Monitor->Init_Stack_Base) + ", LENGTH = 0x" + Main::Hex(this->Proj->Monitor->Init_Stack_Size));
    List->push_back("    /* Monitor flash segment */");
    List->push_back(std::string("    FLASH  (rx)  : ORIGIN =  0x") + Main::Hex(this->Proj->Monitor->Code_Base) + ", LENGTH = 0x" + Main::Hex(this->Proj->Monitor->Code_Size));
    List->push_back("}");
    List->push_back("/* End Memory Definitions ****************************************************/");
    List->push_back("");
    List->push_back("");

    List->push_back("/* Entry Point Definitions ***************************************************/");
    List->push_back("/* The entry point is informative, for debuggers and simulators,");
    List->push_back(" * since the Cortex-M vector points to it anyway. */");
    List->push_back("ENTRY(_RVM_Entry);");
    List->push_back("/* End Entry Point Definitions ***********************************************/");
    List->push_back("SECTIONS");
    List->push_back("{");
    List->push_back("/* Begin Section:.Monitor_code ***************************************************");
    List->push_back("Description : The program code for the init process, which goes to flash.");
    List->push_back("Location    : Flash");
    List->push_back("Component   : .text - The code segment.");
    List->push_back("              .rodata.* - The read-only data segment.");
    List->push_back("******************************************************************************/");
    List->push_back(".MONITOR_CODE : ALIGN(4)");
    List->push_back("{");
    List->push_back("    KEEP(*rvm_platform_a7m_gcc.o(.text .text.*))");
    List->push_back("    /* Place the init process here */");
    List->push_back("    *(.text .text.*)");
    List->push_back("    *(.rodata .rodata.* .constdata .constdata.*)");
    List->push_back("} > FLASH");
    List->push_back("/* End Section:.Monitor_code *****************************************************/");
    List->push_back("");
    List->push_back("");

    List->push_back("/* Begin Section:.MONITOR_DATA ********************************************************");
    List->push_back("Description : The main initialized data section. The program executes knowing that");
    List->push_back("              the data is in the RAM but the loader puts the initial values in the");
    List->push_back("              FLASH (inidata). It is one task of the startup to copy the initial");
    List->push_back("              values from FLASH to RAM. The RME kernel does not really rely on the");
    List->push_back("              data section to be initialized because it never uses preinitialized");
    List->push_back("              global variables.");
    List->push_back("Location    : RAM");
    List->push_back("Component   : .MONITOR_DATA - The sections to put into the RAM.");
    List->push_back("******************************************************************************/");
    List->push_back("/* Used by the startup code to initialise the .data section */");
    List->push_back("_sidata = LOADADDR(.MONITOR_DATA);");
    List->push_back(".MONITOR_DATA : ALIGN(8192)");
    List->push_back("{");
    List->push_back("    FILL(0xFF)");
    List->push_back("    /* This is used by the startup code to initialise the .data section */");
    List->push_back("    _sdata = . ;        	/* STM specific definition */");
    List->push_back("    __data_start__ = . ;");
    List->push_back("    *(.data_begin .data_begin.*)");
    List->push_back("    *(.data .data.*)");
    List->push_back("    *(.data_end .data_end.*)");
    List->push_back("    . = ALIGN(4);");
    List->push_back("    /* This is used by the startup code to initialise the .data section */");
    List->push_back("    _edata = . ;        	/* STM specific definition */");
    List->push_back("    __data_end__ = . ;");
    List->push_back("} > RAM AT > FLASH");
    List->push_back("/* End Section:.data *********************************************************/");
    List->push_back("");
    List->push_back("");
    List->push_back("/* Begin Section:.bss *********************************************************");
    List->push_back("Description : The initialised-to-0 data sections. NOLOAD is used to avoid");
    List->push_back("              the section .bss type changed to PROGBITS warning. This is the");
    List->push_back("              main region which is placed in RAM. Actually the RME does not");
    List->push_back("              initialize its bss because there is no such need.");
    List->push_back("Location    : RAM");
    List->push_back("Component   : .bss - The sections to put into the RAM, and initialized to 0.");
    List->push_back("******************************************************************************/");
    List->push_back(".init_bss (NOLOAD) : ALIGN(4)");
    List->push_back("{");
    List->push_back("    __bss_start__ = .;     	/* standard newlib definition */");
    List->push_back("    _sbss = .;              /* STM specific definition */");
    List->push_back("    *(.bss_begin .bss_begin.*)");
    List->push_back("    *(.bss .bss.*)");
    List->push_back("    *(COMMON)");
    List->push_back("    *(.bss_end .bss_end.*)");
    List->push_back("    . = ALIGN(4);");
    List->push_back("    __bss_end__ = .;        /* standard newlib definition */");
    List->push_back("    _ebss = . ;             /* STM specific definition */");
    List->push_back("} > RAM");
    List->push_back("/* End Section:.bss **********************************************************/");
    List->push_back("");
    List->push_back("");
    List->push_back("/* Begin Section:.noinit ******************************************************");
    List->push_back("Description : The uninitialised data sections. NOLOAD is used to avoid");
    List->push_back("              the \"section `.noinit' type changed to PROGBITS\" warning.");
    List->push_back("Location    : RAM");
    List->push_back("Component   : .noinit - The sections to put into the RAM, and not initialized.");
    List->push_back("******************************************************************************/");
    List->push_back(".noinit (NOLOAD) : ALIGN(4)");
    List->push_back("{");
    List->push_back("    _noinit = .;");
    List->push_back("    *(.noinit .noinit.*) ");
    List->push_back("    . = ALIGN(4) ;");
    List->push_back("    _end_noinit = .;   ");
    List->push_back("} > RAM");
    List->push_back("/* Mandatory to be word aligned, _sbrk assumes this */");
    List->push_back("PROVIDE ( end = _end_noinit ); /* was _ebss */");
    List->push_back("PROVIDE ( _end = _end_noinit );");
    List->push_back("PROVIDE ( __end = _end_noinit );");
    List->push_back("PROVIDE ( __end__ = _end_noinit );");
    List->push_back("/* End Section:.noinit *******************************************************/");
    List->push_back("");
    List->push_back("/* Begin Section:.ARM_LIB_STACK *****************************************************/");
    List->push_back(".ARM_LIB_STACK : ALIGN(4)");
    List->push_back("{");
    List->push_back("");
    List->push_back("} > STACK_RAM");
    List->push_back("/* END Section:.ARM_LIB_STACK *******************************************************/");
    List->push_back("");
    List->push_back("/* End Of File ***************************************************************/");
    List->push_back("}");
    List->push_back("/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/");
}
/* End Function:GCC_Gen::Monitor_Linker **************************************/

/* Begin Function:GCC_Gen::Process_Linker *************************************
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
    List->push_back("Description : The scatter file for ARMv7-M layout. This file is intended");
    List->push_back(std::string("              to be used with ") + this->Chip->Name + ", and the GNU toolchain.");
    List->push_back("******************************************************************************/");
    List->push_back("");

    List->push_back("/* Memory Definitions *********************************************************");
    List->push_back("Description : This section will define the memory layout of the system.");
    List->push_back("Component   : ORIGIN - Starting address of the memory region.");
    List->push_back("              LENGTH - Length of the region.");
    List->push_back("******************************************************************************/");
    List->push_back("MEMORY");
    List->push_back("{");
    List->push_back("    /* Process DESC_FLASH BASE and SIZE*/");
    List->push_back(std::string("   DESC_FLASH   (xrw) : ORIGIN =  0x") + Main::Hex(Prc->Code_Base) + ", LENGTH = 0x" + Main::Hex(Header_Size));
    List->push_back("    /* Process CODE_FLASH BASE and SIZE*/");
    List->push_back(std::string("   CODE_FLASH   (xrw) : ORIGIN = 0x") + Main::Hex(Prc->Code_Front) + ", LENGTH = 0x" + Main::Hex(Real_Code_Size));
    List->push_back("    /* Process RAM  segment */");
    List->push_back(std::string("    RAM  (rx)  : ORIGIN =  0x") + Main::Hex(Prc->Data_Base) + ", LENGTH = 0x" + Main::Hex(Prc->Data_Size));
    List->push_back("    /* Process Stack BASE and SIZE*/");
    if (Prc->Type == PROCESS_NATIVE)
        List->push_back(std::string("    STACK_RAM  (rx)  : ORIGIN =  0x") + Main::Hex(Prc->Thread[0]->Stack_Base) + ", LENGTH = 0x" + Main::Hex(Prc->Thread[0]->Stack_Size));
    else
    {
        ASSERT((Prc->Thread[0]->Name == "Vct") && (Prc->Thread[1]->Name == "Usr"));
        List->push_back(std::string("    STACK_RAM  (rx)  : ORIGIN =  0x") + Main::Hex(Prc->Thread[1]->Stack_Base) + ", LENGTH = 0x" + Main::Hex(Prc->Thread[1]->Stack_Size));
    }
    List->push_back("}");
    List->push_back("/* End Memory Definitions ****************************************************/");
    List->push_back("");
    List->push_back("");

    List->push_back("/* Entry Point Definitions ***************************************************/");
    List->push_back("/* The entry point is informative, for debuggers and simulators,");
    List->push_back(" * since the Cortex-M vector points to it anyway. */");
    List->push_back("ENTRY(_start);");
    List->push_back("/* End Entry Point Definitions ***********************************************/");
    List->push_back("SECTIONS");
    List->push_back("{");
    List->push_back("/* Begin Section:.PROCESS_DESC ***************************************************");
    List->push_back("Description : The program code for the init process, which goes to flash.");
    List->push_back("Location    : Flash");
    List->push_back("Component   : .text - The code segment.");
    List->push_back("              .rodata.* - The read-only data segment.");
    List->push_back("******************************************************************************/");
    List->push_back(".PROCESS_DESC : ALIGN(4)");
    List->push_back("{");
    List->push_back(std::string("    KEEP(*prc_") + Prc->Name_Lower + "_desc.o(.text .text.*))");
    List->push_back("} > DESC_FLASH");
    List->push_back("/* End Section:.PROCESS_DESC *****************************************************/");
    List->push_back("");
    List->push_back("");

    List->push_back("/* Begin Section:.PROCESS_CODE ***************************************************");
    List->push_back("Description : The program code for the init process, which goes to flash.");
    List->push_back("Location    : Flash");
    List->push_back("Component   : .text - The code segment.");
    List->push_back("              .rodata.* - The read-only data segment.");
    List->push_back("******************************************************************************/");
    List->push_back(".PROCESS_CODE : ALIGN(4)");
    List->push_back("{");
    List->push_back(std::string("    KEEP(*rvm_guest_") + this->Plat->Name_Lower + "_" + Tool_Lower + ".o(.text .text.*))");
    List->push_back("    /* Place the init process here */");
    List->push_back("    *(.text .text.*)");
    List->push_back("    *(.rodata .rodata.* .constdata .constdata.*)");
    List->push_back("} > CODE_FLASH");
    List->push_back("/* End Section:.PROCESS_CODE *****************************************************/");
    List->push_back("");
    List->push_back("");

    List->push_back("/* Begin Section:.PROCESS_DATA ********************************************************");
    List->push_back("Description : The main initialized data section. The program executes knowing that");
    List->push_back("              the data is in the RAM but the loader puts the initial values in the");
    List->push_back("              FLASH (inidata). It is one task of the startup to copy the initial");
    List->push_back("              values from FLASH to RAM. The RME kernel does not really rely on the");
    List->push_back("              data section to be initialized because it never uses preinitialized");
    List->push_back("              global variables.");
    List->push_back("Location    : RAM");
    List->push_back("Component   : .PROCESS_DATA - The sections to put into the RAM.");
    List->push_back("******************************************************************************/");
    List->push_back("/* Used by the startup code to initialise the .data section */");
    List->push_back("_sidata = LOADADDR(.PROCESS_DATA);");
    List->push_back(".PROCESS_DATA : ALIGN(8192)");
    List->push_back("{");
    List->push_back("    FILL(0xFF)");
    List->push_back("    /* This is used by the startup code to initialise the .data section */");
    List->push_back("    _sdata = . ;        	/* STM specific definition */");
    List->push_back("    __data_start__ = . ;");
    List->push_back("    *(.data_begin .data_begin.*)");
    List->push_back("    *(.data .data.*)");
    List->push_back("    *(.data_end .data_end.*)");
    List->push_back("    . = ALIGN(4);");
    List->push_back("    /* This is used by the startup code to initialise the .data section */");
    List->push_back("    _edata = . ;        	/* STM specific definition */");
    List->push_back("    __data_end__ = . ;");
    List->push_back("} > RAM AT > CODE_FLASH");
    List->push_back("/* End Section:.PROCESS_DATA *********************************************************/");
    List->push_back("");
    List->push_back("");
    List->push_back("/* Begin Section:.bss *********************************************************");
    List->push_back("Description : The initialised-to-0 data sections. NOLOAD is used to avoid");
    List->push_back("              the \"section \`.bss\' type changed to PROGBITS\" warning. This is the");
    List->push_back("              main region which is placed in RAM. Actually the RME does not");
    List->push_back("              initialize its bss because there is no such need.");
    List->push_back("Location    : RAM");
    List->push_back("Component   : .bss - The sections to put into the RAM, and initialized to 0.");
    List->push_back("******************************************************************************/");
    List->push_back(".init_bss (NOLOAD) : ALIGN(4)");
    List->push_back("{");
    List->push_back("    __bss_start__ = .;     	/* standard newlib definition */");
    List->push_back("    _sbss = .;              /* STM specific definition */");
    List->push_back("    *(.bss_begin .bss_begin.*)");
    List->push_back("    *(.bss .bss.*)");
    List->push_back("    *(COMMON)");
    List->push_back("    *(.bss_end .bss_end.*)");
    List->push_back("    . = ALIGN(4);");
    List->push_back("    __bss_end__ = .;        /* standard newlib definition */");
    List->push_back("    _ebss = . ;             /* STM specific definition */");
    List->push_back("} > RAM");
    List->push_back("/* End Section:.bss **********************************************************/");
    List->push_back("");
    List->push_back("");
    List->push_back("/* Begin Section:.noinit ******************************************************");
    List->push_back("Description : The uninitialised data sections. NOLOAD is used to avoid");
    List->push_back("              the \"section `.noinit' type changed to PROGBITS\" warning.");
    List->push_back("Location    : RAM");
    List->push_back("Component   : .noinit - The sections to put into the RAM, and not initialized.");
    List->push_back("******************************************************************************/");
    List->push_back(".noinit (NOLOAD) : ALIGN(4)");
    List->push_back("{");
    List->push_back("    _noinit = .;");
    List->push_back("    *(.noinit .noinit.*) ");
    List->push_back("    . = ALIGN(4) ;");
    List->push_back("    _end_noinit = .;   ");
    List->push_back("} > RAM");
    List->push_back("/* Mandatory to be word aligned, _sbrk assumes this */");
    List->push_back("PROVIDE ( end = _end_noinit ); /* was _ebss */");
    List->push_back("PROVIDE ( _end = _end_noinit );");
    List->push_back("PROVIDE ( __end = _end_noinit );");
    List->push_back("PROVIDE ( __end__ = _end_noinit );");
    List->push_back("/* End Section:.noinit *******************************************************/");
    List->push_back("");
    List->push_back("/* Begin Section:.ARM_LIB_STACK *****************************************************/");
    List->push_back(".ARM_LIB_STACK : ALIGN(4)");
    List->push_back("{");
    List->push_back("    _estack = .;");
    List->push_back("} > STACK_RAM");
    List->push_back("/* END Section:.ARM_LIB_STACK *******************************************************/");
    List->push_back("");
    List->push_back("/* End Of File ***************************************************************/");
    List->push_back("}");
    List->push_back("/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/");

}
/* End Function:GCC_Gen::Process_Linker **************************************/
}
/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
