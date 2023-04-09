/******************************************************************************
Filename    : native.hpp
Author      : pry
Date        : 08/04/2017
Licence     : LGPL v3+; see COPYING for details.
Description : The header for the native processes.
******************************************************************************/

/* Defines *******************************************************************/
namespace RVM_GEN
{
#ifdef __HDR_DEFS__
#ifndef __NATIVE_HPP_DEFS__
#define __NATIVE_HPP_DEFS__
/*****************************************************************************/

/*****************************************************************************/
/* __NATIVE_HPP_DEFS__ */
#endif
/* __HDR_DEFS__ */
#endif
/* End Defines ***************************************************************/

/* Classes *******************************************************************/
#ifdef __HDR_CLASSES__
#ifndef __NATIVE_HPP_CLASSES__
#define __NATIVE_HPP_CLASSES__
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
/* __NATIVE_HPP_CLASSES__ */
#endif
/* __HDR_CLASSES__ */
#endif
}
/* End Classes ***************************************************************/

/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
