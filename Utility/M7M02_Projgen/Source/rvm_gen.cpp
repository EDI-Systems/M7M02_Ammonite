/******************************************************************************
Filename    : rvm_gen.cpp
Author      : pry
Date        : 20/04/2019
Licence     : LGPL v3+; see COPYING for details.
Description : The configuration generator for the MCU ports. This does not
              apply to the desktop or mainframe port; it uses its own generator.
              This generator includes 12 big steps, and is considerably complex.
               1. Process the command line arguments and figure out where the project is.
               2. Read the project, platform and chip configuration XMLs. Should
                  we find any parsing errors, we report and error out.
               3. Align memory. For program memory and data memory, rounding their
                  size is allowed; for specifically pointed out memory, rounding
                  their size is not allowed. 
               4. Generate memory map. This places all the memory segments into
                  the memory map, and determines their specific size, etc.
               5. Generate the page tables. This will actually perform the memory mapping.
               6. Allocate local and global linear capability IDs for all kernel objects.
               7. Set up the folder structure of the project so that the port-specific
                  generators can directly use them.
               8. Call the port-level generator to generate the project and port-specific
                  files for the project.
                  1. Detect any errors in the configuration structure. If any is found,
                     error out.
                  2. Allocate the page table contents and allocate capid/macros for them.
                  3. Call the tool-level project generator to generate project files.
                     Should the tool have any project group or workspace creation capability,
                     create the project group or workspace.
                     Memory map and linker file is also generated in this phase. 
                     1. The generator should generate separate projects for the RME.
                     2. Then generates project for RVM. 
                     3. And generates project for all other processes.
               9. Generate the vector creation scripts for RME.
              10. Generate the kernel object creation and delegation scripts for RVM.
                  1. Generate the capability tables.
                  2. Generate the page tables, calls the port-specific generator callback.
                  3. Generate the processes.
                  4. Generate the threads.
                  5. Generate the invocations.
                  6. Generate the receive endpoints.
                  7. Generate the delegation scripts.
              11. Generate stubs for all processes.
              12. Install guest operating system.
              13. Report to the user that the project generation is complete.
******************************************************************************/

/* Includes ******************************************************************/
extern "C"
{
#include "xml.h"

#include "stdio.h"
#include "memory.h"
#include "stdlib.h"
#include "stdarg.h"
#include "string.h"
#include "time.h"
#ifdef _MSC_VER
#include "direct.h"
#else
#include "unistd.h"
#endif
}

#include "set"
#include "map"
#include "list"
#include "string"
#include "memory"
#include "vector"
#include "iostream"
#include "iterator"
#include "stdexcept"
#include "algorithm"
#include "filesystem"

#define __HDR_DEFS__
#include "rvm_gen.hpp"
#include "Proj_Info/proj_info.hpp"
#include "Proj_Info/Process/process.hpp"
#include "Chip_Info/chip_info.hpp"
#include "Plat_Info/plat_info.hpp"
#include "Conf_Info/conf_info.hpp"
#include "Mem_Info/mem_info.hpp"
#include "Gen_Tool/gen_tool.hpp"
#undef __HDR_DEFS__

#define __HDR_CLASSES__
#include "rvm_gen.hpp"
#include "Proj_Info/proj_info.hpp"
#include "Proj_Info/Chip/chip.hpp"
#include "Proj_Info/Kernel/kernel.hpp"
#include "Proj_Info/Monitor/monitor.hpp"
#include "Proj_Info/Kobj/kobj.hpp"
#include "Proj_Info/Process/process.hpp"
#include "Proj_Info/Process/Shmem/shmem.hpp"
#include "Proj_Info/Process/Captbl/captbl.hpp"
#include "Proj_Info/Process/Pgtbl/pgtbl.hpp"
#include "Proj_Info/Process/Thread/thread.hpp"
#include "Proj_Info/Process/Invocation/invocation.hpp"
#include "Proj_Info/Process/Port/port.hpp"
#include "Proj_Info/Process/Receive/receive.hpp"
#include "Proj_Info/Process/Send/send.hpp"
#include "Proj_Info/Process/Kfunc/kfunc.hpp"
#include "Proj_Info/Process/Virtual/virtual.hpp"
#include "Chip_Info/chip_info.hpp"
#include "Plat_Info/Compatible/compatible.hpp"
#include "Plat_Info/plat_info.hpp"
#include "Conf_Info/conf_info.hpp"
#include "Vect_Info/vect_info.hpp"
#include "Mem_Info/mem_info.hpp"
#include "Gen_Tool/gen_tool.hpp"
#include "Gen_Tool/Plat_Gen/plat_gen.hpp"
#include "Gen_Tool/Build_Gen/build_gen.hpp"
#include "Gen_Tool/Tool_Gen/tool_gen.hpp"
#include "Gen_Tool/Guest_Gen/guest_gen.hpp"
#undef __HDR_CLASSES__
/* End Includes **************************************************************/
namespace RVM_GEN
{
/* Global Variables **********************************************************/
ptr_t Main::Verbose=0;
ptr_t Main::Mock=0;
std::string Main::Time;
/* End Global Variables ******************************************************/

/* Begin Function:Main::Proj_Parse ********************************************
Description : Parse the main project.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
void Main::Proj_Parse(void)
{
    FILE* File;
    xml_node_t* Root;

    File=nullptr;
    Root=nullptr;
    try
    {
        /* Read in the whole project file */
        File=fopen(this->Input.c_str(),"r");
        if(File==nullptr)
            Main::Error(std::string("XXXXX: '")+this->Input+"': No such project file or cannot be opened.");
        this->Buffer[fread(this->Buffer,1,MAX_FILE_SIZE,File)]='\0';
        if(strlen(this->Buffer)==0)
        {
            File=nullptr;
            Main::Error(std::string("XXXXX: '")+this->Input+"': Project file is empty.");
        }
        fclose(File);
        File=nullptr;

        /* Parse and then release the parsing tree */
        if(XML_Parse(&Root,(xml_s8_t*)(this->Buffer))!=0)
        {
            Root=nullptr;
            Main::Error(std::string("XXXXX: Project file parsing internal error."));
        }
        this->Proj=std::make_unique<class Proj_Info>(Root);
        /* We have done parsing, release the parsing tree */
        if(XML_Del(Root)!=0)
        {
            Root=nullptr;
            Main::Error(std::string("XXXXX: Project file parsing internal error."));
        }
    }
    catch(std::exception& Exc)
    {
        if(File!=nullptr)
            fclose(File);
        if(Root!=nullptr)
            XML_Del(Root);
        Main::Error(Exc.what());
    }
}
/* End Function:Main::Proj_Parse *********************************************/

/* Begin Function:Main::Chip_Parse ********************************************
Description : Parse the chip configuration file.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
void Main::Chip_Parse(void)
{
    FILE* File;
    xml_node_t* Root;
    std::string Path;

    File=nullptr;
    Root=nullptr;
    try
    {
        /* Try to find the correct chip description file */
        Path=this->Proj->Monitor->Monitor_Root+"Include/Platform/"+
             this->Proj->Chip->Platform+"/Chip/"+
             this->Proj->Chip->Class+"/"+
             "rvm_platform_"+this->Proj->Chip->Class+".rvc";
        /* Read in the whole chip description file */
        File=fopen(Path.c_str(),"r");
        if(File==nullptr)
            Main::Error(std::string("XXXXX: '")+Path+"': No such chip description file or cannot be opened.");
        this->Buffer[fread(this->Buffer,1,MAX_FILE_SIZE,File)]='\0';
        if(strlen(this->Buffer)==0)
        {
            File=nullptr;
            Main::Error(std::string("XXXXX: '")+Path+"': Chip description file is empty.");
        }
        fclose(File);
        File=nullptr;

        /* Parse and then release the parsing tree */
        if(XML_Parse(&Root,(xml_s8_t*)(this->Buffer))!=0)
        {
            Root=nullptr;
            Main::Error(std::string("XXXXX: Chip description file parsing internal error."));
        }
        this->Chip=std::make_unique<class Chip_Info>(Root);
        /* We have done parsing, release the parsing tree */
        if(XML_Del(Root)!=0)
        {
            Root=nullptr;
            Main::Error(std::string("XXXXX: Chip description file parsing internal error."));
        }
    }
    catch(std::exception& Exc)
    {
        if(File!=nullptr)
            fclose(File);
        if(Root!=nullptr)
            XML_Del(Root);
        Main::Error(Exc.what());
    }
}
/* End Function:Main::Chip_Parse *********************************************/

/* Begin Function:Main::Plat_Parse ********************************************
Description : Parse the platform description file.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
void Main::Plat_Parse(void)
{
    FILE* File;
    xml_node_t* Root;
    std::string Path;

    File=nullptr;
    Root=nullptr;
    try
    {
        /* Try to find the correct platform description file */
        Path=this->Proj->Chip->Platform;
        Main::Lower(Path);
        Path=this->Proj->Monitor->Monitor_Root+"Include/Platform/"+
             this->Proj->Chip->Platform+"/rvm_platform_"+Path+".rva";
        /* Read in the whole chip description file */
        File=fopen(Path.c_str(),"r");
        if(File==nullptr)
            Main::Error(std::string("XXXXX: '")+Path+"': No such platform description file or cannot be opened.");
        this->Buffer[fread(this->Buffer,1,MAX_FILE_SIZE,File)]='\0';
        if(strlen(this->Buffer)==0)
        {
            File=nullptr;
            Main::Error(std::string("XXXXX: '")+Path+"': Platform description file is empty.");
        }
        fclose(File);
        File=nullptr;

        /* Parse and then release the parsing tree */
        if(XML_Parse(&Root,(xml_s8_t*)(this->Buffer))!=0)
        {
            Root=nullptr;
            Main::Error(std::string("XXXXX: Platform description file parsing internal error."));
        }
        this->Plat=std::make_unique<class Plat_Info>(Root);
        /* We have done parsing, release the parsing tree */
        if(XML_Del(Root)!=0)
        {
            Root=nullptr;
            Main::Error(std::string("XXXXX: Platform description file parsing internal error."));
        }
    }
    catch(std::exception& Exc)
    {
        if(File!=nullptr)
            fclose(File);
        if(Root!=nullptr)
            XML_Del(Root);
        Main::Error(Exc.what());
    }
}
/* End Function:Main::Plat_Parse *********************************************/

