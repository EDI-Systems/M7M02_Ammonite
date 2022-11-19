/******************************************************************************
Filename    : rme_kern.hpp
Author      : pry
Date        : 16/07/2019
Licence     : LGPL v3+; see COPYING for details.
Description : The header for the kernel function class.
******************************************************************************/

/* Defines *******************************************************************/
namespace RVM_GEN
{
#ifdef __HDR_DEFS__
#ifndef __RME_KERN_HPP_DEFS__
#define __RME_KERN_HPP_DEFS__
/*****************************************************************************/
    
/*****************************************************************************/
/* __RME_KERN_HPP_DEFS__ */
#endif
/* __HDR_DEFS__ */
#endif
/* End Defines ***************************************************************/

/* Classes *******************************************************************/
#ifdef __HDR_CLASSES__
#ifndef __RME_KERN_HPP_CLASSES__
#define __RME_KERN_HPP_CLASSES__
/*****************************************************************************/
/* Receive endpoint information */
class Kern:public Kobj
{
public:
    /* The starting number of this kernel function */
    ptr_t Start;
    /* The ending number of this kernel function */
    ptr_t End;

    Kern(xml_node_t* Node);
};
/*****************************************************************************/
/* __RME_KERN_HPP_CLASSES__ */
#endif
/* __HDR_CLASSES__ */
#endif
}
/* End Classes ***************************************************************/

/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
