/******************************************************************************
Filename    : status_bar.cpp
Author      : zjx
Date        : 06/04/2021
Licence     : Proprietary; confidential.
Description : The status bar underlying the whole window. Responsible for displaying
              all types of auxiliary information.
******************************************************************************/

/* Includes ******************************************************************/
extern "C"
{
#include "stdio.h"
#include "ctype.h"
}

#define __HDR_DEFS__
#include "sdm_dl.hpp"
#undef __HDR_DEFS__

#include "wx/wx.h"
#include "memory"

extern "C"
{
#ifdef _WIN32
#include "windows.h"
#include "psapi.h"
#endif
}

#define __HDR_DEFS__
#include "Status_Bar/status_bar.hpp"
#undef __HDR_DEFS__

#define __HDR_CLASSES__
#include "sdm_dl.hpp"
#include "Status_Bar/status_bar.hpp"
#undef __HDR_CLASSES__
/* End Includes **************************************************************/
namespace RVM_CFG
{
/* Begin Function:Status_Bar::Status_Bar **************************************
Description : Status bar class constructor.
Input       : class wxWindow *Parent - The parent window.
Output      : None.
Return      : None.
******************************************************************************/
/* void */ Status_Bar::Status_Bar(class wxWindow *Parent):
wxStatusBar(Parent,wxID_ANY)
{
    static const s32_t Widths[2]={STATUS_FIRST_WIDTH,I2P(STATUS_SECOND_WIDTH)};

    try
    {
        this->Parent=Parent;
        this->SetWindowStyle(wxSTB_DEFAULT_STYLE);
        this->SetFieldsCount(STATUS_BAR_SIZE);
        this->SetStatusWidths(STATUS_BAR_SIZE, Widths);

        this->Status_Gauge=new class wxGauge(this,wxID_ANY,GAUGE_MAX_COUNT,wxDefaultPosition,wxDefaultSize,wxGA_HORIZONTAL|wxGA_SMOOTH);
        this->Status_Gauge->SetSize(I2P(GAUGE_WIDTH), I2P(GAUGE_HEIGHT));
        this->Status_Gauge->Show(true);

        this->Version=new wxStaticText(this,wxID_ANY,SOFTWARE_VERSION,wxDefaultPosition,wxDefaultSize);

        this->Bind(wxEVT_SIZE,&Status_Bar::On_Size,this,this->GetId());
    }
    catch(std::exception& Exc)
    {
        ASSERT(0,Exc.what());
    }
}
/* End Function:Status_Bar::Status_Bar ***************************************/

/* Begin Function:Status_Bar::~Status_Bar *************************************
Description : Status bar class destructor.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
/* void */ Status_Bar::~Status_Bar(void)
{
    this->Unbind(wxEVT_SIZE,&Status_Bar::On_Size,this);
}
/* End Function:Status_Bar::~Status_Bar **************************************/

/* Begin Function:Status_Bar::On_Size *****************************************
Description : wxEVT_SIZE handler.
Input       : class wxSizeEvent& Event - The event.
Output      : None.
Return      : None.
******************************************************************************/
void Status_Bar::On_Size(class wxSizeEvent& Event)
{
    pos_t Frame_Width;
    pos_t Bar_Height;

    Frame_Width=this->Parent->GetSize().GetWidth();
    Bar_Height=this->GetSize().GetHeight();

    this->Status_Gauge->SetPosition(wxPoint(Frame_Width-I2P(STATUS_SECOND_WIDTH)-I2P(40),(Bar_Height-I2P(GAUGE_HEIGHT))/2+1));
    this->Version->SetPosition(wxPoint(0,I2P(4)));

    Event.Skip();
}
/* End Function:Status_Bar::On_Size ******************************************/
}
/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
