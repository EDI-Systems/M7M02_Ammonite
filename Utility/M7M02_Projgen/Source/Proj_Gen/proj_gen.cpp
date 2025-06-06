/******************************************************************************
Filename    : gen_tool.cpp
Author      : pry
Date        : 16/07/2019
Licence     : LGPL v3+; see COPYING for details.
Description : The memory block class. This contains the memory block information.
******************************************************************************/

/* Include *******************************************************************/
extern "C"
{
#include "xml.h"
#include "string.h"
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
#include "Conf_Info/conf_info.hpp"
#include "Mem_Info/mem_info.hpp"
#include "Proj_Gen/proj_gen.hpp"
#include "Proj_Info/Process/process.hpp"
#include "Proj_Gen/Plat_Gen/plat_gen.hpp"
#include "Proj_Gen/Build_Gen/build_gen.hpp"
#include "Proj_Gen/Tool_Gen/tool_gen.hpp"
#include "Proj_Gen/Guest_Gen/guest_gen.hpp"
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
#include "Proj_Info/Process/Native/native.hpp"
#include "Proj_Info/Process/Virtual/virtual.hpp"
#include "Mem_Info/mem_info.hpp"
#include "Vect_Info/vect_info.hpp"
#include "Conf_Info/conf_info.hpp"

#include "Proj_Gen/proj_gen.hpp"

#include "Proj_Gen/Plat_Gen/plat_gen.hpp"
#include "Proj_Gen/Plat_Gen/A6M_Gen/a6m_gen.hpp"
#include "Proj_Gen/Plat_Gen/A7M_Gen/a7m_gen.hpp"
#include "Proj_Gen/Plat_Gen/RV32P_Gen/rv32p_gen.hpp"

#include "Proj_Gen/Build_Gen/build_gen.hpp"
#include "Proj_Gen/Build_Gen/Keil_Gen/keil_gen.hpp"
#include "Proj_Gen/Build_Gen/Makefile_Gen/makefile_gen.hpp"

#include "Proj_Gen/Tool_Gen/tool_gen.hpp"
#include "Proj_Gen/Tool_Gen/ARMCC_Gen/armcc_gen.hpp"
#include "Proj_Gen/Tool_Gen/GCC_Gen/gcc_gen.hpp"

#include "Proj_Gen/Guest_Gen/guest_gen.hpp"
#include "Proj_Gen/Guest_Gen/RMP_Gen/rmp_gen.hpp"
#undef __HDR_CLASS__
/* End Include ***************************************************************/
namespace RVM_GEN
{
/* Function:Proj_Gen::Proj_Gen ************************************************
Description : Constructor for generation tool class.
Input       : const std::string& Name - The name of the platform.
Input       : class Proj_Info* Proj - The project information.
              class Plat_Info* Plat - The platform information.
              class Chip_Info* Chip - The chip information.
Output      : None.
Return      : None.
******************************************************************************/
/* void */ Proj_Gen::Proj_Gen(const std::string& Name,
                              class Proj_Info* Proj, class Plat_Info* Plat, class Chip_Info* Chip)
{
    try
    {
        if(Name=="A6M")
            this->Plat=std::make_unique<class A6M_Gen>(Proj,Plat,Chip);
        else if(Name=="A7M")
        	this->Plat=std::make_unique<class A7M_Gen>(Proj,Plat,Chip);
        else if(Name=="RV32P")
            this->Plat=std::make_unique<class RV32P_Gen>(Proj,Plat,Chip);
        else
            Main::Error("XXXXX: Platform generator for '"+Name+"' is not found.");
    }
    catch(std::exception& Exc)
    {
        Main::Error(std::string("Tool:\n")+Exc.what());
    }
}
/* End Function:Proj_Gen::Proj_Gen *******************************************/

/* Function:Proj_Gen::Build_Load ***********************************************
Description : Load one of the buildsystems.
Input       : const std::string& Name - The name of the buildsystem.
Output      : None.
Return      : None.
******************************************************************************/
void  Proj_Gen::Build_Load(const std::string& Name)
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
/* End Function:Proj_Gen::Build_Load *****************************************/

/* Function:Proj_Gen::Tool_Load ***********************************************
Description : Load one of the build tools.
Input       : const std::string& Name - The name of the toolchain.
Output      : None.
Return      : None.
******************************************************************************/
void Proj_Gen::Tool_Load(const std::string& Name)
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
/* End Function:Proj_Gen::Tool_Load ******************************************/

/* Function:Proj_Gen::Guest_Load **********************************************
Description : Load one of the guest OSes.
Input       : const std::string& Name - The name of the guest OS.
Output      : None.
Return      : None.
******************************************************************************/
void  Proj_Gen::Guest_Load(const std::string& Name)
{
    std::unique_ptr<class Guest_Gen> Guest;

    try
    {
        if(this->Guest_Map.find(Name)!=this->Guest_Map.end())
            return;

        if(Name=="RMP")
        {
            /* Check guest OS root path, it must be filled in to load the corresponding tool */
            if(Main::Guest_RMP_Root=="")
                Main::Error("XXXXX: Guest path for '"+Name+"' is missing.");
            Guest=std::make_unique<class RMP_Gen>(this->Plat->Proj,this->Plat->Plat,this->Plat->Chip);
        }
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
/* End Function:Proj_Gen::Guest_Load *****************************************/

/* Function:Proj_Gen::Line_Read ***********************************************
Description : Read a source file to line-based buffer for further processing.
Input       : const std::string& Path - The path to the file.
Output      : None.
Return      : std::unique_ptr<std::vector<std::string>> - The file returned by line.
******************************************************************************/
std::unique_ptr<std::vector<std::string>> Proj_Gen::Line_Read(const std::string& Path)
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
/* End Function:Proj_Gen::Line_Read *******************************************/

/* Function:Proj_Gen::Line_Write ***********************************************
Description : Write a line-based buffer to destination file.
Input       : std::unique_ptr<std::vector<std::string>>& List - The file contents.
              const std::string& Path - The path to the file.
Output      : None.
Return      : None.
******************************************************************************/
void Proj_Gen::Line_Write(std::unique_ptr<std::vector<std::string>>& List, const std::string& Path)
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
/* End Function:Proj_Gen::Line_Write *****************************************/

/* Function:Proj_Gen::Macro_String ********************************************
Description : See if a macro is defined in the file, and modify it as a string.
Input       : std::unique_ptr<std::vector<std::string>>& List - The file contents.
              const std::string& Macro - The macro name.
              const std::string& Value - The new name to assign to that macro.
              ptr_t Mode - The operation mode, replace or add.
Output      : std::unique_ptr<std::vector<std::string>>& List - The updated file.
Return      : None.
******************************************************************************/
void Proj_Gen::Macro_String(std::unique_ptr<std::vector<std::string>>& List,
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
/* End Function:Proj_Gen::Macro_String ***************************************/

/* Function:Proj_Gen::Macro_Hex ***********************************************
Description : See if a macro is defined in the file, and modify it as a hex number.
Input       : std::unique_ptr<std::vector<std::string>>& List - The file contents.
              const std::string& Macro - The macro name.
              ptr_t Value - The new value to assign to that macro.
              ptr_t Mode - The operation mode, replace or add.
Output      : std::unique_ptr<std::vector<std::string>>& List - The updated file.
Return      : None.
******************************************************************************/
void Proj_Gen::Macro_Hex(std::unique_ptr<std::vector<std::string>>& List,
                         const std::string& Macro, ptr_t Value, ptr_t Mode)
{
    char Buf[64];

    sprintf(Buf,"0x%llXU",Value);
    Proj_Gen::Macro_String(List,Macro,Buf,Mode);
}
/* End Function:Proj_Gen::Macro_Hex ******************************************/

/* Function:Proj_Gen::Macro_Int ***********************************************
Description : See if a macro is defined in the file, and modify it as a dec number.
Input       : std::unique_ptr<std::vector<std::string>>& List - The file contents.
              const std::string& Macro - The macro name.
              ptr_t Value - The new value to assign to that macro.
              ptr_t Mode - The operation mode, replace or add.
Output      : std::unique_ptr<std::vector<std::string>>& List - The updated file.
Return      : None.
******************************************************************************/
void Proj_Gen::Macro_Int(std::unique_ptr<std::vector<std::string>>& List,
                         const std::string& Macro, ptr_t Value, ptr_t Mode)
{
    char Buf[64];

    sprintf(Buf,"%lldU",Value);
    Proj_Gen::Macro_String(List,Macro,Buf,Mode);
}
/* End Function:Proj_Gen::Macro_Int ******************************************/

/* Function:Proj_Gen::Src_Head ************************************************
Description : Emit source descriptor into a source document.
Input       : std::unique_ptr<std::vector<std::string>>& List - The input file.
              const std::string& Name - The filename.
              const std::string& Desc - The file description.
Output      : std::unique_ptr<std::vector<std::string>>& List - The appended file.
Return      : None.
******************************************************************************/
void Proj_Gen::Src_Head(std::unique_ptr<std::vector<std::string>>& List,
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
/* End Function:Proj_Gen::Src_Head *******************************************/

/* Function:Proj_Gen::Src_Foot ************************************************
Description : Emit source footer into a source document.
Input       : std::unique_ptr<std::vector<std::string>>& List - The input file.
Output      : std::unique_ptr<std::vector<std::string>>& List - The appended file.
Return      : None.
******************************************************************************/
void Proj_Gen::Src_Foot(std::unique_ptr<std::vector<std::string>>& List)
{
    List->push_back("/* End Of File ***************************************************************/");
    List->push_back("");
    List->push_back("/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/");
    List->push_back("");
}
/* End Function:Proj_Gen::Src_Foot *******************************************/

/* Function:Proj_Gen::Func_Head ***********************************************
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
void Proj_Gen::Func_Head(std::unique_ptr<std::vector<std::string>>& List,
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
/* End Function:Proj_Gen::Func_Head ******************************************/

/* Function:Proj_Gen::Func_Foot ***********************************************
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
void Proj_Gen::Func_Foot(std::unique_ptr<std::vector<std::string>>& List, const std::string& Name)
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
/* End Function:Proj_Gen::Func_Foot ******************************************/

/* Function:Proj_Gen::Path_Conv ***********************************************
Description : Deduplicate all paths, and make them relative to the root path.
Input       : const std::string& Root - The root path.
              std::vector<std::string>& List - The path list.
Output      : std::vector<std::string>& List - The modified path list.
Return      : None.
******************************************************************************/
void Proj_Gen::Path_Conv(const std::string& Root, std::vector<std::string>& List)
{
    std::set<std::string> Result;

    /* Convert then dump redundant files */
    for(const std::string& Path:List)
    {
        if((Path.back()=='/')||(Path.back()=='\\'))
            Result.insert(Main::Path_Relative(PATH_DIR, Root, Path));
        else
            Result.insert(Main::Path_Relative(PATH_FILE, Root, Path));
    }

    /* Substitute path specifiers */
    List.clear();
    for(const std::string& Path:Result)
    {
        List.push_back(Path);
    }

    /* Sort the list from small to large for better view */
    std::sort(List.begin(),List.end(),
              [](const std::string& Left, const std::string& Right)->bool
              {
                  return Left>Right;
              });
}
/* End Function:Proj_Gen::Path_Conv ******************************************/

/* Function:Proj_Gen::Macro_Mem ***********************************************
Description : Print memory referernce for config headers.
Input       : std::unique_ptr<std::vector<std::string>>& List - The file.
              const std::string& Prefix - The prefix to append to the front.
              const class Mem_Info* Mem - The memory to generate for.
Output      : std::unique_ptr<std::vector<std::string>>& List - The appended file.
Return      : None.
******************************************************************************/
void Proj_Gen::Macro_Mem(const std::string& Prefix,
                         std::unique_ptr<std::vector<std::string>>& List,
                         const class Mem_Info* Mem)
{
    std::string Macro;

    /* If the memory does not have a name, we don't generate macros for them */
    if(Mem->Name=="")
        return;

    switch(Mem->Type)
    {
        case MEM_CODE: Macro=Prefix+"CODE_"; break;
        case MEM_DATA: Macro=Prefix+"DATA_"; break;
        case MEM_DEVICE: Macro=Prefix+"DEVICE_"; break;
        default:ASSERT(0);
    }

    if(Mem->Is_Shared!=0)
        Macro+="SHARED_";

    Macro+=Mem->Name_Upper;

    Proj_Gen::Macro_Hex(List, Macro+"_BASE", Mem->Base, MACRO_REPLACE);
    Proj_Gen::Macro_Hex(List, Macro+"_SIZE", Mem->Size, MACRO_REPLACE);
}
/* End Function:Proj_Gen::Macro_Mem ******************************************/

/* Function:Proj_Gen::Boot_Hdr_Mem ********************************************
Description : Print memory reference for kernel config header.
Input       : const std::string& Prefix;
              std::unique_ptr<std::vector<std::string>>& List - The file.
Output      : std::unique_ptr<std::vector<std::string>>& List - The appended file.
Return      : None.
******************************************************************************/
void Proj_Gen::Boot_Hdr_Mem(const std::string& Prefix,
                            std::unique_ptr<std::vector<std::string>>& List)
{
    List->push_back("/* Code memory blocks */");
    for(const std::unique_ptr<class Process>& Prc:this->Plat->Proj->Process)
    {
        for(const class Mem_Info* Mem: Prc->Memory_Code)
        {
            if(Mem->Is_Shared==0)
                Proj_Gen::Macro_Mem(Prefix+Prc->Name_Upper+"_", List, Mem);
        }
    }
    for(const std::unique_ptr<class Process>& Prc:this->Plat->Proj->Process)
    {
        for(const class Mem_Info* Mem: Prc->Memory_Code)
        {
            if(Mem->Is_Shared!=0)
                Proj_Gen::Macro_Mem(Prefix, List, Mem);
        }
    }

    List->push_back("");
    List->push_back("/* Data memory blocks */");
    for(const std::unique_ptr<class Process>& Prc:this->Plat->Proj->Process)
    {
        for(const class Mem_Info* Mem: Prc->Memory_Data)
        {
            if(Mem->Is_Shared==0)
                Proj_Gen::Macro_Mem(Prefix+Prc->Name_Upper+"_", List, Mem);
        }
    }
    for(const std::unique_ptr<class Process>& Prc:this->Plat->Proj->Process)
    {
        for(const class Mem_Info* Mem: Prc->Memory_Data)
        {
            if(Mem->Is_Shared!=0)
                Proj_Gen::Macro_Mem(Prefix, List, Mem);
        }
    }

    List->push_back("");
    List->push_back("/* Device memory blocks */");
    for(const std::unique_ptr<class Process>& Prc:this->Plat->Proj->Process)
    {
        for(const class Mem_Info* Mem: Prc->Memory_Device)
        {
            if(Mem->Is_Shared==0)
                Proj_Gen::Macro_Mem(Prefix+Prc->Name_Upper+"_", List, Mem);
        }
    }
    for(const std::unique_ptr<class Process>& Prc:this->Plat->Proj->Process)
    {
        for(const class Mem_Info* Mem: Prc->Memory_Device)
        {
            if(Mem->Is_Shared!=0)
                Proj_Gen::Macro_Mem(Prefix, List, Mem);
        }
    }
}
/* End Function:Proj_Gen::Boot_Hdr_Mem ***************************************/

/* Function:Proj_Gen::Kernel_Inc **********************************************
Description : Write the include files for kernel.
Input       : std::unique_ptr<std::vector<std::string>>& List - The input file.
Output      : std::unique_ptr<std::vector<std::string>>& List - The appended file.
Return      : None.
******************************************************************************/
void Proj_Gen::Kernel_Inc(std::unique_ptr<std::vector<std::string>>& List)
{
    std::string Temp;

    List->push_back("/* Include *******************************************************************/");
    Temp=std::string("#include \"Platform/")+this->Plat->Name+"/rme_platform_"+this->Plat->Name_Lower+".h\"";
    List->push_back("#define __HDR_DEF__");
    List->push_back(Temp);
    List->push_back("#include \"Kernel/rme_kernel.h\"");
    List->push_back("#undef __HDR_DEF__");
    List->push_back("");
    List->push_back("#define __HDR_STRUCT__");
    List->push_back(Temp);
    List->push_back("#include \"Kernel/rme_kernel.h\"");
    List->push_back("#undef __HDR_STRUCT__");
    List->push_back("");
    List->push_back("#define __HDR_PUBLIC__");
    List->push_back(Temp);
    List->push_back("#include \"Kernel/rme_kernel.h\"");
    List->push_back("#undef __HDR_PUBLIC__");
    List->push_back("");
    List->push_back("#include \"rme_boot.h\"");
    List->push_back("/* End Include ***************************************************************/");
}
/* End Function:Proj_Gen::Kernel_Inc *****************************************/

/* Function:Proj_Gen::Kernel_Conf_Hdr *****************************************
Description : Create the platform configuration headers for kernel.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
void Proj_Gen::Kernel_Conf_Hdr(void)
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
    Proj_Gen::Src_Head(List, "rme_platform.h", "The platform selection header.");
    List->push_back("");
    List->push_back("/* Platform Include **********************************************************/");
    List->push_back(std::string("#include \"Platform/")+this->Plat->Name+"/rme_platform_"+this->Plat->Name_Lower+".h\"");
    List->push_back("/* End Platform Include ******************************************************/");
    List->push_back("");
    Proj_Gen::Src_Foot(List);
    Proj_Gen::Line_Write(List, Kernel->Config_Header_Output+"/rme_platform.h");

    /* Generate rme_platform_xxx_conf.h */
    Filename=std::string("rme_platform_")+this->Plat->Name_Lower+"_conf.h";
    Main::Info(std::string("> Generating '")+Filename+"'.");
    List->clear();
    Proj_Gen::Src_Head(List, Filename, "The chip selection header.");
    List->push_back("");
    List->push_back("/* Platform Include **********************************************************/");
    List->push_back(std::string("#include \"rme_platform_")+this->Plat->Chip->Name_Lower+".h\"");
    List->push_back("/* End Platform Include ******************************************************/");
    List->push_back("");
    Proj_Gen::Src_Foot(List);
    Proj_Gen::Line_Write(List, Kernel->Config_Header_Output+"rme_platform_"+this->Plat->Name_Lower+"_conf.h");

    /* Generate rme_platform_chipname.h */
    Filename=std::string("rme_platform_")+this->Plat->Chip->Name_Lower+".h";
    Main::Info(std::string("> Generating '")+Filename+"'.");
    List=Proj_Gen::Line_Read(Main::Kernel_Root+
                             "Include/Platform/"+this->Plat->Name+"/Chip/"+this->Plat->Chip->Name+"/"+Filename);
    /* Replace general parameter macros */
    /* Debugging control */
    Proj_Gen::Macro_Int(List, "RME_ASSERT_ENABLE", this->Plat->Proj->Assert_Enable, MACRO_REPLACE);
    Proj_Gen::Macro_Int(List, "RME_DBGLOG_ENABLE", this->Plat->Proj->Debug_Log_Enable, MACRO_REPLACE);
    /* Generator enabled */
    Proj_Gen::Macro_Int(List, "RME_RVM_GEN_ENABLE", 1, MACRO_REPLACE);
    /* If region is fixed, we use user-level raw page tables */
    Proj_Gen::Macro_Int(List, "RME_PGT_RAW_ENABLE", this->Plat->Proj->Pgtbl_Raw_Enable, MACRO_REPLACE);
    /* The virtual memory base/size for the kernel objects */
    Proj_Gen::Macro_Hex(List, "RME_KOM_VA_BASE", Kernel->Kom_Base, MACRO_REPLACE);
    Proj_Gen::Macro_Hex(List, "RME_KOM_VA_SIZE", Kernel->Kom_Size, MACRO_REPLACE);
    /* The virtual memory base/size for the virtual machines - always full memory range, just in case */
    Proj_Gen::Macro_Hex(List, "RME_HYP_VA_BASE", 0x00000000ULL, MACRO_REPLACE);
    Proj_Gen::Macro_Hex(List, "RME_HYP_VA_SIZE", 0xFFFFFFFFULL, MACRO_REPLACE);
    /* Kernel stack base/size */
    Proj_Gen::Macro_Hex(List, "RME_KSTK_VA_BASE", Kernel->Stack_Base, MACRO_REPLACE);
    Proj_Gen::Macro_Hex(List, "RME_KSTK_VA_SIZE", Kernel->Stack_Size, MACRO_REPLACE);
    /* The maximum number of preemption priority levels in the system.
     * This parameter must be divisible by the word length - 32 is usually sufficient */
    Proj_Gen::Macro_Int(List, "RME_PREEMPT_PRIO_NUM", Kernel->Kern_Prio, MACRO_REPLACE);
    /* The granularity of kernel memory allocation, in bytes */
    Proj_Gen::Macro_Int(List, "RME_KOM_SLOT_ORDER", Kernel->Kom_Order, MACRO_REPLACE);
    /* Physical vector number, flag area base and its size */
    Proj_Gen::Macro_Int(List, "RME_RVM_PHYS_VCT_NUM", this->Plat->Chip->Vector, MACRO_REPLACE);
    Proj_Gen::Macro_Hex(List, "RME_RVM_PHYS_VCTF_BASE", Kernel->Vctf_Base, MACRO_REPLACE);
    Proj_Gen::Macro_Hex(List, "RME_RVM_PHYS_VCTF_SIZE", Kernel->Vctf_Size, MACRO_REPLACE);
    /* Virtual event number, flag area base and its size */
    Proj_Gen::Macro_Int(List, "RME_RVM_VIRT_EVT_NUM", Monitor->Virt_Event, MACRO_REPLACE);
    Proj_Gen::Macro_Hex(List, "RME_RVM_VIRT_EVTF_BASE", Kernel->Evtf_Base, MACRO_REPLACE);
    Proj_Gen::Macro_Hex(List, "RME_RVM_VIRT_EVTF_SIZE", Kernel->Evtf_Size, MACRO_REPLACE);
    /* The initial capability table size */
    Proj_Gen::Macro_Int(List, "RME_RVM_INIT_CPT_SIZE", Monitor->Captbl_Size, MACRO_REPLACE);
    /* Initial kernel object frontier limit */
    Proj_Gen::Macro_Int(List, "RME_RVM_CPT_BOOT_FRONT", Monitor->Before_Cap_Front, MACRO_REPLACE);
    Proj_Gen::Macro_Hex(List, "RME_RVM_KOM_BOOT_FRONT", Monitor->Before_Kom_Front, MACRO_REPLACE);
    /* Post-boot kernel object frontier limit */
    Proj_Gen::Macro_Int(List, "RME_RVM_CPT_DONE_FRONT", Monitor->After_Cap_Front, MACRO_REPLACE);
    Proj_Gen::Macro_Hex(List, "RME_RVM_KOM_DONE_FRONT", Monitor->After_Kom_Front, MACRO_REPLACE);

    /* Set coprocessor macros - set all of them to 0 first, then modify from there */
    Proj_Gen::Macro_Int(List, "RME_COP_NUM", this->Plat->Proj->Chip->Coprocessor.size(), MACRO_REPLACE);
    for(const std::string& Cop:this->Plat->Plat->Coprocessor)
        Proj_Gen::Macro_Int(List, "RME_"+this->Plat->Name_Upper+"_COP_"+Cop, 0, MACRO_REPLACE);
    for(const std::string& Cop:this->Plat->Proj->Chip->Coprocessor)
        Proj_Gen::Macro_Int(List, "RME_"+this->Plat->Name_Upper+"_COP_"+Cop, 1, MACRO_REPLACE);

    /* Replace platform specific macros */
    this->Plat->Kernel_Conf_Hdr(List);
    /* Chip specific macros - we must be able to find it because we checked before */
    for(std::unique_ptr<class Conf_Info>& Conf:this->Plat->Chip->Config)
    {
        if(Conf->Type==CONFIG_RANGE)
            Proj_Gen::Macro_String(List, Conf->Macro, this->Plat->Proj->Chip->Config[Conf->Name]+"U", MACRO_REPLACE);
        else
            Proj_Gen::Macro_String(List, Conf->Macro, this->Plat->Proj->Chip->Config[Conf->Name], MACRO_REPLACE);
    }
    /* Write to kernel configuration file */
    Proj_Gen::Line_Write(List, Kernel->Config_Header_Output+Filename);

    /* If we're running benchmark, need to generate the test header as well */
    if(Main::Benchmark!=0)
    {
        Main::Info("> Generating test selection header.");
        List->clear();
        Proj_Gen::Src_Head(List, "rme_test.h", "The RME/RVM guest library test header.");
        List->push_back("");
        List->push_back("/* Include *******************************************************************/");
        /* This header is in RVM, we'll add its folder to the kernel include as well */
        List->push_back(std::string("#include \"rvm_test_")+this->Plat->Chip->Name_Lower+".h\"");
        List->push_back("/* End Include ***************************************************************/");
        List->push_back("");
        Proj_Gen::Src_Foot(List);

        Proj_Gen::Line_Write(List, Kernel->Config_Header_Output+"rme_test.h");
    }
}
/* End Function:Proj_Gen::Kernel_Conf_Hdr ************************************/

/* Function:Proj_Gen::Kernel_Boot_Hdr *****************************************
Description : Create the boot header for kernel.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
void Proj_Gen::Kernel_Boot_Hdr(void)
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
    Proj_Gen::Src_Head(List, "rme_boot.h", "The boot-time initialization file header.");
    List->push_back("");

    Main::Info("> Generating defines.");
    List->push_back("/* Define ********************************************************************/");

    /* Capability table maximum size */
    List->push_back("/* Capability table maximum capacity */");
    Proj_Gen::Macro_Int(List, "RME_MAIN_CPT_SIZE", this->Plat->Plat->Captbl_Max, MACRO_ADD);
    List->push_back("/* Boot-time capability table */");
    Proj_Gen::Macro_String(List, "RME_BOOT_CPT_OBJ", std::string("RME_")+this->Plat->Name_Upper+"_CPT", MACRO_ADD);

    /* Vector capability tables */
    List->push_back("");
    List->push_back("/* Vector endpoint capability tables */");
    Cap_Front=Kernel->Vct_Cap_Front;
    for(Obj_Cnt=0;Obj_Cnt<Monitor->Vector.size();Obj_Cnt+=this->Plat->Plat->Captbl_Max)
    {
        Proj_Gen::Macro_Int(List, std::string("RME_MAIN_VCT_")+MIDS(Obj_Cnt), Cap_Front, MACRO_ADD);
        Cap_Front++;
    }

    /* Vector endpoints */
    List->push_back("");
    List->push_back("/* Vector endpoints */");
    Obj_Cnt=0;
    for(const class Vect_Info* Vct:Monitor->Vector)
    {
        Proj_Gen::Macro_Int(List, Vct->Macro_Global, SID(Obj_Cnt), MACRO_ADD);
        Obj_Cnt++;
    }

    /* Receive endpoint capability tables - created by RVM later */
    List->push_back("");
    List->push_back("/* Receive endpoint capability tables - created by RVM later */");
    Cap_Front=Monitor->Rcv_Cap_Front;
    for(Obj_Cnt=0;Obj_Cnt<Monitor->Receive.size();Obj_Cnt+=this->Plat->Plat->Captbl_Max)
    {
        Proj_Gen::Macro_Int(List, std::string("RME_BOOT_SIG_")+MIDS(Obj_Cnt), Cap_Front, MACRO_ADD);
        Cap_Front++;
    }
    
    /* Receive endpoints - created by RVM later */
    List->push_back("");
    List->push_back("/* Receive endpoints - created by RVM later */");
    Obj_Cnt=0;
    for(const class Receive* Rcv:Monitor->Receive)
    {
        Proj_Gen::Macro_String(List, std::string("RME")+Rcv->Macro_Global.substr(3),
                               std::string("RME_CID(RME_BOOT_SIG_")+MIDS(Obj_Cnt)+","+SIDS(Obj_Cnt)+"U)", MACRO_ADD);
        Obj_Cnt++;
    }

    /* All memory trunks ever defined in the project, including all projects */
    List->push_back("");
    this->Boot_Hdr_Mem("RME_", List);
    List->push_back("/* End Define ****************************************************************/");
    List->push_back("");

    Main::Info("> Writing file.");
    Proj_Gen::Src_Foot(List);
    Proj_Gen::Line_Write(List, Kernel->Boot_Header_Output+"rme_boot.h");
}
/* End Function:Proj_Gen::Kernel_Boot_Hdr ************************************/

/* Function:Proj_Gen::Kernel_Boot_Src *****************************************
Description : Create the boot source for kernel.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
void Proj_Gen::Kernel_Boot_Src(void)
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
    Proj_Gen::Src_Head(List, "rme_boot.c", "The boot-time initialization file.");
    List->push_back("");

    /* Print all header includes */
    Main::Info("> Generating C header includes.");
    this->Kernel_Inc(List);
    List->push_back("");

    /* Global variables */
    List->push_back("/* Private Variable **********************************************************/");
    for(const class Vect_Info* Vct:Monitor->Vector)
        List->push_back(std::string("static struct RME_Cap_Sig* ")+Vct->Name+"_Vct_Sig;");
    List->push_back("/* End Private Variable ******************************************************/");
    List->push_back("");

    /* Private prototypes */
    Main::Info("> Generating private C function prototypes.");
    List->push_back("/* Private Function **********************************************************/");
    for(const class Vect_Info* Vct:Monitor->Vector)
        List->push_back(std::string("RME_EXTERN rme_ptr_t RME_Vct_")+Vct->Name+"_Handler(struct RME_Reg_Struct* Reg);");
    List->push_back("RME_EXTERN rme_ptr_t RME_Spurious_Handler(struct RME_Reg_Struct* Reg,");
    List->push_back("                                          rme_ptr_t Vct_Num);");
    List->push_back("/* End Private Function ******************************************************/");
    List->push_back("");

    /* Public prototypes */
    Main::Info("> Generating public C function prototypes.");
    List->push_back("/* Public Function ***********************************************************/");
    List->push_back("rme_ptr_t RME_Boot_Vct_Init(struct RME_Cap_Cpt* Cpt,");
    List->push_back("                            rme_ptr_t Cap_Front,");
    List->push_back("                            rme_ptr_t Kom_Front);");
    List->push_back("rme_ptr_t RME_Boot_Vct_Handler(struct RME_Reg_Struct* Reg,");
    List->push_back("                               rme_ptr_t Vct_Num);");
    List->push_back("/* End Public Function *******************************************************/");
    List->push_back("");

    /* Boot-time setup routine for the interrupt endpoints */
    Main::Info("> Generating boot-time vector endpoint initialization routine.");
    Input.push_back("rme_ptr_t Cap_Front - The current capability table frontier.");
    Input.push_back("rme_ptr_t Kom_Front - The current kernel absolute memory frontier.");
    Proj_Gen::Func_Head(List, "RME_Boot_Vct_Init",
                        "Initialize all the vector endpoints at boot-time.",
                        Input, Output, "rme_ptr_t - The kernel memory frontier after booting is all done.");
    Input.clear();
    List->push_back("rme_ptr_t RME_Boot_Vct_Init(struct RME_Cap_Cpt* Cpt,");
    List->push_back("                            rme_ptr_t Cap_Front,");
    List->push_back("                            rme_ptr_t Kom_Front)");
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
        List->push_back(std::string("    RME_ASSERT(_RME_Cpt_Boot_Crt(Cpt, RME_BOOT_INIT_CPT, RME_MAIN_VCT_")+
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
    Proj_Gen::Func_Foot(List, "RME_Boot_Vct_Init");
    List->push_back("");

    /* Print the interrupt relaying function */
    Main::Info("> Generating interrupt relay facility.");
    Input.push_back("struct RME_Reg_Struct* Reg - The register set.");
    Input.push_back("rme_ptr_t Vct_Num - The vector number.");
    Proj_Gen::Func_Head(List, "RME_Boot_Vct_Handler",
                        "The interrupt handler entry for all the vectors.",
                        Input, Output, "rme_ptr_t - Whether to send to the generic vector endpoint.");
    Input.clear();
    List->push_back("rme_ptr_t RME_Boot_Vct_Handler(struct RME_Reg_Struct* Reg,");
    List->push_back("                               rme_ptr_t Vct_Num)");
    List->push_back("{");
    List->push_back("    rme_ptr_t Send;");
    List->push_back("    struct RME_Cap_Sig* Endp;");
    List->push_back("");
    List->push_back("    Send=0U;");
    List->push_back("");
    List->push_back("    switch(Vct_Num)");
    List->push_back("    {");
    /* Registered handlers */
    for(const class Vect_Info* Vct:Monitor->Vector)
    {
        List->push_back(std::string("        /* ")+Vct->Name+" */");
        List->push_back(std::string("        case ")+std::to_string(Vct->Number)+"U: ");
        List->push_back("        {");
        List->push_back(std::string("            Send=RME_Vct_")+Vct->Name+"_Handler(Reg);");
        List->push_back(std::string("            Endp=")+Vct->Name+"_Vct_Sig;");
        List->push_back("            break;");
        List->push_back("        }");
    }
    /* Spurious handler */
    List->push_back("        /* Spurious */");
    List->push_back("        default: ");
    List->push_back("        {");
    List->push_back("            Send=RME_Spurious_Handler(Reg,Vct_Num);");
    List->push_back("            Endp=RME_NULL;");
    List->push_back("            break;");
    List->push_back("        }");
    List->push_back("    }");
    List->push_back("");
    List->push_back("    if(Endp==RME_NULL)");
    List->push_back("    {");
    List->push_back("        if(Send==RME_RVM_VCT_SIG_NONE)");
    List->push_back("	         return 0U;");
    List->push_back("        else");
    List->push_back("	         return 1U;");
    List->push_back("    }");
    List->push_back("");
    List->push_back("    switch(Send)");
    List->push_back("    {");
    List->push_back("        case RME_RVM_VCT_SIG_ALL:");
    List->push_back("        {");
    List->push_back("            _RME_Kern_Snd(Endp,1U);");
    List->push_back("            return 1U;");
    List->push_back("        }");
    List->push_back("        case RME_RVM_VCT_SIG_SELF:");
    List->push_back("        {");
    List->push_back("            _RME_Kern_Snd(Endp,1U);");
    List->push_back("            _RME_Kern_High(Reg,RME_CPU_LOCAL());");
    List->push_back("            return 0U;");
    List->push_back("        }");
    List->push_back("        case RME_RVM_VCT_SIG_INIT: return 1U;");
    List->push_back("        default: break;");
    List->push_back("    }");
    List->push_back("");
    List->push_back("    return 0U;");
    List->push_back("}");
    Proj_Gen::Func_Foot(List, "RME_Boot_Vct_Handler");
    List->push_back("");

    /* Generate rme_boot.c */
    Main::Info("> Writing file.");
    Proj_Gen::Src_Foot(List);
    Proj_Gen::Line_Write(List, Kernel->Boot_Source_Output+"rme_boot.c");
}
/* End Function:Proj_Gen::Kernel_Boot_Src ************************************/

/* Function:Proj_Gen::Kernel_Hook_Src *****************************************
Description : Create the hook source file for kernel.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
void Proj_Gen::Kernel_Hook_Src(void)
{
    class Kernel* Kernel;
    std::vector<std::string> Input;
    std::vector<std::string> Output;
    std::unique_ptr<std::vector<std::string>> List;

    Kernel=this->Plat->Proj->Kernel.get();
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
    Proj_Gen::Src_Head(List, "rme_hook.c", "The user initialization hook file.");
    List->push_back("");

    /* Print all header includes */
    Main::Info("> Generating C header includes.");
    this->Kernel_Inc(List);
    List->push_back("");

    /* Print all global prototypes */
    Main::Info("> Generating public C function prototypes.");
    List->push_back("/* Public Function ***********************************************************/");
    List->push_back("void RME_Boot_Pre_Init(void);");
    List->push_back("void RME_Boot_Post_Init(void);");
    List->push_back("void RME_Reboot_Failsafe(void);");
    List->push_back("rme_ptr_t RME_Spurious_Handler(struct RME_Reg_Struct* Reg,");
    List->push_back("                               rme_ptr_t Vct_Num);");
    List->push_back("rme_ret_t RME_Hook_Kfn_Handler(rme_ptr_t Func_ID,");
    List->push_back("                               rme_ptr_t Sub_ID,");
    List->push_back("                               rme_ptr_t Param1,");
    List->push_back("                               rme_ptr_t Param2);");
    List->push_back("/* End Public Function *******************************************************/");
    List->push_back("");

    /* Preinitialization of hardware */
    Main::Info("> Generating boot-time pre-initialization stub.");
    Proj_Gen::Func_Head(List, "RME_Boot_Pre_Init",
                        "Initialize critical hardware before any initialization takes place.",
                        Input, Output, "None.");
    List->push_back("void RME_Boot_Pre_Init(void)");
    List->push_back("{");
    List->push_back("    /* Add code here */");
    List->push_back("}");
    Proj_Gen::Func_Foot(List, "RME_Boot_Pre_Init");
    List->push_back("");

    /* Postinitialization of hardware */
    Main::Info("> Generating boot-time post-initialization stub.");
    Proj_Gen::Func_Head(List, "RME_Boot_Post_Init",
                        "Initialize other hardware after all initialization took place.",
                        Input, Output, "None.");
    List->push_back("void RME_Boot_Post_Init(void)");
    List->push_back("{");
    List->push_back("    /* Add code here */");
    List->push_back("}");
    Proj_Gen::Func_Foot(List, "RME_Boot_Post_Init");
    List->push_back("");

    /* Rebooting */
    Main::Info("> Generating reboot fail-safe handler.");
    Proj_Gen::Func_Head(List, "RME_Reboot_Failsafe",
                        "User-modifiable pre-rebooting failsafe sequence.",
                        Input, Output, "None.");
    List->push_back("void RME_Reboot_Failsafe(void)");
    List->push_back("{");
    List->push_back("    /* Add code here */");
    List->push_back("}");
    Proj_Gen::Func_Foot(List, "RME_Reboot_Failsafe");
    List->push_back("");

    /* Spurious interrupt handling */
    Main::Info("> Generating spurious interrupt handler.");
    Input.push_back("struct RME_Reg_Struct* Reg - The register set.");
    Input.push_back("rme_ptr_t Vct_Num - The vector number.");
    Proj_Gen::Func_Head(List, "RME_Spurious_Handler",
                        "User-modifiable spurious interrupt handler.",
                        Input, Output, "rme_ptr_t - Decides what endpoints to send to (see manual).");
    List->push_back("rme_ptr_t RME_Spurious_Handler(struct RME_Reg_Struct* Reg,");
    List->push_back("                               rme_ptr_t Vct_Num)");
    List->push_back("{");
    List->push_back("    /* Add code here */");
    List->push_back("    return RME_RVM_VCT_SIG_NONE;");
    List->push_back("}");
    Proj_Gen::Func_Foot(List, "RME_Spurious_Handler");
    List->push_back("");

    /* Kernel function processing */
    Main::Info("> Generating kernel function handler hook.");
    Input.push_back("rme_ptr_t Func_ID - The function ID.");
    Input.push_back("rme_ptr_t Sub_ID - The subfunction ID.");
    Input.push_back("rme_ptr_t Param1 - The first parameter.");
    Input.push_back("rme_ptr_t Param2 - The second parameter.");
    Proj_Gen::Func_Head(List, "RME_Hook_Kfn_Handler",
                        "User-modifiable kernel function handler hook.",
                        Input, Output, "rme_ret_t - The return value.");
    Input.clear();
    List->push_back("rme_ret_t RME_Hook_Kfn_Handler(rme_ptr_t Func_ID,");
    List->push_back("                               rme_ptr_t Sub_ID,");
    List->push_back("                               rme_ptr_t Param1,");
    List->push_back("                               rme_ptr_t Param2)");
    List->push_back("{");
    List->push_back("    /* Add code here */");
    List->push_back("    return RME_ERR_KFN_FAIL;");
    List->push_back("}");
    Proj_Gen::Func_Foot(List, "RME_Hook_Kfn_Handler");
    List->push_back("");

    /* Generate rme_init.c */
    Main::Info("> Writing file.");
    Proj_Gen::Src_Foot(List);
    Proj_Gen::Line_Write(List, Kernel->Hook_Source_Output+"rme_hook.c");
}
/* End Function:Proj_Gen::Kernel_Hook_Src ************************************/

/* Function:Proj_Gen::Kernel_Handler_Src **************************************
Description : Create the handler source file for kernel. Each handler gets allocated
              a single source file in rme_handler_xxx.c��
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
void Proj_Gen::Kernel_Handler_Src(void)
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
    Input.push_back("struct RME_Reg_Struct* Reg - The register set.");

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
        Proj_Gen::Src_Head(List, "Filename", std::string("The top-half handler file for vector ")+Vct->Name+".");
        List->push_back("");

        /* Print all header includes */
        Main::Info("> Generating C header includes.");
        this->Kernel_Inc(List);
        List->push_back("");

        /* Print all global prototypes */
        Main::Info("> Generating public C function prototypes.");
        List->push_back("/* Public Function ***********************************************************/");
        List->push_back(std::string("rme_ptr_t RME_Vct_")+Vct->Name+"_Handler(struct RME_Reg_Struct* Reg);");
        List->push_back("/* End Public Function *******************************************************/");
        List->push_back("");

        /* Print function body */
        Main::Info(std::string("> Generating vector '")+Vct->Name+"' handler.");
        Proj_Gen::Func_Head(List, std::string("RME_Vct_")+Vct->Name+"_Handler",
                            "The user top-half interrupt handler for vector.",
                            Input, Output, "rme_ptr_t - Decides what endpoints to send to (see manual).");
        List->push_back(std::string("rme_ptr_t ")+std::string("RME_Vct_")+Vct->Name+"_Handler(struct RME_Reg_Struct* Reg)");
        List->push_back("{");
        /* See if we're dealing with benchmarks */
        if(Main::Benchmark!=0)
        {
            List->push_back(std::string("#define RME_TEST_")+Vct->Name_Upper+"_HANDLER");
            List->push_back("#include \"rme_test.h\"");
            List->push_back(std::string("#undef RME_TEST_")+Vct->Name_Upper+"_HANDLER");
        }
        else
        {
            List->push_back("    /* Add code here */");
            List->push_back("    return RME_RVM_VCT_SIG_ALL;");
        }
        List->push_back("}");
        Proj_Gen::Func_Foot(List, std::string("RME_Vct_")+Vct->Name+"_Handler");
        List->push_back("");

        /* Generate rme_init.c */
        Main::Info("> Writing file.");
        Proj_Gen::Src_Foot(List);
        Proj_Gen::Line_Write(List, Kernel->Handler_Source_Output+Filename);
        List->clear();
    }
}
/* End Function:Proj_Gen::Kernel_Handler_Src *********************************/

/* Function:Proj_Gen::Kernel_Linker *******************************************
Description : Create the linker script file for kernel.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
void Proj_Gen::Kernel_Linker(void)
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
    Proj_Gen::Line_Write(List, Kernel->Linker_Output+Kernel->Linker_Filename);
}
/* End Function:Proj_Gen::Kernel_Linker **************************************/

/* Function:Proj_Gen::Kernel_Proj *********************************************
Description : Create the project file for kernel.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
void Proj_Gen::Kernel_Proj(void)
{
    class Kernel* Kernel;
    class Monitor* Monitor;
    class Build_Gen* Build;
    class Tool_Gen* Tool;
    std::unique_ptr<std::vector<std::string>> List;
    std::vector<std::string> Include;
    std::vector<std::string> Source;
    std::vector<std::string> Library;
    std::vector<std::string> Linker;
    std::string Veneer;
    std::string Filename;
    std::string Extension;
    std::string Ending;

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
    Include.push_back(Main::Kernel_Root+"Include/");
    Include.push_back(Kernel->Config_Header_Output);
    Include.push_back(Kernel->Boot_Header_Output);
    /* Benchmarks need to include RVM test headers only */
    if(Main::Benchmark!=0)
        Include.push_back(Main::Monitor_Root+"Include/Test/Chip/");
    Proj_Gen::Path_Conv(Kernel->Project_Output, Include);

    for(const std::string& Path:Include)
        Main::Info(std::string("> > ")+Path);

    /* Extract the source paths */
    Main::Info("> Generating project source paths:");
    /* Regular kernel source */
    Library.push_back(Main::Kernel_Root+"Source/Kernel/rme_kernel.c");
    Library.push_back(Main::Kernel_Root+"Source/Platform/"+this->Plat->Name+"/rme_platform_"+this->Plat->Name_Lower+".c");
    Library.push_back(Main::Kernel_Root+"Source/Platform/"+this->Plat->Name+
                      "/rme_platform_"+this->Plat->Name_Lower+"_"+Tool->Name_Lower+Tool->Suffix(TOOL_ASSEMBLER));
    /* Additional chip-specific veneers (required only by some architectures to handle idiosyncrasies) */
    Veneer=Main::Kernel_Root+"Include/Platform/"+this->Plat->Name+"/Chip/"+this->Plat->Chip->Name_Upper;
    Ending=std::string("_")+Tool->Name_Lower+Tool->Suffix(TOOL_ASSEMBLER);
    for(const std::filesystem::directory_entry& File:std::filesystem::directory_iterator(Veneer))
    {
        Filename=File.path().filename().string();
        Extension=File.path().extension().string();
        /* If this is a c veneer, always add them to project */
        if((Extension=="C")||(Extension=="c"))
            Library.push_back(Veneer+"/"+Filename);
        /* If this is an assembly veneer, it will have to match the toolchain ending */
        else if((Filename.length()>Ending.length())&&(std::equal(Ending.rbegin(),Ending.rend(),Filename.rbegin())))
            Library.push_back(Veneer+"/"+Filename);
    }

    /* Hooks */
    Source.push_back(Kernel->Boot_Source_Output+"rme_boot.c");
    Source.push_back(Kernel->Hook_Source_Output+"rme_hook.c");
    for(const class Vect_Info* Vct:Monitor->Vector)
        Source.push_back(Kernel->Handler_Source_Output+"rme_handler_"+Vct->Name_Lower+".c");
    /* Other process compartments, if full image generation is required */
    if(Kernel->Full_Image!=0)
    {
        Source.push_back(Monitor->Project_Output+"monitor_image.c");
        for(const std::unique_ptr<class Process>& Prc:this->Plat->Proj->Process)
            Source.push_back(Prc->Project_Output+Prc->Name_Lower+"_image.c");
    }

    Proj_Gen::Path_Conv(Kernel->Project_Output, Source);
    Proj_Gen::Path_Conv(Kernel->Project_Output, Library);
    for(const std::string& Path:Source)
        Main::Info(std::string("> > ")+Path);
    for(const std::string& Path:Library)
        Main::Info(std::string("> > ")+Path);

    /* Extract the linker paths */
    Main::Info("> Generating project linker paths:");

    Linker.push_back(Kernel->Linker_Output+"rme_platform_"+this->Plat->Chip->Name_Lower+Tool->Suffix(TOOL_LINKER));
    Proj_Gen::Path_Conv(Kernel->Project_Output, Linker);

    for(const std::string& Path:Linker)
        Main::Info(std::string("> > ")+Path);

    Build->Kernel_Proj(List, Include, Source, Library, Linker);
    Proj_Gen::Line_Write(List, Kernel->Project_Output+Kernel->Project_Filename);
}
/* End Function:Proj_Gen::Kernel_Proj ****************************************/

