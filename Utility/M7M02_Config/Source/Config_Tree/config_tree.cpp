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
/* option panels begin*/
#include "Option_Panel/option_panel.hpp"
#include "Option_Panel/Native_Notebook/native_notebook.hpp"
#include "Option_Panel/Virtual_Notebook/virtual_notebook.hpp"

#include "Option_Panel/Basic_Panel/basic_panel.hpp"
#include "Option_Panel/Memory_Notebook/memory_notebook.hpp"
#include "Option_Panel/Memory_Notebook/Extmem_Panel/extmem_panel.hpp"
#include "Option_Panel/Memory_Panel/memory_panel.hpp"
#include "Option_Panel/Kernel_Panel/kernel_panel.hpp"
#include "Option_Panel/Monitor_Panel/monitor_panel.hpp"

#include "Proj_Info/proj_info.hpp"
#include "Proj_Info/Process/process.hpp"
#include "Proj_Info/Process/Native/native.hpp"
#include "Proj_Info/Process/Virtual/virtual.hpp"
#include "Mem_Info/mem_info.hpp"
/* option panels end*/
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
    class wxTreeItemId Basic_Config;
    class wxTreeItemId Memory_Config;
    class wxTreeItemId Kernel_Config;
    class wxTreeItemId Monitor_Config;

    wxLogDebug("Config_Tree::Load");

    if(this->Root_Text=="")
        this->Root_Text=_("Project");
    this->Basic_Config_Text=_("Basic Config");
    this->Memory_Config_Text=_("Memory Config");
    this->Kernel_Config_Text=_("Kernel Config");
    this->Monitor_Config_Text=_("Monirot Config");
    this->Native_Config_Text=_("Native Process Config");
    this->Virtual_Config_Text=_("Virtual Process Config");

    /* Start from a clean tree */
    this->DeleteAllItems();

    /* Create tree */
    Root=this->AddRoot(_(this->Root_Text));
    Basic_Config=this->AppendItem(Root,_(this->Basic_Config_Text));
    Memory_Config=this->AppendItem(Root,_(this->Memory_Config_Text));
    Kernel_Config=this->AppendItem(Root,_(this->Kernel_Config_Text));
    Monitor_Config=this->AppendItem(Root,_(this->Monitor_Config_Text));

    this->Native_Config=this->AppendItem(Root,_(this->Native_Config_Text));
    this->Virtual_Config=this->AppendItem(Root,_(this->Virtual_Config_Text));

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

    if(Select_Text==this->Root_Text)
        this->Select_Detail=SELECT_DETAIL_ROOT;
    else if(Select_Text==this->Basic_Config_Text)
        this->Select_Detail=SELECT_DETAIL_BASIC;
    else if(Select_Text==this->Memory_Config_Text)
        this->Select_Detail=SELECT_DETAIL_MEMORY;
    else if(Select_Text==this->Kernel_Config_Text)
        this->Select_Detail=SELECT_DETAIL_KERNEL;
    else if(Select_Text==this->Monitor_Config_Text)
        this->Select_Detail=SELECT_DETAIL_MONITOR;
    else if(Select_Text==this->Native_Config_Text)
        this->Select_Detail=SELECT_DETAIL_NATIVE;
    else if(Select_Text==this->Virtual_Config_Text)
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

