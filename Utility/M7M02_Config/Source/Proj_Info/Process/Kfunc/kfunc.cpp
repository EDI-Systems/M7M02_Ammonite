/******************************************************************************
Filename    : kfunc.cpp
Author      : pry
Date        : 13/01/2023
Licence     : The Unlicense; see LICENSE for details.
Description : Kernel function information implementation.
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
#include "Proj_Info/Process/Kfunc/kfunc.hpp"
#undef __HDR_DEF__

#define __HDR_CLASS__
#include "rvm_cfg.hpp"
#include "Proj_Info/Process/Kfunc/kfunc.hpp"
#undef __HDR_CLASS__
/* End Include ***************************************************************/
namespace RVM_CFG
{
/* Function:Kfunc::Kfunc ******************************************************
Description : Constructor for kernel function information.
Input       : class wxXmlNode* Node - The node containing information.
              ptr_t Type - Whether this is a native process or a VM.
Output      : None.
Return      : None.
******************************************************************************/
/* void */ Kfunc::Kfunc(class wxXmlNode* Node)
{
	/* Name */
	this->Name=Main::Text_Load(Node,"Name");
    /* Begin */
    this->Begin=Main::Num_Load(Node,"Begin");
    /* End */
    this->End=Main::Num_Load(Node,"End");
}
/* End Function:Kfunc::Kfunc *************************************************/

/* Function:Kfunc::~Kfunc *****************************************************
Description : Destructor for process information.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
/* void */ Kfunc::~Kfunc(void)
{

}
/* End Function:Kfunc::~Kfunc ************************************************/

/* Function:Kfunc::Save *******************************************************
Description : Save process information to XML file.
Input       : class wxXmlNode* Parent - The parent node.
Output      : None.
Return      : None.
******************************************************************************/
void Kfunc::Save(class wxXmlNode* Parent)
{

}
/* End Function:Kfunc::Save **************************************************/
}
/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
