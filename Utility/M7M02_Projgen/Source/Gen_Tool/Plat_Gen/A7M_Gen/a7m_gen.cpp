/******************************************************************************
Filename    : a7m_gen.cpp
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

#include "map"
#include "string"
#include "memory"
#include "vector"
#include "stdexcept"
#include "algorithm"

#define __HDR_DEFS__
#include "rvm_gen.hpp"
#include "Mem_Info/mem_info.hpp"
#include "Gen_Tool/Plat_Gen/plat_gen.hpp"
#include "Gen_Tool/Plat_Gen/A7M_Gen/a7m_gen.hpp"
#undef __HDR_DEFS__

#define __HDR_CLASSES__
#include "rvm_gen.hpp"
#include "Mem_Info/mem_info.hpp"
#include "Proj_Info/Kobj/kobj.hpp"
#include "Proj_Info/Process/Pgtbl/pgtbl.hpp"
#include "Gen_Tool/Plat_Gen/plat_gen.hpp"
#include "Gen_Tool/Plat_Gen/plat_gen.hpp"
#include "Gen_Tool/Plat_Gen/A7M_Gen/a7m_gen.hpp"
#undef __HDR_STRUCTS__
/* End Includes **************************************************************/
namespace RVM_GEN
{
/* Begin Function:A7M_Gen::A7M_Gen ********************************************
Description : Generator for the ARMv7-M platform.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
/* void */ A7M_Gen::A7M_Gen(void):
Plat_Gen("A7M")
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

/* Begin Function:A7M_Gen::Compatible_Get *************************************
Description : Extract compatible generator options.
Input       : ptr_t Base - The memory base address.
              ptr_t Size - The memory size.
Output      : None.
Return      : ptr_t - The computed alignment.
******************************************************************************/
void A7M_Gen::Compatible_Get(std::vector<std::tuple<std::string,std::string,std::string>>& List)
{
    List.push_back(std::make_tuple("Keil","ARMCC","Native"));
    List.push_back(std::make_tuple("Makefile","GCC","Native"));
    List.push_back(std::make_tuple("Keil","ARMCC","RMP"));
    List.push_back(std::make_tuple("Makefile","GCC","RMP"));
}
/* End Function:A7M_Gen::Compatible_Get **************************************/

/* Begin Function:A7M_Gen::Mem_Align ******************************************
Description : Memory aligner for the ARMv7-M platform.
Input       : ptr_t Base - The memory base address.
              ptr_t Size - The memory size.
Output      : None.
Return      : ptr_t - The computed alignment.
******************************************************************************/
ptr_t A7M_Gen::Mem_Align(ptr_t Base, ptr_t Size)
{
    ptr_t Align;

    try
    {
        /* Check all sensitive memory block allocation */
        if(Base!=MEM_AUTO)
        {
            if((Base%A7M_MEM_ALIGN)!=0)
                Main::Error("A0100: Static memory base address not properly aligned.");
        }

        if((Size%A7M_MEM_ALIGN)!=0)
            Main::Error("A0100: Memory size not properly aligned.");

        /* This memory's start address is not designated yet. Decide its size after
         * alignment and calculate its start address alignment granularity */
        if(Base==MEM_AUTO)
        {

             /* For ARMv7-M, the minimum granularity is 1/8 of the nearest power of 2 for the size */
            Align=1;
            while(Align<Size)
                Align<<=1;
        }
        else
            Align=A7M_MEM_ALIGN;

        return Align;
    }
    catch(std::exception& Exc)
    {
        Main::Error(std::string("ARMv7-M (A7M) generator:\n")+Exc.what());
    }
}
/* End Function:A7M_Gen::Mem_Align *******************************************/

/* Begin Function:A7M_Gen::Pgtbl_Total_Order **********************************
Description : Get the total order and the start address of the page table.
Input       : std::vector<std::unique_ptr<class Mem_Info>>& List - The memory block list.
Output      : ptr_t* Base - The base address of this page table.
Return      : ptr_t - The total order of the page table.
******************************************************************************/
ptr_t A7M_Gen::Pgtbl_Total_Order(std::vector<std::unique_ptr<class Mem_Info>>& List, ptr_t* Base)
{
    /* Start is inclusive, end is exclusive */
    ptr_t Start;
    ptr_t End;
    ptr_t Total_Order;

    /* What ranges does these stuff cover? */
    Start=(ptr_t)(-1ULL);
    End=0;
    for(std::unique_ptr<class Mem_Info>& Mem:List)
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
        if(End<=(ROUND_DOWN_POW2(Start, Total_Order)+POW2(Total_Order)))
            break;
        Total_Order++;
    }

    /* If the total order less than 8, we wish to extend that to 8, because if we are smaller than this it makes no sense */
    if(Total_Order<8)
        Total_Order=8;

    /* Do not shift more than 32 or we get undefined behavior */
    if(Total_Order==32)
        *Base=0;
    else
        *Base=ROUND_DOWN_POW2(Start, Total_Order);

    return Total_Order;
}
/* End Function:A7M_Gen::Pgtbl_Total_Order ***********************************/

/* Begin Function:A7M_Gen::Pgtbl_Num_Order ************************************
Description : Get the number order of the page table.
Input       : std::vector<std::unique_ptr<class Mem_Info>>& List - The memory block list.
              ptr_t Total_Order - The total order of the page table.
              ptr_t Base - The start address of the page table.
Output      : None.
Return      : ptr_t - The number order of the page table.
******************************************************************************/
ptr_t A7M_Gen::Pgtbl_Num_Order(std::vector<std::unique_ptr<class Mem_Info>>& List,
                               ptr_t Total_Order, ptr_t Base)
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
    for(std::unique_ptr<class Mem_Info>& Mem:List)
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

            for(std::unique_ptr<class Mem_Info>& Mem:List)
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

    return Num_Order;
}
/* End Function:A7M_Gen::Pgtbl_Num_Order *************************************/