/* Function:Config_Tree::Option_Open ******************************************
Description : Open clicked option.
Input       : const std::string& Select_Text - The name of selected tree item.
Input       : ptr_t Type - The type for selected tree item.
Output      : None.
Return      : 0 for OK
******************************************************************************/
ret_t Config_Tree::Option_Open(const std::string& Select_Text, const ptr_t& Type)
{

    wxLogDebug("Config_Tree::Option_Open: %s",Select_Text);

    /* Check and try to save the current panel */
    if(Main::Check_And_Save_Current_Edit()!=0)
        return -1;

    if(Main::Last_Option!=nullptr)
        Main::Last_Option->Hide();

    /* Load and show the selected page */
    switch(Type)
    {
        case SELECT_DETAIL_BASIC:
        {
            Main::Basic_Panel->Load();
            Main::Basic_Panel->Show();
            Main::Basic_Panel->Has_Been_Shown=HAS_SHOWN;
            Main::Last_Option=Main::Basic_Panel;
            break;
        }
        case SELECT_DETAIL_MEMORY:
        {
            Main::Memory_Notebook->Load();
            Main::Memory_Notebook->Show();
            Main::Memory_Notebook->Has_Been_Shown=HAS_SHOWN;
            Main::Last_Option=Main::Memory_Notebook;
            break;
        }
        case SELECT_DETAIL_KERNEL:
        {
            Main::Kernel_Panel->Load();
            Main::Kernel_Panel->Show();
            Main::Kernel_Panel->Has_Been_Shown=HAS_SHOWN;
            Main::Last_Option=Main::Kernel_Panel;
            break;
        }
        case SELECT_DETAIL_MONITOR:
        {
            Main::Monitor_Panel->Load();
            Main::Monitor_Panel->Show();
            Main::Monitor_Panel->Has_Been_Shown=HAS_SHOWN;
            Main::Last_Option=Main::Monitor_Panel;
            break;
        }
        case SELECT_DETAIL_NATIVE_CHILD:
        {
            Main::Native_Notebook->Load(Select_Text);
            Main::Native_Notebook->Show();
            Main::Native_Notebook->Has_Been_Shown=HAS_SHOWN;
            Main::Last_Option=Main::Native_Notebook;
            break;
        }
        case SELECT_DETAIL_VIRTUAL_CHILD:
        {
            Main::Virtual_Notebook->Load(Select_Text);
            Main::Virtual_Notebook->Show();
            Main::Virtual_Notebook->Has_Been_Shown=HAS_SHOWN;
            Main::Last_Option=Main::Virtual_Notebook;
            break;
        }
        default:
        {
            return -1;
            break;
        }
    }

    Main::Option->Layout();
    return 0;
}
/* End Function:Config_Tree::Option_Open *************************************/

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

    /* Project root, native process config and virtual machine config won't be selected */
    if(Select_Text==this->Root_Text||Select_Text==this->Native_Config_Text||Select_Text==this->Virtual_Config_Text)
    {
        this->UnselectAll();
        return;
    }

    /* Try to open the option panel */
    if(this->Detail(Select)!=0)
        return;

    this->Option_Open(Select_Text,this->Select_Detail);
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

    /* See what is this - only allow sheets */
    Select=Event.GetItem();
    if(this->Detail(Select)!=0)
        return;

    if(this->Select_Detail==SELECT_DETAIL_VIRTUAL||this->Select_Detail==SELECT_DETAIL_NATIVE||this->Select_Detail==SELECT_DETAIL_ROOT)
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
    class wxTreeItemId Parent_Of_Drag;
    class wxTreeItemId Parent_Of_Target;

    wxLogDebug("Config_Tree::On_Drag_End");

    if (!this->Drag.IsOk())
        return;

    Target = Event.GetItem();
    if (!Target.IsOk()||Target==this->Drag)
        return;

    Parent_Of_Drag=this->GetItemParent(this->Drag);
    Parent_Of_Target=this->GetItemParent(Target);

    /* The two nodes must belong to the same parent node */
    if (Parent_Of_Drag!=Parent_Of_Target)
        return;

    /* Get the item text of the drag */
    Drag_Text=this->GetItemText(this->Drag);

    /* Find the position to insert */
    Child=this->GetFirstChild(Parent_Of_Target,Cookie);
    while (Child.IsOk())
    {
        if (Child==Target)
        {
            Insert_After=Child;
            break;
        }
        Child=this->GetNextChild(Parent_Of_Target,Cookie);
    }

    /* Delete the original node - the drag */
    this->Delete(this->Drag);
    /* Insert */
    this->InsertItem(Parent_Of_Target,Insert_After,Drag_Text);
    /* Reset */
    this->Drag = wxTreeItemId();
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
