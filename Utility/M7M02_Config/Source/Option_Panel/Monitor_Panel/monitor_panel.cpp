/******************************************************************************
Filename    : monitor_panel.cpp
Author      : lbc
Date        : 24/04/2025
License     : Proprietary; confidential.
Description : Monitor panel implementation.
******************************************************************************/

/* Include *******************************************************************/
#define __HDR_DEF__
#include "rvm_cfg.hpp"
#undef __HDR_DEF__

#include "wx/wx.h"
#include "wx/notebook.h"
#include "wx/xml/xml.h"

#include "set"
#include "map"
#include "memory"

#define __HDR_DEF__
#include "Option_Panel/Monitor_Panel/monitor_panel.hpp"
#undef __HDR_DEF__

#define __HDR_CLASS__
#include "rvm_cfg.hpp"

#include "Option_Panel/Monitor_Panel/monitor_panel.hpp"

#include "Proj_Info/Proj_info.hpp"
#include "Proj_Info/Monitor/monitor.hpp"
#include "Plat_Info/plat_info.hpp"
#include "Plat_Info/Compatible/compatible.hpp"
#undef __HDR_CLASS__
/* End Include ***************************************************************/
namespace RVM_CFG
{
/* Function:Monitor_Panel::Monitor_Panel **************************************
Description : Constructor for basic notebook. This notebook include
              basic information, chip, extmem, shmem
Input       : class wxWindow* Parent - The parent window.
Output      : None.
Return      : None.
******************************************************************************/
/* void */ Monitor_Panel::Monitor_Panel(class wxWindow* Parent):
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
        this->Basic_Line3_Sizer=new class wxBoxSizer(wxHORIZONTAL);

        this->Code_Size_Sizer=new class wxBoxSizer(wxHORIZONTAL);
        this->Code_Size_Label=new class wxStaticText(this,wxID_ANY,_("Code Size"));
        this->Code_Size=new class wxTextCtrl(this,wxID_ANY);
        this->Code_Size->Bind(wxEVT_KILL_FOCUS, &Monitor_Panel::On_Text_Hex, this);
        this->Code_Size_Sizer->Add(this->Code_Size_Label,1,wxALL|wxALIGN_CENTER_VERTICAL,I2P(5));
        this->Code_Size_Sizer->Add(this->Code_Size,1,wxALL|wxALIGN_CENTER_VERTICAL,I2P(5));

        this->Data_Size_Sizer=new class wxBoxSizer(wxHORIZONTAL);
        this->Data_Size_Label=new class wxStaticText(this,wxID_ANY,_("Data Size"));
        this->Data_Size=new class wxTextCtrl(this,wxID_ANY);
        this->Data_Size->Bind(wxEVT_KILL_FOCUS, &Monitor_Panel::On_Text_Hex, this);
        this->Data_Size_Sizer->Add(this->Data_Size_Label,1,wxALL|wxALIGN_CENTER_VERTICAL,I2P(5));
        this->Data_Size_Sizer->Add(this->Data_Size,1,wxALL|wxALIGN_CENTER_VERTICAL,I2P(5));

        this->Init_Stack_Size_Sizer=new class wxBoxSizer(wxHORIZONTAL);
        this->Init_Stack_Size_Label=new class wxStaticText(this,wxID_ANY,_("Init Daemon Stack Size"));
        this->Init_Stack_Size=new class wxTextCtrl(this,wxID_ANY);
        this->Init_Stack_Size->Bind(wxEVT_KILL_FOCUS, &Monitor_Panel::On_Text_Hex, this);
        this->Init_Stack_Size_Sizer->Add(this->Init_Stack_Size_Label,1,wxALL|wxALIGN_CENTER_VERTICAL,I2P(5));
        this->Init_Stack_Size_Sizer->Add(this->Init_Stack_Size,1,wxALL|wxALIGN_CENTER_VERTICAL,I2P(5));

