/******************************************************************************
Filename    : virtual_basic_panel.cpp
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
#include "Option_Panel/Virtual_Notebook/Virtual_Basic_Panel/virtual_basic_panel.hpp"
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
#include "Option_Panel/Virtual_Notebook/Virtual_Basic_Panel/virtual_basic_panel.hpp"
#include "Config_Tree/config_tree.hpp"
#include "Config_Tree/Config_Popup/config_popup.hpp"
#include "Name_Dialog/name_dialog.hpp"
#undef __HDR_CLASS__
/* End Include ***************************************************************/
namespace RVM_CFG
{
/* Function:Virtual_Basic_Panel::Virtual_Basic_Panel **************************
Description : Constructor for basic notebook.
Input       : class wxWindow* Parent - The parent window.
Output      : None.
Return      : None.
******************************************************************************/
/* void */ Virtual_Basic_Panel::Virtual_Basic_Panel(class wxWindow* Parent):
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
        this->Name->Bind(wxEVT_LEFT_DOWN, &Virtual_Basic_Panel::On_Rename, this);

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

        /* Group 2 */
//        this->Sizer2=new class wxStaticBoxSizer(wxVERTICAL,this,_("Sizer2"));
//        this->Sizer2_1=new class wxBoxSizer(wxHORIZONTAL);
//
//        this->Coprocessor_Sizer=new class wxBoxSizer(wxHORIZONTAL);
//
//        this->Sizer2_1->Add(this->Coprocessor_Sizer,1,wxEXPAND);
//        this->Sizer2->Add(this->Sizer2_1,0,wxEXPAND);

        /* Group 3*/
        this->Sizer3=new class wxStaticBoxSizer(wxVERTICAL,this,_("Sizer3"));
        this->Sizer3_1=new class wxBoxSizer(wxHORIZONTAL);
        this->Sizer3_2=new class wxBoxSizer(wxHORIZONTAL);

        this->Toolchain_Sizer=new class wxBoxSizer(wxHORIZONTAL);
        this->Toolchain_Label=new class wxStaticText(this,wxID_ANY,_("Toolchain"));
        this->Toolchain=new class wxChoice(this,wxID_ANY);
        this->Toolchain_Sizer->Add(this->Toolchain_Label,2,wxALL|wxALIGN_CENTER_VERTICAL,I2P(5));
        this->Toolchain_Sizer->Add(this->Toolchain,3,wxALL|wxALIGN_CENTER_VERTICAL,I2P(5));
        this->Toolchain->Bind(wxEVT_CHOICE, &Virtual_Basic_Panel::On_Toolchain_Change, this, this->Toolchain->GetId());

        this->Buildsystem_Sizer=new class wxBoxSizer(wxHORIZONTAL);
        this->Buildsystem_Label=new class wxStaticText(this,wxID_ANY,_("Buildsystem"));
        this->Buildsystem=new class wxChoice(this,wxID_ANY);
        this->Buildsystem_Sizer->Add(this->Buildsystem_Label,2,wxALL|wxALIGN_CENTER_VERTICAL,I2P(5));
        this->Buildsystem_Sizer->Add(this->Buildsystem,3,wxALL|wxALIGN_CENTER_VERTICAL,I2P(5));
        //this->Buildsystem->Bind(wxEVT_CHOICE, &Virtual_Basic_Panel::On_Buildsystem_Change, this, this->Buildsystem->GetId());

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

        this->Guest_Type_Sizer=new class wxBoxSizer(wxHORIZONTAL);
        this->Guest_Type_Label=new class wxStaticText(this,wxID_ANY,_("Guest Type"));
        this->Guest_Type=new class wxChoice(this,wxID_ANY);
        this->Guest_Type_Sizer->Add(this->Guest_Type_Label,2,wxALL|wxALIGN_CENTER_VERTICAL,I2P(5));
        this->Guest_Type_Sizer->Add(this->Guest_Type,3,wxALL|wxALIGN_CENTER_VERTICAL,I2P(5));
        //this->Guest_Type->Bind(wxEVT_CHOICE, &Virtual_Basic_Panel::On_Guest_Change, this, this->Guest_Type->GetId());

