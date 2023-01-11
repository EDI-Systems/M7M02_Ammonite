/******************************************************************************
Filename    : tool_bar.hpp
Author      : pry
Date        : 11/01/2023
License     : Proprietary; confidential.
Description : Tool bar class header.
******************************************************************************/

/* Defines *******************************************************************/
#ifdef __HDR_DEFS__
#ifndef __TOOL_BAR_HPP_TYPES__
#define __TOOL_BAR_HPP_TYPES__
/*****************************************************************************/

/*****************************************************************************/
/* __TOOL_BAR_HPP_TYPES__ */
#endif
/* __HDR_DEFS__ */
#endif

namespace RVM_CFG
{
#ifdef __HDR_DEFS__
#ifndef __TOOL_BAR_HPP_DEFS__
#define __TOOL_BAR_HPP_DEFS__
/*****************************************************************************/

/*****************************************************************************/
/* __TOOL_BAR_HPP_DEFS__ */
#endif
/* __HDR_DEFS__ */
#endif
/* End Defines ***************************************************************/

/* Classes *******************************************************************/
#ifdef __HDR_CLASSES__
#ifndef __TOOL_BAR_HPP_CLASSES__
#define __TOOL_BAR_HPP_CLASSES__
/*****************************************************************************/
class Tool_Bar:public wxToolBar
{
public:
    class wxToolBarToolBase* New_Proj;
    class wxToolBarToolBase* Open_Proj;
    class wxToolBarToolBase* Save_Proj;
    class wxToolBarToolBase* Generate;
    class wxToolBarToolBase* Validate;

    /* void */ Tool_Bar(class wxWindow* Parent);
    /* void */ ~Tool_Bar(void);

    void State_Set(ptr_t Type);

    void On_New_Proj(class wxCommandEvent& Event);
    void On_Open_Proj(class wxCommandEvent& Event);
    void On_Save_Proj(class wxCommandEvent& Event);
    void On_Generate(class wxCommandEvent& Event);
    void On_Validate(class wxCommandEvent& Event);
};
/*****************************************************************************/
/* __TOOL_BAR_HPP_CLASSES__ */
#endif
/* __HDR_CLASSES__ */
#endif
}
/* End Classes ***************************************************************/

/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
