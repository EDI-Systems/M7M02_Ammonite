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
                this->Base=std::stoul(Temp);

            /* Size */
            this->Size=Main::XML_Get_Number(Root,"Size","DXXXX","DXXXX");
        }

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
            this->Attr|=MEM_BUFFERABLE;
        if(Temp.rfind('C')!=std::string::npos)
            this->Attr|=MEM_CACHEABLE;
        if(Temp.rfind('S')!=std::string::npos)
            this->Attr|=MEM_STATIC;
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
    this->Reference=Block->Reference;
    this->Name=Block->Name;
    this->Base=Block->Base;
    this->Size=Block->Size;
    this->Type=Block->Type;
    this->Attr=Block->Attr;
    this->Align=Block->Align;
}
/* End Function:Mem_Info::Mem_Info *******************************************/

/* Begin Function:Mem_Info::Mem_Info ******************************************
Description : Constructor for memory class.
Input       : ptr_t Base - The start address.
              ptr_t Size - The memory trunk size.
              ptr_t Type - The memory type.
              ptr_t Attr - The attributes of this memory block.
              ptr_t Align - The alignment size.
Output      : None.
Return      : None.
******************************************************************************/
/* void */ Mem_Info::Mem_Info(ptr_t Base, ptr_t Size, ptr_t Type, ptr_t Attr, ptr_t Align)
{
    this->Reference=MEM_DECL;
    this->Base=Base;
    this->Size=Size;
    this->Type=Type;
    this->Attr=Attr;
    this->Align=Align;
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
Input       : std::vector<class Mem_Info*>& Code - Code memory segments.
              std::vector<class Mem_Info*>& Data - Data memory segments.
              std::vector<class Mem_Info*>& Device - Device memory segments.
Output      : None.
Return      : None.
******************************************************************************/
void Mem_Info::Overlap_Check(std::vector<class Mem_Info*>& Code,
                             std::vector<class Mem_Info*>& Data,
                             std::vector<class Mem_Info*>& Device)
{


}
/* End Function:Mem_Info::Overlap_Check **************************************/
}
/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
