/******************************************************************************
Filename    : process.cpp
Author      : pry
Date        : 16/07/2019
Licence     : LGPL v3+; see COPYING for details.
Description : The process class.
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
#include "Mem_Info/mem_info.hpp"
#include "Proj_Info/Process/process.hpp"
#undef __HDR_DEF__

#define __HDR_CLASS__
#include "rvm_gen.hpp"
#include "Proj_Info/proj_info.hpp"
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
#include "Mem_Info/mem_info.hpp"
#include "Vect_Info/vect_info.hpp"
#undef __HDR_CLASS__
/* End Include ***************************************************************/
namespace RVM_GEN
{
/* Function:Process::Process **************************************************
Description : Constructor for process class. This only handles the common portion
              between virtual machines and the native process.
Input       : xml_node_t* Root - The node containing the process information.
              ptr_t Type - The process type.
Output      : None.
Return      : None.
******************************************************************************/
/* void */ Process::Process(xml_node_t* Root, ptr_t Type):
Kobj(this)
{
    try
    {
        /* The type of this process is by default a native process */
        this->Type=Type;
        /* Name */
        this->Name=Main::XML_Get_String(Root, "Name", "DXXXX", "DXXXX");
        Name_Gen(this);
        /* Extra_Captbl */
        this->Extra_Captbl=Main::XML_Get_Number(Root, "Extra_Captbl", "DXXXX", "DXXXX");

        /* Coprocessor */
        Main::XML_Get_CSV(Root,"Coprocessor",this->Coprocessor,"DXXXX","DXXXX");
        None_Filter(this->Coprocessor);
        /* Build */
        this->Buildsystem=Main::XML_Get_String(Root, "Buildsystem", "DXXXX", "DXXXX");
        /* Toolchain */
        this->Toolchain=Main::XML_Get_String(Root, "Toolchain", "DXXXX", "DXXXX");
        /* Optimization */
        this->Optimization=Main::XML_Get_String(Root, "Optimization", "DXXXX", "DXXXX");
        /* Project_Output - relative to workspace */
        this->Project_Output=Main::XML_Get_String(Root, "Project_Output", "DXXXX", "DXXXX");
        this->Project_Output=Main::Path_Absolute(PATH_DIR, Main::Workspace_Output, this->Project_Output);
        this->Project_Overwrite=Main::XML_Get_Yesno(Root, "Project_Overwrite", "DXXXX", "DXXXX");
        /* Linker_Output - relative to project */
        this->Linker_Output=Main::XML_Get_String(Root, "Linker_Output", "DXXXX", "DXXXX");
        this->Linker_Output=Main::Path_Absolute(PATH_DIR, this->Project_Output, this->Linker_Output);
        /* Main_Header_Output - relative to project */
        this->Main_Header_Output=Main::XML_Get_String(Root, "Main_Header_Output", "DXXXX", "DXXXX");
        this->Main_Header_Output=Main::Path_Absolute(PATH_DIR, this->Project_Output, this->Main_Header_Output);
        /* Main_Source_Output - relative to project */
        this->Main_Source_Output=Main::XML_Get_String(Root, "Main_Source_Output", "DXXXX", "DXXXX");
        this->Main_Source_Output=Main::Path_Absolute(PATH_DIR, this->Project_Output, this->Main_Source_Output);

        /* Memory */
        Trunk_Parse_Param<class Mem_Info, class Mem_Info, ptr_t>(Root,"Memory",this->Memory,MEM_PSEG,"DXXXX","DXXXX");
        /* Shmem */
        Trunk_Parse<class Shmem, class Shmem>(Root,"Shmem",this->Shmem,"DXXXX","DXXXX");
        /* Send */
        Trunk_Parse_Param<class Send, class Send>(Root,"Send",this->Send,this,"DXXXX","DXXXX");
        /* Kfunc */
        Trunk_Parse_Param<class Kfunc, class Kfunc>(Root,"Kfunc",this->Kfunc,this,"DXXXX","DXXXX");
    }
    catch(std::exception& Exc)
    {
        if(this->Name!="")
            Main::Error(std::string("Process: ")+this->Name+"\n"+Exc.what());
        else
            Main::Error(std::string("Process: ")+"Unknown"+"\n"+Exc.what());
    }
}
/* End Function:Process::Process *********************************************/

/* Function:Process::Check ****************************************************
Description : Check whether the process configuration makes sense.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
void Process::Check(void)
{
    try
    {
        /* Check coprocessor list */
        Duplicate_Check<std::string>(this->Coprocessor, this->Coprocessor_Set, "PXXXX", "name", "Coprocessor");

