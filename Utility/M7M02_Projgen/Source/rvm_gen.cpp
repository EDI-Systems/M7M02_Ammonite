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
#include "pbfs.h"
}

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
#include "Main/rme_fsys.hpp"
#include "Main/rme_chip.hpp"
#include "Main/rme_comp.hpp"
#include "Main/rme_raw.hpp"
#include "Main/rme_mem.hpp"

#include "Kobj/rme_kobj.hpp"
#include "Kobj/rme_captbl.hpp"
#include "Kobj/rme_pgtbl.hpp"
#include "Kobj/rme_thd.hpp"
#include "Kobj/rme_inv.hpp"
#include "Kobj/rme_port.hpp"
#include "Kobj/rme_recv.hpp"
#include "Kobj/rme_send.hpp"
#include "Kobj/rme_vect.hpp"
#include "Kobj/rme_kern.hpp"
#include "Kobj/rme_proc.hpp"

#include "Main/rme_proj.hpp"

#include "Gen/rme_doc.hpp"
#include "Gen/rme_genrme.hpp"
#include "Gen/rme_genrvm.hpp"
#include "Gen/rme_genproc.hpp"
#include "Gen/rme_genproj.hpp"
#undef __HDR_DEFS__

#define __HDR_CLASSES__
#include "Main/rme_fsys.hpp"
#include "Main/rme_chip.hpp"
#include "Main/rme_comp.hpp"
#include "Main/rme_raw.hpp"
#include "Main/rme_mem.hpp"

#include "Kobj/rme_kobj.hpp"
#include "Kobj/rme_captbl.hpp"
#include "Kobj/rme_pgtbl.hpp"
#include "Kobj/rme_thd.hpp"
#include "Kobj/rme_inv.hpp"
#include "Kobj/rme_port.hpp"
#include "Kobj/rme_recv.hpp"
#include "Kobj/rme_send.hpp"
#include "Kobj/rme_vect.hpp"
#include "Kobj/rme_kern.hpp"
#include "Kobj/rme_proc.hpp"

#include "Main/rme_proj.hpp"
#include "Main/rme_mcu.hpp"

#include "Gen/rme_doc.hpp"
#include "Gen/rme_genrme.hpp"
#include "Gen/rme_genrvm.hpp"
#include "Gen/rme_genproc.hpp"
#include "Gen/rme_genproj.hpp"
#undef __HDR_CLASSES__

#include "A7M/rme_a7m_mcu.hpp"
/* End Includes **************************************************************/
namespace RVM_GEN
{
/* Begin Function:Main::XML_Get_String ****************************************
Description : Get strings from the XML entry.
Input       : xml_node_t* Root - The pointer to the root node.
              const char* Name - The entry to look for.
              const char* Errno0 - The error number 0.
              const char* Errno1 - The error number 1.
Output      : None.
Return      : std::string - The string extracted.
******************************************************************************/
static std::string XML_Get_String(xml_node_t* Root, const char* Name,
                                  const char* Errno0, const char* Errno1)
{
    xml_node_t* Temp;

    if((XML_Child(Root,(xml_s8_t*)Name,&Temp)<0)||(Temp==0))
        Main::Error(std::string(Errno0)+": '"+Name+"' section is missing.");
    if(Temp->XML_Val_Len==0)
        Main::Error(std::string(Errno1)+": '"+Name+"' section is empty.");
    return std::string(Temp->XML_Val,(int)Temp->XML_Val_Len);
}
/* End Function:Main::XML_Get_String ****************************************/

/* Begin Function:Main::Main *************************************************
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
    ptr_t Count;

    try
    {
        if(argc!=7)
        throw std::runtime_error("C0000: Too many or too few input parameters.\n"
                                 "Usage: -i input.xml -o output_path -f format.\n"
                                 "       -i: Project description file.\n"
                                 "       -o: Output folder, must be empty.\n"
                                 "       -f: Output file format.\n"
                                 "           keil: Keil uVision IDE.\n"
                                 "           eclipse: Eclipse IDE.\n"
                                 "           makefile: Makefile project.\n");

        this->Input=nullptr;
        this->Output=nullptr;
        this->Format=nullptr;

        Count=1;
        /* Read the command line one by one */
        while(Count<(ptr_t)argc)
        {
            /* We need to open some input file */
            if(strcmp(argv[Count],"-i")==0)
            {
                if(this->Input!=nullptr)
                    throw std::invalid_argument("C0001: More than one input file.");

                this->Input=std::make_unique<std::string>(argv[Count+1]);
                Count+=2;
            }
            /* We need to check some output path. We no longer care if it is empty */
            else if(strcmp(argv[Count],"-o")==0)
            {
                if(this->Output!=nullptr)
                    throw std::invalid_argument("C0002: More than one output path.");

                this->Output=std::make_unique<std::string>(argv[Count+1]);
                Count+=2;
            }
            /* We need to set the format of the output project */
            else if(strcmp(argv[Count],"-f")==0)
            {
                if(this->Format!=nullptr)
                    throw std::invalid_argument("C0003: More than one output format.");
            
                this->Format=std::make_unique<std::string>(argv[Count+1]);
                Count+=2;
            }
            else
                throw std::invalid_argument("C0004: Unrecognized command line argument.");
        }

        if(this->Input==nullptr)
            throw std::invalid_argument("C0005: No input file specified.");
        if(this->Output==nullptr)
            throw std::invalid_argument("C0006: No output path specified.");
        if(this->Format==nullptr)
            throw std::invalid_argument("C0007: No output project format specified.");
    }
    catch(std::exception& Exc)
    {
        throw std::runtime_error(std::string("Command line parsing:\n")+Exc.what());
    }
}
/* End Function:Main::Main ***************************************************/

/* Begin Function:Main::Parse *************************************************
Description : Parse the files.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
void Main::Parse(void)
{
    std::unique_ptr<std::string> Str;
    xml_node_t* Node;

    try
    {
        this->Dstfs=std::make_unique<class Dstfs>(std::make_unique<std::string>(this->Output->c_str()));

        this->Srcfs=std::make_unique<class Sysfs>(std::make_unique<std::string>("F:/Code_Library/MCU/Mutatus"),
                                                  std::make_unique<std::string>(this->Output->c_str()));
        /* Read project */
        Str=this->Dstfs->Read_Proj(this->Input);
        if(XML_Parse(&Node,(xml_s8_t*)(Str->c_str()))<0)
            throw std::runtime_error("P0000: Project XML parsing failed.");
        this->Proj=std::make_unique<class Proj>(Node);
        if(XML_Del(Node)<0)
            throw std::runtime_error("P0000: Project XML parsing failed.");

        /* Read chip */
        Str=std::make_unique<std::string>("M7M1_MuEukaron/MEukaron/Include/Platform/");
        *Str+=*(this->Proj->Plat_Name)+"/Chips/"+*(this->Proj->Chip_Class)+"/rme_platform_"+*(this->Proj->Chip_Class)+".xml";
        Str=this->Srcfs->Read_Chip(Str);
        if(XML_Parse(&Node,(xml_s8_t*)(Str->c_str()))<0)
            throw std::runtime_error("D0000: Chip XML parsing failed.");
        this->Chip=std::make_unique<class Chip>(Node);
        if(XML_Del(Node)<0)
            throw std::runtime_error("D0000: Chip XML parsing failed.");

        /* Select platform */
        if(*(this->Chip->Plat)!=*(this->Proj->Plat_Name))
            throw std::runtime_error("P0025: Project XML and chip XML platform does not match.");
        if(*(this->Chip->Chip_Class)!=*(this->Proj->Chip_Class))
            throw std::runtime_error("P0026: Project XML and chip XML chip class does not match.");
        if((*(this->Chip->Chip_Compat)).find(*(this->Proj->Chip_Full),0)==std::string::npos)
            throw std::runtime_error("P0027: The specific chip designated in project XML not found in chip XML.");

        if(*(this->Chip->Plat)=="A7M")
        {
            this->Plat=std::make_unique<class A7M>(this->Proj,this->Chip);
            this->RME_Gen=std::make_unique<class A7M_RME_Gen>();
            this->RVM_Gen=std::make_unique<class A7M_RVM_Gen>();
            this->Proc_Gen=std::make_unique<class A7M_Proc_Gen>();
            this->Proj_Gen=std::make_unique<class A7M_Proj_Gen>();
        }
        else
            throw std::runtime_error("P0028: The specific platform is currently not supported.");

        this->Plat->Kmem_Order=this->Proj->RME->Kmem_Order;

        /* Generator objects */
        this->RME_Gen->Main=this;
        this->RVM_Gen->Main=this;
        this->Proc_Gen->Main=this;
        this->Proj_Gen->Main=this;
    }
    catch(std::exception& Exc)
    {
        throw std::runtime_error(std::string("Parse:\n")+Exc.what());
    }
}
/* End Function:Main::Parse **************************************************/

