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
    /* The virtual memory base/size for the kernel objects */
    Gen_Tool::Macro_Hex(List, "RME_KMEM_VA_BASE", Kernel->Kmem_Base, MACRO_REPLACE);
    Gen_Tool::Macro_Hex(List, "RME_KMEM_VA_SIZE", Kernel->Kmem_Size, MACRO_REPLACE);
    /* The virtual memory base/size for the virtual machines - always full memory range, just in case */
    Gen_Tool::Macro_Hex(List, "RME_HYP_VA_BASE", 0x00000001ULL, MACRO_REPLACE);
    Gen_Tool::Macro_Hex(List, "RME_HYP_VA_SIZE", 0xFFFFFFFFULL, MACRO_REPLACE);
    /* Kernel stack base/size */
    Gen_Tool::Macro_Hex(List, "RME_KSTK_VA_BASE", Kernel->Stack_Base, MACRO_REPLACE);
    Gen_Tool::Macro_Hex(List, "RME_KSTK_VA_SIZE", Kernel->Stack_Size, MACRO_REPLACE);
    /* The maximum number of preemption priority levels in the system.
     * This parameter must be divisible by the word length - 32 is usually sufficient */
    Gen_Tool::Macro_Int(List, "RME_PREEMPT_PRIO_NUM", Kernel->Kern_Prio, MACRO_REPLACE);
    /* The granularity of kernel memory allocation, in bytes */
    Gen_Tool::Macro_Int(List, "RME_KMEM_SLOT_ORDER", Kernel->Kmem_Order, MACRO_REPLACE);
    /* Physical vector flag area base and its size */
    Gen_Tool::Macro_Hex(List, "RME_RVM_PHYS_VECT_BASE", Kernel->Vctf_Base, MACRO_REPLACE);
    Gen_Tool::Macro_Hex(List, "RME_RVM_PHYS_VECT_SIZE", Kernel->Vctf_Size, MACRO_REPLACE);
    /* Shared event flag region address */
    Gen_Tool::Macro_Hex(List, "RME_RVM_VIRT_EVENT_BASE", Kernel->Evtf_Base, MACRO_REPLACE);
    Gen_Tool::Macro_Hex(List, "RME_RVM_VIRT_EVENT_SIZE", Kernel->Evtf_Size, MACRO_REPLACE);
    /* Initial kernel object frontier limit */
    Gen_Tool::Macro_Hex(List, "RME_RVM_KMEM_BOOT_FRONTIER", Kernel->Kmem_Base+Monitor->Before_Kmem_Front, MACRO_REPLACE);

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
    Cap_Front=Kernel->Vect_Cap_Front;
    for(Obj_Cnt=0;Obj_Cnt<Monitor->Vector.size();Obj_Cnt+=this->Plat->Plat->Captbl_Max)
    {
        Gen_Tool::Macro_Int(List, std::string("RME_BOOT_CTVECT_")+CTIDS(Obj_Cnt), Cap_Front, MACRO_ADD);
        Cap_Front++;
    }

    /* Vector endpoints */
    List->push_back("");
    List->push_back("/* Vector endpoints */");
    Obj_Cnt=0;
    for(const class Vect_Info* Vect:Monitor->Vector)
    {
        Gen_Tool::Macro_Int(List, Vect->Macro_Global, OID(Obj_Cnt), MACRO_ADD);
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
        List->push_back(std::string("extern rme_ptr_t RME_Vect_")+Vect->Name+"_User(rme_ptr_t Vect_Num);");
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
    List->push_back(std::string("    RME_ASSERT(Cap_Front==")+std::to_string(Kernel->Vect_Cap_Front)+"U);");
    List->push_back(std::string("    RME_ASSERT(Kmem_Front==0x")+Main::Hex(Kernel->Vect_Kmem_Front+Kernel->Kmem_Base)+"U);");
    List->push_back("");
    List->push_back("    Cur_Addr=Kmem_Front;");
    List->push_back("    /* Create all the vector capability tables first */");
    for(Obj_Cnt=0;Obj_Cnt<Monitor->Vector.size();Obj_Cnt+=this->Plat->Plat->Captbl_Max)
    {
        Captbl_Size=CTSIZE(Monitor->Vector.size(), Obj_Cnt, this->Plat->Plat->Captbl_Max);
        List->push_back(std::string("    RME_ASSERT(_RME_Captbl_Boot_Crt(Captbl, RME_BOOT_CAPTBL, RME_BOOT_CTVECT_")+
                        CTIDS(Obj_Cnt)+", Cur_Addr, "+std::to_string(Captbl_Size)+"U)==0U);");
        List->push_back(std::string("    Cur_Addr+=RME_KOTBL_ROUND(RME_CAPTBL_SIZE(")+std::to_string(Captbl_Size)+"U));");
    }
    List->push_back("");
    List->push_back("    /* Then all the vectors */");
    Obj_Cnt=0;
    for(const class Vect_Info* Vect:Monitor->Vector)
    {
        List->push_back(std::string("    Sig_Captbl=&(RME_CAP_GETOBJ(Captbl,struct RME_Cap_Captbl*)[RME_BOOT_CTVECT_")+
                        CTIDS(Obj_Cnt)+"]);");
        List->push_back(std::string("    ")+Vect->Name+
                        "_Vect_Sig=&(RME_CAP_GETOBJ(Sig_Captbl,struct RME_Cap_Sig*)["+Vect->Macro_Global+"]);");
        List->push_back(std::string("    RME_ASSERT(_RME_Sig_Boot_Crt(Captbl, RME_BOOT_CTVECT_")+
                        CTIDS(Obj_Cnt)+", "+Vect->Macro_Global+")==0U);");
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
    List->push_back("    Send_Num=0U;");
    List->push_back("");
    List->push_back("    switch(Vect_Num)");
    List->push_back("    {");
    for(const class Vect_Info* Vect:Monitor->Vector)
    {
        List->push_back(std::string("        /* ")+Vect->Name+" */");
        List->push_back(std::string("        case ")+std::to_string(Vect->Number)+"U: ");
        List->push_back("        {");
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
    List->push_back("rme_ret_t RME_User_Kern_Func_Handler(rme_ptr_t Func_ID, rme_ptr_t Sub_ID, rme_ptr_t Param1, rme_ptr_t Param2);");
    /* Add all the user vector stubs */
    for(const class Vect_Info* Vect:Monitor->Vector)
        List->push_back(std::string("rme_ptr_t RME_Vect_")+Vect->Name+"_User(rme_ptr_t Int_Num);");
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
    Source.push_back(Kernel->Hook_Source_Output+"rme_hook.c");
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
    List->push_back("");
    List->push_back("#include \"rvm_boot.h\"");
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
    /* The virtual memory base/size for the kernel objects */
    Gen_Tool::Macro_Hex(List, "RVM_KMEM_VA_BASE", Kernel->Kmem_Base, MACRO_REPLACE);
    Gen_Tool::Macro_Hex(List, "RVM_KMEM_VA_SIZE", Kernel->Kmem_Size, MACRO_REPLACE);
    /* The granularity of kernel memory allocation, in bytes */
    Gen_Tool::Macro_Int(List, "RVM_KMEM_SLOT_ORDER", Kernel->Kmem_Order, MACRO_REPLACE);
    /* The maximum number of preemption priority levels in the system.
     * This parameter must be divisible by the word length - 32 is usually sufficient */
    Gen_Tool::Macro_Int(List, "RVM_PREEMPT_PRIO_NUM", Kernel->Kern_Prio, MACRO_REPLACE);
    /* Number of virtual priorities in the system */
    Gen_Tool::Macro_Int(List, "RVM_PREEMPT_VPRIO_NUM", Monitor->Virt_Prio, MACRO_REPLACE);

    /* Physical vector number, flag area base and its size */
    Gen_Tool::Macro_Int(List, "RVM_PHYS_VECT_NUM", this->Plat->Chip->Vect_Num, MACRO_REPLACE);
    Gen_Tool::Macro_Int(List, "RVM_PHYS_VECT_BASE", Kernel->Vctf_Base, MACRO_REPLACE);
    Gen_Tool::Macro_Int(List, "RVM_PHYS_VECT_SIZE", Kernel->Vctf_Size, MACRO_REPLACE);
    /* Virtual event number, flag area base and its size */
    Gen_Tool::Macro_Int(List, "RVM_VIRT_EVENT_NUM", Monitor->Virt_Event, MACRO_REPLACE);
    Gen_Tool::Macro_Int(List, "RVM_VIRT_EVENT_BASE", Kernel->Evtf_Base, MACRO_REPLACE);
    Gen_Tool::Macro_Int(List, "RVM_VIRT_EVENT_SIZE", Kernel->Evtf_Size, MACRO_REPLACE);
    /* Maximum number of mappings allowed */
    Gen_Tool::Macro_Int(List, "RVM_VIRT_MAP_NUM", Monitor->Virt_Map, MACRO_REPLACE);

    /* Stack base and size of the daemon threads, in bytes */
    Gen_Tool::Macro_Int(List, "RVM_SFTD_STACK_BASE", Monitor->Sftd_Stack_Base, MACRO_REPLACE);
    Gen_Tool::Macro_Int(List, "RVM_SFTD_STACK_SIZE", Monitor->Sftd_Stack_Size, MACRO_REPLACE);
    Gen_Tool::Macro_Int(List, "RVM_TIMD_STACK_BASE", Monitor->Timd_Stack_Base, MACRO_REPLACE);
    Gen_Tool::Macro_Int(List, "RVM_TIMD_STACK_SIZE", Monitor->Timd_Stack_Size, MACRO_REPLACE);
    Gen_Tool::Macro_Int(List, "RVM_VMMD_STACK_BASE", Monitor->Vmmd_Stack_Base, MACRO_REPLACE);
    Gen_Tool::Macro_Int(List, "RVM_VMMD_STACK_SIZE", Monitor->Vmmd_Stack_Size, MACRO_REPLACE);
    Gen_Tool::Macro_Int(List, "RVM_VCTD_STACK_BASE", Monitor->Vctd_Stack_Base, MACRO_REPLACE);
    Gen_Tool::Macro_Int(List, "RVM_VCTD_STACK_SIZE", Monitor->Vctd_Stack_Size, MACRO_REPLACE);
    /* Initial capability frontier limit */
    Gen_Tool::Macro_Int(List, "RVM_CAP_BOOT_FRONTIER", Monitor->Before_Cap_Front, MACRO_REPLACE);
    /* Initial kernel memory frontier limit */
    Gen_Tool::Macro_Int(List, "RVM_KMEM_BOOT_FRONTIER", Monitor->Before_Kmem_Front, MACRO_REPLACE);

    /* Stack safety redundancy, in bytes - fixed to 16 words */
    Gen_Tool::Macro_Int(List, "RVM_STACK_SAFE_RDCY", 16, MACRO_REPLACE);

    /* Replace platform specific macros */
    this->Plat->Monitor_Conf_Hdr(List);
    /* Write to Monitor configuration file */
    Gen_Tool::Line_Write(List, Monitor->Config_Header_Output+Filename);
}
/* End Function:Gen_Tool::Monitor_Conf_Hdr ***********************************/

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
    class Kernel* Kernel;
    class Monitor* Monitor;
    std::unique_ptr<std::vector<std::string>> List;

    Kernel=this->Plat->Proj->Kernel.get();
    Monitor=this->Plat->Proj->Monitor.get();
    List=std::make_unique<std::vector<std::string>>();

    Main::Info("> Generating file header.");
    Gen_Tool::Src_Head(List, "rvm_boot.h", "The boot-time initialization file header.");
    List->push_back("");

    Main::Info("> Generating defines.");
    List->push_back("/* Defines *******************************************************************/");

    /* Vector capability tables & objects */
    List->push_back("/* Vector endpoint capability tables */");
    Cap_Front=Kernel->Vect_Cap_Front;
    for(Obj_Cnt=0;Obj_Cnt<Monitor->Vector.size();Obj_Cnt+=this->Plat->Plat->Captbl_Max)
    {
        Gen_Tool::Macro_Int(List, std::string("RVM_BOOT_CTVECT_")+CTIDS(Obj_Cnt), Cap_Front, MACRO_ADD);
        Cap_Front++;
    }
    List->push_back("");
    List->push_back("/* Vector endpoints */");
    for(const class Vect_Info* Vect:Monitor->Vector)
    {
        Gen_Tool::Macro_String(List, Vect->Macro_Global,
                               std::string("RVM_CAPID(RVM_BOOT_CTVECT_")+
                               CTIDS(Vect->Capid_Global)+", "+OIDS(Vect->Capid_Global)+"U)", MACRO_ADD);
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
            Gen_Tool::Macro_Int(List, std::string("RVM_BOOT_CTVEP_")+CTIDS(Obj_Cnt), Cap_Front, MACRO_ADD);
            Cap_Front++;
        }
        if(Cap_Front!=Monitor->Captbl_Cap_Front)
            Main::Error("G1000: Virtual machine capability table computation failure.");
        List->push_back("");
    }

    /* Captbl capability tables & objects */
    List->push_back("/* Process capability table capability tables */");
    Cap_Front=Monitor->Captbl_Cap_Front;
    for(Obj_Cnt=0;Obj_Cnt<Monitor->Captbl.size();Obj_Cnt+=this->Plat->Plat->Captbl_Max)
    {
        Gen_Tool::Macro_Int(List, std::string("RVM_BOOT_CTCAPTBL_")+CTIDS(Obj_Cnt), Cap_Front, MACRO_ADD);
        Cap_Front++;
    }
    if(Cap_Front!=Monitor->Pgtbl_Cap_Front)
        Main::Error("G1000: Captbl capability table computation failure.");
    List->push_back("");
    List->push_back("/* Process capability tables */");
    for(const class Captbl* Captbl:Monitor->Captbl)
    {
        Gen_Tool::Macro_String(List, Captbl->Macro_Global,
                               std::string("RVM_CAPID(RVM_BOOT_CTCAPTBL_")+
                               CTIDS(Captbl->Capid_Global)+", "+OIDS(Captbl->Capid_Global)+"U)", MACRO_ADD);
    }
    List->push_back("");

    /* Pgtbl capability tables & objects */
    List->push_back("/* Process page table capability tables */");
    Cap_Front=Monitor->Pgtbl_Cap_Front;
    for(Obj_Cnt=0;Obj_Cnt<Monitor->Pgtbl.size();Obj_Cnt+=this->Plat->Plat->Captbl_Max)
    {
        Gen_Tool::Macro_Int(List, std::string("RVM_BOOT_CTPGTBL_")+CTIDS(Obj_Cnt), Cap_Front, MACRO_ADD);
        Cap_Front++;
    }
    if(Cap_Front!=Monitor->Proc_Cap_Front)
        Main::Error("G1000: Pgtbl capability table computation failure.");
    List->push_back("");
    List->push_back("/* Process page tables */");
    for(const class Pgtbl* Pgtbl:Monitor->Pgtbl)
    {
        Gen_Tool::Macro_String(List, Pgtbl->Macro_Global,
                               std::string("RVM_CAPID(RVM_BOOT_CTPGTBL_")+
                               CTIDS(Pgtbl->Capid_Global)+", "+OIDS(Pgtbl->Capid_Global)+"U)", MACRO_ADD);
    }
    List->push_back("");

    /* Process capability tables & objects */
    List->push_back("/* Process capability tables */");
    Cap_Front=Monitor->Proc_Cap_Front;
    for(Obj_Cnt=0;Obj_Cnt<Monitor->Process.size();Obj_Cnt+=this->Plat->Plat->Captbl_Max)
    {
        Gen_Tool::Macro_Int(List, std::string("RVM_BOOT_CTPROC_")+CTIDS(Obj_Cnt), Cap_Front, MACRO_ADD);
        Cap_Front++;
    }
    if(Cap_Front!=Monitor->Thd_Cap_Front)
        Main::Error("G1000: Process capability table computation failure.");
    List->push_back("");
    List->push_back("/* Processes */");
    for(const class Process* Proc:Monitor->Process)
    {
        Gen_Tool::Macro_String(List, Proc->Macro_Global,
                               std::string("RVM_CAPID(RVM_BOOT_CTPROC_")+
                               CTIDS(Proc->Capid_Global)+", "+ OIDS(Proc->Capid_Global)+"U)", MACRO_ADD);
    }
    List->push_back("");

    /* Thread capability tables & objects */
    List->push_back("/* Thread capability tables */");
    Cap_Front=Monitor->Thd_Cap_Front;
    for(Obj_Cnt=0;Obj_Cnt<Monitor->Thread.size();Obj_Cnt+=this->Plat->Plat->Captbl_Max)
    {
        Gen_Tool::Macro_Int(List, std::string("RVM_BOOT_CTTHD_")+CTIDS(Obj_Cnt), Cap_Front, MACRO_ADD);
        Cap_Front++;
    }
    if(Cap_Front!=Monitor->Inv_Cap_Front)
        Main::Error("G1000: Thread capability table computation failure.");
    List->push_back("");
    List->push_back("/* Threads */");
    for(const class Thread* Thd:Monitor->Thread)
    {
        Gen_Tool::Macro_String(List, Thd->Macro_Global,
                               std::string("RVM_CAPID(RVM_BOOT_CTTHD_")+
                               CTIDS(Thd->Capid_Global)+", "+OIDS(Thd->Capid_Global)+"U)", MACRO_ADD);
    }
    List->push_back("");

    /* Invocation capability tables & objects */
    List->push_back("/* Invocation capability tables */");
    Cap_Front=Monitor->Inv_Cap_Front;
    for(Obj_Cnt=0;Obj_Cnt<Monitor->Invocation.size();Obj_Cnt+=this->Plat->Plat->Captbl_Max)
    {
        Gen_Tool::Macro_Int(List, std::string("RVM_BOOT_CTINV_")+CTIDS(Obj_Cnt), Cap_Front, MACRO_ADD);
        Cap_Front++;
    }
    if(Cap_Front!=Monitor->Recv_Cap_Front)
        Main::Error("G1000: Invocation capability table computation failure.");
    List->push_back("");
    List->push_back("/* Invocations */");
    for(const class Invocation* Inv:Monitor->Invocation)
    {
        Gen_Tool::Macro_String(List, Inv->Macro_Global,
                               std::string("RVM_CAPID(RVM_BOOT_CTINV_")+
                               CTIDS(Inv->Capid_Global)+", "+OIDS(Inv->Capid_Global)+"U)", MACRO_ADD);
    }
    List->push_back("");

    /* Receive endpoint capability tables & objects */
    List->push_back("/* Receive endpoint capability tables */");
    Cap_Front=Monitor->Recv_Cap_Front;
    for(Obj_Cnt=0;Obj_Cnt<Monitor->Receive.size();Obj_Cnt+=this->Plat->Plat->Captbl_Max)
    {
        Gen_Tool::Macro_Int(List, std::string("RVM_BOOT_CTRECV_")+CTIDS(Obj_Cnt), Cap_Front, MACRO_ADD);
        Cap_Front++;
    }
    if(Cap_Front!=Monitor->After_Cap_Front)
        Main::Error("Receive endpoint:\nG1000: Internal capability table computation failure.");
    List->push_back("");
    List->push_back("/* Receive endpoints */");
    for(const class Receive* Recv:Monitor->Receive)
    {
        Gen_Tool::Macro_String(List, Recv->Macro_Global,
                               std::string("RVM_CAPID(RVM_BOOT_CTRECV_")+
                               CTIDS(Recv->Capid_Global)+", "+OIDS(Recv->Capid_Global)+"U)", MACRO_ADD);
    }
    List->push_back("");

    /* Extra capability table frontier */
    List->push_back("/* Capability table frontier */");
    Gen_Tool::Macro_Int(List, "RVM_BOOT_CAP_FRONTIER", Monitor->After_Cap_Front, MACRO_ADD);
    List->push_back("");
    /* Extra kernel memory frontier */
    List->push_back("/* Kernel memory frontier */");
    Gen_Tool::Macro_Hex(List, "RVM_BOOT_KMEM_FRONTIER", Monitor->After_Kmem_Front, MACRO_ADD);
    List->push_back("");

    List->push_back("/* End Defines ***************************************************************/");
    List->push_back("");

    Main::Info("> Writing file.");
    Gen_Tool::Src_Foot(List);
    Gen_Tool::Line_Write(List, Monitor->Boot_Header_Output+"rvm_boot.h");
}
/* End Function:Gen_Tool::Monitor_Boot_Hdr ***********************************/

