/******************************************************************************
Filename    : gen_tool.cpp
Author      : pry
Date        : 16/07/2019
Licence     : LGPL v3+; see COPYING for details.
Description : The memory block class. This contains the memory block information.
******************************************************************************/

/* Includes ******************************************************************/
extern "C"
{
#include "xml.h"
#include "string.h"
}

#include "map"
#include "set"
#include "string"
#include "memory"
#include "vector"
#include "stdexcept"
#include "algorithm"
#include "filesystem"

#define __HDR_DEFS__
#include "rvm_gen.hpp"
#include "Conf_Info/conf_info.hpp"
#include "Mem_Info/mem_info.hpp"
#include "Gen_Tool/gen_tool.hpp"
#include "Proj_Info/Process/process.hpp"
#include "Gen_Tool/Plat_Gen/plat_gen.hpp"
#include "Gen_Tool/Build_Gen/build_gen.hpp"
#include "Gen_Tool/Tool_Gen/tool_gen.hpp"
#include "Gen_Tool/Guest_Gen/guest_gen.hpp"
#undef __HDR_DEFS__

#define __HDR_CLASSES__
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

#include "Gen_Tool/gen_tool.hpp"

#include "Gen_Tool/Plat_Gen/plat_gen.hpp"
#include "Gen_Tool/Plat_Gen/A7M_Gen/a7m_gen.hpp"
#include "Gen_Tool/Plat_Gen/A6M_Gen/a6m_gen.hpp"

#include "Gen_Tool/Build_Gen/build_gen.hpp"
#include "Gen_Tool/Build_Gen/Keil_Gen/keil_gen.hpp"
#include "Gen_Tool/Build_Gen/Makefile_Gen/makefile_gen.hpp"

#include "Gen_Tool/Tool_Gen/tool_gen.hpp"
#include "Gen_Tool/Tool_Gen/ARMCC_Gen/armcc_gen.hpp"
#include "Gen_Tool/Tool_Gen/GCC_Gen/gcc_gen.hpp"

#include "Gen_Tool/Guest_Gen/guest_gen.hpp"
#include "Gen_Tool/Guest_Gen/RMP_Gen/rmp_gen.hpp"
#undef __HDR_CLASSES__
/* End Includes **************************************************************/
namespace RVM_GEN
{
/* Begin Function:Gen_Tool::Gen_Tool ******************************************
Description : Constructor for generation tool class.
Input       : const std::string& Name - The name of the platform.
Input       : class Proj_Info* Proj - The project information.
              class Plat_Info* Plat - The platform information.
              class Chip_Info* Chip - The chip information.
Output      : None.
Return      : None.
******************************************************************************/
/* void */ Gen_Tool::Gen_Tool(const std::string& Name,
                              class Proj_Info* Proj, class Plat_Info* Plat, class Chip_Info* Chip)
{
    try
    {
        if(Name=="A7M")
            this->Plat=std::make_unique<class A7M_Gen>(Proj,Plat,Chip);
        else if(Name=="A6M")
        	this->Plat=std::make_unique<class A6M_Gen>(Proj,Plat,Chip);
        else
            Main::Error("XXXXX: Platform generator for '"+Name+"' is not found.");
    }
    catch(std::exception& Exc)
    {
        Main::Error(std::string("Tool:\n")+Exc.what());
    }
}
/* End Function:Gen_Tool::Gen_Tool *******************************************/

/* Begin Function:Gen_Tool::Build_Load *****************************************
Description : Load one of the buildsystems.
Input       : const std::string& Name - The name of the buildsystem.
Output      : None.
Return      : None.
******************************************************************************/
void  Gen_Tool::Build_Load(const std::string& Name)
{
    std::unique_ptr<class Build_Gen> Build;

    try
    {
        if(this->Build_Map.find(Name)!=this->Build_Map.end())
            return;

        if(Name=="Keil")
            Build=std::make_unique<class Keil_Gen>(this->Plat->Proj,this->Plat->Plat,this->Plat->Chip);
        else if(Name=="Makefile")
            Build=std::make_unique<class Makefile_Gen>(this->Plat->Proj,this->Plat->Plat,this->Plat->Chip);
        else
            Main::Error("XXXXX: Buildsystem generator for '"+Name+"' is not found.");

        this->Build_Map.insert(std::make_pair(Name,Build.get()));
        this->Build.push_back(std::move(Build));
    }
    catch(std::exception& Exc)
    {
        Main::Error(std::string("Tool:\n")+Exc.what());
    }
}
/* End Function:Gen_Tool::Build_Load *****************************************/

/* Begin Function:Gen_Tool::Tool_Load *****************************************
Description : Load one of the build tools.
Input       : const std::string& Name - The name of the toolchain.
Output      : None.
Return      : None.
******************************************************************************/
void  Gen_Tool::Tool_Load(const std::string& Name)
{
    std::unique_ptr<class Tool_Gen> Tool;

    try
    {
        if(this->Tool_Map.find(Name)!=this->Tool_Map.end())
            return;

        if(Name=="ARMCC")
            Tool=std::make_unique<class ARMCC_Gen>(this->Plat->Proj,this->Plat->Plat,this->Plat->Chip);
        else if(Name=="GCC")
            Tool=std::make_unique<class GCC_Gen>(this->Plat->Proj,this->Plat->Plat,this->Plat->Chip);
        else
            Main::Error("XXXXX: Toolchain generator for '"+Name+"' is not found.");

        this->Tool_Map.insert(std::make_pair(Name,Tool.get()));
        this->Tool.push_back(std::move(Tool));
    }
    catch(std::exception& Exc)
    {
        Main::Error(std::string("Tool:\n")+Exc.what());
    }
}
/* End Function:Gen_Tool::Tool_Load ******************************************/

/* Begin Function:Gen_Tool::Guest_Load ****************************************
Description : Load one of the guest OSes.
Input       : const std::string& Name - The name of the guest OS.
Output      : None.
Return      : None.
******************************************************************************/
void  Gen_Tool::Guest_Load(const std::string& Name)
{
    std::unique_ptr<class Guest_Gen> Guest;

    try
    {
        if(this->Guest_Map.find(Name)!=this->Guest_Map.end())
            return;

        if(Name=="RMP")
            Guest=std::make_unique<class RMP_Gen>(this->Plat->Proj,this->Plat->Plat,this->Plat->Chip);
        else
            Main::Error("XXXXX: Guest generator for '"+Name+"' is not found.");

        this->Guest_Map.insert(std::make_pair(Name,Guest.get()));
        this->Guest.push_back(std::move(Guest));
    }
    catch(std::exception& Exc)
    {
        Main::Error(std::string("Tool:\n")+Exc.what());
    }
}
/* End Function:Gen_Tool::Guest_Load *****************************************/

/* Begin Function:Gen_Tool::Line_Read *****************************************
Description : Read a source file to line-based buffer for further processing.
Input       : const std::string& Path - The path to the file.
Output      : None.
Return      : std::unique_ptr<std::vector<std::string>> - The file returned by line.
******************************************************************************/
std::unique_ptr<std::vector<std::string>> Gen_Tool::Line_Read(const std::string& Path)
{
    FILE* File;
    char* Buf;
    ptr_t Len;
    std::string Line;
    std::unique_ptr<std::vector<std::string>> List;

    try
    {
        Buf=nullptr;
        File=nullptr;
        List=std::make_unique<std::vector<std::string>>();
        Buf=(char*)malloc(BUF_SIZE);
        if(Buf==nullptr)
            Main::Error("XXXXX: Cannot read source file '%s'.",Path.c_str());

        /* Read using text mode */
        File=fopen(Path.c_str(), "r");
        if(File==nullptr)
            Main::Error("XXXXX: Cannot read source file '%s'.",Path.c_str());

        while(1)
        {
            if(fgets(Buf,BUF_SIZE-10,File)==nullptr)
                break;

            Len=strlen(Buf);
            if(Buf[Len-1]=='\n')
                Buf[Len-1]='\0';

            List->push_back(Buf);
        }

        fclose(File);
        free(Buf);

        return List;
    }
    catch(std::exception& Exc)
    {
        if(File!=nullptr)
            fclose(File);
        if(Buf!=nullptr)
            free(Buf);

        Main::Error(std::string("Line buffer read:\n")+Exc.what());
    }
}
/* End Function:Gen_Tool::Line_Read *******************************************/

/* Begin Function:Gen_Tool::Line_Write *****************************************
Description : Write a line-based buffer to destination file.
Input       : std::unique_ptr<std::vector<std::string>>& List - The file contents.
              const std::string& Path - The path to the file.
Output      : None.
Return      : None.
******************************************************************************/
void Gen_Tool::Line_Write(std::unique_ptr<std::vector<std::string>>& List, const std::string& Path)
{
    FILE* File;

    try
    {
        /* Write using text mode */
        File=nullptr;
        File=fopen(Path.c_str(), "w");
        if(File==nullptr)
            Main::Error("XXXXX: Cannot read source file '%s'.",Path.c_str());

        for(const std::string& Line:*List)
            fprintf(File,"%s\n",Line.c_str());

        fclose(File);
    }
    catch(std::exception& Exc)
    {
        if(File!=nullptr)
            fclose(File);

        Main::Error(std::string("Line buffer write:\n")+Exc.what());
    }
}
/* End Function:Gen_Tool::Line_Write *****************************************/

/* Begin Function:Gen_Tool::Macro_String **************************************
Description : See if a macro is defined in the file, and modify it as a string.
Input       : std::unique_ptr<std::vector<std::string>>& List - The file contents.
              const std::string& Macro - The macro name.
              const std::string& Value - The new name to assign to that macro.
              ptr_t Mode - The operation mode, replace or add.
Output      : std::unique_ptr<std::vector<std::string>>& List - The updated file.
Return      : None.
******************************************************************************/
void Gen_Tool::Macro_String(std::unique_ptr<std::vector<std::string>>& List,
                            const std::string& Macro, const std::string& Value, ptr_t Mode)
{
    ptr_t Pos;
    char Buf[256];
    char Format[32];
    std::string Compare;

    if(Mode==MACRO_REPLACE)
    {
        Compare=std::string("#define ")+Macro;

        for(std::string& Line:*List)
        {
            if(Line.find(Compare)!=std::string::npos)
            {
                /* Replace wherever it was */
                Pos=Line.find_last_of(" \t");
                if(Pos==std::string::npos)
                    Pos=MACRO_ALIGN;
                else
                    Pos-=11;
                /* Decide format first, then generate the line */
                sprintf(Format, "#define %%-%llds    (%%s)", Pos);
                sprintf(Buf, Format, Macro.c_str(), Value.c_str());
                Line=Buf;
                return;
            }
        }
    }

    /* Print to file if this does not yet exist */
    Pos=MACRO_ALIGN;
    sprintf(Format, "#define %%-%llds    (%%s)", Pos);
    sprintf(Buf, Format, Macro.c_str(), Value.c_str());
    List->push_back(Buf);
}
/* End Function:Gen_Tool::Macro_String ***************************************/

/* Begin Function:Gen_Tool::Macro_Hex *****************************************
Description : See if a macro is defined in the file, and modify it as a hex number.
Input       : std::unique_ptr<std::vector<std::string>>& List - The file contents.
              const std::string& Macro - The macro name.
              ptr_t Value - The new value to assign to that macro.
              ptr_t Mode - The operation mode, replace or add.
Output      : std::unique_ptr<std::vector<std::string>>& List - The updated file.
Return      : None.
******************************************************************************/
void Gen_Tool::Macro_Hex(std::unique_ptr<std::vector<std::string>>& List,
                         const std::string& Macro, ptr_t Value, ptr_t Mode)
{
    char Buf[64];

    sprintf(Buf,"0x%llXU",Value);
    Gen_Tool::Macro_String(List,Macro,Buf,Mode);
}
/* End Function:Gen_Tool::Macro_Hex ******************************************/

/* Begin Function:Gen_Tool::Macro_Int *****************************************
Description : See if a macro is defined in the file, and modify it as a dec number.
Input       : std::unique_ptr<std::vector<std::string>>& List - The file contents.
              const std::string& Macro - The macro name.
              ptr_t Value - The new value to assign to that macro.
              ptr_t Mode - The operation mode, replace or add.
Output      : std::unique_ptr<std::vector<std::string>>& List - The updated file.
Return      : None.
******************************************************************************/
void Gen_Tool::Macro_Int(std::unique_ptr<std::vector<std::string>>& List,
                         const std::string& Macro, ptr_t Value, ptr_t Mode)
{
    char Buf[64];

    sprintf(Buf,"%lldU",Value);
    Gen_Tool::Macro_String(List,Macro,Buf,Mode);
}
/* End Function:Gen_Tool::Macro_Int ******************************************/

/* Begin Function:Gen_Tool::Src_Head ******************************************
Description : Emit source descriptor into a source document.
Input       : std::unique_ptr<std::vector<std::string>>& List - The input file.
              const std::string& Name - The filename.
              const std::string& Desc - The file description.
Output      : std::unique_ptr<std::vector<std::string>>& List - The appended file.
Return      : None.
******************************************************************************/
void Gen_Tool::Src_Head(std::unique_ptr<std::vector<std::string>>& List,
                        const std::string& Name, const std::string& Desc)
{
    List->push_back("/******************************************************************************");
    List->push_back(std::string("Filename    : ")+Name);
    List->push_back("Author      : " CODE_AUTHOR);
    List->push_back(std::string("Date        : ")+Main::Time);
    List->push_back("License     : " CODE_LICENSE);
    List->push_back(std::string("Description : ")+Desc);
    List->push_back("******************************************************************************/");
}
/* End Function:Gen_Tool::Src_Head *******************************************/

/* Begin Function:Gen_Tool::Src_Foot ******************************************
Description : Emit source footer into a source document.
Input       : std::unique_ptr<std::vector<std::string>>& List - The input file.
Output      : std::unique_ptr<std::vector<std::string>>& List - The appended file.
Return      : None.
******************************************************************************/
void Gen_Tool::Src_Foot(std::unique_ptr<std::vector<std::string>>& List)
{
    List->push_back("/* End Of File ***************************************************************/");
    List->push_back("");
    List->push_back("/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/");
    List->push_back("");
}
/* End Function:Gen_Tool::Src_Foot *******************************************/

/* Begin Function:Gen_Tool::Func_Head *****************************************
Description : Write the description part of a C function.
Input       : std::unique_ptr<std::vector<std::string>>& List - The input file.
              const std::string& Name - Name of the function.
              const std::string& Description - One-line description.
              const std::vector<std::string>& Input - The inputs.
              const std::vector<std::string>& Output - The outputs.
              const std::string& Return - The return value.
Output      : std::unique_ptr<std::vector<std::string>>& List - The appended file.
Return      : None.
******************************************************************************/
void Gen_Tool::Func_Head(std::unique_ptr<std::vector<std::string>>& List,
                         const std::string& Name,
                         const std::string& Description,
                         const std::vector<std::string>& Input,
                         const std::vector<std::string>& Output,
                         const std::string& Return)
{
    ptr_t Len;
    char Buf[256];

    /* Print headline */
    for(Len=sprintf(Buf, "/* Begin Function:%s ", Name.c_str());Len<79;Len++)
        Buf[Len]='*';
    Buf[Len]='\0';
    List->push_back(Buf);

    /* Print description */
    List->push_back("Description : "+Description);

    /* Print all inputs */
    if(Input.size()==0)
        List->push_back("Input       : None.");
    else
    {
        List->push_back(std::string("Input       : ")+Input[0]);
        for(const std::string& Entry:Input)
        {
            if(Entry==Input[0])
                continue;
            List->push_back(std::string("            : ")+Entry);
        }
    }

    /* Print all outputs */
    if(Output.size()==0)
        List->push_back("Output      : None.");
    else
    {
        List->push_back("Output      : "+Output[0]);
        for(const std::string& Entry:Output)
        {
            if(Entry==Output[0])
                continue;
            List->push_back(std::string("            : ")+Entry);
        }
    }

    /* Print return */
    List->push_back(std::string("Return      : ")+Return);
    List->push_back("******************************************************************************/");
}
/* End Function:Gen_Tool::Func_Head ******************************************/

/* Begin Function:Gen_Tool::Func_Foot *****************************************
Description : Write the footer part of a C function.
Input       : std::unique_ptr<std::vector<std::string>>& List - The input file.
              const std::string& Name - Name of the function.
              const std::string& Desc - One-line description.
              const std::vector<std::string>& Input - The inputs.
              const std::vector<std::string>& Output - The outputs.
              const std::string& Return - The return value.
Output      : std::unique_ptr<std::vector<std::string>>& List - The appended file.
Return      : None.
******************************************************************************/
void Gen_Tool::Func_Foot(std::unique_ptr<std::vector<std::string>>& List, const std::string& Name)
{
    ptr_t Len;
    char Buf[256];

    /* Print headline */
    for(Len=sprintf(Buf, "/* End Function:%s ", Name.c_str());Len<78;Len++)
        Buf[Len]='*';
    Buf[Len]='/';
    Buf[Len+1]='\0';
    List->push_back(Buf);
}
/* End Function:Gen_Tool::Func_Foot ******************************************/

/* Begin Function:Gen_Tool::Path_Conv *****************************************
Description : Deduplicate all paths, and make them relative to the root path.
Input       : const std::string& Root - The root path.
              std::vector<std::string>& List - The path list.
Output      : std::vector<std::string>& List - The modified path list.
Return      : None.
******************************************************************************/
void Gen_Tool::Path_Conv(const std::string& Root, std::vector<std::string>& List)
{
    std::filesystem::path Root_Abs;
    std::filesystem::path Path_Abs;
    std::filesystem::path Path_Rel;
    std::set<std::string> Result;
    std::string Temp;

    /* Convert root to absolute mode */
    Root_Abs=std::filesystem::absolute(Root);

    /* Convert then dump redundant files */
    for(const std::string& Path:List)
    {
        Path_Abs=std::filesystem::absolute(Path);
        Path_Rel=std::filesystem::relative(Path_Abs, Root_Abs);
        /* If whatever being converted is a path, make it a path */
        if((Path.back()=='/')||(Path.back()=='\\'))
            Result.insert(Path_Rel.string()+"/");
        else
            Result.insert(Path_Rel.string());
    }

    /* Substitute path specifiers */
    List.clear();
    for(const std::string& Path:Result)
    {
        Temp=Path;
        std::replace(Temp.begin(),Temp.end(),'\\','/');
        List.push_back(Temp);
    }

    /* Sort the list from small to large for better view */
    std::sort(List.begin(),List.end(),
              [](const std::string& Left, const std::string& Right)->bool
              {
                  return Left>Right;
              });
}
/* End Function:Gen_Tool::Path_Conv ******************************************/

/* Begin Function:Gen_Tool::Kernel_Inc ****************************************
Description : Write the include files for kernel.
Input       : std::unique_ptr<std::vector<std::string>>& List - The input file.
Output      : std::unique_ptr<std::vector<std::string>>& List - The appended file.
Return      : None.
******************************************************************************/
void Gen_Tool::Kernel_Inc(std::unique_ptr<std::vector<std::string>>& List)
{
    std::string Temp;

    List->push_back("/* Includes ******************************************************************/");
    Temp=std::string("#include \"Platform/")+this->Plat->Name+"/rme_platform_"+this->Plat->Name_Lower+".h\"";
    List->push_back("#define __HDR_DEFS__");
    List->push_back(Temp);
    List->push_back("#include \"Kernel/rme_kernel.h\"");
    List->push_back("#undef __HDR_DEFS__");
    List->push_back("");
    List->push_back("#define __HDR_STRUCTS__");
    List->push_back(Temp);
    List->push_back("#include \"Kernel/rme_kernel.h\"");
    List->push_back("#undef __HDR_STRUCTS__");
    List->push_back("");
    List->push_back("#define __HDR_PUBLIC_MEMBERS__");
    List->push_back(Temp);
    List->push_back("#include \"Kernel/rme_kernel.h\"");
    List->push_back("#undef __HDR_PUBLIC_MEMBERS__");
    List->push_back("");
    List->push_back("#include \"rme_boot.h\"");
    List->push_back("/* End Includes **************************************************************/");
}
/* End Function:Gen_Tool::Kernel_Inc *****************************************/

/* Begin Function:Gen_Tool::Kernel_Conf_Hdr ***********************************
Description : Create the platform configuration headers for kernel.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
void Gen_Tool::Kernel_Conf_Hdr(void)
{
    std::string Filename;
    class Kernel* Kernel;
    class Monitor* Monitor;
    std::unique_ptr<std::vector<std::string>> List;

    Kernel=this->Plat->Proj->Kernel.get();
    Monitor=this->Plat->Proj->Monitor.get();
    List=std::make_unique<std::vector<std::string>>();

    /* Generate rme_platform.h */
    Main::Info("> Generating 'rme_platform.h'.");
    Gen_Tool::Src_Head(List, "rme_platform.h", "The platform selection header.");
    List->push_back("");
    List->push_back("/* Platform Includes *********************************************************/");
    List->push_back(std::string("#include \"Platform/")+this->Plat->Name+"/rme_platform_"+this->Plat->Name_Lower+".h\"");
    List->push_back("/* End Platform Includes *****************************************************/");
    List->push_back("");
    Gen_Tool::Src_Foot(List);
    Gen_Tool::Line_Write(List, Kernel->Config_Header_Output+"/rme_platform.h");

