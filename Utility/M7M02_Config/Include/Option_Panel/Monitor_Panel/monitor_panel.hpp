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

    class wxStaticBoxSizer* Sizer1;
    class wxStaticBoxSizer* Sizer2;
    class wxStaticBoxSizer* Sizer3;
    class wxStaticBoxSizer* Sizer4;

    /* Group 1 */
    class wxBoxSizer* Sizer1_1;
    class wxBoxSizer* Sizer1_2;
    class wxBoxSizer* Sizer1_3;

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

    /* Group 2 */
    class wxBoxSizer* Sizer2_1;
    class wxBoxSizer* Sizer2_2;

    class wxBoxSizer* Extra_Captbl_Sizer;
    class wxStaticText* Extra_Captbl_Label;
    class wxTextCtrl* Extra_Captbl;

    class wxBoxSizer* Virt_Prio_Sizer;
    class wxStaticText* Virt_Prio_Label;
    class wxTextCtrl* Virt_Prio;

    class wxBoxSizer* Virt_Event_Sizer;
    class wxStaticText* Virt_Event_Label;
    class wxTextCtrl* Virt_Event;

    class wxBoxSizer* Virt_Map_Sizer;
    class wxStaticText* Virt_Map_Label;
    class wxTextCtrl* Virt_Map;

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

    ret_t Has_Been_Shown;

    /* void */ Monitor_Panel(class wxWindow*parent);
    /* void */ ~Monitor_Panel(void);

    void Load(void);
    void Save(void);
    ret_t Check(void);
    void Buildsystem_Toolchain_Set(void);
    void On_Trans_Hex(class wxFocusEvent& Event);
    void On_Toolchain_Change(class wxCommandEvent& Event);
    void On_Buildsystem_Change(class wxCommandEvent& Event);
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
