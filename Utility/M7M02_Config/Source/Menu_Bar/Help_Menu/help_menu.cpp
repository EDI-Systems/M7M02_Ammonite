/******************************************************************************
Filename    : help_menu.cpp
Author      : zjx
Date        : 08/04/2021
License     : Proprietary; confidential.
Description : Help menu class implementation.
******************************************************************************/

/* Includes ******************************************************************/
#define __HDR_DEFS__
#include "rvm_cfg.hpp"
#undef __HDR_DEFS__

#include "wx/wx.h"
#ifdef WIN32
#include "wx/msw/registry.h"
#include "shlobj.h"
#include "sddl.h"
#include "winreg.h"
#endif

#define __HDR_DEFS__
#include "Menu_Bar/Help_Menu/help_menu.hpp"
#undef __HDR_DEFS__

#define __HDR_CLASSES__
#include "rvm_cfg.hpp"
#include "Menu_Bar/Help_Menu/help_menu.hpp"
#include "About_Dialog/about_dialog.hpp"
#undef __HDR_CLASSES__
/* End Includes **************************************************************/
namespace RVM_CFG
{
/* Begin Function:Help_Menu::Help_Menu ****************************************
Description : Constructor for help menu.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
/* void */ Help_Menu::Help_Menu(void)
{
    try
    {
        this->Kernel_Manual=new wxMenuItem(this,wxID_ANY,_("Kernel Manual(&K)"));
        this->Append(this->Kernel_Manual);
        this->Bind(wxEVT_MENU,&Help_Menu::On_Kernel_Manual,this,this->Kernel_Manual->GetId());

        this->Monitor_Manual=new wxMenuItem(this,wxID_ANY,_("Monitor Manual(&M)"));
        this->Append(this->Monitor_Manual);
        this->Bind(wxEVT_MENU,&Help_Menu::On_Monitor_Manual,this,this->Monitor_Manual->GetId());
#ifndef OEM_NEUTRAL
        this->AppendSeparator();

        this->About_Program=new wxMenuItem(this,wxID_ANY,_("About Program(&A)"));
        this->Append(this->About_Program);
        this->Bind(wxEVT_MENU,&Help_Menu::On_About_Program,this,this->About_Program->GetId());
#endif
        /* For exclusive debug use - trigger an error to test the rescue logic */
#ifdef DEBUG
        this->AppendSeparator();

        this->Error_Trigger=new wxMenuItem(this,wxID_ANY,_("Trigger Exception(&T)"));
        this->Append(this->Error_Trigger);
        this->Bind(wxEVT_MENU,&Help_Menu::On_Error_Trigger,this,this->Error_Trigger->GetId());
#endif
    }
    catch(std::exception& Exc)
    {
        ASSERT(0,Exc.what());
    }
}
/* End Function:Help_Menu::Help_Menu *****************************************/

/* Begin Function:Help_Menu::~Help_Menu ***************************************
Description : Destructor for help menu.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
/* void */ Help_Menu::~Help_Menu(void)
{
    this->Unbind(wxEVT_MENU,&Help_Menu::On_Kernel_Manual,this);
    this->Unbind(wxEVT_MENU,&Help_Menu::On_Monitor_Manual,this);
#ifndef OEM_NEUTRAL
    this->Unbind(wxEVT_MENU,&Help_Menu::On_About_Program,this);
#endif
#ifdef DEBUG
    this->Unbind(wxEVT_MENU,&Help_Menu::On_Error_Trigger,this);
#endif
}
/* End Function:Help_Menu::~Help_Menu ****************************************/

/* Begin Function:Help_Menu::State_Set ****************************************
Description : Set the current UI state, and decide what controls are usable.
Input       : ptr_t Type - The state type.
Output      : None.
Return      : None.
******************************************************************************/
void Help_Menu::State_Set(ptr_t Type)
{
    /* Always enable all */
}
/* End Function:Help_Menu::State_Set *****************************************/

/* Begin Function:Help_Menu::On_Kernel_Manual *********************************
Description : wxEVT_MENU handler for 'Kernel_Manual'.
Input       : class wxCommandEvent& Event - The event.
Output      : None.
Return      : None.
******************************************************************************/
void Help_Menu::On_Kernel_Manual(class wxCommandEvent& Event)
{
    wxLogDebug("Help_Menu::On_Kernel_Manual");
    Main::Manual_Open("Kernel");
}
/* End Function:Help_Menu::On_Hardware_Manual ********************************/

/* Begin Function:Help_Menu::On_Monitor_Manual ********************************
Description : wxEVT_MENU handler for 'Monitor Manual'.
Input       : class wxCommandEvent& Event - The event.
Output      : None.
Return      : None.
******************************************************************************/
void Help_Menu::On_Monitor_Manual(class wxCommandEvent& Event)
{
    wxLogDebug("Help_Menu::On_Monitor_Manual");
    Main::Manual_Open("Monitor");
}
/* End Function:Help_Menu::On_Monitor_Manual *********************************/

/* Begin Function:Help_Menu::On_About_Program *********************************
Description : wxEVT_MENU handler for 'About Program'.
Input       : class wxCommandEvent& Event - The event.
Output      : None.
Return      : None.
******************************************************************************/
void Help_Menu::On_About_Program(class wxCommandEvent& Event)
{
    wxLogDebug("Help_Menu::On_About_Program");
    Main::About_Dialog->ShowModal();
}
/* End Function:Help_Menu::On_About_Program **********************************/

/* Begin Function:Help_Menu::On_Error_Trigger *********************************
Description : wxEVT_MENU handler for 'Trigger Error'.
Input       : class wxCommandEvent& Event - The event.
Output      : None.
Return      : None.
******************************************************************************/
#ifdef DEBUG
void Help_Menu::On_Error_Trigger(class wxCommandEvent& Event)
{
    volatile ptr_t Test;

    wxLogDebug("Help_Menu::On_Error_Trigger - Exclusive Internal Debugging Use.");
    /* throw std::runtime_error("Error manually triggered."); */
    Test=0;
    Test=1234/Test;
    /* *((int*)(0))=1234; */
}
#endif
/* End Function:Help_Menu::On_Error_Trigger **********************************/
}
/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