        this->Sftd_Stack_Size_Sizer=new class wxBoxSizer(wxHORIZONTAL);
        this->Sftd_Stack_Size_Label=new class wxStaticText(this,wxID_ANY,_("Safety Daemon Stack Size"));
        this->Sftd_Stack_Size=new class wxTextCtrl(this,wxID_ANY);
        this->Sftd_Stack_Size->Bind(wxEVT_KILL_FOCUS, &Monitor_Panel::On_Text_Hex, this);
        this->Sftd_Stack_Size_Sizer->Add(this->Sftd_Stack_Size_Label,1,wxALL|wxALIGN_CENTER_VERTICAL,I2P(5));
        this->Sftd_Stack_Size_Sizer->Add(this->Sftd_Stack_Size,1,wxALL|wxALIGN_CENTER_VERTICAL,I2P(5));

        this->Vmmd_Stack_Size_Sizer=new class wxBoxSizer(wxHORIZONTAL);
        this->Vmmd_Stack_Size_Label=new class wxStaticText(this,wxID_ANY,_("VMM Daemon Stack Size"));
        this->Vmmd_Stack_Size=new class wxTextCtrl(this,wxID_ANY);
        this->Vmmd_Stack_Size->Bind(wxEVT_KILL_FOCUS, &Monitor_Panel::On_Text_Hex, this);
        this->Vmmd_Stack_Size_Sizer->Add(this->Vmmd_Stack_Size_Label,1,wxALL|wxALIGN_CENTER_VERTICAL,I2P(5));
        this->Vmmd_Stack_Size_Sizer->Add(this->Vmmd_Stack_Size,1,wxALL|wxALIGN_CENTER_VERTICAL,I2P(5));

        this->Basic_Line1_Sizer->Add(this->Code_Size_Sizer,6,wxEXPAND);
        this->Basic_Line1_Sizer->AddStretchSpacer(1);
        this->Basic_Line1_Sizer->Add(this->Data_Size_Sizer,6,wxEXPAND);

        this->Basic_Line2_Sizer->Add(this->Init_Stack_Size_Sizer,6,wxEXPAND);
        this->Basic_Line2_Sizer->AddStretchSpacer(1);
        this->Basic_Line2_Sizer->Add(this->Sftd_Stack_Size_Sizer,6,wxEXPAND);

        this->Basic_Line3_Sizer->Add(this->Vmmd_Stack_Size_Sizer,6,wxEXPAND);
        this->Basic_Line3_Sizer->AddStretchSpacer(7);

        this->Basic_Sizer->Add(this->Basic_Line1_Sizer,0,wxEXPAND);
        this->Basic_Sizer->Add(this->Basic_Line2_Sizer,0,wxEXPAND);
        this->Basic_Sizer->Add(this->Basic_Line3_Sizer,0,wxEXPAND);

        /* Hypervisor options */
        this->Hyper_Sizer=new class wxStaticBoxSizer(wxVERTICAL,this,_("Hypervisor Options"));
        this->Hyper_Line1_Sizer=new class wxBoxSizer(wxHORIZONTAL);
        this->Hyper_Line2_Sizer=new class wxBoxSizer(wxHORIZONTAL);

        this->Extra_Captbl_Sizer=new class wxBoxSizer(wxHORIZONTAL);
        this->Extra_Captbl_Label=new class wxStaticText(this,wxID_ANY,_("Extra Capability Slot Number"));
        this->Extra_Captbl=new class wxTextCtrl(this,wxID_ANY);
        this->Extra_Captbl_Sizer->Add(this->Extra_Captbl_Label,1,wxALL|wxALIGN_CENTER_VERTICAL,I2P(5));
        this->Extra_Captbl_Sizer->Add(this->Extra_Captbl,1,wxALL|wxALIGN_CENTER_VERTICAL,I2P(5));

        this->Virt_Prio_Sizer=new class wxBoxSizer(wxHORIZONTAL);
        this->Virt_Prio_Label=new class wxStaticText(this,wxID_ANY,_("VM Priority Number"));
        this->Virt_Prio=new class wxChoice(this,wxID_ANY);
        this->Virt_Prio_Sizer->Add(this->Virt_Prio_Label,1,wxALL|wxALIGN_CENTER_VERTICAL,I2P(5));
        this->Virt_Prio_Sizer->Add(this->Virt_Prio,1,wxALL|wxALIGN_CENTER_VERTICAL,I2P(5));

