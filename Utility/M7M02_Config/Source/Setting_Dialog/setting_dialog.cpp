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
#include <wx/stdpaths.h>
#include <wx/filename.h>

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
wxDialog(Parent,wxID_ANY,_("Settings"),wxDefaultPosition,I2P(wxSize(512,230)))
{
    this->Center();

    try
    {
        /* close event */
        this->Bind(wxEVT_CLOSE_WINDOW, &Setting_Dialog::On_Close, this);

        /* GUI */
        this->SetMinSize(I2P(wxSize(512,600)));

        this->Main_Sizer=new class wxBoxSizer(wxVERTICAL);
        this->RVM_Sizer=new class wxBoxSizer(wxHORIZONTAL);
        this->RME_Sizer=new class wxBoxSizer(wxHORIZONTAL);
        this->RMP_Sizer=new class wxBoxSizer(wxHORIZONTAL);
        this->Button_Sizer=new class wxBoxSizer(wxHORIZONTAL);


        this->Required_Path_Sizer=new class wxStaticBoxSizer(wxVERTICAL,this,_("Essential Path"));
        this->RVM_Label=new class wxStaticText(this,wxID_ANY,_("RVM Path"));
        this->RVM_Text=new class wxTextCtrl(this,wxID_ANY,wxT(""),wxDefaultPosition,wxDefaultSize,wxTE_READONLY);
        this->RVM_Path=new class wxButton(this,wxID_ANY,_("..."),wxDefaultPosition,I2P(wxSize(20,0)));
        this->Bind(wxEVT_BUTTON,&Setting_Dialog::On_RVM_Path,this,this->RVM_Path->GetId());
        this->RVM_Sizer->Add(RVM_Label,1,wxALL,I2P(5));
        this->RVM_Sizer->Add(RVM_Text,4,wxALL,I2P(5));
        this->RVM_Sizer->Add(RVM_Path,0,wxALL|wxEXPAND,I2P(5));
        this->Required_Path_Sizer->Add(this->RVM_Sizer,0,wxEXPAND);

        this->RME_Label=new class wxStaticText(this,wxID_ANY,_("RME Path"));
        this->RME_Text=new class wxTextCtrl(this,wxID_ANY,wxT(""),wxDefaultPosition,wxDefaultSize,wxTE_READONLY);
        this->RME_Path=new class wxButton(this,wxID_ANY,_("..."),wxDefaultPosition,I2P(wxSize(20,0)));
        this->Bind(wxEVT_BUTTON,&Setting_Dialog::On_RME_Path,this,this->RME_Path->GetId());
        this->RME_Sizer->Add(RME_Label,1,wxALL,I2P(5));
        this->RME_Sizer->Add(RME_Text,4,wxALL,I2P(5));
        this->RME_Sizer->Add(RME_Path,0,wxALL|wxEXPAND,I2P(5));
        this->Required_Path_Sizer->Add(this->RME_Sizer,0,wxEXPAND);

        this->Optional_Path_Sizer=new class wxStaticBoxSizer(wxVERTICAL,this,_("Optional Path"));
        this->RMP_Label=new class wxStaticText(this,wxID_ANY,_("RMP Path"));
        this->RMP_Text=new class wxTextCtrl(this,wxID_ANY,wxT(""),wxDefaultPosition,wxDefaultSize,wxTE_READONLY);
        this->RMP_Path=new class wxButton(this,wxID_ANY,_("..."),wxDefaultPosition,I2P(wxSize(20,0)));
        this->Bind(wxEVT_BUTTON,&Setting_Dialog::On_RMP_Path,this,this->RMP_Path->GetId());
        this->RMP_Sizer->Add(RMP_Label,1,wxALL,I2P(5));
        this->RMP_Sizer->Add(RMP_Text,4,wxALL,I2P(5));
        this->RMP_Sizer->Add(RMP_Path,0,wxALL|wxEXPAND,I2P(5));
        this->Optional_Path_Sizer->Add(this->RMP_Sizer,0,wxEXPAND);

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

        this->Main_Sizer->Add(this->Required_Path_Sizer, 0, wxEXPAND|wxALL,I2P(5));
        this->Main_Sizer->AddSpacer(5);
        this->Main_Sizer->Add(this->Optional_Path_Sizer, 0, wxEXPAND|wxALL,I2P(5));
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
              <Settings>
                  <Path>
                      <RME> </RME> stored as relative path to this software.
                      <RVM> </RVM>
                      <RMP> </RMP>
                  </Path>
              </Settings>
              rme location, rvm location, rmp location -> use xml  .rvi
Input       : class wxCommandEvent& Event - The event.
Output      : None.
Return      : None.
******************************************************************************/
void Setting_Dialog::On_Confirm(class wxCommandEvent& Event)
{
    wxLogDebug("Setting_Dialog::On_Confirm");

    wxXmlNode*Root;
    wxXmlDocument Doc;
    wxString Exe_Path;

    if(this->RVM_Text->GetValue()==""||this->RME_Text->GetValue()=="")
    {
        wxLogDebug("ERROR!!!Essential path is empty!!!");
        return;
    }

    /* Root will be delete with Doc after Doc.SetRoot */
    Root=new wxXmlNode(nullptr, wxXML_ELEMENT_NODE, "Setting");
    Main::Text_Save(Root,"RVM",this->RVM_Text->GetValue().ToStdString());
    Main::Text_Save(Root,"RME",this->RME_Text->GetValue().ToStdString());
    Main::Text_Save(Root,"RMP",this->RMP_Text->GetValue().ToStdString());
    Doc.SetRoot(Root);

    Exe_Path=wxPathOnly(wxStandardPaths::Get().GetExecutablePath());

    /* Save setting.rvi */
    if (Doc.Save(Exe_Path+ wxFileName::GetPathSeparator() + "setting.rvi"))
        wxLogDebug("The setting.rvi has been made successfully in %s.",Exe_Path);
    else
        wxLogDebug("Failed to make the setting.rvi");

    /* Save the variables in Main */
    Main::RVM_Path=this->RVM_Text->GetValue();
    Main::RME_Path=this->RME_Text->GetValue();
    Main::RMP_Path=this->RMP_Text->GetValue();

    wxLogDebug("RVM_Path has been set %s",Main::RVM_Path);
    wxLogDebug("RME_Path has been set %s",Main::RME_Path);
    wxLogDebug("RMP_Path has been set %s",Main::RMP_Path);

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
    Main::Setting_Dialog->Set_Setting(Main::RVM_Path, Main::RME_Path, Main::RMP_Path);
}
/* End Function:Setting_Dialog::On_Restore ***********************************/

/* Function:Setting_Dialog::On_Default ****************************************
Description : wxEVT_BUTTON handler for 'Default'.
Input       : class wxCommandEvent& Event - The event..
Output      : None.
Return      : None.
******************************************************************************/
void Setting_Dialog::On_Default(class wxCommandEvent& Event)
{
    this->RVM_Text->SetValue("./../../../../M7M02_Ammonite/");
    this->RME_Text->SetValue("./../../../../M5P01_Prokaron/");
    this->RMP_Text->SetValue("./../../../../M7M01_Eukaron/");
}
/* End Function:Setting_Dialog::On_Default ***********************************/

/* Function:Setting_Dialog::On_RME_Folder *************************************
Description : wxEVT_BUTTON handler for 'RME_Folder'.
Input       : class wxCommandEvent& Event - The event..
Output      : None.
Return      : None.
******************************************************************************/

/* Function:Setting_Dialog::On_RVM_Folder *************************************
Description : wxEVT_BUTTON handler for 'RVM_Folder'.
Input       : class wxCommandEvent& Event - The event..
Output      : None.
Return      : None.
******************************************************************************/
void Setting_Dialog::On_RVM_Path(class wxCommandEvent& Event)
{
    std::string Rel_Path;

    wxLogDebug("Setting_Dialog::On_Folder");

    Rel_Path=this->Rel_Path_Get();
    if(Rel_Path=="")
    {
        Main::Msgbox_Show(RVM_CFG_App::Main, MSGBOX_ERROR,
                          _("Setting"),
                          _("RVM Path is invalid."));
        return;
    }
    this->RVM_Text->SetValue(Rel_Path);
}
/* End Function:Setting_Dialog::On_RVM_Folder ********************************/

/* Function:Setting_Dialog::On_RME_Folder *************************************
Description : wxEVT_BUTTON handler for 'RME_Folder'.
Input       : class wxCommandEvent& Event - The event..
Output      : None.
Return      : None.
******************************************************************************/
void Setting_Dialog::On_RME_Path(class wxCommandEvent& Event)
{
    wxLogDebug("Setting_Dialog::On_Folder");
    std::string Rel_Path;
    Rel_Path=this->Rel_Path_Get();
    if(Rel_Path=="")
    {
        Main::Msgbox_Show(RVM_CFG_App::Main, MSGBOX_ERROR,
                          _("Setting"),
                          _("RME Path is invalid."));
        return;
    }
    this->RME_Text->SetValue(Rel_Path);
}
/* End Function:Setting_Dialog::On_RME_Folder ********************************/

/* Function:Setting_Dialog::On_RMP_Folder *************************************
Description : wxEVT_BUTTON handler for 'RMP_Folder'.
Input       : class wxCommandEvent& Event - The event..
Output      : None.
Return      : None.
******************************************************************************/
void Setting_Dialog::On_RMP_Path(class wxCommandEvent& Event)
{
    std::string Rel_Path;

    wxLogDebug("Setting_Dialog::On_Folder");

    Rel_Path=this->Rel_Path_Get();
    if(Rel_Path=="")
    {
        Main::Msgbox_Show(RVM_CFG_App::Main, MSGBOX_ERROR,
                          _("Setting"),
                          _("RMP Path is invalid."));
        return;
    }
    this->RMP_Text->SetValue(Rel_Path);
}
/* End Function:Setting_Dialog::On_RMP_Folder ********************************/

/* Function:Setting_Dialog::Set_Setting ***************************************
Description : Set rvm, rme, rmp text in the setting dialog.
Input       : const std::string& rvm - The path of rvm,
              const std::string& rme - The path of rme,
              const std::string& rmp - The path of rmp.
Output      : None.
Return      : None.
******************************************************************************/
void Setting_Dialog::Set_Setting(const std::string& rvm,const std::string& rme,
                                 const std::string& rmp)
{
    wxLogDebug("Setting_Dialog::Set_Setting");
    this->RVM_Text->SetValue(rvm);
    this->RME_Text->SetValue(rme);
    this->RMP_Text->SetValue(rmp);
}
/* End Function:Setting_Dialog::On_RMP_Folder ********************************/

/* Function:Setting_Dialog::Rel_Path_Get **************************************
Description : Get the relative path from this program.
Input       : None.
Output      : None.
Return      : std::string - The relative path.
******************************************************************************/
std::string Setting_Dialog::Rel_Path_Get()
{
    wxLogDebug("Setting_Dialog::Folder_Function");

    std::string Path;
    std::string Exe_Path;
    std::unique_ptr<class wxDirDialog>Dir;

    /* Let the user choose the folder */
    Dir=std::make_unique<class wxDirDialog>(this,_(""),wxT(""),
                                            wxDD_DEFAULT_STYLE | wxDD_DIR_MUST_EXIST);
    if(Dir->ShowModal()!=wxID_OK)
        return "";
    Path=Dir->GetPath();

    /* Get the location of this executable program */
    Exe_Path = wxPathOnly(wxStandardPaths::Get().GetExecutablePath()).ToStdString();

    /* Return relative path */
    return Main::Path_Relative(PATH_DIR, Exe_Path, Path);
}
/* End Function:Setting_Dialog::Rel_Path_Get *********************************/
}
/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/

