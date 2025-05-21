/******************************************************************************
Filename    : native_basic_panel.cpp
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
#include "Option_Panel/Native_Notebook/Native_Basic_Panel/native_basic_panel.hpp"
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
#include "Option_Panel/Native_Notebook/Native_Basic_Panel/native_basic_panel.hpp"
#include "Config_Tree/config_tree.hpp"
#include "Config_Tree/Config_Popup/config_popup.hpp"
#include "Name_Dialog/name_dialog.hpp"
#undef __HDR_CLASS__
/* End Include ***************************************************************/
namespace RVM_CFG
{
/* Function:Native_Basic_Panel::Native_Basic_Panel ****************************
Description : Constructor for basic notebook.
Input       : class wxWindow* Parent - The parent window.
Output      : None.
Return      : None.
******************************************************************************/
/* void */ Native_Basic_Panel::Native_Basic_Panel(class wxWindow* Parent):
wxPanel(Parent,wxID_ANY)
{
    try
    {
        this->Main_Sizer=new class wxBoxSizer(wxVERTICAL);

        /* Group 1 */
        this->Sizer1=new class wxStaticBoxSizer(wxVERTICAL,this,_("Sizer1"));
        this->Sizer1_1=new class wxBoxSizer(wxHORIZONTAL);

        this->Name_Sizer=new class wxBoxSizer(wxHORIZONTAL);
        this->Name_Label=new class wxStaticText(this,wxID_ANY,_("Name"));
        this->Name=new class wxTextCtrl(this,wxID_ANY,wxEmptyString,wxDefaultPosition,wxDefaultSize,wxTE_READONLY);
        this->Name_Sizer->Add(this->Name_Label,2,wxALL|wxALIGN_CENTER_VERTICAL,I2P(5));
        this->Name_Sizer->Add(this->Name,3,wxALL|wxALIGN_CENTER_VERTICAL,I2P(5));
        this->Name->Bind(wxEVT_LEFT_DOWN, &Native_Basic_Panel::On_Rename, this);

        this->Extra_Captbl_Sizer=new class wxBoxSizer(wxHORIZONTAL);
        this->Extra_Captbl_Label=new class wxStaticText(this,wxID_ANY,_("Extra Capacity Table"));
        this->Extra_Captbl=new class wxTextCtrl(this,wxID_ANY);
        this->Extra_Captbl_Sizer->Add(this->Extra_Captbl_Label,2,wxALL|wxALIGN_CENTER_VERTICAL,I2P(5));
        this->Extra_Captbl_Sizer->Add(this->Extra_Captbl,3,wxALL|wxALIGN_CENTER_VERTICAL,I2P(5));

        this->Coprocessor_Sizer=new class wxBoxSizer(wxHORIZONTAL);

        this->Sizer1_1->Add(this->Name_Sizer,6,wxEXPAND);
        this->Sizer1_1->AddStretchSpacer(1);
        this->Sizer1_1->Add(this->Extra_Captbl_Sizer,6,wxEXPAND);
        this->Sizer1->Add(this->Sizer1_1,0,wxEXPAND);
        this->Sizer1->Add(this->Coprocessor_Sizer,0,wxEXPAND);

//        /* Group 2 */
//        this->Sizer2=new class wxStaticBoxSizer(wxVERTICAL,this,_("Sizer2"));
//        this->Sizer2_1=new class wxBoxSizer(wxHORIZONTAL);
//
//        this->Coprocessor_Sizer=new class wxBoxSizer(wxHORIZONTAL);
//
//        this->Sizer2_1->Add(this->Coprocessor_Sizer,1,wxEXPAND);
//        this->Sizer2->Add(this->Sizer2_1,0,wxEXPAND);

        /* Group 3 */
        this->Sizer3=new class wxStaticBoxSizer(wxVERTICAL,this,_("Sizer3"));
        this->Sizer3_1=new class wxBoxSizer(wxHORIZONTAL);
        this->Sizer3_2=new class wxBoxSizer(wxHORIZONTAL);

        this->Toolchain_Sizer=new class wxBoxSizer(wxHORIZONTAL);
        this->Toolchain_Label=new class wxStaticText(this,wxID_ANY,_("Toolchain"));
        this->Toolchain=new class wxChoice(this,wxID_ANY);
        this->Toolchain_Sizer->Add(this->Toolchain_Label,2,wxALL|wxALIGN_CENTER_VERTICAL,I2P(5));
        this->Toolchain_Sizer->Add(this->Toolchain,3,wxALL|wxALIGN_CENTER_VERTICAL,I2P(5));
        this->Toolchain->Bind(wxEVT_CHOICE, &Native_Basic_Panel::On_Toolchain_Change, this, this->Toolchain->GetId());

        this->Buildsystem_Sizer=new class wxBoxSizer(wxHORIZONTAL);
        this->Buildsystem_Label=new class wxStaticText(this,wxID_ANY,_("Buildsystem"));
        this->Buildsystem=new class wxChoice(this,wxID_ANY);
        this->Buildsystem_Sizer->Add(this->Buildsystem_Label,2,wxALL|wxALIGN_CENTER_VERTICAL,I2P(5));
        this->Buildsystem_Sizer->Add(this->Buildsystem,3,wxALL|wxALIGN_CENTER_VERTICAL,I2P(5));
        //this->Buildsystem->Bind(wxEVT_CHOICE, &Native_Basic_Panel::On_Buildsystem_Change, this, this->Buildsystem->GetId());

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


        this->Sizer3_1->Add(this->Toolchain_Sizer,6,wxEXPAND);
        this->Sizer3_1->AddStretchSpacer(1);
        this->Sizer3_1->Add(this->Buildsystem_Sizer,6,wxEXPAND);
        this->Sizer3_2->Add(this->Optimization_Sizer,6,wxEXPAND);
        this->Sizer3_2->AddStretchSpacer(7);
        this->Sizer3->Add(this->Sizer3_1,0,wxEXPAND);
        this->Sizer3->Add(this->Sizer3_2,0,wxEXPAND);

        /* Group 4 */
        this->Sizer4=new class wxStaticBoxSizer(wxVERTICAL,this,_("Sizer4"));

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

        this->Sizer4->Add(this->Project_Sizer,0,wxEXPAND);
        this->Sizer4->Add(this->Linker_Output_Sizer,0,wxEXPAND);
        this->Sizer4->Add(this->Main_Header_Output_Sizer,0,wxEXPAND);
        this->Sizer4->Add(this->Main_Source_Output_Sizer,0,wxEXPAND);
        this->Sizer4->Add(this->Entry_Source_Sizer,0,wxEXPAND);

        this->Main_Sizer->Add(this->Sizer1,0,wxEXPAND|wxALL,I2P(5));
        //this->Main_Sizer->Add(this->Sizer2,0,wxEXPAND|wxALL,I2P(5));
        this->Main_Sizer->Add(this->Sizer3,0,wxEXPAND|wxALL,I2P(5));
        this->Main_Sizer->Add(this->Sizer4,0,wxEXPAND|wxALL,I2P(5));

        this->SetSizer(this->Main_Sizer);
        this->Layout();
    }
    catch(std::exception& Exc)
    {
        ASSERT(0,Exc.what());
    }
}
/* End Function:Native_Basic_Panel::Native_Basic_Panel *************((********/

/* Function:Native_Basic_Panel::~Native_Basic_Panel ***************************
Description : Destructor for sheet notebook.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
/* void */ Native_Basic_Panel::~Native_Basic_Panel(void)
{

}
/* End Function:Native_Basic_Panel::Native_Basic_Panel ***********************/

/* Function:Native_Basic_Panel::Load ***************************
Description : Load.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
void Native_Basic_Panel::Load(const class Native* Native)
{
    this->Name->SetValue(Native->Name);
    this->Extra_Captbl->SetValue(std::to_string( Native->Extra_Captbl));
    this->Buildsystem->SetStringSelection(Native->Buildsystem);
    this->Toolchain->SetStringSelection(Native->Toolchain);
    this->Optimization->SetSelection(Native->Optimization);
    this->Project_Output->SetValue(Native->Project_Output);
    this->Project_Overwrite->SetValue(Native->Project_Overwrite);
    this->Linker_Output->SetValue(Native->Linker_Output);
    this->Main_Header_Output->SetValue(Native->Main_Header_Output);
    this->Main_Source_Output->SetValue(Native->Main_Source_Output);
    this->Entry_Source_Output->SetValue(Native->Entry_Source_Output);
    this->Entry_Source_Overwrite->SetValue(Native->Entry_Source_Overwrite);

    /* Clear check box which might be used before */
    for (auto& Cop : this->Coprocessor)
        Cop.second->SetValue(false);

