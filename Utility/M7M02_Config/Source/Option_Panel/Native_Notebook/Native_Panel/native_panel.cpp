/******************************************************************************
Filename    : native_panel.cpp
Author      : lbc
Date        : 25/04/2025
License     : Proprietary; confidential.
Description : First basic information of native process panel implementation.
******************************************************************************/

/* Include *******************************************************************/
#define __HDR_DEF__
#include "rvm_cfg.hpp"
#undef __HDR_DEF__

#include "wx/wx.h"
#include "wx/notebook.h"
#include "wx/xml/xml.h"
#include "wx/dnd.h"
#include "wx/treectrl.h"

#include "set"
#include "map"
#include "memory"

#define __HDR_DEF__
#include "Option_Panel/Native_Notebook/Native_Panel/native_panel.hpp"
#undef __HDR_DEF__

#define __HDR_CLASS__
#include "rvm_cfg.hpp"
#include "Proj_Info/proj_info.hpp"
#include "Proj_Info/Chip/chip.hpp"
#include "Proj_Info/Process/process.hpp"
#include "Proj_Info/Process/Native/native.hpp"
#include "Proj_Info/Process/Virtual/virtual.hpp"
#include "Plat_Info/plat_info.hpp"
#include "Plat_Info/Compatible/compatible.hpp"
#include "Chip_Info/chip_info.hpp"
#include "Option_Panel/Native_Notebook/native_notebook.hpp"
#include "Option_Panel/Native_Notebook/Native_Panel/native_panel.hpp"
#include "Config_Tree/config_tree.hpp"
#include "Config_Tree/Config_Popup/config_popup.hpp"
#include "Name_Dialog/name_dialog.hpp"
#undef __HDR_CLASS__
/* End Include ***************************************************************/
namespace RVM_CFG
{
/* Function:Native_Panel::Native_Panel ****************************************
Description : Constructor for basic notebook.
Input       : class wxWindow* Parent - The parent window.
Output      : None.
Return      : None.
******************************************************************************/
/* void */ Native_Panel::Native_Panel(class wxWindow* Parent):
wxPanel(Parent,wxID_ANY)
{
    try
    {
        this->SetBackgroundColour(Parent->GetBackgroundColour());

        this->Main_Sizer=new class wxBoxSizer(wxVERTICAL);

        /* Basic options */
        this->Basic_Sizer=new class wxStaticBoxSizer(wxVERTICAL,this,_("Basic Options"));
        this->Basic_Line1_Sizer=new class wxBoxSizer(wxHORIZONTAL);

        this->Name_Sizer=new class wxBoxSizer(wxHORIZONTAL);
        this->Name_Label=new class wxStaticText(this,wxID_ANY,_("Name"));
        this->Name=new class wxTextCtrl(this,wxID_ANY,wxEmptyString,wxDefaultPosition,wxDefaultSize,wxTE_READONLY);
        this->Name_Sizer->Add(this->Name_Label,2,wxALL|wxALIGN_CENTER_VERTICAL,I2P(5));
        this->Name_Sizer->Add(this->Name,3,wxALL|wxALIGN_CENTER_VERTICAL,I2P(5));
        this->Name->Bind(wxEVT_LEFT_DOWN, &Native_Panel::On_Name, this);

        this->Extra_Captbl_Sizer=new class wxBoxSizer(wxHORIZONTAL);
        this->Extra_Captbl_Label=new class wxStaticText(this,wxID_ANY,_("Extra Capability Slot Number"));
        this->Extra_Captbl=new class wxTextCtrl(this,wxID_ANY);
        this->Extra_Captbl_Sizer->Add(this->Extra_Captbl_Label,2,wxALL|wxALIGN_CENTER_VERTICAL,I2P(5));
        this->Extra_Captbl_Sizer->Add(this->Extra_Captbl,3,wxALL|wxALIGN_CENTER_VERTICAL,I2P(5));

        this->Coprocessor_Sizer=new class wxBoxSizer(wxHORIZONTAL);

        this->Basic_Line1_Sizer->Add(this->Name_Sizer,6,wxEXPAND);
        this->Basic_Line1_Sizer->AddStretchSpacer(1);
        this->Basic_Line1_Sizer->Add(this->Extra_Captbl_Sizer,6,wxEXPAND);

        this->Basic_Sizer->Add(this->Basic_Line1_Sizer,0,wxEXPAND);
        this->Basic_Sizer->Add(this->Coprocessor_Sizer,0,wxEXPAND);

        /* Buildsystem options */
        this->Build_Sizer=new class wxStaticBoxSizer(wxVERTICAL,this,_("Buildsystem Options"));
        this->Build_Line1_Sizer=new class wxBoxSizer(wxHORIZONTAL);
        this->Build_Line2_Sizer=new class wxBoxSizer(wxHORIZONTAL);

        this->Toolchain_Sizer=new class wxBoxSizer(wxHORIZONTAL);
        this->Toolchain_Label=new class wxStaticText(this,wxID_ANY,_("Toolchain"));
        this->Toolchain=new class wxChoice(this,wxID_ANY);
        this->Toolchain_Sizer->Add(this->Toolchain_Label,2,wxALL|wxALIGN_CENTER_VERTICAL,I2P(5));
        this->Toolchain_Sizer->Add(this->Toolchain,3,wxALL|wxALIGN_CENTER_VERTICAL,I2P(5));
        this->Bind(wxEVT_CHOICE, &Native_Panel::On_Toolchain, this, this->Toolchain->GetId());

        this->Buildsystem_Sizer=new class wxBoxSizer(wxHORIZONTAL);
        this->Buildsystem_Label=new class wxStaticText(this,wxID_ANY,_("Buildsystem"));
        this->Buildsystem=new class wxChoice(this,wxID_ANY);
        this->Buildsystem_Sizer->Add(this->Buildsystem_Label,2,wxALL|wxALIGN_CENTER_VERTICAL,I2P(5));
        this->Buildsystem_Sizer->Add(this->Buildsystem,3,wxALL|wxALIGN_CENTER_VERTICAL,I2P(5));

        this->Optimization_Sizer=new class wxBoxSizer(wxHORIZONTAL);
        this->Optimization_Label=new class wxStaticText(this,wxID_ANY,_("Optimization"));
        this->Optimization=new class wxChoice(this,wxID_ANY);
        this->Optimization->Append("-O0");
        this->Optimization->Append("-O1");
        this->Optimization->Append("-O2");
        this->Optimization->Append("-O3");
        this->Optimization->Append("-Of");
        this->Optimization->Append("-Os");
        this->Optimization->SetSelection(0);
        this->Optimization_Sizer->Add(this->Optimization_Label,2,wxALL|wxALIGN_CENTER_VERTICAL,I2P(5));
        this->Optimization_Sizer->Add(this->Optimization,3,wxALL|wxALIGN_CENTER_VERTICAL,I2P(5));

        this->Build_Line1_Sizer->Add(this->Toolchain_Sizer,6,wxEXPAND);
        this->Build_Line1_Sizer->AddStretchSpacer(1);
        this->Build_Line1_Sizer->Add(this->Buildsystem_Sizer,6,wxEXPAND);

        this->Build_Line2_Sizer->Add(this->Optimization_Sizer,6,wxEXPAND);
        this->Build_Line2_Sizer->AddStretchSpacer(7);

        this->Build_Sizer->Add(this->Build_Line1_Sizer,0,wxEXPAND);
        this->Build_Sizer->Add(this->Build_Line2_Sizer,0,wxEXPAND);

        /* Output options */
        this->Output_Sizer=new class wxStaticBoxSizer(wxVERTICAL,this,_("Output Options"));

        this->Project_Sizer=new class wxBoxSizer(wxHORIZONTAL);
        this->Project_Output_Label=new class wxStaticText(this,wxID_ANY,_("Project Output"));
        this->Project_Output=new class wxTextCtrl(this,wxID_ANY,_("./Default_Native_Process_Name/Project/"));
        this->Project_Overwrite=new class wxCheckBox(this,wxID_ANY,_("Overwrite"));
        this->Project_Sizer->Add(this->Project_Output_Label,5,wxALL|wxALIGN_CENTER_VERTICAL,I2P(5));
        this->Project_Sizer->Add(this->Project_Output,16,wxALL|wxALIGN_CENTER_VERTICAL,I2P(5));
        this->Project_Sizer->Add(this->Project_Overwrite,3,wxALL|wxALIGN_CENTER_VERTICAL,I2P(5));

        this->Linker_Output_Sizer=new class wxBoxSizer(wxHORIZONTAL);
        this->Linker_Output_Label=new class wxStaticText(this,wxID_ANY,_("Linker Output"));
        this->Linker_Output=new class wxTextCtrl(this,wxID_ANY);
        this->Linker_Output_Sizer->Add(this->Linker_Output_Label,5,wxALL|wxALIGN_CENTER_VERTICAL,I2P(5));
        this->Linker_Output_Sizer->Add(this->Linker_Output,16,wxALL|wxALIGN_CENTER_VERTICAL,I2P(5));
        this->Linker_Output_Sizer->AddStretchSpacer(3);

        this->Main_Header_Output_Sizer=new class wxBoxSizer(wxHORIZONTAL);
        this->Main_Header_Output_Label=new class wxStaticText(this,wxID_ANY,_("Main Header Output"));
        this->Main_Header_Output=new class wxTextCtrl(this,wxID_ANY);
        this->Main_Header_Output_Sizer->Add(this->Main_Header_Output_Label,5,wxALL|wxALIGN_CENTER_VERTICAL,I2P(5));
        this->Main_Header_Output_Sizer->Add(this->Main_Header_Output,16,wxALL|wxALIGN_CENTER_VERTICAL,I2P(5));
        this->Main_Header_Output_Sizer->AddStretchSpacer(3);

        this->Main_Source_Output_Sizer=new class wxBoxSizer(wxHORIZONTAL);
        this->Main_Source_Output_Label=new class wxStaticText(this,wxID_ANY,_("Main Source Output"));
        this->Main_Source_Output=new class wxTextCtrl(this,wxID_ANY);
        this->Main_Source_Output_Sizer->Add(this->Main_Source_Output_Label,5,wxALL|wxALIGN_CENTER_VERTICAL,I2P(5));
        this->Main_Source_Output_Sizer->Add(this->Main_Source_Output,16,wxALL|wxALIGN_CENTER_VERTICAL,I2P(5));
        this->Main_Source_Output_Sizer->AddStretchSpacer(3);

        this->Entry_Source_Sizer=new class wxBoxSizer(wxHORIZONTAL);
        this->Entry_Source_Output_Label=new class wxStaticText(this,wxID_ANY,_("Entry Source Output"));
        this->Entry_Source_Output=new class wxTextCtrl(this,wxID_ANY,_("../Source/"));
        this->Entry_Source_Overwrite=new class wxCheckBox(this,wxID_ANY,_("Overwrite"));
        this->Entry_Source_Sizer->Add(this->Entry_Source_Output_Label,5,wxALL|wxALIGN_CENTER_VERTICAL,I2P(5));
        this->Entry_Source_Sizer->Add(this->Entry_Source_Output,16,wxALL|wxALIGN_CENTER_VERTICAL,I2P(5));
        this->Entry_Source_Sizer->Add(this->Entry_Source_Overwrite,3,wxALL|wxALIGN_CENTER_VERTICAL,I2P(5));

        this->Output_Sizer->Add(this->Project_Sizer,0,wxEXPAND);
        this->Output_Sizer->Add(this->Linker_Output_Sizer,0,wxEXPAND);
        this->Output_Sizer->Add(this->Main_Header_Output_Sizer,0,wxEXPAND);
        this->Output_Sizer->Add(this->Main_Source_Output_Sizer,0,wxEXPAND);
        this->Output_Sizer->Add(this->Entry_Source_Sizer,0,wxEXPAND);

        this->Main_Sizer->Add(this->Basic_Sizer,0,wxEXPAND|wxALL,I2P(5));
        this->Main_Sizer->Add(this->Build_Sizer,0,wxEXPAND|wxALL,I2P(5));
        this->Main_Sizer->Add(this->Output_Sizer,0,wxEXPAND|wxALL,I2P(5));

        this->SetSizer(this->Main_Sizer);
        this->Layout();
    }
    catch(std::exception& Exc)
    {
        ASSERT(0,Exc.what());
    }
}
/* End Function:Native_Panel::Native_Panel ***********************************/

/* Function:Native_Panel::~Native_Panel ***************************************
Description : Destructor for sheet notebook.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
/* void */ Native_Panel::~Native_Panel(void)
{
    /* Have to bind to the text itself because we are processing window events on it directly */
    this->Name->Unbind(wxEVT_LEFT_DOWN, &Native_Panel::On_Name, this);
    this->Unbind(wxEVT_CHOICE, &Native_Panel::On_Toolchain, this, this->Toolchain->GetId());
}
/* End Function:Native_Panel::Native_Panel ***********************************/

/* Function:Native_Panel::Rename **********************************************
Description : Carry out the rename on the panel in case it is shown now.
Input       : const std::string& Original - The original name of native process.
              const std::string& Current - The current name of native process.
Output      : None.
Return      : None.
******************************************************************************/
void Native_Panel::Rename(const std::string& Original,const std::string& Current)
{
    class wxString Path;

    /* Update this native process panel */
    this->Name->SetValue(Current);

    /* Update the project output if it meets the format requirements.*/
    Path=this->Project_Output->GetValue();
    if(Path.starts_with("./"+Original))
    {
        Path.Replace(Original,Current);
        this->Project_Output->SetValue(Path);
    }
}
/* End Function:Native_Panel::Rename *****************************************/

/* Function:Native_Panel::Load ************************************************
Description : Load.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
void Native_Panel::Load(const class Native* Native)
{
    /* Basic options */
    this->Name->SetValue(Native->Name);
    this->Extra_Captbl->SetValue(std::to_string( Native->Extra_Captbl));

