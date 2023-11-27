/******************************************************************************
Filename    : rv32p_gen.cpp
Author      : pry
Date        : 12/07/2019
Licence     : LGPL v3+; see COPYING for details.
Description : This toolset is for RISC-V 32-bit PMP.
              RISC-V PMP can map any number of static and dynamic pages, with
              any power-of-2 that is bigger than 16.
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
#include "Gen_Tool/Plat_Gen/RV32P_Gen/rv32p_gen.hpp"
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
#include "Gen_Tool/Plat_Gen/RV32P_Gen/rv32p_gen.hpp"
#undef __HDR_CLASS__
/* End Include ***************************************************************/
namespace RVM_GEN
{
/* Function:RV32P_Gen::RV32P_Gen **************************************************
Description : Generator for the ARMv7-M platform.
Input       : class Proj_Info* Proj - The project information.
              class Plat_Info* Plat - The platform information.
              class Chip_Info* Chip - The chip information.
Output      : None.
Return      : None.
******************************************************************************/
/* void */ RV32P_Gen::RV32P_Gen(class Proj_Info* Proj, class Plat_Info* Plat, class Chip_Info* Chip):
Plat_Gen("RV32P", Proj, Plat, Chip)
{
    try
    {

    }
    catch(std::exception& Exc)
    {
        Main::Error(std::string("RISC-V 32-bit PMP (RV32P) generator:\n")+Exc.what());
    }
}
/* End Function:RV32P_Gen::RV32P_Gen *********************************************/

/* Function:RV32P_Gen::Mem_Align ************************************************
Description : Memory aligner for the ARMv7-M platform.
Input       : ptr_t Base - The memory base address.
              ptr_t Size - The memory size.
              ptr_t Align_Order - The user suggested align order.
Output      : None.
Return      : ptr_t - The computed alignment.
******************************************************************************/
ptr_t RV32P_Gen::Mem_Align(ptr_t Base, ptr_t Size, ptr_t Align_Order)
{
    ptr_t Align;

    try
    {
        /* Check all sensitive memory block allocation */
        if(Base!=MEM_AUTO)
        {
            if((Base%RV32P_MEM_ALIGN)!=0)
                Main::Error("XXXXX: Static memory base address not properly aligned.");
        }

        if((Size%RV32P_MEM_ALIGN)!=0)
            Main::Error("XXXXX: Memory size not properly aligned.");

        /* This memory's start address is not designated yet. Decide its size after
         * alignment and calculate its start address alignment granularity */
        if(Base==MEM_AUTO)
        {
        	/* Compute maximum alignment that make sense for this memory trunk */
            Align=1;
            while(Align<Size)
                Align<<=1;

            /* If the user did not supply an alignment order, produce an appropriate default.
             * For RV32P, the default is 1/16 of the nearest power of 2 for the size. RV32P
             * itself does not put restrictions on this, but we don't want to cut memory into
             * too many pieces */
        	if(Align_Order==MEM_AUTO)
        	{
                Align>>=4;
                if(Align<RV32P_MEM_ALIGN)
                	Align=RV32P_MEM_ALIGN;
        	}
        	/* If the user supplied the alignment order, check if that order makes sense */
        	else
        	{
				if((Align_Order>=32)||(POW2(Align_Order)>Align))
					Main::Error("XXXXX: Alignment order too large.");
				if(Align_Order<4)
					Main::Error("XXXXX: Alignment order too small.");
				Align=POW2(Align_Order);
        	}
        }
        else
        {
            Align=RV32P_MEM_ALIGN;
            if(Align_Order!=MEM_AUTO)
                Main::Error("XXXXX: Cannot designate fixed alignment for statically allocated memory.");
        }

        return Align;
    }
    catch(std::exception& Exc)
    {
        Main::Error(std::string("RISC-V 32-bit PMP (RV32P) generator:\n")+Exc.what());
    }
}
/* End Function:RV32P_Gen::Mem_Align *******************************************/

/* Function:RV32P_Gen::Pgt_Total_Order ****************************************
Description : Get the total order and the start address of the page table.
Input       : std::vector<std::unique_ptr<class Mem_Info>>& List - The memory block list.
Output      : ptr_t* Base - The base address of this page table.
Return      : ptr_t - The total order of the page table.
******************************************************************************/
ptr_t RV32P_Gen::Pgt_Total_Order(std::vector<std::unique_ptr<class Mem_Info>>& List, ptr_t* Base)
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

