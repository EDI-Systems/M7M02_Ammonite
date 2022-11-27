/******************************************************************************
Filename    : kfunc.hpp
Author      : pry
Date        : 16/07/2019
Licence     : LGPL v3+; see COPYING for details.
Description : The header for the kernel function class.
******************************************************************************/

/* Defines *******************************************************************/
namespace RVM_GEN
{
#ifdef __HDR_DEFS__
#ifndef __KFUNC_HPP_DEFS__
#define __KFUNC_HPP_DEFS__
/*****************************************************************************/

/*****************************************************************************/
/* __KFUNC_HPP_DEFS__ */
#endif
/* __HDR_DEFS__ */
#endif
/* End Defines ***************************************************************/

/* Classes *******************************************************************/
#ifdef __HDR_CLASSES__
#ifndef __KFUNC_HPP_CLASSES__
#define __KFUNC_HPP_CLASSES__
/*****************************************************************************/
/* Kernel function information */
class Kfunc:public Kobj
{
public:
    /* The starting number of this kernel function */
    ptr_t Start;
    /* The ending number of this kernel function */
    ptr_t End;

    /* void */ Kfunc(xml_node_t* Root, class Process* Owner);
};
/*****************************************************************************/
/* __KFUNC_HPP_CLASSES__ */
#endif
/* __HDR_CLASSES__ */
#endif
}
/* End Classes ***************************************************************/

/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