/* Function:Proj_Gen::Monitor_Inc *********************************************
Description : Write the include files for monitor.
Input       : std::unique_ptr<std::vector<std::string>>& List - The input file.
Output      : std::unique_ptr<std::vector<std::string>>& List - The appended file.
Return      : None.
******************************************************************************/
void Proj_Gen::Monitor_Inc(std::unique_ptr<std::vector<std::string>>& List)
{
    std::string Temp;

    List->push_back("/* Include *******************************************************************/");
    List->push_back("#include \"rvm_boot.h\"");
    List->push_back("");
    Temp=std::string("#include \"Platform/")+this->Plat->Name+"/rvm_platform_"+this->Plat->Name_Lower+".h\"";
    List->push_back("#define __HDR_DEF__");
    List->push_back(Temp);
    List->push_back("#include \"Syslib/rvm_syslib.h\"");
    List->push_back("#include \"Monitor/rvm_monitor.h\"");
    List->push_back("#undef __HDR_DEF__");
    List->push_back("");
    List->push_back("#define __HDR_STRUCT__");
    List->push_back(Temp);
    List->push_back("#include \"Syslib/rvm_syslib.h\"");
    List->push_back("#include \"Monitor/rvm_monitor.h\"");
    List->push_back("#undef __HDR_STRUCT__");
    List->push_back("");
    List->push_back("#define __HDR_PUBLIC__");
    List->push_back(Temp);
    List->push_back("#include \"Syslib/rvm_syslib.h\"");
    List->push_back("#include \"Monitor/rvm_monitor.h\"");
    List->push_back("#undef __HDR_PUBLIC__");
    List->push_back("/* End Include ***************************************************************/");
}
/* End Function:Proj_Gen::Monitor_Inc ****************************************/

