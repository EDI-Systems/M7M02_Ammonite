/******************************************************************************
Filename    : native_basic_panel.hpp
Author      : lbc
Date        : 25/04/2025
Licence     : The Unlicense; see LICENSE for details.
Description : Basic information panel of native process class header.
******************************************************************************/

/* Define ********************************************************************/
#ifdef __HDR_DEF__
#ifndef __NATIVE_BASIC_PANEL_TYPE__
#define __NATIVE_BASIC_PANEL_TYPE__
/*****************************************************************************/

/*****************************************************************************/
/* __NATIVE_BASIC_PANEL_TYPE__ */
#endif
/* __HDR_DEF__ */
#endif

namespace RVM_CFG
{
#ifdef __HDR_DEF__
#ifndef __NATIVE_BASIC_PANEL_DEF__
#define __NATIVE_BASIC_PANEL_DEF__
/*****************************************************************************/

/*****************************************************************************/
/* __NATIVE_BASIC_PANEL_DEF__ */
#endif
/* __HDR_DEF__ */
#endif
/* End Define ****************************************************************/

/* Classes *******************************************************************/
#ifdef __HDR_CLASS__
#ifndef __NATIVE_BASIC_PANEL_CLASS__
#define __NATIVE_BASIC_PANEL_CLASS__
/*****************************************************************************/
class Native_Basic_Panel:public wxPanel
{
public:
    class wxBoxSizer* Main_Sizer;

    class wxStaticBoxSizer* Sizer1;
    //class wxStaticBoxSizer* Sizer2;
    class wxStaticBoxSizer* Sizer3;
    class wxStaticBoxSizer* Sizer4;

    /* Group 1 */
    class wxBoxSizer* Sizer1_1;

    class wxBoxSizer* Name_Sizer;
    class wxStaticText* Name_Label;
    class wxTextCtrl* Name;

    class wxBoxSizer* Extra_Captbl_Sizer;
    class wxStaticText* Extra_Captbl_Label;
    class wxTextCtrl* Extra_Captbl;

    /* Group 2 */
    //class wxBoxSizer* Sizer2_1;

    class wxStaticText* Coprocessor_Label;
    class std::map<std::string,wxCheckBox*> Coprocessor;
    class wxBoxSizer* Coprocessor_Sizer;

    /* Group 3 */
    class wxBoxSizer* Sizer3_1;
    class wxBoxSizer* Sizer3_2;

    class wxBoxSizer* Toolchain_Sizer;
    class wxStaticText* Toolchain_Label;
    class wxChoice* Toolchain;

    class wxBoxSizer* Buildsystem_Sizer;
    class wxStaticText* Buildsystem_Label;
    class wxChoice* Buildsystem;

    class wxBoxSizer* Optimization_Sizer;
    class wxStaticText* Optimization_Label;
    class wxChoice* Optimization;

    /* Group 4 */
    class wxBoxSizer* Project_Sizer;
    class wxStaticText* Project_Output_Label;
    class wxTextCtrl* Project_Output;
    class wxCheckBox* Project_Overwrite;

    class wxBoxSizer* Linker_Output_Sizer;
    class wxStaticText* Linker_Output_Label;
    class wxTextCtrl* Linker_Output;

    class wxBoxSizer* Main_Header_Output_Sizer;
    class wxStaticText* Main_Header_Output_Label;
    class wxTextCtrl* Main_Header_Output;

    class wxBoxSizer* Main_Source_Output_Sizer;
    class wxStaticText* Main_Source_Output_Label;
    class wxTextCtrl* Main_Source_Output;

    class wxBoxSizer* Entry_Source_Sizer;
    class wxStaticText* Entry_Source_Output_Label;
    class wxTextCtrl* Entry_Source_Output;
    class wxCheckBox* Entry_Source_Overwrite;

    /* void */ Native_Basic_Panel(class wxWindow*parent);
    /* void */ ~Native_Basic_Panel(void);

    void Load(const class Native* Native);
    void Save(class Native* Native);
    ret_t Check();

    void Coprocessor_Set(void);
    void Buildsystem_Toolchain_Set(void);
    void On_Toolchain_Change(class wxCommandEvent& Event);
    void On_Buildsystem_Change(class wxCommandEvent& Event);
    void On_Rename(class wxMouseEvent& Event);
};
/*****************************************************************************/
/* __NATIVE_BASIC_PANEL_CLASS__ */
#endif
/* __HDR_CLASS__ */
#endif
}
/* End Classes ***************************************************************/

/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
