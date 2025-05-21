/******************************************************************************
Filename    : kernel_panel.cpp
Author      : lbc
Date        : 24/04/2025
License     : Proprietary; confidential.
Description : Kernel panel implementation.
******************************************************************************/

/* Include *******************************************************************/
#define __HDR_DEF__
#include "rvm_cfg.hpp"
#undef __HDR_DEF__

#include "wx/wx.h"
#include "wx/notebook.h"
#include "wx/xml/xml.h"
#include "wx/regex.h"

#include "set"
#include "map"
#include "memory"

#define __HDR_DEF__
#include "Option_Panel/Kernel_Panel/kernel_panel.hpp"
#undef __HDR_DEF__

#define __HDR_CLASS__
#include "rvm_cfg.hpp"
#include "Proj_Info/proj_info.hpp"
#include "Proj_Info/Kernel/kernel.hpp"

#include "Plat_Info/plat_info.hpp"
#include "Plat_Info/Compatible/compatible.hpp"

#include "Option_Panel/Kernel_Panel/kernel_panel.hpp"
#undef __HDR_CLASS__
/* End Include ***************************************************************/
namespace RVM_CFG
{
/* Function:Kernel_Panel::Kernel_Panel ****************************************
Description : Constructor for kernel panel.
Input       : class wxWindow* Parent - The parent window.
Output      : None.
Return      : None.
******************************************************************************/
/* void */ Kernel_Panel::Kernel_Panel(class wxWindow* Parent):
wxPanel(Parent,wxID_ANY)
{
    try
    {
         this->Main_Sizer=new class wxBoxSizer(wxVERTICAL);

        /* Group 1 */
        this->Sizer1=new class wxStaticBoxSizer(wxVERTICAL,this,_("Size1"));
        this->Sizer1_1=new class wxBoxSizer(wxHORIZONTAL);
        this->Sizer1_2=new class wxBoxSizer(wxHORIZONTAL);
        this->Sizer1_3=new class wxBoxSizer(wxHORIZONTAL);

        this->Code_Base_Sizer=new class wxBoxSizer(wxHORIZONTAL);
        this->Code_Base_Label=new class wxStaticText(this,wxID_ANY,_("Code Base"));
        this->Code_Base=new class wxTextCtrl(this,wxID_ANY);
        this->Code_Base_Sizer->Add(this->Code_Base_Label,1,wxALL|wxALIGN_CENTER_VERTICAL,I2P(5));
        this->Code_Base_Sizer->Add(this->Code_Base,2,wxALL|wxALIGN_CENTER_VERTICAL,I2P(5));

        this->Code_Size_Sizer=new class wxBoxSizer(wxHORIZONTAL);
        this->Code_Size_Label=new class wxStaticText(this,wxID_ANY,_("Code Size"));
        this->Code_Size=new class wxTextCtrl(this,wxID_ANY);
        this->Code_Size_Sizer->Add(this->Code_Size_Label,1,wxALL|wxALIGN_CENTER_VERTICAL,I2P(5));
        this->Code_Size_Sizer->Add(this->Code_Size,2,wxALL|wxALIGN_CENTER_VERTICAL,I2P(5));

        this->Data_Base_Sizer=new class wxBoxSizer(wxHORIZONTAL);
        this->Data_Base_Label=new class wxStaticText(this,wxID_ANY,_("Data Base"));
        this->Data_Base=new class wxTextCtrl(this,wxID_ANY);
        this->Data_Base_Sizer->Add(this->Data_Base_Label,1,wxALL|wxALIGN_CENTER_VERTICAL,I2P(5));
        this->Data_Base_Sizer->Add(this->Data_Base,2,wxALL|wxALIGN_CENTER_VERTICAL,I2P(5));

        this->Data_Size_Sizer=new class wxBoxSizer(wxHORIZONTAL);
        this->Data_Size_Label=new class wxStaticText(this,wxID_ANY,_("Data Size"));
        this->Data_Size=new class wxTextCtrl(this,wxID_ANY);
        this->Data_Size_Sizer->Add(this->Data_Size_Label,1,wxALL|wxALIGN_CENTER_VERTICAL,I2P(5));
        this->Data_Size_Sizer->Add(this->Data_Size,2,wxALL|wxALIGN_CENTER_VERTICAL,I2P(5));

        this->Kern_Prio_Sizer=new class wxBoxSizer(wxHORIZONTAL);
        this->Kern_Prio_Label=new class wxStaticText(this,wxID_ANY,_("Kernel Priority"));
        this->Kern_Prio=new class wxChoice(this,wxID_ANY);
        this->Kern_Prio_Sizer->Add(this->Kern_Prio_Label,1,wxALL|wxALIGN_CENTER_VERTICAL,I2P(5));
        this->Kern_Prio_Sizer->Add(this->Kern_Prio,2,wxALL|wxALIGN_CENTER_VERTICAL,I2P(5));

        this->Stack_Size_Sizer=new class wxBoxSizer(wxHORIZONTAL);
        this->Stack_Size_Label=new class wxStaticText(this,wxID_ANY,_("Stack Size"));
        this->Stack_Size=new class wxTextCtrl(this,wxID_ANY);
        this->Stack_Size_Sizer->Add(this->Stack_Size_Label,1,wxALL|wxALIGN_CENTER_VERTICAL,I2P(5));
        this->Stack_Size_Sizer->Add(this->Stack_Size,2,wxALL|wxALIGN_CENTER_VERTICAL,I2P(5));

        this->Sizer1_1->Add(this->Code_Base_Sizer,6,wxEXPAND);
        this->Sizer1_1->AddStretchSpacer(1);
        this->Sizer1_1->Add(this->Code_Size_Sizer,6,wxEXPAND);
        this->Sizer1_2->Add(this->Data_Base_Sizer,6,wxEXPAND);
        this->Sizer1_2->AddStretchSpacer(1);
        this->Sizer1_2->Add(this->Data_Size_Sizer,6,wxEXPAND);
        this->Sizer1_3->Add(this->Kern_Prio_Sizer,6,wxEXPAND);
        this->Sizer1_3->AddStretchSpacer(1);
        this->Sizer1_3->Add(this->Stack_Size_Sizer,6,wxEXPAND);
        this->Sizer1->Add(this->Sizer1_1,0,wxEXPAND);
        this->Sizer1->Add(this->Sizer1_2,0,wxEXPAND);
        this->Sizer1->Add(this->Sizer1_3,0,wxEXPAND);

        /* Group 2 */
        this->Sizer2=new class wxStaticBoxSizer(wxVERTICAL,this,_("Size2"));
        this->Sizer2_1=new class wxBoxSizer(wxHORIZONTAL);

        this->Kom_Order_Sizer=new class wxBoxSizer(wxHORIZONTAL);
        this->Kom_Order_Label=new class wxStaticText(this,wxID_ANY,_("KOM Order"));
        this->Kom_Order=new class wxChoice(this,wxID_ANY);
        /* This option is a power of 2 */
        this->Kom_Order->Append("3");
        this->Kom_Order->Append("4");
        this->Kom_Order->Append("5");
        this->Kom_Order->SetSelection(0);
        this->Kom_Order_Sizer->Add(this->Kom_Order_Label,1,wxALL|wxALIGN_CENTER_VERTICAL,I2P(5));
        this->Kom_Order_Sizer->Add(this->Kom_Order,2,wxALL|wxALIGN_CENTER_VERTICAL,I2P(5));

        this->Extra_Kom_Sizer=new class wxBoxSizer(wxHORIZONTAL);
        this->Extra_Kom_Label=new class wxStaticText(this,wxID_ANY,_("Extra KOM"));
        this->Extra_Kom=new class wxTextCtrl(this,wxID_ANY);
        this->Extra_Kom_Sizer->Add(this->Extra_Kom_Label,1,wxALL|wxALIGN_CENTER_VERTICAL,I2P(5));
        this->Extra_Kom_Sizer->Add(this->Extra_Kom,2,wxALL|wxALIGN_CENTER_VERTICAL,I2P(5));

        this->Sizer2_1->Add(this->Kom_Order_Sizer,6,wxEXPAND);
        this->Sizer2_1->AddStretchSpacer(1);
        this->Sizer2_1->Add(this->Extra_Kom_Sizer,6,wxEXPAND);
        this->Sizer2->Add(this->Sizer2_1,0,wxEXPAND);

        /* Group 3 */
        this->Sizer3=new class wxStaticBoxSizer(wxVERTICAL,this,_("Size3"));
        this->Sizer3_1=new class wxBoxSizer(wxHORIZONTAL);
        this->Sizer3_2=new class wxBoxSizer(wxHORIZONTAL);

        this->Toolchain_Sizer=new class wxBoxSizer(wxHORIZONTAL);
        this->Toolchain_Label=new class wxStaticText(this,wxID_ANY,_("Toolchain"));
        this->Toolchain=new class wxChoice(this,wxID_ANY);
        this->Toolchain_Sizer->Add(this->Toolchain_Label,1,wxALL|wxALIGN_CENTER_VERTICAL,I2P(5));
        this->Toolchain_Sizer->Add(this->Toolchain,2,wxALL|wxALIGN_CENTER_VERTICAL,I2P(5));
        this->Toolchain->Bind(wxEVT_CHOICE, &Kernel_Panel::On_Toolchain_Change, this, this->Toolchain->GetId());

        this->Buildsystem_Sizer=new class wxBoxSizer(wxHORIZONTAL);
        this->Buildsystem_Label=new class wxStaticText(this,wxID_ANY,_("Buildsystem"));
        this->Buildsystem=new class wxChoice(this,wxID_ANY);
        this->Buildsystem_Sizer->Add(this->Buildsystem_Label,1,wxALL|wxALIGN_CENTER_VERTICAL,I2P(5));
        this->Buildsystem_Sizer->Add(this->Buildsystem,2,wxALL|wxALIGN_CENTER_VERTICAL,I2P(5));
        //this->Buildsystem->Bind(wxEVT_CHOICE, &Kernel_Panel::On_Buildsystem_Change, this, this->Buildsystem->GetId());

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

        this->Full_Image_Sizer=new class wxBoxSizer(wxHORIZONTAL);
        this->Full_Image_Label=new class wxStaticText(this,wxID_ANY,_("Full Image"));
        this->Full_Image=new class wxCheckBox(this,wxID_ANY,wxEmptyString);
        this->Full_Image_Sizer->Add(this->Full_Image_Label,1,wxALL|wxALIGN_CENTER_VERTICAL,I2P(5));
        this->Full_Image_Sizer->Add(this->Full_Image,2,wxALL|wxALIGN_CENTER_VERTICAL,I2P(5));

        this->Sizer3_1->Add(this->Toolchain_Sizer,6,wxEXPAND);
        this->Sizer3_1->AddStretchSpacer(1);
        this->Sizer3_1->Add(this->Buildsystem_Sizer,6,wxEXPAND);
        this->Sizer3_2->Add(this->Optimization_Sizer,6,wxEXPAND);
        this->Sizer3_2->AddStretchSpacer(1);
        this->Sizer3_2->Add(this->Full_Image_Sizer,6,wxEXPAND);
        this->Sizer3->Add(this->Sizer3_1,0,wxEXPAND);
        this->Sizer3->Add(this->Sizer3_2,0,wxEXPAND);

        /* Group 4 */
        this->Sizer4=new class wxStaticBoxSizer(wxVERTICAL,this,_("Size4"));

        this->Project_Output_Sizer=new class wxBoxSizer(wxHORIZONTAL);
        this->Project_Output_Label=new class wxStaticText(this,wxID_ANY,_("Project Output"));
        this->Project_Output=new class wxTextCtrl(this,wxID_ANY);
        this->Project_Overwrite=new class wxCheckBox(this,wxID_ANY,_("Overwrite"));
        this->Project_Output_Sizer->Add(this->Project_Output_Label,5,wxALL|wxALIGN_CENTER_VERTICAL,I2P(5));
        this->Project_Output_Sizer->Add(this->Project_Output,16,wxALL|wxALIGN_CENTER_VERTICAL,I2P(5));
        this->Project_Output_Sizer->Add(this->Project_Overwrite,3,wxALL|wxALIGN_CENTER_VERTICAL,I2P(5));

        this->Linker_Output_Sizer=new class wxBoxSizer(wxHORIZONTAL);
        this->Linker_Output_Label=new class wxStaticText(this,wxID_ANY,_("Linker Output"));
        this->Linker_Output=new class wxTextCtrl(this,wxID_ANY);
        this->Linker_Output_Sizer->Add(this->Linker_Output_Label,5,wxALL|wxALIGN_CENTER_VERTICAL,I2P(5));
        this->Linker_Output_Sizer->Add(this->Linker_Output,16,wxALL|wxALIGN_CENTER_VERTICAL,I2P(5));;
        this->Linker_Output_Sizer->AddStretchSpacer(3);

        this->Config_Header_Output_Sizer=new class wxBoxSizer(wxHORIZONTAL);
        this->Config_Header_Output_Label=new class wxStaticText(this,wxID_ANY,_("Config Header Output"));
        this->Config_Header_Output=new class wxTextCtrl(this,wxID_ANY);
        this->Config_Header_Output_Sizer->Add(this->Config_Header_Output_Label,5,wxALL|wxALIGN_CENTER_VERTICAL,I2P(5));
        this->Config_Header_Output_Sizer->Add(this->Config_Header_Output,16,wxALL|wxALIGN_CENTER_VERTICAL,I2P(5));
        this->Config_Header_Output_Sizer->AddStretchSpacer(3);

        this->Boot_Header_Output_Sizer=new class wxBoxSizer(wxHORIZONTAL);
        this->Boot_Header_Output_Label=new class wxStaticText(this,wxID_ANY,_("Boot Header Output"));
        this->Boot_Header_Output=new class wxTextCtrl(this,wxID_ANY);
        this->Boot_Header_Output_Sizer->Add(this->Boot_Header_Output_Label,5,wxALL|wxALIGN_CENTER_VERTICAL,I2P(5));
        this->Boot_Header_Output_Sizer->Add(this->Boot_Header_Output,16,wxALL|wxALIGN_CENTER_VERTICAL,I2P(5));
        this->Boot_Header_Output_Sizer->AddStretchSpacer(3);

        this->Boot_Source_Output_Sizer=new class wxBoxSizer(wxHORIZONTAL);
        this->Boot_Source_Output_Label=new class wxStaticText(this,wxID_ANY,_("Boot Source Output"));
        this->Boot_Source_Output=new class wxTextCtrl(this,wxID_ANY);
        this->Boot_Source_Output_Sizer->Add(this->Boot_Source_Output_Label,5,wxALL|wxALIGN_CENTER_VERTICAL,I2P(5));
        this->Boot_Source_Output_Sizer->Add(this->Boot_Source_Output,16,wxALL|wxALIGN_CENTER_VERTICAL,I2P(5));
        this->Boot_Source_Output_Sizer->AddStretchSpacer(3);

        this->Hook_Source_Sizer=new class wxBoxSizer(wxHORIZONTAL);
        this->Hook_Source_Output_Label=new class wxStaticText(this,wxID_ANY,_("Hook Source Output"));
        this->Hook_Source_Output=new class wxTextCtrl(this,wxID_ANY,_("../Source/"));
        this->Hook_Source_Overwrite=new class wxCheckBox(this,wxID_ANY,_("Overwirte"));
        this->Hook_Source_Sizer->Add(this->Hook_Source_Output_Label,5,wxALL|wxALIGN_CENTER_VERTICAL,I2P(5));
        this->Hook_Source_Sizer->Add(this->Hook_Source_Output,16,wxALL|wxALIGN_CENTER_VERTICAL,I2P(5));
        this->Hook_Source_Sizer->Add(this->Hook_Source_Overwrite,3,wxALL|wxALIGN_CENTER_VERTICAL,I2P(5));

        this->Handler_Source_Sizer=new class wxBoxSizer(wxHORIZONTAL);
        this->Handler_Source_Output_Label=new class wxStaticText(this,wxID_ANY,_("Handler Source Output"));
        this->Handler_Source_Output=new class wxTextCtrl(this,wxID_ANY);
        this->Handler_Source_Overwrite=new class wxCheckBox(this,wxID_ANY,_("Overwrite"));
        this->Handler_Source_Sizer->Add(this->Handler_Source_Output_Label,5,wxALL|wxALIGN_CENTER_VERTICAL,I2P(5));
        this->Handler_Source_Sizer->Add(this->Handler_Source_Output,16,wxALL|wxALIGN_CENTER_VERTICAL,I2P(5));
        this->Handler_Source_Sizer->Add(this->Handler_Source_Overwrite,3,wxALL|wxALIGN_CENTER_VERTICAL,I2P(5));

        this->Sizer4->Add(this->Project_Output_Sizer,0,wxEXPAND);
        this->Sizer4->Add(this->Linker_Output_Sizer,0,wxEXPAND);
        this->Sizer4->Add(this->Config_Header_Output_Sizer,0,wxEXPAND);
        this->Sizer4->Add(this->Boot_Header_Output_Sizer,0,wxEXPAND);
        this->Sizer4->Add(this->Boot_Source_Output_Sizer,0,wxEXPAND);
        this->Sizer4->Add(this->Hook_Source_Sizer,0,wxEXPAND);
        this->Sizer4->Add(this->Handler_Source_Sizer,0,wxEXPAND);

        this->Main_Sizer->Add(this->Sizer1,0,wxEXPAND|wxALL,I2P(5));
        this->Main_Sizer->Add(this->Sizer2,0,wxEXPAND|wxALL,I2P(5));
        this->Main_Sizer->Add(this->Sizer3,0,wxEXPAND|wxALL,I2P(5));
        this->Main_Sizer->Add(this->Sizer4,0,wxEXPAND|wxALL,I2P(5));

        this->SetSizer(this->Main_Sizer);
        this->Layout();

        this->Code_Base->Bind(wxEVT_KILL_FOCUS, &Kernel_Panel::On_Trans_Hex, this);
        this->Code_Size->Bind(wxEVT_KILL_FOCUS, &Kernel_Panel::On_Trans_Hex, this);
        this->Data_Base->Bind(wxEVT_KILL_FOCUS, &Kernel_Panel::On_Trans_Hex, this);
        this->Data_Size->Bind(wxEVT_KILL_FOCUS, &Kernel_Panel::On_Trans_Hex, this);
        this->Stack_Size->Bind(wxEVT_KILL_FOCUS, &Kernel_Panel::On_Trans_Hex, this);
        this->Extra_Kom->Bind(wxEVT_KILL_FOCUS, &Kernel_Panel::On_Trans_Hex, this);
    }
    catch(std::exception& Exc)
    {
        ASSERT(0,Exc.what());
    }
}
/* End Function:Kernel_Panel::Kernel_Panel ***********************************/

/* Function:Kernel_Panel::~Kernel_Panel ***************************************
Description : Destructor for kernel panel.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
/* void */ Kernel_Panel::~Kernel_Panel(void)
{

}
/* End Function:Kernel_Panel::~Kernel_Panel **********************************/

/* Function:Kernel_Panel::Load ************************************************
Description : Load information onto kernel panel."
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
void Kernel_Panel::Load(void)
{
    char Buf[32];

    std::sprintf(Buf, "0x%llX", Main::Proj_Info->Kernel->Code_Base);
    this->Code_Base->SetValue(Buf);
    std::sprintf(Buf, "0x%llX", Main::Proj_Info->Kernel->Code_Size);
    this->Code_Size->SetValue(Buf);
    std::sprintf(Buf, "0x%llX", Main::Proj_Info->Kernel->Data_Base);
    this->Data_Base->SetValue(Buf);
    std::sprintf(Buf, "0x%llX", Main::Proj_Info->Kernel->Data_Size);
    this->Data_Size->SetValue(Buf);
    std::sprintf(Buf, "0x%llX", Main::Proj_Info->Kernel->Stack_Size);
    this->Stack_Size->SetValue(Buf);
    std::sprintf(Buf, "0x%llX", Main::Proj_Info->Kernel->Extra_Kom);
    this->Extra_Kom->SetValue(Buf);
    std::sprintf(Buf, "%lld", Main::Proj_Info->Kernel->Kom_Order);
    this->Kom_Order->SetStringSelection(Buf);
    std::sprintf(Buf, "%lld", Main::Proj_Info->Kernel->Kern_Prio);
    this->Kern_Prio->SetStringSelection(Buf);
    this->Buildsystem->SetStringSelection(Main::Proj_Info->Kernel->Buildsystem);
    this->Toolchain->SetStringSelection(Main::Proj_Info->Kernel->Toolchain);
    this->Optimization->SetSelection(Main::Proj_Info->Kernel->Optimization);
    this->Full_Image->SetValue(Main::Proj_Info->Kernel->Full_Image);

    this->Project_Output->SetValue(Main::Proj_Info->Kernel->Project_Output);
    this->Project_Overwrite->SetValue(Main::Proj_Info->Kernel->Project_Overwrite);
    this->Linker_Output->SetValue(Main::Proj_Info->Kernel->Linker_Output);
    this->Config_Header_Output->SetValue(Main::Proj_Info->Kernel->Config_Header_Output);
    this->Boot_Header_Output->SetValue(Main::Proj_Info->Kernel->Boot_Header_Output);
    this->Boot_Source_Output->SetValue(Main::Proj_Info->Kernel->Boot_Source_Output);
    this->Hook_Source_Output->SetValue(Main::Proj_Info->Kernel->Hook_Source_Output);
    this->Hook_Source_Overwrite->SetValue(Main::Proj_Info->Kernel->Hook_Source_Overwrite);
    this->Handler_Source_Output->SetValue(Main::Proj_Info->Kernel->Handler_Source_Output);
    this->Handler_Source_Overwrite->SetValue(Main::Proj_Info->Kernel->Handler_Source_Overwrite);
}
/* End Function:Kernel_Panel::Load *******************************************/

/* Function:Kernel_Panel::Save ************************************************
Description : Save information to project information.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
void Kernel_Panel::Save(void)
{
    Main::Proj_Info->Kernel->Code_Base=std::stoull(this->Code_Base->GetValue().ToStdString(),0,0);
    Main::Proj_Info->Kernel->Code_Size=std::stoull(this->Code_Size->GetValue().ToStdString(),0,0);
    Main::Proj_Info->Kernel->Data_Base=std::stoull(this->Data_Base->GetValue().ToStdString(),0,0);
    Main::Proj_Info->Kernel->Data_Size=std::stoull(this->Data_Size->GetValue().ToStdString(),0,0);
    Main::Proj_Info->Kernel->Stack_Size=std::stoull(this->Stack_Size->GetValue().ToStdString(),0,0);
    Main::Proj_Info->Kernel->Extra_Kom=std::stoull(this->Extra_Kom->GetValue().ToStdString(),0,0);
    Main::Proj_Info->Kernel->Kom_Order=std::stoull(this->Kom_Order->GetStringSelection().ToStdString(),0,0);
    Main::Proj_Info->Kernel->Kern_Prio=std::stoull(this->Kern_Prio->GetStringSelection().ToStdString(),0,0);
    Main::Proj_Info->Kernel->Buildsystem=this->Buildsystem->GetStringSelection();
    Main::Proj_Info->Kernel->Toolchain=this->Toolchain->GetStringSelection();
    Main::Proj_Info->Kernel->Optimization=this->Optimization->GetSelection();
    Main::Proj_Info->Kernel->Full_Image=this->Full_Image->GetValue();

    Main::Proj_Info->Kernel->Project_Output=this->Project_Output->GetValue();
    Main::Proj_Info->Kernel->Project_Overwrite=this->Project_Overwrite->GetValue();
    Main::Proj_Info->Kernel->Linker_Output=this->Linker_Output->GetValue();
    Main::Proj_Info->Kernel->Config_Header_Output=this->Config_Header_Output->GetValue();
    Main::Proj_Info->Kernel->Boot_Header_Output=this->Boot_Header_Output->GetValue();
    Main::Proj_Info->Kernel->Boot_Source_Output=this->Boot_Source_Output->GetValue();
    Main::Proj_Info->Kernel->Hook_Source_Output=this->Hook_Source_Output->GetValue();
    Main::Proj_Info->Kernel->Hook_Source_Overwrite=this->Hook_Source_Overwrite->GetValue();
    Main::Proj_Info->Kernel->Handler_Source_Output=this->Handler_Source_Output->GetValue();
    Main::Proj_Info->Kernel->Handler_Source_Overwrite=this->Handler_Source_Overwrite->GetValue();
}
/* End Function:Kernel_Panel::Save *******************************************/

/* Function:Kernel_Panel::Check ***********************************************
Description : Check whether the kernel panel contains any errors..
Input       : None.
Output      : None.
Return      : ret_t - if 0, no error exists; else -1.
******************************************************************************/
ret_t Kernel_Panel::Check(void)
{
    std::string Comp_Sug;
    std::string Toolchain;
    std::string Buildsystem;
    class wxArrayString Comp_Buildsystem;

    if(Main::Num_GEZ_Hex_Check(this->Code_Base->GetValue().ToStdString())!=0)
    {
        Main::Msgbox_Show(this, MSGBOX_ERROR,
                          _("Kernel Config"),
                          _("Code base is not a valid hexadecimal nonnegative integer"));
        return -1;
    }
    if(Main::Num_GEZ_Hex_Check(this->Code_Size->GetValue().ToStdString())!=0)
    {
        Main::Msgbox_Show(this, MSGBOX_ERROR,
                          _("Kernel Config"),
                          _("Code size is not a valid hexadecimal nonnegative integer"));
        return -1;
    }
    if(Main::Num_GEZ_Hex_Check(this->Data_Base->GetValue().ToStdString())!=0)
    {
        Main::Msgbox_Show(this, MSGBOX_ERROR,
                          _("Kernel Config"),
                          _("Data base is not a valid hexadecimal nonnegative integer"));
        return -1;
    }
    if(Main::Num_GEZ_Hex_Check(this->Data_Size->GetValue().ToStdString())!=0)
    {
        Main::Msgbox_Show(this, MSGBOX_ERROR,
                          _("Kernel Config"),
                          _("Data size is not a valid hexadecimal nonnegative integer"));
        return -1;
    }
    if(Main::Num_GEZ_Hex_Check(this->Stack_Size->GetValue().ToStdString())!=0)
    {
        Main::Msgbox_Show(this, MSGBOX_ERROR,
                          _("Kernel Config"),
                          _("Stack size is not a valid hexadecimal nonnegative integer"));
        return -1;
    }
    if(Main::Num_GEZ_Hex_Check(this->Extra_Kom->GetValue().ToStdString())!=0)
    {
        Main::Msgbox_Show(this, MSGBOX_ERROR,
                          _("Kernel Config"),
                          _("Extra KOM is not a valid hexadecimal nonnegative integer"));
        return -1;
    }
    if(Main::Num_GEZ_Check(this->Kom_Order->GetStringSelection().ToStdString())!=0)
    {
        Main::Msgbox_Show(this, MSGBOX_ERROR,
                          _("Kernel Config"),
                          _("KOM order is not a valid decimal nonnegative integer"));
        return -1;
    }
    if(Main::Num_GEZ_Check(this->Kern_Prio->GetStringSelection().ToStdString())!=0)
    {
        Main::Msgbox_Show(this, MSGBOX_ERROR,
                          _("Kernel Config"),
                          _("Kernel priority is not a valid decimal nonnegative integer"));
        return -1;
    }

    /* Output check */
    if(this->Project_Output->GetValue()=="")
    {
        Main::Msgbox_Show(this,MSGBOX_ERROR,
                          _("Kernel Config"),
                          _("The project output folder is unspecified."));
        return -1;
    }
    if(this->Linker_Output->GetValue()=="")
    {
        Main::Msgbox_Show(this,MSGBOX_ERROR,
                          _("Kernel Config"),
                          _("The linker output folder is unspecified."));
        return -1;
    }
    if(this->Config_Header_Output->GetValue()=="")
    {
        Main::Msgbox_Show(this,MSGBOX_ERROR,
                          _("Kernel Config"),
                          _("The config header output folder is unspecified."));
        return -1;
    }
    if(this->Boot_Header_Output->GetValue()=="")
    {
        Main::Msgbox_Show(this,MSGBOX_ERROR,
                          _("Kernel Config"),
                          _("The boot header output folder is unspecified."));
        return -1;
    }
    if(this->Boot_Source_Output->GetValue()=="")
    {
        Main::Msgbox_Show(this,MSGBOX_ERROR,
                          _("Kernel Config"),
                          _("The boot source output folder is unspecified."));
        return -1;
    }
    if(this->Hook_Source_Output->GetValue()=="")
    {
        Main::Msgbox_Show(this,MSGBOX_ERROR,
                          _("Kernel Config"),
                          _("The hook source output folder is unspecified."));
        return -1;
    }
    if(this->Handler_Source_Output->GetValue()=="")
    {
        Main::Msgbox_Show(this,MSGBOX_ERROR,
                          _("Kernel Config"),
                          _("The handler source output folder is unspecified."));
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
                          _("Kernel Config"),
                          _("Compatibility error, you may try:\n"+Comp_Sug));
        return -1;
    }
    return 0;
}
/* End Function:Kernel_Panel::Check ******************************************/

