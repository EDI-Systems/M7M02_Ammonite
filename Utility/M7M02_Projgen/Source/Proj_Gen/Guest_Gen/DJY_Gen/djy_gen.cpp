/*****************************************************************************
Filename    : DJY_Gen.cpp
Author      : pry, Cline
Date        : 28/10/2026
Licence     : LGPL v3+; see COPYING for details.
Description : The DjyOS guest OS generator.
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
#include "Proj_Gen/Guest_Gen/DJY_Gen/djy_gen.hpp"
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
#include "Proj_Gen/Build_Gen/build_gen.hpp"
#include "Proj_Gen/Tool_Gen/tool_gen.hpp"
#include "Proj_Gen/Guest_Gen/guest_gen.hpp"
#include "Proj_Gen/Guest_Gen/DJY_Gen/djy_gen.hpp"
#undef __HDR_CLASS__

namespace RVM_GEN
{

/* Function:DJY_Gen::DJY_Gen ***************************************************
Description : Constructor function.
Input       : class Proj_Info* Proj - The project information.
               class Plat_Info* Plat - The platform information.
               class Chip_Info* Chip - The chip information.
Output      : None.
Return      : None.
******************************************************************************/
DJY_Gen::DJY_Gen(class Proj_Info* Proj, class Plat_Info* Plat, class Chip_Info* Chip) :
           Guest_Gen("DJY",Proj,Plat,Chip)
{

}
/* End Function:DJY_Gen::DJY_Gen *********************************************/

/* Function:DJY_Gen::Plat_Get **************************************************
Description : Map the platform name onto the DjyOS bsp architecture directory
              name.  The port layer is architecture level and integer-only,
              so the v7 and v7e levels are deliberately not distinguished -
              both Cortex-M4 and Cortex-M7 take the armv7e-m_rvm variant of
              their shared armv7e-m node, exactly like the official tree
              shares the switch code between the two levels.
Input       : None.
Output      : None.
Return      : std::string - The port directory name.
******************************************************************************/
std::string DJY_Gen::Plat_Get(void)
{
    if(this->Plat->Name=="A7M")
        return "armv7e-m";
    else if(this->Plat->Name=="A6M")
        return "armv6-m";

    Main::Error("DjyOS generator:\nNo RVM port available for platform '"+this->Plat->Name+"'.");
    return "";
}
/* End Function:DJY_Gen::Plat_Get ********************************************/

/* Function:DJY_Gen::Toolchain_Get *********************************************
Description : Map the toolchain name onto the DjyOS compiler file suffix
              (gcc = GNU as, iar = iasmarm, rvds = armasm - the official
              bsp naming adapted to our lowercase port file convention).
Input       : class Virtual* Virt - The process to generate for.
Output      : None.
Return      : std::string - The compiler file suffix.
******************************************************************************/
std::string DJY_Gen::Toolchain_Get(class Virtual* Virt)
{
    if(Virt->Toolchain=="ARMCC")
        return "rvds";
    else if(Virt->Toolchain=="ICCARM")
        return "iar";

    return "gcc";
}
/* End Function:DJY_Gen::Toolchain_Get ***************************************/

/* Function:DJY_Gen::Port_Dir_Get **********************************************
Description : Map the toolchain and the platform onto the port directory of
              the DjyOS tree.  The port directories sit next to the official
              bsp architecture directories, as the RVM variant of the very
              node the official tree uses for the Cortex-M4/M7 switch code.
Input       : class Virtual* Virt - The process to generate for.
Output      : None.
Return      : std::string - The port directory.
******************************************************************************/
std::string DJY_Gen::Port_Dir_Get(class Virtual* Virt)
{
    (void)Virt;

    return Main::Guest_DJY_Root+"bsp/arch/arm/cortex-m/"+this->Plat_Get()+"_rvm/";
}
/* End Function:DJY_Gen::Port_Dir_Get ****************************************/

