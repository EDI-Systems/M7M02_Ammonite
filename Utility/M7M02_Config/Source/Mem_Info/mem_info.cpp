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
Description : Default constructor for memory information.
Input       : const std::string& Name - The name of the memory.
              ptr_t Base - The base address of the memory.
              ptr_t Size - The size of the memory.
              ptr_t Type - The type of the memory.
              ptr_t Attr - Access attributes.
              ptr_t Align - The align of memory,
Output      : None.
Return      : None.
******************************************************************************/
/* void */ Mem_Info::Mem_Info(const std::string& Name,ptr_t Base, ptr_t Size, ptr_t Type, ptr_t Attr,ptr_t Align)
{
    this->Name=Name;
    this->Base=Base;
    this->Size=Size;
    this->Type=Type;
    this->Attr=Attr;
    this->Align=Align;
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
    std::string Type;
    std::string Base;
    std::string Attr;
    std::string Align;
    std::string Message;

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
    Base=Main::Text_Load(Node,"Base");
    if(Base=="Auto")
        this->Base=MEM_AUTO;
    else
        this->Base=std::stoull(Base,0,0);

    /* Size */
    this->Size=Main::Num_Load(Node,"Size");

    /* Type */
    Type=Main::Text_Load(Node,"Type");
    if(Type=="Code")
        this->Type=MEM_CODE;
    else if(Type=="Data")
        this->Type=MEM_DATA;
    else if(Type=="Device")
        this->Type=MEM_DEVICE;
    else
        throw std::runtime_error("Memory Type is malformed.");

    /* Attribute */
    this->Attr=0;
    try
    {
        Attr=Main::Text_Load(Node,"Attribute");
        if(Attr.rfind('R')!=std::string::npos)
            this->Attr|=MEM_READ;
        if(Attr.rfind('W')!=std::string::npos)
            this->Attr|=MEM_WRITE;
        if(Attr.rfind('X')!=std::string::npos)
            this->Attr|=MEM_EXECUTE;
        if(this->Attr==0)
            throw std::runtime_error("Attribute does not allow any access and is malformed.");
        if(Attr.rfind('B')!=std::string::npos)
            this->Attr|=MEM_BUFFER;
        if(Attr.rfind('C')!=std::string::npos)
            this->Attr|=MEM_CACHE;
        if(Attr.rfind('S')!=std::string::npos)
            this->Attr|=MEM_STATIC;
    }
    catch(std::exception& Exc)
    {
        Message=std::string(Exc.what());
        if(this->Name!="")
            Message=Message+"The malformed memory is "+this->Name;
        Main::Msgbox_Show(RVM_CFG_App::Main, MSGBOX_WARN,
                          _("Project information - Memry information load"),
                          _(Message));
    }

    /* Align - not necessarily exist(memory information in .rvc doesn't have it) */
    try
    {
        Align=Main::Text_Load(Node,"Align");
        if(Align=="Auto")
            this->Align=MEM_AUTO;
        else if(Align=="")
            this->Align=0;
        else
            this->Align=std::stoull(Align,0,0);
    }
    catch(std::exception& Exc)
    {
        this->Align=ALIGN_INVALID;
    }

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
    std::string Attr;

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
        Attr+="R";
    if((this->Attr&MEM_WRITE)!=0)
        Attr+="W";
    if((this->Attr&MEM_EXECUTE)!=0)
        Attr+="X";
    if((this->Attr&MEM_BUFFER)!=0)
        Attr+="B";
    if((this->Attr&MEM_CACHE)!=0)
        Attr+="C";
    if((this->Attr&MEM_STATIC)!=0)
        Attr+="S";

    Main::Text_Save(Parent,"Attribute",Attr);

    /*Align*/
    if(this->Align!=ALIGN_INVALID)
    {
        if(this->Align==MEM_AUTO)
            Main::Text_Save(Parent,"Align","Auto");
        else
            Main::Num_Save(Parent,"Align",this->Align);
    }

}
/* End Function:Mem_Info::Save ***********************************************/
}
/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
