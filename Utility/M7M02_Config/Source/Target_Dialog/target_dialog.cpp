/******************************************************************************
Filename    : target_dialog.cpp
Author      : lbc
Date        : 22/04/2025
License     : Proprietary; confidential.
Description : Target dialog class implementation.
******************************************************************************/

/* Include *******************************************************************/
#define __HDR_DEF__
#include "rvm_cfg.hpp"
#undef __HDR_DEF__

#include "wx/wx.h"
#include "wx/xml/xml.h"
#include <wx/stdpaths.h>
#include <wx/filename.h>
#include "wx/notebook.h"

#include "map"
#include "string"
#include "memory"

#define __HDR_DEF__
#include "Target_Dialog/target_dialog.hpp"
#undef __HDR_DEF__

#define __HDR_CLASS__
#include "rvm_cfg.hpp"
#include "Proj_Info/proj_info.hpp"
#include "Plat_Info/plat_info.hpp"
#include "Chip_Info/chip_info.hpp"
#include "Plat_Info/Config/config.hpp"
#include "Target_Dialog/target_dialog.hpp"
#include "Option_Panel/Native_Notebook/native_notebook.hpp"
#include "Option_Panel/Native_Notebook/Native_Panel/native_panel.hpp"
#include "Option_Panel/Virtual_Notebook/virtual_notebook.hpp"
#include "Option_Panel/Virtual_Notebook/Virtual_Panel/virtual_panel.hpp"
#include "Option_Panel/option_panel.hpp"

#include "Option_Panel/Basic_Panel/basic_panel.hpp"
#include "Option_Panel/Kernel_Panel/kernel_panel.hpp"
#include "Option_Panel/Monitor_Panel/monitor_panel.hpp"

