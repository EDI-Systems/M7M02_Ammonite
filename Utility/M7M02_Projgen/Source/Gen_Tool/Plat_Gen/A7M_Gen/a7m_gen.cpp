/******************************************************************************
Filename    : a7m_gen.cpp
Author      : pry
Date        : 12/07/2019
Licence     : LGPL v3+; see COPYING for details.
Description : This toolset is for ARMv7-M. Specifically, this suits Cortex-M0+,
              Cortex-M3, Cortex-M4, Cortex-M7. For ARMv8-M, please use A8M port.
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
#include "Plat_Info/plat_info.hpp"
#include "Chip_Info/chip_info.hpp"
#include "Proj_Info/Kobj/kobj.hpp"
#include "Proj_Info/proj_info.hpp"
#include "Proj_Info/Process/process.hpp"
#include "Gen_Tool/Gen_Tool.hpp"
#include "Gen_Tool/Plat_Gen/plat_gen.hpp"
#include "Gen_Tool/Plat_Gen/A7M_Gen/a7m_gen.hpp"
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
#include "Proj_Info/Process/Virtual/virtual.hpp"
#include "Mem_Info/mem_info.hpp"
#include "Vect_Info/vect_info.hpp"
#include "Conf_Info/conf_info.hpp"

#include "Gen_Tool/Gen_Tool.hpp"
#include "Gen_Tool/Plat_Gen/plat_gen.hpp"
#include "Gen_Tool/Plat_Gen/A7M_Gen/a7m_gen.hpp"
#undef __HDR_CLASS__
/* End Include ***************************************************************/
namespace RVM_GEN
{
/* Function:A7M_Gen::A7M_Gen **************************************************
Description : Generator for the ARMv7-M platform.
Input       : class Proj_Info* Proj - The project information.
              class Plat_Info* Plat - The platform information.
              class Chip_Info* Chip - The chip information.
Output      : None.
Return      : None.
******************************************************************************/
/* void */ A7M_Gen::A7M_Gen(class Proj_Info* Proj, class Plat_Info* Plat, class Chip_Info* Chip):
Plat_Gen("A7M", Proj, Plat, Chip)
{
    try
    {

    }
    catch(std::exception& Exc)
    {
        Main::Error(std::string("ARMv7-M (A7M) generator:\n")+Exc.what());
    }
}
/* End Function:A7M_Gen::A7M_Gen *********************************************/

/* Function:A7M_Gen::Mem_Align ************************************************
Description : Memory aligner for the ARMv7-M platform.
Input       : ptr_t Base - The memory base address.
              ptr_t Size - The memory size.
              ptr_t Align_Order - The user suggested align order.
Output      : None.
Return      : ptr_t - The computed alignment.
******************************************************************************/
ptr_t A7M_Gen::Mem_Align(ptr_t Base, ptr_t Size, ptr_t Align_Order)
{
    ptr_t Align;

    try
    {
        /* Check all sensitive memory block allocation */
        if(Base!=MEM_AUTO)
        {
            if((Base%A7M_MEM_ALIGN)!=0)
                Main::Error("XXXXX: Static memory base address not properly aligned.");
        }

        if((Size%A7M_MEM_ALIGN)!=0)
            Main::Error("XXXXX: Memory size not properly aligned.");

        /* This memory's begin address is not designated yet. Decide its size after
         * alignment and calculate its begin address alignment granularity */
        if(Base==MEM_AUTO)
        {
        	/* Compute maximum alignment that make sense for this memory trunk */
            Align=1;
            while(Align<Size)
                Align<<=1;

            /* If the user did not supply an alignment order, produce an appropriate default.
             * For ARMv7-M, the default is 1/8 of the nearest power of 2 for the size */
        	if(Align_Order==MEM_AUTO)
        	{
                Align>>=3;
                if(Align<A7M_MEM_ALIGN)
                	Align=A7M_MEM_ALIGN;
        	}
        	/* If the user supplied the alignment order, check if that order makes sense */
        	else
        	{
				if((Align_Order>=32)||(POW2(Align_Order)>Align))
					Main::Error("XXXXX: Alignment order too large.");
				if(Align_Order<5)
					Main::Error("XXXXX: Alignment order too small.");
				Align=POW2(Align_Order);
        	}
        }
        else
        {
            Align=A7M_MEM_ALIGN;
            if(Align_Order!=MEM_AUTO)
                Main::Error("XXXXX: Cannot designate fixed alignment for statically allocated memory.");
        }

        return Align;
    }
    catch(std::exception& Exc)
    {
        Main::Error(std::string("ARMv7-M (A7M) generator:\n")+Exc.what());
    }
}
/* End Function:A7M_Gen::Mem_Align *******************************************/

/* Function:A7M_Gen::Pgt_Total_Order ******************************************
Description : Get the total order and the begin address of the page table.
Input       : std::vector<std::unique_ptr<class Mem_Info>>& List - The memory block list.
Output      : ptr_t* Base - The base address of this page table.
Return      : ptr_t - The total order of the page table.
******************************************************************************/
ptr_t A7M_Gen::Pgt_Total_Order(std::vector<std::unique_ptr<class Mem_Info>>& List, ptr_t* Base)
{
    /* Begin is inclusive, end is exclusive */
    ptr_t Begin;
    ptr_t End;
    ptr_t Total_Order;

    /* What ranges does these stuff cover? */
    Begin=(ptr_t)(-1ULL);
    End=0;
    for(std::unique_ptr<class Mem_Info>& Mem:List)
    {
        if(Begin>Mem->Base)
            Begin=Mem->Base;
        if(End<(Mem->Base+Mem->Size))
            End=Mem->Base+Mem->Size;
    }

    /* If the total order less than 8, we wish to extend that to 8, because if we are smaller than
     * this it makes no sense. ARMv7-M MPU only allows subregions for regions more than 256 bytes */
    Total_Order=this->Pow2_Box(Begin, End);
    if(Total_Order<8)
        Total_Order=8;

    /* Do not shift more than 32 or we get undefined behavior */
    if(Total_Order==32)
        *Base=0;
    else
        *Base=ROUND_DOWN_POW2(Begin, Total_Order);

    return Total_Order;
}
/* End Function:A7M_Gen::Pgt_Total_Order ***********************************/

/* Function:A7M_Gen::Pgt_Num_Order ******************************************
Description : Get the number order of the page table.
Input       : std::vector<std::unique_ptr<class Mem_Info>>& List - The memory block list.
              ptr_t Total_Order - The total order of the page table.
              ptr_t Base - The begin address of the page table.
Output      : None.
Return      : ptr_t - The number order of the page table.
******************************************************************************/
ptr_t A7M_Gen::Pgt_Num_Order(std::vector<std::unique_ptr<class Mem_Info>>& List,
                             ptr_t Total_Order, ptr_t Base)
{
    ptr_t Num_Order;
    ptr_t Pivot_Cnt;
    ptr_t Pivot_Addr;
    ptr_t Cut_Apart;
    /* Whether the memory block is good */
    ptr_t Uniform;
    ptr_t Aligned;

    /* Can the memory segments get fully mapped in? If yes, there are two conditions
     * that must be met:
     * 1. There cannot be different access permissions in these memory segments.
     * 2. The memory begin address and the size must be fully divisible by POW2(Total_Order-3). */
    Uniform=1;
    Aligned=1;
    for(std::unique_ptr<class Mem_Info>& Mem:List)
    {
        if(Mem->Attr!=List[0]->Attr)
        	Uniform=0;
        if((Mem->Base%POW2(Total_Order-3))!=0)
        	Aligned=0;
        if((Mem->Size%POW2(Total_Order-3))!=0)
        	Aligned=0;
        if((Uniform==0)||(Aligned==0))
            break;
    }

    /* Is this directly mappable? If yes, we always create page tables with 8 pages. */
    if((Uniform!=0)&&(Aligned!=0))
    {
        /* Yes, it is directly mappable. We choose the smallest number order, in this way
         * we have the largest size order. This will leave us plenty of chances to use huge
         * pages, as this facilitates delegation as well. Number order = 0 is also possible,
         * as this maps in a single huge page. */
        for(Num_Order=0;Num_Order<=3;Num_Order++)
        {
        	Aligned=1;

            for(std::unique_ptr<class Mem_Info>& Mem:List)
            {
                if((Mem->Base%POW2(Total_Order-Num_Order))!=0)
                	Aligned=0;
                if((Mem->Size%POW2(Total_Order-Num_Order))!=0)
                	Aligned=0;
                if(Aligned==0)
                    break;
            }

            if(Aligned!=0)
                break;
        }

        if(Num_Order>3)
        	Main::Error("XXXXX: Internal number order miscalculation.");
    }
    /* Non-uniform attributes caused this to be unmappable. Avoid cutting contiguous regions
     * up because this can potentially increase our region usage */
    else if(Uniform==0)
    {
        /* Not directly mappable. What's the maximum number order that do not cut things apart? */
        Cut_Apart=0;
        for(Num_Order=1;Num_Order<=3;Num_Order++)
        {
            for(std::unique_ptr<class Mem_Info>& Mem:List)
            {
                for(Pivot_Cnt=1;Pivot_Cnt<POW2(Num_Order);Pivot_Cnt++)
                {
                    Pivot_Addr=Base+Pivot_Cnt*POW2(Total_Order-Num_Order);
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
    /* Uniform, but some is not aligned. Cut up as much as we can to reduce lower-level tables. */
    else if(Aligned==0)
    	Num_Order=3;
    else
    	Main::Error("XXXXX: Internal number order miscalculation.");

    return Num_Order;
}
/* End Function:A7M_Gen::Pgt_Num_Order *************************************/

/* Function:A7M_Gen::Page_Map *************************************************
Description : Map pages into the page table as we can.
Input       : std::vector<std::unique_ptr<class Mem>>& List - The memory block list.
              std::unique_ptr<class Pgtbl>& Pgt - The current page table.
Output      : std::unique_ptr<class Pgtbl>& Pgt - The updated current page table.
Return      : None.
******************************************************************************/
void A7M_Gen::Page_Map(std::vector<std::unique_ptr<class Mem_Info>>& List,
                       std::unique_ptr<class Pgtbl>& Pgt)
{
    ptr_t Attr;
    ptr_t Page_Cnt;
    ptr_t Page_Start;
    ptr_t Page_End;
    ptr_t Page_Num;
    ptr_t Page_Num_Max;
    ptr_t Cur_Attr;

    Main::Info("> Mapping pages into pgdir base 0x%llX size order %lld num order %lld.",
               Pgt->Base,Pgt->Size_Order,Pgt->Num_Order);

    /* Use the attribute that covers most pages - there are O(N) algorithms,
     * but this is easier to guarantee correctness */
    Page_Num_Max=0;
    for(std::unique_ptr<class Mem_Info>& Mem:List)
    {
        Cur_Attr=Mem->Attr;
        Page_Num=0;

        for(std::unique_ptr<class Mem_Info>& Mem:List)
        {
            if(Mem->Attr==Cur_Attr)
            {
                for(Page_Cnt=0;Page_Cnt<POW2(Pgt->Num_Order);Page_Cnt++)
                {
                    Page_Start=Pgt->Base+Page_Cnt*POW2(Pgt->Size_Order);
                    Page_End=Page_Start+POW2(Pgt->Size_Order);

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
    for(Page_Cnt=0;Page_Cnt<POW2(Pgt->Num_Order);Page_Cnt++)
    {
        Page_Start=Pgt->Base+Page_Cnt*POW2(Pgt->Size_Order);
        Page_End=Page_Start+POW2(Pgt->Size_Order);

        /* Can this compartment be mapped? It can if there is one segment covering the range */
        for(std::unique_ptr<class Mem_Info>& Mem:List)
        {
            if((Mem->Base<=Page_Start)&&((Mem->Base+Mem->Size)>=Page_End))
            {
                /* The attribute must be the same as what we map, cause we're mapping the stuff
                 * as a single region - we ignore the static attribute when comparing to map as much
                 * memory with the same attributes as possible */
                if((Attr&(~MEM_STATIC))==(Mem->Attr&(~MEM_STATIC)))
                {
                    Pgt->Page[Page_Cnt]=Mem->Attr;
                    Main::Info("> > Memory page base 0x%0llX size 0x%0llX mapped with attr 0x%0llX.",
                               Page_Start,Page_End-Page_Start,Mem->Attr);
                }
            }
        }
    }
}
/* End Function:A7M_Gen::Page_Map ********************************************/

/* Function:A7M_Gen::Pgdir_Map ************************************************
Description : Map page directories into the page table.
Input       : std::vector<std::unique_ptr<class Mem>>& List - The memory block list.
              class Process* Owner - The owner process of this kernel object.
              std::unique_ptr<class Pgtbl>& Pgt - The current page table.
Output      : std::unique_ptr<class Pgtbl>& Pgt - The updated current page table.
              ptr_t& Total_Static - The total number of static regions used.
Return      : None.
******************************************************************************/
void A7M_Gen::Pgdir_Map(std::vector<std::unique_ptr<class Mem_Info>>& List,
                        class Process* Owner, std::unique_ptr<class Pgtbl>& Pgt, ptr_t& Total_Static)
{
    ptr_t Base;
    ptr_t Size;
    ptr_t Page_Cnt;
    ptr_t Page_Start;
    ptr_t Page_End;
    std::vector<std::unique_ptr<class Mem_Info>> Child_List;

    Main::Info("> Mapping pgdirs into pgdir base 0x%llX size order %lld num order %lld.",
               Pgt->Base,Pgt->Size_Order,Pgt->Num_Order);

    for(Page_Cnt=0;Page_Cnt<POW2(Pgt->Num_Order);Page_Cnt++)
    {
        Page_Start=Pgt->Base+Page_Cnt*POW2(Pgt->Size_Order);
        Page_End=Page_Start+POW2(Pgt->Size_Order);

        if(Pgt->Page[Page_Cnt]==0)
        {
            /* See if any residue memory list are here */
            for(std::unique_ptr<class Mem_Info>& Mem:List)
            {
                if((Mem->Base>=Page_End)||((Mem->Base+Mem->Size)<=Page_Start))
                    continue;

                /* Round base address into this page slot */
                if(Mem->Base<Page_Start)
                    Base=Page_Start;
                else
                    Base=Mem->Base;

                /* Round the size to slot too */
                Size=Mem->Base+Mem->Size-Base;
                if((Base+Size)>Page_End)
                    Size=Page_End-Base;

                Main::Info("> > Residual memory base 0x%0llX size 0x%0llX into slot %lld.",Base,Size,Page_Cnt);
                Child_List.push_back(std::make_unique<class Mem_Info>("",Base,Size,Mem->Type,Mem->Attr));
            }

            /* Map in the child list if there are any at all */
            if(!Child_List.empty())
            {
                Pgt->Pgdir[Page_Cnt]=this->Pgt_Gen(Child_List,Owner,Pgt->Size_Order,Total_Static);
                /* Clean up what we have allocated */
                Child_List.clear();
            }
        }
    }
}
/* End Function:A7M_Gen::Pgdir_Map *******************************************/

/* Function:A7M_Gen::Pgt_Gen **************************************************
Description : Recursively construct the page table for the ARMv7-M port.
Input       : std::vector<std::unique_ptr<class Mem_Info>>& - The list containing
                                                              memory segments to fit
                                                              into this level (and below).
              class Process* Owner - The owner process of this kernel object.
              ptr_t Total_Max - The maximum total order of the page table, cannot
                                be exceeded when deciding the total order of the
                                page table.
Output      : ptr_t Total_Static - The total number of static regions used.
Return      : std::unique_ptr<class Pgtbl> - The page table structure returned. This
                                             function will never return NULL; if an
                                             error is encountered, it will directly
                                             error out.
******************************************************************************/
std::unique_ptr<class Pgtbl> A7M_Gen::Pgt_Gen(std::vector<std::unique_ptr<class Mem_Info>>& List,
                                              class Process* Owner, ptr_t Total_Max, ptr_t& Total_Static)
{
    ptr_t Base;
    ptr_t Num_Order;
    ptr_t Size_Order;
    ptr_t Total_Order;
    std::unique_ptr<class Pgtbl> Pgt;

    /* Total order and begin address of the page table */
    Total_Order=this->Pgt_Total_Order(List, &Base);
    /* See if this will violate the extension limit */
    if(Total_Order>Total_Max)
        Main::Error("XXXXX: Memory segment too small, cannot find a reasonable placement.");

    /* Number order */
    Num_Order=this->Pgt_Num_Order(List, Total_Order, Base);
    /* Size order */
    Size_Order=Total_Order-Num_Order;

    /* Page table attributes are in fact not used in A7M, we always set to full attributes */
    Pgt=std::make_unique<class Pgtbl>(Base, Size_Order, Num_Order, MEM_FULL, Owner);
    Main::Info("> Creating pgdir base 0x%llX size order %lld num order %lld.",Base,Size_Order,Num_Order);
    /* Map in all pages */
    this->Page_Map(List, Pgt);
    /* Map in all page directories - recursive */
    this->Pgdir_Map(List, Owner, Pgt, Total_Static);

    /* If a single page is static, we now have a static MPU region */
    for(ptr_t Page:Pgt->Page)
    {
        if((Page&MEM_STATIC)!=0)
        {
            Total_Static++;
            break;
        }
    }

    return Pgt;
}
/* End Function:A7M::Pgt_Gen *************************************************/

/* Function:A7M_Gen::Pgt_Gen **************************************************
Description : Construct the raw page table MPU cache for the ARMv7-M port.
Input       : std::vector<std::unique_ptr<class Mem_Info>>& - The list containing
                                                              memory segments.
Output      : ptr_t Total_Static - The total number of static regions used.
Return      : std::unique_ptr<std::vector<ptr_t>> - The generated content.
******************************************************************************/
std::unique_ptr<std::vector<ptr_t>> A7M_Gen::Pgt_Gen(std::vector<std::unique_ptr<class Mem_Info>>& List,
                                                     ptr_t& Total_Static)
{
    std::unique_ptr<std::vector<ptr_t>> Pgt;
    std::unique_ptr<std::vector<std::unique_ptr<class Mem_Info>>> Info;
    std::unique_ptr<std::vector<std::unique_ptr<class Mem_Info>>> Left;
    ptr_t Count;
    ptr_t Order;
    ptr_t Contain;
    ptr_t Base;
    ptr_t Size;
    ptr_t Begin;
    ptr_t End;
    ptr_t Attr;
    ptr_t SRD;
    ptr_t RBAR;
    ptr_t RASR;

    Pgt=std::make_unique<std::vector<ptr_t>>();
    Info=std::make_unique<std::vector<std::unique_ptr<class Mem_Info>>>();

    /* Copy the memory list */
    for(std::unique_ptr<class Mem_Info>& Mem:List)
        Info->push_back(std::make_unique<class Mem_Info>(Mem.get(),Mem->Attr));

    /* The regions have been sorted. Just dump MPU regions at them, until they're empty */
    Total_Static=0;
    while(!Info->empty())
    {
        /* Choose the smaller order of the base and size */
        Order=this->Pow2_Align(Info->front()->Base);
        Contain=this->Pow2_Contain(Info->front()->Size);
        Attr=Info->front()->Attr&~MEM_STATIC;
        if(Order>Contain)
            Order=Contain;

        /* Use as many subregions as possible, 8 subregions, thus +3 */
        Order+=3;
        if(Order>this->Plat->Wordlength)
            Order=this->Plat->Wordlength;
        /* Subregion size */
        Size=POW2(Order-3);
        /* Region base */
        Base=ROUND_DOWN_POW2(Info->front()->Base, Order);
        Main::Info("> Mapping pages into MPU fixed raw region base 0x%llX size order %lld.", Base, Order);

        /* Map in as much from the list as possible */
        SRD=0xFF;
        for(Count=0;Count<8;Count++)
        {
            Begin=Base+Size*Count;
            End=Begin+Size;
            for(std::unique_ptr<class Mem_Info>& Mem:*Info)
            {
                if((Mem->Base<=Begin)&&((Mem->Base+Mem->Size)>=End)&&((Mem->Attr&~MEM_STATIC)==Attr))
                {
                    SRD&=~POW2(Count);
                    Main::Info("> > Memory page base 0x%0llX size 0x%0llX mapped with attr 0x%0llX.", Begin, Size, Mem->Attr);
                    break;
                }
            }
        }

        /* Generate the MPU pair */
        RBAR=Base|A7M_RBAR_VALID|Total_Static;
        RASR=A7M_RASR_ENABLE;

        if((Attr&MEM_WRITE)!=0)
            RASR|=A7M_RASR_RW;
        else
            RASR|=A7M_RASR_RO;
        if((Attr&MEM_EXECUTE)==0)
            RASR|=A7M_RASR_XN;
        if((Attr&MEM_CACHE)!=0)
            RASR|=A7M_RASR_C;
        if((Attr&MEM_BUFFER)!=0)
            RASR|=A7M_RASR_B;

        /* Push to data array */
        RASR|=A7M_RASR_SIZE(Order);
        RASR|=SRD<<8;
        Total_Static++;
        Pgt->push_back(RBAR);
        Pgt->push_back(RASR);

        /* Generate leftovers - for each slot that have mapped something,
         * the mapped portion must be removed from the current list */
        for(Count=0;Count<8;Count++)
        {
            Left=std::make_unique<std::vector<std::unique_ptr<class Mem_Info>>>();
            Begin=Base+Size*Count;
            End=Begin+Size;
            /* This subregion is not mapped, skip it */
            if((SRD&POW2(Count))!=0)
                continue;
            /* This subregion is mapped. Remove mapped parts */
            for(std::unique_ptr<class Mem_Info>& Mem:*Info)
            {
                /* Not mapped, keep it */
                if(((Mem->Base+Mem->Size)<=Begin)||(Mem->Base>=End)||(((Mem->Attr&~MEM_STATIC)!=Attr)))
                    Left->push_back(std::make_unique<class Mem_Info>(Mem.get(),Mem->Attr));
                /* This memory segment is mapped. Remove mapped parts */
                else
                {
                    if(Mem->Base<Begin)
                    {
                        Left->push_back(std::make_unique<class Mem_Info>("",Mem->Base,Begin-Mem->Base,Mem->Type,Mem->Attr));
                        Main::Info("> > Residual memory base 0x%0llX size 0x%0llX.",Mem->Base,Begin-Mem->Base);
                    }
                    if((Mem->Base+Mem->Size)>End)
                    {
                        Left->push_back(std::make_unique<class Mem_Info>("",End,Mem->Base+Mem->Size-End,Mem->Type,Mem->Attr));
                        Main::Info("> > Residual memory base 0x%0llX size 0x%0llX.",End,Mem->Base+Mem->Size-End);
                    }
                }
            }
            /* Until mapped subregions are all accounted for */
            Info=std::move(Left);
        }
    }

    /* Fill the rest with useless data, but set REGION to avoid overwriting useful entries */
    for(Count=Total_Static;Count<this->Chip->Region;Count++)
    {
        Pgt->push_back(A7M_RBAR_VALID|Count);
        Pgt->push_back(0);
    }

    return Pgt;
}
/* End Function:A7M::Pgt_Gen *************************************************/

/* Function:A7M_Gen::Raw_Pgt **************************************************
Description : Query the size of page table given the parameters.
Input       : ptr_t Num_Order - The number order.
              ptr_t Is_Top - Whether this is a top-level.
Output      : None.
Return      : ptr_t - The size in bytes.
******************************************************************************/
ptr_t A7M_Gen::Raw_Pgt(ptr_t Num_Order, ptr_t Is_Top)
{
    if(Is_Top!=0)
        return A7M_RAW_PGT_SIZE_TOP(Num_Order, this->Chip->Region);
    else
        return A7M_RAW_PGT_SIZE_NOM(Num_Order);
}
/* End Function:A7M_Gen::Raw_Pgt *********************************************/

/* Function:A7M_Gen::Raw_Thread ***********************************************
Description : Query the size of the minimal thread object.
Input       : None.
Output      : None.
Return      : ptr_t - The size in bytes.
******************************************************************************/
ptr_t A7M_Gen::Raw_Thread(void)
{
    return A7M_RAW_HYP_SIZE;
}
/* End Function:A7M_Gen::Raw_Thread ******************************************/

/* Function:A7M_Gen::Raw_Invocation *******************************************
Description : Query the size of a invocation.
Input       : None¡£
Output      : None.
Return      : ptr_t - The size in bytes.
******************************************************************************/
ptr_t A7M_Gen::Raw_Invocation(void)
{
    return A7M_RAW_INV_SIZE;
}
/* End Function:A7M_Gen::Raw_Invocation **************************************/

/* Function:A7M_Gen::Raw_Register *********************************************
Description : Query the size of the register set.
Input       : const std::vector<std::string>& Coprocessor - The coprocessor list.
Output      : None.
Return      : ptr_t - The size in bytes.
******************************************************************************/
ptr_t A7M_Gen::Raw_Register(const std::vector<std::string>& Coprocessor)
{
    if(Coprocessor.empty())
        return A7M_RAW_REG_SIZE;
    else
        return A7M_RAW_REG_FPU_SIZE;
}
/* End Function:A7M_Gen::Raw_Register ****************************************/

/* Function:A7M_Gen::Kernel_Conf_Hdr ******************************************
Description : Replace kernel configuration header macros.
Input       : std::unique_ptr<std::vector<std::string>>& List - The input file.
Output      : std::unique_ptr<std::vector<std::string>>& List - The modified file.
Return      : None.
******************************************************************************/
void A7M_Gen::Kernel_Conf_Hdr(std::unique_ptr<std::vector<std::string>>& List)
{
    /* Init process's first thread's entry point address is always at head */
    Gen_Tool::Macro_Hex(List, "RME_A7M_INIT_ENTRY",PRC_DESC_ALIGN(this->Proj->Monitor->Code_Base)|0x01, MACRO_REPLACE);
    /* Init process's first thread's stack address */
    Gen_Tool::Macro_Hex(List, "RME_A7M_INIT_STACK",
                        this->Proj->Monitor->Init_Stack_Base+this->Proj->Monitor->Init_Stack_Size-16, MACRO_REPLACE);
    /* What is the NVIC priority grouping? */
    Gen_Tool::Macro_String(List,"RME_A7M_NVIC_GROUPING",
                           std::string("RME_A7M_NVIC_GROUPING_")+this->Proj->Chip->Config["NVIC_Grouping"], MACRO_REPLACE);
    /* What is the Systick value? - (usually) 10ms per tick */
    Gen_Tool::Macro_String(List, "RME_A7M_SYSTICK_VAL", this->Proj->Chip->Config["Systick_Value"]+"U", MACRO_REPLACE);

    /* Fixed attributes - we will refill these with database values */
    /* Number of MPU regions available */
    Gen_Tool::Macro_Int(List, "RME_A7M_REGION_NUM", this->Chip->Region, MACRO_REPLACE);

    /* CPU & Endianness currently unused */
}
/* End Function:A7M_Gen::Kernel_Conf_Hdr *************************************/

/* Function:A7M_Gen::Monitor_Conf_Hdr *****************************************
Description : Replace monitor configuration header macros.
Input       : std::unique_ptr<std::vector<std::string>>& List - The input file.
Output      : std::unique_ptr<std::vector<std::string>>& List - The modified file.
Return      : None.
******************************************************************************/
void A7M_Gen::Monitor_Conf_Hdr(std::unique_ptr<std::vector<std::string>>& List)
{
    /* Init process's first thread's entry point address is always at head */
    Gen_Tool::Macro_Hex(List, "RVM_A7M_INIT_ENTRY", PRC_DESC_ALIGN(this->Proj->Monitor->Code_Base)|0x01, MACRO_REPLACE);
    /* Init process's first thread's stack address */
    Gen_Tool::Macro_Hex(List, "RVM_A7M_INIT_STACK",
                        this->Proj->Monitor->Init_Stack_Base+this->Proj->Monitor->Init_Stack_Size-16, MACRO_REPLACE);
    /* Fixed attributes - we will refill these with database values */
    /* Number of MPU regions available */
    Gen_Tool::Macro_Int(List, "RVM_A7M_REGION_NUM", this->Chip->Region, MACRO_REPLACE);

    /* CPU & Endianness currently unused */
}
/* End Function:A7M_Gen::Monitor_Conf_Hdr ************************************/

/* Function:A7M_Gen::Process_Main_Hdr *****************************************
Description : Replace process main header macros.
Input       : std::unique_ptr<std::vector<std::string>>& List - The input file.
              const class Process* Prc - The process information.
Output      : std::unique_ptr<std::vector<std::string>>& List - The modified file.
Return      : None.
******************************************************************************/
void A7M_Gen::Process_Main_Hdr(std::unique_ptr<std::vector<std::string>>& List,
		                       const class Process* Prc)
{
    /* CPU & Endianness currently unused */
}
/* End Function:A7M_Gen::Process_Main_Hdr ************************************/
}
/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
