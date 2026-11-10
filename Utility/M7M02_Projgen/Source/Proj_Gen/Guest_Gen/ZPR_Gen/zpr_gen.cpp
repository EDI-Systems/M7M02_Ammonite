/******************************************************************************
Filename    : zpr_gen.cpp
Author      : Evo-Devo Instrum.
Date        : 08/11/2026
Licence     : LGPL v3+; see COPYING for details.
Description : This toolset is for ZPR, the Zephyr guest on the RVM.
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
#include "Proj_Info/Process/process.hpp"
#include "Proj_Gen/proj_gen.hpp"
#include "Proj_Gen/Tool_Gen/tool_gen.hpp"
#include "Proj_Gen/Guest_Gen/guest_gen.hpp"
#include "Proj_Gen/Guest_Gen/ZPR_Gen/zpr_gen.hpp"
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
#include "Proj_Gen/Guest_Gen/guest_gen.hpp"
#include "Proj_Gen/Guest_Gen/ZPR_Gen/zpr_gen.hpp"
#undef __HDR_CLASS__
/* End Include ***************************************************************/
namespace RVM_GEN
{
/* Function:ZPR_Gen::ZPR_Gen **************************************************
Description : Generator for the Zephyr-on-RVM guest.
Input       : class Proj_Info* Proj - The project information.
              class Plat_Info* Plat - The platform information.
              class Chip_Info* Chip - The chip information.
Output      : None.
Return      : None.
******************************************************************************/
/* void */ ZPR_Gen::ZPR_Gen(class Proj_Info* Proj, class Plat_Info* Plat, class Chip_Info* Chip):
Guest_Gen("ZPR", Proj, Plat, Chip)
{
    try
    {

    }
    catch(std::exception& Exc)
    {
        Main::Error(std::string("ZPR generator:\n")+Exc.what());
    }
}
/* End Function:ZPR_Gen::ZPR_Gen *********************************************/

/* Function:ZPR_Gen::Root_Get *************************************************
Description : Return the Zephyr workspace root.
Input       : None.
Output      : None.
Return      : std::string - The Zephyr workspace root.
******************************************************************************/
std::string ZPR_Gen::Root_Get(void)
{
    if(Main::Guest_ZPR_Root=="")
        Main::Error("ZPR generator:\nThe Zephyr workspace root is not specified.");
    return Main::Guest_ZPR_Root;
}
/* End Function:ZPR_Gen::Root_Get ********************************************/

/* Function:ZPR_Gen::Board_Get ************************************************
Description : Select the single RVM board and derive its SoC qualifier.
Input       : None.
Output      : None.
Return      : std::string - The Zephyr board name.
******************************************************************************/
std::string ZPR_Gen::Board_Get(void)
{
    /* One virtual board serves every ARMv7-M guest.  CPU/FPU and memory are
     * selected per VM from the .rvp and written into the generated DTS. */
    return "armv7m";
}
/* End Function:ZPR_Gen::Board_Get *******************************************/

/* Function:ZPR_Gen::Process_Virt_Hdr *****************************************
Description : Generate the Zephyr app, its configuration and linker data.
Input       : class Virtual* Virt - The process to generate for.
Output      : None.
Return      : None.
******************************************************************************/
void ZPR_Gen::Process_Virt_Hdr(class Virtual* Virt)
{
    const class Mem_Info* Code;
    const class Mem_Info* Data;
    ptr_t Code_Base;
    ptr_t Code_Size;
    ptr_t Data_Base;
    ptr_t Data_Size;
    ptr_t Exact;
    ptr_t Rte_Size;
    bool Fast_Path;
    bool User_App;
    bool Bench_App;
    std::string App_Dir;
    std::string App_Source_List;
    std::string Include_Dir;
    std::string Port_Dir;
    std::string Rvm_Root;
    std::string Board;
    std::vector<std::string> App_Files;
    std::unique_ptr<std::vector<std::string>> List;

    Code=nullptr;
    Data=nullptr;
    if(Virt->Memory_Code.empty()==false)
        Code=Virt->Memory_Code[0];
    if(Virt->Memory_Data.empty()==false)
        Data=Virt->Memory_Data[0];
    if((Code==nullptr)||(Data==nullptr))
        Main::Error("ZPR generator:\nThe Virt Code1/Data1 blocks are missing.");

    Code_Base=Code->Base;
    Code_Size=Code->Size;
    Data_Base=Data->Base;
    Data_Size=Data->Size;
    Exact=Data_Size-Virt->Reg_Size-Virt->State_Size-
          Virt->User_Stack_Size-Virt->Vector_Stack_Size;
    Rte_Size=Exact-0x400U;

    Fast_Path=true;
    {
        const char* Fast_Path_Env=std::getenv("ZPR_FAST_PATH");
        if(Fast_Path_Env==nullptr)
            Fast_Path_Env=std::getenv("RTE_FAST_PATH"); /* Legacy name. */
        if((Fast_Path_Env!=nullptr)&&(std::string(Fast_Path_Env)=="0"))
            Fast_Path=false;
    }

    Rvm_Root=Main::Monitor_Root;
    if((Rvm_Root.empty()==false)&&(Rvm_Root.back()!='/'))
        Rvm_Root+="/";
    Port_Dir=Root_Get();
    if((Port_Dir.empty()==false)&&(Port_Dir.back()!='/'))
        Port_Dir+="/";
    Port_Dir+="zephyr/arch/arm/core/cortex_m/v7m_rvm/port/";
    Board=Board_Get();
    App_Dir=std::filesystem::absolute(
        Virt->Main_Source_Output+"app").generic_string();
    Include_Dir=std::filesystem::absolute(
        Virt->Main_Header_Output).generic_string();
    std::filesystem::create_directories(App_Dir);
    std::filesystem::create_directories(Include_Dir);

    for(const std::filesystem::directory_entry& Entry:
        std::filesystem::directory_iterator(App_Dir))
    {
        if((Entry.is_regular_file()!=false)&&
           (Entry.path().extension()==".c"))
            App_Files.push_back(Entry.path().filename().string());
    }
    std::sort(App_Files.begin(),App_Files.end());
    User_App=(App_Files.empty()==false);

    /* A user app wins over the benchmark in every guest.  The benchmark is
     * the fallback when the Virt app folder has no .c; ZPR_NORMAL still forces
     * a normal image (smoke app if empty) because generate.sh passes -b only
     * for the native TIM4 heartbeat. */
    Bench_App=(Main::Benchmark!=0)&&(User_App==false)&&
              (std::getenv("ZPR_NORMAL")==nullptr);

    /* Configuration header consumed by zpr_context.c. */
    Main::Info("> Generating 'zpr_config.h'.");
    List=std::make_unique<std::vector<std::string>>();
    List->push_back("/* Generated by ZPR_Gen: Zephyr-on-RVM configuration. */");
    List->push_back("#ifndef ZPR_CONFIG_H");
    List->push_back("#define ZPR_CONFIG_H");
    List->push_back(std::string("#define ZPR_CODE_BASE 0x")+Main::Hex(Code_Base)+"U");
    List->push_back(std::string("#define ZPR_CODE_SIZE 0x")+Main::Hex(Code_Size)+"U");
    List->push_back(std::string("#define ZPR_FAST_PATH ")+(Fast_Path?"1":"0"));
    List->push_back("#endif /* ZPR_CONFIG_H */");
    List->push_back("");
    Proj_Gen::Line_Write(List, Include_Dir+"/zpr_config.h");

    /* RVM platform selection: use the generic per-VM prc header. */
    List=std::make_unique<std::vector<std::string>>();
    List->push_back("/* Generated by ZPR_Gen: RVM per-VM macros. */");
    List->push_back("#ifndef ZPR_PRC_H");
    List->push_back("#define ZPR_PRC_H");
    List->push_back(std::string("#include \"prc_")+Virt->Name_Lower+".h\"");
    List->push_back("#endif /* ZPR_PRC_H */");
    List->push_back("");
    Proj_Gen::Line_Write(List, Include_Dir+"/zpr_prc.h");

    /* The benchmark-only chip selection header; normal apps never see it. */
    if(Bench_App)
    {
        List=std::make_unique<std::vector<std::string>>();
        List->push_back("/* Generated by ZPR_Gen: chip test header selection. */");
        List->push_back("#ifndef ZPR_TEST_H");
        List->push_back("#define ZPR_TEST_H");
        List->push_back(std::string("#include \"zpr_test_")+Chip->Name_Lower+".h\"");
        List->push_back("#endif /* ZPR_TEST_H */");
        List->push_back("");
        Proj_Gen::Line_Write(List, Include_Dir+"/zpr_test.h");
    }
    else
    {
        /* A normal image must not keep a stale benchmark chip header. */
        std::filesystem::remove(Include_Dir+"/zpr_test.h");
    }

    /* The benchmark application lives in the Zephyr tree. */
    if(Bench_App)
    {
        std::string Bench=Root_Get();
        if((Bench.empty()==false)&&(Bench.back()!='/'))
            Bench+="/";
        App_Source_List="\""+Bench+"zephyr/arch/arm/core/cortex_m/v7m_rvm/test/bench.c\"";
    }
    /* A missing user app gets a small smoke-test main. */
    else if(User_App==false)
    {
        Main::Info("> Generating ZPR smoke-test application.");
        List=std::make_unique<std::vector<std::string>>();
        List->push_back("/* Generated by ZPR_Gen: ZPR smoke test. */");
        List->push_back("#include <zephyr/kernel.h>");
        List->push_back("int main(void)");
        List->push_back("{");
        List->push_back("    printk(\"ZPR guest: no user application selected\\n\");");
        List->push_back("    return 0;");
        List->push_back("}");
        List->push_back("");
        Proj_Gen::Line_Write(List, App_Dir+"/main.c");
        App_Source_List="\"${CMAKE_CURRENT_SOURCE_DIR}/main.c\"";
    }
    else
    {
        for(size_t Index=0U;Index<App_Files.size();++Index)
        {
            if(Index!=0U)
                App_Source_List+=" ";
            App_Source_List+="\"${CMAKE_CURRENT_SOURCE_DIR}/"+App_Files[Index]+"\"";
        }
    }

    /* The generic generator owns the VM descriptor and the vector thread.
     * ZPR supplies its user entry and descriptor section through the
     * Guest_Gen descriptor policy; only the source list is assembled here. */
    {
        std::string Desc_Name="prc_"+Virt->Name_Lower+"_desc.c";
        std::string Main_Name="prc_"+Virt->Name_Lower+".c";
        std::string Desc_Path=std::filesystem::absolute(
            Virt->Main_Source_Output+Desc_Name).generic_string();
        std::string Main_Path=std::filesystem::absolute(
            Virt->Main_Source_Output+Main_Name).generic_string();

        App_Source_List+=" \""+Main_Path+"\" \""+Desc_Path+"\"";
    }

    Main::Info("> Generating ZPR application CMake project.");
    List=std::make_unique<std::vector<std::string>>();
    List->push_back("# Generated by ZPR_Gen: ZPR sources and include paths.");
    List->push_back("set(RVM_ROOT \""+Rvm_Root+"\")");
    List->push_back("set(ZPR_PORT \""+Port_Dir+"\")");
    List->push_back("configure_file(\"${RVM_ROOT}Source/Platform/A7M/rvm_platform_a7m_gcc.s\"");
    List->push_back("               \"${CMAKE_CURRENT_BINARY_DIR}/rvm_platform_a7m_gcc.S\"");
    List->push_back("               COPYONLY)");
    List->push_back("target_sources(app PRIVATE");
    List->push_back("  "+App_Source_List);
    List->push_back("  ${ZPR_PORT}/zpr_entry.c");
    List->push_back("  ${ZPR_PORT}/zpr_context.c");
    List->push_back("  ${ZPR_PORT}/zpr_irq.c");
    List->push_back("  ${ZPR_PORT}/zpr_printk.c");
    List->push_back("  ${ZPR_PORT}/zpr_timer.c");
    List->push_back("  ${RVM_ROOT}Source/Platform/A7M/rvm_platform_a7m.c");
    List->push_back("  ${RVM_ROOT}Source/Syslib/rvm_syslib.c");
    List->push_back("  ${RVM_ROOT}Source/Virtlib/rvm_virtlib.c");
    List->push_back("  \"${CMAKE_CURRENT_BINARY_DIR}/rvm_platform_a7m_gcc.S\"");
    List->push_back("  \"${ZPR_PORT}/v7m_context_switch_rvm.S\"");
    List->push_back(")");
    List->push_back("set_source_files_properties(\"${ZPR_PORT}/v7m_context_switch_rvm.S\"");
    List->push_back(std::string("  PROPERTIES COMPILE_DEFINITIONS \"ZPR_FAST_PATH=")+
                    (Fast_Path?"1\"":"0\"")+")");
    List->push_back("target_include_directories(app PRIVATE");
    List->push_back("  \""+Include_Dir+"\"");
    if(Bench_App)
    {
        std::string Chip_Dir=Root_Get();
        if((Chip_Dir.empty()==false)&&(Chip_Dir.back()!='/'))
            Chip_Dir+="/";
        Chip_Dir+="zephyr/arch/arm/core/cortex_m/v7m_rvm/test/chip";
        List->push_back("  \""+Chip_Dir+"\"");
    }
    List->push_back("  ${ZPR_PORT}/include");
    List->push_back("  ${RVM_ROOT}Include");
    List->push_back(")");
    List->push_back("zephyr_linker_sources(ROM_START SORT_KEY 0x0zpr_desc ${ZPR_PORT}/zpr_desc.ld)");
    List->push_back("");
    Proj_Gen::Line_Write(List, App_Dir+"/zpr_app.cmake");

    /* Keep a user CMakeLists.txt: a generated file is replaced, but a
     * hand-written one only gets the generated fragment included once. */
    {
        std::filesystem::path CMake_Path(App_Dir+"/CMakeLists.txt");
        bool Generated=false;
        bool Includes_Fragment=false;
        if(std::filesystem::exists(CMake_Path)!=false)
        {
            std::unique_ptr<std::vector<std::string>> Lines=Proj_Gen::Line_Read(CMake_Path.string());
            for(const std::string& Line:*Lines)
            {
                if(Line.find("# Generated by ZPR_Gen")!=std::string::npos)
                    Generated=true;
                if(Line.find("zpr_app.cmake")!=std::string::npos)
                    Includes_Fragment=true;
            }
            if((Generated==false)&&(Includes_Fragment==false))
            {
                Lines->push_back("");
                Lines->push_back("# Added by ZPR_Gen; the generated block lives in zpr_app.cmake.");
                Lines->push_back("include(${CMAKE_CURRENT_SOURCE_DIR}/zpr_app.cmake)");
                Proj_Gen::Line_Write(Lines, CMake_Path.string());
            }
        }
        if((std::filesystem::exists(CMake_Path)==false)||Generated)
        {
            List=std::make_unique<std::vector<std::string>>();
            List->push_back("# Generated by ZPR_Gen: Zephyr application for "+Virt->Name_Lower+".");
            List->push_back("cmake_minimum_required(VERSION 3.20.0)");
            List->push_back("find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})");
            List->push_back("project(prc_"+Virt->Name_Lower+")");
            List->push_back("include(${CMAKE_CURRENT_SOURCE_DIR}/zpr_app.cmake)");
            List->push_back("");
            Proj_Gen::Line_Write(List, CMake_Path.string());
        }
    }

    /* Keep user Kconfig options.  The VM window and the three defaults ZPR
     * needs are generator-owned; everything else in prj.conf is preserved. */
    {
        std::filesystem::path Prj_Path(App_Dir+"/prj.conf");
        bool Generated=false;
        bool Has_Main=false;
        bool Has_Events=false;
        bool Has_Heap=false;
        bool M4=false;
        for(const std::string& Cop:Virt->Coprocessor)
        {
            if(Cop=="FPV4_SP")
                M4=true;
        }
        if(std::filesystem::exists(Prj_Path)!=false)
        {
            std::unique_ptr<std::vector<std::string>> Lines=Proj_Gen::Line_Read(Prj_Path.string());
            for(const std::string& Line:*Lines)
            {
                if(Line.find("# Generated by ZPR_Gen")!=std::string::npos)
                    Generated=true;
            }
            if(Generated==false)
            {
                std::unique_ptr<std::vector<std::string>> Output=std::make_unique<std::vector<std::string>>();
                for(const std::string& Line:*Lines)
                {
                    if((Line.rfind("CONFIG_FLASH_BASE_ADDRESS=",0)==0)||
                       (Line.rfind("CONFIG_FLASH_SIZE=",0)==0)||
                       (Line.rfind("CONFIG_SRAM_BASE_ADDRESS=",0)==0)||
                       (Line.rfind("CONFIG_SRAM_SIZE=",0)==0))
                        continue;
                    if(Line.rfind("CONFIG_MAIN_STACK_SIZE=",0)==0)
                        Has_Main=true;
                    if(Line.rfind("CONFIG_EVENTS=",0)==0)
                        Has_Events=true;
                    if(Line.rfind("CONFIG_HEAP_MEM_POOL_SIZE=",0)==0)
                        Has_Heap=true;
                    if(Line.rfind("CONFIG_RVM_CPU_",0)==0)
                        continue;
                    Output->push_back(Line);
                }
                if(Has_Main==false)
                    Output->push_back("CONFIG_MAIN_STACK_SIZE=4096");
                if(Has_Events==false)
                    Output->push_back("CONFIG_EVENTS=y");
                if(Has_Heap==false)
                    Output->push_back("CONFIG_HEAP_MEM_POOL_SIZE=8192");
                Output->push_back(M4?"CONFIG_RVM_CPU_M4=y":"CONFIG_RVM_CPU_M7=y");
                Output->push_back("");
                Proj_Gen::Line_Write(Output, Prj_Path.string());
            }
        }
        if((std::filesystem::exists(Prj_Path)==false)||Generated)
        {
            List=std::make_unique<std::vector<std::string>>();
            List->push_back("# Generated by ZPR_Gen.");
            List->push_back("CONFIG_MAIN_STACK_SIZE=4096");
            List->push_back("CONFIG_EVENTS=y");
            List->push_back("CONFIG_HEAP_MEM_POOL_SIZE=8192");
            List->push_back(M4?"CONFIG_RVM_CPU_M4=y":"CONFIG_RVM_CPU_M7=y");
            List->push_back("");
            Proj_Gen::Line_Write(List, Prj_Path.string());
        }
    }

    /* Generate the complete per-VM devicetree.  The board keeps a fixed
     * fallback DTS; every generated build passes DTS_SOURCE so the VM's CPU
     * and memory windows are never taken from a stale board file. */
    Main::Info("> Generating ZPR board DTS.");
    bool Dts_M4=false;
    for(const std::string& Cop:Virt->Coprocessor)
    {
        if(Cop=="FPV4_SP")
            Dts_M4=true;
    }
    List=std::make_unique<std::vector<std::string>>();
    List->push_back("/dts-v1/;");
    List->push_back("");
    List->push_back("#include <arm/armv7-m.dtsi>");
    List->push_back("");
    List->push_back("&nvic {");
    List->push_back("\tarm,num-irq-priority-bits = <4>;");
    List->push_back("};");
    List->push_back("");
    List->push_back("/ {");
    List->push_back("\tmodel = \"RVM ARMv7-M guest\";");
    List->push_back("\tcompatible = \"rvm,armv7m\";");
    List->push_back("");
    List->push_back("\tchosen {");
    List->push_back("\t\tzephyr,flash = &flash0;");
    List->push_back("\t\tzephyr,sram = &sram0;");
    List->push_back("\t};");
    List->push_back("");
    List->push_back("\tcpus {");
    List->push_back("\t\t#address-cells = <1>;");
    List->push_back("\t\t#size-cells = <0>;");
    List->push_back("");
    List->push_back("\t\tcpu@0 {");
    List->push_back(std::string("\t\t\tcompatible = \"")+
                    (Dts_M4?"arm,cortex-m4":"arm,cortex-m7")+"\";");
    List->push_back("\t\t\treg = <0>;");
    List->push_back("\t\t};");
    List->push_back("\t};");
    List->push_back("");
    List->push_back(std::string("\tflash0: flash@")+Main::Hex(Code_Base)+" {");
    List->push_back("\t\tcompatible = \"soc-nv-flash\";");
    List->push_back(std::string("\t\treg = <0x")+Main::Hex(Code_Base)+
                    " 0x"+Main::Hex(Code_Size)+">;");
    List->push_back("\t};");
    List->push_back("");
    List->push_back(std::string("\tsram0: memory@")+Main::Hex(Data_Base)+" {");
    List->push_back("\t\tcompatible = \"mmio-sram\";");
    List->push_back(std::string("\t\treg = <0x")+Main::Hex(Data_Base)+
                    " 0x"+Main::Hex(Rte_Size)+">;");
    List->push_back("\t};");
    List->push_back("};");
    List->push_back("");
    Proj_Gen::Line_Write(List, std::filesystem::absolute(
        Virt->Project_Output+"zpr_board.dts").generic_string());

    (void) Board;
}
/* End Function:ZPR_Gen::Process_Virt_Hdr ************************************/

/* Function:ZPR_Gen::Process_Virt_Src *****************************************
Description : Generate the Virt makefile that drives the Zephyr west build.
Input       : class Virtual* Virt - The process to generate for.
              class Tool_Gen* Tool - The toolchain to use.
Output      : None.
Return      : None.
******************************************************************************/
void ZPR_Gen::Process_Virt_Src(class Virtual* Virt, class Tool_Gen* Tool)
{
    std::string Root;
    std::string Vm;
    std::string App_Dir;
    std::string Board;
    std::string Build_Dir;
    std::unique_ptr<std::vector<std::string>> List;

    (void) Tool;

    Root=Root_Get();
    Vm=Virt->Name_Lower;
    App_Dir=std::filesystem::absolute(
        Virt->Main_Source_Output+"app").generic_string();
    Build_Dir="$(CURDIR)/build";
    Board=Board_Get();

    Main::Info("> Generating ZPR "+Vm+" west makefile.");
    List=std::make_unique<std::vector<std::string>>();
    List->push_back("# Generated by ZPR_Gen: the "+Vm+" makefile drives the Zephyr west build.");
    List->push_back("TARGET := "+Vm);
    List->push_back("OBJDIR := Object");
    List->push_back("ZEPHYR_ROOT := "+Root);
    List->push_back("ZEPHYR_BASE := "+Root+"zephyr");
    List->push_back("ZPR_PORT := "+Root+"zephyr/arch/arm/core/cortex_m/v7m_rvm/port");
    List->push_back("APP_DIR := "+App_Dir);
    List->push_back("BUILD_DIR := "+Build_Dir);
    List->push_back("BOARD := "+Board);
    List->push_back("DTS := $(CURDIR)/zpr_board.dts");
    List->push_back("WEST ?= west");
    List->push_back("PREFIX ?=");
    List->push_back("ifeq ($(strip $(PREFIX)),)");
    List->push_back("  ARM_GCC := $(shell command -v arm-none-eabi-gcc 2>/dev/null)");
    List->push_back("  PREFIX := $(patsubst %/bin/arm-none-eabi-gcc,%,$(patsubst %.exe,%,$(ARM_GCC)))");
    List->push_back("endif");
    List->push_back("WIN := $(findstring MINGW,$(shell uname -s))$(findstring MSYS,$(shell uname -s))");
    List->push_back("BINCOPY := ../../../../Utility/M7M02_Final/bincopy");
    List->push_back("ifneq ($(strip $(WIN)),)");
    List->push_back("  BINCOPY := ../../../../Utility/M7M02_Final/bincopy.exe");
    List->push_back("endif");
    List->push_back("");
    List->push_back("export ZEPHYR_BASE");
    List->push_back("export ZEPHYR_TOOLCHAIN_VARIANT := gnuarmemb");
    List->push_back("PREFIX_NATIVE := $(shell command -v cygpath >/dev/null 2>&1 && cygpath -m '$(PREFIX)' || echo '$(PREFIX)')");
    List->push_back("export GNUARMEMB_TOOLCHAIN_PATH := $(PREFIX_NATIVE)");
    List->push_back("ifneq ($(strip $(WIN)),)");
    List->push_back("  export USERPROFILE := $(shell cygpath -w \"$(HOME)\")");
    List->push_back("  export TEMP := C:/Temp");
    List->push_back("  export TMP := C:/Temp");
    List->push_back("  export TMPDIR := C:/Temp");
    List->push_back("endif");
    List->push_back("");
    List->push_back(".PHONY: all clean zpr_force");
    List->push_back("all: "+Vm+"_image.c");
    List->push_back("");
    List->push_back("$(OBJDIR):");
    List->push_back("\tmkdir -p $(OBJDIR)");
    List->push_back("");
    List->push_back("$(OBJDIR)/"+Vm+".bin: zpr_force | $(OBJDIR)");
    List->push_back("\t$(WEST) build -p always -b $(BOARD) -d $(BUILD_DIR) $(APP_DIR) -- -DDTS_SOURCE=$(DTS)");
    List->push_back("\tcp $(BUILD_DIR)/zephyr/zephyr.bin $@");
    List->push_back("");
    List->push_back(Vm+"_image.c: $(OBJDIR)/"+Vm+".bin");
    List->push_back("\t$(BINCOPY) -i $< -o $@");
    List->push_back("");
    List->push_back("zpr_force:");
    List->push_back("");
    List->push_back("clean:");
    List->push_back("\trm -rf $(OBJDIR) $(BUILD_DIR) "+Vm+"_image.c");
    List->push_back("");
    Proj_Gen::Line_Write(List, Virt->Project_Output+"prc_"+Vm);
}
/* End Function:ZPR_Gen::Process_Virt_Src ************************************/

/* Function:ZPR_Gen::Process_Linker *******************************************
Description : Skip linker script generation; Zephyr owns the linker, with the
              descriptor fragment injected by zpr_desc.ld.
Input       : std::unique_ptr<std::vector<std::string>>& List - The lines.
              const class Process* Prc - The process to generate for.
              class Tool_Gen* Tool - The toolchain generator.
Output      : None.
Return      : None.
******************************************************************************/
void ZPR_Gen::Process_Linker(std::unique_ptr<std::vector<std::string>>& List,
                             const class Process* Prc, class Tool_Gen* Tool)
{
    (void)List;
    (void)Tool;
    Main::Info("> ZPR: using the guest link script, skipping linker script generation.");
    std::filesystem::remove(Prc->Linker_Output+Prc->Linker_Filename);
}
/* End Function:ZPR_Gen::Process_Linker **************************************/

/* Function:ZPR_Gen::Desc_Usr_Entry *******************************************
Description : The RVM descriptor starts the Zephyr user thread at this entry.
Input       : None.
Output      : None.
Return      : The ZPR user entry symbol.
******************************************************************************/
std::string ZPR_Gen::Desc_Usr_Entry(void)
{
    return "__zpr_entry";
}
/* End Function:ZPR_Gen::Desc_Usr_Entry **************************************/
}
/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
