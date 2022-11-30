/******************************************************************************
Filename    : invocation.hpp
Author      : pry
Date        : 16/07/2019
Licence     : LGPL v3+; see COPYING for details.
Description : The header for the invocation class.
******************************************************************************/

/* Defines *******************************************************************/
namespace RVM_GEN
{
#ifdef __HDR_DEFS__
#ifndef __INVOCATION_HPP_DEFS__
#define __INVOCATION_HPP_DEFS__
/*****************************************************************************/
    
/*****************************************************************************/
/* __INVOCATION_HPP_DEFS__ */
#endif
/* __HDR_DEFS__ */
#endif
/* End Defines ***************************************************************/

/* Classes *******************************************************************/
#ifdef __HDR_CLASSES__
#ifndef __INVOCATION_HPP_CLASSES__
#define __INVOCATION_HPP_CLASSES__
/*****************************************************************************/
/* Invocation information */
class Invocation:public Kobj
{
public:
    /* The stack size of the invocation */
    ptr_t Stack_Size;

    /* Position of the entry address from the header */
    ptr_t Entry_Addr;
    /* The address of the stack */
    ptr_t Stack_Base;
 
    /* void */ Invocation(xml_node_t* Root, class Process* Owner);
};
/*****************************************************************************/
/* __INVOCATION_HPP_CLASSES__ */
#endif
/* __HDR_CLASSES__ */
#endif
}
/* End Classes ***************************************************************/

/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
