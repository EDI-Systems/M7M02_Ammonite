/******************************************************************************
Filename    : option_panel.hpp
Author      : lbc
Date        : 24/04/2025
Licence     : The Unlicense; see LICENSE for details.
Description : Option panel class header.
******************************************************************************/

/* Define ********************************************************************/
#ifdef __HDR_DEF__
#ifndef __OPTION_PANEL_TYPE__
#define __OPTION_PANEL_TYPE__
/*****************************************************************************/

/*****************************************************************************/
/* __OPTION_PANEL_TYPE__ */
#endif
/* __HDR_DEF__ */
#endif

namespace RVM_CFG
{
#ifdef __HDR_DEF__
#ifndef __OPTION_PANEL_DEF__
#define __OPTION_PANEL_DEF__
/*****************************************************************************/
#define PROCESS_NATIVE      0
#define PROCESS_VIRTUAL     1
/*****************************************************************************/
/* __OPTION_PANEL_DEF__ */
#endif
/* __HDR_DEF__ */
#endif
/* End Define ****************************************************************/

/* Classes *******************************************************************/
#ifdef __HDR_CLASS__
#ifndef __OPTION_PANEL_CLASS__
#define __OPTION_PANEL_CLASS__
/*****************************************************************************/
class Option_Panel:public wxPanel
{
public:
    class wxBoxSizer* Main_Sizer;

    class wxWindow* Window_Current;

    class Basic_Panel* Basic_Panel;
    class Memory_Notebook* Memory_Notebook;
    class Kernel_Panel* Kernel_Panel;
    class Monitor_Panel* Monitor_Panel;
    class Native_Notebook* Native_Notebook;
    class Virtual_Notebook* Virtual_Notebook;

    /* void */ Option_Panel(class wxWindow* Parent);
    /* void */ ~Option_Panel(void);

    void Load(void);
    ret_t Current_Save(void);

    void Window_Switch(class wxWindow* Window_Next);
    ret_t Option_Show(const std::string& Select_Text, ptr_t Type);
    void Native_Rename(const std::string& Original,const std::string& Current);
    void Virtual_Rename(const std::string& Original,const std::string& Current);
};
/*****************************************************************************/
/* __OPTION_PANEL_CLASS__ */
#endif
/* __HDR_CLASS__ */
#endif
}
/* End Classes ***************************************************************/

/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