/* Begin Function:Main::Check_Chip ********************************************
Description : Check whether the option for that chip is valid.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
void Main::Check_Chip(void)
{
    std::unique_ptr<std::string>* Value;
    ptr_t Lower;
    ptr_t Upper;
    ptr_t Number;

    /* The number of options in the project description file should be the same
     * as the specific chip description file */
    if(this->Chip->Option.size()!=this->Proj->RME->Chip.size())
        throw std::runtime_error("P0121: Chip option mismatch in project option list.");

    /* Traverse them one by one and see if there are unsupported ones */
    for(std::unique_ptr<class Option>& Option:this->Chip->Option)
    {
        try
        {
            Value=Raw::Match(this->Proj->RME->Chip, Option->Name);
            if(Value==nullptr)
                throw std::runtime_error("P0121: Chip option mismatch in project option list.");

            /* We have found it - what is the type */
            if(Option->Type==OPTION_RANGE)
            {
                /* See if the value is a number and is within range */
                try
                {
                    Number=std::stoull(**Value,0,0);
                    Lower=std::stoull(*(Option->Range[0]),0,0);
                    Upper=std::stoull(*(Option->Range[1]),0,0);
                }
                catch(std::exception& Exc)
                {
                    throw std::runtime_error(std::string("P0122: Value conversion failure.\n")+Exc.what());
                }

                if(Number>Upper)
                    throw std::runtime_error("P0123: Value overflow.");

                if(Number<Lower)
                    throw std::runtime_error("P0124: Value underflow.");
            }
            else
            {
                /* See if we can find the value in the selection */
                Number=0;
                for(std::unique_ptr<std::string>& Opt:Option->Range)
                {
                    if(*Opt==**Value)
                    {
                        Number=1;
                        break;
                    }
                }
                if(Number==0)
                    throw std::runtime_error("P0125: Value not found.");
            }
        }
        catch(std::exception& Exc)
        {
            throw std::runtime_error(std::string("Option ")+*(Option->Name)+":\n"+Exc.what());
        }
    }
}
/* End Function:Main::Check_Chip *********************************************/

/* Begin Function:Main::Add_Extmem ********************************************
Description : Add external memory sections into the system memory map so that 
              allocations can be performed on them. Also checks whether there
              are overlapping memory segments.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
void Main::Add_Extmem(void)
{
    class Mem* Prev;
    class Mem* Next;
    std::vector<class Mem*> Allmem;
    std::vector<class Mem*>::iterator Iter;

    /* Replicate all extra memories into the chip's memory map */
    for(std::unique_ptr<class Mem>& Mem:this->Proj->Extmem_Code)
        this->Chip->Code.push_back(std::make_unique<class Mem>(Mem.get()));
    for(std::unique_ptr<class Mem>& Mem:this->Proj->Extmem_Data)
        this->Chip->Data.push_back(std::make_unique<class Mem>(Mem.get()));
    for(std::unique_ptr<class Mem>& Mem:this->Proj->Extmem_Device)
        this->Chip->Device.push_back(std::make_unique<class Mem>(Mem.get()));

    /* Place all memory map together to see if two segments ever overlap. If yes, we reject this memory map */
    for(std::unique_ptr<class Mem>& Mem:this->Chip->Code)
        Allmem.push_back(Mem.get());
    for(std::unique_ptr<class Mem>& Mem:this->Chip->Data)
        Allmem.push_back(Mem.get());
    for(std::unique_ptr<class Mem>& Mem:this->Chip->Device)
        Allmem.push_back(Mem.get());

    std::sort(Allmem.begin(),Allmem.end(),
    [](class Mem* Left, class Mem* Right)
    {
            return Left->Base<Right->Base;
    });

    /* Check if adjacent memories will overlap */
    for(Iter=Allmem.begin();Iter!=Allmem.end();Iter++)
    {
        /* If we still have something after us, check for overlap */
        Prev=*Iter;
        if(std::next(Iter)!=Allmem.end())
        {
            Next=*std::next(Iter);
            if((Prev->Base+Prev->Size)>Next->Base)
                throw std::runtime_error("M0000: Chip internal and/or extra memory section overlapped.");
        }
    }
}
/* End Function:Main::Add_Extmem *********************************************/

/* Begin Function:Main::Alloc_Code ********************************************
Description : Allocate the code section of all processes.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
void Main::Alloc_Code(void)
{
    std::vector<std::unique_ptr<class Memmap>> Map;
    std::vector<class Mem*> Auto;
    ptr_t Attr;

    for(std::unique_ptr<class Mem>& Mem:this->Chip->Code)
        Map.push_back(std::make_unique<class Memmap>(Mem));

    std::sort(Map.begin(),Map.end(),
    [](std::unique_ptr<class Memmap> const& Left, std::unique_ptr<class Memmap> const& Right)
    {
            return Left->Mem->Base<Right->Mem->Base;
    });

    /* Now populate the RME & RVM sections - must be continuous */
    Attr=MEM_READ|MEM_EXECUTE|MEM_STATIC;
    if(Memmap::Fit_Static(Map,this->Proj->RME->Code_Base,this->Proj->RME->Code_Size,Attr)!=0)
        throw std::runtime_error("M0001: RME code section is invalid, either wrong range or wrong attribute.");
    if(Memmap::Fit_Static(Map,this->Proj->RME->Code_Base+this->Proj->RME->Code_Size,this->Proj->RVM->Code_Size,Attr)!=0)
        throw std::runtime_error("M0002: RVM code section is invalid, either wrong range or wrong attribute.");

    /* Insert AUTO process memories into that list */
    for(std::unique_ptr<class Proc>& Proc:this->Proj->Proc)
    {
        try
        {
            for(std::unique_ptr<class Mem>& Mem:Proc->Code)
            {
                /* If this memory is not auto memory, we wait to deal with it */
                if(Mem->Base!=MEM_AUTO)
                {
                    if(Memmap::Fit_Static(Map, Mem->Base, Mem->Size, Mem->Attr)!=0)
                        throw std::runtime_error("M0003: Code section is invalid, either wrong range or wrong attribute.");
                }
                else
                    Auto.push_back(Mem.get());
            }
        }
        catch(std::exception& Exc)
        {
            throw std::runtime_error(std::string("Process: ")+*(Proc->Name)+"\n"+Exc.what());
        }
    }

    /* Insert AUTO shared memories into that list */
    try
    {
        for(std::unique_ptr<class Mem>& Mem:this->Proj->Shmem_Code)
        {
            /* If this memory is not auto memory, we wait to deal with it */
            if(Mem->Base!=MEM_AUTO)
            {
                if(Memmap::Fit_Static(Map, Mem->Base, Mem->Size, Mem->Attr)!=0)
                    throw std::runtime_error("M0003: Code section is invalid, either wrong range or wrong attribute.");
            }
            else
                Auto.push_back(Mem.get());
        }
    }
    catch(std::exception& Exc)
    {
        throw std::runtime_error(std::string("Shared memory: ")+Exc.what());
    }
    
    /* Sort all memories then populate from large to small */
    std::sort(Auto.begin(),Auto.end(),
    [](class Mem* const& Left, class Mem* const& Right)
    {
            return Left->Size<Right->Size;
    });

    /* Fit whatever that does not have a fixed address */
    for(class Mem*& Mem:Auto)
    {
        if(Memmap::Fit_Auto(Map, &(Mem->Base), Mem->Size, Mem->Align, Mem->Attr)!=0)
            throw std::runtime_error("M0004: Code memory fitter failed.");
    }
}
/* End Function:Main::Alloc_Code *********************************************/

