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
#include "Proj_Info/Proj_info.hpp"
#include "Proj_Info/Monitor/monitor.hpp"
#include "Plat_Info/plat_info.hpp"
#include "Plat_Info/Compatible/compatible.hpp"
#include "Option_Panel/Monitor_Panel/monitor_panel.hpp"
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
        this->Main_Sizer=new class wxBoxSizer(wxVERTICAL);

        /* Group 1 */
        this->Sizer1=new class wxStaticBoxSizer(wxVERTICAL,this,_("Sizer1"));
        this->Sizer1_1=new class wxBoxSizer(wxHORIZONTAL);
        this->Sizer1_2=new class wxBoxSizer(wxHORIZONTAL);
        this->Sizer1_3=new class wxBoxSizer(wxHORIZONTAL);

        this->Code_Size_Sizer=new class wxBoxSizer(wxHORIZONTAL);
        this->Code_Size_Label=new class wxStaticText(this,wxID_ANY,_("Code Size"));
        this->Code_Size=new class wxTextCtrl(this,wxID_ANY);
        this->Code_Size_Sizer->Add(this->Code_Size_Label,1,wxALL|wxALIGN_CENTER_VERTICAL,I2P(5));
        this->Code_Size_Sizer->Add(this->Code_Size,2,wxALL|wxALIGN_CENTER_VERTICAL,I2P(5));

        this->Data_Size_Sizer=new class wxBoxSizer(wxHORIZONTAL);
        this->Data_Size_Label=new class wxStaticText(this,wxID_ANY,_("Data Size"));
        this->Data_Size=new class wxTextCtrl(this,wxID_ANY);
        this->Data_Size_Sizer->Add(this->Data_Size_Label,1,wxALL|wxALIGN_CENTER_VERTICAL,I2P(5));
        this->Data_Size_Sizer->Add(this->Data_Size,2,wxALL|wxALIGN_CENTER_VERTICAL,I2P(5));

        this->Init_Stack_Size_Sizer=new class wxBoxSizer(wxHORIZONTAL);
        this->Init_Stack_Size_Label=new class wxStaticText(this,wxID_ANY,_("Initial Stack Size"));
        this->Init_Stack_Size=new class wxTextCtrl(this,wxID_ANY);
        this->Init_Stack_Size_Sizer->Add(this->Init_Stack_Size_Label,1,wxALL|wxALIGN_CENTER_VERTICAL,I2P(5));
        this->Init_Stack_Size_Sizer->Add(this->Init_Stack_Size,2,wxALL|wxALIGN_CENTER_VERTICAL,I2P(5));

        this->Sftd_Stack_Size_Sizer=new class wxBoxSizer(wxHORIZONTAL);
        this->Sftd_Stack_Size_Label=new class wxStaticText(this,wxID_ANY,_("SFTD Stack Size"));
        this->Sftd_Stack_Size=new class wxTextCtrl(this,wxID_ANY);
        this->Sftd_Stack_Size_Sizer->Add(this->Sftd_Stack_Size_Label,1,wxALL|wxALIGN_CENTER_VERTICAL,I2P(5));
        this->Sftd_Stack_Size_Sizer->Add(this->Sftd_Stack_Size,2,wxALL|wxALIGN_CENTER_VERTICAL,I2P(5));

        this->Vmmd_Stack_Size_Sizer=new class wxBoxSizer(wxHORIZONTAL);
        this->Vmmd_Stack_Size_Label=new class wxStaticText(this,wxID_ANY,_("VMMD Stack Size"));
        this->Vmmd_Stack_Size=new class wxTextCtrl(this,wxID_ANY);
        this->Vmmd_Stack_Size_Sizer->Add(this->Vmmd_Stack_Size_Label,1,wxALL|wxALIGN_CENTER_VERTICAL,I2P(5));
        this->Vmmd_Stack_Size_Sizer->Add(this->Vmmd_Stack_Size,2,wxALL|wxALIGN_CENTER_VERTICAL,I2P(5));

        this->Sizer1_1->Add(this->Code_Size_Sizer,6,wxEXPAND);
        this->Sizer1_1->AddStretchSpacer(1);
        this->Sizer1_1->Add(this->Data_Size_Sizer,6,wxEXPAND);
        this->Sizer1_2->Add(this->Init_Stack_Size_Sizer,6,wxEXPAND);
        this->Sizer1_2->AddStretchSpacer(1);
        this->Sizer1_2->Add(this->Sftd_Stack_Size_Sizer,6,wxEXPAND);
        this->Sizer1_3->Add(this->Vmmd_Stack_Size_Sizer,6,wxEXPAND);
        this->Sizer1_3->AddStretchSpacer(7);
        this->Sizer1->Add(this->Sizer1_1,0,wxEXPAND);
        this->Sizer1->Add(this->Sizer1_2,0,wxEXPAND);
        this->Sizer1->Add(this->Sizer1_3,0,wxEXPAND);

        /* Group 2 */
        this->Sizer2=new class wxStaticBoxSizer(wxVERTICAL,this,_("Sizer2"));
        this->Sizer2_1=new class wxBoxSizer(wxHORIZONTAL);
        this->Sizer2_2=new class wxBoxSizer(wxHORIZONTAL);

        this->Extra_Captbl_Sizer=new class wxBoxSizer(wxHORIZONTAL);
        this->Extra_Captbl_Label=new class wxStaticText(this,wxID_ANY,_("Extra Captbl"));
        this->Extra_Captbl=new class wxTextCtrl(this,wxID_ANY);
        this->Extra_Captbl_Sizer->Add(this->Extra_Captbl_Label,1,wxALL|wxALIGN_CENTER_VERTICAL,I2P(5));
        this->Extra_Captbl_Sizer->Add(this->Extra_Captbl,2,wxALL|wxALIGN_CENTER_VERTICAL,I2P(5));

        this->Virt_Prio_Sizer=new class wxBoxSizer(wxHORIZONTAL);
        this->Virt_Prio_Label=new class wxStaticText(this,wxID_ANY,_("VM Priority"));
        this->Virt_Prio=new class wxChoice(this,wxID_ANY);
        this->Virt_Prio_Sizer->Add(this->Virt_Prio_Label,1,wxALL|wxALIGN_CENTER_VERTICAL,I2P(5));
        this->Virt_Prio_Sizer->Add(this->Virt_Prio,2,wxALL|wxALIGN_CENTER_VERTICAL,I2P(5));

        this->Virt_Event_Sizer=new class wxBoxSizer(wxHORIZONTAL);
        this->Virt_Event_Label=new class wxStaticText(this,wxID_ANY,_("VM Event"));
        this->Virt_Event=new class wxChoice(this,wxID_ANY);
        this->Virt_Event_Sizer->Add(this->Virt_Event_Label,1,wxALL|wxALIGN_CENTER_VERTICAL,I2P(5));
        this->Virt_Event_Sizer->Add(this->Virt_Event,2,wxALL|wxALIGN_CENTER_VERTICAL,I2P(5));

        this->Virt_Map_Sizer=new class wxBoxSizer(wxHORIZONTAL);
        this->Virt_Map_Label=new class wxStaticText(this,wxID_ANY,_("VM Map"));
        this->Virt_Map=new class wxTextCtrl(this,wxID_ANY);
        this->Virt_Map_Sizer->Add(this->Virt_Map_Label,1,wxALL|wxALIGN_CENTER_VERTICAL,I2P(5));
        this->Virt_Map_Sizer->Add(this->Virt_Map,2,wxALL|wxALIGN_CENTER_VERTICAL,I2P(5));

        this->Sizer2_1->Add(this->Extra_Captbl_Sizer,6,wxEXPAND);
        this->Sizer2_1->AddStretchSpacer(1);
        this->Sizer2_1->Add(this->Virt_Prio_Sizer,6,wxEXPAND);
        this->Sizer2_2->Add(this->Virt_Event_Sizer,6,wxEXPAND);
        this->Sizer2_2->AddStretchSpacer(1);
        this->Sizer2_2->Add(this->Virt_Map_Sizer,6,wxEXPAND);
        this->Sizer2->Add(this->Sizer2_1,0,wxEXPAND);
        this->Sizer2->Add(this->Sizer2_2,0,wxEXPAND);

        /* Group 3 */
        this->Sizer3=new class wxStaticBoxSizer(wxVERTICAL,this,_("Sizer3"));
        this->Sizer3_1=new class wxBoxSizer(wxHORIZONTAL);
        this->Sizer3_2=new class wxBoxSizer(wxHORIZONTAL);

        this->Toolchain_Sizer=new class wxBoxSizer(wxHORIZONTAL);
        this->Toolchain_Label=new class wxStaticText(this,wxID_ANY,_("Toolchain"));
        this->Toolchain=new class wxChoice(this,wxID_ANY);
        this->Toolchain_Sizer->Add(this->Toolchain_Label,1,wxALL|wxALIGN_CENTER_VERTICAL,I2P(5));
        this->Toolchain_Sizer->Add(this->Toolchain,2,wxALL|wxALIGN_CENTER_VERTICAL,I2P(5));
        this->Toolchain->Bind(wxEVT_CHOICE, &Monitor_Panel::On_Toolchain_Change, this, this->Toolchain->GetId());

        this->Buildsystem_Sizer=new class wxBoxSizer(wxHORIZONTAL);
        this->Buildsystem_Label=new class wxStaticText(this,wxID_ANY,_("Buildsystem"));
        this->Buildsystem=new class wxChoice(this,wxID_ANY);
        this->Buildsystem_Sizer->Add(this->Buildsystem_Label,1,wxALL|wxALIGN_CENTER_VERTICAL,I2P(5));
        this->Buildsystem_Sizer->Add(this->Buildsystem,2,wxALL|wxALIGN_CENTER_VERTICAL,I2P(5));

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
        this->Optimization_Sizer->Add(this->Optimization,2,wxALL|wxALIGN_CENTER_VERTICAL,I2P(5));

        this->Idle_Sleep_Enable_Sizer=new class wxBoxSizer(wxHORIZONTAL);
        this->Idle_Sleep_Enable_Label=new class wxStaticText(this,wxID_ANY,_("Idle Sleep Enable"));
        this->Idle_Sleep_Enable=new class wxCheckBox(this,wxID_ANY,wxEmptyString);
        this->Idle_Sleep_Enable_Sizer->Add(this->Idle_Sleep_Enable_Label,1,wxALL|wxALIGN_CENTER_VERTICAL,I2P(5));
        this->Idle_Sleep_Enable_Sizer->Add(this->Idle_Sleep_Enable,2,wxALL|wxALIGN_CENTER_VERTICAL,I2P(5));

        this->Sizer3_1->Add(this->Toolchain_Sizer,6,wxEXPAND);
        this->Sizer3_1->AddStretchSpacer(1);
        this->Sizer3_1->Add(this->Buildsystem_Sizer,6,wxEXPAND);
        this->Sizer3_2->Add(this->Optimization_Sizer,6,wxEXPAND);
        this->Sizer3_2->AddStretchSpacer(1);
        this->Sizer3_2->Add(this->Idle_Sleep_Enable_Sizer,6,wxEXPAND);
        this->Sizer3->Add(this->Sizer3_1,0,wxEXPAND);
        this->Sizer3->Add(this->Sizer3_2,0,wxEXPAND);

        /* Group 4 */
        this->Sizer4=new class wxStaticBoxSizer(wxVERTICAL,this,_("Sizer4"));

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

        this->Sizer4->Add(this->Project_Sizer,0,wxEXPAND);
        this->Sizer4->Add(this->Linker_Output_Sizer,0,wxEXPAND);
        this->Sizer4->Add(this->Config_Header_Output_Sizer,0,wxEXPAND);
        this->Sizer4->Add(this->Boot_Header_Output_Sizer,0,wxEXPAND);
        this->Sizer4->Add(this->Boot_Source_Output_Sizer,0,wxEXPAND);
        this->Sizer4->Add(this->Hook_Source_Sizer,0,wxEXPAND);


        this->Main_Sizer->Add(this->Sizer1,0,wxEXPAND|wxALL,I2P(5));
        this->Main_Sizer->Add(this->Sizer2,0,wxEXPAND|wxALL,I2P(5));
        this->Main_Sizer->Add(this->Sizer3,0,wxEXPAND|wxALL,I2P(5));
        this->Main_Sizer->Add(this->Sizer4,0,wxEXPAND|wxALL,I2P(5));

        this->SetSizer(this->Main_Sizer);
        this->Layout();

        this->Code_Size->Bind(wxEVT_KILL_FOCUS, &Monitor_Panel::On_Trans_Hex, this);
        this->Data_Size->Bind(wxEVT_KILL_FOCUS, &Monitor_Panel::On_Trans_Hex, this);
        this->Init_Stack_Size->Bind(wxEVT_KILL_FOCUS, &Monitor_Panel::On_Trans_Hex, this);
        this->Sftd_Stack_Size->Bind(wxEVT_KILL_FOCUS, &Monitor_Panel::On_Trans_Hex, this);
        this->Vmmd_Stack_Size->Bind(wxEVT_KILL_FOCUS, &Monitor_Panel::On_Trans_Hex, this);
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
    std::sprintf(Buf, "%lld", Main::Proj_Info->Monitor->Virt_Prio);
    this->Virt_Prio->SetStringSelection(Buf);
    std::sprintf(Buf, "%lld", Main::Proj_Info->Monitor->Virt_Event);
    this->Virt_Event->SetStringSelection(Buf);
    std::sprintf(Buf, "%lld", Main::Proj_Info->Monitor->Virt_Map);
    this->Virt_Map->SetValue(Buf);
    this->Buildsystem->SetStringSelection(Main::Proj_Info->Monitor->Buildsystem);
    this->Toolchain->SetStringSelection(Main::Proj_Info->Monitor->Toolchain);
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
//    Main::Proj_Info->Monitor->Virt_Prio=std::stoull(this->Virt_Prio->GetValue().ToStdString(),0,0);
//    Main::Proj_Info->Monitor->Virt_Event=std::stoull(this->Virt_Event->GetValue().ToStdString(),0,0);
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

