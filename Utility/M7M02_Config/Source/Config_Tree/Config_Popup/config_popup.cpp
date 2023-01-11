/******************************************************************************
Filename    : config_popup.cpp
Author      : zjx
Date        : 11/01/2023
License     : Proprietary; confidential.
Description : Config popup-menu class implementation.
******************************************************************************/

/* Includes ******************************************************************/
#define __HDR_DEFS__
#include "rvm_cfg.hpp"
#undef __HDR_DEFS__

#include "wx/wx.h"
#include "wx/xml/xml.h"
#include "wx/treectrl.h"
#include "wx/textdlg.h"
#include "wx/dnd.h"

#include "map"
#include "set"
#include "memory"

#define __HDR_DEFS__
#include "Config_Tree/Config_Popup/config_popup.hpp"
#include "Config_Tree/config_tree.hpp"
#undef __HDR_DEFS__

#define __HDR_CLASSES__
#include "rvm_cfg.hpp"
#include "Config_Tree/Config_Popup/config_popup.hpp"
#include "Config_Tree/config_tree.hpp"
#undef __HDR_CLASSES__
/* End Includes **************************************************************/
namespace RVM_CFG
{
/* Begin Function:Config_Popup::Config_Popup **************************************
Description : Constructor for file popup menu.
Input       : class Config_Tree* Tree - The tree that this menu corresponds to.
Output      : None.
Return      : None.
******************************************************************************/
/* void */ Config_Popup::Config_Popup(class Config_Tree* Tree)
{
    try
    {
        this->Tree=Tree;

        this->New_Item=new wxMenuItem(this,wxID_ANY,_("New Item"));
        this->Append(this->New_Item);
        this->Bind(wxEVT_MENU,&Config_Popup::On_New_Item,this,this->New_Item->GetId());

        this->Delete_Item=new wxMenuItem(this,wxID_ANY,_("Delete Item"));
        this->Append(this->Delete_Item);
        this->Bind(wxEVT_MENU,&Config_Popup::On_Delete_Item,this,this->Delete_Item->GetId());

        this->AppendSeparator();

        this->Generate=new wxMenuItem(this,wxID_ANY,_("Generate"));
        this->Append(this->Generate);
        this->Bind(wxEVT_MENU,&Config_Popup::On_Generate,this,this->Generate->GetId());

        this->Validate=new wxMenuItem(this,wxID_ANY,_("Validate"));
        this->Append(this->Validate);
        this->Bind(wxEVT_MENU,&Config_Popup::On_Validate,this,this->Validate->GetId());
    }
    catch(std::exception& Exc)
    {
        ASSERT(0,Exc.what());
    }
}
/* End Function:Config_Popup::Config_Popup ***********************************/

/* Begin Function:Config_Popup::~Config_Popup *********************************
Description : Destructor for file menu.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
/* void */ Config_Popup::~Config_Popup(void)
{
    this->Unbind(wxEVT_MENU,&Config_Popup::On_New_Item,this);
    this->Unbind(wxEVT_MENU,&Config_Popup::On_Delete_Item,this);
    this->Unbind(wxEVT_MENU,&Config_Popup::On_Generate,this);
    this->Unbind(wxEVT_MENU,&Config_Popup::On_Validate,this);
}
/* End Function:Config_Popup::~Config_Popup **********************************/

/* Begin Function:Config_Popup::State_Set *************************************
Description : Set the current UI state, and decide what controls are usable.
Input       : ptr_t Type - The state type.
Output      : None.
Return      : None.
******************************************************************************/
void Config_Popup::State_Set(ptr_t Type)
{
    if(Type==STATE_UI)
    {
        if(Main::UI_State==UI_NONE)
        {
            this->New_Item->Enable(false);
            this->Delete_Item->Enable(false);
            this->Generate->Enable(false);
            this->Validate->Enable(false);
        }
        else
        {
            switch(this->Tree->Select_Detail)
            {
                case SELECT_DETAIL_NONE:
                case SELECT_DETAIL_KERNEL:
                case SELECT_DETAIL_MONITOR:
                {
                    this->New_Item->Enable(false);
                    this->Delete_Item->Enable(false);
                    break;
                }

                case SELECT_DETAIL_FOLDER:
                {
                    this->New_Item->Enable(true);
                    this->Delete_Item->Enable(false);
                    break;
                }
                case SELECT_DETAIL_VIRTUAL:
                case SELECT_DETAIL_PROCESS:
                {
                    this->New_Item->Enable(true);
                    this->Delete_Item->Enable(true);
                    break;
                }
                default:ASSERT(0,"The select detail is incorrect.");
            }
            this->Generate->Enable(true);
            this->Validate->Enable(true);
        }
    }
}
/* End Function:Config_Popup::State_Set ****************************************/

/* Begin Function:Config_Popup::On_New_Item *************************************
Description : wxEVT_MENU handler for 'New Item'.
Input       : class wxCommandEvent& Event - The event.
Output      : None.
Return      : None.
******************************************************************************/
void Config_Popup::On_New_Item(class wxCommandEvent& Event)
{
    wxLogDebug("Config_Popup::On_New_Item");
}
/* End Function:Config_Popup::On_New_File ************************************/

/* Begin Function:Config_Popup::On_Delete_Item ********************************
Description : wxEVT_MENU handler for 'Delete Item'.
Input       : class wxCommandEvent& Event - The event.
Output      : None.
Return      : None.
******************************************************************************/
void Config_Popup::On_Delete_Item(class wxCommandEvent& Event)
{
    wxLogDebug("Config_Popup::On_Delete_Item");
}
/* End Function:Config_Popup::On_Delete_Item *********************************/

/* Begin Function:Config_Popup::On_Generate ***********************************
Description : wxEVT_MENU handler for 'Generate'.
Input       : class wxCommandEvent& Event - The event.
Output      : None.
Return      : None.
******************************************************************************/
void Config_Popup::On_Generate(class wxCommandEvent& Event)
{
    wxLogDebug("Config_Popup::On_Generate");
}
/* End Function:Config_Popup::On_Generate ************************************/

/* Begin Function:Config_Popup::On_Validate ***********************************
Description : wxEVT_MENU handler for 'Validate'.
Input       : class wxCommandEvent& Event - The event.
Output      : None.
Return      : None.
******************************************************************************/
void Config_Popup::On_Validate(class wxCommandEvent& Event)
{
    wxLogDebug("Config_Popup::On_Validate");
}
/* End Function:Config_Popup::On_Validate ************************************/
}
/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
