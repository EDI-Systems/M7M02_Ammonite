/******************************************************************************
Filename    : setting_dialog.cpp
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
wxDialog(Parent,wxID_ANY,_("Settings"),wxDefaultPosition,I2P(wxSize(640,510)))
{
    this->Center();

    try
    {
        this->Main_Sizer=new class wxBoxSizer(wxVERTICAL);

        this->Preference_Sizer=new class wxStaticBoxSizer(wxVERTICAL,this,_("Preferences"));
        this->Preference_Line1_Sizer=new class wxBoxSizer(wxHORIZONTAL);
        this->Generate_Report=new class wxCheckBox(this,wxID_ANY,_("Generate Report"));
        this->Bind(wxEVT_CHECKBOX,&Setting_Dialog::On_Generate_Report,this,this->Generate_Report->GetId());
        this->Open_Report=new class wxCheckBox(this,wxID_ANY,_("Open Report when Complete"));

        this->Preference_Line1_Sizer->Add(this->Generate_Report, 5, wxALL|wxALIGN_CENTER_VERTICAL,I2P(5));
        this->Preference_Line1_Sizer->Add(this->Open_Report, 5, wxALL|wxALIGN_CENTER_VERTICAL,I2P(5));
        this->Preference_Sizer->Add(Preference_Line1_Sizer,0,wxEXPAND);

        this->Core_Component_Sizer=new class wxStaticBoxSizer(wxVERTICAL,this,_("Core Component Folders"));

        this->RME_Sizer=new class wxBoxSizer(wxHORIZONTAL);
        this->RME_Label=new class wxStaticText(this,wxID_ANY,_("RME"));
        this->RME_Picker=new class wxDirPickerCtrl(this,wxID_ANY,wxT(""),_("Select RME Folder"),
                                                   wxDefaultPosition,I2P(wxSize(480,-1)),wxDIRP_USE_TEXTCTRL|wxDIRP_DIR_MUST_EXIST|wxDIRP_SMALL);
        this->RME_Picker->GetTextCtrl()->SetEditable(false);
        this->Bind(wxEVT_DIRPICKER_CHANGED,&Setting_Dialog::On_Dir_Picker,this,this->RME_Picker->GetId());
        this->RME_Sizer->Add(this->RME_Label,2,wxALL|wxALIGN_CENTER_VERTICAL,I2P(5));
        this->RME_Sizer->Add(this->RME_Picker,4,wxALL|wxALIGN_CENTER_VERTICAL,I2P(5));
        this->Core_Component_Sizer->Add(this->RME_Sizer,0,wxEXPAND);

        this->RVM_Sizer=new class wxBoxSizer(wxHORIZONTAL);
        this->RVM_Label=new class wxStaticText(this,wxID_ANY,_("RVM"));
        this->RVM_Picker=new class wxDirPickerCtrl(this,wxID_ANY,wxT(""),_("Select RVM Folder"),
                                                   wxDefaultPosition,I2P(wxSize(480,-1)),wxDIRP_USE_TEXTCTRL|wxDIRP_DIR_MUST_EXIST|wxDIRP_SMALL);
        this->RVM_Picker->GetTextCtrl()->SetEditable(false);
        this->Bind(wxEVT_DIRPICKER_CHANGED,&Setting_Dialog::On_Dir_Picker,this,this->RVM_Picker->GetId());
        this->RVM_Sizer->Add(this->RVM_Label,2,wxALL|wxALIGN_CENTER_VERTICAL,I2P(5));
        this->RVM_Sizer->Add(this->RVM_Picker,4,wxALL|wxALIGN_CENTER_VERTICAL,I2P(5));
        this->Core_Component_Sizer->Add(this->RVM_Sizer,0,wxEXPAND);

        this->Guest_OS_Sizer=new class wxStaticBoxSizer(wxVERTICAL,this,_("Guest RTOS Folders"));
        this->RMP_Sizer=new class wxBoxSizer(wxHORIZONTAL);
        this->RMP_Label=new class wxStaticText(this,wxID_ANY,_("RMP"));
        this->RMP_Picker=new class wxDirPickerCtrl(this,wxID_ANY,wxT(""),_("Select RMP Folder"),
                                                   wxDefaultPosition,I2P(wxSize(480,-1)),wxDIRP_USE_TEXTCTRL|wxDIRP_DIR_MUST_EXIST|wxDIRP_SMALL);
        this->RMP_Picker->GetTextCtrl()->SetEditable(false);
        this->Bind(wxEVT_DIRPICKER_CHANGED,&Setting_Dialog::On_Dir_Picker,this,this->RMP_Picker->GetId());
        this->RMP_Sizer->Add(this->RMP_Label,2,wxALL|wxALIGN_CENTER_VERTICAL,I2P(5));
        this->RMP_Sizer->Add(this->RMP_Picker,4,wxALL|wxALIGN_CENTER_VERTICAL,I2P(5));
        this->Guest_OS_Sizer->Add(this->RMP_Sizer,0,wxEXPAND);

        this->FRT_Sizer=new class wxBoxSizer(wxHORIZONTAL);
        this->FRT_Label=new class wxStaticText(this,wxID_ANY,_("FreeRTOS"));
        this->FRT_Picker=new class wxDirPickerCtrl(this,wxID_ANY,wxT(""),_("Select FreeRTOS Folder"),
                                                   wxDefaultPosition,I2P(wxSize(480,-1)),wxDIRP_USE_TEXTCTRL|wxDIRP_DIR_MUST_EXIST|wxDIRP_SMALL);
        this->FRT_Picker->GetTextCtrl()->SetEditable(false);
        this->Bind(wxEVT_DIRPICKER_CHANGED,&Setting_Dialog::On_Dir_Picker,this,this->FRT_Picker->GetId());
        this->FRT_Sizer->Add(this->FRT_Label,2,wxALL|wxALIGN_CENTER_VERTICAL,I2P(5));
        this->FRT_Sizer->Add(this->FRT_Picker,4,wxALL|wxALIGN_CENTER_VERTICAL,I2P(5));
        this->Guest_OS_Sizer->Add(this->FRT_Sizer,0,wxEXPAND);

        this->RTT_Sizer=new class wxBoxSizer(wxHORIZONTAL);
        this->RTT_Label=new class wxStaticText(this,wxID_ANY,_("RT-Thread"));
        this->RTT_Picker=new class wxDirPickerCtrl(this,wxID_ANY,wxT(""),_("Select RT-Thread Folder"),
                                                   wxDefaultPosition,I2P(wxSize(480,-1)),wxDIRP_USE_TEXTCTRL|wxDIRP_DIR_MUST_EXIST|wxDIRP_SMALL);
        this->RTT_Picker->GetTextCtrl()->SetEditable(false);
        this->Bind(wxEVT_DIRPICKER_CHANGED,&Setting_Dialog::On_Dir_Picker,this,this->RTT_Picker->GetId());
        this->RTT_Sizer->Add(this->RTT_Label,2,wxALL|wxALIGN_CENTER_VERTICAL,I2P(5));
        this->RTT_Sizer->Add(this->RTT_Picker,4,wxALL|wxALIGN_CENTER_VERTICAL,I2P(5));
        this->Guest_OS_Sizer->Add(this->RTT_Sizer,0,wxEXPAND);

        this->UO2_Sizer=new class wxBoxSizer(wxHORIZONTAL);
        this->UO2_Label=new class wxStaticText(this,wxID_ANY,_("uC/OS II"));
        this->UO2_Picker=new class wxDirPickerCtrl(this,wxID_ANY,wxT(""),_("Select uC/OS II Folder"),
                                                   wxDefaultPosition,I2P(wxSize(480,-1)),wxDIRP_USE_TEXTCTRL|wxDIRP_DIR_MUST_EXIST|wxDIRP_SMALL);
        this->UO2_Picker->GetTextCtrl()->SetEditable(false);
        this->Bind(wxEVT_DIRPICKER_CHANGED,&Setting_Dialog::On_Dir_Picker,this,this->UO2_Picker->GetId());
        this->UO2_Sizer->Add(this->UO2_Label,2,wxALL|wxALIGN_CENTER_VERTICAL,I2P(5));
        this->UO2_Sizer->Add(this->UO2_Picker,4,wxALL|wxALIGN_CENTER_VERTICAL,I2P(5));
        this->Guest_OS_Sizer->Add(this->UO2_Sizer,0,wxEXPAND);

        this->UO3_Sizer=new class wxBoxSizer(wxHORIZONTAL);
        this->UO3_Label=new class wxStaticText(this,wxID_ANY,_("uC/OS III"));
        this->UO3_Picker=new class wxDirPickerCtrl(this,wxID_ANY,wxT(""),_("Select uC/OS III Folder"),
                                                   wxDefaultPosition,I2P(wxSize(480,-1)),wxDIRP_USE_TEXTCTRL|wxDIRP_DIR_MUST_EXIST|wxDIRP_SMALL);
        this->UO3_Picker->GetTextCtrl()->SetEditable(false);
        this->Bind(wxEVT_DIRPICKER_CHANGED,&Setting_Dialog::On_Dir_Picker,this,this->UO3_Picker->GetId());
        this->UO3_Sizer->Add(this->UO3_Label,2,wxALL|wxALIGN_CENTER_VERTICAL,I2P(5));
        this->UO3_Sizer->Add(this->UO3_Picker,4,wxALL|wxALIGN_CENTER_VERTICAL,I2P(5));
        this->Guest_OS_Sizer->Add(this->UO3_Sizer,0,wxEXPAND);

        this->Guest_Script_Sizer=new class wxStaticBoxSizer(wxVERTICAL,this,_("Guest Scripting Interpreter Folders"));
        this->MPY_Sizer=new class wxBoxSizer(wxHORIZONTAL);
        this->MPY_Label=new class wxStaticText(this,wxID_ANY,_("MicroPython"));
        this->MPY_Picker=new class wxDirPickerCtrl(this,wxID_ANY,wxT(""),_("Select MicroPython Folder"),
                                                   wxDefaultPosition,I2P(wxSize(480,-1)),wxDIRP_USE_TEXTCTRL|wxDIRP_DIR_MUST_EXIST|wxDIRP_SMALL);
        this->MPY_Picker->GetTextCtrl()->SetEditable(false);
        this->Bind(wxEVT_DIRPICKER_CHANGED,&Setting_Dialog::On_Dir_Picker,this,this->MPY_Picker->GetId());
        this->MPY_Sizer->Add(this->MPY_Label,2,wxALL|wxALIGN_CENTER_VERTICAL,I2P(5));
        this->MPY_Sizer->Add(this->MPY_Picker,4,wxALL|wxALIGN_CENTER_VERTICAL,I2P(5));
        this->Guest_Script_Sizer->Add(this->MPY_Sizer,0,wxEXPAND);

        this->Button_Sizer=new class wxBoxSizer(wxHORIZONTAL);
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

        this->Main_Sizer->Add(this->Preference_Sizer, 0, wxEXPAND|wxALL,I2P(5));
        this->Main_Sizer->AddStretchSpacer(1);
        this->Main_Sizer->Add(this->Core_Component_Sizer, 0, wxEXPAND|wxALL,I2P(5));
        this->Main_Sizer->AddStretchSpacer(1);
        this->Main_Sizer->Add(this->Guest_OS_Sizer, 0, wxEXPAND|wxALL,I2P(5));
        this->Main_Sizer->AddStretchSpacer(1);
        this->Main_Sizer->Add(this->Guest_Script_Sizer, 0, wxEXPAND|wxALL,I2P(5));
        this->Main_Sizer->AddStretchSpacer(1);
        this->Main_Sizer->Add(this->Button_Sizer, 0, wxEXPAND);

        this->Bind(wxEVT_CLOSE_WINDOW, &Setting_Dialog::On_Close, this);

        this->SetSizer(this->Main_Sizer);
        this->Layout();

        this->File_Load();
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
    this->Unbind(wxEVT_CHECKBOX,&Setting_Dialog::On_Generate_Report,this);
    this->Unbind(wxEVT_DIRPICKER_CHANGED,&Setting_Dialog::On_Dir_Picker, this);
    this->Unbind(wxEVT_BUTTON,&Setting_Dialog::On_Confirm, this);
    this->Unbind(wxEVT_BUTTON,&Setting_Dialog::On_Cancel, this);
    this->Unbind(wxEVT_BUTTON,&Setting_Dialog::On_Restore, this);
    this->Unbind(wxEVT_BUTTON,&Setting_Dialog::On_Default, this);
    this->Unbind(wxEVT_CLOSE_WINDOW, &Setting_Dialog::On_Close, this);
}
/* End Function:Setting_Dialog::~Setting_Dialog ******************************/

