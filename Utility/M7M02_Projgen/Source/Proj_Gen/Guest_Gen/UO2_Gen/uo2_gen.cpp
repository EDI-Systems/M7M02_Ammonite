/*****************************************************************************
Filename    : uo2_gen.cpp
Author      : pry, ds41f
Date        : 02/11/2026
Licence     : LGPL v3+; see COPYING for details.
Description : The source of the uC/OS-II guest OS generator.
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
#include "filesystem"

#define __HDR_DEF__
#include "rvm_gen.hpp"
#include "Proj_Gen/proj_gen.hpp"
#include "Proj_Gen/Tool_Gen/tool_gen.hpp"
#include "Proj_Gen/Guest_Gen/guest_gen.hpp"
#include "Proj_Gen/Guest_Gen/UO2_Gen/UO2_Gen.hpp"
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
#include "Proj_Gen/Guest_Gen/guest_gen.hpp"
#include "Proj_Gen/Guest_Gen/UO2_Gen/UO2_Gen.hpp"
#undef __HDR_CLASS__
/* End Include ***************************************************************/
namespace RVM_GEN
{

/* Function:UO2_Gen::UO2_Gen***************************************************
Description : Constructor function.
Input       : class Proj_Info* Proj - The project information.
               class Plat_Info* Plat - The platform information.
               class Chip_Info* Chip - The chip information.
Output      : None.
Return      : None.
******************************************************************************/
/* void */ UO2_Gen::UO2_Gen(class Proj_Info* Proj, class Plat_Info* Plat, class Chip_Info* Chip):
           Guest_Gen("UO2", Proj, Plat, Chip)
{
    try
    {

    }
    catch(std::exception& Exc)
    {
        Main::Error(std::string("UO2 generator:\n")+Exc.what());
    }
}
/* End Function:UO2_Gen::UO2_Gen *********************************************/

/* Function:UO2_Gen::Plat_Get**************************************************
Description : Map the platform name onto the uC/OS-II port directory name.
              The uC/OS-II V2.93 tree names its ARM ports after the
              architecture level, exactly like the uC/OS-III tree does.
Input       : None.
Output      : None.
Return      : std::string - The port directory name.
******************************************************************************/
std::string UO2_Gen::Plat_Get(void)
{
    if(this->Plat->Name=="A7M")
        return "ARMv7-M";
    else if(this->Plat->Name=="A6M")
        return "ARMv6-M";
    else if(this->Plat->Name=="RV32P")
        return "RISC-V/RV32";

    Main::Error("UO2 generator:\nNo RVM port available for platform '"+this->Plat->Name+"'.");
    return "";
}
/* End Function:UO2_Gen::Plat_Get ********************************************/

/* Function:UO2_Gen::Toolchain_Get*********************************************
Description : Map the toolchain name onto the uC/OS-II compiler file suffix.
Input       : class Virtual* Virt - The process to generate for.
Output      : None.
Return      : std::string - The assembler file suffix.
******************************************************************************/
std::string UO2_Gen::Toolchain_Get(class Virtual* Virt)
{
    if(Virt->Toolchain=="ARMCC")
        return "ARM";
    else if(Virt->Toolchain=="ICCARM")
        return "IAR";
    else if(this->Plat->Name=="RV32P")
        return "GCC";
    else
        return "GNU";
}
/* End Function:UO2_Gen::Toolchain_Get ***************************************/

/* Function:UO2_Gen::Port_Dir_Get**********************************************
Description : Map the toolchain and the platform onto the port directory of
              the uC/OS-II tree.  The uC/OS-II tree keeps its ARM ports
              under Ports/ARM-Cortex-M/ and its RISC-V ports under
              Ports/RISC-V/, exactly like the uC/OS-III tree.
Input       : class Virtual* Virt - The process to generate for.
Output      : None.
Return      : std::string - The port directory path.
******************************************************************************/
std::string UO2_Gen::Port_Dir_Get(class Virtual* Virt)
{
    if(this->Plat->Name=="RV32P")
        return Main::Guest_UO2_Root+"Ports/"+this->Plat_Get()+"-RVM/"+this->Toolchain_Get(Virt)+"/";
    else
        return Main::Guest_UO2_Root+"Ports/ARM-Cortex-M/"+this->Plat_Get()+"-RVM/"+this->Toolchain_Get(Virt)+"/";
}
/* End Function:UO2_Gen::Port_Dir_Get ****************************************/

/* Function:UO2_Gen::Process_Virt_Hdr******************************************
Description : Create the configuration headers for VMs.
Input       : class Virtual* Virt - The process to generate for.
Output      : None.
Return      : None.
******************************************************************************/
void UO2_Gen::Process_Virt_Hdr(class Virtual* Virt)
{
    std::string Port_Dir;
    std::unique_ptr<std::vector<std::string>> List;

    /* Map the toolchain and the platform onto the port directory. */
    Port_Dir=Port_Dir_Get(Virt);

    /* See if the uC/OS-II tree and the port files are actually there. */
    if(std::filesystem::exists(Main::Guest_UO2_Root+"Source/ucos_ii.h")==false)
        Main::Error("UO2 generator:\nThe uC/OS-II tree '"+Main::Guest_UO2_Root+"' does not contain 'Source/ucos_ii.h'.");
    if(std::filesystem::exists(Port_Dir+"os_cpu.h")==false)
        Main::Error("UO2 generator:\nThe RVM port folder '"+Port_Dir+"' does not contain 'os_cpu.h'.");
    if(std::filesystem::exists(Port_Dir+"os_cpu_a.s")==false)
        Main::Error("UO2 generator:\nThe RVM port folder '"+Port_Dir+"' does not contain 'os_cpu_a.s'.");
    if(std::filesystem::exists(Port_Dir+"../Template/os_cfg.h")==false)
        Main::Error("UO2 generator:\nThe RVM port folder '"+Port_Dir+"' does not contain '../Template/os_cfg.h'.");

    /* Add the kernel headers and the port itself into the VM.  The port's
     * os_cpu.h sits in the toolchain subdirectory, the shared os_cpu_c.c
     * sits one level up, and the kernel's ucos_ii.h includes <os_cpu.h>
     * with angle brackets so both directories must be on the path. */
    Virt->Virtual_Include.push_back(Main::Guest_UO2_Root+"Source/");
    Virt->Virtual_Include.push_back(Port_Dir);
    Virt->Virtual_Include.push_back(Port_Dir+"../");

    /* Copy the configuration files from the port template folders into the VM.
     * The configurations belong to each individual project - the user is free
     * to edit the generated copies - so an existing copy is never replaced
     * unless regeneration is forced.  The template folders themselves are
     * never added into the include paths.  uC/OS-II has only the two
     * OS-side templates on the architecture layer - no os_cfg_app.h, no
     * cpu_cfg.h, no lib_cfg.h - because the kernel does not depend on the
     * uC-CPU or uC-LIB distributions. */
    const std::pair<std::string,std::string> Cfg_List[]=
    {
        std::make_pair(Port_Dir+"../Template/", "os_cfg.h"),
        std::make_pair(Port_Dir+"../Template/", "app_cfg.h")
    };
    for(const std::pair<std::string,std::string>& Cfg:Cfg_List)
    {
        if((std::filesystem::exists(Virt->Virtual_Header_Output+Cfg.second)==false)||
           (Virt->Virtual_Header_Overwrite!=0))
        {
            Main::Info(std::string("> Generating '")+Cfg.second+"'.");
            /* This may pop up: gcc bug - ignoring copy_options, will fix in the future */
            std::filesystem::copy(Cfg.first+Cfg.second,
                                  Virt->Virtual_Header_Output+Cfg.second,
                                  std::filesystem::copy_options::overwrite_existing);
        }
        else
            Main::Info(std::string("> File '")+Cfg.second+"' exists, skipping generation.");
    }

    /* The test header only exists in benchmark builds - the plain application
     * boots through the port's os_cpu_c.c and needs no test hardware. */
    if(Main::Benchmark!=0)
    {
        /* Add the test include path into the VM. */
        Virt->Virtual_Include.push_back(Main::Guest_UO2_Root+"Test/");

        Main::Info("> Generating test selection header.");
        List=std::make_unique<std::vector<std::string>>();
        Proj_Gen::Src_Head(List, "uo2_test.h", "The uC/OS-II test header.");
        List->push_back("");
        List->push_back("/* Include *******************************************************************/");
        List->push_back(std::string("#include \"chip/uo2_test_")+this->Chip->Name_Lower+"_rvm.h\"");
        List->push_back("/* End Include ***************************************************************/");
        List->push_back("");
        Proj_Gen::Src_Foot(List);

        Proj_Gen::Line_Write(List, Virt->Virtual_Header_Output+"uo2_test.h");
    }
}
/* End Function:UO2_Gen::Process_Virt_Hdr ************************************/

/* Function:UO2_Gen::Process_Virt_Src******************************************
Description : Create the configuration source for VMs.
Input       : class Virtual* Virt - The process to generate for.
              class Tool_Gen* Tool - The toolchain to use.
Output      : None.
Return      : None.
******************************************************************************/
void UO2_Gen::Process_Virt_Src(class Virtual* Virt, class Tool_Gen* Tool)
{
    std::string Port_Dir;

    (void)Tool;

    /* Map the toolchain and the platform onto the port directory. */
    Port_Dir=Port_Dir_Get(Virt);

    /* Add all sources that this depends on: the RVM port (the fast-path
     * switch assembly and the C layer with the critical sections, the hooks,
     * the context handler and the boot hooks) and the complete uC/OS-II
     * kernel core.  The kernel is self-contained - no uC-CPU, no uC-LIB. */
    Virt->Virtual_Library.push_back(Port_Dir+"../os_cpu_c.c");
    Virt->Virtual_Library.push_back(Port_Dir+"os_cpu_a.s");
    Virt->Virtual_Library.push_back(Main::Guest_UO2_Root+"Source/os_core.c");
    Virt->Virtual_Library.push_back(Main::Guest_UO2_Root+"Source/os_flag.c");
    Virt->Virtual_Library.push_back(Main::Guest_UO2_Root+"Source/os_mbox.c");
    Virt->Virtual_Library.push_back(Main::Guest_UO2_Root+"Source/os_mem.c");
    Virt->Virtual_Library.push_back(Main::Guest_UO2_Root+"Source/os_mutex.c");
    Virt->Virtual_Library.push_back(Main::Guest_UO2_Root+"Source/os_q.c");
    Virt->Virtual_Library.push_back(Main::Guest_UO2_Root+"Source/os_sem.c");
    Virt->Virtual_Library.push_back(Main::Guest_UO2_Root+"Source/os_task.c");
    Virt->Virtual_Library.push_back(Main::Guest_UO2_Root+"Source/os_time.c");
    Virt->Virtual_Library.push_back(Main::Guest_UO2_Root+"Source/os_tmr.c");
    Virt->Virtual_Library.push_back(Main::Guest_UO2_Root+"Source/os_dbg_r.c");

    /* The benchmark carries the two test timers and the chip test header;
     * the plain application boots through the port and never sees them. */
    if(Main::Benchmark!=0)
        Virt->Virtual_Source.push_back(Main::Guest_UO2_Root+"Test/uo2_benchmark.c");
    else
    {
        Virt->Virtual_Source.push_back(Virt->Virtual_Source_Output+"main.c");

        /* Copy the plain application to the folder, in case that it is not there before. */
        if(std::filesystem::exists(Virt->Virtual_Source_Output+"main.c")==true)
        {
            /* See if we'll use forced regenerate. */
            if(Virt->Virtual_Source_Overwrite==0)
            {
                Main::Info("> File 'main.c' exists, skipping generation.");
                return;
            }
        }

        Main::Info("> Generating 'main.c'.");
        std::filesystem::copy(Main::Guest_UO2_Root+"Test/uo2_main.c",
                              Virt->Virtual_Source_Output+"main.c",
                              std::filesystem::copy_options::overwrite_existing);
    }
}
/* End Function:UO2_Gen::Process_Virt_Src ************************************/
}
/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
