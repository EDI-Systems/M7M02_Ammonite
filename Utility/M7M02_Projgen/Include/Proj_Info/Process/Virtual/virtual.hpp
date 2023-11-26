/******************************************************************************
Filename    : virtual.hpp
Author      : pry
Date        : 08/04/2017
Licence     : LGPL v3+; see COPYING for details.
Description : The header for the virtual machine processes.
******************************************************************************/

/* Define ********************************************************************/
namespace RVM_GEN
{
#ifdef __HDR_DEF__
#ifndef __VIRTUAL_DEF__
#define __VIRTUAL_DEF__
/*****************************************************************************/
/* The priority of the vector handling thread is always 3 on all architectures */
#define VIRT_VECTOR_PRIO        (3)
/* The priority of the user thread is always 2 on all architectures */
#define VIRT_USER_PRIO          (2)
/*****************************************************************************/
/* __VIRTUAL_DEF__ */
#endif
/* __HDR_DEF__ */
#endif
/* End Define ****************************************************************/

/* Classes *******************************************************************/
#ifdef __HDR_CLASS__
#ifndef __VIRTUAL_CLASS__
#define __VIRTUAL_CLASS__
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
    ptr_t Vector_Stack_Size;
    ptr_t User_Stack_Size;
    /* The priority of the virtual machine */
    ptr_t Priority;
    /* The number of slices for timer interrupts */
    ptr_t Timeslice;
    /* The period for this virtual machine */
    ptr_t Period;
    /* The watchdog timer interval, in slices */
    ptr_t Watchdog;
    /* The maximum number of virtual interrupt vectors */
    ptr_t Vector_Num;

    /* Register set memory */
    ptr_t Reg_Base;
    ptr_t Reg_Size;
    /* State block memory */
    ptr_t State_Base;
    ptr_t State_Size;

    std::vector<std::string> Virtual_Include;
    std::vector<std::string> Virtual_Source;

    /* void */ Virtual(xml_node_t* Root);

    static ptr_t State_Alloc(ptr_t Source, ptr_t Wordlength, ptr_t Kom_Order);
};
/*****************************************************************************/
/* __VIRTUAL_CLASS__ */
#endif
/* __HDR_CLASS__ */
#endif
}
/* End Classes ***************************************************************/

/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