        /* Check memory layout */
        if(this->Memory.empty())
            Main::Error("XXXXX: The process contains no memory segments.");

        /* Classify memory - regular memory does not have name duplication requirements */
        for(std::unique_ptr<class Mem_Info>& Mem:this->Memory)
        {
            Mem->Check();
            switch(Mem->Type)
            {
                case MEM_CODE:this->Memory_Code.push_back(Mem.get());break;
                case MEM_DATA:this->Memory_Data.push_back(Mem.get());break;
                case MEM_DEVICE:this->Memory_Device.push_back(Mem.get());break;
                default:ASSERT(0);
            }
        }

        /* Every process must have at least one code and data segment, and they must be static.
         * The primary code segment allow RXS, the primary data segment must allow RWS */
        if(this->Memory_Code.empty())
            Main::Error("XXXXX: No primary code section exists.");
        if(this->Memory_Data.empty())
            Main::Error("XXXXX: No primary data section exists.");
        if((this->Memory_Code[0]->Attr&MEM_CODE_PRIME)!=MEM_CODE_PRIME)
            Main::Error("XXXXX: Primary code section does not have RXS attribute.");
        if((this->Memory_Data[0]->Attr&MEM_DATA_PRIME)!=MEM_DATA_PRIME)
            Main::Error("XXXXX: Primary data section does not have RWS attribute.");

        /* Make sure process memory declarations do not overlap */
        Mem_Info::Overlap_Check(this->Memory_Code,this->Memory_Data,this->Memory_Device,"Process memory");

        /* Check shared memory references */
        for(std::unique_ptr<class Shmem>& Shm:this->Shmem)
            Shm->Check();
        Duplicate_Check<class Shmem,std::string>(this->Shmem,this->Shmem_Map,
                                                 [](std::unique_ptr<class Shmem>& Shm)->std::string{return Shm->Name;},
                                                 "XXXXX","name","Shmem");

        /* All native processes shall have at least one thread */
        if((this->Type==PROCESS_NATIVE)&&(this->Thread.empty()))
            Main::Error("XXXXX: No thread exists in native process.");

        /* Check threads */
        Duplicate_Check<class Thread,std::string>(this->Thread,this->Thread_Map,
                                                  [](std::unique_ptr<class Thread>& Thd)->std::string{return Thd->Name;},
                                                  "XXXXX","name","Thread");

        /* Check invocations */
        Duplicate_Check<class Invocation,std::string>(this->Invocation,this->Invocation_Map,
                                                      [](std::unique_ptr<class Invocation>& Inv)->std::string{return Inv->Name;},
                                                      "XXXXX","name","Invocation");

        /* Check ports - both process and name must be the same for duplication */
        Duplicate_Check<class Port,std::string>(this->Port,this->Port_Map,
                                                [](std::unique_ptr<class Port>& Port)->std::string{return Port->Process+"_"+Port->Name;},
                                                "XXXXX","process/name pair","Port");

        /* Check receive endpoints */
        Duplicate_Check<class Receive,std::string>(this->Receive,this->Receive_Map,
                                                   [](std::unique_ptr<class Receive>& Recv)->std::string{return Recv->Name;},
                                                   "XXXXX","name","Receive");

        /* Check send endpoints - both process and name must be the same for duplication */
        Duplicate_Check<class Send,std::string>(this->Send,this->Send_Map,
                                                [](std::unique_ptr<class Send>& Send)->std::string{return Send->Process+"_"+Send->Name;},
                                                "XXXXX","process/name pair","Send");

