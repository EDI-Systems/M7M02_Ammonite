/******************************************************************************
Filename    : option_notebook.hpp
Author      : pry
Date        : 11/01/2023
License     : Proprietary; confidential.
Description : Option notebook file header.
******************************************************************************/

/* Defines *******************************************************************/
#ifdef __HDR_DEFS__
#ifndef __OPTION_NOTEBOOK_HPP_TYPES__
#define __OPTION_NOTEBOOK_HPP_TYPES__
/*****************************************************************************/

/*****************************************************************************/
/* __OPTION_NOTEBOOK_HPP_TYPES__ */
#endif
/* __HDR_DEFS__ */
#endif

namespace RVM_CFG
{
#ifdef __HDR_DEFS__
#ifndef __OPTION_NOTEBOOK_HPP_DEFS__
#define __OPTION_NOTEBOOK_HPP_DEFS__
/*****************************************************************************/

/*****************************************************************************/
/* __OPTION_NOTEBOOK_HPP_DEFS__ */
#endif
/* __HDR_DEFS__ */
#endif
/* End Defines ***************************************************************/

/* Classes *******************************************************************/
#ifdef __HDR_CLASSES__
#ifndef __OPTION_NOTEBOOK_HPP_CLASSES__
#define __OPTION_NOTEBOOK_HPP_CLASSES__
/*****************************************************************************/
class Option_Notebook:public wxAuiNotebook
{
public:

    /* void */ Option_Notebook(class wxWindow* Parent);
    /* void */ ~Option_Notebook(void);

    void State_Set(ptr_t Type);

    void On_Sheet_Prechange(class wxAuiNotebookEvent& Event);
    void On_Sheet_Change(class wxAuiNotebookEvent& Event);
};
/*****************************************************************************/
/* __OPTION_NOTEBOOK_HPP_CLASSES__ */
#endif
/* __HDR_CLASSES__ */
#endif
}
/* End Classes ***************************************************************/

/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
