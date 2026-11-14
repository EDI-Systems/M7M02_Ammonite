/*****************************************************************************
Filename    : chb_gen.cpp
Author      : Evo-Devo Instrum.
Date        : 2026
Licence     : LGPL v3+; see COPYING for details.
Description : The source of the ChibiOS guest OS generator.
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
#include "fstream"
#include "cstdlib"

#define __HDR_DEF__
#include "rvm_gen.hpp"
#include "Plat_Info/plat_info.hpp"
#include "Chip_Info/chip_info.hpp"
#include "Conf_Info/conf_info.hpp"

#include "Proj_Gen/proj_gen.hpp"
#include "Proj_Gen/Tool_Gen/tool_gen.hpp"
#include "Proj_Gen/Guest_Gen/guest_gen.hpp"
#include "Proj_Gen/Guest_Gen/CHB_Gen/chb_gen.hpp"
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
#include "Proj_Gen/Guest_Gen/CHB_Gen/chb_gen.hpp"
#undef __HDR_CLASS__
/* End Include ***************************************************************/
namespace RVM_GEN
{
/* Function:CHB_Gen::CHB_Gen **************************************************
Description : Constructor function.
Input       : class Proj_Info* Proj - The project information.
              class Plat_Info* Plat - The platform information.
              class Chip_Info* Chip - The chip information.
Output      : None.
Return      : None.
******************************************************************************/
/* void */ CHB_Gen::CHB_Gen(class Proj_Info* Proj, class Plat_Info* Plat, class Chip_Info* Chip):
Guest_Gen("CHB", Proj, Plat, Chip)
{
    try
    {

    }
    catch(std::exception& Exc)
    {
        Main::Error(std::string("ChibiOS generator:\n")+Exc.what());
    }
}
/* End Function:CHB_Gen::CHB_Gen *********************************************/

/* Function:CHB_Gen::Plat_Get *************************************************
Description : Map the platform name onto the ChibiOS RVM port directory.
Input       : None.
Output      : None.
Return      : std::string - The port architecture name.
******************************************************************************/
std::string CHB_Gen::Plat_Get(void)
{
    if(this->Plat->Name=="A7M")
        return "ARMv7-M-RVM";
    else if(this->Plat->Name=="A6M")
        return "ARMv6-M-RVM";
    else if(this->Plat->Name=="RV32P")
        return "RV32P-RVM";

    Main::Error("ChibiOS generator:\nNo RVM port available for platform '"+this->Plat->Name+"'.");
    return "";
}
/* End Function:CHB_Gen::Plat_Get ********************************************/

/* Function:CHB_Gen::Toolchain_Get ********************************************
Description : Map the toolchain name onto the ChibiOS port directory name.
Input       : class Virtual* Virt - The process to generate for.
Output      : None.
Return      : std::string - The toolchain directory name.
******************************************************************************/
std::string CHB_Gen::Toolchain_Get(class Virtual* Virt)
{
    if(Virt->Toolchain=="ARMCC")
        return "RVCT";
    else if(Virt->Toolchain=="ICCARM")
        return "IAR";
    else if(Virt->Toolchain=="GCC")
        return "GCC";

    Main::Error("ChibiOS generator:\nNo CHB port for toolchain '"+Virt->Toolchain+"'.");
    return "";
}
/* End Function:CHB_Gen::Toolchain_Get ***************************************/

/* Function:CHB_Gen::Port_Dir_Get *********************************************
Description : Map the toolchain and the platform onto the port directory of
              the ChibiOS tree.
Input       : class Virtual* Virt - The process to generate for.
Output      : None.
Return      : std::string - The port directory path.
******************************************************************************/
std::string CHB_Gen::Port_Dir_Get(class Virtual* Virt)
{
    (void)this->Toolchain_Get(Virt);
    return Main::Guest_CHB_Root+"os/common/ports/"+this->Plat_Get()+"/";
}
/* End Function:CHB_Gen::Port_Dir_Get ****************************************/

/* Function:CHB_Gen::Process_Virt_Hdr *****************************************
Description : Create the configuration headers for VMs.
Input       : class Virtual* Virt - The process to generate for.
Output      : None.
Return      : None.
******************************************************************************/
void CHB_Gen::Process_Virt_Hdr(class Virtual* Virt)
{
    std::string Port_Dir;
    std::string Portability;
    std::unique_ptr<std::vector<std::string>> List;

    Port_Dir=Port_Dir_Get(Virt);
    if(Virt->Toolchain=="ICCARM")
        Portability="IAR";
    else if(Virt->Toolchain=="ARMCC")
        Portability="ARMC6";
    else
        Portability="GCC";

    if(std::filesystem::exists(Main::Guest_CHB_Root+"os/rt/include/ch.h")==false)
        Main::Error("ChibiOS generator:\nThe ChibiOS tree '"+Main::Guest_CHB_Root+"' does not contain 'os/rt/include/ch.h'.");
    if(std::filesystem::exists(Port_Dir+"chcore.c")==false)
        Main::Error("ChibiOS generator:\nThe RVM port folder '"+Port_Dir+"' does not contain 'chcore.c'.");
    if(std::filesystem::exists(Port_Dir+"templates/chconf.h")==false)
        Main::Error("ChibiOS generator:\nThe RVM port folder '"+Port_Dir+"' does not contain 'templates/chconf.h'.");

    Virt->Virtual_Include.push_back(Main::Guest_CHB_Root+"os/license/");
    Virt->Virtual_Include.push_back(Main::Guest_CHB_Root+"os/rt/include/");
    Virt->Virtual_Include.push_back(Main::Guest_CHB_Root+"os/oslib/include/");
    Virt->Virtual_Include.push_back(Main::Guest_CHB_Root+"os/common/portability/"+Portability+"/");
    if((this->Plat->Name=="A7M")||(this->Plat->Name=="A6M"))
        Virt->Virtual_Include.push_back(Main::Guest_CHB_Root+"os/common/ports/ARM-common/");
    Virt->Virtual_Include.push_back(Port_Dir);

    /* The configuration belongs to each VM.  Never replace a user-edited
     * copy unless the generator is explicitly forced. */
    if((std::filesystem::exists(Virt->Virtual_Header_Output+"chconf.h")==false)||
       (Virt->Virtual_Header_Overwrite!=0))
    {
        Main::Info("> Generating 'chconf.h'.");
        std::filesystem::copy(Port_Dir+"templates/chconf.h",
                              Virt->Virtual_Header_Output+"chconf.h",
                              std::filesystem::copy_options::overwrite_existing);
    }
    else
        Main::Info("> File 'chconf.h' exists, skipping generation.");

    /* The tick frequency follows the VM configuration, so it is generated
     * on every run and is not meant to be user-edited. */
    Main::Info("> Generating 'chb_config.h'.");
    List=std::make_unique<std::vector<std::string>>();
    Proj_Gen::Src_Head(List, "chb_config.h", "The CHB generated configuration.");
    List->push_back("");
    List->push_back("/* Define ********************************************************************/");
    List->push_back("#ifndef CHB_CONFIG_H");
    List->push_back("#define CHB_CONFIG_H");
    List->push_back("");
    List->push_back("#define CHB_ST_FREQUENCY                    100U");
    {
        const char* Fast = std::getenv("CHB_FAST_PATH");
        bool Fast_On = (Fast == NULL) || (std::string(Fast) != "0");
        List->push_back(std::string("#define CHB_FAST_PATH                       ")+
                        (Fast_On ? "1U" : "0U"));
    }
    List->push_back(std::string("#define CHB_USE_FPU                         ")+
                    ((Virt->Coprocessor_Set.find("None")==Virt->Coprocessor_Set.end() &&
                      Virt->Coprocessor_Set.empty()==false) ? "1U" : "0U"));
    List->push_back(std::string("#define CHB_CPU_M4                          ")+
                    ((this->Chip->Name.find("STM32F4")!=std::string::npos) ? "1U" : "0U"));
    List->push_back("");
    List->push_back("#endif /* CHB_CONFIG_H */");
    List->push_back("/* End Define ****************************************************************/");
    List->push_back("");
    Proj_Gen::Src_Foot(List);
    Proj_Gen::Line_Write(List, Virt->Virtual_Header_Output+"chb_config.h");

    if(Main::Benchmark!=0)
    {
        Virt->Virtual_Include.push_back(Main::Guest_CHB_Root+"test/");

        Main::Info("> Generating test selection header.");
        List=std::make_unique<std::vector<std::string>>();
        Proj_Gen::Src_Head(List, "chb_test.h", "The CHB test header.");
        List->push_back("");
        List->push_back("/* Include *******************************************************************/");
        List->push_back(std::string("#include \"chip/chb_test_")+this->Chip->Name_Lower+"_rvm.h\"");
        List->push_back("/* End Include ***************************************************************/");
        List->push_back("");
        Proj_Gen::Src_Foot(List);
        Proj_Gen::Line_Write(List, Virt->Virtual_Header_Output+"chb_test.h");
    }
}
/* End Function:CHB_Gen::Process_Virt_Hdr ************************************/

/* Function:CHB_Gen::Process_Virt_Src *****************************************
Description : Create the configuration source for VMs.
Input       : class Virtual* Virt - The process to generate for.
              class Tool_Gen* Tool - The toolchain to use.
Output      : None.
Return      : None.
******************************************************************************/
void CHB_Gen::Process_Virt_Src(class Virtual* Virt, class Tool_Gen* Tool)
{
    std::string Port_Dir;
    std::string Compiler;

    (void)Tool;

    Port_Dir=Port_Dir_Get(Virt);
    Compiler=this->Toolchain_Get(Virt);

    /* Port layer. */
    if(std::filesystem::exists(Port_Dir+"compilers/"+Compiler+"/chcoreasm.s")==false)
        Main::Error("ChibiOS generator:\nThe RVM port folder '"+Port_Dir+"' does not contain 'compilers/"+Compiler+"/chcoreasm.s'.");
    Virt->Virtual_Library.push_back(Port_Dir+"chcore.c");
    Virt->Virtual_Library.push_back(Port_Dir+"compilers/"+Compiler+"/chcoreasm.s");

    /* ChibiOS/RT core.  The list mirrors os/rt/rt.mk with the options
     * selected in templates/chconf.h. */
    Virt->Virtual_Library.push_back(Main::Guest_CHB_Root+"os/rt/src/chsys.c");
    Virt->Virtual_Library.push_back(Main::Guest_CHB_Root+"os/rt/src/chrfcu.c");
    Virt->Virtual_Library.push_back(Main::Guest_CHB_Root+"os/rt/src/chdebug.c");
    Virt->Virtual_Library.push_back(Main::Guest_CHB_Root+"os/rt/src/chtrace.c");
    Virt->Virtual_Library.push_back(Main::Guest_CHB_Root+"os/rt/src/chvt.c");
    Virt->Virtual_Library.push_back(Main::Guest_CHB_Root+"os/rt/src/chschd.c");
    Virt->Virtual_Library.push_back(Main::Guest_CHB_Root+"os/rt/src/chinstances.c");
    Virt->Virtual_Library.push_back(Main::Guest_CHB_Root+"os/rt/src/chthreads.c");
    Virt->Virtual_Library.push_back(Main::Guest_CHB_Root+"os/rt/src/chsem.c");
    Virt->Virtual_Library.push_back(Main::Guest_CHB_Root+"os/rt/src/chevents.c");
    Virt->Virtual_Library.push_back(Main::Guest_CHB_Root+"os/rt/src/chmsg.c");
    Virt->Virtual_Library.push_back(Main::Guest_CHB_Root+"os/oslib/src/chmboxes.c");
    Virt->Virtual_Library.push_back(Main::Guest_CHB_Root+"os/oslib/src/chmemcore.c");
    Virt->Virtual_Library.push_back(Main::Guest_CHB_Root+"os/oslib/src/chmemheaps.c");

    if(this->Plat->Name=="A7M" && Virt->Toolchain=="GCC" &&
       Virt->Coprocessor_Set.find("None")==Virt->Coprocessor_Set.end() &&
       Virt->Coprocessor_Set.empty()==false)
    {
        std::string User_File=Virt->Project_Output+"user";
        if(std::filesystem::exists(User_File)==false)
        {
            std::filesystem::create_directories(Virt->Project_Output);
            std::ofstream User_Stream(User_File);
            User_Stream << "CPU+=-mfloat-abi=hard" << std::endl;
            User_Stream.close();
        }
    }

    if(Main::Benchmark!=0)
        Virt->Virtual_Source.push_back(Main::Guest_CHB_Root+"test/chb_benchmark.c");
    else
    {
        Virt->Virtual_Source.push_back(Virt->Virtual_Source_Output+"main.c");

        if(std::filesystem::exists(Virt->Virtual_Source_Output+"main.c")==true)
        {
            if(Virt->Virtual_Source_Overwrite==0)
            {
                Main::Info("> File 'main.c' exists, skipping generation.");
                return;
            }
        }

        Main::Info("> Generating 'main.c'.");
        std::filesystem::copy(Main::Guest_CHB_Root+"test/chb_main.c",
                              Virt->Virtual_Source_Output+"main.c",
                              std::filesystem::copy_options::overwrite_existing);
    }
}
/* End Function:CHB_Gen::Process_Virt_Src ************************************/
}
/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