/* Begin Function:Main::Parse *************************************************
Description : Parse the files.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
void Main::Parse(void)
{
    try
    {
        Main::Info("Parsing project.");

        /* Parse project file */
        this->Proj_Parse();
        /* Change current path - everything is now relative to the project */
        std::filesystem::current_path(this->Input.substr(0,this->Input.find_last_of("/\\")+1));
        /* Parse platform description file */
        this->Plat_Parse();
        /* Parse chip description file */
        this->Chip_Parse();
    }
    catch(std::exception& Exc)
    {
        Main::Error(std::string("Parse:\n")+Exc.what());
    }
}
/* End Function:Main::Parse **************************************************/

/* Begin Function:Main::Standalone_Check **************************************
Description : Check the config files individually for early errors.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
void Main::Standalone_Check(void)
{
    try
    {
        /* Check each configuration file individually */
        this->Proj->Check();
        this->Plat->Check();
        this->Chip->Check();
    }
    catch(std::exception& Exc)
    {
        Main::Error(std::string("Standalone file:\n")+Exc.what());
    }
}
/* End Function:Main::Standalone_Check ***************************************/

/* Begin Function:Main::Compatible_Check **************************************
Description : Check whether the description files are mutually compatible.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
void Main::Compatible_Check(void)
{
    try
    {
        /* Check project/platform/chip compatibility */
        if((this->Proj->Chip->Platform!=this->Chip->Platform)||(this->Plat->Name!=this->Chip->Platform))
            Main::Error("XXXXX: The project platform and the chip platform is different.");
        if(this->Proj->Chip->Class!=this->Chip->Name)
            Main::Error("XXXXX: The project class and the chip class is different.");
        if(std::find(this->Chip->Compatible.begin(),
                     this->Chip->Compatible.end(),
                     this->Proj->Chip->Name)==this->Chip->Compatible.end())
            Main::Error("XXXXX: The project chip is not compatible with the chip description file.");

        /* Check if the kernel priorities are a multiple of word size */
        if((this->Proj->Kernel->Kern_Prio%this->Plat->Wordlength)!=0)
            Main::Error("XXXXX: Total number of kernel priorities must be a multiple of word size.");

        /* Check if the kernel memory granularity order is within range - we allow 2^3=8 bytes to 2^5=32 bytes */
        if(this->Proj->Kernel->Kom_Order<3)
            Main::Error("XXXXX: Kernel memory allocation granularity order must be no less than 2^3=8 bytes.");
        if(this->Proj->Kernel->Kom_Order>5)
            Main::Error("XXXXX: Kernel memory allocation granularity order must be no more than 2^5=32 bytes.");

        /* Check VMM configs */
        if(this->Proj->Virtual.empty()==false)
        {
            if(this->Proj->Monitor->Virt_Prio==0)
                Main::Error("XXXXX: Virtual machine exists but the total number of virtual machine priorities is set to 0.");
            if((this->Proj->Monitor->Virt_Prio%this->Plat->Wordlength)!=0)
                Main::Error("XXXXX: Total number of virtual machine priorities must be a multiple of processor wordlength.");
            if(this->Proj->Monitor->Virt_Event==0)
                Main::Error("XXXXX: Virtual machine exists but the total number of virtual event sources is set to 0.");
            if((this->Proj->Monitor->Virt_Event%this->Plat->Wordlength)!=0)
                Main::Error("XXXXX: Total number of virtual event sources must be a multiple of processor wordlength.");
            if(this->Proj->Monitor->Virt_Event>VIRT_EVENT_MAX)
                Main::Error("XXXXX: Total number of virtual event sources cannot exceed %d.",VIRT_EVENT_MAX);
            if(this->Proj->Monitor->Virt_Map<this->Proj->Monitor->Virt_Event)
                Main::Error("XXXXX: Total number of virtual event to interrupt mappings cannot be smaller than the virtual event source number.");

        }
        /* Set all virtual machine related portion to zero because these options do not make sense anymore */
        else
        {
            this->Proj->Monitor->Virt_Prio=0;
            this->Proj->Monitor->Virt_Event=0;
            this->Proj->Monitor->Virt_Map=0;
        }
    }
    catch(std::exception& Exc)
    {
        Main::Error(std::string("Compatibility:\n")+Exc.what());
    }
}
/* End Function:Main::Compatible_Check ***************************************/

/* Begin Function:Main::Config_Check ******************************************
Description : Check whether the config are correctly configured - no more, no less.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
void Main::Config_Check(void)
{
    std::map<std::string,class Conf_Info*>::iterator Iter;

    try
    {
        /* Check if all configs are correctly configured: no more, no less */
        for(const std::pair<const std::string, std::string>& Conf:this->Proj->Chip->Config)
        {
            Iter=this->Chip->Config_Map.find(Conf.first);
            if(Iter==this->Chip->Config_Map.end())
            {
                Iter=this->Plat->Config_Map.find(Conf.first);
                if(Iter==this->Plat->Config_Map.end())
                    Main::Error("XXXXX: Cannot find config '"+Conf.first+"' in platform or chip description file.");
            }
            /* See if the config value falls into the range, and mark it */
            Iter->second->Project_Config_Mark(Conf.first,Conf.second);
        }
        this->Plat->Project_Config_Mark_Check();
        this->Chip->Project_Config_Mark_Check();
    }
    catch(std::exception& Exc)
    {
        Main::Error(std::string("Config:\n")+Exc.what());
    }
}
/* End Function:Main::Config_Check *******************************************/

/* Begin Function:Main::Physical_Check ****************************************
Description : Check whether the physical memory will overlap with each other.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
void Main::Physical_Check(void)
{
    std::map<std::string,class Conf_Info*>::iterator Iter;

    try
    {
        /* Merge all physical memory from chip and EMIF */
        for(std::unique_ptr<class Mem_Info>& Mem:this->Chip->Memory)
        {
            switch(Mem->Type)
            {
                case MEM_CODE:this->Proj->Memory_Code.push_back(Mem.get());break;
                case MEM_DATA:this->Proj->Memory_Data.push_back(Mem.get());break;
                case MEM_DEVICE:this->Proj->Memory_Device.push_back(Mem.get());break;
                default:ASSERT(0);
            }
        }
        for(std::unique_ptr<class Mem_Info>& Mem:this->Proj->Extmem)
        {
            switch(Mem->Type)
            {
                case MEM_CODE:this->Proj->Memory_Code.push_back(Mem.get());break;
                case MEM_DATA:this->Proj->Memory_Data.push_back(Mem.get());break;
                case MEM_DEVICE:this->Proj->Memory_Device.push_back(Mem.get());break;
                default:ASSERT(0);
            }
        }

        /* Make sure all mounted physical memory do not overlap */
        Mem_Info::Overlap_Check(this->Proj->Memory_Code,this->Proj->Memory_Data,this->Proj->Memory_Device,"Physical memory");
    }
    catch(std::exception& Exc)
    {
        Main::Error(std::string("Physical memory:\n")+Exc.what());
    }
}
/* End Function:Main::Physical_Check *****************************************/

/* Begin Function:Main::Static_Check ******************************************
Description : Check whether the statically allocated memory will overlap with each other.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
void Main::Static_Check(void)
{
    std::vector<class Mem_Info*> Code;
    std::vector<class Mem_Info*> Data;
    std::vector<class Mem_Info*> Device;

    try
    {
        /* Shared memory */
        Code=this->Proj->Shmem_Code;
        Data=this->Proj->Shmem_Data;
        Device=this->Proj->Shmem_Device;

        /* Kernel and monitor memory */
        Code.push_back(this->Proj->Kernel->Code.get());
        Data.push_back(this->Proj->Kernel->Data.get());
        Code.push_back(this->Proj->Monitor->Code.get());
        Data.push_back(this->Proj->Monitor->Data.get());

        /* Process memory */
        for(std::unique_ptr<class Process>& Prc:this->Proj->Process)
        {
            Code.insert(Code.end(), Prc->Memory_Code.begin(), Prc->Memory_Code.end());
            Data.insert(Data.end(), Prc->Memory_Data.begin(), Prc->Memory_Data.end());
            Device.insert(Device.end(), Prc->Memory_Device.begin(), Prc->Memory_Device.end());
        }

        /* Make sure the statically allocated memory do not overlap */
        Mem_Info::Overlap_Check(Code,Data,Device,"Statically allocated memory");
    }
    catch(std::exception& Exc)
    {
        Main::Error(std::string("Static memory:\n")+Exc.what());
    }
}
/* End Function:Main::Static_Check *******************************************/

