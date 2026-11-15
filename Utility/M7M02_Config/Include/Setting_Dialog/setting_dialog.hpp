/******************************************************************************
Filename    : setting dialog.hpp
Author      : lbc
Date        : 22/04/2025
License     : Proprietary; confidential.
Description : Setting dialog class header.
******************************************************************************/

/* Define ********************************************************************/
#ifdef __HDR_DEF__
#ifndef __SETTING_DIALOG_TYPE__
#define __SETTING_DIALOG_TYPE__
/*****************************************************************************/

/*****************************************************************************/
/* __SETTING_DIALOG_TYPE__ */
#endif
/* __HDR_DEF__ */
#endif

namespace RVM_CFG
{
#ifdef __HDR_DEF__
#ifndef __SETTING_DIALOG_DEF__
#define __SETTING_DIALOG_DEF__
/*****************************************************************************/
/* My define */
#define TEST _("TEST")
/* Default paths */
#define RME_DEFAULT "./../../../M7M01_Eukaron/"
#define RVM_DEFAULT "./../../../M7M02_Ammonite/"
#define RMP_DEFAULT "./../../../M5P01_Prokaron/"
#define FRT_DEFAULT "./../../../FreeRTOS_V900/"
#define RTT_DEFAULT "./../../../RTThread_V411/"
#define UO2_DEFAULT "./../../../uCOSII_V293/"
#define UO3_DEFAULT "./../../../uCOSIII_V308/"
#define CHB_DEFAULT "./../../../ChibiOS_V2111/"
#define RTE_DEFAULT "./../../../RTEMS_V701/"
#define ZPR_DEFAULT "./../../../Zephyr_V370/"
#define NTX_DEFAULT "./../../../NuttX_V131/"
#define DJY_DEFAULT "./../../../DjyOS_V110/"
#define KLT_DEFAULT "./../../../KLite_V521/"
/*****************************************************************************/
/* __SETTING_DIALOG_DEF__ */
#endif
/* __HDR_DEF__ */
#endif
/* End Define ****************************************************************/

/* Classes *******************************************************************/
#ifdef __HDR_CLASS__
#ifndef __SETTING_DIALOG_CLASS__
#define __SETTING_DIALOG_CLASS__
/*****************************************************************************/
class Setting_Dialog:public wxDialog
{
public:
    class wxBoxSizer* Main_Sizer;

    class wxStaticBoxSizer* Preference_Sizer;
    class wxBoxSizer* Preference_Line1_Sizer;
    class wxCheckBox* Generate_Report;
    class wxCheckBox* Open_Report;

    class wxStaticBoxSizer* Core_Component_Sizer;

    class wxStaticText* RME_Label;
    class wxBoxSizer* RME_Sizer;
    class wxDirPickerCtrl* RME_Picker;

    class wxStaticText* RVM_Label;
    class wxBoxSizer* RVM_Sizer;
    class wxDirPickerCtrl* RVM_Picker;

    class wxStaticBoxSizer* Guest_OS_Sizer;

    class wxStaticText* RMP_Label;
    class wxBoxSizer* RMP_Sizer;
    class wxDirPickerCtrl* RMP_Picker;

    class wxStaticText* FRT_Label;
    class wxBoxSizer* FRT_Sizer;
    class wxDirPickerCtrl* FRT_Picker;

    class wxStaticText* RTT_Label;
    class wxBoxSizer* RTT_Sizer;
    class wxDirPickerCtrl* RTT_Picker;

    class wxStaticText* UO2_Label;
    class wxBoxSizer* UO2_Sizer;
    class wxDirPickerCtrl* UO2_Picker;

    class wxStaticText* UO3_Label;
    class wxBoxSizer* UO3_Sizer;
    class wxDirPickerCtrl* UO3_Picker;

    class wxStaticText* CHB_Label;
    class wxBoxSizer* CHB_Sizer;
    class wxDirPickerCtrl* CHB_Picker;

    class wxStaticText* RTE_Label;
    class wxBoxSizer* RTE_Sizer;
    class wxDirPickerCtrl* RTE_Picker;

    class wxStaticText* ZPR_Label;
    class wxBoxSizer* ZPR_Sizer;
    class wxDirPickerCtrl* ZPR_Picker;

    class wxStaticText* NTX_Label;
    class wxBoxSizer* NTX_Sizer;
    class wxDirPickerCtrl* NTX_Picker;

    class wxStaticText* DJY_Label;
    class wxBoxSizer* DJY_Sizer;
    class wxDirPickerCtrl* DJY_Picker;

    class wxStaticText* KLT_Label;
    class wxBoxSizer* KLT_Sizer;
    class wxDirPickerCtrl* KLT_Picker;

    class wxBoxSizer* Button_Sizer;
    class wxButton* Confirm;
    class wxButton* Cancel;
    class wxButton* Restore;
    class wxButton* Default;

    Setting_Dialog(class wxWindow* Parent);
    ~Setting_Dialog(void);

    void File_Load(void);
    void File_Save(void);

    void Load(void);
    void Save(void);

    void On_Generate_Report(class wxCommandEvent& Event);
    void On_Dir_Picker(class wxFileDirPickerEvent& Event);
    void On_Close(class wxCloseEvent& Event);
    void On_Confirm(class wxCommandEvent& Event);
    void On_Cancel(class wxCommandEvent& Event);
    void On_Restore(class wxCommandEvent& Event);
    void On_Default(class wxCommandEvent& Event);
};
/*****************************************************************************/
/* __SETTING_DIALOG_CLASS__ */
#endif
/* __HDR_CLASS__ */
#endif
}
/* End Classes ***************************************************************/

/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