    for(const std::string&Cop : Native->Coprocessor)
        if(this->Coprocessor.find(Cop)!=this->Coprocessor.end())
            this->Coprocessor.find(Cop)->second->SetValue(true);
}
/* End Function:Native_Basic_Panel::Load ***********************/

/* Function:Native_Basic_Panel::Save ***************************
Description : Save.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
void Native_Basic_Panel::Save(class Native* Native)
{

    Native->Name=this->Name->GetValue();
    Native->Extra_Captbl=std::stoull(this->Extra_Captbl->GetValue().ToStdString(),0,0);
    Native->Buildsystem=this->Buildsystem->GetStringSelection();
    Native->Toolchain=this->Toolchain->GetStringSelection();
    Native->Optimization=this->Optimization->GetSelection();
    Native->Project_Output=this->Project_Output->GetValue();
    Native->Project_Overwrite=this->Project_Overwrite->GetValue();
    Native->Linker_Output=this->Linker_Output->GetValue();
    Native->Main_Header_Output=this->Main_Header_Output->GetValue();
    Native->Main_Source_Output=this->Main_Source_Output->GetValue();
    Native->Entry_Source_Output=this->Entry_Source_Output->GetValue();
    Native->Entry_Source_Overwrite=this->Entry_Source_Overwrite->GetValue();

    /* Clear */
    Native->Coprocessor.clear();

    /* Coprocessor */
    for(const auto&Cop : this->Coprocessor)
        if(Cop.second->GetValue())
            Native->Coprocessor.push_back(Cop.first);
    if(Native->Coprocessor.size()==0)
        Native->Coprocessor.push_back("None");
}
/* End Function:Native_Basic_Panel::Save ***********************/