/* Begin Function:Main::Alloc_Data ********************************************
Description : Allocate the data section of all processes and shared memories.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
void Main::Alloc_Data(void)
{
    std::vector<std::unique_ptr<class Memmap>> Map;
    std::vector<class Mem*> Auto;
    ptr_t Attr;

    for(std::unique_ptr<class Mem>& Mem:this->Chip->Data)
        Map.push_back(std::make_unique<class Memmap>(Mem));

    std::sort(Map.begin(),Map.end(),
    [](std::unique_ptr<class Memmap> const& Left, std::unique_ptr<class Memmap> const& Right)
    {
            return Left->Mem->Base<Right->Mem->Base;
    });

    /* Now populate the RME & RVM sections - must be continuous */
    Attr=MEM_READ|MEM_WRITE|MEM_STATIC;
    if(Memmap::Fit_Static(Map,this->Proj->RME->Data_Base,this->Proj->RME->Data_Size,Attr)!=0)
        throw std::runtime_error("M0005: RME data section is invalid, either wrong range or wrong attribute.");
    if(Memmap::Fit_Static(Map,this->Proj->RME->Data_Base+this->Proj->RME->Data_Size,this->Proj->RVM->Data_Size,Attr)!=0)
        throw std::runtime_error("M0006: RVM data section is invalid, either wrong range or wrong attribute.");
    
    /* Insert AUTO process memories into that list */
    for(std::unique_ptr<class Proc>& Proc:Proj->Proc)
    {
        try
        {
            for(std::unique_ptr<class Mem>& Mem:Proc->Data)
            {
                /* If this memory is not auto memory, we wait to deal with it */
                if(Mem->Base!=MEM_AUTO)
                {
                    if(Memmap::Fit_Static(Map, Mem->Base, Mem->Size, Mem->Attr)!=0)
                        throw std::runtime_error("M0007: Data section is invalid, either wrong range or wrong attribute.");
                }
                else
                    Auto.push_back(Mem.get());
            }
        }
        catch(std::exception& Exc)
        {
            throw std::runtime_error(std::string("Process: ")+*(Proc->Name)+"\n"+Exc.what());
        }
    }
    
    /* Insert AUTO shared memories into that list */
    try
    {
        for(std::unique_ptr<class Mem>& Mem:this->Proj->Shmem_Data)
        {
            /* If this memory is not auto memory, we wait to deal with it */
            if(Mem->Base!=MEM_AUTO)
            {
                if(Memmap::Fit_Static(Map, Mem->Base, Mem->Size, Mem->Attr)!=0)
                    throw std::runtime_error("M0007: Data section is invalid, either wrong range or wrong attribute.");
            }
            else
                Auto.push_back(Mem.get());
        }
    }
    catch(std::exception& Exc)
    {
        throw std::runtime_error(std::string("Shared memory: ")+Exc.what());
    }
    
    /* Sort all memories then populate from large to small */
    std::sort(Auto.begin(),Auto.end(),
    [](const class Mem* const& Left, const class Mem* const& Right)
    {
        return Left->Size<Right->Size;
    });

    /* Fit whatever that does not have a fixed address */
    for(class Mem*& Mem:Auto)
    {
        if(Memmap::Fit_Auto(Map, &(Mem->Base), Mem->Size, Mem->Align, Mem->Attr)!=0)
            throw std::runtime_error("M0008: Data memory fitter failed.");
    }
}
/* End Function:Main::Alloc_Data *********************************************/

/* Begin Function:Main::Add_Shmem *********************************************
Description : Add shared memories into their respective processes. We also check
              if the attributes override the attributes in the shared memory.
              In the future, consider giving an warning that some shared memory
              declared is not used.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
void Main::Add_Shmem(void)
{
    class Mem* Found;
    std::string* Errmsg;

    /* Check shared memory name validity */
    Errmsg=Kobj::Check_Kobj<class Mem>(this->Proj->Shmem_Code);
    if(Errmsg!=0)
        throw std::invalid_argument(std::string("Global shared code memory: ")+*Errmsg+"\nM0009: Name is duplicate or invalid.");
    Errmsg=Kobj::Check_Kobj<class Mem>(this->Proj->Shmem_Data);
    if(Errmsg!=0)
        throw std::invalid_argument(std::string("Global shared data memory: ")+*Errmsg+"\nM0009: Name is duplicate or invalid.");
    Errmsg=Kobj::Check_Kobj<class Mem>(this->Proj->Shmem_Device);
    if(Errmsg!=0)
        throw std::invalid_argument(std::string("lobal shared device memory: ")+*Errmsg+"\nM0009: Name is duplicate or invalid.");

    /* Link the shared memory blocks with references in processes */
    for(std::unique_ptr<class Proc>& Proc:this->Proj->Proc)
    {
        try
        {
            Errmsg=Kobj::Check_Kobj<class Shmem>(Proc->Shmem_Code);
            if(Errmsg!=0)
                throw std::invalid_argument(std::string("Shared code memory: ")+*Errmsg+"\nM0009: Name is duplicate or invalid.");
            Errmsg=Kobj::Check_Kobj<class Shmem>(Proc->Shmem_Data);
            if(Errmsg!=0)
                throw std::invalid_argument(std::string("Shared data memory: ")+*Errmsg+"\nM0009: Name is duplicate or invalid.");
            Errmsg=Kobj::Check_Kobj<class Shmem>(Proc->Shmem_Device);
            if(Errmsg!=0)
                throw std::invalid_argument(std::string("Shared device memory: ")+*Errmsg+"\nM0009: Name is duplicate or invalid.");

            /* Populate all shared code memory */
            for(std::unique_ptr<class Shmem>& Shmem:Proc->Shmem_Code)
            {
                Found=nullptr;
                /* Find the shared memory block declared in the project */
                for(std::unique_ptr<class Mem>& Mem:this->Proj->Shmem_Code)
                {
                    if(*(Mem->Name)==*(Shmem->Name))
                    {
                        Found=Mem.get();
                        break;
                    }
                }
                if(Found==nullptr)
                    throw std::invalid_argument(std::string("Shared code memory: ")+*(Shmem->Name)+"\nM0010: Entry not found in global database.");

                /* Check attributes then populate */
                if((Found->Attr&Shmem->Attr)!=Shmem->Attr)
                    throw std::invalid_argument(std::string("Shared code memory: ")+*(Shmem->Name)+"\nM0011: Attributes not satisfied by global entry.");
                Proc->Code.push_back(std::make_unique<class Mem>(Found));
            }

            /* Populate all shared data memory */
            for(std::unique_ptr<class Shmem>& Shmem:Proc->Shmem_Data)
            {
                Found=nullptr;
                /* Find the shared memory block declared in the project */
                for(std::unique_ptr<class Mem>& Mem:this->Proj->Shmem_Data)
                {
                    if(*(Mem->Name)==*(Shmem->Name))
                    {
                        Found=Mem.get();
                        break;
                    }
                }
                if(Found==nullptr)
                    throw std::invalid_argument(std::string("Shared data memory: ")+*(Shmem->Name)+"\nM0010: Entry not found in global database.");

                /* Check attributes then populate */
                if((Found->Attr&Shmem->Attr)!=Shmem->Attr)
                    throw std::invalid_argument(std::string("Shared data memory: ")+*(Shmem->Name)+"\nM0011: Attributes not satisfied by global entry.");
                Proc->Data.push_back(std::make_unique<class Mem>(Found));
            }

            /* Populate all shared device memory */
            for(std::unique_ptr<class Shmem>& Shmem:Proc->Shmem_Device)
            {
                Found=nullptr;
                /* Find the shared memory block declared in the project */
                for(std::unique_ptr<class Mem>& Mem:this->Proj->Shmem_Device)
                {
                    if(*(Mem->Name)==*(Shmem->Name))
                    {
                        Found=Mem.get();
                        break;
                    }
                }
                if(Found==nullptr)
                    throw std::invalid_argument(std::string("Shared device memory: ")+*(Shmem->Name)+"\nM0010: Entry not found in global database.");

                /* Check attributes then populate */
                if((Found->Attr&Shmem->Attr)!=Shmem->Attr)
                    throw std::invalid_argument(std::string("Shared device memory: ")+*(Shmem->Name)+"\nM0011: Attributes not satisfied by global entry.");
                Proc->Device.push_back(std::make_unique<class Mem>(Found));
            }
        }
        catch(std::exception& Exc)
        {
            throw std::runtime_error(std::string("Process: ")+*(Proc->Name)+"\n"+Exc.what());
        }
    }
}
/* End Function:Main::Add_Shmem **********************************************/

