/******************************************************************************
Filename    : chip.cpp
Author      : pry
Date        : 13/01/2023
License     : Proprietary; confidential.
Description : Chip information implementation.
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
#include "Proj_Info/proj_info.hpp"
#include "Proj_Info/Chip/chip.hpp"
#undef __HDR_DEF__

#define __HDR_CLASS__
#include "rvm_cfg.hpp"
#include "Proj_Info/Chip/chip.hpp"
#include "Proj_Info/proj_info.hpp"
#include "Chip_Info/chip_info.hpp"
#include "Plat_Info/plat_Info.hpp"
#include "Plat_Info/Config/Config.hpp"
#undef __HDR_CLASS__
/* End Include ***************************************************************/
namespace RVM_CFG
{
/* Function:Chip::Chip ********************************************************
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
    /* Coprocessor */
    Main::CSV_Load(Node, "Coprocessor", this->Coprocessor);
    /* Config */
    Main::Pair_Load(Node,"Config",this->Config);
}
/* End Function:Chip::Chip ***************************************************/


/* Function:Chip::Chip ********************************************************
Description : Constructor for chip information.
Input       : const class Plat_Info* Plat_Info - The platform information.
              const class Chip_Info* Chip_Info - The chip information.
              const std::string& Chipname - The exact name of the chip.
Output      : None.
Return      : None.
******************************************************************************/
/* void */ Chip::Chip(const class Plat_Info* Plat_Info,
                      const class Chip_Info* Chip_Info, const std::string& Chipname)
{
    /* Platform */
    this->Platform=Plat_Info->Name;
    /* Class */
    this->Class=Chip_Info->Name;
    /* Name */
    this->Name=Chipname;
    /* There is no coprocessor when we initially create this */
    /* Config */
    for(const std::unique_ptr<class Conf_Info>&Conf:Plat_Info->Config)
        this->Config.insert(std::make_pair(Conf->Name,Conf->Default));
}
/* End Function:Chip::Chip ***************************************************/

/* Function:Chip::~Chip *******************************************************
Description : Destructor for chip information.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
/* void */ Chip::~Chip(void)
{

}
/* End Function:Chip::~Chip **************************************************/

/* Function:Chip::Save ********************************************************
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
    Main::CSV_Save(Parent,"Coprocessor",this->Coprocessor);
    Main::Pair_Save(Parent,"Config",this->Config);
}
/* End Function:Chip::Save ***************************************************/
}
/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
