/******************************************************************************
Filename    : output_notebook.hpp
Author      : pry
Date        : 11/01/2023
License     : Proprietary; confidential.
Description : Output notebook file header.
******************************************************************************/

/* Define ********************************************************************/
#ifdef __HDR_DEF__
#ifndef __OUTPUT_NOTEBOOK_TYPE__
#define __OUTPUT_NOTEBOOK_TYPE__
/*****************************************************************************/

/*****************************************************************************/
/* __OUTPUT_NOTEBOOK_TYPE__ */
#endif
/* __HDR_DEF__ */
#endif

namespace RVM_CFG
{
#ifdef __HDR_DEF__
#ifndef __OUTPUT_NOTEBOOK_DEF__
#define __OUTPUT_NOTEBOOK_DEF__
/*****************************************************************************/
#define OUTPUT_GENERATE              (0)
#define OUTPUT_PROCESS               (1)
#define OUTPUT_STATISTICS            (2)
/*****************************************************************************/
/* __OUTPUT_NOTEBOOK_DEF__ */
#endif
/* __HDR_DEF__ */
#endif
/* End Define ****************************************************************/

/* Classes *******************************************************************/
#ifdef __HDR_CLASS__
#ifndef __OUTPUT_NOTEBOOK_CLASS__
#define __OUTPUT_NOTEBOOK_CLASS__
/*****************************************************************************/
class Output_Notebook:public wxNotebook
{
public:
    class wxTextCtrl* Generate;
    class wxTextCtrl* Process;
    class wxTextCtrl* Statistics;

    /* void */ Output_Notebook(class wxWindow* Parent);
    /* void */ ~Output_Notebook(void);

    void State_Set(ptr_t Type);
};
/*****************************************************************************/
/* __OUTPUT_NOTEBOOK_CLASS__ */
#endif
/* __HDR_CLASS__ */
#endif
}
/* End Classes ***************************************************************/

/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
