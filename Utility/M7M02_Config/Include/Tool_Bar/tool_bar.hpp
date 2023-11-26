/******************************************************************************
Filename    : tool_bar.hpp
Author      : pry
Date        : 11/01/2023
License     : Proprietary; confidential.
Description : Tool bar class header.
******************************************************************************/

/* Define ********************************************************************/
#ifdef __HDR_DEF__
#ifndef __TOOL_BAR_TYPE__
#define __TOOL_BAR_TYPE__
/*****************************************************************************/

/*****************************************************************************/
/* __TOOL_BAR_TYPE__ */
#endif
/* __HDR_DEF__ */
#endif

namespace RVM_CFG
{
#ifdef __HDR_DEF__
#ifndef __TOOL_BAR_DEF__
#define __TOOL_BAR_DEF__
/*****************************************************************************/

/*****************************************************************************/
/* __TOOL_BAR_DEF__ */
#endif
/* __HDR_DEF__ */
#endif
/* End Define ****************************************************************/

/* Classes *******************************************************************/
#ifdef __HDR_CLASS__
#ifndef __TOOL_BAR_CLASS__
#define __TOOL_BAR_CLASS__
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
/* __TOOL_BAR_CLASS__ */
#endif
/* __HDR_CLASS__ */
#endif
}
/* End Classes ***************************************************************/

/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
