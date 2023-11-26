/******************************************************************************
Filename    : help_menu.hpp
Author      : zjx
Date        : 06/04/2021
License     : Proprietary; confidential.
Description : Help menu class header.
******************************************************************************/

/* Define ********************************************************************/
#ifdef __HDR_DEF__
#ifndef __HELP_MENU_TYPE__
#define __HELP_MENU_TYPE__
/*****************************************************************************/

/*****************************************************************************/
/* __HELP_MENU_TYPE__ */
#endif
/* __HDR_DEF__ */
#endif

namespace RVM_CFG
{
#ifdef __HDR_DEF__
#ifndef __HELP_MENU_DEF__
#define __HELP_MENU_DEF__
/*****************************************************************************/

/*****************************************************************************/
/* __HELP_MENU_DEF__ */
#endif
/* __HDR_DEF__ */
#endif
/* End Define ****************************************************************/

/* Classes *******************************************************************/
#ifdef __HDR_CLASS__
#ifndef __HELP_MENU_CLASS__
#define __HELP_MENU_CLASS__
/*****************************************************************************/
class Help_Menu:public wxMenu
{
public:
    class wxMenuItem* Kernel_Manual;
    class wxMenuItem* Monitor_Manual;
    class wxMenuItem* About_Program;
    class wxMenuItem* Quit;
#ifdef DEBUG
    class wxMenuItem* Error_Trigger;
#endif

    /* void */ Help_Menu(void);
    /* void */ ~Help_Menu(void);

    void State_Set(ptr_t Type);

    void On_Kernel_Manual(class wxCommandEvent& Event);
    void On_Monitor_Manual(class wxCommandEvent& Event);
    void On_About_Program(class wxCommandEvent& Event);
    void On_Quit(class wxCommandEvent& Event);
#ifdef DEBUG
    void On_Error_Trigger(class wxCommandEvent& Event);
#endif
};
/*****************************************************************************/
/* __HELP_MENU_CLASS__ */
#endif
/* __HDR_CLASS__ */
#endif
}
/* End Classes ***************************************************************/

/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
