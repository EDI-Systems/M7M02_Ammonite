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
        this->SetBackgroundColour(Parent->GetBackgroundColour());

        this->Main_Sizer=new class wxBoxSizer(wxVERTICAL);

        /* Basic options */
        this->Basic_Sizer=new class wxStaticBoxSizer(wxVERTICAL,this,_("Basic Options"));
        this->Basic_Line1_Sizer=new class wxBoxSizer(wxHORIZONTAL);
        this->Basic_Line2_Sizer=new class wxBoxSizer(wxHORIZONTAL);
        this->Basic_Line3_Sizer=new class wxBoxSizer(wxHORIZONTAL);

        this->Code_Base_Sizer=new class wxBoxSizer(wxHORIZONTAL);
        this->Code_Base_Label=new class wxStaticText(this,wxID_ANY,_("Code Base"));
        this->Code_Base=new class wxTextCtrl(this,wxID_ANY);
        this->Code_Base->Bind(wxEVT_KILL_FOCUS, &Kernel_Panel::On_Text_Hex, this);
        this->Code_Base_Sizer->Add(this->Code_Base_Label,1,wxALL|wxALIGN_CENTER_VERTICAL,I2P(5));
        this->Code_Base_Sizer->Add(this->Code_Base,1,wxALL|wxALIGN_CENTER_VERTICAL,I2P(5));

        this->Code_Size_Sizer=new class wxBoxSizer(wxHORIZONTAL);
        this->Code_Size_Label=new class wxStaticText(this,wxID_ANY,_("Code Size"));
        this->Code_Size=new class wxTextCtrl(this,wxID_ANY);
        this->Code_Size->Bind(wxEVT_KILL_FOCUS, &Kernel_Panel::On_Text_Hex, this);
        this->Code_Size_Sizer->Add(this->Code_Size_Label,1,wxALL|wxALIGN_CENTER_VERTICAL,I2P(5));
        this->Code_Size_Sizer->Add(this->Code_Size,1,wxALL|wxALIGN_CENTER_VERTICAL,I2P(5));

        this->Data_Base_Sizer=new class wxBoxSizer(wxHORIZONTAL);
        this->Data_Base_Label=new class wxStaticText(this,wxID_ANY,_("Data Base"));
        this->Data_Base=new class wxTextCtrl(this,wxID_ANY);
        this->Data_Base->Bind(wxEVT_KILL_FOCUS, &Kernel_Panel::On_Text_Hex, this);
        this->Data_Base_Sizer->Add(this->Data_Base_Label,1,wxALL|wxALIGN_CENTER_VERTICAL,I2P(5));
        this->Data_Base_Sizer->Add(this->Data_Base,1,wxALL|wxALIGN_CENTER_VERTICAL,I2P(5));

        this->Data_Size_Sizer=new class wxBoxSizer(wxHORIZONTAL);
        this->Data_Size_Label=new class wxStaticText(this,wxID_ANY,_("Data Size"));
        this->Data_Size=new class wxTextCtrl(this,wxID_ANY);
        this->Data_Size->Bind(wxEVT_KILL_FOCUS, &Kernel_Panel::On_Text_Hex, this);
        this->Data_Size_Sizer->Add(this->Data_Size_Label,1,wxALL|wxALIGN_CENTER_VERTICAL,I2P(5));
        this->Data_Size_Sizer->Add(this->Data_Size,1,wxALL|wxALIGN_CENTER_VERTICAL,I2P(5));

        this->Kern_Prio_Sizer=new class wxBoxSizer(wxHORIZONTAL);
        this->Kern_Prio_Label=new class wxStaticText(this,wxID_ANY,_("Kernel Priority Number"));
        this->Kern_Prio=new class wxChoice(this,wxID_ANY);
        this->Kern_Prio_Sizer->Add(this->Kern_Prio_Label,1,wxALL|wxALIGN_CENTER_VERTICAL,I2P(5));
        this->Kern_Prio_Sizer->Add(this->Kern_Prio,1,wxALL|wxALIGN_CENTER_VERTICAL,I2P(5));

        this->Stack_Size_Sizer=new class wxBoxSizer(wxHORIZONTAL);
        this->Stack_Size_Label=new class wxStaticText(this,wxID_ANY,_("Stack Size"));
        this->Stack_Size=new class wxTextCtrl(this,wxID_ANY);
        this->Stack_Size->Bind(wxEVT_KILL_FOCUS, &Kernel_Panel::On_Text_Hex, this);
        this->Stack_Size_Sizer->Add(this->Stack_Size_Label,1,wxALL|wxALIGN_CENTER_VERTICAL,I2P(5));
        this->Stack_Size_Sizer->Add(this->Stack_Size,1,wxALL|wxALIGN_CENTER_VERTICAL,I2P(5));

        this->Basic_Line1_Sizer->Add(this->Code_Base_Sizer,6,wxEXPAND);
        this->Basic_Line1_Sizer->AddStretchSpacer(1);
        this->Basic_Line1_Sizer->Add(this->Code_Size_Sizer,6,wxEXPAND);

        this->Basic_Line2_Sizer->Add(this->Data_Base_Sizer,6,wxEXPAND);
        this->Basic_Line2_Sizer->AddStretchSpacer(1);
        this->Basic_Line2_Sizer->Add(this->Data_Size_Sizer,6,wxEXPAND);

        this->Basic_Line3_Sizer->Add(this->Kern_Prio_Sizer,6,wxEXPAND);
        this->Basic_Line3_Sizer->AddStretchSpacer(1);
        this->Basic_Line3_Sizer->Add(this->Stack_Size_Sizer,6,wxEXPAND);

        this->Basic_Sizer->Add(this->Basic_Line1_Sizer,0,wxEXPAND);
        this->Basic_Sizer->Add(this->Basic_Line2_Sizer,0,wxEXPAND);
        this->Basic_Sizer->Add(this->Basic_Line3_Sizer,0,wxEXPAND);

        /* Kernel memory options */
        this->KOM_Sizer=new class wxStaticBoxSizer(wxVERTICAL,this,_("Kernel Memory Options"));
        this->KOM_Line1_Sizer=new class wxBoxSizer(wxHORIZONTAL);

        this->Kom_Order_Sizer=new class wxBoxSizer(wxHORIZONTAL);
        this->Kom_Order_Label=new class wxStaticText(this,wxID_ANY,_("Allocation Granularity"));
        this->Kom_Order=new class wxChoice(this,wxID_ANY);
        /* This option is a power of 2 */
        this->Kom_Order->Append("3 (8B)");
        this->Kom_Order->Append("4 (16B)");
        this->Kom_Order->Append("5 (32B)");
        this->Kom_Order->Append("6 (64B)");
        this->Kom_Order->SetSelection(0);
        this->Kom_Order_Sizer->Add(this->Kom_Order_Label,1,wxALL|wxALIGN_CENTER_VERTICAL,I2P(5));
        this->Kom_Order_Sizer->Add(this->Kom_Order,1,wxALL|wxALIGN_CENTER_VERTICAL,I2P(5));

        this->Extra_Kom_Sizer=new class wxBoxSizer(wxHORIZONTAL);
        this->Extra_Kom_Label=new class wxStaticText(this,wxID_ANY,_("Extra Amount"));
        this->Extra_Kom=new class wxTextCtrl(this,wxID_ANY);
        this->Extra_Kom->Bind(wxEVT_KILL_FOCUS, &Kernel_Panel::On_Text_Hex, this);
        this->Extra_Kom_Sizer->Add(this->Extra_Kom_Label,1,wxALL|wxALIGN_CENTER_VERTICAL,I2P(5));
        this->Extra_Kom_Sizer->Add(this->Extra_Kom,1,wxALL|wxALIGN_CENTER_VERTICAL,I2P(5));

        this->KOM_Line1_Sizer->Add(this->Kom_Order_Sizer,6,wxEXPAND);
        this->KOM_Line1_Sizer->AddStretchSpacer(1);
        this->KOM_Line1_Sizer->Add(this->Extra_Kom_Sizer,6,wxEXPAND);
        this->KOM_Sizer->Add(this->KOM_Line1_Sizer,0,wxEXPAND);

        /* Buildsystem options */
        this->Build_Sizer=new class wxStaticBoxSizer(wxVERTICAL,this,_("Buildsystem Options"));
        this->Build_Line1_Sizer=new class wxBoxSizer(wxHORIZONTAL);
        this->Build_Line2_Sizer=new class wxBoxSizer(wxHORIZONTAL);

        this->Toolchain_Sizer=new class wxBoxSizer(wxHORIZONTAL);
        this->Toolchain_Label=new class wxStaticText(this,wxID_ANY,_("Toolchain"));
        this->Toolchain=new class wxChoice(this,wxID_ANY);
        this->Bind(wxEVT_CHOICE, &Kernel_Panel::On_Toolchain, this, this->Toolchain->GetId());
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

        this->Full_Image_Sizer=new class wxBoxSizer(wxHORIZONTAL);
        this->Full_Image_Label=new class wxStaticText(this,wxID_ANY,_("Generate Full Image"));
        this->Full_Image=new class wxCheckBox(this,wxID_ANY,wxEmptyString);
        this->Full_Image_Sizer->Add(this->Full_Image_Label,1,wxALL|wxALIGN_CENTER_VERTICAL,I2P(5));
        this->Full_Image_Sizer->Add(this->Full_Image,1,wxALL|wxALIGN_CENTER_VERTICAL,I2P(5));

        this->Build_Line1_Sizer->Add(this->Toolchain_Sizer,6,wxEXPAND);
        this->Build_Line1_Sizer->AddStretchSpacer(1);
        this->Build_Line1_Sizer->Add(this->Buildsystem_Sizer,6,wxEXPAND);

        this->Build_Line2_Sizer->Add(this->Optimization_Sizer,6,wxEXPAND);
        this->Build_Line2_Sizer->AddStretchSpacer(1);
        this->Build_Line2_Sizer->Add(this->Full_Image_Sizer,6,wxEXPAND);

        this->Build_Sizer->Add(this->Build_Line1_Sizer,0,wxEXPAND);
        this->Build_Sizer->Add(this->Build_Line2_Sizer,0,wxEXPAND);

        /* Output options */
        this->Output_Sizer=new class wxStaticBoxSizer(wxVERTICAL,this,_("Output Options"));

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

        this->Output_Sizer->Add(this->Project_Output_Sizer,0,wxEXPAND);
        this->Output_Sizer->Add(this->Linker_Output_Sizer,0,wxEXPAND);
        this->Output_Sizer->Add(this->Config_Header_Output_Sizer,0,wxEXPAND);
        this->Output_Sizer->Add(this->Boot_Header_Output_Sizer,0,wxEXPAND);
        this->Output_Sizer->Add(this->Boot_Source_Output_Sizer,0,wxEXPAND);
        this->Output_Sizer->Add(this->Hook_Source_Sizer,0,wxEXPAND);
        this->Output_Sizer->Add(this->Handler_Source_Sizer,0,wxEXPAND);

        this->Main_Sizer->Add(this->Basic_Sizer,0,wxEXPAND|wxALL,I2P(5));
        this->Main_Sizer->Add(this->KOM_Sizer,0,wxEXPAND|wxALL,I2P(5));
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
/* End Function:Kernel_Panel::Kernel_Panel ***********************************/