/* Function:Proj_Gen::Monitor_Conf_Hdr ****************************************
Description : Generate the platform configuration headers for monitor.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
void Proj_Gen::Monitor_Conf_Hdr(void)
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
    Proj_Gen::Src_Head(List, "rvm_platform.h", "The platform selection header.");
    List->push_back("");
    List->push_back("/* Platform Include **********************************************************/");
    List->push_back(std::string("#include \"Platform/")+this->Plat->Name+"/rvm_platform_"+this->Plat->Name_Lower+".h\"");
    List->push_back("/* End Platform Include ******************************************************/");
    List->push_back("");
    Proj_Gen::Src_Foot(List);
    Proj_Gen::Line_Write(List, Monitor->Config_Header_Output+"/rvm_platform.h");

    /* Generate rvm_platform_xxx_conf.h */
    Filename=std::string("rvm_platform_")+this->Plat->Name_Lower+"_conf.h";
    Main::Info(std::string("> Generating '")+Filename+"'.");
    List->clear();
    Proj_Gen::Src_Head(List, Filename, "The chip selection header.");
    List->push_back("");
    List->push_back("/* Platform Include **********************************************************/");
    List->push_back(std::string("#include \"rvm_platform_")+this->Plat->Chip->Name_Lower+".h\"");
    List->push_back("/* End Platform Include ******************************************************/");
    List->push_back("");
    Proj_Gen::Src_Foot(List);
    Proj_Gen::Line_Write(List, Monitor->Config_Header_Output+"rvm_platform_"+this->Plat->Name_Lower+"_conf.h");


    /* Generate rvm_platform_chipname.h */
    Filename=std::string("rvm_platform_")+this->Plat->Chip->Name_Lower+".h";
    Main::Info(std::string("> Generating '")+Filename+"'.");
    List=Proj_Gen::Line_Read(Main::Monitor_Root+
                             "Include/Platform/"+this->Plat->Name+"/Chip/"+this->Plat->Chip->Name+"/"+Filename);
    /* Replace general parameter macros */
    /* Debugging */
    Proj_Gen::Macro_Int(List, "RVM_ASSERT_ENABLE", this->Plat->Proj->Assert_Enable, MACRO_REPLACE);
    Proj_Gen::Macro_Int(List, "RVM_DBGLOG_ENABLE", this->Plat->Proj->Debug_Log_Enable, MACRO_REPLACE);
    /* Whether the region mappings are fixed hence the RVM should provide them */
    Proj_Gen::Macro_Int(List, "RVM_PGT_RAW_ENABLE", this->Plat->Proj->Pgtbl_Raw_Enable, MACRO_REPLACE);
    /* The virtual memory base/size for the kernel objects */
    Proj_Gen::Macro_Hex(List, "RVM_KOM_VA_BASE", Kernel->Kom_Base, MACRO_REPLACE);
    Proj_Gen::Macro_Hex(List, "RVM_KOM_VA_SIZE", Kernel->Kom_Size, MACRO_REPLACE);
    /* The granularity of kernel memory allocation, in bytes */
    Proj_Gen::Macro_Int(List, "RVM_KOM_SLOT_ORDER", Kernel->Kom_Order, MACRO_REPLACE);
    /* The maximum number of preemption priority levels in the system.
     * This parameter must be divisible by the word length - 32 is usually sufficient */
    Proj_Gen::Macro_Int(List, "RVM_PREEMPT_PRIO_NUM", Kernel->Kern_Prio, MACRO_REPLACE);
    /* Initial kernel object frontier limit */
    Proj_Gen::Macro_Int(List, "RVM_CPT_BOOT_FRONT", Monitor->Before_Cap_Front, MACRO_REPLACE);
    Proj_Gen::Macro_Hex(List, "RVM_KOM_BOOT_FRONT", Monitor->Before_Kom_Front, MACRO_REPLACE);
    /* Post-boot kernel object frontier limit */
    Proj_Gen::Macro_Int(List, "RVM_CPT_DONE_FRONT", Monitor->After_Cap_Front, MACRO_REPLACE);
    Proj_Gen::Macro_Hex(List, "RVM_KOM_DONE_FRONT", Monitor->After_Kom_Front, MACRO_REPLACE);
    /* Set coprocessor macros - set all of them to 0 first, then modify from there */
    Proj_Gen::Macro_Int(List, "RVM_COP_NUM", this->Plat->Proj->Chip->Coprocessor.size(), MACRO_REPLACE);
    for(const std::string& Cop:this->Plat->Plat->Coprocessor)
        Proj_Gen::Macro_Int(List, "RVM_"+this->Plat->Name_Upper+"_COP_"+Cop, 0, MACRO_REPLACE);
    for(const std::string& Cop:this->Plat->Proj->Chip->Coprocessor)
        Proj_Gen::Macro_Int(List, "RVM_"+this->Plat->Name_Upper+"_COP_"+Cop, 1, MACRO_REPLACE);

    /* Monitor */
    /* Using idle sleep or not */
    Proj_Gen::Macro_Int(List, "RVM_IDLE_SLEEP_ENABLE", Monitor->Idle_Sleep_Enable, MACRO_REPLACE);
    /* Number of virtual priorities in the system */
    Proj_Gen::Macro_Int(List, "RVM_PREEMPT_VPRIO_NUM", Monitor->Virt_Prio, MACRO_REPLACE);
    /* Physical vector number, flag area base and its size */
    Proj_Gen::Macro_Int(List, "RVM_PHYS_VCT_NUM", this->Plat->Chip->Vector, MACRO_REPLACE);
    Proj_Gen::Macro_Hex(List, "RVM_PHYS_VCTF_BASE", Kernel->Vctf_Base, MACRO_REPLACE);
    Proj_Gen::Macro_Hex(List, "RVM_PHYS_VCTF_SIZE", Kernel->Vctf_Size, MACRO_REPLACE);
    /* Virtual event number, flag area base and its size */
    Proj_Gen::Macro_Int(List, "RVM_VIRT_EVT_NUM", Monitor->Virt_Event, MACRO_REPLACE);
    Proj_Gen::Macro_Hex(List, "RVM_VIRT_EVTF_BASE", Kernel->Evtf_Base, MACRO_REPLACE);
    Proj_Gen::Macro_Hex(List, "RVM_VIRT_EVTF_SIZE", Kernel->Evtf_Size, MACRO_REPLACE);
    /* Maximum number of mappings allowed */
    Proj_Gen::Macro_Int(List, "RVM_VIRT_MAP_NUM", Monitor->Virt_Map, MACRO_REPLACE);

    /* Stack base and size of the daemon threads, in bytes */
    Proj_Gen::Macro_Hex(List, "RVM_SFTD_STACK_BASE", Monitor->Sftd_Stack_Base, MACRO_REPLACE);
    Proj_Gen::Macro_Hex(List, "RVM_SFTD_STACK_SIZE", Monitor->Sftd_Stack_Size, MACRO_REPLACE);
    Proj_Gen::Macro_Hex(List, "RVM_VMMD_STACK_BASE", Monitor->Vmmd_Stack_Base, MACRO_REPLACE);
    Proj_Gen::Macro_Hex(List, "RVM_VMMD_STACK_SIZE", Monitor->Vmmd_Stack_Size, MACRO_REPLACE);

    /* Replace platform specific macros */
    this->Plat->Monitor_Conf_Hdr(List);
    /* Write to Monitor configuration file */
    Proj_Gen::Line_Write(List, Monitor->Config_Header_Output+Filename);

    /* If we're running benchmark, need to generate the test header as well */
    if(Main::Benchmark!=0)
    {
        Main::Info("> Generating test selection header.");
        List->clear();
        Proj_Gen::Src_Head(List, "rvm_test.h", "The RVM guest library test header.");
        List->push_back("");
        List->push_back("/* Include *******************************************************************/");
        List->push_back(std::string("#include \"Test/Chip/rvm_test_")+this->Plat->Chip->Name_Lower+".h\"");
        List->push_back("/* End Include ***************************************************************/");
        List->push_back("");
        Proj_Gen::Src_Foot(List);

        Proj_Gen::Line_Write(List, Monitor->Config_Header_Output+"rvm_test.h");
    }
}
/* End Function:Proj_Gen::Monitor_Conf_Hdr ***********************************/

