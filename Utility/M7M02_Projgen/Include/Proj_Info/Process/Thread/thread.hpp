/******************************************************************************
Filename    : thread.hpp
Author      : pry
Date        : 16/07/2019
Licence     : LGPL v3+; see COPYING for details.
Description : The header for the thread class.
******************************************************************************/

/* Defines *******************************************************************/
namespace RVM_GEN
{
#ifdef __HDR_DEFS__
#ifndef __THREAD_HPP_DEFS__
#define __THREAD_HPP_DEFS__
/*****************************************************************************/

/*****************************************************************************/
/* __THREAD_HPP_DEFS__ */
#endif
/* __HDR_DEFS__ */
#endif
/* End Defines ***************************************************************/

/* Classes *******************************************************************/
#ifdef __HDR_CLASSES__
#ifndef __THREAD_HPP_CLASSES__
#define __THREAD_HPP_CLASSES__
/*****************************************************************************/
/* Thread information */
class Thread:public Kobj
{
public:
    /* Name */
    std::string Name;
    /* The stack size of the thread */
    ptr_t Stack_Size;
    /* The parameter passed to the thread */
    ptr_t Parameter;
    /* The priority of the thread */
    ptr_t Priority;

    /* void */ Thread(xml_node_t* Root);
};
/*****************************************************************************/
/* __THREAD_HPP_CLASSES__ */
#endif
/* __HDR_CLASSES__ */
#endif
}
/* End Classes ***************************************************************/

/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
