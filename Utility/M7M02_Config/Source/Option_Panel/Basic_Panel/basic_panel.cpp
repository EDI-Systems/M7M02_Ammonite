/******************************************************************************
Filename    : basic_panel.cpp
Author      : lbc
Date        : 24/04/2025
License     : Proprietary; confidential.
Description : Basic panel implementation.
******************************************************************************/

/* Include *******************************************************************/
#define __HDR_DEF__
#include "rvm_cfg.hpp"
#undef __HDR_DEF__

#include "wx/wx.h"
#include "wx/notebook.h"
#include "wx/xml/xml.h"
#include "wx/treectrl.h"
#include "wx/dnd.h"

#include "set"
#include "map"
#include "memory"

#define __HDR_DEF__
#include "Option_Panel/Basic_Panel/basic_panel.hpp"
#undef __HDR_DEF__

#define __HDR_CLASS__
#include "rvm_cfg.hpp"
#include "Proj_Info/proj_info.hpp"
#include "Option_Panel/Basic_Panel/basic_panel.hpp"
#include "Proj_Info/Chip/chip.hpp"
#include "Proj_Info/Process/process.hpp"
#include "Proj_Info/Kernel/kernel.hpp"
#include "Proj_Info/Monitor/monitor.hpp"
#include "Proj_Info/Process/Native/native.hpp"
#include "Proj_Info/Process/Virtual/virtual.hpp"
#include "Chip_Info/chip_info.hpp"
#include "Plat_Info/plat_info.hpp"
#include "Plat_Info/Config/config.hpp"
#include "Plat_Info/Compatible/compatible.hpp"
#include "Name_Dialog/name_dialog.hpp"
#include "Config_Tree/config_tree.hpp"
#undef __HDR_CLASS__
/* End Include ***************************************************************/
namespace RVM_CFG
{
/* Function:Basic_Panel::Basic_Panel ******************************************
Description : Constructor for basic information. This panel include
              name, version, assert, debug, page table, build system
              and workspace.
Input       : class wxWindow* Parent - The parent window.
Output      : None.
Return      : None.
******************************************************************************/
/* void */ Basic_Panel::Basic_Panel(class wxWindow* Parent):
wxPanel(Parent,wxID_ANY),Has_Been_Shown(HAS_NOT_SHOWN)
{
    try
    {
        this->SetBackgroundColour(*wxWHITE);
        this->Main_Sizer=new class wxBoxSizer(wxVERTICAL);

        /* Group 1 */
        this->Sizer1=new class wxStaticBoxSizer(wxVERTICAL,this,_("Sizer1"));
        this->Sizer1_1=new class wxBoxSizer(wxHORIZONTAL);;
        this->Sizer1_2=new class wxBoxSizer(wxHORIZONTAL);

        this->Name_Sizer=new class wxBoxSizer(wxHORIZONTAL);
        this->Name_Label=new class wxStaticText(this,wxID_ANY,_("Name"));
        this->Name=new class wxTextCtrl(this,wxID_ANY,wxEmptyString,wxDefaultPosition,wxDefaultSize,wxTE_READONLY);
        this->Name_Sizer->Add(this->Name_Label,2,wxALL|wxALIGN_CENTER_VERTICAL,I2P(5));
        this->Name_Sizer->Add(this->Name,3,wxALL|wxALIGN_CENTER_VERTICAL,I2P(5));
        this->Name->Bind(wxEVT_LEFT_DOWN, &Basic_Panel::On_Rename, this);

        this->Assert_Enable_Sizer=new class wxBoxSizer(wxHORIZONTAL);
        this->Assert_Enable_Label=new class wxStaticText(this,wxID_ANY,_("Assert Enable"));
        this->Assert_Enable=new class wxCheckBox(this,wxID_ANY,wxEmptyString);
        this->Assert_Enable_Sizer->Add(this->Assert_Enable_Label,2,wxALL|wxALIGN_CENTER_VERTICAL,I2P(5));
        this->Assert_Enable_Sizer->Add(this->Assert_Enable,3,wxALL|wxALIGN_CENTER_VERTICAL,I2P(5));

        this->Debug_Log_Enable_Sizer=new class wxBoxSizer(wxHORIZONTAL);
        this->Debug_Log_Enable_Label=new class wxStaticText(this,wxID_ANY,_("Debug Log Enable"));
        this->Debug_Log_Enable=new class wxCheckBox(this,wxID_ANY,wxEmptyString);
        this->Debug_Log_Enable_Sizer->Add(this->Debug_Log_Enable_Label,2,wxALL|wxALIGN_CENTER_VERTICAL,I2P(5));
        this->Debug_Log_Enable_Sizer->Add(this->Debug_Log_Enable,3,wxALL|wxALIGN_CENTER_VERTICAL,I2P(5));

        this->Pgtbl_Raw_Enable_Sizer=new class wxBoxSizer(wxHORIZONTAL);
        this->Pgtbl_Raw_Enable_Label=new class wxStaticText(this,wxID_ANY,_("Raw Page Table Enable"));
        this->Pgtbl_Raw_Enable=new class wxCheckBox(this,wxID_ANY,wxEmptyString);
        this->Pgtbl_Raw_Enable_Sizer->Add(this->Pgtbl_Raw_Enable_Label,2,wxALL|wxALIGN_CENTER_VERTICAL,I2P(5));
        this->Pgtbl_Raw_Enable_Sizer->Add(this->Pgtbl_Raw_Enable,3,wxALL|wxALIGN_CENTER_VERTICAL,I2P(5));

        this->Sizer1_1->Add(this->Name_Sizer,6,wxEXPAND);
        this->Sizer1_1->AddStretchSpacer(1);
        this->Sizer1_1->Add(this->Assert_Enable_Sizer,6,wxEXPAND);
        this->Sizer1_2->Add(this->Debug_Log_Enable_Sizer,6,wxEXPAND);
        this->Sizer1_2->AddStretchSpacer(1);
        this->Sizer1_2->Add(this->Pgtbl_Raw_Enable_Sizer,6,wxEXPAND);
        this->Sizer1->Add(this->Sizer1_1,0,wxEXPAND);
        this->Sizer1->Add(this->Sizer1_2,0,wxEXPAND);

        /* Group 2 */
        this->Sizer2=new class wxStaticBoxSizer(wxVERTICAL,this,_("Sizer2"));
        this->Sizer2_1=new class wxBoxSizer(wxHORIZONTAL);

        this->Buildsystem_Sizer=new class wxBoxSizer(wxHORIZONTAL);
        this->Buildsystem_Label=new class wxStaticText(this,wxID_ANY,_("Buildsystem"));
        this->Buildsystem=new class wxChoice(this,wxID_ANY);
        this->Buildsystem_Sizer->Add(this->Buildsystem_Label,2,wxALL|wxALIGN_CENTER_VERTICAL,I2P(5));
        this->Buildsystem_Sizer->Add(this->Buildsystem,3,wxALL|wxALIGN_CENTER_VERTICAL,I2P(5));

        this->Workspace_Overwrite_Sizer=new class wxBoxSizer(wxHORIZONTAL);
        this->Workspace_Overwrite_Label=new class wxStaticText(this,wxID_ANY,_("Workspace Overwrite"));
        this->Workspace_Overwrite=new class wxCheckBox(this,wxID_ANY,wxEmptyString);
        this->Workspace_Overwrite_Sizer->Add(this->Workspace_Overwrite_Label,2,wxALL|wxALIGN_CENTER_VERTICAL,I2P(5));
        this->Workspace_Overwrite_Sizer->Add(this->Workspace_Overwrite,3,wxALL|wxALIGN_CENTER_VERTICAL,I2P(5));

        this->Coprocessor_Sizer=new class wxBoxSizer(wxHORIZONTAL);

        this->Sizer2_1->Add(Buildsystem_Sizer,6,wxEXPAND);
        this->Sizer2_1->AddStretchSpacer(1);
        this->Sizer2_1->Add(Workspace_Overwrite_Sizer,6,wxEXPAND);
        this->Sizer2->Add(this->Sizer2_1,0,wxEXPAND);
        this->Sizer2->Add(this->Coprocessor_Sizer,0,wxEXPAND);

//        /* Group 3 */
//        this->Sizer3=new class wxStaticBoxSizer(wxVERTICAL,this,_("Sizer3"));
//        this->Sizer3_1=new class wxBoxSizer(wxHORIZONTAL);
//
//        this->Coprocessor_Sizer=new class wxBoxSizer(wxHORIZONTAL);
//
//        this->Sizer3_1->Add(this->Coprocessor_Sizer,1,wxEXPAND);
//        this->Sizer3->Add(this->Sizer3_1,0,wxEXPAND);

        /* Group 4 */
        this->Sizer4=new class wxStaticBoxSizer(wxVERTICAL,this,_("Sizer4"));
        this->Sizer4_1=new class wxBoxSizer(wxHORIZONTAL);
        this->Sizer4_2=new class wxBoxSizer(wxHORIZONTAL);

        this->Platform_Sizer=new class wxBoxSizer(wxHORIZONTAL);
        this->Platform_Label=new class wxStaticText(this,wxID_ANY,_("Platform"));
        this->Platform=new class wxTextCtrl(this,wxID_ANY,wxEmptyString,wxDefaultPosition,wxDefaultSize,wxTE_READONLY);
        this->Platform_Sizer->Add(this->Platform_Label,2,wxALL|wxALIGN_CENTER_VERTICAL,I2P(5));
        this->Platform_Sizer->Add(this->Platform,3,wxALL|wxALIGN_CENTER_VERTICAL,I2P(5));

        this->Class_Sizer=new class wxBoxSizer(wxHORIZONTAL);
        this->Class_Label=new class wxStaticText(this,wxID_ANY,_("Chip Class"));
        this->Class=new class wxTextCtrl(this,wxID_ANY,wxEmptyString,wxDefaultPosition,wxDefaultSize,wxTE_READONLY);
        this->Class_Sizer->Add(this->Class_Label,2,wxALL|wxALIGN_CENTER_VERTICAL,I2P(5));
        this->Class_Sizer->Add(this->Class,3,wxALL|wxALIGN_CENTER_VERTICAL,I2P(5));

        this->Chip_Name_Sizer=new class wxBoxSizer(wxHORIZONTAL);
        this->Chip_Name_Label=new class wxStaticText(this,wxID_ANY,_("Chip Name"));
        this->Chip_Name=new class wxTextCtrl(this,wxID_ANY,wxEmptyString,wxDefaultPosition,wxDefaultSize,wxTE_READONLY);
        this->Chip_Name_Sizer->Add(this->Chip_Name_Label,2,wxALL|wxALIGN_CENTER_VERTICAL,I2P(5));
        this->Chip_Name_Sizer->Add(this->Chip_Name,3,wxALL|wxALIGN_CENTER_VERTICAL,I2P(5));

        this->Sizer4_1->Add(this->Platform_Sizer,6,wxEXPAND);
        this->Sizer4_1->AddStretchSpacer(1);
        this->Sizer4_1->Add(this->Class_Sizer,6,wxEXPAND);
        this->Sizer4_2->Add(this->Chip_Name_Sizer,6,wxEXPAND);
        this->Sizer4_2->AddStretchSpacer(7);
        this->Sizer4->Add(this->Sizer4_1,0,wxEXPAND);
        this->Sizer4->Add(this->Sizer4_2,0,wxEXPAND);

        /* Group 5 */
        this->Sizer5=new class wxStaticBoxSizer(wxVERTICAL,this,_("Sizer5"));

        this->Main_Sizer->Add(this->Sizer1,0,wxEXPAND|wxALL,I2P(5));
        this->Main_Sizer->Add(this->Sizer2,0,wxEXPAND|wxALL,I2P(5));
        //this->Main_Sizer->Add(this->Sizer3,0,wxEXPAND|wxALL,I2P(5));
        this->Main_Sizer->Add(this->Sizer4,0,wxEXPAND|wxALL,I2P(5));
        this->Main_Sizer->Add(this->Sizer5,0,wxEXPAND|wxALL,I2P(5));

        this->SetSizer(this->Main_Sizer);
        this->Layout();
    }
    catch(std::exception& Exc)
    {
        ASSERT(0,Exc.what());
    }
}
/* End Function:Basic_Panel::Basic_Panel *************************************/

/* Function:Basic_Panel::~Basic_Panel *****************************************
Description : Destructor for sheet notebook.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
/* void */ Basic_Panel::~Basic_Panel(void)
{

}
/* End Function:Basic_Panel::Basic_Panel *************************************/

/* Function:Basic_Panel::Load *************************************************
Description : Load.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
void Basic_Panel::Load(void)
{
    std::string Conf_Val;
    std::string Conf_Name;

    wxLogDebug("Basic_Panel::Load");

    this->Name->SetValue(Main::Proj_Info->Name);
    this->Assert_Enable->SetValue(Main::Proj_Info->Assert_Enable);
    this->Debug_Log_Enable->SetValue(Main::Proj_Info->Debug_Log_Enable);
    this->Pgtbl_Raw_Enable->SetValue(Main::Proj_Info->Pgtbl_Raw_Enable);
    this->Buildsystem->SetStringSelection(Main::Proj_Info->Buildsystem);
    this->Workspace_Overwrite->SetValue(Main::Proj_Info->Workspace_Overwrite);

    /* Save chip information */
    this->Platform->SetValue(Main::Proj_Info->Chip.get()->Platform);
    this->Class->SetValue(Main::Proj_Info->Chip.get()->Class);
    this->Chip_Name->SetValue(Main::Proj_Info->Chip.get()->Name);