/* Function:Proj_Gen::Monitor_Cpt_Init ****************************************
Description : Build capability table contents.
Input       : std::unique_ptr<std::vector<std::string>>& List - The file to add these to.
              ptr_t Is_Kfn - Whether this iteration is kfunc dedicated.
Output      : None.
Return      : ptr_t - The number of operations.
******************************************************************************/
ptr_t Proj_Gen::Monitor_Cpt_Init(std::unique_ptr<std::vector<std::string>>& List, ptr_t Is_Kfn)
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
                                Main::Hex(Kfn->Begin)+"U, 0x"+Main::Hex(Kfn->End)+"U},");
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
/* End Function:Proj_Gen::Monitor_Cpt_Init ***********************************/

/* Function:Proj_Gen::Monitor_Pgt_Con *****************************************
Description : Construct the page table for monitor. This will produce the desired
              final page table tree, and is recursive.
Input       : std::unique_ptr<std::vector<std::string>>& List - The file to add these to.
              const class Pgtbl* Pgt - The page table structure.
Output      : None.
Return      : ptr_t - The number of operations.
******************************************************************************/
ptr_t Proj_Gen::Monitor_Pgt_Con(std::unique_ptr<std::vector<std::string>>& List, const class Pgtbl* Pgt)
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
/* End Function:Proj_Gen::Monitor_Pgt_Con ************************************/

/* Function:Proj_Gen::Monitor_Pgt_Add *****************************************
Description : Map pages into a page table. This is not recursive.
Input       : std::unique_ptr<std::vector<std::string>>& List - The file to add these to.
              const class Pgtbl* Pgt - The page table structure.
              ptr_t Init_Size_Ord - The initial page table's size order.
Output      : None.
Return      : ptr_t - The number of operations.
******************************************************************************/
ptr_t Proj_Gen::Monitor_Pgt_Add(std::unique_ptr<std::vector<std::string>>& List,
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
        Flags.clear();
        Total++;
    }

    return Total;
}
/* End Function:Proj_Gen::Monitor_Pgt_Add ************************************/

/* Function:Proj_Gen::Monitor_Thd_Init ****************************************
Description : Initialize threads.
Input       : std::unique_ptr<std::vector<std::string>>& List - The file to add these to.
Output      : None.
Return      : ptr_t - The number of operations.
******************************************************************************/
ptr_t Proj_Gen::Monitor_Thd_Init(std::unique_ptr<std::vector<std::string>>& List)
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
				Line+="RVM_VIRT_TID_FLAG, ";
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
/* End Function:Proj_Gen::Monitor_Thd_Init ***********************************/

/* Function:Proj_Gen::Monitor_Inv_Init ****************************************
Description : Initialize invocations.
Input       : std::unique_ptr<std::vector<std::string>>& List - The file to add these to.
Output      : None.
Return      : ptr_t - The number of operations.
******************************************************************************/
ptr_t Proj_Gen::Monitor_Inv_Init(std::unique_ptr<std::vector<std::string>>& List)
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
/* End Function:Proj_Gen::Monitor_Inv_Init ***********************************/