/* Begin Function:Gen_Tool::Monitor_Pgtbl_Con *********************************
Description : Construct the page table for monitor. This will produce the desired
              final page table tree, and is recursive.
Input       : std::unique_ptr<std::vector<std::string>>& List - The file to add these to.
              const class Pgtbl* Pgtbl - The page table structure.
Output      : None.
Return      : None.
******************************************************************************/
void Gen_Tool::Monitor_Pgtbl_Con(std::unique_ptr<std::vector<std::string>>& List,
                                 const class Pgtbl* Pgtbl)
{
    ptr_t Count;
    class Pgtbl* Child;

    /* Construct whatever page table to this page table */
    for(Count=0;Count<POW2(Pgtbl->Num_Order);Count++)
    {
        Child=Pgtbl->Pgdir[Count].get();
        if(Child==nullptr)
            continue;
        List->push_back(std::string("    RVM_ASSERT(RVM_Pgtbl_Con(")+
                        Pgtbl->Macro_Global+", 0x"+
                        Main::Hex(Count)+"U, "+
                        Child->Macro_Global+", RVM_PGTBL_ALL_PERM)==0U);");
        List->push_back(std::string("    RVM_LOG_S(\"Init:Constructed page table child '")+
                        Child->Macro_Global+"' into page table parent '"+
                        Pgtbl->Macro_Global+"' @ position 0x"+
                        Main::Hex(Count)+".\\r\\n\");");
        /* Recursively call this for all the page tables */
        this->Monitor_Pgtbl_Con(List, Child);
    }
}
/* End Function:Gen_Tool::Monitor_Pgtbl_Con **********************************/