/* Begin Function:Main::Check_Addrspace ***************************************
Description : Check the address space of every process to make sure that in each
               process different memory segments never overlap with each other.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
void Main::Check_Addrspace(void)
{
    class Mem* Prev;
    class Mem* Next;
    std::vector<class Mem*> Allmem;
    std::vector<class Mem*>::iterator Iter;

    for(std::unique_ptr<class Proc>& Proc:this->Proj->Proc)
    {
        try
        {
            Allmem.clear();

            /* List all memories together then sort it out */
            for(std::unique_ptr<class Mem>& Mem:Proc->Code)
                Allmem.push_back(Mem.get());
            for(std::unique_ptr<class Mem>& Mem:Proc->Data)
                Allmem.push_back(Mem.get());
            for(std::unique_ptr<class Mem>& Mem:Proc->Device)
                Allmem.push_back(Mem.get());

            std::sort(Allmem.begin(),Allmem.end(),
            [](class Mem* Left, class Mem* Right)
            {
                    return Left->Base<Right->Base;
            });

            /* Check if adjacent memories will overlap */
            for(Iter=Allmem.begin();Iter!=Allmem.end();Iter++)
            {
                /* If we still have something after us, check for overlap */
                Prev=*Iter;
                if(std::next(Iter)!=Allmem.end())
                {
                    Next=*std::next(Iter);
                    if((Prev->Base+Prev->Size)>Next->Base)
                        throw std::runtime_error("M0012: Private and/or shared memory section overlapped.");
                }
            }
        }
        catch(std::exception& Exc)
        {
            throw std::runtime_error(std::string("Process: ")+*(Proc->Name)+"\n"+Exc.what());
        }
    }
}
/* End Function:Main::Check_Addrspace ****************************************/

/* Begin Function:Main::Check_Code ********************************************
Description : Check if the code layout is valid.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
void Main::Check_Code(void)
{
    std::vector<class Mem*> Prim;
    std::vector<class Mem*>::iterator Iter;
    std::unique_ptr<class Mem> Sys;
    class Mem* Prev;
    class Mem* Next;

    /* Create and insert system memory into that list */
    Sys=std::make_unique<class Mem>(this->Proj->RME->Code_Base,this->Proj->RME->Code_Size+this->Proj->RVM->Code_Size,0,0);
    Prim.push_back(Sys.get());

    /* Insert all primary code sections into that list */
    for(std::unique_ptr<class Proc>& Proc:this->Proj->Proc)
        Prim.push_back(Proc->Code[0].get());

    std::sort(Prim.begin(),Prim.end(),
    [](const class Mem* Left, const class Mem* Right)
    {
            return Left->Base<Right->Base;
    });

    /* Check if adjacent memories will overlap */
    for(Iter=Prim.begin();Iter!=Prim.end();Iter++)
    {
        /* If we still have something after us, check for overlap */
        Prev=*Iter;
        if(std::next(Iter)!=Prim.end())
        {
            Next=*std::next(Iter);
            if((Prev->Base+Prev->Size)>Next->Base)
                throw std::runtime_error("M0013: Process primary code section overlapped.");
        }
    }
}
/* End Function:Main::Check_Code *********************************************/

/* Begin Function:Main::Check_Device ******************************************
Description : Check the device memory to make sure that all of them falls into range.
              There are faster algorithms; but we don't do such optimization now.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
void Main::Check_Device(void)
{
    ptr_t Found;

    /* Is it true that the device memory is in device memory range? */
    for(std::unique_ptr<class Proc>& Proc:this->Proj->Proc)
    {
        for(std::unique_ptr<class Mem>& Proc_Mem:Proc->Device)
        {
            Found=0;

            for(std::unique_ptr<class Mem>& Chip_Mem:this->Chip->Device)
            {
                if(Proc_Mem->Base>=Chip_Mem->Base)
                {
                    if((Proc_Mem->Base+Proc_Mem->Size)<=(Chip_Mem->Base+Chip_Mem->Size))
                    {
                        if((Chip_Mem->Attr&Proc_Mem->Attr)!=Proc_Mem->Attr)
                            throw std::runtime_error(std::string("Process: ")+*(Proc->Name)+"\nM0014: Device memory have wrong attributes.");
                        Found=1;
                        break;
                    }
                }
            }

            if(Found==0)
                throw std::runtime_error(std::string("Process: ")+*(Proc->Name)+"\nM0015: Device memory segment is out of bound.");
        }
    }
}
/* End Function:Main::Check_Device *******************************************/

/* Begin Function:Main::Alloc_Mem *********************************************
Description : Allocate memoro trunks to their respective positions.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
void Main::Alloc_Mem(void)
{
    try
    {
        /* Add all extra memory segments into the system and check overlap */
        Add_Extmem();
        /* Allocate code sections */
        Alloc_Code();
        /* Allocate data sections */
        Alloc_Data();
        /* Add shared memory into processes */
        Add_Shmem();
        /* Check process memory map validity */
        Check_Addrspace();
        /* Check allocation successful */
        Check_Code();
        /* Check if device section is successfully allocated */
        Check_Device();
    }
    catch(std::exception& Exc)
    {
        throw std::runtime_error(std::string("Memory allocator:\n")+Exc.what());
    }
}
/* End Function:Main::Alloc_Mem **********************************************/

/* Begin Function:Main::Link_Cap **********************************************
Description : Back propagate the global ID to all the ports and send endpoints,
              which are derived from kernel objects. Also detects if all the port
              and send endpoint names in the system are valid. If any of them includes
              dangling references to invocations and receive endpoints, abort.
              These comparisons use strcmp because we require that the process name
              cases match.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
void Main::Link_Cap(void)
{
    class Proc* Proc_Dst;
    class Inv* Inv_Dst;
    class Recv* Recv_Dst;
    class Vect* Vect_Dst;

    for(std::unique_ptr<class Proc>& Proc:this->Proj->Proc)
    {
        /* For every port, there must be a invocation somewhere */
        for(std::unique_ptr<class Port>& Port:Proc->Port)
        {
            Proc_Dst=0;
            for(std::unique_ptr<class Proc>& Proc_Temp:this->Proj->Proc)
            {
                if(*(Proc_Temp->Name)==*(Port->Proc_Name))
                {
                    Proc_Dst=Proc_Temp.get();
                    break;
                }
            }
            if(Proc_Dst==0)
                throw std::runtime_error(std::string("Port:")+*(Port->Name)+"\nM1400: Invalid process name.");

            Inv_Dst=0;
            for(std::unique_ptr<class Inv>& Inv:Proc_Dst->Inv)
            {
                if(*(Inv->Name)==*(Port->Name))
                {
                    Inv_Dst=Inv.get();
                    break;
                }
            }
            if(Inv_Dst==0)
                throw std::runtime_error(std::string("Port:")+*(Port->Name)+"\nM1401: Invalid invocation name.");

            Port->RVM_Capid=Inv_Dst->RVM_Capid;
            Port->RVM_Macro=std::make_unique<std::string>(*(Inv_Dst->RVM_Macro));
        }

        /* For every send endpoint, there must be a receive endpoint somewhere */
        for(std::unique_ptr<class Send>& Send:Proc->Send)
        {
            Proc_Dst=0;
            for(std::unique_ptr<class Proc>& Proc_Temp:this->Proj->Proc)
            {
                if(*(Proc_Temp->Name)==*(Send->Proc_Name))
                {
                    Proc_Dst=Proc_Temp.get();
                    break;
                }
            }
            if(Proc_Dst==0)
                throw std::runtime_error(std::string("Send endpoint:")+*(Send->Name)+"\nM1400: Invalid process name.");

            Recv_Dst=0;
            for(std::unique_ptr<class Recv>& Recv:Proc_Dst->Recv)
            {
                if(*(Recv->Name)==*(Send->Name))
                {
                    Recv_Dst=Recv.get();
                    break;
                }
            }
            if(Recv_Dst==0)
                throw std::runtime_error(std::string("Send endpoint:")+*(Send->Name)+"\nM1402: Invalid receive endpoint name.");

            Send->RVM_Capid=Recv_Dst->RVM_Capid;
            Send->RVM_Macro=std::make_unique<std::string>(*(Recv_Dst->RVM_Macro));
        }

        /* For every vector, there must be a corresponding chip interrupt vector somewhere */
        for(std::unique_ptr<class Vect>& Vect:Proc->Vect)
        {
            Vect_Dst=0;
            for(std::unique_ptr<class Vect>& Vect_Chip:Chip->Vect)
            {
                if(*(Vect_Chip->Name)==*(Vect->Name))
                {
                    if(Vect_Chip->Num!=Vect->Num)
                        throw std::runtime_error(std::string("Vector endpoint:")+*(Vect->Name)+"\nM1403: Invalid vector number.");

                    Vect_Dst=Vect_Chip.get();
                    break;
                }
            }
            if(Vect_Dst==0)
                throw std::runtime_error(std::string("Vector endpoint:")+*(Vect->Name)+"\nM1404: Invalid vector name.");

            Vect->Num=Vect_Dst->Num;
        }
    }
}
/* End Function:Main::Link_Cap ***********************************************/

