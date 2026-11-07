/******************************************************************************
Filename    : rte_gen.cpp
Author      : Evo-Devo Instrum.
Date        : 06/11/2026
Licence     : LGPL v3+; see COPYING for details.
Description : This toolset is for RTE, the RTEMS guest on the RVM.
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
#include "cstdlib"

#define __HDR_DEF__
#include "rvm_gen.hpp"
#include "Proj_Gen/proj_gen.hpp"
#include "Proj_Gen/Tool_Gen/tool_gen.hpp"
#include "Proj_Gen/Guest_Gen/guest_gen.hpp"
#include "Proj_Gen/Guest_Gen/RTE_Gen/rte_gen.hpp"
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
#include "Proj_Gen/Guest_Gen/RTE_Gen/rte_gen.hpp"
#undef __HDR_CLASS__
/* End Include ***************************************************************/
namespace RVM_GEN
{
/* Function:RTE_Gen::RTE_Gen **************************************************
Description : Generator for the RTEMS-on-RVM guest.
Input       : class Proj_Info* Proj - The project information.
              class Plat_Info* Plat - The platform information.
              class Chip_Info* Chip - The chip information.
Output      : None.
Return      : None.
******************************************************************************/
/* void */ RTE_Gen::RTE_Gen(class Proj_Info* Proj, class Plat_Info* Plat, class Chip_Info* Chip):
Guest_Gen("RTE", Proj, Plat, Chip)
{
    try
    {

    }
    catch(std::exception& Exc)
    {
        Main::Error(std::string("RTE generator:\n")+Exc.what());
    }
}
/* End Function:RTE_Gen::RTE_Gen *********************************************/

/* Function:RTE_Gen::Root_Get *************************************************
Description : Return the RTEMS tree root.
Input       : None.
Output      : None.
Return      : std::string - The RTEMS tree root.
******************************************************************************/
std::string RTE_Gen::Root_Get(void)
{
    if(Main::Guest_RTE_Root=="")
        Main::Error("RTE generator:\nThe RTEMS tree root is not specified.");
    return Main::Guest_RTE_Root;
}
/* End Function:RTE_Gen::Root_Get ********************************************/

/* Function:RTE_Gen::Prefix_Get ***********************************************
Description : Return the RTEMS toolchain prefix.
Input       : None.
Output      : None.
Return      : std::string - The RTEMS toolchain prefix.
******************************************************************************/
std::string RTE_Gen::Prefix_Get(void)
{
    if(Main::Guest_RTE_Prefix=="")
        Main::Error("RTE generator:\nThe RTEMS toolchain prefix is not specified.");
    return Main::Guest_RTE_Prefix;
}
/* End Function:RTE_Gen::Prefix_Get ******************************************/

/* Function:RTE_Gen::App_Get **************************************************
Description : Return the in-tree RTE application name.
Input       : None.
Output      : None.
Return      : std::string - The RTEMS sample application name.
******************************************************************************/
std::string RTE_Gen::App_Get(void)
{
    if(Main::Guest_RTE_App=="")
        Main::Error("RTE generator:\nThe RTE application name is not specified.");
    return Main::Guest_RTE_App;
}
/* End Function:RTE_Gen::App_Get *********************************************/

/* Function:RTE_Gen::Process_Virt_Hdr *****************************************
Description : Generate the RTE waf config and the guest config header.
Input       : class Virtual* Virt - The process to generate for.
Output      : None.
Return      : None.
******************************************************************************/
void RTE_Gen::Process_Virt_Hdr(class Virtual* Virt)
{
    const class Mem_Info* Code;
    const class Mem_Info* Data;
    ptr_t Code_Base;
    ptr_t Code_Size;
    ptr_t Data_Base;
    ptr_t Data_Size;
    ptr_t Exact;
    ptr_t Rte_Size;
    ptr_t User_Base;
    ptr_t Vector_Base;
    ptr_t Data_End;
    std::string App;
    std::string App_Source_List;
    bool Bench_Vm;
    bool User_App;
    bool App_Enabled;
    bool Fast_Path;
    std::string Cpu_Flags;
    std::vector<std::string> App_Files;
    std::filesystem::path App_Dir;
    std::unique_ptr<std::vector<std::string>> List;
    std::unique_ptr<std::vector<std::string>> Hdr;

    Code=nullptr;
    Data=nullptr;
    if(Virt->Memory_Code.empty()==false)
        Code=Virt->Memory_Code[0];
    if(Virt->Memory_Data.empty()==false)
        Data=Virt->Memory_Data[0];
    if((Code==nullptr)||(Data==nullptr))
        Main::Error("RTE generator:\nThe Virt Code1/Data1 blocks are missing.");

    Code_Base=Code->Base;
    Code_Size=Code->Size;
    Data_Base=Data->Base;
    Data_Size=Data->Size;
    Exact=Data_Size-Virt->Reg_Size-Virt->State_Size-Virt->User_Stack_Size-Virt->Vector_Stack_Size;
    Rte_Size=Exact-0x400U;
    User_Base=Data_Base+Exact+Virt->Reg_Size+Virt->State_Size;
    Vector_Base=Data_Base+Data_Size-Virt->Vector_Stack_Size;
    Data_End=Data_Base+Data_Size;
    App=App_Get();
    Fast_Path=true;
    {
        const char* Fast_Path_Env=std::getenv("RTE_FAST_PATH");
        if((Fast_Path_Env!=nullptr)&&(std::string(Fast_Path_Env)=="0"))
            Fast_Path=false;
    }
    Cpu_Flags="-mcpu=cortex-m7 -mfpu=fpv5-d16";
    for(const std::string& Cop:Virt->Coprocessor)
    {
        if(Cop=="FPV4_SP")
        {
            Cpu_Flags="-mcpu=cortex-m4 -mfpu=fpv4-sp-d16";
            break;
        }
        if(Cop=="FPV5_SP")
        {
            Cpu_Flags="-mcpu=cortex-m7 -mfpu=fpv5-sp-d16";
            break;
        }
        if(Cop=="FPV5_DP")
        {
            Cpu_Flags="-mcpu=cortex-m7 -mfpu=fpv5-d16";
            break;
        }
    }
    App_Dir=std::filesystem::path(Virt->Main_Source_Output+"app");

    /* Preserve the application folder across regeneration.  Every .c file
     * placed there is a user application source and takes precedence over the
     * benchmark special case; the benchmark is used only when the folder has
     * no user sources. */
    std::filesystem::create_directories(App_Dir);
    bool Placeholder_File=false;
    for(const std::filesystem::directory_entry& Entry:
        std::filesystem::directory_iterator(App_Dir))
    {
        if((Entry.is_regular_file()==false)||
           (Entry.path().extension()!=".c"))
            continue;
        if(Entry.path().filename()=="rte_app.c")
        {
            bool Placeholder=false;
            auto Lines=Proj_Gen::Line_Read(Entry.path().string());
            for(const std::string& Line:*Lines)
            {
                if(Line.find("No RTE application selected")!=std::string::npos)
                {
                    Placeholder=true;
                    break;
                }
            }
            if(Placeholder)
            {
                Placeholder_File=true;
                continue;
            }
        }
        App_Files.push_back(Entry.path().filename().string());
    }
    std::sort(App_Files.begin(),App_Files.end());
    User_App=(App_Files.empty()==false);
    Bench_Vm=(Main::Benchmark!=0)&&(Virt->Name_Lower=="virt1")&&
             (User_App==false);
    App_Enabled=Bench_Vm||User_App;
    if(Bench_Vm)
        App_Source_List="testsuites/samples/"+App+"/init.c";
    else if(User_App)
    {
        for(size_t Index=0U;Index<App_Files.size();++Index)
        {
            if(Index!=0U)
                App_Source_List+=" ";
            App_Source_List+="bsps/arm/v7m_rvm/app/"+App_Files[Index];
        }
    }
    if((User_App!=false)&&(Placeholder_File!=false))
        std::filesystem::remove(App_Dir/"rte_app.c");

    /* Generate the waf config for the RTEMS build. */
    Main::Info("> Generating 'rte_config.ini'.");
    List=std::make_unique<std::vector<std::string>>();
    List->push_back("[arm/v7m_rvm]");
    List->push_back("BUILD_TESTS = False");
    List->push_back("BUILD_SAMPLES = False");
    List->push_back("RVM_CODE_BASE = 0x"+Main::Hex(Code_Base));
    List->push_back("RVM_CODE_SIZE = 0x"+Main::Hex(Code_Size));
    List->push_back("RVM_DATA_BASE = 0x"+Main::Hex(Data_Base));
    List->push_back("RVM_DATA_SIZE = 0x"+Main::Hex(Rte_Size));
    List->push_back("RVM_USER_STACK_TOP = 0x"+Main::Hex(Vector_Base));
    List->push_back("RVM_ROOT = "+Main::Monitor_Root);
    List->push_back("RTEMS_RVM = True");
    List->push_back("RTE_APP = prc_"+Virt->Name_Lower);
    List->push_back("RTE_APP_ENABLE = "+(App_Enabled?std::string("True"):std::string("False")));
    List->push_back("RTE_APP_SOURCES = "+App_Source_List);
    List->push_back("RTE_BENCHMARK = "+(Bench_Vm?std::string("True"):std::string("False")));
    List->push_back("RTE_FAST_PATH = "+(Fast_Path?std::string("True"):std::string("False")));
    List->push_back("RTE_CPU_FLAGS = "+Cpu_Flags);
    List->push_back("");
    Proj_Gen::Line_Write(List, Virt->Project_Output+"rte_config.ini");

    /* Generate the RTE config header inside the workspace.  The build-time
     * copy script stages it, together with rte_prc.h, into the BSP. */
    Main::Info("> Generating 'rte_config.h'.");
    Hdr=std::make_unique<std::vector<std::string>>();
    Hdr->push_back("/* Generated by RTE_Gen: RTE BSP configuration. */");
    Hdr->push_back("#ifndef RTE_CONFIG_H");
    Hdr->push_back("#define RTE_CONFIG_H");
    Hdr->push_back("");
    Hdr->push_back("#include \"rte_prc.h\"");
    Hdr->push_back("");
    Hdr->push_back("/* Direct context-switch fast path; 0 forces all switches");
    Hdr->push_back(" * through the RVM vector thread (RTE_FAST_PATH in the .ini). */");
    Hdr->push_back("#ifndef RTE_FAST_PATH");
    Hdr->push_back(std::string("#define RTE_FAST_PATH ")+(Fast_Path?"1":"0"));
    Hdr->push_back("#endif");
    Hdr->push_back("");
    Hdr->push_back("/* RTE RTEMS data region inside the RVM data block */");
    Proj_Gen::Macro_Hex(Hdr, "RVM_DATA_EXACT", Exact, MACRO_ADD);
    Proj_Gen::Macro_Hex(Hdr, "RVM_DATA_SIZE", Rte_Size, MACRO_ADD);
    Hdr->push_back("");
    Hdr->push_back("/* RVM-provided stacks (managed by the monitor) */");
    Proj_Gen::Macro_Hex(Hdr, "RTE_USER_STACK_BASE", User_Base, MACRO_ADD);
    Proj_Gen::Macro_Hex(Hdr, "RTE_USER_STACK_SIZE", Virt->User_Stack_Size, MACRO_ADD);
    Proj_Gen::Macro_Hex(Hdr, "RTE_VECTOR_STACK_BASE", Vector_Base, MACRO_ADD);
    Proj_Gen::Macro_Hex(Hdr, "RTE_VECTOR_STACK_SIZE", Virt->Vector_Stack_Size, MACRO_ADD);
    Proj_Gen::Macro_Hex(Hdr, "RTE_DATA_BLOCK_END", Data_End, MACRO_ADD);
    Hdr->push_back("");
    Hdr->push_back("#endif /* RTE_CONFIG_H */");
    Hdr->push_back("");
    Proj_Gen::Line_Write(Hdr, Virt->Main_Header_Output+"rte_config.h");

    /* Per-VM application folder.  Everything in Source/app is the app and is
     * copied as a folder before waf; the benchmark wrapper includes the
     * generated test header (chip selection) and the test file, while other
     * VMs keep an empty placeholder. */
    Main::Info("> Generating RTE application folder.");
    std::filesystem::create_directories(Virt->Main_Source_Output+"app");
    if(Bench_Vm)
    {
        /* The benchmark application is rte_benchmark/init.c itself; the app
         * folder only supplies the chip-selection test header. */
        Main::Info("> Generating RTE test chip selection header.");
        List=std::make_unique<std::vector<std::string>>();
        List->push_back("/* Generated by RTE_Gen: benchmark chip selection. */");
        List->push_back("#ifndef RTE_TEST_H");
        List->push_back("#define RTE_TEST_H");
        List->push_back("#include \""+App+"/chip/rte_test_"+
                        Chip->Name_Lower+".h\"");
        List->push_back("#endif /* RTE_TEST_H */");
        List->push_back("");
        Proj_Gen::Line_Write(List, Virt->Main_Source_Output+"app/rte_test.h");
    }
    else if(User_App==false)
    {
        /* Non-benchmark VMs without user sources get an empty template. */
        List=std::make_unique<std::vector<std::string>>();
        List->push_back("/* Generated by RTE_Gen: empty per-VM RTE app. */");
        List->push_back("/* No RTE application selected for this VM. */");
        List->push_back("");
        Proj_Gen::Line_Write(List, Virt->Main_Source_Output+"app/rte_app.c");
    }
}
/* End Function:RTE_Gen::Process_Virt_Hdr ************************************/

/* Function:RTE_Gen::Process_Virt_Src *****************************************
Description : Generate the Virt makefile that drives the RTEMS waf build.
Input       : class Virtual* Virt - The process to generate for.
              class Tool_Gen* Tool - The toolchain to use.
Output      : None.
Return      : None.
******************************************************************************/
void RTE_Gen::Process_Virt_Src(class Virtual* Virt, class Tool_Gen* Tool)
{
    std::string Root;
    std::string Prefix;
    std::string Vm;
    std::string Entry_Dir;
    std::string Header_Dir;
    std::string Bsp_Dir;
    std::string Bsp_Inc_Dir;
    std::string Bsp_App_Dir;
    std::unique_ptr<std::vector<std::string>> List;

    (void) Tool;

    Root=Root_Get();
    Prefix=Prefix_Get();
    Vm=Virt->Name_Lower;
    Entry_Dir=std::filesystem::absolute(
        Virt->Main_Source_Output).generic_string();
    Header_Dir=std::filesystem::absolute(
        Virt->Main_Header_Output).generic_string();
    Bsp_Dir=std::filesystem::path(
        Root+"bsps/arm/v7m_rvm/start").generic_string();
    Bsp_Inc_Dir=std::filesystem::path(
        Root+"bsps/arm/v7m_rvm/include").generic_string();
    Bsp_App_Dir=std::filesystem::path(
        Root+"bsps/arm/v7m_rvm/app").generic_string();

    /* Write the staging script used before waf.  The BSP source list uses
     * fixed tree-relative paths; waf cannot resolve the generated workspace
     * outside the RTEMS tree. */
    Main::Info("> Generating RTE entry copy script.");
    List=std::make_unique<std::vector<std::string>>();
    List->push_back("#!/bin/sh");
    List->push_back("# Generated by RTE_Gen: stage the generated headers and entry");
    List->push_back("# sources into the RTEMS BSP tree before waf runs.");
    List->push_back("set -e");
    List->push_back("SRC=\""+Entry_Dir+"\"");
    List->push_back("INC=\""+Header_Dir+"\"");
    List->push_back("DST=\""+Bsp_Dir+"\"");
    List->push_back("DST_INC=\""+Bsp_Inc_Dir+"\"");
    List->push_back("DST_APP=\""+Bsp_App_Dir+"\"");
    List->push_back("mkdir -p \"$DST\" \"$DST_INC\"");
    List->push_back("cp -f \""+Main::Monitor_Root+
                    "Source/Platform/A7M/rvm_platform_a7m_gcc.s\" \"$DST/rvm_platform_a7m_gcc.S\"");
    List->push_back("cp -f \"$SRC/prc_"+Vm+".c\" \"$DST/rte_entry.c\"");
    List->push_back("cp -f \"$SRC/prc_"+Vm+"_desc.c\" \"$DST/rte_entry_desc.c\"");
    List->push_back("cp -f \"$INC/prc_"+Vm+".h\" \"$DST_INC/rte_prc.h\"");
    List->push_back("cp -f \"$INC/prc_"+Vm+".h\" \"$DST_INC/prc_"+Vm+".h\"");
    List->push_back("cp -f \"$INC/rvm_platform.h\" \"$DST_INC/rvm_platform.h\"");
    List->push_back("cp -f \"$INC/rvm_platform_a7m_conf.h\" \"$DST_INC/rvm_platform_a7m_conf.h\"");
    List->push_back("cp -f \"$INC/rte_config.h\" \"$DST_INC/rte_config.h\"");
    List->push_back("rm -rf \"$DST_APP\"");
    List->push_back("mkdir -p \"$DST_APP\"");
    List->push_back("cp -rf \"$SRC/app/.\" \"$DST_APP/\"");
    List->push_back("");
    Proj_Gen::Line_Write(List, Virt->Project_Output+"rte_copy.sh");

    Main::Info("> Generating RTE "+Vm+" waf makefile.");
    List=std::make_unique<std::vector<std::string>>();
    List->push_back("# Generated by RTE_Gen: the "+Vm+" makefile drives the RTEMS waf build.");
    List->push_back("TARGET := "+Vm);
    List->push_back("OBJDIR := Object");
    List->push_back("RTEMS_DIR := "+Root);
    List->push_back("RTEMS_PREFIX := "+Prefix);
    List->push_back("RTEMS_PREFIX_POSIX := $(shell cygpath -u '$(RTEMS_PREFIX)')");
    List->push_back("RTEMS_BSP := arm/v7m_rvm");
    List->push_back("RTE_APP := prc_"+Vm);
    List->push_back("RTE_CFG := $(CURDIR)/rte_config.ini");
    List->push_back("RTE_COPY := $(CURDIR)/rte_copy.sh");
    List->push_back("PYTHON := /usr/bin/python3");
    List->push_back("OBJCOPY := $(RTEMS_PREFIX)/bin/arm-rtems7-objcopy");
    List->push_back("BINCOPY := ../../../../Utility/M7M02_Final/bincopy.exe");
    List->push_back("");
    List->push_back("export PATH := $(RTEMS_PREFIX_POSIX)/bin:/usr/bin:/c/Toolchain/msys64/mingw64/bin:$(PATH)");
    List->push_back("export TEMP := C:/Temp");
    List->push_back("export TMP := C:/Temp");
    List->push_back("export TMPDIR := C:/Temp");
    List->push_back("");
    List->push_back(".PHONY: all clean rte_force");
    List->push_back("all: "+Vm+"_image.c");
    List->push_back("");
    List->push_back("$(OBJDIR):");
    List->push_back("\tmkdir -p $(OBJDIR)");
    List->push_back("");
    List->push_back("# The RTEMS tree and its build directory are shared.  Re-stage this VM and");
    List->push_back("# reconfigure on every build so a previous VM's layout cannot leak in;");
    List->push_back("# Virt builds must be serialized by the caller.");
    List->push_back("$(OBJDIR)/"+Vm+".bin: rte_force | $(OBJDIR)");
    List->push_back("\tsh $(RTE_COPY)");
    List->push_back("\tcd $(RTEMS_DIR) && $(PYTHON) ./waf configure --rtems-config=$(RTE_CFG) --prefix=$(RTEMS_PREFIX)");
    List->push_back("\trm -f $(RTEMS_DIR)/build/$(RTEMS_BSP)/testsuites/samples/$(RTE_APP).exe");
    List->push_back("\tcd $(RTEMS_DIR) && $(PYTHON) ./waf");
    List->push_back("\t$(OBJCOPY) -O binary $(RTEMS_DIR)/build/$(RTEMS_BSP)/testsuites/samples/$(RTE_APP).exe $@");
    List->push_back("");
    List->push_back(Vm+"_image.c: $(OBJDIR)/"+Vm+".bin");
    List->push_back("\t$(BINCOPY) -i $< -o $@");
    List->push_back("");
    List->push_back("rte_force:");
    List->push_back("");
    List->push_back("clean:");
    List->push_back("\trm -rf $(OBJDIR) "+Vm+"_image.c");
    List->push_back("");
    Proj_Gen::Line_Write(List, Virt->Project_Output+"prc_"+Vm);
}
/* End Function:RTE_Gen::Process_Virt_Src ************************************/
}
/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
