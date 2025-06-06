/******************************************************************************
Filename    : option_panel.cpp
Author      : lbc
Date        : 24/04/2025
Licence     : The Unlicense; see LICENSE for details.
Description : option panel.
******************************************************************************/

/* Include *******************************************************************/
#define __HDR_DEF__
#include "rvm_cfg.hpp"
#undef __HDR_DEF__

#include "wx/wx.h"
#include "wx/xml/xml.h"
#include "wx/filename.h"
#include "wx/notebook.h"

#include "map"
#include "string"
#include "memory"

#define __HDR_DEF__
#include "Option_Panel/option_panel.hpp"
#include "Config_Tree/config_tree.hpp"
#undef __HDR_DEF__

#define __HDR_CLASS__
#include "rvm_cfg.hpp"

#include "Option_Panel/Basic_Panel/basic_panel.hpp"
#include "Option_Panel/Memory_Panel/memory_panel.hpp"
#include "Option_Panel/Memory_Notebook/Extmem_Panel/extmem_panel.hpp"
#include "Option_Panel/Memory_Notebook/Ocmem_Panel/ocmem_panel.hpp"
#include "Option_Panel/Memory_Notebook/memory_notebook.hpp"
#include "Option_Panel/Kernel_Panel/kernel_panel.hpp"
#include "Option_Panel/Monitor_Panel/monitor_panel.hpp"

#include "Option_Panel/Native_Notebook/native_notebook.hpp"
#include "Option_Panel/Native_Notebook/Native_Panel/native_panel.hpp"
#include "Option_Panel/Virtual_Notebook/virtual_notebook.hpp"
#include "Option_Panel/Virtual_Notebook/Virtual_Panel/virtual_panel.hpp"

