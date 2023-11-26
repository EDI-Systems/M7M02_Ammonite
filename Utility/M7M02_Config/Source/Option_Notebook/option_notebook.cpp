/******************************************************************************
Filename    : sheet_notebook.cpp
Author      : pry
Date        : 16/08/2021
License     : Proprietary; confidential.
Description : Sheet notebook implementation.
******************************************************************************/

/* Include *******************************************************************/
#define __HDR_DEF__
#include "rvm_cfg.hpp"
#undef __HDR_DEF__

#include "wx/wx.h"
#include "wx/notebook.h"
#include "wx/xml/xml.h"

#include "set"
#include "map"
#include "memory"

#define __HDR_DEF__
#include "Option_Notebook/option_notebook.hpp"
#undef __HDR_DEF__

#define __HDR_CLASS__
#include "rvm_cfg.hpp"
#include "Option_Notebook/option_notebook.hpp"
#undef __HDR_CLASS__
/* End Include ***************************************************************/
namespace RVM_CFG
{
/* Function:Option_Notebook::Option_Notebook **********************************
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

/* Function:Option_Notebook::~Option_Notebook *********************************
Description : Destructor for sheet notebook.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
/* void */ Option_Notebook::~Option_Notebook(void)
{

}
/* End Function:Option_Notebook::Option_Notebook *****************************/

/* Function:Option_Notebook::State_Set ****************************************
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
