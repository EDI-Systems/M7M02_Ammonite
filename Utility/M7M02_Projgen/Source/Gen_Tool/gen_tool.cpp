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
#include "Gen_Tool/gen_tool.hpp"
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
        else
            Main::Error("PXXXX: Platform generator for '"+Name+"' is not found.");
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
            Main::Error("PXXXX: Buildsystem generator for '"+Name+"' is not found.");

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
            Main::Error("PXXXX: Toolchain generator for '"+Name+"' is not found.");

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
            Main::Error("PXXXX: Guest generator for '"+Name+"' is not found.");

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
            Main::Error("PXXXX: Cannot read source file '%s'.",Path.c_str());

        /* Read using text mode */
        File=fopen(Path.c_str(), "r");
        if(File==nullptr)
            Main::Error("PXXXX: Cannot read source file '%s'.",Path.c_str());

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
            Main::Error("PXXXX: Cannot read source file '%s'.",Path.c_str());

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
    /* Generator enabled */
    Gen_Tool::Macro_Int(List, "RME_RVM_GEN_ENABLE", 1, MACRO_REPLACE);
    /* The initial capability table size */
    Gen_Tool::Macro_Int(List, "RME_BOOT_CAPTBL_SIZE", Monitor->Captbl_Size, MACRO_REPLACE);
    /* The virtual memory start address for the kernel objects */
    Gen_Tool::Macro_Hex(List, "RME_KMEM_VA_BASE", Kernel->Kmem_Base, MACRO_REPLACE);
    /* The size of the kernel object virtual memory */
    Gen_Tool::Macro_Hex(List, "RME_KMEM_VA_SIZE", Kernel->Kmem_Size, MACRO_REPLACE);
    /* The virtual memory start address for the virtual machines - always full memory range, just in case */
    Gen_Tool::Macro_Hex(List, "RME_HYP_VA_BASE", 0x00000001ULL, MACRO_REPLACE);
    /* The size of the hypervisor reserved virtual memory */
    Gen_Tool::Macro_Hex(List, "RME_HYP_VA_SIZE", 0xFFFFFFFFULL, MACRO_REPLACE);
    /* Kernel stack base/size */
    Gen_Tool::Macro_Hex(List, "RME_KSTK_VA_BASE", Kernel->Stack_Base, MACRO_REPLACE);
    Gen_Tool::Macro_Hex(List, "RME_KSTK_VA_SIZE", Kernel->Stack_Size, MACRO_REPLACE);
    /* The maximum number of preemption priority levels in the system.
     * This parameter must be divisible by the word length - 32 is usually sufficient */
    Gen_Tool::Macro_Int(List, "RME_MAX_PREEMPT_PRIO", Kernel->Kern_Prio, MACRO_REPLACE);
    /* The granularity of kernel memory allocation, in bytes */
    Gen_Tool::Macro_Int(List, "RME_KMEM_SLOT_ORDER", Kernel->Kmem_Order, MACRO_REPLACE);
    /* Replace platform specific macros */
    this->Plat->Kernel_Conf_Hdr(List);
    /* Chip specific macros - we must be able to find it because we checked before */
    for(std::unique_ptr<class Conf_Info>& Conf:this->Plat->Chip->Config)
        Gen_Tool::Macro_String(List, Conf->Macro, this->Plat->Proj->Chip->Config[Conf->Name], MACRO_REPLACE);
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
    List->push_back("/* Vector endpoint capability tables */");

    /* Vector capability tables */
    Cap_Front=Kernel->Vect_Cap_Front;
    for(Obj_Cnt=0;Obj_Cnt<Monitor->Vector.size();Obj_Cnt+=this->Plat->Plat->Captbl_Max)
    {
        Gen_Tool::Macro_Int(List, std::string("RME_BOOT_CTVECT_")+std::to_string(Obj_Cnt/this->Plat->Plat->Captbl_Max),
                            Cap_Front, MACRO_ADD);
        Cap_Front++;
    }

    /* Vector endpoints */
    List->push_back("");
    List->push_back("/* Vector endpoints */");
    Obj_Cnt=0;
    for(const class Vect_Info* Vect:Monitor->Vector)
    {
        Gen_Tool::Macro_Int(List, Vect->Macro_Global, Obj_Cnt%this->Plat->Plat->Captbl_Max, MACRO_ADD);
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
    ptr_t Captbl_Size;
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
    for(const class Vect_Info* Vect:Monitor->Vector)
        List->push_back(std::string("static struct RME_Cap_Sig* ")+Vect->Name+"_Vect_Sig;");
    List->push_back("/* End Private Global Variables **********************************************/");
    List->push_back("");

    /* Private prototypes */
    Main::Info("> Generating private C function prototypes.");
    List->push_back("/* Private C Function Prototypes *********************************************/");
    for(const class Vect_Info* Vect:Monitor->Vector)
        List->push_back(std::string("extern rme_ptr_t RME_Vect_")+Vect->Name+"_User(rme_ptr_t Int_Num);");
    List->push_back("/* End Private C Function Prototypes *****************************************/");
    List->push_back("");

    /* Public prototypes */
    Main::Info("> Generating public C function prototypes.");
    List->push_back("/* Public C Function Prototypes **********************************************/");
    List->push_back("rme_ptr_t RME_Boot_Vect_Init(struct RME_Cap_Captbl* Captbl, rme_ptr_t Cap_Front, rme_ptr_t Kmem_Front);");
    List->push_back("rme_ptr_t RME_Boot_Vect_Handler(rme_ptr_t Vect_Num);");
    List->push_back("/* End Public C Function Prototypes ******************************************/");
    List->push_back("");

    /* Boot-time setup routine for the interrupt endpoints */
    Main::Info("> Generating boot-time vector endpoint initialization routine.");
    Input.push_back("rme_ptr_t Cap_Front - The current capability table frontier.");
    Input.push_back("rme_ptr_t Kmem_Front - The current kernel absolute memory frontier.");
    Gen_Tool::Func_Head(List, "RME_Boot_Vect_Init",
                        "Initialize all the vector endpoints at boot-time.",
                        Input, Output, "rme_ptr_t - The kernel memory frontier after booting is all done.");
    Input.clear();
    List->push_back("rme_ptr_t RME_Boot_Vect_Init(struct RME_Cap_Captbl* Captbl, rme_ptr_t Cap_Front, rme_ptr_t Kmem_Front)");
    List->push_back("{");
    List->push_back("    rme_ptr_t Cur_Addr;");
    List->push_back("    struct RME_Cap_Captbl* Sig_Captbl;");
    List->push_back("");
    List->push_back("    /* The address here shall match what is in the generator */");
    List->push_back(std::string("    RME_ASSERT(Cap_Front==")+std::to_string(Kernel->Vect_Cap_Front)+");");
    List->push_back(std::string("    RME_ASSERT(Kmem_Front==0x")+Main::Hex(Kernel->Vect_Kmem_Front+Kernel->Kmem_Base)+");");
    List->push_back("");
    List->push_back("    Cur_Addr=Kmem_Front;");
    List->push_back("    /* Create all the vector capability tables first */");
    for(Obj_Cnt=0;Obj_Cnt<Monitor->Vector.size();Obj_Cnt+=this->Plat->Plat->Captbl_Max)
    {
        /* See if this is the last (residue) captbl to create */
        if(Monitor->Vector.size()>=((Obj_Cnt+1)*this->Plat->Plat->Captbl_Max))
            Captbl_Size=this->Plat->Plat->Captbl_Max;
        else
            Captbl_Size=Monitor->Vector.size()%this->Plat->Plat->Captbl_Max;

        List->push_back(std::string("    RME_ASSERT(_RME_Captbl_Boot_Crt(Captbl, RME_BOOT_CAPTBL, RME_BOOT_CTVECT_")+
                        std::to_string(Obj_Cnt/this->Plat->Plat->Captbl_Max)+", Cur_Addr, "+std::to_string(Captbl_Size)+")==0);");
        List->push_back(std::string("    Cur_Addr+=RME_KOTBL_ROUND(RME_CAPTBL_SIZE(")+std::to_string(Captbl_Size)+"));");
    }
    List->push_back("");
    List->push_back("    /* Then all the vectors */");
    Obj_Cnt=0;
    for(const class Vect_Info* Vect:Monitor->Vector)
    {
        List->push_back(std::string("    Sig_Captbl=&(RME_CAP_GETOBJ(Captbl,struct RME_Cap_Captbl*)[RME_BOOT_CTVECT_")+
                        std::to_string(Obj_Cnt/this->Plat->Plat->Captbl_Max)+"]);");
        List->push_back(std::string("    ")+Vect->Name+
                        "_Vect_Sig=&(RME_CAP_GETOBJ(Sig_Captbl,struct RME_Cap_Sig*)["+Vect->Macro_Global+"]);");
        List->push_back(std::string("    RME_ASSERT(_RME_Sig_Boot_Crt(Captbl, RME_BOOT_CTVECT_")+
                        std::to_string(Obj_Cnt/this->Plat->Plat->Captbl_Max)+", "+Vect->Macro_Global+")==0);");
        Obj_Cnt++;
    }
    List->push_back("");
    List->push_back("    return Cur_Addr;");
    List->push_back("}");
    Gen_Tool::Func_Foot(List, "RME_Boot_Vect_Init");
    List->push_back("");

    /* Print the interrupt relaying function */
    Main::Info("> Generating interrupt relay facility.");
    Input.push_back("rme_ptr_t Vect_Num - The vector number.");
    Gen_Tool::Func_Head(List, "RME_Boot_Vect_Handler",
                        "The interrupt handler entry for all the vectors.",
                        Input, Output, "rme_ptr_t - The number of signals to send to the generic vector endpoint.");
    Input.clear();
    List->push_back("rme_ptr_t RME_Boot_Vect_Handler(rme_ptr_t Vect_Num)");
    List->push_back("{");
    List->push_back("    rme_ptr_t Send_Num;");
    List->push_back("");
    List->push_back("    Send_Num=0;");
    List->push_back("");
    List->push_back("    switch(Vect_Num)");
    List->push_back("    {");
    for(const class Vect_Info* Vect:Monitor->Vector)
    {
        List->push_back(std::string("        /* ")+Vect->Name+" */");
        List->push_back(std::string("        case ")+std::to_string(Vect->Number)+": ");
        List->push_back("        {");
        List->push_back(std::string("            extern rme_ptr_t RME_Vect_")+Vect->Name+"_User(rme_ptr_t Vect_Num);");
        List->push_back(std::string("            Send_Num=RME_Vect_")+Vect->Name+"_User(Vect_Num);");
        List->push_back(std::string("            _RME_Kern_Snd(")+Vect->Name+"_Vect_Sig);");
        List->push_back("            break;");
        List->push_back("        }");
    }
    List->push_back("        default: break;");
    List->push_back("    }");
    List->push_back("");
    List->push_back("    return Send_Num;");
    List->push_back("}");
    Gen_Tool::Func_Foot(List, "RME_Boot_Vect_Handler");
    List->push_back("");

    /* Generate rme_boot.c */
    Main::Info("> Writing file.");
    Gen_Tool::Src_Foot(List);
    Gen_Tool::Line_Write(List, Kernel->Boot_Source_Output+"rme_boot.c");
}
/* End Function:Gen_Tool::Kernel_Boot_Src ************************************/