#include "Plat_Info/Compatible/compatible.hpp"
#undef __HDR_CLASS__
/* End Include ***************************************************************/
namespace RVM_CFG
{
/* Function:Target_Dialog::Target_Dialog **************************************
Description : Default constructor for choose dialog.
Input       : class wxWindow* Parent - parent window.
Output      : None.
Return      : None.
******************************************************************************/
/* void */ Target_Dialog::Target_Dialog(class wxWindow* Parent):
wxDialog(Parent,wxID_ANY,_("Target Selection"),wxDefaultPosition,I2P(wxSize(350,190)))
{
    this->Center();

    try
    {
        this->SetMinSize(I2P(wxSize(512,600)));

        this->Main_Sizer=new class wxBoxSizer(wxVERTICAL);
        this->Target_Sizer=new class wxStaticBoxSizer(wxVERTICAL,this,_("Target Selection"));

        this->Platform_Sizer=new class wxBoxSizer(wxHORIZONTAL);
        this->Platform_Label=new class wxStaticText(this,wxID_ANY,_("Platform"));
        this->Platform=new class wxChoice(this, wxID_ANY);
        this->Bind(wxEVT_CHOICE, &Target_Dialog::On_Platform, this, this->Platform->GetId());
        this->Platform_Sizer->Add(this->Platform_Label,2,wxALL,I2P(5));
        this->Platform_Sizer->Add(this->Platform,3,wxALL,I2P(5));
        this->Target_Sizer->Add(this->Platform_Sizer,0,wxEXPAND);

        this->Chipclass_Sizer=new class wxBoxSizer(wxHORIZONTAL);
        this->Chipclass_Label=new class wxStaticText(this,wxID_ANY,_("Chip Class"));
        this->Chipclass=new class wxChoice(this, wxID_ANY);
        this->Bind(wxEVT_CHOICE, &Target_Dialog::On_Chipclass, this, this->Chipclass->GetId());
        this->Chipclass_Sizer->Add(this->Chipclass_Label,2,wxALL,I2P(5));
        this->Chipclass_Sizer->Add(this->Chipclass,3,wxALL,I2P(5));
        this->Target_Sizer->Add(this->Chipclass_Sizer,0,wxEXPAND);

        this->Chipname_Sizer=new class wxBoxSizer(wxHORIZONTAL);
        this->Chipname_Label=new class wxStaticText(this,wxID_ANY,_("Chip Name"));
        this->Chipname=new class wxChoice(this, wxID_ANY);
        this->Chipname_Sizer->Add(this->Chipname_Label,2,wxALL,I2P(5));
        this->Chipname_Sizer->Add(this->Chipname,3,wxALL,I2P(5));
        this->Target_Sizer->Add(this->Chipname_Sizer,0,wxEXPAND);

        this->Button_Sizer=new class wxBoxSizer(wxHORIZONTAL);
        this->Confirm=new class wxButton(this,wxID_ANY,_("Confirm"));
        this->Bind(wxEVT_BUTTON,&Target_Dialog::On_Confirm,this,this->Confirm->GetId());
        this->Cancel=new class wxButton(this,wxID_ANY,_("Cancel"));
        this->Bind(wxEVT_BUTTON,&Target_Dialog::On_Cancel,this,this->Cancel->GetId());
        this->Button_Sizer->AddStretchSpacer(1);
        this->Button_Sizer->Add(this->Confirm,0,wxALL,I2P(5));
        this->Button_Sizer->AddStretchSpacer(1);
        this->Button_Sizer->Add(this->Cancel,0,wxALL,I2P(5));
        this->Button_Sizer->AddStretchSpacer(1);

        this->Main_Sizer->Add(this->Target_Sizer,0,wxEXPAND|wxALL,I2P(5));
        this->Main_Sizer->Add(this->Button_Sizer, 0, wxEXPAND);
        this->SetSizer(this->Main_Sizer);
        this->Layout();

        /* close event */
        this->Bind(wxEVT_CLOSE_WINDOW, &Target_Dialog::On_Close, this);
    }
    catch(std::exception& Exc)
    {
        ASSERT(0,Exc.what());
    }
}
/* End Function:Target_Dialog::Target_Dialog *********************************/

/* Function:Target_Dialog::~Target_Dialog *************************************
Description : Destructor for choose dialog.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
/* void */ Target_Dialog::~Target_Dialog(void)
{
    this->Unbind(wxEVT_CLOSE_WINDOW, &Target_Dialog::On_Close, this);
}
/* End Function:Target_Dialog::~Target_Dialog ********************************/

/* Function:Target_Dialog::On_Close *******************************************
Description : Hide modal dialog after 'close' button is clicked.
Input       : class wxCloseEvent& Event - The event..
Output      : None.
Return      : None.
******************************************************************************/
void Target_Dialog::On_Close(class wxCloseEvent& Event)
{
    this->Hide();
}
/* End Function:Target_Dialog::On_Close **************************************/

/* Function:Target_Dialog::On_Confirm *****************************************
Description : Confirm platform and chip.
Input       : class wxCommandEvent& Event - The event.
Output      : None.
Return      : None.
******************************************************************************/
void Target_Dialog::On_Confirm(class wxCommandEvent& Event)
{
    wxLogDebug("Target_Dialog::On_Confirm");

    if((this->Platform->GetStringSelection()=="")||
       (this->Chipclass->GetStringSelection()=="")||
       (this->Chipname->GetStringSelection()==""))
    {
        Main::Msgbox_Show(RVM_CFG_App::Main, MSGBOX_ERROR,
                          _("Target Selection"),
                          _("Please complete target selection."));
        return;
    }

    this->Hide();
}
/* End Function:Target_Dialog::On_Confirm ************************************/

/* Function:Target_Dialog::On_Cancel ******************************************
Description : Hide this dialog.
Input       : class wxCommandEvent& Event - The event.
Output      : None.
Return      : None.
******************************************************************************/
void Target_Dialog::On_Cancel(class wxCommandEvent& Event)
{
    wxLogDebug("Target_Dialog::On_Cancel");

    this->Hide();
}
/* End Function:Target_Dialog::On_Cancel *************************************/

/* Function:Target_Dialog::On_Platform ****************************************
Description : Get chips which belongs to a certain platform.
Input       : class wxCommandEvent& Event - The event.
Output      : None.
Return      : None.
******************************************************************************/
void Target_Dialog::On_Platform(class wxCommandEvent& Event)
{
    std::vector<std::string> Chipclass_List;
    std::vector<std::string> Chipname_List;

    wxLogDebug("Target_Dialog::On_Platform");

    Chipclass_List=Main::Plat_Load(std::string(this->Platform->GetStringSelection()));

    this->Chipclass->Clear();
    for(const std::string& Class:Chipclass_List)
        this->Chipclass->Append(Class);
    this->Chipclass->SetSelection(0);

    Chipname_List=Main::Chip_Load(std::string(this->Platform->GetStringSelection()),
                                 std::string(this->Chipclass->GetStringSelection()));

    this->Chipname->Clear();
    for(const std::string& Name:Chipname_List)
        this->Chipname->Append(Name);
    this->Chipname->SetSelection(0);
}
/* End Function:Target_Dialog::On_Platform ***********************************/

/* Function:Target_Dialog::On_Chipclass ***************************************
Description : Get specific type of chip which belongs to a certain chip.
Input       : class wxCommandEvent& Event - The event.
Output      : None.
Return      : None.
******************************************************************************/
void Target_Dialog::On_Chipclass(class wxCommandEvent& Event)
{
    std::vector<std::string> Chipname_List;

    wxLogDebug("Target_Dialog::On_Chipclass");

    Chipname_List=Main::Chip_Load(std::string(this->Platform->GetStringSelection()),
                                  std::string(this->Chipclass->GetStringSelection()));

    this->Chipname->Clear();
    for(const std::string& Name:Chipname_List)
        this->Chipname->Append(Name);
    this->Chipname->SetSelection(0);
}
/* End Function:Target_Dialog::On_Chipclass **********************************/

/* Function:Target_Dialog::Load ***********************************************
Description : Load information of platform and chip.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
void Target_Dialog::Load(void)
{
    std::string Path;
    std::vector<std::string> Platform_List;
    std::vector<std::string> Chipclass_List;
    std::vector<std::string> Chipname_List;

    wxLogDebug("Target_Dialog::Refresh_Info");

    this->Platform->Clear();
    this->Chipclass->Clear();
    this->Chipname->Clear();
    Path=Main::Path_Absolute(PATH_DIR,Main::Exe_Folder,Main::RVM_Folder+"Include/Platform/");

    /* Get valid platform selections */
    Main::Dir_List(Path,Platform_List);
    for(const std::string& Plat:Platform_List)
        this->Platform->Append(Plat);
    this->Platform->SetSelection(0);

    /* Load platform and get valid chipclass selections */
    Chipclass_List=Main::Plat_Load(std::string(this->Platform->GetStringSelection()));
    for(const std::string& Class:Chipclass_List)
        this->Chipclass->Append(Class);
    this->Chipclass->SetSelection(0);

    /* Load chipclass and get valid chipname selections */
    Chipname_List=Main::Chip_Load(std::string(this->Platform->GetStringSelection()),
                                  std::string(this->Chipclass->GetStringSelection()));

    this->Chipname->Clear();
    for(const std::string& Name:Chipname_List)
        this->Chipname->Append(Name);
    this->Chipname->SetSelection(0);
}
/* End Function:Target_Dialog::Load *******************************************/
}
/* End Of File ****************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ************************/