    /* Coprocessor */
    for(const std::string& Cop:Main::Proj_Info->Chip->Coprocessor)
    {
        if(Cop=="None")
            break;
        if(this->Coprocessor.find(Cop)!=this->Coprocessor.end())
            this->Coprocessor.find(Cop)->second->SetValue(true);
    }

    /* Chip config */
    for(const std::unique_ptr<class Conf_Info>& Conf:Main::Plat_Info->Config)
    {
        if(Main::Proj_Info->Chip->Config.find(Conf->Name)==Main::Proj_Info->Chip->Config.end())
            continue;
        Conf_Val=Main::Main::Proj_Info->Chip->Config.find(Conf->Name)->second;
        Conf_Name=Main::Proj_Info->Chip->Config.find(Conf->Name)->first;
        if(Conf->Type==CONFIG_RANGE)
            this->Config_Range.find(Conf_Name)->second->SetValue(Conf_Val);
        else if(Conf->Type==CONFIG_CHOICE)
            this->Config_Choice.find(Conf_Name)->second->SetStringSelection(Conf_Val);
    }
}
/* End Function:Basic_Panel::Load ********************************************/

/* Function:Basic_Panel::Save *************************************************
Description : Save.
Input       : None.
Output      : None.
Return      : ret_t - 0 for OK, -1 for error.
******************************************************************************/
void Basic_Panel::Save(void)
{
    std::vector<std::string> Coprocessor_Temp;
    std::map<std::string,std::string> Config_Temp;
    std::map<std::string,class wxChoice*>::iterator Conf_Choice_It;
    std::map<std::string,class wxTextCtrl*>::iterator Conf_Range_It;
    std::map<std::string,class wxCheckBox*>::iterator Coprocessor_Iter;

    Main::Proj_Info->Name=this->Name->GetValue();
    Main::Proj_Info->Assert_Enable=this->Assert_Enable->GetValue();;
    Main::Proj_Info->Debug_Log_Enable=this->Debug_Log_Enable->GetValue();
    Main::Proj_Info->Pgtbl_Raw_Enable=this->Pgtbl_Raw_Enable->GetValue();
    Main::Proj_Info->Buildsystem=this->Buildsystem->GetStringSelection();
    Main::Proj_Info->Workspace_Overwrite=this->Workspace_Overwrite->GetValue();

    /* Clear */
    Main::Proj_Info->Chip->Coprocessor.clear();

    /* Save chip information */
    for(Coprocessor_Iter=this->Coprocessor.begin();Coprocessor_Iter!=this->Coprocessor.end();++Coprocessor_Iter)
        if(Coprocessor_Iter->second->GetValue())
            Coprocessor_Temp.push_back(Coprocessor_Iter->first);

    if(Coprocessor_Temp.size()==0)
        Coprocessor_Temp.push_back("None");


    /* Config */
    for(Conf_Range_It=this->Config_Range.begin();Conf_Range_It!=this->Config_Range.end();++Conf_Range_It)
        Config_Temp.insert(std::make_pair(Conf_Range_It->first,Conf_Range_It->second->GetValue()));
    for(Conf_Choice_It=this->Config_Choice.begin();Conf_Choice_It!=this->Config_Choice.end();++Conf_Choice_It)
        Config_Temp.insert(std::make_pair(Conf_Choice_It->first,Conf_Choice_It->second->GetStringSelection()));

    Main::Proj_Info->Chip->Save(this->Platform->GetValue().ToStdString(),this->Class->GetValue().ToStdString(),
                                this->Chip_Name->GetValue().ToStdString(),Coprocessor_Temp,Config_Temp);
}
/* End Function:Basic_Panel::Save ********************************************/