    /* Generate rme_platform_xxx_conf.h */
    Filename=std::string("rme_platform_")+this->Plat->Name_Lower+"_conf.h";
    Main::Info(std::string("> Generating '")+Filename+"'.");
    List->clear();
    Gen_Tool::Src_Head(List, Filename, "The chip selection header.");
    List->push_back("");
    List->push_back("/* Platform Includes *********************************************************/");
    List->push_back(std::string("#include \"rme_platform_")+this->Plat->Chip->Name_Lower+".h\"");
    List->push_back("/* End Platform Includes *****************************************************/");
    List->push_back("");
    Gen_Tool::Src_Foot(List);
    Gen_Tool::Line_Write(List, Kernel->Config_Header_Output+"rme_platform_"+this->Plat->Name_Lower+"_conf.h");

    /* Generate rme_platform_chipname.h */
    Filename=std::string("rme_platform_")+this->Plat->Chip->Name_Lower+".h";
    Main::Info(std::string("> Generating '")+Filename+"'.");
    List=Gen_Tool::Line_Read(Kernel->Kernel_Root+
                             "Include/Platform/"+this->Plat->Name+"/Chip/"+this->Plat->Chip->Name+"/"+Filename);
    /* Replace general parameter macros */
    /* Debugging control */
    Gen_Tool::Macro_Int(List, "RME_ASSERT_CORRECT", this->Plat->Proj->Assert_Correct, MACRO_REPLACE);
    Gen_Tool::Macro_Int(List, "RME_DEBUG_PRINT", this->Plat->Proj->Debug_Print, MACRO_REPLACE);
    /* Generator enabled */
    Gen_Tool::Macro_Int(List, "RME_RVM_GEN_ENABLE", 1, MACRO_REPLACE);
    /* The initial capability table size */
    Gen_Tool::Macro_Int(List, "RME_BOOT_CPT_SIZE", Monitor->Captbl_Size, MACRO_REPLACE);
    /* The virtual memory base/size for the kernel objects */
    Gen_Tool::Macro_Hex(List, "RME_KOM_VA_BASE", Kernel->Kom_Base, MACRO_REPLACE);
    Gen_Tool::Macro_Hex(List, "RME_KOM_VA_SIZE", Kernel->Kom_Size, MACRO_REPLACE);
    /* The virtual memory base/size for the virtual machines - always full memory range, just in case */
    Gen_Tool::Macro_Hex(List, "RME_HYP_VA_BASE", 0x00000000ULL, MACRO_REPLACE);
    Gen_Tool::Macro_Hex(List, "RME_HYP_VA_SIZE", 0xFFFFFFFFULL, MACRO_REPLACE);
    /* Kernel stack base/size */
    Gen_Tool::Macro_Hex(List, "RME_KSTK_VA_BASE", Kernel->Stack_Base, MACRO_REPLACE);
    Gen_Tool::Macro_Hex(List, "RME_KSTK_VA_SIZE", Kernel->Stack_Size, MACRO_REPLACE);
    /* The maximum number of preemption priority levels in the system.
     * This parameter must be divisible by the word length - 32 is usually sufficient */
    Gen_Tool::Macro_Int(List, "RME_PREEMPT_PRIO_NUM", Kernel->Kern_Prio, MACRO_REPLACE);
    /* The granularity of kernel memory allocation, in bytes */
    Gen_Tool::Macro_Int(List, "RME_KOM_SLOT_ORDER", Kernel->Kom_Order, MACRO_REPLACE);
    /* Physical vector number, flag area base and its size */
    Gen_Tool::Macro_Int(List, "RME_RVM_PHYS_VCT_NUM", this->Plat->Chip->Vect_Num, MACRO_REPLACE);
    Gen_Tool::Macro_Hex(List, "RME_RVM_PHYS_VCTF_BASE", Kernel->Vctf_Base, MACRO_REPLACE);
    Gen_Tool::Macro_Hex(List, "RME_RVM_PHYS_VCTF_SIZE", Kernel->Vctf_Size, MACRO_REPLACE);
    /* Virtual event number, flag area base and its size */
    Gen_Tool::Macro_Int(List, "RME_RVM_VIRT_EVT_NUM", Monitor->Virt_Event, MACRO_REPLACE);
    Gen_Tool::Macro_Hex(List, "RME_RVM_VIRT_EVTF_BASE", Kernel->Evtf_Base, MACRO_REPLACE);
    Gen_Tool::Macro_Hex(List, "RME_RVM_VIRT_EVTF_SIZE", Kernel->Evtf_Size, MACRO_REPLACE);
    /* Initial kernel object frontier limit */
    Gen_Tool::Macro_Hex(List, "RME_RVM_CAP_BOOT_FRONT", Monitor->Before_Cap_Front, MACRO_REPLACE);
    Gen_Tool::Macro_Hex(List, "RME_RVM_KOM_BOOT_FRONT", Monitor->Before_Kom_Front, MACRO_REPLACE);

    /* Replace platform specific macros */
    this->Plat->Kernel_Conf_Hdr(List);
    /* Chip specific macros - we must be able to find it because we checked before */
    for(std::unique_ptr<class Conf_Info>& Conf:this->Plat->Chip->Config)
    {
        if(Conf->Type==CONFIG_RANGE)
            Gen_Tool::Macro_String(List, Conf->Macro, this->Plat->Proj->Chip->Config[Conf->Name]+"U", MACRO_REPLACE);
        else
            Gen_Tool::Macro_String(List, Conf->Macro, this->Plat->Proj->Chip->Config[Conf->Name], MACRO_REPLACE);
    }
    /* Write to kernel configuration file */
    Gen_Tool::Line_Write(List, Kernel->Config_Header_Output+Filename);
}
/* End Function:Gen_Tool::Kernel_Conf_Hdr ************************************/

/* Begin Function:Gen_Tool::Kernel_Boot_Hdr ***********************************
Description : Create the boot header for kernel.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
void Gen_Tool::Kernel_Boot_Hdr(void)
{
    ptr_t Obj_Cnt;
    ret_t Cap_Front;
    class Kernel* Kernel;
    class Monitor* Monitor;
    std::unique_ptr<std::vector<std::string>> List;

    Kernel=this->Plat->Proj->Kernel.get();
    Monitor=this->Plat->Proj->Monitor.get();
    List=std::make_unique<std::vector<std::string>>();

    Main::Info("> Generating file header.");
    Gen_Tool::Src_Head(List, "rme_boot.h", "The boot-time initialization file header.");
    List->push_back("");

    Main::Info("> Generating defines.");
    List->push_back("/* Defines *******************************************************************/");

    /* Vector capability tables */
    List->push_back("/* Vector endpoint capability tables */");
    Cap_Front=Kernel->Vct_Cap_Front;
    for(Obj_Cnt=0;Obj_Cnt<Monitor->Vector.size();Obj_Cnt+=this->Plat->Plat->Captbl_Max)
    {
        Gen_Tool::Macro_Int(List, std::string("RME_MAIN_VCT_")+MIDS(Obj_Cnt), Cap_Front, MACRO_ADD);
        Cap_Front++;
    }

    /* Vector endpoints */
    List->push_back("");
    List->push_back("/* Vector endpoints */");
    Obj_Cnt=0;
    for(const class Vect_Info* Vct:Monitor->Vector)
    {
        Gen_Tool::Macro_Int(List, Vct->Macro_Global, SID(Obj_Cnt), MACRO_ADD);
        Obj_Cnt++;
    }

    List->push_back("/* End Defines ***************************************************************/");
    List->push_back("");

    Main::Info("> Writing file.");
    Gen_Tool::Src_Foot(List);
    Gen_Tool::Line_Write(List, Kernel->Boot_Header_Output+"rme_boot.h");
}
/* End Function:Gen_Tool::Kernel_Boot_Hdr ************************************/

/* Begin Function:Gen_Tool::Kernel_Boot_Src ***********************************
Description : Create the boot source for kernel.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
void Gen_Tool::Kernel_Boot_Src(void)
{
    ptr_t Obj_Cnt;
    ptr_t Cpt_Size;
    class Kernel* Kernel;
    class Monitor* Monitor;
    std::vector<std::string> Input;
    std::vector<std::string> Output;
    std::unique_ptr<std::vector<std::string>> List;

    Kernel=this->Plat->Proj->Kernel.get();
    Monitor=this->Plat->Proj->Monitor.get();
    List=std::make_unique<std::vector<std::string>>();

    /* File header */
    Main::Info("> Generating file header.");
    Gen_Tool::Src_Head(List, "rme_boot.c", "The boot-time initialization file.");
    List->push_back("");

    /* Print all header includes */
    Main::Info("> Generating C header includes.");
    this->Kernel_Inc(List);
    List->push_back("");

    /* Global variables */
    List->push_back("/* Private Global Variables **************************************************/");
    for(const class Vect_Info* Vct:Monitor->Vector)
        List->push_back(std::string("static struct RME_Cap_Sig* ")+Vct->Name+"_Vct_Sig;");
    List->push_back("/* End Private Global Variables **********************************************/");
    List->push_back("");

    /* Private prototypes */
    Main::Info("> Generating private C function prototypes.");
    List->push_back("/* Private C Function Prototypes *********************************************/");
    for(const class Vect_Info* Vct:Monitor->Vector)
        List->push_back(std::string("EXTERN rme_ptr_t RME_Vct_")+Vct->Name+"_Handler(void);");
    List->push_back("/* End Private C Function Prototypes *****************************************/");
    List->push_back("");

    /* Public prototypes */
    Main::Info("> Generating public C function prototypes.");
    List->push_back("/* Public C Function Prototypes **********************************************/");
    List->push_back("rme_ptr_t RME_Boot_Vct_Init(struct RME_Cap_Cpt* Cpt, rme_ptr_t Cap_Front, rme_ptr_t Kom_Front);");
    List->push_back("rme_ptr_t RME_Boot_Vct_Handler(rme_ptr_t Vct_Num);");
    List->push_back("/* End Public C Function Prototypes ******************************************/");
    List->push_back("");

    /* Boot-time setup routine for the interrupt endpoints */
    Main::Info("> Generating boot-time vector endpoint initialization routine.");
    Input.push_back("rme_ptr_t Cap_Front - The current capability table frontier.");
    Input.push_back("rme_ptr_t Kom_Front - The current kernel absolute memory frontier.");
    Gen_Tool::Func_Head(List, "RME_Boot_Vct_Init",
                        "Initialize all the vector endpoints at boot-time.",
                        Input, Output, "rme_ptr_t - The kernel memory frontier after booting is all done.");
    Input.clear();
    List->push_back("rme_ptr_t RME_Boot_Vct_Init(struct RME_Cap_Cpt* Cpt, rme_ptr_t Cap_Front, rme_ptr_t Kom_Front)");
    List->push_back("{");
    List->push_back("    rme_ptr_t Cur_Addr;");
    if(Monitor->Vector.size()!=0)
    	List->push_back("    struct RME_Cap_Cpt* Sig_Cpt;");
    List->push_back("");
    List->push_back("    /* The address here shall match what is in the generator */");
    List->push_back(std::string("    RME_ASSERT(Cap_Front==")+std::to_string(Kernel->Vct_Cap_Front)+"U);");
    List->push_back(std::string("    RME_ASSERT(Kom_Front==0x")+Main::Hex(Kernel->Vct_Kom_Front+Kernel->Kom_Base)+"U);");
    List->push_back("");
    List->push_back("    Cur_Addr=Kom_Front;");
    List->push_back("    /* Create all the vector capability tables first */");
    for(Obj_Cnt=0;Obj_Cnt<Monitor->Vector.size();Obj_Cnt+=this->Plat->Plat->Captbl_Max)
    {
        Cpt_Size=CTSIZE(Monitor->Vector.size(), Obj_Cnt);
        List->push_back(std::string("    RME_ASSERT(_RME_Cpt_Boot_Crt(Cpt, RME_BOOT_CPT, RME_MAIN_VCT_")+
                        MIDS(Obj_Cnt)+", Cur_Addr, "+std::to_string(Cpt_Size)+"U)==0U);");
        List->push_back(std::string("    Cur_Addr+=RME_KOM_ROUND(RME_CPT_SIZE(")+std::to_string(Cpt_Size)+"U));");
    }
    List->push_back("");
    List->push_back("    /* Then all the vectors */");
    Obj_Cnt=0;
    for(const class Vect_Info* Vct:Monitor->Vector)
    {
        List->push_back(std::string("    Sig_Cpt=&(RME_CAP_GETOBJ(Cpt,struct RME_Cap_Cpt*)[RME_MAIN_VCT_")+
                        MIDS(Obj_Cnt)+"]);");
        List->push_back(std::string("    ")+Vct->Name+
                        "_Vct_Sig=&(RME_CAP_GETOBJ(Sig_Cpt,struct RME_Cap_Sig*)["+Vct->Macro_Global+"]);");
        List->push_back(std::string("    RME_ASSERT(_RME_Sig_Boot_Crt(Cpt, RME_MAIN_VCT_")+
                        MIDS(Obj_Cnt)+", "+Vct->Macro_Global+")==0U);");
        Obj_Cnt++;
    }
    List->push_back("");
    List->push_back("    return Cur_Addr;");
    List->push_back("}");
    Gen_Tool::Func_Foot(List, "RME_Boot_Vct_Init");
    List->push_back("");

    /* Print the interrupt relaying function */
    Main::Info("> Generating interrupt relay facility.");
    Input.push_back("rme_ptr_t Vct_Num - The vector number.");
    Gen_Tool::Func_Head(List, "RME_Boot_Vct_Handler",
                        "The interrupt handler entry for all the vectors.",
                        Input, Output, "rme_ptr_t - The number of signals to send to the generic vector endpoint.");
    Input.clear();
    List->push_back("rme_ptr_t RME_Boot_Vct_Handler(rme_ptr_t Vct_Num)");
    List->push_back("{");
    List->push_back("    rme_ptr_t Skip;");
    List->push_back("");
    List->push_back("    Skip=0U;");
    List->push_back("");
    List->push_back("    switch(Vct_Num)");
    List->push_back("    {");
    for(const class Vect_Info* Vct:Monitor->Vector)
    {
        List->push_back(std::string("        /* ")+Vct->Name+" */");
        List->push_back(std::string("        case ")+std::to_string(Vct->Number)+"U: ");
        List->push_back("        {");
        List->push_back(std::string("            Skip=RME_Vct_")+Vct->Name+"_Handler();");
        List->push_back(std::string("            _RME_Kern_Snd(")+Vct->Name+"_Vct_Sig);");
        List->push_back("            break;");
        List->push_back("        }");
    }
    List->push_back("        default: break;");
    List->push_back("    }");
    List->push_back("");
    List->push_back("    return Skip;");
    List->push_back("}");
    Gen_Tool::Func_Foot(List, "RME_Boot_Vct_Handler");
    List->push_back("");

    /* Generate rme_boot.c */
    Main::Info("> Writing file.");
    Gen_Tool::Src_Foot(List);
    Gen_Tool::Line_Write(List, Kernel->Boot_Source_Output+"rme_boot.c");
}
/* End Function:Gen_Tool::Kernel_Boot_Src ************************************/

