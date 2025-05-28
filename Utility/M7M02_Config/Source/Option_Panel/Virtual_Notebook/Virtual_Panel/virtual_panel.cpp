/******************************************************************************
Filename    : virtual_panel.cpp
Author      : lbc
Date        : 25/04/2025
License     : Proprietary; confidential.
Description : First basic information of virtual machine panel implementation.
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
#include "Option_Panel/Virtual_Notebook/Virtual_Panel/virtual_panel.hpp"
#undef __HDR_DEF__

#define __HDR_CLASS__
#include "rvm_cfg.hpp"
#include "Chip_Info/chip_info.hpp"
#include "Plat_Info/plat_info.hpp"
#include "Plat_Info/Compatible/compatible.hpp"
#include "Proj_Info/proj_info.hpp"
#include "Proj_Info/Chip/chip.hpp"
#include "Proj_Info/Process/process.hpp"
#include "Proj_Info/Process/Native/native.hpp"
#include "Proj_Info/Process/Virtual/virtual.hpp"
#include "Option_Panel/Virtual_Notebook/Virtual_Panel/virtual_panel.hpp"
#include "Config_Tree/config_tree.hpp"
#include "Config_Tree/Config_Popup/config_popup.hpp"
#include "Name_Dialog/name_dialog.hpp"
#undef __HDR_CLASS__
/* End Include ***************************************************************/
namespace RVM_CFG
{
/* Function:Virtual_Panel::Virtual_Panel **************************************
Description : Constructor for virtual machine config panel.
Input       : class wxWindow* Parent - The parent window.
Output      : None.
Return      : None.
******************************************************************************/
/* void */ Virtual_Panel::Virtual_Panel(class wxWindow* Parent):
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
        this->Name->Bind(wxEVT_LEFT_DOWN, &Virtual_Panel::On_Name, this);

        this->Extra_Captbl_Sizer=new class wxBoxSizer(wxHORIZONTAL);
        this->Extra_Captbl_Label=new class wxStaticText(this,wxID_ANY,_("Extra Capability Slot"));
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
        this->Bind(wxEVT_CHOICE, &Virtual_Panel::On_Toolchain, this, this->Toolchain->GetId());

        this->Buildsystem_Sizer=new class wxBoxSizer(wxHORIZONTAL);
        this->Buildsystem_Label=new class wxStaticText(this,wxID_ANY,_("Buildsystem"));
        this->Buildsystem=new class wxChoice(this,wxID_ANY);
        this->Buildsystem_Sizer->Add(this->Buildsystem_Label,2,wxALL|wxALIGN_CENTER_VERTICAL,I2P(5));
        this->Buildsystem_Sizer->Add(this->Buildsystem,3,wxALL|wxALIGN_CENTER_VERTICAL,I2P(5));
        this->Bind(wxEVT_CHOICE, &Virtual_Panel::On_Buildsystem, this, this->Buildsystem->GetId());

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

        this->Guest_Sizer=new class wxBoxSizer(wxHORIZONTAL);
        this->Guest_Label=new class wxStaticText(this,wxID_ANY,_("Guest Type"));
        this->Guest=new class wxChoice(this,wxID_ANY);
        this->Guest_Sizer->Add(this->Guest_Label,2,wxALL|wxALIGN_CENTER_VERTICAL,I2P(5));
        this->Guest_Sizer->Add(this->Guest,3,wxALL|wxALIGN_CENTER_VERTICAL,I2P(5));

        this->Build_Line1_Sizer->Add(this->Toolchain_Sizer,6,wxEXPAND);
        this->Build_Line1_Sizer->AddStretchSpacer(1);
        this->Build_Line1_Sizer->Add(this->Buildsystem_Sizer,6,wxEXPAND);

        this->Build_Line2_Sizer->Add(this->Optimization_Sizer,6,wxEXPAND);
        this->Build_Line2_Sizer->AddStretchSpacer(1);
        this->Build_Line2_Sizer->Add(this->Guest_Sizer,6,wxEXPAND);

        this->Build_Sizer->Add(this->Build_Line1_Sizer,0,wxEXPAND);
        this->Build_Sizer->Add(this->Build_Line2_Sizer,0,wxEXPAND);