/* Function:Basic_Panel::Check ************************************************
Description : Check whether the current panel contains any errors..
Input       : None.
Output      : None.
Return      : ret_t - If 0, no error exists; else -1..
******************************************************************************/
ret_t Basic_Panel::Check(void)
{
    ret_t Val;
    ret_t Floor;
    ret_t Ceiling;
    std::string Num;
    std::string Buildsystem;
    std::map<std::string,class wxTextCtrl*>::iterator Iter;

    /* The project name cannot be empty */
    if(Main::Idtfr_Check(this->Name->GetValue().Upper().ToStdString())!=0)
    {
        Main::Msgbox_Show(this, MSGBOX_ERROR,
                                          _("Basic Config"),
                                          _("The project name is not a valid C identifier"));
        return -1;
    }

    /* Check config */
    for(const std::unique_ptr<class Conf_Info>& Conf:Main::Plat_Info->Config)
    {
        /* Wrong situation, project information is incomplete */
        if(Main::Proj_Info->Chip->Config.find(Conf->Name)==Main::Proj_Info->Chip->Config.end())
            continue;
        Iter=this->Config_Range.find(Conf->Name);
        /* Only "Range" needs to be checked, "Choice" does not need to be checked */
        if(Iter!=this->Config_Range.end()&&Conf->Type==CONFIG_RANGE)
        {
            Num=Iter->second->GetValue().ToStdString();
            if(Main::Num_GZ_Check(Num)!=0)
            {
                Main::Msgbox_Show(this, MSGBOX_ERROR,
                                  _("Basic Config"),
                                  _(Conf->Name+" is not a valid decimal positive integer"));
                return -1;
            }
            Val=std::stoull(Iter->second->GetValue().ToStdString(),0,0);
            Floor=std::stoull(Conf->Range[0],0,0);
            Ceiling=std::stoull(Conf->Range[1],0,0);
            if(Val<Floor)
            {
                Main::Msgbox_Show(this, MSGBOX_ERROR,
                                  _("Basic Config"),
                                  _(Conf->Name+" must be greater than or equal to "+Conf->Range[0]));
                return -1;
            }
            else if(Val>Ceiling)
            {
                Main::Msgbox_Show(this, MSGBOX_ERROR,
                                  _("Basic Config"),
                                  _(Conf->Name+" must be less than or equal to "+Conf->Range[1]));
                return -1;
            }
        }
    }

    /* Check buildsystem */
    Buildsystem=this->Buildsystem->GetStringSelection();
    if(Buildsystem!="None")
    {
        if(Main::Proj_Info->Kernel->Buildsystem!=Main::Proj_Info->Monitor->Buildsystem)
        {
            Main::Msgbox_Show(this, MSGBOX_ERROR,
                              _("Basic Config"),
                              _("Buildsystem can only choose None"));
            return -1;
        }
        for(std::unique_ptr<class Native>&Native : Main::Proj_Info->Native)
        {
            if(Main::Proj_Info->Kernel->Buildsystem!=Native->Buildsystem)
            {
                Main::Msgbox_Show(this, MSGBOX_ERROR,
                                  _("Basic Config"),
                                  _("Buildsystem can only choose None"));
                return -1;
            }
        }
        for(std::unique_ptr<class Virtual>&Virtual : Main::Proj_Info->Virtual)
        {
            if(Main::Proj_Info->Kernel->Buildsystem!=Virtual->Buildsystem)
            {
                Main::Msgbox_Show(this, MSGBOX_ERROR,
                                  _("Basic Config"),
                                  _("Buildsystem can only choose None"));
                return -1;
            }
        }
    }

    return 0;
}
/* End Function:Basic_Panel::Check *******************************************/

