/******************************************************************************
Filename    : virtual.hpp
Author      : pry
Date        : 08/04/2017
Licence     : LGPL v3+; see COPYING for details.
Description : The header for the virtual machine processes.
******************************************************************************/

/* Defines *******************************************************************/
namespace RVM_GEN
{
#ifdef __HDR_DEFS__
#ifndef __VIRTUAL_HPP_DEFS__
#define __VIRTUAL_HPP_DEFS__
/*****************************************************************************/

/*****************************************************************************/
/* __VIRTUAL_HPP_DEFS__ */
#endif
/* __HDR_DEFS__ */
#endif
/* End Defines ***************************************************************/

/* Classes *******************************************************************/
#ifdef __HDR_CLASSES__
#ifndef __VIRTUAL_HPP_CLASSES__
#define __VIRTUAL_HPP_CLASSES__
/*****************************************************************************/
/* Virtual machine info - VM is a special process type */
class Virtual:public Process
{
public:
    /* Name and root folder of the guest operating system */
    std::string Guest_Type;
    std::string Guest_Root;
    /* Configuration header output folder */
    std::string Config_Header_Output;

    /* The stack size for interrupt thread and user thread */
    ptr_t Stack_Size;
    /* The priority of the virtual machine */
    ptr_t Priority;
    /* The number of slices for timer interrupts */
    ptr_t Timeslice;
    /* The period for this virtual machine */
    ptr_t Period;
    /* The watchdog timer interval, in slices */
    ptr_t Watchdog;
    /* The maximum number of virtual interrupt vectors */
    ptr_t Vect_Num;

    /* void */ Virtual(xml_node_t* Root);
};
/*****************************************************************************/
/* __VIRTUAL_HPP_CLASSES__ */
#endif
/* __HDR_CLASSES__ */
#endif
}
/* End Classes ***************************************************************/

/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/