/***************************************************************************** 
Filename    : RTT_Gen.cpp
Author      : pry, Cline
Date        : 26/10/2026
Licence     : LGPL v3+; see COPYING for details.
Description : The RT-Thread guest OS generator.
/******************************************************************************/

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
#include "Proj_Gen/Guest_Gen/RTT_Gen/rtt_gen.hpp"
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
#include "Proj_Gen/Guest_Gen/RTT_Gen/rtt_gen.hpp"
#undef __HDR_CLASS__
/* End Include ***************************************************************/

/* Define ********************************************************************/
namespace RVM_GEN
{
/* Function:RTT_Gen::RTT_Gen **************************************************
Description : Generator for the RT-Thread.
Input       : class Proj_Info* Proj - The project information.
               class Plat_Info* Plat - The platform information.
               class Chip_Info* Chip - The chip information.
Output      : None.
Return      : None.
/******************************************************************************/
/* void */ RTT_Gen::RTT_Gen(class Proj_Info* Proj, class Plat_Info* Plat, class Chip_Info* Chip):
Guest_Gen("RTT", Proj, Plat, Chip)
{
    try
    {

    }
    catch(std::exception& Exc)
    {
        Main::Error(std::string("RT-Thread generator:\n")+Exc.what());
    }
}
/* End Function:RTT_Gen::RTT_Gen *********************************************/

/* Function:RTT_Gen::Plat_Get *************************************************
Description : Map the platform name onto the RT-Thread libcpu architecture
              directory name that carries the official taxonomy.
Input       : None.
Output      : None.
Return      : std::string - The architecture directory name.
/******************************************************************************/
std::string RTT_Gen::Plat_Get(void)
{
    if(this->Plat->Name=="A6M")
        return "armv6m";
    else if(this->Plat->Name=="A7M")
        return "armv7m";
    else if(this->Plat->Name=="RV32P")
        return "rv32p";

    Main::Error("RT-Thread generator:\nNo RVM port available for platform '"+this->Plat->Name+"'.");
    return "";
}
/* End Function:RTT_Gen::Plat_Get *************************************/

/* Function:RTT_Gen::Toolchain_Get ********************************************
Description : Map the toolchain name onto the RT-Thread compiler file suffix
              (gcc = GNU as, iar = iarasym, rvds = armasm - the official
              libcpu naming, in lowercase like our ports).
Input       : class Virtual* Virt - The process to generate for.
Output      : None.
Return      : std::string - The compiler file suffix.
/******************************************************************************/
std::string RTT_Gen::Toolchain_Get(class Virtual* Virt)
{
    if(Virt->Toolchain=="ARMCC")
        return "rvds";
    else if(Virt->Toolchain=="ICCARM")
        return "iar";

    return "gcc";
}
/* End Function:RTT_Gen::Toolchain_Get *******************************/

/* Function:RTT_Gen::Port_Dir_Get *********************************************
Description : Map the toolchain and the platform onto the port directory of
              the RT-Thread tree. The port directories sit flat next to the
              official libcpu architecture directories, and keep the official
              flat layout - one cpuport.c for all compilers, and one context
              switch assembly per compiler suffix.
Input       : class Virtual* Virt - The process to generate for.
Output      : None.
Return      : std::string - The port directory.
/******************************************************************************/
std::string RTT_Gen::Port_Dir_Get(class Virtual* Virt)
{
    (void)Virt;

    if(this->Plat->Name=="RV32P")
        return Main::Guest_RTT_Root+"libcpu/risc-v/"+this->Plat_Get()+"_rvm/";

    return Main::Guest_RTT_Root+"libcpu/arm/"+this->Plat_Get()+"_rvm/";
}
/* End Function:RTT_Gen::Port_Dir_Get *******************************/

/* Function:RTT_Gen::Process_Virt_Hdr *****************************************
Description : Create the configuration headers for VMs.
Input       : class Virtual* Virt - The process to generate for.
Output      : None.
Return      : None.
/******************************************************************************/
void RTT_Gen::Process_Virt_Hdr(class Virtual* Virt)
{
    std::string Port_Dir;
    std::unique_ptr<std::vector<std::string>> List;

    /* Map the toolchain and the platform onto the port directory. */
    Port_Dir=Port_Dir_Get(Virt);

    /* See if the RT-Thread tree and the port files are actually there.  The
     * context switch assembly is a hard requirement - the switch is its
     * whole point; only the GCC variant exists for now, and the check on
     * the lowercase name doubles as the port capability probe for the
     * toolchains whose official capitalization is still in place. */
    if(std::filesystem::exists(Main::Guest_RTT_Root+"include/rtthread.h")==false)
        Main::Error("RT-Thread generator:\nThe RT-Thread tree '"+Main::Guest_RTT_Root+"' does not contain 'include/rtthread.h'.");
    if(std::filesystem::exists(Port_Dir+"cpuport.c")==false)
        Main::Error("RT-Thread generator:\nThe RVM port folder '"+Port_Dir+"' does not contain 'cpuport.c'.");
    if(std::filesystem::exists(Port_Dir+"context_"+this->Toolchain_Get(Virt)+".s")==false)
        Main::Error("RT-Thread generator:\nThe RVM port folder '"+Port_Dir+"' does not contain 'context_"+this->Toolchain_Get(Virt)+".s'.");
    if(std::filesystem::exists(Port_Dir+"Template/rtconfig.h")==false)
        Main::Error("RT-Thread generator:\nThe RVM port folder '"+Port_Dir+"' does not contain 'Template/rtconfig.h'.");

    /* Add the kernel headers and the port headers into the VM. */
    Virt->Virtual_Include.push_back(Main::Guest_RTT_Root+"include/");
    Virt->Virtual_Include.push_back(Port_Dir);

    /* Copy the configuration file from the port template folder into the VM.
     * The configuration belongs to each individual project - the user is
     * free to edit the generated copy - so an existing copy is never
     * replaced unless regeneration is forced.  The template folder itself
     * is never added into the include paths. */
    if((std::filesystem::exists(Virt->Virtual_Header_Output+"rtconfig.h")==false)||
       (Virt->Virtual_Header_Overwrite!=0))
    {
        Main::Info("> Generating 'rtconfig.h'.");
        /* This may pop up: gcc bug - ignoring copy_options, will fix in the future */
        std::filesystem::copy(Port_Dir+"Template/rtconfig.h",
                              Virt->Virtual_Header_Output+"rtconfig.h",
                              std::filesystem::copy_options::overwrite_existing);
    }
    else
        Main::Info("> File 'rtconfig.h' exists, skipping generation.");

    /* If we're running benchmark, need to generate the test header as well */
    if(Main::Benchmark!=0)
    {
        /* Add the test include path into the VM */
        Virt->Virtual_Include.push_back(Main::Guest_RTT_Root+"test/");

        Main::Info("> Generating test selection header.");
        List=std::make_unique<std::vector<std::string>>();
        Proj_Gen::Src_Head(List, "rtt_test.h", "The RTT test header.");
        List->push_back("");
        List->push_back("/* Include *******************************************************************/");
        List->push_back(std::string("#include \"chip/rtt_test_")+this->Chip->Name_Lower+"_rvm.h\"");
        List->push_back("/* End Include ***************************************************************/");
        List->push_back("");
        Proj_Gen::Src_Foot(List);

        Proj_Gen::Line_Write(List, Virt->Virtual_Header_Output+"rtt_test.h");
    }
}
/* End Function:RTT_Gen::Process_Virt_Hdr ************************************/

/* Function:RTT_Gen::Process_Virt_Src *****************************************
Description : Create the configuration source for VMs.
Input       : class Virtual* Virt - The process to generate for.
               class Tool_Gen* Tool - The toolchain to use.
Output      : None.
Return      : None.
/******************************************************************************/
void RTT_Gen::Process_Virt_Src(class Virtual* Virt, class Tool_Gen* Tool)
{
    std::string Port_Dir;

    (void)Tool;

    /* Map the toolchain and the platform onto the port directory. */
    Port_Dir=Port_Dir_Get(Virt);

    /* Add all sources that this depends on: the OS-side port (the shared
     * cpuport.c and the per-compiler context switch assembly), the board
     * support that hooks the kernel up to the hypervisor, and the complete
     * RT-Thread kernel source subset that the default configuration uses -
     * the disabled kernel modules compile empty. */
    Virt->Virtual_Library.push_back(Port_Dir+"cpuport.c");
    Virt->Virtual_Library.push_back(Port_Dir+"context_"+this->Toolchain_Get(Virt)+".s");
    Virt->Virtual_Library.push_back(Main::Guest_RTT_Root+"test/rtt_board.c");
    Virt->Virtual_Library.push_back(Main::Guest_RTT_Root+"src/clock.c");
    Virt->Virtual_Library.push_back(Main::Guest_RTT_Root+"src/components.c");
    Virt->Virtual_Library.push_back(Main::Guest_RTT_Root+"src/cpu.c");
    Virt->Virtual_Library.push_back(Main::Guest_RTT_Root+"src/idle.c");
    Virt->Virtual_Library.push_back(Main::Guest_RTT_Root+"src/ipc.c");
    Virt->Virtual_Library.push_back(Main::Guest_RTT_Root+"src/irq.c");
    Virt->Virtual_Library.push_back(Main::Guest_RTT_Root+"src/kservice.c");
    Virt->Virtual_Library.push_back(Main::Guest_RTT_Root+"src/mem.c");
    Virt->Virtual_Library.push_back(Main::Guest_RTT_Root+"src/mempool.c");
    Virt->Virtual_Library.push_back(Main::Guest_RTT_Root+"src/object.c");
    Virt->Virtual_Library.push_back(Main::Guest_RTT_Root+"src/scheduler.c");
    Virt->Virtual_Library.push_back(Main::Guest_RTT_Root+"src/thread.c");
    Virt->Virtual_Library.push_back(Main::Guest_RTT_Root+"src/timer.c");

    /* Add benchmark or basic user stubs */
    if(Main::Benchmark!=0)
        Virt->Virtual_Source.push_back(Main::Guest_RTT_Root+"test/rtt_benchmark.c");
    else
    {
        Virt->Virtual_Source.push_back(Virt->Virtual_Source_Output+"main.c");

        /* Copy the blank file to the folder, in case that it is not there before */
        if(std::filesystem::exists(Virt->Virtual_Source_Output+"main.c")==true)
        {
            /* See if we'll use forced regenerate */
            if(Virt->Virtual_Source_Overwrite==0)
            {
                Main::Info("> File 'main.c' exists, skipping generation.");
                return;
            }
        }

        Main::Info("> Generating 'main.c'.");
        std::filesystem::copy(Main::Guest_RTT_Root+"test/rtt_main.c",
                              Virt->Virtual_Source_Output+"main.c",
                              std::filesystem::copy_options::overwrite_existing);
    }
}
/* End Function:RTT_Gen::Process_Virt_Src ************************************/
}
/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