/* Function:Setting_Dialog::File_Load *****************************************
Description : Load settings from the IDE configuration file. We know very well
              what this configuration file is.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
void Setting_Dialog::File_Load(void)
{
    ptr_t Success;
    class wxString Path;
    class wxXmlNode* Root;
    class wxXmlDocument Doc;

    /* Check if the preferences exist - if not, load defaults */
    Path=Main::Exe_Folder+"setting.rvi";
    wxLogDebug("Setting_Dialog::File_Load - %s", Path);

    Success=1;
    if(wxFileExists(Path))
    {
        if(Doc.Load(Path)!=0)
        {
            Root=Doc.GetRoot();
            try
            {
                Main::Generate_Report=Main::Yesno_Load(Root, "Generate_Report");
                Main::Open_Report=Main::Yesno_Load(Root, "Open_Report");
                Main::RME_Folder=Main::Text_Load(Root, "RME");
                Main::RVM_Folder=Main::Text_Load(Root, "RVM");
                Main::RMP_Folder=Main::Text_Load(Root, "RMP");
                Main::FRT_Folder=Main::Text_Load(Root, "FRT");
                Main::RTT_Folder=Main::Text_Load(Root, "RTT");
                Main::UO2_Folder=Main::Text_Load(Root, "UO2");
                Main::UO3_Folder=Main::Text_Load(Root, "UO3");
                Main::MPY_Folder=Main::Text_Load(Root, "MPY");
            }
            catch(std::exception& Exc)
            {
                wxLogDebug("Setting_Dialog::File_Load - %s",Exc.what());
                Success=0;
            }
        }
    }
    else
        Success=0;

    if(Success==0)
    {
        wxLogDebug("Setting_Dialog::File_Load - file missing or corrupted, loading defaults.");
        Main::Generate_Report=1;
        Main::Open_Report=1;
        Main::RME_Folder="./../../../M7M01_Eukaron/";
        Main::RVM_Folder="./../../../M7M02_Ammonite/";
        Main::RMP_Folder="./../../../M5P01_Prokaron/";
        Main::FRT_Folder="./../../../M7M00_Guest/FreeRTOS/";
        Main::RTT_Folder="./../../../M7M00_Guest/RTThread/";
        Main::UO2_Folder="./../../../M7M00_Guest/uCOSII/";
        Main::UO3_Folder="./../../../M7M00_Guest/uCOSIII/";
        Main::MPY_Folder="./../../../M7M00_Guest/MicroPython/";
        this->File_Save();
    }
}
/* End Function:Setting_Dialog::File_Load ************************************/

