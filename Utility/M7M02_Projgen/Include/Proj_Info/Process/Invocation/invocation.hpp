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
class Invocation
{
public:
    /* Name */
    std::string Name;
    /* The stack size of the invocation */
    ptr_t Stack_Size;
 
    /* void */ Invocation(xml_node_t* Root);
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