/* Function:Monitor_Panel::Check **********************************************
Description : Check the basic information settings.
Input       : None.
Output      : None.
Return      : ret_t - 0 for OK, -1 for error.
******************************************************************************/
ret_t Monitor_Panel::Check(void)
{
    ret_t Virt_Map_Val;
    ret_t Virt_Prio_Val;
    ret_t Virt_Event_Val;
    std::string Comp_Sug;
    std::string Toolchain;
    std::string Buildsystem;
    class wxArrayString Comp_Buildsystem;

    if(Main::Num_GEZ_Hex_Check(this->Code_Size->GetValue().ToStdString())!=0)
    {
        Main::Msgbox_Show(this, MSGBOX_ERROR,
                          _("Monitor Config"),
                          _("Code size is not a valid hexadecimal nonnegative integer"));
        return -1;
    }
    if(Main::Num_GEZ_Hex_Check(this->Data_Size->GetValue().ToStdString())!=0)
    {
        Main::Msgbox_Show(this, MSGBOX_ERROR,
                          _("Monitor Config"),
                          _("Data size is not a valid hexadecimal nonnegative integer"));
        return -1;
    }
    if(Main::Num_GEZ_Hex_Check(this->Init_Stack_Size->GetValue().ToStdString())!=0)
    {
        Main::Msgbox_Show(this, MSGBOX_ERROR,
                          _("Monitor Config"),
                          _("Initial stack size is not a valid hexadecimal nonnegative integer"));
        return -1;
    }
    if(Main::Num_GEZ_Hex_Check(this->Sftd_Stack_Size->GetValue().ToStdString())!=0)
    {
        Main::Msgbox_Show(this, MSGBOX_ERROR,
                          _("Monitor Config"),
                          _("SFTD stack size is not a valid hexadecimal nonnegative integer"));
        return -1;
    }
    if(Main::Num_GEZ_Hex_Check(this->Vmmd_Stack_Size->GetValue().ToStdString())!=0)
    {
        Main::Msgbox_Show(this, MSGBOX_ERROR,
                          _("Monitor Config"),
                          _("VMMD stack size is not a valid hexadecimal nonnegative integer"));
        return -1;
    }
    if(Main::Num_GEZ_Check(this->Extra_Captbl->GetValue().ToStdString())!=0)
    {
        Main::Msgbox_Show(this, MSGBOX_ERROR,
                          _("Monitor Config"),
                          _("Extra capability table is not a valid positive integer"));
        return -1;
    }
    if(Main::Num_GEZ_Check(this->Virt_Prio->GetStringSelection().ToStdString())!=0)
    {
        Main::Msgbox_Show(this, MSGBOX_ERROR,
                          _("Monitor Config"),
                          _("Virtual priority is not a valid positive integer"));
        return -1;
    }
    if(Main::Num_GEZ_Check(this->Virt_Event->GetStringSelection().ToStdString())!=0)
    {
        Main::Msgbox_Show(this, MSGBOX_ERROR,
                          _("Monitor Config"),
                          _("Virtual event is not a valid positive integer"));
        return -1;
    }
    if(Main::Num_GEZ_Check(this->Virt_Map->GetValue().ToStdString())!=0)
    {
        Main::Msgbox_Show(this, MSGBOX_ERROR,
                          _("Monitor Config"),
                          _("Virtual map is not a valid positive integer"));
        return -1;
    }

//    /* If user don't use a virtual machine, Virtual Priority can be filled with 0 */
//    this->Virt_Prio->GetStringSelection().ToLongLong(&Virt_Prio_Val,10);
//    if(Main::Proj_Info->Virtual.size()!=0&&Virt_Prio_Val==0)
//    {
//        Main::Msgbox_Show(this, MSGBOX_ERROR,
//                          _("Monitor Config"),
//                          _("If user don't use a virtual machine, virtual priority can be filled with 0"));
//        return -1;
//    }
    /* Virtual Event cannot be set to 0 when using a virtual machine */
//    this->Virt_Event->GetStringSelection().ToLongLong(&Virt_Event_Val,10);
//    if(Main::Proj_Info->Virtual.size()!=0&&Virt_Event_Val==0)
//    {
//        Main::Msgbox_Show(this, MSGBOX_ERROR,
//                          _("Monitor Config"),
//                          _("Virtual event cannot be set to 0 when using a virtual machine"));
//        return -1;
//    }
    if(Virt_Event_Val>1024)
    {
        Main::Msgbox_Show(this, MSGBOX_ERROR,
                          _("Monitor Config"),
                          _("Virtual event cannot exceed the hard limit of 1024."));
        return -1;
    }
//    /* Virtual Map cannot be set to 0 when using a virtual machine */
//    this->Virt_Map->GetValue().ToLongLong(&Virt_Map_Val,10);
//    if(Main::Proj_Info->Virtual.size()!=0&&Virt_Map_Val==0)
//    {
//        Main::Msgbox_Show(this, MSGBOX_ERROR,
//                          _("Monitor Config"),
//                          _("Virtual map cannot be set to 0 when using a virtual machine"));
//        return -1;
//    }
    /* Virtual Map must be greater than or equal to Virtual Event */
    if(Virt_Map_Val<Virt_Event_Val)
    {
        Main::Msgbox_Show(this, MSGBOX_ERROR,
                          _("Monitor Config"),
                          _("Virtual map must be greater than or equal to Virtual Event"));
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
                          _("Monitor Config"),
                          _("Compatibility error, you may try:\n"+Comp_Sug));
        return -1;
    }
    return 0;
}
/* End Function:Monitor_Panel::Check *****************************************/