/* Function:Setting_Dialog::File_Save *****************************************
Description : Save settings to the IDE configuration file.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
void Setting_Dialog::File_Save(void)
{
    class wxXmlNode* Root;
    std::unique_ptr<class wxXmlDocument> Doc;

    wxLogDebug("Setting_Dialog::File_Save");

    Root=new wxXmlNode(nullptr, wxXML_ELEMENT_NODE, "Setting");
    Doc=std::make_unique<class wxXmlDocument>();
    Doc->SetRoot(Root);

    Main::Yesno_Save(Root, "Generate_Report", Main::Generate_Report);
    Main::Yesno_Save(Root, "Open_Report", Main::Open_Report);
    Main::Text_Save(Root,"RME",Main::RME_Folder);
    Main::Text_Save(Root,"RVM",Main::RVM_Folder);
    Main::Text_Save(Root,"RMP",Main::RMP_Folder);
    Main::Text_Save(Root,"FRT",Main::FRT_Folder);
    Main::Text_Save(Root,"RTT",Main::RTT_Folder);
    Main::Text_Save(Root,"UO2",Main::UO2_Folder);
    Main::Text_Save(Root,"UO3",Main::UO3_Folder);
    Main::Text_Save(Root,"MPY",Main::MPY_Folder);

    /* Save setting.rvi */
    Doc->Save(Main::Exe_Folder+"setting.rvi");
}
/* End Function:Setting_Dialog::File_Save ************************************/