        this->Sizer3_1->Add(this->Toolchain_Sizer,6,wxEXPAND);
        this->Sizer3_1->AddStretchSpacer(1);
        this->Sizer3_1->Add(this->Buildsystem_Sizer,6,wxEXPAND);
        this->Sizer3_2->Add(this->Optimization_Sizer,6,wxEXPAND);
        this->Sizer3_2->AddStretchSpacer(1);
        this->Sizer3_2->Add(this->Guest_Type_Sizer,6,wxEXPAND);
        this->Sizer3->Add(this->Sizer3_1,0,wxEXPAND);
        this->Sizer3->Add(this->Sizer3_2,0,wxEXPAND);

        /* Group 4*/
        this->Sizer4=new class wxStaticBoxSizer(wxVERTICAL,this,_("Sizer4"));
        this->Sizer4_1=new class wxBoxSizer(wxHORIZONTAL);
        this->Sizer4_2=new class wxBoxSizer(wxHORIZONTAL);
        this->Sizer4_3=new class wxBoxSizer(wxHORIZONTAL);

        this->Priority_Sizer=new class wxBoxSizer(wxHORIZONTAL);
        this->Priority_Label=new class wxStaticText(this,wxID_ANY,_("Priority"));
        this->Priority=new class wxTextCtrl(this,wxID_ANY);
        this->Priority_Sizer->Add(this->Priority_Label,2,wxALL|wxALIGN_CENTER_VERTICAL,I2P(5));
        this->Priority_Sizer->Add(this->Priority,3,wxALL|wxALIGN_CENTER_VERTICAL,I2P(5));

        this->Timeslice_Sizer=new class wxBoxSizer(wxHORIZONTAL);
        this->Timeslice_Label=new class wxStaticText(this,wxID_ANY,_("Timeslice"));
        this->Timeslice=new class wxTextCtrl(this,wxID_ANY);
        this->Timeslice_Sizer->Add(this->Timeslice_Label,2,wxALL|wxALIGN_CENTER_VERTICAL,I2P(5));
        this->Timeslice_Sizer->Add(this->Timeslice,3,wxALL|wxALIGN_CENTER_VERTICAL,I2P(5));

        this->Period_Sizer=new class wxBoxSizer(wxHORIZONTAL);
        this->Period_Label=new class wxStaticText(this,wxID_ANY,_("Period"));
        this->Period=new class wxTextCtrl(this,wxID_ANY);
        this->Period_Sizer->Add(this->Period_Label,2,wxALL|wxALIGN_CENTER_VERTICAL,I2P(5));
        this->Period_Sizer->Add(this->Period,3,wxALL|wxALIGN_CENTER_VERTICAL,I2P(5));

        this->Watchdog_Sizer=new class wxBoxSizer(wxHORIZONTAL);
        this->Watchdog_Label=new class wxStaticText(this,wxID_ANY,_("Watchdog"));
        this->Watchdog=new class wxTextCtrl(this,wxID_ANY);
        this->Watchdog_Sizer->Add(this->Watchdog_Label,2,wxALL|wxALIGN_CENTER_VERTICAL,I2P(5));
        this->Watchdog_Sizer->Add(this->Watchdog,3,wxALL|wxALIGN_CENTER_VERTICAL,I2P(5));

        this->Vector_Num_Sizer=new class wxBoxSizer(wxHORIZONTAL);
        this->Vector_Num_Label=new class wxStaticText(this,wxID_ANY,_("Vector Num"));
        this->Vector_Num=new class wxTextCtrl(this,wxID_ANY);
        this->Vector_Num_Sizer->Add(this->Vector_Num_Label,2,wxALL|wxALIGN_CENTER_VERTICAL,I2P(5));
        this->Vector_Num_Sizer->Add(this->Vector_Num,3,wxALL|wxALIGN_CENTER_VERTICAL,I2P(5));

        this->Sizer4_1->Add(this->Priority_Sizer,6,wxEXPAND);
        this->Sizer4_1->AddStretchSpacer(1);
        this->Sizer4_1->Add(this->Timeslice_Sizer,6,wxEXPAND);
        this->Sizer4_2->Add(this->Period_Sizer,6,wxEXPAND);
        this->Sizer4_2->AddStretchSpacer(1);
        this->Sizer4_2->Add(this->Watchdog_Sizer,6,wxEXPAND);
        this->Sizer4_3->Add(this->Vector_Num_Sizer,6,wxEXPAND);
        this->Sizer4_3->AddStretchSpacer(7);
        this->Sizer4->Add(this->Sizer4_1,0,wxEXPAND);
        this->Sizer4->Add(this->Sizer4_2,0,wxEXPAND);
        this->Sizer4->Add(this->Sizer4_3,0,wxEXPAND);

