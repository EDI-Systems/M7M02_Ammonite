/******************************************************************************
Filename    : modal dialog.cpp
Author      : lbc
Date        : 10/05/2025
License     : Proprietary; confidential.
Description : Name or rename dialog class implementation.
******************************************************************************/

/* Include *******************************************************************/
#define __HDR_DEF__
#include "rvm_cfg.hpp"
#undef __HDR_DEF__

#include "wx/wx.h"
#include "wx/xml/xml.h"
#include <wx/stdpaths.h>
#include <wx/filename.h>
#include "wx/notebook.h"

#include "map"
#include "string"
#include "memory"

#define __HDR_DEF__
#include "Name_Dialog/name_dialog.hpp"
#undef __HDR_DEF__

#define __HDR_CLASS__
#include "rvm_cfg.hpp"
#include "Name_Dialog/name_dialog.hpp"
#undef __HDR_CLASS__
/* End Include ***************************************************************/
namespace RVM_CFG
{
/* Function:Modal_Dialog::Choose_Dialog ***************************************
Description : Default constructor for name dialog.
Input       : class wxWindow* Parent - parent window.
Output      : None.
Return      : None.
******************************************************************************/
/* void */ Name_Dialog::Name_Dialog(class wxWindow* Parent):
wxDialog(Parent,wxID_ANY,_("Rename"),wxDefaultPosition,I2P(wxSize(350,120)))
{
    this->Center();

    try
    {

        /* close event */
        this->Bind(wxEVT_CLOSE_WINDOW, &Name_Dialog::On_Close, this);

        /* GUI */
        this->SetMinSize(I2P(wxSize(512,600)));

        this->Main_Sizer=new class wxBoxSizer(wxVERTICAL);
        this->Name_Sizer=new class wxBoxSizer(wxHORIZONTAL);
        this->Button_Sizer=new class wxBoxSizer(wxHORIZONTAL);

        this->Name_Label=new wxStaticText(this,wxID_ANY,_("New Name:"));
        this->Name=new wxTextCtrl(this,wxID_ANY);
        this->Name_Sizer->Add(Name_Label,0,wxEXPAND|wxALL,I2P(5));
        this->Name_Sizer->Add(Name,1,wxEXPAND|wxALL,I2P(5));

        this->Confirm=new class wxButton(this,wxID_ANY,_("Confirm"));
        this->Cancel=new class wxButton(this,wxID_ANY,_("Cancel"));
        this->Bind(wxEVT_BUTTON,&Name_Dialog::On_Confirm,this,this->Confirm->GetId());
        this->Bind(wxEVT_BUTTON,&Name_Dialog::On_Cancel,this,this->Cancel->GetId());
        this->Button_Sizer->AddStretchSpacer(1);
        this->Button_Sizer->Add(this->Confirm,0,wxALL,I2P(5));
        this->Button_Sizer->AddStretchSpacer(1);
        this->Button_Sizer->Add(this->Cancel,0,wxALL,I2P(5));
        this->Button_Sizer->AddStretchSpacer(1);

        this->Main_Sizer->Add(this->Name_Sizer,0,wxEXPAND|wxALL,I2P(5));
        this->Main_Sizer->AddStretchSpacer(1);
        this->Main_Sizer->Add(this->Button_Sizer,0,wxEXPAND|wxALL,I2P(5));
        this->SetSizer(this->Main_Sizer);
        this->Layout();
    }
    catch(std::exception& Exc)
    {
        ASSERT(0,Exc.what());
    }
}
/* End Function:Name_Dialog::Name_Dialog ***************************************/

/* Function:Name_Dialog::~Name_Dialog *******************************************
Description : Destructor for name dialog.
Input       : None.
Output      : None.
Return      : None.
********************************************************************************/
/* void */ Name_Dialog::~Name_Dialog(void)
{

}
/* End Function:Name_Dialog::~Name_Dialog **************************************/

/* Function:Name_Dialog::On_Close ***********************************************
Description : Hide this dialog after 'cancel' button is clicked.
Input       : class wxCloseEvent& Event - The event..
Output      : None.
Return      : None.
********************************************************************************/
void Name_Dialog::On_Close(class wxCloseEvent& Event)
{
    this->EndModal(wxID_CANCEL);
    this->Hide();
}
/* End Function:Name_Dialog::On_Close ******************************************/

/* Function:Name_Dialog::On_Confirm *********************************************
Description : Check the name, if check pass, Show_Dialog will get wxID_OK.
Input       : class wxCommandEvent& Event - The event.
Output      : None.
Return      : None.
********************************************************************************/
void Name_Dialog::On_Confirm(class wxCommandEvent& Event)
{
    if(Main::Idtfr_Check(this->Name->GetValue().Upper().ToStdString()))
    {
        Main::Msgbox_Show(this,MSGBOX_ERROR,
                          _("Rename"),
                          _("It is not a valid C identifier"));
        return;
    }
    this->EndModal(wxID_OK);
    this->Hide();
}
/* End Function:Name_Dialog::On_Confirm ****************************************/

/* Function:Name_Dialog::On_Cancel **********************************************
Description : Hide this dialog, and Show_Dialog will get wxID_CANCEL.
Input       : class wxCommandEvent& Event - The event.
Output      : None.
Return      : None.
********************************************************************************/
void Name_Dialog::On_Cancel(class wxCommandEvent& Event)
{
    this->EndModal(wxID_CANCEL);
    this->Hide();
}
/* End Function:Name_Dialog::On_Cancel *****************************************/

/* Function:Name_Dialog::Load ***************************************************
Description : Load name in this dialog.
Input       : None.
Output      : None.
Return      : None.
********************************************************************************/
void Name_Dialog::Load(const std::string& Input)
{
    this->Name->SetValue(Input);
    this->Name->SetFocus();
    this->Name->SetSelection(0, this->Name->GetLastPosition());
}
/* End Function:Name_Dialog::Load **********************************************/

/* Function:Name_Dialog::Name_Get ***********************************************
Description : Get the name set by the user
Input       : None.
Output      : None.
Return      : std::string - The name set by the user.
********************************************************************************/
std::string Name_Dialog::Name_Get()
{
    return this->Name->GetValue().ToStdString();
}
/* End Function:Name_Dialog::Name_Get *****************************************/

}
/* End Of File ****************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ************************/

