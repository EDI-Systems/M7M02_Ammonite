/******************************************************************************
Filename    : keil_gen.cpp
Author      : pry
Date        : 12/07/2019
Licence     : LGPL v3+; see COPYING for details.
Description : This toolset is for Keil uVision build system.
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

#define __HDR_DEF__
#include "rvm_gen.hpp"
#include "Proj_Gen/proj_gen.hpp"
#include "Proj_Gen/Build_Gen/build_gen.hpp"
#include "Proj_Gen/Build_Gen/Keil_Gen/keil_gen.hpp"
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
#include "Proj_Gen/Build_Gen/Keil_Gen/keil_gen.hpp"
#undef __HDR_CLASS__
/* End Include ***************************************************************/
namespace RVM_GEN
{
/* Function:Keil_Gen::Keil_Gen ************************************************
Description : Generator for the Keil uVision.
Input       : class Proj_Info* Proj - The project information.
              class Plat_Info* Plat - The platform information.
              class Chip_Info* Chip - The chip information.
Output      : None.
Return      : None.
******************************************************************************/
/* void */ Keil_Gen::Keil_Gen(class Proj_Info* Proj, class Plat_Info* Plat, class Chip_Info* Chip):
Build_Gen("Keil", Proj, Plat, Chip)
{
    try
    {

    }
    catch(std::exception& Exc)
    {
        Main::Error(std::string("Keil uVision generator:\n")+Exc.what());
    }
}
/* End Function:Keil_Gen::Keil_Gen *******************************************/

/* Function:Keil_Gen::Suffix **************************************************
Description : Returns suffix for a given type of file.
Input       : ptr_t Type - The file type.
Output      : None.
Return      : std::string - The file suffix.
******************************************************************************/
std::string Keil_Gen::Suffix(ptr_t Type)
{
    if(Type==BUILD_PROJECT)
        return ".uvprojx";
    else if(Type==BUILD_WORKSPACE)
        return ".uvmpw";
    else
        Main::Error("XXXXX: File type not recognized.");
}
/* End Function:Keil_Gen::Suffix *********************************************/

/* Function:Keil_Gen::Keil_Proj ***********************************************
Description : Generate the keil project for ARMv7-M.
Input       : std::unique_ptr<std::vector<std::string>>& List - The file.
              const std::string& After1 - The first command to run after compilation.
              const std::string& After2 - The second command to run after compilation.
              const std::string& Target - The target name.
              ptr_t Optimization - The optimization level chosen.
              const std::vector<std::string>& Coprocessor - The coprocessor options.
              const std::vector<std::string>& Include - The include paths.
              const std::vector<std::string>& Source - The source file paths.
              const std::vector<std::string>& Library - The library file list.
              const std::string& Linker - The linker script (scatter file) path.
              const std::string& Linker_Misc - The linker miscellaneous control string.
Output      : std::unique_ptr<std::vector<std::string>>& List - The updated file.
Return      : None.
******************************************************************************/
void Keil_Gen::Raw_Proj(std::unique_ptr<std::vector<std::string>>& List,
                        const std::string& After1, const std::string& After2,
                        const std::string& Target, const std::string& Optimization,
						const std::vector<std::string>& Coprocessor,
                        const std::vector<std::string>& Include,
                        const std::vector<std::string>& Source,
                        const std::vector<std::string>& Library,
                        const std::string& Linker, const std::string& Linker_Misc)
{
    ptr_t Opt_Level;
    std::string CPU_Type;
    std::string FPU_Type;
    std::string Endian;
    std::string Dlloption;
    std::string Device;
    std::string Include_Path;

    /* CPU Type */
    CPU_Type=this->Chip->Attribute["CPU"];
    if(CPU_Type=="CM0")
    {
        CPU_Type="Cortex-M0";
        Dlloption="-pCM0";
    }
    else if(CPU_Type=="CM0P")
    {
        CPU_Type="Cortex-M0+";
        Dlloption="-pCM0+";
    }
    else if(CPU_Type=="CM1")
    {
        CPU_Type="Cortex-M1";
        Dlloption="-pCM1";
    }
    else if(CPU_Type=="CM3")
    {
        CPU_Type="Cortex-M3";
        Dlloption="-pCM3";
    }
    else if(CPU_Type=="CM4")
    {
        CPU_Type="Cortex-M4";
        Dlloption="-pCM4";
    }
    else if(CPU_Type=="CM7")
    {
        CPU_Type="Cortex-M7";
        Dlloption="-pCM7";
    }
    else
        Main::Error("XXXXX: Internal processor type error.");

    /* FPU Type - only one should exist for Keil uVision. We may change the logic later */
    if(Coprocessor.empty())
    	FPU_Type="";
    else if(Coprocessor.size()==1)
    {
		FPU_Type=Coprocessor[0];
		if(FPU_Type=="NONE")
			FPU_Type="";
		else if(FPU_Type=="FPV4_SP")
			FPU_Type="FPU2";
		else if(FPU_Type=="FPV5_SP")
			FPU_Type="FPU3(SFPU)";
		else if(FPU_Type=="FPV5_DP")
			FPU_Type="FPU3(DFPU)";
		else
			Main::Error("XXXXX: Internal FPU type error.");
    }
    else
		Main::Error("XXXXX: Internal FPU type error.");

    /* Endianness */
    Endian=this->Chip->Attribute["Endian"];
    if(Endian=="Little")
        Endian="ELITTLE";
    else if(Endian=="Big")
        Endian="EBIG";
    else
        Main::Error("XXXXX: Internal endianness error.");

    /* Decide device */
    if(this->Chip->Name.find("STM32")!=std::string::npos)
    {
        /* Except for STM32F1, all STM32s end with suffix, and the last number is substituted with 'x' */
        if(this->Chip->Name.find("STM32F1")!=std::string::npos)
            Device=this->Chip->Name;
        else
        {
            Device=this->Proj->Chip->Name;
            Device.back()='x';
        }
    }
    else
        Device=this->Proj->Chip->Name;

    /* Decide optimization level */
    if(Optimization=="O0")
        Opt_Level=1;
    else if(Optimization=="O1")
        Opt_Level=2;
    else if(Optimization=="O2")
        Opt_Level=3;
    else if(Optimization=="O3")
        Opt_Level=4;
    else if(Optimization=="Of")
        Opt_Level=5;
    else if(Optimization=="Os")
        Opt_Level=7;
    else
        Main::Error("XXXXX: Internal optimization level error.");

    /* Construct include paths */
    for(const std::string& Inc:Include)
        Include_Path+=Inc+";";

    List->push_back("<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\" ?>");
    List->push_back("<Project xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xsi:noNamespaceSchemaLocation=\"project_projx.xsd\">");
    List->push_back("  <SchemaVersion>2.1</SchemaVersion>");
    List->push_back("  <Header>### uVision Project, (C) RVM Project Generator</Header>");
    List->push_back("  <Targets>");
    List->push_back("    <Target>");
    List->push_back(std::string("      <TargetName>")+Target+"</TargetName>");
    List->push_back("      <ToolsetNumber>0x4</ToolsetNumber>");
    List->push_back("      <ToolsetName>ARM-ADS</ToolsetName>");
    List->push_back("      <pCCUsed>ARMCC</pCCUsed>");
    /* We're using ARMCC 6 by default from now on */;
    List->push_back("      <uAC6>1</uAC6>");
    List->push_back("      <TargetOption>");
    List->push_back("        <TargetCommonOption>");
    List->push_back(std::string("          <Device>")+Device+"</Device>");
    List->push_back(std::string("          <Vendor>")+this->Chip->Vendor+"</Vendor>");
    List->push_back(std::string("          <Cpu>IRAM(0x08000000,0x10000) IROM(0x20000000,0x10000) CPUTYPE(\"")+
                    CPU_Type+"\") "+FPU_Type+" CLOCK(12000000) "+Endian+"</Cpu>");
    List->push_back("          <OutputDirectory>.\\Object\\</OutputDirectory>");
    List->push_back(std::string("          <OutputName>")+Target+"</OutputName>");
    List->push_back("          <CreateExecutable>1</CreateExecutable>");
    List->push_back("          <CreateHexFile>1</CreateHexFile>");
    List->push_back("          <DebugInformation>1</DebugInformation>");
    List->push_back("          <BrowseInformation>1</BrowseInformation>");
    List->push_back("          <ListingPath>.\\Listing\\</ListingPath>");
    List->push_back("          <HexFormatSelection>1</HexFormatSelection>");
    List->push_back("          <AfterMake>");
    List->push_back(std::string("            <RunUserProg1>")+std::to_string((int)((After1=="")?0:1))+"</RunUserProg1>");
    List->push_back(std::string("            <RunUserProg2>")+std::to_string((int)((After2=="")?0:1))+"</RunUserProg2>");
    List->push_back(std::string("            <UserProg1Name>")+After1+"</UserProg1Name>");
    List->push_back(std::string("            <UserProg2Name>")+After2+"</UserProg2Name>");
    List->push_back("            <UserProg1Dos16Mode>0</UserProg1Dos16Mode>");
    List->push_back("            <UserProg2Dos16Mode>0</UserProg2Dos16Mode>");
    List->push_back("            <nStopA1X>0</nStopA1X>");
    List->push_back("            <nStopA2X>0</nStopA2X>");
    List->push_back("          </AfterMake>");
    List->push_back("        </TargetCommonOption>");
    List->push_back("        <CommonProperty>");
    List->push_back("          <UseCPPCompiler>0</UseCPPCompiler>");
    List->push_back("          <RVCTCodeConst>0</RVCTCodeConst>");
    List->push_back("          <RVCTZI>0</RVCTZI>");
    List->push_back("          <RVCTOtherData>0</RVCTOtherData>");
    List->push_back("          <ModuleSelection>0</ModuleSelection>");
    List->push_back("          <IncludeInBuild>1</IncludeInBuild>");
    List->push_back("          <AlwaysBuild>0</AlwaysBuild>");
    List->push_back("          <GenerateAssemblyFile>0</GenerateAssemblyFile>");
    List->push_back("          <AssembleAssemblyFile>0</AssembleAssemblyFile>");
    List->push_back("          <PublicsOnly>0</PublicsOnly>");
    List->push_back("          <StopOnExitCode>3</StopOnExitCode>");
    List->push_back("          <CustomArgument></CustomArgument>");
    List->push_back("          <IncludeLibraryModules></IncludeLibraryModules>");
    List->push_back("          <ComprImg>1</ComprImg>");
    List->push_back("        </CommonProperty>");
    List->push_back("        <DllOption>");
    List->push_back("          <SimDllName>SARMCM3.DLL</SimDllName>");
    List->push_back("          <SimDllArguments> -REMAP -MPU</SimDllArguments>");
    List->push_back("          <SimDlgDll>DCM.DLL</SimDlgDll>");
    List->push_back(std::string("          <SimDlgDllArguments>")+Dlloption+"</SimDlgDllArguments>");
    List->push_back("          <TargetDllName>SARMCM3.DLL</TargetDllName>");
    List->push_back("          <TargetDllArguments> -MPU</TargetDllArguments>");
    List->push_back("          <TargetDlgDll>TCM.DLL</TargetDlgDll>");
    List->push_back(std::string("          <TargetDlgDllArguments>")+Dlloption+"</TargetDlgDllArguments>");
    List->push_back("        </DllOption>");
    List->push_back("        <TargetArmAds>");
    List->push_back("          <ArmAdsMisc>");
    List->push_back("            <useUlib>1</useUlib>");
    List->push_back("            <OptFeed>0</OptFeed>");
    List->push_back("          </ArmAdsMisc>");
    List->push_back("          <Cads>");
    List->push_back("            <interw>1</interw>");
    List->push_back(std::string("            <Optim>")+std::to_string(Opt_Level)+"</Optim>");
    List->push_back(std::string("            <oTime>")+std::to_string((int)(Opt_Level==4))+"</oTime>");
    List->push_back("            <SplitLS>0</SplitLS>");
    List->push_back("            <OneElfS>1</OneElfS>");
    List->push_back("            <Strict>0</Strict>");
    List->push_back("            <EnumInt>1</EnumInt>");
    List->push_back("            <PlainCh>1</PlainCh>");
    List->push_back("            <Ropi>0</Ropi>");
    List->push_back("            <Rwpi>0</Rwpi>");
    List->push_back("            <wLevel>2</wLevel>");
    List->push_back("            <uThumb>0</uThumb>");
    List->push_back("            <uSurpInc>0</uSurpInc>");
    List->push_back("            <uC99>1</uC99>");
    List->push_back("            <uGnu>1</uGnu>");
    List->push_back("            <useXO>0</useXO>");
    List->push_back("            <v6Lang>3</v6Lang>");
    List->push_back("            <v6LangP>1</v6LangP>");
    List->push_back("            <vShortEn>1</vShortEn>");
    List->push_back("            <vShortWch>1</vShortWch>");
    List->push_back("            <v6Lto>0</v6Lto>");
    List->push_back("            <v6WtE>0</v6WtE>");
    List->push_back("            <v6Rtti>0</v6Rtti>");
    List->push_back("            <VariousControls>");
    /* These warnings are spurious */
    List->push_back("              <MiscControls>-Wno-unknown-warning-option -Wno-unsafe-buffer-usage -Wno-date-time -Wno-unused-parameter "
                                                "-Wno-missing-noreturn -Wno-unsafe-buffer-usage -Wno-missing-variable-declarations</MiscControls>");
    List->push_back("              <Define></Define>");
    List->push_back("              <Undefine></Undefine>");
    List->push_back(std::string("              <IncludePath>")+Include_Path+"</IncludePath>");
    List->push_back("            </VariousControls>");
    List->push_back("          </Cads>");
    List->push_back("          <Aads>");
    List->push_back("            <interw>1</interw>");
    List->push_back("            <Ropi>0</Ropi>");
    List->push_back("            <Rwpi>0</Rwpi>");
    List->push_back("            <thumb>0</thumb>");
    List->push_back("            <SplitLS>0</SplitLS>");
    List->push_back("            <SwStkChk>0</SwStkChk>");
    List->push_back("            <NoWarn>0</NoWarn>");
    List->push_back("            <uSurpInc>0</uSurpInc>");
    List->push_back("            <useXO>0</useXO>");
    List->push_back("            <uClangAs>0</uClangAs>");
    List->push_back("            <VariousControls>");
    List->push_back("              <MiscControls></MiscControls>");
    List->push_back("              <Define></Define>");
    List->push_back("              <Undefine></Undefine>");
    List->push_back(std::string("              <IncludePath>")+Include_Path+"</IncludePath>");
    List->push_back("            </VariousControls>");
    List->push_back("          </Aads>");
    List->push_back("          <LDads>");
    List->push_back("            <umfTarg>0</umfTarg>");
    List->push_back("            <Ropi>0</Ropi>");
    List->push_back("            <Rwpi>0</Rwpi>");
    List->push_back("            <noStLib>0</noStLib>");
    List->push_back("            <RepFail>1</RepFail>");
    List->push_back("            <useFile>0</useFile>");
    List->push_back("            <TextAddressRange>0x08000000</TextAddressRange>");
    List->push_back("            <DataAddressRange>0x20000000</DataAddressRange>");
    List->push_back("            <pXoBase></pXoBase>");
    List->push_back(std::string("            <ScatterFile>")+Linker+"</ScatterFile>");
    List->push_back("            <IncludeLibs></IncludeLibs>");
    List->push_back("            <IncludeLibsPath></IncludeLibsPath>");
    List->push_back(std::string("            <Misc>")+Linker_Misc+"</Misc>");
    List->push_back("            <LinkerInputFile></LinkerInputFile>");
    List->push_back("            <DisabledWarnings>6314,6329</DisabledWarnings>");
    List->push_back("          </LDads>");
    List->push_back("        </TargetArmAds>");
    List->push_back("      </TargetOption>");
    List->push_back("      <Groups>");
    /* Project group containing all stubs or images */
    List->push_back("        <Group>");
    List->push_back(std::string("          <GroupName>")+Target+"</GroupName>");
    List->push_back("          <Files>");
    for(const std::string& Src:Source)
    {
        List->push_back("            <File>");
        /* File display name, and real path */
        if(Src.find_last_of("/")==std::string::npos)
            List->push_back(std::string("              <FileName>")+Src+"</FileName>");
        else
            List->push_back(std::string("              <FileName>")+Src.substr(Src.find_last_of("/")+1)+"</FileName>");
        /* File type - C or assembly */
        if(Src.back()=='c')
            List->push_back("              <FileType>1</FileType>");
        else
            List->push_back("              <FileType>2</FileType>");
        List->push_back(std::string("              <FilePath>")+Src+"</FilePath>");
        List->push_back("            </File>");
    }
    List->push_back("          </Files>");
    List->push_back("        </Group>");
    /* Library group containing all fixed files */
    List->push_back("        <Group>");
    List->push_back("          <GroupName>Lib</GroupName>");
    List->push_back("          <Files>");
    for(const std::string& Src:Library)
    {
        List->push_back("            <File>");
        /* File display name, and real path */
        if(Src.find_last_of("/")==std::string::npos)
            List->push_back(std::string("              <FileName>")+Src+"</FileName>");
        else
            List->push_back(std::string("              <FileName>")+Src.substr(Src.find_last_of("/")+1)+"</FileName>");
        /* File type - C or assembly */
        if(Src.back()=='c')
            List->push_back("              <FileType>1</FileType>");
        else
            List->push_back("              <FileType>2</FileType>");
        List->push_back(std::string("              <FilePath>")+Src+"</FilePath>");
        List->push_back("            </File>");
    }
    List->push_back("          </Files>");
    List->push_back("        </Group>");
    /* User group for user additions */
    List->push_back("        <Group>");
    List->push_back("          <GroupName>User</GroupName>");
    List->push_back("        </Group>");
    List->push_back("      </Groups>");
    List->push_back("    </Target>");
    List->push_back("  </Targets>");
    List->push_back("</Project>");
}
/* End Function:Keil_Gen::Keil_Proj ******************************************/

/* Function:Keil_Gen::Kernel_Proj *********************************************
Description : Generate kernel project.
Input       : std::unique_ptr<std::vector<std::string>>& List - The file.
              const std::vector<std::string>& Include - The include file list.
              const std::vector<std::string>& Source - The source file list.
              const std::vector<std::string>& Library - The library file list.
              const std::vector<std::string>& Linker - The linker script file list.
Output      : std::unique_ptr<std::vector<std::string>>& List - The updated file.
Output      : None.
Return      : None.
******************************************************************************/
void Keil_Gen::Kernel_Proj(std::unique_ptr<std::vector<std::string>>& List,
                           const std::vector<std::string>& Include,
                           const std::vector<std::string>& Source,
                           const std::vector<std::string>& Library,
                           const std::vector<std::string>& Linker)
{
	std::vector<std::string> None;

    this->Raw_Proj(List,
                   "",                                      /* After 1 */
                   "",                                      /* After 2 */
                   "Kernel",                                /* Target */
                   this->Proj->Kernel->Optimization,        /* Optimization */
				   None,                                    /* Coprocessor */
                   Include,                                 /* Include */
                   Source,                                  /* Source */
                   Library,                                 /* Library */
                   Linker[0],                               /* Linker */
                   "--keep=*_image");                       /* Linker_Misc */
}
/* End Function:Keil_Gen::Kernel_Proj ****************************************/

/* Function:Keil_Gen::Monitor_Proj ********************************************
Description : Generate monitor project.
Input       : std::unique_ptr<std::vector<std::string>>& List - The file.
              const std::vector<std::string>& Include - The include file list.
              const std::vector<std::string>& Source - The source file list.
              const std::vector<std::string>& Library - The library file list.
              const std::vector<std::string>& Linker - The linker script file list.
Output      : std::unique_ptr<std::vector<std::string>>& List - The updated file.
Output      : None.
Return      : None.
******************************************************************************/
void Keil_Gen::Monitor_Proj(std::unique_ptr<std::vector<std::string>>& List,
                            const std::vector<std::string>& Include,
                            const std::vector<std::string>& Source,
                            const std::vector<std::string>& Library,
                            const std::vector<std::string>& Linker)
{
    std::string After1;
    std::string After2;
    std::string Bincopy;
	std::vector<std::string> None;

    if(this->Proj->Kernel->Full_Image!=0)
    {
        Bincopy=Main::Path_Absolute(PATH_FILE,Main::Monitor_Root,"Utility/M7M02_Final/bincopy.exe");
        Bincopy=Main::Path_Relative(PATH_FILE,this->Proj->Monitor->Project_Output,Bincopy);
        After1="fromelf.exe --binCombined -o \"$L@L.bin\" \"#L\"";
        After2=Bincopy+" -i $L@L.bin -o ./monitor_image.c";
    }

    this->Raw_Proj(List,
                   After1,                                  /* After 1 */
                   After2,                                  /* After 2 */
                   "Monitor",                               /* Target */
                   this->Proj->Monitor->Optimization,       /* Optimization */
				   None,                                    /* Coprocessor */
                   Include,                                 /* Include */
                   Source,                                  /* Source */
                   Library,                                 /* Library */
                   Linker[0],                               /* Linker */
                   "");                                     /* Linker_Misc */
}
/* End Function:Keil_Gen::Monitor_Proj ***************************************/

/* Function:Keil_Gen::Process_Proj ********************************************
Description : Generate process project.
Input       : std::unique_ptr<std::vector<std::string>>& List - The file.
              const std::vector<std::string>& Include - The include file list.
              const std::vector<std::string>& Source - The source file list.
              const std::vector<std::string>& Library - The library file list.
              const std::vector<std::string>& Linker - The linker script file list.
              const class Process* Prc - The process to generate for.
Output      : std::unique_ptr<std::vector<std::string>>& List - The updated file.
Output      : None.
Return      : None.
******************************************************************************/
void Keil_Gen::Process_Proj(std::unique_ptr<std::vector<std::string>>& List,
                            const std::vector<std::string>& Include,
                            const std::vector<std::string>& Source,
                            const std::vector<std::string>& Library,
                            const std::vector<std::string>& Linker,
                            const class Process* Prc)
{
    std::string After1;
    std::string After2;
    std::string Bincopy;

    if(this->Proj->Kernel->Full_Image!=0)
    {
        Bincopy=Main::Path_Absolute(PATH_FILE,Main::Monitor_Root,"Utility/M7M02_Final/bincopy.exe");
        Bincopy=Main::Path_Relative(PATH_FILE,Prc->Project_Output,Bincopy);
        After1="fromelf.exe --binCombined -o \"$L@L.bin\" \"#L\"";
        After2=Bincopy+" -i $L@L.bin -o ./"+Prc->Name_Lower+"_image.c";
    }

    this->Raw_Proj(List,
                   After1,                                  /* After 1 */
                   After2,                                  /* After 2 */
                   Prc->Name,                              	/* Target */
                   Prc->Optimization,                      	/* Optimization */
				   Prc->Coprocessor,                        /* Coprocessor */
                   Include,                                 /* Include */
                   Source,                                  /* Source */
                   Library,                                 /* Library */
                   Linker[0],                               /* Linker */
                   "--keep=*_desc");                        /* Linker_Misc */
}
/* End Function:Keil_Gen::Process_Proj ***************************************/

/* Function:Keil_Gen::Workspace_Sect ******************************************
Description : Generate a workspace project section.
Input       : std::unique_ptr<std::vector<std::string>>& List - The file.
              const std::vector<std::string>& Project - The project list.
Output      : std::unique_ptr<std::vector<std::string>>& List - The updated file.
Output      : None.
Return      : None.
******************************************************************************/
void Keil_Gen::Workspace_Sect(std::unique_ptr<std::vector<std::string>>& List,
                              const std::vector<std::string>& Project)
{
    for(const std::string& Proj:Project)
    {
        List->push_back("  <project>");
        List->push_back(std::string("    <PathAndName>")+Proj+"</PathAndName>");
        List->push_back("    <NodeIsExpanded>1</NodeIsExpanded>");
        List->push_back("    <NodeIsCheckedInBatchBuild>1</NodeIsCheckedInBatchBuild>");
        List->push_back("  </project>");
        List->push_back("");
    }
}
/* End Function:Keil_Gen::Workspace_Sect *************************************/

/* Function:Keil_Gen::Workspace_Proj ******************************************
Description : Generate workspace project.
Input       : std::unique_ptr<std::vector<std::string>>& List - The file.
              const std::vector<std::string>& Kernel - The kernel project.
              const std::vector<std::string>& Monitor - The monitor project.
              const std::vector<std::string>& Native - The native process project.
              const std::vector<std::string>& Virtual - The virtual machine project.
Output      : std::unique_ptr<std::vector<std::string>>& List - The updated file.
Output      : None.
Return      : None.
******************************************************************************/
void Keil_Gen::Workspace_Proj(std::unique_ptr<std::vector<std::string>>& List,
                              const std::vector<std::string>& Kernel,
                              const std::vector<std::string>& Monitor,
                              const std::vector<std::string>& Native,
                              const std::vector<std::string>& Virtual)
{
    List->push_back("<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\" ?>");
    List->push_back("<ProjectWorkspace xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xsi:noNamespaceSchemaLocation=\"project_mpw.xsd\">");
    List->push_back("");
    List->push_back("  <SchemaVersion>1.0</SchemaVersion>");
    List->push_back("");
    List->push_back("  <Header>### uVision Project, (C) RVM Project Generator</Header>");
    List->push_back("");
    List->push_back("  <WorkspaceName>WorkSpace</WorkspaceName>");
    List->push_back("");
    this->Workspace_Sect(List, Virtual);
    this->Workspace_Sect(List, Native);
    this->Workspace_Sect(List, Monitor);
    this->Workspace_Sect(List, Kernel);
    List->push_back("");
    List->push_back("</ProjectWorkspace>");
    List->push_back("");
}
/* End Function:Keil_Gen::Workspace_Proj *************************************/
}
/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
