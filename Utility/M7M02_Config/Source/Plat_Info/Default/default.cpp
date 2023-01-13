/******************************************************************************
Filename    : default.cpp
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
#include "Plat_Info/Default/default.hpp"
#undef __HDR_DEFS__

#define __HDR_CLASSES__
#include "rvm_cfg.hpp"
#include "Plat_Info/Default/default.hpp"
#undef __HDR_CLASSES__
/* End Includes **************************************************************/
namespace RVM_CFG
{
/* Begin Function:Default::Default ********************************************
Description : Constructor for chip information.
Input       : class wxXmlNode* Node - The node containing information.
Output      : None.
Return      : None.
******************************************************************************/
/* void */ Default::Default(class wxXmlNode* Node)
{
    /* Buildsystem */
    this->Buildsystem=Main::Text_Load(Node,"Buildsystem");
    /* Toolchain */
    this->Toolchain=Main::Text_Load(Node,"Toolchain");
    /* Optimization */
    this->Optimization=Main::Opt_Load(Node,"Optimization");

    /* Kernel_Code_Size */
    this->Kernel_Code_Size=Main::Num_Load(Node,"Kernel_Code_Size");
    /* Kernel_Data_Size */
    this->Kernel_Data_Size=Main::Num_Load(Node,"Kernel_Data_Size");
    /* Kernel_Stack_Size */
    this->Kernel_Stack_Size=Main::Num_Load(Node,"Kernel_Stack_Size");

    /* Monitor_Code_Size */
    this->Monitor_Code_Size=Main::Num_Load(Node,"Monitor_Code_Size");
    /* Monitor_Data_Size */
    this->Monitor_Data_Size=Main::Num_Load(Node,"Monitor_Data_Size");

    /* Init_Stack_Size */
    this->Init_Stack_Size=Main::Num_Load(Node,"Init_Stack_Size");
    /* Sftd_Stack_Size */
    this->Sftd_Stack_Size=Main::Num_Load(Node,"Sftd_Stack_Size");
    /* Vctd_Stack_Size */
    this->Vctd_Stack_Size=Main::Num_Load(Node,"Vctd_Stack_Size");
    /* Timd_Stack_Size */
    this->Timd_Stack_Size=Main::Num_Load(Node,"Timd_Stack_Size");
    /* Hypd_Stack_Size */
    this->Hypd_Stack_Size=Main::Num_Load(Node,"Hypd_Stack_Size");
}
/* End Function:Default::Default *********************************************/

/* Begin Function:Default::~Default *******************************************
Description : Destructor for chip information.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
/* void */ Default::~Default(void)
{

}
/* End Function:Default::~Default ********************************************/
}
/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