        this->Virt_Event_Sizer=new class wxBoxSizer(wxHORIZONTAL);
        this->Virt_Event_Label=new class wxStaticText(this,wxID_ANY,_("VM Event Source Number"));
        this->Virt_Event=new class wxChoice(this,wxID_ANY);
        this->Virt_Event_Sizer->Add(this->Virt_Event_Label,1,wxALL|wxALIGN_CENTER_VERTICAL,I2P(5));
        this->Virt_Event_Sizer->Add(this->Virt_Event,1,wxALL|wxALIGN_CENTER_VERTICAL,I2P(5));

        this->Virt_Map_Sizer=new class wxBoxSizer(wxHORIZONTAL);
        this->Virt_Map_Label=new class wxStaticText(this,wxID_ANY,_("VM Vector Mapping Number"));
        this->Virt_Map=new class wxTextCtrl(this,wxID_ANY);
        this->Virt_Map_Sizer->Add(this->Virt_Map_Label,1,wxALL|wxALIGN_CENTER_VERTICAL,I2P(5));
        this->Virt_Map_Sizer->Add(this->Virt_Map,1,wxALL|wxALIGN_CENTER_VERTICAL,I2P(5));

        this->Hyper_Line1_Sizer->Add(this->Extra_Captbl_Sizer,6,wxEXPAND);
        this->Hyper_Line1_Sizer->AddStretchSpacer(1);
        this->Hyper_Line1_Sizer->Add(this->Virt_Prio_Sizer,6,wxEXPAND);

        this->Hyper_Line2_Sizer->Add(this->Virt_Event_Sizer,6,wxEXPAND);
        this->Hyper_Line2_Sizer->AddStretchSpacer(1);
        this->Hyper_Line2_Sizer->Add(this->Virt_Map_Sizer,6,wxEXPAND);

        this->Hyper_Sizer->Add(this->Hyper_Line1_Sizer,0,wxEXPAND);
        this->Hyper_Sizer->Add(this->Hyper_Line2_Sizer,0,wxEXPAND);

        /* Buildsystem options */
        this->Build_Sizer=new class wxStaticBoxSizer(wxVERTICAL,this,_("Buildsystem Options"));
        this->Build_Line1_Sizer=new class wxBoxSizer(wxHORIZONTAL);
        this->Build_Line2_Sizer=new class wxBoxSizer(wxHORIZONTAL);

        this->Toolchain_Sizer=new class wxBoxSizer(wxHORIZONTAL);
        this->Toolchain_Label=new class wxStaticText(this,wxID_ANY,_("Toolchain"));
        this->Toolchain=new class wxChoice(this,wxID_ANY);
        this->Bind(wxEVT_CHOICE, &Monitor_Panel::On_Toolchain, this, this->Toolchain->GetId());
        this->Toolchain_Sizer->Add(this->Toolchain_Label,1,wxALL|wxALIGN_CENTER_VERTICAL,I2P(5));
        this->Toolchain_Sizer->Add(this->Toolchain,1,wxALL|wxALIGN_CENTER_VERTICAL,I2P(5));

        this->Buildsystem_Sizer=new class wxBoxSizer(wxHORIZONTAL);
        this->Buildsystem_Label=new class wxStaticText(this,wxID_ANY,_("Buildsystem"));
        this->Buildsystem=new class wxChoice(this,wxID_ANY);
        this->Buildsystem_Sizer->Add(this->Buildsystem_Label,1,wxALL|wxALIGN_CENTER_VERTICAL,I2P(5));
        this->Buildsystem_Sizer->Add(this->Buildsystem,1,wxALL|wxALIGN_CENTER_VERTICAL,I2P(5));

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
        this->Optimization_Sizer->Add(this->Optimization_Label,1,wxALL|wxALIGN_CENTER_VERTICAL,I2P(5));
        this->Optimization_Sizer->Add(this->Optimization,1,wxALL|wxALIGN_CENTER_VERTICAL,I2P(5));

        this->Idle_Sleep_Enable_Sizer=new class wxBoxSizer(wxHORIZONTAL);
        this->Idle_Sleep_Enable_Label=new class wxStaticText(this,wxID_ANY,_("Idle Sleep Enable"));
        this->Idle_Sleep_Enable=new class wxCheckBox(this,wxID_ANY,wxEmptyString);
        this->Idle_Sleep_Enable_Sizer->Add(this->Idle_Sleep_Enable_Label,1,wxALL|wxALIGN_CENTER_VERTICAL,I2P(5));
        this->Idle_Sleep_Enable_Sizer->Add(this->Idle_Sleep_Enable,1,wxALL|wxALIGN_CENTER_VERTICAL,I2P(5));

