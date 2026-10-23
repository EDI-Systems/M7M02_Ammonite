/******************************************************************************
Filename    : iarew_gen.cpp
Author      : pry ; Cline
Date        : 21/10/2026
Licence     : LGPL v3+; see COPYING for details.
Description : This toolset is for IAR Embedded Workbench. The project file
              layout mirrors the verified M5P01 G071 template (IAR 9.2:
              General v36 / ICCARM v38 / AARM v12 / ILINK v27).
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
#include "Proj_Gen/Build_Gen/IAREW_Gen/iarew_gen.hpp"
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
#include "Proj_Gen/Build_Gen/IAREW_Gen/iarew_gen.hpp"
#undef __HDR_CLASS__
/* End Include ***************************************************************/
namespace RVM_GEN
{
/* Function:IAREW_Gen::IAREW_Gen **********************************************
Description : Generator for the IAR Embedded Workbench.
Input       : class Proj_Info* Proj - The project information.
              class Plat_Info* Plat - The platform information.
              class Chip_Info* Chip - The chip information.
Output      : None.
Return      : None.
******************************************************************************/
/* void */ IAREW_Gen::IAREW_Gen(class Proj_Info* Proj, class Plat_Info* Plat, class Chip_Info* Chip):
Build_Gen("IAREW", Proj, Plat, Chip)
{
    try
    {

    }
    catch(std::exception& Exc)
    {
        Main::Error(std::string("IAREW generator:\n")+Exc.what());
    }
}
/* End Function:IAREW_Gen::IAREW_Gen *****************************************/

/* Function:IAREW_Gen::Suffix **************************************************
Description : Returns suffix for a given type of file.
Input       : ptr_t Type - The file type.
Output      : None.
Return      : std::string - The file suffix.
******************************************************************************/
std::string IAREW_Gen::Suffix(ptr_t Type)
{
    if(Type==BUILD_PROJECT)
        return ".ewp";
    else if(Type==BUILD_WORKSPACE)
        return ".eww";
    else
        Main::Error("XXXXX: File type not recognized.");
}
/* End Function:IAREW_Gen::Suffix ********************************************/

/* Function:IAREW_Gen::Raw_Proj ************************************************
Description : Generate the IAR project for ARMv6-M.
Input       : std::unique_ptr<std::vector<std::string>>& List - The file.
              const std::string& Postbuild - The command to run after build.
              const std::string& Target - The target name.
              const std::string& Optimization - The optimization level chosen.
              const std::vector<std::string>& Coprocessor - The coprocessor options.
              const std::vector<std::string>& Include - The include paths.
              const std::vector<std::string>& Source - The source file paths.
              const std::vector<std::string>& Library - The library file list.
              const std::string& Linker - The linker script (.icf) path.
Output      : std::unique_ptr<std::vector<std::string>>& List - The updated file.
Return      : None.
******************************************************************************/
void IAREW_Gen::Raw_Proj(std::unique_ptr<std::vector<std::string>>& List,
                         const std::string& Postbuild,
                         const std::string& Target, const std::string& Optimization,
                         const std::vector<std::string>& Coprocessor,
                         const std::vector<std::string>& Include,
                         const std::vector<std::string>& Source,
                         const std::vector<std::string>& Library,
                         const std::string& Linker)
{
    ptr_t Opt_Level;
    ptr_t Opt_Strategy;
    ptr_t Core_State;
    std::string CPU_Type;
    std::string Device;
    std::string Path;

    /* CPU Type - IAR CoreVariant states (version 33 context) */
    CPU_Type=this->Chip->Attribute["CPU"];
    if(CPU_Type=="CM0")
        Core_State=34;
    else if(CPU_Type=="CM0P")
        Core_State=35;
    else if(CPU_Type=="CM1")
        Core_State=34;
    else if(CPU_Type=="CM3")
        Core_State=36;
    else if(CPU_Type=="CM4")
        Core_State=39;
    else if(CPU_Type=="CM7")
        Core_State=41;
    else
        Main::Error("XXXXX: Internal processor type error.");

    /* FPU Type - IAR FPU2/NrRegs/DSPExtension values verified against the
     * M5P01 native EWARM projects: F429(FPV4_SP)=FPU2 4, H743(FPV5_DP)=FPU2 7 */
    ptr_t FPU2_State;
    ptr_t NrRegs_State;
    ptr_t DSP_State;
    if(Coprocessor.empty()!=0)
    {
        FPU2_State=0;
        NrRegs_State=0;
        DSP_State=0;
    }
    else if((Coprocessor.size()==1)&&(Coprocessor[0]=="NONE"))
    {
        FPU2_State=0;
        NrRegs_State=0;
        DSP_State=0;
    }
    else if((Coprocessor.size()==1)&&(Coprocessor[0]=="FPV4_SP"))
    {
        FPU2_State=4;
        NrRegs_State=1;
        DSP_State=1;
    }
    else if((Coprocessor.size()==1)&&(Coprocessor[0]=="FPV5_SP"))
    {
        FPU2_State=6;
        NrRegs_State=1;
        DSP_State=1;
    }
    else if((Coprocessor.size()==1)&&(Coprocessor[0]=="FPV5_DP"))
    {
        FPU2_State=7;
        NrRegs_State=1;
        DSP_State=1;
    }
    else
        Main::Error("XXXXX: Internal FPU type error.");

    /* Decide device - the .rvc name is exactly what IAR expects */
    Device=this->Chip->Name+"\t"+this->Chip->Vendor+" "+this->Chip->Name;

    /* Decide optimization level */
    if(Optimization=="O0")
    {
        Opt_Level=0;
        Opt_Strategy=0;
    }
    else if(Optimization=="O1")
    {
        Opt_Level=1;
        Opt_Strategy=0;
    }
    else if(Optimization=="O2")
    {
        Opt_Level=2;
        Opt_Strategy=0;
    }
    else if(Optimization=="O3")
    {
        Opt_Level=3;
        Opt_Strategy=3;
    }
    else if(Optimization=="Of")
    {
        Opt_Level=3;
        Opt_Strategy=0;
    }
    else if(Optimization=="Os")
    {
        Opt_Level=3;
        Opt_Strategy=2;
    }
    else
        Main::Error("XXXXX: Internal optimization level error.");

    List->push_back("<?xml version=\"1.0\" encoding=\"UTF-8\"?>");
    List->push_back("<project>");
    List->push_back("    <fileVersion>4</fileVersion>");
    List->push_back("    <configuration>");
    List->push_back("        <name>Debug</name>");
    List->push_back("        <toolchain>");
    List->push_back("            <name>ARM</name>");
    List->push_back("        </toolchain>");
    List->push_back("        <debug>1</debug>");
    List->push_back("        <settings>");
    List->push_back("            <name>General</name>");
    List->push_back("            <archiveVersion>3</archiveVersion>");
    List->push_back("            <data>");
    List->push_back("                <version>36</version>");
    List->push_back("                <wantNonLocal>1</wantNonLocal>");
    List->push_back("                <debug>1</debug>");
    List->push_back("                <option>");
    List->push_back("                    <name>BrowseInfoPath</name>");
    List->push_back("                    <state>Debug\\BrowseInfo</state>");
    List->push_back("                </option>");
    List->push_back("                <option>");
    List->push_back("                    <name>ExePath</name>");
    List->push_back("                    <state>Debug\\Output</state>");
    List->push_back("                </option>");
    List->push_back("                <option>");
    List->push_back("                    <name>ObjPath</name>");
    List->push_back("                    <state>Debug\\Output</state>");
    List->push_back("                </option>");
    List->push_back("                <option>");
    List->push_back("                    <name>ListPath</name>");
    List->push_back("                    <state>Debug\\Listings</state>");
    List->push_back("                </option>");
    List->push_back("                <option>");
    List->push_back("                    <name>GEndianMode</name>");
    List->push_back("                    <state>0</state>");
    List->push_back("                </option>");
    List->push_back("                <option>");
    List->push_back("                    <name>Input description</name>");
    List->push_back("                    <state>Automatic choice of formatter, without multibyte support.</state>");
    List->push_back("                </option>");
    List->push_back("                <option>");
    List->push_back("                    <name>Output description</name>");
    List->push_back("                    <state>Automatic choice of formatter, without multibyte support.</state>");
    List->push_back("                </option>");
    List->push_back("                <option>");
    List->push_back("                    <name>GOutputBinary</name>");
    List->push_back("                    <state>0</state>");
    List->push_back("                </option>");
    List->push_back("                <option>");
    List->push_back("                    <name>OGCoreOrChip</name>");
    List->push_back("                    <state>1</state>");
    List->push_back("                </option>");
    List->push_back("                <option>");
    List->push_back("                    <name>GRuntimeLibSelect</name>");
    List->push_back("                    <version>0</version>");
    List->push_back("                    <state>1</state>");
    List->push_back("                </option>");
    List->push_back("                <option>");
    List->push_back("                    <name>GRuntimeLibSelectSlave</name>");
    List->push_back("                    <version>0</version>");
    List->push_back("                    <state>1</state>");
    List->push_back("                </option>");
    List->push_back("                <option>");
    List->push_back("                    <name>RTDescription</name>");
    List->push_back("                    <state>A compact configuration of the C/C++14 runtime library. No locale interface, C locale, no file descriptor support, no multibytes in printf and scanf, and no hex floats in strtod.</state>");
    List->push_back("                </option>");
    List->push_back("                <option>");
    List->push_back("                    <name>OGProductVersion</name>");
    List->push_back("                    <state>9.20.2.10770</state>");
    List->push_back("                </option>");
    List->push_back("                <option>");
    List->push_back("                    <name>OGLastSavedByProductVersion</name>");
    List->push_back("                    <state>9.40.1.63870</state>");
    List->push_back("                </option>");
    List->push_back("                <option>");
    List->push_back("                    <name>OGChipSelectEditMenu</name>");
    List->push_back("                    <state>"+Device+"</state>");
    List->push_back("                </option>");
    List->push_back("                <option>");
    List->push_back("                    <name>GenLowLevelInterface</name>");
    List->push_back("                    <state>0</state>");
    List->push_back("                </option>");
    List->push_back("                <option>");
    List->push_back("                    <name>GEndianModeBE</name>");
    List->push_back("                    <state>1</state>");
    List->push_back("                </option>");
    List->push_back("                <option>");
    List->push_back("                    <name>OGBufferedTerminalOutput</name>");
    List->push_back("                    <state>0</state>");
    List->push_back("                </option>");
    List->push_back("                <option>");
    List->push_back("                    <name>GenStdoutInterface</name>");
    List->push_back("                    <state>0</state>");
    List->push_back("                </option>");
    List->push_back("                <option>");
    List->push_back("                    <name>RTConfigPath2</name>");
    List->push_back("                    <state>$TOOLKIT_DIR$\\inc\\c\\DLib_Config_Normal.h</state>");
    List->push_back("                </option>");
    List->push_back("                <option>");
    List->push_back("                    <name>GBECoreSlave</name>");
    List->push_back("                    <version>33</version>");
    List->push_back("                    <state>"+std::to_string(Core_State)+"</state>");
    List->push_back("                </option>");
    List->push_back("                <option>");
    List->push_back("                    <name>OGUseCmsis</name>");
    List->push_back("                    <state>0</state>");
    List->push_back("                </option>");
    List->push_back("                <option>");
    List->push_back("                    <name>OGUseCmsisDspLib</name>");
    List->push_back("                    <state>0</state>");
    List->push_back("                </option>");
    List->push_back("                <option>");
    List->push_back("                    <name>GRuntimeLibThreads</name>");
    List->push_back("                    <state>0</state>");
    List->push_back("                </option>");
    List->push_back("                <option>");
    List->push_back("                    <name>CoreVariant</name>");
    List->push_back("                    <version>33</version>");
    List->push_back("                    <state>"+std::to_string(Core_State)+"</state>");
    List->push_back("                </option>");
    List->push_back("                <option>");
    List->push_back("                    <name>GFPUDeviceSlave</name>");
    List->push_back("                    <state>"+Device+"</state>");
    List->push_back("                </option>");
    List->push_back("                <option>");
    List->push_back("                    <name>FPU2</name>");
    List->push_back("                    <version>0</version>");
    List->push_back("                    <state>"+std::to_string(FPU2_State)+"</state>");
    List->push_back("                </option>");
    List->push_back("                <option>");
    List->push_back("                    <name>NrRegs</name>");
    List->push_back("                    <version>0</version>");
    List->push_back("                    <state>"+std::to_string(NrRegs_State)+"</state>");
    List->push_back("                </option>");
    List->push_back("                <option>");
    List->push_back("                    <name>NEON</name>");
    List->push_back("                    <state>0</state>");
    List->push_back("                </option>");
    List->push_back("                <option>");
    List->push_back("                    <name>GFPUCoreSlave2</name>");
    List->push_back("                    <version>33</version>");
    List->push_back("                    <state>"+std::to_string(Core_State)+"</state>");
    List->push_back("                </option>");
    List->push_back("                <option>");
    List->push_back("                    <name>OGCMSISPackSelectDevice</name>");
    List->push_back("                </option>");
    List->push_back("                <option>");
    List->push_back("                    <name>OgLibHeap</name>");
    List->push_back("                    <state>0</state>");
    List->push_back("                </option>");
    List->push_back("                <option>");
    List->push_back("                    <name>OGLibAdditionalLocale</name>");
    List->push_back("                    <state>0</state>");
    List->push_back("                </option>");
    List->push_back("                <option>");
    List->push_back("                    <name>OGPrintfVariant</name>");
    List->push_back("                    <version>0</version>");
    List->push_back("                    <state>0</state>");
    List->push_back("                </option>");
    List->push_back("                <option>");
    List->push_back("                    <name>OGPrintfMultibyteSupport</name>");
    List->push_back("                    <state>0</state>");
    List->push_back("                </option>");
    List->push_back("                <option>");
    List->push_back("                    <name>OGScanfVariant</name>");
    List->push_back("                    <version>0</version>");
    List->push_back("                    <state>0</state>");
    List->push_back("                </option>");
    List->push_back("                <option>");
    List->push_back("                    <name>OGScanfMultibyteSupport</name>");
    List->push_back("                    <state>0</state>");
    List->push_back("                </option>");
    List->push_back("                <option>");
    List->push_back("                    <name>GenLocaleTags</name>");
    List->push_back("                    <state></state>");
    List->push_back("                </option>");
    List->push_back("                <option>");
    List->push_back("                    <name>GenLocaleDisplayOnly</name>");
    List->push_back("                    <state></state>");
    List->push_back("                </option>");
    List->push_back("                <option>");
    List->push_back("                    <name>DSPExtension</name>");
    List->push_back("                    <state>"+std::to_string(DSP_State)+"</state>");
    List->push_back("                </option>");
    List->push_back("                <option>");
    List->push_back("                    <name>TrustZone</name>");
    List->push_back("                    <state>0</state>");
    List->push_back("                </option>");
    List->push_back("                <option>");
    List->push_back("                    <name>TrustZoneModes</name>");
    List->push_back("                    <version>0</version>");
    List->push_back("                    <state>0</state>");
    List->push_back("                </option>");
    List->push_back("                <option>");
    List->push_back("                    <name>OGAarch64Abi</name>");
    List->push_back("                    <state>0</state>");
    List->push_back("                </option>");
    List->push_back("                <option>");
    List->push_back("                    <name>OG_32_64Device</name>");
    List->push_back("                    <state>0</state>");
    List->push_back("                </option>");
    List->push_back("                <option>");
    List->push_back("                    <name>BuildFilesPath</name>");
    List->push_back("                    <state>Debug</state>");
    List->push_back("                </option>");
    List->push_back("                <option>");
    List->push_back("                    <name>PointerAuthentication</name>");
    List->push_back("                    <state>0</state>");
    List->push_back("                </option>");
    List->push_back("                <option>");
    List->push_back("                    <name>FPU64</name>");
    List->push_back("                    <state>1</state>");
    List->push_back("                </option>");
    List->push_back("                <option>");
    List->push_back("                    <name>OG_32_64DeviceCoreSlave</name>");
    List->push_back("                    <version>33</version>");
    List->push_back("                    <state>"+std::to_string(Core_State)+"</state>");
    List->push_back("                </option>");
    List->push_back("            </data>");
    List->push_back("        </settings>");
    /* ICCARM *******************************************************************/
    List->push_back("        <settings>");
    List->push_back("            <name>ICCARM</name>");
    List->push_back("            <archiveVersion>2</archiveVersion>");
    List->push_back("            <data>");
    List->push_back("                <version>38</version>");
    List->push_back("                <wantNonLocal>1</wantNonLocal>");
    List->push_back("                <debug>1</debug>");
    /* RVM is pure register level - no preprocessor defines needed */
    List->push_back("                <option>");
    List->push_back("                    <name>CCDefines</name>");
    List->push_back("                </option>");
    List->push_back("                <option>");
    List->push_back("                    <name>CCPreprocFile</name>");
    List->push_back("                    <state>0</state>");
    List->push_back("                </option>");
    List->push_back("                <option>");
    List->push_back("                    <name>CCPreprocComments</name>");
    List->push_back("                    <state>0</state>");
    List->push_back("                </option>");
    List->push_back("                <option>");
    List->push_back("                    <name>CCPreprocLine</name>");
    List->push_back("                    <state>1</state>");
    List->push_back("                </option>");
    List->push_back("                <option>");
    List->push_back("                    <name>CCListCFile</name>");
    List->push_back("                    <state>0</state>");
    List->push_back("                </option>");
    List->push_back("                <option>");
    List->push_back("                    <name>CCListCMnemonics</name>");
    List->push_back("                    <state>0</state>");
    List->push_back("                </option>");
    List->push_back("                <option>");
    List->push_back("                    <name>CCListCMessages</name>");
    List->push_back("                    <state>0</state>");
    List->push_back("                </option>");
    List->push_back("                <option>");
    List->push_back("                    <name>CCListAssFile</name>");
    List->push_back("                    <state>0</state>");
    List->push_back("                </option>");
    List->push_back("                <option>");
    List->push_back("                    <name>CCListAssSource</name>");
    List->push_back("                    <state>0</state>");
    List->push_back("                </option>");
    List->push_back("                <option>");
    List->push_back("                    <name>CCEnableRemarks</name>");
    List->push_back("                    <state>0</state>");
    List->push_back("                </option>");
    /* Suppress only Pa082 (volatile multi-access ordering) - it is intrinsic
     * to the RMP guest kernel style (and likely many other kernels).
     * Pa078 (lvalue double-modification) must stay visible: its root cause
     * in the G071 clock init macro has been fixed at the source, so it can
     * never legitimately appear again. Pa084 stays visible by owner's call. */
    List->push_back("                <option>");
    List->push_back("                    <name>CCDiagSuppress</name>");
    List->push_back("                    <state>Pa082</state>");
    List->push_back("                </option>");
    List->push_back("                <option>");
    List->push_back("                    <name>CCDiagRemark</name>");
    List->push_back("                    <state></state>");
    List->push_back("                </option>");
    List->push_back("                <option>");
    List->push_back("                    <name>CCDiagWarning</name>");
    List->push_back("                    <state></state>");
    List->push_back("                </option>");
    List->push_back("                <option>");
    List->push_back("                    <name>CCDiagError</name>");
    List->push_back("                    <state></state>");
    List->push_back("                </option>");
    List->push_back("                <option>");
    List->push_back("                    <name>CCObjPrefix</name>");
    List->push_back("                    <state>1</state>");
    List->push_back("                </option>");
    List->push_back("                <option>");
    List->push_back("                    <name>CCAllowList</name>");
    List->push_back("                    <version>1</version>");
    List->push_back("                    <state>11111110</state>");
    List->push_back("                </option>");
    List->push_back("                <option>");
    List->push_back("                    <name>CCDebugInfo</name>");
    List->push_back("                    <state>1</state>");
    List->push_back("                </option>");
    List->push_back("                <option>");
    List->push_back("                    <name>IEndianMode</name>");
    List->push_back("                    <state>1</state>");
    List->push_back("                </option>");
    List->push_back("                <option>");
    List->push_back("                    <name>IProcessor</name>");
    List->push_back("                    <state>1</state>");
    List->push_back("                </option>");
    List->push_back("                <option>");
    List->push_back("                    <name>IExtraOptionsCheck</name>");
    List->push_back("                    <state>0</state>");
    List->push_back("                </option>");
    List->push_back("                <option>");
    List->push_back("                    <name>IExtraOptions</name>");
    List->push_back("                    <state></state>");
    List->push_back("                </option>");
    List->push_back("                <option>");
    List->push_back("                    <name>CCLangConformance</name>");
    List->push_back("                    <state>0</state>");
    List->push_back("                </option>");
    List->push_back("                <option>");
    List->push_back("                    <name>CCSignedPlainChar</name>");
    List->push_back("                    <state>0</state>");
    List->push_back("                </option>");
    List->push_back("                <option>");
    List->push_back("                    <name>CCRequirePrototypes</name>");
    List->push_back("                    <state>0</state>");
    List->push_back("                </option>");
    List->push_back("                <option>");
    List->push_back("                    <name>CCDiagWarnAreErr</name>");
    List->push_back("                    <state>0</state>");
    List->push_back("                </option>");
    List->push_back("                <option>");
    List->push_back("                    <name>CCCompilerRuntimeInfo</name>");
    List->push_back("                    <state>0</state>");
    List->push_back("                </option>");
    List->push_back("                <option>");
    List->push_back("                    <name>IFpuProcessor</name>");
    List->push_back("                    <state>1</state>");
    List->push_back("                </option>");
    List->push_back("                <option>");
    List->push_back("                    <name>OutputFile</name>");
    List->push_back("                    <state>$FILE_BNAME$.o</state>");
    List->push_back("                </option>");
    List->push_back("                <option>");
    List->push_back("                    <name>CCLibConfigHeader</name>");
    List->push_back("                    <state>1</state>");
    List->push_back("                </option>");
    List->push_back("                <option>");
    List->push_back("                    <name>PreInclude</name>");
    List->push_back("                    <state></state>");
    List->push_back("                </option>");
    List->push_back("                <option>");
    List->push_back("                    <name>CCIncludePath2</name>");
    for(const std::string& Inc:Include)
    {
        Path=Inc;
        std::replace(Path.begin(),Path.end(),'/','\\');
        List->push_back("                    <state>$PROJ_DIR$\\"+Path+"</state>");
    }
    List->push_back("                </option>");
    List->push_back("                <option>");
    List->push_back("                    <name>CCStdIncCheck</name>");
    List->push_back("                    <state>0</state>");
    List->push_back("                </option>");
    List->push_back("                <option>");
    List->push_back("                    <name>CCCodeSection</name>");
    List->push_back("                    <state>.text</state>");
    List->push_back("                </option>");
    List->push_back("                <option>");
    List->push_back("                    <name>IProcessorMode2</name>");
    List->push_back("                    <state>1</state>");
    List->push_back("                </option>");
    List->push_back("                <option>");
    List->push_back("                    <name>CCOptLevel</name>");
    List->push_back("                    <state>"+std::to_string(Opt_Level)+"</state>");
    List->push_back("                </option>");
    List->push_back("                <option>");
    List->push_back("                    <name>CCOptStrategy</name>");
    List->push_back("                    <version>0</version>");
    List->push_back("                    <state>"+std::to_string(Opt_Strategy)+"</state>");
    List->push_back("                </option>");
    List->push_back("                <option>");
    List->push_back("                    <name>CCOptLevelSlave</name>");
    List->push_back("                    <state>"+std::to_string(Opt_Level)+"</state>");
    List->push_back("                </option>");
    List->push_back("                <option>");
    List->push_back("                    <name>CCPosIndRopi</name>");
    List->push_back("                    <state>0</state>");
    List->push_back("                </option>");
    List->push_back("                <option>");
    List->push_back("                    <name>CCPosIndRwpi</name>");
    List->push_back("                    <state>0</state>");
    List->push_back("                </option>");
    List->push_back("                <option>");
    List->push_back("                    <name>CCPosIndNoDynInit</name>");
    List->push_back("                    <state>0</state>");
    List->push_back("                </option>");
    List->push_back("                <option>");
    List->push_back("                    <name>IccLang</name>");
    List->push_back("                    <state>0</state>");
    List->push_back("                </option>");
    List->push_back("                <option>");
    List->push_back("                    <name>IccCDialect</name>");
    List->push_back("                    <state>1</state>");
    List->push_back("                </option>");
    List->push_back("                <option>");
    List->push_back("                    <name>IccAllowVLA</name>");
    List->push_back("                    <state>0</state>");
    List->push_back("                </option>");
    List->push_back("                <option>");
    List->push_back("                    <name>IccStaticDestr</name>");
    List->push_back("                    <state>1</state>");
    List->push_back("                </option>");
    List->push_back("                <option>");
    List->push_back("                    <name>IccCppInlineSemantics</name>");
    List->push_back("                    <state>0</state>");
    List->push_back("                </option>");
    List->push_back("                <option>");
    List->push_back("                    <name>IccCmsis</name>");
    List->push_back("                    <state>1</state>");
    List->push_back("                </option>");
    List->push_back("                <option>");
    List->push_back("                    <name>IccFloatSemantics</name>");
    List->push_back("                    <state>0</state>");
    List->push_back("                </option>");
    List->push_back("                <option>");
    List->push_back("                    <name>CCOptimizationNoSizeConstraints</name>");
    List->push_back("                    <state>0</state>");
    List->push_back("                </option>");
    List->push_back("                <option>");
    List->push_back("                    <name>CCNoLiteralPool</name>");
    List->push_back("                    <state>0</state>");
    List->push_back("                </option>");
    List->push_back("                <option>");
    List->push_back("                    <name>CCOptStrategySlave</name>");
    List->push_back("                    <version>0</version>");
    List->push_back("                    <state>"+std::to_string(Opt_Strategy)+"</state>");
    List->push_back("                </option>");
    List->push_back("                <option>");
    List->push_back("                    <name>CCGuardCalls</name>");
    List->push_back("                    <state>1</state>");
    List->push_back("                </option>");
    List->push_back("                <option>");
    List->push_back("                    <name>CCEncSource</name>");
    List->push_back("                    <state>0</state>");
    List->push_back("                </option>");
    List->push_back("                <option>");
    List->push_back("                    <name>CCEncOutput</name>");
    List->push_back("                    <state>0</state>");
    List->push_back("                </option>");
    List->push_back("                <option>");
    List->push_back("                    <name>CCEncOutputBom</name>");
    List->push_back("                    <state>1</state>");
    List->push_back("                </option>");
    List->push_back("                <option>");
    List->push_back("                    <name>CCEncInput</name>");
    List->push_back("                    <state>0</state>");
    List->push_back("                </option>");
    List->push_back("                <option>");
    List->push_back("                    <name>IccExceptions2</name>");
    List->push_back("                    <state>0</state>");
    List->push_back("                </option>");
    List->push_back("                <option>");
    List->push_back("                    <name>IccRTTI2</name>");
    List->push_back("                    <state>0</state>");
    List->push_back("                </option>");
    List->push_back("                <option>");
    List->push_back("                    <name>OICompilerExtraOption</name>");
    List->push_back("                    <state>1</state>");
    List->push_back("                </option>");
    List->push_back("                <option>");
    List->push_back("                    <name>CCStackProtection</name>");
    List->push_back("                    <state>0</state>");
    List->push_back("                </option>");
    List->push_back("                <option>");
    List->push_back("                    <name>CCPointerAutentiction</name>");
    List->push_back("                    <state>0</state>");
    List->push_back("                </option>");
    List->push_back("                <option>");
    List->push_back("                    <name>CCBranchTargetIdentification</name>");
    List->push_back("                    <state>0</state>");
    List->push_back("                </option>");
    List->push_back("            </data>");
    List->push_back("        </settings>");
    /* AARM *********************************************************************/
    List->push_back("        <settings>");
    List->push_back("            <name>AARM</name>");
    List->push_back("            <archiveVersion>2</archiveVersion>");
    List->push_back("            <data>");
    List->push_back("                <version>12</version>");
    List->push_back("                <wantNonLocal>1</wantNonLocal>");
    List->push_back("                <debug>1</debug>");
    List->push_back("                <option>");
    List->push_back("                    <name>AObjPrefix</name>");
    List->push_back("                    <state>1</state>");
    List->push_back("                </option>");
    List->push_back("                <option>");
    List->push_back("                    <name>AEndian</name>");
    List->push_back("                    <state>1</state>");
    List->push_back("                </option>");
    List->push_back("                <option>");
    List->push_back("                    <name>ACaseSensitivity</name>");
    List->push_back("                    <state>1</state>");
    List->push_back("                </option>");
    List->push_back("                <option>");
    List->push_back("                    <name>MacroChars</name>");
    List->push_back("                    <version>0</version>");
    List->push_back("                    <state>0</state>");
    List->push_back("                </option>");
    /* The hand-assembled CODE-section data definitions trigger Warning 410 */
    List->push_back("                <option>");
    List->push_back("                    <name>AWarnEnable</name>");
    List->push_back("                    <state>1</state>");
    List->push_back("                </option>");
    List->push_back("                <option>");
    List->push_back("                    <name>AWarnWhat</name>");
    List->push_back("                    <state>1</state>");
    List->push_back("                </option>");
    List->push_back("                <option>");
    List->push_back("                    <name>AWarnOne</name>");
    List->push_back("                    <state>410</state>");
    List->push_back("                </option>");
    List->push_back("                <option>");
    List->push_back("                    <name>AWarnRange1</name>");
    List->push_back("                    <state></state>");
    List->push_back("                </option>");
    List->push_back("                <option>");
    List->push_back("                    <name>AWarnRange2</name>");
    List->push_back("                    <state></state>");
    List->push_back("                </option>");
    List->push_back("                <option>");
    List->push_back("                    <name>ADebug</name>");
    List->push_back("                    <state>1</state>");
    List->push_back("                </option>");
    List->push_back("                <option>");
    List->push_back("                    <name>AltRegisterNames</name>");
    List->push_back("                    <state>0</state>");
    List->push_back("                </option>");
    List->push_back("                <option>");
    List->push_back("                    <name>ADefines</name>");
    List->push_back("                    <state></state>");
    List->push_back("                </option>");
    List->push_back("                <option>");
    List->push_back("                    <name>AList</name>");
    List->push_back("                    <state>0</state>");
    List->push_back("                </option>");
    List->push_back("                <option>");
    List->push_back("                    <name>AListHeader</name>");
    List->push_back("                    <state>1</state>");
    List->push_back("                </option>");
    List->push_back("                <option>");
    List->push_back("                    <name>AListing</name>");
    List->push_back("                    <state>1</state>");
    List->push_back("                </option>");
    List->push_back("                <option>");
    List->push_back("                    <name>Includes</name>");
    List->push_back("                    <state>0</state>");
    List->push_back("                </option>");
    List->push_back("                <option>");
    List->push_back("                    <name>MacDefs</name>");
    List->push_back("                    <state>0</state>");
    List->push_back("                </option>");
    List->push_back("                <option>");
    List->push_back("                    <name>MacExps</name>");
    List->push_back("                    <state>1</state>");
    List->push_back("                </option>");
    List->push_back("                <option>");
    List->push_back("                    <name>MacExec</name>");
    List->push_back("                    <state>0</state>");
    List->push_back("                </option>");
    List->push_back("                <option>");
    List->push_back("                    <name>OnlyAssed</name>");
    List->push_back("                    <state>0</state>");
    List->push_back("                </option>");
    List->push_back("                <option>");
    List->push_back("                    <name>MultiLine</name>");
    List->push_back("                    <state>0</state>");
    List->push_back("                </option>");
    List->push_back("                <option>");
    List->push_back("                    <name>PageLengthCheck</name>");
    List->push_back("                    <state>0</state>");
    List->push_back("                </option>");
    List->push_back("                <option>");
    List->push_back("                    <name>PageLength</name>");
    List->push_back("                    <state>80</state>");
    List->push_back("                </option>");
    List->push_back("                <option>");
    List->push_back("                    <name>TabSpacing</name>");
    List->push_back("                    <state>8</state>");
    List->push_back("                </option>");
    List->push_back("                <option>");
    List->push_back("                    <name>AXRef</name>");
    List->push_back("                    <state>0</state>");
    List->push_back("                </option>");
    List->push_back("                <option>");
    List->push_back("                    <name>AXRefDefines</name>");
    List->push_back("                    <state>0</state>");
    List->push_back("                </option>");
    List->push_back("                <option>");
    List->push_back("                    <name>AXRefInternal</name>");
    List->push_back("                    <state>0</state>");
    List->push_back("                </option>");
    List->push_back("                <option>");
    List->push_back("                    <name>AXRefDual</name>");
    List->push_back("                    <state>0</state>");
    List->push_back("                </option>");
    List->push_back("                <option>");
    List->push_back("                    <name>AProcessor</name>");
    List->push_back("                    <state>1</state>");
    List->push_back("                </option>");
    List->push_back("                <option>");
    List->push_back("                    <name>AFpuProcessor</name>");
    List->push_back("                    <state>1</state>");
    List->push_back("                </option>");
    List->push_back("                <option>");
    List->push_back("                    <name>AOutputFile</name>");
    List->push_back("                    <state>$FILE_BNAME$.o</state>");
    List->push_back("                </option>");
    List->push_back("                <option>");
    List->push_back("                    <name>ALimitErrorsCheck</name>");
    List->push_back("                    <state>0</state>");
    List->push_back("                </option>");
    List->push_back("                <option>");
    List->push_back("                    <name>ALimitErrorsEdit</name>");
    List->push_back("                    <state>100</state>");
    List->push_back("                </option>");
    List->push_back("                <option>");
    List->push_back("                    <name>AIgnoreStdInclude</name>");
    List->push_back("                    <state>0</state>");
    List->push_back("                </option>");
    List->push_back("                <option>");
    List->push_back("                    <name>AUserIncludes</name>");
    List->push_back("                    <state></state>");
    List->push_back("                </option>");
    List->push_back("                <option>");
    List->push_back("                    <name>AExtraOptionsCheckV2</name>");
    List->push_back("                    <state>0</state>");
    List->push_back("                </option>");
    /* Note - the iasmarm -w switch may only appear once, never add it here */
    List->push_back("                <option>");
    List->push_back("                    <name>AExtraOptionsV2</name>");
    List->push_back("                    <state></state>");
    List->push_back("                </option>");
    List->push_back("                <option>");
    List->push_back("                    <name>AsmNoLiteralPool</name>");
    List->push_back("                    <state>0</state>");
    List->push_back("                </option>");
    List->push_back("                <option>");
    List->push_back("                    <name>PreInclude</name>");
    List->push_back("                    <state></state>");
    List->push_back("                </option>");
    List->push_back("                <option>");
    List->push_back("                    <name>A_32_64Device</name>");
    List->push_back("                    <state>1</state>");
    List->push_back("                </option>");
    List->push_back("            </data>");
    List->push_back("        </settings>");
    /* OBJCOPY ******************************************************************/
    List->push_back("        <settings>");
    List->push_back("            <name>OBJCOPY</name>");
    List->push_back("            <archiveVersion>0</archiveVersion>");
    List->push_back("            <data>");
    List->push_back("                <version>1</version>");
    List->push_back("                <wantNonLocal>1</wantNonLocal>");
    List->push_back("                <debug>1</debug>");
    List->push_back("                <option>");
    List->push_back("                    <name>OOCOutputFormat</name>");
    List->push_back("                    <version>3</version>");
    List->push_back("                    <state>0</state>");
    List->push_back("                </option>");
    List->push_back("                <option>");
    List->push_back("                    <name>OCOutputOverride</name>");
    List->push_back("                    <state>0</state>");
    List->push_back("                </option>");
    List->push_back("                <option>");
    List->push_back("                    <name>OOCOutputFile</name>");
    List->push_back("                    <state>"+Target+".srec</state>");
    List->push_back("                </option>");
    List->push_back("                <option>");
    List->push_back("                    <name>OOCCommandLineProducer</name>");
    List->push_back("                    <state>1</state>");
    List->push_back("                </option>");
    List->push_back("                <option>");
    List->push_back("                    <name>OOCObjCopyEnable</name>");
    List->push_back("                    <state>0</state>");
    List->push_back("                </option>");
    List->push_back("            </data>");
    List->push_back("        </settings>");
    /* CUSTOM *******************************************************************/
    List->push_back("        <settings>");
    List->push_back("            <name>CUSTOM</name>");
    List->push_back("            <archiveVersion>3</archiveVersion>");
    List->push_back("            <data>");
    List->push_back("                <extensions></extensions>");
    List->push_back("                <cmdline></cmdline>");
    List->push_back("                <hasPrio>1</hasPrio>");
    List->push_back("                <buildSequence>inputOutputBased</buildSequence>");
    List->push_back("            </data>");
    List->push_back("        </settings>");
    /* BUILDACTION - the post-build image conversion steps **********************/
    List->push_back("        <settings>");
    List->push_back("            <name>BUILDACTION</name>");
    List->push_back("            <archiveVersion>2</archiveVersion>");
    List->push_back("            <data>");
    List->push_back("                <prebuild></prebuild>");
    List->push_back("                <postbuild>"+Postbuild+"</postbuild>");
    List->push_back("            </data>");
    List->push_back("        </settings>");
    /* ILINK ********************************************************************/
    List->push_back("        <settings>");
    List->push_back("            <name>ILINK</name>");
    List->push_back("            <archiveVersion>0</archiveVersion>");
    List->push_back("            <data>");
    List->push_back("                <version>27</version>");
    List->push_back("                <wantNonLocal>1</wantNonLocal>");
    List->push_back("                <debug>1</debug>");
    List->push_back("                <option>");
    List->push_back("                    <name>CrcSize</name>");
    List->push_back("                    <version>0</version>");
    List->push_back("                    <state>1</state>");
    List->push_back("                </option>");
    List->push_back("                <option>");
    List->push_back("                    <name>CrcAlign</name>");
    List->push_back("                    <state>1</state>");
    List->push_back("                </option>");
    List->push_back("                <option>");
    List->push_back("                    <name>CrcPoly</name>");
    List->push_back("                    <state>0x11021</state>");
    List->push_back("                </option>");
    List->push_back("                <option>");
    List->push_back("                    <name>CrcCompl</name>");
    List->push_back("                    <version>0</version>");
    List->push_back("                    <state>0</state>");
    List->push_back("                </option>");
    List->push_back("                <option>");
    List->push_back("                    <name>CrcBitOrder</name>");
    List->push_back("                    <version>0</version>");
    List->push_back("                    <state>0</state>");
    List->push_back("                </option>");
    List->push_back("                <option>");
    List->push_back("                    <name>CrcInitialValue</name>");
    List->push_back("                    <state>0x0</state>");
    List->push_back("                </option>");
    List->push_back("                <option>");
    List->push_back("                    <name>DoCrc</name>");
    List->push_back("                    <state>0</state>");
    List->push_back("                </option>");
    List->push_back("                <option>");
    List->push_back("                    <name>IlinkBE8Slave</name>");
    List->push_back("                    <state>1</state>");
    List->push_back("                </option>");
    List->push_back("                <option>");
    List->push_back("                    <name>IlinkBufferedTerminalOutput</name>");
    List->push_back("                    <state>1</state>");
    List->push_back("                </option>");
    List->push_back("                <option>");
    List->push_back("                    <name>IlinkStdoutInterfaceSlave</name>");
    List->push_back("                    <state>1</state>");
    List->push_back("                </option>");
    List->push_back("                <option>");
    List->push_back("                    <name>CrcFullSize</name>");
    List->push_back("                    <state>0</state>");
    List->push_back("                </option>");
    List->push_back("                <option>");
    List->push_back("                    <name>IlinkIElfToolPostProcess</name>");
    List->push_back("                    <state>0</state>");
    List->push_back("                </option>");
    List->push_back("                <option>");
    List->push_back("                    <name>IlinkLogAutoLibSelect</name>");
    List->push_back("                    <state>0</state>");
    List->push_back("                </option>");
    List->push_back("                <option>");
    List->push_back("                    <name>IlinkLogRedirSymbols</name>");
    List->push_back("                    <state>0</state>");
    List->push_back("                </option>");
    List->push_back("                <option>");
    List->push_back("                    <name>IlinkLogUnusedFragments</name>");
    List->push_back("                    <state>0</state>");
    List->push_back("                </option>");
    List->push_back("                <option>");
    List->push_back("                    <name>IlinkCrcReverseByteOrder</name>");
    List->push_back("                    <state>0</state>");
    List->push_back("                </option>");
    List->push_back("                <option>");
    List->push_back("                    <name>IlinkCrcUseAsInput</name>");
    List->push_back("                    <state>1</state>");
    List->push_back("                </option>");
    List->push_back("                <option>");
    List->push_back("                    <name>IlinkOptInline</name>");
    List->push_back("                    <state>0</state>");
    List->push_back("                </option>");
    List->push_back("                <option>");
    List->push_back("                    <name>IlinkOptExceptionsAllow</name>");
    List->push_back("                    <state>1</state>");
    List->push_back("                </option>");
    List->push_back("                <option>");
    List->push_back("                    <name>IlinkOptExceptionsForce</name>");
    List->push_back("                    <state>0</state>");
    List->push_back("                </option>");
    List->push_back("                <option>");
    List->push_back("                    <name>IlinkCmsis</name>");
    List->push_back("                    <state>1</state>");
    List->push_back("                </option>");
    List->push_back("                <option>");
    List->push_back("                    <name>IlinkOptMergeDuplSections</name>");
    List->push_back("                    <state>0</state>");
    List->push_back("                </option>");
    List->push_back("                <option>");
    List->push_back("                    <name>IlinkOptUseVfe</name>");
    List->push_back("                    <state>1</state>");
    List->push_back("                </option>");
    List->push_back("                <option>");
    List->push_back("                    <name>IlinkOptForceVfe</name>");
    List->push_back("                    <state>0</state>");
    List->push_back("                </option>");
    List->push_back("                <option>");
    List->push_back("                    <name>IlinkStackAnalysisEnable</name>");
    List->push_back("                    <state>0</state>");
    List->push_back("                </option>");
    List->push_back("                <option>");
    List->push_back("                    <name>IlinkStackControlFile</name>");
    List->push_back("                    <state></state>");
    List->push_back("                </option>");
    List->push_back("                <option>");
    List->push_back("                    <name>IlinkStackCallGraphFile</name>");
    List->push_back("                    <state></state>");
    List->push_back("                </option>");
    List->push_back("                <option>");
    List->push_back("                    <name>CrcAlgorithm</name>");
    List->push_back("                    <version>1</version>");
    List->push_back("                    <state>1</state>");
    List->push_back("                </option>");
    List->push_back("                <option>");
    List->push_back("                    <name>CrcUnitSize</name>");
    List->push_back("                    <version>0</version>");
    List->push_back("                    <state>0</state>");
    List->push_back("                </option>");
    List->push_back("                <option>");
    List->push_back("                    <name>IlinkThreadsSlave</name>");
    List->push_back("                    <state>1</state>");
    List->push_back("                </option>");
    List->push_back("                <option>");
    List->push_back("                    <name>IlinkLogCallGraph</name>");
    List->push_back("                    <state>0</state>");
    List->push_back("                </option>");
    List->push_back("                <option>");
    List->push_back("                    <name>IlinkIcfFile_AltDefault</name>");
    List->push_back("                    <state></state>");
    List->push_back("                </option>");
    List->push_back("                <option>");
    List->push_back("                    <name>IlinkEncInput</name>");
    List->push_back("                    <state>0</state>");
    List->push_back("                </option>");
    List->push_back("                <option>");
    List->push_back("                    <name>IlinkEncOutput</name>");
    List->push_back("                    <state>0</state>");
    List->push_back("                </option>");
    List->push_back("                <option>");
    List->push_back("                    <name>IlinkEncOutputBom</name>");
    List->push_back("                    <state>1</state>");
    List->push_back("                </option>");
    List->push_back("                <option>");
    List->push_back("                    <name>IlinkHeapSelect</name>");
    List->push_back("                    <state>1</state>");
    List->push_back("                </option>");
    List->push_back("                <option>");
    List->push_back("                    <name>IlinkLocaleSelect</name>");
    List->push_back("                    <state>1</state>");
    List->push_back("                </option>");
    List->push_back("                <option>");
    List->push_back("                    <name>IlinkTrustzoneImportLibraryOut</name>");
    List->push_back("                    <state>"+Target+"_import_lib.o</state>");
    List->push_back("                </option>");
    List->push_back("                <option>");
    List->push_back("                    <name>OILinkExtraOption</name>");
    List->push_back("                    <state>1</state>");
    List->push_back("                </option>");
    List->push_back("                <option>");
    List->push_back("                    <name>IlinkLibIOConfig</name>");
    List->push_back("                    <state>1</state>");
    List->push_back("                </option>");
    List->push_back("                <option>");
    List->push_back("                    <name>IlinkInputFileSlave</name>");
    List->push_back("                    <state>0</state>");
    List->push_back("                </option>");
    List->push_back("                <option>");
    List->push_back("                    <name>IlinkOutputFile</name>");
    List->push_back("                    <state>"+Target+".out</state>");
    List->push_back("                </option>");
    List->push_back("                <option>");
    List->push_back("                    <name>IlinkDebugInfoEnable</name>");
    List->push_back("                    <state>1</state>");
    List->push_back("                </option>");
    List->push_back("                <option>");
    List->push_back("                    <name>IlinkKeepSymbols</name>");
    List->push_back("                    <state></state>");
    List->push_back("                </option>");
    List->push_back("                <option>");
    List->push_back("                    <name>IlinkRawBinaryFile</name>");
    List->push_back("                    <state></state>");
    List->push_back("                </option>");
    List->push_back("                <option>");
    List->push_back("                    <name>IlinkRawBinarySymbol</name>");
    List->push_back("                    <state></state>");
    List->push_back("                </option>");
    List->push_back("                <option>");
    List->push_back("                    <name>IlinkRawBinarySegment</name>");
    List->push_back("                    <state></state>");
    List->push_back("                </option>");
    List->push_back("                <option>");
    List->push_back("                    <name>IlinkRawBinaryAlign</name>");
    List->push_back("                    <state></state>");
    List->push_back("                </option>");
    List->push_back("                <option>");
    List->push_back("                    <name>IlinkDefines</name>");
    List->push_back("                    <state></state>");
    List->push_back("                </option>");
    List->push_back("                <option>");
    List->push_back("                    <name>IlinkConfigDefines</name>");
    List->push_back("                    <state></state>");
    List->push_back("                </option>");
    List->push_back("                <option>");
    List->push_back("                    <name>IlinkMapFile</name>");
    List->push_back("                    <state>1</state>");
    List->push_back("                </option>");
    List->push_back("                <option>");
    List->push_back("                    <name>IlinkLogFile</name>");
    List->push_back("                    <state>0</state>");
    List->push_back("                </option>");
    List->push_back("                <option>");
    List->push_back("                    <name>IlinkLogInitialization</name>");
    List->push_back("                    <state>0</state>");
    List->push_back("                </option>");
    List->push_back("                <option>");
    List->push_back("                    <name>IlinkLogModule</name>");
    List->push_back("                    <state>0</state>");
    List->push_back("                </option>");
    List->push_back("                <option>");
    List->push_back("                    <name>IlinkLogSection</name>");
    List->push_back("                    <state>0</state>");
    List->push_back("                </option>");
    List->push_back("                <option>");
    List->push_back("                    <name>IlinkLogVeneer</name>");
    List->push_back("                    <state>0</state>");
    List->push_back("                </option>");
    /* Custom icf override - without this the linker silently falls back to
     * the device default configuration file */
    List->push_back("                <option>");
    List->push_back("                    <name>IlinkIcfOverride</name>");
    List->push_back("                    <state>1</state>");
    List->push_back("                </option>");
    List->push_back("                <option>");
    List->push_back("                    <name>IlinkIcfFile</name>");
    Path=Linker;
    if(!((Path.length()>2)&&(Path[1]==':')))
        Path="$PROJ_DIR$\\"+Path;
    std::replace(Path.begin(),Path.end(),'/','\\');
    List->push_back("                    <state>"+Path+"</state>");
    List->push_back("                </option>");
    List->push_back("                <option>");
    List->push_back("                    <name>IlinkIcfFileSlave</name>");
    List->push_back("                    <state></state>");
    List->push_back("                </option>");
    List->push_back("                <option>");
    List->push_back("                    <name>IlinkEnableRemarks</name>");
    List->push_back("                    <state>0</state>");
    List->push_back("                </option>");
    List->push_back("                <option>");
    List->push_back("                    <name>IlinkSuppressDiags</name>");
    List->push_back("                    <state></state>");
    List->push_back("                </option>");
    List->push_back("                <option>");
    List->push_back("                    <name>IlinkTreatAsRem</name>");
    List->push_back("                    <state></state>");
    List->push_back("                </option>");
    List->push_back("                <option>");
    List->push_back("                    <name>IlinkTreatAsWarn</name>");
    List->push_back("                    <state></state>");
    List->push_back("                </option>");
    List->push_back("                <option>");
    List->push_back("                    <name>IlinkTreatAsErr</name>");
    List->push_back("                    <state></state>");
    List->push_back("                </option>");
    List->push_back("                <option>");
    List->push_back("                    <name>IlinkWarningsAreErrors</name>");
    List->push_back("                    <state>0</state>");
    List->push_back("                </option>");
    List->push_back("                <option>");
    List->push_back("                    <name>IlinkUseExtraOptions</name>");
    List->push_back("                    <state>0</state>");
    List->push_back("                </option>");
    List->push_back("                <option>");
    List->push_back("                    <name>IlinkExtraOptions</name>");
    List->push_back("                    <state></state>");
    List->push_back("                </option>");
    List->push_back("                <option>");
    List->push_back("                    <name>IlinkLowLevelInterfaceSlave</name>");
    List->push_back("                    <state>1</state>");
    List->push_back("                </option>");
    List->push_back("                <option>");
    List->push_back("                    <name>IlinkAutoLibEnable</name>");
    List->push_back("                    <state>1</state>");
    List->push_back("                </option>");
    List->push_back("                <option>");
    List->push_back("                    <name>IlinkAdditionalLibs</name>");
    List->push_back("                    <state></state>");
    List->push_back("                </option>");
    List->push_back("                <option>");
    List->push_back("                    <name>IlinkOverrideProgramEntryLabel</name>");
    List->push_back("                    <state>0</state>");
    List->push_back("                </option>");
    List->push_back("                <option>");
    List->push_back("                    <name>IlinkProgramEntryLabelSelect</name>");
    List->push_back("                    <state>0</state>");
    List->push_back("                </option>");
    List->push_back("                <option>");
    List->push_back("                    <name>IlinkProgramEntryLabel</name>");
    List->push_back("                    <state>__iar_program_start</state>");
    List->push_back("                </option>");
    List->push_back("                <option>");
    List->push_back("                    <name>DoFill</name>");
    List->push_back("                    <state>0</state>");
    List->push_back("                </option>");
    List->push_back("                <option>");
    List->push_back("                    <name>FillerByte</name>");
    List->push_back("                    <state>0xFF</state>");
    List->push_back("                </option>");
    List->push_back("                <option>");
    List->push_back("                    <name>FillerStart</name>");
    List->push_back("                    <state>0x0</state>");
    List->push_back("                </option>");
    List->push_back("                <option>");
    List->push_back("                    <name>FillerEnd</name>");
    List->push_back("                    <state>0x0</state>");
    List->push_back("                </option>");
    List->push_back("                <option>");
    List->push_back("                    <name>IlinkRawBinaryFile2</name>");
    List->push_back("                    <state></state>");
    List->push_back("                </option>");
    List->push_back("                <option>");
    List->push_back("                    <name>IlinkRawBinarySymbol2</name>");
    List->push_back("                    <state></state>");
    List->push_back("                </option>");
    List->push_back("                <option>");
    List->push_back("                    <name>IlinkRawBinarySegment2</name>");
    List->push_back("                    <state></state>");
    List->push_back("                </option>");
    List->push_back("                <option>");
    List->push_back("                    <name>IlinkRawBinaryAlign2</name>");
    List->push_back("                    <state></state>");
    List->push_back("                </option>");
    List->push_back("                <option>");
    List->push_back("                    <name>IlinkLogCrtRoutineSelection</name>");
    List->push_back("                    <state>0</state>");
    List->push_back("                </option>");
    List->push_back("                <option>");
    List->push_back("                    <name>IlinkLogFragmentInfo</name>");
    List->push_back("                    <state>0</state>");
    List->push_back("                </option>");
    List->push_back("                <option>");
    List->push_back("                    <name>IlinkLogInlining</name>");
    List->push_back("                    <state>0</state>");
    List->push_back("                </option>");
    List->push_back("                <option>");
    List->push_back("                    <name>IlinkLogMerging</name>");
    List->push_back("                    <state>0</state>");
    List->push_back("                </option>");
    List->push_back("                <option>");
    List->push_back("                    <name>IlinkDemangle</name>");
    List->push_back("                    <state>0</state>");
    List->push_back("                </option>");
    List->push_back("                <option>");
    List->push_back("                    <name>IlinkWrapperFileEnable</name>");
    List->push_back("                    <state>0</state>");
    List->push_back("                </option>");
    List->push_back("                <option>");
    List->push_back("                    <name>IlinkWrapperFile</name>");
    List->push_back("                    <state></state>");
    List->push_back("                </option>");
    List->push_back("                <option>");
    List->push_back("                    <name>IlinkProcessor</name>");
    List->push_back("                    <state>1</state>");
    List->push_back("                </option>");
    List->push_back("                <option>");
    List->push_back("                    <name>IlinkFpuProcessor</name>");
    List->push_back("                    <state>1</state>");
    List->push_back("                </option>");
    List->push_back("            </data>");
    List->push_back("        </settings>");
    /* IARCHIVE *****************************************************************/
    List->push_back("        <settings>");
    List->push_back("            <name>IARCHIVE</name>");
    List->push_back("            <archiveVersion>0</archiveVersion>");
    List->push_back("            <data>");
    List->push_back("                <version>0</version>");
    List->push_back("                <wantNonLocal>1</wantNonLocal>");
    List->push_back("                <debug>1</debug>");
    List->push_back("                <option>");
    List->push_back("                    <name>IarchiveInputs</name>");
    List->push_back("                    <state></state>");
    List->push_back("                </option>");
    List->push_back("                <option>");
    List->push_back("                    <name>IarchiveOverride</name>");
    List->push_back("                    <state>0</state>");
    List->push_back("                </option>");
    List->push_back("                <option>");
    List->push_back("                    <name>IarchiveOutput</name>");
    List->push_back("                    <state>###Unitialized###</state>");
    List->push_back("                </option>");
    List->push_back("            </data>");
    List->push_back("        </settings>");
    List->push_back("    </configuration>");
    /* Target group - all stubs, boot code and generated sources */
    List->push_back("    <group>");
    List->push_back("        <name>"+Target+"</name>");
    for(const std::string& Src:Source)
    {
        Path=Src;
        if(!((Path.length()>2)&&(Path[1]==':')))
            Path="$PROJ_DIR$\\"+Path;
        std::replace(Path.begin(),Path.end(),'/','\\');
        List->push_back("        <file>");
        List->push_back("            <name>"+Path+"</name>");
        List->push_back("        </file>");
    }
    List->push_back("    </group>");
    /* Library group - the fixed kernel/monitor/platform sources */
    List->push_back("    <group>");
    List->push_back("        <name>Lib</name>");
    for(const std::string& Src:Library)
    {
        Path=Src;
        if(!((Path.length()>2)&&(Path[1]==':')))
            Path="$PROJ_DIR$\\"+Path;
        std::replace(Path.begin(),Path.end(),'/','\\');
        List->push_back("        <file>");
        List->push_back("            <name>"+Path+"</name>");
        List->push_back("        </file>");
    }
    List->push_back("    </group>");
    List->push_back("</project>");
}
/* End Function:IAREW_Gen::Raw_Proj ******************************************/

/* Function:IAREW_Gen::Kernel_Proj *********************************************
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
void IAREW_Gen::Kernel_Proj(std::unique_ptr<std::vector<std::string>>& List,
                            const std::vector<std::string>& Include,
                            const std::vector<std::string>& Source,
                            const std::vector<std::string>& Library,
                            const std::vector<std::string>& Linker)
{
    std::string Postbuild;
    std::vector<std::string> None;

    /* The flat binary is required for flashing via the OpenOCD flow */
    Postbuild="\"$TOOLKIT_DIR$\\bin\\ielftool.exe\" --bin \"$TARGET_PATH$\" \"$EXE_DIR$\\Kernel.bin\"";

