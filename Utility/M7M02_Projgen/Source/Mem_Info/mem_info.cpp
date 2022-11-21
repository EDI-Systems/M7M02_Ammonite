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
#include "bitset"
#include "stdexcept"

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
              ptr_t Ref - Whether this is a shared memory reference, that does not
                          have a base and a size.
Output      : None.
Return      : None.
******************************************************************************/
/* void */ Mem_Info::Mem_Info(xml_node_t* Root, ptr_t Ref)
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

        if(Ref==0)
        {
            /* Base */
            Temp=Main::XML_Get_String(Root,"Base","DXXXX","DXXXX");
            if(Temp=="Auto")
                this->Base=MEM_AUTO;
            else
                this->Base=std::stoul(Temp);

            /* Size */
            this->Size=Main::XML_Get_Number(Root,"Size","DXXXX","DXXXX");
        }

        /* Type */
        this->Type=Main::XML_Get_String(Root,"Type","DXXXX","DXXXX");

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
            throw std::invalid_argument("P0510: Attribute does not allow any access and is malformed.");
        if(Temp.rfind('B')!=std::string::npos)
            this->Attr|=MEM_BUFFERABLE;
        if(Temp.rfind('C')!=std::string::npos)
            this->Attr|=MEM_CACHEABLE;
        if(Temp.rfind('S')!=std::string::npos)
            this->Attr|=MEM_STATIC;

        /* By default, we are not shared memory (this attribute used only in processes) */
        this->Is_Shared=0;
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
Output      : None.
Return      : None.
******************************************************************************/
/* void */ Mem_Info::Mem_Info(class Mem_Info* Block)
{
    /* Copy everything of that source block */
    this->Name=Block->Name;
    this->Base=Block->Base;
    this->Size=Block->Size;
    this->Type=Block->Type;
    this->Attr=Block->Attr;
    this->Align=Block->Align;
    this->Is_Shared=Block->Is_Shared;
}
/* End Function:Mem_Info::Mem_Info *******************************************/

/* Begin Function:Mem_Info::Mem_Info ******************************************
Description : Constructor for memory class.
Input       : ptr_t Base - The start address.
              ptr_t Size - The memory trunk size.
              std::string& Type - The memory type.
              ptr_t Attr - The attributes of this memory block.
              ptr_t Align - The alignment size.
Output      : None.
Return      : None.
******************************************************************************/
/* void */ Mem_Info::Mem_Info(ptr_t Base, ptr_t Size, std::string& Type, ptr_t Attr, ptr_t Align)
{
    this->Base=Base;
    this->Size=Size;
    this->Type=Type;
    this->Attr=Attr;
    this->Align=Align;
    /* By default, we are not shared memory (this attribute used only in processes) */
    this->Is_Shared=0;
}
/* End Function:Mem_Info::Mem_Info *******************************************/
}
/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
