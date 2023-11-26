/******************************************************************************
Filename    : menu_bar.cpp
Author      : zjx
Date        : 06/04/2021
License     : Proprietary; confidential.
Description : Menu bar class implementation.
******************************************************************************/

/* Include *******************************************************************/
#define __HDR_DEF__
#include "rvm_cfg.hpp"
#undef __HDR_DEF__

#include "wx/wx.h"
#include "wx/xml/xml.h"

#include "map"
#include "memory"
#include "vector"

#define __HDR_DEF__
#include "Menu_Bar/menu_bar.hpp"
#undef __HDR_DEF__

#define __HDR_CLASS__
#include "rvm_cfg.hpp"
#include "Menu_Bar/File_Menu/file_menu.hpp"
#include "Menu_Bar/Tool_Menu/tool_menu.hpp"
#include "Menu_Bar/Help_Menu/help_menu.hpp"
#include "Menu_Bar/menu_bar.hpp"
#undef __HDR_CLASS__
/* End Include ***************************************************************/
namespace RVM_CFG
{
/* Function:Menu_Bar::Menu_Bar ************************************************
Description : Constructor for menu bar.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
/* void */ Menu_Bar::Menu_Bar(void)
{
    try
    {
        this->File=new File_Menu();
        this->Append(this->File, _("File(&F)"));

        this->Tool=new Tool_Menu();
        this->Append(this->Tool, _("Tool(&T)"));

        this->Help=new Help_Menu();
        this->Append(this->Help, _("Help(&H)"));
    }
    catch(std::exception& Exc)
    {
        ASSERT(0,Exc.what());
    }
}
/* End Function:Menu_Bar::Menu_Bar *******************************************/

/* Function:Menu_Bar::State_Set ***********************************************
Description : Set the current UI state, and decide what controls are usable.
Input       : ptr_t Type - The state type.
Output      : None.
Return      : None.
******************************************************************************/
void Menu_Bar::State_Set(ptr_t Type)
{
    this->File->State_Set(Type);
    this->Tool->State_Set(Type);
    this->Help->State_Set(Type);

    this->Refresh();
}
/* End Function:Menu_Bar::State_Set ******************************************/
}
/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