        /* Check vectors - neither the name nor the number can be the same */
        Duplicate_Check<class Vect_Info,std::string>(this->Vector,this->Vector_Map,
                                                     [](std::unique_ptr<class Vect_Info>& Vect)->std::string{return Vect->Name;},
                                                     "XXXXX","name","Vector");
        Duplicate_Check<class Vect_Info,ptr_t>(this->Vector,this->Vector_Number_Map,
                                               [](std::unique_ptr<class Vect_Info>& Vect)->ptr_t{return Vect->Number;},
                                               "XXXXX","number","Vector");
    }
    catch(std::exception& Exc)
    {
        if(this->Name!="")
            Main::Error(std::string("Process: ")+this->Name+"\n"+Exc.what());
        else
            Main::Error(std::string("Process: ")+"Unknown"+"\n"+Exc.what());
    }
}
/* End Function:Process::Check ***********************************************/

/* Function:Process::Local_Alloc **********************************************
Description : Allocate local capability table. 
              If this is a native process, the allocation starts with slot 1
              because the first slot contains the kernel function capability
              for event sending; if this is a virtual machine, the allocation
              starts with slot 2 because the first slot contains hypercall
              send endpoint capability and the second slot contains software
              interrupt send/receive endpoint capability.
              The local and global macros are:
-------------------------------------------------------------------------------
Type            Local                           Global
-------------------------------------------------------------------------------
Process         -                               RVM_PRC_<PRC>
-------------------------------------------------------------------------------
Pgt             -                               RVM_PGT_<PRC>_#
-------------------------------------------------------------------------------
Cpt             -                               RVM_CPT_<PRC>
-------------------------------------------------------------------------------
Thread          -                               RVM_THD_<THD>_PRC_<PRC>
-------------------------------------------------------------------------------
Invocation      -                               RVM_INV_<INV>_PRC_<PRC>
-------------------------------------------------------------------------------
Port            PRT_<PRT>_PRC_<PRC>           	(Inherit invocation name)
-------------------------------------------------------------------------------
Receive         RCV_<RCV>                       RVM_RCV_<RCV>_PRC_<PRC>
-------------------------------------------------------------------------------
Send            SND_<SND>_PRC_<PRC>          	(Inherit receive endpoint name)
-------------------------------------------------------------------------------
Vector          VCT_<VCT>                       RVM_VCT_<VCT> (RVM)
                                                RME_VCT_<VCT> (RME)
-------------------------------------------------------------------------------
Kfunc           KFN_<KFN>                   -
-------------------------------------------------------------------------------
Input       : ptr_t Max - The maximum capacity of the capability table.
Output      : None.
Return      : None.
******************************************************************************/
void Process::Local_Alloc(ptr_t Max)
{
    ptr_t Cid;

    try
    {
        if(this->Type==PROCESS_NATIVE)
            Cid=NATIVE_CPT_BASE;
        else
            Cid=VIRTUAL_CPT_BASE;

        for(std::unique_ptr<class Port>& Prt:this->Port)
        {
            Prt->Cid_Local=Cid++;
            Prt->Macro_Local=std::string("PRT_")+Prt->Name+"_PRC_"+Prt->Process;
            Main::Upper(Prt->Macro_Local);
            Main::Info("> Port %s allocated local capid %lld.", Prt->Macro_Local.c_str(), Prt->Cid_Local);
        }

        for(std::unique_ptr<class Receive>& Rcv:this->Receive)
        {
            Rcv->Cid_Local=Cid++;
            Rcv->Macro_Local=std::string("RCV_")+Rcv->Name;
            Main::Upper(Rcv->Macro_Local);
            Main::Info("> Receive endpoint %s allocated local capid %lld.", Rcv->Macro_Local.c_str(), Rcv->Cid_Local);
        }

        for(std::unique_ptr<class Send>& Snd:this->Send)
        {
            Snd->Cid_Local=Cid++;
            Snd->Macro_Local=std::string("SND_")+Snd->Name+"_PRC_"+Snd->Process;
            Main::Upper(Snd->Macro_Local);
            Main::Info("> Send endpoint %s allocated local capid %lld.", Snd->Macro_Local.c_str(), Snd->Cid_Local);
        }

        for(std::unique_ptr<class Vect_Info>& Vct:this->Vector)
        {
            Vct->Cid_Local=Cid++;
            Vct->Macro_Local=std::string("VCT_")+Vct->Name;
            Main::Upper(Vct->Macro_Local);
            Main::Info("> Vector endpoint %s allocated local capid %lld.", Vct->Macro_Local.c_str(), Vct->Cid_Local);
        }

        for(std::unique_ptr<class Kfunc>& Kfn:this->Kfunc)
        {
        	Kfn->Cid_Local=Cid++;
        	Kfn->Macro_Local=std::string("KFN_")+Kfn->Name;
            Main::Upper(Kfn->Macro_Local);
            Main::Info("> Kernel function %s allocated local capid %lld.", Kfn->Macro_Local.c_str(), Kfn->Cid_Local);
        }

        /* Check extra capability table sizes */
        this->Captbl=std::make_unique<class Captbl>(Cid, Cid+this->Extra_Captbl,this);
        if(this->Captbl->Size>Max)
            Main::Error("XXXXX: Total captbl capacity %lld cannot be larger than the platform limit %lld.",this->Captbl->Size,Max);
    }
    catch(std::exception& Exc)
    {
        Main::Error(std::string("Process: ")+this->Name+"\n"+Exc.what());
    }
}
/* End Function:Process::Local_Alloc *****************************************/

