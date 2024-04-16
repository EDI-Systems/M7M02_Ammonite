/******************************************************************************
Filename    : kfunc.hpp
Author      : pry
Date        : 16/07/2019
Licence     : LGPL v3+; see COPYING for details.
Description : The header for the kernel function class.
******************************************************************************/

/* Define ********************************************************************/
namespace RVM_GEN
{
#ifdef __HDR_DEF__
#ifndef __KFUNC_DEF__
#define __KFUNC_DEF__
/*****************************************************************************/

/*****************************************************************************/
/* __KFUNC_DEF__ */
#endif
/* __HDR_DEF__ */
#endif
/* End Define ****************************************************************/

/* Classes *******************************************************************/
#ifdef __HDR_CLASS__
#ifndef __KFUNC_CLASS__
#define __KFUNC_CLASS__
/*****************************************************************************/
/* Kernel function information */
class Kfunc:public Kobj
{
public:
    /* The beginning number of this kernel function */
    ptr_t Begin;
    /* The ending number of this kernel function - inclusive*/
    ptr_t End;

    /* void */ Kfunc(xml_node_t* Root, class Process* Owner);
};
/*****************************************************************************/
/* __KFUNC_CLASS__ */
#endif
/* __HDR_CLASS__ */
#endif
}
/* End Classes ***************************************************************/

/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
