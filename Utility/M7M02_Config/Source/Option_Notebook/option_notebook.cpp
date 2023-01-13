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
#include "wx/notebook.h"
#include "wx/xml/xml.h"

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
wxNotebook(Parent,wxID_ANY,wxDefaultPosition,wxDefaultSize,wxNB_TOP)
{

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
}
/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