/* Function:Process::Global_Alloc_Captbl **************************************
Description : Allocate global capid for capability table.
Input       : std::vector<class Captbl*>& Global - The global array.
Output      : std::vector<class Captbl*>& Global - The updated global array.
Return      : None.
******************************************************************************/
void Process::Global_Alloc_Captbl(std::vector<class Captbl*>& Global)
{
    this->Captbl->Cid_Global=Global.size();
    this->Captbl->Macro_Global=std::string("RVM_CPT_")+this->Name;
    Main::Upper(this->Captbl->Macro_Global);
    Global.push_back(this->Captbl.get());
    Main::Info("> Captbl %s allocated global capid %lld.",
               this->Captbl->Macro_Global.c_str(), this->Captbl->Cid_Global);
}
/* End Function:Process::Global_Alloc_Captbl *********************************/

/* Function:Process::Global_Alloc_Pgtbl ***************************************
Description : Allocate global capid for page tables.
Input       : std::vector<class Pgtbl*>& Global - The global array.
              std::unique_ptr<class Pgtbl>& Pgt - The page table to allocate.
Output      : std::vector<class Pgtbl*>& Global - The updated global array.
Return      : None.
******************************************************************************/
void Process::Global_Alloc_Pgtbl(std::vector<class Pgtbl*>& Global,
                                 std::unique_ptr<class Pgtbl>& Pgt)
{
    static ptr_t Serial=0;
    ptr_t Count;

    /* For top-level page table, we reset serial to 0 */
    Serial++;
    if(Pgt->Is_Top!=0)
        Serial=0;

    Pgt->Cid_Global=Global.size();
    Pgt->Macro_Global=std::string("RVM_PGT_")+this->Name+"_"+std::to_string(Serial);
    Main::Upper(Pgt->Macro_Global);
    Global.push_back(Pgt.get());
    Main::Info("> Pgt %s allocated global capid %lld.",
               Pgt->Macro_Global.c_str(), Pgt->Cid_Global);

    /* Recursively do allocation */
    for(Count=0;Count<Pgt->Pgdir.size();Count++)
    {
        if(Pgt->Pgdir[Count]!=nullptr)
            this->Global_Alloc_Pgtbl(Global, Pgt->Pgdir[Count]);
    }
}
/* End Function:Process::Global_Alloc_Pgtbl **********************************/

/* Function:Process::Global_Alloc_Process *************************************
Description : Allocate global capid for process.
Input       : std::vector<class Process*>& Global - The global array.
Output      : std::vector<class Process*>& Global - The updated global array.
Return      : None.
******************************************************************************/
void Process::Global_Alloc_Process(std::vector<class Process*>& Global)
{
    this->Cid_Global=Global.size();
    this->Macro_Global=std::string("RVM_PRC_")+this->Name;
    Main::Upper(this->Macro_Global);
    Global.push_back(this);
    Main::Info("> Process %s allocated global capid %lld.",
               this->Macro_Global.c_str(), this->Cid_Global);
}
/* End Function:Process::Global_Alloc_Process ********************************/

