/******************************************************************************
Filename    : virtual_notebook.cpp
Author      : lbc
Date        : 25/04/2025
License     : Proprietary; confidential.
Description : Virtual notebook implementation.
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
#include "Option_Panel/Virtual_Notebook/virtual_notebook.hpp"
#undef __HDR_DEF__

#define __HDR_CLASS__
#include "rvm_cfg.hpp"

#include "Proj_Info/proj_info.hpp"
#include "Proj_Info/Process/process.hpp"
#include "Proj_Info/Process/Virtual/virtual.hpp"

#include "Option_Panel/Virtual_Notebook/virtual_notebook.hpp"
#include "Option_Panel/Virtual_Notebook/Virtual_Basic_Panel/virtual_basic_panel.hpp"
#include "Option_Panel/Memory_Panel/memory_panel.hpp"
#include "Option_Panel/Shmem_Panel/shmem_panel.hpp"
#include "Option_Panel/Send_Panel/send_panel.hpp"
#include "Option_Panel/Kfunc_Panel/kfunc_panel.hpp"
#undef __HDR_CLASS__
/* End Include ***************************************************************/
namespace RVM_CFG
{
/* Function:Virtual_Notebook::Virtual_Notebook ********************************
Description : Constructor for virtual machine notebook. This notebook include
              basic information, memory, shmem, send, kfunc.
Input       : class wxWindow* Parent - The parent window.
Output      : None.
Return      : None.
******************************************************************************/
/* void */ Virtual_Notebook::Virtual_Notebook(class wxWindow* Parent):
wxNotebook(Parent,wxID_ANY,wxDefaultPosition,wxDefaultSize,wxNB_TOP)
{
    try
    {
        this->Basic=new class Virtual_Basic_Panel(this);
        this->Memory=new class Memory_Panel(this,"Virtual machine - Memory",BLANK_NAME_PERMIT);
        this->Shmem=new class Shmem_Panel(this,"Virtual machine - Shared memory");
        this->Send=new class Send_Panel(this,"Virtual machine - Send");
        this->Kfunc=new class Kfunc_Panel(this,"Virtual machine - Kfunc");

        this->AddPage(this->Basic,_("Basic"));
        this->AddPage(this->Memory,_("Memory"));
        this->AddPage(this->Shmem,_("Shmem"));
        this->AddPage(this->Send,_("Send"));
        this->AddPage(this->Kfunc,_("Kfunc"));

        this->Bind(wxEVT_NOTEBOOK_PAGE_CHANGING, &Virtual_Notebook::On_Config, this);
    }
    catch(std::exception& Exc)
    {
        ASSERT(0,Exc.what());
    }
}
/* End Function:Virtual_Notebook::Virtual_Notebook ***************************/

/* Function:Virtual_Notebook::~Virtual_Notebook *******************************
Description : Destructor for kernel notebook.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
/* void */ Virtual_Notebook::~Virtual_Notebook(void)
{

}
/* End Function:Virtual_Notebook::Virtual_Notebook ***************************/


/* Function:Virtual_Notebook::Check *******************************************
Description : Check the page. If there are any issues, make sure that the user
              can't get away with this.
Input       : None.
Output      : None.
Return      : ret_t - The return value indicating whether errors are found.
                      If there are no errors, this should return 0.
******************************************************************************/
ret_t Virtual_Notebook::Check(void)
{
    wxLogDebug("Virtual_Notebook::Check");

    if(this->Basic->Check())return -1;
    if(this->Memory->Check())return -1;
    if(this->Shmem->Check())return -1;
    if(this->Send->Check())return -1;
    if(this->Kfunc->Check())return -1;
    return 0;
}
/* End Function:Virtual_Notebook::Check **************************************/

/* Function:Virtual_Notebook::Load ********************************************
Description : Load the information of virtual machine from data model to GUI.
Input       : const std::string& Virtual_Name - The name of virtual machine.
Output      : None.
Return      : None.
******************************************************************************/
void Virtual_Notebook::Load(const std::string& Virtual_Name)
{
    std::map<std::string, class Virtual*>::iterator Iter;

    /* Find */
    Iter=Main::Proj_Info->Virtual_Map.find(Virtual_Name);
    if(Iter==Main::Proj_Info->Virtual_Map.end())
    {
        wxLogDebug("Virtual_Notebook::Load: Cannot find %s",Virtual_Name);
        return;
    }
    wxLogDebug("Virtual_Notebook::Load: %s has been found",Virtual_Name);

    this->Basic->Load(Iter->second);
    this->Memory->Load(Iter->second->Memory);
    this->Shmem->Load(Iter->second->Shmem);
    this->Send->Load(Iter->second->Send);
    this->Kfunc->Load(Iter->second->Kfunc);
}
/* End Function:Virtual_Notebook::Load ***************************************/

/* Function:Virtual_Notebook::Save ********************************************
Description : Find the corresponding Native process and save the contents from
              the GUI to the information layer.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
void Virtual_Notebook::Save()
{
    std::string Virtual_Name;
    std::map<std::string, Virtual*>::iterator Iter;

    /* Find the corresponding information */
    Virtual_Name=this->Basic->Name->GetValue();
    Iter=Main::Proj_Info->Virtual_Map.find(Virtual_Name);
    if(Iter==Main::Proj_Info->Virtual_Map.end())
    {
        wxLogDebug("Virtual_Notebook::Save: Cannot find %s",Virtual_Name);
        return;
    }
    wxLogDebug("Virtual_Notebook::Save: %s has been saved",Virtual_Name);

    this->Basic->Save(Iter->second);
    this->Memory->Save(Iter->second->Memory);
    this->Shmem->Save(Iter->second->Shmem);
    this->Send->Save(Iter->second->Send);
    this->Kfunc->Save(Iter->second->Kfunc);
}
/* End Function:Virtual_Notebook::Save ***************************************/

/* Function:Virtual_Notebook::Check *******************************************
Description : Check whether the current panel contains any errors.
Input       : ptr_t Page - The page that needs to be checked.
Output      : None.
Return      : ret_t - If 0, no error exist; else -1
******************************************************************************/
ret_t Virtual_Notebook::Check(ptr_t Page)
{
    wxLogDebug("Virtual_Notebook::Check");

    /* What page is active? Check that page */
    switch(Page)
    {
        case 0:return this->Basic->Check();
        case 1:return this->Memory->Check();
        case 2:return this->Shmem->Check();
        case 3:return this->Send->Check();
        case 4:return this->Kfunc->Check();
        default:ASSERT(0,"No such page in this dialog.");
    }

    return -1;
}
/* End Function:Virtual_Notebook::Check **************************************/

/* Function:Virtual_Notebook::On_Config ***************************************
Description : wxEVT_NOTEBOOK_PAGE_CHANGING handler for tab pre-switch events.
              If we find any issues on that tab, we'll prevent the user from
              switching it unless it is fixed.
Input       : class wxBookCtrlEvent& Event - The event.
Output      : None.
Return      : None.
******************************************************************************/
void Virtual_Notebook::On_Config(class wxBookCtrlEvent& Event)
{
    if(this->GetSelection()==wxNOT_FOUND)
        return;
    else if(this->Check(this->GetSelection())!=0)
        Event.Veto();
}
/* End Function:Virtual_Notebook::On_Config ***************************************/

}
/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
