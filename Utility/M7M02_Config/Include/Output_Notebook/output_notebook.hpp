/******************************************************************************
Filename    : output_notebook.hpp
Author      : pry
Date        : 11/01/2023
License     : Proprietary; confidential.
Description : Output notebook file header.
******************************************************************************/

/* Defines *******************************************************************/
#ifdef __HDR_DEFS__
#ifndef __OUTPUT_NOTEBOOK_HPP_TYPES__
#define __OUTPUT_NOTEBOOK_HPP_TYPES__
/*****************************************************************************/

/*****************************************************************************/
/* __OUTPUT_NOTEBOOK_HPP_TYPES__ */
#endif
/* __HDR_DEFS__ */
#endif

namespace RVM_CFG
{
#ifdef __HDR_DEFS__
#ifndef __OUTPUT_NOTEBOOK_HPP_DEFS__
#define __OUTPUT_NOTEBOOK_HPP_DEFS__
/*****************************************************************************/
#define OUTPUT_GENERATE              (0)
#define OUTPUT_PROCESS               (1)
#define OUTPUT_STATISTICS            (2)
/*****************************************************************************/
/* __OUTPUT_NOTEBOOK_HPP_DEFS__ */
#endif
/* __HDR_DEFS__ */
#endif
/* End Defines ***************************************************************/

/* Classes *******************************************************************/
#ifdef __HDR_CLASSES__
#ifndef __OUTPUT_NOTEBOOK_HPP_CLASSES__
#define __OUTPUT_NOTEBOOK_HPP_CLASSES__
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
/* __OUTPUT_NOTEBOOK_HPP_CLASSES__ */
#endif
/* __HDR_CLASSES__ */
#endif
}
/* End Classes ***************************************************************/

/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
