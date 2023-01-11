/******************************************************************************
Filename    : file_menu.hpp
Author      : zjx
Date        : 06/04/2021
License     : Proprietary; confidential.
Description : File menu class header.
******************************************************************************/

/* Defines *******************************************************************/
#ifdef __HDR_DEFS__
#ifndef __FILE_MENU_HPP_TYPES__
#define __FILE_MENU_HPP_TYPES__
/*****************************************************************************/

/*****************************************************************************/
/* __FILE_MENU_HPP_TYPES__ */
#endif
/* __HDR_DEFS__ */
#endif

namespace RVM_CFG
{
#ifdef __HDR_DEFS__
#ifndef __FILE_MENU_HPP_DEFS__
#define __FILE_MENU_HPP_DEFS__
/*****************************************************************************/

/*****************************************************************************/
/* __FILE_MENU_HPP_DEFS__ */
#endif
/* __HDR_DEFS__ */
#endif
/* End Defines ***************************************************************/

/* Classes *******************************************************************/
#ifdef __HDR_CLASSES__
#ifndef __FILE_MENU_HPP_CLASSES__
#define __FILE_MENU_HPP_CLASSES__
/*****************************************************************************/
class File_Menu:public wxMenu
{
public:
    class wxMenuItem* New_Proj;
    class wxMenuItem* Open_Proj;
    class wxMenuItem* Close_Proj;
    class wxMenuItem* Save_Proj;
    class wxMenuItem* Save_As;

    /* void */ File_Menu(void);
    /* void */ ~File_Menu(void);

    void State_Set(ptr_t Type);

    void On_New_Proj(class wxCommandEvent& Event);
    void On_Open_Proj(class wxCommandEvent& Event);
    void On_Close_Proj(class wxCommandEvent& Event);
    void On_Save_Proj(class wxCommandEvent& Event);
    void On_Save_As(class wxCommandEvent& Event);
};
/*****************************************************************************/
/* __FILE_MENU_HPP_CLASSES__ */
#endif
/* __HDR_CLASSES__ */
#endif
}
/* End Classes ***************************************************************/

/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