/* Begin Function:Main::Alloc_Cap *********************************************
Description : Allocate the capability ID and macros. Both the local one and
              the global one will be allocated.
              All parameters are either checked at the XML parsing time or checked
              here (for the portion that cannot be locally checked, we check them here).
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
void Main::Alloc_Cap(void)
{    
    try
    {
        std::string* Errmsg;
        class Virt* Virt;

        /* Check if the kernel priorities are a multiple of word size */
        if((this->Proj->RME->Kern_Prios%this->Plat->Word_Bits)!=0)
            throw std::runtime_error("M1000: Total number of kernel priorities must be a multiple of word size.");

        /* Check if the kernel memory granularity order is within range - we allow 2^3=8 bytes to 2^5=32 bytes */
        if(this->Proj->RME->Kmem_Order<3)
            throw std::runtime_error("M1001: Kernel memory allocation granularity order must be no less than 2^3=8 bytes.");
        if(this->Proj->RME->Kmem_Order>5)
            throw std::runtime_error("M1002: Kernel memory allocation granularity order must be no more than 2^5=32 bytes.");

        /* If there are neither processes nor virtual machines, system is not that useful */
        if(this->Proj->Proc.size()==0)
            throw std::runtime_error("M1003: There are neither virtual machines nor processes in the system.");

        /* Set all virtual machine related portion to zero because these options do not make sense anymore */
        if(this->Proj->Virt.size()==0)
        {
            this->Proj->RVM->Virt_Prios=0;
            this->Proj->RVM->Virt_Evts=0;
            this->Proj->RVM->Virt_Maps=0;
        }
        else
        {
            if(this->Proj->RVM->Virt_Prios==0)
                throw std::runtime_error("M1004: Virtual machine exists but the total number of virtual machine priorities is set to 0.");
            if((this->Proj->RVM->Virt_Prios%this->Plat->Word_Bits)!=0)
                throw std::runtime_error("M1005: Total number of virtual machine priorities must be a multiple of word size.");
            if(this->Proj->RVM->Virt_Evts==0)
                throw std::runtime_error("M1006: Virtual machine exists but the total number of virtual event sources is set to 0.");
            if((this->Proj->RVM->Virt_Evts%this->Plat->Word_Bits)!=0)
                throw std::runtime_error("M1007: Total number of virtual event sources must be a multiple of word size.");
            if(this->Proj->RVM->Virt_Evts>1024)
                throw std::runtime_error("M1008: Total number of virtual event sources cannot exceed 1024.");
            if(this->Proj->RVM->Virt_Maps<this->Proj->RVM->Virt_Evts)
                throw std::runtime_error("M1009: Total number of virtual event to interrupt mappings cannot be smaller than the virtual event source number.");
        }

        /* Check all threads' priority in non-virtual machine processes */
        for(std::unique_ptr<class Proc>& Proc:this->Proj->Proc)
        {
            try
            {
                if(Proc->Type==PROC_TYPE_NATIVE)
                {
                    for(std::unique_ptr<class Thd>& Thd:Proc->Thd)
                    {
                        if(Thd->Prio<PROC_THD_PRIO_MIN)
                            throw std::invalid_argument(std::string("Thread: ")+*(Thd->Name.get())+"\nM1010: Priority must be bigger than service daemons' priority (4).");
                        else if(Thd->Prio>(this->Proj->RME->Kern_Prios-2))
                            throw std::invalid_argument(std::string("Thread: ")+*(Thd->Name.get())+"\nM1011: Priority must be smaller than safety daemon's priority (Kern_Prios-1).");
                    }
                }
                else
                {
                    /* Check virtual machine priorities */
                    Virt=static_cast<class Virt*>(Proc.get());
                    if(Virt->Prio>=this->Proj->RVM->Virt_Prios)
                        throw std::invalid_argument(std::string("M1012: Priority must be smaller than total number of virtual machine priorities."));
                }

                /* Check extra capability table sizes */
                if(Proc->Captbl->Extra>this->Plat->Capacity)
                    throw std::invalid_argument(std::string("M1013: Extra capacity cannot be larger than the architectural limit."));

                /* Check kernel function ranges */
                for(std::unique_ptr<class Kern>& Kern:Proc->Kern)
                {
                    if(Kern->End>=POW2(this->Plat->Word_Bits/2))
                        throw std::invalid_argument(std::string("Kernel function: ")+*(Kern->Name.get())+"\nM1014 Kernel function number range exceeded the architectural limit.");
                }
            }
            catch(std::exception& Exc)
            {
                throw std::runtime_error(std::string("Process: ")+*(Proc->Name.get())+"\n"+Exc.what());
            }
        }

        /* Check processes */
        Errmsg=Kobj::Check_Kobj<class Proc>(this->Proj->Proc);
        if(Errmsg!=0)
            throw std::invalid_argument(std::string("Process: ")+*Errmsg+"\nM1015: Name is duplicate or invalid.");

        /* Check other kernel objects */
        for(std::unique_ptr<class Proc>& Proc:this->Proj->Proc)
            Proc->Check_Kobj();
    
        /* Check for duplicate vectors */
        Errmsg=Vect::Check_Vect(this->Proj);
        if(Errmsg!=0)
            throw std::invalid_argument(std::string("Vector endpoint: ")+*Errmsg+"\nM1017: Name/number is duplicate or invalid.");

        for(std::unique_ptr<class Proc>& Proc:this->Proj->Proc)
            Proc->Alloc_Loc(this->Plat->Capacity);

        for(std::unique_ptr<class Proc>& Proc:this->Proj->Proc)
            Proc->Alloc_RVM(this->Proj->RVM);

        for(std::unique_ptr<class Proc>& Proc:this->Proj->Proc)
            Proc->Alloc_Macro();

        Link_Cap();
    }
    catch(std::exception& Exc)
    {
        throw std::runtime_error(std::string("Capability allocator:\n")+Exc.what());
    }
}
/* End Function:Main::Alloc_Cap ********************************************/

/* Begin Function:Main::Alloc_Obj *********************************************
Description : Allocate the kernel objects and memory.
Input       : struct Proj_Info* Proj - The project structure.
Output      : struct Proj_Info* Proj - The updated project structure.
Return      : None.
******************************************************************************/
void Main::Alloc_Obj(void)
{
    try
    {
        /* Compute the kernel memory needed, disregarding the initial
         * capability table size because we don't know its size yet */
        this->Proj->Kobj_Alloc(this->Plat,0);
        /* Now recompute to get the real usage */
        this->Proj->Kobj_Alloc(this->Plat,this->Proj->RVM->Map->Captbl_Size);

        /* Populate RME information */
        this->Proj->RME->Alloc_Kmem(this->Proj->RVM->Map->After_Kmem_Front, this->Plat->Kmem_Order);

        /* Populate RVM information */
        this->Proj->RVM->Alloc_Mem(this->Proj->RME->Code_Base+this->Proj->RME->Code_Size,
                                   this->Proj->RME->Data_Base+this->Proj->RME->Data_Size,
                                   this->Plat->Kmem_Order);
    
        /* Populate Process information */
        for(std::unique_ptr<class Proc>& Proc:this->Proj->Proc)
        {
            try
            {
                Proc->Alloc_Mem(this->Plat->Word_Bits, this->Plat->Kmem_Order);
            }
            catch(std::exception& Exc)
            {
                throw std::runtime_error(std::string("Process: ")+*(Proc->Name)+"\n"+Exc.what());
            }
        }
    }
    catch(std::exception& Exc)
    {
        throw std::runtime_error(std::string("Object allocator:\n")+Exc.what());
    }
}
/* End Function:Main::Alloc_Obj **********************************************/

/* Begin Function:Main::Gen_RME ***********************************************
Description : Generate the RME project.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
void Main::Gen_RME(void)
{
    this->RME_Gen->Folder();
    this->RME_Gen->Conf_Hdr();
    this->RME_Gen->Boot_Hdr();
    this->RME_Gen->Boot_Src();
    this->RME_Gen->User_Src();
    this->RME_Gen->Plat_Gen();
}
/* End Function:Main::Gen_RME ************************************************/

/* Begin Function:Main::Gen_RVM ***********************************************
Description : Generate the RVM project.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
void Main::Gen_RVM(void)
{
    this->RVM_Gen->Folder();
    this->RVM_Gen->Conf_Hdr();
    this->RVM_Gen->Boot_Hdr();
    this->RVM_Gen->Boot_Src();
    this->RVM_Gen->User_Src();
    this->RVM_Gen->Plat_Gen();
}
/* End Function:Main::Gen_RVM ************************************************/