/* Function:DJY_Gen::Process_Virt_Hdr ******************************************
Description : Create the configuration headers for VMs.
Input       : class Virtual* Virt - The process to generate for.
Output      : None.
Return      : None.
******************************************************************************/
void DJY_Gen::Process_Virt_Hdr(class Virtual* Virt)
{
    std::string Port_Dir;
    std::unique_ptr<std::vector<std::string>> List;

    /* Map the toolchain and the platform onto the port directory. */
    Port_Dir=Port_Dir_Get(Virt);

    /* See if the DjyOS tree and the port files are actually there.  The
     * context switch assembly is a hard requirement - the switch is its
     * whole point; only the GCC variant exists for now, and the check on
     * the lowercase name doubles as the port capability probe for the
     * toolchains whose official capitalization is still in place. */
    if(std::filesystem::exists(Main::Guest_DJY_Root+"djyos/include/djyos.h")==false)
        Main::Error("DjyOS generator:\nThe DjyOS tree '"+Main::Guest_DJY_Root+"' does not contain 'djyos/include/djyos.h'.");
    if(std::filesystem::exists(Port_Dir+"cpus.s")==false)
        Main::Error("DjyOS generator:\nThe RVM port folder '"+Port_Dir+"' does not contain 'cpus.s'.");
    if(std::filesystem::exists(Port_Dir+"ints.s")==false)
        Main::Error("DjyOS generator:\nThe RVM port folder '"+Port_Dir+"' does not contain 'ints.s'.");
    if(std::filesystem::exists(Port_Dir+"template/project_config.h")==false)
        Main::Error("DjyOS generator:\nThe RVM port folder '"+Port_Dir+"' does not contain 'template/project_config.h'.");

    /* Add the kernel headers, the port headers, the port header shims, the
     * port configuration templates and the machine-visible library
     * headers into the VM.  Both port header folders stay on the include
     * path - 'include/' carries the shims that shadow the upstream and
     * libc headers, 'template/' carries the configuration layer.  They
     * are the authoritative header set: the makefile generator sorts the
     * include list alphabetically, and any workspace path
     * ('../Include/...') sorts after the DjyOS kernel folders, so a copy
     * could never shadow an upstream header - only the bsp-level port
     * paths can. */
    Virt->Virtual_Include.push_back(Main::Guest_DJY_Root+"djyos/");
    Virt->Virtual_Include.push_back(Main::Guest_DJY_Root+"djyos/include/");
    Virt->Virtual_Include.push_back(Main::Guest_DJY_Root+"djyos/core/");
    Virt->Virtual_Include.push_back(Main::Guest_DJY_Root+"djyos/heap/");
    Virt->Virtual_Include.push_back(Main::Guest_DJY_Root+"djyos/time/");
    Virt->Virtual_Include.push_back(Main::Guest_DJY_Root+"bsp/arch/arm/include/");
    Virt->Virtual_Include.push_back(Main::Guest_DJY_Root+"bsp/arch/arm/cortex-m/include/");
    Virt->Virtual_Include.push_back(Main::Guest_DJY_Root+"bsp/arch/arm/cortex-m/armv7e-m/include/");
    Virt->Virtual_Include.push_back(Main::Guest_DJY_Root+"bsp/arch/arm/cortex-m/src/");
    Virt->Virtual_Include.push_back(Port_Dir);
    Virt->Virtual_Include.push_back(Port_Dir+"include/");
    Virt->Virtual_Include.push_back(Port_Dir+"template/");

    /* The test header only exists in benchmark builds - the plain
     * application boots through the port's board.c, which needs no test
     * hardware, so the test tree stays out of its include path. */
    if(Main::Benchmark!=0)
    {
        /* Add the test include path into the VM */
        Virt->Virtual_Include.push_back(Main::Guest_DJY_Root+"test/");

        Main::Info("> Generating test selection header.");
        List=std::make_unique<std::vector<std::string>>();
        Proj_Gen::Src_Head(List, "djy_test.h", "The DjyOS test header.");
        List->push_back("");
        List->push_back("/* Include *******************************************************************/");
        List->push_back(std::string("#include \"chip/djy_test_")+this->Chip->Name_Lower+"_rvm.h\"");
        List->push_back("/* End Include ***************************************************************/");
        List->push_back("");
        Proj_Gen::Src_Foot(List);

        Proj_Gen::Line_Write(List, Virt->Virtual_Header_Output+"djy_test.h");
    }
}
/* End Function:DJY_Gen::Process_Virt_Hdr ************************************/

