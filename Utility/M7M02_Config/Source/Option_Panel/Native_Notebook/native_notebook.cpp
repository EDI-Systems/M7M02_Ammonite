/******************************************************************************
Filename    : native_notebook.cpp
Author      : lbc
Date        : 25/04/2025
License     : Proprietary; confidential.
Description : Native process notebook implementation.
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
#include "Option_Panel/Native_Notebook/native_notebook.hpp"
#include "Option_Panel/Memory_Panel/memory_panel.hpp"
#undef __HDR_DEF__

#define __HDR_CLASS__
#include "rvm_cfg.hpp"

#include "Proj_Info/proj_info.hpp"
#include "Proj_Info/Process/process.hpp"
#include "Proj_Info/Process/Native/native.hpp"

#include "Option_Panel/Native_Notebook/native_notebook.hpp"
#include "Option_Panel/Native_Notebook/Native_Panel/native_panel.hpp"
#include "Option_Panel/Native_Notebook/Thread_Panel/thread_panel.hpp"
#include "Option_Panel/Native_Notebook/Invocation_Panel/invocation_panel.hpp"
#include "Option_Panel/Native_Notebook/Port_Panel/port_panel.hpp"
#include "Option_Panel/Native_Notebook/Receive_Panel/receive_panel.hpp"
#include "Option_Panel/Native_Notebook/Vector_Panel/vector_panel.hpp"
#include "Option_Panel/Memory_Panel/memory_panel.hpp"
#include "Option_Panel/Shmem_Panel/shmem_panel.hpp"
#include "Option_Panel/Send_Panel/send_panel.hpp"
#include "Option_Panel/Kfunc_Panel/kfunc_panel.hpp"

#undef __HDR_CLASS__
/* End Include ***************************************************************/
namespace RVM_CFG
{
/* Function:Native_Notebook::Native_Notebook **********************************
Description : Constructor for native process notebook. This notebook include
              basic information, memory, shmem, send, kfunc, thread(native only),
              invocation(native only), receive(native only), vector(native only).
Input       : class wxWindow* Parent - The parent window.
Output      : None.
Return      : None.
******************************************************************************/
/* void */ Native_Notebook::Native_Notebook(class wxWindow* Parent):
wxNotebook(Parent,wxID_ANY,wxDefaultPosition,wxDefaultSize,wxNB_TOP)
{
    try
    {
        this->SetBackgroundColour(Parent->GetBackgroundColour());

        this->Basic=new class Native_Panel(this);
        this->Memory=new class Memory_Panel(this,MEM_PANEL_PRIVATE);
        this->Shmem=new class Shmem_Panel(this);
        this->Thread=new class Thread_Panel(this);
        this->Invocation=new class Invocation_Panel(this);
        this->Port=new class Port_Panel(this);
        this->Receive=new class Receive_Panel(this);
        this->Send=new class Send_Panel(this);
        this->Vector=new class Vector_Panel(this);
        this->Kfunc=new class Kfunc_Panel(this);

        this->AddPage(this->Basic,_("Basic "));
        this->AddPage(this->Memory,_("Memory"));
        this->AddPage(this->Shmem,_("Shmem"));
        this->AddPage(this->Thread,_("Thread"));
        this->AddPage(this->Invocation,_("Invocation"));
        this->AddPage(this->Port,_("Port"));
        this->AddPage(this->Receive,_("Receive"));
        this->AddPage(this->Send,_("Send"));
        this->AddPage(this->Vector,_("Vector"));
        this->AddPage(this->Kfunc,_("Kfunc"));

        this->Bind(wxEVT_NOTEBOOK_PAGE_CHANGING, &Native_Notebook::On_Config, this);
    }
    catch(std::exception& Exc)
    {
        ASSERT(0,Exc.what());
    }
}
/* End Function:Native_Notebook::Native_Notebook *****************************/

/* Function:Native_Notebook::~Native_Notebook *********************************
Description : Destructor for kernel notebook.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
/* void */ Native_Notebook::~Native_Notebook(void)
{

}
/* End Function:Native_Notebook::~Native_Notebook ****************************/

/* Function:Native_Notebook::Load *********************************************
Description : Load
Input       : const std::string& Key.
Output      : None.
Return      : None.
******************************************************************************/
void Native_Notebook::Load(const std::string& Native_Name)
{
    std::map<std::string, Native*>::iterator Iter;

    Iter=Main::Proj_Info->Native_Map.find(Native_Name);
    if(Iter==Main::Proj_Info->Native_Map.end())
    {
        wxLogError("No such Native Process: %s",Native_Name);
        return;
    }

    wxLogDebug("Native_Notebook::Load: Find %s",Native_Name);
    this->Basic->Load(Iter->second);
    this->Memory->Load(Iter->second->Memory);
    this->Shmem->Load(Iter->second->Shmem);
    this->Thread->Load(Iter->second->Thread);
    this->Invocation->Load(Iter->second->Invocation);
    this->Port->Load(Iter->second->Port);
    this->Receive->Load(Iter->second->Receive);
    this->Send->Load(Iter->second->Send);
    this->Vector->Load(Iter->second->Vector);
    this->Kfunc->Load(Iter->second->Kfunc);
}
/* End Function:Native_Notebook::Load ****************************************/

/* Function:Native_Notebook::Check ********************************************
Description : Check the page. If there are any issues, make sure that the user
              can't get away with this.
Input       : None.
Output      : None.
Return      : ret_t - The return value indicating whether errors are found.
                      If there are no errors, this should return 0.
******************************************************************************/
ret_t Native_Notebook::Check()
{
    wxLogDebug("Native_Notebook::Check");

    /* What page is active? Check that page */
    if(this->Basic->Check())
        return -1;
    if(this->Memory->Check())
        return -1;
    if(this->Shmem->Check())
        return -1;
    if(this->Thread->Check())
        return -1;
    if(this->Invocation->Check())
        return -1;
    if(this->Port->Check())
        return -1;
    if(this->Receive->Check())
        return -1;
    if(this->Send->Check())
        return -1;
    if(this->Vector->Check())
        return -1;
    if(this->Kfunc->Check())
        return -1;

    return 0;
}
/* End Function:Native_Notebook::Check ***************************************/

/* Function:Native_Notebook::Save *********************************************
Description : Find the corresponding Native process and save the contents from
              the GUI to the information layer.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
void Native_Notebook::Save(void)
{
    std::string Native_Name;
    std::map<std::string, Native*>::iterator Iter;

    Native_Name=this->Basic->Name->GetValue();
    Iter=Main::Proj_Info->Native_Map.find(Native_Name);
    if(Iter==Main::Proj_Info->Native_Map.end())
    {
        wxLogError("Native_Notebook::Save: Cannot find %s",Native_Name);
        return;
    }
    wxLogDebug("Native_Notebook::Save: Find %s",Native_Name);
    this->Basic->Save(Iter->second);
    this->Memory->Save(Iter->second->Memory);
    this->Shmem->Save(Iter->second->Shmem);
    this->Thread->Save(Iter->second->Thread);
    this->Invocation->Save(Iter->second->Invocation);

    this->Port->Save(Iter->second->Port);
    this->Receive->Save(Iter->second->Receive);
    this->Send->Save(Iter->second->Send);
    this->Vector->Save(Iter->second->Vector);
    this->Kfunc->Save(Iter->second->Kfunc);
}
/* End Function:Native_Notebook::Save ****************************************/

/* Function:Native_Notebook::Check ********************************************
Description : Check whether the current panel contains any errors.
Input       : ptr_t Page - The page that needs to be checked.
Output      : None.
Return      : ret_t - If 0, no error exist; else -1
******************************************************************************/
ret_t Native_Notebook::Check(ptr_t Page)
{
    wxLogDebug("Native_Notebook::Check");

    /* What page is active? Check that page */
    switch(Page)
    {
        case 0:return this->Basic->Check();
        case 1:return this->Memory->Check();
        case 2:return this->Shmem->Check();
        case 3:return this->Thread->Check();
        case 4:return this->Invocation->Check();
        case 5:return this->Port->Check();
        case 6:return this->Receive->Check();
        case 7:return this->Send->Check();
        case 8:return this->Vector->Check();
        case 9:return this->Kfunc->Check();
        default:ASSERT(0,"No such page in this dialog.");
    }

    return -1;
}
/* End Function:Native_Notebook::Check ***************************************/

/* Function:Native_Notebook::On_Config ****************************************
Description : wxEVT_NOTEBOOK_PAGE_CHANGING handler for tab pre-switch events.
              If we find any issues on that tab, we'll prevent the user from
              switching it unless it is fixed.
Input       : class wxBookCtrlEvent& Event - The event.
Output      : None.
Return      : None.
******************************************************************************/
void Native_Notebook::On_Config(class wxBookCtrlEvent& Event)
{
    if(this->GetSelection()==wxNOT_FOUND)
        return;
    else if(this->Check(this->GetSelection())!=0)
        Event.Veto();
}
/* End Function:Native_Notebook::On_Config ***********************************/
}
/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
