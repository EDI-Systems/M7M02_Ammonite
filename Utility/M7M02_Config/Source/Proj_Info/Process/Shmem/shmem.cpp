/******************************************************************************
Filename    : shmem.cpp
Author      : lbc
Date        : 21/04/2025
Licence     : The Unlicense; see LICENSE for details.
Description : Shmem information implementation.
******************************************************************************/

/* Include *******************************************************************/
#define __HDR_DEF__
#include "rvm_cfg.hpp"
#undef __HDR_DEF__

#include "wx/wx.h"
#include "wx/xml/xml.h"
#include "wx/filename.h"

#include "map"
#include "string"
#include "memory"

#define __HDR_DEF__
#include "Proj_Info/Process/Shmem/shmem.hpp"
#undef __HDR_DEF__

#define __HDR_CLASS__
#include "rvm_cfg.hpp"
#include "Proj_Info/Process/Shmem/shmem.hpp"
#undef __HDR_CLASS__
/* End Include ***************************************************************/
namespace RVM_CFG
{
/* Function:Shmem::Shmem ******************************************************
Description : Constructor for shmem information.
Input       : class wxXmlNode* Node - The node containing information.
              ptr_t Type - Whether this is a native process or a VM.
Output      : None.
Return      : None.
******************************************************************************/
/* void */ Shmem::Shmem(class wxXmlNode* Node)
{
    std::string Attr;

	/* Name */
	this->Name=Main::Text_Load(Node,"Name");

    /* Attribute */
    this->Attr=0;
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
/* End Function:Shmem::Shmem *************************************************/
//
///* Function:Shmem::Shmem ******************************************************
//Description : Constructor for shmem information.
//Input       : const std::string& Name
//              ptr_t Attr
//Output      : None.
//Return      : None.
//******************************************************************************/
///* void */ Shmem::Shmem(const std::string& Name, ptr_t Attr,ptr_t Type,ptr_t Base, ptr_t Size, ptr_t Align)
//{
//    /* Name */
//    this->Name=Name;
//    /* Attributes */
//    this->Attr=Attr;
//    this->Base=Base;
//    this->Size=Size;
//    this->Type=Type;
//    this->Align=Align;
//}
///* End Function:Shmem::Shmem ***********************************************/

/* Function:Shmem::Shmem ******************************************************
Description : Constructor for shmem information.
Input       : const std::string& Name
              ptr_t Attr
Output      : None.
Return      : None.
******************************************************************************/
/* void */ Shmem::Shmem(const std::string& Name, ptr_t Attr)
{
    /* Name */
    this->Name=Name;
    /* Attributes */
    this->Attr=Attr;
}
/* End Function:Shmem::Shmem *************************************************/

/* Function:Shmem::~Shmem *****************************************************
Description : Destructor for shmem information.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
/* void */ Shmem::~Shmem(void)
{

}
/* End Function:Shmem::~Shmem ************************************************/

/* Function:Shmem::Save *******************************************************
Description : Save shmem information to XML file.
Input       : class wxXmlNode* Parent - The parent node.
Output      : None.
Return      : None.
******************************************************************************/
void Shmem::Save(class wxXmlNode* Parent)
{
    std::string Attr;

    /* Name */
    Main::Text_Save(Parent,"Name",this->Name);

    /* Attribute */
    if((this->Attr&MEM_READ)!=0)
        Attr+="R";
    if((this->Attr&MEM_WRITE)!=0)
        Attr+="W";
    if((this->Attr&MEM_EXECUTE)!=0)
        Attr+="E";
    if((this->Attr&MEM_BUFFER)!=0)
        Attr+="B";
    if((this->Attr&MEM_CACHE)!=0)
        Attr+="C";
    if((this->Attr&MEM_STATIC)!=0)
        Attr+="S";

    Main::Text_Save(Parent,"Attribute",Attr);
}
/* End Function:Shmem::Save **************************************************/
}
/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