/* Function:Proj_Gen::Monitor_Boot_Hdr ****************************************
Description : Create the boot header for monitor.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
void Proj_Gen::Monitor_Boot_Hdr(void)
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
    Proj_Gen::Src_Head(List, "rvm_boot.h", "The boot-time initialization file header.");
    List->push_back("");

    Main::Info("> Generating defines.");
    List->push_back("/* Define ********************************************************************/");

    /* Vector capability tables & objects */
    List->push_back("/* Vector endpoint capability tables */");
    Cap_Front=Kernel->Vct_Cap_Front;
    for(Obj_Cnt=0;Obj_Cnt<Monitor->Vector.size();Obj_Cnt+=this->Plat->Plat->Captbl_Max)
    {
        Proj_Gen::Macro_Int(List, std::string("RVM_MAIN_VCT_")+MIDS(Obj_Cnt), Cap_Front, MACRO_ADD);
        Cap_Front++;
    }
    List->push_back("");
    List->push_back("/* Vector endpoints */");
    for(const class Vect_Info* Vct:Monitor->Vector)
    {
        Proj_Gen::Macro_String(List, Vct->Macro_Global,
                               std::string("RVM_CID(RVM_MAIN_VCT_")+
                               MIDS(Vct->Cid_Global)+", "+SIDS(Vct->Cid_Global)+"U)", MACRO_ADD);
    }
    List->push_back("");

    /* These are reserved for RVM kernel objects */
    List->push_back("/* 2 threads and 1 endpoint for RVM */");
    List->push_back("");

    /* Virual machine endpoint capability tables */
    if(this->Plat->Proj->Virtual.size()!=0)
    {
        List->push_back("/* Virtual machine endpoint capability tables */");
        Cap_Front=Monitor->Vep_Cap_Front;
        for(Obj_Cnt=0;Obj_Cnt<this->Plat->Proj->Virtual.size();Obj_Cnt+=this->Plat->Plat->Captbl_Max)
        {
            Proj_Gen::Macro_Int(List, std::string("RVM_MAIN_VEP_")+MIDS(Obj_Cnt), Cap_Front, MACRO_ADD);
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
        Proj_Gen::Macro_Int(List, std::string("RVM_MAIN_CPT_")+MIDS(Obj_Cnt), Cap_Front, MACRO_ADD);
        Cap_Front++;
    }
    if(this->Plat->Proj->Pgtbl_Raw_Enable==0)
    {
        if(Cap_Front!=Monitor->Pgt_Cap_Front)
            Main::Error("XXXXX: Capability table capability table computation failure.");
    }
    else
    {
        if(Cap_Front!=Monitor->Prc_Cap_Front)
            Main::Error("XXXXX: Capability table capability table computation failure.");
    }
    List->push_back("");
    List->push_back("/* Process capability tables */");
    for(const class Captbl* Cpt:Monitor->Captbl)
    {
        Proj_Gen::Macro_String(List, Cpt->Macro_Global,
                               std::string("RVM_CID(RVM_MAIN_CPT_")+
                               MIDS(Cpt->Cid_Global)+", "+SIDS(Cpt->Cid_Global)+"U)", MACRO_ADD);
    }
    List->push_back("");

    /* Pgt capability tables & objects */
    if(this->Plat->Proj->Pgtbl_Raw_Enable==0)
    {
        List->push_back("/* Process page table capability tables */");
        Cap_Front=Monitor->Pgt_Cap_Front;
        for(Obj_Cnt=0;Obj_Cnt<Monitor->Pgtbl.size();Obj_Cnt+=this->Plat->Plat->Captbl_Max)
        {
            Proj_Gen::Macro_Int(List, std::string("RVM_MAIN_PGT_")+MIDS(Obj_Cnt), Cap_Front, MACRO_ADD);
            Cap_Front++;
        }
        if(Cap_Front!=Monitor->Prc_Cap_Front)
            Main::Error("XXXXX: Page table capability table computation failure.");
        List->push_back("");
        List->push_back("/* Process page tables */");
        for(const class Pgtbl* Pgt:Monitor->Pgtbl)
        {
            Proj_Gen::Macro_String(List, Pgt->Macro_Global,
                                   std::string("RVM_CID(RVM_MAIN_PGT_")+
                                   MIDS(Pgt->Cid_Global)+", "+SIDS(Pgt->Cid_Global)+"U)", MACRO_ADD);
        }
        List->push_back("");
    }

    /* Process capability tables & objects */
    List->push_back("/* Process capability tables */");
    Cap_Front=Monitor->Prc_Cap_Front;
    for(Obj_Cnt=0;Obj_Cnt<Monitor->Process.size();Obj_Cnt+=this->Plat->Plat->Captbl_Max)
    {
        Proj_Gen::Macro_Int(List, std::string("RVM_MAIN_PRC_")+MIDS(Obj_Cnt), Cap_Front, MACRO_ADD);
        Cap_Front++;
    }
    if(Cap_Front!=Monitor->Thd_Cap_Front)
        Main::Error("XXXXX: Process capability table computation failure.");
    List->push_back("");
    List->push_back("/* Processes */");
    for(const class Process* Prc:Monitor->Process)
    {
        Proj_Gen::Macro_String(List, Prc->Macro_Global,
                               std::string("RVM_CID(RVM_MAIN_PRC_")+
                               MIDS(Prc->Cid_Global)+", "+ SIDS(Prc->Cid_Global)+"U)", MACRO_ADD);
    }
    List->push_back("");

    /* Thread capability tables & objects */
    List->push_back("/* Thread capability tables */");
    Cap_Front=Monitor->Thd_Cap_Front;
    for(Obj_Cnt=0;Obj_Cnt<Monitor->Thread.size();Obj_Cnt+=this->Plat->Plat->Captbl_Max)
    {
        Proj_Gen::Macro_Int(List, std::string("RVM_MAIN_THD_")+MIDS(Obj_Cnt), Cap_Front, MACRO_ADD);
        Cap_Front++;
    }
    if(Cap_Front!=Monitor->Inv_Cap_Front)
        Main::Error("XXXXX: Thread capability table computation failure.");
    List->push_back("");
    List->push_back("/* Threads */");
    for(const class Thread* Thd:Monitor->Thread)
    {
        Proj_Gen::Macro_String(List, Thd->Macro_Global,
                               std::string("RVM_CID(RVM_MAIN_THD_")+
                               MIDS(Thd->Cid_Global)+", "+SIDS(Thd->Cid_Global)+"U)", MACRO_ADD);
    }
    List->push_back("");

    /* Invocation capability tables & objects */
    List->push_back("/* Invocation capability tables */");
    Cap_Front=Monitor->Inv_Cap_Front;
    for(Obj_Cnt=0;Obj_Cnt<Monitor->Invocation.size();Obj_Cnt+=this->Plat->Plat->Captbl_Max)
    {
        Proj_Gen::Macro_Int(List, std::string("RVM_MAIN_INV_")+MIDS(Obj_Cnt), Cap_Front, MACRO_ADD);
        Cap_Front++;
    }
    if(Cap_Front!=Monitor->Rcv_Cap_Front)
        Main::Error("XXXXX: Invocation capability table computation failure.");
    List->push_back("");
    List->push_back("/* Invocations */");
    for(const class Invocation* Inv:Monitor->Invocation)
    {
        Proj_Gen::Macro_String(List, Inv->Macro_Global,
                               std::string("RVM_CID(RVM_MAIN_INV_")+
                               MIDS(Inv->Cid_Global)+", "+SIDS(Inv->Cid_Global)+"U)", MACRO_ADD);
    }
    List->push_back("");

    /* Receive endpoint capability tables & objects */
    List->push_back("/* Receive endpoint capability tables */");
    Cap_Front=Monitor->Rcv_Cap_Front;
    for(Obj_Cnt=0;Obj_Cnt<Monitor->Receive.size();Obj_Cnt+=this->Plat->Plat->Captbl_Max)
    {
        Proj_Gen::Macro_Int(List, std::string("RVM_MAIN_RCV_")+MIDS(Obj_Cnt), Cap_Front, MACRO_ADD);
        Cap_Front++;
    }
    if(Cap_Front!=Monitor->After_Cap_Front)
        Main::Error("XXXXX: Receive endpoint capability table computation failure.");
    List->push_back("");
    List->push_back("/* Receive endpoints */");
    for(const class Receive* Rcv:Monitor->Receive)
    {
        Proj_Gen::Macro_String(List, Rcv->Macro_Global,
                               std::string("RVM_CID(RVM_MAIN_RCV_")+
                               MIDS(Rcv->Cid_Global)+", "+SIDS(Rcv->Cid_Global)+"U)", MACRO_ADD);
    }
    List->push_back("");

    /* Number of virtual machines */
    List->push_back("/* VM number */");
    Proj_Gen::Macro_Int(List, "RVM_VIRT_NUM", this->Plat->Proj->Virtual.size(), MACRO_ADD);
    List->push_back("");

    /* Other frontiers */
    List->push_back("/* Virtual endpoint frontiers & number */");
    Proj_Gen::Macro_Hex(List, "RVM_BOOT_VEP_BEFORE", Monitor->Vep_Kom_Front, MACRO_ADD);
    Proj_Gen::Macro_Hex(List, "RVM_BOOT_VEP_AFTER", Monitor->Cpt_Kom_Front, MACRO_ADD);
    Proj_Gen::Macro_Int(List, "RVM_BOOT_VEP_MAIN_NUM", CTNUM(this->Plat->Proj->Virtual.size()), MACRO_ADD);
    Proj_Gen::Macro_Int(List, "RVM_BOOT_VEP_CRT_NUM", this->Plat->Proj->Virtual.size(), MACRO_ADD);
    Proj_Gen::Macro_Int(List, "RVM_BOOT_VCAP_INIT_NUM", this->Plat->Proj->Virtual.size(), MACRO_ADD);
    List->push_back("");

    List->push_back("/* Cpt frontiers & number */");
    Proj_Gen::Macro_Hex(List, "RVM_BOOT_INIT_CPT_BEFORE", Monitor->Cpt_Kom_Front, MACRO_ADD);
    /* See if we're skipping page tables when all regions are fixed */
    if(this->Plat->Proj->Pgtbl_Raw_Enable==0)
        Proj_Gen::Macro_Hex(List, "RVM_BOOT_INIT_CPT_AFTER", Monitor->Pgt_Kom_Front, MACRO_ADD);
    else
        Proj_Gen::Macro_Hex(List, "RVM_BOOT_INIT_CPT_AFTER", Monitor->Prc_Kom_Front, MACRO_ADD);
    Proj_Gen::Macro_Int(List, "RVM_BOOT_INIT_CPT_MAIN_NUM", CTNUM(Monitor->Captbl.size()), MACRO_ADD);
    Proj_Gen::Macro_Int(List, "RVM_BOOT_INIT_CPT_CRT_NUM", Monitor->Captbl.size(), MACRO_ADD);
    this->Cpt_Init_Total=this->Monitor_Cpt_Init(Dummy, 0);
    Proj_Gen::Macro_Int(List, "RVM_BOOT_INIT_CPT_INIT_NUM", this->Cpt_Init_Total, MACRO_ADD);
    this->Cpt_Kfn_Total=this->Monitor_Cpt_Init(Dummy, 1);
    Proj_Gen::Macro_Int(List, "RVM_BOOT_INIT_CPT_KFN_NUM", this->Cpt_Kfn_Total, MACRO_ADD);
    List->push_back("");

    List->push_back("/* Pgt frontiers & number */");
    /* Only exist when using kernel-managed PCTrie */
    if(this->Plat->Proj->Pgtbl_Raw_Enable==0)
    {
        Proj_Gen::Macro_Hex(List, "RVM_BOOT_INIT_PGT_BEFORE", Monitor->Pgt_Kom_Front, MACRO_ADD);
        Proj_Gen::Macro_Hex(List, "RVM_BOOT_INIT_PGT_AFTER", Monitor->Prc_Kom_Front, MACRO_ADD);
        Proj_Gen::Macro_Int(List, "RVM_BOOT_INIT_PGT_MAIN_NUM", CTNUM(Monitor->Pgtbl.size()), MACRO_ADD);
        Proj_Gen::Macro_Int(List, "RVM_BOOT_INIT_PGT_CRT_NUM", Monitor->Pgtbl.size(), MACRO_ADD);
        Total=0;
        for(const std::unique_ptr<class Process>& Prc:this->Plat->Proj->Process)
            Total+=this->Monitor_Pgt_Con(Dummy, Prc->Pgtbl.get());
        this->Pgt_Con_Total=Total;
        Proj_Gen::Macro_Int(List, "RVM_BOOT_INIT_PGT_CON_NUM", Total, MACRO_ADD);
        Total=0;
        for(class Pgtbl* Pgt:Monitor->Pgtbl)
            Total+=this->Monitor_Pgt_Add(Dummy, Pgt, this->Plat->Plat->Wordlength);
        Proj_Gen::Macro_Int(List, "RVM_BOOT_INIT_PGT_ADD_NUM", Total, MACRO_ADD);
        List->push_back("");
    }

    List->push_back("/* Process frontiers & number */");
    Proj_Gen::Macro_Hex(List, "RVM_BOOT_PRC_BEFORE", Monitor->Prc_Kom_Front, MACRO_ADD);
    Proj_Gen::Macro_Hex(List, "RVM_BOOT_PRC_AFTER", Monitor->Thd_Kom_Front, MACRO_ADD);
    Proj_Gen::Macro_Int(List, "RVM_BOOT_PRC_MAIN_NUM", CTNUM(Monitor->Process.size()), MACRO_ADD);
    Proj_Gen::Macro_Int(List, "RVM_BOOT_PRC_CRT_NUM", Monitor->Process.size(), MACRO_ADD);
    List->push_back("");

    List->push_back("/* Thread frontiers & number */");
    Proj_Gen::Macro_Hex(List, "RVM_BOOT_THD_BEFORE", Monitor->Thd_Kom_Front, MACRO_ADD);
    Proj_Gen::Macro_Hex(List, "RVM_BOOT_THD_AFTER", Monitor->Inv_Kom_Front, MACRO_ADD);
    Proj_Gen::Macro_Int(List, "RVM_BOOT_THD_MAIN_NUM", CTNUM(Monitor->Thread.size()), MACRO_ADD);
    Proj_Gen::Macro_Int(List, "RVM_BOOT_THD_CRT_NUM", Monitor->Thread.size(), MACRO_ADD);
    Proj_Gen::Macro_Int(List, "RVM_BOOT_THD_INIT_NUM", Monitor->Thread.size(), MACRO_ADD);
    ASSERT(Monitor->Thread.size()==this->Monitor_Thd_Init(Dummy));
    List->push_back("");

    List->push_back("/* Invocation frontiers & number */");
    Proj_Gen::Macro_Hex(List, "RVM_BOOT_INV_BEFORE", Monitor->Inv_Kom_Front, MACRO_ADD);
    Proj_Gen::Macro_Hex(List, "RVM_BOOT_INV_AFTER", Monitor->Rcv_Kom_Front, MACRO_ADD);
    Proj_Gen::Macro_Int(List, "RVM_BOOT_INV_MAIN_NUM", CTNUM(Monitor->Invocation.size()), MACRO_ADD);
    Proj_Gen::Macro_Int(List, "RVM_BOOT_INV_CRT_NUM", Monitor->Invocation.size(), MACRO_ADD);
    Proj_Gen::Macro_Int(List, "RVM_BOOT_INV_INIT_NUM", Monitor->Invocation.size(), MACRO_ADD);
    ASSERT(Monitor->Invocation.size()==this->Monitor_Inv_Init(Dummy));
    List->push_back("");

    List->push_back("/* Receive endpoint frontiers & number */");
    Proj_Gen::Macro_Hex(List, "RVM_BOOT_RCV_BEFORE", Monitor->Rcv_Kom_Front, MACRO_ADD);
    Proj_Gen::Macro_Hex(List, "RVM_BOOT_RCV_AFTER", Monitor->After_Kom_Front, MACRO_ADD);
    Proj_Gen::Macro_Int(List, "RVM_BOOT_RCV_MAIN_NUM", CTNUM(Monitor->Receive.size()), MACRO_ADD);
    Proj_Gen::Macro_Int(List, "RVM_BOOT_RCV_CRT_NUM", Monitor->Receive.size(), MACRO_ADD);
    List->push_back("");

    /* All memory trunks ever defined in the project, including all projects */
    this->Boot_Hdr_Mem("RVM_", List);
    List->push_back("/* End Define ****************************************************************/");
    List->push_back("");
    Proj_Gen::Src_Foot(List);

    Main::Info("> Writing file.");
    Proj_Gen::Line_Write(List, Monitor->Boot_Header_Output+"rvm_boot.h");
}
/* End Function:Proj_Gen::Monitor_Boot_Hdr ***********************************/