/* Begin Function:Gen_Tool::Monitor_Pgtbl_Map *********************************
Description : Map pages into a page table. This is not recursive.
Input       : std::unique_ptr<std::vector<std::string>>& List - The file to add these to.
              const class Pgtbl* Pgtbl - The page table structure.
              ptr_t Init_Size_Ord - The initial page table's number order.
Output      : None.
Return      : None.
******************************************************************************/
void Gen_Tool::Monitor_Pgtbl_Map(std::unique_ptr<std::vector<std::string>>& List,
                                 const class Pgtbl* Pgtbl, ptr_t Init_Size_Ord)
{
    ptr_t Count;
    ptr_t Attr;
    ptr_t Pos_Src;
    ptr_t Index;
    ptr_t Page_Start;
    ptr_t Page_Size;
    ptr_t Page_Num;
    ptr_t Init_Size;
    std::string Flags;

    Page_Size=POW2(Pgtbl->Size_Order);
    Page_Num=POW2(Pgtbl->Num_Order);
    Init_Size=POW2(Init_Size_Ord);

    /* Map whatever pages into this page table */
    for(Count=0;Count<Page_Num;Count++)
    {
        Attr=Pgtbl->Page[Count];
        if(Attr==0)
            continue;
        /* Compute flags */
        if((Attr&MEM_READ)!=0)
            Flags+="RVM_PGTBL_READ|";
        if((Attr&MEM_WRITE)!=0)
            Flags+="RVM_PGTBL_WRITE|";
        if((Attr&MEM_EXECUTE)!=0)
            Flags+="RVM_PGTBL_EXECUTE|";
        if((Attr&MEM_CACHE)!=0)
            Flags+="RVM_PGTBL_CACHE|";
        if((Attr&MEM_BUFFER)!=0)
            Flags+="RVM_PGTBL_BUFFER|";
        if((Attr&MEM_STATIC)!=0)
            Flags+="RVM_PGTBL_STATIC|";
        /* Pop off the final | */
        Flags.pop_back();
        /* Compute Pos_Src and Index */
        Page_Start=Pgtbl->Base+Count*Page_Size;
        Pos_Src=Page_Start/Init_Size;
        Index=(Page_Start-Pos_Src*Init_Size)/Page_Size;
        List->push_back(std::string("    RVM_ASSERT(RVM_Pgtbl_Add(")+
                        Pgtbl->Macro_Global+", 0x"+Main::Hex(Count)+"U, \n"+
                        "                             "+Flags+", \n"+
                        "                             RVM_BOOT_PGTBL, 0x"+
                        Main::Hex(Pos_Src)+"U, 0x"+Main::Hex(Index)+"U)==0U);");
        List->push_back(std::string("    RVM_LOG_S(\"Init:Added page addr 0x")+
                        Main::Hex(Page_Start)+"U size 0x"+
                        Main::Hex(Page_Size)+" to page table '"+
                        Pgtbl->Macro_Global+"' @ position 0x"+
                        Main::Hex(Count)+".\\r\\n\");");
    }
}
/* End Function:Gen_Tool::Monitor_Pgtbl_Map **********************************/

