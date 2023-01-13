/******************************************************************************
Filename    : compatible.cpp
Author      : pry
Date        : 13/01/2023
License     : Proprietary; confidential.
Description : Default option implementation.
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
#include "Plat_Info/Compatible/compatible.hpp"
#undef __HDR_DEFS__

#define __HDR_CLASSES__
#include "rvm_cfg.hpp"
#include "Plat_Info/Compatible/compatible.hpp"
#undef __HDR_CLASSES__
/* End Includes **************************************************************/
namespace RVM_CFG
{
/* Begin Function:Compatible::Compatible **************************************
Description : Constructor for chip information.
Input       : class wxXmlNode* Node - The node containing information.
Output      : None.
Return      : None.
******************************************************************************/
/* void */ Compatible::Compatible(class wxXmlNode* Node)
{
    std::string Temp;
    std::vector<std::string> List;

    /* This node should directly contain the contents */
    Temp=Node->GetContent();

    Main::CSV_Read(Temp, List);
    if(List.size()!=3)
        throw std::runtime_error("Compatibility entry malformed.");

    this->Buildsystem=List[0];
    this->Toolchain=List[1];
    this->Guest=List[2];
}
/* End Function:Compatible::Compatible ***************************************/

/* Begin Function:Compatible::~Compatible *************************************
Description : Destructor for chip information.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
/* void */ Compatible::~Compatible(void)
{

}
/* End Function:Compatible::~Compatible **************************************/
}
/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
