/******************************************************************************
Filename    : virtual_panel.hpp
Author      : lbc
Date        : 25/04/2025
Licence     : The Unlicense; see LICENSE for details.
Description : Basic information panel of VM class header.
******************************************************************************/

/* Define ********************************************************************/
#ifdef __HDR_DEF__
#ifndef __VIRTUAL_PANEL_TYPE__
#define __VIRTUAL_PANEL_TYPE__
/*****************************************************************************/

/*****************************************************************************/
/* __VIRTUAL_PANEL_TYPE__ */
#endif
/* __HDR_DEF__ */
#endif

namespace RVM_CFG
{
#ifdef __HDR_DEF__
#ifndef __VIRTUAL_PANEL_DEF__
#define __VIRTUAL_PANEL_DEF__
/*****************************************************************************/

/*****************************************************************************/
/* __VIRTUAL_PANEL_DEF__ */
#endif
/* __HDR_DEF__ */
#endif
/* End Define ****************************************************************/

/* Classes *******************************************************************/
#ifdef __HDR_CLASS__
#ifndef __VIRTUAL_PANEL_CLASS__
#define __VIRTUAL_PANEL_CLASS__
/*****************************************************************************/
class Virtual_Panel:public wxPanel
{
public:
    class wxBoxSizer* Main_Sizer;
    class wxStaticBoxSizer* Basic_Sizer;
    class wxStaticBoxSizer* Build_Sizer;
    class wxStaticBoxSizer* Virt_Sizer;
    class wxStaticBoxSizer* Output_Sizer;

    /* Basic options */
    class wxBoxSizer* Basic_Line1_Sizer;

    class wxBoxSizer* Name_Sizer;
    class wxStaticText* Name_Label;
    class wxTextCtrl* Name;

    class wxBoxSizer* Extra_Captbl_Sizer;
    class wxStaticText* Extra_Captbl_Label;
    class wxTextCtrl* Extra_Captbl;

    class wxBoxSizer* Vector_Stack_Size_Sizer;
    class wxStaticText* Vector_Stack_Size_Label;
    class wxTextCtrl* Vector_Stack_Size;

    class wxBoxSizer* User_Stack_Size_Sizer;
    class wxStaticText* User_Stack_Size_Label;
    class wxTextCtrl* User_Stack_Size;

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

    class wxBoxSizer* Guest_Sizer;
    class wxStaticText* Guest_Label;
    class wxChoice* Guest;

    /* Virtualization options */
    class wxBoxSizer* Virt_Line1_Sizer;
    class wxBoxSizer* Virt_Line2_Sizer;
    class wxBoxSizer* Virt_Line3_Sizer;
    class wxBoxSizer* Virt_Line4_Sizer;

    class wxBoxSizer* Priority_Sizer;
    class wxStaticText* Priority_Label;
    class wxTextCtrl* Priority;

    class wxBoxSizer* Timeslice_Sizer;
    class wxStaticText* Timeslice_Label;
    class wxTextCtrl* Timeslice;

    class wxBoxSizer* Period_Sizer;
    class wxStaticText* Period_Label;
    class wxTextCtrl* Period;

    class wxBoxSizer* Watchdog_Sizer;
    class wxStaticText* Watchdog_Label;
    class wxTextCtrl* Watchdog;

    class wxBoxSizer* Vector_Num_Sizer;
    class wxStaticText* Vector_Num_Label;
    class wxTextCtrl* Vector_Num;

    /* Output options */
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

    class wxBoxSizer* Virtual_Header_Sizer;
    class wxStaticText* Virtual_Header_Output_Label;
    class wxTextCtrl* Virtual_Header_Output;
    class wxCheckBox* Virtual_Header_Overwrite;

    class wxBoxSizer* Virtual_Source_Sizer;
    class wxStaticText* Virtual_Source_Output_Label;
    class wxTextCtrl* Virtual_Source_Output;
    class wxCheckBox* Virtual_Source_Overwrite;

    /* void */ Virtual_Panel(class wxWindow* Parent);
    /* void */ ~Virtual_Panel(void);

    void Load(const class Virtual* Virtual_Info);
    ret_t Check();
    void Save(class Virtual* Virtual_Info);

    void Rename(const std::string& Original, const std::string& Current);

    void Coprocessor_Set(void);
    void Toolchain_Set(void);
    void Buildsystem_Set(const class wxString& Toolchain);
    void Guest_Set(const class wxString& Toolchain,
                   const class wxString& Buildsystem);

    void On_Name(class wxMouseEvent& Event);
    void On_Text_Hex(class wxFocusEvent& Event);
    void On_Toolchain(class wxCommandEvent& Event);
    void On_Buildsystem(class wxCommandEvent& Event);
};
/*****************************************************************************/
/* __VIRTUAL_PANEL_CLASS__ */
#endif
/* __HDR_CLASS__ */
#endif
}
/* End Classes ***************************************************************/

/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