        /* Virtualization options */
        this->Virt_Sizer=new class wxStaticBoxSizer(wxVERTICAL,this,_("Virtualization Options"));
        this->Virt_Line1_Sizer=new class wxBoxSizer(wxHORIZONTAL);
        this->Virt_Line2_Sizer=new class wxBoxSizer(wxHORIZONTAL);
        this->Virt_Line3_Sizer=new class wxBoxSizer(wxHORIZONTAL);
        this->Virt_Line4_Sizer=new class wxBoxSizer(wxHORIZONTAL);

        this->Vector_Stack_Size_Sizer=new class wxBoxSizer(wxHORIZONTAL);
        this->Vector_Stack_Size_Label=new class wxStaticText(this,wxID_ANY,_("Vector Stack Size"));
        this->Vector_Stack_Size=new class wxTextCtrl(this,wxID_ANY);
        this->Vector_Stack_Size->Bind(wxEVT_KILL_FOCUS, &Virtual_Panel::On_Text_Hex, this);
        this->Vector_Stack_Size_Sizer->Add(this->Vector_Stack_Size_Label,2,wxALL|wxALIGN_CENTER_VERTICAL,I2P(5));
        this->Vector_Stack_Size_Sizer->Add(this->Vector_Stack_Size,3,wxALL|wxALIGN_CENTER_VERTICAL,I2P(5));

        this->User_Stack_Size_Sizer=new class wxBoxSizer(wxHORIZONTAL);
        this->User_Stack_Size_Label=new class wxStaticText(this,wxID_ANY,_("User Stack Size"));
        this->User_Stack_Size=new class wxTextCtrl(this,wxID_ANY);
        this->User_Stack_Size->Bind(wxEVT_KILL_FOCUS, &Virtual_Panel::On_Text_Hex, this);
        this->User_Stack_Size_Sizer->Add(this->User_Stack_Size_Label,2,wxALL|wxALIGN_CENTER_VERTICAL,I2P(5));
        this->User_Stack_Size_Sizer->Add(this->User_Stack_Size,3,wxALL|wxALIGN_CENTER_VERTICAL,I2P(5));

        this->Priority_Sizer=new class wxBoxSizer(wxHORIZONTAL);
        this->Priority_Label=new class wxStaticText(this,wxID_ANY,_("Virtual Priority Number"));
        this->Priority=new class wxTextCtrl(this,wxID_ANY);
        this->Priority_Sizer->Add(this->Priority_Label,2,wxALL|wxALIGN_CENTER_VERTICAL,I2P(5));
        this->Priority_Sizer->Add(this->Priority,3,wxALL|wxALIGN_CENTER_VERTICAL,I2P(5));

        this->Timeslice_Sizer=new class wxBoxSizer(wxHORIZONTAL);
        this->Timeslice_Label=new class wxStaticText(this,wxID_ANY,_("VM Timeslice Ticks"));
        this->Timeslice=new class wxTextCtrl(this,wxID_ANY);
        this->Timeslice_Sizer->Add(this->Timeslice_Label,2,wxALL|wxALIGN_CENTER_VERTICAL,I2P(5));
        this->Timeslice_Sizer->Add(this->Timeslice,3,wxALL|wxALIGN_CENTER_VERTICAL,I2P(5));

        this->Period_Sizer=new class wxBoxSizer(wxHORIZONTAL);
        this->Period_Label=new class wxStaticText(this,wxID_ANY,_("Periodic Timer Ticks"));
        this->Period=new class wxTextCtrl(this,wxID_ANY);
        this->Period_Sizer->Add(this->Period_Label,2,wxALL|wxALIGN_CENTER_VERTICAL,I2P(5));
        this->Period_Sizer->Add(this->Period,3,wxALL|wxALIGN_CENTER_VERTICAL,I2P(5));

        this->Watchdog_Sizer=new class wxBoxSizer(wxHORIZONTAL);
        this->Watchdog_Label=new class wxStaticText(this,wxID_ANY,_("Watchdog Timeout Ticks"));
        this->Watchdog=new class wxTextCtrl(this,wxID_ANY);
        this->Watchdog_Sizer->Add(this->Watchdog_Label,2,wxALL|wxALIGN_CENTER_VERTICAL,I2P(5));
        this->Watchdog_Sizer->Add(this->Watchdog,3,wxALL|wxALIGN_CENTER_VERTICAL,I2P(5));

