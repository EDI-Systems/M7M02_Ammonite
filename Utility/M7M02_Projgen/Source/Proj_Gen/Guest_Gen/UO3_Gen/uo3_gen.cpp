/****************************************************************************** 
Filename    : UO3_Gen.cpp
Author      : pry, Cline
Date        : 22/10/2026
Licence     : LGPL v3+; see COPYING for details.
Description : The uC/OS-III guest OS generator.
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
#include "Proj_Gen/Guest_Gen/UO3_Gen/uo3_gen.hpp"
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
#include "Proj_Gen/Guest_Gen/UO3_Gen/uo3_gen.hpp"
#undef __HDR_CLASS__
/* End Include ***************************************************************/

/* Define ********************************************************************/
namespace RVM_GEN
{
/* Function:UO3_Gen::UO3_Gen **************************************************
Description : Generator for the uC/OS-III.
Input       : class Proj_Info* Proj - The project information.
               class Plat_Info* Plat - The platform information.
               class Chip_Info* Chip - The chip information.
Output      : None.
Return      : None.
******************************************************************************/
/* void */ UO3_Gen::UO3_Gen(class Proj_Info* Proj, class Plat_Info* Plat, class Chip_Info* Chip):
Guest_Gen("UO3", Proj, Plat, Chip)
{
    try
    {

    }
    catch(std::exception& Exc)
    {
        Main::Error(std::string("uC/OS-III generator:\n")+Exc.what());
    }
}
/* End Function:UO3_Gen::UO3_Gen *********************************************/

/* Function:UO3_Gen::Plat_Get *************************************************
Description : Map the platform name onto the uC/OS-III architecture directory
               name that carries the official taxonomy.
Input       : None.
Output      : None.
Return      : std::string - The architecture directory name.
******************************************************************************/
std::string UO3_Gen::Plat_Get(void)
{
    if(this->Plat->Name=="A6M")
        return "ARMv6-M";
    else if(this->Plat->Name=="A7M")
        return "ARMv7-M";
    else if(this->Plat->Name=="RV32P")
        return "RISC-V/RV32";

    Main::Error("uC/OS-III generator:\nNo RVM port available for platform '"+this->Plat->Name+"'.");
    return "";
}
/* End Function:UO3_Gen::Plat_Get ***************************************/

/* Function:UO3_Gen::Toolchain_Get ********************************************
Description : Map the toolchain name onto the Micrium compiler directory name
               (ARM = RealView/armcc, GNU = GCC, IAR = ICCARM).
Input       : class Virtual* Virt - The process to generate for.
Output      : None.
Return      : std::string - The compiler directory name.
******************************************************************************/
std::string UO3_Gen::Toolchain_Get(class Virtual* Virt)
{
    if(Virt->Toolchain=="ARMCC")
        return "ARM";
    else if(Virt->Toolchain=="ICCARM")
        return "IAR";
    else if(this->Plat->Name=="RV32P")
        return "GCC";

    return "GNU";
}
/* End Function:UO3_Gen::Toolchain_Get ***********************************/

/* Function:UO3_Gen::Port_Dir_Get *********************************************
Description : Map the toolchain and the platform onto the port directories of
               the uC/OS-III tree. The port directories follow the native
               Micrium taxonomy: the RVM variants sit next to the official
               architecture ports, and the compiler sublayers keep the
               official names.
Input       : class Virtual* Virt - The process to generate for.
Output      : None.
Return      : std::string - The OS-side port directory.
               std::string - The CPU-side port directory.
******************************************************************************/
std::pair<std::string, std::string> UO3_Gen::Port_Dir_Get(class Virtual* Virt)
{
    /* The RISC-V CPU side has no architecture sublayer in the official tree,
     * so the RVM variant sits flat under the uC/CPU root next to the official
     * RISC-V directory. */
    if(this->Plat->Name=="RV32P")
        return std::make_pair(Main::Guest_UO3_Root+"uC-OS3/Ports/"+this->Plat_Get()+"-RVM/"+
                              this->Toolchain_Get(Virt)+"/",
                              Main::Guest_UO3_Root+"uC-CPU/RISC-V-RVM/");

    return std::make_pair(Main::Guest_UO3_Root+"uC-OS3/Ports/ARM-Cortex-M/"+
                          this->Plat_Get()+"-RVM/"+this->Toolchain_Get(Virt)+"/",
                          Main::Guest_UO3_Root+"uC-CPU/ARM-Cortex-M/"+this->Plat_Get()+"-RVM/");
}
/* End Function:UO3_Gen::Port_Dir_Get ****************************************/

/* Function:UO3_Gen::Process_Virt_Hdr *****************************************
Description : Create the configuration headers for VMs.
Input       : class Virtual* Virt - The process to generate for.
Output      : None.
Return      : None.
******************************************************************************/
void UO3_Gen::Process_Virt_Hdr(class Virtual* Virt)
{
    std::pair<std::string, std::string> Port_Dir;
    std::unique_ptr<std::vector<std::string>> List;

    /* Map the toolchain and the platform onto the port directories. */
    Port_Dir=Port_Dir_Get(Virt);

    /* See if the uC/OS-III tree and the port files are actually there.  The
     * port's context switch assembly is a hard requirement - the switch is
     * its whole point; the CPU layer's cpu_a.s below is the optional one. */
    if(std::filesystem::exists(Main::Guest_UO3_Root+"uC-OS3/Source/os.h")==false)
        Main::Error("uC/OS-III generator:\nThe uC/OS-III tree '"+Main::Guest_UO3_Root+"' does not contain 'uC-OS3/Source/os.h'.");
    if(std::filesystem::exists(Port_Dir.first+"os_cpu.h")==false)
        Main::Error("uC/OS-III generator:\nThe RVM port folder '"+Port_Dir.first+"' does not contain 'os_cpu.h'.");
    if(std::filesystem::exists(Port_Dir.first+"os_cpu_a.s")==false)
        Main::Error("uC/OS-III generator:\nThe RVM port folder '"+Port_Dir.first+"' does not contain 'os_cpu_a.s'.");
    if(std::filesystem::exists(Port_Dir.first+"../Template/os_cfg.h")==false)
        Main::Error("uC/OS-III generator:\nThe RVM port folder '"+Port_Dir.first+"' does not contain '../Template/os_cfg.h'.");
    if(std::filesystem::exists(Port_Dir.first+"../Template/os_cfg_app.h")==false)
        Main::Error("uC/OS-III generator:\nThe RVM port folder '"+Port_Dir.first+"' does not contain '../Template/os_cfg_app.h'.");
    if(std::filesystem::exists(Port_Dir.second+"Template/cpu_cfg.h")==false)
        Main::Error("uC/OS-III generator:\nThe RVM CPU folder '"+Port_Dir.second+"' does not contain 'Template/cpu_cfg.h'.");
    if(std::filesystem::exists(Port_Dir.second+"Template/lib_cfg.h")==false)
        Main::Error("uC/OS-III generator:\nThe RVM CPU folder '"+Port_Dir.second+"' does not contain 'Template/lib_cfg.h'.");

    /* Add the kernel headers, the port headers, the port configuration and
     * the uC/LIB headers into the VM.  The RVM CPU layer is self-sufficient. */
    Virt->Virtual_Include.push_back(Port_Dir.first);
    Virt->Virtual_Include.push_back(Main::Guest_UO3_Root+"uC-OS3/Source/");
    Virt->Virtual_Include.push_back(Main::Guest_UO3_Root+"uC-CPU/");
    Virt->Virtual_Include.push_back(Main::Guest_UO3_Root+"uC-LIB/");
    Virt->Virtual_Include.push_back(Port_Dir.second);
    Virt->Virtual_Include.push_back(Port_Dir.second+this->Toolchain_Get(Virt)+"/");

    /* Copy the configuration files from the port template folders into the VM.
     * The configurations belong to each individual project - the user is free
     * to edit the generated copies - so an existing copy is never replaced
     * unless regeneration is forced.  The template folders themselves are
     * never added into the include paths.  The OS-side templates sit on the
     * architecture layer (shared by all compiler sublayers), the CPU-side
     * templates sit in the CPU port root. */
    const std::pair<std::string,std::string> Cfg_List[]=
    {
        std::make_pair(Port_Dir.first+"../Template/", "os_cfg.h"),
        std::make_pair(Port_Dir.first+"../Template/", "os_cfg_app.h"),
        std::make_pair(Port_Dir.second+"Template/", "cpu_cfg.h"),
        std::make_pair(Port_Dir.second+"Template/", "lib_cfg.h")
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

    /* If we're running benchmark, need to generate the test header as well */
    if(Main::Benchmark!=0)
    {
        /* Add the test include path into the VM */
        Virt->Virtual_Include.push_back(Main::Guest_UO3_Root+"uC-OS3/Test/");

        Main::Info("> Generating test selection header.");
        List=std::make_unique<std::vector<std::string>>();
        Proj_Gen::Src_Head(List, "uo3_test.h", "The UO3 test header.");
        List->push_back("");
        List->push_back("/* Include *******************************************************************/");
        List->push_back(std::string("#include \"chip/uo3_test_")+this->Chip->Name_Lower+"_rvm.h\"");
        List->push_back("/* End Include ***************************************************************/");
        List->push_back("");
        Proj_Gen::Src_Foot(List);

        Proj_Gen::Line_Write(List, Virt->Virtual_Header_Output+"uo3_test.h");
    }
}
/* End Function:UO3_Gen::Process_Virt_Hdr ************************************/

/* Function:UO3_Gen::Process_Virt_Src *****************************************
Description : Create the configuration source for VMs.
Input       : class Virtual* Virt - The process to generate for.
               class Tool_Gen* Tool - The toolchain to use.
Output      : None.
Return      : None.
******************************************************************************/
void UO3_Gen::Process_Virt_Src(class Virtual* Virt, class Tool_Gen* Tool)
{
    std::pair<std::string, std::string> Port_Dir;

    (void)Tool;

    /* Map the toolchain and the platform onto the port directories. */
    Port_Dir=Port_Dir_Get(Virt);

    /* Add all sources that this depends on: the OS-side port (the shared
     * os_cpu_c.c at the architecture level and the per-compiler os_cpu_a
     * context switch assembly), the uC/CPU core, the six-function RVM
     * replacement of the official cpu_a masking functions, and the complete
     * uC/OS-III kernel source - the disabled kernel modules compile empty.
     * The CPU layer's own cpu_a.s - the count-zeros accelerations that the
     * instruction set actually offers - is compiled whenever the layer
     * ships one; the layers with nothing to accelerate carry no file. */
    Virt->Virtual_Library.push_back(Port_Dir.first+"../os_cpu_c.c");
    Virt->Virtual_Library.push_back(Port_Dir.first+"os_cpu_a.s");
    Virt->Virtual_Library.push_back(Port_Dir.second+"cpu_rvm.c");
    if(std::filesystem::exists(Port_Dir.second+this->Toolchain_Get(Virt)+"/cpu_a.s")==true)
        Virt->Virtual_Library.push_back(Port_Dir.second+this->Toolchain_Get(Virt)+"/cpu_a.s");
    Virt->Virtual_Library.push_back(Main::Guest_UO3_Root+"uC-CPU/cpu_core.c");
    Virt->Virtual_Library.push_back(Main::Guest_UO3_Root+"uC-OS3/Source/os_cfg_app.c");
    Virt->Virtual_Library.push_back(Main::Guest_UO3_Root+"uC-OS3/Source/os_core.c");
    Virt->Virtual_Library.push_back(Main::Guest_UO3_Root+"uC-OS3/Source/os_dbg.c");
    Virt->Virtual_Library.push_back(Main::Guest_UO3_Root+"uC-OS3/Source/os_flag.c");
    Virt->Virtual_Library.push_back(Main::Guest_UO3_Root+"uC-OS3/Source/os_mem.c");
    Virt->Virtual_Library.push_back(Main::Guest_UO3_Root+"uC-OS3/Source/os_msg.c");
    Virt->Virtual_Library.push_back(Main::Guest_UO3_Root+"uC-OS3/Source/os_mutex.c");
    Virt->Virtual_Library.push_back(Main::Guest_UO3_Root+"uC-OS3/Source/os_prio.c");
    Virt->Virtual_Library.push_back(Main::Guest_UO3_Root+"uC-OS3/Source/os_q.c");
    Virt->Virtual_Library.push_back(Main::Guest_UO3_Root+"uC-OS3/Source/os_sem.c");
    Virt->Virtual_Library.push_back(Main::Guest_UO3_Root+"uC-OS3/Source/os_stat.c");
    Virt->Virtual_Library.push_back(Main::Guest_UO3_Root+"uC-OS3/Source/os_task.c");
    Virt->Virtual_Library.push_back(Main::Guest_UO3_Root+"uC-OS3/Source/os_tick.c");
    Virt->Virtual_Library.push_back(Main::Guest_UO3_Root+"uC-OS3/Source/os_time.c");
    Virt->Virtual_Library.push_back(Main::Guest_UO3_Root+"uC-OS3/Source/os_tmr.c");
    Virt->Virtual_Library.push_back(Main::Guest_UO3_Root+"uC-OS3/Source/os_var.c");

    /* Add benchmark or basic user stubs */
    if(Main::Benchmark!=0)
        Virt->Virtual_Source.push_back(Main::Guest_UO3_Root+"uC-OS3/Test/uo3_benchmark.c");
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
        std::filesystem::copy(Main::Guest_UO3_Root+"uC-OS3/Test/uo3_main.c",
                              Virt->Virtual_Source_Output+"main.c",
                              std::filesystem::copy_options::overwrite_existing);
    }
}
/* End Function:UO3_Gen::Process_Virt_Src ************************************/
}
/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
