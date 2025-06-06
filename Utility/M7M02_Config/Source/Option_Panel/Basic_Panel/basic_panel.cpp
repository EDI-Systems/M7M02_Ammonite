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
Description : Constructor for basic panel.
Input       : class wxWindow* Parent - The parent window.
Output      : None.
Return      : None.
******************************************************************************/
/* void */ Basic_Panel::Basic_Panel(class wxWindow* Parent):
wxPanel(Parent,wxID_ANY)
{
    try
    {
        this->SetBackgroundColour(Parent->GetBackgroundColour());

        this->Main_Sizer=new class wxBoxSizer(wxVERTICAL);

        /* Basic options */
        this->Basic_Sizer=new class wxStaticBoxSizer(wxVERTICAL,this,_("Basic Options"));

        this->Basic_Line1_Sizer=new class wxBoxSizer(wxHORIZONTAL);
        this->Basic_Line2_Sizer=new class wxBoxSizer(wxHORIZONTAL);

        this->Name_Sizer=new class wxBoxSizer(wxHORIZONTAL);
        this->Name_Label=new class wxStaticText(this,wxID_ANY,_("Project Name"));
        this->Name=new class wxTextCtrl(this,wxID_ANY,wxEmptyString,wxDefaultPosition,wxDefaultSize,wxTE_READONLY);
        this->Name->Bind(wxEVT_LEFT_DOWN, &Basic_Panel::On_Name_Text, this);
        this->Name_Sizer->Add(this->Name_Label,1,wxALL|wxALIGN_CENTER_VERTICAL,I2P(5));
        this->Name_Sizer->Add(this->Name,1,wxALL|wxALIGN_CENTER_VERTICAL,I2P(5));

        this->Assert_Enable_Sizer=new class wxBoxSizer(wxHORIZONTAL);
        this->Assert_Enable_Label=new class wxStaticText(this,wxID_ANY,_("Enable Debug Asserts"));
        this->Assert_Enable=new class wxCheckBox(this,wxID_ANY,wxEmptyString);
        this->Assert_Enable_Sizer->Add(this->Assert_Enable_Label,1,wxALL|wxALIGN_CENTER_VERTICAL,I2P(5));
        this->Assert_Enable_Sizer->Add(this->Assert_Enable,1,wxALL|wxALIGN_CENTER_VERTICAL,I2P(5));

        this->Pgtbl_Raw_Enable_Sizer=new class wxBoxSizer(wxHORIZONTAL);
        this->Pgtbl_Raw_Enable_Label=new class wxStaticText(this,wxID_ANY,_("Enable Raw Page Table"));
        this->Pgtbl_Raw_Enable=new class wxCheckBox(this,wxID_ANY,wxEmptyString);
        this->Pgtbl_Raw_Enable_Sizer->Add(this->Pgtbl_Raw_Enable_Label,1,wxALL|wxALIGN_CENTER_VERTICAL,I2P(5));
        this->Pgtbl_Raw_Enable_Sizer->Add(this->Pgtbl_Raw_Enable,1,wxALL|wxALIGN_CENTER_VERTICAL,I2P(5));

        this->Debug_Log_Enable_Sizer=new class wxBoxSizer(wxHORIZONTAL);
        this->Debug_Log_Enable_Label=new class wxStaticText(this,wxID_ANY,_("Enable Debug Logging"));
        this->Debug_Log_Enable=new class wxCheckBox(this,wxID_ANY,wxEmptyString);
        this->Debug_Log_Enable_Sizer->Add(this->Debug_Log_Enable_Label,1,wxALL|wxALIGN_CENTER_VERTICAL,I2P(5));
        this->Debug_Log_Enable_Sizer->Add(this->Debug_Log_Enable,1,wxALL|wxALIGN_CENTER_VERTICAL,I2P(5));

        this->Basic_Line1_Sizer->Add(this->Name_Sizer,6,wxEXPAND);
        this->Basic_Line1_Sizer->AddStretchSpacer(1);
        this->Basic_Line1_Sizer->Add(this->Assert_Enable_Sizer,6,wxEXPAND);

        this->Basic_Line2_Sizer->Add(this->Pgtbl_Raw_Enable_Sizer,6,wxEXPAND);
        this->Basic_Line2_Sizer->AddStretchSpacer(1);
        this->Basic_Line2_Sizer->Add(this->Debug_Log_Enable_Sizer,6,wxEXPAND);

        this->Basic_Sizer->Add(this->Basic_Line1_Sizer,0,wxEXPAND);
        this->Basic_Sizer->Add(this->Basic_Line2_Sizer,0,wxEXPAND);

        /* Workspace options */
        this->Workspace_Sizer=new class wxStaticBoxSizer(wxVERTICAL,this,_("Workspace Options"));
        this->Workspace_Line1_Sizer=new class wxBoxSizer(wxHORIZONTAL);

        this->Buildsystem_Sizer=new class wxBoxSizer(wxHORIZONTAL);
        this->Buildsystem_Label=new class wxStaticText(this,wxID_ANY,_("Buildsystem"));
        this->Buildsystem=new class wxChoice(this,wxID_ANY);
        this->Buildsystem_Sizer->Add(this->Buildsystem_Label,1,wxALL|wxALIGN_CENTER_VERTICAL,I2P(5));
        this->Buildsystem_Sizer->Add(this->Buildsystem,1,wxALL|wxALIGN_CENTER_VERTICAL,I2P(5));

        this->Workspace_Overwrite_Sizer=new class wxBoxSizer(wxHORIZONTAL);
        this->Workspace_Overwrite_Label=new class wxStaticText(this,wxID_ANY,_("Workspace Overwrite"));
        this->Workspace_Overwrite=new class wxCheckBox(this,wxID_ANY,wxEmptyString);
        this->Workspace_Overwrite_Sizer->Add(this->Workspace_Overwrite_Label,1,wxALL|wxALIGN_CENTER_VERTICAL,I2P(5));
        this->Workspace_Overwrite_Sizer->Add(this->Workspace_Overwrite,1,wxALL|wxALIGN_CENTER_VERTICAL,I2P(5));

        this->Coprocessor_Sizer=new class wxBoxSizer(wxHORIZONTAL);

        this->Workspace_Line1_Sizer->Add(Buildsystem_Sizer,6,wxEXPAND);
        this->Workspace_Line1_Sizer->AddStretchSpacer(1);
        this->Workspace_Line1_Sizer->Add(Workspace_Overwrite_Sizer,6,wxEXPAND);
        this->Workspace_Sizer->Add(this->Workspace_Line1_Sizer,0,wxEXPAND);
        this->Workspace_Sizer->Add(this->Coprocessor_Sizer,0,wxEXPAND);

        /* Target information */
        this->Target_Sizer=new class wxStaticBoxSizer(wxVERTICAL,this,_("Target Information"));
        this->Target_Line1_Sizer=new class wxBoxSizer(wxHORIZONTAL);
        this->Target_Line2_Sizer=new class wxBoxSizer(wxHORIZONTAL);

        this->Platform_Sizer=new class wxBoxSizer(wxHORIZONTAL);
        this->Platform_Label=new class wxStaticText(this,wxID_ANY,_("Platform"));
        this->Platform=new class wxTextCtrl(this,wxID_ANY,wxEmptyString,wxDefaultPosition,wxDefaultSize,wxTE_READONLY);
        this->Platform_Sizer->Add(this->Platform_Label,1,wxALL|wxALIGN_CENTER_VERTICAL,I2P(5));
        this->Platform_Sizer->Add(this->Platform,1,wxALL|wxALIGN_CENTER_VERTICAL,I2P(5));

        this->Class_Sizer=new class wxBoxSizer(wxHORIZONTAL);
        this->Class_Label=new class wxStaticText(this,wxID_ANY,_("Chip Class"));
        this->Class=new class wxTextCtrl(this,wxID_ANY,wxEmptyString,wxDefaultPosition,wxDefaultSize,wxTE_READONLY);
        this->Class_Sizer->Add(this->Class_Label,1,wxALL|wxALIGN_CENTER_VERTICAL,I2P(5));
        this->Class_Sizer->Add(this->Class,1,wxALL|wxALIGN_CENTER_VERTICAL,I2P(5));

        this->Chip_Name_Sizer=new class wxBoxSizer(wxHORIZONTAL);
        this->Chip_Name_Label=new class wxStaticText(this,wxID_ANY,_("Chip Name"));
        this->Chip_Name=new class wxTextCtrl(this,wxID_ANY,wxEmptyString,wxDefaultPosition,wxDefaultSize,wxTE_READONLY);
        this->Chip_Name_Sizer->Add(this->Chip_Name_Label,1,wxALL|wxALIGN_CENTER_VERTICAL,I2P(5));
        this->Chip_Name_Sizer->Add(this->Chip_Name,1,wxALL|wxALIGN_CENTER_VERTICAL,I2P(5));

        this->Target_Line1_Sizer->Add(this->Platform_Sizer,6,wxEXPAND);
        this->Target_Line1_Sizer->AddStretchSpacer(1);
        this->Target_Line1_Sizer->Add(this->Class_Sizer,6,wxEXPAND);

        this->Target_Line2_Sizer->Add(this->Chip_Name_Sizer,6,wxEXPAND);
        this->Target_Line2_Sizer->AddStretchSpacer(7);

        this->Target_Sizer->Add(this->Target_Line1_Sizer,0,wxEXPAND);
        this->Target_Sizer->Add(this->Target_Line2_Sizer,0,wxEXPAND);

        /* Target-specific options - wait for later */
        this->Specific_Sizer=new class wxStaticBoxSizer(wxVERTICAL,this,_("Target-specific Options"));

        this->Main_Sizer->Add(this->Basic_Sizer,0,wxEXPAND|wxALL,I2P(5));
        this->Main_Sizer->Add(this->Workspace_Sizer,0,wxEXPAND|wxALL,I2P(5));
        this->Main_Sizer->Add(this->Target_Sizer,0,wxEXPAND|wxALL,I2P(5));
        this->Main_Sizer->Add(this->Specific_Sizer,0,wxEXPAND|wxALL,I2P(5));

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
Description : Destructor for basic panel.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
/* void */ Basic_Panel::~Basic_Panel(void)
{
    /* Have to bind to the text itself because we are processing window events on it directly */
    this->Name->Unbind(wxEVT_LEFT_DOWN,&Basic_Panel::On_Name_Text,this);
}
/* End Function:Basic_Panel::Basic_Panel *************************************/

/* Function:Basic_Panel::Load *************************************************
Description : Load the information onto the basic panel.
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
    Main::Coprocessor_Load(Main::Proj_Info->Chip->Coprocessor,this->Coprocessor);

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
    if(Main::Idtfr_Check(std::string(this->Name->GetValue().Upper()))!=0)
    {
        Main::Msgbox_Show(this, MSGBOX_ERROR,
                                          _("Basic Config"),
                                          _("The project name is not a valid C identifier."));
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
            try
            {
                Val=std::stoull(Iter->second->GetValue().ToStdString(),0,0);
                Floor=std::stoull(Conf->Range[0],0,0);
                Ceiling=std::stoull(Conf->Range[1],0,0);
            }
            catch(std::exception& Exc)
            {
                Main::Msgbox_Show(this, MSGBOX_ERROR,
                                  _("Basic Config"),
                                  "\""+Conf->Name+"\""+_(" must be greater than or equal to "+Conf->Range[0]));
                return -1;
            }

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

    /* Check buildsystem - only when all agree can we choose something other than "None" */
    Buildsystem=this->Buildsystem->GetStringSelection();
    if(Buildsystem!="None")
    {
        if(Main::Proj_Info->Kernel->Buildsystem!=Main::Proj_Info->Monitor->Buildsystem)
        {
            Main::Msgbox_Show(this, MSGBOX_ERROR,
                              _("Basic Config"),
                              _("Workspace buildsystem must agree with all subprojects, otherwise only \"None\" can be chosen."));
            return -1;
        }

        for(std::unique_ptr<class Native>&Native:Main::Proj_Info->Native)
        {
            if(Main::Proj_Info->Kernel->Buildsystem!=Native->Buildsystem)
            {
                Main::Msgbox_Show(this, MSGBOX_ERROR,
                                  _("Basic Config"),
                                  _("Workspace buildsystem must agree with all subprojects, otherwise only \"None\" can be chosen."));
                return -1;
            }
        }

        for(std::unique_ptr<class Virtual>&Virtual:Main::Proj_Info->Virtual)
        {
            if(Main::Proj_Info->Kernel->Buildsystem!=Virtual->Buildsystem)
            {
                Main::Msgbox_Show(this, MSGBOX_ERROR,
                                  _("Basic Config"),
                                  _("Workspace buildsystem must agree with all subprojects, otherwise only \"None\" can be chosen."));
                return -1;
            }
        }
    }

    return 0;
}
/* End Function:Basic_Panel::Check *******************************************/

/* Function:Basic_Panel::Save *************************************************
Description : Save.
Input       : None.
Output      : None.
Return      : ret_t - 0 for OK, -1 for error.
******************************************************************************/
void Basic_Panel::Save(void)
{
    std::vector<std::string> Cop_Temp;
    std::map<std::string,std::string> Config_Temp;
    std::map<std::string,class wxChoice*>::iterator Conf_Choice_It;
    std::map<std::string,class wxTextCtrl*>::iterator Conf_Range_It;
    std::map<std::string,class wxCheckBox*>::iterator Cop_Iter;

    /* Project info */
    Main::Proj_Info->Name=this->Name->GetValue();
    Main::Proj_Info->Assert_Enable=this->Assert_Enable->GetValue();;
    Main::Proj_Info->Debug_Log_Enable=this->Debug_Log_Enable->GetValue();
    Main::Proj_Info->Pgtbl_Raw_Enable=this->Pgtbl_Raw_Enable->GetValue();
    Main::Proj_Info->Buildsystem=this->Buildsystem->GetStringSelection();
    Main::Proj_Info->Workspace_Overwrite=this->Workspace_Overwrite->GetValue();

    /* Chip target */
    Main::Proj_Info->Chip->Platform=this->Platform->GetValue();
    Main::Proj_Info->Chip->Class=this->Class->GetValue();
    Main::Proj_Info->Chip->Name=this->Chip_Name->GetValue();

    /* Register enabled coprocessors */
    Main::Coprocessor_Save(Main::Proj_Info->Chip->Coprocessor,this->Coprocessor);

    /* Register configs */
    Main::Proj_Info->Chip->Config.clear();
    for(const std::pair<const std::string, class wxTextCtrl*>& Config:this->Config_Range)
        Main::Proj_Info->Chip->Config.insert(std::make_pair(Config.first,std::string(Config.second->GetValue())));
    for(const std::pair<const std::string, class wxChoice*>& Config:this->Config_Choice)
        Main::Proj_Info->Chip->Config.insert(std::make_pair(Config.first,std::string(Config.second->GetStringSelection())));
}
/* End Function:Basic_Panel::Save ********************************************/

/* Function:Basic_Panel::Coprocessor_Set **************************************
Description : Set coprocessors.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
void Basic_Panel::Coprocessor_Set(void)
{
    Main::Coprocessor_Refill(this,
                             this->Coprocessor_Label,
                             this->Coprocessor_Sizer,
                             this->Coprocessor);

    this->Layout();
}
/* End Function:Basic_Panel::Coprocessor_Set *********************************/

/* Function:Basic_Panel::Buildsystem_Set **************************************
Description : Set build system.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
void Basic_Panel::Buildsystem_Set(void)
{
    std::vector<std::string> Build_Avail;

    wxLogDebug("Basic_Panel::Buildsystem_Set");

    /* Ask for possible buildsystems */
    Main::Plat_Info->Buildsystem_Workspace(Build_Avail);
    Main::Choice_Refill(this->Buildsystem,Build_Avail);
    this->Buildsystem->Append("None");
}
/* End Function:Basic_Panel::Buildsystem_Set *********************************/

