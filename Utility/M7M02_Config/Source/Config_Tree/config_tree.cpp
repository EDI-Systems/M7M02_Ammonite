/******************************************************************************
Filename    : config_tree.cpp
Author      : zjx
Date        : 03/09/2021
License     : Proprietary; confidential.
Description : Config tree implementation.
******************************************************************************/

/* Include *******************************************************************/
#define __HDR_DEF__
#include "rvm_cfg.hpp"
#undef __HDR_DEF__

#include "wx/wx.h"
#include "wx/xml/xml.h"
#include "wx/treectrl.h"
#include "wx/dnd.h"
#include "wx/notebook.h"

#include "map"
#include "set"
#include "memory"

#define __HDR_DEF__
#include "Config_Tree/Config_Popup/config_popup.hpp"
#include "Config_Tree/config_tree.hpp"
#undef __HDR_DEF__

#define __HDR_CLASS__
#include "rvm_cfg.hpp"
#include "Config_Tree/Config_Popup/config_popup.hpp"
#include "Config_Tree/config_tree.hpp"

#include "Option_Panel/option_panel.hpp"

#include "Proj_Info/proj_info.hpp"
#include "Proj_Info/Process/process.hpp"
#include "Proj_Info/Process/Native/native.hpp"
#include "Proj_Info/Process/Virtual/virtual.hpp"
#include "Mem_Info/mem_info.hpp"
#undef __HDR_CLASS__
/* End Include ***************************************************************/
namespace RVM_CFG
{
/* Function:Config_Tree::Config_Tree ******************************************
Description : Constructor for config tree.
Input       : class wxWindow* Parent - The parent panel.
              ptr_t Type - The type of this tree.
Output      : None.
Return      : None.
******************************************************************************/
/* void */ Config_Tree::Config_Tree(class wxWindow* Parent):
wxTreeCtrl(Parent,wxID_ANY,wxDefaultPosition,wxDefaultSize,wxTR_HAS_BUTTONS|wxTR_LINES_AT_ROOT)
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

/* Function:Config_Tree::~Config_Tree *****************************************
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

/* Function:Config_Tree::Load *************************************************
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

    /* Create tree - showing basic config by default as always */
    Root=this->AddRoot(Main::Proj_Info->Name);
    this->Basic_Config=this->AppendItem(Root,_("Basic Config"));
    this->Bold=this->Basic_Config;
    this->SetItemBold(this->Bold);
    this->AppendItem(Root,_("Memory Config"));
    this->AppendItem(Root,_("Kernel Config"));
    this->AppendItem(Root,_("Monitor Config"));
    this->Native_Config=this->AppendItem(Root,_("Native Process Config"));
    this->Virtual_Config=this->AppendItem(Root,_("Virtual Machine Config"));

    /* Update config tree */
    for(std::unique_ptr<class Native>&It : Main::Proj_Info->Native)
        this->AppendItem(this->Native_Config,It->Name);

    for(std::unique_ptr<class Virtual>&It : Main::Proj_Info->Virtual)
        this->AppendItem(this->Virtual_Config,It->Name);

    /* Expand all nodes by default */
    this->ExpandAll();
    this->UnselectAll();
}
/* End Function:Config_Tree::Load ********************************************/