        /* Group 5 */
        this->Sizer5=new class wxStaticBoxSizer(wxVERTICAL,this,_("Sizer5"));
        this->Sizer5_1=new class wxBoxSizer(wxHORIZONTAL);

        this->Vector_Stack_Size_Sizer=new class wxBoxSizer(wxHORIZONTAL);
        this->Vector_Stack_Size_Label=new class wxStaticText(this,wxID_ANY,_("Vector Stack Size"));
        this->Vector_Stack_Size=new class wxTextCtrl(this,wxID_ANY);
        this->Vector_Stack_Size_Sizer->Add(this->Vector_Stack_Size_Label,2,wxALL|wxALIGN_CENTER_VERTICAL,I2P(5));
        this->Vector_Stack_Size_Sizer->Add(this->Vector_Stack_Size,3,wxALL|wxALIGN_CENTER_VERTICAL,I2P(5));

        this->User_Stack_Size_Sizer=new class wxBoxSizer(wxHORIZONTAL);
        this->User_Stack_Size_Label=new class wxStaticText(this,wxID_ANY,_("User Stack Size"));
        this->User_Stack_Size=new class wxTextCtrl(this,wxID_ANY);
        this->User_Stack_Size_Sizer->Add(this->User_Stack_Size_Label,2,wxALL|wxALIGN_CENTER_VERTICAL,I2P(5));
        this->User_Stack_Size_Sizer->Add(this->User_Stack_Size,3,wxALL|wxALIGN_CENTER_VERTICAL,I2P(5));

        this->Sizer5_1->Add(this->Vector_Stack_Size_Sizer,6,wxEXPAND);
        this->Sizer5_1->AddStretchSpacer(1);
        this->Sizer5_1->Add(this->User_Stack_Size_Sizer,6,wxEXPAND);
        this->Sizer5->Add(this->Sizer5_1,0,wxEXPAND);

        /* Group 6 */
        this->Sizer6=new class wxStaticBoxSizer(wxVERTICAL,this,_("Sizer6"));

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

        this->Sizer6->Add(this->Project_Sizer,0,wxEXPAND);
        this->Sizer6->Add(this->Linker_Output_Sizer,0,wxEXPAND);
        this->Sizer6->Add(this->Main_Header_Output_Sizer,0,wxEXPAND);
        this->Sizer6->Add(this->Main_Source_Output_Sizer,0,wxEXPAND);
        this->Sizer6->Add(this->Virtual_Header_Sizer,0,wxEXPAND);
        this->Sizer6->Add(this->Virtual_Source_Sizer,0,wxEXPAND);

        this->Main_Sizer->Add(this->Sizer1,0,wxEXPAND|wxRIGHT|wxLEFT,I2P(5));
        this->Main_Sizer->AddSpacer(I2P(5));
        //this->Main_Sizer->Add(this->Sizer2,0,wxEXPAND|wxRIGHT|wxLEFT,I2P(5));
        this->Main_Sizer->Add(this->Sizer3,0,wxEXPAND|wxRIGHT|wxLEFT,I2P(5));
        this->Main_Sizer->AddSpacer(I2P(5));
        this->Main_Sizer->Add(this->Sizer4,0,wxEXPAND|wxRIGHT|wxLEFT,I2P(5));
        this->Main_Sizer->AddSpacer(I2P(5));
        this->Main_Sizer->Add(this->Sizer5,0,wxEXPAND|wxRIGHT|wxLEFT,I2P(5));
        this->Main_Sizer->AddSpacer(I2P(5));
        this->Main_Sizer->Add(this->Sizer6,0,wxEXPAND|wxRIGHT|wxLEFT,I2P(5));

        this->SetSizer(this->Main_Sizer);
        this->Layout();

        this->Vector_Stack_Size->Bind(wxEVT_KILL_FOCUS, &Virtual_Basic_Panel::On_Trans_Hex, this);
        this->User_Stack_Size->Bind(wxEVT_KILL_FOCUS, &Virtual_Basic_Panel::On_Trans_Hex, this);
    }
    catch(std::exception& Exc)
    {
        ASSERT(0,Exc.what());
    }
}
/* End Function:Virtual_Basic_Panel::Virtual_Basic_Panel *********************/