        this->Vector_Num_Sizer=new class wxBoxSizer(wxHORIZONTAL);
        this->Vector_Num_Label=new class wxStaticText(this,wxID_ANY,_("Virtual Vector Number"));
        this->Vector_Num=new class wxTextCtrl(this,wxID_ANY);
        this->Vector_Num_Sizer->Add(this->Vector_Num_Label,2,wxALL|wxALIGN_CENTER_VERTICAL,I2P(5));
        this->Vector_Num_Sizer->Add(this->Vector_Num,3,wxALL|wxALIGN_CENTER_VERTICAL,I2P(5));

        this->Virt_Line1_Sizer->Add(this->Vector_Stack_Size_Sizer,6,wxEXPAND);
        this->Virt_Line1_Sizer->AddStretchSpacer(1);
        this->Virt_Line1_Sizer->Add(this->User_Stack_Size_Sizer,6,wxEXPAND);

        this->Virt_Line2_Sizer->Add(this->Priority_Sizer,6,wxEXPAND);
        this->Virt_Line2_Sizer->AddStretchSpacer(1);
        this->Virt_Line2_Sizer->Add(this->Timeslice_Sizer,6,wxEXPAND);

        this->Virt_Line3_Sizer->Add(this->Period_Sizer,6,wxEXPAND);
        this->Virt_Line3_Sizer->AddStretchSpacer(1);
        this->Virt_Line3_Sizer->Add(this->Watchdog_Sizer,6,wxEXPAND);

        this->Virt_Line4_Sizer->Add(this->Vector_Num_Sizer,6,wxEXPAND);
        this->Virt_Line4_Sizer->AddStretchSpacer(7);

        this->Virt_Sizer->Add(this->Virt_Line1_Sizer,0,wxEXPAND);
        this->Virt_Sizer->Add(this->Virt_Line2_Sizer,0,wxEXPAND);
        this->Virt_Sizer->Add(this->Virt_Line3_Sizer,0,wxEXPAND);
        this->Virt_Sizer->Add(this->Virt_Line4_Sizer,0,wxEXPAND);

        /* Output options */
        this->Output_Sizer=new class wxStaticBoxSizer(wxVERTICAL,this,_("Output Options"));

        this->Project_Sizer=new class wxBoxSizer(wxHORIZONTAL);
        this->Project_Output_Label=new class wxStaticText(this,wxID_ANY,_("Project Output"));
        this->Project_Output=new class wxTextCtrl(this,wxID_ANY);
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

        this->Virtual_Header_Sizer=new class wxBoxSizer(wxHORIZONTAL);
        this->Virtual_Header_Output_Label=new class wxStaticText(this,wxID_ANY,_("Virtual Header Output"));
        this->Virtual_Header_Output=new class wxTextCtrl(this,wxID_ANY);
        this->Virtual_Header_Overwrite=new class wxCheckBox(this,wxID_ANY,_("Overwrite"));
        this->Virtual_Header_Sizer->Add(this->Virtual_Header_Output_Label,5,wxALL|wxALIGN_CENTER_VERTICAL,I2P(5));
        this->Virtual_Header_Sizer->Add(this->Virtual_Header_Output,16,wxALL|wxALIGN_CENTER_VERTICAL,I2P(5));
        this->Virtual_Header_Sizer->Add(this->Virtual_Header_Overwrite,3,wxALL|wxALIGN_CENTER_VERTICAL,I2P(5));

        this->Virtual_Source_Sizer=new class wxBoxSizer(wxHORIZONTAL);
        this->Virtual_Source_Output_Label=new class wxStaticText(this,wxID_ANY,_("Virtual Source Output"));
        this->Virtual_Source_Output=new class wxTextCtrl(this,wxID_ANY);
        this->Virtual_Source_Overwrite=new class wxCheckBox(this,wxID_ANY,_("Overwrite"));
        this->Virtual_Source_Sizer->Add(this->Virtual_Source_Output_Label,5,wxALL|wxALIGN_CENTER_VERTICAL,I2P(5));
        this->Virtual_Source_Sizer->Add(this->Virtual_Source_Output,16,wxALL|wxALIGN_CENTER_VERTICAL,I2P(5));
        this->Virtual_Source_Sizer->Add(this->Virtual_Source_Overwrite,3,wxALL|wxALIGN_CENTER_VERTICAL,I2P(5));