/* Function:Proj_Gen::Monitor_Main_Crt ****************************************
Description : Generate the main container capability tables.
Input       : std::unique_ptr<std::vector<std::string>>& List - The file to add these to.
              ptr_t Number - The total number of slots that the captbls should have.
              const std::string& Macro - The macro infix of these captbls.
Output      : None.
Return      : None.
******************************************************************************/
void Proj_Gen::Monitor_Main_Crt(std::unique_ptr<std::vector<std::string>>& List,
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
/* End Function:Proj_Gen::Monitor_Main_Crt ***********************************/

/* Function:Proj_Gen::Monitor_Boot_Src ****************************************
Description : Create the boot source for monitor. This function is pretty lengthy,
              but it generates a single source, so we're not splitting it.
              We're using metadata for most of the initializations. For those that
              are particularly entangled, we opt to use a customized function.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
void Proj_Gen::Monitor_Boot_Src(void)
{
    ptr_t Obj_Cnt;
    class Monitor* Monitor;
    std::vector<std::string> Input;
    std::vector<std::string> Output;
    std::unique_ptr<std::vector<std::string>> List;
    std::string Coprocessor;

    Monitor=this->Plat->Proj->Monitor.get();
    List=std::make_unique<std::vector<std::string>>();

    /* File header */
    Main::Info("> Generating file header.");
    Proj_Gen::Src_Head(List, "rvm_boot.c", "The boot-time initialization file.");
    List->push_back("");

    /* Print all header includes */
    Main::Info("> Generating C header includes.");
    this->Monitor_Inc(List);
    List->push_back("");

    Main::Info("> Generating public global variables.");
    List->push_back("/* Public Variable ***********************************************************/");
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
    List->push_back("const struct RVM_Meta_Main_Struct RVM_Meta_Cpt_Main[RVM_BOOT_INIT_CPT_MAIN_NUM]=");
    List->push_back("{");
    this->Monitor_Main_Crt(List, Monitor->Captbl.size(), "CPT");
    List->push_back("};");
    List->push_back("const struct RVM_Meta_Cpt_Crt_Struct RVM_Meta_Cpt_Crt[RVM_BOOT_INIT_CPT_CRT_NUM]=");
    List->push_back("{");
    for(const class Captbl* Cpt:Monitor->Captbl)
    {
        List->push_back(std::string("{RVM_MAIN_CPT_")+MIDS(Cpt->Cid_Global)+", "+
                        SIDS(Cpt->Cid_Global)+", "+std::to_string(Cpt->Size)+"U},");
    }
    List->push_back("};");
    if(this->Cpt_Init_Total!=0)
    {
        List->push_back("const struct RVM_Meta_Cpt_Init_Struct RVM_Meta_Cpt_Init[RVM_BOOT_INIT_CPT_INIT_NUM]=");
        List->push_back("{");
        this->Monitor_Cpt_Init(List, 0);
        List->push_back("};");
    }
    if(this->Cpt_Kfn_Total!=0)
    {
        List->push_back("const struct RVM_Meta_Cpt_Kfn_Struct RVM_Meta_Cpt_Kfn[RVM_BOOT_INIT_CPT_KFN_NUM]=");
        List->push_back("{");
        this->Monitor_Cpt_Init(List, 1);
        List->push_back("};");
    }
    List->push_back("");

    /* Page table metadata - only exist when using kernel-managed page tables */
    if(this->Plat->Proj->Pgtbl_Raw_Enable==0)
    {
        List->push_back("/* Page table metadata */");
        List->push_back("const struct RVM_Meta_Main_Struct RVM_Meta_Pgt_Main[RVM_BOOT_INIT_PGT_MAIN_NUM]=");
        List->push_back("{");
        this->Monitor_Main_Crt(List, Monitor->Pgtbl.size(), "PGT");
        List->push_back("};");
        List->push_back("const struct RVM_Meta_Pgt_Crt_Struct RVM_Meta_Pgt_Crt[RVM_BOOT_INIT_PGT_CRT_NUM]=");
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
            List->push_back("const struct RVM_Meta_Pgt_Con_Struct RVM_Meta_Pgt_Con[RVM_BOOT_INIT_PGT_CON_NUM]=");
            List->push_back("{");
            for(const std::unique_ptr<class Process>& Prc:this->Plat->Proj->Process)
                this->Monitor_Pgt_Con(List, Prc->Pgtbl.get());
            List->push_back("};");
        }
        List->push_back("const struct RVM_Meta_Pgt_Add_Struct RVM_Meta_Pgt_Add[RVM_BOOT_INIT_PGT_ADD_NUM]=");
        List->push_back("{");
        for(const class Pgtbl* Pgt:Monitor->Pgtbl)
            this->Monitor_Pgt_Add(List, Pgt, this->Plat->Plat->Wordlength);
        List->push_back("};");
    }
    /* Process raw page table when fixed mode is selected */
    else
    {
        List->push_back("/* Process page table raw data */");
        for(const class Process* Prc:Monitor->Process)
        {
            List->push_back(std::string("const rvm_ptr_t RVM_Meta_Raw_")+Prc->Name+"["+std::to_string(Prc->Rawtbl->size())+"U]=");
            List->push_back("{");
            for(ptr_t Raw:*Prc->Rawtbl)
                List->push_back(std::string("    0x")+Main::Hex(Raw)+"U,");
            List->push_back("};");
        }
    }
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
        /* Load normal page table capability if kernel-managed */
        if(this->Plat->Proj->Pgtbl_Raw_Enable==0)
        {
            List->push_back(std::string("{RVM_MAIN_PRC_")+MIDS(Prc->Cid_Global)+", "+SIDS(Prc->Cid_Global)+", "+
                            Prc->Captbl->Macro_Global+", "+Prc->Pgtbl->Macro_Global+"},");
        }
        /* Load raw table when fixed mode is selected */
        else
        {
            List->push_back(std::string("{RVM_MAIN_PRC_")+MIDS(Prc->Cid_Global)+", "+SIDS(Prc->Cid_Global)+", "+
                            Prc->Captbl->Macro_Global+", (rvm_ptr_t)RVM_Meta_Raw_"+Prc->Name+"},");
        }
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
    	/* Generate coprocessor list for this thread */
    	Coprocessor.clear();
    	for(const std::string& Cop:Thd->Owner->Coprocessor)
    		Coprocessor+="RVM_"+this->Plat->Name_Upper+"_ATTR_"+Cop+"|";
        /* If list is empty, specify "none" */
    	if(Coprocessor=="")
    	    Coprocessor="RVM_"+this->Plat->Name_Upper+"_ATTR_NONE";
    	else
    	    Coprocessor.pop_back();
        List->push_back(std::string("{RVM_MAIN_THD_")+MIDS(Thd->Cid_Global)+", "+SIDS(Thd->Cid_Global)+", "+
                        Thd->Owner->Macro_Global+", "+std::to_string(Thd->Priority)+"U, "+
						Coprocessor+", "+std::to_string(Thd->Is_Hyp)+"U},");
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
    List->push_back("/* End Public Variable *******************************************************/");
    List->push_back("");
    Proj_Gen::Src_Foot(List);

    /* Generate rvm_boot.c */
    Main::Info("> Writing file.");
    Proj_Gen::Line_Write(List, Monitor->Boot_Source_Output+"rvm_boot.c");
}
/* End Function:Proj_Gen::Monitor_Boot_Src ***********************************/

/* Function:Proj_Gen::Monitor_Hook_Src ****************************************
Description : Create the hook source file for monitor.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
void Proj_Gen::Monitor_Hook_Src(void)
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
    Proj_Gen::Src_Head(List, "rvm_hook.c", "The user initialization hook file.");
    List->push_back("");

    /* Print all header includes */
    Main::Info("> Generating C header includes.");
    this->Monitor_Inc(List);
    List->push_back("");

    /* Print all global prototypes */
    Main::Info("> Generating public C function prototypes.");
    List->push_back("/* Public Function ***********************************************************/");
    List->push_back("void RVM_Boot_Pre_Init(void);");
    List->push_back("void RVM_Boot_Post_Init(void);");
    List->push_back("rvm_ret_t RVM_Vct_Filter(rvm_ptr_t Vct_Num);");
    List->push_back("rvm_ret_t RVM_Evt_Filter(rvm_ptr_t Evt_Num);");
    List->push_back("/* End Public Function *******************************************************/");
    List->push_back("");

    /* Preinitialization of hardware */
    Main::Info("> Generating boot-time pre-initialization stub.");
    Proj_Gen::Func_Head(List, "RVM_Boot_Pre_Init",
                        "Initialize critical hardware before any initialization takes place.",
                        Input, Output, "None.");
    List->push_back("void RVM_Boot_Pre_Init(void)");
    List->push_back("{");
    List->push_back("    /* Add code here */");
    List->push_back("}");
    Proj_Gen::Func_Foot(List, "RVM_Boot_Pre_Init");
    List->push_back("");

    /* Postinitialization of hardware */
    Main::Info("> Generating boot-time post-initialization stub.");
    Proj_Gen::Func_Head(List, "RVM_Boot_Post_Init",
                        "Initialize other hardware after all initialization took place.",
                        Input, Output, "None.");
    List->push_back("void RVM_Boot_Post_Init(void)");
    List->push_back("{");
    List->push_back("    /* Add code here */");
    List->push_back("}");
    Proj_Gen::Func_Foot(List, "RVM_Boot_Post_Init");
    List->push_back("");

    /* Vector activation filter - generate regardless; won't be called when no VMs exist */
    Main::Info("> Generating vector filter stub.");
    Input.push_back("rvm_ptr_t Vct_Num - The number of the activated vector.");
    Proj_Gen::Func_Head(List, "RVM_Vct_Filter",
                        "Filter all forwarded vector activations before they reach VMs.",
                        Input, Output,
                        "rvm_ret_t - If forwarding is needed, return RVM_FILTER_PASS (0);\n"
                        "                          if not, return RVM_FILTER_DROP (-1).");
    List->push_back("rvm_ret_t RVM_Vct_Filter(rvm_ptr_t Vct_Num)");
    List->push_back("{");
    List->push_back("    /* Add code here */");
    List->push_back("    return RVM_FILTER_PASS;");
    List->push_back("}");
    Proj_Gen::Func_Foot(List, "RVM_Vct_Filter");
    List->push_back("");

    /* Event activation filter - generate regardless; won't be called when no VMs exist */
    Main::Info("> Generating event filter stub.");
    Input.clear();
    Input.push_back("rvm_ptr_t Evt_Num - The number of the activated event.");
    Proj_Gen::Func_Head(List, "RVM_Evt_Filter",
                        "Filter all forwarded event activations before they reach VMs.",
                        Input, Output,
                        "rvm_ret_t - If forwarding is needed, return RVM_FILTER_PASS (0);\n"
                        "                          if not, return RVM_FILTER_DROP (-1).");
    List->push_back("rvm_ret_t RVM_Evt_Filter(rvm_ptr_t Evt_Num)");
    List->push_back("{");
    List->push_back("    /* Add code here */");
    List->push_back("    return RVM_FILTER_PASS;");
    List->push_back("}");
    Proj_Gen::Func_Foot(List, "RVM_Evt_Filter");
    List->push_back("");
    Proj_Gen::Src_Foot(List);

    /* Generate rme_init.c */
    Main::Info("> Writing file.");
    Proj_Gen::Line_Write(List, Monitor->Hook_Source_Output+"rvm_hook.c");
}
/* End Function:Proj_Gen::Monitor_Hook_Src ***********************************/

/* Function:Proj_Gen::Monitor_Linker ******************************************
Description : Create the linker script file for monitor.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
void Proj_Gen::Monitor_Linker(void)
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
    Proj_Gen::Line_Write(List, Monitor->Linker_Output+Monitor->Linker_Filename);
}
/* End Function:Proj_Gen::Monitor_Linker *************************************/

/* Function:Proj_Gen::Monitor_Proj ********************************************
Description : Create the project file for monitor.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
void Proj_Gen::Monitor_Proj(void)
{
    class Monitor* Monitor;
    class Build_Gen* Build;
    class Tool_Gen* Tool;
    std::unique_ptr<std::vector<std::string>> List;
    std::vector<std::string> Include;
    std::vector<std::string> Source;
    std::vector<std::string> Library;
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
    Include.push_back(Main::Monitor_Root+"Include/");
    Include.push_back(Monitor->Config_Header_Output);
    Include.push_back(Monitor->Boot_Header_Output);
    Proj_Gen::Path_Conv(Monitor->Project_Output, Include);

    for(const std::string& Path:Include)
        Main::Info(std::string("> > ")+Path);

    /* Extract the source paths */
    Main::Info("> Generating project source paths:");
    Library.push_back(Main::Monitor_Root+"Source/Monitor/rvm_monitor.c");
    Library.push_back(Main::Monitor_Root+"Source/Syslib/rvm_syslib.c");
    Library.push_back(Main::Monitor_Root+"Source/Platform/"+this->Plat->Name+"/rvm_platform_"+this->Plat->Name_Lower+".c");
    Library.push_back(Main::Monitor_Root+"Source/Platform/"+this->Plat->Name+
                      "/rvm_platform_"+this->Plat->Name_Lower+"_"+Tool->Name_Lower+Tool->Suffix(TOOL_ASSEMBLER));

    /* Hooks */
    Source.push_back(Monitor->Boot_Source_Output+"rvm_boot.c");
    Source.push_back(Monitor->Hook_Source_Output+"rvm_hook.c");

    Proj_Gen::Path_Conv(Monitor->Project_Output, Source);
    Proj_Gen::Path_Conv(Monitor->Project_Output, Library);
    for(const std::string& Path:Source)
        Main::Info(std::string("> > ")+Path);
    for(const std::string& Path:Library)
        Main::Info(std::string("> > ")+Path);

    /* Extract the linker paths */
    Main::Info("> Generating project linker paths:");

    Linker.push_back(Monitor->Linker_Output+"rvm_platform_"+this->Plat->Chip->Name_Lower+Tool->Suffix(TOOL_LINKER));
    Proj_Gen::Path_Conv(Monitor->Project_Output, Linker);

    for(const std::string& Path:Linker)
        Main::Info(std::string("> > ")+Path);

    Build->Monitor_Proj(List, Include, Source, Library, Linker);
    Proj_Gen::Line_Write(List, Monitor->Project_Output+Monitor->Project_Filename);
}
/* End Function:Proj_Gen::Monitor_Proj ***************************************/

/* Function:Proj_Gen::Process_Inc *********************************************
Description : Write the include files for process files.
Input       : std::unique_ptr<std::vector<std::string>>& List - The input file.
              class Process* Prc - The process.
Output      : std::unique_ptr<std::vector<std::string>>& List - The appended file.
Return      : None.
******************************************************************************/
void Proj_Gen::Process_Inc(std::unique_ptr<std::vector<std::string>>& List,
                           class Process* Prc)
{
    List->push_back("/* Include *******************************************************************/");
    List->push_back("#include \"rvm.h\"");
    /* Virtual machines use their own benchmark headers that does the correct inclusion */
    if((Prc->Type==PROCESS_NATIVE)&&(Main::Benchmark!=0))
    {
        List->push_back("");
        List->push_back("#define RVM_TEST_NATIVE_INC");
        List->push_back("#include \"rvm_test.h\"");
        List->push_back("#undef RVM_TEST_NATIVE_INC");
        List->push_back("");
        List->push_back("#define RVM_BENCHMARK_NATIVE_INC");
        List->push_back("#include \"Test/rvm_benchmark.h\"");
        List->push_back("#undef RVM_BENCHMARK_NATIVE_INC");
    }
    List->push_back("/* End Include ***************************************************************/");
}
/* End Function:Proj_Gen::Process_Inc ****************************************/

