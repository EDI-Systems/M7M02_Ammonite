/******************************************************************************
Filename    : config_tree.cpp
Author      : zjx
Date        : 03/09/2021
License     : Proprietary; confidential.
Description : Config tree implementation.
******************************************************************************/

/* Includes ******************************************************************/
#define __HDR_DEFS__
#include "rvm_cfg.hpp"
#undef __HDR_DEFS__

#include "wx/wx.h"
#include "wx/xml/xml.h"
#include "wx/treectrl.h"
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
/* Begin Function:Config_Tree::Config_Tree ************************************
Description : Constructor for config tree.
Input       : class wxWindow* Parent - The parent panel.
              ptr_t Type - The type of this tree.
Output      : None.
Return      : None.
******************************************************************************/
/* void */ Config_Tree::Config_Tree(class wxWindow* Parent):
wxTreeCtrl(Parent,wxID_ANY,wxDefaultPosition,wxDefaultSize,wxTR_HIDE_ROOT|wxTR_HAS_BUTTONS|wxTR_LINES_AT_ROOT)
{
    try
    {
        this->Select_Detail=SELECT_DETAIL_NONE;
        this->Popup=new class Config_Popup(this);

        this->Bind(wxEVT_TREE_ITEM_ACTIVATED,&Config_Tree::On_Activate,this,this->GetId());
        this->Bind(wxEVT_TREE_ITEM_MENU,&Config_Tree::On_Menu,this,this->GetId());
        this->Bind(wxEVT_TREE_BEGIN_DRAG,&Config_Tree::On_Drag_Begin,this,this->GetId());
        this->Bind(wxEVT_TREE_END_DRAG,&Config_Tree::On_Drag_End,this,this->GetId());

        this->SetDropTarget(new class Config_Drop());
    }
    catch(std::exception& Exc)
    {
        ASSERT(0,Exc.what());
    }
}
/* End Function:Config_Tree::Config_Tree *************************************/

/* Begin Function:Config_Tree::~Config_Tree ***********************************
Description : Destructor for file tree.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
/* void */ Config_Tree::~Config_Tree(void)
{
    this->Unbind(wxEVT_TREE_ITEM_ACTIVATED,&Config_Tree::On_Activate,this);
    this->Unbind(wxEVT_TREE_ITEM_MENU,&Config_Tree::On_Menu,this);
    this->Unbind(wxEVT_TREE_BEGIN_DRAG,&Config_Tree::On_Drag_Begin,this);
    this->Unbind(wxEVT_TREE_END_DRAG,&Config_Tree::On_Drag_End,this);
    /* The pop-ups does not have a parent and must be explicitly deleted */
    delete this->Popup;
}
/* End Function:Config_Tree::~Config_Tree ************************************/

/* Begin Function:Config_Tree::Load *******************************************
Description : Load up the config tree.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
void Config_Tree::Load(void)
{
    class wxTreeItemId Root;

    wxLogDebug("Config_Tree::Load");

    /* Start from a clean tree */
    this->DeleteAllItems();

    /* Expand all nodes by default */
    this->ExpandAll();
    this->UnselectAll();
}
/* End Function:Config_Tree::Load ********************************************/

/* Begin Function:Config_Tree::Detail *****************************************
Description : Update the selection marker and details.
Input       : class wxTreeItemId& Select - The selected entry.
Output      : None.
Return      : ret_t - If successful, 0; else -1.
******************************************************************************/
ret_t Config_Tree::Detail(class wxTreeItemId& Select)
{
    std::string Temp;
    std::string Fullname;

    wxLogDebug("Config_Tree::Detail");

    return 0;
}
/* End Function:Config_Tree::Detail ******************************************/

/* Begin Function:Config_Tree::State_Set **************************************
Description : Set the current UI state, and decide what controls are usable.
Input       : ptr_t Type - The state type.
Output      : None.
Return      : None.
******************************************************************************/
void Config_Tree::State_Set(ptr_t Type)
{
    this->Popup->State_Set(Type);
}
/* End Function:Config_Tree::State_Set ***************************************/

/* Begin Function:Config_Tree::On_Activate ************************************
Description : wxEVT_TREE_ITEM_ACTIVATED handler for 'Selection activate'.
Input       : class wxTreeEvent& Event - The event.
Output      : None.
Return      : None.
******************************************************************************/
void Config_Tree::On_Activate(class wxTreeEvent& Event)
{
    class wxTreeItemId Sheet;

    wxLogDebug("Config_Tree::On_Activate");
}
/* End Function:Config_Tree::On_Activate *************************************/

/* Begin Function:Config_Tree::On_Menu ****************************************
Description : wxEVT_TREE_ITEM_MENU handler.
Input       : class wxTreeEvent& Event - The event.
Output      : None.
Return      : None.
******************************************************************************/
void Config_Tree::On_Menu(class wxTreeEvent& Event)
{
    class wxTreeItemId Select;
    std::string Temp;
    std::string Fullname;

    wxLogDebug("Config_Tree::On_Right_Down");

    /* This menu is popped up on something. See if we'll be able to */
    Select=Event.GetItem();
    if(this->Detail(Select)!=0)
        return;

    /* Update state and pop-up */
    this->Popup->State_Set(STATE_UI);
    this->PopupMenu(this->Popup,Event.GetPoint());
}
/* End Function:Config_Tree::On_Menu *****************************************/

/* Begin Function:Config_Tree::On_Drag_Begin **********************************
Description : wxEVT_TREE_BEGIN_DRAG handler for 'Begin Drag'.
              Only the closed sheets may be dragged. Open sheets are not allowed.
Input       : class wxTreeEvent& Event - The event.
Output      : None.
Return      : None.
******************************************************************************/
void Config_Tree::On_Drag_Begin(class wxTreeEvent& Event)
{
    class wxTreeItemId Select;

    wxLogDebug("Config_Tree::On_Drag_Begin");

    Event.Allow();
}
/* End Function:Config_Tree::On_Drag_Begin ***********************************/

/* Begin Function:Config_Tree::On_Drag_End ************************************
Description : wxEVT_TREE_END_DRAG handler for 'End Drag'.
              Only the closed sheets may be dragged. Open sheets are not allowed.
Input       : class wxTreeEvent& Event - The event.
Output      : None.
Return      : None.
******************************************************************************/
void Config_Tree::On_Drag_End(class wxTreeEvent& Event)
{
    ptr_t Load_Type;
    class wxTreeItemId Select;
    std::string Original;
    std::string Pathname;
    std::string Current;

    wxLogDebug("Config_Tree::On_Drag_End");
}
/* End Function:Config_Tree::On_Drag_End *************************************/

/* Begin Function:Config_Drop::OnDropFiles ************************************
Description : Drop project onto the tree to open it.
Input       : wxCoord X - The X coordinate.
              wxCoord Y - The Y coordinate.
              const class wxArrayString& Filenames - The name of the files dragged.
Output      : None.
Return      : bool - If true, accept the files; else reject the files.
******************************************************************************/
bool Config_Drop::OnDropFiles(wxCoord X, wxCoord Y, const class wxArrayString& Filenames)
{
    wxLogDebug("Config_Drop::OnDropFiles");

    return true;
}
/* End Function:Config_Drop::OnDropFiles *************(***********************/
}
/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
