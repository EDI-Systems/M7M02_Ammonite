/******************************************************************************
Filename    : config_popup.cpp
Author      : zjx
Date        : 11/01/2023
License     : Proprietary; confidential.
Description : Config popup-menu class implementation.
******************************************************************************/

/* Include *******************************************************************/
#define __HDR_DEF__
#include "rvm_cfg.hpp"
#undef __HDR_DEF__

#include "wx/wx.h"
#include "wx/xml/xml.h"
#include "wx/treectrl.h"
#include "wx/textdlg.h"
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
#include "Proj_Info/proj_info.hpp"
#include "Proj_Info/Process/process.hpp"
#include "Proj_Info/Process/Native/native.hpp"
#include "Proj_Info/Process/Virtual/virtual.hpp"

#include "Option_Panel/option_panel.hpp"
//#include "Option_Panel/Basic_Notebook/basic_notebook.hpp"
#include "Option_Panel/Basic_Panel/basic_panel.hpp"
#include "Option_Panel/Native_Notebook/native_notebook.hpp"
#include "Option_Panel/Native_Notebook/Native_Panel/native_panel.hpp"
#include "Option_Panel/Virtual_Notebook/virtual_notebook.hpp"
#include "Option_Panel/Virtual_Notebook/Virtual_Panel/virtual_panel.hpp"

#include "Proj_Info/Process/Native/native.hpp"
#include "Proj_Info/Process/Send/send.hpp"

#include "Option_Panel/Basic_Panel/basic_panel.hpp"

