/******************************************************************************
Filename    : invocation.cpp
Author      : pry
Date        : 13/01/2023
Licence     : The Unlicense; see LICENSE for details.
Description : Invocation information implementation.
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
#include "Proj_Info/Process/Invocation/invocation.hpp"
#undef __HDR_DEF__

#define __HDR_CLASS__
#include "rvm_cfg.hpp"
#include "Proj_Info/Process/Invocation/invocation.hpp"
#undef __HDR_CLASS__
/* End Include ***************************************************************/
namespace RVM_CFG
{
/* Function:Invocation::Invocation ********************************************
Description : Constructor for invocation information.
Input       : class wxXmlNode* Node - The node containing information.
              ptr_t Type - Whether this is a native process or a VM.
Output      : None.
Return      : None.
******************************************************************************/
/* void */ Invocation::Invocation(class wxXmlNode* Node)
{
    /* Name */
    this->Name=Main::Text_Load(Node,"Name");
    /* Stack_Size */
    this->Stack_Size=Main::Num_Load(Node,"Stack_Size");
}
/* End Function:Invocation::Invocation ***************************************/

/* Function:Invocation::Invocation ********************************************
Description : Constructor for invocation information.
Input       : const std::string Name
              const ptr_t& Stack_Size
Output      : None.
Return      : None.
******************************************************************************/
/* void */ Invocation::Invocation(const std::string Name,const ptr_t& Stack_Size)
{
    /* Name */
    this->Name=Name;
    /* Stack_Size */
    this->Stack_Size=Stack_Size;
}
/* End Function:Invocation::Invocation ***************************************/

/* Function:Invocation::~Invocation *******************************************
Description : Destructor for invocation information.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
/* void */ Invocation::~Invocation(void)
{

}
/* End Function:Invocation::~Invocation **************************************/

/* Function:Invocation::Save **************************************************
Description : Save invocation information to XML file.
Input       : class wxXmlNode* Parent - The parent node.
Output      : None.
Return      : None.
******************************************************************************/
void Invocation::Save(class wxXmlNode* Parent)
{
    /* Name */
    Main::Text_Save(Parent,"Name",this->Name);
    /* Stack_Size */
    Main::Hex_Save(Parent,"Stack_Size",4,this->Stack_Size);
}
/* End Function:Invocation::Save *********************************************/
}
/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