/* Begin Function:Main::Gen_Proc **********************************************
Description : Generate the process projects.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
void Main::Gen_Proc(void)
{
    for(std::unique_ptr<class Proc>& Proc:this->Proj->Proc)
    {
        this->Proc_Gen->Folder(Proc.get());
        this->Proc_Gen->Proc_Hdr(Proc.get());
        this->Proc_Gen->Proc_Src(Proc.get());
        this->Proc_Gen->Plat_Gen(Proc.get());
    }
}
/* End Function:Main::Gen_Proc ***********************************************/

/* Begin Function:Main::Gen_Proj **********************************************
Description : Generate the workspace projects.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
void Main::Gen_Proj(void)
{
    this->Proj_Gen->Folder();
    this->Proj_Gen->Plat_Gen();
}
/* End Function:Main::Gen_Proj ***********************************************/

/* Begin Function:Main::Gen_Report ********************************************
Description : Generate the report.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
void Main::Gen_Report(void)
{
    FILE* File;
    std::unique_ptr<class Doc> Doc;
    std::unique_ptr<class Para> Para;
    std::unique_ptr<class Stats> Stats;
    std::unique_ptr<std::string> Left;
    std::unique_ptr<std::string> Right;
    std::unique_ptr<std::string> Date;
    std::vector<class Mem*> Allmem;

    Stats=std::make_unique<class Stats>();

    /* Generate generate statistics */
    Stats->Kobj_Stats(this);
    Stats->Kmem_Stats(this);
    Stats->User_Stats(this);
    Stats->Plat_Stats(this);

    /* Print general statistics */
    Doc=std::make_unique<class Doc>();
    Date=Doc::Date();
    Para=std::make_unique<class Para>("Doc:Project statistics");
    Para->Add("###############################################################################");
    Left=std::make_unique<std::string>("Project ");
    *Left+=*(this->Proj->Name)+" ["+*(this->Proj->Plat_Name)+", "+*(this->Proj->Chip_Class)+", "+*(this->Proj->Chip_Full)+"]";
    Para->Add(Left->c_str(), Date->c_str());
    Para->Add("-------------------------------------------------------------------------------");
    Left=std::make_unique<std::string>("Main capability table");
    Para->Add(Left,"%lld/%lld/%lld", Stats->Kobj.Main_Capability_Table_Usage, 
                                     Stats->Kobj.Main_Capability_Table_Total,
                                     Stats->Kobj.Main_Capability_Table_Limit);
    Left=std::make_unique<std::string>("Total kernel objects");
    Para->Add(Left,"%lld", Stats->Kobj.Total_Kernel_Objects);
    Left=std::make_unique<std::string>("Auxiliary capability tables");
    Para->Add(Left, "%lld", Stats->Kobj.Auxiliary_Capability_Tables);
    Left=std::make_unique<std::string>("Processes");
    Para->Add(Left, "%lld (%lld Virtual machines)", Stats->Kobj.Processes, Stats->Kobj.Virtual_Machines);
    Left=std::make_unique<std::string>("Capability tables");
    Para->Add(Left, "%lld", Stats->Kobj.Capability_Tables);
    Left=std::make_unique<std::string>("Page tables");
    Para->Add(Left, "%lld", Stats->Kobj.Page_Tables);
    Left=std::make_unique<std::string>("Threads");
    Para->Add(Left, "%lld", Stats->Kobj.Threads);
    Left=std::make_unique<std::string>("Invocations");
    Para->Add(Left, "%lld", Stats->Kobj.Invocations);
    Left=std::make_unique<std::string>("Endpoints");
    Para->Add(Left, "%lld", Stats->Kobj.Endpoints);
    Para->Add("-------------------------------------------------------------------------------");
    Left=std::make_unique<std::string>("RME code memory");
    Para->Add(Left, "0x%llX", Stats->Kmem.RME_Code_Memory);
    Left=std::make_unique<std::string>("RME data memory");
    Para->Add(Left, "0x%llX", Stats->Kmem.RME_Data_Memory);
    Left=std::make_unique<std::string>("RVM code memory");
    Para->Add(Left, "0x%llX", Stats->Kmem.RVM_Code_Memory);
    Left=std::make_unique<std::string>("RVM data memory");
    Para->Add(Left, "0x%llX", Stats->Kmem.RVM_Data_Memory);
    Left=std::make_unique<std::string>("Kernel memory");
    Para->Add(Left, "0x%llX/0x%llX", Stats->Kmem.Kernel_Memory_Usage, Stats->Kmem.Kernel_Memory_Total);
    Left=std::make_unique<std::string>("Processes");
    Para->Add(Left, "0x%llX", Stats->Kmem.Processes);
    Left=std::make_unique<std::string>("Capability tables");
    Para->Add(Left, "0x%llX", Stats->Kmem.Capability_Tables);
    Left=std::make_unique<std::string>("Page tables");
    Para->Add(Left, "0x%llX", Stats->Kmem.Page_Tables);
    Left=std::make_unique<std::string>("Threads");
    Para->Add(Left, "0x%llX", Stats->Kmem.Threads);
    Left=std::make_unique<std::string>("Invocations");
    Para->Add(Left, "0x%llX", Stats->Kmem.Invocations);
    Left=std::make_unique<std::string>("Endpoints");
    Para->Add(Left, "0x%llX", Stats->Kmem.Endpoints);
    Para->Add("-------------------------------------------------------------------------------");
    Left=std::make_unique<std::string>("Private code memory");
    Para->Add(Left, "0x%llX", Stats->User.Private_Code_Memory);
    Left=std::make_unique<std::string>("Private data memory");
    Para->Add(Left, "0x%llX", Stats->User.Private_Data_Memory);
    Left=std::make_unique<std::string>("Private device memory");
    Para->Add(Left, "0x%llX", Stats->User.Private_Device_Memory);
    Left=std::make_unique<std::string>("Shared code memory");
    Para->Add(Left, "0x%llX", Stats->User.Shared_Code_Memory);
    Left=std::make_unique<std::string>("Shared data memory");
    Para->Add(Left, "0x%llX", Stats->User.Shared_Data_Memory);
    Left=std::make_unique<std::string>("Shared device memory");
    Para->Add(Left, "0x%llX", Stats->User.Shared_Device_Memory);
    Para->Add("-------------------------------------------------------------------------------");
    Left=std::make_unique<std::string>("Code memory usage");
    Para->Add(Left, "0x%llX/0x%llX (%05.2f%%)", Stats->Plat.Used_Code_Memory, 
                                                Stats->Plat.Platform_Code_Memory,
                                                (float)(Stats->Plat.Used_Code_Memory)/(float)(Stats->Plat.Platform_Code_Memory)*100.0f);
    Left=std::make_unique<std::string>("Data memory usage");
    Para->Add(Left, "0x%llX/0x%llX (%05.2f%%)", Stats->Plat.Used_Data_Memory, 
                                                Stats->Plat.Platform_Data_Memory,
                                                (float)(Stats->Plat.Used_Data_Memory)/(float)(Stats->Plat.Platform_Data_Memory)*100.0f);
    Left=std::make_unique<std::string>("Device memory usage");
    Para->Add(Left, "0x%llX/0x%llX (------)", Stats->Plat.Used_Device_Memory, Stats->Plat.Platform_Device_Memory);
    Para->Add("###############################################################################");

    /* Generate and print statistics of each process */
    for(std::unique_ptr<class Proc>& Proc:this->Proj->Proc)
    {
        Stats->Proc_Stats(Proc.get());
        /* Print statistics about every single process */
        Left=std::make_unique<std::string>("Process ");
        *Left+=*(Proc->Name)+" ";
        /* Is this a native process or a virtual machine? */
        if(Proc->Type==PROC_TYPE_NATIVE)
            *Left+="[Native]";
        else
            *Left+="[Virtual]";
        Para->Add(Left, "%s", Proc->RVM_Macro->c_str());
        Para->Add("- Stats -----------------------------------------------------------------------");
        Left=std::make_unique<std::string>("Capability table");
        Para->Add(Left, "%lld/%lld/%lld", Stats->Proc.Capability_Table_Usage,
                                          Stats->Proc.Capability_Table_Total,
                                          Stats->Proc.Capability_Table_Limit);
        Left=std::make_unique<std::string>("Code memory");
        Para->Add(Left, "0x%llX", Stats->Proc.Code_Memory);
        Left=std::make_unique<std::string>("Data memory");
        Para->Add(Left, "0x%llX", Stats->Proc.Data_Memory);
        Left=std::make_unique<std::string>("Device memory");
        Para->Add(Left, "0x%llX", Stats->Proc.Device_Memory);
        Para->Add("- Memory Map ------------------------------------------------------------------");
        /* Add all memory into queue */
        Allmem.clear();
        for(std::unique_ptr<class Mem>& Mem:Proc->Code)
            Allmem.push_back(Mem.get());
        for(std::unique_ptr<class Mem>& Mem:Proc->Data)
            Allmem.push_back(Mem.get());
        for(std::unique_ptr<class Mem>& Mem:Proc->Device)
            Allmem.push_back(Mem.get());
        /* Sort them from small end to large end */
        std::sort(Allmem.begin(),Allmem.end(),
        [](class Mem* Left, class Mem* Right)
        {
            return Left->Base<Right->Base;
        });
        /* Print them one by one */
        for(class Mem* Mem:Allmem)
        {
            if(Mem->Name==nullptr)
                Left=std::make_unique<std::string>("(Anonymous)");
            else
                Left=std::make_unique<std::string>(Mem->Name->c_str());

            if(Mem->Is_Shared!=0)
                Right=std::make_unique<std::string>("Shared, ");
            else
                Right=std::make_unique<std::string>("Private, ");

            ((Mem->Attr&MEM_READ)!=0)?*Right+="R":*Right+="-";
            ((Mem->Attr&MEM_WRITE)!=0)?*Right+="W":*Right+="-";
            ((Mem->Attr&MEM_EXECUTE)!=0)?*Right+="X":*Right+="-";
            ((Mem->Attr&MEM_BUFFERABLE)!=0)?*Right+="B":*Right+="-";
            ((Mem->Attr&MEM_CACHEABLE)!=0)?*Right+="C":*Right+="-";
            ((Mem->Attr&MEM_STATIC)!=0)?*Right+="S":*Right+="-";

            Para->Add(Left, "%s, 0x%llX - 0x%llX", Right->c_str(), Mem->Base, Mem->Base+Mem->Size-1);
        }
        if(Proc->Thd.size()!=0)
        {
            Para->Add("- Threads ---------------------------------------------------------------------");
            for(std::unique_ptr<class Thd>& Thd:Proc->Thd)
            {
                Para->Add(Thd->Name, "%s, 0x%llX - 0x%llX", Thd->RVM_Macro->c_str(), 
                                                            Thd->Map->Stack_Base, 
                                                            Thd->Map->Stack_Base+Thd->Map->Stack_Size-1);
            }
        }
        if(Proc->Inv.size()!=0)
        {
            Para->Add("- Invocations -----------------------------------------------------------------");
            for(std::unique_ptr<class Inv>& Inv:Proc->Inv)
            {
                Para->Add(Inv->Name, "%s, 0x%llX - 0x%llX", Inv->RVM_Macro->c_str(), 
                                                            Inv->Map->Stack_Base, 
                                                            Inv->Map->Stack_Base+Inv->Map->Stack_Size-1);
            }
        }
        if(Proc->Recv.size()!=0)
        {
            Para->Add("- Receive endpoints -----------------------------------------------------------");
            for(std::unique_ptr<class Recv>& Recv:Proc->Recv)
                Para->Add(Recv->Name, "%s, %s", Recv->Loc_Macro->c_str(), Recv->RVM_Macro->c_str());
        }
        if(Proc->Send.size()!=0)
        {
            Para->Add("- Send endpoints --------------------------------------------------------------");
            for(std::unique_ptr<class Send>& Send:Proc->Send)
            {
                Left=std::make_unique<std::string>(Send->Name->c_str());
                *Left+=", ";
                *Left+=*(Send->Proc_Name);
                Para->Add(Left, "%s, %s", Send->Loc_Macro->c_str(), Send->RVM_Macro->c_str());
            }
        }
        if(Proc->Vect.size()!=0)
        {
            Para->Add("- Vector endpoints ------------------------------------------------------------");
            for(std::unique_ptr<class Vect>& Vect:Proc->Vect)
            {
                Left=std::make_unique<std::string>(Vect->Name->c_str());
                *Left+=" (";
                *Left+=std::to_string(Vect->Num)+")";
                Para->Add(Left, "%s, %s", Vect->Loc_Macro->c_str(), Vect->RVM_Macro->c_str());
            }
        }
        if(Proc->Kern.size()!=0)
        {
            Para->Add("- Kernel functions ------------------------------------------------------------");
            for(std::unique_ptr<class Kern>& Kern:Proc->Kern)
                Para->Add(Kern->Name, "%s, 0x%llX - 0x%llX", Kern->Loc_Macro->c_str(), 
                                                             Kern->Start, Kern->End);
        }

        Para->Add("###############################################################################");
    }
    Doc->Add(std::move(Para));

    /* Generate rvm_boot.c */
    File=this->Dstfs->Open_File("Workspace/report.txt");
    Doc->Write(File);
    fclose(File);
}
/* End Function:Main::Gen_Report *********************************************/

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
Description : Output information about the compile/assemble/link process in verbose mode.
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

