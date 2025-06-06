/******************************************************************************
Filename    : memory_notebook.cpp
Author      : lbc
Date        : 16/05/2025
License     : Proprietary; confidential.
Description : Memory notebook implementation.
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
#include "Option_Panel/Memory_Notebook/memory_notebook.hpp"
#include "Option_Panel/Memory_Panel/memory_panel.hpp"
#undef __HDR_DEF__

#define __HDR_CLASS__
#include "rvm_cfg.hpp"
#include "Proj_Info/proj_info.hpp"
#include "Chip_Info/chip_info.hpp"
#include "Option_Panel/Memory_Panel/memory_panel.hpp"
#include "Option_Panel/Memory_Notebook/memory_notebook.hpp"
#include "Option_Panel/Memory_Notebook/Extmem_Panel/extmem_panel.hpp"
#include "Option_Panel/Memory_Notebook/Ocmem_Panel/ocmem_panel.hpp"
#undef __HDR_CLASS__
/* End Include ***************************************************************/
namespace RVM_CFG
{
/* Function:Memory_Notebook::Memory_Notebook **********************************
Description : Constructor for memory notebook. This notebook include extra memory,
              shared memory and chip memory
Input       : class wxWindow* Parent - The parent window.
Output      : None.
Return      : None.
******************************************************************************/
/* void */ Memory_Notebook::Memory_Notebook(class wxWindow* Parent):
wxNotebook(Parent,wxID_ANY,wxDefaultPosition,wxDefaultSize,wxNB_TOP)
{
    try
    {
        this->SetBackgroundColour(Parent->GetBackgroundColour());

        this->Extmem_Panel=new class Extmem_Panel(this);
        this->Shmem_Panel=new class Memory_Panel(this,MEM_PANEL_SHARED);
        this->Ocmem_Panel=new class Ocmem_Panel(this);

        this->AddPage(this->Shmem_Panel,_("Shared Memory"));
        this->AddPage(this->Extmem_Panel,_("External Memory"));
        this->AddPage(this->Ocmem_Panel,_("On-chip Memory"));

        this->Bind(wxEVT_NOTEBOOK_PAGE_CHANGING, &Memory_Notebook::On_Config, this);
    }
    catch(std::exception& Exc)
    {
        ASSERT(0,Exc.what());
    }
}
/* End Function:Memory_Notebook::Memory_Notebook ****************************/

/* Function:Memory_Notebook::~Memory_Notebook ********************************
Description : Destructor for memory notebook.
Input       : None.
Output      : None.
Return      : None.
*****************************************************************************/
/* void */ Memory_Notebook::~Memory_Notebook(void)
{

}
/* End Function:Memory_Notebook::Memory_Notebook ****************************/

/* Function:Memory_Notebook::Load ********************************************
Description : Load information from Proj_Info into the this panel.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
void Memory_Notebook::Load(void)
{
    this->Ocmem_Panel->Load(Main::Chip_Info->Memory);
    this->Extmem_Panel->Load(Main::Proj_Info->Extmem);
    this->Shmem_Panel->Load(Main::Proj_Info->Shmem);
}
/* End Function:Memory_Notebook::Load ****************************************/

/* Function:Memory_Notebook::Save *********************************************
Description : Save information to Proj_Info.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
void Memory_Notebook::Save(void)
{
    this->Extmem_Panel->Save(Main::Proj_Info->Extmem);
    this->Shmem_Panel->Save(Main::Proj_Info->Shmem);
}
/* End Function:Memory_Notebook::Save ****************************************/

/* Function:Memory_Notebook::Check ********************************************
Description : Check whether the current panel contains any errors..
Input       : None.
Output      : None.
Return      : ret_t - if 0, no error exists; else -1.
******************************************************************************/
ret_t Memory_Notebook::Check()
{
    wxLogDebug("Memory_Notebook::Check");
    if(this->Extmem_Panel->Check())
        return -1;
    if(this->Shmem_Panel->Check())
        return -1;
    return 0;
}
/* End Function:Memory_Notebook::Check ***************************************/

/* Function:Memory_Notebook::Check ********************************************
Description : Check whether the current panel contains any errors..
Input       : ptr_t Page - The page needs to be checked.
Output      : None.
Return      : ret_t - if 0, no error exists; else -1.
******************************************************************************/
ret_t Memory_Notebook::Check(ptr_t Page)
{
    wxLogDebug("Memory_Notebook::Check");

    /* What page is active? Check that page */
    switch(Page)
    {
        case 0:return this->Shmem_Panel->Check();
        case 1:return this->Extmem_Panel->Check();
        case 2:return 0;
        default:ASSERT(0,"No such page in this dialog.");
    }

    return -1;
}
/* End Function:Memory_Notebook::Check ***************************************/

/* Function:Proj_Dialog::On_Config ********************************************
Description : wxEVT_NOTEBOOK_PAGE_CHANGING handler for tab pre-switch events.
              If we find any issues on that tab, we'll prevent the user from
              switching it unless it is fixed.
Input       : class wxBookCtrlEvent& Event - The event.
Output      : None.
Return      : None.
******************************************************************************/
void Memory_Notebook::On_Config(class wxBookCtrlEvent& Event)
{
    if(this->GetSelection()==wxNOT_FOUND)
        return;
    else if(this->Check(this->GetSelection())!=0)
        Event.Veto();
}
/* End Function:Proj_Dialog::On_Config ***************************************/

}
/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
