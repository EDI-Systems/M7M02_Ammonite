/******************************************************************************
Filename    : compatible.cpp
Author      : pry
Date        : 13/01/2023
License     : Proprietary; confidential.
Description : Default option implementation.
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
#include "Plat_Info/Compatible/compatible.hpp"
#undef __HDR_DEF__

#define __HDR_CLASS__
#include "rvm_cfg.hpp"
#include "Plat_Info/Compatible/compatible.hpp"
#undef __HDR_CLASS__
/* End Include ***************************************************************/
namespace RVM_CFG
{
/* Function:Compatible::Compatible ********************************************
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
    /* GetNodeContent() instead of GetContent() */
    Temp=Node->GetNodeContent();
    Main::CSV_Read(Temp, List);

    /* wxLogDebug("Compatible: %d",List.size()); */
    /* wxLogDebug("Compatible: %s",Temp); */
    if(List.size()!=3)
        throw std::runtime_error("Compatibility entry malformed.");

    this->Buildsystem=List[0];
    this->Toolchain=List[1];
    this->Guest=List[2];
}
/* End Function:Compatible::Compatible ***************************************/

/* Function:Compatible::~Compatible *******************************************
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