/* Function:Basic_Panel::Config_Set *******************************************
Description : Set chip config.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
void Basic_Panel::Config_Set(void)
{
    class wxChoice*Temp_Choice;
    class wxTextCtrl* Temp_Text;
    class wxBoxSizer* Temp_Sizer;
    class wxStaticText* Temp_Label;

    /* Clear */
    this->Specific_Sizer->Clear(true);
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

        this->Specific_Sizer->Add(Temp_Sizer, 0, wxEXPAND);
    }
    this->Layout();
}
/* End Function:Basic_Panel::Config_Set **************************************/

/* Function:Basic_Panel::On_Name_Text *****************************************
Description : wxEVT_LEFT_DOWN handler for 'Name_Text'.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
void Basic_Panel::On_Name_Text(class wxMouseEvent& Event)
{
    ptr_t Name_State;
    std::string Current;

    Current=this->Name->GetValue();
    Main::Name_Dialog->Load(Current);

    Name_State=Main::Name_Dialog->ShowModal();
    if(Name_State==wxID_OK)
    {
        Current=Main::Name_Dialog->Name_Get();

        /* Update config tree */
        Main::Config_Tree->SetItemText(Main::Config_Tree->GetRootItem(),Current);
        /* Update basic config */
        this->Name->SetValue(Current);
    }
}
/* End Function:Basic_Panel::On_Name_Text ************************************/
}
/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