/* Begin Function:Gen_Tool::Kernel_Hook_Src ***********************************
Description : Create the hook source file for kernel.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
void Gen_Tool::Kernel_Hook_Src(void)
{
    class Kernel* Kernel;
    class Monitor* Monitor;
    std::vector<std::string> Input;
    std::vector<std::string> Output;
    std::unique_ptr<std::vector<std::string>> List;

    Kernel=this->Plat->Proj->Kernel.get();
    Monitor=this->Plat->Proj->Monitor.get();
    List=std::make_unique<std::vector<std::string>>();

    /* Does the file already exist? */
    if(std::filesystem::exists(Kernel->Hook_Source_Output+"rme_hook.c")==true)
    {
        /* See if we'll use forced regenerate */
        if(Kernel->Hook_Source_Overwrite==0)
        {
            Main::Info("> File 'rme_hook.c' exists, skipping generation.");
            return;
        }
    }

    /* File header */
    Main::Info("> Generating file header.");
    Gen_Tool::Src_Head(List, "rme_hook.c", "The user initialization hook file.");
    List->push_back("");

    /* Print all header includes */
    Main::Info("> Generating C header includes.");
    this->Kernel_Inc(List);
    List->push_back("");

    /* Print all global prototypes */
    Main::Info("> Generating public C function prototypes.");
    List->push_back("/* Public C Function Prototypes **********************************************/");
    List->push_back("void RME_Boot_Pre_Init(void);");
    List->push_back("void RME_Boot_Post_Init(void);");
    List->push_back("void RME_Reboot_Failsafe(void);");
    List->push_back("rme_ret_t RME_Hook_Kfn_Handler(rme_ptr_t Func_ID, rme_ptr_t Sub_ID, rme_ptr_t Param1, rme_ptr_t Param2);");
    /* Add all the user vector stubs */
    for(const class Vect_Info* Vct:Monitor->Vector)
        List->push_back(std::string("rme_ptr_t RME_Vct_")+Vct->Name+"_Handler(rme_ptr_t Int_Num);");
    List->push_back("/* End Public C Function Prototypes ******************************************/");
    List->push_back("");

    /* Preinitialization of hardware */
    Main::Info("> Generating boot-time pre-initialization stub.");
    Gen_Tool::Func_Head(List, "RME_Boot_Pre_Init",
                        "Initialize critical hardware before any kernel initialization takes place.",
                        Input, Output, "None.");
    List->push_back("void RME_Boot_Pre_Init(void)");
    List->push_back("{");
    List->push_back("    /* Add code here */");
    List->push_back("}");
    Gen_Tool::Func_Foot(List, "RME_Boot_Pre_Init");
    List->push_back("");

    /* Postinitialization of hardware */
    Main::Info("> Generating boot-time post-initialization stub.");
    Gen_Tool::Func_Head(List, "RME_Boot_Post_Init",
                        "Initialize hardware after all kernel initialization took place.",
                        Input, Output, "None.");
    List->push_back("void RME_Boot_Post_Init(void)");
    List->push_back("{");
    List->push_back("    /* Add code here */");
    List->push_back("}");
    Gen_Tool::Func_Foot(List, "RME_Boot_Post_Init");
    List->push_back("");

    /* Kernel function processing */
    Main::Info("> Generating kernel function handler hook.");
    Input.push_back("rme_ptr_t Func_ID - The function ID.");
    Input.push_back("rme_ptr_t Sub_ID - The subfunction ID.");
    Input.push_back("rme_ptr_t Param1 - The first parameter.");
    Input.push_back("rme_ptr_t Param2 - The second parameter.");
    Gen_Tool::Func_Head(List, "RME_Hook_Kfn_Handler",
                        "User-modifiable kernel function handler hook.",
                        Input, Output, "rme_ret_t - The return value.");
    Input.clear();
    List->push_back("rme_ret_t RME_Hook_Kfn_Handler(rme_ptr_t Func_ID, rme_ptr_t Sub_ID, rme_ptr_t Param1, rme_ptr_t Param2)");
    List->push_back("{");
    List->push_back("    /* Add code here */");
    List->push_back("    return RME_ERR_KFN_FAIL;");
    List->push_back("}");
    Gen_Tool::Func_Foot(List, "RME_Hook_Kfn_Handler");
    List->push_back("");

    /* Rebooting */
    Main::Info("> Generating reboot fail-safe handler.");
    Gen_Tool::Func_Head(List, "RME_Reboot_Failsafe",
                        "User-modifiable pre-rebooting failsafe sequence.",
                        Input, Output, "None.");
    List->push_back("void RME_Reboot_Failsafe(void)");
    List->push_back("{");
    List->push_back("    /* Add code here */");
    List->push_back("}");
    Gen_Tool::Func_Foot(List, "RME_Reboot_Failsafe");
    List->push_back("");

    /* Generate rme_init.c */
    Main::Info("> Writing file.");
    Gen_Tool::Src_Foot(List);
    Gen_Tool::Line_Write(List, Kernel->Hook_Source_Output+"rme_hook.c");
}
/* End Function:Gen_Tool::Kernel_Hook_Src ************************************/

/* Begin Function:Gen_Tool::Kernel_Handler_Src ********************************
Description : Create the handler source file for kernel. Each handler gets allocated
              a single source file in rme_handler_xxx.c¡£
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
void Gen_Tool::Kernel_Handler_Src(void)
{
    std::string Filename;
    class Kernel* Kernel;
    class Monitor* Monitor;
    std::vector<std::string> Input;
    std::vector<std::string> Output;
    std::unique_ptr<std::vector<std::string>> List;

    Kernel=this->Plat->Proj->Kernel.get();
    Monitor=this->Plat->Proj->Monitor.get();
    List=std::make_unique<std::vector<std::string>>();

    for(const class Vect_Info* Vct:Monitor->Vector)
    {
        /* Generate filename */
        Filename=std::string("rme_handler_")+Vct->Name_Lower+".c";

        /* Does the file already exist? */
        if(std::filesystem::exists(Kernel->Handler_Source_Output+Filename)==true)
        {
            /* See if we'll use forced regenerate */
            if(Kernel->Handler_Source_Overwrite==0)
            {
                Main::Info(std::string("> File '")+Filename+"' exists, skipping generation.");
                continue;
            }
        }

        /* File header */
        Main::Info("> Generating file header.");
        Gen_Tool::Src_Head(List, "Filename", std::string("The top-half handler file for vector ")+Vct->Name+".");
        List->push_back("");

        /* Print all header includes */
        Main::Info("> Generating C header includes.");
        this->Kernel_Inc(List);
        List->push_back("");

        /* Print all global prototypes */
        Main::Info("> Generating public C function prototypes.");
        List->push_back("/* Public C Function Prototypes **********************************************/");
        List->push_back(std::string("rme_ptr_t RME_Vct_")+Vct->Name+"_Handler(void);");
        List->push_back("/* End Public C Function Prototypes ******************************************/");
        List->push_back("");

        /* Print function body */
        Main::Info(std::string("> Generating vector '")+Vct->Name+"' handler.");
        Gen_Tool::Func_Head(List, std::string("RME_Vct_")+Vct->Name+"_Handler",
                            "The user top-half interrupt handler for vector.",
                            Input, Output, "rme_ptr_t - Return 0 to send to the RME_BOOT_INIT_VCT as well,\n"
                            		       "            return other values to skip.");
        List->push_back(std::string("rme_ptr_t ")+std::string("RME_Vct_")+Vct->Name+"_Handler(void)\n");
        List->push_back("{");
        List->push_back("    /* Add code here */");
        List->push_back("");
        List->push_back("    return 0;");
        List->push_back("}");
        Gen_Tool::Func_Foot(List, std::string("RME_Vct_")+Vct->Name+"_Handler");
        List->push_back("");

        /* Generate rme_init.c */
        Main::Info("> Writing file.");
        Gen_Tool::Src_Foot(List);
        Gen_Tool::Line_Write(List, Kernel->Handler_Source_Output+Filename);
        List->clear();
    }
}
/* End Function:Gen_Tool::Kernel_Handler_Src *********************************/

/* Begin Function:Gen_Tool::Kernel_Linker *************************************
Description : Create the linker script file for kernel.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
void Gen_Tool::Kernel_Linker(void)
{
    class Kernel* Kernel;
    class Tool_Gen* Tool;
    std::unique_ptr<std::vector<std::string>> List;

    Kernel=this->Plat->Proj->Kernel.get();
    List=std::make_unique<std::vector<std::string>>();
    Tool=this->Tool_Map[Kernel->Toolchain];

    Main::Info("> Generating linker script.");
    Kernel->Linker_Filename=std::string("rme_platform_")+this->Plat->Chip->Name_Lower+Tool->Suffix(TOOL_LINKER);
    Tool->Kernel_Linker(List);
    Gen_Tool::Line_Write(List, Kernel->Linker_Output+Kernel->Linker_Filename);
}
/* End Function:Gen_Tool::Kernel_Linker **************************************/

/* Begin Function:Gen_Tool::Kernel_Proj ***************************************
Description : Create the project file for kernel.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
void Gen_Tool::Kernel_Proj(void)
{
    class Kernel* Kernel;
    class Monitor* Monitor;
    class Build_Gen* Build;
    class Tool_Gen* Tool;
    std::unique_ptr<std::vector<std::string>> List;
    std::vector<std::string> Include;
    std::vector<std::string> Source;
    std::vector<std::string> Linker;

    Kernel=this->Plat->Proj->Kernel.get();
    Monitor=this->Plat->Proj->Monitor.get();
    List=std::make_unique<std::vector<std::string>>();
    Build=this->Build_Map[Kernel->Buildsystem];
    Tool=this->Tool_Map[Kernel->Toolchain];

    /* Does the file already exist? */
    Kernel->Project_Filename=std::string("kernel")+Build->Suffix(BUILD_PROJECT);
    if(std::filesystem::exists(Kernel->Project_Output+Kernel->Project_Filename)==true)
    {
        /* See if we'll use forced regenerate */
        if(Kernel->Project_Overwrite==0)
        {
            Main::Info(std::string("> File '")+Kernel->Project_Filename+"' exists, skipping generation.");
            return;
        }
    }

    /* Extract the include paths */
    Main::Info("> Generating project include paths:");

    Include.push_back(Kernel->Project_Output);
    Include.push_back(Kernel->Kernel_Root+"Include/");
    Include.push_back(Kernel->Config_Header_Output);
    Include.push_back(Kernel->Boot_Header_Output);
    Gen_Tool::Path_Conv(Kernel->Project_Output, Include);

    for(const std::string& Path:Include)
        Main::Info(std::string("> > ")+Path);

    /* Extract the source paths */
    Main::Info("> Generating project source paths:");
    /* Regular kernel source */
    Source.push_back(Kernel->Kernel_Root+"Source/Kernel/rme_kernel.c");
    Source.push_back(Kernel->Kernel_Root+"Source/Platform/"+this->Plat->Name+"/rme_platform_"+this->Plat->Name_Lower+".c");
    Source.push_back(Kernel->Kernel_Root+"Source/Platform/"+this->Plat->Name+
                     "/rme_platform_"+this->Plat->Name_Lower+"_"+Tool->Name_Lower+Tool->Suffix(TOOL_ASSEMBLER));
    Source.push_back(Kernel->Boot_Source_Output+"rme_boot.c");
    Source.push_back(Kernel->Hook_Source_Output+"rme_hook.c");
    for(const class Vect_Info* Vct:Monitor->Vector)
        Source.push_back(Kernel->Handler_Source_Output+"rme_handler_"+Vct->Name_Lower+".c");
    /* Other process compartments, if full image generation is required */
    if(Kernel->Project_Full_Image!=0)
    {
        Source.push_back(Monitor->Project_Output+"monitor_image.c");
        for(const std::unique_ptr<class Process>& Prc:this->Plat->Proj->Process)
            Source.push_back(Prc->Project_Output+Prc->Name_Lower+"_image.c");
    }
    Gen_Tool::Path_Conv(Kernel->Project_Output, Source);

    for(const std::string& Path:Source)
        Main::Info(std::string("> > ")+Path);

    /* Extract the linker paths */
    Main::Info("> Generating project linker paths:");

    Linker.push_back(Kernel->Linker_Output+"rme_platform_"+this->Plat->Chip->Name_Lower+Tool->Suffix(TOOL_LINKER));
    Gen_Tool::Path_Conv(Kernel->Project_Output, Linker);

    for(const std::string& Path:Linker)
        Main::Info(std::string("> > ")+Path);

    Build->Kernel_Proj(List, Include, Source, Linker);
    Gen_Tool::Line_Write(List, Kernel->Project_Output+Kernel->Project_Filename);
}
/* End Function:Gen_Tool::Kernel_Proj ****************************************/

/* Begin Function:Gen_Tool::Monitor_Inc ***************************************
Description : Write the include files for monitor.
Input       : std::unique_ptr<std::vector<std::string>>& List - The input file.
Output      : std::unique_ptr<std::vector<std::string>>& List - The appended file.
Return      : None.
******************************************************************************/
void Gen_Tool::Monitor_Inc(std::unique_ptr<std::vector<std::string>>& List)
{
    std::string Temp;

    List->push_back("/* Includes ******************************************************************/");
    List->push_back("#include \"rvm.h\"");
    List->push_back("#include \"rvm_boot.h\"");
    List->push_back("");
    Temp=std::string("#include \"Platform/")+this->Plat->Name+"/rvm_platform_"+this->Plat->Name_Lower+".h\"";
    List->push_back("#define __HDR_DEFS__");
    List->push_back(Temp);
    List->push_back("#include \"Monitor/rvm_syssvc.h\"");
    List->push_back("#include \"Monitor/rvm_init.h\"");
    List->push_back("#include \"Monitor/rvm_hyper.h\"");
    List->push_back("#undef __HDR_DEFS__");
    List->push_back("");
    List->push_back("#define __HDR_STRUCTS__");
    List->push_back(Temp);
    List->push_back("#include \"Monitor/rvm_syssvc.h\"");
    List->push_back("#include \"Monitor/rvm_init.h\"");
    List->push_back("#include \"Monitor/rvm_hyper.h\"");
    List->push_back("#undef __HDR_STRUCTS__");
    List->push_back("");
    List->push_back("#define __HDR_PUBLIC_MEMBERS__");
    List->push_back(Temp);
    List->push_back("#include \"Monitor/rvm_syssvc.h\"");
    List->push_back("#include \"Monitor/rvm_init.h\"");
    List->push_back("#include \"Monitor/rvm_hyper.h\"");
    List->push_back("#undef __HDR_PUBLIC_MEMBERS__");
    List->push_back("/* End Includes **************************************************************/");
}
/* End Function:Gen_Tool::Monitor_Inc ****************************************/