        this->Build_Line1_Sizer->Add(this->Toolchain_Sizer,6,wxEXPAND);
        this->Build_Line1_Sizer->AddStretchSpacer(1);
        this->Build_Line1_Sizer->Add(this->Buildsystem_Sizer,6,wxEXPAND);

        this->Build_Line2_Sizer->Add(this->Optimization_Sizer,6,wxEXPAND);
        this->Build_Line2_Sizer->AddStretchSpacer(1);
        this->Build_Line2_Sizer->Add(this->Idle_Sleep_Enable_Sizer,6,wxEXPAND);

        this->Build_Sizer->Add(this->Build_Line1_Sizer,0,wxEXPAND);
        this->Build_Sizer->Add(this->Build_Line2_Sizer,0,wxEXPAND);

        /* Output options */
        this->Output_Sizer=new class wxStaticBoxSizer(wxVERTICAL,this,_("Output Options"));

        this->Project_Sizer=new class wxBoxSizer(wxHORIZONTAL);
        this->Project_Output_Label=new class wxStaticText(this,wxID_ANY,_("Project Output"));
        this->Project_Output=new class wxTextCtrl(this,wxID_ANY,_("./Monitor/Project/"));
        this->Project_Overwrite=new class wxCheckBox(this,wxID_ANY,_("Overwrite"));
        this->Project_Sizer->Add(this->Project_Output_Label,5,wxALL|wxALIGN_CENTER_VERTICAL,I2P(5));
        this->Project_Sizer->Add(this->Project_Output,16,wxALL|wxALIGN_CENTER_VERTICAL,I2P(5));
        this->Project_Sizer->Add(this->Project_Overwrite,3,wxALL|wxALIGN_CENTER_VERTICAL,I2P(5));

        this->Linker_Output_Sizer=new class wxBoxSizer(wxHORIZONTAL);
        this->Linker_Output_Label=new class wxStaticText(this,wxID_ANY,_("Linker Output"));
        this->Linker_Output=new class wxTextCtrl(this,wxID_ANY,_("./"));
        this->Linker_Output_Sizer->Add(this->Linker_Output_Label,5,wxALL|wxALIGN_CENTER_VERTICAL,I2P(5));
        this->Linker_Output_Sizer->Add(this->Linker_Output,16,wxALL|wxALIGN_CENTER_VERTICAL,I2P(5));
        this->Linker_Output_Sizer->AddStretchSpacer(3);

        this->Config_Header_Output_Sizer=new class wxBoxSizer(wxHORIZONTAL);
        this->Config_Header_Output_Label=new class wxStaticText(this,wxID_ANY,_("Config Header Output"));
        this->Config_Header_Output=new class wxTextCtrl(this,wxID_ANY,_("../Include/"));
        this->Config_Header_Output_Sizer->Add(this->Config_Header_Output_Label,5,wxALL|wxALIGN_CENTER_VERTICAL,I2P(5));
        this->Config_Header_Output_Sizer->Add(this->Config_Header_Output,16,wxALL|wxALIGN_CENTER_VERTICAL,I2P(5));
        this->Config_Header_Output_Sizer->AddStretchSpacer(3);

        this->Boot_Header_Output_Sizer=new class wxBoxSizer(wxHORIZONTAL);
        this->Boot_Header_Output_Label=new class wxStaticText(this,wxID_ANY,_("Boot Header Output"));
        this->Boot_Header_Output=new class wxTextCtrl(this,wxID_ANY,_("../Include/"));
        this->Boot_Header_Output_Sizer->Add(this->Boot_Header_Output_Label,5,wxALL|wxALIGN_CENTER_VERTICAL,I2P(5));
        this->Boot_Header_Output_Sizer->Add(this->Boot_Header_Output,16,wxALL|wxALIGN_CENTER_VERTICAL,I2P(5));
        this->Boot_Header_Output_Sizer->AddStretchSpacer(3);

