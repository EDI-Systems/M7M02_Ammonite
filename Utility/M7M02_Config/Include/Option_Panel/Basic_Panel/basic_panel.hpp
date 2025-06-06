/******************************************************************************
Filename    : basic_panel.hpp
Author      : lbc
Date        : 24/04/2025
Licence     : The Unlicense; see LICENSE for details.
Description : Basic panel class header.
******************************************************************************/

/* Define ********************************************************************/
#ifdef __HDR_DEF__
#ifndef __BASIC_PANEL_TYPE__
#define __BASIC_PANEL_TYPE__
/*****************************************************************************/
#define CONFIG_CHOICE       0
#define CONFIG_RANGE        1

/*****************************************************************************/
/* __BASIC_PANEL_TYPE__ */
#endif
/* __HDR_DEF__ */
#endif

namespace RVM_CFG
{
#ifdef __HDR_DEF__
#ifndef __BASIC_PANEL_DEF__
#define __BASIC_PANEL_DEF__
/*****************************************************************************/

/*****************************************************************************/
/* __BASIC_PANEL_DEF__ */
#endif
/* __HDR_DEF__ */
#endif
/* End Define ****************************************************************/

/* Classes *******************************************************************/
#ifdef __HDR_CLASS__
#ifndef __BASIC_PANEL_CLASS__
#define __BASIC_PANEL_CLASS__
/*****************************************************************************/
class Basic_Panel:public wxPanel
{
public:

    class wxBoxSizer* Main_Sizer;
    class wxStaticBoxSizer* Basic_Sizer;
    class wxStaticBoxSizer* Workspace_Sizer;
    class wxStaticBoxSizer* Target_Sizer;
    class wxStaticBoxSizer* Specific_Sizer;

    /* Group 1 */
    class wxBoxSizer* Basic_Line1_Sizer;
    class wxBoxSizer* Basic_Line2_Sizer;

    class wxStaticText* Name_Label;
    class wxTextCtrl* Name;
    class wxBoxSizer* Name_Sizer;

    class wxStaticText* Assert_Enable_Label;
    class wxCheckBox* Assert_Enable;
    class wxBoxSizer* Assert_Enable_Sizer;

    class wxStaticText* Debug_Log_Enable_Label;
    class wxCheckBox* Debug_Log_Enable;
    class wxBoxSizer* Debug_Log_Enable_Sizer;

    class wxStaticText* Pgtbl_Raw_Enable_Label;
    class wxCheckBox* Pgtbl_Raw_Enable;
    class wxBoxSizer* Pgtbl_Raw_Enable_Sizer;

    /* Group 2 */
    class wxBoxSizer* Workspace_Line1_Sizer;

    class wxStaticText* Buildsystem_Label;
    class wxChoice* Buildsystem;
    class wxBoxSizer* Buildsystem_Sizer;

    class wxStaticText* Workspace_Overwrite_Label;
    class wxCheckBox* Workspace_Overwrite;
    class wxBoxSizer* Workspace_Overwrite_Sizer;

    class wxStaticText* Coprocessor_Label;
    std::map<std::string,wxCheckBox*> Coprocessor;
    class wxBoxSizer* Coprocessor_Sizer;

    /* Group 4 */
    class wxBoxSizer* Target_Line1_Sizer;
    class wxBoxSizer* Target_Line2_Sizer;

    class wxStaticText* Platform_Label;
    class wxTextCtrl* Platform;
    class wxBoxSizer* Platform_Sizer;

    class wxStaticText* Class_Label;
    class wxTextCtrl* Class;
    class wxBoxSizer* Class_Sizer;

    class wxStaticText* Chip_Name_Label;
    class wxTextCtrl* Chip_Name;
    class wxBoxSizer* Chip_Name_Sizer;

    /* Group 5 */
    std::map<std::string, class wxTextCtrl*>Config_Range;
    std::map<std::string, class wxChoice*>Config_Choice;

    /* void */ Basic_Panel(class wxWindow* Parent);
    /* void */ ~Basic_Panel(void);

    void Load(void);
    ret_t Check(void);
    void Save(void);

    void Coprocessor_Set(void);
    void Buildsystem_Set(void);
    void Config_Set(void);

    void On_Name_Text(class wxMouseEvent& Event);
};
/*****************************************************************************/
/* __BASIC_PANEL_CLASS__ */
#endif
/* __HDR_CLASS__ */
#endif
}
/* End Classes ***************************************************************/

/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