/* Function:Virtual_Basic_Panel::~Virtual_Basic_Panel *************************
Description : Destructor for sheet notebook.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
/* void */ Virtual_Basic_Panel::~Virtual_Basic_Panel(void)
{

}
/* End Function:Virtual_Basic_Panel::Virtual_Basic_Panel *********************/

/* Function:Virtual_Basic_Panel::Load ****************************************
Description : Load.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
void Virtual_Basic_Panel::Load(const class Virtual* virtual_info)
{
    char Buf[32];

    this->Name->SetValue(virtual_info->Name);

    std::sprintf(Buf, "%lld", virtual_info->Extra_Captbl);
    this->Extra_Captbl->SetValue(Buf);
    std::sprintf(Buf, "0x%llX", virtual_info->Vector_Stack_Size);
    this->Vector_Stack_Size->SetValue(Buf);
    std::sprintf(Buf, "0x%llX", virtual_info->User_Stack_Size);
    this->User_Stack_Size->SetValue(Buf);
    std::sprintf(Buf, "%lld", virtual_info->Priority);
    this->Priority->SetValue(Buf);
    std::sprintf(Buf, "%lld", virtual_info->Timeslice);
    this->Timeslice->SetValue(Buf);
    std::sprintf(Buf, "%lld", virtual_info->Period);
    this->Period->SetValue(Buf);
    std::sprintf(Buf, "%lld", virtual_info->Watchdog);
    this->Watchdog->SetValue(Buf);
    std::sprintf(Buf, "%lld", virtual_info->Vector_Num);
    this->Vector_Num->SetValue(Buf);

    this->Guest_Type->SetStringSelection(virtual_info->Guest_Type);
    this->Buildsystem->SetStringSelection(virtual_info->Buildsystem);
    this->Toolchain->SetStringSelection(virtual_info->Toolchain);
    this->Optimization->SetSelection(virtual_info->Optimization);

    this->Project_Output->SetValue(virtual_info->Project_Output);
    this->Project_Overwrite->SetValue(virtual_info->Project_Overwrite);
    this->Linker_Output->SetValue(virtual_info->Linker_Output);
    this->Main_Header_Output->SetValue(virtual_info->Main_Header_Output);
    this->Main_Source_Output->SetValue(virtual_info->Main_Source_Output);
    this->Virtual_Header_Output->SetValue(virtual_info->Virtual_Header_Output);
    this->Virtual_Header_Overwrite->SetValue(virtual_info->Virtual_Header_Overwrite);
    this->Virtual_Source_Output->SetValue(virtual_info->Virtual_Source_Output);
    this->Virtual_Source_Overwrite->SetValue(virtual_info->Virtual_Source_Overwrite);

    /* Clear check box which might be used before */
    for (auto& Cop : this->Coprocessor)
        Cop.second->SetValue(false);

    for(const std::string&Cop : virtual_info->Coprocessor)
        if(this->Coprocessor.find(Cop)!=this->Coprocessor.end())
            this->Coprocessor.find(Cop)->second->SetValue(true);
}
/* End Function:Virtual_Basic_Panel::Load ************************************/