    /* If the total order less than 4, we wish to extend that to 4 */
    if(Total_Order<4)
        Total_Order=4;

    /* Do not shift more than 32 or we get undefined behavior */
    if(Total_Order==32)
        *Base=0;
    else
        *Base=ROUND_DOWN_POW2(Start, Total_Order);

    return Total_Order;
}
/* End Function:RV32P_Gen::Pgt_Total_Order ***********************************/

/* Function:RV32P_Gen::Pgt_Num_Order ******************************************
Description : Get the number order of the page table.
Input       : std::vector<std::unique_ptr<class Mem_Info>>& List - The memory block list.
              ptr_t Total_Order - The total order of the page table.
              ptr_t Base - The start address of the page table.
Output      : None.
Return      : ptr_t - The number order of the page table.
******************************************************************************/
ptr_t RV32P_Gen::Pgt_Num_Order(std::vector<std::unique_ptr<class Mem_Info>>& List,
                               ptr_t Total_Order, ptr_t Base)
{
    ptr_t Num_Order;
    ptr_t Aligned;

    /* We choose the smallest number order, in this way we have the largest size
     * order. This will leave us plenty of chances to use huge pages, as this
     * facilitates delegation as well. Number order = 0 is also possible, as this
     * maps in a single huge page. Don't increase the number order aggressively,
     * or it will take too much RAM. The current limit is 4. */
    for(Num_Order=0;Num_Order<4;Num_Order++)
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

    return Num_Order;
}
/* End Function:RV32P_Gen::Pgt_Num_Order *************************************/

/* Function:RV32P_Gen::Page_Map ***********************************************
Description : Map pages into the page table as we can.
Input       : std::vector<std::unique_ptr<class Mem>>& List - The memory block list.
              std::unique_ptr<class Pgtbl>& Pgt - The current page table.
Output      : std::unique_ptr<class Pgtbl>& Pgt - The updated current page table.
Return      : None.
******************************************************************************/
void RV32P_Gen::Page_Map(std::vector<std::unique_ptr<class Mem_Info>>& List,
                         std::unique_ptr<class Pgtbl>& Pgt)
{
    ptr_t Page_Cnt;
    ptr_t Page_Start;
    ptr_t Page_End;

    Main::Info("> Mapping pages into pgdir base 0x%llX size order %lld num order %lld.",
               Pgt->Base,Pgt->Size_Order,Pgt->Num_Order);

    /* Map whatever we can map, and postpone whatever we will have to postpone */
    for(Page_Cnt=0;Page_Cnt<POW2(Pgt->Num_Order);Page_Cnt++)
    {
        Page_Start=Pgt->Base+Page_Cnt*POW2(Pgt->Size_Order);
        Page_End=Page_Start+POW2(Pgt->Size_Order);

        /* Map whatever that we can map */
        for(std::unique_ptr<class Mem_Info>& Mem:List)
        {
            if((Mem->Base<=Page_Start)&&((Mem->Base+Mem->Size)>=Page_End))
            {
                Pgt->Page[Page_Cnt]=Mem->Attr;
                Main::Info("> > Memory page base 0x%0llX size 0x%0llX mapped with attr 0x%0llX.",
                           Page_Start,Page_End-Page_Start,Mem->Attr);
            }
        }
    }
}
/* End Function:RV32P_Gen::Page_Map ******************************************/

