/******************************************************************************
Filename    : monitor panel.hpp
Author      : lbc
Date        : 24/04/2025
Licence     : The Unlicense; see LICENSE for details.
Description : Monitor panel class header.
******************************************************************************/

/* Define ********************************************************************/
#ifdef __HDR_DEF__
#ifndef __MONITOR_PANEL_TYPE__
#define __MONITOR_PANEL_TYPE__
/*****************************************************************************/

/*****************************************************************************/
/* __MONITOR_PANEL_TYPE__ */
#endif
/* __HDR_DEF__ */
#endif

namespace RVM_CFG
{
#ifdef __HDR_DEF__
#ifndef __MONITOR_PANEL_DEF__
#define __MONITOR_PANEL_DEF__
/*****************************************************************************/

/*****************************************************************************/
/* __MONITOR_PANEL_DEF__ */
#endif
/* __HDR_DEF__ */
#endif
/* End Define ****************************************************************/

/* Classes *******************************************************************/
#ifdef __HDR_CLASS__
#ifndef __MONITOR_PANEL_CLASS__
#define __MONITOR_PANEL_CLASS__
/*****************************************************************************/
class Monitor_Panel:public wxPanel
{
public:
    class wxBoxSizer* Main_Sizer;

    class wxStaticBoxSizer* Basic_Sizer;
    class wxStaticBoxSizer* Hyper_Sizer;
    class wxStaticBoxSizer* Build_Sizer;
    class wxStaticBoxSizer* Output_Sizer;

    /* Basic options */
    class wxBoxSizer* Basic_Line1_Sizer;
    class wxBoxSizer* Basic_Line2_Sizer;
    class wxBoxSizer* Basic_Line3_Sizer;

    class wxBoxSizer* Code_Size_Sizer;
    class wxStaticText* Code_Size_Label;
    class wxTextCtrl* Code_Size;

    class wxBoxSizer* Data_Size_Sizer;
    class wxStaticText* Data_Size_Label;
    class wxTextCtrl* Data_Size;

    class wxBoxSizer* Init_Stack_Size_Sizer;
    class wxStaticText* Init_Stack_Size_Label;
    class wxTextCtrl* Init_Stack_Size;

    class wxBoxSizer* Sftd_Stack_Size_Sizer;
    class wxStaticText* Sftd_Stack_Size_Label;
    class wxTextCtrl* Sftd_Stack_Size;

    class wxBoxSizer* Vmmd_Stack_Size_Sizer;
    class wxStaticText* Vmmd_Stack_Size_Label;
    class wxTextCtrl* Vmmd_Stack_Size;

    /* Hypervisor options */
    class wxBoxSizer* Hyper_Line1_Sizer;
    class wxBoxSizer* Hyper_Line2_Sizer;

    class wxBoxSizer* Extra_Captbl_Sizer;
    class wxStaticText* Extra_Captbl_Label;
    class wxTextCtrl* Extra_Captbl;

    class wxBoxSizer* Virt_Prio_Sizer;
    class wxStaticText* Virt_Prio_Label;
    class wxChoice* Virt_Prio;

    class wxBoxSizer* Virt_Event_Sizer;
    class wxStaticText* Virt_Event_Label;
    class wxChoice* Virt_Event;

    class wxBoxSizer* Virt_Map_Sizer;
    class wxStaticText* Virt_Map_Label;
    class wxTextCtrl* Virt_Map;

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

    class wxBoxSizer* Idle_Sleep_Enable_Sizer;
    class wxStaticText* Idle_Sleep_Enable_Label;
    class wxCheckBox* Idle_Sleep_Enable;

    /* Group 4 */
    class wxBoxSizer* Project_Sizer;
    class wxStaticText* Project_Output_Label;
    class wxTextCtrl* Project_Output;
    class wxCheckBox* Project_Overwrite;

    class wxBoxSizer* Linker_Output_Sizer;
    class wxStaticText* Linker_Output_Label;
    class wxTextCtrl* Linker_Output;

    class wxBoxSizer* Config_Header_Output_Sizer;
    class wxStaticText* Config_Header_Output_Label;
    class wxTextCtrl* Config_Header_Output;

    class wxBoxSizer* Boot_Header_Output_Sizer;
    class wxStaticText* Boot_Header_Output_Label;
    class wxTextCtrl* Boot_Header_Output;

    class wxBoxSizer* Boot_Source_Output_Sizer;
    class wxStaticText* Boot_Source_Output_Label;
    class wxTextCtrl* Boot_Source_Output;

    class wxBoxSizer* Hook_Source_Sizer;
    class wxStaticText* Hook_Source_Output_Label;
    class wxTextCtrl* Hook_Source_Output;
    class wxCheckBox* Hook_Source_Overwrite;

    /* void */ Monitor_Panel(class wxWindow* Parent);
    /* void */ ~Monitor_Panel(void);

    void Load(void);
    ret_t Check(void);
    void Save(void);

    void Toolchain_Set(void);
    void Buildsystem_Set(const class wxString& Toolchain);
    void Virt_Prio_Set(void);
    void Virt_Event_Set(void);

    void On_Text_Hex(class wxFocusEvent& Event);
    void On_Toolchain(class wxCommandEvent& Event);
};
/*****************************************************************************/
/* __MONITOR_PANEL_CLASS__ */
#endif
/* __HDR_CLASS__ */
#endif
}
/* End Classes ***************************************************************/

/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
