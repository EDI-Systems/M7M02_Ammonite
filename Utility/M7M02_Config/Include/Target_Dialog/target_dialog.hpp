/******************************************************************************
Filename    : target_dialog.hpp
Author      : lbc
Date        : 23/04/2025
License     : Proprietary; confidential.
Description : Target dialog class header.
******************************************************************************/

/* Define ********************************************************************/
#ifdef __HDR_DEF__
#ifndef __TARGET_DIALOG_TYPE__
#define __TARGET_DIALOG_TYPE__
/*****************************************************************************/

/*****************************************************************************/
/* __TARGET_DIALOG_TYPE__ */
#endif
/* __HDR_DEF__ */
#endif

namespace RVM_CFG
{
#ifdef __HDR_DEF__
#ifndef __TARGET_DIALOG_DEF__
#define __TARGET_DIALOG_DEF__
/*****************************************************************************/

/*****************************************************************************/
/* __TARGET_DIALOG_DEF__ */
#endif
/* __HDR_DEF__ */
#endif
/* End Define ****************************************************************/

/* Classes *******************************************************************/
#ifdef __HDR_CLASS__
#ifndef __TARGET_DIALOG_CLASS__
#define __TARGET_DIALOG_CLASS__
/*****************************************************************************/
class Target_Dialog:public wxDialog
{
public:
    /* Path of the newly created file */
    std::string RVP_File_Path;

    class wxStaticBoxSizer* Target_Sizer;

    class wxBoxSizer* Main_Sizer;
    class wxBoxSizer* Platform_Sizer;
    class wxBoxSizer* Chipclass_Sizer;;
    class wxBoxSizer* Chipname_Sizer;
    class wxBoxSizer* Button_Sizer;

    class wxChoice* Platform;
    class wxStaticText* Platform_Label;
    class wxChoice* Chipclass;
    class wxStaticText* Chipclass_Label;
    class wxChoice* Chipname;
    class wxStaticText* Chipname_Label;

    class wxButton* Confirm;
    class wxButton* Cancel;

    /* void */ Target_Dialog(class wxWindow* Parent);
    /* void */ ~Target_Dialog(void);

    void Load(void);

    void On_Close(class wxCloseEvent& Event);
    void On_Confirm(class wxCommandEvent& Event);
    void On_Cancel(class wxCommandEvent& Event);
    void On_Platform(class wxCommandEvent& Event);
    void On_Chipclass(class wxCommandEvent& Event);
};
/*****************************************************************************/
/* __TARGET_DIALOG_CLASS__ */
#endif
/* __HDR_CLASS__ */
#endif
}
/* End Classes ***************************************************************/

/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