/* Begin Function:Gen_Tool::Monitor_Boot_Src **********************************
Description : Create the boot source for monitor. This function is pretty lengthy,
              but it generates a single source, so we're not splitting it.
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

    /* Print global variables for virtual machine management - don't print when no VMs exist */
    if(this->Plat->Proj->Virtual.size()!=0)
    {
        Main::Info("> Generating global variables.");
        List->push_back("/* Public Global Varibles ****************************************************/");
        List->push_back(std::string("struct RVM_Virt_Struct RVM_Virt[")+std::to_string(this->Plat->Proj->Virtual.size())+"];");
        List->push_back(std::string("const struct RVM_Vmap_Struct RVM_Vmap[")+std::to_string(this->Plat->Proj->Virtual.size())+"]=");
        List->push_back("{");
        Obj_Cnt=0;
        for(class Virtual* Virt:this->Plat->Proj->Virtual)
        {
            List->push_back(std::string("{(rvm_s8_t*)\"")+Virt->Name+"\", "+
                            std::to_string(Virt->Priority)+"U, "+
                            std::to_string(Virt->Timeslice)+"U, "+
                            std::to_string(Virt->Period)+"U, "+
                            std::to_string(Virt->Watchdog)+"U, "+
                            std::to_string(Virt->Vect_Num)+"U, ");
            List->push_back(std::string(" (struct RVM_Regs*)0x")+
                            Main::Hex(Virt->Reg_Base)+"U, (struct RVM_Param*)0x"+
                            Main::Hex(Virt->Param_Base)+"U,");
            List->push_back(std::string(" (struct RVM_Vctf*)0x")+
                            Main::Hex(Virt->Vctf_Base)+"U, 0x"+
                            Main::Hex(Virt->Entry_Code_Front)+"U, RVM_CAPID(RVM_BOOT_CTVEP_"+
                            CTIDS(Obj_Cnt)+", "+OIDS(Obj_Cnt)+"U),");
            List->push_back(std::string(" ")+
                            Virt->Thread[0]->Macro_Global+", "+
                            Virt->Thread[0]->Macro_Global+", 0x"+
                            Main::Hex(Virt->Thread[0]->Entry_Addr)+"U, 0x"+
                            Main::Hex(Virt->Thread[0]->Stack_Base)+"U, 0x"+
                            Main::Hex(Virt->Thread[0]->Stack_Size)+"U,");
            List->push_back(std::string(" ")+
                            Virt->Thread[1]->Macro_Global+", "+
                            Virt->Thread[1]->Macro_Global+", 0x"+
                            Main::Hex(Virt->Thread[1]->Entry_Addr)+"U, 0x"+
                            Main::Hex(Virt->Thread[1]->Stack_Base)+"U, 0x"+
                            Main::Hex(Virt->Thread[1]->Stack_Size)+"U},");
            Obj_Cnt++;
        }
        List->push_back("};");
        List->push_back("/* End Public Global Varibles ************************************************/");
        List->push_back("");
    }

    /* Private prototypes */
    Main::Info("> Generating private C function prototypes.");
    List->push_back("/* Private C Function Prototypes *********************************************/");
    List->push_back("/* Kernel object creation */");
    if(this->Plat->Proj->Virtual.size()!=0)
        List->push_back("static void RVM_Boot_Vep_Crt(void);");
    List->push_back("static void RVM_Boot_Captbl_Crt(void);");
    List->push_back("static void RVM_Boot_Pgtbl_Crt(void);");
    List->push_back("static void RVM_Boot_Proc_Crt(void);");
    List->push_back("static void RVM_Boot_Thd_Crt(void);");
    List->push_back("static void RVM_Boot_Inv_Crt(void);");
    List->push_back("static void RVM_Boot_Recv_Crt(void);");
    List->push_back("");
    List->push_back("/* Kernel object initialization */");
    if(this->Plat->Proj->Virtual.size()!=0)
        List->push_back("static void RVM_Boot_Vcap_Init(void);");
    List->push_back("static void RVM_Boot_Captbl_Init(void);");
    List->push_back("static void RVM_Boot_Pgtbl_Init(void);");
    List->push_back("static void RVM_Boot_Thd_Init(void);");
    List->push_back("static void RVM_Boot_Inv_Init(void);");
    List->push_back("/* End Private C Function Prototypes *****************************************/");
    List->push_back("");

    /* Public prototypes */
    Main::Info("> Generating public C function prototypes.");
    List->push_back("/* Public C Function Prototypes **********************************************/");
    List->push_back("void RVM_Boot_Kobj_Crt(void);");
    List->push_back("void RVM_Boot_Kobj_Init(void);");
    List->push_back("/* End Public C Function Prototypes ******************************************/");
    List->push_back("");

    /* Virtual endpoint creation */
    if(this->Plat->Proj->Virtual.size()!=0)
    {
        Main::Info("> Generating VM vector endpoint creation code.");
        Gen_Tool::Func_Head(List, "RVM_Boot_Vep_Crt",
                            "Create all VM vector endpoints at boot-time.", Input, Output, "None.");
        List->push_back("void RVM_Boot_Vep_Crt(void)");
        List->push_back("{");
        List->push_back("    rvm_ptr_t Cur_Addr;");
        List->push_back("");
        List->push_back(std::string("    Cur_Addr=0x")+Main::Hex(Monitor->Vep_Kmem_Front)+"U;");
        List->push_back("    RVM_LOG_S(\"-------------------------------------------------------------------------------\\r\\n\");");
        List->push_back("    RVM_LOG_S(\"Init:Creating virtual machine endpoints.\\r\\n\");");
        List->push_back("    /* Create all the virtual machine endpoint capability tables first */");
        Monitor_Captbl_Crt_Gen(List, this->Plat->Proj->Virtual, "VM vector endpoint", "VEP", this->Plat->Plat->Captbl_Max);
        List->push_back("");
        List->push_back("    /* Then the endpoints themselves */");
        Obj_Cnt=0;
        for(class Virtual* Virt:this->Plat->Proj->Virtual)
        {
            List->push_back(std::string("    RVM_ASSERT(RVM_Sig_Crt(RVM_BOOT_CTVEP_")+
                            CTIDS(Obj_Cnt)+", "+OIDS(Obj_Cnt)+"U)==0U);");
            List->push_back(std::string("    RVM_LOG_SIS(\"Init:Created virtual machine '")+
                            Virt->Name+"' endpoint CID \", RVM_CAPID(RVM_BOOT_CTVEP_"+
                            CTIDS(Obj_Cnt)+", "+OIDS(Obj_Cnt)+"U), \".\\r\\n\");");
            Obj_Cnt++;
        }
        List->push_back("");
        List->push_back(std::string("    RVM_ASSERT(Cur_Addr==0x")+Main::Hex(Monitor->Captbl_Kmem_Front)+"U);");
        List->push_back("}");
        Gen_Tool::Func_Foot(List, "RVM_Boot_Vep_Crt");
        List->push_back("");
    }

    /* Capability table creation */
    Main::Info("> Generating capability table creation code.");
    Gen_Tool::Func_Head(List, "RVM_Boot_Captbl_Crt",
                        "Create all capability tables at boot-time.", Input, Output, "None.");
    List->push_back("void RVM_Boot_Captbl_Crt(void)");
    List->push_back("{");
    List->push_back("    rvm_ptr_t Cur_Addr;");
    List->push_back("");
    List->push_back(std::string("    Cur_Addr=0x")+Main::Hex(Monitor->Captbl_Kmem_Front)+"U;");
    List->push_back("    RVM_LOG_S(\"-------------------------------------------------------------------------------\\r\\n\");");
    List->push_back("    RVM_LOG_S(\"Init:Creating process capability tables.\\r\\n\");");
    List->push_back("    /* Create all the capability table capability tables first */");
    Monitor_Captbl_Crt_Gen(List, Monitor->Captbl, "capability table", "CAPTBL", this->Plat->Plat->Captbl_Max);
    List->push_back("");
    List->push_back("    /* Then the capability tables themselves */");
    for(const class Captbl* Captbl:Monitor->Captbl)
    {
        List->push_back(std::string("    RVM_ASSERT(RVM_Captbl_Crt(RVM_BOOT_CTCAPTBL_")+
                        CTIDS(Captbl->Capid_Global)+", RVM_BOOT_INIT_KMEM, "+OIDS(Captbl->Capid_Global)+"U, Cur_Addr, "+
                        std::to_string(Captbl->Size)+"U)==0U);");
        List->push_back(std::string("    RVM_LOG_SISUS(\"Init:Created capability table '")+
                        Captbl->Macro_Global+"' CID \", RVM_CAPID(RVM_BOOT_CTCAPTBL_"+
                        CTIDS(Captbl->Capid_Global)+", "+OIDS(Captbl->Capid_Global)+"U), \" @ address 0x\", Cur_Addr, \".\\r\\n\");");
        List->push_back(std::string("    Cur_Addr+=RVM_KOTBL_ROUND(RVM_CAPTBL_SIZE(")+std::to_string(Captbl->Size)+"U));");
    }
    List->push_back("");
    List->push_back(std::string("    RVM_ASSERT(Cur_Addr==0x")+Main::Hex(Monitor->Pgtbl_Kmem_Front)+"U);");
    List->push_back("}");
    Gen_Tool::Func_Foot(List, "RVM_Boot_Captbl_Crt");
    List->push_back("");

    /* Page table creation */
    Main::Info("> Generating page table creation code.");
    Gen_Tool::Func_Head(List, "RVM_Boot_Pgtbl_Crt",
                        "Create all page tables at boot-time.", Input, Output, "None.");
    List->push_back("void RVM_Boot_Pgtbl_Crt(void)");
    List->push_back("{");
    List->push_back("    rvm_ptr_t Cur_Addr;");
    List->push_back("");
    List->push_back(std::string("    Cur_Addr=0x")+Main::Hex(Monitor->Pgtbl_Kmem_Front)+"U;");
    List->push_back("    RVM_LOG_S(\"-------------------------------------------------------------------------------\\r\\n\");");
    List->push_back("    RVM_LOG_S(\"Init:Creating process page tables.\\r\\n\");");
    List->push_back("    /* Create all the page tables capability tables first */");
    Monitor_Captbl_Crt_Gen(List, Monitor->Pgtbl, "page table", "PGTBL", this->Plat->Plat->Captbl_Max);
    List->push_back("");
    List->push_back("    /* Then the page tables themselves */");
    for(const class Pgtbl* Pgtbl:Monitor->Pgtbl)
    {
        List->push_back(std::string("    RVM_ASSERT(RVM_Pgtbl_Crt(RVM_BOOT_CTPGTBL_")+
                        CTIDS(Pgtbl->Capid_Global)+", RVM_BOOT_INIT_KMEM, "+OIDS(Pgtbl->Capid_Global)+"U, Cur_Addr, 0x"+
                        Main::Hex(Pgtbl->Base)+"U, "+std::to_string((ptr_t)(Pgtbl->Is_Top!=0))+"U, "+
                        std::to_string(Pgtbl->Size_Order)+"U, "+std::to_string(Pgtbl->Num_Order)+"U)==0U);");
        List->push_back(std::string("    RVM_LOG_SISUS(\"Init:Created page table '")+
                        Pgtbl->Macro_Global+"' CID \", RVM_CAPID(RVM_BOOT_CTPGTBL_"+
                        CTIDS(Pgtbl->Capid_Global)+", "+OIDS(Pgtbl->Capid_Global)+"U), \" @ address 0x\", Cur_Addr, \".\\r\\n\");");
        List->push_back(std::string("    Cur_Addr+=0x")+Main::Hex(this->Plat->Size_Pgtbl(Pgtbl->Num_Order, Pgtbl->Is_Top))+"U;");
    }
    List->push_back("");
    List->push_back(std::string("    RVM_ASSERT(Cur_Addr==0x")+Main::Hex(Monitor->Proc_Kmem_Front)+");");
    List->push_back("}");
    Gen_Tool::Func_Foot(List, "RVM_Boot_Pgtbl_Crt");
    List->push_back("");

    /* Process creation */
    Main::Info("> Generating process creation code.");
    Gen_Tool::Func_Head(List, "RVM_Boot_Proc_Crt",
                        "Create all processes at boot-time.", Input, Output, "None.");
    List->push_back("void RVM_Boot_Proc_Crt(void)");
    List->push_back("{");
    List->push_back("    rvm_ptr_t Cur_Addr;");
    List->push_back("");
    List->push_back(std::string("    Cur_Addr=0x")+Main::Hex(Monitor->Proc_Kmem_Front)+"U;");
    List->push_back("    RVM_LOG_S(\"-------------------------------------------------------------------------------\\r\\n\");");
    List->push_back("    RVM_LOG_S(\"Init:Creating processes.\\r\\n\");");
    List->push_back("    /* Create all the process capability tables first */");
    Monitor_Captbl_Crt_Gen(List, Monitor->Process, "process", "PROC", this->Plat->Plat->Captbl_Max);
    List->push_back("");
    List->push_back("    /* Then the processes themselves */");
    for(const class Process* Proc:Monitor->Process)
    {
        List->push_back(std::string("    RVM_ASSERT(RVM_Proc_Crt(RVM_BOOT_CTPROC_")+
                        CTIDS(Proc->Capid_Global)+", "+OIDS(Proc->Capid_Global)+"U, "+
                        Proc->Captbl->Macro_Global+", "+Proc->Pgtbl->Macro_Global+")==0U);");
        List->push_back(std::string("    RVM_LOG_SIS(\"Init:Created process '")+
                        Proc->Macro_Global+"' CID \", RVM_CAPID(RVM_BOOT_CTPROC_"+
                        CTIDS(Proc->Capid_Global)+", "+OIDS(Proc->Capid_Global)+"U), \".\\r\\n\");");
    }
    List->push_back("");
    List->push_back(std::string("    RVM_ASSERT(Cur_Addr==0x")+Main::Hex(Monitor->Thd_Kmem_Front)+"U);");
    List->push_back("}");
    Gen_Tool::Func_Foot(List, "RVM_Boot_Proc_Crt");
    List->push_back("");

    /* Thread creation */
    Main::Info("> Generating thread creation code.");
    Gen_Tool::Func_Head(List, "RVM_Boot_Thd_Crt",
                        "Create all threads at boot-time.", Input, Output, "None.");
    List->push_back("void RVM_Boot_Thd_Crt(void)");
    List->push_back("{");
    List->push_back("    rvm_ptr_t Cur_Addr;");
    List->push_back("");
    List->push_back(std::string("    Cur_Addr=0x")+Main::Hex(Monitor->Thd_Kmem_Front)+"U;");
    List->push_back("    RVM_LOG_S(\"-------------------------------------------------------------------------------\\r\\n\");");
    List->push_back("    RVM_LOG_S(\"Init:Creating threads.\\r\\n\");");
    List->push_back("    /* Create all the thread capability tables first */");
    Monitor_Captbl_Crt_Gen(List, Monitor->Thread, "thread", "THD", this->Plat->Plat->Captbl_Max);
    List->push_back("");
    List->push_back("    /* Then the threads themselves */");
    for(const class Thread* Thd:Monitor->Thread)
    {
        List->push_back(std::string("    RVM_ASSERT(RVM_Thd_Crt(RVM_BOOT_CTTHD_")+
                        CTIDS(Thd->Capid_Global)+", RVM_BOOT_INIT_KMEM, "+OIDS(Thd->Capid_Global)+"U, "+
                        Thd->Owner->Macro_Global+", "+std::to_string(Thd->Priority)+"U, Cur_Addr)==0U);");
        List->push_back(std::string("    RVM_LOG_SISUS(\"Init:Created thread '")+
                        Thd->Macro_Global+"' CID \", RVM_CAPID(RVM_BOOT_CTTHD_"+
                        CTIDS(Thd->Capid_Global)+", "+OIDS(Thd->Capid_Global)+"U), \" @ address 0x\", Cur_Addr, \".\\r\\n\");");
        List->push_back("    Cur_Addr+=RVM_KOTBL_ROUND(RVM_THD_SIZE);");
    }
    List->push_back("");
    List->push_back(std::string("    RVM_ASSERT(Cur_Addr==0x")+Main::Hex(Monitor->Inv_Kmem_Front)+"U);");
    List->push_back("}");
    Gen_Tool::Func_Foot(List, "RVM_Boot_Thd_Crt");
    List->push_back("");

    /* Invocation creation */
    Main::Info("> Generating invocation creation code.");
    Gen_Tool::Func_Head(List, "RVM_Boot_Inv_Crt",
                        "Create all invocations at boot-time.", Input, Output, "None.");
    List->push_back("void RVM_Boot_Inv_Crt(void)");
    List->push_back("{");
    List->push_back("    rvm_ptr_t Cur_Addr;");
    List->push_back("");
    List->push_back(std::string("    Cur_Addr=0x")+Main::Hex(Monitor->Inv_Kmem_Front)+"U;");
    List->push_back("    RVM_LOG_S(\"-------------------------------------------------------------------------------\\r\\n\");");
    List->push_back("    RVM_LOG_S(\"Init:Creating invocations.\\r\\n\");");
    List->push_back("    /* Create all the invocation capability tables first */");
    Monitor_Captbl_Crt_Gen(List, Monitor->Invocation, "invocation", "INV", this->Plat->Plat->Captbl_Max);
    List->push_back("");
    List->push_back("    /* Then the invocations themselves */");
    for(const class Invocation* Inv:Monitor->Invocation)
    {
        List->push_back(std::string("    RVM_ASSERT(RVM_Inv_Crt(RVM_BOOT_CTINV_")+
                        CTIDS(Inv->Capid_Global)+", RVM_BOOT_INIT_KMEM, "+OIDS(Inv->Capid_Global)+"U, "+
                        Inv->Owner->Macro_Global+", Cur_Addr)==0U);");
        List->push_back(std::string("    RVM_LOG_SISUS(\"Init:Created invocation '")+
                        Inv->Macro_Global+"' CID \", RVM_CAPID(RVM_BOOT_CTINV_"+
                        CTIDS(Inv->Capid_Global)+", "+OIDS(Inv->Capid_Global)+"U), \" @ address 0x\", Cur_Addr, \".\\r\\n\");");
        List->push_back("    Cur_Addr+=RVM_KOTBL_ROUND(RVM_INV_SIZE);");
    }
    List->push_back("");
    List->push_back(std::string("    RVM_ASSERT(Cur_Addr==0x")+Main::Hex(Monitor->Recv_Kmem_Front)+"U);");
    List->push_back("}");
    Gen_Tool::Func_Foot(List, "RVM_Boot_Inv_Crt");
    List->push_back("");

    /* Receive endpoint creation */
    Main::Info("> Generating receive endpoint creation code.");
    Gen_Tool::Func_Head(List, "RVM_Boot_Recv_Crt",
                        "Create all receive endpoints at boot-time.", Input, Output, "None.");
    List->push_back("void RVM_Boot_Recv_Crt(void)");
    List->push_back("{");
    List->push_back("    rvm_ptr_t Cur_Addr;");
    List->push_back("");
    List->push_back(std::string("    Cur_Addr=0x")+Main::Hex(Monitor->Recv_Kmem_Front)+"U;");
    List->push_back("    RVM_LOG_S(\"-------------------------------------------------------------------------------\\r\\n\");");
    List->push_back("    RVM_LOG_S(\"Init:Creating receive endpoints.\\r\\n\");");
    List->push_back("    /* Create all the receive endpoint capability tables first */");
    Monitor_Captbl_Crt_Gen(List, Monitor->Receive, "receive endpoint", "RECV", this->Plat->Plat->Captbl_Max);
    List->push_back("");
    List->push_back("    /* Then the receive endpoints themselves */");
    for(const class Receive* Recv:Monitor->Receive)
    {
        List->push_back(std::string("    RVM_ASSERT(RVM_Sig_Crt(RVM_BOOT_CTRECV_")+
                        CTIDS(Recv->Capid_Global)+", "+OIDS(Recv->Capid_Global)+"U)==0U);");
        List->push_back(std::string("    RVM_LOG_SIS(\"Init:Created receive endpoint '")+
                        Recv->Macro_Global+"' CID \", RVM_CAPID(RVM_BOOT_CTRECV_"+
                        CTIDS(Recv->Capid_Global)+", "+OIDS(Recv->Capid_Global)+"U), \".\\r\\n\");");
    }
    List->push_back("");
    List->push_back(std::string("    RVM_ASSERT(Cur_Addr==0x")+Main::Hex(Monitor->After_Kmem_Front)+"U);");
    List->push_back("}");
    Gen_Tool::Func_Foot(List, "RVM_Boot_Recv_Crt");
    List->push_back("");

    /* Main creation function */
    Main::Info("> Generating master kernel object creation code.");
    Gen_Tool::Func_Head(List, "RVM_Boot_Kobj_Crt",
                        "Create all kernel objects at boot-time.", Input, Output, "None.");
    List->push_back("void RVM_Boot_Kobj_Crt(void)");
    List->push_back("{");
    if(this->Plat->Proj->Virtual.size()!=0)
        List->push_back("    RVM_Boot_Vep_Crt();");
    List->push_back("    RVM_Boot_Captbl_Crt();");
    List->push_back("    RVM_Boot_Pgtbl_Crt();");
    List->push_back("    RVM_Boot_Proc_Crt();");
    List->push_back("    RVM_Boot_Thd_Crt();");
    List->push_back("    RVM_Boot_Inv_Crt();");
    List->push_back("    RVM_Boot_Recv_Crt();");
    List->push_back("}");
    Gen_Tool::Func_Foot(List, "RVM_Boot_Kobj_Crt");
    List->push_back("");

    /* VM object initialization */
    if(this->Plat->Proj->Virtual.size()!=0)
    {
        Main::Info("> Generating VM object initialization code.");
        Gen_Tool::Func_Head(List, "RVM_Boot_Vcap_Init",
                            "Initialize all VM capability table special contents.", Input, Output, "None.");
        List->push_back("void RVM_Boot_Vcap_Init(void)");
        List->push_back("{");
        List->push_back("    RVM_LOG_S(\"-------------------------------------------------------------------------------\\r\\n\");");
        List->push_back("    RVM_LOG_S(\"Init:Initializing virtual machine capability tables.\\r\\n\");");
        Obj_Cnt=0;
        for(const class Virtual* Virt:this->Plat->Proj->Virtual)
        {
            List->push_back(std::string("    /* Initializing virtual machine '")+Virt->Name+"' */");
            /* Setup system call send endpoint at capability table location 0 */
            List->push_back("    /* System call endpoint */");
            List->push_back(std::string("    RVM_ASSERT(RVM_Captbl_Add(")+
                            Virt->Captbl->Macro_Global+", 0U, RVM_BOOT_CAPTBL, RVM_Vmmd_Sig_Cap, RVM_SIG_FLAG_SND)==0U);");
            List->push_back(std::string("    RVM_LOG_S(\"Init:Delegated system call send endpoint to virtual machine '")+
                            Virt->Macro_Global+"'.\\r\\n\");");
            /* Setup virtual machine vector endpoint at capability table location 1 */
            List->push_back("    /* Vector endpoint - this is a special one */");
            List->push_back(std::string("    RVM_ASSERT(RVM_Captbl_Add(")+
                            Virt->Captbl->Macro_Global+", 1U, RVM_BOOT_CTVEP_"+
                            CTIDS(Obj_Cnt)+", "+OIDS(Obj_Cnt)+"U, RVM_SIG_FLAG_SND|RVM_SIG_FLAG_RCV)==0U);");
            List->push_back(std::string("    RVM_LOG_S(\"Init:Delegated system call send endpoint to virtual machine '")+
                            Virt->Macro_Global+"'.\\r\\n\");");
            List->push_back("");
            /* Initialize virtual machine data structure */
            Obj_Cnt++;
        }
        List->push_back("}");
        Gen_Tool::Func_Foot(List, "RVM_Boot_Vcap_Init");
        List->push_back("");
    }

    /* Capability table initialization */
    Main::Info("> Generating capability table initialization code.");
    Gen_Tool::Func_Head(List, "RVM_Boot_Captbl_Init",
                        "Initialize the capability tables of all processes.", Input, Output, "None.");
    List->push_back("void RVM_Boot_Captbl_Init(void)");
    List->push_back("{");
    List->push_back("    RVM_LOG_S(\"-------------------------------------------------------------------------------\\r\\n\");");
    List->push_back("    RVM_LOG_S(\"Init:Initializing process capability tables.\\r\\n\");");
    for(const std::unique_ptr<class Process>& Proc:this->Plat->Proj->Process)
    {
        List->push_back(std::string("    /* Initializing captbl for process: '")+Proc->Name+"' */");
        /* If this is not a virtual machine, we intialize the slot 0 as kernel capability
         * for sending events to the RVM's Vctd */
        if(Proc->Type==PROC_NATIVE)
        {
            List->push_back("    /* Event kernel capability */");
            List->push_back(std::string("    RVM_ASSERT(RVM_Captbl_Kern(")+
                            Proc->Captbl->Macro_Global+", 0U, RVM_BOOT_CAPTBL, RVM_BOOT_INIT_KERN, RVM_KERN_EVT_LOCAL_TRIG, RVM_KERN_EVT_LOCAL_TRIG)==0U);");
            List->push_back(std::string("    RVM_LOG_S(\"Init:Delegated kernel capability for event sending to process '")+
                            Proc->Macro_Global+"' capability table position 0.\\r\\n\");");
        }
        /* Ports */
        List->push_back("    /* Ports */");
        for(std::unique_ptr<class Port>& Port:Proc->Port)
        {
            List->push_back(std::string("    RVM_ASSERT(RVM_Captbl_Add(")+
                            Proc->Captbl->Macro_Global+", "+
                            std::to_string(Port->Capid_Local)+"U, RVM_BOOT_CTINV_"+
                            CTIDS(Port->Capid_Global)+", "+OIDS(Port->Capid_Global)+"U, RVM_INV_FLAG_ACT)==0U);");
            List->push_back(std::string("    RVM_LOG_S(\"Init:Delegated invocation port '")+
                            Port->Macro_Global+"' to process '"+
                            Proc->Macro_Global+"' capability table position "+
                            std::to_string(Port->Capid_Local)+".\\r\\n\");");
        }
        /* Receive endpoints */
        List->push_back("    /* Receive endpoints */");
        for(const std::unique_ptr<class Receive>& Recv:Proc->Receive)
        {
            List->push_back(std::string("    RVM_ASSERT(RVM_Captbl_Add(")+
                            Proc->Captbl->Macro_Global+", "+
                            std::to_string(Recv->Capid_Local)+"U, RVM_BOOT_CTRECV_"+
                            CTIDS(Recv->Capid_Global)+", "+OIDS(Recv->Capid_Global)+"U, RVM_SIG_FLAG_SND|RVM_SIG_FLAG_RCV)==0U);");
            List->push_back(std::string("    RVM_LOG_S(\"Init:Delegated receive endpoint '")+
                            Recv->Macro_Global+"' to process '"+
                            Proc->Macro_Global+"' capability table position "+
                            std::to_string(Recv->Capid_Global)+".\\r\\n\");");
        }
        /* Send endpoints */
        List->push_back("    /* Send endpoints */");
        for(const std::unique_ptr<class Send>& Send:Proc->Send)
        {
            List->push_back(std::string("    RVM_ASSERT(RVM_Captbl_Add(")+
                            Proc->Captbl->Macro_Global+", "+
                            std::to_string(Send->Capid_Local)+"U, RVM_BOOT_CTRECV_"+
                            CTIDS(Send->Capid_Global)+", "+OIDS(Send->Capid_Global)+"U, RVM_SIG_FLAG_SND)==0U);");
            List->push_back(std::string("    RVM_LOG_S(\"Init:Delegated send endpoint '")+
                            Send->Macro_Global+"' to process '"+Proc->Macro_Global+"' capability table position "+
                            std::to_string(Send->Capid_Local)+".\\r\\n\");");
        }
        /* Vector endpoints */
        List->push_back("    /* Vector endpoints */");
        for(const std::unique_ptr<class Vect_Info>& Vect:Proc->Vector)
        {
            List->push_back(std::string("    RVM_ASSERT(RVM_Captbl_Add(")+
                            Proc->Captbl->Macro_Global+", "+
                            std::to_string(Vect->Capid_Local)+"U, RVM_BOOT_CTVECT_"+
                            CTIDS(Vect->Capid_Global)+", "+OIDS(Vect->Capid_Global)+"U, RVM_SIG_FLAG_RCV)==0U);");
            List->push_back(std::string("    RVM_LOG_S(\"Init:Delegated vector endpoint '")+
                            Vect->Macro_Global+"' to process '"+
                            Proc->Macro_Global+"' capability table position "+
                            std::to_string(Vect->Capid_Global)+".\\r\\n\");");
        }
        /* Kernel functions */
        List->push_back("    /* Kernel functions */");
        for(const std::unique_ptr<class Kfunc>& Kfunc:Proc->Kfunc)
        {
            List->push_back(std::string("    RVM_ASSERT(RVM_Captbl_Kern(")+
                            Proc->Captbl->Macro_Global+", "+
                            std::to_string(Kfunc->Capid_Local)+"U, RVM_BOOT_CAPTBL, RVM_BOOT_INIT_KERN, 0x"+
                            Main::Hex(Kfunc->Start)+"U, 0x"+Main::Hex(Kfunc->End)+"U)==0U);");
            List->push_back(std::string("    RVM_LOG_S(\"Init:Delegated kernel function 0x")+
                            Main::Hex(Kfunc->Start)+" - 0x"+Main::Hex(Kfunc->End)+" to process '"+
                            Proc->Macro_Global+"' capability table position "+std::to_string(Kfunc->Capid_Local)+".\\r\\n\");");
        }
        List->push_back("");
    }
    List->push_back("}");
    Gen_Tool::Func_Foot(List, "RVM_Boot_Captbl_Init");
    List->push_back("");

    /* Page table initialization */
    Main::Info("> Generating page table initialization code.");
    Gen_Tool::Func_Head(List, "RVM_Boot_Pgtbl_Init",
                        "Initialize the page tables of all processes.", Input, Output, "None.");
    List->push_back("void RVM_Boot_Pgtbl_Init(void)");
    List->push_back("{");
    List->push_back("    RVM_LOG_S(\"-------------------------------------------------------------------------------\\r\\n\");");
    List->push_back("    RVM_LOG_S(\"Init:Initializing process page tables.\\r\\n\");");
    /* Do page table construction first */
    for(const std::unique_ptr<class Process>& Proc:this->Plat->Proj->Process)
    {
        List->push_back(std::string("    /* Constructing page tables for process: '")+Proc->Name+"' */");
        this->Monitor_Pgtbl_Con(List, Proc->Pgtbl.get());
        List->push_back("");
    }
    /* Then do the mapping for all page tables */
    for(const class Pgtbl* Pgtbl:this->Plat->Proj->Monitor->Pgtbl)
    {
        List->push_back(std::string("    /* Mapping pages into page tables - '")+Pgtbl->Macro_Global+"' */");
        this->Monitor_Pgtbl_Map(List, Pgtbl, this->Plat->Plat->Wordlength);
        List->push_back("");
    }
    List->push_back("}");
    Gen_Tool::Func_Foot(List, "RVM_Boot_Pgtbl_Init");
    List->push_back("");

    /* Thread initialization */
    Main::Info("> Generating thread initialization code.");
    Gen_Tool::Func_Head(List, "RVM_Boot_Thd_Init",
                        "Initialize the all threads.", Input, Output, "None.");
    List->push_back("void RVM_Boot_Thd_Init(void)");
    List->push_back("{");
    List->push_back("    rvm_ptr_t Init_Stack_Addr;");
    List->push_back("");
    List->push_back("    Init_Stack_Addr=0U;");
    List->push_back("    RVM_LOG_S(\"-------------------------------------------------------------------------------\\r\\n\");");
    List->push_back("    RVM_LOG_S(\"Init:Initializing threads.\\r\\n\");");
    for(const std::unique_ptr<class Process>& Proc:this->Plat->Proj->Process)
    {
        List->push_back(std::string("    /* Initializing thread for process: '")+Proc->Name+"' */");
        for(const std::unique_ptr<class Thread>& Thd:Proc->Thread)
        {
            /* The marker is non-zero only for virtual machine threads */
            if(Proc->Type==PROC_VIRTUAL)
            {
                List->push_back(std::string("    RVM_ASSERT(RVM_Thd_Sched_Bind(")+
                                Thd->Macro_Global+", RVM_Sftd_Thd_Cap, RVM_Sftd_Sig_Cap, "+
                                Thd->Macro_Global+"|RVM_VIRT_THDID_MARKER, "+
                                std::to_string(Thd->Priority)+"U)==0U);");
            }
            else
            {
                List->push_back(std::string("    RVM_ASSERT(RVM_Thd_Sched_Bind(")+
                                Thd->Macro_Global+", RVM_Sftd_Thd_Cap, RVM_Sftd_Sig_Cap, "+
                                Thd->Macro_Global+", "+
                                std::to_string(Thd->Priority)+"U)==0U);");
            }
            /* If this process is a virtual machine and we are dealing with its user thread, we need to have its HYP set */
            if(Proc->Type==PROC_VIRTUAL)
            {
                if(Thd->Name=="User")
                {
                    List->push_back(std::string("    RVM_ASSERT(RVM_Thd_Hyp_Set(")+
                                    Thd->Macro_Global+", 0x"+
                                    Main::Hex(static_cast<class Virtual*>(Proc.get())->Reg_Base)+"U)==0U);");
                }
            }
            List->push_back(std::string("    Init_Stack_Addr=RVM_Stack_Init(0x")+
                            Main::Hex(Thd->Stack_Base)+"U, 0x"+
                            Main::Hex(Thd->Stack_Size)+"U, 0x"+
                            Main::Hex(Thd->Entry_Addr)+"U, 0x"+
                            Main::Hex(Proc->Entry_Code_Front)+"U);");
            List->push_back(std::string("    RVM_ASSERT(RVM_Thd_Exec_Set(")+
                            Thd->Macro_Global+", 0x"+
                            Main::Hex(Thd->Entry_Addr)+"U, Init_Stack_Addr, 0x"+
                            Main::Hex(Thd->Parameter)+"U)==0U);");
            List->push_back(std::string("    RVM_ASSERT(RVM_Thd_Time_Xfer(")+
                            Thd->Macro_Global+", RVM_BOOT_INIT_THD, RVM_THD_INF_TIME)==RVM_THD_INF_TIME);");
            List->push_back(std::string("    RVM_LOG_S(\"Init:Initialized thread '")+
                            Thd->Macro_Global+"' process '"+
                            Thd->Macro_Global+"' entry 0x"+
                            Main::Hex(Thd->Entry_Addr)+" stack 0x"+
                            Main::Hex(Thd->Stack_Base)+" - 0x"+
                            Main::Hex(Thd->Stack_Size)+" param 0x"+
                            Main::Hex(Thd->Parameter)+".\\r\\n\");");
            List->push_back("");
        }
    }
    List->push_back("}");
    Gen_Tool::Func_Foot(List, "RVM_Boot_Thd_Init");
    List->push_back("");

    /* Invocation initialization */
    Main::Info("> Generating invocation initialization code.");
    Gen_Tool::Func_Head(List, "RVM_Boot_Inv_Init",
                        "Initialize the all invocations.", Input, Output, "None.");
    List->push_back("void RVM_Boot_Inv_Init(void)");
    List->push_back("{");
    List->push_back("    rvm_ptr_t Init_Stack_Addr;");
    List->push_back("");
    List->push_back("    Init_Stack_Addr=0U;");
    List->push_back("    RVM_LOG_S(\"-------------------------------------------------------------------------------\\r\\n\");");
    List->push_back("    RVM_LOG_S(\"Init:Initializing invocations.\\r\\n\");");
    for(const std::unique_ptr<class Process>& Proc:this->Plat->Proj->Process)
    {
        List->push_back(std::string("    /* Initializing invocation for process: '")+Proc->Name+"' */");
        for(const std::unique_ptr<class Invocation>& Inv:Proc->Invocation)
        {
            List->push_back(std::string("    Init_Stack_Addr=RVM_Stack_Init(0x")+
                            Main::Hex(Inv->Stack_Base)+"U, 0x"+
                            Main::Hex(Inv->Stack_Size)+"U, 0x"+
                            Main::Hex(Inv->Entry_Addr)+"U, 0x"+
                            Main::Hex(Proc->Entry_Code_Front)+"U);");
            /* We always return directly on fault for MCUs, because RVM does not do fault handling there */
            List->push_back(std::string("    RVM_ASSERT(RVM_Inv_Set(")+
                            Inv->Macro_Global+", 0x"+
                            Main::Hex(Inv->Entry_Addr)+"U, Init_Stack_Addr, 1U)==0U);");
            List->push_back(std::string("    RVM_LOG_S(\"Init:Initialized invocation '")+
                            Inv->Macro_Global+"' process '"+
                            Proc->Macro_Global+"' entry 0x"+
                            Main::Hex(Inv->Entry_Addr)+" stack 0x"+
                            Main::Hex(Inv->Stack_Base)+" - 0x"+
                            Main::Hex(Inv->Stack_Size)+".\\r\\n\");");
            List->push_back("");
        }
    }
    List->push_back("}");
    Gen_Tool::Func_Foot(List, "RVM_Boot_Inv_Init");
    List->push_back("");

    /* Master kernel object initialization function */
    Main::Info("> Generating master kernel object initialization code.");
    Gen_Tool::Func_Head(List, "RVM_Boot_Kobj_Init",
                        "Initialize all kernel objects at boot-time.", Input, Output, "None.");
    List->push_back("void RVM_Boot_Kobj_Init(void)");
    List->push_back("{");
    if(this->Plat->Proj->Virtual.size()!=0)
        List->push_back("    RVM_Boot_Vcap_Init();");
    List->push_back("    RVM_Boot_Captbl_Init();");
    List->push_back("    RVM_Boot_Pgtbl_Init();");
    List->push_back("    RVM_Boot_Thd_Init();");
    List->push_back("    RVM_Boot_Inv_Init();");
    if(this->Plat->Proj->Virtual.size()!=0)
    {
        List->push_back(std::string("    RVM_Virt_Crt(RVM_Virt, RVM_Vmap, ")+
                        std::to_string(this->Plat->Proj->Virtual.size())+"U);");
    }
    List->push_back("}");
    Gen_Tool::Func_Foot(List, "RVM_Boot_Kobj_Init");
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