/* Begin Function:Main::Reference_Check ***************************************
Description : Check whether the shared memory or kernel object references are valid.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
void Main::Reference_Check(void)
{
    class Virtual* Virt;
    std::map<std::string,class Mem_Info*>::iterator Shm_Iter;
    std::map<std::string,class Process*>::iterator Prc_Iter;
    std::map<std::string,class Vect_Info*>::iterator Vect_Iter;

    try
    {
        for(std::unique_ptr<class Process>& Prc:this->Proj->Process)
        {
            try
            {
                /* Check extra capability table sizes */
                if(Prc->Extra_Captbl>this->Plat->Captbl_Max)
                    Main::Error("XXXXX: Extra captbl capacity cannot be larger than the platform limit %lld.",this->Plat->Captbl_Max);

                /* Check virtual machine priorities */
                if(Prc->Type==PROCESS_VIRTUAL)
                {
                    Virt=static_cast<class Virtual*>(Prc.get());
                    if(Virt->Priority>=this->Proj->Monitor->Virt_Prio)
                        Main::Error("XXXXX: Virtual machine priority %lld must be smaller than total number of virtual machine priorities %lld.",
                                    Virt->Priority, this->Proj->Monitor->Virt_Prio);
                }

                /* Check shared memory references */
                for(std::unique_ptr<class Shmem>& Shm:Prc->Shmem)
                {
                    /* Check existence */
                    Shm_Iter=this->Proj->Shmem_Map.find(Shm->Name);
                    if(Shm_Iter==this->Proj->Shmem_Map.end())
                        Main::Error("XXXXX: Shared memory '"+Shm->Name+"' does not exist.");
                    /* Check access permissions */
                    if((Shm_Iter->second->Attr&Shm->Attr)!=Shm->Attr)
                        Main::Error("XXXXX: Shared memory '"+Shm->Name+"' contains wrong attributes.");
                }

                /* Check thread parameters if this is not a VM - VM priorities are generated so fine */
                if(Prc->Type==PROCESS_NATIVE)
                {
                    for(std::unique_ptr<class Thread>& Thd:Prc->Thread)
                    {
                        if(Thd->Priority<PRC_THD_PRIO_MIN)
                            Main::Error("XXXXX: Thread '"+Thd->Name+"' priority must be bigger than service daemons' priority (4).");
                        else if(Thd->Priority>(this->Proj->Kernel->Kern_Prio-2))
                            Main::Error("XXXXX: Thread '"+Thd->Name+"' priority must be smaller than safety daemon's priority (Kern_Prio-1).");
                    }
                }

                /* Check port references */
                for(std::unique_ptr<class Port>& Prt:Prc->Port)
                {
                    /* Check process existence */
                    Prc_Iter=this->Proj->Process_Map.find(Prt->Process);
                    if(Prc_Iter==this->Proj->Process_Map.end())
                        Main::Error("XXXXX: Port '"+Prt->Process+"."+Prt->Name+"' refers to a nonexistent process.");
                    /* Check invocation existence */
                    if(Prc_Iter->second->Invocation_Map.find(Prt->Name)==Prc_Iter->second->Invocation_Map.end())
                        Main::Error("XXXXX: Port '"+Prt->Process+"."+Prt->Name+"' refers to a nonexistent invocation socket.");
                }

                /* Check send references */
                for(std::unique_ptr<class Send>& Snd:Prc->Send)
                {
                    /* Check process existence */
                    Prc_Iter=this->Proj->Process_Map.find(Snd->Process);
                    if(Prc_Iter==this->Proj->Process_Map.end())
                        Main::Error("XXXXX: Send endpoint '"+Snd->Process+"."+Snd->Name+"' refers to a nonexistent process.");
                    /* Check receive endpoint existence */
                    if(Prc_Iter->second->Receive_Map.find(Snd->Name)==Prc_Iter->second->Receive_Map.end())
                        Main::Error("XXXXX: Send endpoint '"+Snd->Process+"."+Snd->Name+"' refers to a nonexistent receive endpoint.");
                }

                /* Check vector references */
                for(std::unique_ptr<class Vect_Info>& Vct:Prc->Vector)
                {
                    Vect_Iter=this->Chip->Vector_Map.find(Vct->Name);
                    if(Vect_Iter==this->Chip->Vector_Map.end())
                        Main::Error("XXXXX: Vector '"+Vct->Name+"' refers to a nonexistent vector.");
                    if(Vect_Iter->second->Number!=Vct->Number)
                        Main::Error("XXXXX: Vector '"+Vct->Name+"' contains a wrong vector number.");
                }

                /* Check kernel function ranges */
                for(std::unique_ptr<class Kfunc>& Kfn:Prc->Kfunc)
                {
                    if(Kfn->End>=this->Plat->Kfunc_Max)
                        Main::Error("XXXXX: Kernel function '%s' number range exceeded the platform limit %lld.",
                                    Kfn->Name.c_str(),this->Plat->Kfunc_Max);
                }
            }
            catch(std::exception& Exc)
            {
                Main::Error(std::string("Process: ")+Prc->Name+"\n"+Exc.what());
            }
        }
    }
    catch(std::exception& Exc)
    {
        Main::Error(std::string("Kernel object reference:\n")+Exc.what());
    }
}
/* End Function:Main::Reference_Check ****************************************/

/* Begin Function:Main::Check *************************************************
Description : Check whether the configurations make sense.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
void Main::Check(void)
{
    try
    {
        this->Standalone_Check();
        this->Compatible_Check();
        this->Config_Check();
        this->Physical_Check();
        this->Static_Check();
        this->Reference_Check();
    }
    catch(std::exception& Exc)
    {
        Main::Error(std::string("Check:\n")+Exc.what());
    }
}
/* End Function:Main::Check **************************************************/

/* Begin Function:Main::Setup *************************************************
Description : Load toolset used for generation. This also passes in all data
              structures they'll ever need to generate everything.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
void Main::Setup(void)
{
    try
    {
        /* Load platform toolset */
        this->Gen=std::make_unique<class Gen_Tool>(this->Plat->Name,
                                                   this->Proj.get(),this->Plat.get(),this->Chip.get());

        /* Load buildsystem toolset */
        this->Gen->Build_Load(this->Proj->Kernel->Buildsystem);
        this->Gen->Build_Load(this->Proj->Monitor->Buildsystem);
        for(std::unique_ptr<class Process>& Prc:this->Proj->Process)
            this->Gen->Build_Load(Prc->Buildsystem);

        /* Load toolchain toolset */
        this->Gen->Tool_Load(this->Proj->Kernel->Toolchain);
        this->Gen->Tool_Load(this->Proj->Monitor->Toolchain);
        for(std::unique_ptr<class Process>& Prc:this->Proj->Process)
            this->Gen->Tool_Load(Prc->Toolchain);

        /* Load guest OS toolset */
        for(class Virtual* Virt:this->Proj->Virtual)
            this->Gen->Guest_Load(Virt->Guest_Type);

        /* Check kernel */
        this->Plat->Compatible_Check("Kernel", this->Proj->Kernel->Buildsystem, this->Proj->Kernel->Toolchain, "Native");
        /* Check monitor */
        this->Plat->Compatible_Check("Monitor", this->Proj->Kernel->Buildsystem, this->Proj->Kernel->Toolchain, "Native");
        /* Check each process */
        for(std::unique_ptr<class Process>& Prc:this->Proj->Process)
        {
            if(Prc->Type==PROCESS_NATIVE)
                this->Plat->Compatible_Check(std::string("Process '")+Prc->Name+"'",
                                             Prc->Buildsystem, Prc->Toolchain, "Native");
            else
                this->Plat->Compatible_Check(std::string("Virtual machine '")+Prc->Name+"'",
                                             Prc->Buildsystem, Prc->Toolchain, static_cast<class Virtual*>(Prc.get())->Guest_Type);
        }
    }
    catch(std::exception& Exc)
    {
        Main::Error(std::string("Check:\n")+Exc.what());
    }
}
/* End Function:Main::Setup **************************************************/

/* Begin Function:Main::Mem_Align *********************************************
Description : Align all memories that does not have a fixed start address, and
              check all memories that have an address is aligned.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
void Main::Mem_Align(void)
{
    try
    {
        Main::Info("Aligning memory.");

        /* Align system memory */
        try
        {
            this->Gen->Plat->Mem_Align(this->Proj->Kernel->Code_Base,this->Proj->Kernel->Code_Size,MEM_AUTO);
            this->Gen->Plat->Mem_Align(this->Proj->Kernel->Data_Base,this->Proj->Kernel->Data_Size,MEM_AUTO);
            this->Gen->Plat->Mem_Align(MEM_AUTO,this->Proj->Kernel->Stack_Size,MEM_AUTO);
            this->Gen->Plat->Mem_Align(this->Proj->Monitor->Code_Base,this->Proj->Monitor->Code_Size,MEM_AUTO);
            this->Gen->Plat->Mem_Align(this->Proj->Monitor->Data_Base,this->Proj->Monitor->Data_Size,MEM_AUTO);
            this->Gen->Plat->Mem_Align(MEM_AUTO,this->Proj->Monitor->Init_Stack_Size,MEM_AUTO);
            this->Gen->Plat->Mem_Align(MEM_AUTO,this->Proj->Monitor->Sftd_Stack_Size,MEM_AUTO);
            this->Gen->Plat->Mem_Align(MEM_AUTO,this->Proj->Monitor->Vmmd_Stack_Size,MEM_AUTO);
        }
        catch(std::exception& Exc)
        {
            Main::Error(std::string("System memory:\n")+Exc.what());
        }

        /* Align all shared memory */
        try
        {
            for(std::unique_ptr<class Mem_Info>& Mem:this->Proj->Shmem)
                Mem->Align=this->Gen->Plat->Mem_Align(Mem->Base,Mem->Size,Mem->Align_Order);
        }
        catch(std::exception& Exc)
        {
            Main::Error(std::string("Shared memory:\n")+Exc.what());
        }

        /* Align all private memory for each process */
        for(std::unique_ptr<class Process>& Prc:this->Proj->Process)
        {
            try
            {
                for(std::unique_ptr<class Mem_Info>& Mem:Prc->Memory)
                    Mem->Align=this->Gen->Plat->Mem_Align(Mem->Base,Mem->Size,Mem->Align_Order);
            }
            catch(std::exception& Exc)
            {
                Main::Error(std::string("Process: '")+Prc->Name+"'\n"+"Private memory:\n"+Exc.what());
            }
        }
    }
    catch(std::exception& Exc)
    {
        Main::Error(std::string("Memory alignment:\n")+Exc.what());
    }
}
/* End Function:Main::Mem_Align **********************************************/

/* Begin Function:Main::Code_Alloc ********************************************
Description : Allocate code memory.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
void Main::Code_Alloc(void)
{
    std::vector<class Mem_Info*> Auto;

    try
    {
        /* Initialize memory allocator map for physical memory trunks */
        for(class Mem_Info* Mem:this->Proj->Memory_Code)
            Mem->Memmap_Init();

        /* Sort memory according to base address */
        std::sort(this->Proj->Memory_Code.begin(),this->Proj->Memory_Code.end(),
        [](const class Mem_Info* Left, const class Mem_Info* Right)->bool
        {
            return Left->Base<Right->Base;
        });

        /* Now populate the system sections - must be continuous */
        if(this->Proj->Kernel->Code->Static_Fit(this->Proj->Memory_Code)!=0)
            throw std::runtime_error("XXXXX: Kernel code section is invalid, either wrong range or wrong attribute.");
        if(this->Proj->Monitor->Code->Static_Fit(this->Proj->Memory_Code)!=0)
            throw std::runtime_error("XXXXX: Monitor code section is invalid, either wrong range or wrong attribute.");

        /* Fit all static shared memory regions, and leave the automatic ones for later */
        try
        {
            for(class Mem_Info* Mem:this->Proj->Shmem_Code)
            {
                /* If this memory is not auto memory, we wait to deal with it */
                if(Mem->Base!=MEM_AUTO)
                {
                    if(Mem->Static_Fit(this->Proj->Memory_Code)!=0)
                        Main::Error(std::string("XXXXX: Code memory '")+Mem->Name+"' is invalid, either wrong range or wrong attribute.");
                }
                else
                    Auto.push_back(Mem);
            }
        }
        catch(std::exception& Exc)
        {
            Main::Error(std::string("Shared memory:\n")+Exc.what());
        }

        /* Fit all static private memory regions, and leave the automatic ones for later */
        for(std::unique_ptr<class Process>& Prc:this->Proj->Process)
        {
            try
            {
                for(class Mem_Info* Mem:Prc->Memory_Code)
                {
                    /* If this memory is not auto memory, we wait to deal with it */
                    if(Mem->Base!=MEM_AUTO)
                    {
                        if(Mem->Static_Fit(this->Proj->Memory_Code)!=0)
                            Main::Error(std::string("XXXXX: Code memory '")+Mem->Name+"' is invalid, either wrong range or wrong attribute.");
                    }
                    else
                        Auto.push_back(Mem);
                }
            }
            catch(std::exception& Exc)
            {
                Main::Error(std::string("Process: ")+Prc->Name+"\n"+Exc.what());
            }
        }

        /* Sort all memories from large to small */
        std::sort(Auto.begin(),Auto.end(),
        [](const class Mem_Info* Left, const class Mem_Info* Right)
        {
            return Left->Size>Right->Size;
        });

        /* Fit whatever that does not have a fixed address */
        for(class Mem_Info* Mem:Auto)
        {
            if(Mem->Auto_Fit(this->Proj->Memory_Code)!=0)
                Main::Error(std::string("XXXXX: Code memory fitter failed for automatic memory '")+Mem->Name+"'.");
            if(Mem->Name=="")
                Main::Info("> Allocated code memory size 0x%0llX to base address 0x%0llX.",Mem->Size,Mem->Base);
            else
                Main::Info("> Allocated code memory '%s' size 0x%0llX to base address 0x%0llX.",Mem->Name.c_str(),Mem->Size,Mem->Base);
        }
    }
    catch(std::exception& Exc)
    {
        Main::Error(std::string("Code memory allocation:\n")+Exc.what());
    }
}
/* End Function:Main::Code_Alloc *********************************************/