/* Function:Setting_Dialog::Load **********************************************
Description : Load path settings.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
void Setting_Dialog::Load(void)
{
    wxLogDebug("Setting_Dialog::Load");

    this->Generate_Report->SetValue(Main::Generate_Report);
    if(Main::Generate_Report==0)
    {
        this->Open_Report->SetValue(0);
        this->Open_Report->Disable();
    }
    else
    {
        this->Open_Report->SetValue(Main::Open_Report);
        this->Open_Report->Enable();
    }
    this->RME_Picker->SetPath(Main::RME_Folder);
    this->RVM_Picker->SetPath(Main::RVM_Folder);
    this->RMP_Picker->SetPath(Main::RMP_Folder);
    this->FRT_Picker->SetPath(Main::FRT_Folder);
    this->RTT_Picker->SetPath(Main::RTT_Folder);
    this->UO2_Picker->SetPath(Main::UO2_Folder);
    this->UO3_Picker->SetPath(Main::UO3_Folder);
    this->MPY_Picker->SetPath(Main::MPY_Folder);
}
/* End Function:Setting_Dialog::Load *****************************************/

/* Function:Setting_Dialog::Save **********************************************
Description : Save settings to the IDE configuration file.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
void Setting_Dialog::Save(void)
{
    wxLogDebug("Setting_Dialog::Save");

    Main::Generate_Report=this->Generate_Report->GetValue();
    Main::Open_Report=this->Open_Report->GetValue();
    Main::RME_Folder=this->RME_Picker->GetPath();
    Main::RVM_Folder=this->RVM_Picker->GetPath();
    Main::RMP_Folder=this->RMP_Picker->GetPath();
    Main::FRT_Folder=this->FRT_Picker->GetPath();
    Main::RTT_Folder=this->RTT_Picker->GetPath();
    Main::UO2_Folder=this->UO2_Picker->GetPath();
    Main::UO3_Folder=this->UO3_Picker->GetPath();
    Main::MPY_Folder=this->MPY_Picker->GetPath();
}
/* End Function:Setting_Dialog::Save *****************************************/

