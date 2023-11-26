/******************************************************************************
Filename    : about_dialog.hpp
Author      : pry
Date        : 08/02/2022
License     : Proprietary; confidential.
Description : About information dialog class file header.
******************************************************************************/

/* Define ********************************************************************/
#ifdef __HDR_DEF__
#ifndef __ABOUT_DIALOG_TYPE__
#define __ABOUT_DIALOG_TYPE__
/*****************************************************************************/

/*****************************************************************************/
/* __ABOUT_DIALOG_TYPE__ */
#endif
/* __HDR_DEF__ */
#endif

namespace RVM_CFG
{
#ifdef __HDR_DEF__
#ifndef __ABOUT_DIALOG_DEF__
#define __ABOUT_DIALOG_DEF__
/*****************************************************************************/

/*****************************************************************************/
/* __ABOUT_DIALOG_DEF__ */
#endif
/* __HDR_DEF__ */
#endif
/* End Define ****************************************************************/

/* Classes *******************************************************************/
#ifdef __HDR_CLASS__
#ifndef __ABOUT_DIALOG_CLASS__
#define __ABOUT_DIALOG_CLASS__
/*****************************************************************************/
class About_Dialog:public wxDialog
{
public:
    class wxBoxSizer* Border_Sizer;
    class wxBoxSizer* Main_Sizer;
    class wxBoxSizer* Content_Sizer;

    class wxStaticBitmap* Logo;
    class wxTextCtrl* Text;

    class wxButton* Confirm;

    /* void */ About_Dialog(class wxWindow* Parent);
    /* void */ ~About_Dialog(void);

    void On_Confirm(class wxCommandEvent& Event);
};
/*****************************************************************************/
/* __ABOUT_DIALOG_CLASS__ */
#endif
/* __HDR_CLASS__ */
#endif
}
/* End Classes ***************************************************************/

/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