/* Function:Process::Global_Alloc_Thread **************************************
Description : Allocate global capid for threads.
Input       : std::vector<class Thread*>& Global - The global array.
Output      : std::vector<class Thread*>& Global - The updated global array.
Return      : None.
******************************************************************************/
void Process::Global_Alloc_Thread(std::vector<class Thread*>& Global)
{
    for(std::unique_ptr<class Thread>& Thd:this->Thread)
    {
        Thd->Cid_Global=Global.size();
        Thd->Macro_Global=std::string("RVM_THD_")+Thd->Name+"_PRC_"+this->Name;
        Main::Upper(Thd->Macro_Global);
        Global.push_back(Thd.get());
        Main::Info("> Thread %s allocated global capid %lld.",
                   Thd->Macro_Global.c_str(), Thd->Cid_Global);
    }
}
/* End Function:Process::Global_Alloc_Thread *********************************/

/* Function:Process::Global_Alloc_Invocation **********************************
Description : Allocate global capid for invocations.
Input       : std::vector<class Invocation*>& Global - The global array.
Output      : std::vector<class Invocation*>& Global - The updated global array.
Return      : None.
******************************************************************************/
void Process::Global_Alloc_Invocation(std::vector<class Invocation*>& Global)
{
    for(std::unique_ptr<class Invocation>& Inv:this->Invocation)
    {
        Inv->Cid_Global=Global.size();
        Inv->Macro_Global=std::string("RVM_INV_")+Inv->Name+"_PRC_"+this->Name;
        Main::Upper(Inv->Macro_Global);
        Global.push_back(Inv.get());
        Main::Info("> Invocation %s allocated global capid %lld.",
                   Inv->Macro_Global.c_str(), Inv->Cid_Global);
    }
}
/* End Function:Process::Global_Alloc_Invocation *****************************/

/* Function:Process::Global_Alloc_Receive *************************************
Description : Allocate global capid for receive endpoints.
Input       : std::vector<class Receive*>& Global - The global array.
Output      : std::vector<class Receive*>& Global - The updated global array.
Return      : None.
******************************************************************************/
void Process::Global_Alloc_Receive(std::vector<class Receive*>& Global)
{
    for(std::unique_ptr<class Receive>& Rcv:this->Receive)
    {
        Rcv->Cid_Global=Global.size();
        Rcv->Macro_Global=std::string("RVM_RCV_")+Rcv->Name+"_PRC_"+this->Name;
        Main::Upper(Rcv->Macro_Global);
        Global.push_back(Rcv.get());
        Main::Info("> Receive endpoint %s allocated global capid %lld.",
                   Rcv->Macro_Global.c_str(), Rcv->Cid_Global);
    }
}
/* End Function:Process::Global_Alloc_Receive ********************************/

/* Function:Process::Global_Alloc_Vector **************************************
Description : Allocate global capid for vector endpoints.
Input       : std::vector<class Vect_Info*>& Global - The global array.
Output      : std::vector<class Vect_Info*>& Global - The updated global array.
Return      : None.
******************************************************************************/
void Process::Global_Alloc_Vector(std::vector<class Vect_Info*>& Global)
{
    for(std::unique_ptr<class Vect_Info>& Vct:this->Vector)
    {
        Vct->Cid_Global=Global.size();
        Vct->Macro_Global=std::string("RVM_VCT_")+Vct->Name;
        Main::Upper(Vct->Macro_Global);
        Vct->Macro_Kernel=std::string("RME_VCT_")+Vct->Name;
        Main::Upper(Vct->Macro_Kernel);
        Global.push_back(Vct.get());
        Main::Info("> Vector endpoint %s (%s) allocated global capid %lld.",
                   Vct->Macro_Global.c_str(), Vct->Macro_Kernel.c_str(),Vct->Cid_Global);
    }
}
/* End Function:Process::Global_Alloc_Vector *********************************/

