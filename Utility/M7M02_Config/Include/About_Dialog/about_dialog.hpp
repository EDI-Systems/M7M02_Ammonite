/******************************************************************************
Filename    : about_dialog.hpp
Author      : pry
Date        : 08/02/2022
License     : Proprietary; confidential.
Description : About information dialog class file header.
******************************************************************************/

/* Defines *******************************************************************/
#ifdef __HDR_DEFS__
#ifndef __ABOUT_DIALOG_HPP_TYPES__
#define __ABOUT_DIALOG_HPP_TYPES__
/*****************************************************************************/

/*****************************************************************************/
/* __ABOUT_DIALOG_HPP_TYPES__ */
#endif
/* __HDR_DEFS__ */
#endif

namespace RVM_CFG
{
#ifdef __HDR_DEFS__
#ifndef __ABOUT_DIALOG_HPP_DEFS__
#define __ABOUT_DIALOG_HPP_DEFS__
/*****************************************************************************/

/*****************************************************************************/
/* __ABOUT_DIALOG_HPP_DEFS__ */
#endif
/* __HDR_DEFS__ */
#endif
/* End Defines ***************************************************************/

/* Classes *******************************************************************/
#ifdef __HDR_CLASSES__
#ifndef __ABOUT_DIALOG_HPP_CLASSES__
#define __ABOUT_DIALOG_HPP_CLASSES__
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
/* __ABOUT_DIALOG_HPP_CLASSES__ */
#endif
/* __HDR_CLASSES__ */
#endif
}
/* End Classes ***************************************************************/

/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