/* Begin Function:Gen_Tool::Process_Main_Hdr **********************************
Description : Create the main header for process.
Input       : const class Process* Proc - The process to generate for.
Output      : None.
Return      : None.
******************************************************************************/
void Gen_Tool::Process_Main_Hdr(const class Process* Proc)
{

}
/* End Function:Gen_Tool::Process_Main_Hdr ***********************************/

/* Begin Function:Gen_Tool::Virtual_Conf_Hdr **********************************
Description : Create the configuration header for VMs.
Input       : const class Virtual* Virt - The process to generate for.
Output      : None.
Return      : None.
******************************************************************************/
void Gen_Tool::Virtual_Conf_Hdr(const class Virtual* Virt)
{

}
/* End Function:Gen_Tool::Virtual_Conf_Hdr ***********************************/

/* Begin Function:Gen_Tool::Process_Main_Src **********************************
Description : Create the main source for process.
Input       : const class Process* Proc - The process to generate for.
Output      : None.
Return      : None.
******************************************************************************/
void Gen_Tool::Process_Main_Src(const class Process* Proc)
{

}
/* End Function:Gen_Tool::Process_Main_Src ***********************************/

/* Begin Function:Gen_Tool::Process_Linker ************************************
Description : Create the linker script file for process.
Input       : const class Process* Proc - The process to generate for.
Output      : None.
Return      : None.
******************************************************************************/
void Gen_Tool::Process_Linker(const class Process* Proc)
{

}
/* End Function:Gen_Tool::Process_Linker *************************************/

/* Begin Function:Gen_Tool::Process_Proj **************************************
Description : Create the project file for process.
Input       : const class Process* Proc - The process to generate for.
Output      : None.
Return      : None.
******************************************************************************/
void Gen_Tool::Process_Proj(const class Process* Proc)
{

}
/* End Function:Gen_Tool::Process_Proj ***************************************/

/* Begin Function:Gen_Tool::Workspace_Proj ************************************
Description : Create the workspace file for all projects.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
void Gen_Tool::Workspace_Proj(void)
{

}
/* End Function:Gen_Tool::Workspace_Proj *************************************/
}
/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
