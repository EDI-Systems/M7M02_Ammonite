/******************************************************************************
Filename    : receive.cpp
Author      : pry
Date        : 13/01/2023
Licence     : The Unlicense; see LICENSE for details.
Description : Receive information implementation.
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
#include "Proj_Info/Process/Receive/receive.hpp"
#undef __HDR_DEF__

#define __HDR_CLASS__
#include "rvm_cfg.hpp"
#include "Proj_Info/Process/Receive/receive.hpp"
#undef __HDR_CLASS__
/* End Include ***************************************************************/
namespace RVM_CFG
{
/* Function:Receive::Receive **************************************************
Description : Constructor for receive information.
Input       : class wxXmlNode* Node - The node containing information.
              ptr_t Type - Whether this is a native process or a VM.
Output      : None.
Return      : None.
******************************************************************************/
/* void */ Receive::Receive(class wxXmlNode* Node)
{
    /* Name */
    this->Name=Main::Text_Load(Node,"Name");
}
/* End Function:Receive::Receive *********************************************/

/* Function:Receive::Receive **************************************************
Description : Constructor for receive information.
Input       : const std::string& Name - The name.
Output      : None.
Return      : None.
******************************************************************************/
/* void */ Receive::Receive(const std::string& Name)
{
    /* Name */
    this->Name=Name;
}
/* End Function:Receive::Receive *********************************************/

/* Function:Receive::~Receive *************************************************
Description : Destructor for receive information.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
/* void */ Receive::~Receive(void)
{

}
/* End Function:Receive::~Receive ********************************************/

/* Function:Receive::Save *****************************************************
Description : Save thread information to XML file.
Input       : class wxXmlNode* Parent - The parent node.
Output      : None.
Return      : None.
******************************************************************************/
void Receive::Save(class wxXmlNode* Parent)
{
    /* Name */
    Main::Text_Save(Parent,"Name",this->Name);
}
/* End Function:Receive::Save ************************************************/
}
/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