#include "Option_Panel/option_panel.hpp"
#undef __HDR_CLASS__
/* End Include ***************************************************************/
namespace RVM_CFG
{
/* Function:Option_Panel::Option_Panel ****************************************
Description : Constructor for option panel.
Input       : class wxWindow* Parent - The parent window.
Output      : None.
Return      : None.
******************************************************************************/
/* void */ Option_Panel::Option_Panel(class wxWindow* Parent):
wxPanel(Parent,wxID_ANY,wxDefaultPosition,wxDefaultSize)
{
    try
    {
        this->SetBackgroundColour(Parent->GetBackgroundColour());

        this->Window_Current=nullptr;

        this->Main_Sizer=new class wxBoxSizer(wxVERTICAL);

        /* Option panel */
        this->Basic_Panel=new class Basic_Panel(this);
        this->Main_Sizer->Add(this->Basic_Panel,1,wxEXPAND);
        this->Basic_Panel->Hide();

        this->Memory_Notebook=new class Memory_Notebook(this);
        this->Main_Sizer->Add(this->Memory_Notebook,1,wxEXPAND);
        this->Memory_Notebook->Hide();

        this->Kernel_Panel=new class Kernel_Panel(this);
        this->Main_Sizer->Add(this->Kernel_Panel,1,wxEXPAND);
        this->Kernel_Panel->Hide();

        this->Monitor_Panel=new class Monitor_Panel(this);
        this->Main_Sizer->Add(this->Monitor_Panel,1,wxEXPAND);
        this->Monitor_Panel->Hide();

        this->Native_Notebook=new class Native_Notebook(this);
        this->Main_Sizer->Add(this->Native_Notebook,1,wxEXPAND);
        this->Native_Notebook->Hide();

        this->Virtual_Notebook=new class Virtual_Notebook(this);
        this->Main_Sizer->Add(this->Virtual_Notebook,1,wxEXPAND);
        this->Virtual_Notebook->Hide();

        this->SetSizer(this->Main_Sizer);
        this->Layout();
    }
    catch(std::exception& Exc)
    {
        ASSERT(0,Exc.what());
    }
}
/* End Function:Option_Panel::Option_Panel ***********************************/

/* Function:Option_Panel::~Option_Panel ***************************************
Description : Destructor for option panel.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
/* void */ Option_Panel::~Option_Panel(void)
{

}
/* End Function:Option_Panel::~Option_Panel **********************************/

/* Function:Option_Panel::Load ************************************************
Description : Load up the option panel given the project data.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
void Option_Panel::Load(void)
{
    wxLogDebug("Option_Panel::Load");

    this->Basic_Panel->Config_Set();
    this->Basic_Panel->Coprocessor_Set();
    this->Basic_Panel->Buildsystem_Set();
    this->Kernel_Panel->Toolchain_Set();
    this->Kernel_Panel->Kernel_Prio_Set();
    this->Monitor_Panel->Toolchain_Set();
    this->Monitor_Panel->Virt_Prio_Set();
    this->Monitor_Panel->Virt_Event_Set();
    this->Native_Notebook->Basic->Coprocessor_Set();
    this->Native_Notebook->Basic->Toolchain_Set();
    this->Virtual_Notebook->Basic->Coprocessor_Set();
    this->Virtual_Notebook->Basic->Toolchain_Set();

    this->Basic_Panel->Load();
    this->Basic_Panel->Show();
    this->Window_Current=this->Basic_Panel;
    this->Layout();
    this->Show();
}
/* End Function:Option_Panel::Load *******************************************/

/* Function:Option_Panel::Current_Save ****************************************
Description : Check and try to save the current panel. Note: If a certain panel is
              currently being shown, the user may have modified its information,
              so it needs to be checked and saved. This function is necessary before
              Check_And_Save_All, especially when you finish editing a panel and leave
              it in a shown state before clicking Save or Save As.
Input       : None.
Output      : None.
Return      : ret_t - If 0, check pass; else -1.
********************************************************************************/
ret_t Option_Panel::Current_Save(void)
{
    /* Check and try to save the current panel */
    if(this->Basic_Panel->IsShown())
    {
        if(this->Basic_Panel->Check())
            return -1;
        this->Basic_Panel->Save();
    }
    else if(this->Memory_Notebook->IsShown())
    {
        if(this->Memory_Notebook->Check())
            return -1;
        this->Memory_Notebook->Save();
    }
    else if(this->Kernel_Panel->IsShown())
    {
        if(this->Kernel_Panel->Check())
            return -1;
        this->Kernel_Panel->Save();
    }
    else if(this->Monitor_Panel->IsShown())
    {
        if(this->Monitor_Panel->Check())
            return -1;
        this->Monitor_Panel->Save();
    }
    else if(this->Native_Notebook->IsShown())
    {
        if(this->Native_Notebook->Check())
            return -1;
        this->Native_Notebook->Save();
    }
    else if(this->Virtual_Notebook->IsShown())
    {
        if(this->Virtual_Notebook->Check())
            return -1;
        this->Virtual_Notebook->Save();
    }

    return 0;
}
/* End Function:Option_Panel::Current_Save ***********************************/

/* Function:Option_Panel::Window_Show *****************************************
Description : Open clicked option.
Input       : class wxWindow* Window_Next - The window to switch to.
Output      : None.
Return      : NMone
******************************************************************************/
void Option_Panel::Window_Switch(class wxWindow* Window_Next)
{
    if(Window_Next!=this->Window_Current)
    {
        if(this->Window_Current!=nullptr)
            this->Window_Current->Hide();
        Window_Next->Show();
        this->Layout();
    }

    this->Window_Current=Window_Next;
}
/* End Function:Option_Panel::Window_Switch **********************************/

/* Function:Option_Panel::Option_Show *****************************************
Description : Show the appropriate option window accordingly.
Input       : const std::string& Select_Text - The name of selected tree item.
              ptr_t Type - The type for selected tree item.
Output      : None.
Return      : ret_t - If the switch is successful, 0; else -1.
******************************************************************************/
ret_t Option_Panel::Option_Show(const std::string& Select_Text, ptr_t Type)
{
    wxLogDebug("Option_Panel::Option_Open: %s",Select_Text);

    /* Check and try to save the current panel */
    if(this->Current_Save()!=0)
        return -1;

    /* Load and show the selected panel */
    switch(Type)
    {
        case SELECT_DETAIL_BASIC:
        {
            this->Basic_Panel->Load();
            this->Window_Switch(this->Basic_Panel);
            break;
        }
        case SELECT_DETAIL_MEMORY:
        {
            this->Memory_Notebook->Load();
            this->Window_Switch(this->Memory_Notebook);
            break;
        }
        case SELECT_DETAIL_KERNEL:
        {
            this->Kernel_Panel->Load();
            this->Window_Switch(this->Kernel_Panel);
            break;
        }
        case SELECT_DETAIL_MONITOR:
        {
            this->Monitor_Panel->Load();
            this->Window_Switch(this->Monitor_Panel);
            break;
        }
        case SELECT_DETAIL_NATIVE_CHILD:
        {
            this->Native_Notebook->Load(Select_Text);
            this->Window_Switch(this->Native_Notebook);
            break;
        }
        case SELECT_DETAIL_VIRTUAL_CHILD:
        {
            this->Virtual_Notebook->Load(Select_Text);
            this->Window_Switch(this->Virtual_Notebook);
            break;
        }
        default:break;
    }

    return 0;
}
/* End Function:Option_Panel::Option_Show ************************************/

/* Function:Option_Panel::Native_Rename ***************************************
Description : Carry out the rename on the panel in case it is shown.
Input       : const std::string& Original - The original name of native process.
              const std::string& Current - The current name of native process.
Output      : None.
Return      : None.
******************************************************************************/
void Option_Panel::Native_Rename(const std::string& Original,const std::string& Current)
{
    /* Is the current one a native panel that has the name? if yes, rename it */
    if((this->Native_Notebook->IsShown())&&
       (this->Native_Notebook->Basic->Name->GetValue()==Original))
        this->Native_Notebook->Basic->Rename(Original,Current);
}
/* End Function:Option_Panel::Native_Rename **********************************/

/* Function:Option_Panel::Virtual_Rename **************************************
Description : Carry out the rename on the panel in case it is shown.
Input       : const std::string& Original - The original name of native process.
              const std::string& Current - The current name of native process.
Output      : None.
Return      : None.
******************************************************************************/
void Option_Panel::Virtual_Rename(const std::string& Original,const std::string& Current)
{
    /* Is the current one a native panel that has the name? if yes, rename it */
    if((this->Virtual_Notebook->IsShown())&&
       (this->Virtual_Notebook->Basic->Name->GetValue()==Original))
        this->Virtual_Notebook->Basic->Rename(Original,Current);
}
/* End Function:Option_Panel::Virtual_Rename *********************************/
}
/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
