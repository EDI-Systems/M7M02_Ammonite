/******************************************************************************
Filename    : about_dialog.cpp
Author      : pry
Date        : 08/02/2022
License     : Proprietary; confidential.
Description : About information dialog implementation.
******************************************************************************/

/* Include *******************************************************************/
#define __HDR_DEF__
#include "rvm_cfg.hpp"
#undef __HDR_DEF__

#include "wx/wx.h"
#include "wx/xml/xml.h"

#include "map"
#include "set"
#include "string"
#include "memory"
#include "vector"
#include "algorithm"

#define __HDR_DEF__
#include "About_Dialog/about_dialog.hpp"
#undef __HDR_DEF__

#define __HDR_CLASS__
#include "rvm_cfg.hpp"
#include "About_Dialog/about_dialog.hpp"
#undef __HDR_CLASS__
/* End Include ***************************************************************/
namespace RVM_CFG
{
/* Function:About_Dialog::About_Dialog ****************************************
Description : Constructor for reference viewer dialog.
Input       : class wxWindow* Parent - The parent window.
Output      : None.
Return      : None.
******************************************************************************/
/* void */ About_Dialog::About_Dialog(class wxWindow* Parent):
wxDialog(Parent,wxID_ANY,_("About Program"),wxDefaultPosition,I2P(wxSize(512,600)),wxDEFAULT_DIALOG_STYLE)
{
    this->Center();

    try
    {
        this->SetMinSize(I2P(wxSize(512,600)));

        this->Border_Sizer=new class wxBoxSizer(wxVERTICAL);
        this->Main_Sizer=new class wxBoxSizer(wxVERTICAL);

        this->Content_Sizer=new class wxBoxSizer(wxVERTICAL);

        //this->Logo=new class wxStaticBitmap(this,wxID_ANY,wxBitmap(Company_Logo));
        //this->Content_Sizer->Add(this->Logo,1,wxEXPAND);
        this->Content_Sizer->AddStretchSpacer(1);
        this->Text=new class wxTextCtrl(this,wxID_ANY,
                                        SOFTWARE_NAME+"\n"+
                                        _("Version:")+" "+SOFTWARE_VERSION+"\n",
                                        wxDefaultPosition,wxDefaultSize,wxTE_MULTILINE|wxTE_READONLY|wxTE_NO_VSCROLL);
        this->Content_Sizer->Add(this->Text,100,wxEXPAND);
        this->Content_Sizer->AddStretchSpacer(1);

        this->Confirm=new class wxButton(this,wxID_ANY,_("Confirm"));
        this->Bind(wxEVT_BUTTON,&About_Dialog::On_Confirm,this,this->Confirm->GetId());

        this->Main_Sizer->Add(this->Content_Sizer,100,wxEXPAND);
        this->Main_Sizer->AddStretchSpacer(1);
        this->Main_Sizer->Add(this->Confirm,0,wxALIGN_CENTER);

        this->Border_Sizer->Add(this->Main_Sizer,1,wxEXPAND|wxALL,I2P(5));
        this->SetSizer(this->Border_Sizer);
        this->Border_Sizer->Layout();
    }
    catch(std::exception& Exc)
    {
        ASSERT(0,Exc.what());
    }
}
/* End Function:About_Dialog::About_Dialog ***********************************/

/* Function:About_Dialog::~About_Dialog ***************************************
Description : Destructor for truth table dialog.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
/* void */ About_Dialog::~About_Dialog(void)
{
    this->Unbind(wxEVT_BUTTON,&About_Dialog::On_Confirm,this);
}
/* End Function:About_Dialog::~About_Dialog **********************************/

/* Function:About_Dialog::On_Confirm ******************************************
Description : wxEVT_BUTTON handler for 'Confirm'.
Input       : class wxCommandEvent& Event - The event.
Output      : None.
Return      : None.
******************************************************************************/
void About_Dialog::On_Confirm(class wxCommandEvent& Event)
{
    wxLogDebug("About_Dialog::On_Confirm");
    this->EndModal(wxID_OK);
}
/* End Function:About_Dialog::On_Confirm *************************************/
}
/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