/* Begin Function:Stats::Kobj_Stats *******************************************
Description : Gather statistics about kernel objects.
Input       : class Main* Main - The main application instance.
Output      : None.
Return      : None.
******************************************************************************/
void Stats::Kobj_Stats(class Main* Main)
{
    std::unique_ptr<class RME>& RME=Main->Proj->RME;
    std::unique_ptr<class RVM>& RVM=Main->Proj->RVM;

    this->Kobj.Main_Capability_Table_Usage=RVM->Map->After_Cap_Front;
    this->Kobj.Main_Capability_Table_Total=RVM->Map->Captbl_Size;
    this->Kobj.Main_Capability_Table_Limit=Main->Plat->Capacity;

    this->Kobj.Auxiliary_Capability_Tables=RVM->Map->Before_Cap_Front-RME->Map->Vect_Cap_Front;
    this->Kobj.Auxiliary_Capability_Tables+=RVM->Map->After_Cap_Front-RVM->Map->Virtep_Cap_Front;

    this->Kobj.Processes=Main->Proj->Proc.size();
    this->Kobj.Virtual_Machines=Main->Proj->Virt.size();
    this->Kobj.Capability_Tables=RVM->Captbl.size();
    this->Kobj.Page_Tables=RVM->Pgtbl.size();
    this->Kobj.Threads=RVM->Thd.size();
    this->Kobj.Invocations=RVM->Inv.size();
    this->Kobj.Endpoints=RVM->Recv.size()+RVM->Vect.size();

    this->Kobj.Total_Kernel_Objects=this->Kobj.Processes+
                                    this->Kobj.Virtual_Machines+
                                    this->Kobj.Capability_Tables+
                                    this->Kobj.Page_Tables+
                                    this->Kobj.Threads+
                                    this->Kobj.Invocations+
                                    this->Kobj.Endpoints;
}
/* End Function:Stats::Kobj_Stats ********************************************/

/* Begin Function:Stats::Kmem_Stats *******************************************
Description : Gather statistics about kernel memory.
Input       : class Main* Main - The main application instance.
Output      : None.
Return      : None.
******************************************************************************/
void Stats::Kmem_Stats(class Main* Main)
{
    class Kmem_Stats* Kmem=&(this->Kmem);
    std::unique_ptr<class RME>& RME=Main->Proj->RME;
    std::unique_ptr<class RVM>& RVM=Main->Proj->RVM;

    this->Kmem.RME_Code_Memory=RME->Map->Code_Size;
    this->Kmem.RME_Data_Memory=RME->Map->Data_Size;
    this->Kmem.RVM_Code_Memory=RVM->Map->Code_Size;
    this->Kmem.RVM_Data_Memory=RVM->Map->Data_Size;

    this->Kmem.Kernel_Memory_Usage=RVM->Map->After_Kmem_Front;
    this->Kmem.Kernel_Memory_Total=RME->Map->Kmem_Size;

    this->Kmem.Processes=RVM->Map->Thd_Kmem_Front-RVM->Map->Proc_Kmem_Front;
    this->Kmem.Capability_Tables=RVM->Map->Thd_Kmem_Front-RVM->Map->Proc_Kmem_Front;
    this->Kmem.Page_Tables=RVM->Map->Proc_Kmem_Front-RVM->Map->Pgtbl_Kmem_Front;
    this->Kmem.Threads=RVM->Map->Inv_Kmem_Front-RVM->Map->Thd_Kmem_Front;
    this->Kmem.Invocations=RVM->Map->Recv_Kmem_Front-RVM->Map->Inv_Kmem_Front;
    this->Kmem.Endpoints=RVM->Map->Before_Kmem_Front-RME->Map->Vect_Kmem_Front;
    this->Kmem.Endpoints+=RVM->Map->After_Kmem_Front-RVM->Map->Recv_Kmem_Front;
}
/* End Function:Stats::Kmem_Stats ********************************************/