#include "Name_Dialog/name_dialog.hpp"
/* option panels end*/
#undef __HDR_CLASS__
/* End Include ***************************************************************/
namespace RVM_CFG
{
/* Function:Config_Popup::Config_Popup ****************************************
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

        this->Rename=new wxMenuItem(this,wxID_ANY,_("Rename"));
        this->Append(this->Rename);
        this->Bind(wxEVT_MENU,&Config_Popup::On_Rename,this,this->Rename->GetId());
    }
    catch(std::exception& Exc)
    {
        ASSERT(0,Exc.what());
    }
}
/* End Function:Config_Popup::Config_Popup ***********************************/

/* Function:Config_Popup::~Config_Popup ***************************************
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

/* Function:Config_Popup::Native_Add ******************************************
Description : Add a new native process and update the config tree.
Input       : const std::string& Name - The name of new native process.
Output      : None.
Return      : ret_t - Return 0 if this native process was added successfully.
******************************************************************************/
ret_t Config_Popup::Native_Add(const std::string& Name)
{
    if(Main::Proj_Info->Native_Add(Name)==0)
    {
        this->Tree->AppendItem(this->Tree->Native_Config,Name);
        this->Tree->Expand(this->Tree->Native_Config);
        wxLogDebug("Config_Tree::Add_Native: Success");
        return 0;
    }

    wxLogDebug("Config_Tree::Add_Native: Fail");
    return -1;
}
/* End Function:Config_Popup::Native_Add *************************************/

/* Function:Config_Popup::Virtual_Add *****************************************
Description : Add a new virtual machine and update the config tree.
Input       : const std::string& Name - The name of new vitual machine.
Output      : None.
Return      : ret_t - Return 0 if this virtual machine was added successfully.
******************************************************************************/
ret_t Config_Popup::Virtual_Add(const std::string& Name)
{
    if(Main::Proj_Info->Virtual_Add(Name)==0)
    {
        this->Tree->AppendItem(this->Tree->Virtual_Config,Name);
        this->Tree->Expand(this->Tree->Virtual_Config);
        wxLogDebug("Config_Tree::Add_Virtual: Success");
        return 0;
    }

    wxLogDebug("Config_Tree::Add_Virtual: Fail");
    return -1;
}
/* End Function:Config_Popup::Virtual_Add ************************************/

/* Function:Config_Popup::State_Set *******************************************
Description : Set the current UI state, and decide what controls are usable.
Input       : ptr_t Type - The state type.
Output      : None.
Return      : None.
******************************************************************************/
void Config_Popup::State_Set(void)
{
    switch(this->Tree->Select_Detail)
    {
        case SELECT_DETAIL_ROOT:
        {
            this->New_Item->Enable(false);
            this->Delete_Item->Enable(false);
            this->Generate->Enable(false);
            this->Validate->Enable(false);
            this->Rename->Enable(true);
            break;
        }
        case SELECT_DETAIL_BASIC:
        case SELECT_DETAIL_MEMORY:
        case SELECT_DETAIL_KERNEL:
        case SELECT_DETAIL_MONITOR:
        {
            this->New_Item->Enable(false);
            this->Delete_Item->Enable(false);
            this->Generate->Enable(true);
            this->Validate->Enable(true);
            this->Rename->Enable(false);
            break;
        }
        case SELECT_DETAIL_NATIVE:
        case SELECT_DETAIL_VIRTUAL:
        {
            this->New_Item->Enable(true);
            this->Delete_Item->Enable(false);
            this->Generate->Enable(false);
            this->Validate->Enable(false);
            this->Rename->Enable(false);
            break;
        }
        case SELECT_DETAIL_NATIVE_CHILD:
        case SELECT_DETAIL_VIRTUAL_CHILD:
        {
            this->New_Item->Enable(true);
            this->Delete_Item->Enable(true);
            this->Generate->Enable(true);
            this->Validate->Enable(true);
            this->Rename->Enable(true);
            break;
        }
        default:
        {
            this->New_Item->Enable(false);
            this->Delete_Item->Enable(false);
            this->Generate->Enable(false);
            this->Validate->Enable(false);
            this->Rename->Enable(false);
            break;
        }
    }
}
/* End Function:Config_Popup::State_Set ****************************************/

/* Function:Config_Popup::On_New_Item *******************************************
Description : wxEVT_MENU handler for 'New Item'.
Input       : class wxCommandEvent& Event - The event.
Output      : None.
Return      : None.
******************************************************************************/
void Config_Popup::On_New_Item(class wxCommandEvent& Event)
{
    std::string Name;

    switch(this->Tree->Select_Detail)
    {
        case SELECT_DETAIL_NATIVE_CHILD:
        case SELECT_DETAIL_NATIVE:
        {
            /* Create a default name */
            Name="Default_Native_Process"+std::to_string(++Main::Native_Cnt);
            /* Rename */
            Main::Name_Dialog->Load(Name);
            /* New name is valid */
            if (Main::Name_Dialog->ShowModal()==wxID_OK)
            {
                Name=Main::Name_Dialog->Name_Get();
                this->Native_Add(Name);
            }
            break;
        }
        case SELECT_DETAIL_VIRTUAL_CHILD:
        case SELECT_DETAIL_VIRTUAL:
        {
            Name="Default_Virtual_Machine"+std::to_string(++Main::Virtual_Cnt);
            Main::Name_Dialog->Load(Name);
            if (Main::Name_Dialog->ShowModal()==wxID_OK)
            {
                Name=Main::Name_Dialog->Name_Get();
                this->Virtual_Add(Name);
            }
            break;
        }
        default:
        {

            break;
        }
    }
}
/* End Function:Config_Popup::On_New_File ************************************/

/* Function:Config_Popup::On_Delete_Item **************************************
Description : wxEVT_MENU handler for 'Delete Item'.
Input       : class wxCommandEvent& Event - The event.
Output      : None.
Return      : None.
******************************************************************************/
void Config_Popup::On_Delete_Item(class wxCommandEvent& Event)
{
    std::string Select_Text;
    Select_Text=this->Tree->GetItemText(this->Tree->Active);
    switch(this->Tree->Select_Detail)
    {
        case SELECT_DETAIL_NATIVE_CHILD:
        {
            if(Main::Proj_Info->Native_Remove(Select_Text)==0)
            {
                this->Tree->Delete(this->Tree->Active);
                if(Main::Option_Panel->Native_Notebook->IsShown())
                {
                    Main::Option_Panel->Window_Switch(Main::Option_Panel->Basic_Panel);
                    this->Tree->Bold=this->Tree->Basic_Config;
                    this->Tree->SetItemBold(this->Tree->Bold);
                }
            }
            break;
        }
        case SELECT_DETAIL_VIRTUAL_CHILD:
        {
            if(Main::Proj_Info->Virtual_Remove(Select_Text)==0)
            {
                this->Tree->Delete(this->Tree->Active);
                if(Main::Option_Panel->Virtual_Notebook->IsShown())
                {
                    Main::Option_Panel->Window_Switch(Main::Option_Panel->Basic_Panel);
                    this->Tree->Bold=this->Tree->Basic_Config;
                    this->Tree->SetItemBold(this->Tree->Bold);
                }
            }
            break;
        }
        default:break;
    }
}
/* End Function:Config_Popup::On_Delete_Item *********************************/

/* Function:Config_Popup::On_Generate *****************************************
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

/* Function:Config_Popup::On_Validate *****************************************
Description : wxEVT_MENU handler for 'Validate'.
Input       : class wxCommandEvent& Event - The event.
Output      : None.
Return      : None.
******************************************************************************/
void Config_Popup::On_Validate(class wxCommandEvent& Event)
{
    /* TODO */
}
/* End Function:Config_Popup::On_Validate ************************************/

/* Function:Config_Popup::On_Rename *******************************************
Description : wxEVT_MENU handler for 'Validate'.
Input       : class wxCommandEvent& Event - The event.
Output      : None.
Return      : None.
******************************************************************************/
void Config_Popup::On_Rename(class wxCommandEvent& Event)
{
    std::string Current;
    std::string Original;

    Original=this->Tree->GetItemText(this->Tree->Active);

    Main::Name_Dialog->Load(Original);
    if(Main::Name_Dialog->ShowModal()!=wxID_OK)
        return;

    /* Name must be OK at this point */
    Current=Main::Name_Dialog->Name_Get();
    switch(this->Tree->Select_Detail)
    {
        case SELECT_DETAIL_ROOT:
        {
            this->Tree->SetItemText(this->Tree->Active,Current);
            Main::Option_Panel->Basic_Panel->Name->SetValue(Current);
            break;
        }
        case SELECT_DETAIL_NATIVE_CHILD:
        {
            Main::Native_Rename(Original,Current);
            wxLogDebug("Config_Popup::On_Rename: %s -> %s, Native Process",Original,Current);
            break;
        }
        case SELECT_DETAIL_VIRTUAL_CHILD:
        {
            Main::Virtual_Rename(Original,Current);
            wxLogDebug("Config_Popup::On_Rename: %s -> %s, Virtual Machine",Original,Current);
            break;
        }
        default:break;
    }
}
/* End Function:Config_Popup::On_Rename **************************************/
}
/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
