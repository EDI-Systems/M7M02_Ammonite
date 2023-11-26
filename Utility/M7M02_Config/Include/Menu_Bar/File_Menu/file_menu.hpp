/******************************************************************************
Filename    : file_menu.hpp
Author      : zjx
Date        : 06/04/2021
License     : Proprietary; confidential.
Description : File menu class header.
******************************************************************************/

/* Define ********************************************************************/
#ifdef __HDR_DEF__
#ifndef __FILE_MENU_TYPE__
#define __FILE_MENU_TYPE__
/*****************************************************************************/

/*****************************************************************************/
/* __FILE_MENU_TYPE__ */
#endif
/* __HDR_DEF__ */
#endif

namespace RVM_CFG
{
#ifdef __HDR_DEF__
#ifndef __FILE_MENU_DEF__
#define __FILE_MENU_DEF__
/*****************************************************************************/

/*****************************************************************************/
/* __FILE_MENU_DEF__ */
#endif
/* __HDR_DEF__ */
#endif
/* End Define ****************************************************************/

/* Classes *******************************************************************/
#ifdef __HDR_CLASS__
#ifndef __FILE_MENU_CLASS__
#define __FILE_MENU_CLASS__
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
/* __FILE_MENU_CLASS__ */
#endif
/* __HDR_CLASS__ */
#endif
}
/* End Classes ***************************************************************/

/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