        this->Output_Sizer->Add(this->Project_Sizer,0,wxEXPAND);
        this->Output_Sizer->Add(this->Linker_Output_Sizer,0,wxEXPAND);
        this->Output_Sizer->Add(this->Main_Header_Output_Sizer,0,wxEXPAND);
        this->Output_Sizer->Add(this->Main_Source_Output_Sizer,0,wxEXPAND);
        this->Output_Sizer->Add(this->Virtual_Header_Sizer,0,wxEXPAND);
        this->Output_Sizer->Add(this->Virtual_Source_Sizer,0,wxEXPAND);

        this->Main_Sizer->Add(this->Basic_Sizer,0,wxEXPAND|wxRIGHT|wxLEFT,I2P(5));
        this->Main_Sizer->AddSpacer(I2P(5));
        this->Main_Sizer->Add(this->Build_Sizer,0,wxEXPAND|wxRIGHT|wxLEFT,I2P(5));
        this->Main_Sizer->AddSpacer(I2P(5));
        this->Main_Sizer->Add(this->Virt_Sizer,0,wxEXPAND|wxRIGHT|wxLEFT,I2P(5));
        this->Main_Sizer->AddSpacer(I2P(5));
        this->Main_Sizer->Add(this->Output_Sizer,0,wxEXPAND|wxRIGHT|wxLEFT,I2P(5));

        this->SetSizer(this->Main_Sizer);
        this->Layout();
    }
    catch(std::exception& Exc)
    {
        ASSERT(0,Exc.what());
    }
}
/* End Function:Virtual_Panel::Virtual_Panel *********************************/

/* Function:Virtual_Panel::~Virtual_Panel *************************************
Description : Destructor for virtual machine config panel.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
/* void */ Virtual_Panel::~Virtual_Panel(void)
{

    /* Have to bind to the text itself because we are processing window events on it directly */
    this->Vector_Stack_Size->Unbind(wxEVT_KILL_FOCUS, &Virtual_Panel::On_Text_Hex, this);
    this->User_Stack_Size->Unbind(wxEVT_KILL_FOCUS, &Virtual_Panel::On_Text_Hex, this);
    this->Unbind(wxEVT_CHOICE, &Virtual_Panel::On_Toolchain, this, this->Toolchain->GetId());
    this->Unbind(wxEVT_CHOICE, &Virtual_Panel::On_Buildsystem, this, this->Buildsystem->GetId());
}
/* End Function:Virtual_Panel::Virtual_Panel *********************************/

/* Function:Virtual_Panel::Load ***********************************************
Description : Load.
Input       : const class Virtual* Virtual - The virtual machine object.
Output      : None.
Return      : None.
******************************************************************************/
void Virtual_Panel::Load(const class Virtual* Virtual)
{
    char Buf[32];

    this->Name->SetValue(Virtual->Name);

    std::sprintf(Buf, "%lld", Virtual->Extra_Captbl);
    this->Extra_Captbl->SetValue(Buf);
    std::sprintf(Buf, "0x%llX", Virtual->Vector_Stack_Size);
    this->Vector_Stack_Size->SetValue(Buf);
    std::sprintf(Buf, "0x%llX", Virtual->User_Stack_Size);
    this->User_Stack_Size->SetValue(Buf);
    std::sprintf(Buf, "%lld", Virtual->Priority);
    this->Priority->SetValue(Buf);
    std::sprintf(Buf, "%lld", Virtual->Timeslice);
    this->Timeslice->SetValue(Buf);
    std::sprintf(Buf, "%lld", Virtual->Period);
    this->Period->SetValue(Buf);
    std::sprintf(Buf, "%lld", Virtual->Watchdog);
    this->Watchdog->SetValue(Buf);
    std::sprintf(Buf, "%lld", Virtual->Vector_Num);
    this->Vector_Num->SetValue(Buf);

    this->Toolchain->SetStringSelection(Virtual->Toolchain);
    this->Buildsystem_Set(this->Toolchain->GetStringSelection());
    this->Buildsystem->SetStringSelection(Virtual->Buildsystem);
    this->Guest_Set(this->Toolchain->GetStringSelection(),
                    this->Buildsystem->GetStringSelection());
    this->Guest->SetStringSelection(Virtual->Guest_Type);
    this->Optimization->SetSelection(Virtual->Optimization);

    this->Project_Output->SetValue(Virtual->Project_Output);
    this->Project_Overwrite->SetValue(Virtual->Project_Overwrite);
    this->Linker_Output->SetValue(Virtual->Linker_Output);
    this->Main_Header_Output->SetValue(Virtual->Main_Header_Output);
    this->Main_Source_Output->SetValue(Virtual->Main_Source_Output);
    this->Virtual_Header_Output->SetValue(Virtual->Virtual_Header_Output);
    this->Virtual_Header_Overwrite->SetValue(Virtual->Virtual_Header_Overwrite);
    this->Virtual_Source_Output->SetValue(Virtual->Virtual_Source_Output);
    this->Virtual_Source_Overwrite->SetValue(Virtual->Virtual_Source_Overwrite);

    /* Clear check box which might be used before */
    for (auto& Cop : this->Coprocessor)
        Cop.second->SetValue(false);

    for(const std::string&Cop : Virtual->Coprocessor)
        if(this->Coprocessor.find(Cop)!=this->Coprocessor.end())
            this->Coprocessor.find(Cop)->second->SetValue(true);
}
/* End Function:Virtual_Panel::Load ******************************************/