/* Function:Native_Basic_Panel::Check ****************************************
Description : Check the basic information settings.
Input       : None.
Output      : None.
Return      : ret_t - 0 for OK, -1 for error.
******************************************************************************/
ret_t Native_Basic_Panel::Check()
{
    std::string Comp_Sug;
    std::string Toolchain;
    std::string Buildsystem;
    class wxArrayString Comp_Buildsystem;

    /* Extra_Captbl */
    if(Main::Num_GEZ_Check(this->Extra_Captbl->GetValue().ToStdString())!=0)
    {
        Main::Msgbox_Show(this,MSGBOX_ERROR,
                          _("Native Process Config - Basic information"),
                          _("Extra capability table is not a valid decimal nonnegative integer."));
        return -1;
    }

    if(this->Project_Output->GetValue()=="")
    {
        Main::Msgbox_Show(this,MSGBOX_ERROR,
                          _("Native Process Config - Basic information"),
                          _("The project output folder is unspecified."));
        return -1;
    }
    if(this->Linker_Output->GetValue()=="")
    {
        Main::Msgbox_Show(this,MSGBOX_ERROR,
                          _("Native Process Config - Basic information"),
                          _("The linker output folder is unspecified."));
        return -1;
    }
    if(this->Main_Header_Output->GetValue()=="")
    {
        Main::Msgbox_Show(this,MSGBOX_ERROR,
                          _("Native Process Config - Basic information"),
                          _("The main header output folder is unspecified."));
        return -1;
    }
    if(this->Main_Source_Output->GetValue()=="")
    {
        Main::Msgbox_Show(this,MSGBOX_ERROR,
                          _("Native Process Config - Basic information"),
                          _("The main source output folder is unspecified."));
        return -1;
    }
    if(this->Entry_Source_Output->GetValue()=="")
    {
        Main::Msgbox_Show(this,MSGBOX_ERROR,
                          _("Native Process Config - Basic information"),
                          _("The entry source output folder is unspecified."));
        return -1;
    }

    /*  Compatible */
    Buildsystem=this->Buildsystem->GetStringSelection();
    Toolchain=this->Toolchain->GetStringSelection();
    if(Main::Comp_Check(Buildsystem,Toolchain,"Native")!=0)
    {
        for(const std::unique_ptr<class Compatible>&Comp : Main::Plat_Info->Compatible)
        {
            if(Toolchain==Comp->Toolchain&&Comp_Buildsystem.Index(Comp->Buildsystem)==wxNOT_FOUND)
            {
                Comp_Buildsystem.push_back(Comp->Buildsystem);
                Comp_Sug=Comp_Sug+Comp->Toolchain+", "+Comp->Buildsystem+"\n";
            }
        }
        Main::Msgbox_Show(this, MSGBOX_ERROR,
                          _("Native Process Config - Basic information"),
                          _("Compatibility error, you may try:\n"+Comp_Sug));
        return -1;
    }
    return 0;
}
/* End Function:Native_Basic_Panel::Check ***********************************/