/* Function:Kernel_Panel::Compatible_Set **************************************
Description : Set build system and tool chain.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
void Kernel_Panel::Compatible_Set()
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
/* End Function:Kernel_Panel::Compatible_Set *********************************/

/* Function:Kernel_Panel::Kernel_Prio_Set *************************************
Description : Set kernel priority according to word length.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
void Kernel_Panel::Kernel_Prio_Set()
{
    cnt_t Cnt;
    ptr_t Word_Length;

    Word_Length=Main::Plat_Info->Wordlength;

    this->Kern_Prio->Clear();
    for(Cnt=1;Cnt<=4;++Cnt)
        this->Kern_Prio->Append(std::to_string(Word_Length*Cnt));

}
/* End Function:Kernel_Panel::Kernel_Prio_Set ********************************/

/* Function:Kernel_Panel::On_Trans_Hex ****************************************
Description : Convert the content of the control to upper case letters and add
              the '0x'  as prefix.
Input       : class wxFocusEvent& Event - The event.
Output      : None.
Return      : None.
******************************************************************************/
void Kernel_Panel::On_Trans_Hex(class wxFocusEvent& Event)
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
/* End Function:Kernel_Panel::On_Trans_Hex ***********************************/

/* Function:Kernel_Panel::On_Toolchain_Change *********************************
Description : Configure a compatible build system based on the tool chain settings.
Input       : class wxFocusEvent& Event - The event.
Output      : None.
Return      : None.
******************************************************************************/
void Kernel_Panel::On_Toolchain_Change(class wxCommandEvent& Event)
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
/* End Function:Kernel_Panel::On_Toolchain_Change ****************************/

}
/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
