/******************************************************************************
Filename    : sned.cpp
Author      : lbc
Date        : 21/04/2025
Licence     : The Unlicense; see LICENSE for details.
Description : Send implementation.
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
#include "Proj_Info/Process/Send/send.hpp"
#undef __HDR_DEF__

#define __HDR_CLASS__
#include "rvm_cfg.hpp"
#include "Proj_Info/Process/Send/send.hpp"
#undef __HDR_CLASS__
/* End Include ***************************************************************/
namespace RVM_CFG
{
/* Function:Send::Send ********************************************************
Description : Constructor for send.
Input       : class wxXmlNode* Node - The node containing information.
              ptr_t Type - Whether this is a native process or a VM.
Output      : None.
Return      : None.
******************************************************************************/
/* void */ Send::Send(class wxXmlNode* Node)
{
	/* Name */
	this->Name=Main::Text_Load(Node,"Name");
	/* Process */
	this->Process=Main::Text_Load(Node,"Process");
}
/* End Function:Send::Send ***************************************************/

/* Function:Send::Send ********************************************************
Description : Constructor for send.
Input       : class wxXmlNode* Node - The node containing information.
              ptr_t Type - Whether this is a native process or a VM.
Output      : None.
Return      : None.
******************************************************************************/
/* void */ Send::Send(const std::string& Name, const std::string& Process)
{
   this->Name=Name;
   this->Process=Process;
}
/* End Function:Send::Send ***************************************************/

/* Function:Send::~Send *******************************************************
Description : Destructor for Send information.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
/* void */ Send::~Send(void)
{

}
/* End Function:Send::~Send **************************************************/

/* Function:Send::Save ********************************************************
Description : Save send information to XML file.
Input       : class wxXmlNode* Parent - The parent node.
Output      : None.
Return      : None.
******************************************************************************/
void Send::Save(class wxXmlNode* Parent)
{
    /* Name */
    Main::Text_Save(Parent,"Name",this->Name);
    /* Process */
    Main::Text_Save(Parent,"Process",this->Process);
}
/* End Function:Send::Save ***************************************************/
}
/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
