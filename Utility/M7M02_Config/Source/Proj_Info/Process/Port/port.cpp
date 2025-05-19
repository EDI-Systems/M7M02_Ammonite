/******************************************************************************
Filename    : port.cpp
Author      : pry
Date        : 13/01/2023
Licence     : The Unlicense; see LICENSE for details.
Description : Thread information implementation.
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
#include "Proj_Info/Process/Port/port.hpp"
#undef __HDR_DEF__

#define __HDR_CLASS__
#include "rvm_cfg.hpp"
#include "Proj_Info/Process/Port/port.hpp"
#undef __HDR_CLASS__
/* End Include ***************************************************************/
namespace RVM_CFG
{
/* Function:Port::Port ********************************************************
Description : Constructor for thread information.
Input       : class wxXmlNode* Node - The node containing information.
              ptr_t Type - Whether this is a native process or a VM.
Output      : None.
Return      : None.
******************************************************************************/
/* void */ Port::Port(class wxXmlNode* Node)
{
    /* Name */
    this->Name=Main::Text_Load(Node,"Name");
    /* Process */
    this->Process=Main::Text_Load(Node,"Process");
}
/* End Function:Port::Port ***************************************************/

/* Function:Port::Port **************************************************
Description : Constructor for send information.
Input       : class wxXmlNode* Node - The node containing information.
              ptr_t Type - Whether this is a native process or a VM.
Output      : None.
Return      : None.
******************************************************************************/
/* void */ Port::Port(const std::string& Name, const std::string& Process)
{
   this->Name=Name;
   this->Process=Process;
}
/* End Function:Port::Port *********************************************/

/* Function:Port::~Port *******************************************************
Description : Destructor for thread information.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
/* void */ Port::~Port(void)
{

}
/* End Function:Port::~Port **************************************************/

/* Function:Port::Save ********************************************************
Description : Save thread information to XML file.
Input       : class wxXmlNode* Parent - The parent node.
Output      : None.
Return      : None.
******************************************************************************/
void Port::Save(class wxXmlNode* Parent)
{
    /* Name */
    Main::Text_Save(Parent,"Name",this->Name);
    /* Process */
    Main::Text_Save(Parent,"Process",this->Process);

}
/* End Function:Port::Save ***************************************************/
}
/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
