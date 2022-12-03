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
#include "string"
#include "memory"
#include "vector"
#include "stdexcept"
#include "algorithm"

#define __HDR_DEFS__
#include "rvm_gen.hpp"
#include "Gen_Tool/gen_tool.hpp"
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
Output      : std::unique_ptr<std::vector<std::string>>& List - The updated file.
Return      : None.
******************************************************************************/
void Gen_Tool::Macro_String(std::unique_ptr<std::vector<std::string>>& List,
                            const std::string& Macro, const std::string& Value)
{
    ptr_t Pos;
    char Buf[256];
    char Format[32];
    std::string Compare;

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
            sprintf(Format, "#define %%-%llds    %%s", Pos);
            sprintf(Buf, Format, Macro.c_str(), Value.c_str());
            Line=Buf;
            return;
        }
    }

    /* Print to file if this does not yet exist */
    Pos=MACRO_ALIGN;
    sprintf(Format, "#define %%-%llds    %%s", Pos);
    sprintf(Buf, Format, Macro.c_str(), Value.c_str());
    List->push_back(Buf);
}
/* End Function:Gen_Tool::Macro_String ***************************************/

/* Begin Function:Gen_Tool::Macro_Hex *****************************************
Description : See if a macro is defined in the file, and modify it as a hex number.
Input       : std::unique_ptr<std::vector<std::string>>& List - The file contents.
              const std::string& Macro - The macro name.
              ptr_t Value - The new value to assign to that macro.
Output      : std::unique_ptr<std::vector<std::string>>& List - The updated file.
Return      : None.
******************************************************************************/
void Gen_Tool::Macro_Hex(std::unique_ptr<std::vector<std::string>>& List,
                         const std::string& Macro, ptr_t Value)
{
    char Buf[64];

    sprintf(Buf,"0x%llX",Value);
    Gen_Tool::Macro_String(List,Macro,Buf);
}
/* End Function:Gen_Tool::Macro_Hex ******************************************/

/* Begin Function:Gen_Tool::Macro_Int *****************************************
Description : See if a macro is defined in the file, and modify it as a dec number.
Input       : std::unique_ptr<std::vector<std::string>>& List - The file contents.
              const std::string& Macro - The macro name.
              ptr_t Value - The new value to assign to that macro.
Output      : std::unique_ptr<std::vector<std::string>>& List - The updated file.
Return      : None.
******************************************************************************/
void Gen_Tool::Macro_Int(std::unique_ptr<std::vector<std::string>>& List,
                         const std::string& Macro, ptr_t Value)
{
    char Buf[64];

    sprintf(Buf,"%lld",Value);
    Gen_Tool::Macro_String(List,Macro,Buf);
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

/* Begin Function:Gen_Tool::Kernel_Conf_Hdr ***********************************
Description : Create the platform configuration headers for kernel.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
void Gen_Tool::Kernel_Conf_Hdr(void)
{
    class Kernel* Kernel;
    class Monitor* Monitor;
    std::unique_ptr<std::vector<std::string>> List;

    Kernel=this->Plat->Proj->Kernel.get();
    Monitor=this->Plat->Proj->Monitor.get();
    List=std::make_unique<std::vector<std::string>>();

    /* Generate rme_platform.h */
    Gen_Tool::Src_Head(List, "rme_platform.h", "The platform selection header.");
    List->push_back("");
    List->push_back("/* Platform Includes *********************************************************/");
    List->push_back(std::string("#include \"Platform/")+this->Plat->Name+"/rme_platform_"+this->Plat->Lower+".h\"");
    List->push_back("/* End Platform Includes *****************************************************/");
    List->push_back("");
    Gen_Tool::Src_Foot(List);
    Gen_Tool::Line_Write(List, Kernel->Config_Header_Output+"/rme_platform.h");

    /* Generate rme_platform_xxx_conf.h */
    List->clear();
    Gen_Tool::Src_Head(List, std::string("rme_platform_")+this->Plat->Lower+"_conf.h", "The chip selection header.");
    List->push_back("");
    List->push_back("/* Platform Includes *********************************************************/");
    List->push_back(std::string("#include \"rme_platform_")+this->Plat->Chip->Class+".h\"");
    List->push_back("/* End Platform Includes *****************************************************/");
    List->push_back("");
    Gen_Tool::Src_Foot(List);
    Gen_Tool::Line_Write(List, Kernel->Config_Header_Output+"/rme_platform_"+this->Plat->Lower+"_conf.h");

    /* Generate rme_platform_CHIPNAME.h */
    List=Gen_Tool::Line_Read(Kernel->Kernel_Root+
                             "/Include/Platform/"+this->Plat->Name+"/Chip/"+this->Plat->Chip->Class+
                             "/rme_platform_"+this->Plat->Chip->Class+".h");

    /* Replace general parameter macros */
    /* Generator enabled */
    Gen_Tool::Macro_String(List, "RME_RVM_GEN_ENABLE", "RME_TRUE");
    /* The initial capability table size */
    Gen_Tool::Macro_Int(List, "RME_BOOT_CAPTBL_SIZE", Monitor->Captbl_Size);
    /* The virtual memory start address for the kernel objects */
    Gen_Tool::Macro_Hex(List, "RME_KMEM_VA_START", Kernel->Kmem_Base);
    /* The size of the kernel object virtual memory */
    Gen_Tool::Macro_Hex(List, "RME_KMEM_SIZE", Kernel->Kmem_Size);
    /* The virtual memory start address for the virtual machines - always full memory range, just in case */
    Gen_Tool::Macro_Hex(List, "RME_HYP_VA_START", 0x00000000ULL);
    /* The size of the hypervisor reserved virtual memory */
    Gen_Tool::Macro_Hex(List, "RME_HYP_SIZE", 0xFFFFFFFFULL);
    /* Kernel stack base/size */
    Gen_Tool::Macro_Hex(List, "RME_KMEM_STACK_BASE", Kernel->Stack_Base);
    Gen_Tool::Macro_Hex(List, "RME_KMEM_STACK_SIZE", Kernel->Stack_Size);
    /* The maximum number of preemption priority levels in the system.
     * This parameter must be divisible by the word length - 32 is usually sufficient */
    Gen_Tool::Macro_Int(List, "RME_MAX_PREEMPT_PRIO", Kernel->Kern_Prio);
     /* The granularity of kernel memory allocation, in bytes */
    Gen_Tool::Macro_Int(List, "RME_KMEM_SLOT_ORDER", Kernel->Kmem_Order);

    /* Replace platform specific macros */
    this->Plat->Kernel_Conf_Hdr(List);

    /* Chip specific macros - we must be able to find it because we checked before */
    for(std::unique_ptr<class Conf_Info>& Conf:this->Plat->Chip->Config)
        Gen_Tool::Macro_String(List, Conf->Macro, this->Plat->Proj->Chip->Config[Conf->Name]);

    /* Write to kernel configuration file */
    Gen_Tool::Line_Write(List, Kernel->Config_Header_Output+"/rme_platform_"+this->Plat->Chip->Class+".h");
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

}
/* End Function:Gen_Tool::Kernel_Init_Src ************************************/

/* Begin Function:Gen_Tool::Kernel_Handler_Src ********************************
Description : Create the handler source file for kernel.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
void Gen_Tool::Kernel_Handler_Src(void)
{

}
/* End Function:Gen_Tool::Kernel_Handler_Src *********************************/
}
/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
