/******************************************************************************
Filename    : modal dialog.cpp
Author      : lbc
Date        : 22/04/2025
License     : Proprietary; confidential.
Description : Modal dialog class implementation.
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
#include "Choose_Dialog/choose_dialog.hpp"
#undef __HDR_DEF__

#define __HDR_CLASS__
#include "rvm_cfg.hpp"
#include "Proj_Info/proj_info.hpp"
#include "Plat_Info/plat_info.hpp"
#include "Chip_Info/chip_info.hpp"
#include "Plat_Info/Config/config.hpp"
#include "Choose_Dialog/choose_dialog.hpp"
//#include "Option_Panel/Basic_Notebook/basic_notebook.hpp"
//#include "Option_Panel/Basic_Notebook/Basic_Panel/basic_panel.hpp"
//#include "Option_Panel/Kernel_Notebook/kernel_notebook.hpp"
//#include "Option_Panel/Kernel_Notebook/Kernel_Panel/kernel_panel.hpp"
//#include "Option_Panel/Monitor_Notebook/monitor_notebook.hpp"
//#include "Option_Panel/Monitor_Notebook/Monitor_Panel/monitor_panel.hpp"
#include "Option_Panel/Native_Notebook/native_notebook.hpp"
#include "Option_Panel/Native_Notebook/Native_Basic_Panel/native_basic_panel.hpp"
#include "Option_Panel/Virtual_Notebook/virtual_notebook.hpp"
#include "Option_Panel/Virtual_Notebook/Virtual_Basic_Panel/virtual_basic_panel.hpp"
#include "Option_Panel/option_panel.hpp"

#include "Option_Panel/Basic_Panel/basic_panel.hpp"
#include "Option_Panel/Kernel_Panel/kernel_panel.hpp"
#include "Option_Panel/Monitor_Panel/monitor_panel.hpp"

#include "Plat_Info/Compatible/compatible.hpp"
#undef __HDR_CLASS__
/* End Include ***************************************************************/
namespace RVM_CFG
{
/* Function:Modal_Dialog::Choose_Dialog ***************************************
Description : Default constructor for choose dialog.
Input       : class wxWindow* Parent - parent window.
Output      : None.
Return      : None.
******************************************************************************/
/* void */ Choose_Dialog::Choose_Dialog(class wxWindow* Parent):
wxDialog(Parent,wxID_ANY,_("Platform & Chip"),wxDefaultPosition,I2P(wxSize(350,190)))
{
    this->Center();

    try
    {

        /* close event */
        this->Bind(wxEVT_CLOSE_WINDOW, &Choose_Dialog::On_Close, this);

        /* GUI */
        this->SetMinSize(I2P(wxSize(512,600)));

        this->Main_Sizer=new class wxBoxSizer(wxVERTICAL);
        this->Plat_Sizer=new class wxBoxSizer(wxHORIZONTAL);
        this->Chip_Sizer=new class wxBoxSizer(wxHORIZONTAL);
        this->Type_Sizer=new class wxBoxSizer(wxHORIZONTAL);
        this->Button_Sizer=new class wxBoxSizer(wxHORIZONTAL);

        this->Plat_And_Chip_Sizer=new class wxStaticBoxSizer(wxVERTICAL,this,_("Choose platform and chip"));
        this->Plat_Label=new class wxStaticText(this,wxID_ANY,_("Platform"));
        this->Plat=new class wxChoice(this, wxID_ANY);
        this->Plat->Bind(wxEVT_CHOICE, &Choose_Dialog::On_Platform_Change, this, this->Plat->GetId());
        this->Plat_Sizer->Add(this->Plat_Label,2,wxALL,I2P(5));
        this->Plat_Sizer->Add(this->Plat,3,wxALL,I2P(5));
        this->Plat_And_Chip_Sizer->Add(this->Plat_Sizer,0,wxEXPAND);

        this->Chip_Label=new class wxStaticText(this,wxID_ANY,_("Chip"));
        this->Chip=new class wxChoice(this, wxID_ANY);
        this->Chip->Bind(wxEVT_CHOICE, &Choose_Dialog::On_Chip_Change, this, this->Chip->GetId());
        this->Chip_Sizer->Add(Chip_Label,2,wxALL,I2P(5));
        this->Chip_Sizer->Add(Chip,3,wxALL,I2P(5));
        this->Plat_And_Chip_Sizer->Add(this->Chip_Sizer,0,wxEXPAND);

        this->Type_Label=new class wxStaticText(this,wxID_ANY,_("Specific Type"));
        this->Type=new class wxChoice(this, wxID_ANY);
        this->Type_Sizer->Add(Type_Label,2,wxALL,I2P(5));
        this->Type_Sizer->Add(Type,3,wxALL,I2P(5));
        this->Plat_And_Chip_Sizer->Add(this->Type_Sizer,0,wxEXPAND);

        this->Confirm=new class wxButton(this,wxID_ANY,_("Confirm"));
        this->Bind(wxEVT_BUTTON,&Choose_Dialog::On_Confirm,this,this->Confirm->GetId());
        this->Cancel=new class wxButton(this,wxID_ANY,_("Cancel"));
        this->Bind(wxEVT_BUTTON,&Choose_Dialog::On_Cancel,this,this->Cancel->GetId());
        this->Button_Sizer->AddStretchSpacer(1);
        this->Button_Sizer->Add(this->Confirm,0,wxALL,I2P(5));
        this->Button_Sizer->AddStretchSpacer(1);
        this->Button_Sizer->Add(this->Cancel,0,wxALL,I2P(5));
        this->Button_Sizer->AddStretchSpacer(1);

        this->Main_Sizer->Add(this->Plat_And_Chip_Sizer,0,wxEXPAND|wxALL,I2P(5));
        this->Main_Sizer->Add(this->Button_Sizer, 0, wxEXPAND);
        this->SetSizer(this->Main_Sizer);
        this->Layout();
    }
    catch(std::exception& Exc)
    {
        ASSERT(0,Exc.what());
    }
}
/* End Function:Choose_Dialog::Choose_Dialog ***********************************/

/* Function:Choose_Dialog::~Choose_Dialog ***************************************
Description : Destructor for choose dialog.
Input       : None.
Output      : None.
Return      : None.
********************************************************************************/
/* void */ Choose_Dialog::~Choose_Dialog(void)
{

}
/* End Function:Choose_Dialog::~Choose_Dialog **********************************/

/* Function:Choose_Dialog::On_Close *********************************************
Description : Hide modal dialog after 'close' button is clicked.
Input       : class wxCloseEvent& Event - The event..
Output      : None.
Return      : None.
********************************************************************************/
void Choose_Dialog::On_Close(class wxCloseEvent& Event)
{
    this->Hide();
}
/* End Function:Choose_Dialog::On_Close ****************************************/

/* Function:Choose_Dialog::On_Confirm *******************************************
Description : Confirm platform and chip.
Input       : class wxCommandEvent& Event - The event.
Output      : None.
Return      : None.
********************************************************************************/
void Choose_Dialog::On_Confirm(class wxCommandEvent& Event)
{
    wxLogDebug("Choose_Dialog::On_Confirm");

    if(this->Plat->GetStringSelection()==""||this->Chip->GetStringSelection()==""||this->Type->GetStringSelection()=="")
    {
        Main::Msgbox_Show(RVM_CFG_App::Main, MSGBOX_ERROR,
                          _("Choose"),
                          _("Check platform and chip."));
        return;
    }
    Main::Platform=this->Plat->GetStringSelection();
    Main::Chip=this->Chip->GetStringSelection();
    Main::Type=this->Type->GetStringSelection();

    /* Create a new Proj_Info */
    Main::Proj_Info=std::make_unique<class Proj_Info>();
    if(this->RVP_File_Path=="")
    {
        Main::Msgbox_Show(RVM_CFG_App::Main, MSGBOX_ERROR,
                          _("Choose"),
                          _("The new file can not be created on an invalid path."));
        return;
    }
    Main::Proj_Info->Create(this->RVP_File_Path,Main::Plat_Info.get(),Main::Chip_Info.get(),Main::Platform,Main::Chip,Main::Type);

    Main::GUI_Update();

    wxLogDebug("Choose_Dialog::On_Confirm: Show Basic Notebook");
    this->Hide();

}
/* End Function:Choose_Dialog::On_Confirm **************************************/

/* Function:Choose_Dialog::On_Cancel ********************************************
Description : Hide this dialog.
Input       : class wxCommandEvent& Event - The event.
Output      : None.
Return      : None.
********************************************************************************/
void Choose_Dialog::On_Cancel(class wxCommandEvent& Event)
{
    this->Hide();
}
/* End Function:Choose_Dialog::On_Cancel ***************************************/

/* Function:Choose_Dialog::On_Platform_Change ***********************************
Description : Get chips which belongs to a certain platform.
Input       : class wxCommandEvent& Event - The event.
Output      : None.
Return      : None.
********************************************************************************/
void Choose_Dialog::On_Platform_Change(class wxCommandEvent& Event)
{
    std::vector<std::string> Chip_Vec;
    std::vector<std::string> Type_Vec;

    Chip_Vec=Main::Plat_Load(this->Plat->GetStringSelection().ToStdString());
    this->Chip->Clear();
    for(std::string&Chip_Str : Chip_Vec)
        this->Chip->Append(Chip_Str);
    if(this->Chip->GetCount()>0)
        this->Chip->SetSelection(0);

    Type_Vec=Main::Chip_Load(this->Plat->GetStringSelection().ToStdString(),this->Chip->GetStringSelection().ToStdString());
    this->Type->Clear();
    for(std::string&Type_Str : Type_Vec)
        this->Type->Append(Type_Str);
    if(this->Type->GetCount()>0)
        this->Type->SetSelection(0);
}
/* End Function:Choose_Dialog::On_Platform_Change *******************************/

/* Function:Choose_Dialog::On_Chip_Change ****************************************
Description : Get specific type of chip which belongs to a certain chip.
Input       : class wxCommandEvent& Event - The event.
Output      : None.
Return      : None.
*********************************************************************************/
void Choose_Dialog::On_Chip_Change(class wxCommandEvent& Event)
{
    std::vector<std::string> Type_Vec;

    Type_Vec=Main::Chip_Load(this->Plat->GetStringSelection().ToStdString(),this->Chip->GetStringSelection().ToStdString());
    this->Type->Clear();
    for(std::string&Type : Type_Vec)
        this->Type->Append(Type);
    if(this->Type->GetCount()>0)
        this->Type->SetSelection(0);
}
/* End Function:Choose_Dialog::On_Chip_Change **********************************/

/* Function:Choose_Dialog::Load *************************************************
Description : Load information of platform and chip.
Input       : None.
Output      : None.
Return      : None.
********************************************************************************/
void Choose_Dialog::Load(void)
{
    std::string Abs_Path;
    std::string Chip_Selection;
    std::string Plat_Selection;
    std::vector<std::string> Plats;
    std::vector<std::string> Chips;
    std::string Chip_Selection_Lower;
    std::string Plat_Selection_Lower;
    //std::vector<std::string>::iterator Iter;

    wxLogDebug("Choose_Dialog::Refresh_Info");

    /* If there are information about platform and chip , use them first*/
    if(Main::Platform!=""&&Main::Chip!=""&&Main::Type!="")
    {
        this->Plat->SetStringSelection(Main::Platform);
        this->Chip->SetStringSelection(Main::Chip);
        this->Type->SetStringSelection(Main::Type);
        return;
    }

    /* Default platform and chip*/
    /* Clear */
    this->Plat->Clear();
    this->Chip->Clear();
    this->Type->Clear();

    if(Main::RVM_Path=="")
    {
        Main::Msgbox_Show(RVM_CFG_App::Main, MSGBOX_ERROR,
                          _("Choose"),
                          _("Set essential paths first."));
        return;
    }

    /* Refresh the platform & chip information */
    /* Get platforms selections */
    Abs_Path=Main::Path_Absolute(PATH_DIR,
                                wxPathOnly(wxStandardPaths::Get().GetExecutablePath()).ToStdString(),
                                Main::RVM_Path+"Include/Platform");
    Main::Dir_List(Abs_Path,Plats);
    for(std::string&Str : Plats)
        this->Plat->Append(Str);

    /* Default Selection*/
    if(this->Plat->GetCount()>0)
        this->Plat->SetSelection(0);

    /* Try to get .rva */
    Plat_Selection=this->Plat->GetStringSelection();
    Plat_Selection_Lower=this->Plat->GetStringSelection().Lower();
    Abs_Path=Abs_Path+Plat_Selection;
    wxLogDebug(wxString("/rvm_platform_"+Plat_Selection_Lower+".rva"));
    Main::Plat_Info=std::make_unique<class Plat_Info>(Abs_Path+"/rvm_platform_"+Plat_Selection_Lower+".rva");

    /*Valid Path */
    if(wxFileName::DirExists(Abs_Path))
    {
        /* Get chips selections */
        Abs_Path=Abs_Path+"/Chip/";
        Main::Dir_List(Abs_Path,Chips);
        for(std::string&Str : Chips)
            this->Chip->Append(Str);
        /* Default Selection*/
        if(this->Chip->GetCount()>0)
            this->Chip->SetSelection(0);

        /* Try to get .rvc */
        Chip_Selection=this->Chip->GetStringSelection();
        Chip_Selection_Lower=this->Chip->GetStringSelection().Lower();
        Abs_Path=Abs_Path+Chip_Selection+"/rvm_platform_"+Chip_Selection_Lower+".rvc";
        wxLogDebug(wxString(Abs_Path));
        Main::Chip_Info=std::make_unique<class Chip_Info>(Abs_Path);

        /* Set specific type of chip */
        for(std::string&Str : Main::Chip_Info->Compatible)
            this->Type->Append(Str);
        /* Set default type */
        if(this->Type->GetCount()>0)
            this->Type->SetSelection(0);
    }
}
/* End Function:Choose_Dialog::Load *******************************************/

}
/* End Of File ****************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ************************/