/* Begin Function:Main::Data_Alloc ********************************************
Description : Allocate data memory.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
void Main::Data_Alloc(void)
{
    std::vector<class Mem_Info*> Auto;

    try
    {
        /* Initialize memory allocator map for physical memory trunks */
        for(class Mem_Info* Mem:this->Proj->Memory_Data)
            Mem->Memmap_Init();

        /* Sort memory according to base address */
        std::sort(this->Proj->Memory_Data.begin(),this->Proj->Memory_Data.end(),
        [](const class Mem_Info* Left, const class Mem_Info* Right)->bool
        {
            return Left->Base<Right->Base;
        });

        /* Now populate the system sections - must be continuous */
        if(this->Proj->Kernel->Data->Static_Fit(this->Proj->Memory_Data)!=0)
            Main::Error("XXXXX: Kernel data section is invalid, either wrong range or wrong attribute.");
        if(this->Proj->Monitor->Data->Static_Fit(this->Proj->Memory_Data)!=0)
            Main::Error("XXXXX: Monitor data section is invalid, either wrong range or wrong attribute.");

        /* Fit all static shared memory regions, and leave the automatic ones for later */
        try
        {
            for(class Mem_Info* Mem:this->Proj->Shmem_Data)
            {
                /* If this memory is not auto memory, we wait to deal with it */
                if(Mem->Base!=MEM_AUTO)
                {
                    if(Mem->Static_Fit(this->Proj->Memory_Data)!=0)
                        Main::Error(std::string("XXXXX: Data memory '")+Mem->Name+"' is invalid, either wrong range or wrong attribute.");
                }
                else
                    Auto.push_back(Mem);
            }
        }
        catch(std::exception& Exc)
        {
            Main::Error(std::string("Shared memory:\n")+Exc.what());
        }

        /* Fit all static private memory regions, and leave the automatic ones for later */
        for(std::unique_ptr<class Process>& Prc:this->Proj->Process)
        {
            try
            {
                for(class Mem_Info* Mem:Prc->Memory_Data)
                {
                    /* If this memory is not auto memory, we wait to deal with it */
                    if(Mem->Base!=MEM_AUTO)
                    {
                        if(Mem->Static_Fit(this->Proj->Memory_Data)!=0)
                            Main::Error(std::string("XXXXX: Data memory '")+Mem->Name+"' is invalid, either wrong range or wrong attribute.");
                    }
                    else
                        Auto.push_back(Mem);
                }
            }
            catch(std::exception& Exc)
            {
                Main::Error(std::string("Process: ")+Prc->Name+"\n"+Exc.what());
            }
        }

        /* Sort all memories from large to small */
        std::sort(Auto.begin(),Auto.end(),
        [](const class Mem_Info* Left, const class Mem_Info* Right)
        {
                return Left->Size>Right->Size;
        });

        /* Fit whatever that does not have a fixed address */
        for(class Mem_Info* Mem:Auto)
        {
            if(Mem->Auto_Fit(this->Proj->Memory_Data)!=0)
                Main::Error(std::string("XXXXX: Data memory fitter failed for automatic memory '")+Mem->Name+"'.");
            Main::Info("> Allocated data memory '%s' size 0x%0llX to base address 0x%0llX.",Mem->Name.c_str(),Mem->Size,Mem->Base);
        }
    }
    catch(std::exception& Exc)
    {
        Main::Error(std::string("Data allocation:\n")+Exc.what());
    }
}
/* End Function:Main::Data_Alloc *********************************************/

/* Begin Function:Main::Device_Alloc ******************************************
Description : Allocate data memory.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
void Main::Device_Alloc(void)
{
    try
    {
        /* Initialize memory allocator map for physical memory trunks */
        for(class Mem_Info* Mem:this->Proj->Memory_Device)
            Mem->Memmap_Init();

        /* Sort memory according to base address */
        std::sort(this->Proj->Memory_Device.begin(),this->Proj->Memory_Device.end(),
        [](const class Mem_Info* Left, const class Mem_Info* Right)->bool
        {
            return Left->Base<Right->Base;
        });

        /* Fit all static shared memory regions, and leave the automatic ones for later */
        try
        {
            for(class Mem_Info* Mem:this->Proj->Shmem_Device)
            {
                /* Device memory can't be auto */
                ASSERT(Mem->Base!=MEM_AUTO);
                if(Mem->Static_Fit(this->Proj->Memory_Device)!=0)
                    Main::Error(std::string("XXXXX: Device memory '")+Mem->Name+"' is invalid, either wrong range or wrong attribute.");
            }
        }
        catch(std::exception& Exc)
        {
            Main::Error(std::string("Shared memory:\n")+Exc.what());
        }

        /* Fit all static private memory regions, and leave the automatic ones for later */
        for(std::unique_ptr<class Process>& Prc:this->Proj->Process)
        {
            try
            {
                for(class Mem_Info* Mem:Prc->Memory_Device)
                {
                    /* Device memory can't be auto */
                    ASSERT(Mem->Base!=MEM_AUTO);
                    if(Mem->Static_Fit(this->Proj->Memory_Device)!=0)
                        Main::Error(std::string("XXXXX: Device memory '")+Mem->Name+"' is invalid, either wrong range or wrong attribute.");
                }
            }
            catch(std::exception& Exc)
            {
                Main::Error(std::string("Process: ")+Prc->Name+"\n"+Exc.what());
            }
        }
    }
    catch(std::exception& Exc)
    {
        Main::Error(std::string("Device allocation:\n")+Exc.what());
    }
}
/* End Function:Main::Device_Alloc *******************************************/

/* Begin Function:Main::Mem_Alloc *********************************************
Description : Allocate memory to each one that still does not have a valid address.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
void Main::Mem_Alloc(void)
{
    try
    {
        Main::Info("Allocating memory.");

        this->Code_Alloc();
        this->Data_Alloc();
        this->Device_Alloc();
    }
    catch(std::exception& Exc)
    {
        Main::Error(std::string("Memory allocation:\n")+Exc.what());
    }
}
/* End Function:Main::Mem_Alloc **********************************************/

/* Begin Function:Main::Shmem_Add *********************************************
Description : Allocate memory to each one that still does not have a valid address.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
void Main::Shmem_Add(void)
{
    std::unique_ptr<class Mem_Info> Temp;
    std::map<std::string,class Mem_Info*>::iterator Mem;

    try
    {
        Main::Info("Instantiating shared memory.");

        for(std::unique_ptr<class Process>& Prc:this->Proj->Process)
        {
            try
            {
                /* Link the shared memory blocks with references in processes, and add result to private memory pool */
                for(std::unique_ptr<class Shmem>& Shm:Prc->Shmem)
                {
                    /* Find the shared memory block declared in the project */
                    Mem=this->Proj->Shmem_Map.find(Shm->Name);
                    ASSERT(Mem!=this->Proj->Shmem_Map.end());
                    /* Check attributes then populate */
                    Temp=std::make_unique<class Mem_Info>(Mem->second,Shm->Attr);
                    Temp->Is_Shared=1;
                    switch(Temp->Type)
                    {
                        case MEM_CODE:Prc->Memory_Code.push_back(Temp.get());break;
                        case MEM_DATA:Prc->Memory_Data.push_back(Temp.get());break;
                        case MEM_DEVICE:Prc->Memory_Device.push_back(Temp.get());break;
                        default:ASSERT(0);
                    }
                    Prc->Memory.push_back(std::move(Temp));
                }

                /* Copy everything over to the final memory list so that we can operate on that list */
                for(std::unique_ptr<class Mem_Info>& Mem:Prc->Memory)
                    Prc->Memory_All.push_back(std::make_unique<class Mem_Info>(Mem.get(), Mem->Attr));
            }
            catch(std::exception& Exc)
            {
                Main::Error(std::string("Process: ")+Prc->Name+"\n"+Exc.what());
            }
        }
    }
    catch(std::exception& Exc)
    {
        Main::Error(std::string("Shared memory instantiation:\n")+Exc.what());
    }
}
/* End Function:Main::Shmem_Add **********************************************/