/* Function:Process::Mem_Alloc ************************************************
Description : Allocate process memory. The header of a process is like
              struct RVM_Header
              {
                  rvm_ptr_t Magic;
                  rvm_ptr_t Number;
                  rvm_ptr_t Entry[1024];
              };
              and the real entry point immediately follows it.
Input       : ptr_t Wordlength - The number of bits in a word.
              ptr_t Hyp_Reg_Size - The register set size for hypervisor-managed thread.
              ptr_t Kom_Order - The kernel memory order.
Output      : None.
Return      : None.
******************************************************************************/
void Process::Mem_Alloc(ptr_t Wordlength, ptr_t Hyp_Reg_Size, ptr_t Kom_Order)
{
    class Virtual* Virt;

    /* Allocate everything to primary code and data sections */
    this->Code_Base=this->Memory_Code[0]->Base;
    this->Code_Size=this->Memory_Code[0]->Size;
    this->Data_Base=this->Memory_Data[0]->Base;
    this->Data_Size=this->Memory_Data[0]->Size;
    this->Desc_Front=0;

    /* Sort the threads according to their priority - The highest priority comes first */
    std::sort(this->Thread.begin(),this->Thread.end(),
    [](const std::unique_ptr<class Thread>& Left, const std::unique_ptr<class Thread>& Right)
    {
            return Left->Priority>Right->Priority;
    });

    /* Threads come first */
    for(const std::unique_ptr<class Thread>& Thd:this->Thread)
    {
        try
        {
            /* Allocate stack from the main data memory */
            Thd->Stack_Size=ROUND_UP_POW2(Thd->Stack_Size,Kom_Order);
            Thd->Stack_Base=this->Data_Base+this->Data_Size-Thd->Stack_Size;
            if(Thd->Stack_Base<=this->Data_Base)
                Main::Error("XXXXX: Data section size is not big enough, unable to allocate stack.");
            this->Data_Size=Thd->Stack_Base-this->Data_Base;
            /* Allocate entry from header */
            Thd->Desc_Slot=this->Desc_Front;
            Main::Info("> Thread '%s' stack base 0x%llX size 0x%llX header slot %lld.",
                       Thd->Name.c_str(), Thd->Stack_Base, Thd->Stack_Size, Thd->Desc_Slot);
            this->Desc_Front++;
        }
        catch(std::exception& Exc)
        {
            Main::Error(std::string("Thread: ")+Thd->Name+"\n"+Exc.what());
        }
    }

    /* Then invocations */
    for(std::unique_ptr<class Invocation>& Inv:this->Invocation)
    {
        try
        {
            /* Allocate stack from the main data memory */
            Inv->Stack_Size=ROUND_UP_POW2(Inv->Stack_Size,Kom_Order);
            Inv->Stack_Base=this->Data_Base+this->Data_Size-Inv->Stack_Size;
            if(Inv->Stack_Base<=this->Data_Base)
                Main::Error("XXXXX: Data section size is not big enough, unable to allocate stack.");
            this->Data_Size=Inv->Stack_Base-this->Data_Base;
            /* Allocate entry from descriptor header */
            Inv->Desc_Slot=this->Desc_Front;
            Main::Info("> Invocation '%s' stack base 0x%llX size 0x%llX header slot %lld.",
                       Inv->Name.c_str(), Inv->Stack_Base, Inv->Stack_Size, Inv->Desc_Slot);
            this->Desc_Front++;
        }
        catch(std::exception& Exc)
        {
            Main::Error(std::string("Invocation: ")+Inv->Name+"\n"+Exc.what());
        }
    }

    /* Compensate for the Magic, Number & optional jump Stub that fills the last slot */
    this->Desc_Front+=3;

    /* The code memory frontier just follows the header but is aligned */
    this->Code_Front=PRC_DESC_ALIGN(this->Code_Base+this->Desc_Front*(Wordlength/8));

    /* See if this is a virtual machine. If yes, we go on to allocate its register set space,
     * parameter space and interrupt space. These are used for communicating through the VM. */
    if(this->Type==PROCESS_VIRTUAL)
    {
        Virt=static_cast<class Virtual*>(this);

        /* Vector flag space */
        Virt->State_Size=Virtual::State_Alloc(Virt->Vector_Num, Wordlength, Kom_Order);
        Virt->State_Base=this->Data_Base+this->Data_Size-Virt->State_Size;
        Main::Info("> State block base 0x%llX size 0x%llX.", Virt->State_Base, Virt->State_Size);
        if(Virt->State_Base<=this->Data_Base)
            Main::Error("XXXXX: Data section size is not big enough, unable to allocate virtual interrupt flags.");
        this->Data_Size=Virt->State_Base-this->Data_Base;

        /* Register space for hypervisor-managed thread */
        Virt->Reg_Size=ROUND_UP_POW2(Hyp_Reg_Size,Kom_Order);
        Virt->Reg_Base=this->Data_Base+this->Data_Size-Virt->Reg_Size;
        Main::Info("> Register base 0x%llX size 0x%llX.", Virt->Reg_Base, Virt->Reg_Size);
        if(Virt->Reg_Base<=this->Data_Base)
            Main::Error("XXXXX: Data section size is not big enough, unable to allocate virtual registers.");
        this->Data_Size=Virt->Reg_Base-this->Data_Base;
    }

    /* Data section - whatever is left */
    Main::Info("> Data base 0x%llX size 0x%llX.", this->Data_Base, this->Data_Size);
}
/* End Function:Process::Mem_Alloc *******************************************/