/* Begin Function:Gen_Tool::Kernel_Init_Src ***********************************
Description : Create the initialization source file for kernel.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
void Gen_Tool::Kernel_Init_Src(void)
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
    if(std::filesystem::exists(Kernel->Init_Source_Output+"rme_init.c")==true)
    {
        /* See if we'll use forced regenerate */
        if(Kernel->Init_Source_Overwrite==0)
        {
            Main::Info("> File 'rme_init.c' exists, skipping generation.");
            return;
        }
    }

    /* File header */
    Main::Info("> Generating file header.");
    Gen_Tool::Src_Head(List, "rme_init.c", "The user initialization file.");
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
    List->push_back("rme_ret_t RME_User_Kern_Func_Handler(rme_ptr_t Func_ID, rme_ptr_t Sub_ID, rme_ptr_t Param1, rme_ptr_t Param2);");
    /* Add all the user vector stubs */
    for(const class Vect_Info* Vect:Monitor->Vector)
        List->push_back(std::string("rme_ptr_t RME_Vect_")+Vect->Name+"_User(rme_ptr_t Int_Num);");
    List->push_back("/* End Public C Function Prototypes ******************************************/");

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
    Main::Info("> Generating kernel function handler.");
    Input.push_back("rme_ptr_t Func_ID - The function ID.");
    Input.push_back("rme_ptr_t Sub_ID - The subfunction ID.");
    Input.push_back("rme_ptr_t Param1 - The first parameter.");
    Input.push_back("rme_ptr_t Param2 - The second parameter.");
    Gen_Tool::Func_Head(List, "RME_User_Kern_Func_Handler",
                        "User-modifiable kernel function handler.",
                        Input, Output, "rme_ret_t - The return value.");
    Input.clear();
    List->push_back("rme_ret_t RME_User_Kern_Func_Handler(rme_ptr_t Func_ID, rme_ptr_t Sub_ID, rme_ptr_t Param1, rme_ptr_t Param2)");
    List->push_back("{");
    List->push_back("    /* Add code here */");
    List->push_back("    return RME_ERR_KERN_OPFAIL;");
    List->push_back("}");
    Gen_Tool::Func_Foot(List, "RME_User_Kern_Func_Handler");
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
    Gen_Tool::Line_Write(List, Kernel->Init_Source_Output+"rme_init.c");
}
/* End Function:Gen_Tool::Kernel_Init_Src ************************************/

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

    Input.push_back("rme_ptr_t Vect_Num - The vector number.");
    for(const class Vect_Info* Vect:Monitor->Vector)
    {
        /* Generate filename */
        Filename=std::string("rme_handler_")+Vect->Name_Lower+".c";

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
        Gen_Tool::Src_Head(List, "Filename", std::string("The top-half handler file for vector ")+Vect->Name+".");
        List->push_back("");

        /* Print all header includes */
        Main::Info("> Generating C header includes.");
        this->Kernel_Inc(List);
        List->push_back("");

        /* Print all global prototypes */
        Main::Info("> Generating public C function prototypes.");
        List->push_back("/* Public C Function Prototypes **********************************************/");
        List->push_back(std::string("rme_ptr_t RME_Vect_")+Vect->Name+"_User(rme_ptr_t Int_Num);");
        List->push_back("/* End Public C Function Prototypes ******************************************/");

        /* Print function body */
        Main::Info(std::string("> Generating vector '")+Vect->Name+"' handler.");
        Gen_Tool::Func_Head(List, std::string("RME_Vect_")+Vect->Name+"_User",
                            "The user top-half interrupt handler for vector.",
                            Input, Output, "rme_ptr_t - Return 0 to send to the RME_BOOT_INIT_VECT, return other values to skip.");
        List->push_back(std::string("rme_ptr_t ")+std::string("RME_Vect_")+Vect->Name+"_User(rme_ptr_t Vect_Num)\n");
        List->push_back("{");
        List->push_back("    /* Add code here */");
        List->push_back("");
        List->push_back("    return 0;");
        List->push_back("}");
        Gen_Tool::Func_Foot(List, std::string("RME_Vect_")+Vect->Name+"_User");
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

    Source.push_back(Kernel->Kernel_Root+"Source/Kernel/rme_kernel.c");
    Source.push_back(Kernel->Kernel_Root+"Source/Platform/"+this->Plat->Name+"/rme_platform_"+this->Plat->Name_Lower+".c");
    Source.push_back(Kernel->Kernel_Root+"Source/Platform/"+this->Plat->Name+
                     "/rme_platform_"+this->Plat->Name_Lower+"_"+Tool->Name_Lower+Tool->Suffix(TOOL_ASSEMBLER));
    Source.push_back(Kernel->Boot_Source_Output+"rme_boot.c");
    Source.push_back(Kernel->Init_Source_Output+"rme_init.c");
    for(const class Vect_Info* Vect:Monitor->Vector)
        Source.push_back(Kernel->Handler_Source_Output+"rme_handler_"+Vect->Name_Lower+".c");
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
}
/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