/* Begin Function:Main::Pgt_Alloc *******************************************
Description : Allocate page tables.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
void Main::Pgt_Alloc(void)
{
    std::vector<std::unique_ptr<class Mem_Info>> List;
    ptr_t Cur_Start;
    ptr_t Cur_End;
    ptr_t Cur_Attr;
    ptr_t Cur_Type;
    ptr_t Total_Static;

    try
    {
        /* Allocate page table global captbl */
        for(std::unique_ptr<class Process>& Prc:this->Proj->Process)
        {
            Main::Info("Allocating page tables for process '%s'.",Prc->Name.c_str());

            try
            {
                /* Merge whatever this list has to offer - if two adjacent memory blocks have the
                 * same attributes, we simply merge them into one block, hopefully reducing MPU
                 * region number usage. */
                std::sort(Prc->Memory_All.begin(),Prc->Memory_All.end(),
                [](std::unique_ptr<class Mem_Info>& Left, std::unique_ptr<class Mem_Info>& Right)
                {
                    return Left->Base<Right->Base;
                });

                Cur_Start=Prc->Memory_All[0]->Base;
                Cur_End=Cur_Start+Prc->Memory_All[0]->Size;
                Cur_Type=Prc->Memory_All[0]->Type;
                Cur_Attr=Prc->Memory_All[0]->Attr;

                Prc->Memory_All.erase(Prc->Memory_All.begin());
                for(std::unique_ptr<class Mem_Info>& Mem:Prc->Memory_All)
                {
                    if(Mem->Base>Cur_End)
                    {
                        /* Time to accumulate the old segment and start counting for the new one */
                        List.push_back(std::make_unique<class Mem_Info>("",Cur_Start,Cur_End-Cur_Start,
                                                                        Cur_Type,Cur_Attr));
                        Cur_Start=Mem->Base;
                        Cur_End=Cur_Start+Mem->Size;
                        Cur_Type=Mem->Type;
                        Cur_Attr=Mem->Attr;
                    }
                    else if(Mem->Base==Cur_End)
                    {
                        /* Accumulate this segment to the old one if the attributes are the same */
                        if(Mem->Attr==Cur_Attr)
                            Cur_End=Mem->Base+Mem->Size;
                        /* If not the same, we start a new segment */
                        else
                        {
                            List.push_back(std::make_unique<class Mem_Info>("",Cur_Start,Cur_End-Cur_Start,
                                                                            Cur_Type,Cur_Attr));
                            Cur_Start=Mem->Base;
                            Cur_End=Cur_Start+Mem->Size;
                            Cur_Type=Mem->Type;
                            Cur_Attr=Mem->Attr;
                        }
                    }
                    /* This is an overlap and we throw an error. Shouldn't happen at here */
                    else
                        ASSERT(0);
                }
                /* Accumulate the last segment */
                List.push_back(std::make_unique<class Mem_Info>("",Cur_Start,Cur_End-Cur_Start,Cur_Type,Cur_Attr));

                /* Replace the list with whatever we have now */
                Prc->Memory_All.clear();
                for(std::unique_ptr<class Mem_Info>& Mem:List)
                    Prc->Memory_All.push_back(std::move(Mem));
                List.clear();

                /* We must reserve at least 2 MPU regions. If this is not the case, we give an warning, cause new static
                 * regions will now begin to replace each other at the kernel level. The global algorithm is:
                 * 1 If the incoming one is STATIC:
                 *   1.1 If there are DYNAMIC regions, replace one of them.
                 *   1.2 If there are no DYNAMIC regions, replace one of the STATIC regions.
                 * 2 If the incoming one is DYNAMIC:
                 *   2.1 If there are no less than 2 DYNAMIC regions, replace one of them.
                 *   2.2 If there are less than 2 DYNAMIC regions, replace a STATIC region. */
                Total_Static=0;
                Prc->Pgtbl=this->Gen->Plat->Pgt_Gen(Prc->Memory_All, Prc.get(), this->Plat->Wordlength, Total_Static);
                Prc->Pgtbl->Is_Top=1;
                if(Total_Static>(this->Chip->Region-2))
                {
                    Main::Warning(std::string("A0203: Too many static memory segments declared in process '")+Prc->Name+"', using "+
                                  std::to_string(Total_Static)+" regions, exceeding limit "+std::to_string(this->Chip->Region-2)+".");
                }
            }
            catch(std::exception& Exc)
            {
                Main::Error(std::string("Process:\n")+Prc->Name+"\n"+Exc.what());
            }
        }
    }
    catch(std::exception& Exc)
    {
        Main::Error(std::string("Page table allocation:\n")+Exc.what());
    }
}
/* End Function:Main::Pgt_Alloc ********************************************/

/* Begin Function:Main::Cap_Alloc *********************************************
Description : Allocate capabilities for kernel objects. This includes both local and
              global capids, and also their macros.
              Each global object will reside in its own capability table.
              This facilitates management, and circumvents the capability size
              limit that may present on 32-bit systems.
              How many distinct kernel objects are there? We just need to add up
              the following: All captbls (each process have one), all processes,
              all threads, all invocations, all receive endpoints. The ports and
              send endpoints do not have a distinct kernel object; the vector
              endpoints are created by the kernel at boot-time, while the pgtbls
              are decided by architecture-specific code.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
void Main::Cap_Alloc(void)
{
    try
    {
        /* Local capid/macro allocation */
        for(std::unique_ptr<class Process>& Prc:this->Proj->Process)
        {
            Main::Info(std::string("Allocating local caps for process '")+Prc->Name+"'.");
            Prc->Local_Alloc(this->Plat->Captbl_Max);
        }

        /* Global (monitor) capid/macro allocation */
        for(std::unique_ptr<class Process>& Prc:this->Proj->Process)
        {
            Main::Info(std::string("Allocating global caps for process '")+Prc->Name+"'.");
            Prc->Global_Alloc_Captbl(this->Proj->Monitor->Captbl);
            Prc->Global_Alloc_Pgtbl(this->Proj->Monitor->Pgtbl,Prc->Pgtbl);
            Prc->Global_Alloc_Process(this->Proj->Monitor->Process);
            Prc->Global_Alloc_Thread(this->Proj->Monitor->Thread);
            Prc->Global_Alloc_Invocation(this->Proj->Monitor->Invocation);
            Prc->Global_Alloc_Receive(this->Proj->Monitor->Receive);
            Prc->Global_Alloc_Vector(this->Proj->Monitor->Vector);
        }
    }
    catch(std::exception& Exc)
    {
        Main::Error(std::string("Capability allocation:\n")+Exc.what());
    }
}
/* End Function:Main::Cap_Alloc **********************************************/

/* Begin Function:Main::Cap_Link **********************************************
Description : Link relevant capabilities for kernel objects.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
void Main::Cap_Link(void)
{
    class Invocation* Inv_Dst;
    class Receive* Recv_Dst;

    try
    {
        /* Guaranteed to find stuff, or there'll be an internal error */
        for(std::unique_ptr<class Process>& Prc:this->Proj->Process)
        {
            Main::Info(std::string("Linking caps for process '")+Prc->Name+"'.");

            /* For every port, there must be a invocation somewhere */
            for(std::unique_ptr<class Port>& Port:Prc->Port)
            {
                Inv_Dst=this->Proj->Process_Map[Port->Process]->Invocation_Map[Port->Name];
                Port->Cid_Global=Inv_Dst->Cid_Global;
                Port->Macro_Global=Inv_Dst->Macro_Global;
                Main::Info("> Port %s co-allocated global capid %lld with invocation %s.",
                           Port->Macro_Local.c_str(),Port->Cid_Global,Port->Macro_Global.c_str());
            }

            /* For every send endpoint, there must be a receive endpoint somewhere */
            for(std::unique_ptr<class Send>& Send:Prc->Send)
            {
                Recv_Dst=this->Proj->Process_Map[Send->Process]->Receive_Map[Send->Name];
                Send->Cid_Global=Recv_Dst->Cid_Global;
                Send->Macro_Global=Recv_Dst->Macro_Global;
                Main::Info("> Send endpoint %s co-allocated global capid %lld with invocation %s.",
                           Send->Macro_Local.c_str(),Send->Cid_Global,Send->Macro_Global.c_str());
             }
        }
    }
    catch(std::exception& Exc)
    {
        Main::Error(std::string("Capability linking:\n")+Exc.what());
    }
}
/* End Function:Main::Cap_Link ***********************************************/