/* Function:Virtual_Panel::Check **********************************************
Description : Check the basic information of virtual machine.
Input       : None.
Output      : None.
Return      : ret_t - 0 for OK, -1 for error.
******************************************************************************/
ret_t Virtual_Panel::Check()
{
    /* Basic options */
    if(Main::Dec_Check(this,std::string(this->Extra_Captbl->GetValue()),
                       _("Virtual Machine Config - Basic Options"),
                       _("Extra capability slots"))!=0)
        return -1;

    /* Virtualization options */
    if(Main::Hex_Pos_Check(this,std::string(this->Vector_Stack_Size->GetValue()),
                           _("Virtual Machine Config - Virtualization Options"),
                           _("Vector stack size"))!=0)
        return -1;

    if(Main::Hex_Pos_Check(this,std::string(this->User_Stack_Size->GetValue()),
                           _("Virtual Machine Config - Virtualization Options"),
                           _("User stack size"))!=0)
        return -1;

    if(Main::Dec_Pos_Check(this,std::string(this->Priority->GetValue()),
                           _("Virtual Machine Config - Virtualization Options"),
                           _("Virtual Priority"))!=0)
        return -1;

    if(Main::Dec_Pos_Check(this,std::string(this->Timeslice->GetValue()),
                           _("Virtual Machine Config - Virtualization Options"),
                           _("VM timeslice ticks"))!=0)
        return -1;

    if(Main::Dec_Pos_Check(this,std::string(this->Period->GetValue()),
                           _("Virtual Machine Config - Virtualization Options"),
                           _("Periodic timer ticks"))!=0)
        return -1;

    if(Main::Dec_Check(this,std::string(this->Watchdog->GetValue()),
                       _("Virtual Machine Config - Virtualization Options"),
                       _("Watchdog timeout ticks"))!=0)
        return -1;

    if(Main::Dec_Check(this,std::string(this->Vector_Num->GetValue()),
                       _("Virtual Machine Config - Virtualization Options"),
                       _("Virtual vector"))!=0)
        return -1;

    /* Output options */
    if(this->Project_Output->GetValue()=="")
    {
        Main::Msgbox_Show(this,MSGBOX_ERROR,
                          _("Virtual Machine Config - Output Options"),
                          _("The project output folder is unspecified."));
        return -1;
    }

    if(this->Linker_Output->GetValue()=="")
    {
        Main::Msgbox_Show(this,MSGBOX_ERROR,
                          _("Virtual Machine Config - Output Options"),
                          _("The linker output folder is unspecified."));
        return -1;
    }

    if(this->Main_Header_Output->GetValue()=="")
    {
        Main::Msgbox_Show(this,MSGBOX_ERROR,
                          _("Virtual Machine Config - Output Options"),
                          _("The main header output folder is unspecified."));
        return -1;
    }

    if(this->Main_Source_Output->GetValue()=="")
    {
        Main::Msgbox_Show(this,MSGBOX_ERROR,
                          _("Virtual Machine Config - Output Options"),
                          _("The main source output folder is unspecified."));
        return -1;
    }

    if(this->Virtual_Header_Output->GetValue()=="")
    {
        Main::Msgbox_Show(this,MSGBOX_ERROR,
                          _("Virtual Machine Config - Output Options"),
                          _("The virtual header output folder is unspecified."));
        return -1;
    }

    if(this->Virtual_Source_Output->GetValue()=="")
    {
        Main::Msgbox_Show(this,MSGBOX_ERROR,
                          _("Virtual Machine Config - Output Options"),
                          _("The virtual source output folder is unspecified."));
        return -1;
    }

    return 0;
}
/* End Function:Virtual_Panel::Check *****************************************/

