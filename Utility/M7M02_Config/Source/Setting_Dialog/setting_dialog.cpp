/******************************************************************************
Filename    : setting dialog.cpp
Author      : lbc
Date        : 22/04/2025
License     : Proprietary; confidential.
Description : Setting dialog class implementation.
******************************************************************************/

/* Include *******************************************************************/
#define __HDR_DEF__
#include "rvm_cfg.hpp"
#undef __HDR_DEF__

#include "wx/wx.h"
#include "wx/xml/xml.h"
#include "wx/stdpaths.h"
#include "wx/filename.h"
#include "wx/validate.h"
#include "wx/valnum.h"
#include "wx/filepicker.h"

#include "map"
#include "string"
#include "memory"

#define __HDR_DEF__
#include "rvm_cfg.hpp"
#include "Setting_Dialog/setting_dialog.hpp"
#undef __HDR_DEF__

#define __HDR_CLASS__
#include "rvm_cfg.hpp"
#include "Setting_Dialog/setting_dialog.hpp"
#undef __HDR_CLASS__
/* End Include ***************************************************************/
namespace RVM_CFG
{
/* Function:Setting_Dialog::Setting_Dialog ************************************
Description : Default constructor for setting dialog.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
/* void */ Setting_Dialog::Setting_Dialog(class wxWindow* Parent):
wxDialog(Parent,wxID_ANY,_("Settings"),wxDefaultPosition,I2P(wxSize(512,240)))
{
    this->Center();

    try
    {
        /* close event */
        this->Bind(wxEVT_CLOSE_WINDOW, &Setting_Dialog::On_Close, this);

        /* GUI */
        this->Main_Sizer=new class wxBoxSizer(wxVERTICAL);
        this->RME_Sizer=new class wxBoxSizer(wxHORIZONTAL);
        this->RVM_Sizer=new class wxBoxSizer(wxHORIZONTAL);
        this->RMP_Sizer=new class wxBoxSizer(wxHORIZONTAL);
        this->Button_Sizer=new class wxBoxSizer(wxHORIZONTAL);

        this->Required_Sizer=new class wxStaticBoxSizer(wxVERTICAL,this,_("Essential Folders"));

        this->RME_Label=new class wxStaticText(this,wxID_ANY,_("RME Folder"));
        this->RME_Picker=new class wxDirPickerCtrl(this,wxID_ANY,wxT(""),_("Select RME Folder"),
                                                   wxDefaultPosition,I2P(wxSize(425,-1)),wxDIRP_USE_TEXTCTRL|wxDIRP_DIR_MUST_EXIST|wxDIRP_SMALL);
        this->RME_Picker->GetTextCtrl()->SetEditable(false);
        this->Bind(wxEVT_DIRPICKER_CHANGED,&Setting_Dialog::On_RME_Picker,this,this->RME_Picker->GetId());
        this->RME_Sizer->Add(this->RME_Label,1,wxALL,I2P(5));
        this->RME_Sizer->Add(this->RME_Picker,4,wxALL,I2P(5));
        this->Required_Sizer->Add(this->RME_Sizer,0,wxEXPAND);

        this->RVM_Label=new class wxStaticText(this,wxID_ANY,_("RVM Folder"));
        this->RVM_Picker=new class wxDirPickerCtrl(this,wxID_ANY,wxT(""),_("Select RVM Folder"),
                                                   wxDefaultPosition,I2P(wxSize(425,-1)),wxDIRP_USE_TEXTCTRL|wxDIRP_DIR_MUST_EXIST|wxDIRP_SMALL);
        this->RVM_Picker->GetTextCtrl()->SetEditable(false);
        this->Bind(wxEVT_DIRPICKER_CHANGED,&Setting_Dialog::On_RVM_Picker,this,this->RVM_Picker->GetId());
        this->RVM_Sizer->Add(this->RVM_Label,1,wxALL,I2P(5));
        this->RVM_Sizer->Add(this->RVM_Picker,4,wxALL,I2P(5));
        this->Required_Sizer->Add(this->RVM_Sizer,0,wxEXPAND);

        this->Optional_Sizer=new class wxStaticBoxSizer(wxVERTICAL,this,_("Optional Folders"));
        this->RMP_Label=new class wxStaticText(this,wxID_ANY,_("RMP Folder"));
        this->RMP_Picker=new class wxDirPickerCtrl(this,wxID_ANY,wxT(""),_("Select RMP Folder"),
                                                   wxDefaultPosition,I2P(wxSize(425,-1)),wxDIRP_USE_TEXTCTRL|wxDIRP_DIR_MUST_EXIST|wxDIRP_SMALL);
        this->RMP_Picker->GetTextCtrl()->SetEditable(false);
        this->Bind(wxEVT_DIRPICKER_CHANGED,&Setting_Dialog::On_RVM_Picker,this,this->RME_Picker->GetId());
        this->RMP_Sizer->Add(this->RMP_Label,1,wxALL,I2P(5));
        this->RMP_Sizer->Add(this->RMP_Picker,4,wxALL,I2P(5));
        this->Optional_Sizer->Add(this->RMP_Sizer,0,wxEXPAND);

        this->Confirm=new class wxButton(this,wxID_ANY,_("Confirm"));
        this->Bind(wxEVT_BUTTON,&Setting_Dialog::On_Confirm,this,this->Confirm->GetId());
        this->Cancel=new class wxButton(this,wxID_ANY,_("Cancel"));
        this->Bind(wxEVT_BUTTON,&Setting_Dialog::On_Cancel,this,this->Cancel->GetId());
        this->Restore=new class wxButton(this,wxID_ANY,_("Restore"));
        this->Bind(wxEVT_BUTTON,&Setting_Dialog::On_Restore,this,this->Restore->GetId());
        this->Default=new class wxButton(this,wxID_ANY,_("Default"));
        this->Bind(wxEVT_BUTTON,&Setting_Dialog::On_Default,this,this->Default->GetId());
        this->Button_Sizer->AddStretchSpacer(1);
        this->Button_Sizer->Add(this->Confirm,0,wxALL,I2P(5));
        this->Button_Sizer->AddStretchSpacer(1);
        this->Button_Sizer->Add(this->Cancel,0,wxALL,I2P(5));
        this->Button_Sizer->AddStretchSpacer(1);
        this->Button_Sizer->Add(this->Restore,0,wxALL,I2P(5));
        this->Button_Sizer->AddStretchSpacer(1);
        this->Button_Sizer->Add(this->Default,0,wxALL,I2P(5));
        this->Button_Sizer->AddStretchSpacer(1);

        this->Main_Sizer->Add(this->Required_Sizer, 0, wxEXPAND|wxALL,I2P(5));
        this->Main_Sizer->AddStretchSpacer(1);
        this->Main_Sizer->Add(this->Optional_Sizer, 0, wxEXPAND|wxALL,I2P(5));
        this->Main_Sizer->Add(this->Button_Sizer, 0, wxEXPAND);

        this->SetSizer(this->Main_Sizer);
        this->Layout();
    }
    catch(std::exception& Exc)
    {
        ASSERT(0,Exc.what());
    }
}
/* End Function:Setting_Dialog::Setting_Dialog *******************************/

/* Function:Setting_Dialog::~Setting_Dialog ***********************************
Description : Destructor for setting dialog.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
/* void */ Setting_Dialog::~Setting_Dialog(void)
{

}
/* End Function:Setting_Dialog::~Setting_Dialog ******************************/

/* Function:Setting_Dialog::Load **********************************************
Description : Load path settings.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
void Setting_Dialog::Load(void)
{
    wxLogDebug("Setting_Dialog::Load");

    this->RME_Picker->SetPath(Main::RME_Folder);
    this->RVM_Picker->SetPath(Main::RVM_Folder);
    this->RMP_Picker->SetPath(Main::RMP_Folder);
}
/* End Function:Setting_Dialog::Load *****************************************/

/* Function:Setting_Dialog::On_RME_Picker *************************************
Description : wxEVT_DIRPICKER_CHANGED handler for 'RME_Picker'.
Input       : class wxFileDirPickerEvent& Event - The event.
Output      : None.
Return      : None.
******************************************************************************/
void Setting_Dialog::On_RME_Picker(class wxCommandEvent& Event)
{
    std::string Path;

    wxLogDebug("Setting_Dialog::On_RME_Picker");

    Path=this->RME_Picker->GetPath();
    Main::Path_Relative(PATH_DIR, Main::Exe_Folder, Path);
    this->RME_Picker->SetPath(Path);
}
/* End Function:Setting_Dialog::On_RME_Folder ********************************/

/* Function:Setting_Dialog::On_RVM_Picker *************************************
Description : wxEVT_DIRPICKER_CHANGED handler for 'RVM_Picker'.
Input       : class wxFileDirPickerEvent& Event - The event.
Output      : None.
Return      : None.
******************************************************************************/
void Setting_Dialog::On_RVM_Picker(class wxCommandEvent& Event)
{
    std::string Path;

    wxLogDebug("Setting_Dialog::On_RVM_Picker");

    Path=this->RVM_Picker->GetPath();
    Main::Path_Relative(PATH_DIR, Main::Exe_Folder, Path);
    this->RVM_Picker->SetPath(Path);
}
/* End Function:Setting_Dialog::On_RVM_Picker ********************************/

/* Function:Setting_Dialog::On_RMP_Picker *************************************
Description : wxEVT_DIRPICKER_CHANGED handler for 'RMP_Picker'.
Input       : class wxFileDirPickerEvent& Event - The event.
Output      : None.
Return      : None.
******************************************************************************/
void Setting_Dialog::On_RMP_Picker(class wxCommandEvent& Event)
{
    std::string Path;

    wxLogDebug("Setting_Dialog::On_RMP_Picker");

    Path=this->RMP_Picker->GetPath();
    Main::Path_Relative(PATH_DIR, Main::Exe_Folder, Path);
    this->RMP_Picker->SetPath(Path);
}
/* End Function:Setting_Dialog::On_RMP_Picker ********************************/

/* Function:Setting_Dialog::On_Close ******************************************
Description : Hide setting dialog after 'close' button is clicked.
Input       : class wxCloseEvent& Event - The event..
Output      : None.
Return      : None.
******************************************************************************/
void Setting_Dialog::On_Close(class wxCloseEvent& Event)
{
    this->Hide();
}
/* End Function:Setting_Dialog::On_Close *************************************/

/* Function:Setting_Dialog::On_Confirm ****************************************
Description : wxEVT_BUTTON handler for 'Confirm',
              and set locations of the folders.
              <Setting>
                  <Path>
                      <RME> </RME> stored as relative path to this software.
                      <RVM> </RVM>
                      <RMP> </RMP>
                  </Path>
              </Setting>
              rme location, rvm location, rmp location -> use xml  .rvi
Input       : class wxCommandEvent& Event - The event.
Output      : None.
Return      : None.
******************************************************************************/
void Setting_Dialog::On_Confirm(class wxCommandEvent& Event)
{
    class wxXmlNode* Root;
    std::unique_ptr<class wxXmlDocument> Doc;

    wxLogDebug("Setting_Dialog::On_Confirm");

    /* Root will be delete with Doc after Doc.SetRoot */
    Root=new wxXmlNode(nullptr, wxXML_ELEMENT_NODE, "Setting");
    Doc=std::make_unique<class wxXmlDocument>();
    Doc->SetRoot(Root);

    Main::Text_Save(Root,"RME",std::string(this->RME_Picker->GetPath()));
    Main::Text_Save(Root,"RVM",std::string(this->RVM_Picker->GetPath()));
    Main::Text_Save(Root,"RMP",std::string(this->RMP_Picker->GetPath()));

    /* Save setting.rvi */
    Doc->Save(Main::Exe_Folder+"setting.rvi");

    /* Save the variables in Main */
    Main::RME_Folder=this->RME_Picker->GetPath();
    Main::RVM_Folder=this->RVM_Picker->GetPath();
    Main::RMP_Folder=this->RMP_Picker->GetPath();

    wxLogDebug("RME_Path has been set %s",Main::RME_Folder);
    wxLogDebug("RVM_Path has been set %s",Main::RVM_Folder);
    wxLogDebug("RMP_Path has been set %s",Main::RMP_Folder);

    this->Hide();
}
/* End Function:Setting_Dialog::On_Confirm ***********************************/

/* Function:Setting_Dialog::On_Cancel *****************************************
Description : wxEVT_BUTTON handler for 'Cancel'.
Input       : class wxCommandEvent& Event - The event..
Output      : None.
Return      : None.
******************************************************************************/
void Setting_Dialog::On_Cancel(class wxCommandEvent& Event)
{
    this->Hide();
}
/* End Function:Setting_Dialog::On_Cancel ************************************/

/* Function:Setting_Dialog::On_Restore ****************************************
Description : wxEVT_BUTTON handler for 'Restore'.
Input       : class wxCommandEvent& Event - The event..
Output      : None.
Return      : None.
******************************************************************************/
void Setting_Dialog::On_Restore(class wxCommandEvent& Event)
{
    this->Load();
}
/* End Function:Setting_Dialog::On_Restore ***********************************/

/* Function:Setting_Dialog::On_Default ****************************************
Description : wxEVT_BUTTON handler for 'Default'.
Input       : class wxCommandEvent& Event - The event.
Output      : None.
Return      : None.
******************************************************************************/
void Setting_Dialog::On_Default(class wxCommandEvent& Event)
{
    this->RME_Picker->SetPath("./../../../../M5P01_Prokaron/");
    this->RVM_Picker->SetPath("./../../../../M7M02_Ammonite/");
    this->RMP_Picker->SetPath("./../../../../M7M01_Eukaron/");
}
/* End Function:Setting_Dialog::On_Default ***********************************/
}
/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/