/* Begin Function:Main::Kom_Alloc ********************************************
Description : Get the size of the kernel memory, and generate the initial states
              for kernel object creation.
Input       : ptr_t Init_Capsz - The initial capability table's size;
Output      : None.
Return      : None.
******************************************************************************/
void Main::Kom_Alloc(ptr_t Init_Capsz)
{
    ptr_t Cap_Front;
    ptr_t Kom_Front;

    /* Compute initial state when creating the vectors */
    /* Initial capability table */
    Cap_Front=1;
    Kom_Front=this->Gen->Plat->Size_Cpt(Init_Capsz);
    /* Initial page table - always order 0, containing all address space */
    Cap_Front++;
    Kom_Front+=this->Gen->Plat->Size_Pgt(0,1);
    /* Initial RVM process */
    Cap_Front++;
    /* Initial thread */
    Cap_Front++;
    /* Initial kfn */
    Cap_Front++;
    /* Initial kmem */
    Cap_Front++;
    /* Initial tick timer/interrupt/event endpoint */
    Cap_Front++;
    Kom_Front+=this->Gen->Plat->Size_Thread();

    /* Create capability table for vectors */
    Main::Info("> Kernel vector cap front %lld kmem front 0x%llX.", Cap_Front, Kom_Front);
    this->Proj->Kernel->Vct_Cap_Front=Cap_Front;
    this->Proj->Kernel->Vct_Kom_Front=Kom_Front;
    /* Capability tables for containing vector endpoints */
    Cap_Front+=ROUND_DIV(this->Proj->Monitor->Vector.size(), this->Plat->Captbl_Max);
    Kom_Front+=this->Gen->Plat->Size_Cpt(this->Proj->Monitor->Vector.size());

    /* Create RVM */
    Main::Info("> Monitor before cap front %lld kmem front 0x%llX.", Cap_Front, Kom_Front);
    this->Proj->Monitor->Before_Cap_Front=Cap_Front;
    this->Proj->Monitor->Before_Kom_Front=Kom_Front;
    /* When there are virtual machines, generate everything */
    if(this->Proj->Virtual.size()!=0)
    {
        /* Two threads (sftd/main) and one endpoint (sftd) for RVM */
        Cap_Front+=3;
        Kom_Front+=2*this->Gen->Plat->Size_Thread();

        /* Create virtual machine endpoints. These endpoints does not have names.
         * Each virtual machine have two endpoints, but only one is dedicated to
         * it, so we only need to create one for each VM. */
        Main::Info("> Monitor VM endpoint cap front %lld kmem front 0x%llX.", Cap_Front, Kom_Front);
        this->Proj->Monitor->Vep_Cap_Front=Cap_Front;
        this->Proj->Monitor->Vep_Kom_Front=Kom_Front;
        Cap_Front+=ROUND_DIV(this->Proj->Virtual.size(),this->Plat->Captbl_Max);
        Kom_Front+=this->Gen->Plat->Size_Cpt(this->Proj->Virtual.size());
    }
    /* When there are no virtual machines at all, just generate one thread (sftd) and one endpoint (sftd) */
    else
    {
        Cap_Front+=2;
        Kom_Front+=this->Gen->Plat->Size_Thread();

        /* No virtual machine endpoints to create at all */
        this->Proj->Monitor->Vep_Cap_Front=Cap_Front;
        this->Proj->Monitor->Vep_Kom_Front=Kom_Front;
    }

    /* Create capability tables */
    Main::Info("> Monitor captbl cap front %lld kmem front 0x%llX.", Cap_Front, Kom_Front);
    this->Proj->Monitor->Cpt_Cap_Front=Cap_Front;
    this->Proj->Monitor->Cpt_Kom_Front=Kom_Front;
    /* Capability tables for containing capability tables */
    Cap_Front+=ROUND_DIV(this->Proj->Monitor->Captbl.size(),this->Plat->Captbl_Max);
    Kom_Front+=this->Gen->Plat->Size_Cpt(this->Proj->Monitor->Captbl.size());
    /* Capability tables themselves */
    for(class Captbl* Cpt:this->Proj->Monitor->Captbl)
        Kom_Front+=this->Gen->Plat->Size_Cpt(Cpt->Size);

    /* Create page tables */
    Main::Info("> Monitor pgtbl cap front %lld kmem front 0x%llX.", Cap_Front, Kom_Front);
    this->Proj->Monitor->Pgt_Cap_Front=Cap_Front;
    this->Proj->Monitor->Pgt_Kom_Front=Kom_Front;
    /* Capability tables for containing page tables */
    Cap_Front+=ROUND_DIV(this->Proj->Monitor->Pgtbl.size(),this->Plat->Captbl_Max);
    Kom_Front+=this->Gen->Plat->Size_Cpt(this->Proj->Monitor->Pgtbl.size());
    /* Page table themselves */
    for(class Pgtbl* Pgt:this->Proj->Monitor->Pgtbl)
        Kom_Front+=this->Gen->Plat->Size_Pgt(Pgt->Num_Order, Pgt->Is_Top);

    /* Create processes */
    Main::Info("> Monitor process cap front %lld kmem front 0x%llX.", Cap_Front, Kom_Front);
    this->Proj->Monitor->Prc_Cap_Front=Cap_Front;
    this->Proj->Monitor->Prc_Kom_Front=Kom_Front;
    /* Capability tables for containing processes */
    Cap_Front+=ROUND_DIV(this->Proj->Monitor->Process.size(),this->Plat->Captbl_Max);
    Kom_Front+=this->Gen->Plat->Size_Cpt(this->Proj->Monitor->Process.size());
    /* Processes themselves - they do not use extra memory */

    /* Create threads */
    Main::Info("> Monitor thread cap front %lld kmem front 0x%llX.", Cap_Front, Kom_Front);
    this->Proj->Monitor->Thd_Cap_Front=Cap_Front;
    this->Proj->Monitor->Thd_Kom_Front=Kom_Front;
    /* Capability tables for containing threads */
    Cap_Front+=ROUND_DIV(this->Proj->Monitor->Thread.size(),this->Plat->Captbl_Max);
    Kom_Front+=this->Gen->Plat->Size_Cpt(this->Proj->Monitor->Thread.size());
    /* Threads themselves */
    Kom_Front+=this->Proj->Monitor->Thread.size()*this->Gen->Plat->Size_Thread();

    /* Create invocations */
    Main::Info("> Monitor invocation cap front %lld kmem front 0x%llX.", Cap_Front, Kom_Front);
    this->Proj->Monitor->Inv_Cap_Front=Cap_Front;
    this->Proj->Monitor->Inv_Kom_Front=Kom_Front;
    /* Capability tables for containing invocations */
    Cap_Front+=ROUND_DIV(this->Proj->Monitor->Invocation.size(),this->Plat->Captbl_Max);
    Kom_Front+=this->Gen->Plat->Size_Cpt(this->Proj->Monitor->Invocation.size());
    /* Invocations themselves */
    Kom_Front+=this->Proj->Monitor->Invocation.size()*this->Gen->Plat->Size_Invocation();

    /* Create receive endpoints */
    Main::Info("> Monitor recv cap front %lld kmem front 0x%llX.", Cap_Front, Kom_Front);
    this->Proj->Monitor->Rcv_Cap_Front=Cap_Front;
    this->Proj->Monitor->Rcv_Kom_Front=Kom_Front;
    /* Capability tables for containing receive endpoints */
    Cap_Front+=ROUND_DIV(this->Proj->Monitor->Receive.size(),this->Plat->Captbl_Max);
    Kom_Front+=this->Gen->Plat->Size_Cpt(this->Proj->Monitor->Receive.size());
    /* Receive endpoints themselves - they do not use memory */

    /* Final pointer positions */
    Main::Info("> Monitor after cap front %lld kmem front 0x%llX.", Cap_Front, Kom_Front);
    this->Proj->Monitor->After_Cap_Front=Cap_Front;
    this->Proj->Monitor->After_Kom_Front=Kom_Front;

    /* Check if we are out of table space */
    this->Proj->Monitor->Captbl_Size=this->Proj->Monitor->Extra_Captbl+this->Proj->Monitor->After_Cap_Front;
    if(this->Proj->Monitor->Captbl_Size>this->Plat->Captbl_Max)
    {
        Main::Error("XXXXX: Monitor capability table size %lld exceeded the architectural limit %lld.",
                    this->Proj->Monitor->Captbl_Size,this->Plat->Captbl_Max);
    }
}
/* End Function:Main::Kom_Alloc *********************************************/

/* Begin Function:Main::Obj_Alloc *********************************************
Description : Allocate kernel objects.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
void Main::Obj_Alloc(void)
{
    try
    {
        /* Compute the kernel memory needed, disregarding the initial
         * capability table size because we don't know its size yet */
        Main::Info("Allocating kernel objects (first pass).");
        this->Kom_Alloc(0);
        /* Now recompute to get the real usage */
        Main::Info("Allocating kernel objects (second pass).");
        this->Kom_Alloc(this->Proj->Monitor->Captbl_Size);

        /* Populate RME information - the vector flag max is not number
         * of vectors, but the last vector number + 1 (!) */
        Main::Info("Allocating kernel memory.");
        this->Proj->Kernel->Mem_Alloc(this->Proj->Monitor->After_Kom_Front,
                                      this->Chip->Vect_Num,
                                      this->Proj->Monitor->Virt_Event,
                                      this->Plat->Wordlength);

        /* Populate RVM information */
        Main::Info("Allocating monitor memory.");
        this->Proj->Monitor->Mem_Alloc(this->Proj->Kernel->Kom_Order);

        /* Populate Process information */
        for(std::unique_ptr<class Process>& Prc:this->Proj->Process)
        {
            try
            {
                Main::Info("Allocating memory for process '%s'.",Prc->Name.c_str());
                Prc->Mem_Alloc(this->Plat->Wordlength,
                               this->Gen->Plat->Size_Register(),
                               this->Proj->Kernel->Kom_Order);
            }
            catch(std::exception& Exc)
            {
                Main::Error(std::string("Process: ")+Prc->Name+"\n"+Exc.what());
            }
        }
    }
    catch(std::exception& Exc)
    {
        Main::Error(std::string("Capability allocation:\n")+Exc.what());
    }
}
/* End Function:Main::Obj_Alloc **********************************************/

/* Begin Function:Main::Kernel_Gen ********************************************
Description : Generate kernel project.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
void Main::Kernel_Gen(void)
{
    try
    {
        /* Generate kernel folders, if they do not exist already */
        Main::Info("Creating kernel directories.");
        std::filesystem::create_directories(this->Proj->Kernel->Project_Output);
        std::filesystem::create_directories(this->Proj->Kernel->Linker_Output);
        std::filesystem::create_directories(this->Proj->Kernel->Config_Header_Output);
        std::filesystem::create_directories(this->Proj->Kernel->Boot_Header_Output);
        std::filesystem::create_directories(this->Proj->Kernel->Boot_Source_Output);
        std::filesystem::create_directories(this->Proj->Kernel->Hook_Source_Output);
        std::filesystem::create_directories(this->Proj->Kernel->Handler_Source_Output);

        /* Generate kernel configuration header */
        Main::Info("Generating kernel configuration header.");
        this->Gen->Kernel_Conf_Hdr();

        /* Generate kernel boot header/source */
        Main::Info("Generating kernel boot header.");
        this->Gen->Kernel_Boot_Hdr();
        Main::Info("Generating kernel boot source.");
        this->Gen->Kernel_Boot_Src();

        /* Generate kernel hook source */
        Main::Info("Generating kernel hook source.");
        this->Gen->Kernel_Hook_Src();

        /* Generate kernel handler source */
        Main::Info("Generating kernel vector handler sources.");
        this->Gen->Kernel_Handler_Src();

        /* Generate kernel linker script */
        Main::Info("Generating kernel linker script.");
        this->Gen->Kernel_Linker();

        /* Generate kernel project */
        Main::Info("Generating kernel project.");
        this->Gen->Kernel_Proj();
    }
    catch(std::exception& Exc)
    {
        Main::Error(std::string("Kernel project generation:\n")+Exc.what());
    }
}
/* End Function:Main::Kernel_Gen *********************************************/

/* Begin Function:Main::Monitor_Gen *******************************************
Description : Generate monitor project.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
void Main::Monitor_Gen(void)
{
    try
    {
        /* Generate monitor folders, if they do not exist already */
        Main::Info("Creating monitor directories.");
        std::filesystem::create_directories(this->Proj->Monitor->Project_Output);
        std::filesystem::create_directories(this->Proj->Monitor->Linker_Output);
        std::filesystem::create_directories(this->Proj->Monitor->Config_Header_Output);
        std::filesystem::create_directories(this->Proj->Monitor->Boot_Header_Output);
        std::filesystem::create_directories(this->Proj->Monitor->Boot_Source_Output);
        std::filesystem::create_directories(this->Proj->Monitor->Hook_Source_Output);

        /* Generate monitor configuration header */
        Main::Info("Generating monitor configuration header.");
        this->Gen->Monitor_Conf_Hdr();

        /* Generate monitor boot header/source */
        Main::Info("Generating monitor boot header.");
        this->Gen->Monitor_Boot_Hdr();
        Main::Info("Generating monitor boot source.");
        this->Gen->Monitor_Boot_Src();

        /* Generate monitor hook source */
        Main::Info("Generating monitor hook source.");
        this->Gen->Monitor_Hook_Src();

        /* Generate monitor linker script */
        Main::Info("Generating monitor linker script.");
        this->Gen->Monitor_Linker();

        /* Generate monitor project */
        Main::Info("Generating monitor project.");
        this->Gen->Monitor_Proj();
    }
    catch(std::exception& Exc)
    {
        Main::Error(std::string("Monitor project generation:\n")+Exc.what());
    }
}
/* End Function:Main::Monitor_Gen ********************************************/

