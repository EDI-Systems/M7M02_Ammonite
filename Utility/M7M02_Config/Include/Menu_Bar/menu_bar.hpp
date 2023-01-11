/******************************************************************************
Filename    : menu_bar.hpp
Author      : zjx
Date        : 06/04/2021
License     : Proprietary; confidential.
Description : Menu bar class header.
******************************************************************************/

/* Defines *******************************************************************/
#ifdef __HDR_DEFS__
#ifndef __MENU_BAR_HPP_TYPES__
#define __MENU_BAR_HPP_TYPES__
/*****************************************************************************/

/*****************************************************************************/
/* __MENU_BAR_HPP_TYPES__ */
#endif
/* __HDR_DEFS__ */
#endif

namespace RVM_CFG
{
#ifdef __HDR_DEFS__
#ifndef __MENU_BAR_HPP_DEFS__
#define __MENU_BAR_HPP_DEFS__
/*****************************************************************************/

/*****************************************************************************/
/* __MENU_BAR_HPP_DEFS__ */
#endif
/* __HDR_DEFS__ */
#endif
/* End Defines ***************************************************************/

/* Classes *******************************************************************/
#ifdef __HDR_CLASSES__
#ifndef __MENU_BAR_HPP_CLASSES__
#define __MENU_BAR_HPP_CLASSES__
/*****************************************************************************/
class Menu_Bar:public wxMenuBar
{
public:
    class File_Menu* File;
    class Tool_Menu* Tool;
    class Help_Menu* Help;

    /* void */ Menu_Bar(void);

    void State_Set(ptr_t Type);
};
/*****************************************************************************/
/* __MENU_BAR_HPP_CLASSES__ */
#endif
/* __HDR_CLASSES__ */
#endif
}
/* End Classes ***************************************************************/

/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
