/******************************************************************************
Filename    : sheet_notebook.cpp
Author      : pry
Date        : 16/08/2021
License     : Proprietary; confidential.
Description : Sheet notebook implementation.
******************************************************************************/

/* Includes ******************************************************************/
#define __HDR_DEFS__
#include "rvm_cfg.hpp"
#undef __HDR_DEFS__

#include "wx/wx.h"
#include "wx/xml/xml.h"
#include "wx/aui/auibook.h"

#include "set"
#include "map"
#include "memory"

#define __HDR_DEFS__
#include "Option_Notebook/option_notebook.hpp"
#undef __HDR_DEFS__

#define __HDR_CLASSES__
#include "rvm_cfg.hpp"
#include "Option_Notebook/option_notebook.hpp"
#undef __HDR_CLASSES__
/* End Includes **************************************************************/
namespace RVM_CFG
{
/* Begin Function:Option_Notebook::Option_Notebook ****************************
Description : Constructor for sheet notebook. This notebook is very flexible and
              allows close buttons.
Input       : class wxWindow* Parent - The parent panel.
Output      : None.
Return      : None.
******************************************************************************/
/* void */ Option_Notebook::Option_Notebook(class wxWindow* Parent):
wxAuiNotebook(Parent,wxID_ANY,wxDefaultPosition,wxDefaultSize,
              wxAUI_NB_TOP|wxAUI_NB_CLOSE_ON_ACTIVE_TAB|wxAUI_NB_WINDOWLIST_BUTTON|wxAUI_NB_SCROLL_BUTTONS|wxAUI_NB_TAB_MOVE)
{

    this->Bind(wxEVT_AUINOTEBOOK_PAGE_CHANGING,&Option_Notebook::On_Sheet_Prechange,this,this->GetId());
    this->Bind(wxEVT_AUINOTEBOOK_PAGE_CLOSE,&Option_Notebook::On_Sheet_Prechange,this,this->GetId());
    /* These will fire up only when there still are pages left */
    this->Bind(wxEVT_AUINOTEBOOK_PAGE_CHANGED,&Option_Notebook::On_Sheet_Change,this,this->GetId());
    this->Bind(wxEVT_AUINOTEBOOK_PAGE_CLOSED,&Option_Notebook::On_Sheet_Change,this,this->GetId());
}
/* End Function:Option_Notebook::Option_Notebook *****************************/

/* Begin Function:Option_Notebook::~Option_Notebook ***************************
Description : Destructor for sheet notebook.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
/* void */ Option_Notebook::~Option_Notebook(void)
{
    this->Unbind(wxEVT_AUINOTEBOOK_PAGE_CHANGING,&Option_Notebook::On_Sheet_Prechange,this);
    this->Unbind(wxEVT_AUINOTEBOOK_PAGE_CLOSE,&Option_Notebook::On_Sheet_Prechange,this);
    this->Unbind(wxEVT_AUINOTEBOOK_PAGE_CHANGED,&Option_Notebook::On_Sheet_Change,this);
    this->Unbind(wxEVT_AUINOTEBOOK_PAGE_CLOSED,&Option_Notebook::On_Sheet_Change,this);
}
/* End Function:Option_Notebook::Option_Notebook *****************************/

/* Begin Function:Option_Notebook::State_Set **********************************
Description : Set the current UI state, and decide what controls are usable.
Input       : ptr_t Type - The state type.
Output      : None.
Return      : None.
******************************************************************************/
void Option_Notebook::State_Set(ptr_t Type)
{
    wxLogDebug("Option_Notebook::State_Set - %llu", Type);
}
/* End Function:Option_Notebook::State_Set ***********************************/

/* Begin Function:Option_Notebook::On_Sheet_Prechange *************************
Description : wxEVT_AUINOTEBOOK_PAGE_CHANGING/wxEVT_AUINOTEBOOK_PAGE_CLOSE handler
              for tab switch events.
Input       : class wxAuiNotebookEvent& Event - The event.
Output      : None.
Return      : None.
******************************************************************************/
void Option_Notebook::On_Sheet_Prechange(class wxAuiNotebookEvent& Event)
{
    wxLogDebug("Option_Notebook::On_Sheet_Prechange");
}
/* End Function:Option_Notebook::On_Sheet_Prechange **************************/

/* Begin Function:Option_Notebook::On_Sheet_Change ****************************
Description : wxEVT_AUINOTEBOOK_PAGE_CHANGED/wxEVT_AUINOTEBOOK_PAGE_CLOSED handler
              for tab switch events.
              This is weird in that: 1. When delete is called, nothing is fired.
                                     2. When we add a page to it, the event gives null.
Input       : class wxAuiNotebookEvent& Event - The event.
Output      : None.
Return      : None.
******************************************************************************/
void Option_Notebook::On_Sheet_Change(class wxAuiNotebookEvent& Event)
{
    wxLogDebug("Option_Notebook::On_Sheet_Change");
}
/* End Function:Option_Notebook::On_Sheet_Change *****************************/
}
/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