/* Function:Process::Report_Basic *********************************************
Description : Report basics about the process.
Input       : None.
Output      : std::unique_ptr<std::vector<std::string>>& List - The text output.
Return      : None.
******************************************************************************/
void Process::Report_Basic(std::unique_ptr<std::vector<std::string>>& List)
{
    List->push_back("Name                           : "+this->Name+"("+this->Macro_Global+")");
    List->push_back("Buildsystem                    : "+this->Buildsystem);
    List->push_back("Toolchain                      : "+this->Toolchain);
    List->push_back("Enabled coprocessors           : "+Main::Vec2CSV(this->Coprocessor));
    List->push_back("Descriptor table frontier      : "+std::to_string(this->Desc_Front));
    List->push_back("-------------------------------------------------------------------------------");
    List->push_back("Code base                      : 0x"+Main::Hex(this->Code_Base));
    List->push_back("Code size                      : 0x"+Main::Hex(this->Code_Size));
    List->push_back("Data base                      : 0x"+Main::Hex(this->Data_Base));
    List->push_back("Data size                      : 0x"+Main::Hex(this->Data_Size));
    List->push_back("Entry point                    : 0x"+Main::Hex(this->Code_Front));
}
/* End Function:Process::Report_Basic ****************************************/

/* Function:Process::Report_Object ********************************************
Description : Report kernel objects concerning the process.
Input       : None.
Output      : std::unique_ptr<std::vector<std::string>>& List - The text output.
Return      : None.
******************************************************************************/
void Process::Report_Object(std::unique_ptr<std::vector<std::string>>& List)
{
    /* These include shared memory as well */
    for(class Mem_Info* Mem:this->Memory_Code)
        List->push_back("Code "+Mem->Report());
    for(class Mem_Info* Mem:this->Memory_Data)
        List->push_back("Data "+Mem->Report());
    for(class Mem_Info* Mem:this->Memory_Device)
        List->push_back("Device "+Mem->Report());
    List->push_back("-------------------------------------------------------------------------------");
    /* What is actually mapped */
    for(std::unique_ptr<class Mem_Info>& Mem:this->Memory_All)
        List->push_back("Actual "+Mem->Report());
    List->push_back("-------------------------------------------------------------------------------");

    /* Relevant kernel objects */
    List->push_back(this->Captbl->Report());
    if(this->Pgtbl!=nullptr)
        this->Pgtbl->Report_Tree(List,"");
    for(std::unique_ptr<class Kfunc>& Kfn:this->Kfunc)
        List->push_back(Kfn->Report());
    List->push_back("-------------------------------------------------------------------------------");
    for(std::unique_ptr<class Thread>& Thd:this->Thread)
        List->push_back(Thd->Report());
    for(std::unique_ptr<class Invocation>& Inv:this->Invocation)
        List->push_back(Inv->Report());
    for(std::unique_ptr<class Port>& Prt:this->Port)
        List->push_back(Prt->Report());
    List->push_back("-------------------------------------------------------------------------------");
    for(std::unique_ptr<class Receive>& Rcv:this->Receive)
        List->push_back(Rcv->Report());
    for(std::unique_ptr<class Send>& Snd:this->Send)
        List->push_back(Snd->Report());
    for(std::unique_ptr<class Vect_Info>& Vct:this->Vector)
        List->push_back(Vct->Report());
}
/* End Function:Process::Report_Object ***************************************/
}
/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