/* Function:Basic_Panel::Coprocessor_Set **************************************
Description : Set coprocessors.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
void Basic_Panel::Coprocessor_Set()
{
    class wxCheckBox* TEMP_CBox;

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
        TEMP_CBox=new wxCheckBox(this,wxID_ANY,_(Cop));
        Coprocessor.insert(std::make_pair(Cop,TEMP_CBox ));
        this->Coprocessor_Sizer->Add(TEMP_CBox, 0, wxEXPAND);
        this->Coprocessor_Sizer->AddStretchSpacer(1);
    }
    this->Layout();
}
/* End Function:Basic_Panel::Coprocessor_Set *********************************/

/* Function:Basic_Panel::Config_Set *******************************************
Description : Set chip config.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
void Basic_Panel::Config_Set()
{
    class wxChoice*Temp_Choice;
    class wxTextCtrl* Temp_Text;
    class wxBoxSizer* Temp_Sizer;
    class wxStaticText* Temp_Label;

    /* Clear */
    this->Sizer5->Clear(true);
    this->Config_Choice.clear();
    this->Config_Range.clear();

    for(const std::unique_ptr<class Conf_Info>& Conf : Main::Plat_Info->Config)
    {
        Temp_Sizer=new class wxBoxSizer(wxHORIZONTAL);
        Temp_Label=new class wxStaticText(this,wxID_ANY,Conf->Name);
        Temp_Sizer->Add(Temp_Label,1,wxALL|wxALIGN_CENTER_VERTICAL,I2P(5));
        if(Conf->Type==CONFIG_CHOICE)
        {
            Temp_Choice=new class wxChoice(this,wxID_ANY);
            for(const std::string& Choi : Conf->Range)
                Temp_Choice->Append(Choi);
            Temp_Choice->SetStringSelection(Conf->Default);
            Temp_Sizer->Add(Temp_Choice,4,wxALL|wxALIGN_CENTER_VERTICAL,I2P(5));
            this->Config_Choice.insert(std::make_pair(Conf->Name,Temp_Choice));
        }
        else if(Conf->Type==CONFIG_RANGE)
        {
            Temp_Text=new class wxTextCtrl(this,wxID_ANY);
            Temp_Text->SetValue(Conf->Default);
            Temp_Sizer->Add(Temp_Text,4,wxALL|wxALIGN_CENTER_VERTICAL,I2P(5));
            this->Config_Range.insert(std::make_pair(Conf->Name,Temp_Text));
        }
        this->Sizer5->Add(Temp_Sizer, 0, wxEXPAND);
    }
    this->Layout();
}
/* End Function:Basic_Panel::Config_Set **************************************/

