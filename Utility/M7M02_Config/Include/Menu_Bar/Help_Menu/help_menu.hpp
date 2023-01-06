/******************************************************************************
Filename    : help_menu.hpp
Author      : zjx
Date        : 06/04/2021
License     : Proprietary; confidential.
Description : Help menu class header.
******************************************************************************/

/* Defines *******************************************************************/
#ifdef __HDR_DEFS__
#ifndef __HELP_MENU_HPP_TYPES__
#define __HELP_MENU_HPP_TYPES__
/*****************************************************************************/

/*****************************************************************************/
/* __HELP_MENU_HPP_TYPES__ */
#endif
/* __HDR_DEFS__ */
#endif

namespace RVM_CFG
{
#ifdef __HDR_DEFS__
#ifndef __HELP_MENU_HPP_DEFS__
#define __HELP_MENU_HPP_DEFS__
/*****************************************************************************/

/*****************************************************************************/
/* __HELP_MENU_HPP_DEFS__ */
#endif
/* __HDR_DEFS__ */
#endif
/* End Defines ***************************************************************/

/* Classes *******************************************************************/
#ifdef __HDR_CLASSES__
#ifndef __HELP_MENU_HPP_CLASSES__
#define __HELP_MENU_HPP_CLASSES__
/*****************************************************************************/
class Help_Menu:public wxMenu
{
public:
    class wxMenuItem* Hardware_Manual;
    class wxMenuItem* Downloader_Manual;
    class wxMenuItem* About_Program;
    class wxMenuItem* Quit;
#ifdef DEBUG
    class wxMenuItem* Error_Trigger;
#endif

    /* void */ Help_Menu(void);
    /* void */ ~Help_Menu(void);

    void On_Hardware_Manual(class wxCommandEvent& Event);
    void On_Downloader_Manual(class wxCommandEvent& Event);
    void On_About_Program(class wxCommandEvent& Event);
    void On_Quit(class wxCommandEvent& Event);
#ifdef DEBUG
    void On_Error_Trigger(class wxCommandEvent& Event);
#endif
};
/*****************************************************************************/
/* __HELP_MENU_HPP_CLASSES__ */
#endif
/* __HDR_CLASSES__ */
#endif
}
/* End Classes ***************************************************************/

/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