/* Function:Config_Tree::Detail ***********************************************
Description : Update the selection marker and details.
Input       : class wxTreeItemId& Select - The selected entry.
Output      : None.
Return      : ret_t - If successful, 0; else -1.
******************************************************************************/
ret_t Config_Tree::Detail(class wxTreeItemId& Select)
{
    std::string Select_Text;

    wxLogDebug("Config_Tree::Detail");

    if(Select.IsOk()==false)
        return -1;
    Select_Text=this->GetItemText(Select);

    if(Select_Text==this->GetItemText(this->GetRootItem()))
        this->Select_Detail=SELECT_DETAIL_ROOT;
    else if(Select_Text==_("Basic Config"))
        this->Select_Detail=SELECT_DETAIL_BASIC;
    else if(Select_Text==_("Memory Config"))
        this->Select_Detail=SELECT_DETAIL_MEMORY;
    else if(Select_Text==_("Kernel Config"))
        this->Select_Detail=SELECT_DETAIL_KERNEL;
    else if(Select_Text==_("Monitor Config"))
        this->Select_Detail=SELECT_DETAIL_MONITOR;
    else if(Select_Text==_("Native Process Config"))
        this->Select_Detail=SELECT_DETAIL_NATIVE;
    else if(Select_Text==_("Virtual Machine Config"))
        this->Select_Detail=SELECT_DETAIL_VIRTUAL;
    else
    {
        if(Main::Proj_Info->Native_Map.find(Select_Text)!=Main::Proj_Info->Native_Map.end())
            this->Select_Detail=SELECT_DETAIL_NATIVE_CHILD;
        else if(Main::Proj_Info->Virtual_Map.find(Select_Text)!=Main::Proj_Info->Virtual_Map.end())
            this->Select_Detail=SELECT_DETAIL_VIRTUAL_CHILD;
        else
        {
            this->Select_Detail=SELECT_DETAIL_NONE;
            return -1;
        }
    }

    return 0;
}
/* End Function:Config_Tree::Detail ******************************************/

/* Function:Config_Tree::Native_Rename ****************************************
Description : Rename the native process and update the config tree.
Input       : const std::string& Original - The original name of native process.
              const std::string& Current - The current name of native process.
Output      : None.
Return      : None.
******************************************************************************/
void Config_Tree::Native_Rename(const std::string& Original, const std::string& Current)
{
    /* Cookie is a typedef, not a class */
    wxTreeItemIdValue Cookie;
    class wxTreeItemId Child;

    Child=this->GetFirstChild(this->Native_Config,Cookie);
    while(Child.IsOk()==true)
    {
        if(this->GetItemText(Child)==Original)
        {
            this->SetItemText(Child,Current);
            return;
        }
        Child=this->GetNextChild(this->Native_Config,Cookie);
    }

    ASSERT(0,"No such child found in native tree!");
}
/* End Function:Config_Tree::Native_Rename ***********************************/

/* Function:Config_Tree::Virtual_Rename ***************************************
Description : Rename the native process and update the config tree.
Input       : const std::string& Original - The original name of native process.
              const std::string& Current - The current name of native process.
Output      : None.
Return      : None.
******************************************************************************/
void Config_Tree::Virtual_Rename(const std::string& Original, const std::string& Current)
{
    /* Cookie is a typedef, not a class */
    wxTreeItemIdValue Cookie;
    class wxTreeItemId Child;

    Child=this->GetFirstChild(this->Virtual_Config,Cookie);
    while(Child.IsOk()==true)
    {
        if(this->GetItemText(Child)==Original)
        {
            this->SetItemText(Child,Current);
            return;
        }
        Child=this->GetNextChild(this->Virtual_Config,Cookie);
    }

    ASSERT(0,"No such child found in virtual tree!");
}
/* End Function:Config_Tree::Virtual_Rename **********************************/

/* Function:Config_Tree::State_Set ********************************************
Description : Set the current UI state, and decide what controls are usable.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
void Config_Tree::State_Set(void)
{
    this->Popup->State_Set();
}
/* End Function:Config_Tree::State_Set ***************************************/

/* Function:Config_Tree::On_Activate ******************************************
Description : wxEVT_TREE_ITEM_ACTIVATED handler for 'Selection activate'.
Input       : class wxTreeEvent& Event - The event.
Output      : None.
Return      : None.
******************************************************************************/
void Config_Tree::On_Activate(class wxTreeEvent& Event)
{
    std::string Select_Text;
    class wxTreeItemId Select;

    wxLogDebug("Config_Tree::On_Activate");

    Select=Event.GetItem();
    Select_Text=this->GetItemText(Select);

    /* Project root, native process config and virtual machine config
     * won't be selected here and they don't have an option panel. */
    if((Select_Text==this->GetItemText(this->GetRootItem()))||
       (Select_Text==_("Native Process Config"))||
       (Select_Text==_("Virtual Machine Config")))
    {
        this->UnselectAll();
        return;
    }

    /* See what is selected, or nothing at all */
    if(this->Detail(Select)!=0)
        return;

    /* See if we can activate that panel (current one has no errors) */
    if(Main::Option_Panel->Option_Show(Select_Text,this->Select_Detail)!=0)
        return;

    /* Yes we can, and paint in bold the selected entry */
    if(this->Bold.IsOk())
        this->SetItemBold(this->Bold,false);
    this->Bold=Select;
    this->SetItemBold(Select);
}
/* End Function:Config_Tree::On_Activate *************************************/

