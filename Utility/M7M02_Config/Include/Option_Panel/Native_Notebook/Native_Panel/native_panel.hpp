/******************************************************************************
Filename    : native_panel.hpp
Author      : lbc
Date        : 25/04/2025
Licence     : The Unlicense; see LICENSE for details.
Description : Basic information panel of native process class header.
******************************************************************************/

/* Define ********************************************************************/
#ifdef __HDR_DEF__
#ifndef __NATIVE_PANEL_TYPE__
#define __NATIVE_PANEL_TYPE__
/*****************************************************************************/

/*****************************************************************************/
/* __NATIVE_PANEL_TYPE__ */
#endif
/* __HDR_DEF__ */
#endif

namespace RVM_CFG
{
#ifdef __HDR_DEF__
#ifndef __NATIVE_PANEL_DEF__
#define __NATIVE_PANEL_DEF__
/*****************************************************************************/

/*****************************************************************************/
/* __NATIVE_PANEL_DEF__ */
#endif
/* __HDR_DEF__ */
#endif
/* End Define ****************************************************************/

/* Classes *******************************************************************/
#ifdef __HDR_CLASS__
#ifndef __NATIVE_PANEL_CLASS__
#define __NATIVE_PANEL_CLASS__
/*****************************************************************************/
class Native_Panel:public wxPanel
{
public:
    class wxBoxSizer* Main_Sizer;

    class wxStaticBoxSizer* Basic_Sizer;
    class wxStaticBoxSizer* Build_Sizer;
    class wxStaticBoxSizer* Output_Sizer;

    /* Basic options */
    class wxBoxSizer* Basic_Line1_Sizer;

    class wxBoxSizer* Name_Sizer;
    class wxStaticText* Name_Label;
    class wxTextCtrl* Name;

    class wxBoxSizer* Extra_Captbl_Sizer;
    class wxStaticText* Extra_Captbl_Label;
    class wxTextCtrl* Extra_Captbl;

    class wxStaticText* Coprocessor_Label;
    class std::map<std::string,wxCheckBox*> Coprocessor;
    class wxBoxSizer* Coprocessor_Sizer;

    /* Buildsystem options */
    class wxBoxSizer* Build_Line1_Sizer;
    class wxBoxSizer* Build_Line2_Sizer;

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

    /* void */ Native_Panel(class wxWindow* Parent);
    /* void */ ~Native_Panel(void);

    void Load(const class Native* Native);
    ret_t Check(void);
    void Save(class Native* Native);

    void Rename(const std::string& Original,const std::string& Current);
    void Coprocessor_Set(void);
    void Toolchain_Set(void);
    void Buildsystem_Set(const class wxString& Toolchain);

    void On_Name(class wxMouseEvent& Event);
    void On_Toolchain(class wxCommandEvent& Event);

};
/*****************************************************************************/
/* __NATIVE_PANEL_CLASS__ */
#endif
/* __HDR_CLASS__ */
#endif
}
/* End Classes ***************************************************************/

/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
