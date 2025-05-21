/******************************************************************************
Filename    : choose dialog.hpp
Author      : lbc
Date        : 10/05/2025
License     : Proprietary; confidential.
Description : Name or Rename dialog class header.
******************************************************************************/

/* Define ********************************************************************/
#ifdef __HDR_DEF__
#ifndef __NAME_DIALOG_TYPE__
#define __NAME_DIALOG_TYPE__
/*****************************************************************************/

/*****************************************************************************/
/* __NAME_DIALOG_TYPE__ */
#endif
/* __HDR_DEF__ */
#endif

namespace RVM_CFG
{
#ifdef __HDR_DEF__
#ifndef __NAME_DIALOG_DEF__
#define __NAME_DIALOG_DEF__
/*****************************************************************************/
/* My define */
#define TEST _("TEST")
/*****************************************************************************/
/* __NAME_DIALOG_DEF__ */
#endif
/* __HDR_DEF__ */
#endif
/* End Define ****************************************************************/

/* Classes *******************************************************************/
#ifdef __HDR_CLASS__
#ifndef __NAME_DIALOG_CLASS__
#define __NAME_DIALOG_CLASS__
/*****************************************************************************/
class Name_Dialog:public wxDialog
{
public:
    class wxBoxSizer* Main_Sizer;
    class wxBoxSizer* Name_Sizer;
    class wxBoxSizer* Button_Sizer;

    class wxStaticText* Name_Label;
    class wxTextCtrl* Name;

    class wxButton* Confirm;
    class wxButton* Cancel;

    Name_Dialog(class wxWindow* Parent);
    ~Name_Dialog(void);

    void On_Close(class wxCloseEvent& Event);
    void On_Confirm(class wxCommandEvent& Event);
    void On_Cancel(class wxCommandEvent& Event);
    void Load(const std::string& Input);
    std::string Name_Get(void);
};
/*****************************************************************************/
/* __NAME_DIALOG_CLASS__ */
#endif
/* __HDR_CLASS__ */
#endif
}
/* End Classes ***************************************************************/

/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