    /* Buildsystem options */
    this->Toolchain->SetStringSelection(Native->Toolchain);
    this->Buildsystem_Set(this->Toolchain->GetStringSelection());
    this->Buildsystem->SetStringSelection(Native->Buildsystem);
    this->Optimization->SetSelection(Native->Optimization);

    /* Output options */
    this->Project_Output->SetValue(Native->Project_Output);
    this->Project_Overwrite->SetValue(Native->Project_Overwrite);
    this->Linker_Output->SetValue(Native->Linker_Output);
    this->Main_Header_Output->SetValue(Native->Main_Header_Output);
    this->Main_Source_Output->SetValue(Native->Main_Source_Output);
    this->Entry_Source_Output->SetValue(Native->Entry_Source_Output);
    this->Entry_Source_Overwrite->SetValue(Native->Entry_Source_Overwrite);

    /* Coprocessors */
    Main::Coprocessor_Load(Native->Coprocessor,this->Coprocessor);
}
/* End Function:Native_Panel::Load *******************************************/

/* Function:Native_Panel::Check ***********************************************
Description : Check the basic information settings.
Input       : None.
Output      : None.
Return      : ret_t - 0 for OK, -1 for error.
******************************************************************************/
ret_t Native_Panel::Check(void)
{
    std::string Comp_Sug;
    std::string Toolchain;
    std::string Buildsystem;
    class wxArrayString Comp_Buildsystem;

    /* Extra_Captbl */
    if(Main::Dec_Check(this,this->Extra_Captbl->GetValue(),
                       _("Native Process Config - Basic Options"),
                       _("Extra capability slot"))!=0)
        return -1;

    if(this->Project_Output->GetValue()=="")
    {
        Main::Msgbox_Show(this,MSGBOX_ERROR,
                          _("Native Process Config - Basic Options"),
                          _("The project output folder is unspecified."));
        return -1;
    }
    if(this->Linker_Output->GetValue()=="")
    {
        Main::Msgbox_Show(this,MSGBOX_ERROR,
                          _("Native Process Config - Output Options"),
                          _("The linker output folder is unspecified."));
        return -1;
    }
    if(this->Main_Header_Output->GetValue()=="")
    {
        Main::Msgbox_Show(this,MSGBOX_ERROR,
                          _("Native Process Config - Output Options"),
                          _("The main header output folder is unspecified."));
        return -1;
    }
    if(this->Main_Source_Output->GetValue()=="")
    {
        Main::Msgbox_Show(this,MSGBOX_ERROR,
                          _("Native Process Config - Output Options"),
                          _("The main source output folder is unspecified."));
        return -1;
    }
    if(this->Entry_Source_Output->GetValue()=="")
    {
        Main::Msgbox_Show(this,MSGBOX_ERROR,
                          _("Native Process Config - Output Options"),
                          _("The entry source output folder is unspecified."));
        return -1;
    }

    return 0;
}
/* End Function:Native_Panel::Check ******************************************/

