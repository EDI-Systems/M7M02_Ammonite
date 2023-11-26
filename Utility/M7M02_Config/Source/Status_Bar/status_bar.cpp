/******************************************************************************
Filename    : status_bar.cpp
Author      : zjx
Date        : 06/04/2021
Licence     : Proprietary; confidential.
Description : The status bar underlying the whole window. Responsible for displaying
              all types of auxiliary information.
******************************************************************************/

/* Include *******************************************************************/
extern "C"
{
#include "stdio.h"
#include "ctype.h"
}

#define __HDR_DEF__
#include "rvm_cfg.hpp"
#undef __HDR_DEF__

#include "wx/wx.h"
#include "wx/xml/xml.h"

#include "map"
#include "memory"

extern "C"
{
#ifdef _WIN32
#include "windows.h"
#include "psapi.h"
#endif
}

#define __HDR_DEF__
#include "Status_Bar/status_bar.hpp"
#undef __HDR_DEF__

#define __HDR_CLASS__
#include "rvm_cfg.hpp"
#include "Status_Bar/status_bar.hpp"
#undef __HDR_CLASS__
/* End Include ***************************************************************/
namespace RVM_CFG
{
/* Function:Status_Bar::Status_Bar ********************************************
Description : Status bar class constructor.
Input       : class wxWindow *Parent - The parent window.
Output      : None.
Return      : None.
******************************************************************************/
/* void */ Status_Bar::Status_Bar(class wxWindow *Parent):
wxStatusBar(Parent,wxID_ANY)
{
    static const s32_t Widths[3]=
    {
            STATUS_FIRST_WIDTH,
            I2P(STATUS_SECOND_WIDTH),
            I2P(STATUS_SECOND_WIDTH)
    };

    try
    {
        this->Parent=Parent;
        this->SetWindowStyle(wxSTB_DEFAULT_STYLE);
        this->SetFieldsCount(STATUS_BAR_SIZE);
        this->SetStatusWidths(STATUS_BAR_SIZE, Widths);

        this->Version=new wxStaticText(this,wxID_ANY,SOFTWARE_VERSION,wxDefaultPosition,wxDefaultSize);
        this->Memory=new wxStaticText(this,wxID_ANY,wxT("kB: 0"),wxDefaultPosition,wxDefaultSize);

        /* Timer for updating memory info every 1s */
        this->Timer=new class wxTimer(this);
#ifndef DEBUG
        this->Timer->Start(1000);
#endif
        /* Our own events */
        this->Bind(wxEVT_SIZE,&Status_Bar::On_Size,this,this->GetId());
        this->Bind(wxEVT_TIMER,&Status_Bar::On_Timer,this,this->Timer->GetId());
    }
    catch(std::exception& Exc)
    {
        ASSERT(0,Exc.what());
    }
}
/* End Function:Status_Bar::Status_Bar ***************************************/

/* Function:Status_Bar::~Status_Bar *******************************************
Description : Status bar class destructor.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
/* void */ Status_Bar::~Status_Bar(void)
{
#ifndef DEBUG
    this->Timer->Stop();
#endif
    this->Unbind(wxEVT_SIZE,&Status_Bar::On_Size,this);
    this->Unbind(wxEVT_TIMER,&Status_Bar::On_Timer,this);
    delete this->Timer;
}
/* End Function:Status_Bar::~Status_Bar **************************************/

/* Function:Status_Bar::State_Set *********************************************
Description : Set the current UI state, and decide what controls are usable.
Input       : ptr_t Type - The state type.
Output      : None.
Return      : None.
******************************************************************************/
void Status_Bar::State_Set(ptr_t Type)
{
    /* Always enable all */
    this->Refresh();
}
/* End Function:Status_Bar::State_Set ****************************************/

/* Function:Status_Bar::On_Size ***********************************************
Description : wxEVT_SIZE handler.
Input       : class wxSizeEvent& Event - The event.
Output      : None.
Return      : None.
******************************************************************************/
void Status_Bar::On_Size(class wxSizeEvent& Event)
{
    pos_t Frame_Width;

    Frame_Width=this->Parent->GetSize().GetWidth();

    this->Version->SetPosition(wxPoint(0,I2P(4)));
    this->Memory->SetPosition(wxPoint(Frame_Width-I2P(STATUS_SECOND_WIDTH)-I2P(30),I2P(4)));

    Event.Skip();
}
/* End Function:Status_Bar::On_Size ******************************************/

/* Function:Status_Bar::On_Timer **********************************************
Description : wxEVT_TIMER handler. This will get the memory usage.
Input       : class wxTimerEvent& Event - The event.
Output      : None.
Return      : None.
******************************************************************************/
void Status_Bar::On_Timer(class wxTimerEvent& Event)
{
    u32_t Kb;
    char Buf[64];

#ifdef _WIN32
    PROCESS_MEMORY_COUNTERS Mem;

    GetProcessMemoryInfo(GetCurrentProcess(),&Mem,sizeof(Mem));
    Kb=Mem.PagefileUsage/1024;
#else

#endif

    sprintf(Buf,"kB: %u ", Kb);
    this->Memory->SetLabel(Buf);

    wxLogDebug("Status_Bar::On_Timer");
    Event.Skip();
}
/* End Function:Status_Bar::On_Timer *****************************************/
}
/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
