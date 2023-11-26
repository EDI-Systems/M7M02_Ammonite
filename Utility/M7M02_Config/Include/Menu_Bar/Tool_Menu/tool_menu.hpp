/******************************************************************************
Filename    : tool_menu.hpp
Author      : zjx
Date        : 06/04/2021
License     : Proprietary; confidential.
Description : Tool menu class header.
******************************************************************************/

/* Define ********************************************************************/
#ifdef __HDR_DEF__
#ifndef __TOOL_MENU_TYPE__
#define __TOOL_MENU_TYPE__
/*****************************************************************************/

/*****************************************************************************/
/* __TOOL_MENU_TYPE__ */
#endif
/* __HDR_DEF__ */
#endif

namespace RVM_CFG
{
#ifdef __HDR_DEF__
#ifndef __TOOL_MENU_DEF__
#define __TOOL_MENU_DEF__
/*****************************************************************************/

/*****************************************************************************/
/* __TOOL_MENU_DEF__ */
#endif
/* __HDR_DEF__ */
#endif
/* End Define ****************************************************************/

/* Classes *******************************************************************/
#ifdef __HDR_CLASS__
#ifndef __TOOL_MENU_CLASS__
#define __TOOL_MENU_CLASS__
/*****************************************************************************/
class Tool_Menu:public wxMenu
{
public:
    class wxMenuItem* Shm_View;
    class wxMenuItem* Inv_View;
    class wxMenuItem* Rcv_View;
    class wxMenuItem* Generate;
    class wxMenuItem* Validate;

    /* void */ Tool_Menu(void);
    /* void */ ~Tool_Menu(void);

    void State_Set(ptr_t Type);

    void On_Shm_View(class wxCommandEvent& Event);
    void On_Inv_View(class wxCommandEvent& Event);
    void On_Rcv_View(class wxCommandEvent& Event);
    void On_Generate(class wxCommandEvent& Event);
    void On_Validate(class wxCommandEvent& Event);
};
/*****************************************************************************/
/* __TOOL_MENU_CLASS__ */
#endif
/* __HDR_CLASS__ */
#endif
}
/* End Classes ***************************************************************/

/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