/* Function:Native_Panel::Save ************************************************
Description : Save.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
void Native_Panel::Save(class Native* Native)
{
    /* Basic options */
    Native->Name=this->Name->GetValue();
    Native->Extra_Captbl=std::stoull(std::string(this->Extra_Captbl->GetValue()),0,0);

    /* Buildsystem options */
    Native->Buildsystem=this->Buildsystem->GetStringSelection();
    Native->Toolchain=this->Toolchain->GetStringSelection();
    Native->Optimization=this->Optimization->GetSelection();

    /* Output options */
    Native->Project_Output=this->Project_Output->GetValue();
    Native->Project_Overwrite=this->Project_Overwrite->GetValue();
    Native->Linker_Output=this->Linker_Output->GetValue();
    Native->Main_Header_Output=this->Main_Header_Output->GetValue();
    Native->Main_Source_Output=this->Main_Source_Output->GetValue();
    Native->Entry_Source_Output=this->Entry_Source_Output->GetValue();
    Native->Entry_Source_Overwrite=this->Entry_Source_Overwrite->GetValue();

    /* Coprocessors */
    Main::Coprocessor_Save(Native->Coprocessor,this->Coprocessor);
}
/* End Function:Native_Panel::Save *******************************************/

/* Function:Native_Panel::Coprocessor_Set *************************************
Description : Set coprocessors.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
void Native_Panel::Coprocessor_Set(void)
{
    Main::Coprocessor_Refill(this,
                             this->Coprocessor_Label,
                             this->Coprocessor_Sizer,
                             this->Coprocessor);

    this->Layout();
}
/* End Function:Native_Panel::Coprocessor_Set ********************************/

