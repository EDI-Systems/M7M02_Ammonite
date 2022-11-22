/******************************************************************************
Filename    : rme_mcu.cpp
Author      : pry
Date        : 20/04/2019
Licence     : LGPL v3+; see COPYING for details.
Description : The configuration generator for the MCU ports. This does not
              apply to the desktop or mainframe port; it uses its own generator.
              This generator includes 12 big steps, and is considerably complex.
               1. Process the command line arguments and figure out where the source
                  are located at.
               2. Read the project-level configuration XMLs and device-level 
                  configuration XMLs into its internal data structures. Should
                  we find any parsing errors, we report and error out.
               3. Align memory. For program memory and data memory, rounding their
                  size is allowed; for specifically pointed out memory, rounding
                  their size is not allowed. 
               4. Generate memory map. This places all the memory segments into
                  the memory map, and fixes their specific size, etc. 
               5. Check if the generated memory map is valid. Each process should have
                  at least one code section and one data section, and they shall all
                  be STATIC.
               6. Allocate local and global linear capability IDs for all kernel objects.
                  The global linear capability ID assumes that all capability in the
                  same class are in the same capability table, and in 32-bit systems
                  this may not be the case.
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

#include "map"
#include "list"
#include "string"
#include "memory"
#include "vector"
#include "iostream"
#include "iterator"
#include "stdexcept"
#include "algorithm"

#define __HDR_DEFS__
#include "rvm_gen.hpp"
#include "Proj_Info/proj_info.hpp"
#include "Chip_Info/chip_info.hpp"
#include "Mem_Info/mem_info.hpp"
#undef __HDR_DEFS__

#define __HDR_CLASSES__
#include "rvm_gen.hpp"
#include "Proj_Info/proj_info.hpp"
#include "Proj_Info/Chip/chip.hpp"
#include "Proj_Info/Kernel/kernel.hpp"
#include "Proj_Info/Monitor/monitor.hpp"
#include "Proj_Info/Process/process.hpp"
#include "Proj_Info/Process/Thread/thread.hpp"
#include "Proj_Info/Process/Invocation/invocation.hpp"
#include "Proj_Info/Process/Port/port.hpp"
#include "Proj_Info/Process/Receive/receive.hpp"
#include "Proj_Info/Process/Send/send.hpp"
#include "Proj_Info/Process/Kfunc/kfunc.hpp"
#include "Chip_Info/chip_info.hpp"
#include "Chip_Info/Config/config.hpp"
#include "Vect_Info/vect_info.hpp"
#include "Mem_Info/mem_info.hpp"
#undef __HDR_CLASSES__
/* End Includes **************************************************************/
namespace RVM_GEN
{
/* Global Variables **********************************************************/
ptr_t Main::Verbose=0;
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
            Main::Error(std::string("T1001: '")+this->Input+"': No such project file or cannot be opened.");
        this->Buffer[fread(this->Buffer,1,MAX_FILE_SIZE,File)]='\0';
        if(strlen(this->Buffer)==0)
        {
            File=nullptr;
            Main::Error(std::string("T1002: '")+this->Input+"': Project file is empty.");
        }
        fclose(File);
        File=nullptr;

        /* Parse and then release the parsing tree */
        if(XML_Parse(&Root,(xml_s8_t*)(this->Buffer))!=0)
        {
            Root=nullptr;
            Main::Error(std::string("T1003: Project file parsing internal error."));
        }
        this->Proj=std::make_unique<class Proj_Info>(Root);
        /* We have done parsing, release the parsing tree */
        if(XML_Del(Root)!=0)
        {
            Root=nullptr;
            Main::Error(std::string("T1003: Project file parsing internal error."));
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
        Path=this->Proj->Monitor->Monitor_Root+"/Include/Platform/"+
             this->Proj->Chip->Platform+"/Chip/"+
             this->Proj->Chip->Class+"/"+
             "rvm_platform_"+this->Proj->Chip->Class+".rvc";
        /* Read in the whole chip description file */
        File=fopen(Path.c_str(),"r");
        if(File==nullptr)
            Main::Error(std::string("T1001: '")+Path+"': No such chip description file or cannot be opened.");
        this->Buffer[fread(this->Buffer,1,MAX_FILE_SIZE,File)]='\0';
        if(strlen(this->Buffer)==0)
        {
            File=nullptr;
            Main::Error(std::string("T1002: '")+Path+"': Chip description file is empty.");
        }
        fclose(File);
        File=nullptr;

        /* Parse and then release the parsing tree */
        if(XML_Parse(&Root,(xml_s8_t*)(this->Buffer))!=0)
        {
            Root=nullptr;
            Main::Error(std::string("T1003: Chip description file parsing internal error."));
        }
        this->Chip=std::make_unique<class Chip_Info>(Root);
        /* We have done parsing, release the parsing tree */
        if(XML_Del(Root)!=0)
        {
            Root=nullptr;
            Main::Error(std::string("T1003: Chip description file parsing internal error."));
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

/* Begin Function:Main::Parse *************************************************
Description : Parse the files.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
void Main::Parse(void)
{
    char Buffer[1024];
    try
    {
        /* Parse project file */
        this->Proj_Parse();
        /* Change current path - everything is now relative to the project */
        Buffer[0]=chdir(this->Input.substr(0,this->Input.find_last_of("/\\")+1).c_str());
        getcwd(Buffer,1024);
        /* Parse chip description file */
        this->Chip_Parse();
    }
    catch(std::exception& Exc)
    {
        throw std::runtime_error(std::string("Parse:\n")+Exc.what());
    }
}
/* End Function:Main::Parse **************************************************/

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
        /* Check each configuration file individually */
        this->Proj->Check();
        this->Chip->Check();