/* Function:Virtual_Basic_Panel::Save *****************************************
Description : Save.
Input       : None.
Output      : None.
Return      : ret_t - 0 for OK, -1 for error.
******************************************************************************/
void Virtual_Basic_Panel::Save(class Virtual* virtual_info)
{
    virtual_info->Name=this->Name->GetValue();
    virtual_info->Extra_Captbl=std::stoull(this->Extra_Captbl->GetValue().ToStdString(),0,0);
    virtual_info->Vector_Stack_Size=std::stoull(this->Vector_Stack_Size->GetValue().ToStdString(),0,0);
    virtual_info->User_Stack_Size=std::stoull(this->User_Stack_Size->GetValue().ToStdString(),0,0);
    virtual_info->Priority=std::stoull(this->Priority->GetValue().ToStdString(),0,0);
    virtual_info->Timeslice=std::stoull(this->Timeslice->GetValue().ToStdString(),0,0);
    virtual_info->Period=std::stoull(this->Period->GetValue().ToStdString(),0,0);
    virtual_info->Watchdog=std::stoull(this->Watchdog->GetValue().ToStdString(),0,0);
    virtual_info->Vector_Num=std::stoull(this->Vector_Num->GetValue().ToStdString(),0,0);
    virtual_info->Guest_Type=this->Guest_Type->GetStringSelection();
    virtual_info->Buildsystem=this->Buildsystem->GetStringSelection();
    virtual_info->Toolchain=this->Toolchain->GetStringSelection();
    virtual_info->Optimization=this->Optimization->GetSelection();

    virtual_info->Project_Output=this->Project_Output->GetValue();
    virtual_info->Project_Overwrite=this->Project_Overwrite->GetValue();
    virtual_info->Linker_Output=this->Linker_Output->GetValue();
    virtual_info->Main_Header_Output=this->Main_Header_Output->GetValue();
    virtual_info->Main_Source_Output=this->Main_Source_Output->GetValue();
    virtual_info->Virtual_Header_Output=this->Virtual_Header_Output->GetValue();
    virtual_info->Virtual_Header_Overwrite=this->Virtual_Header_Overwrite->GetValue();
    virtual_info->Virtual_Source_Output=this->Virtual_Source_Output->GetValue();
    virtual_info->Virtual_Source_Overwrite=this->Virtual_Source_Overwrite->GetValue();

    /* Clear */
    virtual_info->Coprocessor.clear();

    /* Coprocessor */
    for(const auto&Cop : this->Coprocessor)
        if(Cop.second->GetValue())
            virtual_info->Coprocessor.push_back(Cop.first);
    if(virtual_info->Coprocessor.size()==0)
        virtual_info->Coprocessor.push_back("None");

}
/* End Function:Virtual_Basic_Panel::Save ************************************/

/* Function:Virtual_Basic_Panel::Check ****************************************
Description : Check the basic information of virtual machine.
Input       : None.
Output      : None.
Return      : ret_t - 0 for OK, -1 for error.
******************************************************************************/
ret_t Virtual_Basic_Panel::Check()
{
    std::string Comp_Sug;
    std::string Toolchain;
    std::string Buildsystem;
    class wxArrayString Comp_Buildsystem;

    /* Extra_Captbl */
    if(Main::Num_GEZ_Check(this->Extra_Captbl->GetValue().ToStdString())!=0)
    {
        Main::Msgbox_Show(this,MSGBOX_ERROR,
                          _("Virtual Machine Config - Basic information"),
                          _("Extra capability table is not a valid decimal nonnegative integer."));
        return -1;
    }


    if(Main::Num_GEZ_Hex_Check(this->Vector_Stack_Size->GetValue().ToStdString())!=0)
    {
        Main::Msgbox_Show(this,MSGBOX_ERROR,
                          _("Virtual Machine Config - Basic information"),
                          _("Vector stack size is not a valid hexadecimal nonnegative integer"));
        return -1;
    }

    if(Main::Num_GEZ_Hex_Check(this->User_Stack_Size->GetValue().ToStdString())!=0)
    {
        Main::Msgbox_Show(this,MSGBOX_ERROR,
                          _("Virtual Machine Config - Basic information"),
                          _("User stack size is not a valid hexadecimal nonnegative integer"));
        return -1;
    }
    if(Main::Num_GEZ_Check(this->Priority->GetValue().ToStdString())!=0)
    {
        Main::Msgbox_Show(this,MSGBOX_ERROR,
                          _("Virtual Machine Config - Basic information"),
                          _("Priority is not a valid decimal nonnegative integer."));
        return -1;
    }
    if(Main::Num_GZ_Check(this->Timeslice->GetValue().ToStdString())!=0)
    {
        Main::Msgbox_Show(this,MSGBOX_ERROR,
                          _("Virtual Machine Config - Basic information"),
                          _("Timeslice is not a valid decimal positive integer."));
        return -1;
    }

    if(Main::Num_GZ_Check(this->Period->GetValue().ToStdString())!=0)
    {
        Main::Msgbox_Show(this,MSGBOX_ERROR,
                          _("Virtual Machine Config - Basic information"),
                          _("Period is not a valid decimal positive integer."));
        return -1;
    }
    if(Main::Num_GEZ_Check(this->Watchdog->GetValue().ToStdString())!=0)
    {
        Main::Msgbox_Show(this,MSGBOX_ERROR,
                          _("Virtual Machine Config - Basic information"),
                          _("Watchdog is not a valid decimal nonnegative integer."));
        return -1;
    }
    if(Main::Num_GEZ_Check(this->Vector_Num->GetValue().ToStdString())!=0)
    {
        Main::Msgbox_Show(this,MSGBOX_ERROR,
                          _("Virtual Machine Config - Basic information"),
                          _("Vector Num is not a valid decimal nonnegative integer."));
        return -1;
    }
    if(this->Project_Output->GetValue()=="")
    {
        Main::Msgbox_Show(this,MSGBOX_ERROR,
                          _("Virtual Machine Config - Basic information"),
                          _("The project output folder is unspecified."));
        return -1;
    }
    if(this->Linker_Output->GetValue()=="")
    {
        Main::Msgbox_Show(this,MSGBOX_ERROR,
                          _("Virtual Machine Config - Basic information"),
                          _("The linker output folder is unspecified."));
        return -1;
    }
    if(this->Main_Header_Output->GetValue()=="")
    {
        Main::Msgbox_Show(this,MSGBOX_ERROR,
                          _("Virtual Machine Config - Basic information"),
                          _("The main header output folder is unspecified."));
        return -1;
    }
    if(this->Main_Source_Output->GetValue()=="")
    {
        Main::Msgbox_Show(this,MSGBOX_ERROR,
                          _("Virtual Machine Config - Basic information"),
                          _("The main source output folder is unspecified."));
        return -1;
    }
    if(this->Virtual_Header_Output->GetValue()=="")
    {
        Main::Msgbox_Show(this,MSGBOX_ERROR,
                          _("Virtual Machine Config - Basic information"),
                          _("The virtual header output folder is unspecified."));
        return -1;
    }
    if(this->Virtual_Source_Output->GetValue()=="")
    {
        Main::Msgbox_Show(this,MSGBOX_ERROR,
                          _("Virtual Machine Config - Basic information"),
                          _("The virtual source output folder is unspecified."));
        return -1;
    }

    /*  Compatible */
    Buildsystem=this->Buildsystem->GetStringSelection();
    Toolchain=this->Toolchain->GetStringSelection();
    if(Main::Comp_Check(Buildsystem,Toolchain,"RMP")!=0)
    {
        for(const std::unique_ptr<class Compatible>&Comp : Main::Plat_Info->Compatible)
        {
            if(Toolchain==Comp->Toolchain&&Comp_Buildsystem.Index(Comp->Buildsystem)==wxNOT_FOUND)
            {
                Comp_Buildsystem.push_back(Comp->Buildsystem);
                Comp_Sug=Comp_Sug+Comp->Toolchain+", "+Comp->Buildsystem+", "+"RMP\n";
            }
        }
        Main::Msgbox_Show(this, MSGBOX_ERROR,
                          _("Virtual Machine Config - Basic information"),
                          _("Compatibility error, you may try:\n"+Comp_Sug));
        return -1;
    }
    return 0;
}
/* End Function:Virtual_Basic_Panel::Check ***********************************/