/* Function:Basic_Panel::Buildsystem_Set **************************************
Description : Set build system.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
void Basic_Panel::Buildsystem_Set()
{
    for(std::unique_ptr<class Compatible>& Comp : Main::Plat_Info->Compatible)
        if(this->Buildsystem->FindString(Comp->Buildsystem)==wxNOT_FOUND)
            this->Buildsystem->Append(Comp->Buildsystem);
    if(this->Buildsystem->FindString("None")==wxNOT_FOUND)
        this->Buildsystem->Append("None");
    if(this->Buildsystem->GetCount()>0)
        this->Buildsystem->SetSelection(0);
}
/* End Function:Basic_Panel::Buildsystem_Set *********************************/

/* Function:Basic_Panel::On_Rename ********************************************
Description : Pop up a rename dialog, and rename this project.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
void Basic_Panel::On_Rename(class wxMouseEvent& Event)
{
    ptr_t Name_State;
    std::string Current;
    Main::Name_Dialog->Load(this->Name->GetValue().ToStdString());

    Name_State=Main::Name_Dialog->ShowModal();
    if(Name_State==wxID_OK)
    {
        Current=Main::Name_Dialog->Name_Get();
        /* Update config tree */
        Main::Config->Root_Text=Current;
        Main::Config->SetItemText(Main::Config->GetRootItem(),Current);
        /* Update basic config */
        this->Name->SetValue(Current);
    }
}
/* End Function:Basic_Panel::On_Rename ***************************************/
}
/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
