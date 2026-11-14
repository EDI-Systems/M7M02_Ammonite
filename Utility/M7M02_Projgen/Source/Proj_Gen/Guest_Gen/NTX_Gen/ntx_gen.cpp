/******************************************************************************
Filename    : ntx_gen.cpp
Author      : Evo-Devo Instrum.
Date        : 12/11/2026
Licence     : LGPL v3+; see COPYING for details.
Description : This toolset is for NTX, the NuttX guest on the RVM.
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
#include "Proj_Gen/Guest_Gen/NTX_Gen/ntx_gen.hpp"
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
#include "Proj_Info/Process/Kfunc/kfunc.hpp"
#include "Proj_Info/Process/Virtual/virtual.hpp"
#include "Mem_Info/mem_info.hpp"
#include "Vect_Info/vect_info.hpp"
#include "Conf_Info/conf_info.hpp"
#include "Proj_Gen/proj_gen.hpp"
#include "Proj_Gen/Guest_Gen/guest_gen.hpp"
#include "Proj_Gen/Guest_Gen/NTX_Gen/ntx_gen.hpp"
#undef __HDR_CLASS__
/* End Include ***************************************************************/
namespace RVM_GEN
{
/* Function:NTX_Gen::NTX_Gen **************************************************
Description : Generator for the NuttX-on-RVM guest.
Input       : class Proj_Info* Proj - The project information.
              class Plat_Info* Plat - The platform information.
              class Chip_Info* Chip - The chip information.
Output      : None.
Return      : None.
******************************************************************************/
/* void */ NTX_Gen::NTX_Gen(class Proj_Info* Proj, class Plat_Info* Plat, class Chip_Info* Chip):
Guest_Gen("NTX", Proj, Plat, Chip)
{
    try
    {

    }
    catch(std::exception& Exc)
    {
        Main::Error(std::string("NTX generator:\n")+Exc.what());
    }
}
/* End Function:NTX_Gen::NTX_Gen *********************************************/

/* Function:NTX_Gen::Root_Get *************************************************
Description : Return the NuttX tree root.
Input       : None.
Output      : None.
Return      : std::string - The NuttX tree root.
******************************************************************************/
std::string NTX_Gen::Root_Get(void)
{
    if(Main::Guest_NTX_Root=="")
        Main::Error("NTX generator:\nThe NuttX tree root is not specified.");
    return Main::Guest_NTX_Root;
}
/* End Function:NTX_Gen::Root_Get ********************************************/

/* Function:NTX_Gen::Process_Virt_Hdr *****************************************
Description : Generate the per-Virt NuttX board, its defconfig and the app.
Input       : class Virtual* Virt - The process to generate for.
Output      : None.
Return      : None.
******************************************************************************/
void NTX_Gen::Process_Virt_Hdr(class Virtual* Virt)
{
    const class Mem_Info* Code;
    const class Mem_Info* Data;
    ptr_t Code_Base;
    ptr_t Code_Size;
    ptr_t Data_Base;
    ptr_t Data_Size;
    ptr_t Exact;
    ptr_t Reg_Base;
    ptr_t State_Base;
    ptr_t Stack_Base;
    bool M4;
    bool Dpfpu;
    bool Has_Cop;
    bool Fast_Path;
    bool User_App;
    bool Bench_App;
    std::string App_Dir;
    std::string Board_Dir;
    std::string Include_Dir;
    std::string Src_Dir;
    std::string Script_Dir;
    std::string Root;
    std::string Vm;
    std::string Base_Config;
    std::string App_Source_List;
    std::vector<std::string> App_Files;
    std::unique_ptr<std::vector<std::string>> List;
    std::unique_ptr<std::vector<std::string>> Base;
    std::unique_ptr<std::vector<std::string>> Output;

    Code=nullptr;
    Data=nullptr;
    if(Virt->Memory_Code.empty()==false)
        Code=Virt->Memory_Code[0];
    if(Virt->Memory_Data.empty()==false)
        Data=Virt->Memory_Data[0];
    if((Code==nullptr)||(Data==nullptr))
        Main::Error("NTX generator:\nThe Virt Code1/Data1 blocks are missing.");

    Code_Base=Code->Base;
    Code_Size=Code->Size;
    Data_Base=Data->Base;
    Data_Size=Data->Size;
    Exact=Data_Size-Virt->Reg_Size-Virt->State_Size-
          Virt->User_Stack_Size-Virt->Vector_Stack_Size;
    if(Exact<=0x800U)
        Main::Error("NTX generator:\nThe Virt data window is too small.");
    Reg_Base=Data_Base+Exact;
    State_Base=Reg_Base+Virt->Reg_Size;
    Stack_Base=Data_Base+Exact+Virt->Reg_Size+Virt->State_Size;

    M4=false;
    Dpfpu=false;
    Has_Cop=false;
    for(const std::string& Cop:Virt->Coprocessor)
    {
        if(Cop=="None")
            continue;
        Has_Cop=true;
        if(Cop=="FPV4_SP")
            M4=true;
        if(Cop=="FPV5_DP")
            Dpfpu=true;
    }

    Fast_Path=true;
    {
        const char* Fast_Path_Env=std::getenv("NTX_FAST_PATH");
        if((Fast_Path_Env!=nullptr)&&(std::string(Fast_Path_Env)=="0"))
            Fast_Path=false;
    }

    Root=Root_Get();
    if((Root.empty()==false)&&(Root.back()!='/'))
        Root+="/";
    Vm=Virt->Name_Lower;
    Board_Dir=std::filesystem::absolute(
        Virt->Project_Output+"ntx_board").generic_string();
    Include_Dir=Board_Dir+"/include";
    Src_Dir=Board_Dir+"/src";
    Script_Dir=Board_Dir+"/scripts";
    std::filesystem::create_directories(Board_Dir+"/configs/virt");
    std::filesystem::create_directories(Include_Dir);
    std::filesystem::create_directories(Src_Dir);
    std::filesystem::create_directories(Script_Dir);

    App_Dir=std::filesystem::absolute(
        Virt->Main_Source_Output+"app").generic_string();
    std::filesystem::create_directories(App_Dir);

    /* The application folder wins over the benchmark, matching every other
     * guest.  The generated smoke app is not a user app. */
    for(const std::filesystem::directory_entry& Entry:
        std::filesystem::directory_iterator(App_Dir))
    {
        if((Entry.is_regular_file()==false)||
           (Entry.path().extension()!=".c"))
            continue;
        if(Entry.path().filename()=="ntx_app.c")
        {
            bool Placeholder=false;
            auto Lines=Proj_Gen::Line_Read(Entry.path().string());
            for(const std::string& Line:*Lines)
            {
                if(Line.find("Generated by NTX_Gen")!=std::string::npos)
                {
                    Placeholder=true;
                    break;
                }
            }
            if(Placeholder)
                continue;
        }
        App_Files.push_back(Entry.path().filename().string());
    }
    std::sort(App_Files.begin(),App_Files.end());
    User_App=(App_Files.empty()==false);
    Bench_App=(Main::Benchmark!=0)&&(User_App==false);

    /* defconfig: start from the in-tree reference board and replace every
     * per-Virt item, so the port defaults stay in one place. */
    Main::Info("> Generating '"+std::string("ntx_board/configs/virt/defconfig")+"'.");
    Base_Config=Root+"boards/arm/armv7-m-rvm/ntx/configs/"+
                (M4?"virt-m4":"virt")+"/defconfig";
    Base=Proj_Gen::Line_Read(Base_Config);
    if(Base==nullptr)
        Main::Error("NTX generator:\nCannot read "+Base_Config+".");
    Output=std::make_unique<std::vector<std::string>>();
    for(const std::string& Line:*Base)
    {
        if((Line.rfind("CONFIG_ARCH_CORTEXM",0)==0)||
           (Line.rfind("CONFIG_ARMV7M_RVM_CORTEXM",0)==0)||
           (Line.rfind("CONFIG_ARMV7M_RVM_FAST_PATH",0)==0)||
           (Line.rfind("CONFIG_ARCH_DPFPU",0)==0)||
           (Line.rfind("CONFIG_ARCH_FPU",0)==0)||
           (Line.rfind("CONFIG_NTX_RVM_",0)==0)||
           (Line.rfind("CONFIG_RAM_START",0)==0)||
           (Line.rfind("CONFIG_RAM_SIZE",0)==0))
            continue;
        Output->push_back(Line);
    }
    Output->push_back(std::string("CONFIG_ARCH_CORTEXM")+(M4?"4=y":"7=y"));
    Output->push_back(std::string("CONFIG_ARMV7M_RVM_CORTEXM")+(M4?"4=y":"7=y"));
    if(Has_Cop!=false)
    {
        Output->push_back("CONFIG_ARCH_FPU=y");
        if(Dpfpu!=false)
            Output->push_back("CONFIG_ARCH_DPFPU=y");
    }
    Output->push_back(std::string("CONFIG_ARMV7M_RVM_FAST_PATH=")+
                      (Fast_Path?"y":"n"));
    Output->push_back(std::string("CONFIG_NTX_RVM_DATA_BASE=0x")+Main::Hex(Data_Base));
    Output->push_back(std::string("CONFIG_NTX_RVM_DATA_SIZE=0x")+Main::Hex(Exact));
    Output->push_back(std::string("CONFIG_NTX_RVM_STACK_BASE=0x")+Main::Hex(Stack_Base));
    Output->push_back(std::string("CONFIG_NTX_RVM_STACK_SIZE=0x")+Main::Hex(Virt->User_Stack_Size));
    Output->push_back(std::string("CONFIG_NTX_RVM_DESC_BASE=0x")+Main::Hex(Code_Base));
    Output->push_back(std::string("CONFIG_NTX_RVM_CODE_BASE=0x")+Main::Hex(Code_Base+0x20U));
    Output->push_back(std::string("CONFIG_NTX_RVM_CODE_SIZE=0x")+Main::Hex(Code_Size-0x20U));
    Output->push_back(std::string("CONFIG_NTX_RVM_STATE_BASE=0x")+Main::Hex(State_Base));
    Output->push_back(std::string("CONFIG_NTX_RVM_STATE_SIZE=0x")+Main::Hex(Virt->State_Size));
    Output->push_back(std::string("CONFIG_NTX_RVM_REG_BASE=0x")+Main::Hex(Reg_Base));
    Output->push_back(std::string("CONFIG_NTX_RVM_REG_SIZE=0x")+Main::Hex(Virt->Reg_Size));
    if(Has_Cop!=false)
    {
        for(const std::string& Cop:Virt->Coprocessor)
        {
            if(Cop!="None")
                Output->push_back("CONFIG_NTX_RVM_COP_"+Cop+"=y");
        }
    }
    else
        Output->push_back("CONFIG_NTX_RVM_COP_NONE=y");
    {
        std::string Rvm_Root=Main::Monitor_Root;
        if((Rvm_Root.empty()==false)&&(Rvm_Root.back()!='/'))
            Rvm_Root+="/";
        Output->push_back("CONFIG_NTX_RVM_ROOT=\""+Rvm_Root+"\"");
    }
    Output->push_back(std::string("CONFIG_RAM_START=0x")+Main::Hex(Data_Base));
    Output->push_back(std::string("CONFIG_RAM_SIZE=")+std::to_string(Exact));
    Output->push_back("");
    Proj_Gen::Line_Write(Output, Board_Dir+"/configs/virt/defconfig");

    /* Guest-side RVM parameters, including the Kfunc IDs that the
     * benchmark chip header uses.  The in-tree board uses the same layout. */
    Main::Info("> Generating 'ntx_board/include/prc_virt.h'.");
    List=std::make_unique<std::vector<std::string>>();
    List->push_back("/* Generated by NTX_Gen: RVM per-VM parameters. */");
    List->push_back("#ifndef NTX_PRC_VIRT_H");
    List->push_back("#define NTX_PRC_VIRT_H");
    List->push_back("");
    List->push_back("#define RVM_VIRT_LIB_ENABLE 1U");
    List->push_back(std::string("#define RVM_VIRT_VCT_NUM ")+
                    std::to_string(Virt->Vector_Num)+"U");
    List->push_back("#define RVM_PGT_RAW_ENABLE 0U");
    List->push_back(std::string("#define RVM_VIRT_STATE_BASE 0x")+
                    Main::Hex(State_Base)+"U");
    List->push_back(std::string("#define RVM_VIRT_STATE_SIZE 0x")+
                    Main::Hex(Virt->State_Size)+"U");
    List->push_back(std::string("#define RVM_VIRT_REG_BASE 0x")+
                    Main::Hex(Reg_Base)+"U");
    List->push_back(std::string("#define RVM_VIRT_REG_SIZE 0x")+
                    Main::Hex(Virt->Reg_Size)+"U");
    List->push_back("");
    List->push_back("#define RVM_ASSERT_ENABLE 1U");
    List->push_back("#define RVM_DBGLOG_ENABLE 1U");
    List->push_back("");
    if(Has_Cop==false)
        List->push_back("#define RVM_COP_NUM 0U");
    else
    {
        List->push_back("#define RVM_COP_NUM 1U");
        List->push_back(std::string("#define RVM_A7M_COP_FPV4_SP ")+
                        (M4?"1U":"0U"));
        List->push_back(std::string("#define RVM_A7M_COP_FPV5_SP ")+
                        ((Has_Cop && !M4 && !Dpfpu)?"1U":"0U"));
        List->push_back(std::string("#define RVM_A7M_COP_FPV5_DP ")+
                        (Dpfpu?"1U":"0U"));
    }
    List->push_back("");
    List->push_back("/* Kernel functions */");
    for(const std::unique_ptr<class Kfunc>& Kfn:Virt->Kfunc)
        Proj_Gen::Macro_Int(List, Kfn->Macro_Local, Kfn->Cid_Local, MACRO_ADD);
    List->push_back("");
    List->push_back("#endif /* NTX_PRC_VIRT_H */");
    List->push_back("");
    Proj_Gen::Line_Write(List, Include_Dir+"/prc_virt.h");

    List=std::make_unique<std::vector<std::string>>();
    List->push_back("/* Generated by NTX_Gen: platform header selection. */");
    List->push_back("#include \"Platform/A7M/rvm_platform_a7m.h\"");
    List->push_back("");
    Proj_Gen::Line_Write(List, Include_Dir+"/rvm_platform.h");

    List=std::make_unique<std::vector<std::string>>();
    List->push_back("/* Generated by NTX_Gen: RVM platform configuration. */");
    List->push_back("#include \"prc_virt.h\"");
    List->push_back("");
    Proj_Gen::Line_Write(List, Include_Dir+"/rvm_platform_a7m_conf.h");

    List=std::make_unique<std::vector<std::string>>();
    List->push_back("/* Generated by NTX_Gen: generic RVM board. */");
    List->push_back("#ifndef __BOARD_H");
    List->push_back("#define __BOARD_H");
    List->push_back("");
    List->push_back("#include <nuttx/config.h>");
    List->push_back("");
    List->push_back("#endif /* __BOARD_H */");
    List->push_back("");
    Proj_Gen::Line_Write(List, Include_Dir+"/board.h");

    /* Benchmark-only chip selection.  A normal image must not keep a stale
     * header, otherwise the benchmark peripherals would leak into it. */
    if(Bench_App!=false)
    {
        Main::Info("> Generating 'ntx_board/include/ntx_test.h'.");
        List=std::make_unique<std::vector<std::string>>();
        List->push_back("/* Generated by NTX_Gen: benchmark chip selection. */");
        List->push_back("#ifndef NTX_TEST_H");
        List->push_back("#define NTX_TEST_H");
        List->push_back(std::string("#include \"ntx_test_")+
                        Chip->Name_Lower+"_rvm.h\"");
        List->push_back("#endif /* NTX_TEST_H */");
        List->push_back("");
        Proj_Gen::Line_Write(List, Include_Dir+"/ntx_test.h");
    }
    else
        std::filesystem::remove(Include_Dir+"/ntx_test.h");

    /* Linker script: keep the in-tree template authoritative. */
    Main::Info("> Generating 'ntx_board/scripts/ntx.ld'.");
    List=Proj_Gen::Line_Read(Root+"boards/arm/armv7-m-rvm/ntx/scripts/ntx.ld.in");
    if(List==nullptr)
        Main::Error("NTX generator:\nCannot read the ntx.ld.in template.");
    for(std::string& Line:*List)
    {
        auto Replace=[&Line](const std::string& Key,const std::string& Value)
        {
            size_t Pos=Line.find(Key);
            if(Pos!=std::string::npos)
                Line.replace(Pos,Key.size(),Value);
        };
        Replace("@CONFIG_NTX_RVM_DATA_BASE@","0x"+Main::Hex(Data_Base));
        Replace("@CONFIG_NTX_RVM_DATA_SIZE@","0x"+Main::Hex(Exact));
        Replace("@CONFIG_NTX_RVM_STACK_BASE@","0x"+Main::Hex(Stack_Base));
        Replace("@CONFIG_NTX_RVM_STACK_SIZE@","0x"+Main::Hex(Virt->User_Stack_Size));
        Replace("@CONFIG_NTX_RVM_DESC_BASE@","0x"+Main::Hex(Code_Base));
        Replace("@CONFIG_NTX_RVM_CODE_BASE@","0x"+Main::Hex(Code_Base+0x20U));
        Replace("@CONFIG_NTX_RVM_CODE_SIZE@","0x"+Main::Hex(Code_Size-0x20U));
    }
    Proj_Gen::Line_Write(List, Script_Dir+"/ntx.ld");

    /* Application source list.  The framework already wrote the descriptor
     * and Thd_Vct into the Virt source folder; only the app varies. */
    if(Bench_App!=false)
    {
        App_Source_List="\""+Root+"boards/arm/armv7-m-rvm/ntx/test/ntx_benchmark.c\"";
    }
    else if(User_App!=false)
    {
        for(size_t Index=0U;Index<App_Files.size();++Index)
        {
            if(Index!=0U)
                App_Source_List+=" ";
            App_Source_List+="\""+App_Dir+"/"+App_Files[Index]+"\"";
        }
    }
    else
    {
        Main::Info("> Generating NTX smoke-test application.");
        List=std::make_unique<std::vector<std::string>>();
        List->push_back("/* Generated by NTX_Gen: NTX smoke test. */");
        List->push_back("#include <nuttx/config.h>");
        List->push_back("#include <nuttx/arch.h>");
        List->push_back("#include <unistd.h>");
        List->push_back("");
        List->push_back("int ntx_main(int argc, char* argv[])");
        List->push_back("{");
        List->push_back("    (void)argc;");
        List->push_back("    (void)argv;");
        List->push_back("    for(;;)");
        List->push_back("    {");
        List->push_back("        sleep(1);");
        List->push_back("        up_putc('.');");
        List->push_back("    }");
        List->push_back("}");
        List->push_back("");
        Proj_Gen::Line_Write(List, App_Dir+"/ntx_app.c");
        App_Source_List="\""+App_Dir+"/ntx_app.c\"";
    }

    /* Board source list and linker property. */
    Main::Info("> Generating 'ntx_board/src/CMakeLists.txt'.");
    List=std::make_unique<std::vector<std::string>>();
    List->push_back("# Generated by NTX_Gen: NuttX guest sources for "+Vm+".");
    List->push_back("set(SRCS");
    List->push_back("  \""+std::filesystem::absolute(Virt->Main_Source_Output).generic_string()+
                    "/prc_"+Vm+".c\"");
    List->push_back("  \""+std::filesystem::absolute(Virt->Main_Source_Output).generic_string()+
                    "/prc_"+Vm+"_desc.c\"");
    List->push_back("  "+App_Source_List+")");
    List->push_back("target_sources(board PRIVATE ${SRCS})");
    if(Bench_App!=false)
        List->push_back("target_include_directories(board PRIVATE \""+Root+
                        "boards/arm/armv7-m-rvm/ntx/test/chip\")");
    List->push_back("set_property(GLOBAL PROPERTY LD_SCRIPT \""+Script_Dir+"/ntx.ld\")");
    List->push_back("");
    Proj_Gen::Line_Write(List, Src_Dir+"/CMakeLists.txt");

    List=std::make_unique<std::vector<std::string>>();
    List->push_back("# Generated by NTX_Gen: board for "+Vm+".");
    List->push_back("add_subdirectory(src)");
    List->push_back("");
    Proj_Gen::Line_Write(List, Board_Dir+"/CMakeLists.txt");
}
/* End Function:NTX_Gen::Process_Virt_Hdr ************************************/

/* Function:NTX_Gen::Process_Virt_Src *****************************************
Description : Generate the Virt makefile that drives the NuttX CMake build.
Input       : class Virtual* Virt - The process to generate for.
              class Tool_Gen* Tool - The toolchain to use.
Output      : None.
Return      : None.
******************************************************************************/
void NTX_Gen::Process_Virt_Src(class Virtual* Virt, class Tool_Gen* Tool)
{
    std::string Root;
    std::string Vm;
    std::string App_Dir;
    std::string Board_Config;
    std::string Build_Dir;
    std::unique_ptr<std::vector<std::string>> List;

    (void) Tool;

    Root=Root_Get();
    if((Root.empty()==false)&&(Root.back()!='/'))
        Root+="/";
    Vm=Virt->Name_Lower;
    App_Dir=std::filesystem::absolute(
        Virt->Main_Source_Output+"app").generic_string();
    Board_Config=std::filesystem::absolute(
        Virt->Project_Output+"ntx_board/configs/virt").generic_string();
    Build_Dir="$(CURDIR)/build";

    Main::Info("> Generating NTX "+Vm+" CMake makefile.");
    List=std::make_unique<std::vector<std::string>>();
    List->push_back("# Generated by NTX_Gen: the "+Vm+" makefile drives the NuttX CMake build.");
    List->push_back("TARGET := "+Vm);
    List->push_back("OBJDIR := Object");
    List->push_back("NTX_ROOT := "+Root);
    List->push_back("BOARD_CONFIG := "+Board_Config);
    List->push_back("BUILD_DIR := "+Build_Dir);
    List->push_back("APP_DIR := "+App_Dir);
    List->push_back("CMAKE ?= cmake");
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
    List->push_back("export PATH := $(if $(strip $(PREFIX)),$(PREFIX)/bin:)$(PATH)");
    List->push_back("ifneq ($(strip $(WIN)),)");
    List->push_back("  export USERPROFILE := $(shell cygpath -w \"$(HOME)\" 2>/dev/null || echo \"$(HOME)\")");
    List->push_back("  export TEMP := C:/Temp");
    List->push_back("  export TMP := C:/Temp");
    List->push_back("  export TMPDIR := C:/Temp");
    List->push_back("endif");
    List->push_back("");
    List->push_back(".PHONY: all clean ntx_force");
    List->push_back("all: "+Vm+"_image.c");
    List->push_back("");
    List->push_back("$(OBJDIR):");
    List->push_back("\tmkdir -p $(OBJDIR)");
    List->push_back("");
    List->push_back("# Every Virt has its own build directory; regenerate the config so a");
    List->push_back("# previous VM layout can never leak into this image.");
    List->push_back("$(OBJDIR)/"+Vm+".bin: ntx_force | $(OBJDIR)");
    List->push_back("\trm -rf $(BUILD_DIR)");
    List->push_back("\t$(CMAKE) -S $(NTX_ROOT) -B $(BUILD_DIR) -GNinja -DBOARD_CONFIG=$(BOARD_CONFIG)");
    List->push_back("\t$(CMAKE) --build $(BUILD_DIR) -j");
    List->push_back("\tcp $(BUILD_DIR)/nuttx.bin $@");
    List->push_back("");
    List->push_back(Vm+"_image.c: $(OBJDIR)/"+Vm+".bin");
    List->push_back("\t$(BINCOPY) -i $< -o $@");
    List->push_back("");
    List->push_back("ntx_force:");
    List->push_back("");
    List->push_back("clean:");
    List->push_back("\trm -rf $(OBJDIR) $(BUILD_DIR) "+Vm+"_image.c");
    List->push_back("");
    Proj_Gen::Line_Write(List, Virt->Project_Output+"prc_"+Vm);
}
/* End Function:NTX_Gen::Process_Virt_Src ************************************/

/* Function:NTX_Gen::Process_Linker *******************************************
Description : Skip linker script generation; the generated NuttX board owns it.
Input       : std::unique_ptr<std::vector<std::string>>& List - The lines.
              const class Process* Prc - The process to generate for.
              class Tool_Gen* Tool - The toolchain generator.
Output      : None.
Return      : None.
******************************************************************************/
void NTX_Gen::Process_Linker(std::unique_ptr<std::vector<std::string>>& List,
                             const class Process* Prc, class Tool_Gen* Tool)
{
    (void)List;
    (void)Tool;
    Main::Info("> NTX: using the generated board linker script, skipping linker generation.");
    std::filesystem::remove(Prc->Linker_Output+Prc->Linker_Filename);
}
/* End Function:NTX_Gen::Process_Linker **************************************/
}
/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
