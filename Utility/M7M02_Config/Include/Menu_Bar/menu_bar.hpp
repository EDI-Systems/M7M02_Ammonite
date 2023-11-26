/******************************************************************************
Filename    : menu_bar.hpp
Author      : zjx
Date        : 06/04/2021
License     : Proprietary; confidential.
Description : Menu bar class header.
******************************************************************************/

/* Define ********************************************************************/
#ifdef __HDR_DEF__
#ifndef __MENU_BAR_TYPE__
#define __MENU_BAR_TYPE__
/*****************************************************************************/

/*****************************************************************************/
/* __MENU_BAR_TYPE__ */
#endif
/* __HDR_DEF__ */
#endif

namespace RVM_CFG
{
#ifdef __HDR_DEF__
#ifndef __MENU_BAR_DEF__
#define __MENU_BAR_DEF__
/*****************************************************************************/

/*****************************************************************************/
/* __MENU_BAR_DEF__ */
#endif
/* __HDR_DEF__ */
#endif
/* End Define ****************************************************************/

/* Classes *******************************************************************/
#ifdef __HDR_CLASS__
#ifndef __MENU_BAR_CLASS__
#define __MENU_BAR_CLASS__
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
/* __MENU_BAR_CLASS__ */
#endif
/* __HDR_CLASS__ */
#endif
}
/* End Classes ***************************************************************/

/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