/* Function:Proj_Gen::Process_Main_Hdr ****************************************
Description : Create the main header for process.
Input       : class Process* Prc - The process to generate for.
Output      : None.
Return      : None.
******************************************************************************/
void Proj_Gen::Process_Main_Hdr(class Process* Prc)
{
    std::string Filename;
    const class Virtual* Virt;
    std::vector<std::string> Input;
    std::vector<std::string> Output;
    std::unique_ptr<std::vector<std::string>> List;

    List=std::make_unique<std::vector<std::string>>();

    /* File header */
    Main::Info("> Generating main header.");
    Proj_Gen::Src_Head(List, std::string("prc_")+Prc->Name_Lower+".h", "The user kernel object header.");
    List->push_back("");

    /* Definitions */
    List->push_back("/* Define ********************************************************************/");
    List->push_back(std::string("#ifndef __PRC_")+Prc->Name_Upper+"__");
    List->push_back(std::string("#define __PRC_")+Prc->Name_Upper+"__");
    List->push_back("");

    /* The capability table info */
    List->push_back("/* Process capability table frontier & size */");
    Proj_Gen::Macro_Int(List, "CPT_SIZE", Prc->Captbl->Size, MACRO_ADD);
    Proj_Gen::Macro_Int(List, "CPT_FRONT", Prc->Captbl->Front, MACRO_ADD);
    List->push_back("");

    /* Ports */
    List->push_back("/* Ports */");
    for(const std::unique_ptr<class Port>& Port: Prc->Port)
        Proj_Gen::Macro_Int(List, Port->Macro_Local, Port->Cid_Local, MACRO_ADD);
    List->push_back("");
    /* Receive endpoint */
    List->push_back("/* Receive endpoints */");
    for(const std::unique_ptr<class Receive>& Rcv: Prc->Receive)
        Proj_Gen::Macro_Int(List, Rcv->Macro_Local, Rcv->Cid_Local, MACRO_ADD);
    List->push_back("");
    /* Send endpoint */
    List->push_back("/* Send endpoints */");
    for(const std::unique_ptr<class Send>& Send: Prc->Send)
        Proj_Gen::Macro_Int(List, Send->Macro_Local, Send->Cid_Local, MACRO_ADD);
    List->push_back("");
    /* Vector endpoint */
    List->push_back("/* Vector endpoints */");
    for(const std::unique_ptr<class Vect_Info>& Vct: Prc->Vector)
        Proj_Gen::Macro_Int(List, Vct->Macro_Local, Vct->Cid_Local, MACRO_ADD);
    List->push_back("");
    /* Kernel function */
    List->push_back("/* Kernel functions */");
    for(const std::unique_ptr<class Kfunc>& Kfn: Prc->Kfunc)
        Proj_Gen::Macro_Int(List, Kfn->Macro_Local, Kfn->Cid_Local, MACRO_ADD);
    List->push_back("");

    /* Memory block */
    List->push_back("/* Code memory blocks */");
    for(const class Mem_Info* Mem: Prc->Memory_Code)
        Proj_Gen::Macro_Mem("", List, Mem);
    List->push_back("");
    List->push_back("/* Data memory blocks */");
    for(const class Mem_Info* Mem: Prc->Memory_Data)
        Proj_Gen::Macro_Mem("", List, Mem);
    List->push_back("");
    List->push_back("/* Device memory blocks */");
    for(const class Mem_Info* Mem: Prc->Memory_Device)
        Proj_Gen::Macro_Mem("", List, Mem);
    List->push_back("");

    /* Whether using raw page table or not */
    List->push_back("/* Page table settings */");
    Proj_Gen::Macro_Int(List, "RVM_PGT_RAW_ENABLE", this->Plat->Proj->Pgtbl_Raw_Enable, MACRO_ADD);

    /* The total priority numbers */
    List->push_back("/* Total priority number */");
    Proj_Gen::Macro_Int(List, "RVM_PREEMPT_PRIO_NUM", this->Plat->Proj->Kernel->Kern_Prio, MACRO_ADD);
    /* The VM priority numbers */
    List->push_back("/* Total VM priority number */");
    Proj_Gen::Macro_Int(List, "RVM_PREEMPT_VPRIO_NUM", this->Plat->Proj->Monitor->Virt_Prio, MACRO_ADD);
    /* The kernel memory slot order */
    List->push_back("/* The kernel memory allocation granularity order */");
    Proj_Gen::Macro_Int(List, "RVM_KOM_SLOT_ORDER", this->Plat->Proj->Kernel->Kom_Order, MACRO_ADD);
    List->push_back("");

    /* If this is a virtual machine, define the following specific macros */
    if(Prc->Type==PROCESS_VIRTUAL)
    {
        Virt=static_cast<const class Virtual*>(Prc);
        List->push_back("/* Virtual machine library enable */");
        Proj_Gen::Macro_Int(List, "RVM_VIRT_LIB_ENABLE", 1, MACRO_ADD);
        List->push_back("/* Virtual vector total number */");
        Proj_Gen::Macro_Int(List, "RVM_VIRT_VCT_NUM", Virt->Vector_Num, MACRO_ADD);
        List->push_back("/* State block base address & size */");
        Proj_Gen::Macro_Hex(List, "RVM_VIRT_STATE_BASE", Virt->State_Base, MACRO_ADD);
        Proj_Gen::Macro_Hex(List, "RVM_VIRT_STATE_SIZE", Virt->State_Size, MACRO_ADD);
        List->push_back("/* Virtual register base address & size */");
        Proj_Gen::Macro_Hex(List, "RVM_VIRT_REG_BASE", Virt->Reg_Base, MACRO_ADD);
        Proj_Gen::Macro_Hex(List, "RVM_VIRT_REG_SIZE", Virt->Reg_Size, MACRO_ADD);
    }
    else
    {
        List->push_back("/* Virtual machine library disable */");
        Proj_Gen::Macro_Int(List, "RVM_VIRT_LIB_ENABLE", 0, MACRO_ADD);
    }
    List->push_back("");

    /* Assert & debugging */
    List->push_back("/* Debugging setting */");
    Proj_Gen::Macro_Int(List, "RVM_ASSERT_ENABLE", this->Plat->Proj->Assert_Enable, MACRO_ADD);
    Proj_Gen::Macro_Int(List, "RVM_DBGLOG_ENABLE", this->Plat->Proj->Debug_Log_Enable, MACRO_ADD);
    List->push_back("");

    /* Set coprocessor macros - set all of them to 0 first, then modify from there */
    List->push_back("/* Coprocessor option */");
    Proj_Gen::Macro_Int(List, "RVM_COP_NUM", Prc->Coprocessor.size(), MACRO_REPLACE);
    for(const std::string& Cop:this->Plat->Plat->Coprocessor)
        Proj_Gen::Macro_Int(List, "RVM_"+this->Plat->Name_Upper+"_COP_"+Cop, 0, MACRO_REPLACE);
    for(const std::string& Cop:Prc->Coprocessor)
        Proj_Gen::Macro_Int(List, "RVM_"+this->Plat->Name_Upper+"_COP_"+Cop, 1, MACRO_REPLACE);

    /* Generate platform specific macros */
    this->Plat->Process_Main_Hdr(List, Prc);

    List->push_back(std::string("#endif /* __PRC_")+Prc->Name_Upper+"__ */");
    List->push_back("/* End Define ****************************************************************/");
    List->push_back("");

    Proj_Gen::Src_Foot(List);
    List->push_back("");

    /* Generate proc_xxx.h */
    Proj_Gen::Line_Write(List, Prc->Main_Header_Output+"prc_"+Prc->Name_Lower+".h");

    /* Generate rvm_platform.h */
    Main::Info("> Generating 'rvm_platform.h'.");
    List->clear();
    Proj_Gen::Src_Head(List, "rvm_platform.h", "The platform selection header.");
    List->push_back("");
    List->push_back("/* Platform Include **********************************************************/");
    List->push_back(std::string("#include \"Platform/")+this->Plat->Name+"/rvm_platform_"+this->Plat->Name_Lower+".h\"");
    List->push_back("/* End Platform Include ******************************************************/");
    List->push_back("");
    Proj_Gen::Src_Foot(List);
    Proj_Gen::Line_Write(List, Prc->Main_Header_Output+"rvm_platform.h");

    /* Generate rvm_platform_xxx_conf.h */
    Filename=std::string("rvm_platform_")+this->Plat->Name_Lower+"_conf.h";
    Main::Info(std::string("> Generating '")+Filename+"'.");
    List->clear();
    Proj_Gen::Src_Head(List, Filename, "The process selection header.");
    List->push_back("");
    List->push_back("/* Platform Include **********************************************************/");
    List->push_back(std::string("#include \"prc_")+Prc->Name_Lower+".h\"");
    List->push_back("/* End Platform Include ******************************************************/");
    List->push_back("");
    Proj_Gen::Src_Foot(List);
    Proj_Gen::Line_Write(List, Prc->Main_Header_Output+"rvm_platform_"+this->Plat->Name_Lower+"_conf.h");

    /* If we're running benchmark, need to generate the test header as well */
    if(Main::Benchmark!=0)
    {
        Main::Info("> Generating test selection header.");
        List->clear();
        Proj_Gen::Src_Head(List, "rvm_test.h", "The RVM guest library test header.");
        List->push_back("");
        List->push_back("/* Include *******************************************************************/");
        List->push_back(std::string("#include \"Test/Chip/rvm_test_")+this->Plat->Chip->Name_Lower+".h\"");
        List->push_back("/* End Include ***************************************************************/");
        List->push_back("");
        Proj_Gen::Src_Foot(List);

        Proj_Gen::Line_Write(List, Prc->Main_Header_Output+"rvm_test.h");
    }
}
/* End Function:Proj_Gen::Process_Main_Hdr ***********************************/

