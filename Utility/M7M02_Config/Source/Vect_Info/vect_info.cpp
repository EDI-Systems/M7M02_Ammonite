/******************************************************************************
Filename    : vect_info.cpp
Author      : pry
Date        : 15/01/2023
License     : Proprietary; confidential.
Description : Vector information implementation.
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
#include "Vect_Info/vect_info.hpp"
#undef __HDR_DEF__

#define __HDR_CLASS__
#include "rvm_cfg.hpp"
#include "Vect_Info/vect_info.hpp"
#undef __HDR_CLASS__
/* End Include ***************************************************************/
namespace RVM_CFG
{
/* Function:Vect_Info::Vect_Info **********************************************
Description : Default constructor for vector information.
Input       : const std::string& Name - The vector name.
              ptr_t Number - The vector number.
Output      : None.
Return      : None.
******************************************************************************/
/* void */ Vect_Info::Vect_Info(const std::string& Name, ptr_t Number)
{
    this->Name=Name;
    this->Number=Number;
    /* Populate later */
}
/* End Function:Vect_Info::Vect_Info *****************************************/

/* Function:Vect_Info::Vect_Info **********************************************
Description : Constructor for chip information.
Input       : class wxXmlNode* Node - The node containing information.
Output      : None.
Return      : None.
******************************************************************************/
/* void */ Vect_Info::Vect_Info(class wxXmlNode* Node)
{
    /* Name */
    this->Name=Main::Text_Load(Node,"Name");
    /* Number */
    this->Number=Main::Num_Load(Node,"Number");
}
/* End Function:Vect_Info::Vect_Info *****************************************/

/* Function:Vect_Info::~Vect_Info *********************************************
Description : Destructor for chip information.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
/* void */ Vect_Info::~Vect_Info(void)
{

}
/* End Function:Vect_Info::~Vect_Info ****************************************/

/* Function:Vect_Info::Save ***************************************************
Description : Save target information to XML file.
Input       : class wxXmlNode* Parent - The parent node.
Output      : None.
Return      : None.
******************************************************************************/
void Vect_Info::Save(class wxXmlNode* Parent)
{
    Main::Text_Save(Parent,"Name",this->Name);
    Main::Num_Save(Parent,"Number",this->Number);
}
/* End Function:Vect_Info::Save **********************************************/
}
/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