/* Begin Function:Gen_Tool::Monitor_Conf_Hdr **********************************
Description : Generate the platform configuration headers for monitor.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
void Gen_Tool::Monitor_Conf_Hdr(void)
{
    std::string Filename;
    class Kernel* Kernel;
    class Monitor* Monitor;
    std::vector<std::string> Input;
    std::vector<std::string> Output;
    std::unique_ptr<std::vector<std::string>> List;

    Kernel=this->Plat->Proj->Kernel.get();
    Monitor=this->Plat->Proj->Monitor.get();
    List=std::make_unique<std::vector<std::string>>();

    /* Generate rvm_platform.h */
    Main::Info("> Generating 'rvm_platform.h'.");
    Gen_Tool::Src_Head(List, "rvm_platform.h", "The platform selection header.");
    List->push_back("");
    List->push_back("/* Platform Includes *********************************************************/");
    List->push_back(std::string("#include \"Platform/")+this->Plat->Name+"/rvm_platform_"+this->Plat->Name_Lower+".h\"");
    List->push_back("/* End Platform Includes *****************************************************/");
    List->push_back("");
    Gen_Tool::Src_Foot(List);
    Gen_Tool::Line_Write(List, Monitor->Config_Header_Output+"/rvm_platform.h");

    /* Generate rvm_platform_xxx_conf.h */
    Filename=std::string("rvm_platform_")+this->Plat->Name_Lower+"_conf.h";
    Main::Info(std::string("> Generating '")+Filename+"'.");
    List->clear();
    Gen_Tool::Src_Head(List, Filename, "The chip selection header.");
    List->push_back("");
    List->push_back("/* Platform Includes *********************************************************/");
    List->push_back(std::string("#include \"rvm_platform_")+this->Plat->Chip->Name_Lower+".h\"");
    List->push_back("/* End Platform Includes *****************************************************/");
    List->push_back("");
    Gen_Tool::Src_Foot(List);
    Gen_Tool::Line_Write(List, Monitor->Config_Header_Output+"rvm_platform_"+this->Plat->Name_Lower+"_conf.h");


    /* Generate rvm_platform_chipname.h */
    Filename=std::string("rvm_platform_")+this->Plat->Chip->Name_Lower+".h";
    Main::Info(std::string("> Generating '")+Filename+"'.");
    List=Gen_Tool::Line_Read(Monitor->Monitor_Root+
                             "Include/Platform/"+this->Plat->Name+"/Chip/"+this->Plat->Chip->Name+"/"+Filename);
    /* Replace general parameter macros */
    /* Debugging control */
    Gen_Tool::Macro_Int(List, "RVM_ASSERT_CORRECT", this->Plat->Proj->Assert_Correct, MACRO_REPLACE);
    Gen_Tool::Macro_Int(List, "RVM_DEBUG_PRINT", this->Plat->Proj->Debug_Print, MACRO_REPLACE);
    /* The virtual memory base/size for the kernel objects */
    Gen_Tool::Macro_Hex(List, "RVM_KOM_VA_BASE", Kernel->Kom_Base, MACRO_REPLACE);
    Gen_Tool::Macro_Hex(List, "RVM_KOM_VA_SIZE", Kernel->Kom_Size, MACRO_REPLACE);
    /* The granularity of kernel memory allocation, in bytes */
    Gen_Tool::Macro_Int(List, "RVM_KOM_SLOT_ORDER", Kernel->Kom_Order, MACRO_REPLACE);
    /* The maximum number of preemption priority levels in the system.
     * This parameter must be divisible by the word length - 32 is usually sufficient */
    Gen_Tool::Macro_Int(List, "RVM_PREEMPT_PRIO_NUM", Kernel->Kern_Prio, MACRO_REPLACE);
    /* Number of virtual priorities in the system */
    Gen_Tool::Macro_Int(List, "RVM_PREEMPT_VPRIO_NUM", Monitor->Virt_Prio, MACRO_REPLACE);

    /* Physical vector number, flag area base and its size */
    Gen_Tool::Macro_Int(List, "RVM_PHYS_VCT_NUM", this->Plat->Chip->Vect_Num, MACRO_REPLACE);
    Gen_Tool::Macro_Hex(List, "RVM_PHYS_VCTF_BASE", Kernel->Vctf_Base, MACRO_REPLACE);
    Gen_Tool::Macro_Hex(List, "RVM_PHYS_VCTF_SIZE", Kernel->Vctf_Size, MACRO_REPLACE);
    /* Virtual event number, flag area base and its size */
    Gen_Tool::Macro_Int(List, "RVM_VIRT_EVT_NUM", Monitor->Virt_Event, MACRO_REPLACE);
    Gen_Tool::Macro_Hex(List, "RVM_VIRT_EVTF_BASE", Kernel->Evtf_Base, MACRO_REPLACE);
    Gen_Tool::Macro_Hex(List, "RVM_VIRT_EVTF_SIZE", Kernel->Evtf_Size, MACRO_REPLACE);
    /* Maximum number of mappings allowed */
    Gen_Tool::Macro_Int(List, "RVM_VIRT_MAP_NUM", Monitor->Virt_Map, MACRO_REPLACE);

    /* Stack base and size of the daemon threads, in bytes */
    Gen_Tool::Macro_Hex(List, "RVM_SFTD_STACK_BASE", Monitor->Sftd_Stack_Base, MACRO_REPLACE);
    Gen_Tool::Macro_Hex(List, "RVM_SFTD_STACK_SIZE", Monitor->Sftd_Stack_Size, MACRO_REPLACE);
    Gen_Tool::Macro_Hex(List, "RVM_TIMD_STACK_BASE", Monitor->Timd_Stack_Base, MACRO_REPLACE);
    Gen_Tool::Macro_Hex(List, "RVM_TIMD_STACK_SIZE", Monitor->Timd_Stack_Size, MACRO_REPLACE);
    Gen_Tool::Macro_Hex(List, "RVM_VMMD_STACK_BASE", Monitor->Hypd_Stack_Base, MACRO_REPLACE);
    Gen_Tool::Macro_Hex(List, "RVM_VMMD_STACK_SIZE", Monitor->Hypd_Stack_Size, MACRO_REPLACE);
    Gen_Tool::Macro_Hex(List, "RVM_VCTD_STACK_BASE", Monitor->Vctd_Stack_Base, MACRO_REPLACE);
    Gen_Tool::Macro_Hex(List, "RVM_VCTD_STACK_SIZE", Monitor->Vctd_Stack_Size, MACRO_REPLACE);
    /* Initial capability frontier limit */
    Gen_Tool::Macro_Int(List, "RVM_CAP_BOOT_FRONT", Monitor->Before_Cap_Front, MACRO_REPLACE);
    /* Initial kernel memory frontier limit */
    Gen_Tool::Macro_Hex(List, "RVM_KOM_BOOT_FRONT", Monitor->Before_Kom_Front, MACRO_REPLACE);

    /* Stack safety redundancy, in bytes - fixed to 16 words */
    Gen_Tool::Macro_Int(List, "RVM_STACK_SAFE_RDCY", 16, MACRO_REPLACE);

    /* Replace platform specific macros */
    this->Plat->Monitor_Conf_Hdr(List);
    /* Write to Monitor configuration file */
    Gen_Tool::Line_Write(List, Monitor->Config_Header_Output+Filename);
}
/* End Function:Gen_Tool::Monitor_Conf_Hdr ***********************************/

/* Begin Function:Gen_Tool::Monitor_Cpt_Init **********************************
Description : Build capability table contents.
Input       : std::unique_ptr<std::vector<std::string>>& List - The file to add these to.
              ptr_t Is_Kfn - Whether this iteration is kfunc dedicated.
Output      : None.
Return      : ptr_t - The number of operations.
******************************************************************************/
ptr_t Gen_Tool::Monitor_Cpt_Init(std::unique_ptr<std::vector<std::string>>& List, ptr_t Is_Kfn)
{
    ptr_t Total;

    Total=0;

    for(const std::unique_ptr<class Process>& Prc:this->Plat->Proj->Process)
    {
        if(Is_Kfn==1)
        {
            /* For native processes, initialize the slot 0 as kernel function capability for sending events to the RVM's Vctd */
            if(Prc->Type==PROCESS_NATIVE)
            {
                List->push_back(std::string("{")+Prc->Captbl->Macro_Global+", 0U, RVM_KFN_EVT_LOCAL_TRIG, RVM_KFN_EVT_LOCAL_TRIG},");
                Total++;
            }

            /* Kernel functions */
            for(const std::unique_ptr<class Kfunc>& Kfn:Prc->Kfunc)
            {
                List->push_back(std::string("{")+Prc->Captbl->Macro_Global+", "+std::to_string(Kfn->Cid_Local)+"U, 0x"+
                                Main::Hex(Kfn->Start)+"U, 0x"+Main::Hex(Kfn->End)+"U},");
                Total++;
            }
        }
        else
        {
            /* Ports */
            for(std::unique_ptr<class Port>& Port:Prc->Port)
            {
                List->push_back(std::string("{")+Prc->Captbl->Macro_Global+", "+std::to_string(Port->Cid_Local)+"U, RVM_MAIN_INV_"+
                                MIDS(Port->Cid_Global)+", "+SIDS(Port->Cid_Global)+"U, RVM_INV_FLAG_ACT},");
                Total++;
            }
            /* Receive endpoints */
            for(const std::unique_ptr<class Receive>& Rcv:Prc->Receive)
            {
                List->push_back(std::string("{")+Prc->Captbl->Macro_Global+", "+std::to_string(Rcv->Cid_Local)+"U, RVM_MAIN_RCV_"+
                                MIDS(Rcv->Cid_Global)+", "+SIDS(Rcv->Cid_Global)+"U, RVM_SIG_FLAG_SND|RVM_SIG_FLAG_RCV},");
                Total++;
            }
            /* Send endpoints */
            for(const std::unique_ptr<class Send>& Snd:Prc->Send)
            {
                List->push_back(std::string("{")+Prc->Captbl->Macro_Global+", "+std::to_string(Snd->Cid_Local)+"U, RVM_MAIN_RCV_"+
                                MIDS(Snd->Cid_Global)+", "+SIDS(Snd->Cid_Global)+"U, RVM_SIG_FLAG_SND},");
                Total++;
            }
            /* Vector endpoints */
            for(const std::unique_ptr<class Vect_Info>& Vct:Prc->Vector)
            {
                List->push_back(std::string("{")+Prc->Captbl->Macro_Global+", "+std::to_string(Vct->Cid_Local)+"U, RVM_MAIN_VCT_"+
                                MIDS(Vct->Cid_Global)+", "+SIDS(Vct->Cid_Global)+"U, RVM_SIG_FLAG_RCV},");
                Total++;
            }
        }
    }

    return Total;
}
/* End Function:Gen_Tool::Monitor_Cpt_Init ***********************************/

/* Begin Function:Gen_Tool::Monitor_Pgt_Con ***********************************
Description : Construct the page table for monitor. This will produce the desired
              final page table tree, and is recursive.
Input       : std::unique_ptr<std::vector<std::string>>& List - The file to add these to.
              const class Pgtbl* Pgt - The page table structure.
Output      : None.
Return      : ptr_t - The number of operations.
******************************************************************************/
ptr_t Gen_Tool::Monitor_Pgt_Con(std::unique_ptr<std::vector<std::string>>& List, const class Pgtbl* Pgt)
{
    ptr_t Count;
    ptr_t Total;
    class Pgtbl* Child;

    Total=0;

    /* Construct whatever page table to this page table */
    for(Count=0;Count<POW2(Pgt->Num_Order);Count++)
    {
        Child=Pgt->Pgdir[Count].get();
        if(Child==nullptr)
            continue;

        List->push_back(std::string("{")+Pgt->Macro_Global+", "+Child->Macro_Global+", 0x"+Main::Hex(Count)+"U},");
        Total++;

        /* Recursively call this for all the page tables */
        Total+=this->Monitor_Pgt_Con(List, Child);
    }

    return Total;
}
/* End Function:Gen_Tool::Monitor_Pgt_Con ************************************/

/* Begin Function:Gen_Tool::Monitor_Pgt_Add ***********************************
Description : Map pages into a page table. This is not recursive.
Input       : std::unique_ptr<std::vector<std::string>>& List - The file to add these to.
              const class Pgtbl* Pgt - The page table structure.
              ptr_t Init_Size_Ord - The initial page table's size order.
Output      : None.
Return      : ptr_t - The number of operations.
******************************************************************************/
ptr_t Gen_Tool::Monitor_Pgt_Add(std::unique_ptr<std::vector<std::string>>& List,
                                  const class Pgtbl* Pgt, ptr_t Init_Size_Ord)
{
    ptr_t Count;
    ptr_t Attr;
    ptr_t Pos_Src;
    ptr_t Index;
    ptr_t Page_Start;
    ptr_t Page_Size;
    ptr_t Page_Num;
    ptr_t Init_Size;
    ptr_t Total;
    std::string Flags;

    Page_Size=POW2(Pgt->Size_Order);
    Page_Num=POW2(Pgt->Num_Order);
    Init_Size=POW2(Init_Size_Ord);
    Total=0;

    /* Map whatever pages into this page table */
    for(Count=0;Count<Page_Num;Count++)
    {
        Attr=Pgt->Page[Count];
        if(Attr==0)
            continue;
        /* Compute flags */
        if((Attr&MEM_READ)!=0)
            Flags+="RVM_PGT_READ|";
        if((Attr&MEM_WRITE)!=0)
            Flags+="RVM_PGT_WRITE|";
        if((Attr&MEM_EXECUTE)!=0)
            Flags+="RVM_PGT_EXECUTE|";
        if((Attr&MEM_CACHE)!=0)
            Flags+="RVM_PGT_CACHE|";
        if((Attr&MEM_BUFFER)!=0)
            Flags+="RVM_PGT_BUFFER|";
        if((Attr&MEM_STATIC)!=0)
            Flags+="RVM_PGT_STATIC|";
        /* Pop off the final | */
        Flags.pop_back();
        /* Compute Pos_Src and Index */
        Page_Start=Pgt->Base+Count*Page_Size;
        Pos_Src=Page_Start/Init_Size;
        Index=(Page_Start-Pos_Src*Init_Size)/Page_Size;
        /* Push to list */
        List->push_back(std::string("{")+Pgt->Macro_Global+", 0x"+Main::Hex(Count)+"U,");
        List->push_back(std::string(" ")+Flags+",");
        List->push_back(std::string(" 0x")+Main::Hex(Pos_Src)+"U, 0x"+Main::Hex(Index)+"U},");
        Total++;
    }

    return Total;
}
/* End Function:Gen_Tool::Monitor_Pgt_Add ************************************/

/* Begin Function:Gen_Tool::Monitor_Thd_Init **********************************
Description : Initialize threads.
Input       : std::unique_ptr<std::vector<std::string>>& List - The file to add these to.
Output      : None.
Return      : ptr_t - The number of operations.
******************************************************************************/
ptr_t Gen_Tool::Monitor_Thd_Init(std::unique_ptr<std::vector<std::string>>& List)
{
    ptr_t Total;
    std::string Line;

    Total=0;

	for(const std::unique_ptr<class Process>& Prc:this->Plat->Proj->Process)
	{
		for(const std::unique_ptr<class Thread>& Thd:Prc->Thread)
		{
			Line=std::string("{")+Thd->Macro_Global+", ";

			/* The marker is non-zero only for virtual machine threads */
			if(Prc->Type==PROCESS_VIRTUAL)
				Line+="RVM_VIRT_TID_MARKER, ";
			else
				Line+="0U, ";

			Line+="0x"+Main::Hex(Prc->Code_Base)+"U, ";
			Line+=std::to_string(Thd->Desc_Slot)+"U, ";
			Line+="0x"+Main::Hex(Thd->Stack_Base)+"U, ";
			Line+="0x"+Main::Hex(Thd->Stack_Size)+"U, ";
			Line+="0x"+Main::Hex(Thd->Parameter)+"U, ";
			Line+=std::to_string(Thd->Priority)+"U, ";

			/* If this process is a VM and we are dealing with its user thread, we need to have its HYP set */
			if((Prc->Type==PROCESS_VIRTUAL)&&(Thd->Name=="Usr"))
				Line+="0x"+Main::Hex(static_cast<class Virtual*>(Prc.get())->Reg_Base)+"U},";
			else
				Line+="0U},";

			List->push_back(Line);
			Total++;
		}
	}

	return Total;
}
/* End Function:Gen_Tool::Monitor_Thd_Init ***********************************/

/* Begin Function:Gen_Tool::Monitor_Inv_Init **********************************
Description : Initialize invocations.
Input       : std::unique_ptr<std::vector<std::string>>& List - The file to add these to.
Output      : None.
Return      : ptr_t - The number of operations.
******************************************************************************/
ptr_t Gen_Tool::Monitor_Inv_Init(std::unique_ptr<std::vector<std::string>>& List)
{
    ptr_t Total;
    std::string Line;

    Total=0;

	for(const std::unique_ptr<class Process>& Prc:this->Plat->Proj->Process)
	{
        for(const std::unique_ptr<class Invocation>& Inv:Prc->Invocation)
		{
			Line=std::string("{")+Inv->Macro_Global+", ";
			Line+="0x"+Main::Hex(Prc->Code_Base)+"U, ";
			Line+=std::to_string(Inv->Desc_Slot)+"U, ";
			Line+="0x"+Main::Hex(Inv->Stack_Base)+"U, ";
			Line+="0x"+Main::Hex(Inv->Stack_Size)+"U},";

			List->push_back(Line);
			Total++;
		}
	}

	return Total;
}
/* End Function:Gen_Tool::Monitor_Inv_Init ***********************************/