    this->Raw_Proj(List,
                   Postbuild,                               /* Postbuild */
                   "Kernel",                                /* Target */
                   this->Proj->Kernel->Optimization,        /* Optimization */
                   None,                                    /* Coprocessor */
                   Include,                                 /* Include */
                   Source,                                  /* Source */
                   Library,                                 /* Library */
                   Linker[0]);                              /* Linker */
}
/* End Function:IAREW_Gen::Kernel_Proj ***************************************/

/* Function:IAREW_Gen::Monitor_Proj ********************************************
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
void IAREW_Gen::Monitor_Proj(std::unique_ptr<std::vector<std::string>>& List,
                             const std::vector<std::string>& Include,
                             const std::vector<std::string>& Source,
                             const std::vector<std::string>& Library,
                             const std::vector<std::string>& Linker)
{
    std::string Postbuild;
    std::string Bincopy;
    std::vector<std::string> None;

    if(this->Proj->Kernel->Full_Image!=0)
    {
        Bincopy=Main::Path_Absolute(PATH_FILE,Main::Monitor_Root,"Utility/M7M02_Final/bincopy.exe");
        Bincopy=Main::Path_Relative(PATH_FILE,this->Proj->Monitor->Project_Output,Bincopy);
        Postbuild="\"$TOOLKIT_DIR$\\bin\\ielftool.exe\" --bin \"$TARGET_PATH$\" \"$EXE_DIR$\\Monitor.bin\" &amp;&amp; \""+
                  Bincopy+"\" -i \"$EXE_DIR$\\Monitor.bin\" -o ./monitor_image.c";
    }

    this->Raw_Proj(List,
                   Postbuild,                               /* Postbuild */
                   "Monitor",                               /* Target */
                   this->Proj->Monitor->Optimization,       /* Optimization */
                   None,                                    /* Coprocessor */
                   Include,                                 /* Include */
                   Source,                                  /* Source */
                   Library,                                 /* Library */
                   Linker[0]);                              /* Linker */
}
/* End Function:IAREW_Gen::Monitor_Proj **************************************/