/* Function:Virtual_Basic_Panel::Coprocessor_Set ******************************
Description : Set coprocessors.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
void Virtual_Basic_Panel::Coprocessor_Set()
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
/* End Function:Virtual_Basic_Panel::Coprocessor_Set *************************/

/* Function:Virtual_Basic_Panel::Buildsystem_Toolchain_Guest_Set **************
Description : Set buildsystem, toolchain and guest.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
void Virtual_Basic_Panel::Buildsystem_Toolchain_Guest_Set()
{
    for(std::unique_ptr<class Compatible>& Comp : Main::Plat_Info->Compatible)
    {
        if(this->Buildsystem->FindString(Comp->Buildsystem)==wxNOT_FOUND)
            this->Buildsystem->Append(Comp->Buildsystem);
        if(this->Toolchain->FindString(Comp->Toolchain)==wxNOT_FOUND)
            this->Toolchain->Append(Comp->Toolchain);
        if(this->Guest_Type->FindString(Comp->Guest)==wxNOT_FOUND&&Comp->Guest!="Native")
            this->Guest_Type->Append(Comp->Guest);
    }
    if(this->Buildsystem->GetCount()>0)
        this->Buildsystem->SetSelection(0);
    if(this->Toolchain->GetCount()>0)
        this->Toolchain->SetSelection(0);
    if(this->Guest_Type->GetCount()>0)
        this->Guest_Type->SetSelection(0);
}
/* End Function:Virtual_Basic_Panel::Buildsystem_Toolchain_Guest_Set *********/