/* Begin Function:Gen_Tool::Monitor_Boot_Hdr **********************************
Description : Create the boot header for monitor.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
void Gen_Tool::Monitor_Boot_Hdr(void)
{
    ptr_t Obj_Cnt;
    ptr_t Cap_Front;
    ptr_t Total;
    class Kernel* Kernel;
    class Monitor* Monitor;
    std::unique_ptr<std::vector<std::string>> List;
    std::unique_ptr<std::vector<std::string>> Dummy;

    Kernel=this->Plat->Proj->Kernel.get();
    Monitor=this->Plat->Proj->Monitor.get();
    List=std::make_unique<std::vector<std::string>>();
    Dummy=std::make_unique<std::vector<std::string>>();

    Main::Info("> Generating file header.");
    Gen_Tool::Src_Head(List, "rvm_boot.h", "The boot-time initialization file header.");
    List->push_back("");

    Main::Info("> Generating defines.");
    List->push_back("/* Defines *******************************************************************/");

    /* Vector capability tables & objects */
    List->push_back("/* Vector endpoint capability tables */");
    Cap_Front=Kernel->Vct_Cap_Front;
    for(Obj_Cnt=0;Obj_Cnt<Monitor->Vector.size();Obj_Cnt+=this->Plat->Plat->Captbl_Max)
    {
        Gen_Tool::Macro_Int(List, std::string("RVM_MAIN_VCT_")+MIDS(Obj_Cnt), Cap_Front, MACRO_ADD);
        Cap_Front++;
    }
    List->push_back("");
    List->push_back("/* Vector endpoints */");
    for(const class Vect_Info* Vct:Monitor->Vector)
    {
        Gen_Tool::Macro_String(List, Vct->Macro_Global,
                               std::string("RVM_CID(RVM_MAIN_VCT_")+
                               MIDS(Vct->Cid_Global)+", "+SIDS(Vct->Cid_Global)+"U)", MACRO_ADD);
    }
    List->push_back("");

    /* These are reserved for RVM kernel objects */
    List->push_back("/* 4 threads and 2 endpoints for RVM */");
    List->push_back("");

    /* Virual machine endpoint capability tables */
    if(this->Plat->Proj->Virtual.size()!=0)
    {
        List->push_back("/* Virtual machine endpoint capability tables */");
        Cap_Front=Monitor->Vep_Cap_Front;
        for(Obj_Cnt=0;Obj_Cnt<this->Plat->Proj->Virtual.size();Obj_Cnt+=this->Plat->Plat->Captbl_Max)
        {
            Gen_Tool::Macro_Int(List, std::string("RVM_MAIN_VEP_")+MIDS(Obj_Cnt), Cap_Front, MACRO_ADD);
            Cap_Front++;
        }
        if(Cap_Front!=Monitor->Cpt_Cap_Front)
            Main::Error("XXXXX: Virtual machine capability table computation failure.");
        List->push_back("");
    }

    /* Cpt capability tables & objects */
    List->push_back("/* Process capability table capability tables */");
    Cap_Front=Monitor->Cpt_Cap_Front;
    for(Obj_Cnt=0;Obj_Cnt<Monitor->Captbl.size();Obj_Cnt+=this->Plat->Plat->Captbl_Max)
    {
        Gen_Tool::Macro_Int(List, std::string("RVM_MAIN_CPT_")+MIDS(Obj_Cnt), Cap_Front, MACRO_ADD);
        Cap_Front++;
    }
    if(Cap_Front!=Monitor->Pgt_Cap_Front)
        Main::Error("XXXXX: Cpt capability table computation failure.");
    List->push_back("");
    List->push_back("/* Process capability tables */");
    for(const class Captbl* Cpt:Monitor->Captbl)
    {
        Gen_Tool::Macro_String(List, Cpt->Macro_Global,
                               std::string("RVM_CID(RVM_MAIN_CPT_")+
                               MIDS(Cpt->Cid_Global)+", "+SIDS(Cpt->Cid_Global)+"U)", MACRO_ADD);
    }
    List->push_back("");

    /* Pgt capability tables & objects */
    List->push_back("/* Process page table capability tables */");
    Cap_Front=Monitor->Pgt_Cap_Front;
    for(Obj_Cnt=0;Obj_Cnt<Monitor->Pgtbl.size();Obj_Cnt+=this->Plat->Plat->Captbl_Max)
    {
        Gen_Tool::Macro_Int(List, std::string("RVM_MAIN_PGT_")+MIDS(Obj_Cnt), Cap_Front, MACRO_ADD);
        Cap_Front++;
    }
    if(Cap_Front!=Monitor->Prc_Cap_Front)
        Main::Error("XXXXX: Pgt capability table computation failure.");
    List->push_back("");
    List->push_back("/* Process page tables */");
    for(const class Pgtbl* Pgt:Monitor->Pgtbl)
    {
        Gen_Tool::Macro_String(List, Pgt->Macro_Global,
                               std::string("RVM_CID(RVM_MAIN_PGT_")+
                               MIDS(Pgt->Cid_Global)+", "+SIDS(Pgt->Cid_Global)+"U)", MACRO_ADD);
    }
    List->push_back("");

    /* Process capability tables & objects */
    List->push_back("/* Process capability tables */");
    Cap_Front=Monitor->Prc_Cap_Front;
    for(Obj_Cnt=0;Obj_Cnt<Monitor->Process.size();Obj_Cnt+=this->Plat->Plat->Captbl_Max)
    {
        Gen_Tool::Macro_Int(List, std::string("RVM_MAIN_PRC_")+MIDS(Obj_Cnt), Cap_Front, MACRO_ADD);
        Cap_Front++;
    }
    if(Cap_Front!=Monitor->Thd_Cap_Front)
        Main::Error("XXXXX: Process capability table computation failure.");
    List->push_back("");
    List->push_back("/* Processes */");
    for(const class Process* Prc:Monitor->Process)
    {
        Gen_Tool::Macro_String(List, Prc->Macro_Global,
                               std::string("RVM_CID(RVM_MAIN_PRC_")+
                               MIDS(Prc->Cid_Global)+", "+ SIDS(Prc->Cid_Global)+"U)", MACRO_ADD);
    }
    List->push_back("");

    /* Thread capability tables & objects */
    List->push_back("/* Thread capability tables */");
    Cap_Front=Monitor->Thd_Cap_Front;
    for(Obj_Cnt=0;Obj_Cnt<Monitor->Thread.size();Obj_Cnt+=this->Plat->Plat->Captbl_Max)
    {
        Gen_Tool::Macro_Int(List, std::string("RVM_MAIN_THD_")+MIDS(Obj_Cnt), Cap_Front, MACRO_ADD);
        Cap_Front++;
    }
    if(Cap_Front!=Monitor->Inv_Cap_Front)
        Main::Error("XXXXX: Thread capability table computation failure.");
    List->push_back("");
    List->push_back("/* Threads */");
    for(const class Thread* Thd:Monitor->Thread)
    {
        Gen_Tool::Macro_String(List, Thd->Macro_Global,
                               std::string("RVM_CID(RVM_MAIN_THD_")+
                               MIDS(Thd->Cid_Global)+", "+SIDS(Thd->Cid_Global)+"U)", MACRO_ADD);
    }
    List->push_back("");

    /* Invocation capability tables & objects */
    List->push_back("/* Invocation capability tables */");
    Cap_Front=Monitor->Inv_Cap_Front;
    for(Obj_Cnt=0;Obj_Cnt<Monitor->Invocation.size();Obj_Cnt+=this->Plat->Plat->Captbl_Max)
    {
        Gen_Tool::Macro_Int(List, std::string("RVM_MAIN_INV_")+MIDS(Obj_Cnt), Cap_Front, MACRO_ADD);
        Cap_Front++;
    }
    if(Cap_Front!=Monitor->Rcv_Cap_Front)
        Main::Error("XXXXX: Invocation capability table computation failure.");
    List->push_back("");
    List->push_back("/* Invocations */");
    for(const class Invocation* Inv:Monitor->Invocation)
    {
        Gen_Tool::Macro_String(List, Inv->Macro_Global,
                               std::string("RVM_CID(RVM_MAIN_INV_")+
                               MIDS(Inv->Cid_Global)+", "+SIDS(Inv->Cid_Global)+"U)", MACRO_ADD);
    }
    List->push_back("");

    /* Receive endpoint capability tables & objects */
    List->push_back("/* Receive endpoint capability tables */");
    Cap_Front=Monitor->Rcv_Cap_Front;
    for(Obj_Cnt=0;Obj_Cnt<Monitor->Receive.size();Obj_Cnt+=this->Plat->Plat->Captbl_Max)
    {
        Gen_Tool::Macro_Int(List, std::string("RVM_MAIN_RCV_")+MIDS(Obj_Cnt), Cap_Front, MACRO_ADD);
        Cap_Front++;
    }
    if(Cap_Front!=Monitor->After_Cap_Front)
        Main::Error("XXXXX: Receive endpoint capability table computation failure.");
    List->push_back("");
    List->push_back("/* Receive endpoints */");
    for(const class Receive* Rcv:Monitor->Receive)
    {
        Gen_Tool::Macro_String(List, Rcv->Macro_Global,
                               std::string("RVM_CID(RVM_MAIN_RCV_")+
                               MIDS(Rcv->Cid_Global)+", "+SIDS(Rcv->Cid_Global)+"U)", MACRO_ADD);
    }
    List->push_back("");

    /* Extra capability table frontier */
    List->push_back("/* Capability table frontier */");
    Gen_Tool::Macro_Int(List, "RVM_BOOT_CAP_FRONTIER", Monitor->After_Cap_Front, MACRO_ADD);
    List->push_back("");
    /* Extra kernel memory frontier */
    List->push_back("/* Kernel memory frontier */");
    Gen_Tool::Macro_Hex(List, "RVM_BOOT_KOM_FRONTIER", Monitor->After_Kom_Front, MACRO_ADD);
    List->push_back("");
    /* Number of virtual machines */
    List->push_back("/* VM number */");
    Gen_Tool::Macro_Int(List, "RVM_VIRT_NUM", this->Plat->Proj->Virtual.size(), MACRO_ADD);
    List->push_back("");

    /* Other frontiers */
    List->push_back("/* Virtual endpoint frontiers & number */");
    Gen_Tool::Macro_Hex(List, "RVM_BOOT_VEP_BEFORE", Monitor->Vep_Kom_Front, MACRO_ADD);
    Gen_Tool::Macro_Hex(List, "RVM_BOOT_VEP_AFTER", Monitor->Cpt_Kom_Front, MACRO_ADD);
    Gen_Tool::Macro_Int(List, "RVM_BOOT_VEP_MAIN_NUM", CTNUM(this->Plat->Proj->Virtual.size()), MACRO_ADD);
    Gen_Tool::Macro_Int(List, "RVM_BOOT_VEP_CRT_NUM", this->Plat->Proj->Virtual.size(), MACRO_ADD);
    Gen_Tool::Macro_Int(List, "RVM_BOOT_VCAP_INIT_NUM", this->Plat->Proj->Virtual.size(), MACRO_ADD);
    List->push_back("");

    List->push_back("/* Cpt frontiers & number */");
    Gen_Tool::Macro_Hex(List, "RVM_BOOT_CPT_BEFORE", Monitor->Cpt_Kom_Front, MACRO_ADD);
    Gen_Tool::Macro_Hex(List, "RVM_BOOT_CPT_AFTER", Monitor->Pgt_Kom_Front, MACRO_ADD);
    Gen_Tool::Macro_Int(List, "RVM_BOOT_CPT_MAIN_NUM", CTNUM(Monitor->Captbl.size()), MACRO_ADD);
    Gen_Tool::Macro_Int(List, "RVM_BOOT_CPT_CRT_NUM", Monitor->Captbl.size(), MACRO_ADD);
    this->Cpt_Init_Total=this->Monitor_Cpt_Init(Dummy, 0);
    Gen_Tool::Macro_Int(List, "RVM_BOOT_CPT_INIT_NUM", this->Cpt_Init_Total, MACRO_ADD);
    this->Cpt_Kfn_Total=this->Monitor_Cpt_Init(Dummy, 1);
    Gen_Tool::Macro_Int(List, "RVM_BOOT_CPT_KFN_NUM", this->Cpt_Kfn_Total, MACRO_ADD);
    List->push_back("");

    List->push_back("/* Pgt frontiers & number */");
    Gen_Tool::Macro_Hex(List, "RVM_BOOT_PGT_BEFORE", Monitor->Pgt_Kom_Front, MACRO_ADD);
    Gen_Tool::Macro_Hex(List, "RVM_BOOT_PGT_AFTER", Monitor->Prc_Kom_Front, MACRO_ADD);
    Gen_Tool::Macro_Int(List, "RVM_BOOT_PGT_MAIN_NUM", CTNUM(Monitor->Pgtbl.size()), MACRO_ADD);
    Gen_Tool::Macro_Int(List, "RVM_BOOT_PGT_CRT_NUM", Monitor->Pgtbl.size(), MACRO_ADD);
    Total=0;
    for(const std::unique_ptr<class Process>& Prc:this->Plat->Proj->Process)
        Total+=this->Monitor_Pgt_Con(Dummy, Prc->Pgtbl.get());
    this->Pgt_Con_Total=Total;
    Gen_Tool::Macro_Int(List, "RVM_BOOT_PGT_CON_NUM", Total, MACRO_ADD);
    Total=0;
    for(class Pgtbl* Pgt:Monitor->Pgtbl)
        Total+=this->Monitor_Pgt_Add(Dummy, Pgt, this->Plat->Plat->Wordlength);
    Gen_Tool::Macro_Int(List, "RVM_BOOT_PGT_ADD_NUM", Total, MACRO_ADD);
    List->push_back("");

    List->push_back("/* Process frontiers & number */");
    Gen_Tool::Macro_Hex(List, "RVM_BOOT_PRC_BEFORE", Monitor->Prc_Kom_Front, MACRO_ADD);
    Gen_Tool::Macro_Hex(List, "RVM_BOOT_PRC_AFTER", Monitor->Thd_Kom_Front, MACRO_ADD);
    Gen_Tool::Macro_Int(List, "RVM_BOOT_PRC_MAIN_NUM", CTNUM(Monitor->Process.size()), MACRO_ADD);
    Gen_Tool::Macro_Int(List, "RVM_BOOT_PRC_CRT_NUM", Monitor->Process.size(), MACRO_ADD);
    List->push_back("");

    List->push_back("/* Thread frontiers & number */");
    Gen_Tool::Macro_Hex(List, "RVM_BOOT_THD_BEFORE", Monitor->Thd_Kom_Front, MACRO_ADD);
    Gen_Tool::Macro_Hex(List, "RVM_BOOT_THD_AFTER", Monitor->Inv_Kom_Front, MACRO_ADD);
    Gen_Tool::Macro_Int(List, "RVM_BOOT_THD_MAIN_NUM", CTNUM(Monitor->Thread.size()), MACRO_ADD);
    Gen_Tool::Macro_Int(List, "RVM_BOOT_THD_CRT_NUM", Monitor->Thread.size(), MACRO_ADD);
    Gen_Tool::Macro_Int(List, "RVM_BOOT_THD_INIT_NUM", Monitor->Thread.size(), MACRO_ADD);
    ASSERT(Monitor->Thread.size()==this->Monitor_Thd_Init(Dummy));
    List->push_back("");

    List->push_back("/* Invocation frontiers & number */");
    Gen_Tool::Macro_Hex(List, "RVM_BOOT_INV_BEFORE", Monitor->Inv_Kom_Front, MACRO_ADD);
    Gen_Tool::Macro_Hex(List, "RVM_BOOT_INV_AFTER", Monitor->Rcv_Kom_Front, MACRO_ADD);
    Gen_Tool::Macro_Int(List, "RVM_BOOT_INV_MAIN_NUM", CTNUM(Monitor->Invocation.size()), MACRO_ADD);
    Gen_Tool::Macro_Int(List, "RVM_BOOT_INV_CRT_NUM", Monitor->Invocation.size(), MACRO_ADD);
    Gen_Tool::Macro_Int(List, "RVM_BOOT_INV_INIT_NUM", Monitor->Invocation.size(), MACRO_ADD);
    ASSERT(Monitor->Invocation.size()==this->Monitor_Inv_Init(Dummy));
    List->push_back("");

    List->push_back("/* Receive endpoint frontiers & number */");
    Gen_Tool::Macro_Hex(List, "RVM_BOOT_RCV_BEFORE", Monitor->Rcv_Kom_Front, MACRO_ADD);
    Gen_Tool::Macro_Hex(List, "RVM_BOOT_RCV_AFTER", Monitor->After_Kom_Front, MACRO_ADD);
    Gen_Tool::Macro_Int(List, "RVM_BOOT_RCV_MAIN_NUM", CTNUM(Monitor->Receive.size()), MACRO_ADD);
    Gen_Tool::Macro_Int(List, "RVM_BOOT_RCV_CRT_NUM", Monitor->Receive.size(), MACRO_ADD);
    List->push_back("");
    List->push_back("/* End Defines ***************************************************************/");
    List->push_back("");

    Main::Info("> Writing file.");
    Gen_Tool::Src_Foot(List);
    Gen_Tool::Line_Write(List, Monitor->Boot_Header_Output+"rvm_boot.h");
}
/* End Function:Gen_Tool::Monitor_Boot_Hdr ***********************************/

/* Begin Function:Gen_Tool::Monitor_Main_Crt **********************************
Description : Generate the main container capability tables.
Input       : std::unique_ptr<std::vector<std::string>>& List - The file to add these to.
              ptr_t Number - The total number of slots that the captbls should have.
              const std::string& Macro - The macro infix of these captbls.
Output      : None.
Return      : None.
******************************************************************************/
void Gen_Tool::Monitor_Main_Crt(std::unique_ptr<std::vector<std::string>>& List,
                                ptr_t Number, const std::string& Macro)
{
    ptr_t Obj_Cnt;
    ptr_t Cpt_Size;

    for(Obj_Cnt=0;Obj_Cnt<Number;Obj_Cnt+=this->Plat->Plat->Captbl_Max)
    {
        Cpt_Size=CTSIZE(Number, Obj_Cnt);
        List->push_back(std::string("{RVM_MAIN_")+Macro+"_"+
                        std::to_string(Obj_Cnt/this->Plat->Plat->Captbl_Max)+", "+std::to_string(Cpt_Size)+"U},");
    }
}
/* End Function:Gen_Tool::Monitor_Main_Crt ***********************************/