/* Function:Virtual_Panel::Save ***********************************************
Description : Save.
Input       : None.
Output      : None.
Return      : ret_t - 0 for OK, -1 for error.
******************************************************************************/
void Virtual_Panel::Save(class Virtual* Virtual)
{
    Virtual->Name=this->Name->GetValue();
    Virtual->Extra_Captbl=std::stoull(std::string(this->Extra_Captbl->GetValue()),0,0);
    Virtual->Vector_Stack_Size=std::stoull(std::string(this->Vector_Stack_Size->GetValue()),0,0);
    Virtual->User_Stack_Size=std::stoull(std::string(this->User_Stack_Size->GetValue()),0,0);
    Virtual->Priority=std::stoull(std::string(this->Priority->GetValue()),0,0);
    Virtual->Timeslice=std::stoull(std::string(this->Timeslice->GetValue()),0,0);
    Virtual->Period=std::stoull(std::string(this->Period->GetValue()),0,0);
    Virtual->Watchdog=std::stoull(std::string(this->Watchdog->GetValue()),0,0);
    Virtual->Vector_Num=std::stoull(std::string(this->Vector_Num->GetValue()),0,0);
    Virtual->Guest_Type=this->Guest->GetStringSelection();
    Virtual->Buildsystem=this->Buildsystem->GetStringSelection();
    Virtual->Toolchain=this->Toolchain->GetStringSelection();
    Virtual->Optimization=this->Optimization->GetSelection();

    Virtual->Project_Output=this->Project_Output->GetValue();
    Virtual->Project_Overwrite=this->Project_Overwrite->GetValue();
    Virtual->Linker_Output=this->Linker_Output->GetValue();
    Virtual->Main_Header_Output=this->Main_Header_Output->GetValue();
    Virtual->Main_Source_Output=this->Main_Source_Output->GetValue();
    Virtual->Virtual_Header_Output=this->Virtual_Header_Output->GetValue();
    Virtual->Virtual_Header_Overwrite=this->Virtual_Header_Overwrite->GetValue();
    Virtual->Virtual_Source_Output=this->Virtual_Source_Output->GetValue();
    Virtual->Virtual_Source_Overwrite=this->Virtual_Source_Overwrite->GetValue();

    /* Clear */
    Virtual->Coprocessor.clear();

    /* Coprocessor */
    for(const auto&Cop : this->Coprocessor)
        if(Cop.second->GetValue())
            Virtual->Coprocessor.push_back(Cop.first);
    if(Virtual->Coprocessor.size()==0)
        Virtual->Coprocessor.push_back("None");

}
/* End Function:Virtual_Panel::Save ******************************************/

/* Function:Virtual_Panel::Rename *********************************************
Description : Carry out the rename on the panel in case it is shown now.
Input       : const std::string& Original - The original name of native process.
              const std::string& Current - The current name of native process.
Output      : None.
Return      : None.
******************************************************************************/
void Virtual_Panel::Rename(const std::string& Original,const std::string& Current)
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
/* End Function:Virtual_Panel::Rename ****************************************/

/* Function:Virtual_Panel::Coprocessor_Set ************************************
Description : Set coprocessors.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
void Virtual_Panel::Coprocessor_Set(void)
{
    Main::Coprocessor_Refill(this,
                             this->Coprocessor_Label,
                             this->Coprocessor_Sizer,
                             this->Coprocessor);

    this->Layout();
}
/* End Function:Virtual_Panel::Coprocessor_Set *******************************/

/* Function:Virtual_Panel::Toolchain_Set **************************************
Description : Set the collection of possible tool chains.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
void Virtual_Panel::Toolchain_Set(void)
{
    std::vector<std::string> Tool_Avail;

    wxLogDebug("Virtual_Panel::Toolchain_Set");

    /* Ask for possible toolchains */
    Main::Plat_Info->Toolchain_Virtual(Tool_Avail);
    Main::Choice_Refill(this->Toolchain,Tool_Avail);
}
/* End Function:Virtual_Panel::Toolchain_Set *********************************/