        this->Boot_Source_Output_Sizer=new class wxBoxSizer(wxHORIZONTAL);
        this->Boot_Source_Output_Label=new class wxStaticText(this,wxID_ANY,_("Boot Source Output"));
        this->Boot_Source_Output=new class wxTextCtrl(this,wxID_ANY,_("../Source/"));
        this->Boot_Source_Output_Sizer->Add(this->Boot_Source_Output_Label,5,wxALL|wxALIGN_CENTER_VERTICAL,I2P(5));
        this->Boot_Source_Output_Sizer->Add(this->Boot_Source_Output,16,wxALL|wxALIGN_CENTER_VERTICAL,I2P(5));
        this->Boot_Source_Output_Sizer->AddStretchSpacer(3);

        this->Hook_Source_Sizer=new class wxBoxSizer(wxHORIZONTAL);
        this->Hook_Source_Output_Label=new class wxStaticText(this,wxID_ANY,_("Hook Source Output"));
        this->Hook_Source_Output=new class wxTextCtrl(this,wxID_ANY,_("../Source/"));
        this->Hook_Source_Overwrite=new class wxCheckBox(this,wxID_ANY,_("Overwrite"));
        this->Hook_Source_Sizer->Add(this->Hook_Source_Output_Label,5,wxALL|wxALIGN_CENTER_VERTICAL,I2P(5));
        this->Hook_Source_Sizer->Add(this->Hook_Source_Output,16,wxALL|wxALIGN_CENTER_VERTICAL,I2P(5));
        this->Hook_Source_Sizer->Add(this->Hook_Source_Overwrite,3,wxALL|wxALIGN_CENTER_VERTICAL,I2P(5));

        this->Output_Sizer->Add(this->Project_Sizer,0,wxEXPAND);
        this->Output_Sizer->Add(this->Linker_Output_Sizer,0,wxEXPAND);
        this->Output_Sizer->Add(this->Config_Header_Output_Sizer,0,wxEXPAND);
        this->Output_Sizer->Add(this->Boot_Header_Output_Sizer,0,wxEXPAND);
        this->Output_Sizer->Add(this->Boot_Source_Output_Sizer,0,wxEXPAND);
        this->Output_Sizer->Add(this->Hook_Source_Sizer,0,wxEXPAND);

        this->Main_Sizer->Add(this->Basic_Sizer,0,wxEXPAND|wxALL,I2P(5));
        this->Main_Sizer->Add(this->Hyper_Sizer,0,wxEXPAND|wxALL,I2P(5));
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
/* End Function:Monitor_Panel::Monitor_Panel *********************************/

/* Function:Monitor_Panel::~Monitor_Panel *************************************
Description : Destructor for sheet notebook.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
/* void */ Monitor_Panel::~Monitor_Panel(void)
{
    this->Unbind(wxEVT_CHOICE, &Monitor_Panel::On_Toolchain, this, this->Toolchain->GetId());
    /* Have to bind to the text itself because we are processing window events on it directly */
    this->Code_Size->Unbind(wxEVT_KILL_FOCUS, &Monitor_Panel::On_Text_Hex, this);
    this->Data_Size->Unbind(wxEVT_KILL_FOCUS, &Monitor_Panel::On_Text_Hex, this);
    this->Init_Stack_Size->Unbind(wxEVT_KILL_FOCUS, &Monitor_Panel::On_Text_Hex, this);
    this->Sftd_Stack_Size->Unbind(wxEVT_KILL_FOCUS, &Monitor_Panel::On_Text_Hex, this);
    this->Vmmd_Stack_Size->Unbind(wxEVT_KILL_FOCUS, &Monitor_Panel::On_Text_Hex, this);
}
/* End Function:Monitor_Panel::Monitor_Panel *********************************/

/* Function:Monitor_Panel::Load ***********************************************
Description : Load.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
void Monitor_Panel::Load(void)
{
    char Buf[32];

    std::sprintf(Buf, "0x%llX", Main::Proj_Info->Monitor->Code_Size);
    this->Code_Size->SetValue(Buf);
    std::sprintf(Buf, "0x%llX", Main::Proj_Info->Monitor->Data_Size);
    this->Data_Size->SetValue(Buf);
    std::sprintf(Buf, "0x%llX", Main::Proj_Info->Monitor->Init_Stack_Size);
    this->Init_Stack_Size->SetValue(Buf);
    std::sprintf(Buf, "0x%llX", Main::Proj_Info->Monitor->Sftd_Stack_Size);
    this->Sftd_Stack_Size->SetValue(Buf);
    std::sprintf(Buf, "0x%llX", Main::Proj_Info->Monitor->Vmmd_Stack_Size);
    this->Vmmd_Stack_Size->SetValue(Buf);

    std::sprintf(Buf, "%lld", Main::Proj_Info->Monitor->Extra_Captbl);
    this->Extra_Captbl->SetValue(Buf);

    /* If the selections are not within the allowed numbers, just give the first */
    std::sprintf(Buf, "%lld", Main::Proj_Info->Monitor->Virt_Prio);
    if(this->Virt_Prio->SetStringSelection(Buf)==false)
        this->Virt_Prio->SetSelection(0);
    std::sprintf(Buf, "%lld", Main::Proj_Info->Monitor->Virt_Event);
    if(this->Virt_Event->SetStringSelection(Buf)==false)
        this->Virt_Event->SetSelection(0);

