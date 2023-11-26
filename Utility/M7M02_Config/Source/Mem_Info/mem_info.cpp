/******************************************************************************
Filename    : mem_info.cpp
Author      : pry
Date        : 13/01/2023
License     : Proprietary; confidential.
Description : Mem_Info information implementation.
******************************************************************************/

/* Include *******************************************************************/
#define __HDR_DEF__
#include "rvm_cfg.hpp"
#undef __HDR_DEF__

#include "wx/wx.h"
#include "wx/xml/xml.h"

#include "map"
#include "string"
#include "memory"

#define __HDR_DEF__
#include "Mem_Info/mem_info.hpp"
#undef __HDR_DEF__

#define __HDR_CLASS__
#include "rvm_cfg.hpp"
#include "Proj_Info/proj_info.hpp"
#include "Mem_Info/mem_info.hpp"
#undef __HDR_CLASS__
/* End Include ***************************************************************/
namespace RVM_CFG
{
/* Function:Mem_Info::Mem_Info ************************************************
Description : Default constructor for chip information.
Input       : const std::string& Name - The exact chip name.
              ptr_t Base - The base address.
              ptr_t Size - The size of the memory trunk.
              ptr_t Type - The memory type.
              ptr_t Attr - Access attributes.
Output      : None.
Return      : None.
******************************************************************************/
/* void */ Mem_Info::Mem_Info(const std::string& Name,
                              ptr_t Base, ptr_t Size, ptr_t Type, ptr_t Attr)
{
    this->Name=Name;
    this->Base=Base;
    this->Size=Size;
    this->Type=Type;
    this->Attr=Attr;
}
/* End Function:Mem_Info::Mem_Info *******************************************/

/* Function:Mem_Info::Mem_Info ************************************************
Description : Constructor for chip information.
Input       : class wxXmlNode* Node - The node containing information.
Output      : None.
Return      : None.
******************************************************************************/
/* void */ Mem_Info::Mem_Info(class wxXmlNode* Node)
{
    std::string Temp;

    /* Name - not necessarily exist */
    try
    {
        this->Name=Main::Text_Load(Node,"Name");
    }
    catch(std::exception& Exc)
    {
        this->Name="";
    }

    /* Base */
    Temp=Main::Text_Load(Node,"Base");
    if(Temp=="Auto")
        this->Base=MEM_AUTO;
    else
        this->Base=std::stoull(Temp,0,0);

    /* Size */
    this->Size=Main::Num_Load(Node,"Size");

    /* Type */
    Temp=Main::Text_Load(Node,"Type");
    if(Temp=="Code")
        this->Type=MEM_CODE;
    else if(Temp=="Data")
        this->Type=MEM_DATA;
    else if(Temp=="Device")
        this->Type=MEM_DEVICE;
    else
        throw std::runtime_error("Memory Type is malformed.");

    /* Attribute */
    this->Attr=0;
    Temp=Main::Text_Load(Node,"Attribute");
    if(Temp.rfind('R')!=std::string::npos)
        this->Attr|=MEM_READ;
    if(Temp.rfind('W')!=std::string::npos)
        this->Attr|=MEM_WRITE;
    if(Temp.rfind('X')!=std::string::npos)
        this->Attr|=MEM_EXECUTE;
    if(this->Attr==0)
        throw std::runtime_error("Attribute does not allow any access and is malformed.");
    if(Temp.rfind('B')!=std::string::npos)
        this->Attr|=MEM_BUFFER;
    if(Temp.rfind('C')!=std::string::npos)
        this->Attr|=MEM_CACHE;
    if(Temp.rfind('S')!=std::string::npos)
        this->Attr|=MEM_STATIC;
}
/* End Function:Mem_Info::Mem_Info *******************************************/

/* Function:Mem_Info::~Mem_Info ***********************************************
Description : Destructor for chip information.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
/* void */ Mem_Info::~Mem_Info(void)
{

}
/* End Function:Mem_Info::~Mem_Info ******************************************/

/* Function:Mem_Info::Save ****************************************************
Description : Save target information to XML file.
Input       : class wxXmlNode* Parent - The parent node.
Output      : None.
Return      : None.
******************************************************************************/
void Mem_Info::Save(class wxXmlNode* Parent)
{
    std::string Temp;

    /* Name */
    if(this->Name!="")
        Main::Text_Save(Parent,"Name",this->Name);

    /* Base */
    if(this->Base==MEM_AUTO)
        Main::Text_Save(Parent,"Base","Auto");
    else
        Main::Hex_Save(Parent,"Base",4,this->Base);

    /* Size */
    Main::Hex_Save(Parent,"Size",4,this->Size);

    /* Type */
    switch(this->Type)
    {
        case MEM_CODE:Main::Text_Save(Parent,"Type","Code");break;
        case MEM_DATA:Main::Text_Save(Parent,"Type","Data");break;
        case MEM_DEVICE:Main::Text_Save(Parent,"Type","Device");break;
        default:ASSERT(0,"Memory type is invalid.");
    }

    /* Attribute */
    if((this->Attr&MEM_READ)!=0)
        Temp+="R";
    if((this->Attr&MEM_WRITE)!=0)
        Temp+="R";
    if((this->Attr&MEM_EXECUTE)!=0)
        Temp+="R";
    if((this->Attr&MEM_BUFFER)!=0)
        Temp+="B";
    if((this->Attr&MEM_CACHE)!=0)
        Temp+="C";
    if((this->Attr&MEM_STATIC)!=0)
        Temp+="S";

    Main::Text_Save(Parent,"Attribute",Temp);
}
/* End Function:Mem_Info::Save ***********************************************/
}
/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
