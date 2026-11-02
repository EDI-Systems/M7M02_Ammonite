/*****************************************************************************
Filename    : klt_gen.cpp
Author      : pry, ds41f
Date        : 31/10/2026
Licence     : LGPL v3+; see COPYING for details.
Description : The source of the KLite guest OS generator.
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
#include "Plat_Info/plat_info.hpp"
#include "Chip_Info/chip_info.hpp"
#include "Conf_Info/conf_info.hpp"

#include "Proj_Gen/proj_gen.hpp"
#include "Proj_Gen/Tool_Gen/tool_gen.hpp"
#include "Proj_Gen/Guest_Gen/guest_gen.hpp"
#include "Proj_Gen/Guest_Gen/KLT_Gen/KLT_Gen.hpp"
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
#include "Proj_Gen/Guest_Gen/KLT_Gen/KLT_Gen.hpp"
#undef __HDR_CLASS__
/* End Include ***************************************************************/
namespace RVM_GEN
{
/* Function:KLT_Gen::KLT_Gen **************************************************
Description : Constructor function.
Input       : class Proj_Info* Proj - The project information.
               class Plat_Info* Plat - The platform information.
               class Chip_Info* Chip - The chip information.
Output      : None.
Return      : None.
******************************************************************************/
/* void */ KLT_Gen::KLT_Gen(class Proj_Info* Proj, class Plat_Info* Plat, class Chip_Info* Chip):
Guest_Gen("KLT", Proj, Plat, Chip)
{
    try
    {

    }
    catch(std::exception& Exc)
    {
        Main::Error(std::string("KLT generator:\n")+Exc.what());
    }
}
/* End Function:KLT_Gen::KLT_Gen *********************************************/

/* Function:KLT_Gen::Plat_Get *************************************************
Description : Map the platform name onto the KLite cpu directory name.  The
              port is architecture level and integer-only - it uses no
              ARMv7E-M extensions, so it lives under the plain ARMv7-M
              name, the same convention the uC/OS-III port follows.
Input       : None.
Output      : None.
Return      : std::string - The port directory name.
******************************************************************************/
std::string KLT_Gen::Plat_Get(void)
{
    if(this->Plat->Name=="A7M")
        return "armv7-m";

    Main::Error("KLite generator:\nNo RVM port available for platform '"+this->Plat->Name+"'.");
    return "";
}
/* End Function:KLT_Gen::Plat_Get ********************************************/

/* Function:KLT_Gen::Toolchain_Get ********************************************
Description : Map the toolchain name onto the KLite assembler file suffix.
Input       : class Virtual* Virt - The process to generate for.
Output      : None.
Return      : std::string - The assembler file suffix.
******************************************************************************/
std::string KLT_Gen::Toolchain_Get(class Virtual* Virt)
{
    (void)Virt;

    /* Only the GCC variant exists - the single combination this
     * distribution carries. */
    return "gcc";
}
/* End Function:KLT_Gen::Toolchain_Get ***************************************/

/* Function:KLT_Gen::Port_Dir_Get *********************************************
Description : Map the toolchain and the platform onto the port directory of
              the KLite tree.  KLite keeps its ports under sources/cpu/,
              next to the official cores it names after the ARM core.
Input       : class Virtual* Virt - The process to generate for.
Output      : None.
Return      : std::string - The port directory path.
******************************************************************************/
std::string KLT_Gen::Port_Dir_Get(class Virtual* Virt)
{
    return Main::Guest_KLT_Root+"sources/cpu/"+this->Plat_Get()+"_rvm/";
}
/* End Function:KLT_Gen::Port_Dir_Get ****************************************/

/* Function:KLT_Gen::Process_Virt_Hdr *****************************************
Description : Create the configuration headers for VMs.
Input       : class Virtual* Virt - The process to generate for.
Output       : None.
Return       : None.
******************************************************************************/
void KLT_Gen::Process_Virt_Hdr(class Virtual* Virt)
{
    std::string Port_Dir;
    std::unique_ptr<std::vector<std::string>> List;

    /* Map the toolchain and the platform onto the port directory. */
    Port_Dir=Port_Dir_Get(Virt);

    /* See if the KLite tree and the port files are actually there. */
    if(std::filesystem::exists(Main::Guest_KLT_Root+"sources/kernel/kernel.h")==false)
        Main::Error("KLite generator:\nThe KLite tree '"+Main::Guest_KLT_Root+"' does not contain 'sources/kernel/kernel.h'.");
    if(std::filesystem::exists(Port_Dir+"cpu.c")==false)
        Main::Error("KLite generator:\nThe RVM port folder '"+Port_Dir+"' does not contain 'cpu.c'.");
    if(std::filesystem::exists(Port_Dir+"cpu_asm_"+this->Toolchain_Get(Virt)+".s")==false)
        Main::Error("KLite generator:\nThe RVM port folder '"+Port_Dir+"' does not contain 'cpu_asm_"+this->Toolchain_Get(Virt)+".s'.");

    /* Add the kernel headers, the optional components the default build
     * uses, and the port itself into the VM. */
    Virt->Virtual_Include.push_back(Main::Guest_KLT_Root+"sources/kernel/");
    Virt->Virtual_Include.push_back(Main::Guest_KLT_Root+"sources/opt/");
    Virt->Virtual_Include.push_back(Port_Dir);

    /* The test header only exists in benchmark builds - the plain
     * application boots through the port's cpu.c and needs no test
     * hardware, so the test tree stays out of its include path. */
    if(Main::Benchmark!=0)
    {
        /* Add the test include path into the VM */
        Virt->Virtual_Include.push_back(Main::Guest_KLT_Root+"test/");

        Main::Info("> Generating test selection header.");
        List=std::make_unique<std::vector<std::string>>();
        Proj_Gen::Src_Head(List, "klt_test.h", "The KLite test header.");
        List->push_back("");
        List->push_back("/* Include *******************************************************************/");
        List->push_back(std::string("#include \"chip/klite_test_")+this->Chip->Name_Lower+"_rvm.h\"");
        List->push_back("/* End Include ***************************************************************/");
        List->push_back("");
        Proj_Gen::Src_Foot(List);

        Proj_Gen::Line_Write(List, Virt->Virtual_Header_Output+"klt_test.h");
    }
}
/* End Function:KLT_Gen::Process_Virt_Hdr ************************************/

/* Function:KLT_Gen::Process_Virt_Src *****************************************
Description : Create the configuration source for VMs.
Input       : class Virtual* Virt - The process to generate for.
               class Tool_Gen* Tool - The toolchain to use.
Output       : None.
Return      : None.
******************************************************************************/
void KLT_Gen::Process_Virt_Src(class Virtual* Virt, class Tool_Gen* Tool)
{
    std::string Port_Dir;

    (void)Tool;

    /* Map the toolchain and the platform onto the port directory. */
    Port_Dir=Port_Dir_Get(Virt);

    /* Add all sources that this depends on: the RVM port (the fast-path
     * switch assembly and the C layer with the critical sections, the
     * deferred context handler and the boot hooks), the complete KLite
     * kernel core, and the optional components the default build uses -
     * the queue with the mpool it is built on.  KLite has no conditional
     * compilation, everything else the tree offers stays out. */
    Virt->Virtual_Library.push_back(Port_Dir+"cpu.c");
    Virt->Virtual_Library.push_back(Port_Dir+"cpu_asm_"+this->Toolchain_Get(Virt)+".s");
    Virt->Virtual_Library.push_back(Main::Guest_KLT_Root+"sources/kernel/kernel.c");
    Virt->Virtual_Library.push_back(Main::Guest_KLT_Root+"sources/kernel/sched.c");
    Virt->Virtual_Library.push_back(Main::Guest_KLT_Root+"sources/kernel/thread.c");
    Virt->Virtual_Library.push_back(Main::Guest_KLT_Root+"sources/kernel/heap.c");
    Virt->Virtual_Library.push_back(Main::Guest_KLT_Root+"sources/kernel/sem.c");
    Virt->Virtual_Library.push_back(Main::Guest_KLT_Root+"sources/kernel/event.c");
    Virt->Virtual_Library.push_back(Main::Guest_KLT_Root+"sources/kernel/mutex.c");
    Virt->Virtual_Library.push_back(Main::Guest_KLT_Root+"sources/kernel/cond.c");
    Virt->Virtual_Library.push_back(Main::Guest_KLT_Root+"sources/opt/queue.c");
    Virt->Virtual_Library.push_back(Main::Guest_KLT_Root+"sources/opt/mpool.c");

    /* The benchmark carries the two test timers and the chip test header;
     * the plain application boots through the port and never sees them. */
    if(Main::Benchmark!=0)
        Virt->Virtual_Source.push_back(Main::Guest_KLT_Root+"test/klite_benchmark.c");
    else
    {
        Virt->Virtual_Source.push_back(Virt->Virtual_Source_Output+"main.c");

        /* Copy the plain application to the folder, in case that it is not there before */
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
        std::filesystem::copy(Main::Guest_KLT_Root+"test/klite_main.c",
                              Virt->Virtual_Source_Output+"main.c",
                              std::filesystem::copy_options::overwrite_existing);
    }
}
/* End Function:KLT_Gen::Process_Virt_Src ************************************/
}
/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