/* Function:Monitor_Panel::Compatible_Set *************************************
Description : Set build system and tool chain.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
void Monitor_Panel::Compatible_Set()
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
/* End Function:Monitor_Panel::Buildsystem_Toolchain_Set *********************/

/* Function:Monitor_Panel::Virt_Prio_Set ************************************
Description : Set virtual priority according to word length.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
void Monitor_Panel::Virt_Prio_Set()
{
    cnt_t Cnt;
    ptr_t Word_Length;

    Word_Length=Main::Plat_Info->Wordlength;

    this->Virt_Prio->Clear();
    this->Virt_Prio->Append("0");
    for(Cnt=1;Cnt<=4;++Cnt)
        this->Virt_Prio->Append(std::to_string(Word_Length*Cnt));

}
/* End Function:Monitor_Panel::Kernel_Prio_Set *******************************/

/* Function:Monitor_Panel::Virt_Event_Set *************************************
Description : Set virtual event according to word length.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
void Monitor_Panel::Virt_Event_Set()
{
    cnt_t Cnt;
    ptr_t Word_Length;

    Word_Length=Main::Plat_Info->Wordlength;

    this->Virt_Event->Clear();
    this->Virt_Event->Append("0");
    for(Cnt=1;Cnt<=4;++Cnt)
        this->Virt_Event->Append(std::to_string(Word_Length*Cnt));

}
/* End Function:Monitor_Panel::Virt_Event_Set ********************************/

/* Function:Monitor_Panel::On_Trans_Hex ***************************************
Description : Convert the content of the control to upper case letters and add
              the '0x'  as prefix.
Input       : class wxFocusEvent& Event - The event.
Output      : None.
Return      : None.
******************************************************************************/
void Monitor_Panel::On_Trans_Hex(wxFocusEvent& Event)
{
    class wxString Value;
    class wxTextCtrl* Temp;

    Temp=dynamic_cast<wxTextCtrl*>(Event.GetEventObject());
    if (Temp)
    {
        Value=Temp->GetValue();
        Value=Value.Upper();
        if (Value.starts_with("0X"))
            Value[1]='x';
        else
            Value="0x"+Value;
        Temp->SetValue(Value);
    }
    Event.Skip();
}
/* End Function:Monitor_Panel::On_Trans_Hex **********************************/

/* Function:Monitor_Panel::On_Toolchain_Change ********************************
Description : Configure a compatible build system based on the tool chain settings.
Input       : class wxFocusEvent& Event - The event.
Output      : None.
Return      : None.
******************************************************************************/
void Monitor_Panel::On_Toolchain_Change(class wxCommandEvent& Event)
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
/* End Function:Monitor_Panel::On_Toolchain_Change ***************************/

}
/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
