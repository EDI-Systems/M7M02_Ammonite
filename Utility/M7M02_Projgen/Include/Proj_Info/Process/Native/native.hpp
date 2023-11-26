/******************************************************************************
Filename    : native.hpp
Author      : pry
Date        : 08/04/2017
Licence     : LGPL v3+; see COPYING for details.
Description : The header for the native processes.
******************************************************************************/

/* Define ********************************************************************/
namespace RVM_GEN
{
#ifdef __HDR_DEF__
#ifndef __NATIVE_DEF__
#define __NATIVE_DEF__
/*****************************************************************************/

/*****************************************************************************/
/* __NATIVE_DEF__ */
#endif
/* __HDR_DEF__ */
#endif
/* End Define ****************************************************************/

/* Classes *******************************************************************/
#ifdef __HDR_CLASS__
#ifndef __NATIVE_CLASS__
#define __NATIVE_CLASS__
/*****************************************************************************/
/* Native process type */
class Native:public Process
{
public:
    /* Entry source folder & overwrite */
    std::string Entry_Source_Output;
    ptr_t Entry_Source_Overwrite;

    /* void */ Native(xml_node_t* Root);
};
/*****************************************************************************/
/* __NATIVE_CLASS__ */
#endif
/* __HDR_CLASS__ */
#endif
}
/* End Classes ***************************************************************/

/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