/* Function:Proj_Gen::Process_Desc_Src ****************************************
Description : Create the descriptor source for process.
Input       : class Process* Prc - The process to generate for.
Output      : None.
Return      : None.
******************************************************************************/
void Proj_Gen::Process_Desc_Src(class Process* Prc)
{
    std::string Filename;
    std::vector<std::string> Input;
    std::vector<std::string> Output;
    std::unique_ptr<std::vector<std::string>> List;

    List=std::make_unique<std::vector<std::string>>();

    Main::Info("> Generating descriptor source.");
    Filename=std::string("prc_")+Prc->Name_Lower+"_desc.c";
    Proj_Gen::Src_Head(List, Filename, "The process descriptor header file - do not edit!\n"
    								   "              When using LTO, make sure this file is exempt from the LTO option,\n"
    								   "              so that it be firmly linked to the head of the image!");
    List->push_back("");

    /* Include */
    this->Process_Inc(List, Prc);
    List->push_back("");

    /* Public prototypes */
    List->push_back("/* Public Function ***********************************************************/");
    if(Prc->Type==PROCESS_VIRTUAL)
        List->push_back("RVM_EXTERN rvm_ret_t Thd_Vct(rvm_ret_t Param);");
    else
    {
		for(const std::unique_ptr<class Thread>& Thd:Prc->Thread)
		{
		    /* The first one is entry, pass it */
            if(Thd.get()==Prc->Thread[0].get())
                continue;
			List->push_back(std::string("RVM_EXTERN rvm_ret_t Thd_")+Thd->Name+"(rvm_ret_t Param);");
		}
		for(const std::unique_ptr<class Invocation>& Inv:Prc->Invocation)
			List->push_back(std::string("RVM_EXTERN rvm_ret_t Inv_")+Inv->Name+"(rvm_ret_t Param);");
    }
    List->push_back("/* End Public Function *******************************************************/");
    List->push_back("");

    /* Global variables */
    List->push_back("/* Public Variable ***********************************************************/");
    List->push_back(std::string("const rvm_ptr_t RVM_Desc[")+std::to_string(Prc->Desc_Front)+"]=");
    List->push_back("{");
    if(Prc->Type==PROCESS_NATIVE)
    {
        List->push_back("RVM_MAGIC_NATIVE,");
        List->push_back(std::string("    0x")+Main::Hex(Prc->Desc_Front-2)+"U,");
        /* For native processes, the first (higher-priority) thread's entry is always the main entry
         * point that immediately follows the header, and aligned to a (at least) 16-byte boundary.
         * This is due to some proprietary toolchains requiring an aligned address - e.g. ARMCC. */
        for(const std::unique_ptr<class Thread>& Thd:Prc->Thread)
        {
            if(Thd.get()==Prc->Thread[0].get())
                List->push_back("    (rvm_ptr_t)__RVM_Entry,");
            else
                List->push_back(std::string("    (rvm_ptr_t)Thd_")+Thd->Name+",");
        }
    }
    else
    {
        List->push_back("RVM_MAGIC_VIRTUAL,");
        List->push_back(std::string("    0x")+Main::Hex(Prc->Desc_Front-2)+"U,");
        /* For VMs, the second (lower-priority) thread's entry is always the main entry point.
         * Because the second thread that runs the user code is low-priority, and the VM may
         * have already defined a main function. We want to be as less intrusive as possible. */
        for(const std::unique_ptr<class Thread>& Thd:Prc->Thread)
        {
            if(Thd.get()==Prc->Thread[1].get())
            {
                ASSERT(Thd->Name=="Usr");
                List->push_back("    (rvm_ptr_t)__RVM_Entry,");
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
    List->push_back("    (rvm_ptr_t)__RVM_Stub,");
    List->push_back("};");
    List->push_back("/* End Public Variable *******************************************************/");
    List->push_back("");
    Proj_Gen::Src_Foot(List);

    /* Generate proc_xxx_header.c */
    Proj_Gen::Line_Write(List, Prc->Main_Source_Output+Filename);
}
/* End Function:Proj_Gen::Process_Desc_Src ***********************************/

/* Function:Proj_Gen::Process_Main_Src ****************************************
Description : Create the main sources for process. Each invocation and thread will
              have its own file, so there is least interference between them.
Input       : class Process* Prc - The process to generate for.
Output      : None.
Return      : None.
******************************************************************************/
void Proj_Gen::Process_Main_Src(class Process* Prc)
{
    std::string Filename;
    std::vector<std::string> Input;
    std::vector<std::string> Output;
    std::unique_ptr<std::vector<std::string>> List;

    List=std::make_unique<std::vector<std::string>>();

    Main::Info("> Generating main source.");
    Filename=std::string("prc_")+Prc->Name_Lower+".c";
    Proj_Gen::Src_Head(List, Filename, "The main user stub file.");
    List->push_back("");

    /* Include **/
    this->Process_Inc(List, Prc);
    List->push_back("");

    /* Global variable - The only one being the process header reference */
    List->push_back("/* Public Variable ***********************************************************/");
    List->push_back(std::string("RVM_EXTERN const rvm_ptr_t RVM_Desc[")+std::to_string(Prc->Desc_Front)+"];");
    List->push_back("/* End Public Variable *******************************************************/");
    List->push_back("");

    /* Private prototypes */
    List->push_back("/* Private Function **********************************************************/");
    if(Prc->Type==PROCESS_VIRTUAL)
        List->push_back("rvm_ret_t Thd_Vct(rvm_ret_t Param);");
    else
    {
        for(const std::unique_ptr<class Thread>& Thd:Prc->Thread)
            List->push_back(std::string("RVM_EXTERN rvm_ret_t Thd_")+Thd->Name+"(rvm_ret_t Param);");
        for(const std::unique_ptr<class Invocation>& Inv:Prc->Invocation)
            List->push_back(std::string("RVM_EXTERN rvm_ret_t Inv_")+Inv->Name+"(rvm_ret_t Param);");
    }
    List->push_back("RVM_EXTERN void _RVM_Stub(void);");
    List->push_back("void RVM_Putchar(char Char);");
    List->push_back("/* End Private Function ******************************************************/");
    List->push_back("");

    /* If this is a virtual machine, create the two essential threads */
    if(Prc->Type==PROCESS_VIRTUAL)
    {
        Input.push_back("rvm_ret_t Param - The parameter supplied by the OS.");
        /* The vector handler thread */
        Proj_Gen::Func_Head(List, "Thd_Vct", "The function body for vector handler.",
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
        Proj_Gen::Func_Foot(List, "Thd_Vct");
        List->push_back("");
        Input.clear();
    }
    /* If not, create the main function */
    else
    {
        Proj_Gen::Func_Head(List, "main", "The function body for thread.", Input, Output, "rvm_ret_t - Should never return.");
        List->push_back("rvm_ret_t main(void)");
        List->push_back("{");
        List->push_back("    /* Check header validity */");
        List->push_back("    RVM_ASSERT(RVM_Desc[0]==RVM_MAGIC_NATIVE);");
        List->push_back(std::string("    RVM_ASSERT(RVM_Desc[1]==")+std::to_string(Prc->Desc_Front-2)+"U);");
        List->push_back("");
        List->push_back("    /* Call the first (highest-priority) thread */");
        List->push_back(std::string("    Thd_")+Prc->Thread[0]->Name+"("+Main::Hex(Prc->Thread[0]->Parameter)+"U);");
        List->push_back("}");
        Proj_Gen::Func_Foot(List, "main");
        List->push_back("");
    }

    /* The RVM library putchar function - print character to console */
    Input.push_back("char Char - The character to print to console.");
    Proj_Gen::Func_Head(List, "RVM_Putchar", "The character printing function for debugging.", Input, Output, "None.");
    List->push_back("#if(RVM_DBGLOG_ENABLE!=0U)");
    List->push_back("void RVM_Putchar(char Char)");
    List->push_back("{");
    /* Virtual machines use hypercall to print */
    if(Prc->Type==PROCESS_VIRTUAL)
    	List->push_back("    RVM_Hyp_Putchar(Char);");
    /* Native processes have putchar for benchmark only */
    else if(Main::Benchmark!=0)
    {
        List->push_back("#define RVM_TEST_NATIVE_PUTCHAR");
        List->push_back("#include \"rvm_test.h\"");
        List->push_back("#undef RVM_TEST_NATIVE_PUTCHAR");
    }
    else
        List->push_back("    /* Add your own code */");
    List->push_back("}");
    List->push_back("#endif");
    Proj_Gen::Func_Foot(List, "RVM_Putchar");
    List->push_back("");
    Proj_Gen::Src_Foot(List);

    /* Generate proc_xxx.c */
    Proj_Gen::Line_Write(List, Prc->Main_Source_Output+Filename);
}
/* End Function:Proj_Gen::Process_Main_Src ***********************************/

/* Function:Proj_Gen::Process_Nat_Src *****************************************
Description : Create the stubs for process. Each invocation and thread will
              have its own file, so there is least interference between them.
Input       : class Native* Native - The native process to generate for.
Output      : None.
Return      : None.
******************************************************************************/
void Proj_Gen::Process_Nat_Src(class Native* Nat)
{
    std::string Filename;
    std::vector<std::string> Input;
    std::vector<std::string> Output;
    std::unique_ptr<std::vector<std::string>> List;

    List=std::make_unique<std::vector<std::string>>();

    /* For each thread, create a single file */
    Input.push_back("rvm_ret_t Param - The parameter supplied by the OS.");
    for(const std::unique_ptr<class Thread>& Thd:Nat->Thread)
    {
        Filename=std::string("prc_")+Nat->Name_Lower+"_thd_"+Thd->Name_Lower+".c";
        if(std::filesystem::exists(Nat->Entry_Source_Output+Filename)==true)
        {
            /* See if we'll use forced regenerate */
            if(Nat->Entry_Source_Overwrite==0)
            {
                Main::Info(std::string("> File '")+Filename+"' exists, skipping generation.");
                continue;
            }
        }
        Main::Info(std::string("> Generating source for thread '")+Thd->Name+"'.");
        List->clear();
        Proj_Gen::Src_Head(List, Filename, std::string("The user stub file for thread '")+Thd->Name+"'.");
        List->push_back("");
        /* Include **/
        this->Process_Inc(List, Nat);
        List->push_back("");
        /* Private prototypes */
        List->push_back("/* Private Function **********************************************************/");
        List->push_back(std::string("void Thd_")+Thd->Name+"(rvm_ret_t Param);");
        List->push_back("/* End Private Function ******************************************************/");
        List->push_back("");
        /* Thread functions themselves */
        Proj_Gen::Func_Head(List, std::string("Thd_")+Thd->Name,
                            "The function body for thread.", Input, Output, "None.");
        List->push_back(std::string("void Thd_")+Thd->Name+"(rvm_ret_t Param)");
        List->push_back("{");
        if(Main::Benchmark!=0)
        {
            List->push_back("#define RVM_BENCHMARK_PRC_"+Nat->Name_Upper+"_THD_"+Thd->Name_Upper);
            List->push_back("#include \"Test/rvm_benchmark.h\"");
            List->push_back("#undef RVM_BENCHMARK_PRC_"+Nat->Name_Upper+"_THD_"+Thd->Name_Upper);
        }
        else
        {
            List->push_back("    /* Add your code here */");
            List->push_back("    while(1);");
        }
        List->push_back("}");
        Proj_Gen::Func_Foot(List, std::string("Thd_")+Thd->Name);
        List->push_back("");
        this->Src_Foot(List);
        List->push_back("");
        Proj_Gen::Line_Write(List, Nat->Entry_Source_Output+Filename);
    }
    Input.clear();

    /* For each invocation, create a single file */
    Input.push_back("rvm_ret_t Param - The parameter supplied by the caller.");
    for(const std::unique_ptr<class Invocation>& Inv:Nat->Invocation)
    {
        Filename=std::string("prc_")+Nat->Name_Lower+"_inv_"+Inv->Name_Lower+".c";
        if(std::filesystem::exists(Nat->Entry_Source_Output+Filename)==true)
        {
            /* See if we'll use forced regenerate */
            if(Nat->Entry_Source_Overwrite==0)
            {
                Main::Info(std::string("> File '")+Filename+"' exists, skipping generation.");
                continue;
            }
        }
        Main::Info(std::string("> Generating source for invocation '")+Inv->Name+"'.");
        List->clear();
        Proj_Gen::Src_Head(List, Filename, std::string("The user stub file for thread '")+Inv->Name+"'.");
        List->push_back("");
        /* Include **/
        this->Process_Inc(List, Nat);
        List->push_back("");
        /* Private prototypes */
        List->push_back("/* Private Function **********************************************************/");
        List->push_back(std::string("void Inv_")+Inv->Name+"(rvm_ret_t Param);");
        List->push_back("/* End Private Function ******************************************************/");
        List->push_back("");
        /* Thread functions themselves */
        Proj_Gen::Func_Head(List, std::string("Inv_")+Inv->Name,
                            "The function body for invocation.", Input, Output, "None.");
        List->push_back(std::string("void Inv_")+Inv->Name+"(rvm_ret_t Param)");
        List->push_back("{");
        if(Main::Benchmark!=0)
        {
            List->push_back("#define RVM_BENCHMARK_PRC_"+Nat->Name_Upper+"_INV_"+Inv->Name_Upper);
            List->push_back("#include \"Test/rvm_benchmark.h\"");
            List->push_back("#undef RVM_BENCHMARK_PRC_"+Nat->Name_Upper+"_INV_"+Inv->Name_Upper);
        }
        else
        {
            List->push_back("    /* Add your code here */");
            List->push_back("    while(1);");
        }
        List->push_back("}");
        Proj_Gen::Func_Foot(List, std::string("Inv_")+Inv->Name);
        List->push_back("");
        this->Src_Foot(List);

        Proj_Gen::Line_Write(List, Nat->Entry_Source_Output+Filename);
    }
    Input.clear();
}
/* End Function:Proj_Gen::Process_Nat_Src ************************************/

/* Function:Proj_Gen::Process_Virt_Hdr ****************************************
Description : Create the configuration header for VMs.
Input       : class Virtual* Virt - The process to generate for.
Output      : None.
Return      : None.
******************************************************************************/
void Proj_Gen::Process_Virt_Hdr(class Virtual* Virt)
{
    /* Just pass this work down so that the underlying layers will handle it */
    this->Guest_Map[Virt->Guest_Type]->Process_Virt_Hdr(Virt);
}
/* End Function:Proj_Gen::Process_Virt_Hdr ***********************************/

/* Function:Proj_Gen::Process_Virt_Src ****************************************
Description : Create the configuration header for VMs.
Input       : class Virtual* Virt - The process to generate for.
Output      : None.
Return      : None.
******************************************************************************/
void Proj_Gen::Process_Virt_Src(class Virtual* Virt)
{
    /* Just pass this work down so that the underlying layers will handle it */
    this->Guest_Map[Virt->Guest_Type]->Process_Virt_Src(Virt, this->Tool_Map[Virt->Toolchain]);
}
/* End Function:Proj_Gen::Process_Virt_Src ***********************************/

/* Function:Proj_Gen::Process_Linker ******************************************
Description : Create the linker script file for process.
Input       : class Process* Prc - The process to generate for.
Output      : None.
Return      : None.
******************************************************************************/
void Proj_Gen::Process_Linker(class Process* Prc)
{
    class Tool_Gen* Tool;
    std::unique_ptr<std::vector<std::string>> List;

    List=std::make_unique<std::vector<std::string>>();
    Tool=this->Tool_Map[Prc->Toolchain];

    Main::Info("> Generating linker script.");
    Prc->Linker_Filename=std::string("prc_")+Prc->Name_Lower+"_"+this->Plat->Chip->Name_Lower+Tool->Suffix(TOOL_LINKER);
    Tool->Process_Linker(List, Prc);
    Proj_Gen::Line_Write(List, Prc->Linker_Output+Prc->Linker_Filename);
}
/* End Function:Proj_Gen::Process_Linker *************************************/

/* Function:Proj_Gen::Process_Proj ********************************************
Description : Create the project file for process.
Input       : class Process* Prc - The process to generate for.
Output      : None.
Return      : None.
******************************************************************************/
void Proj_Gen::Process_Proj(class Process* Prc)
{
    class Native* Nat;
    class Virtual* Virt;
    class Build_Gen* Build;
    class Tool_Gen* Tool;
    std::unique_ptr<std::vector<std::string>> List;
    std::vector<std::string> Include;
    std::vector<std::string> Source;
    std::vector<std::string> Library;
    std::vector<std::string> Linker;

    List=std::make_unique<std::vector<std::string>>();
    Nat=static_cast<class Native*>(Prc);
    Virt=static_cast<class Virtual*>(Prc);
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
    Include.push_back(Main::Monitor_Root+"Include/");
    Include.push_back(Prc->Project_Output);
    Include.push_back(Prc->Main_Header_Output);
    /* For virtual machines, add all the VM files as well */
    if(Prc->Type==PROCESS_VIRTUAL)
        Include.insert(Include.end(), Virt->Virtual_Include.begin(), Virt->Virtual_Include.end());
    Proj_Gen::Path_Conv(Prc->Project_Output, Include);

    for(const std::string& Path:Include)
        Main::Info(std::string("> > ")+Path);

    /* Extract the source paths */
    Main::Info("> Generating project source paths:");
    Library.push_back(Main::Monitor_Root+"Source/Virtlib/rvm_virtlib.c");
    Library.push_back(Main::Monitor_Root+"Source/Syslib/rvm_syslib.c");
    Library.push_back(Main::Monitor_Root+"Source/Platform/"+this->Plat->Name+"/rvm_platform_"+this->Plat->Name_Lower+".c");
    Library.push_back(Main::Monitor_Root+"Source/Platform/"+this->Plat->Name+
                      "/rvm_platform_"+this->Plat->Name_Lower+"_"+Tool->Name_Lower+Tool->Suffix(TOOL_ASSEMBLER));

    /* Hooks */
    Source.push_back(Prc->Main_Source_Output+"prc_"+Prc->Name_Lower+".c");
    Source.push_back(Prc->Main_Source_Output+"prc_"+Prc->Name_Lower+"_desc.c");
    /* For native processes, add all the stub files */
    if(Prc->Type==PROCESS_NATIVE)
    {
        for(const std::unique_ptr<class Thread>& Thd:Prc->Thread)
            Source.push_back(Nat->Entry_Source_Output+"prc_"+Prc->Name_Lower+"_thd_"+Thd->Name_Lower+".c");
        for(const std::unique_ptr<class Invocation>& Inv:Prc->Invocation)
            Source.push_back(Nat->Entry_Source_Output+"prc_"+Prc->Name_Lower+"_inv_"+Inv->Name_Lower+".c");
    }
    /* For virtual machines, add all the VM files as well */
    else
    {
        Source.insert(Source.end(), Virt->Virtual_Source.begin(), Virt->Virtual_Source.end());
        Library.insert(Library.end(), Virt->Virtual_Library.begin(), Virt->Virtual_Library.end());
    }

    Proj_Gen::Path_Conv(Prc->Project_Output, Source);
    Proj_Gen::Path_Conv(Prc->Project_Output, Library);
    for(const std::string& Path:Source)
        Main::Info(std::string("> > ")+Path);
    for(const std::string& Path:Library)
        Main::Info(std::string("> > ")+Path);

    /* Extract the linker paths */
    Main::Info("> Generating project linker paths:");

    Linker.push_back(Prc->Linker_Output+"prc_"+Prc->Name_Lower+"_"+this->Plat->Chip->Name_Lower+Tool->Suffix(TOOL_LINKER));
    Proj_Gen::Path_Conv(Prc->Project_Output, Linker);

    for(const std::string& Path:Linker)
        Main::Info(std::string("> > ")+Path);

    Build->Process_Proj(List, Include, Source, Library, Linker, Prc);
    Proj_Gen::Line_Write(List, Prc->Project_Output+Prc->Project_Filename);
}
/* End Function:Proj_Gen::Process_Proj ***************************************/

/* Function:Proj_Gen::Workspace_Proj ******************************************
Description : Create the workspace file for all projects. This generation is only
              possible when (1) all workspace projects use the same build system,
              and (2) the build system supports a workspace project.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
void Proj_Gen::Workspace_Proj(void)
{
    class Build_Gen* Build;
    class Proj_Info* Proj;
    std::unique_ptr<std::vector<std::string>> List;
    std::vector<std::string> Kernel;
    std::vector<std::string> Monitor;
    std::vector<std::string> Native;
    std::vector<std::string> Virtual;

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
    if(std::filesystem::exists(Main::Workspace_Output+Proj->Workspace_Filename)==true)
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
    Kernel.push_back(Proj->Kernel->Project_Output+Proj->Kernel->Project_Filename);
    Monitor.push_back(Proj->Monitor->Project_Output+Proj->Monitor->Project_Filename);
    for(class Native* Nat:Proj->Native)
        Native.push_back(Nat->Project_Output+Nat->Project_Filename);
    for(class Virtual* Virt:Proj->Virtual)
        Virtual.push_back(Virt->Project_Output+Virt->Project_Filename);

    Proj_Gen::Path_Conv(Main::Workspace_Output, Kernel);
    Proj_Gen::Path_Conv(Main::Workspace_Output, Monitor);
    Proj_Gen::Path_Conv(Main::Workspace_Output, Native);
    Proj_Gen::Path_Conv(Main::Workspace_Output, Virtual);

    for(const std::string& Path:Kernel)
        Main::Info(std::string("> > ")+Path);
    for(const std::string& Path:Monitor)
        Main::Info(std::string("> > ")+Path);
    for(const std::string& Path:Native)
        Main::Info(std::string("> > ")+Path);
    for(const std::string& Path:Virtual)
        Main::Info(std::string("> > ")+Path);

    Build->Workspace_Proj(List, Kernel, Monitor, Native, Virtual);
    Proj_Gen::Line_Write(List, Main::Workspace_Output+Proj->Workspace_Filename);
}
/* End Function:Proj_Gen::Workspace_Proj *************************************/
}
/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
