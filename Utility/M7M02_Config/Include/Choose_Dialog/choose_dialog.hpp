/******************************************************************************
Filename    : choose dialog.hpp
Author      : lbc
Date        : 23/04/2025
License     : Proprietary; confidential.
Description : Choose dialog class header.
******************************************************************************/

/* Define ********************************************************************/
#ifdef __HDR_DEF__
#ifndef __CHOOSE_DIALOG_TYPE__
#define __CHOOSE_DIALOG_TYPE__
/*****************************************************************************/

/*****************************************************************************/
/* __CHOOSE_DIALOG_TYPE__ */
#endif
/* __HDR_DEF__ */
#endif

namespace RVM_CFG
{
#ifdef __HDR_DEF__
#ifndef __CHOOSE_DIALOG_DEF__
#define __CHOOSE_DIALOG_DEF__
/*****************************************************************************/
/* My define */
#define TEST _("TEST")
/*****************************************************************************/
/* __CHOOSE_DIALOG_DEF__ */
#endif
/* __HDR_DEF__ */
#endif
/* End Define ****************************************************************/

/* Classes *******************************************************************/
#ifdef __HDR_CLASS__
#ifndef __CHOOSE_DIALOG_CLASS__
#define __CHOOSE_DIALOG_CLASS__
/*****************************************************************************/
class Choose_Dialog:public wxDialog
{
public:
    /* Path of the newly created file */
    std::string RVP_File_Path;

    class wxBoxSizer* Main_Sizer;
    class wxBoxSizer* Plat_Sizer;
    class wxBoxSizer* Chip_Sizer;;
    class wxBoxSizer* Type_Sizer;
    class wxBoxSizer* Button_Sizer;

    class wxStaticBoxSizer* Plat_And_Chip_Sizer;

    class wxChoice* Plat;
    class wxStaticText* Plat_Label;
    class wxChoice* Chip;
    class wxStaticText* Chip_Label;
    class wxChoice* Type;
    class wxStaticText* Type_Label;

    class wxButton* Confirm;
    class wxButton* Cancel;

    Choose_Dialog(class wxWindow* Parent);
    ~Choose_Dialog(void);

    void On_Close(class wxCloseEvent& Event);
    void On_Confirm(class wxCommandEvent& Event);
    void On_Cancel(class wxCommandEvent& Event);
    void On_Platform_Change(class wxCommandEvent& Event);
    void On_Chip_Change(class wxCommandEvent& Event);
    void Load(void);
};
/*****************************************************************************/
/* __CHOOSE_DIALOG_CLASS__ */
#endif
/* __HDR_CLASS__ */
#endif
}
/* End Classes ***************************************************************/

/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
