/******************************************************************************
Filename    : option_notebook.hpp
Author      : pry
Date        : 11/01/2023
License     : Proprietary; confidential.
Description : Option notebook file header.
******************************************************************************/

/* Define ********************************************************************/
#ifdef __HDR_DEF__
#ifndef __OPTION_NOTEBOOK_TYPE__
#define __OPTION_NOTEBOOK_TYPE__
/*****************************************************************************/

/*****************************************************************************/
/* __OPTION_NOTEBOOK_TYPE__ */
#endif
/* __HDR_DEF__ */
#endif

namespace RVM_CFG
{
#ifdef __HDR_DEF__
#ifndef __OPTION_NOTEBOOK_DEF__
#define __OPTION_NOTEBOOK_DEF__
/*****************************************************************************/

/*****************************************************************************/
/* __OPTION_NOTEBOOK_DEF__ */
#endif
/* __HDR_DEF__ */
#endif
/* End Define ****************************************************************/

/* Classes *******************************************************************/
#ifdef __HDR_CLASS__
#ifndef __OPTION_NOTEBOOK_CLASS__
#define __OPTION_NOTEBOOK_CLASS__
/*****************************************************************************/
class Option_Notebook:public wxNotebook
{
public:

    /* void */ Option_Notebook(class wxWindow* Parent);
    /* void */ ~Option_Notebook(void);

    void State_Set(ptr_t Type);
};
/*****************************************************************************/
/* __OPTION_NOTEBOOK_CLASS__ */
#endif
/* __HDR_CLASS__ */
#endif
}
/* End Classes ***************************************************************/

/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
