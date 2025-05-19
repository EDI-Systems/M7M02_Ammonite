/******************************************************************************
Filename    : kfunc.cpp
Author      : pry
Date        : 13/01/2023
Licence     : The Unlicense; see LICENSE for details.
Description : Kfunc implementation.
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
Description : Constructor for Kfunc.
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

/* Function:Kfunc::Kfunc ******************************************************
Description : Constructor for kernel function information.
Input       : const std::string& Name - The name.
              const ptr_t& Begin - The begin.
              const ptr_t& End  - The end.
Output      : None.
Return      : None.
******************************************************************************/
/* void */ Kfunc::Kfunc(const std::string& Name,const ptr_t& Begin,const ptr_t& End)
{
    /* Name */
    this->Name=Name;
    /* Begin */
    this->Begin=Begin;
    /* End */
    this->End=End;
}
/* End Function:Kfunc::Kfunc *************************************************/

/* Function:Kfunc::~Kfunc *****************************************************
Description : Destructor for kfunc information.
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
    /* Name */
    Main::Text_Save(Parent,"Name",this->Name);
    /* Begin */
    Main::Hex_Save(Parent,"Begin",4,this->Begin);
    /* End */
    Main::Hex_Save(Parent,"End",4,this->End);
}
/* End Function:Kfunc::Save **************************************************/
}
/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
