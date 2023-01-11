/******************************************************************************
Filename    : config_popup.hpp
Author      : pry
Date        : 11/01/2023
License     : Proprietary; confidential.
Description : Config popup-menu class header.
******************************************************************************/

/* Defines *******************************************************************/
#ifdef __HDR_DEFS__
#ifndef __CONFIG_POPUP_HPP_TYPES__
#define __CONFIG_POPUP_HPP_TYPES__
/*****************************************************************************/

/*****************************************************************************/
/* __CONFIG_POPUP_HPP_TYPES__ */
#endif
/* __HDR_DEFS__ */
#endif

namespace RVM_CFG
{
#ifdef __HDR_DEFS__
#ifndef __CONFIG_POPUP_HPP_DEFS__
#define __CONFIG_POPUP_HPP_DEFS__
/*****************************************************************************/

/*****************************************************************************/
/* __CONFIG_POPUP_HPP_DEFS__ */
#endif
/* __HDR_DEFS__ */
#endif
/* End Defines ***************************************************************/

/* Classes *******************************************************************/
#ifdef __HDR_CLASSES__
#ifndef __CONFIG_POPUP_HPP_CLASSES__
#define __CONFIG_POPUP_HPP_CLASSES__
/*****************************************************************************/
class Config_Popup:public wxMenu
{
public:
    class Config_Tree* Tree;

    class wxMenuItem* New_Item;
    class wxMenuItem* Delete_Item;
    class wxMenuItem* Generate;
    class wxMenuItem* Validate;

    /* void */ Config_Popup(class Config_Tree* Tree);
    /* void */ ~Config_Popup(void);

    void State_Set(ptr_t Type);

    void On_New_Item(class wxCommandEvent& Event);
    void On_Delete_Item(class wxCommandEvent& Event);
    void On_Generate(class wxCommandEvent& Event);
    void On_Validate(class wxCommandEvent& Event);
};
/*****************************************************************************/
/* __CONFIG_POPUP_HPP_CLASSES__ */
#endif
/* __HDR_CLASSES__ */
#endif
}
/* End Classes ***************************************************************/

/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
