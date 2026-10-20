/******************************************************************************
Filename    : frt_gen.cpp
Author      : pry, ChatGLM
Date        : 18/10/2026
Licence     : LGPL v3+; see COPYING for details.
Description : This toolset is for FreeRTOS.
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
#include "Proj_Gen/Guest_Gen/FRT_Gen/frt_gen.hpp"
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
#include "Proj_Gen/Guest_Gen/FRT_Gen/frt_gen.hpp"
#undef __HDR_CLASS__
/* End Include ***************************************************************/
namespace RVM_GEN
{
/* Function:FRT_Gen::FRT_Gen **************************************************
Description : Generator for the FreeRTOS.
Input       : class Proj_Info* Proj - The project information.
              class Plat_Info* Plat - The platform information.
              class Chip_Info* Chip - The chip information.
Output      : None.
Return      : None.
******************************************************************************/
/* void */ FRT_Gen::FRT_Gen(class Proj_Info* Proj, class Plat_Info* Plat, class Chip_Info* Chip):
Guest_Gen("FRT", Proj, Plat, Chip)
{
    try
    {

    }
    catch(std::exception& Exc)
    {
        Main::Error(std::string("FRT generator:\n")+Exc.what());
    }
}
/* End Function:FRT_Gen::FRT_Gen *********************************************/

/* Function:FRT_Gen::Process_Virt_Hdr *****************************************
Description : Create the configuration headers for VMs.
Input       : class Virtual* Virt - The process to generate for.
Output      : None.
Return      : None.
******************************************************************************/
void FRT_Gen::Process_Virt_Hdr(class Virtual* Virt)
{
    std::string Port_Dir;
    std::unique_ptr<std::vector<std::string>> List;

    /* Map the toolchain name to the port directory name - the ARMCC ports are
     * named after Keil, and all open toolchains share the GCC directory. */
    if(Virt->Toolchain=="ARMCC")
        Port_Dir=Main::Guest_FRT_Root+"portable/Keil/"+this->Plat->Name_Upper+"_RVM/";
    else
        Port_Dir=Main::Guest_FRT_Root+"portable/GCC/"+this->Plat->Name_Upper+"_RVM/";

    /* See if the FreeRTOS root folder and the port folder are actually there */
    if(std::filesystem::exists(Main::Guest_FRT_Root+"tasks.c")==false)
        Main::Error("FRT generator:\nThe FreeRTOS root folder '"+Main::Guest_FRT_Root+"' does not contain 'tasks.c'.");
    if(std::filesystem::exists(Port_Dir+"port.c")==false)
        Main::Error("FRT generator:\nThe RVM port folder '"+Port_Dir+"' does not contain 'port.c'.");

    /* Add the FreeRTOS kernel headers and the port headers into the VM */
    Virt->Virtual_Include.push_back(Main::Guest_FRT_Root+"include/");
    Virt->Virtual_Include.push_back(Port_Dir);

    /* If we're running benchmark, need to generate the test header as well */
    if(Main::Benchmark!=0)
    {
        /* Add the test include path into the VM */
        Virt->Virtual_Include.push_back(Main::Guest_FRT_Root+"test/");

        Main::Info("> Generating test selection header.");
        List=std::make_unique<std::vector<std::string>>();
        Proj_Gen::Src_Head(List, "frt_test.h", "The FRT test header.");
        List->push_back("");
        List->push_back("/* Include *******************************************************************/");
        List->push_back(std::string("#include \"chip/frt_test_")+this->Chip->Name_Lower+"_rvm.h\"");
        List->push_back("/* End Include ***************************************************************/");
        List->push_back("");
        List->push_back("/* Define ********************************************************************/");
        List->push_back("/* The benchmark print macros - the FreeRTOS kernel itself has no printing");
        List->push_back(" * facility, so we map them onto the RVM debug printing functions. */");
        List->push_back("#define benchPRINT_STRING(pcString)            RVM_DBG_S(pcString)");
        List->push_back("#define benchPRINT_NUMBER(ulNumber)            RVM_DBG_I(ulNumber)");
        List->push_back("/* End Define ****************************************************************/");
        List->push_back("");
        Proj_Gen::Src_Foot(List);

        Proj_Gen::Line_Write(List, Virt->Virtual_Header_Output+"frt_test.h");
    }
}
/* End Function:FRT_Gen::Process_Virt_Hdr ************************************/

/* Function:FRT_Gen::Process_Virt_Src *****************************************
Description : Create the configuration source for VMs.
Input       : class Virtual* Virt - The process to generate for.
              class Tool_Gen* Tool - The toolchain to use.
Output      : None.
Return      : None.
******************************************************************************/
void FRT_Gen::Process_Virt_Src(class Virtual* Virt, class Tool_Gen* Tool)
{
    std::string Port_Dir;

    /* Map the toolchain name to the port directory name - the ARMCC ports are
     * named after Keil, and all open toolchains share the GCC directory. */
    if(Virt->Toolchain=="ARMCC")
        Port_Dir=Main::Guest_FRT_Root+"portable/Keil/"+this->Plat->Name_Upper+"_RVM/";
    else
        Port_Dir=Main::Guest_FRT_Root+"portable/GCC/"+this->Plat->Name_Upper+"_RVM/";

    /* Add all sources that this depend on - the port, the kernel and the heap.
     * The 'timers.c' is needed by ports with timers enabled; for all other
     * ports it is compiled empty due to its own internal guards. */
    Virt->Virtual_Library.push_back(Port_Dir+"port.c");
    if(this->Plat->Name=="RV32P")
        Virt->Virtual_Library.push_back(Port_Dir+"portasm.s");
    Virt->Virtual_Library.push_back(Main::Guest_FRT_Root+"tasks.c");
    Virt->Virtual_Library.push_back(Main::Guest_FRT_Root+"list.c");
    Virt->Virtual_Library.push_back(Main::Guest_FRT_Root+"queue.c");
    Virt->Virtual_Library.push_back(Main::Guest_FRT_Root+"timers.c");
    Virt->Virtual_Library.push_back(Main::Guest_FRT_Root+"portable/MemMang/heap_4.c");

    /* Add benchmark or basic user stubs */
    if(Main::Benchmark!=0)
        Virt->Virtual_Source.push_back(Main::Guest_FRT_Root+"test/frt_benchmark.c");
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
        /* This may pop up: gcc bug - ignoring copy_options, will fix in the future */
        std::filesystem::copy(Main::Guest_FRT_Root+"test/frt_main.c",
                              Virt->Virtual_Source_Output+"main.c",
                              std::filesystem::copy_options::overwrite_existing);
    }
}
/* End Function:FRT_Gen::Process_Virt_Src ************************************/
}
/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/

