/******************************************************************************
Filename    : conf_info.cpp
Author      : pry
Date        : 14/01/2023
License     : Proprietary; confidential.
Description : Configuration information implementation.
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
#include <Plat_Info/Config/config.hpp>
#undef __HDR_DEF__

#define __HDR_CLASS__
#include "rvm_cfg.hpp"
#include <Plat_Info/Config/config.hpp>
#undef __HDR_CLASS__
/* End Include ***************************************************************/
namespace RVM_CFG
{
/* Function:Conf_Info::Conf_Info **********************************************
Description : Constructor for configuration information.
Input       : class wxXmlNode* Node - The node containing information.
Output      : None.
Return      : None.
******************************************************************************/
/* void */ Conf_Info::Conf_Info(class wxXmlNode* Node)
{
    std::string Temp;

    /* Platform */
    this->Name=Main::Text_Load(Node,"Name");

    /* Macro */
    this->Macro=Main::Text_Load(Node,"Macro");

    /* Type */
    Temp=Main::Text_Load(Node,"Type");
    if(Temp=="Choice")
        Type=CONFIG_CHOICE;
    else if(Temp=="Range")
        Type=CONFIG_RANGE;
    else
        throw std::runtime_error("Config type is malformed.");

    /* Range */
    Main::CSV_Read(Main::Text_Load(Node,"Range"), this->Range);

    /* Default */
    this->Default=Main::Text_Load(Node,"Default");
}
/* End Function:Conf_Info::Conf_Info *****************************************/

/* Function:Conf_Info::~Conf_Info *********************************************
Description : Destructor for configuration information.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
/* void */ Conf_Info::~Conf_Info(void)
{

}
/* End Function:Conf_Info::~Conf_Info ****************************************/
}
/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