/* Function:Kernel_Panel::~Kernel_Panel ***************************************
Description : Destructor for kernel panel.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
/* void */ Kernel_Panel::~Kernel_Panel(void)
{
    this->Unbind(wxEVT_CHOICE, &Kernel_Panel::On_Toolchain, this, this->Toolchain->GetId());
    /* Have to bind to the text itself because we are processing window events on it directly */
    this->Code_Base->Unbind(wxEVT_KILL_FOCUS, &Kernel_Panel::On_Text_Hex, this);
    this->Code_Size->Unbind(wxEVT_KILL_FOCUS, &Kernel_Panel::On_Text_Hex, this);
    this->Data_Base->Unbind(wxEVT_KILL_FOCUS, &Kernel_Panel::On_Text_Hex, this);
    this->Data_Size->Unbind(wxEVT_KILL_FOCUS, &Kernel_Panel::On_Text_Hex, this);
    this->Stack_Size->Unbind(wxEVT_KILL_FOCUS, &Kernel_Panel::On_Text_Hex, this);
    this->Extra_Kom->Unbind(wxEVT_KILL_FOCUS, &Kernel_Panel::On_Text_Hex, this);
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

    /* Basic options */
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
    std::sprintf(Buf, "%lld", Main::Proj_Info->Kernel->Kern_Prio);
    this->Kern_Prio->SetStringSelection(Buf);

    /* Kernel memory options */
    std::sprintf(Buf, "%lld", Main::Proj_Info->Kernel->Kom_Order);
    this->Kom_Order->SetStringSelection(Buf);
    std::sprintf(Buf, "0x%llX", Main::Proj_Info->Kernel->Extra_Kom);
    this->Extra_Kom->SetValue(Buf);

    this->Toolchain->SetStringSelection(Main::Proj_Info->Kernel->Toolchain);
    this->Buildsystem_Set(this->Toolchain->GetStringSelection());
    this->Buildsystem->SetStringSelection(Main::Proj_Info->Kernel->Buildsystem);
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

/* Function:Kernel_Panel::Check ***********************************************
Description : Check whether the kernel panel contains any errors..
Input       : None.
Output      : None.
Return      : ret_t - if 0, no error exists; else -1.
******************************************************************************/
ret_t Kernel_Panel::Check(void)
{
    /* Code base & size */
    if(Main::Hex_Check(this,this->Code_Base->GetValue(),_("Kernel Config"),_("Code base"))!=0)
        return -1;
    if(Main::Hex_Pos_Check(this,this->Code_Size->GetValue(),_("Kernel Config"),_("Code size"))!=0)
        return -1;

    /* Data base & size */
    if(Main::Hex_Check(this,this->Data_Base->GetValue(),_("Kernel Config"),_("Data base"))!=0)
        return -1;
    if(Main::Hex_Pos_Check(this,this->Data_Size->GetValue(),_("Kernel Config"),_("Data size"))!=0)
        return -1;

    /* Stack size */
    if(Main::Hex_Pos_Check(this,this->Stack_Size->GetValue(),_("Kernel Config"),_("Stack size"))!=0)
        return -1;

    /* Extra kernel memory */
    if(Main::Hex_Check(this,this->Extra_Kom->GetValue(),_("Kernel Config"),"Extra amount")!=0)
        return -1;

    /* Output check */
    if(this->Project_Output->GetValue()=="")
    {
        Main::Msgbox_Show(this,MSGBOX_ERROR,
                          _("Kernel Config"),
                          "\""+_("Project output folder")+"\""+_(" is unspecified."));
        return -1;
    }

    if(this->Linker_Output->GetValue()=="")
    {
        Main::Msgbox_Show(this,MSGBOX_ERROR,
                          _("Kernel Config"),
                          "\""+_("Linker script output folder")+"\""+_(" is unspecified."));
        return -1;
    }

    if(this->Config_Header_Output->GetValue()=="")
    {
        Main::Msgbox_Show(this,MSGBOX_ERROR,
                          _("Kernel Config"),
                          "\""+_("Config header output folder")+"\""+_(" is unspecified."));
        return -1;
    }

    if(this->Boot_Header_Output->GetValue()=="")
    {
        Main::Msgbox_Show(this,MSGBOX_ERROR,
                          _("Kernel Config"),
                          "\""+_("Boot header output folder")+"\""+_(" is unspecified."));
        return -1;
    }

    if(this->Boot_Source_Output->GetValue()=="")
    {
        Main::Msgbox_Show(this,MSGBOX_ERROR,
                          _("Kernel Config"),
                          "\""+_("Boot source output folder")+"\""+_(" is unspecified."));
        return -1;
    }

    if(this->Hook_Source_Output->GetValue()=="")
    {
        Main::Msgbox_Show(this,MSGBOX_ERROR,
                          _("Kernel Config"),
                          "\""+_("Hook source output folder")+"\""+_(" is unspecified."));
        return -1;
    }

    if(this->Handler_Source_Output->GetValue()=="")
    {
        Main::Msgbox_Show(this,MSGBOX_ERROR,
                          _("Kernel Config"),
                          "\""+_("Vector handler source output folder")+"\""+_(" is unspecified."));
        return -1;
    }

    return 0;
}
/* End Function:Kernel_Panel::Check ******************************************/

/* Function:Kernel_Panel::Save ************************************************
Description : Save information to project information.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
void Kernel_Panel::Save(void)
{
    Main::Proj_Info->Kernel->Code_Base=std::stoull(std::string(this->Code_Base->GetValue()),0,0);
    Main::Proj_Info->Kernel->Code_Size=std::stoull(std::string(this->Code_Size->GetValue()),0,0);
    Main::Proj_Info->Kernel->Data_Base=std::stoull(std::string(this->Data_Base->GetValue()),0,0);
    Main::Proj_Info->Kernel->Data_Size=std::stoull(std::string(this->Data_Size->GetValue()),0,0);
    Main::Proj_Info->Kernel->Stack_Size=std::stoull(std::string(this->Stack_Size->GetValue()),0,0);
    Main::Proj_Info->Kernel->Kern_Prio=std::stoull(std::string(this->Kern_Prio->GetStringSelection()),0,0);

    switch(this->Kom_Order->GetSelection())
    {
        case 0:Main::Proj_Info->Kernel->Kom_Order=3;break;
        case 1:Main::Proj_Info->Kernel->Kom_Order=4;break;
        case 2:Main::Proj_Info->Kernel->Kom_Order=5;break;
        case 3:Main::Proj_Info->Kernel->Kom_Order=6;break;
        default:ASSERT(0,"Kernel memory granularity order is impossible.");
    }
    Main::Proj_Info->Kernel->Extra_Kom=std::stoull(std::string(this->Extra_Kom->GetValue()),0,0);

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

/* Function:Kernel_Panel::Toolchain_Set ***************************************
Description : Set the collection of possible tool chains.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
void Kernel_Panel::Toolchain_Set(void)
{
    std::vector<std::string> Tool_Avail;

    wxLogDebug("Kernel_Panel::Toolchain_Set");

    /* Ask for possible toolchains */
    Main::Plat_Info->Toolchain_Native(Tool_Avail);
    Main::Choice_Refill(this->Toolchain,Tool_Avail);
}
/* End Function:Kernel_Panel::Toolchain_Set **********************************/

