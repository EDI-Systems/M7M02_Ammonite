/******************************************************************************
Filename    : invocation.hpp
Author      : pry
Date        : 16/07/2019
Licence     : LGPL v3+; see COPYING for details.
Description : The header for the invocation class.
******************************************************************************/

/* Define ********************************************************************/
namespace RVM_GEN
{
#ifdef __HDR_DEF__
#ifndef __INVOCATION_DEF__
#define __INVOCATION_DEF__
/*****************************************************************************/
    
/*****************************************************************************/
/* __INVOCATION_DEF__ */
#endif
/* __HDR_DEF__ */
#endif
/* End Define ****************************************************************/

/* Classes *******************************************************************/
#ifdef __HDR_CLASS__
#ifndef __INVOCATION_CLASS__
#define __INVOCATION_CLASS__
/*****************************************************************************/
/* Invocation information */
class Invocation:public Kobj
{
public:
    /* The stack size of the invocation */
    ptr_t Stack_Size;

    /* Header entry address slot index */
    ptr_t Desc_Slot;
    /* The address of the stack */
    ptr_t Stack_Base;
 
    /* void */ Invocation(xml_node_t* Root, class Process* Owner);

    virtual std::string Report(void) final override;
};
/*****************************************************************************/
/* __INVOCATION_CLASS__ */
#endif
/* __HDR_CLASS__ */
#endif
}
/* End Classes ***************************************************************/

/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