        /* Check project options */

        /* Merge memory from chip and EMIF */
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
        /* Make sure all mounted memory do not overlap */
        Mem_Info::Overlap_Check(this->Proj->Memory_Code,this->Proj->Memory_Data,this->Proj->Memory_Device);

        /* Check shared memory references */

        /* Check port references */

        /* Check vector references */

        /* Check send references */
    }
    catch(std::exception& Exc)
    {
        throw std::runtime_error(std::string("Check:\n")+Exc.what());
    }
}
/* End Function:Main::Check **************************************************/

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
                    Main::Error("C0001: More than one input file specified.");
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
            else
                Main::Error("C0004: Unrecognized command line argument.");
        }

        if(this->Input=="")
            Main::Error("C0005: No input file specified.");

        this->Buffer=(char*)malloc(MAX_FILE_SIZE);
        if(this->Buffer==nullptr)
            Main::Error(std::string("T0000: File buffer allocation failure."));

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
              const char* Name - The entry to look for.
              const char* Errno0 - The error number 0.
              const char* Errno1 - The error number 1.
Output      : None.
Return      : std::string - The string extracted.
******************************************************************************/
std::string Main::XML_Get_String(xml_node_t* Root, const char* Name,
                                 const char* Errno0, const char* Errno1)
{
    xml_node_t* Temp;

    if((XML_Child(Root,(xml_s8_t*)Name,&Temp)<0)||(Temp==0))
        Main::Error(std::string(Errno0)+": '"+Name+"' section is missing.");
    if(Temp->XML_Val_Len==0)
        Main::Error(std::string(Errno1)+": '"+Name+"' section is empty.");
    return std::string(Temp->XML_Val,(int)Temp->XML_Val_Len);
}
/* End Function:Main::XML_Get_String *****************************************/

/* Begin Function:Main::XML_Get_Number ****************************************
Description : Get numbers from the XML entry.
Input       : xml_node_t* Root - The pointer to the root node.
              const char* Name - The entry to look for.
              const char* Errno0 - The error number 0.
              const char* Errno1 - The error number 1.
Output      : None.
Return      : ptr_t - The number extracted.
******************************************************************************/
ptr_t Main::XML_Get_Number(xml_node_t* Root, const char* Name,
                           const char* Errno0, const char* Errno1)
{
    return std::stoull(Main::XML_Get_String(Root,Name,Errno0,Errno1),0,0);
}
/* End Function:Main::XML_Get_Number *****************************************/

/* Begin Function:Main::XML_Get_Yesno ****************************************
Description : Get strings from the XML entry.
Input       : xml_node_t* Root - The pointer to the root node.
              const char* Name - The entry to look for.
              const char* Errno0 - The error number 0.
              const char* Errno1 - The error number 1.
Output      : None.
Return      : std::string - The string extracted.
******************************************************************************/
ptr_t Main::XML_Get_Yesno(xml_node_t* Root, const char* Name,
                          const char* Errno0, const char* Errno1)
{
    if(Main::XML_Get_String(Root,Name,Errno0,Errno1)=="Yes")
        return 1;

    return 0;
}
/* End Function:Main::XML_Get_String *****************************************/