/* Function:Native_Basic_Panel::Coprocessor_Set ******************************
Description : Set coprocessors.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
void Native_Basic_Panel::Coprocessor_Set()
{
    class wxCheckBox* Temp;

    /* Clear */
    this->Coprocessor.clear();
    this->Coprocessor_Sizer->Clear(true);

    /* Set coprocessor check boxes*/
    this->Coprocessor_Label=new wxStaticText(this,wxID_ANY,_("Coprocessor"));
    this->Coprocessor_Sizer->Add(this->Coprocessor_Label,0,wxALL|wxALIGN_CENTER_VERTICAL,I2P(5));
    this->Coprocessor_Sizer->AddStretchSpacer(1);

    for(std::string Cop : Main::Chip_Info->Coprocessor)
    {
        if(Cop=="None")
            continue;
        Temp=new wxCheckBox(this,wxID_ANY,_(Cop));
        Coprocessor.insert(std::make_pair(Cop,Temp ));
        this->Coprocessor_Sizer->Add(Temp, 0, wxEXPAND);
        this->Coprocessor_Sizer->AddStretchSpacer(1);
    }
    this->Layout();
}
/* End Function:Native_Basic_Panel::Coprocessor_Set **************************/

/* Function:Native_Basic_Panel::Compatible_Set ********************************
Description : Set build system and tool chain.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
void Native_Basic_Panel::Compatible_Set()
{
    this->Toolchain->Clear();
    this->Buildsystem->Clear();
    for(std::unique_ptr<class Compatible>& Comp : Main::Plat_Info->Compatible)
    {
        if(this->Toolchain->FindString(Comp->Toolchain)==wxNOT_FOUND)
            this->Toolchain->Append(Comp->Toolchain);
        if(this->Buildsystem->FindString(Comp->Buildsystem)==wxNOT_FOUND)
            this->Buildsystem->Append(Comp->Buildsystem);
    }
}
/* End Function:Native_Basic_Panel::Compatible_Set ***************************/

/* Function:Native_Basic_Panel::On_Toolchain_Change ***************************
Description : Configure a compatible build system based on the tool chain settings.
Input       : class wxFocusEvent& Event - The event.
Output      : None.
Return      : None.
******************************************************************************/
void Native_Basic_Panel::On_Toolchain_Change(class wxCommandEvent& Event)
{
    std::string Toolchain;

    Toolchain=this->Toolchain->GetStringSelection();

    this->Buildsystem->Clear();
    for(std::unique_ptr<class Compatible>& Comp : Main::Plat_Info->Compatible)
        if(Toolchain==Comp->Toolchain&&this->Buildsystem->FindString(Comp->Buildsystem)==wxNOT_FOUND)
            this->Buildsystem->Append(Comp->Buildsystem);
    /* Default selection */
    if(this->Buildsystem->GetCount()>0)
        this->Buildsystem->SetSelection(0);
}
/* End Function:Native_Basic_Panel::On_Toolchain_Change **********************/

/* Function:Native_Basic_Panel::On_Rename *************************************
Description : Pop up a rename dialog, and rename this native process.
Input       : class wxMouseEvent& Event - The event.
Output      : None.
Return      : None.
******************************************************************************/
void Native_Basic_Panel::On_Rename(class wxMouseEvent& Event)
{
    ptr_t Dialog_State;
    std::string Current;
    std::string Original;
    /* Cookie is a typedef, not a class */
    wxTreeItemIdValue Cookie;
    class wxTreeItemId Child;
    class wxString Project_Output_Path;

    /* Get the original name */
    Original=this->Name->GetValue();
    Main::Name_Dialog->Load(Original);

    /* Pop up the name dialog */
    Dialog_State=Main::Name_Dialog->ShowModal();
    if(Dialog_State!=wxID_OK)
        return;

    /* Get the new name, then check */
    Current=Main::Name_Dialog->Name_Get();
    if(Main::Proj_Info->Native_Rename(Original,Current))
    {
        if (!Main::Config->Native_Config.IsOk())
            return;

        /* Update config tree */
        Child=Main::Config->GetFirstChild(Main::Config->Native_Config,Cookie);
        while (Child.IsOk())
        {
            if(Main::Config->GetItemText(Child)==Original)
            {
                Main::Config->SetItemText(Child,Current);
                break;
            }
            Child=Main::Config->GetNextChild(Main::Config->Native_Config,Cookie);
        }
        /* Update this native process panel */
        this->Name->SetValue(Current);

        /* Update the project output if it meets the format requirements.*/
        Project_Output_Path=this->Project_Output->GetValue();
        if(Project_Output_Path.starts_with("./"+Original))
        {
            Project_Output_Path.Replace(Original,Current);
            this->Project_Output->SetValue(Project_Output_Path);
        }
    }
}
/* End Function:Native_Basic_Panel::On_Rename ********************************/

}
/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