/* Function:RV32P_Gen::Pgdir_Map **********************************************
Description : Map page directories into the page table.
Input       : std::vector<std::unique_ptr<class Mem>>& List - The memory block list.
              class Process* Owner - The owner process of this kernel object.
              std::unique_ptr<class Pgtbl>& Pgt - The current page table.
Output      : std::unique_ptr<class Pgtbl>& Pgt - The updated current page table.
              ptr_t& Total_Static - The total number of static regions used.
Return      : None.
******************************************************************************/
void RV32P_Gen::Pgdir_Map(std::vector<std::unique_ptr<class Mem_Info>>& List,
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
/* End Function:RV32P_Gen::Pgdir_Map *****************************************/

/* Function:RV32P_Gen::Pgt_Gen ************************************************
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
std::unique_ptr<class Pgtbl> RV32P_Gen::Pgt_Gen(std::vector<std::unique_ptr<class Mem_Info>>& List,
                                                class Process* Owner, ptr_t Total_Max, ptr_t& Total_Static)
{
    ptr_t Base;
    ptr_t Num_Order;
    ptr_t Size_Order;
    ptr_t Total_Order;
    std::unique_ptr<class Pgtbl> Pgt;

    /* Total order and start address of the page table */
    Total_Order=this->Pgt_Total_Order(List, &Base);
    /* See if this will violate the extension limit */
    if(Total_Order>Total_Max)
        Main::Error("XXXXX: Memory segment too small, cannot find a reasonable placement.");

    /* Number order */
    Num_Order=this->Pgt_Num_Order(List, Total_Order, Base);
    /* Size order */
    Size_Order=Total_Order-Num_Order;

    /* Page table attributes are in fact not used in RV32P, we always set to full attributes */
    Pgt=std::make_unique<class Pgtbl>(Base, Size_Order, Num_Order, MEM_FULL, Owner);
    Main::Info("> Creating pgdir base 0x%llX size order %lld num order %lld.",Base,Size_Order,Num_Order);
    /* Map in all pages */
    this->Page_Map(List, Pgt);
    /* Map in all page directories - recursive */
    this->Pgdir_Map(List, Owner, Pgt, Total_Static);

    /* The RV32P will not lock-down any static regions, hence always 0 */
    Total_Static=0;

    return Pgt;
}
/* End Function:RV32P::Gen_Pgt ***********************************************/

/* Function:RV32P_Gen::Raw_Pgt ************************************************
Description : Query the size of page table given the parameters.
Input       : ptr_t Num_Order - The number order.
              ptr_t Is_Top - Whether this is a top-level.
Output      : None.
Return      : ptr_t - The size in bytes.
******************************************************************************/
ptr_t RV32P_Gen::Raw_Pgt(ptr_t Num_Order, ptr_t Is_Top)
{
    if(Is_Top!=0)
        return RV32P_RAW_PGT_SIZE_TOP(Num_Order, this->Chip->Region);
    else
        return RV32P_RAW_PGT_SIZE_NOM(Num_Order);
}
/* End Function:RV32P_Gen::Size_Pgt ******************************************/

/* Function:RV32P_Gen::Raw_Thread *********************************************
Description : Query the size of the minimal thread object.
Input       : None.
Output      : None.
Return      : ptr_t - The size in bytes.
******************************************************************************/
ptr_t RV32P_Gen::Raw_Thread(void)
{
    return RV32P_RAW_HYP_SIZE;
}
/* End Function:RV32P_Gen::Raw_Thread ****************************************/

/* Function:RV32P_Gen::Raw_Invocation *****************************************
Description : Query the size of a invocation.
Input       : None¡£
Output      : None.
Return      : ptr_t - The size in bytes.
******************************************************************************/
ptr_t RV32P_Gen::Raw_Invocation(void)
{
    return RV32P_RAW_INV_SIZE;
}
/* End Function:RV32P_Gen::Raw_Invocation ************************************/

/* Function:RV32P_Gen::Raw_Register *******************************************
Description : Query the size of the register set.
Input       : const std::vector<std::string>& Coprocessor - The coprocessor list.
Output      : None.
Return      : ptr_t - The size in bytes.
******************************************************************************/
ptr_t RV32P_Gen::Raw_Register(const std::vector<std::string>& Coprocessor)
{
    if(Coprocessor.empty())
        return RV32P_RAW_REG_SIZE;
    else
        return RV32P_RAW_REG_FPU_SIZE;
}
/* End Function:RV32P_Gen::Raw_Register **************************************/