/* Begin Function:A7M_Gen::Page_Map *******************************************
Description : Map pages into the page table as we can.
Input       : std::vector<std::unique_ptr<class Mem>>& List - The memory block list.
              std::unique_ptr<class Pgtbl>& Pgtbl - The current page table.
Output      : std::unique_ptr<class Pgtbl>& Pgtbl - The updated current page table.
Return      : None.
******************************************************************************/
void A7M_Gen::Page_Map(std::vector<std::unique_ptr<class Mem_Info>>& List,
                       std::unique_ptr<class Pgtbl>& Pgtbl)
{
    ptr_t Attr;
    ptr_t Page_Cnt;
    ptr_t Page_Start;
    ptr_t Page_End;
    ptr_t Page_Num;
    ptr_t Page_Num_Max;
    ptr_t Cur_Attr;

    Main::Info("> Mapping pages into pgdir base 0x%llX size order %lld num order %lld.",
               Pgtbl->Base,Pgtbl->Size_Order,Pgtbl->Num_Order);

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
                for(Page_Cnt=0;Page_Cnt<POW2(Pgtbl->Num_Order);Page_Cnt++)
                {
                    Page_Start=Pgtbl->Base+Page_Cnt*POW2(Pgtbl->Size_Order);
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
        Page_Start=Pgtbl->Base+Page_Cnt*POW2(Pgtbl->Size_Order);
        Page_End=Page_Start+POW2(Pgtbl->Size_Order);

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
                    Pgtbl->Page[Page_Cnt]=Mem->Attr;
                    Main::Info("> > Memory page base 0x%0llX size 0x%0llX mapped with attr 0x%0llX.",
                               Page_Start,Page_End-Page_Start,Mem->Attr);
                }
            }
        }
    }
}
/* End Function:A7M_Gen::Page_Map ********************************************/

/* Begin Function:A7M_Gen::Pgdir_Map ******************************************
Description : Map page directories into the page table.
Input       : std::vector<std::unique_ptr<class Mem>>& List - The memory block list.
              class Process* Owner - The owner process of this kernel object.
              std::unique_ptr<class Pgtbl>& Pgtbl - The current page table.
Output      : std::unique_ptr<class Pgtbl>& Pgtbl - The updated current page table.
              ptr_t& Total_Static - The total number of static regions used.
Return      : None.
******************************************************************************/
void A7M_Gen::Pgdir_Map(std::vector<std::unique_ptr<class Mem_Info>>& List,
                        class Process* Owner, std::unique_ptr<class Pgtbl>& Pgtbl, ptr_t& Total_Static)
{
    ptr_t Base;
    ptr_t Size;
    ptr_t Page_Cnt;
    ptr_t Page_Start;
    ptr_t Page_End;
    std::vector<std::unique_ptr<class Mem_Info>> Child_List;

    Main::Info("> Mapping pgdirs into pgdir base 0x%llX size order %lld num order %lld.",
               Pgtbl->Base,Pgtbl->Size_Order,Pgtbl->Num_Order);

    for(Page_Cnt=0;Page_Cnt<POW2(Pgtbl->Num_Order);Page_Cnt++)
    {
        Page_Start=Pgtbl->Base+Page_Cnt*POW2(Pgtbl->Size_Order);
        Page_End=Page_Start+POW2(Pgtbl->Size_Order);

        if(Pgtbl->Page[Page_Cnt]==0)
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
                Pgtbl->Pgdir[Page_Cnt]=this->Pgtbl_Gen(Child_List,Owner,Pgtbl->Size_Order,Total_Static);
                /* Clean up what we have allocated */
                Child_List.clear();
            }
        }
    }
}
/* End Function:A7M_Gen::Pgdir_Map *******************************************/

/* Begin Function:A7M_Gen:Pgtbl_Gen *******************************************
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
std::unique_ptr<class Pgtbl> A7M_Gen::Pgtbl_Gen(std::vector<std::unique_ptr<class Mem_Info>>& List,
                                                class Process* Owner, ptr_t Total_Max, ptr_t& Total_Static)
{
    ptr_t Base;
    ptr_t Num_Order;
    ptr_t Size_Order;
    ptr_t Total_Order;
    std::unique_ptr<class Pgtbl> Pgtbl;

    /* Total order and start address of the page table */
    Total_Order=this->Pgtbl_Total_Order(List, &Base);
    /* See if this will violate the extension limit */
    if(Total_Order>Total_Max)
        Main::Error("A0201: Memory segment too small, cannot find a reasonable placement.");

    /* Number order */
    Num_Order=this->Pgtbl_Num_Order(List, Total_Order, Base);
    /* Size order */
    Size_Order=Total_Order-Num_Order;

    /* Page table attributes are in fact not used in A7M, we always set to full attributes */
    Pgtbl=std::make_unique<class Pgtbl>(Base, Size_Order, Num_Order, MEM_FULL, Owner);
    Main::Info("> Creating pgdir base 0x%llX size order %lld num order %lld.",Base,Size_Order,Num_Order);
    /* Map in all pages */
    this->Page_Map(List, Pgtbl);
    /* Map in all page directories - recursive */
    this->Pgdir_Map(List, Owner, Pgtbl, Total_Static);

    /* If a single page is static, we now have a static MPU region */
    for(ptr_t Page:Pgtbl->Page)
    {
        if((Page&MEM_STATIC)!=0)
        {
            Total_Static++;
            break;
        }
    }

    return Pgtbl;
}
/* End Function:A7M::Gen_Pgtbl ***********************************************/
}
/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/