/* Begin Function:Main::XML_Get_CSV *******************************************
Description : Get comma-separated values from the XML entry.
Input       : xml_node_t* Root - The pointer to the root node.
              const char* Name - The entry to look for.
              const char* Errno0 - The error number 0.
              const char* Errno1 - The error number 1.
Output      : std::vector<std::string>& Vector - The CSV extracted.
Return      : None.
******************************************************************************/
void Main::XML_Get_CSV(xml_node_t* Root, const char* Name,
                       std::vector<std::string>& Vector,
                       const char* Errno0, const char* Errno1)
{
    cnt_t Pivot;
    ptr_t Begin;
    ptr_t End;
    std::string Temp;
    std::string Push;

    Temp=Main::XML_Get_String(Root,Name,Errno0,Errno1);
    do
    {
        /* Split the element */
        Pivot=Temp.find_first_of(",");
        if(Pivot<0)
            Push=Temp;
        else
        {
            Push=Temp.substr(0,Pivot);
            Temp=Temp.substr(Pivot+1);
        }
        /* Strip the whitespaces */
        Begin=Push.find_first_not_of(" \t\v");
        if(Begin!=std::string::npos)
        {
            End=Push.find_last_not_of(" \t\v");
            Vector.push_back(Push.substr(Begin,End-Begin+1));
        }
    }
    while(Pivot>=0);
}
/* End Function:Main::XML_Get_CSV ********************************************/

/* Begin Function:Main::XML_Get_KVP *******************************************
Description : Get key-value pairs from the XML entry.
Input       : xml_node_t* Root - The pointer to the root node.
              const char* Name - The entry to look for.
              const char* Errno0 - The error number 0.
              const char* Errno1 - The error number 1.
Output      : std::map<std::string,std::string>& Map - The key-value pairs.
Return      : None.
******************************************************************************/
void Main::XML_Get_KVP(xml_node_t* Root, const char* Name,
                       std::map<std::string,std::string>& Map,
                       const char* Errno0, const char* Errno1)
{
    xml_node_t* Trunk;
    xml_node_t* Temp;

    if((XML_Child(Root,(xml_s8_t*)Name,&Trunk)<0)||(Trunk==0))
        Main::Error(std::string(Errno0)+": '"+Name+"' section is missing.");
    if(XML_Child(Trunk,0,&Temp)<0)
        Main::Error(std::string(Errno1)+": '"+Name+"' section parsing internal error.");
    while(Temp!=nullptr)
    {
        Map.insert(std::make_pair(std::string(Temp->XML_Tag,(int)Temp->XML_Tag_Len),
                                  std::string(Temp->XML_Val,(int)Temp->XML_Val_Len)));
        if(XML_Child(Trunk,(xml_s8_t*)"",&Temp)<0)
            Main::Error(std::string(Errno1)+": '"+Name+"' section parsing internal error.");
    }
}
/* End Function:Main::XML_Get_KVP *******************************************/

/* Begin Function:Main::Idtfr_Check *******************************************
Description : Check if the identifier supplied is valid. Valid identifiers must
              contain. If not, we throw an error.
Input       : const std::string& Idtfr - The identifier.
              const char* Name - The section name.
              const char* Errno0 - The first error number.
              const char* Errno1 - The second error number.
Output      : None.
Return      : None.
******************************************************************************/
void Main::Idtfr_Check(const std::string& Idtfr, const char* Name,
                       const char* Errno0, const char* Errno1)
{
    /* This can't be empty, we should have checked this field before in XML_Get_String */
    ASSERT(Idtfr.length()!=0);
    if((Idtfr[0]>='0')&&(Idtfr[0]<='9'))
        Main::Error(std::string(Errno0)+": '"+Name+"' section begins with a number.");
    for(const char& Char:Idtfr)
    {
        if(((Char<'a')||(Char>'z'))&&
           ((Char<'A')||(Char>'Z'))&&
           ((Char<'0')||(Char>'9'))&&
           (Char!='_'))
        Main::Error(std::string(Errno1)+": '"+Name+"' section contains an invalid character.");
    }
}
/* End Function:Main::Idtfr_Check ********************************************/

/* Begin Function:Main::Info **************************************************
Description : Output information about the compile/assemble/link process in verbose mode.
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
Description : Output information about the generation process in verbose mode.
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
        Main::Info("Reading project.");
        Main->Parse();
        Main->Check();

/* Phase 2: Allocate page tables *********************************************/
        //Main->Plat->Align_Mem(Main->Proj);
        //Main->Alloc_Mem();
        //Main->Plat->Alloc_Pgtbl(Main->Proj,Main->Chip);

/* Phase 3: Allocate kernel object ID & macros *******************************/
        //Main->Alloc_Cap();

/* Phase 4: Allocate object memory *******************************************/
        //Main->Alloc_Obj();

/* Phase 5: Produce output ***************************************************/
        //Main->Gen_RME();
        //Main->Gen_RVM();
        //Main->Gen_Proc();
        //Main->Gen_Proj();

/* Phase 6: Generate report **************************************************/
        //Main->Gen_Report();
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