/* Begin Function:Gen_Tool::Monitor_Boot_Src **********************************
Description : Create the boot source for monitor. This function is pretty lengthy,
              but it generates a single source, so we're not splitting it.
              We're using metadata for most of the initializations. For those that
              are particularly entangled, we opt to use a customized function.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
void Gen_Tool::Monitor_Boot_Src(void)
{
    ptr_t Obj_Cnt;
    class Monitor* Monitor;
    std::vector<std::string> Input;
    std::vector<std::string> Output;
    std::unique_ptr<std::vector<std::string>> List;

    Monitor=this->Plat->Proj->Monitor.get();
    List=std::make_unique<std::vector<std::string>>();

    /* File header */
    Main::Info("> Generating file header.");
    Gen_Tool::Src_Head(List, "rvm_boot.c", "The boot-time initialization file.");
    List->push_back("");

    /* Print all header includes */
    Main::Info("> Generating C header includes.");
    this->Monitor_Inc(List);
    List->push_back("");

    Main::Info("> Generating public global variables.");
    List->push_back("/* Public Global Variables ***************************************************/");
    /* Virtual machine management metadata */
    if(this->Plat->Proj->Virtual.size()!=0)
    {
        List->push_back("/* Virtual machine management metadata */");
        List->push_back("struct RVM_Virt_Struct RVM_Virt[RVM_VIRT_NUM];");
        List->push_back("const struct RVM_Vmap_Struct RVM_Vmap[RVM_VIRT_NUM]=");
        List->push_back("{");
        Obj_Cnt=0;
        for(class Virtual* Virt:this->Plat->Proj->Virtual)
        {
            List->push_back(std::string("{(rvm_s8_t*)\"")+Virt->Name+"\", "+
                            std::to_string(Virt->Priority)+"U, "+
                            std::to_string(Virt->Timeslice)+"U, "+
                            std::to_string(Virt->Period)+"U, "+
                            std::to_string(Virt->Watchdog)+"U, "+
                            std::to_string(Virt->Vector_Num)+"U, ");
            List->push_back(std::string(" (struct RVM_Thd_Reg*)0x")+
                            Main::Hex(Virt->Reg_Base)+"U, (struct RVM_State*)0x"+
                            Main::Hex(Virt->State_Base)+"U,");
            List->push_back(std::string(" 0x")+
                            Main::Hex(Virt->State_Size)+"U, 0x"+
                            Main::Hex(Virt->Code_Base)+"U, RVM_CID(RVM_MAIN_VEP_"+
                            MIDS(Obj_Cnt)+", "+SIDS(Obj_Cnt)+"U),");
            List->push_back(std::string(" ")+
                            Virt->Thread[0]->Macro_Global+", 0x"+
                            Main::Hex(Virt->Thread[0]->Stack_Base)+"U, 0x"+
                            Main::Hex(Virt->Thread[0]->Stack_Size)+"U,");
            List->push_back(std::string(" ")+
                            Virt->Thread[1]->Macro_Global+", 0x"+
                            Main::Hex(Virt->Thread[1]->Stack_Base)+"U, 0x"+
                            Main::Hex(Virt->Thread[1]->Stack_Size)+"U},");
            Obj_Cnt++;
        }
        List->push_back("};");
        List->push_back("");
    }

    /* Virtual endpoint metadata */
    if(this->Plat->Proj->Virtual.size()!=0)
    {
        List->push_back("/* Virtual endpoint metadata */");
        List->push_back("const struct RVM_Meta_Main_Struct RVM_Meta_Vep_Main[RVM_BOOT_VEP_MAIN_NUM]=");
        List->push_back("{");
        this->Monitor_Main_Crt(List, this->Plat->Proj->Virtual.size(), "VEP");
        List->push_back("};");
        List->push_back("const struct RVM_Meta_Rcv_Crt_Struct RVM_Meta_Vep_Crt[RVM_BOOT_VEP_CRT_NUM]=");
        List->push_back("{");
        for(Obj_Cnt=0;Obj_Cnt<this->Plat->Proj->Virtual.size();Obj_Cnt++)
            List->push_back(std::string("{RVM_MAIN_VEP_")+MIDS(Obj_Cnt)+", "+SIDS(Obj_Cnt)+"U},");
        List->push_back("};");
        List->push_back("const struct RVM_Meta_Vcap_Init_Struct RVM_Meta_Vcap_Init[RVM_BOOT_VCAP_INIT_NUM]=");
        List->push_back("{");
        Obj_Cnt=0;
        for(const class Virtual* Virt:this->Plat->Proj->Virtual)
        {
            List->push_back(std::string("{")+Virt->Captbl->Macro_Global+", RVM_MAIN_VEP_"+MIDS(Obj_Cnt)+", "+SIDS(Obj_Cnt)+"U},");
            Obj_Cnt++;
        }
        List->push_back("};");
        List->push_back("");
    }

    /* Capability table metadata */
    List->push_back("/* Capability table metadata */");
    List->push_back("const struct RVM_Meta_Main_Struct RVM_Meta_Cpt_Main[RVM_BOOT_CPT_MAIN_NUM]=");
    List->push_back("{");
    this->Monitor_Main_Crt(List, Monitor->Captbl.size(), "CPT");
    List->push_back("};");
    List->push_back("const struct RVM_Meta_Cpt_Crt_Struct RVM_Meta_Cpt_Crt[RVM_BOOT_CPT_CRT_NUM]=");
    List->push_back("{");
    for(const class Captbl* Cpt:Monitor->Captbl)
    {
        List->push_back(std::string("{RVM_MAIN_CPT_")+MIDS(Cpt->Cid_Global)+", "+
                        SIDS(Cpt->Cid_Global)+", "+std::to_string(Cpt->Size)+"U},");
    }
    List->push_back("};");
    if(this->Cpt_Init_Total!=0)
    {
        List->push_back("const struct RVM_Meta_Cpt_Init_Struct RVM_Meta_Cpt_Init[RVM_BOOT_CPT_INIT_NUM]=");
        List->push_back("{");
        this->Monitor_Cpt_Init(List, 0);
        List->push_back("};");
    }
    if(this->Cpt_Kfn_Total!=0)
    {
        List->push_back("const struct RVM_Meta_Cpt_Kfn_Struct RVM_Meta_Cpt_Kfn[RVM_BOOT_CPT_KFN_NUM]=");
        List->push_back("{");
        this->Monitor_Cpt_Init(List, 1);
        List->push_back("};");
    }
    List->push_back("");

    /* Page table metadata */
    List->push_back("/* Page table metadata */");
    List->push_back("const struct RVM_Meta_Main_Struct RVM_Meta_Pgt_Main[RVM_BOOT_PGT_MAIN_NUM]=");
    List->push_back("{");
    this->Monitor_Main_Crt(List, Monitor->Pgtbl.size(), "PGT");
    List->push_back("};");
    List->push_back("const struct RVM_Meta_Pgt_Crt_Struct RVM_Meta_Pgt_Crt[RVM_BOOT_PGT_CRT_NUM]=");
    List->push_back("{");
    for(const class Pgtbl* Pgt:Monitor->Pgtbl)
    {
        List->push_back(std::string("{RVM_MAIN_PGT_")+MIDS(Pgt->Cid_Global)+", "+SIDS(Pgt->Cid_Global)+", 0x"+
                        Main::Hex(Pgt->Base)+"U, "+std::to_string((ptr_t)(Pgt->Is_Top!=0))+"U, "+
                        std::to_string(Pgt->Size_Order)+"U, "+std::to_string(Pgt->Num_Order)+"U},");
    }
    List->push_back("};");
    if(this->Pgt_Con_Total!=0)
    {
        List->push_back("const struct RVM_Meta_Pgt_Con_Struct RVM_Meta_Pgt_Con[RVM_BOOT_PGT_CON_NUM]=");
        List->push_back("{");
        for(const std::unique_ptr<class Process>& Prc:this->Plat->Proj->Process)
            this->Monitor_Pgt_Con(List, Prc->Pgtbl.get());
        List->push_back("};");
    }
    List->push_back("const struct RVM_Meta_Pgt_Add_Struct RVM_Meta_Pgt_Add[RVM_BOOT_PGT_ADD_NUM]=");
    List->push_back("{");
    for(const class Pgtbl* Pgt:Monitor->Pgtbl)
        this->Monitor_Pgt_Add(List, Pgt, this->Plat->Plat->Wordlength);
    List->push_back("};");
    List->push_back("");

    /* Process metadata */
    List->push_back("/* Process metadata */");
    List->push_back("const struct RVM_Meta_Main_Struct RVM_Meta_Prc_Main[RVM_BOOT_PRC_MAIN_NUM]=");
    List->push_back("{");
    this->Monitor_Main_Crt(List, Monitor->Process.size(), "PRC");
    List->push_back("};");
    List->push_back("const struct RVM_Meta_Prc_Crt_Struct RVM_Meta_Prc_Crt[RVM_BOOT_PRC_CRT_NUM]=");
    List->push_back("{");
    for(const class Process* Prc:Monitor->Process)
    {
        List->push_back(std::string("{RVM_MAIN_PRC_")+MIDS(Prc->Cid_Global)+", "+SIDS(Prc->Cid_Global)+", "+
                        Prc->Captbl->Macro_Global+", "+Prc->Pgtbl->Macro_Global+"},");
    }
    List->push_back("};");
    List->push_back("");

    /* Thread metadata */
    List->push_back("/* Thread metadata */");
    List->push_back("const struct RVM_Meta_Main_Struct RVM_Meta_Thd_Main[RVM_BOOT_THD_MAIN_NUM]=");
    List->push_back("{");
    this->Monitor_Main_Crt(List, Monitor->Thread.size(), "THD");
    List->push_back("};");
    List->push_back("const struct RVM_Meta_Thd_Crt_Struct RVM_Meta_Thd_Crt[RVM_BOOT_THD_CRT_NUM]=");
    List->push_back("{");
    for(const class Thread* Thd:Monitor->Thread)
    {
        List->push_back(std::string("{RVM_MAIN_THD_")+MIDS(Thd->Cid_Global)+", "+SIDS(Thd->Cid_Global)+", "+
                        Thd->Owner->Macro_Global+", "+std::to_string(Thd->Priority)+"U},");
    }
    List->push_back("};");
    List->push_back("const struct RVM_Meta_Thd_Init_Struct RVM_Meta_Thd_Init[RVM_BOOT_THD_INIT_NUM]=");
    List->push_back("{");
    this->Monitor_Thd_Init(List);
    List->push_back("};");
    List->push_back("");

    /* Invocation metadata */
    if(Monitor->Invocation.size()!=0)
    {
		List->push_back("/* Invocation metadata */");
		List->push_back("const struct RVM_Meta_Main_Struct RVM_Meta_Inv_Main[RVM_BOOT_INV_MAIN_NUM]=");
		List->push_back("{");
		this->Monitor_Main_Crt(List, Monitor->Invocation.size(), "INV");
		List->push_back("};");
		List->push_back("const struct RVM_Meta_Inv_Crt_Struct RVM_Meta_Inv_Crt[RVM_BOOT_INV_CRT_NUM]=");
		List->push_back("{");
		for(const class Invocation* Inv:Monitor->Invocation)
		{
			List->push_back(std::string("{RVM_MAIN_INV_")+MIDS(Inv->Cid_Global)+", "+SIDS(Inv->Cid_Global)+", "+
							Inv->Owner->Macro_Global+"},");
		}
		List->push_back("};");
		List->push_back("const struct RVM_Meta_Inv_Init_Struct RVM_Meta_Inv_Init[RVM_BOOT_INV_INIT_NUM]=");
		List->push_back("{");
		this->Monitor_Inv_Init(List);
		List->push_back("};");
		List->push_back("");
    }

    /* Receive endpoint metadata */
    if(Monitor->Receive.size()!=0)
    {
		List->push_back("/* Receive endpoint metadata */");
		List->push_back("const struct RVM_Meta_Main_Struct RVM_Meta_Rcv_Main[RVM_BOOT_RCV_MAIN_NUM]=");
		List->push_back("{");
		this->Monitor_Main_Crt(List, Monitor->Receive.size(), "RCV");
		List->push_back("};");
		List->push_back("const struct RVM_Meta_Rcv_Crt_Struct RVM_Meta_Rcv_Crt[RVM_BOOT_RCV_CRT_NUM]=");
		List->push_back("{");
		for(const class Receive* Rcv:Monitor->Receive)
			List->push_back(std::string("{RVM_MAIN_RCV_")+MIDS(Rcv->Cid_Global)+", "+SIDS(Rcv->Cid_Global)+"},");
		List->push_back("};");
	    List->push_back("");
    }
    List->push_back("/* End Public Global Variables ***********************************************/");
    List->push_back("");

    /* Generate rvm_boot.c */
    Main::Info("> Writing file.");
    Gen_Tool::Src_Foot(List);
    Gen_Tool::Line_Write(List, Monitor->Boot_Source_Output+"rvm_boot.c");
}
/* End Function:Gen_Tool::Monitor_Boot_Src ***********************************/

/* Begin Function:Gen_Tool::Monitor_Hook_Src **********************************
Description : Create the hook source file for monitor.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
void Gen_Tool::Monitor_Hook_Src(void)
{
    class Monitor* Monitor;
    std::vector<std::string> Input;
    std::vector<std::string> Output;
    std::unique_ptr<std::vector<std::string>> List;

    Monitor=this->Plat->Proj->Monitor.get();
    List=std::make_unique<std::vector<std::string>>();

    /* Does the file already exist? */
    if(std::filesystem::exists(Monitor->Hook_Source_Output+"rvm_hook.c")==true)
    {
        /* See if we'll use forced regenerate */
        if(Monitor->Hook_Source_Overwrite==0)
        {
            Main::Info("> File 'rvm_hook.c' exists, skipping generation.");
            return;
        }
    }

    /* File header */
    Main::Info("> Generating file header.");
    Gen_Tool::Src_Head(List, "rvm_hook.c", "The user initialization hook file.");
    List->push_back("");

    /* Print all header includes */
    Main::Info("> Generating C header includes.");
    this->Monitor_Inc(List);
    List->push_back("");

    /* Print all global prototypes */
    Main::Info("> Generating public C function prototypes.");
    List->push_back("/* Public C Function Prototypes **********************************************/");
    List->push_back("void RVM_Boot_Pre_Init(void);");
    List->push_back("void RVM_Boot_Post_Init(void);");
    List->push_back("/* End Public C Function Prototypes ******************************************/");
    List->push_back("");

    /* Preinitialization of hardware */
    Main::Info("> Generating boot-time pre-initialization stub.");
    Gen_Tool::Func_Head(List, "RVM_Boot_Pre_Init",
                        "Initialize critical hardware before any kernel initialization takes place.",
                        Input, Output, "None.");
    List->push_back("void RVM_Boot_Pre_Init(void)");
    List->push_back("{");
    List->push_back("    /* Add code here */");
    List->push_back("}");
    Gen_Tool::Func_Foot(List, "RVM_Boot_Pre_Init");
    List->push_back("");

    /* Postinitialization of hardware */
    Main::Info("> Generating boot-time post-initialization stub.");
    Gen_Tool::Func_Head(List, "RVM_Boot_Post_Init",
                        "Initialize hardware after all kernel initialization took place.",
                        Input, Output, "None.");
    List->push_back("void RVM_Boot_Post_Init(void)");
    List->push_back("{");
    List->push_back("    /* Add code here */");
    List->push_back("}");
    Gen_Tool::Func_Foot(List, "RVM_Boot_Post_Init");
    List->push_back("");

    /* Generate rme_init.c */
    Main::Info("> Writing file.");
    Gen_Tool::Src_Foot(List);
    Gen_Tool::Line_Write(List, Monitor->Hook_Source_Output+"rvm_hook.c");
}
/* End Function:Gen_Tool::Monitor_Hook_Src ***********************************/

/* Begin Function:Gen_Tool::Monitor_Linker ************************************
Description : Create the linker script file for monitor.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
void Gen_Tool::Monitor_Linker(void)
{
    class Monitor* Monitor;
    class Tool_Gen* Tool;
    std::unique_ptr<std::vector<std::string>> List;

    Monitor=this->Plat->Proj->Monitor.get();
    List=std::make_unique<std::vector<std::string>>();
    Tool=this->Tool_Map[Monitor->Toolchain];

    Main::Info("> Generating linker script.");
    Monitor->Linker_Filename=std::string("rvm_platform_")+this->Plat->Chip->Name_Lower+Tool->Suffix(TOOL_LINKER);
    Tool->Monitor_Linker(List);
    Gen_Tool::Line_Write(List, Monitor->Linker_Output+Monitor->Linker_Filename);
}
/* End Function:Gen_Tool::Monitor_Linker *************************************/