/* Function:Kernel_Panel::Buildsystem_Set *************************************
Description : Set build system given the toolchain choice.
Input       : const class wxString& Toolchain - The chosen toolchain.
Output      : None.
Return      : None.
******************************************************************************/
void Kernel_Panel::Buildsystem_Set(const class wxString& Toolchain)
{
    std::vector<std::string> Build_Avail;

    wxLogDebug("Kernel_Panel::Buildsystem_Set");

    /* Ask for possible toolchains */
    Main::Plat_Info->Buildsystem_Native(std::string(Toolchain),Build_Avail);
    Main::Choice_Refill(this->Buildsystem,Build_Avail);
}
/* End Function:Kernel_Panel::Buildsystem_Set ********************************/

/* Function:Kernel_Panel::Kernel_Prio_Set *************************************
Description : Set kernel priority according to word length.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
void Kernel_Panel::Kernel_Prio_Set(void)
{
    cnt_t Count;

    this->Kern_Prio->Clear();

    /* We allow priorities from 1-4 wordlength within the GUI */
    for(Count=1;Count<=4;Count++)
        this->Kern_Prio->Append(std::to_string(Main::Plat_Info->Wordlength*Count));

    this->Kern_Prio->SetSelection(0);
}
/* End Function:Kernel_Panel::Kernel_Prio_Set ********************************/

