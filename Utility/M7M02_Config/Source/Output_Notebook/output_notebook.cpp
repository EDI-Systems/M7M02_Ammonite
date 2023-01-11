/******************************************************************************
Filename    : output_notebook.cpp
Author      : zjx
Date        : 16/04/2021
License     : Proprietary; confidential.
Description : Output notebook implementation.
******************************************************************************/

/* Includes ******************************************************************/
#define __HDR_DEFS__
#include "rvm_cfg.hpp"
#undef __HDR_DEFS__

#include "wx/wx.h"
#include "wx/notebook.h"

#define __HDR_DEFS__
#include "Output_Notebook/output_notebook.hpp"
#undef __HDR_DEFS__

#define __HDR_CLASSES__
#include "rvm_cfg.hpp"
#include "Output_Notebook/output_notebook.hpp"
#undef __HDR_CLASSES__
/* End Includes **************************************************************/
namespace RVM_CFG
{
/* Begin Function:Output_Notebook::Output_Notebook ****************************
Description : Constructor for output notebook.
Input       : class wxWindow* Parent - The parent panel.
Output      : None.
Return      : None.
******************************************************************************/
/* void */ Output_Notebook::Output_Notebook(class wxWindow* Parent):
wxNotebook(Parent,wxID_ANY,wxDefaultPosition,wxDefaultSize,wxNB_BOTTOM)
{
    try
    {
        this->Generate=new class wxTextCtrl(this,wxID_ANY,wxT(""),wxDefaultPosition,wxDefaultSize,wxTE_MULTILINE|wxHSCROLL|wxTE_READONLY);
        this->AddPage(this->Generate,_("Generate"),true);

        this->Process=new class wxTextCtrl(this,wxID_ANY,wxT(""),wxDefaultPosition,wxDefaultSize,wxTE_MULTILINE|wxHSCROLL|wxTE_READONLY);
        this->AddPage(this->Process,_("Process"),false);

        this->Statistics=new class wxTextCtrl(this,wxID_ANY,wxT(""),wxDefaultPosition,wxDefaultSize,wxTE_MULTILINE|wxHSCROLL|wxTE_READONLY);
        this->AddPage(this->Statistics,_("Statistics"),false);
    }
    catch(std::exception& Exc)
    {
        ASSERT(0,Exc.what());
    }
}
/* End Function:Output_Notebook::Output_Notebook *****************************/

/* Begin Function:Output_Notebook::~Output_Notebook ***************************
Description : Destructor for output notebook.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
/* void */ Output_Notebook::~Output_Notebook(void)
{

}
/* End Function:Output_Notebook::Output_Notebook *****************************/

/* Begin Function:Output_Notebook::State_Set **********************************
Description : Set the current UI state, and decide what controls are usable.
Input       : ptr_t Type - The state type.
Output      : None.
Return      : None.
******************************************************************************/
void Output_Notebook::State_Set(ptr_t Type)
{
    /* Always enabled */
}
/* End Function:Output_Notebook::State_Set ***********************************/
}
/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
