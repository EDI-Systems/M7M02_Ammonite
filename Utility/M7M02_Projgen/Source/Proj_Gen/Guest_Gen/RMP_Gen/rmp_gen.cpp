/******************************************************************************
Filename    : rmp_gen.cpp
Author      : pry
Date        : 12/07/2019
Licence     : LGPL v3+; see COPYING for details.
Description : This toolset is for RMP OS.
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
#include "Proj_Gen/Guest_Gen/RMP_Gen/rmp_gen.hpp"
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
#include "Proj_Gen/Guest_Gen/RMP_Gen/rmp_gen.hpp"
#undef __HDR_CLASS__
/* End Include ***************************************************************/
namespace RVM_GEN
{
/* Function:RMP_Gen::RMP_Gen **************************************************
Description : Generator for the RMP OS.
Input       : class Proj_Info* Proj - The project information.
              class Plat_Info* Plat - The platform information.
              class Chip_Info* Chip - The chip information.
Output      : None.
Return      : None.
******************************************************************************/
/* void */ RMP_Gen::RMP_Gen(class Proj_Info* Proj, class Plat_Info* Plat, class Chip_Info* Chip):
Guest_Gen("RMP", Proj, Plat, Chip)
{
    try
    {

    }
    catch(std::exception& Exc)
    {
        Main::Error(std::string("RMP generator:\n")+Exc.what());
    }
}
/* End Function:RMP_Gen::RMP_Gen *********************************************/

/* Function:RMP_Gen::Process_Virt_Hdr *****************************************
Description : Create the configuration headers for VMs.
Input       : class Virtual* Virt - The process to generate for.
Output      : None.
Return      : None.
******************************************************************************/
void RMP_Gen::Process_Virt_Hdr(class Virtual* Virt)
{
    std::string Filename;
    std::unique_ptr<std::vector<std::string>> List;

    List=std::make_unique<std::vector<std::string>>();

    /* Add whatever in the supposed include path into the VM */
    Virt->Virtual_Include.push_back(Main::Guest_RMP_Root+"Include/");

    /* Create platform selection header files */
    Main::Info("> Generating 'rmp_platform.h'.");
    Proj_Gen::Src_Head(List, "rmp_platform.h", "The platform selection header.");
    List->push_back("");
    List->push_back("/* Platform Include **********************************************************/");
    List->push_back(std::string("#include \"Platform/")+this->Plat->Name_Upper+"_RVM/rmp_platform_"+this->Plat->Name_Lower+"_rvm.h\"");
    List->push_back("/* End Platform Include ******************************************************/");
    List->push_back("");
    Proj_Gen::Src_Foot(List);
    List->push_back("");
    /* Generate rmp_platform.h */
    Proj_Gen::Line_Write(List, Virt->Virtual_Header_Output+"rmp_platform.h");

    /* Create chip selection header files */
    List->clear();
    Filename=std::string("rmp_platform_")+this->Plat->Name_Lower+"_rvm_conf.h";
    Main::Info(std::string("> Generating '")+Filename+"'.");
    Proj_Gen::Src_Head(List, Filename, "The chip selection header.");
    List->push_back("");
    List->push_back("/* Platform Include **********************************************************/");
    List->push_back(std::string("#include \"rmp_platform_")+this->Chip->Name_Lower+"_rvm.h\"");
    List->push_back("/* End Platform Include ******************************************************/");
    List->push_back("");
    Proj_Gen::Src_Foot(List);
    /* Generate rmp_platform_xxx_conf.h */
    Proj_Gen::Line_Write(List, Virt->Virtual_Header_Output+Filename);

    /* Copy the actual configuration file to the folder - if this ever existed, do not replace */
    Filename=std::string("rmp_platform_"+this->Chip->Name_Lower+"_rvm.h");
    if(std::filesystem::exists(Virt->Virtual_Header_Output+Filename)==true)
    {
        /* See if we'll use forced regenerate */
        if(Virt->Virtual_Header_Overwrite==0)
        {
            Main::Info(std::string("> File '")+Filename+"' exists, skipping generation.");
            return;
        }
    }

    Main::Info(std::string("> Generating '")+Filename+"'.");
    /* This may pop up: gcc bug - ignoring copy_options, will fix in the future */
    std::filesystem::copy(Main::Guest_RMP_Root+"Include/Platform/"+this->Plat->Name_Upper+"_RVM/"+
                          "rmp_platform_"+this->Plat->Name_Lower+"_chip_rvm.h",
                          Virt->Virtual_Header_Output+Filename,
                          std::filesystem::copy_options::overwrite_existing);

    /* If we're running benchmark, need to generate the test header as well */
    if(Main::Benchmark!=0)
    {
        Main::Info("> Generating test selection header.");
        List->clear();
        Proj_Gen::Src_Head(List, "rmp_test.h", "The RMP test header.");
        List->push_back("");
        List->push_back("/* Include *******************************************************************/");
        List->push_back(std::string("#include \"Test/Chip/rmp_test_")+this->Chip->Name_Lower+"_rvm.h\"");
        List->push_back("/* End Include ***************************************************************/");
        List->push_back("");
        Proj_Gen::Src_Foot(List);

        Proj_Gen::Line_Write(List, Virt->Virtual_Header_Output+"rmp_test.h");
    }
}
/* End Function:RMP_Gen::Process_Virt_Hdr ************************************/

/* Function:RMP_Gen::Process_Virt_Src *****************************************
Description : Create the configuration source for VMs.
Input       : class Virtual* Virt - The process to generate for.
              class Tool_Gen* Tool - The toolchain to use.
Output      : None.
Return      : None.
******************************************************************************/
void RMP_Gen::Process_Virt_Src(class Virtual* Virt, class Tool_Gen* Tool)
{
    std::string Platform;

    /* Add all sources that this depend on */
    Platform=Main::Guest_RMP_Root+"Source/Platform/"+this->Plat->Name_Upper+"_RVM/";
    Virt->Virtual_Library.push_back(Main::Guest_RMP_Root+"Source/Kernel/rmp_kernel.c");
    Virt->Virtual_Library.push_back(Platform+"rmp_platform_"+this->Plat->Name_Lower+"_rvm.c");
    Virt->Virtual_Library.push_back(Platform+"rmp_platform_"+this->Plat->Name_Lower+"_rvm_"+
                                    Tool->Name_Lower+Tool->Suffix(TOOL_ASSEMBLER));

    /* Add benchmark or basic hook stubs */
    if(Main::Benchmark!=0)
        Virt->Virtual_Source.push_back(Main::Guest_RMP_Root+"Source/Test/rmp_benchmark.c");
    else
    {
        Virt->Virtual_Source.push_back(Virt->Virtual_Source_Output+"rmp_hook.c");

        /* Copy the blank file to the folder, in case that it is not there before */
        if(std::filesystem::exists(Virt->Virtual_Source_Output+"rmp_hook.c")==true)
        {
            /* See if we'll use forced regenerate */
            if(Virt->Virtual_Source_Overwrite==0)
            {
                Main::Info("> File 'rmp_hook.c' exists, skipping generation.");
                return;
            }
        }

        Main::Info("> Generating 'rmp_hook.c'.");
        /* This may pop up: gcc bug - ignoring copy_options, will fix in the future */
        std::filesystem::copy(Main::Guest_RMP_Root+"Source/Hook/rmp_hook_rvm.c",
                              Virt->Virtual_Source_Output+"rmp_hook.c",
                              std::filesystem::copy_options::overwrite_existing);
    }
}
/* End Function:RMP_Gen::Process_Virt_Src ************************************/
}
/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