/* Function:Virtual_Basic_Panel::On_Trans_Hex *********************************
Description : To Hex.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
void Virtual_Basic_Panel::On_Trans_Hex(class wxFocusEvent& Event)
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
/* End Function:Virtual_Basic_Panel::Check ***********************************/

/* Function:Virtual_Basic_Panel::On_Toolchain_Change **************************
Description : Get the first build system and guest which are compatible.
Input       : class wxFocusEvent& Event - The event.
Output      : None.
Return      : None.
******************************************************************************/
void Virtual_Basic_Panel::On_Toolchain_Change(class wxCommandEvent& Event)
{
    std::string Toolchain;

    Toolchain=this->Toolchain->GetStringSelection();
    for(std::unique_ptr<class Compatible>& Comp : Main::Plat_Info->Compatible)
    {
        if(Toolchain==Comp->Toolchain)
        {
            this->Guest_Type->SetStringSelection(Comp->Guest);
            this->Buildsystem->SetStringSelection(Comp->Buildsystem);
            break;
        }
    }
}
/* End Function:Virtual_Basic_Panel::On_Toolchain_Change *********************/

/* Function:Virtual_Basic_Panel::On_Buildsystem_Change ************************
Description : Get the first tool chain and guest which are compatible.
Input       : class wxFocusEvent& Event - The event.
Output      : None.
Return      : None.
******************************************************************************/
void Virtual_Basic_Panel::On_Buildsystem_Change(class wxCommandEvent& Event)
{
    std::string Buildsystem;

    Buildsystem=this->Buildsystem->GetStringSelection();
    for(std::unique_ptr<class Compatible>& Comp : Main::Plat_Info->Compatible)
    {
        if(Buildsystem==Comp->Buildsystem)
        {
            this->Guest_Type->SetStringSelection(Comp->Guest);
            this->Toolchain->SetStringSelection(Comp->Toolchain);
            break;
        }
    }
}
/* End Function:Virtual_Basic_Panel::On_Buildsystem_Change *******************/

/* Function:Virtual_Basic_Panel::On_Guest_Change ******************************
Description : Get the first tool chain and build system which are compatible.
Input       : class wxFocusEvent& Event - The event.
Output      : None.
Return      : None.
******************************************************************************/
void Virtual_Basic_Panel::On_Guest_Change(class wxCommandEvent& Event)
{
    std::string Guest;

    Guest=this->Guest_Type->GetStringSelection();
    for(std::unique_ptr<class Compatible>& Comp : Main::Plat_Info->Compatible)
    {
        if(Guest==Comp->Guest)
        {
            this->Toolchain->SetStringSelection(Comp->Toolchain);
            this->Buildsystem->SetStringSelection(Comp->Buildsystem);
            break;
        }
    }
}
/* End Function:Virtual_Basic_Panel::On_Guest_Change *************************/


/* Function:Virtual_Basic_Panel::On_Rename ************************************
Description : Pop up a rename dialog, and rename this native machine.
Input       : class wxMouseEvent& Event - The event.
Output      : None.
Return      : None.
******************************************************************************/
void Virtual_Basic_Panel::On_Rename(class wxMouseEvent& Event)
{
    ptr_t Dialog_State;
    std::string Current;
    std::string Original;
    /* Cookie is a typedef, not a class */
    wxTreeItemIdValue Cookie;
    class wxTreeItemId Child;

    /* Get the original name */
    Original=this->Name->GetValue();
    Main::Name_Dialog->Load(Original);

    /* Pop up the name dialog */
    Dialog_State=Main::Name_Dialog->ShowModal();
    if(Dialog_State!=wxID_OK)
        return;

    /* Get the new name, then check */
    Current=Main::Name_Dialog->Name_Get();
    if(Main::Proj_Info->Virtual_Rename(Original,Current))
    {
        if (!Main::Config->Virtual_Config.IsOk())
            return;
        /* Update config tree */
        Child=Main::Config->GetFirstChild(Main::Config->Virtual_Config,Cookie);
        while (Child.IsOk())
        {
            if(Main::Config->GetItemText(Child)==Original)
            {
                Main::Config->SetItemText(Child,Current);
                break;
            }
            Child=Main::Config->GetNextChild(Main::Config->Virtual_Config,Cookie);
        }
        /* Update this native process panel */
        this->Name->SetValue(Current);
    }
}
/* End Function:Virtual_Basic_Panel::On_Rename *******************************/

}
/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
