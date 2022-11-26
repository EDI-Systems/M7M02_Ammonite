/******************************************************************************
Filename    : mem_info.cpp
Author      : pry
Date        : 16/07/2019
Licence     : LGPL v3+; see COPYING for details.
Description : The memory block class. This contains the memory block information.
******************************************************************************/

/* Includes ******************************************************************/
#include "map"
#include "string"
#include "memory"
#include "vector"
#include "stdexcept"
#include "algorithm"

extern "C"
{
#include "xml.h"
}

#define __HDR_DEFS__
#include "rvm_gen.hpp"
#include "Mem_Info/mem_info.hpp"
#undef __HDR_DEFS__

#define __HDR_CLASSES__
#include "rvm_gen.hpp"
#include "Mem_Info/mem_info.hpp"
#undef __HDR_CLASSES__
/* End Includes **************************************************************/
namespace RVM_GEN
{
/* Begin Function:Mem_Info::Mem_Info ******************************************
Description : Constructor for memory information class.
Input       : xml_node_t* Root - The node containing the memory block information.
              ptr_t Reference - Whether this is a shared memory reference, that
                                does not have a base and a size.
Output      : None.
Return      : None.
******************************************************************************/
/* void */ Mem_Info::Mem_Info(xml_node_t* Root, ptr_t Reference)
{
    std::string Temp;
    std::unique_ptr<std::string> Str;

    try
    {
        /* Name - not necessarily exist */
        try
        {
            this->Name=Main::XML_Get_String(Root,"Name","DXXXX","DXXXX");
            Main::Idtfr_Check(this->Name,"Name","DXXXX","DXXXX");
        }
        catch(std::exception& Exc)
        {
            this->Name="";
        }

        this->Reference=Reference;
        if(Reference==MEM_DECL)
        {
            /* Base */
            Temp=Main::XML_Get_String(Root,"Base","DXXXX","DXXXX");
            if(Temp=="Auto")
                this->Base=MEM_AUTO;
            else
                this->Base=std::stoull(Temp,0,0);

            /* Size */
            this->Size=Main::XML_Get_Number(Root,"Size","DXXXX","DXXXX");

            /* Type */
            Temp=Main::XML_Get_String(Root,"Type","DXXXX","DXXXX");
            if(Temp=="Code")
                this->Type=MEM_CODE;
            else if(Temp=="Data")
                this->Type=MEM_DATA;
            else if(Temp=="Device")
                this->Type=MEM_DEVICE;
            else
                Main::Error("P0510: Type is malformed.");
        }

        /* Attribute */
        this->Attr=0;
        Temp=Main::XML_Get_String(Root,"Attribute","DXXXX","DXXXX");
        if(Temp.rfind('R')!=std::string::npos)
            this->Attr|=MEM_READ;
        if(Temp.rfind('W')!=std::string::npos)
            this->Attr|=MEM_WRITE;
        if(Temp.rfind('X')!=std::string::npos)
            this->Attr|=MEM_EXECUTE;
        if(this->Attr==0)
            Main::Error("P0510: Attribute does not allow any access and is malformed.");
        if(Temp.rfind('B')!=std::string::npos)
            this->Attr|=MEM_BUFFER;
        if(Temp.rfind('C')!=std::string::npos)
            this->Attr|=MEM_CACHE;
        if(Temp.rfind('S')!=std::string::npos)
            this->Attr|=MEM_STATIC;

        /* Alignment */
        this->Align=0;
    }
    catch(std::exception& Exc)
    {
        if(this->Name!="")
            Main::Error(std::string("Memory: ")+this->Name+"\n"+Exc.what());
        else
            Main::Error(std::string("Memory: ")+"Unknown"+"\n"+Exc.what());
    }
}
/* End Function:Mem_Info::Mem_Info *******************************************/

/* Begin Function:Mem_Info::Mem_Info ******************************************
Description : Constructor for Mem class.
Input       : class Mem* Block - The block to copy from.
              ptr_t Attr_New - The new attributes.
Output      : None.
Return      : None.
******************************************************************************/
/* void */ Mem_Info::Mem_Info(class Mem_Info* Block, ptr_t Attr_New)
{
    /* Attributes must be compatible */
    ASSERT(Attr_New!=0);
    ASSERT((Block->Attr&Attr_New)==Attr_New);

    /* Copy everything of that source block */
    this->Reference=Block->Reference;
    this->Name=Block->Name;
    this->Base=Block->Base;
    this->Size=Block->Size;
    this->Type=Block->Type;
    this->Attr=Attr_New;
    this->Align=Block->Align;
}
/* End Function:Mem_Info::Mem_Info *******************************************/

/* Begin Function:Mem_Info::Mem_Info ******************************************
Description : Constructor for memory class.
Input       : const std::string& Name - The optional memory trunk name.
              ptr_t Base - The start address.
              ptr_t Size - The memory trunk size.
              ptr_t Type - The memory type.
              ptr_t Attr - The attributes of this memory block.
Output      : None.
Return      : None.
******************************************************************************/
/* void */ Mem_Info::Mem_Info(const std::string& Name, ptr_t Base, ptr_t Size, ptr_t Type, ptr_t Attr)
{
    this->Reference=MEM_DECL;
    this->Name=Name;
    this->Base=Base;
    this->Size=Size;
    this->Type=Type;
    this->Attr=Attr;
    this->Align=0;
}
/* End Function:Mem_Info::Mem_Info *******************************************/

/* Begin Function:Mem_Info::Check *********************************************
Description : Check whether the memory block configuration makes sense.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
void Mem_Info::Check(void)
{
    try
    {
        if(this->Reference==MEM_DECL)
        {
            if(this->Size==0)
                Main::Error("P0504: Size cannot be zero.");
            if(this->Size>0x100000000ULL)
                Main::Error("P0505: Size is out of bound.");

            if((this->Type==MEM_DEVICE)&&(this->Base==MEM_AUTO))
                Main::Error("P0507: Device-type memory cannot be automatically allocated.");
        }
    }
    catch(std::exception& Exc)
    {
        if(this->Name!="")
            Main::Error(std::string("Memory: ")+this->Name+"\n"+Exc.what());
        else
            Main::Error(std::string("Memory: ")+"Unknown"+"\n"+Exc.what());
    }
}
/* End Function:Mem_Info::Check **********************************************/

/* Begin Function:Mem_Info::Overlap_Check *************************************
Description : Check whether the memory blocks declared will overlap.
Input       : const std::vector<class Mem_Info*>& Code - Code memory segments.
              const std::vector<class Mem_Info*>& Data - Data memory segments.
              const std::vector<class Mem_Info*>& Device - Device memory segments.
              const std::string& Type - The descriptive type of the memory being checked.
Output      : None.
Return      : None.
******************************************************************************/
void Mem_Info::Overlap_Check(const std::vector<class Mem_Info*>& Code,
                             const std::vector<class Mem_Info*>& Data,
                             const std::vector<class Mem_Info*>& Device,
                             const std::string& Type)
{
    std::vector<class Mem_Info*> All;
    ptr_t Count;

    /* Gather all memory that is not automatically allocated */
    for(class Mem_Info* Mem:Code)
    {
        if(Mem->Base==MEM_AUTO)
            continue;
        All.push_back(Mem);
    }
    for(class Mem_Info* Mem:Data)
    {
        if(Mem->Base==MEM_AUTO)
            continue;
        All.push_back(Mem);
    }
    for(class Mem_Info* Mem:Device)
    {
        if(Mem->Base==MEM_AUTO)
            continue;
        All.push_back(Mem);
    }

    if(All.size()>1)
    {
        /* Sort memory according to base address */
        std::sort(All.begin(),All.end(),
        [](const class Mem_Info* Left, const class Mem_Info* Right)->bool
        {
            return Left->Base<Right->Base;
        });

        /* Check if adjacent ones overlap */
        for(Count=0;Count<All.size()-1;Count++)
        {
            if((All[Count]->Base+All[Count]->Size)>All[Count+1]->Base)
            {
                Main::Error("PXXXX: %s '%s' base 0x%llX size 0x%llX overlapped with '%s' base 0x%llX size 0x%llX.",
                            Type.c_str(),All[Count]->Name.c_str(),All[Count]->Base,All[Count]->Size,
                            All[Count+1]->Name.c_str(),All[Count+1]->Base,All[Count+1]->Size);
            }
        }
    }
}
/* End Function:Mem_Info::Overlap_Check **************************************/

/* Begin Function:Mem_Info::Memmap_Init ***************************************
Description : Initialize memory map for the memory trunk.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
void Mem_Info::Memmap_Init(void)
{
    this->Map.resize((this->Size/MAP_ALIGN)+1);
    std::fill(this->Map.begin(), this->Map.end(), false);
}
/* End Function:Mem_Info::Memmap_Init ****************************************/

/* Begin Function:Mem_Info::Memmap_Mark ***************************************
Description : Try or perform allocation of this memory trunk.
Input       : ptr_t Base - The base address.
              ptr_t Size - The memory size.
              ptr_t Mark - Whether this is an attempt or an allocation
Output      : None.
Return      : ret_t - If successful, 0; else -1.
******************************************************************************/
ret_t Mem_Info::Memmap_Mark(ptr_t Base, ptr_t Size, ptr_t Mark)
{
    ptr_t Count;
    ptr_t Bit_Base;
    ptr_t Bit_Size;

    /* See if we can possibly fit there */
    if((Base>=this->Base)&&((this->Base+this->Size)>=(Base+Size)))
    {

        Bit_Base=(Base-this->Base)/MAP_ALIGN;
        Bit_Size=Size/MAP_ALIGN;

        /* Test whether we can mark it */
        if(Mark==MEM_TRY)
        {
            for(Count=Bit_Base;Count<Bit_Base+Bit_Size;Count++)
            {
                if(this->Map[Count]!=false)
                    return -1;
            }
        }
        else
        {
            for(Count=Bit_Base;Count<Bit_Base+Bit_Size;Count++)
            {
                ASSERT(this->Map[Count]==false);
                this->Map[Count]=true;
            }
        }

        return 0;
    }

    return -1;
}
/* End Function:Mem_Info::Memmap_Mark ****************************************/

/* Begin Function:Mem_Info::Static_Fit ****************************************
Description : Fit a memory trunk statically.
Input       : std::vector<class Mem_Info*>& Map - The collection of memory maps to fit in.
Output      : None.
Return      : ret_t - If successful, 0; else -1.
******************************************************************************/
ret_t Mem_Info::Static_Fit(std::vector<class Mem_Info*>& Map)
{
    /* See if we can even find a segment that accomodates this */
    for(class Mem_Info* Phys:Map)
    {
        /* Is the attribute a superset of what we require? */
        if((Phys->Attr&this->Attr)!=this->Attr)
            continue;

        /* See if we can fit */
        if(Phys->Memmap_Mark(this->Base,this->Size,MEM_TRY)==0)
        {
            Phys->Memmap_Mark(this->Base,this->Size,MEM_MARK);
            return 0;
        }
    }

    /* Can't find a fit */
    return -1;
}
/* End Function:Mem_Info::Static_Fit *****************************************/

/* Begin Function:Mem_Info::Auto_Fit ******************************************
Description : Fit a memory trunk automatically.
Input       : std::vector<class Mem_Info*>& Map - The collection of memory maps to fit in.
Output      : None.
Return      : ret_t - If successful, 0; else -1.
******************************************************************************/
ret_t Mem_Info::Auto_Fit(std::vector<class Mem_Info*>& Map)
{
    ptr_t Phys_Start;
    ptr_t Phys_End;
    ptr_t Phys_Try;

    /* Find somewhere to fit this memory trunk, and if found, we will populate it */
    for(class Mem_Info* Phys:Map)
    {
        /* Is the size possibly sufficient? */
        if(this->Size>Phys->Size)
            continue;
        /* Is the attribute a superset of what we require? */
        if((Phys->Attr&this->Attr)!=this->Attr)
            continue;

        /* Round start address up, round end address down, to alignment */
        Phys_Start=ROUND_UP(Phys->Base,this->Align);
        Phys_End=ROUND_DOWN(Phys->Base+Phys->Size,this->Align);

        /* Is the effective size still possibly sufficient? */
        if(this->Size>(Phys_End-Phys_Start))
            continue;

        /* Keep trying until we find a fit - optimize this loop later */
        for(Phys_Try=Phys_Start;Phys_Try<Phys_End;Phys_Try+=this->Align)
        {
            if(Phys->Memmap_Mark(Phys_Try,this->Size,MEM_TRY)==0)
            {
                Phys->Memmap_Mark(Phys_Try,this->Size,MEM_MARK);
                this->Base=Phys_Try;
                return 0;
            }
        }
    }

    /* Can't find any fit */
    return -1;
}
/* End Function:Mem_Info::Auto_Fit *******************************************/
}
/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