/* Begin Function:Main::Process_Gen *******************************************
Description : Generate process projects.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
void Main::Process_Gen(void)
{
    for(const std::unique_ptr<class Process>& Prc:this->Proj->Process)
    {
        try
        {
            /* Generate process folders, if they do not exist already */
            Main::Info("Creating directories for process '%s'.", Prc->Name.c_str());
            std::filesystem::create_directories(Prc->Project_Output);
            std::filesystem::create_directories(Prc->Linker_Output);
            std::filesystem::create_directories(Prc->Main_Header_Output);
            std::filesystem::create_directories(Prc->Main_Source_Output);
            if(Prc->Type==PROCESS_VIRTUAL)
            {
                std::filesystem::create_directories(static_cast<class Virtual*>(Prc.get())->Virtual_Header_Output);
                std::filesystem::create_directories(static_cast<class Virtual*>(Prc.get())->Virtual_Source_Output);
            }

            /* Generate process main header */
            Main::Info("Generating process main header.");
            this->Gen->Process_Main_Hdr(Prc.get());

            /* Generate process main source */
            Main::Info("Generating process main source.");
            this->Gen->Process_Desc_Src(Prc.get());
            this->Gen->Process_Main_Src(Prc.get());

            /* If this process is a virtual machine, generate VM configuration header as well */
            if(Prc->Type==PROCESS_VIRTUAL)
            {
                Main::Info("Generating virtual machine configuration file.");
                this->Gen->Process_Virt_Hdr(static_cast<class Virtual*>(Prc.get()));
                this->Gen->Process_Virt_Src(static_cast<class Virtual*>(Prc.get()));
            }
            /* Generate process entry source */
            else
            {
                Main::Info("Generating process entry source.");
                this->Gen->Process_Entry_Src(Prc.get());
            }

            /* Generate monitor linker script */
            Main::Info("Generating process linker script.");
            this->Gen->Process_Linker(Prc.get());

            /* Generate monitor project */
            Main::Info("Generating process project.");
            this->Gen->Process_Proj(Prc.get());
        }
        catch(std::exception& Exc)
        {
            Main::Error(std::string("Process '")+Prc->Name+"' project generation:\n"+Exc.what());
        }
    }
}
/* End Function:Main::Process_Gen ********************************************/

/* Begin Function:Main::Workspace_Gen *****************************************
Description : Generate workspace, if required.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
void Main::Workspace_Gen(void)
{
    try
    {
        /* Generate workspace project */
        Main::Info("Generating workspace project.");
        this->Gen->Workspace_Proj();
    }
    catch(std::exception& Exc)
    {
        Main::Error(std::string("Workspace generation:\n")+Exc.what());
    }
}
/* End Function:Main::Workspace_Gen ******************************************/

/* Begin Function:Main::Report_Gen ********************************************
Description : Generate after report. This is currently not used.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
void Main::Report_Gen(void)
{
    try
    {
    	/* Report kernel code section */

    	/* Report kernel total data section */

    	/* Report kotbl space */
    }
    catch(std::exception& Exc)
    {
        Main::Error(std::string("Report generation:\n")+Exc.what());
    }
}
/* End Function:Main::Report_Gen *********************************************/

/* Begin Function:Main::Main **************************************************
Description : Preprocess the input parameters, and generate a preprocessed
              instruction listing with all the comments stripped.
Input       : int argc - The number of arguments.
              char* argv[] - The arguments.
Output      : s8_t** Input_File - The input project file path.
              s8_t** Output_File - The output folder path, must be empty.
              s8_t** Format - The output format.
Return      : None.
******************************************************************************/
/* void */ Main::Main(int argc, char* argv[])
{
    cnt_t Count;
    s8_t Date[64];
    time_t Time_Value;
    struct tm* Time_Struct;

    try
    {
        Count=1;
        /* Read the command line one by one */
        while(Count<argc)
        {
            /* Input project file */
            if(strcmp(argv[Count],"-i")==0)
            {
                if(this->Input!="")
                    Main::Error("XXXXX: More than one input file specified.");
                this->Input=argv[Count+1];
                Count+=2;
            }
            /* Verbose mode */
            else if(strcmp(argv[Count],"-v")==0)
            {
                Main::Verbose=1;
                Main::Info("Verbose output mode enabled.");
                Count+=1;
            }
            /* Mock mode */
            else if(strcmp(argv[Count],"-m")==0)
            {
                Main::Mock=1;
                Main::Info("Mock generation mode enabled.");
                Count+=1;
            }
            else
                Main::Error("XXXXX: Unrecognized command line argument.");
        }

        if(this->Input=="")
            Main::Error("XXXXX: No input file specified.");

        this->Buffer=(char*)malloc(MAX_FILE_SIZE);
        if(this->Buffer==nullptr)
            Main::Error(std::string("XXXXX: File buffer allocation failure."));

        /* Read current time */
        time(&Time_Value);
        Time_Struct=localtime(&Time_Value);
        sprintf(Date,"%02d/%02d/%d %02d:%02d:%02d",
                     Time_Struct->tm_mday, Time_Struct->tm_mon+1, Time_Struct->tm_year+1900,
                     Time_Struct->tm_hour, Time_Struct->tm_min, Time_Struct->tm_sec);
        this->Time=Date;
    }
    catch(std::exception& Exc)
    {
        Main::Error(std::string("Command line parsing:\n")+Exc.what()+"\n"+
                                "Usage: -i input.rvp\n"
                                "       -i: Project description file.\n");
    }
}
/* End Function:Main::Main ***************************************************/

/* Begin Function:Main::XML_Get_String ****************************************
Description : Get strings from the XML entry.
Input       : xml_node_t* Root - The pointer to the root node.
              const std::string& Name - The entry to look for.
              const std::string& Errno0 - The error number 0.
              const std::string& Errno1 - The error number 1.
Output      : None.
Return      : std::string - The string extracted.
******************************************************************************/
std::string Main::XML_Get_String(xml_node_t* Root, const std::string& Name,
                                 const std::string& Errno0, const std::string& Errno1)
{
    xml_node_t* Temp;

    if(Name=="")
    {
        if(Root->XML_Val_Len==0)
            Main::Error(Errno1+": Root section is empty.");
        return std::string(Root->XML_Val,(int)Root->XML_Val_Len);
    }
    else
    {
        if((XML_Child(Root, (xml_s8_t*)Name.c_str(), &Temp)<0)||(Temp==0))
            Main::Error(Errno0+": '"+Name+"' section is missing.");
        if(Temp->XML_Val_Len==0)
            Main::Error(Errno1+": '"+Name+"' section is empty.");
        return std::string(Temp->XML_Val,(int)Temp->XML_Val_Len);
    }
}
/* End Function:Main::XML_Get_String *****************************************/

/* Begin Function:Main::XML_Get_Number ****************************************
Description : Get numbers from the XML entry.
Input       : xml_node_t* Root - The pointer to the root node.
              const std::string& Name - The entry to look for.
              const std::string& Errno0 - The error number 0.
              const std::string& Errno1 - The error number 1.
Output      : None.
Return      : ptr_t - The number extracted.
******************************************************************************/
ptr_t Main::XML_Get_Number(xml_node_t* Root, const std::string& Name,
                           const std::string& Errno0, const std::string& Errno1)
{
    return std::stoull(Main::XML_Get_String(Root, Name, Errno0, Errno1), 0, 0);
}
/* End Function:Main::XML_Get_Number *****************************************/

/* Begin Function:Main::XML_Get_Yesno ****************************************
Description : Get strings from the XML entry.
Input       : xml_node_t* Root - The pointer to the root node.
              const std::string& Name - The entry to look for.
              const std::string& Errno0 - The error number 0.
              const std::string& Errno1 - The error number 1.
Output      : None.
Return      : std::string - The string extracted.
******************************************************************************/
ptr_t Main::XML_Get_Yesno(xml_node_t* Root, const std::string& Name,
                          const std::string& Errno0, const std::string& Errno1)
{
    if(Main::XML_Get_String(Root, Name, Errno0, Errno1)=="Yes")
        return 1;

    return 0;
}
/* End Function:Main::XML_Get_String *****************************************/

/* Begin Function:Main::XML_Get_CSV *******************************************
Description : Get comma-separated values from the XML entry.
Input       : xml_node_t* Root - The pointer to the root node.
              const std::string& Name - The entry to look for.
              const std::string& Errno0 - The error number 0.
              const std::string& Errno1 - The error number 1.
Output      : std::vector<std::string>& Vector - The CSV extracted.
Return      : None.
******************************************************************************/
void Main::XML_Get_CSV(xml_node_t* Root, const std::string& Name,
                       std::vector<std::string>& Vector,
                       const std::string& Errno0, const std::string& Errno1)
{
    cnt_t Pivot;
    ptr_t Begin;
    ptr_t End;
    std::string Temp;
    std::string Push;

    Temp=Main::XML_Get_String(Root, Name, Errno0, Errno1);
    do
    {
        /* Split the element */
        Pivot=Temp.find_first_of(",");
        if(Pivot<0)
            Push=Temp;
        else
        {
            Push=Temp.substr(0, Pivot);
            Temp=Temp.substr(Pivot+1);
        }
        /* Strip the whitespaces */
        Begin=Push.find_first_not_of(" \t\v");
        if(Begin!=std::string::npos)
        {
            End=Push.find_last_not_of(" \t\v");
            Vector.push_back(Push.substr(Begin, End-Begin+1));
        }
    }
    while(Pivot>=0);
}
/* End Function:Main::XML_Get_CSV ********************************************/