/* Function:RV32P_Gen::Kernel_Conf_Hdr ****************************************
Description : Replace kernel configuration header macros.
Input       : std::unique_ptr<std::vector<std::string>>& List - The input file.
Output      : std::unique_ptr<std::vector<std::string>>& List - The modified file.
Return      : None.
******************************************************************************/
void RV32P_Gen::Kernel_Conf_Hdr(std::unique_ptr<std::vector<std::string>>& List)
{
    /* Init process's first thread's entry point address */
    if (Proj->Buildsystem=="Makefile")
        Gen_Tool::Macro_Hex(List, "RME_RV32P_INIT_ENTRY",PRC_DESC_ALIGN(this->Proj->Monitor->Code_Base), MACRO_REPLACE);
    else
        Gen_Tool::Macro_Hex(List, "RME_RV32P_INIT_ENTRY",PRC_DESC_ALIGN(this->Proj->Monitor->Code_Base+8*4), MACRO_REPLACE);
    /* Init process's first thread's stack address */
    Gen_Tool::Macro_Hex(List, "RME_RV32P_INIT_STACK",
                        this->Proj->Monitor->Init_Stack_Base+this->Proj->Monitor->Init_Stack_Size-16, MACRO_REPLACE);
    /* What is the Systick value? - (usually) 10ms per tick */
    Gen_Tool::Macro_String(List, "RME_RV32P_OSTIM_VAL", this->Proj->Chip->Config["Ostim_Value"]+"U", MACRO_REPLACE);

    /* Fixed attributes - we will refill these with database values */
    /* Number of MPU regions available */
    Gen_Tool::Macro_Int(List, "RME_RV32P_REGION_NUM", this->Chip->Region, MACRO_REPLACE);
}
/* End Function:RV32P_Gen::Kernel_Conf_Hdr ***********************************/

/* Function:RV32P_Gen::Monitor_Conf_Hdr ***************************************
Description : Replace monitor configuration header macros.
Input       : std::unique_ptr<std::vector<std::string>>& List - The input file.
Output      : std::unique_ptr<std::vector<std::string>>& List - The modified file.
Return      : None.
******************************************************************************/
void RV32P_Gen::Monitor_Conf_Hdr(std::unique_ptr<std::vector<std::string>>& List)
{
    /* Init process's first thread's entry point address */
    if(Proj->Buildsystem == "Makefile")
        Gen_Tool::Macro_Hex(List, "RVM_RV32P_INIT_ENTRY", PRC_DESC_ALIGN(this->Proj->Monitor->Code_Base), MACRO_REPLACE);
    else
        Gen_Tool::Macro_Hex(List, "RVM_RV32P_INIT_ENTRY",PRC_DESC_ALIGN(this->Proj->Monitor->Code_Base+8*4), MACRO_REPLACE);
    /* Init process's first thread's stack address */
    Gen_Tool::Macro_Hex(List, "RVM_RV32P_INIT_STACK",
                        this->Proj->Monitor->Init_Stack_Base+this->Proj->Monitor->Init_Stack_Size-16, MACRO_REPLACE);
    /* Fixed attributes - we will refill these with database values */
    /* Number of MPU regions available */
    Gen_Tool::Macro_Int(List, "RVM_RV32P_REGION_NUM", this->Chip->Region, MACRO_REPLACE);
}
/* End Function:RV32P_Gen::Monitor_Conf_Hdr **********************************/

/* Function:RV32P_Gen::Process_Main_Hdr ***************************************
Description : Replace process main header macros.
Input       : std::unique_ptr<std::vector<std::string>>& List - The input file.
              const class Process* Prc - The process information.
Output      : std::unique_ptr<std::vector<std::string>>& List - The modified file.
Return      : None.
******************************************************************************/
void RV32P_Gen::Process_Main_Hdr(std::unique_ptr<std::vector<std::string>>& List,
		                         const class Process* Prc)
{

}
/* End Function:RV32P_Gen::Process_Main_Hdr **********************************/
}
/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