    std::sprintf(Buf, "%lld", Main::Proj_Info->Monitor->Virt_Map);
    this->Virt_Map->SetValue(Buf);
    this->Toolchain->SetStringSelection(Main::Proj_Info->Monitor->Toolchain);
    this->Buildsystem_Set(this->Toolchain->GetStringSelection());
    this->Buildsystem->SetStringSelection(Main::Proj_Info->Monitor->Buildsystem);
    this->Optimization->SetSelection(Main::Proj_Info->Monitor->Optimization);

    this->Project_Output->SetValue(Main::Proj_Info->Monitor->Project_Output);
    this->Project_Overwrite->SetValue(Main::Proj_Info->Monitor->Project_Overwrite);
    this->Linker_Output->SetValue(Main::Proj_Info->Monitor->Linker_Output);
    this->Config_Header_Output->SetValue(Main::Proj_Info->Monitor->Config_Header_Output);
    this->Boot_Header_Output->SetValue(Main::Proj_Info->Monitor->Boot_Header_Output);
    this->Boot_Source_Output->SetValue(Main::Proj_Info->Monitor->Boot_Source_Output);
    this->Hook_Source_Output->SetValue(Main::Proj_Info->Monitor->Hook_Source_Output);
    this->Hook_Source_Overwrite->SetValue(Main::Proj_Info->Monitor->Hook_Source_Overwrite);
}
/* End Function:Monitor_Panel::Load ******************************************/

/* Function:Monitor_Panel::Check **********************************************
Description : Check the basic information settings.
Input       : None.
Output      : None.
Return      : ret_t - 0 for OK, -1 for error.
******************************************************************************/
ret_t Monitor_Panel::Check(void)
{
    ptr_t Virt_Map_Val;
    ptr_t Virt_Event_Val;

    if(Main::Hex_Pos_Check(this,this->Code_Size->GetValue(),_("Monitor Config"),_("Code size"))!=0)
        return -1;
    if(Main::Hex_Pos_Check(this,this->Data_Size->GetValue(),_("Monitor Config"),_("Data size"))!=0)
        return -1;
    if(Main::Hex_Pos_Check(this,this->Init_Stack_Size->GetValue(),_("Monitor Config"),_("Init daemon stack size"))!=0)
        return -1;
    if(Main::Hex_Pos_Check(this,this->Init_Stack_Size->GetValue(),_("Monitor Config"),_("Init daemon stack size"))!=0)
        return -1;
    if(Main::Hex_Pos_Check(this,this->Sftd_Stack_Size->GetValue(),_("Monitor Config"),_("Safety daemon stack size"))!=0)
        return -1;
    if(Main::Hex_Pos_Check(this,this->Vmmd_Stack_Size->GetValue(),_("Monitor Config"),_("VMM daemon stack size"))!=0)
        return -1;

    if(Main::Dec_Check(this,this->Extra_Captbl->GetValue(),_("Monitor Config"),_("Extra capability slot"))!=0)
        return -1;

    if(Main::Dec_Pos_Check(this,this->Virt_Map->GetValue(),_("Monitor Config"),_("VM interrupt map block"))!=0)
        return -1;

    /* Virtual Map must be greater than or equal to Virtual Event */
    Virt_Event_Val=std::stoull(std::string(this->Virt_Event->GetStringSelection()));
    Virt_Map_Val=std::stoull(std::string(this->Virt_Map->GetValue()));
    if(Virt_Map_Val<Virt_Event_Val)
    {
        Main::Msgbox_Show(this, MSGBOX_ERROR,
                          _("Monitor Config"),
                          _("Virtual map must be greater than or equal to the number of event sources."));
        return -1;
    }

    if(this->Project_Output->GetValue()=="")
    {
        Main::Msgbox_Show(this,MSGBOX_ERROR,
                          _("Monitor Config"),
                          _("The project output folder is unspecified."));
        return -1;
    }

    if(this->Linker_Output->GetValue()=="")
    {
        Main::Msgbox_Show(this,MSGBOX_ERROR,
                          _("Monitor Config"),
                          _("The linker output folder is unspecified."));
        return -1;
    }

    if(this->Config_Header_Output->GetValue()=="")
    {
        Main::Msgbox_Show(this,MSGBOX_ERROR,
                          _("Monitor Config"),
                          _("The config header output folder is unspecified."));
        return -1;
    }

    if(this->Boot_Header_Output->GetValue()=="")
    {
        Main::Msgbox_Show(this,MSGBOX_ERROR,
                          _("Monitor Config"),
                          _("The boot header output folder is unspecified."));
        return -1;
    }

    if(this->Boot_Source_Output->GetValue()=="")
    {
        Main::Msgbox_Show(this,MSGBOX_ERROR,
                          _("Monitor Config"),
                          _("The boot source output folder is unspecified."));
        return -1;
    }

    if(this->Hook_Source_Output->GetValue()=="")
    {
        Main::Msgbox_Show(this,MSGBOX_ERROR,
                          _("Monitor Config"),
                          _("The hook source output folder is unspecified."));
        return -1;
    }

    return 0;
}
/* End Function:Monitor_Panel::Check *****************************************/