/* Function:Setting_Dialog::On_Generate_Report ********************************
Description : wxEVT_CHECKBOX handler for 'Generate_Report'.
Input       : class wxCommandEvent& Event - The event.
Output      : None.
Return      : None.
******************************************************************************/
void Setting_Dialog::On_Generate_Report(class wxCommandEvent& Event)
{
    std::string Path;

    if(this->Generate_Report->GetValue()==0)
    {
        wxLogDebug("Setting_Dialog::On_Generate_Report - Open_Report disabled");
        this->Open_Report->SetValue(0);
        this->Open_Report->Disable();
    }
    else
    {
        wxLogDebug("Setting_Dialog::On_Generate_Report - Open_Report enabled");
        this->Open_Report->SetValue(1);
        this->Open_Report->Enable();
    }

}
/* End Function:Setting_Dialog::On_Generate_Report ***************************/

/* Function:Setting_Dialog::On_Dir_Picker *************************************
Description : wxEVT_DIRPICKER_CHANGED handler for all dir pickers.
Input       : class wxFileDirPickerEvent& Event - The event.
Output      : None.
Return      : None.
******************************************************************************/
void Setting_Dialog::On_Dir_Picker(class wxFileDirPickerEvent& Event)
{
    std::string Path;
    class wxDirPickerCtrl* Picker;

    Picker=dynamic_cast<class wxDirPickerCtrl*>(Event.GetEventObject());
    Path=Picker->GetPath();
    Path=Main::Path_Relative(PATH_DIR, Main::Exe_Folder, Path);
    Picker->SetPath(Path);

    wxLogDebug("Setting_Dialog::On_Dir_Picker - %s",Path);
}
/* End Function:Setting_Dialog::On_Dir_Picker ********************************/

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
Description : wxEVT_BUTTON handler for 'Confirm'.
Input       : class wxCommandEvent& Event - The event.
Output      : None.
Return      : None.
******************************************************************************/
void Setting_Dialog::On_Confirm(class wxCommandEvent& Event)
{
    wxLogDebug("Setting_Dialog::On_Confirm");

    this->Save();
    this->File_Save();
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
    this->RME_Picker->SetPath("./../../../M7M01_Eukaron/");
    this->RVM_Picker->SetPath("./../../../M7M02_Ammonite/");
    this->RMP_Picker->SetPath("./../../../M5P01_Prokaron/");
    this->FRT_Picker->SetPath("./../../../M7M00_Guest/FreeRTOS/");
    this->RTT_Picker->SetPath("./../../../M7M00_Guest/RTThread/");
    this->UO2_Picker->SetPath("./../../../M7M00_Guest/uCOSII/");
    this->UO3_Picker->SetPath("./../../../M7M00_Guest/uCOSIII/");
    this->MPY_Picker->SetPath("./../../../M7M00_Guest/MicroPython/");
}
/* End Function:Setting_Dialog::On_Default ***********************************/
}
/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/