/* Function:Native_Panel::Toolchain_Set ***************************************
Description : Set the collection of possible tool chains.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
void Native_Panel::Toolchain_Set(void)
{
    std::vector<std::string> Tool_Avail;

    wxLogDebug("Native_Panel::Toolchain_Set");

    /* Ask for possible toolchains */
    Main::Plat_Info->Toolchain_Native(Tool_Avail);
    Main::Choice_Refill(this->Toolchain,Tool_Avail);
}
/* End Function:Native_Panel::Toolchain_Set **********************************/

/* Function:Native_Panel::Buildsystem_Set *************************************
Description : Set build system given the toolchain choice.
Input       : const class wxString& Toolchain - The chosen toolchain.
Output      : None.
Return      : None.
******************************************************************************/
void Native_Panel::Buildsystem_Set(const class wxString& Toolchain)
{
    std::vector<std::string> Build_Avail;

    wxLogDebug("Native_Panel::Buildsystem_Set");

    /* Ask for possible toolchains */
    Main::Plat_Info->Buildsystem_Native(std::string(Toolchain),Build_Avail);
    Main::Choice_Refill(this->Buildsystem,Build_Avail);
}
/* End Function:Native_Panel::Buildsystem_Set ********************************/

/* Function:Native_Panel::On_Name *********************************************
Description : wxEVT_LEFT_DOWN handler for 'Name'.
Input       : class wxMouseEvent& Event - The event.
Output      : None.
Return      : None.
******************************************************************************/
void Native_Panel::On_Name(class wxMouseEvent& Event)
{
    std::string Current;
    std::string Original;

    /* Get the original name */
    Original=this->Name->GetValue();
    Main::Name_Dialog->Load(Original);

    /* Pop up the name dialog */
    if(Main::Name_Dialog->ShowModal()!=wxID_OK)
        return;

    /* Carry out the rename */
    Current=Main::Name_Dialog->Name_Get();
    Main::Native_Rename(Original, Current);
}
/* End Function:Native_Panel::On_Name ****************************************/

/* Function:Native_Panel::On_Toolchain ****************************************
Description : wxEVT_CHOICE handler for 'Toolchain'.
Input       : class wxCommandEvent& Event - The event.
Output      : None.
Return      : None.
******************************************************************************/
void Native_Panel::On_Toolchain(class wxCommandEvent& Event)
{
    class wxString Toolchain;
    class wxString Buildsystem;

    Toolchain=this->Toolchain->GetStringSelection();
    Buildsystem=this->Buildsystem->GetStringSelection();

    /* Recreate options if the current one is invalid */
    if(Main::Plat_Info->Compat_Check(std::string(Toolchain), std::string(Buildsystem))!=0)
        this->Buildsystem_Set(Toolchain);
}
/* End Function:Native_Panel::On_Toolchain ***********************************/
}
/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