/* Function:Monitor_Panel::Save ***********************************************
Description : Save.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
void Monitor_Panel::Save(void)
{
    Main::Proj_Info->Monitor->Code_Size=std::stoull(this->Code_Size->GetValue().ToStdString(),0,0);
    Main::Proj_Info->Monitor->Data_Size=std::stoull(this->Data_Size->GetValue().ToStdString(),0,0);
    Main::Proj_Info->Monitor->Init_Stack_Size=std::stoull(this->Init_Stack_Size->GetValue().ToStdString(),0,0);
    Main::Proj_Info->Monitor->Sftd_Stack_Size=std::stoull(this->Sftd_Stack_Size->GetValue().ToStdString(),0,0);
    Main::Proj_Info->Monitor->Vmmd_Stack_Size=std::stoull(this->Vmmd_Stack_Size->GetValue().ToStdString(),0,0);
    Main::Proj_Info->Monitor->Extra_Captbl=std::stoull(this->Extra_Captbl->GetValue().ToStdString(),0,0);
    Main::Proj_Info->Monitor->Virt_Prio=std::stoull(this->Virt_Prio->GetStringSelection().ToStdString(),0,0);
    Main::Proj_Info->Monitor->Virt_Event=std::stoull(this->Virt_Event->GetStringSelection().ToStdString(),0,0);
    Main::Proj_Info->Monitor->Virt_Map=std::stoull(this->Virt_Map->GetValue().ToStdString(),0,0);
    Main::Proj_Info->Monitor->Buildsystem=this->Buildsystem->GetStringSelection();
    Main::Proj_Info->Monitor->Toolchain=this->Toolchain->GetStringSelection();
    Main::Proj_Info->Monitor->Optimization=this->Optimization->GetSelection();

    Main::Proj_Info->Monitor->Project_Output=this->Project_Output->GetValue();
    Main::Proj_Info->Monitor->Project_Overwrite=this->Project_Overwrite->GetValue();
    Main::Proj_Info->Monitor->Linker_Output=this->Linker_Output->GetValue();
    Main::Proj_Info->Monitor->Config_Header_Output=this->Config_Header_Output->GetValue();
    Main::Proj_Info->Monitor->Boot_Header_Output=this->Boot_Header_Output->GetValue();
    Main::Proj_Info->Monitor->Boot_Source_Output=this->Boot_Source_Output->GetValue();
    Main::Proj_Info->Monitor->Hook_Source_Output=this->Hook_Source_Output->GetValue();
    Main::Proj_Info->Monitor->Hook_Source_Overwrite=this->Hook_Source_Overwrite->GetValue();
}
/* End Function:Monitor_Panel::Save ******************************************/