/* Function:Kernel_Panel::On_Text_Hex *****************************************
Description : wxEVT_KILL_FOCUS handler for all text input boxes that should
              normally contain a hex integer.
Input       : class wxFocusEvent& Event - The event.
Output      : None.
Return      : None.
******************************************************************************/
void Kernel_Panel::On_Text_Hex(class wxFocusEvent& Event)
{
    class wxTextCtrl* Text;

    Text=dynamic_cast<class wxTextCtrl*>(Event.GetEventObject());
    if(Text!=nullptr)
        Text->SetValue(Main::Num2Hex(std::string(Text->GetValue())));

    Event.Skip();
}
/* End Function:Kernel_Panel::On_Text_Hex ************************************/

/* Function:Kernel_Panel::On_Toolchain ****************************************
Description : wxEVT_CHOICE handler for 'Toolchain'.
Input       : class wxCommandEvent& Event - The event.
Output      : None.
Return      : None.
******************************************************************************/
void Kernel_Panel::On_Toolchain(class wxCommandEvent& Event)
{
    class wxString Toolchain;
    class wxString Buildsystem;

    Toolchain=this->Toolchain->GetStringSelection();
    Buildsystem=this->Buildsystem->GetStringSelection();

    /* Recreate options if the current one is invalid */
    if(Main::Plat_Info->Compat_Check(std::string(Toolchain), std::string(Buildsystem))!=0)
        this->Buildsystem_Set(Toolchain);
}
/* End Function:Kernel_Panel::On_Toolchain ***********************************/
}
/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
