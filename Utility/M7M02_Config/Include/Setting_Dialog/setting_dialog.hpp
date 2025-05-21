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

    class wxStaticText* RVM_Label;
    class wxBoxSizer* RVM_Sizer;
    class wxTextCtrl* RVM_Text;

    class wxStaticText* RME_Label;
    class wxBoxSizer* RME_Sizer;
    class wxTextCtrl* RME_Text;

    class wxStaticText* RMP_Label;
    class wxBoxSizer* RMP_Sizer;
    class wxTextCtrl* RMP_Text;

    class wxStaticBoxSizer* Required_Path_Sizer;
    class wxStaticBoxSizer* Optional_Path_Sizer;

    class wxButton* RVM_Path;
    class wxButton* RME_Path;
    class wxButton* RMP_Path;
    class wxButton* Confirm;
    class wxButton* Cancel;
    class wxButton* Restore;
    class wxButton* Default;
    class wxBoxSizer* Button_Sizer;

    Setting_Dialog(class wxWindow* Parent);
    ~Setting_Dialog(void);

    void On_Close(class wxCloseEvent& Event);
    void On_Confirm(class wxCommandEvent& Event);
    void On_Cancel(class wxCommandEvent& Event);
    void On_Restore(class wxCommandEvent& Event);
    void On_Default(class wxCommandEvent& Event);
    void On_RVM_Path(class wxCommandEvent& Event);
    void On_RME_Path(class wxCommandEvent& Event);
    void On_RMP_Path(class wxCommandEvent& Event);
    void Set_Setting(const std::string& rvm,const std::string& rme,const std::string& rmp);
    std::string Rel_Path_Get(void);

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