/* Begin Function:Main::XML_Get_KVP *******************************************
Description : Get key-value pairs from the XML entry.
Input       : xml_node_t* Root - The pointer to the root node.
              const std::string& Name - The entry to look for.
              const std::string& Errno0 - The error number 0.
              const std::string& Errno1 - The error number 1.
Output      : std::map<std::string,std::string>& Map - The key-value pairs.
Return      : None.
******************************************************************************/
void Main::XML_Get_KVP(xml_node_t* Root, const std::string& Name,
                       std::map<std::string,std::string>& Map,
                       const std::string& Errno0, const std::string& Errno1)
{
    xml_node_t* Trunk;
    xml_node_t* Temp;

    if((XML_Child(Root, (xml_s8_t*)Name.c_str(), &Trunk)<0)||(Trunk==0))
        Main::Error(Errno0+": '"+Name+"' section is missing.");
    if(XML_Child(Trunk, 0, &Temp)<0)
        Main::Error(Errno1+": '"+Name+"' section parsing internal error.");
    while(Temp!=nullptr)
    {
        Map.insert(std::make_pair(std::string(Temp->XML_Tag, (int)Temp->XML_Tag_Len),
                                  std::string(Temp->XML_Val, (int)Temp->XML_Val_Len)));
        if(XML_Child(Trunk, (xml_s8_t*)"", &Temp)<0)
            Main::Error(Errno1+": '"+Name+"' section parsing internal error.");
    }
}
/* End Function:Main::XML_Get_KVP *******************************************/

/* Begin Function:Main::Idtfr_Check *******************************************
Description : Check if the identifier supplied is valid. Valid identifiers must
              contain. If not, we throw an error.
Input       : const std::string& Idtfr - The identifier.
              const std::string& Name - The section name.
              const std::string& Errno0 - The first error number.
              const std::string& Errno1 - The second error number.
Output      : None.
Return      : None.
******************************************************************************/
void Main::Idtfr_Check(const std::string& Idtfr, const std::string& Name,
                       const std::string& Errno0, const std::string& Errno1)
{
    /* This can't be empty, we should have checked this field before in XML_Get_String */
    ASSERT(Idtfr.length()!=0);
    if((Idtfr[0]>='0')&&(Idtfr[0]<='9'))
        Main::Error(Errno0+": '"+Name+"' section begins with a number.");
    for(const char& Char:Idtfr)
    {
        if(((Char<'a')||(Char>'z'))&&
           ((Char<'A')||(Char>'Z'))&&
           ((Char<'0')||(Char>'9'))&&
           (Char!='_'))
        Main::Error(Errno1+": '"+Name+"' section contains an invalid character.");
    }
}
/* End Function:Main::Idtfr_Check ********************************************/

/* Begin Function:Main::Dir_Fixup *********************************************
Description : Fix up all directory paths with a lagging '/'.
Input       : std::string& Dir - The directory path.
Output      : std::string& Dir - The guaranteed correct directory path.
Return      : None.
******************************************************************************/
void Main::Dir_Fixup(std::string& Dir)
{
    if((Dir.back()!='/')&&(Dir.back()!='\\'))
        Dir+='/';
}
/* End Function:Main::Dir_Fixup **********************************************/

/* Begin Function:Main::Hex ***************************************************
Description : Convert a number to a hex string, without 0x prefix.
Input       : ptr_t Number - The number to convert.
Output      : None.
Return      : std::string - The string created.
******************************************************************************/
std::string Main::Hex(ptr_t Number)
{
    char Buf[64];

    sprintf(Buf,"%llX",Number);
    return Buf;
}
/* End Function:Main::Hex ****************************************************/

/* Begin Function:Main::Upper *************************************************
Description : Convert the string to uppercase.
Input       : std::string& Str - The string to convert.
Output      : std::string& Str - The converted string.
Return      : None.
******************************************************************************/
void Main::Upper(std::string& Str)
{
    std::transform(Str.begin(), Str.end(), Str.begin(), (int(*)(int))std::toupper);
}
/* End Function:Main::Upper **************************************************/

/* Begin Function:Main::Lower *************************************************
Description : Convert the string to lowercase.
Input       : std::string& Str - The string to convert.
Output      : std::string& Str - The converted string.
Return      : None.
******************************************************************************/
void Main::Lower(std::string& Str)
{
    std::transform(Str.begin(), Str.end(), Str.begin(), (int(*)(int))std::tolower);
}
/* End Function:Main::Lower **************************************************/

/* Begin Function:Main::Strcicmp **********************************************
Description : Compare two strings in a case insensitive way.
Input       : const std::string& Str1 - The first string.
              const std::string& Str2 - The second string.
Output      : None.
Return      : ret_t - If two strings are equal, then 0; if not, -1.
******************************************************************************/
ret_t Main::Strcicmp(const std::string& Str1, const std::string& Str2)
{
    bool Equal;

    Equal=std::equal(Str1.begin(), Str1.end(), Str2.begin(), Str2.end(),
                     [](s8_t Char1, s8_t Char2)
                     {
                         return std::tolower(Char1)==std::tolower(Char2);
                     });

    if(Equal==true)
        return 0;

    return -1;
}
/* End Function:Main::Strcicmp ***********************************************/

/* Begin Function:Main::Info **************************************************
Description : Output information in verbose mode.
Input       : const char* Format - The printf format.
              ... - Additional printf arguments.
Output      : None.
Return      : None.
******************************************************************************/
void Main::Info(const char* Format, ...)
{
    va_list Arg;
    va_start(Arg, Format);
    if(Main::Verbose!=0)
    {
        printf("Info: ");
        vprintf(Format, Arg);
        printf("\n");
    }
    va_end(Arg);
}
/* End Function:Main::Info ***************************************************/

/* Begin Function:Main::Info **************************************************
Description : Output information in verbose mode.
Input       : const std::string& String - The string.
Output      : None.
Return      : None.
******************************************************************************/
void Main::Info(const std::string& Format)
{
    if(Main::Verbose!=0)
        printf("Info: %s\n", Format.c_str());
}
/* End Function:Main::Info ***************************************************/

/* Begin Function:Main::Warning ***********************************************
Description : Output warning.
Input       : const char* Format - The printf format.
              ... - Additional printf arguments.
Output      : None.
Return      : None.
******************************************************************************/
void Main::Warning(const char* Format, ...)
{
    va_list Arg;
    va_start(Arg, Format);

    printf("Warning: ");
    vprintf(Format, Arg);
    printf("\n");

    va_end(Arg);
}
/* End Function:Main::Warning ************************************************/

/* Begin Function:Main::Warning ***********************************************
Description : Output warning.
Input       : const std::string& String - The string.
Output      : None.
Return      : None.
******************************************************************************/
void Main::Warning(const std::string& Format)
{
    printf("Warning: %s\n", Format.c_str());
}
/* End Function:Main::Warning ************************************************/

/* Begin Function:Main::Error *************************************************
Description : Throw an error.
Input       : const std::string& Format - The string.
Output      : None.
Return      : None.
******************************************************************************/
void Main::Error[[noreturn]](const std::string& Format)
{
    throw std::runtime_error(Format);
}
/* End Function:Main::Error **************************************************/

/* Begin Function:Main::Error *************************************************
Description : Throw an error.
Input       : const char* Format - The printf format.
              ... - Additional printf arguments.
Output      : None.
Return      : None.
******************************************************************************/
void Main::Error[[noreturn]](const char* Format, ...)
{
    char Temp[512];
    va_list Arg;
    va_start(Arg, Format);

    vsprintf(Temp,Format, Arg);
    Main::Error(std::string(Temp));

    va_end(Arg);
}
/* End Function:Main::Error **************************************************/
}
/* Begin Function:Malloc ******************************************************
Description : Allocate some memory and register it with the system.
Input       : ptr_t Size - The size to allocate, in bytes.
Output      : None.
Return      : void* - If successful, the address; else 0.
******************************************************************************/
extern "C" void* Malloc(ptr_t Size)
{
    return malloc((size_t)Size);
}
/* End Function:Malloc *******************************************************/

/* Begin Function:Free ********************************************************
Description : Deallocate the memory and deregister it.
Input       : void* Addr - The address to free.
Output      : None.
Return      : None.
******************************************************************************/
extern "C" void Free(void* Addr)
{
    free(Addr);
}
/* End Function:Free *********************************************************/

/* Begin Function:Memcpy ******************************************************
Description : Memcpy wrapper for 64-bit XML library.
Input       : void* Src - The source string.
              xml_ptr_t Num - The number to copy.
Output      : void* Dst - The destination string.
Return      : void* - The destination is returned.
******************************************************************************/
extern "C" void* Memcpy(void* Dst, void* Src, xml_ptr_t Num)
{
    return memcpy(Dst, Src, (size_t)Num);
}
/* End Function:Memcpy *******************************************************/

/* Begin Function:Strncmp *****************************************************
Description : Strncmp wrapper for 64-bit XML library.
Input       : s8_t* Str1 - The first string.
              s8_t* Str2 - The second string.
              ptr_t Num - The number of characters to compare.
Output      : None.
Return      : ret_t - If Str1 is bigger, positive; if equal, 0; if Str2 is bigger,
                      negative.
******************************************************************************/
extern "C" ret_t Strncmp(s8_t* Str1, s8_t* Str2, ptr_t Num)
{
    return strncmp((const char*)Str1,(const char*)Str2,(size_t)Num);
}
/* End Function:Strncmp ******************************************************/

/* Begin Function:Strlen ******************************************************
Description : Strlen wrapper for 64-bit XML library.
Input       : s8_t* Str - The Input string.
Output      : None.
Return      : ptr_t - The length of the string.
******************************************************************************/
extern "C" ptr_t Strlen(s8_t* Str)
{
    return strlen((const char*)Str);
}
/* End Function:Strlen *******************************************************/

/* Begin Function:main ********************************************************
Description : The entry of the tool.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
using namespace RVM_GEN;
int main(int argc, char* argv[])
{
    try
    {
        std::unique_ptr<class Main> Main;
/* Phase 1: Process command line and do parsing ******************************/
        Main=std::make_unique<class Main>(argc, argv);
        Main::Info("Running generator.");
        Main->Parse();
        Main->Check();
        Main->Setup();

/* Phase 2: Allocate memory **************************************************/
        Main->Mem_Align();
        Main->Mem_Alloc();

/* Phase 3: Set up kernel objects for each process ***************************/
        Main->Shmem_Add();
        Main->Pgt_Alloc();

/* Phase 3: Allocate kernel object IDs & macros & kernel objects *************/
        Main->Cap_Alloc();
        Main->Cap_Link();
        Main->Obj_Alloc();

/* Phase 4: Produce output ***************************************************/
        if(Main::Mock==0)
        {
            Main->Kernel_Gen();
            Main->Monitor_Gen();
            Main->Process_Gen();
            Main->Workspace_Gen();
        }
        Main->Report_Gen();
    }
    catch(std::exception& Exc)
    {
        std::cout<<(std::string("Error:\n")+Exc.what());
    }

    return 0;
}
/* End Function:main *********************************************************/

/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