/* Function:DJY_Gen::Process_Virt_Src ******************************************
Description : Create the configuration source for VMs.
Input       : class Virtual* Virt - The process to generate for.
               class Tool_Gen* Tool - The toolchain to use.
Output      : None.
Return      : None.
******************************************************************************/
void DJY_Gen::Process_Virt_Src(class Virtual* Virt, class Tool_Gen* Tool)
{
    std::string Port_Dir;

    (void)Tool;

    /* Map the toolchain and the platform onto the port directory. */
    Port_Dir=Port_Dir_Get(Virt);

    /* Add all sources that this depends on: the RVM port (the switch
     * assembly, the atomics, the interrupt glue, the CPU services and the
     * libc support shim), the complete DjyOS kernel core subset that the
     * default configuration uses - the disabled kernel modules compile
     * empty - and the board support that hooks the kernel up to the
     * hypervisor. */
    Virt->Virtual_Library.push_back(Port_Dir+"cpus.s");
    Virt->Virtual_Library.push_back(Port_Dir+"ints.s");
    Virt->Virtual_Library.push_back(Port_Dir+"int_hard.c");
    Virt->Virtual_Library.push_back(Port_Dir+"cpu.c");
    Virt->Virtual_Library.push_back(Port_Dir+"djy_support.c");
    Virt->Virtual_Library.push_back(Port_Dir+"board.c");
    Virt->Virtual_Library.push_back(Main::Guest_DJY_Root+"djyos/core/djyos.c");
    Virt->Virtual_Library.push_back(Main::Guest_DJY_Root+"djyos/core/list.c");
    Virt->Virtual_Library.push_back(Main::Guest_DJY_Root+"djyos/core/sys_init.c");
    Virt->Virtual_Library.push_back(Main::Guest_DJY_Root+"djyos/core/cpuinfo.c");
    Virt->Virtual_Library.push_back(Main::Guest_DJY_Root+"djyos/core/lowpower.c");
    Virt->Virtual_Library.push_back(Main::Guest_DJY_Root+"djyos/int/int.c");
    Virt->Virtual_Library.push_back(Main::Guest_DJY_Root+"djyos/int/atomic.c");
    Virt->Virtual_Library.push_back(Main::Guest_DJY_Root+"djyos/lock/lock.c");
    Virt->Virtual_Library.push_back(Main::Guest_DJY_Root+"djyos/msgqueue/msgqueue.c");
    Virt->Virtual_Library.push_back(Main::Guest_DJY_Root+"djyos/heap/heap-static.c");
    Virt->Virtual_Library.push_back(Main::Guest_DJY_Root+"djyos/object/object.c");
    Virt->Virtual_Library.push_back(Main::Guest_DJY_Root+"djyos/object/objhandle.c");
    Virt->Virtual_Library.push_back(Main::Guest_DJY_Root+"djyos/pool/pool.c");
    Virt->Virtual_Library.push_back(Main::Guest_DJY_Root+"djyos/time/time.c");
    Virt->Virtual_Library.push_back(Main::Guest_DJY_Root+"djyos/time/systime.c");
    Virt->Virtual_Library.push_back(Main::Guest_DJY_Root+"djyos/tickless/tickless.c");

    /* The benchmark's board file carries the two test timers and the
     * boot sequence that registers the benchmark event types; the plain
     * application boots through the port's board.c and never sees it
     * (nor the chip test header it includes). */
    if(Main::Benchmark!=0)
    {
        Virt->Virtual_Source.push_back(Main::Guest_DJY_Root+"test/djy_board.c");
        Virt->Virtual_Source.push_back(Main::Guest_DJY_Root+"test/djy_benchmark.c");
    }
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
        std::filesystem::copy(Main::Guest_DJY_Root+"test/djy_main.c",
                              Virt->Virtual_Source_Output+"main.c",
                              std::filesystem::copy_options::overwrite_existing);
    }
}
/* End Function:DJY_Gen::Process_Virt_Src ************************************/
}
/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