/* Function:IAREW_Gen::Process_Proj ********************************************
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
void IAREW_Gen::Process_Proj(std::unique_ptr<std::vector<std::string>>& List,
                             const std::vector<std::string>& Include,
                             const std::vector<std::string>& Source,
                             const std::vector<std::string>& Library,
                             const std::vector<std::string>& Linker,
                             const class Process* Prc)
{
    std::string Postbuild;
    std::string Bincopy;

    if(this->Proj->Kernel->Full_Image!=0)
    {
        Bincopy=Main::Path_Absolute(PATH_FILE,Main::Monitor_Root,"Utility/M7M02_Final/bincopy.exe");
        Bincopy=Main::Path_Relative(PATH_FILE,Prc->Project_Output,Bincopy);
        Postbuild="\"$TOOLKIT_DIR$\\bin\\ielftool.exe\" --bin \"$TARGET_PATH$\" \"$EXE_DIR$\\"+
                  Prc->Name+".bin\" &amp;&amp; \""+Bincopy+"\" -i \"$EXE_DIR$\\"+
                  Prc->Name+".bin\" -o ./"+Prc->Name_Lower+"_image.c";
    }

    this->Raw_Proj(List,
                   Postbuild,                               /* Postbuild */
                   Prc->Name,                               /* Target */
                   Prc->Optimization,                       /* Optimization */
                   Prc->Coprocessor,                        /* Coprocessor */
                   Include,                                 /* Include */
                   Source,                                  /* Source */
                   Library,                                 /* Library */
                   Linker[0]);                              /* Linker */
}
/* End Function:IAREW_Gen::Process_Proj **************************************/

