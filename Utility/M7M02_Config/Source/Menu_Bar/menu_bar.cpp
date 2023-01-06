/******************************************************************************
Filename    : menu_bar.cpp
Author      : zjx
Date        : 06/04/2021
License     : Proprietary; confidential.
Description : Menu bar class implementation.
******************************************************************************/

/* Includes ******************************************************************/
#define __HDR_DEFS__
#include "sdm_dl.hpp"
#undef __HDR_DEFS__

#include "wx/wx.h"

#define __HDR_DEFS__
#include "Menu_Bar/menu_bar.hpp"
#undef __HDR_DEFS__

#define __HDR_CLASSES__
#include "sdm_dl.hpp"
#include "Menu_Bar/Help_Menu/help_menu.hpp"
#include "Menu_Bar/menu_bar.hpp"
#undef __HDR_CLASSES__
/* End Includes **************************************************************/
namespace RVM_CFG
{
/* Begin Function:Menu_Bar::Menu_Bar ******************************************
Description : Constructor for menu bar.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
/* void */ Menu_Bar::Menu_Bar(void)
{
    try
    {
        this->Help=new Help_Menu();
        this->Append(this->Help, sT("Help(&H)"));
    }
    catch(std::exception& Exc)
    {
        ASSERT(0,Exc.what());
    }
}
/* End Function:Menu_Bar::Menu_Bar *******************************************/
}
/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
