/******************************************************************************
Filename    : tool_menu.hpp
Author      : zjx
Date        : 06/04/2021
License     : Proprietary; confidential.
Description : Tool menu class header.
******************************************************************************/

/* Defines *******************************************************************/
#ifdef __HDR_DEFS__
#ifndef __TOOL_MENU_HPP_TYPES__
#define __TOOL_MENU_HPP_TYPES__
/*****************************************************************************/

/*****************************************************************************/
/* __TOOL_MENU_HPP_TYPES__ */
#endif
/* __HDR_DEFS__ */
#endif

namespace RVM_CFG
{
#ifdef __HDR_DEFS__
#ifndef __TOOL_MENU_HPP_DEFS__
#define __TOOL_MENU_HPP_DEFS__
/*****************************************************************************/

/*****************************************************************************/
/* __TOOL_MENU_HPP_DEFS__ */
#endif
/* __HDR_DEFS__ */
#endif
/* End Defines ***************************************************************/

/* Classes *******************************************************************/
#ifdef __HDR_CLASSES__
#ifndef __TOOL_MENU_HPP_CLASSES__
#define __TOOL_MENU_HPP_CLASSES__
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
/* __TOOL_MENU_HPP_CLASSES__ */
#endif
/* __HDR_CLASSES__ */
#endif
}
/* End Classes ***************************************************************/

/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