/* Function:Monitor_Panel::Toolchain_Set **************************************
Description : Set the collection of possible tool chains.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
void Monitor_Panel::Toolchain_Set(void)
{
    std::vector<std::string> Tool_Avail;

    wxLogDebug("Kernel_Panel::Toolchain_Set");

    /* Ask for possible toolchains */
    Main::Plat_Info->Toolchain_Native(Tool_Avail);
    Main::Choice_Refill(this->Toolchain,Tool_Avail);
}
/* End Function:Monitor_Panel::Toolchain_Set *********************************/

/* Function:Monitor_Panel::Buildsystem_Set ************************************
Description : Set build system given the toolchain choice.
Input       : const class wxString& Toolchain - The chosen toolchain.
Output      : None.
Return      : None.
******************************************************************************/
void Monitor_Panel::Buildsystem_Set(const class wxString& Toolchain)
{
    std::vector<std::string> Build_Avail;

    wxLogDebug("Kernel_Panel::Buildsystem_Set");

    /* Ask for possible toolchains */
    Main::Plat_Info->Buildsystem_Native(std::string(Toolchain),Build_Avail);
    Main::Choice_Refill(this->Buildsystem,Build_Avail);
}
/* End Function:Monitor_Panel::Buildsystem_Set *******************************/

/* Function:Monitor_Panel::Virt_Prio_Set **************************************
Description : Set virtual priority according to word length.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
void Monitor_Panel::Virt_Prio_Set(void)
{
    cnt_t Count;

    this->Virt_Prio->Clear();

    /* We allow priorities from 1-4 wordlength within the GUI */
    for(Count=1;Count<=4;Count++)
        this->Virt_Prio->Append(std::to_string(Main::Plat_Info->Wordlength*Count));

    this->Virt_Prio->SetSelection(0);
}
/* End Function:Monitor_Panel::Kernel_Prio_Set *******************************/

/* Function:Monitor_Panel::Virt_Event_Set *************************************
Description : Set virtual event according to word length.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
void Monitor_Panel::Virt_Event_Set(void)
{
    cnt_t Count;

    this->Virt_Event->Clear();

    /* We allow events from 1-4 wordlength within the GUI */
    for(Count=1;Count<=4;Count++)
        this->Virt_Event->Append(std::to_string(Main::Plat_Info->Wordlength*Count));

    this->Virt_Prio->SetSelection(0);
}
/* End Function:Monitor_Panel::Virt_Event_Set ********************************/

/* Function:Monitor_Panel::On_Text_Hex ****************************************
Description : wxEVT_KILL_FOCUS handler for all text input boxes that should
              normally contain a hex integer.
Input       : class wxFocusEvent& Event - The event.
Output      : None.
Return      : None.
******************************************************************************/
void Monitor_Panel::On_Text_Hex(class wxFocusEvent& Event)
{
    class wxTextCtrl* Text;

    Text=dynamic_cast<class wxTextCtrl*>(Event.GetEventObject());
    if(Text!=nullptr)
        Text->SetValue(Main::Num2Hex(std::string(Text->GetValue())));

    Event.Skip();
}
/* End Function:Monitor_Panel::On_Text_Hex ***********************************/

/* Function:Monitor_Panel::On_Toolchain ***************************************
Description : wxEVT_CHOICE handler for 'Toolchain'.
Input       : class wxCommandEvent& Event - The event.
Output      : None.
Return      : None.
******************************************************************************/
void Monitor_Panel::On_Toolchain(class wxCommandEvent& Event)
{
    class wxString Toolchain;
    class wxString Buildsystem;

    Toolchain=this->Toolchain->GetStringSelection();
    Buildsystem=this->Buildsystem->GetStringSelection();

    /* Recreate options if the current one is invalid */
    if(Main::Plat_Info->Compat_Check(std::string(Toolchain), std::string(Buildsystem))!=0)
        this->Buildsystem_Set(Toolchain);
}
/* End Function:Monitor_Panel::On_Toolchain **********************************/
}
/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