/* Function:Virtual_Panel::Buildsystem_Set ************************************
Description : Set build system given the toolchain choice.
Input       : const class wxString& Toolchain - The chosen toolchain.
Output      : None.
Return      : None.
******************************************************************************/
void Virtual_Panel::Buildsystem_Set(const class wxString& Toolchain)
{
    std::vector<std::string> Build_Avail;

    wxLogDebug("Virtual_Panel::Buildsystem_Set");

    /* Ask for possible toolchains */
    Main::Plat_Info->Buildsystem_Virtual(std::string(Toolchain),Build_Avail);
    Main::Choice_Refill(this->Buildsystem,Build_Avail);
}
/* End Function:Virtual_Panel::Buildsystem_Set *******************************/

/* Function:Virtual_Panel::Guest_Set ******************************************
Description : Set guest OS given the toolchain choice.
Input       : const class wxString& Toolchain - The chosen toolchain.
Output      : None.
Return      : None.
******************************************************************************/
void Virtual_Panel::Guest_Set(const class wxString& Toolchain,
                              const class wxString& Buildsystem)
{
    std::vector<std::string> Guest_Avail;

    wxLogDebug("Virtual_Panel::Guest_Set");

    /* Ask for possible toolchains */
    Main::Plat_Info->Guest_Virtual(std::string(Toolchain),
                                   std::string(Buildsystem),
                                   Guest_Avail);
    Main::Choice_Refill(this->Guest,Guest_Avail);
}
/* End Function:Virtual_Panel::Buildsystem_Set *******************************/

/* Function:Virtual_Panel::On_Name ********************************************
Description : Pop up a rename dialog, and rename this native machine.
Input       : class wxMouseEvent& Event - The event.
Output      : None.
Return      : None.
******************************************************************************/
void Virtual_Panel::On_Name(class wxMouseEvent& Event)
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
    Main::Virtual_Rename(Original, Current);
}
/* End Function:Virtual_Panel::On_Name ***************************************/

/* Function:Virtual_Panel::On_Text_Hex ****************************************
Description : wxEVT_KILL_FOCUS handler for all text input boxes that should
              normally contain a hex integer.
Input       : class wxFocusEvent& Event - The event.
Output      : None.
Return      : None.
******************************************************************************/
void Virtual_Panel::On_Text_Hex(class wxFocusEvent& Event)
{
    class wxTextCtrl* Text;

    Text=dynamic_cast<class wxTextCtrl*>(Event.GetEventObject());
    if(Text!=nullptr)
        Text->SetValue(Main::Num2Hex(std::string(Text->GetValue())));

    Event.Skip();
}
/* End Function:Virtual_Panel::On_Text_Hex ***********************************/

/* Function:Virtual_Panel::On_Toolchain ***************************************
Description : Configure a compatible build system based on the tool chain settings.
Input       : class wxCommandEvent& Event - The event.
Output      : None.
Return      : None.
******************************************************************************/
void Virtual_Panel::On_Toolchain(class wxCommandEvent& Event)
{
    class wxString Toolchain;
    class wxString Buildsystem;
    class wxString Guest;

    Toolchain=this->Toolchain->GetStringSelection();
    Buildsystem=this->Buildsystem->GetStringSelection();
    Guest=this->Guest->GetStringSelection();

    /* Recreate options if the current one is invalid */
    if(Main::Plat_Info->Compat_Check(std::string(Toolchain),
                                     std::string(Buildsystem),
                                     std::string(Guest))!=0)
    {
        this->Buildsystem_Set(Toolchain);
        this->Guest_Set(Toolchain,this->Buildsystem->GetStringSelection());
    }
}
/* End Function:Virtual_Panel::On_Toolchain **********************************/

/* Function:Virtual_Panel::On_Buildsystem *************************************
Description : Configure a compatible guest based on the tool chain and build system
              settings.
Input       : class wxCommandEvent& Event - The event.
Output      : None.
Return      : None.
******************************************************************************/
void Virtual_Panel::On_Buildsystem(class wxCommandEvent& Event)
{
    class wxString Toolchain;
    class wxString Buildsystem;
    class wxString Guest;

    Toolchain=this->Toolchain->GetStringSelection();
    Buildsystem=this->Buildsystem->GetStringSelection();
    Guest=this->Guest->GetStringSelection();

    /* Recreate options if the current one is invalid */
    if(Main::Plat_Info->Compat_Check(std::string(Toolchain),
                                     std::string(Buildsystem),
                                     std::string(Guest))!=0)
        this->Guest_Set(Toolchain,this->Buildsystem->GetStringSelection());
}
/* End Function:Virtual_Panel::On_Buildsystem ********************************/
}
/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
