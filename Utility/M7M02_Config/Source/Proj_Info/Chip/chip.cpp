/******************************************************************************
Filename    : chip.cpp
Author      : pry
Date        : 13/01/2023
License     : Proprietary; confidential.
Description : Chip information implementation.
******************************************************************************/

/* Includes ******************************************************************/
#define __HDR_DEFS__
#include "rvm_cfg.hpp"
#undef __HDR_DEFS__

#include "wx/wx.h"
#include "wx/xml/xml.h"

#include "map"
#include "string"
#include "memory"

#define __HDR_DEFS__
#include "Proj_Info/proj_info.hpp"
#include "Proj_Info/Chip/chip.hpp"
#undef __HDR_DEFS__

#define __HDR_CLASSES__
#include "rvm_cfg.hpp"
#include "Proj_Info/Chip/chip.hpp"
#include "Proj_Info/proj_info.hpp"
#undef __HDR_CLASSES__
/* End Includes **************************************************************/
namespace RVM_CFG
{
/* Begin Function:Chip::Chip **************************************************
Description : Default constructor for chip information.
Input       : const std::string& Name - The exact chip name.
              const class Chip_Info* Chip - The chip information.
Output      : None.
Return      : None.
******************************************************************************/
/* void */ Chip::Chip(const std::string& Name,
                      const class Chip_Info* Chip)
{
    this->Name=Name;
    /* Populate later */
}
/* End Function:Chip::Chip ***************************************************/

/* Begin Function:Chip::Chip **************************************************
Description : Constructor for chip information.
Input       : class wxXmlNode* Node - The node containing information.
Output      : None.
Return      : None.
******************************************************************************/
/* void */ Chip::Chip(class wxXmlNode* Node)
{
    /* Platform */
    this->Platform=Main::Text_Load(Node,"Platform");
    /* Class */
    this->Class=Main::Text_Load(Node,"Class");
    /* Name */
    this->Name=Main::Text_Load(Node,"Name");
    /* Config */
    Main::Pair_Load(Node,"Config",this->Config);
}
/* End Function:Chip::Chip ***************************************************/

/* Begin Function:Chip::~Chip *************************************************
Description : Destructor for chip information.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
/* void */ Chip::~Chip(void)
{

}
/* End Function:Chip::~Chip **************************************************/

/* Begin Function:Chip::Save **************************************************
Description : Save target information to XML file.
Input       : class wxXmlNode* Parent - The parent node.
Output      : None.
Return      : None.
******************************************************************************/
void Chip::Save(class wxXmlNode* Parent)
{
    Main::Text_Save(Parent,"Platform",this->Platform);
    Main::Text_Save(Parent,"Class",this->Class);
    Main::Text_Save(Parent,"Name",this->Name);
    Main::Pair_Save(Parent,"Config",this->Config);
}
/* End Function:Chip::Save ***************************************************/
}
/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
