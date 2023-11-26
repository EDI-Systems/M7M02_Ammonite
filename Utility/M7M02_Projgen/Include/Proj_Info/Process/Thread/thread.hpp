/******************************************************************************
Filename    : thread.hpp
Author      : pry
Date        : 16/07/2019
Licence     : LGPL v3+; see COPYING for details.
Description : The header for the thread class.
******************************************************************************/

/* Define ********************************************************************/
namespace RVM_GEN
{
#ifdef __HDR_DEF__
#ifndef __THREAD_DEF__
#define __THREAD_DEF__
/*****************************************************************************/

/*****************************************************************************/
/* __THREAD_DEF__ */
#endif
/* __HDR_DEF__ */
#endif
/* End Define ****************************************************************/

/* Classes *******************************************************************/
#ifdef __HDR_CLASS__
#ifndef __THREAD_CLASS__
#define __THREAD_CLASS__
/*****************************************************************************/
/* Thread information */
class Thread:public Kobj
{
public:
    /* Whether this is hypervisor managed */
    ptr_t Is_Hyp;
    /* The stack size of the thread */
    ptr_t Stack_Size;
    /* The parameter passed to the thread */
    ptr_t Parameter;
    /* The priority of the thread */
    ptr_t Priority;

    /* Header entry address slot index */
    ptr_t Desc_Slot;
    /* The address of the stack */
    ptr_t Stack_Base;

    /* void */ Thread(xml_node_t* Root, class Process* Owner);
    /* void */ Thread(const std::string& Name,
                      ptr_t Is_Hyp, ptr_t Stack_Size, ptr_t Parameter, ptr_t Priority,
                      class Process* Owner);
};
/*****************************************************************************/
/* __THREAD_CLASS__ */
#endif
/* __HDR_CLASS__ */
#endif
}
/* End Classes ***************************************************************/

/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