/* Function:IAREW_Gen::Workspace_Sect ******************************************
Description : Generate a workspace project section.
Input       : std::unique_ptr<std::vector<std::string>>& List - The file.
              const std::vector<std::string>& Project - The project list.
Output      : std::unique_ptr<std::vector<std::string>>& List - The updated file.
Output      : None.
Return      : None.
******************************************************************************/
void IAREW_Gen::Workspace_Sect(std::unique_ptr<std::vector<std::string>>& List,
                               const std::vector<std::string>& Project)
{
    std::string Path;

    for(const std::string& Proj:Project)
    {
        Path=Proj;
        std::replace(Path.begin(),Path.end(),'/','\\');
        List->push_back("  <project>");
        List->push_back("    <path>$WS_DIR$\\"+Path+"</path>");
        List->push_back("  </project>");
    }
}
/* End Function:IAREW_Gen::Workspace_Sect ************************************/

/* Function:IAREW_Gen::Workspace_Proj ******************************************
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
void IAREW_Gen::Workspace_Proj(std::unique_ptr<std::vector<std::string>>& List,
                               const std::vector<std::string>& Kernel,
                               const std::vector<std::string>& Monitor,
                               const std::vector<std::string>& Native,
                               const std::vector<std::string>& Virtual)
{
    List->push_back("<?xml version=\"1.0\" encoding=\"UTF-8\"?>");
    List->push_back("<workspace>");
    this->Workspace_Sect(List, Virtual);
    this->Workspace_Sect(List, Native);
    this->Workspace_Sect(List, Monitor);
    this->Workspace_Sect(List, Kernel);
    List->push_back("  <batchBuild/>");
    List->push_back("</workspace>");
}
/* End Function:IAREW_Gen::Workspace_Proj ************************************/
}
/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
