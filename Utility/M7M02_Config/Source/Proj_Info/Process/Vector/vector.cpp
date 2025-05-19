/******************************************************************************
Filename    : vector.cpp
Author      : pry
Date        : 2022/11/19
Licence     : LGPL v3+; see COPYING for details.
Description : Vector information in native processes.
******************************************************************************/

/* Include *******************************************************************/
#define __HDR_DEF__
#include "rvm_cfg.hpp"
#undef __HDR_DEF__

#include "wx/wx.h"
#include "wx/xml/xml.h"
#include "wx/filename.h"

#include "set"
#include "map"
#include "string"
#include "memory"
#include "vector"
#include "stdexcept"

#define __HDR_DEF__
#include "Proj_Info/Process/Vector/vector.hpp"
#undef __HDR_DEF__

#define __HDR_CLASS__
#include "rvm_cfg.hpp"
#include "Proj_Info/Process/Vector/vector.hpp"
#undef __HDR_CLASS__
/* End Include ***************************************************************/
namespace RVM_CFG
{
/* Function:Vector::Vector ****************************************************
Description : Parse the vector section of a particular chip.
Input       : class wxXmlNode* Node - The vector section's XML node.
Output      : None.
Return      : None.
******************************************************************************/
/* void */ Vector::Vector(class wxXmlNode* Node)
{
    /* Name */
    this->Name=Main::Text_Load(Node,"Name");
    /* Number */
    this->Number=Main::Num_Load(Node,"Number");
}
/* End Function:Vector::Vector ***********************************************/

/* Function:Vector::Vector ****************************************************
Description : Parse the vector section of a particular chip.
Input       : const std::string& Name - The name.
              const ptr_t& Number - The number.
Output      : None.
Return      : None.
******************************************************************************/
/* void */ Vector::Vector(const std::string& Name,const ptr_t& Number)
{
    /* Name */
    this->Name=Name;
    /* Number */
    this->Number=Number;
}
/* End Function:Vector::Vector ***********************************************/

/* Function:Vector::~Vector ***************************************************
Description : Destructor for vector information.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
/* void */ Vector::~Vector(void)
{

}
/* End Function:Vector::~Vector **********************************************/

/* Function:Vector::Save ******************************************************
Description : Save vector information to XML file.
Input       : class wxXmlNode* Parent - The parent node.
Output      : None.
Return      : None.
******************************************************************************/
void Vector::Save(class wxXmlNode* Parent)
{
    /* Name */
    Main::Text_Save(Parent,"Name",this->Name);
    /* Number */
    Main::Num_Save(Parent,"Number",this->Number);
}
/* End Function:Vector::Save *************************************************/
}
/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