/* Begin Function:Gen_Tool::Monitor_Proj **************************************
Description : Create the project file for monitor.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
void Gen_Tool::Monitor_Proj(void)
{
    class Monitor* Monitor;
    class Build_Gen* Build;
    class Tool_Gen* Tool;
    std::unique_ptr<std::vector<std::string>> List;
    std::vector<std::string> Include;
    std::vector<std::string> Source;
    std::vector<std::string> Linker;

    Monitor=this->Plat->Proj->Monitor.get();
    List=std::make_unique<std::vector<std::string>>();
    Build=this->Build_Map[Monitor->Buildsystem];
    Tool=this->Tool_Map[Monitor->Toolchain];

    /* Does the file already exist? */
    Monitor->Project_Filename=std::string("monitor")+Build->Suffix(BUILD_PROJECT);
    if(std::filesystem::exists(Monitor->Project_Output+Monitor->Project_Filename)==true)
    {
        /* See if we'll use forced regenerate */
        if(Monitor->Project_Overwrite==0)
        {
            Main::Info(std::string("> File '")+Monitor->Project_Filename+"' exists, skipping generation.");
            return;
        }
    }

    /* Extract the include paths */
    Main::Info("> Generating project include paths:");

    Include.push_back(Monitor->Project_Output);
    Include.push_back(Monitor->Monitor_Root+"Include/");
    Include.push_back(Monitor->Config_Header_Output);
    Include.push_back(Monitor->Boot_Header_Output);
    Gen_Tool::Path_Conv(Monitor->Project_Output, Include);

    for(const std::string& Path:Include)
        Main::Info(std::string("> > ")+Path);

    /* Extract the source paths */
    Main::Info("> Generating project source paths:");

    Source.push_back(Monitor->Monitor_Root+"Source/Monitor/rvm_init.c");
    Source.push_back(Monitor->Monitor_Root+"Source/Monitor/rvm_hyper.c");
    Source.push_back(Monitor->Monitor_Root+"Source/Monitor/rvm_syssvc.c");
    Source.push_back(Monitor->Monitor_Root+"Source/Platform/"+this->Plat->Name+"/rvm_platform_"+this->Plat->Name_Lower+".c");
    Source.push_back(Monitor->Monitor_Root+"Source/Platform/"+this->Plat->Name+
                     "/rvm_platform_"+this->Plat->Name_Lower+"_"+Tool->Name_Lower+Tool->Suffix(TOOL_ASSEMBLER));
    Source.push_back(Monitor->Boot_Source_Output+"rvm_boot.c");
    Source.push_back(Monitor->Hook_Source_Output+"rvm_hook.c");
    Gen_Tool::Path_Conv(Monitor->Project_Output, Source);

    for(const std::string& Path:Source)
        Main::Info(std::string("> > ")+Path);

    /* Extract the linker paths */
    Main::Info("> Generating project linker paths:");

    Linker.push_back(Monitor->Linker_Output+"rvm_platform_"+this->Plat->Chip->Name_Lower+Tool->Suffix(TOOL_LINKER));
    Gen_Tool::Path_Conv(Monitor->Project_Output, Linker);

    for(const std::string& Path:Linker)
        Main::Info(std::string("> > ")+Path);

    Build->Monitor_Proj(List, Include, Source, Linker);
    Gen_Tool::Line_Write(List, Monitor->Project_Output+Monitor->Project_Filename);
}
/* End Function:Gen_Tool::Monitor_Proj ***************************************/

/* Begin Function:Gen_Tool::Process_Inc ***************************************
Description : Write the include files for process files.
Input       : std::unique_ptr<std::vector<std::string>>& List - The input file.
              class Process* Prc - The process.
Output      : std::unique_ptr<std::vector<std::string>>& List - The appended file.
Return      : None.
******************************************************************************/
void Gen_Tool::Process_Inc(std::unique_ptr<std::vector<std::string>>& List,
                           class Process* Prc)
{
    List->push_back("/* Includes ******************************************************************/");
    List->push_back("#include \"rvm.h\"");
    List->push_back("#include \"rvm_guest.h\"");
    List->push_back("/* End Includes **************************************************************/");
}
/* End Function:Gen_Tool::Process_Inc ****************************************/

/* Begin Function:Gen_Tool::Process_Main_Hdr_Mem ******************************
Description : Print memory referernce for process config header.
Input       : std::unique_ptr<std::vector<std::string>>& List - The file.
              const class Mem_Info* Mem - The memory to generate for.
Output      : std::unique_ptr<std::vector<std::string>>& List - The appended file.
Return      : None.
******************************************************************************/
void Gen_Tool::Process_Main_Hdr_Mem(std::unique_ptr<std::vector<std::string>>& List,
                                    const class Mem_Info* Mem)
{
    std::string Macro;

    if(Mem->Name=="")
        return;

    switch(Mem->Type)
    {
        case MEM_CODE: Macro="CODE_"; break;
        case MEM_DATA: Macro="DATA_"; break;
        case MEM_DEVICE: Macro="DEVICE_"; break;
        default:ASSERT(0);
    }

    if(Mem->Is_Shared!=0)
        Macro+="SHARED_";

    Macro+=Mem->Name_Upper;

    Gen_Tool::Macro_Hex(List, Macro+"_BASE", Mem->Base, MACRO_ADD);
    Gen_Tool::Macro_Hex(List, Macro+"_SIZE", Mem->Size, MACRO_ADD);
}
/* End Function:Gen_Tool::Process_Main_Hdr_Mem *******************************/

/* Begin Function:Gen_Tool::Process_Main_Hdr **********************************
Description : Create the main header for process.
Input       : class Process* Prc - The process to generate for.
Output      : None.
Return      : None.
******************************************************************************/
void Gen_Tool::Process_Main_Hdr(class Process* Prc)
{
    const class Virtual* Virt;
    std::vector<std::string> Input;
    std::vector<std::string> Output;
    std::unique_ptr<std::vector<std::string>> List;

    List=std::make_unique<std::vector<std::string>>();

    /* File header */
    Main::Info("> Generating main header.");
    Gen_Tool::Src_Head(List, std::string("prc_")+Prc->Name_Lower+".h", "The user kernel object header.");
    List->push_back("");

    /* Definitions */
    List->push_back("/* Defines *******************************************************************/");
    List->push_back(std::string("#ifndef __PRC_")+Prc->Name_Upper+"_H__");
    List->push_back(std::string("#define __PRC_")+Prc->Name_Upper+"_H__");

    /* Ports */
    List->push_back("/* Ports */");
    for(const std::unique_ptr<class Port>& Port: Prc->Port)
        Gen_Tool::Macro_Int(List, Port->Macro_Local, Port->Cid_Local, MACRO_ADD);
    List->push_back("");
    /* Receive endpoint */
    List->push_back("/* Receive endpoints */");
    for(const std::unique_ptr<class Receive>& Rcv: Prc->Receive)
        Gen_Tool::Macro_Int(List, Rcv->Macro_Local, Rcv->Cid_Local, MACRO_ADD);
    List->push_back("");
    /* Send endpoint */
    List->push_back("/* Send endpoints */");
    for(const std::unique_ptr<class Send>& Send: Prc->Send)
        Gen_Tool::Macro_Int(List, Send->Macro_Local, Send->Cid_Local, MACRO_ADD);
    List->push_back("");
    /* Vector endpoint */
    List->push_back("/* Vector endpoints */");
    for(const std::unique_ptr<class Vect_Info>& Vct: Prc->Vector)
        Gen_Tool::Macro_Int(List, Vct->Macro_Local, Vct->Cid_Local, MACRO_ADD);
    List->push_back("");
    /* Kernel function */
    List->push_back("/* Kernel functions */");
    for(const std::unique_ptr<class Kfunc>& Kfn: Prc->Kfunc)
        Gen_Tool::Macro_Int(List, Kfn->Macro_Local, Kfn->Cid_Local, MACRO_ADD);
    List->push_back("");

    /* Memory block */
    List->push_back("/* Code memory blocks */");
    for(const class Mem_Info* Mem: Prc->Memory_Code)
        this->Process_Main_Hdr_Mem(List, Mem);
    List->push_back("");
    List->push_back("/* Data memory blocks */");
    for(const class Mem_Info* Mem: Prc->Memory_Data)
        this->Process_Main_Hdr_Mem(List, Mem);
    List->push_back("");
    List->push_back("/* Device memory blocks */");
    for(const class Mem_Info* Mem: Prc->Memory_Device)
        this->Process_Main_Hdr_Mem(List, Mem);
    List->push_back("");

    /* The total priority numbers */
    List->push_back("/* Total priority number */");
    Gen_Tool::Macro_Int(List, "RVM_PREEMPT_PRIO_NUM", this->Plat->Proj->Kernel->Kern_Prio, MACRO_ADD);
    /* The VM priority numbers */
    List->push_back("/* Total VM priority number */");
    Gen_Tool::Macro_Int(List, "RVM_PREEMPT_VPRIO_NUM", this->Plat->Proj->Monitor->Virt_Prio, MACRO_ADD);
    /* The kernel memory slot order */
    List->push_back("/* The kernel memory allocation granularity order */");
    Gen_Tool::Macro_Int(List, "RVM_KOM_SLOT_ORDER", this->Plat->Proj->Kernel->Kom_Order, MACRO_ADD);
    List->push_back("");

    /* If this is a virtual machine, define the following */
    if(Prc->Type==PROCESS_VIRTUAL)
    {
        Virt=static_cast<const class Virtual*>(Prc);
        List->push_back("/* Virtual vector total number */");
        Gen_Tool::Macro_Int(List, "RVM_VIRT_VCT_NUM", Virt->Vector_Num, MACRO_ADD);
        List->push_back("/* State block base address & size */");
        Gen_Tool::Macro_Hex(List, "RVM_VIRT_STATE_BASE", Virt->State_Base, MACRO_ADD);
        Gen_Tool::Macro_Hex(List, "RVM_VIRT_STATE_SIZE", Virt->State_Size, MACRO_ADD);
        List->push_back("/* Virtual register base address & size */");
        Gen_Tool::Macro_Hex(List, "RVM_VIRT_REG_BASE", Virt->Reg_Base, MACRO_ADD);
        Gen_Tool::Macro_Hex(List, "RVM_VIRT_REG_SIZE", Virt->Reg_Size, MACRO_ADD);
        List->push_back("");
    }

    /* Assert & debugging */
    Gen_Tool::Macro_Int(List, "RVM_ASSERT_CORRECT", this->Plat->Proj->Assert_Correct, MACRO_ADD);
    Gen_Tool::Macro_Int(List, "RVM_DEBUG_PRINT", this->Plat->Proj->Debug_Print, MACRO_ADD);

    /* Generate platform specific macros */
    this->Plat->Process_Main_Hdr(List);

    List->push_back(std::string("#endif /* __PRC_")+Prc->Name_Upper+"_H__ */");
    List->push_back("/* End Defines ***************************************************************/");
    List->push_back("");

    Gen_Tool::Src_Foot(List);
    List->push_back("");

    /* Generate proc_xxx.h */
    Gen_Tool::Line_Write(List, Prc->Main_Header_Output+"prc_"+Prc->Name_Lower+".h");

    /* Generate platform selection header */
    Main::Info("> Generating platform selection header.");
    List->clear();
    Gen_Tool::Src_Head(List, "rvm_guest_conf.h", "The guest library configuration header.");
    List->push_back("");
    List->push_back("/* Includes ******************************************************************/");
    List->push_back(std::string("#include \"prc_")+Prc->Name_Lower+".h\"");
    List->push_back(std::string("#include \"")+this->Plat->Name_Upper+"/rvm_guest_"+this->Plat->Name_Lower+".h\"");
    List->push_back("/* End Includes **************************************************************/");
    List->push_back("");
    Gen_Tool::Src_Foot(List);
    List->push_back("");
    Gen_Tool::Line_Write(List, Prc->Main_Header_Output+"rvm_guest_conf.h");
}
/* End Function:Gen_Tool::Process_Main_Hdr ***********************************/

/* Begin Function:Gen_Tool::Process_Stub_Src **********************************
Description : Create the stubs for process. Each invocation and thread will
              have its own file, so there is least interference between them.
Input       : class Process* Prc - The process to generate for.
Output      : None.
Return      : None.
******************************************************************************/
void Gen_Tool::Process_Stub_Src(class Process* Prc)
{
    std::string Filename;
    std::vector<std::string> Input;
    std::vector<std::string> Output;
    std::unique_ptr<std::vector<std::string>> List;

    /* Single files only used for native threads */
    if(Prc->Type==PROCESS_VIRTUAL)
        return;

    List=std::make_unique<std::vector<std::string>>();

    /* For each thread, create a single file */
    Input.push_back("rvm_ret_t Param - The parameter supplied by the OS.");
    for(const std::unique_ptr<class Thread>& Thd:Prc->Thread)
    {
        Filename=std::string("prc_")+Prc->Name_Lower+"_thd_"+Thd->Name_Lower+".c";
        if(std::filesystem::exists(Prc->Main_Source_Output+Filename)==true)
        {
            /* See if we'll use forced regenerate */
            if(Prc->Main_Source_Overwrite==0)
            {
                Main::Info(std::string("> File '")+Filename+"' exists, skipping generation.");
                return;
            }
        }
        Main::Info(std::string("> Generating source for thread '")+Thd->Name+"'.");
        List->clear();
        Gen_Tool::Src_Head(List, Filename, std::string("The user stub file for thread '")+Thd->Name+"'.");
        List->push_back("");
        /* Includes */
        this->Process_Inc(List, Prc);
        List->push_back("");
        /* Private prototypes */
        List->push_back("/* Private C Function Prototypes *********************************************/");
        List->push_back(std::string("rvm_ret_t Thd_")+Thd->Name+"(rvm_ret_t Param);");
        List->push_back("/* End Private C Function Prototypes *****************************************/");
        List->push_back("");
        /* Thread functions themselves */
        Gen_Tool::Func_Head(List, std::string("Thd_")+Thd->Name,
                            "The function body for thread.", Input, Output, "rvm_ret_t - Should never return.");
        List->push_back(std::string("rvm_ret_t Thd_")+Thd->Name+"(rvm_ret_t Param)");
        List->push_back("{");
        List->push_back("    /* Add your code here - Threads shall never return */");
        List->push_back("    while(1);");
        List->push_back("}");
        Gen_Tool::Func_Foot(List, std::string("Thd_")+Thd->Name);
        List->push_back("");
        this->Src_Foot(List);
        List->push_back("");
        Gen_Tool::Line_Write(List, Prc->Main_Source_Output+Filename);
    }
    Input.clear();

    /* For each invocation, create a single file */
    Input.push_back("rvm_ret_t Param - The parameter supplied by the caller.");
    for(const std::unique_ptr<class Invocation>& Inv:Prc->Invocation)
    {
        Filename=std::string("prc_")+Prc->Name_Lower+"_inv_"+Inv->Name_Lower+".c";
        if(std::filesystem::exists(Prc->Main_Source_Output+Filename)==true)
        {
            /* See if we'll use forced regenerate */
            if(Prc->Main_Source_Overwrite==0)
            {
                Main::Info(std::string("> File '")+Filename+"' exists, skipping generation.");
                return;
            }
        }
        Main::Info(std::string("> Generating source for invocation '")+Inv->Name+"'.");
        List->clear();
        Gen_Tool::Src_Head(List, Filename, std::string("The user stub file for thread '")+Inv->Name+"'.");
        List->push_back("");
        /* Includes */
        this->Process_Inc(List, Prc);
        List->push_back("");
        /* Private prototypes */
        List->push_back("/* Private C Function Prototypes *********************************************/");
        List->push_back(std::string("rvm_ret_t Inv_")+Inv->Name+"(rvm_ret_t Param);");
        List->push_back("/* End Private C Function Prototypes *****************************************/");
        List->push_back("");
        /* Thread functions themselves */
        Gen_Tool::Func_Head(List, std::string("Inv_")+Inv->Name,
                            "The function body for invocation.", Input, Output, "rvm_ret_t - Should never return.");
        List->push_back(std::string("rvm_ret_t Inv_")+Inv->Name+"(rvm_ret_t Param)");
        List->push_back("{");
        List->push_back("    /* Add your code here */");
        List->push_back("    return 0;");
        List->push_back("}");
        Gen_Tool::Func_Foot(List, std::string("Inv_")+Inv->Name);
        List->push_back("");
        this->Src_Foot(List);
        List->push_back("");
        Gen_Tool::Line_Write(List, Prc->Main_Source_Output+Filename);
    }
    Input.clear();
}
/* End Function:Gen_Tool::Process_Stub_Src ***********************************/

