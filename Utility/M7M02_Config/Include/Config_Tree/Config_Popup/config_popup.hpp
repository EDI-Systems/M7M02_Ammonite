/******************************************************************************
Filename    : config_popup.hpp
Author      : pry
Date        : 11/01/2023
License     : Proprietary; confidential.
Description : Config popup-menu class header.
******************************************************************************/

/* Define ********************************************************************/
#ifdef __HDR_DEF__
#ifndef __CONFIG_POPUP_TYPE__
#define __CONFIG_POPUP_TYPE__
/*****************************************************************************/

/*****************************************************************************/
/* __CONFIG_POPUP_TYPE__ */
#endif
/* __HDR_DEF__ */
#endif

namespace RVM_CFG
{
#ifdef __HDR_DEF__
#ifndef __CONFIG_POPUP_DEF__
#define __CONFIG_POPUP_DEF__
/*****************************************************************************/

/*****************************************************************************/
/* __CONFIG_POPUP_DEF__ */
#endif
/* __HDR_DEF__ */
#endif
/* End Define ****************************************************************/

/* Classes *******************************************************************/
#ifdef __HDR_CLASS__
#ifndef __CONFIG_POPUP_CLASS__
#define __CONFIG_POPUP_CLASS__
/*****************************************************************************/
class Config_Popup:public wxMenu
{
public:
    class Config_Tree* Tree;

    class wxMenuItem* New_Item;
    class wxMenuItem* Delete_Item;
    class wxMenuItem* Generate;
    class wxMenuItem* Validate;
    class wxMenuItem* Rename;

    /* void */ Config_Popup(class Config_Tree* Tree);
    /* void */ ~Config_Popup(void);

    ret_t Native_Add(const std::string& Name);
    ret_t Virtual_Add(const std::string& Name);
    ret_t Native_Rename(const std::string& Original, const std::string& Current);
    ret_t Virtual_Rename(const std::string& Original, const std::string& Current);

    void State_Set(void);

    void On_New_Item(class wxCommandEvent& Event);
    void On_Delete_Item(class wxCommandEvent& Event);
    void On_Generate(class wxCommandEvent& Event);
    void On_Validate(class wxCommandEvent& Event);
    void On_Rename(class wxCommandEvent& Event);
};
/*****************************************************************************/
/* __CONFIG_POPUP_CLASS__ */
#endif
/* __HDR_CLASS__ */
#endif
}
/* End Classes ***************************************************************/

/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
