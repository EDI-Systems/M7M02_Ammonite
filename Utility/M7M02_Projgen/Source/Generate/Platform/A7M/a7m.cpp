/******************************************************************************
Filename    : rme_a7m.cpp
Author      : pry
Date        : 12/07/2019
Licence     : LGPL v3+; see COPYING for details.
Description : This toolset is for ARMv7-M. Specifically, this suits Cortex-M0+,
              Cortex-M3, Cortex-M4, Cortex-M7. For ARMv8-M, please use A8M port.
******************************************************************************/

/* Includes ******************************************************************/
extern "C"
{
#include "xml.h"
}

#include "string"
#include "memory"
#include "vector"
#include "stdexcept"
#include "algorithm"

#define __HDR_DEFS__
#include "Main/rme_mcu.hpp"
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

#include "A7M/rme_a7m.hpp"
#undef __HDR_DEFS__

#define __HDR_CLASSES__
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

#include "A7M/rme_a7m.hpp"
#undef __HDR_STRUCTS__
/* End Includes **************************************************************/
namespace RVM_GEN
{
/* Begin Function:A7M::A7M ****************************************************
Description : Select the ARMv7-M platform.
Input       : struct Proj_Info* Proj - The project structure.
Output      : struct Proj_Info* Proj - The updated project structure.
Return      : None.
******************************************************************************/
/* void */ A7M::A7M(std::unique_ptr<class Proj>& Proj, std::unique_ptr<class Chip>& Chip):
                Plat(A7M_WORD_BITS, A7M_INIT_NUM_ORD, A7M_RAW_THD_SIZE, A7M_RAW_INV_SIZE)
{
    std::unique_ptr<std::string>* Str;

    try
    {
        /* What is the NVIC grouping that we use? */
        Str=Raw::Match(Proj->RME->Plat, std::make_unique<std::string>("NVIC_Grouping"));
        if(Str==0)
            throw std::invalid_argument("A0000: Missing NVIC grouping settings.");
        if((**Str)=="0-8")
            this->NVIC_Grouping=A7M_NVIC_P0S8;
        else if((**Str)=="1-7")
            this->NVIC_Grouping=A7M_NVIC_P1S7;
        else if((**Str)=="2-6")
            this->NVIC_Grouping=A7M_NVIC_P2S6;
        else if((**Str)=="3-5")
            this->NVIC_Grouping=A7M_NVIC_P3S5;
        else if((**Str)=="4-4")
            this->NVIC_Grouping=A7M_NVIC_P4S4;
        else if((**Str)=="5-3")
            this->NVIC_Grouping=A7M_NVIC_P5S3;
        else if((**Str)=="6-2")
            this->NVIC_Grouping=A7M_NVIC_P6S2;
        else if((**Str)=="7-1")
            this->NVIC_Grouping=A7M_NVIC_P7S1;
        else
            throw std::invalid_argument("A0001: NVIC grouping value is invalid.");

        /* What is the systick value? */
        Str=Raw::Match(Proj->RME->Plat, std::make_unique<std::string>("Systick_Value"));
        if(Str==0)
            throw std::invalid_argument("A0002: Missing systick value settings.");
        this->Systick_Val=strtoull((*Str)->c_str(),0,10);
        if(this->Systick_Val==0)
            throw std::invalid_argument("A0003: Wrong systick value entered.");

        /* What is the CPU type? */
        Str=Raw::Match(Chip->Attr, std::make_unique<std::string>("CPU_Type"));
        if(Str==0)
            throw std::invalid_argument("A0004: Missing CPU type settings.");
        if((**Str)=="Cortex-M0+")
            this->CPU_Type=A7M_CPU_CM0P;
        else if((**Str)=="Cortex-M3")
            this->CPU_Type=A7M_CPU_CM3;
        else if((**Str)=="Cortex-M4")
            this->CPU_Type=A7M_CPU_CM4;
        else if((**Str)=="Cortex-M7")
            this->CPU_Type=A7M_CPU_CM7;
        else
            throw std::invalid_argument("A0005: CPU type value is invalid.");
    
        /* What is the FPU type? */
        Str=Raw::Match(Chip->Attr, std::make_unique<std::string>("FPU_Type"));
        if(Str==0)
            throw std::invalid_argument("A0006: Missing FPU type settings.");
        if((**Str)=="None")
            this->FPU_Type=A7M_FPU_NONE;
        else if((**Str)=="Single")
        {
            if(this->CPU_Type==A7M_CPU_CM4)
                this->FPU_Type=A7M_FPU_FPV4;
            else if(this->CPU_Type==A7M_CPU_CM7)
                this->FPU_Type=A7M_FPU_FPV5_SP;
            else
                throw std::invalid_argument("A0007: FPU type and CPU type mismatch.");
        }
        else if((**Str)=="Double")
        {
            if(this->CPU_Type==A7M_CPU_CM7)
                this->FPU_Type=A7M_FPU_FPV5_DP;
            else
                throw std::invalid_argument("A0007: FPU type and CPU type mismatch.");

        }
        else
            throw std::invalid_argument("A0008: FPU type value is invalid.");

        /* What is the endianness? */
        Str=Raw::Match(Chip->Attr, std::make_unique<std::string>("Endianness"));
        if(Str==0)
            throw std::invalid_argument("A0009: Missing endianness settings.");
        if((**Str)=="Little")
            this->Endianness=A7M_END_LITTLE;
        else if((**Str)=="Big")
            this->Endianness=A7M_END_BIG;
        else
            throw std::invalid_argument("A0010: Endianness value is invalid.");

        /* MPU regions - replicate from Chip */
        this->Regions=Chip->Regions;

        /* Check memory blocks - they all must be at least readable */
        for(std::unique_ptr<class Proc>& Proc:Proj->Proc)
        {
            for(std::unique_ptr<class Mem>& Mem:Proc->Code)
            {
                if((Mem->Attr&MEM_READ)==0)
                    throw std::invalid_argument("A0011: At least one of the code memory blocks is not readable.");
            }
            for(std::unique_ptr<class Mem>& Mem:Proc->Data)
            {
                if((Mem->Attr&MEM_READ)==0)
                    throw std::invalid_argument("A0012: At least one of the data memory blocks is not readable.");
            }
            for(std::unique_ptr<class Mem>& Mem:Proc->Device)
            {
                if((Mem->Attr&MEM_READ)==0)
                    throw std::invalid_argument("A0013: At least one of the device memory blocks is not readable.");
            }
        }
    }
    catch(std::exception& Exc)
    {
        throw std::runtime_error(std::string("ARMv7-M (A7M) parser:\n")+Exc.what());
    }
}
/* End Function:A7M::A7M *****************************************************/

/* Begin Function:A7M::Raw_Pgtbl_Size *****************************************
Description : Compute the raw page table size for ARMv7-M.
Input       : ptr_t Num_Order - The number order.
              ptr_t Is_Top - Whether this is a top-level.
Output      : None.
Return      : None.
******************************************************************************/
ptr_t A7M::Raw_Pgtbl_Size(ptr_t Num_Order, ptr_t Is_Top)
{
    if(Is_Top!=0)
        return A7M_RAW_PGTBL_SIZE_TOP_NOREGIONS(Num_Order)+this->Regions*8;
    else
        return A7M_RAW_PGTBL_SIZE_NOM(Num_Order);
}
/* End Function:A7M::Raw_Pgtbl_Size ******************************************/

/* Begin Function:A7M::Align_Block ********************************************
Description : Align the memory according to Cortex-M platform's requirements.
Input       : std::unique_ptr<class Mem>& Mem - The memory information.
Output      : std::unique_ptr<class Mem>& Mem - The aligned memory information
Return      : None.
******************************************************************************/
void A7M::Align_Block(std::unique_ptr<class Mem>& Mem)
{
    ptr_t Temp;

    if(Mem->Base!=MEM_AUTO)
    {
        /* This memory already have a fixed start address. Can we map it in? */
        if((Mem->Base%A7M_MEM_ALIGN)!=0)
            throw std::invalid_argument("A0108: Static memory base not properly aligned.");
        if((Mem->Size%A7M_MEM_ALIGN)!=0)
            throw std::invalid_argument("A0109: Static memory size not properly aligned.");
        /* This is terrible. Or even horrible, this mapping algorithm is really hard */
    }
    else
    {
        /* This memory's start address is not designated yet. Decide its size after
         * alignment and calculate its start address alignment granularity. 
         * For Cortex-M, the roundup minimum granularity is 1/8 of the nearest power 
         * of 2 for the size. */
        Temp=1;
        while(Temp<Mem->Size)
            Temp<<=1;
        Mem->Align=Temp/8;
        Mem->Size=((Mem->Size-1)/Mem->Align+1)*Mem->Align;
    }
}
/* End Function:A7M::Align_Block *********************************************/

/* Begin Function:A7M::Align_Mem **********************************************
Description : Align the memory according to the A7M platform's alignment functions.
              We will only align the memory of the processes, however the RME and
              RVM memory will also be checked - also stack sizes.
Input       : std::unique_ptr<class Proj>& Proj - The project information.
Output      : std::unique_ptr<class Proj>& Proj - The project information,
                                                  with all memory size aligned.
Return      : None.
******************************************************************************/
void A7M::Align_Mem(std::unique_ptr<class Proj>& Proj)
{
    try
    {
        /* Check all sensitive memory block allocation */
        if((Proj->RME->Code_Base%A7M_MEM_ALIGN)!=0)
            throw std::invalid_argument("A0100: RME code base not properly aligned.");
        if((Proj->RME->Code_Size%A7M_MEM_ALIGN)!=0)
            throw std::invalid_argument("A0101: RME code size not properly aligned.");
        if((Proj->RME->Data_Base%A7M_MEM_ALIGN)!=0)
            throw std::invalid_argument("A0102: RME data base not properly aligned.");
        if((Proj->RME->Data_Size%A7M_MEM_ALIGN)!=0)
            throw std::invalid_argument("A0103: RME data size not properly aligned.");

        if((Proj->RME->Stack_Size%A7M_MEM_ALIGN)!=0)
            throw std::invalid_argument("A0104: RME kernel stack size not properly aligned.");

        if((Proj->RVM->Code_Size%A7M_MEM_ALIGN)!=0)
            throw std::invalid_argument("A0105: RVM code size not properly aligned.");
        if((Proj->RVM->Data_Size%A7M_MEM_ALIGN)!=0)
            throw std::invalid_argument("A0106: RVM data size not properly aligned.");

        if((Proj->RVM->Stack_Size%A7M_MEM_ALIGN)!=0)
            throw std::invalid_argument("A0107: RVM user-level library stack size not properly aligned.");

        /* Align process memory blocks */
        for(std::unique_ptr<class Proc>& Proc:Proj->Proc)
        {
            try
            {
                for(std::unique_ptr<class Mem>& Mem:Proc->Code)
                    Align_Block(Mem);
                for(std::unique_ptr<class Mem>& Mem:Proc->Data)
                    Align_Block(Mem);
                for(std::unique_ptr<class Mem>& Mem:Proc->Device)
                    Align_Block(Mem);
            }
            catch(std::exception& Exc)
            {
                throw std::runtime_error(std::string("Process: ")+*(Proc->Name)+"\n"+Exc.what());
            }
        }

        /* Align shared memory blocks */
        try
        {
            for(std::unique_ptr<class Mem>& Shmem:Proj->Shmem_Code)
                Align_Block(Shmem);
            for(std::unique_ptr<class Mem>& Shmem:Proj->Shmem_Data)
                Align_Block(Shmem);
            for(std::unique_ptr<class Mem>& Shmem:Proj->Shmem_Device)
                Align_Block(Shmem);
        }
        catch(std::exception& Exc)
        {
            throw std::runtime_error(std::string("Shared memory: \n")+Exc.what());
        }
    }
    catch(std::exception& Exc)
    {
        throw std::runtime_error(std::string("ARMv7-M (A7M) memory aligner:\n")+Exc.what());
    }
}
/* End Function:A7M::Align_Mem ***********************************************/

/* Begin Function:A7M::Pgtbl_Tot_Ord ******************************************
Description : Get the total order and the start address of the page table. 
Input       : std::vector<std::unique_ptr<class Mem>>& List - The memory block list.
Output      : ptr_t* Base_Addr - The base address of this page table.
Return      : ptr_t - The total order of the page table.
******************************************************************************/
ptr_t A7M::Pgtbl_Tot_Ord(std::vector<std::unique_ptr<class Mem>>& List, ptr_t* Base_Addr)
{
    /* Start is inclusive, end is exclusive */
    ptr_t Start;
    ptr_t End;
    ptr_t Total_Order;

    /* What ranges does these stuff cover? */
    Start=(ptr_t)(-1);
    End=0;
    for(std::unique_ptr<class Mem>& Mem:List)
    {
        if(Start>Mem->Base)
            Start=Mem->Base;
        if(End<(Mem->Base+Mem->Size))
            End=Mem->Base+Mem->Size;
    }
    
    /* Which power-of-2 box is this in? - do not shift more than 32 or you get undefined behavior */
    Total_Order=0;
    while(1)
    {  
        /* No bigger than 32 is ever possible */
        if(Total_Order>=32)
            break;
        if(End<=(ROUND_DOWN(Start, Total_Order)+POW2(Total_Order)))
            break;
        Total_Order++;
    }

    /* If the total order less than 8, we wish to extend that to 8, because if we are smaller than this it makes no sense */
    if(Total_Order<8)
        Total_Order=8;

    /* Do not shift more than 32 or we get undefined behavior */
    if(Total_Order==32)
        *Base_Addr=0;
    else
        *Base_Addr=ROUND_DOWN(Start, Total_Order);

    return Total_Order;
}
/* End Function:A7M::Pgtbl_Tot_Ord *******************************************/

/* Begin Function:A7M::Pgtbl_Num_Ord ******************************************
Description : Get the number order of the page table. 
Input       : std::vector<std::unique_ptr<class Mem>>& List - The memory block list.
              ptr_t Total_Order - The total order of the page table.
              ptr_t Base_Addr - The start address of the page table.
Output      : None.
Return      : ptr_t - The number order of the page table.
******************************************************************************/
ptr_t A7M::Pgtbl_Num_Ord(std::vector<std::unique_ptr<class Mem>>& List, 
                         ptr_t Total_Order, ptr_t Base_Addr)
{
    ptr_t Num_Order;
    ptr_t Pivot_Cnt;
    ptr_t Pivot_Addr;
    ptr_t Cut_Apart;
    ptr_t Mappable;

    /* Can the memory segments get fully mapped in? If yes, there are two conditions
     * that must be met:
     * 1. There cannot be different access permissions in these memory segments.
     * 2. The memory start address and the size must be fully divisible by POW2(Total_Order-3). */
    Mappable=1;
    for(std::unique_ptr<class Mem>& Mem:List)
    {
        if(Mem->Attr!=List[0]->Attr)
            Mappable=0;
        if((Mem->Base%POW2(Total_Order-3))!=0)
            Mappable=0;
        if((Mem->Size%POW2(Total_Order-3))!=0)
            Mappable=0;
        if(Mappable==0)
            break;
    }

    /* Is this directly mappable? If yes, we always create page tables with 8 pages. */
    if(Mappable!=0)
    {
        /* Yes, it is directly mappable. We choose the smallest number order, in this way
         * we have the largest size order. This will leave us plenty of chances to use huge
         * pages, as this facilitates delegation as well. Number order = 0 is also possible,
         * as this maps in a single huge page. */
        for(Num_Order=0;Num_Order<=3;Num_Order++)
        {
            Mappable=1;

            for(std::unique_ptr<class Mem>& Mem:List)
            {
                if((Mem->Base%POW2(Total_Order-Num_Order))!=0)
                    Mappable=0;
                if((Mem->Size%POW2(Total_Order-Num_Order))!=0)
                    Mappable=0;
                if(Mappable==0)
                    break;
            }

            if(Mappable!=0)
                break;
        }

        if(Num_Order>3)
            throw std::invalid_argument("A0200: Internal number order miscalculation.");
    }
    else
    {
        /* Not directly mappable. What's the maximum number order that do not cut things apart? */
        Cut_Apart=0;
        for(Num_Order=1;Num_Order<=3;Num_Order++)
        {
            for(std::unique_ptr<class Mem>& Mem:List)
            {
                for(Pivot_Cnt=1;Pivot_Cnt<POW2(Num_Order);Pivot_Cnt++)
                {
                    Pivot_Addr=Base_Addr+Pivot_Cnt*POW2(Total_Order-Num_Order);
                    if((Mem->Base<Pivot_Addr)&&((Mem->Base+Mem->Size)>Pivot_Addr))
                    {
                        Cut_Apart=1;
                        break;
                    }
                }
                if(Cut_Apart!=0)
                    break;
            }
            if(Cut_Apart!=0)
                break;
        }

        /* For whatever reason, if it breaks, then the last number order must be good,
         * and the minimum order is 1 because we at least need to split something */
        if(Num_Order>1)
            Num_Order--;
    }

    return Num_Order;
}
/* End Function:A7M::Pgtbl_Num_Ord *******************************************/

/* Begin Function:A7M::Map_Page ***********************************************
Description : Map pages into the page table as we can. 
Input       : std::vector<std::unique_ptr<class Mem>>& List - The memory block list.
              std::unique_ptr<class Pgtbl>& Pgtbl - The current page table.
Output      : std::unique_ptr<class Pgtbl>& Pgtbl - The updated current page table.
Return      : None.
******************************************************************************/
void A7M::Map_Page(std::vector<std::unique_ptr<class Mem>>& List, 
                   std::unique_ptr<class Pgtbl>& Pgtbl)
{
    ptr_t Attr;
    ptr_t Page_Cnt;
    ptr_t Page_Start;
    ptr_t Page_End;
    ptr_t Page_Num;
    ptr_t Page_Num_Max;

    ptr_t Cur_Attr;

    /* Use the attribute that covers most pages - there are O(N) algorithms,
     * but this is easier to guarantee correctness */
    Page_Num_Max=0;
    for(std::unique_ptr<class Mem>& Mem:List)
    {
        Cur_Attr=Mem->Attr;
        Page_Num=0;

        for(std::unique_ptr<class Mem>& Mem:List)
        {
            if(Mem->Attr==Cur_Attr)
            {
                for(Page_Cnt=0;Page_Cnt<POW2(Pgtbl->Num_Order);Page_Cnt++)
                {
                    Page_Start=Pgtbl->Base_Addr+Page_Cnt*POW2(Pgtbl->Size_Order);
                    Page_End=Page_Start+POW2(Pgtbl->Size_Order);

                    if((Mem->Base<=Page_Start)&&((Mem->Base+Mem->Size)>=Page_End))
                        Page_Num++;
                }
            }
        }
        
        if(Page_Num>Page_Num_Max)
        {
            Page_Num_Max=Page_Num;
            Attr=Cur_Attr;
        }
    }

    /* Is there anything that we should map? If no, we return early */
    if(Page_Num_Max==0)
        return;

    /* Map whatever we can map, and postpone whatever we will have to postpone */
    for(Page_Cnt=0;Page_Cnt<POW2(Pgtbl->Num_Order);Page_Cnt++)
    {
        Page_Start=Pgtbl->Base_Addr+Page_Cnt*POW2(Pgtbl->Size_Order);
        Page_End=Page_Start+POW2(Pgtbl->Size_Order);

        /* Can this compartment be mapped? It can if there is one segment covering the range */
        for(std::unique_ptr<class Mem>& Mem:List)
        {
            if((Mem->Base<=Page_Start)&&((Mem->Base+Mem->Size)>=Page_End))
            {
                /* The attribute must be the same as what we map - we ignore the static
                 * attribute when comparing to map as many as possible */
                if((Attr&(~MEM_STATIC))==(Mem->Attr&(~MEM_STATIC)))
                    Pgtbl->Page[(unsigned int)Page_Cnt]=Mem->Attr;
            }
        }
    }
}
/* End Function:A7M::Map_Page ************************************************/

/* Begin Function:A7M_Map_Pgdir ***********************************************
Description : Map page directories into the page table. 
Input       : std::unique_ptr<class Proj>& Proj - The current project.
              std::unique_ptr<class Proc>& Proc - The process we are generating pgtbls for.
              std::vector<std::unique_ptr<class Mem>>& List - The memory block list.
              struct Pgtbl* Pgtbl - The current page table.
Output      : std::unique_ptr<class Pgtbl>& Pgtbl - The updated current page table.
Return      : None.
******************************************************************************/
void A7M::Map_Pgdir(std::unique_ptr<class Proj>& Proj,
                    std::unique_ptr<class Proc>& Proc,
                    std::vector<std::unique_ptr<class Mem>>& List, 
                    std::unique_ptr<class Pgtbl>& Pgtbl)
{
    ptr_t Base;
    ptr_t Size;
    ptr_t Page_Cnt;
    ptr_t Page_Start;
    ptr_t Page_End;
    std::vector<std::unique_ptr<class Mem>> Child_List;

    for(Page_Cnt=0;Page_Cnt<POW2(Pgtbl->Num_Order);Page_Cnt++)
    {
        Page_Start=Pgtbl->Base_Addr+Page_Cnt*POW2(Pgtbl->Size_Order);
        Page_End=Page_Start+POW2(Pgtbl->Size_Order);

        if(Pgtbl->Page[(unsigned int)Page_Cnt]==0)
        {
            /* See if any residue memory list are here */
            for(std::unique_ptr<class Mem>& Mem:List)
            {
                if((Mem->Base>=Page_End)||((Mem->Base+Mem->Size)<=Page_Start))
                    continue;

                /* Round anything inside to this range */
                if(Mem->Base<Page_Start)
                    Base=Page_Start;
                else
                    Base=Mem->Base;

                if((Mem->Base+Mem->Size)>Page_End)
                    Size=Page_End-Mem->Base;
                else
                    Size=Mem->Size;

                Child_List.push_back(std::make_unique<class Mem>(Base, Size, Mem->Attr, Mem->Align));
            }

            /* Map in the child list if there are any at all */
            if(!Child_List.empty())
            {
                Pgtbl->Pgdir[(unsigned int)Page_Cnt]=Gen_Pgtbl(Proj, Proc, Child_List, Pgtbl->Size_Order);

                /* Clean up what we have allocated */
                Child_List.clear();
            }
        }
    }
}
/* End Function:A7M::Map_Pgdir ***********************************************/

/* Begin Function:A7M:Gen_Pgtbl ***********************************************
Description : Recursively construct the page table for the ARMv7-M port.
              This also allocates capid for page tables.
Input       : struct Proc_Info* Proj - The current project.
              struct Proc_Info* Proc - The process we are generating pgtbls for.
              struct List* Mem_List - The list containing memory segments to fit
                                      into this level (and below).
              ptr_t Total_Max - The maximum total order of the page table, cannot
                                be exceeded when deciding the total order of the
                                page table.
Output      : None.
Return      : struct A7M_Pgtbl* - The page table structure returned. This function
                                  will never return NULL; if an error is encountered,
                                  it will directly error out.
******************************************************************************/
std::unique_ptr<class Pgtbl> A7M::Gen_Pgtbl(std::unique_ptr<class Proj>& Proj,
                                            std::unique_ptr<class Proc>& Proc,
                                            std::vector<std::unique_ptr<class Mem>>& List,
                                            ptr_t Total_Max)
{
    ptr_t Start_Addr;
    ptr_t Num_Order;
    ptr_t Size_Order;
    ptr_t Total_Order;
    std::unique_ptr<class Pgtbl> Pgtbl;

    /* Total order and start address of the page table */
    Total_Order=Pgtbl_Tot_Ord(List, &Start_Addr);
    /* See if this will violate the extension limit */
    if(Total_Order>Total_Max)
        throw std::runtime_error("A0201: Memory segment too small, cannot find a reasonable placement.");

    /* Number order */
    Num_Order=Pgtbl_Num_Ord(List, Total_Order, Start_Addr);
    /* Size order */
    Size_Order=Total_Order-Num_Order;
    
    /* Page table attributes are in fact not used in A7M, we always set to full attributes */
    Pgtbl=std::make_unique<class Pgtbl>(Start_Addr, Size_Order, Num_Order,
                                        MEM_READ|MEM_WRITE|MEM_EXECUTE|MEM_BUFFERABLE|MEM_CACHEABLE);
    
    /* Map in all pages */
    Map_Page(List, Pgtbl);
    /* Map in all page directories - recursive */
    Map_Pgdir(Proj, Proc, List, Pgtbl);

    for(ptr_t Page:Pgtbl->Page)
    {
        if((Page&MEM_STATIC)!=0)
        {
            Static_Regions++;
            break;
        }
    }

    return Pgtbl;
}
/* End Function:A7M::Gen_Pgtbl ***********************************************/

/* Begin Function:A7M::Alloc_Pgtbl ********************************************
Description : Allocate page tables for all processes.
Input       : std::unique_ptr<class Proj>& Proj - The project structure.
              std::unique_ptr<class Chip>& Chip - The chip structure.
Output      : None.
Return      : None.
******************************************************************************/
void A7M::Alloc_Pgtbl(std::unique_ptr<class Proj>& Proj, std::unique_ptr<class Chip>& Chip)
{
    std::vector<std::unique_ptr<class Mem>> Raw_List;
    std::vector<std::unique_ptr<class Mem>> List;
    ptr_t Cur_Start;
    ptr_t Cur_End;
    ptr_t Cur_Attr;

    /* Allocate page table global captbl */
    for(std::unique_ptr<class Proc>& Proc:Proj->Proc)
    {
        try
        {
            /* Add everything to list */
            for(std::unique_ptr<class Mem>& Mem:Proc->Code)
                Raw_List.push_back(std::make_unique<class Mem>(Mem->Base,Mem->Size,Mem->Attr,Mem->Align));
            for(std::unique_ptr<class Mem>& Mem:Proc->Data)
                Raw_List.push_back(std::make_unique<class Mem>(Mem->Base,Mem->Size,Mem->Attr,Mem->Align));
            for(std::unique_ptr<class Mem>& Mem:Proc->Device)
                Raw_List.push_back(std::make_unique<class Mem>(Mem->Base,Mem->Size,Mem->Attr,Mem->Align));

            /* Merge whatever this list has to offer - if two adjacent memory blocks have the
             * same attributes, we simply merge them into one block, hopefully reducing MPU
             * region number usage. */
            std::sort(Raw_List.begin(),Raw_List.end(),
            [](std::unique_ptr<class Mem>& Left, std::unique_ptr<class Mem>& Right)
            {
                    return Left->Base<Right->Base;
            });
            Cur_Start=Raw_List[0]->Base;
            Cur_End=Cur_Start+Raw_List[0]->Size;
            Cur_Attr=Raw_List[0]->Attr;
            Raw_List.erase(Raw_List.begin());
            for(std::unique_ptr<class Mem>& Mem:Raw_List)
            {
                if(Mem->Base>Cur_End)
                {
                    /* Time to accumulate the old segment and start counting for the new one */
                    List.push_back(std::make_unique<class Mem>(Cur_Start,Cur_End-Cur_Start,Cur_Attr,0));
                    Cur_Start=Mem->Base;
                    Cur_End=Cur_Start+Mem->Size;
                    Cur_Attr=Mem->Attr;
                }
                else if(Mem->Base==Cur_End)
                {
                    /* Accumulate this segment to the old one if the attributes are the same */
                    if(Mem->Attr==Cur_Attr)
                        Cur_End=Mem->Base+Mem->Size;
                    else
                    {
                        /* If not the same, we start a new segment */
                        List.push_back(std::make_unique<class Mem>(Cur_Start,Cur_End-Cur_Start,Cur_Attr,0));
                        Cur_Start=Mem->Base;
                        Cur_End=Cur_Start+Mem->Size;
                        Cur_Attr=Mem->Attr;
                    }
                }
                else
                {
                    /* This is an overlap and we throw an error. Shouldn't happen at here */
                    throw std::runtime_error("A0202: Internal memory computation indicates overlapping segments.");
                }
            }
            /* Accumulate the last segment */
            List.push_back(std::make_unique<class Mem>(Cur_Start,Cur_End-Cur_Start,Cur_Attr,0));

            this->Static_Regions=0;
            Proc->Pgtbl=Gen_Pgtbl(Proj, Proc, List, 32);
            Proc->Pgtbl->Is_Top=1;

            if(this->Static_Regions>(this->Regions-2))
            {
                throw std::runtime_error(std::string("A0203: Too many static memory segments declared, using ")+
                                         std::to_string(this->Static_Regions)+" regions, exceeding limit "+std::to_string(this->Regions-2)+".");
            }
            
            Raw_List.clear();
            List.clear();
        }
        catch(std::exception& Exc)
        {
            throw std::runtime_error(std::string("ARMv7-M (A7M) page table allocator:\nProcess:\n")+*(Proc->Name)+Exc.what());
        }
    }
}
/* End Function:A7M::Alloc_Pgtbl *********************************************/
}
/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/