/* Begin Function:Gen_Tool::Process_Desc_Src **********************************
Description : Create the descriptor header for process.
Input       : class Process* Prc - The process to generate for.
Output      : None.
Return      : None.
******************************************************************************/
void Gen_Tool::Process_Desc_Src(class Process* Prc)
{
    std::string Filename;
    std::vector<std::string> Input;
    std::vector<std::string> Output;
    std::unique_ptr<std::vector<std::string>> List;

    List=std::make_unique<std::vector<std::string>>();

    Main::Info("> Generating main source descriptor.");
    Filename=std::string("prc_")+Prc->Name_Lower+"_desc.c";
    Gen_Tool::Src_Head(List, Filename, "The process descriptor header file - do not edit!.");
    List->push_back("");

    /* Includes */
    this->Process_Inc(List, Prc);
    List->push_back("");

    /* Public prototypes */
    List->push_back("/* Public C Function Prototypes **********************************************/");
    if(Prc->Type==PROCESS_VIRTUAL)
    {
        List->push_back("EXTERN rvm_ret_t Thd_Vct(rvm_ret_t Param);");
        List->push_back("EXTERN void _RVM_Jmp_Stub(void);");
    }
    else
    {
		for(const std::unique_ptr<class Thread>& Thd:Prc->Thread)
			List->push_back(std::string("EXTERN rvm_ret_t Thd_")+Thd->Name+"(rvm_ret_t Param);");
		for(const std::unique_ptr<class Invocation>& Inv:Prc->Invocation)
			List->push_back(std::string("EXTERN rvm_ret_t Inv_")+Inv->Name+"(rvm_ret_t Param);");
		List->push_back("EXTERN void _RVM_Jmp_Stub(void);");
    }
    List->push_back("/* End Public C Function Prototypes ******************************************/");
    List->push_back("");

    /* Global variables */
    List->push_back("/* Public Global Variables ***************************************************/");
    List->push_back(std::string("const rvm_ptr_t RVM_Desc[")+std::to_string(Prc->Desc_Front)+"]=");
    List->push_back("{");
    if(Prc->Type==PROCESS_NATIVE)
    {
        List->push_back(std::string("    0x")+Main::Hex(MAGIC_NATIVE)+"U,");
        List->push_back(std::string("    0x")+Main::Hex(Prc->Desc_Front-2)+"U,");
        /* For native processes, the first (higher-priority) thread's entry is always the main entry point
         * that immediately follows the header, and aligned to a 16-byte boundary. This is due to some
         * proprietary toolchains requiring an aligned address - e.g. ARMCC. */
        for(const std::unique_ptr<class Thread>& Thd:Prc->Thread)
        {
            if(Thd.get()==Prc->Thread[0].get())
                List->push_back(std::string("    0x")+Main::Hex(Prc->Code_Front)+"U,");
            else
                List->push_back(std::string("    (rvm_ptr_t)Thd_")+Thd->Name+",");
        }
    }
    else
    {
        List->push_back(std::string("    0x")+Main::Hex(MAGIC_VIRTUAL)+"U,");
        List->push_back(std::string("    0x")+Main::Hex(Prc->Desc_Front-2)+"U,");
        /* For native processes, the second (lower-priority) thread's entry is always the main entry point.
         * Because the second thread that runs the user code is low-priority, and the guest VM may have
         * already defined a main function. We want to be as less intrusive as possible */
        for(const std::unique_ptr<class Thread>& Thd:Prc->Thread)
        {
            if(Thd.get()==Prc->Thread[1].get())
            {
                ASSERT(Thd->Name=="Usr");
                List->push_back(std::string("    0x")+Main::Hex(Prc->Code_Front)+"U,");
            }
            else
            {
                ASSERT(Thd->Name=="Vct");
                List->push_back(std::string("    (rvm_ptr_t)Thd_")+Thd->Name+",");
            }
        }
    }
    for(const std::unique_ptr<class Invocation>& Inv:Prc->Invocation)
        List->push_back(std::string("    (rvm_ptr_t)Inv_")+Inv->Name+",");
    List->push_back("    (rvm_ptr_t)_RVM_Jmp_Stub,");
    List->push_back("};");
    List->push_back("/* End Public Global Variables ***********************************************/");
    List->push_back("");
    Gen_Tool::Src_Foot(List);
    List->push_back("");

    /* Generate proc_xxx_header.c */
    Gen_Tool::Line_Write(List, Prc->Main_Source_Output+Filename);
}
/* End Function:Gen_Tool::Process_Desc_Src ***********************************/

/* Begin Function:Gen_Tool::Process_Main_Src **********************************
Description : Create the main sources for process. Each invocation and thread will
              have its own file, so there is least interference between them.
Input       : class Process* Prc - The process to generate for.
Output      : None.
Return      : None.
******************************************************************************/
void Gen_Tool::Process_Main_Src(class Process* Prc)
{
    std::string Filename;
    std::vector<std::string> Input;
    std::vector<std::string> Output;
    std::unique_ptr<std::vector<std::string>> List;

    List=std::make_unique<std::vector<std::string>>();

    Main::Info("> Generating main source.");
    Filename=std::string("prc_")+Prc->Name_Lower+".c";
    Gen_Tool::Src_Head(List, Filename, "The main user stub file.");
    List->push_back("");

    /* Includes */
    this->Process_Inc(List, Prc);
    List->push_back("");

    /* Global variable - The only one being the process header reference */
    List->push_back("/* Public Global Variables ***************************************************/");
    List->push_back(std::string("EXTERN const rvm_ptr_t RVM_Desc[")+std::to_string(Prc->Desc_Front)+"];");
    List->push_back("/* End Public Global Variables ***********************************************/");
    List->push_back("");

    /* Private prototypes */
    if(Prc->Type==PROCESS_VIRTUAL)
    {
        List->push_back("/* Private C Function Prototypes *********************************************/");
        List->push_back("rvm_ret_t Thd_Vct(rvm_ret_t Param);");
        List->push_back("EXTERN void _RVM_Jmp_Stub(void);");
        List->push_back("/* End Private C Function Prototypes *****************************************/");
        List->push_back("");
    }
    else
    {
        List->push_back("/* Public C Function Prototypes **********************************************/");
        for(const std::unique_ptr<class Thread>& Thd:Prc->Thread)
            List->push_back(std::string("extern rvm_ret_t Thd_")+Thd->Name+"(rvm_ret_t Param);");
        for(const std::unique_ptr<class Invocation>& Inv:Prc->Invocation)
            List->push_back(std::string("extern rvm_ret_t Inv_")+Inv->Name+"(rvm_ret_t Param);");
        List->push_back("extern void _RVM_Jmp_Stub(void);");
        List->push_back("/* End Private C Function Prototypes *****************************************/");
        List->push_back("");
    }

    /* If this is a virtual machine, create the two essential threads */
    if(Prc->Type==PROCESS_VIRTUAL)
    {
        Input.push_back("rvm_ret_t Param - The parameter supplied by the OS.");
        /* The vector handler thread */
        Gen_Tool::Func_Head(List, "Thd_Vct", "The function body for vector handler.",
                            Input, Output, "rvm_ret_t - Should never return.");
        List->push_back("rvm_ret_t Thd_Vct(rvm_ret_t Param)");
        List->push_back("{");
        List->push_back("    /* Check header validity on boot */");
        List->push_back("    RVM_ASSERT(RVM_Desc[0]==RVM_MAGIC_VIRTUAL);");
        List->push_back(std::string("    RVM_ASSERT(RVM_Desc[1]==")+std::to_string(Prc->Desc_Front-2)+"U);");
        List->push_back("");
        List->push_back("    /* DO NOT MODIFY - THIS HIGH PRIORITY THREAD CAN RUN ON UNINITIALIZED RAM */");
        List->push_back("    RVM_Virt_Init();");
        List->push_back("    RVM_Vct_Loop();");
        List->push_back("");
        List->push_back("    return 0;");
        List->push_back("}");
        Gen_Tool::Func_Foot(List, "Thd_Vct");
        List->push_back("");
        Input.clear();
    }
    /* If not, create the main function */
    else
    {
        Gen_Tool::Func_Head(List, "main", "The function body for thread.", Input, Output, "rvm_ret_t - Should never return.");
        List->push_back("rvm_ret_t main(void)");
        List->push_back("{");
        List->push_back("    /* Check header validity */");
        List->push_back("    RVM_ASSERT(RVM_Desc[0]==RVM_MAGIC_NATIVE);");
        List->push_back(std::string("    RVM_ASSERT(RVM_Desc[1]==")+std::to_string(Prc->Desc_Front-2)+"U);");
        List->push_back("");
        List->push_back("    /* Call the first (highest-priority) thread */");
        List->push_back(std::string("    Thd_")+Prc->Thread[0]->Name+"("+Main::Hex(Prc->Thread[0]->Parameter)+"U);");
        List->push_back("}");
        Gen_Tool::Func_Foot(List, "main");
        List->push_back("");
    }

    /* The RVM library putchar function - print character to console */
    Input.push_back("char Char - The character to print to console.");
    Gen_Tool::Func_Head(List, "RVM_Putchar", "The character printing function for debugging.", Input, Output, "None.");
    List->push_back("#if(RVM_DEBUG_PRINT==1U)");
    List->push_back("void RVM_Putchar(char Char)");
    List->push_back("{");
    if(Prc->Type==PROCESS_VIRTUAL)
    	List->push_back("    RVM_Hyp_Putchar(Char);");
    else
    	List->push_back("    /* Add your own code */");
    List->push_back("}");
    List->push_back("#endif");
    Gen_Tool::Func_Foot(List, "RVM_Putchar");
    List->push_back("");
    Gen_Tool::Src_Foot(List);

    /* Generate proc_xxx.c */
    Gen_Tool::Line_Write(List, Prc->Main_Source_Output+Filename);
}
/* End Function:Gen_Tool::Process_Main_Src ***********************************/

/* Begin Function:Gen_Tool::Process_Virt_Hdr **********************************
Description : Create the configuration header for VMs.
Input       : class Virtual* Virt - The process to generate for.
Output      : None.
Return      : None.
******************************************************************************/
void Gen_Tool::Process_Virt_Hdr(class Virtual* Virt)
{
    /* Just pass this work down so that the underlying layers will handle it */
    this->Guest_Map[Virt->Guest_Type]->Process_Virt_Hdr(Virt);
}
/* End Function:Gen_Tool::Process_Virt_Hdr ***********************************/

/* Begin Function:Gen_Tool::Process_Virt_Src **********************************
Description : Create the configuration header for VMs.
Input       : class Virtual* Virt - The process to generate for.
Output      : None.
Return      : None.
******************************************************************************/
void Gen_Tool::Process_Virt_Src(class Virtual* Virt)
{
    /* Just pass this work down so that the underlying layers will handle it */
    this->Guest_Map[Virt->Guest_Type]->Process_Virt_Src(Virt, this->Tool_Map[Virt->Toolchain]);
}
/* End Function:Gen_Tool::Process_Virt_Src ***********************************/

/* Begin Function:Gen_Tool::Process_Linker ************************************
Description : Create the linker script file for process.
Input       : class Process* Prc - The process to generate for.
Output      : None.
Return      : None.
******************************************************************************/
void Gen_Tool::Process_Linker(class Process* Prc)
{
    class Tool_Gen* Tool;
    std::unique_ptr<std::vector<std::string>> List;

    List=std::make_unique<std::vector<std::string>>();
    Tool=this->Tool_Map[Prc->Toolchain];

    Main::Info("> Generating linker script.");
    Prc->Linker_Filename=std::string("prc_")+Prc->Name_Lower+"_"+this->Plat->Chip->Name_Lower+Tool->Suffix(TOOL_LINKER);
    Tool->Process_Linker(List, Prc);
    Gen_Tool::Line_Write(List, Prc->Linker_Output+Prc->Linker_Filename);
}
/* End Function:Gen_Tool::Process_Linker *************************************/

/* Begin Function:Gen_Tool::Process_Proj **************************************
Description : Create the project file for process.
Input       : class Process* Prc - The process to generate for.
Output      : None.
Return      : None.
******************************************************************************/
void Gen_Tool::Process_Proj(class Process* Prc)
{
    class Virtual* Virt;
    class Monitor* Monitor;
    class Build_Gen* Build;
    class Tool_Gen* Tool;
    std::unique_ptr<std::vector<std::string>> List;
    std::vector<std::string> Include;
    std::vector<std::string> Source;
    std::vector<std::string> Linker;

    List=std::make_unique<std::vector<std::string>>();
    Virt=static_cast<class Virtual*>(Prc);
    Monitor=this->Plat->Proj->Monitor.get();
    Build=this->Build_Map[Prc->Buildsystem];
    Tool=this->Tool_Map[Prc->Toolchain];

    /* Does the file already exist? */
    Prc->Project_Filename=std::string("prc_")+Prc->Name_Lower+Build->Suffix(BUILD_PROJECT);
    if(std::filesystem::exists(Prc->Project_Output+Prc->Project_Filename)==true)
    {
        /* See if we'll use forced regenerate */
        if(Prc->Project_Overwrite==0)
        {
            Main::Info(std::string("> File '")+Prc->Project_Filename+"' exists, skipping generation.");
            return;
        }
    }

    /* Extract the include paths */
    Main::Info("> Generating project include paths:");

    Include.push_back(Monitor->Monitor_Root+"Guest/");
    Include.push_back(Monitor->Monitor_Root+"Include/");
    Include.push_back(Prc->Project_Output);
    Include.push_back(Prc->Main_Header_Output);
    /* For virtual machines, add all the VM files as well */
    if(Prc->Type==PROCESS_VIRTUAL)
        Include.insert(Include.end(), Virt->Virtual_Include.begin(), Virt->Virtual_Include.end());
    Gen_Tool::Path_Conv(Prc->Project_Output, Include);

    for(const std::string& Path:Include)
        Main::Info(std::string("> > ")+Path);

    /* Extract the source paths */
    Main::Info("> Generating project source paths:");

    Source.push_back(Monitor->Monitor_Root+"Guest/rvm_guest.c");
    Source.push_back(Monitor->Monitor_Root+"Guest/"+this->Plat->Name+"/rvm_guest_"+this->Plat->Name_Lower+".c");
    Source.push_back(Monitor->Monitor_Root+"Guest/"+this->Plat->Name+
                     "/rvm_guest_"+this->Plat->Name_Lower+"_"+Tool->Name_Lower+Tool->Suffix(TOOL_ASSEMBLER));
    Source.push_back(Prc->Main_Source_Output+"prc_"+Prc->Name_Lower+".c");
    Source.push_back(Prc->Main_Source_Output+"prc_"+Prc->Name_Lower+"_desc.c");
    /* For native processes, add all the stub files */
    if(Prc->Type==PROCESS_NATIVE)
    {
        for(const std::unique_ptr<class Thread>& Thd:Prc->Thread)
            Source.push_back(Prc->Main_Source_Output+"prc_"+Prc->Name_Lower+"_thd_"+Thd->Name_Lower+".c");
        for(const std::unique_ptr<class Invocation>& Inv:Prc->Invocation)
            Source.push_back(Prc->Main_Source_Output+"prc_"+Prc->Name_Lower+"_inv_"+Inv->Name_Lower+".c");
    }
    /* For virtual machines, add all the VM files as well */
    else
        Source.insert(Source.end(), Virt->Virtual_Source.begin(), Virt->Virtual_Source.end());
    Gen_Tool::Path_Conv(Prc->Project_Output, Source);

    for(const std::string& Path:Source)
        Main::Info(std::string("> > ")+Path);

    /* Extract the linker paths */
    Main::Info("> Generating project linker paths:");

    Linker.push_back(Prc->Linker_Output+"prc_"+Prc->Name_Lower+"_"+this->Plat->Chip->Name_Lower+Tool->Suffix(TOOL_LINKER));
    Gen_Tool::Path_Conv(Prc->Project_Output, Linker);

    for(const std::string& Path:Linker)
        Main::Info(std::string("> > ")+Path);

    Build->Process_Proj(List, Include, Source, Linker, Prc);
    Gen_Tool::Line_Write(List, Prc->Project_Output+Prc->Project_Filename);
}
/* End Function:Gen_Tool::Process_Proj ***************************************/

/* Begin Function:Gen_Tool::Workspace_Proj ************************************
Description : Create the workspace file for all projects. This generation is only
              possible when (1) all workspace projects use the same build system,
              and (2) the build system supports a workspace project.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
void Gen_Tool::Workspace_Proj(void)
{
    class Build_Gen* Build;
    class Proj_Info* Proj;
    std::unique_ptr<std::vector<std::string>> List;
    std::vector<std::string> Project;

    Proj=this->Plat->Proj;

    /* The user can waive workspace generation if this "None" */
    if(Proj->Buildsystem=="None")
        return;

    /* This buildsystem must be in the map, and no other buildsystems other than it should exist */
    if((this->Build_Map.find(Proj->Buildsystem)==this->Build_Map.end())||(this->Build_Map.size()!=1))
    {
        Main::Info("> Underlying projects have hybrid buildsystems, skipping workspace generation.");
        return;
    }

    Build=this->Build_Map[Proj->Buildsystem];

    /* Does the file already exist? */
    Proj->Workspace_Filename=Proj->Name_Lower+Build->Suffix(BUILD_WORKSPACE);
    if(std::filesystem::exists(Proj->Workspace_Output+Proj->Workspace_Filename)==true)
    {
        /* See if we'll use forced regenerate */
        if(Proj->Workspace_Overwrite==0)
        {
            Main::Info(std::string("> File '")+Proj->Workspace_Filename+"' exists, skipping generation.");
            return;
        }
    }

    List=std::make_unique<std::vector<std::string>>();

    /* Add all project file positions */
    Project.push_back(Proj->Kernel->Project_Output+Proj->Kernel->Project_Filename);
    Project.push_back(Proj->Monitor->Project_Output+Proj->Monitor->Project_Filename);
    for(const std::unique_ptr<class Process>& Prc:Proj->Process)
        Project.push_back(Prc->Project_Output+Prc->Project_Filename);
    Gen_Tool::Path_Conv(Proj->Workspace_Output, Project);

    for(const std::string& Path:Project)
        Main::Info(std::string("> > ")+Path);

    Build->Workspace_Proj(List, Project);
    Gen_Tool::Line_Write(List, Proj->Workspace_Output+Proj->Workspace_Filename);
}
/* End Function:Gen_Tool::Workspace_Proj *************************************/
}
/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
