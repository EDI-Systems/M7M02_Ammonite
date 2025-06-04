/******************************************************************************
Filename    : tool_menu.cpp
Author      : pry
Date        : 11/01/2023
License     : Proprietary; confidential.
Description : Tool menu class implementation.
******************************************************************************/

/* Include *******************************************************************/
#define __HDR_DEF__
#include "rvm_cfg.hpp"
#undef __HDR_DEF__

#include "wx/wx.h"
#include "wx/xml/xml.h"

#include "Image/generate_small.xpm"
#include "Image/validate_small.xpm"
#include "Image/setting_small.xpm"

#include "map"
#include "set"
#include "string"
#include "memory"
#include "vector"
#include "algorithm"

#define __HDR_DEF__
#include "Menu_Bar/Tool_Menu/tool_menu.hpp"
#undef __HDR_DEF__

#define __HDR_CLASS__
#include "rvm_cfg.hpp"
#include "Menu_Bar/Tool_Menu/tool_menu.hpp"
#undef __HDR_CLASS__
/* End Include ***************************************************************/
namespace RVM_CFG
{
/* Function:Tool_Menu::Tool_Menu **********************************************
Description : Constructor for tool menu.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
/* void */ Tool_Menu::Tool_Menu(void)
{
    try
    {
        this->Shm_View=new wxMenuItem(this,wxID_ANY,_("Shared Memory Viewer(&S)"));
        this->Append(this->Shm_View);
        this->Bind(wxEVT_MENU,&Tool_Menu::On_Shm_View,this,this->Shm_View->GetId());

        this->AppendSeparator();

        this->Inv_View=new wxMenuItem(this,wxID_ANY,_("Invocation Viewer(&I)"));
        this->Append(this->Inv_View);
        this->Bind(wxEVT_MENU,&Tool_Menu::On_Inv_View,this,this->Inv_View->GetId());

        this->Rcv_View=new wxMenuItem(this,wxID_ANY,_("Receive Endpoint Viewer(&R)"));
        this->Append(this->Rcv_View);
        this->Bind(wxEVT_MENU,&Tool_Menu::On_Rcv_View,this,this->Rcv_View->GetId());

        this->AppendSeparator();

        this->Generate=new wxMenuItem(this,wxID_ANY,_("Generate(&G)\tCtrl+Shift+G"));
        this->Generate->SetBitmap(wxBitmap(Generate_Icon_Small));
        this->Append(this->Generate);
        this->Bind(wxEVT_MENU,&Tool_Menu::On_Generate,this,this->Generate->GetId());

        this->Validate=new wxMenuItem(this,wxID_ANY,_("Validate(&V)\tCtrl+Shift+V"));
        this->Validate->SetBitmap(wxBitmap(Validate_Icon_Small));
        this->Append(this->Validate);
        this->Bind(wxEVT_MENU,&Tool_Menu::On_Validate,this,this->Validate->GetId());

        this->AppendSeparator();

        this->Setting=new wxMenuItem(this,wxID_ANY,_("Settings(&V)\tCtrl+Shift+S"));
        this->Setting->SetBitmap(wxBitmap(Setting_Icon_Small));
        this->Append(this->Setting);
        this->Bind(wxEVT_MENU,&Tool_Menu::On_Setting,this,this->Setting->GetId());
    }
    catch(std::exception& Exc)
    {
        ASSERT(0,Exc.what());
    }
}
/* End Function:Tool_Menu::Tool_Menu *****************************************/

/* Function:Tool_Menu::~Tool_Menu *********************************************
Description : Destructor for tool menu.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
/* void */ Tool_Menu::~Tool_Menu(void)
{
    this->Unbind(wxEVT_MENU,&Tool_Menu::On_Shm_View,this);
    this->Unbind(wxEVT_MENU,&Tool_Menu::On_Inv_View,this);
    this->Unbind(wxEVT_MENU,&Tool_Menu::On_Rcv_View,this);
    this->Unbind(wxEVT_MENU,&Tool_Menu::On_Generate,this);
    this->Unbind(wxEVT_MENU,&Tool_Menu::On_Validate,this);
}
/* End Function:Tool_Menu::~Tool_Menu ****************************************/

/* Function:Tool_Menu::State_Set **********************************************
Description : Set the current UI state, and decide what controls are usable.
Input       : ptr_t Type - The state type.
Output      : None.
Return      : None.
******************************************************************************/
void Tool_Menu::State_Set(ptr_t Type)
{
    if(Type==STATE_UI)
    {
        if(Main::UI_State==UI_NONE)
        {
            this->Shm_View->Enable(false);
            this->Inv_View->Enable(false);
            this->Rcv_View->Enable(false);
            this->Generate->Enable(false);
            this->Validate->Enable(false);
        }
        else
        {
            this->Shm_View->Enable(true);
            this->Inv_View->Enable(true);
            this->Rcv_View->Enable(true);
            this->Generate->Enable(true);
            this->Validate->Enable(true);
        }
    }
}
/* End Function:Tool_Menu::State_Set *****************************************/

/* Function:Tool_Menu::On_Shm_View ********************************************
Description : wxEVT_MENU handler for 'Shared Memory Viewer'.
Input       : class wxCommandEvent& Event - The event.
Output      : None.
Return      : None.
******************************************************************************/
void Tool_Menu::On_Shm_View(class wxCommandEvent& Event)
{
    wxLogDebug("Tool_Menu::On_Shm_View");

    Main::Msgbox_Show(RVM_CFG_App::Main, MSGBOX_ERROR,
                      _("Tool Menu"),
                      _("This tool is not available in the current version yet."));
}
/* End Function:Tool_Menu::On_Shm_View ***************************************/

/* Function:Tool_Menu::On_Inv_View ********************************************
Description : wxEVT_MENU handler for 'Invocation Viewer'.
Input       : class wxCommandEvent& Event - The event.
Output      : None.
Return      : None.
******************************************************************************/
void Tool_Menu::On_Inv_View(class wxCommandEvent& Event)
{
    wxLogDebug("Tool_Menu::On_Inv_View");

    Main::Msgbox_Show(RVM_CFG_App::Main, MSGBOX_ERROR,
                      _("Tool Menu"),
                      _("This tool is not available in the current version yet."));
}
/* End Function:Tool_Menu::On_Inv_View ***************************************/

/* Function:Tool_Menu::On_Rcv_View ********************************************
Description : wxEVT_MENU handler for 'Receive Endpoint Viewer'.
Input       : class wxCommandEvent& Event - The event.
Output      : None.
Return      : None.
******************************************************************************/
void Tool_Menu::On_Rcv_View(class wxCommandEvent& Event)
{
    wxLogDebug("Tool_Menu::On_Rcv_View");

    Main::Msgbox_Show(RVM_CFG_App::Main, MSGBOX_ERROR,
                      _("Tool Menu"),
                      _("This tool is not available in the current version yet."));
}
/* End Function:Tool_Menu::On_Rcv_View ***************************************/

/* Function:Tool_Menu::On_Generate ********************************************
Description : wxEVT_MENU handler for 'Generate'.
Input       : class wxCommandEvent& Event - The event.
Output      : None.
Return      : None.
******************************************************************************/
void Tool_Menu::On_Generate(class wxCommandEvent& Event)
{
    wxLogDebug("Tool_Menu::On_Generate");
    Main::Generate_Begin(1);
}
/* End Function:Tool_Menu::On_Generate ***************************************/

/* Function:Tool_Menu::On_Validate ********************************************
Description : wxEVT_MENU handler for 'Validate'.
Input       : class wxCommandEvent& Event - The event.
Output      : None.
Return      : None.
******************************************************************************/
void Tool_Menu::On_Validate(class wxCommandEvent& Event)
{
    wxLogDebug("Tool_Menu::On_Validate");
    Main::Generate_Begin(0);
}
/* End Function:Tool_Menu::On_Validate ***************************************/

/* Function:Tool_Menu::On_Setting *********************************************
Description : wxEVT_MENU handler for 'Validate'.
Input       : class wxCommandEvent& Event - The event.
Output      : None.
Return      : None.
******************************************************************************/
void Tool_Menu::On_Setting(class wxCommandEvent& Event)
{
    wxLogDebug("Tool_Menu::On_Setting");
    Main::Setting_Begin();
}
/* End Function:Tool_Menu::On_Setting ****************************************/
}
/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