/* Begin Function:Stats::User_Stats *******************************************
Description : Gather statistics about user memory. 
Input       : class Main* Main - The main application instance.
Output      : None.
Return      : None.
******************************************************************************/
void Stats::User_Stats(class Main* Main)
{
    this->User.Private_Code_Memory=0;
    this->User.Private_Data_Memory=0;
    this->User.Private_Device_Memory=0;
    
    for(std::unique_ptr<class Proc>& Proc:Main->Proj->Proc)
    {
        for(std::unique_ptr<class Mem>& Mem:Proc->Code)
        {
            if(Mem->Is_Shared==0)
                this->User.Private_Code_Memory+=Mem->Size;
        }
        for(std::unique_ptr<class Mem>& Mem:Proc->Data)
        {
            if(Mem->Is_Shared==0)
                this->User.Private_Data_Memory+=Mem->Size;
        }
        for(std::unique_ptr<class Mem>& Mem:Proc->Device)
        {
            if(Mem->Is_Shared==0)
                this->User.Private_Device_Memory+=Mem->Size;
        }
    }

    this->User.Shared_Code_Memory=0;
    this->User.Shared_Data_Memory=0;
    this->User.Shared_Device_Memory=0;

    for(std::unique_ptr<class Mem>& Mem:Main->Proj->Shmem_Code)
        this->User.Shared_Code_Memory+=Mem->Size;
    for(std::unique_ptr<class Mem>& Mem:Main->Proj->Shmem_Data)
        this->User.Shared_Data_Memory+=Mem->Size;
    for(std::unique_ptr<class Mem>& Mem:Main->Proj->Shmem_Device)
        this->User.Shared_Device_Memory+=Mem->Size;
}
/* End Function:Stats::User_Stats ********************************************/

/* Begin Function:Stats::Plat_Stats *******************************************
Description : Gather statistics about the platform. 
Input       : class Main* Main - The main application instance.
Output      : None.
Return      : None.
******************************************************************************/
void Stats::Plat_Stats(class Main* Main)
{
    std::vector<class Mem*> Allmem;
    ptr_t Cur_Start;
    ptr_t Cur_End;
    
    this->Plat.Platform_Code_Memory=0;
    this->Plat.Platform_Data_Memory=0;
    this->Plat.Platform_Device_Memory=0;

    for(std::unique_ptr<class Mem>& Mem:Main->Chip->Code)
        this->Plat.Platform_Code_Memory+=Mem->Size;
    for(std::unique_ptr<class Mem>& Mem:Main->Chip->Data)
        this->Plat.Platform_Data_Memory+=Mem->Size;
    for(std::unique_ptr<class Mem>& Mem:Main->Chip->Device)
        this->Plat.Platform_Device_Memory+=Mem->Size;

    this->Plat.Used_Code_Memory=0;
    this->Plat.Used_Data_Memory=0;
    this->Plat.Used_Device_Memory=0;

    /* Add all memory to queue, sort, then start to see the different factors */
    for(std::unique_ptr<class Proc>& Proc:Main->Proj->Proc)
    {
        for(std::unique_ptr<class Mem>& Mem:Proc->Code)
            Allmem.push_back(Mem.get());
    }
    std::sort(Allmem.begin(),Allmem.end(),
    [](class Mem* Left, class Mem* Right)
    {
        return Left->Base<Right->Base;
    });
    Cur_Start=Main->Proj->RME->Code_Base;
    Cur_End=Cur_Start+Main->Proj->RME->Code_Size+Main->Proj->RVM->Code_Size;
    for(class Mem* Mem:Allmem)
    {
        if(Mem->Base>Cur_End)
        {
            /* Time to accumulate the old segment and start counting for the new one */
            this->Plat.Used_Code_Memory+=Cur_End-Cur_Start;
            Cur_Start=Mem->Base;
            Cur_End=Cur_Start+Mem->Size;
        }
        else
        {
            /* Accumulate this segment to the old one */
            if((Mem->Base+Mem->Size)>Cur_End)
                Cur_End=Mem->Base+Mem->Size;
        }
    }
    /* Accumulate the last segment */
    this->Plat.Used_Code_Memory+=Cur_End-Cur_Start;
    
    /* Add all memory to queue, sort, then start to see the different factors */
    for(std::unique_ptr<class Proc>& Proc:Main->Proj->Proc)
    {
        for(std::unique_ptr<class Mem>& Mem:Proc->Data)
            Allmem.push_back(Mem.get());
    }
    std::sort(Allmem.begin(),Allmem.end(),
    [](class Mem* Left, class Mem* Right)
    {
            return Left->Base<Right->Base;
    });
    Cur_Start=Main->Proj->RME->Data_Base;
    Cur_End=Cur_Start+Main->Proj->RME->Data_Size+Main->Proj->RVM->Data_Size;
    for(class Mem* Mem:Allmem)
    {
        if(Mem->Base>Cur_End)
        {
            /* Time to accumulate the old segment and start counting for the new one */
            this->Plat.Used_Data_Memory+=Cur_End-Cur_Start;
            Cur_Start=Mem->Base;
            Cur_End=Cur_Start+Mem->Size;
        }
        else
        {
            /* Accumulate this segment to the old one */
            if((Mem->Base+Mem->Size)>Cur_End)
                Cur_End=Mem->Base+Mem->Size;
        }
    }
    /* Accumulate the last segment */
    this->Plat.Used_Data_Memory+=Cur_End-Cur_Start;

    /* Add all memory to queue, sort, then start to see the different factors */
    for(std::unique_ptr<class Proc>& Proc:Main->Proj->Proc)
    {
        for(std::unique_ptr<class Mem>& Mem:Proc->Device)
            Allmem.push_back(Mem.get());
    }
    std::sort(Allmem.begin(),Allmem.end(),
    [](class Mem* Left, class Mem* Right)
    {
        return Left->Base<Right->Base;
    });
    if(Allmem.size()==0)
    {
        Cur_Start=0;
        Cur_End=0;
    }
    else
    {
        Cur_Start=Allmem[0]->Base;
        Cur_End=Cur_Start+Allmem[0]->Size;

        for(class Mem* Mem:Allmem)
        {
            if(Mem->Base>Cur_End)
            {
                /* Time to accumulate the old segment and start counting for the new one */
                this->Plat.Used_Device_Memory+=Cur_End-Cur_Start;
                Cur_Start=Mem->Base;
                Cur_End=Cur_Start+Mem->Size;
            }
            else
            {
                /* Accumulate this segment to the old one */
                if((Mem->Base+Mem->Size)>Cur_End)
                    Cur_End=Mem->Base+Mem->Size;
            }
        }
        /* Accumulate the last segment */
        this->Plat.Used_Device_Memory+=Cur_End-Cur_Start;
    }
}
/* End Function:Stats::Plat_Stats ********************************************/

/* Begin Function:Stats::Proc_Stats *******************************************
Description : Gather statistics about the process. 
Input       : class Proc* Proc - The process application instance.
Output      : None.
Return      : None.
******************************************************************************/
void Stats::Proc_Stats(class Proc* Proc)
{
    this->Proc.Capability_Table_Usage=Proc->Captbl->Front;
    this->Proc.Capability_Table_Total=Proc->Captbl->Size;
    this->Proc.Capability_Table_Limit=this->Kobj.Main_Capability_Table_Limit;

    this->Proc.Code_Memory=0;
    this->Proc.Data_Memory=0;
    this->Proc.Device_Memory=0;

    for(std::unique_ptr<class Mem>& Mem:Proc->Code)
        this->Proc.Code_Memory+=Mem->Size;
    for(std::unique_ptr<class Mem>& Mem:Proc->Data)
        this->Proc.Data_Memory+=Mem->Size;
    for(std::unique_ptr<class Mem>& Mem:Proc->Device)
        this->Proc.Device_Memory+=Mem->Size;
}
/* End Function:Stats::Proc_Stats ********************************************/
}
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
        Main->Parse();
        Main->Check_Chip();

/* Phase 2: Allocate page tables *********************************************/
        Main->Plat->Align_Mem(Main->Proj);
        Main->Alloc_Mem();
        Main->Plat->Alloc_Pgtbl(Main->Proj,Main->Chip);

/* Phase 3: Allocate kernel object ID & macros *******************************/
        Main->Alloc_Cap();

/* Phase 4: Allocate object memory *******************************************/
        Main->Alloc_Obj();

/* Phase 5: Produce output ***************************************************/
        Main->Gen_RME();
        Main->Gen_RVM();
        Main->Gen_Proc();
        Main->Gen_Proj();

/* Phase 6: Generate report **************************************************/
        Main->Gen_Report();
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
