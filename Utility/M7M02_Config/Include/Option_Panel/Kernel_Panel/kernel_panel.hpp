/******************************************************************************
Filename    : kernel_panel.hpp
Author      : lbc
Date        : 24/04/2025
Licence     : The Unlicense; see LICENSE for details.
Description : Kernel panel class header.
******************************************************************************/

/* Define ********************************************************************/
#ifdef __HDR_DEF__
#ifndef __KERNEL_PANEL_TYPE__
#define __KERNEL_PANEL_TYPE__
/*****************************************************************************/

/*****************************************************************************/
/* __KERNEL_PANEL_TYPE__ */
#endif
/* __HDR_DEF__ */
#endif

namespace RVM_CFG
{
#ifdef __HDR_DEF__
#ifndef __KERNEL_PANEL_DEF__
#define __KERNEL_PANEL_DEF__
/*****************************************************************************/

/*****************************************************************************/
/* __KERNEL_PANEL_DEF__ */
#endif
/* __HDR_DEF__ */
#endif
/* End Define ****************************************************************/

/* Classes *******************************************************************/
#ifdef __HDR_CLASS__
#ifndef __KERNEL_PANEL_CLASS__
#define __KERNEL_PANEL_CLASS__
/*****************************************************************************/
class Kernel_Panel:public wxPanel
{
public:
    class wxBoxSizer* Main_Sizer;

    class wxStaticBoxSizer* Basic_Sizer;
    class wxStaticBoxSizer* KOM_Sizer;
    class wxStaticBoxSizer* Build_Sizer;
    class wxStaticBoxSizer* Output_Sizer;

    /* Basic options */
    class wxBoxSizer* Basic_Line1_Sizer;
    class wxBoxSizer* Basic_Line2_Sizer;
    class wxBoxSizer* Basic_Line3_Sizer;

    class wxBoxSizer* Code_Base_Sizer;
    class wxStaticText* Code_Base_Label;
    class wxTextCtrl* Code_Base;

    class wxBoxSizer* Code_Size_Sizer;
    class wxStaticText* Code_Size_Label;
    class wxTextCtrl* Code_Size;

    class wxBoxSizer* Data_Base_Sizer;
    class wxStaticText* Data_Base_Label;
    class wxTextCtrl* Data_Base;

    class wxBoxSizer* Data_Size_Sizer;
    class wxStaticText* Data_Size_Label;
    class wxTextCtrl* Data_Size;

    class wxBoxSizer* Stack_Size_Sizer;
    class wxStaticText* Stack_Size_Label;
    class wxTextCtrl* Stack_Size;


    class wxBoxSizer* Kern_Prio_Sizer;
    class wxStaticText* Kern_Prio_Label;
    class wxChoice* Kern_Prio;

    /* KOM options */
    class wxBoxSizer* KOM_Line1_Sizer;

    class wxBoxSizer* Kom_Order_Sizer;
    class wxStaticText* Kom_Order_Label;
    class wxChoice* Kom_Order;

    class wxBoxSizer* Extra_Kom_Sizer;
    class wxStaticText* Extra_Kom_Label;
    class wxTextCtrl* Extra_Kom;

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

    class wxBoxSizer* Full_Image_Sizer;
    class wxStaticText* Full_Image_Label;
    class wxCheckBox* Full_Image;

    /* Group 4 */
    class wxBoxSizer* Project_Output_Sizer;
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

    class wxBoxSizer* Handler_Source_Sizer;
    class wxStaticText* Handler_Source_Output_Label;
    class wxTextCtrl* Handler_Source_Output;
    class wxCheckBox* Handler_Source_Overwrite;

    /* void */ Kernel_Panel(class wxWindow* Parent);
    /* void */ ~Kernel_Panel(void);

    void Load(void);
    ret_t Check(void);
    void Save(void);

    void Toolchain_Set(void);
    void Buildsystem_Set(const class wxString& Toolchain);
    void Kernel_Prio_Set(void);

    void On_Text_Hex(class wxFocusEvent& Event);
    void On_Toolchain(class wxCommandEvent& Event);
};
/*****************************************************************************/
/* __KERNEL_PANEL_CLASS__ */
#endif
/* __HDR_CLASS__ */
#endif
}
/* End Classes ***************************************************************/

/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