/* Function:Config_Tree::On_Menu **********************************************
Description : wxEVT_TREE_ITEM_MENU handler.
Input       : class wxTreeEvent& Event - The event.
Output      : None.
Return      : None.
******************************************************************************/
void Config_Tree::On_Menu(class wxTreeEvent& Event)
{
    class wxTreeItemId Select;

    wxLogDebug("Config_Tree::On_Right_Down");

    /* This menu is popped up on something. See if we'll be able to */
    Select=Event.GetItem();
    if(this->Detail(Select)!=0)
        return;

    this->Active=Select;

    /* Update state and pop-up */
    this->Popup->State_Set();
    this->PopupMenu(this->Popup,Event.GetPoint());
}
/* End Function:Config_Tree::On_Menu *****************************************/

/* Function:Config_Tree::On_Drag_Begin ****************************************
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

    /* See what is this - only allow dragging processes */
    Select=Event.GetItem();
    if(this->Detail(Select)!=0)
        return;

    if((this->Select_Detail!=SELECT_DETAIL_NATIVE_CHILD)&&
       (this->Select_Detail!=SELECT_DETAIL_VIRTUAL_CHILD))
        return;

    this->Drag=Select;
    Event.Allow();
}
/* End Function:Config_Tree::On_Drag_Begin ***********************************/

/* Function:Config_Tree::On_Drag_End ******************************************
Description : wxEVT_TREE_END_DRAG handler for 'End Drag'.
              Only the closed sheets may be dragged. Open sheets are not allowed.
Input       : class wxTreeEvent& Event - The event.
Output      : None.
Return      : None.
******************************************************************************/
void Config_Tree::On_Drag_End(class wxTreeEvent& Event)
{
    std::string Drag_Text;
    wxTreeItemIdValue Cookie;
    class wxTreeItemId Target;
    class wxTreeItemId Child;
    class wxTreeItemId Insert_After;
    class wxTreeItemId Parent_Drag;
    class wxTreeItemId Parent_Target;

    wxLogDebug("Config_Tree::On_Drag_End");

    if(!this->Drag.IsOk())
        return;

    /* Dragging to oneself or elsewhere is not honored */
    Target=Event.GetItem();
    if((!Target.IsOk())||(Target==this->Drag))
        return;

    Parent_Drag=this->GetItemParent(this->Drag);
    Parent_Target=this->GetItemParent(Target);

    /* The two nodes must belong to the same parent node */
    if(Parent_Drag!=Parent_Target)
        return;

    /* Get the item text of the drag */
    Drag_Text=this->GetItemText(this->Drag);

    /* Find the position to insert */
    Child=this->GetFirstChild(Parent_Target,Cookie);
    while(Child.IsOk())
    {
        if(Child==Target)
        {
            Insert_After=Child;
            break;
        }
        Child=this->GetNextChild(Parent_Target,Cookie);
    }

    /* Retain original bold status while inserting */
    if(this->IsBold(this->Drag))
    {
        this->Delete(this->Drag);
        this->Bold=this->InsertItem(Parent_Target,Insert_After,Drag_Text);
        this->SetItemBold(this->Bold);
    }
    else
    {
        this->Delete(this->Drag);
        this->InsertItem(Parent_Target,Insert_After,Drag_Text);
    }

    /* Need to reflect this change to the project, too */
    if(Parent_Drag==this->Native_Config)
        Main::Proj_Info->Native_Move(Drag_Text,std::string(this->GetItemText(Insert_After)));
    else
        Main::Proj_Info->Virtual_Move(Drag_Text,std::string(this->GetItemText(Insert_After)));
}
/* End Function:Config_Tree::On_Drag_End *************************************/

/* Function:Config_Drop::OnDropFiles ******************************************
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
