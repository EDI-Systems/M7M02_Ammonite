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
/* The priority of the vector handling thread is always 3 on all architectures */
#define VIRT_VECTOR_PRIO        (3)
/* The priority of the user thread is always 2 on all architectures */
#define VIRT_USER_PRIO          (2)
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
    /* Virtual header output folder & overwrite */
    std::string Virtual_Header_Output;
    ptr_t Virtual_Header_Overwrite;
    /* Virtual source output folder & overwrite */
    std::string Virtual_Source_Output;
    ptr_t Virtual_Source_Overwrite;

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

    /* Register set memory */
    ptr_t Reg_Base;
    ptr_t Reg_Size;
    /* Parameter memory */
    ptr_t Param_Base;
    ptr_t Param_Size;
    /* Vector flag memory */
    ptr_t Vctf_Base;
    ptr_t Vctf_Size;

    std::vector<std::string> Virtual_Include;
    std::vector<std::string> Virtual_Source;

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
