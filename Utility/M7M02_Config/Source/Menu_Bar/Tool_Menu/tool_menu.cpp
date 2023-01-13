/******************************************************************************
Filename    : tool_menu.cpp
Author      : pry
Date        : 11/01/2023
License     : Proprietary; confidential.
Description : Tool menu class implementation.
******************************************************************************/

/* Includes ******************************************************************/
#define __HDR_DEFS__
#include "rvm_cfg.hpp"
#undef __HDR_DEFS__

#include "wx/wx.h"
#include "wx/xml/xml.h"

#include "map"
#include "set"
#include "string"
#include "memory"
#include "vector"
#include "algorithm"

#define __HDR_DEFS__
#include "Menu_Bar/Tool_Menu/tool_menu.hpp"
#undef __HDR_DEFS__

#define __HDR_CLASSES__
#include "rvm_cfg.hpp"
#include "Menu_Bar/Tool_Menu/tool_menu.hpp"
#undef __HDR_CLASSES__
/* End Includes **************************************************************/
namespace RVM_CFG
{
/* Begin Function:Tool_Menu::Tool_Menu ****************************************
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
        this->Append(this->Generate);
        this->Bind(wxEVT_MENU,&Tool_Menu::On_Generate,this,this->Generate->GetId());

        this->Validate=new wxMenuItem(this,wxID_ANY,_("Validate(&V)\tCtrl+Shift+V"));
        this->Append(this->Validate);
        this->Bind(wxEVT_MENU,&Tool_Menu::On_Validate,this,this->Validate->GetId());
    }
    catch(std::exception& Exc)
    {
        ASSERT(0,Exc.what());
    }
}
/* End Function:Tool_Menu::Tool_Menu *****************************************/

/* Begin Function:Tool_Menu::~Tool_Menu ***************************************
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

/* Begin Function:Tool_Menu::State_Set ****************************************
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

/* Begin Function:Tool_Menu::On_Shm_View **************************************
Description : wxEVT_MENU handler for 'Shared Memory Viewer'.
Input       : class wxCommandEvent& Event - The event.
Output      : None.
Return      : None.
******************************************************************************/
void Tool_Menu::On_Shm_View(class wxCommandEvent& Event)
{
    wxLogDebug("Tool_Menu::On_Shm_View");
}
/* End Function:Tool_Menu::On_Shm_View ***************************************/

/* Begin Function:Tool_Menu::On_Inv_View **************************************
Description : wxEVT_MENU handler for 'Invocation Viewer'.
Input       : class wxCommandEvent& Event - The event.
Output      : None.
Return      : None.
******************************************************************************/
void Tool_Menu::On_Inv_View(class wxCommandEvent& Event)
{
    wxLogDebug("Tool_Menu::On_Inv_View");
}
/* End Function:Tool_Menu::On_Inv_View ***************************************/

/* Begin Function:Tool_Menu::On_Rcv_View **************************************
Description : wxEVT_MENU handler for 'Receive Endpoint Viewer'.
Input       : class wxCommandEvent& Event - The event.
Output      : None.
Return      : None.
******************************************************************************/
void Tool_Menu::On_Rcv_View(class wxCommandEvent& Event)
{
    wxLogDebug("Tool_Menu::On_Rcv_View");
}
/* End Function:Tool_Menu::On_Rcv_View ***************************************/

/* Begin Function:Tool_Menu::On_Generate **************************************
Description : wxEVT_MENU handler for 'Generate'.
Input       : class wxCommandEvent& Event - The event.
Output      : None.
Return      : None.
******************************************************************************/
void Tool_Menu::On_Generate(class wxCommandEvent& Event)
{
    wxLogDebug("Tool_Menu::On_Generate");
}
/* End Function:Tool_Menu::On_Generate ***************************************/

/* Begin Function:Tool_Menu::On_Validate **************************************
Description : wxEVT_MENU handler for 'Validate'.
Input       : class wxCommandEvent& Event - The event.
Output      : None.
Return      : None.
******************************************************************************/
void Tool_Menu::On_Validate(class wxCommandEvent& Event)
{
    wxLogDebug("Tool_Menu::On_Validate");
}
/* End Function:Tool_Menu::On_Validate ***************************************/
}
/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
