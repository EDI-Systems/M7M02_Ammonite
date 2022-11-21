/******************************************************************************
Filename    : kernel.hpp
Author      : pry
Date        : 08/04/2017
Licence     : LGPL v3+; see COPYING for details.
Description : The header for the process.
******************************************************************************/

/* Defines *******************************************************************/
namespace RVM_GEN
{
#ifdef __HDR_DEFS__
#ifndef __KERNEL_HPP_DEFS__
#define __KERNEL_HPP_DEFS__
/*****************************************************************************/

/*****************************************************************************/
/* __KERNEL_HPP_DEFS__ */
#endif
/* __HDR_DEFS__ */
#endif
/* End Defines ***************************************************************/

/* Classes *******************************************************************/
#ifdef __HDR_CLASSES__
#ifndef __KERNEL_HPP_CLASSES__
#define __KERNEL_HPP_CLASSES__
/*****************************************************************************/
/* Kernel information */
class Kernel
{
public:
    /* RME code section start address */
    ptr_t Code_Base;
    /* RME code section size */
    ptr_t Code_Size;
    /* RME data section start address */
    ptr_t Data_Base;
    /* RME data section size */
    ptr_t Data_Size;
    /* RME kernel stack size */
    ptr_t Stack_Size;
    /* Extra amount of kernel memory */
    ptr_t Extra_Kmem;
    /* Slot order of kernel memory */
    ptr_t Kmem_Order;
    /* Priorities supported */
    ptr_t Kern_Prio;

    /* Build system to use */
    std::string Build;
    /* Toolchain to use for that build system */
    std::string Toolchain;
    /* Compiler optimization level */
    std::string Optimization;
    /* Kernel source root folder for reference */
    std::string Kernel_Root;
    /* Project output folder/overwrite */
    std::string Project_Output;
    ptr_t Project_Overwrite;
    /* Linker output folder */
    std::string Linker_Output;
    /* Configuration header output folder */
    std::string Config_Header_Output;
    /* Boot header/source output folder */
    std::string Boot_Header_Output;
    std::string Boot_Source_Output;
    /* Kernel initialization source folder/overwrite */
    std::string Init_Source_Output;
    ptr_t Init_Source_Overwrite;
    /* Interrupt handler source folder/overwrite */
    std::string Handler_Source_Output;
    ptr_t Handler_Source_Overwrite;

    /* void */ Kernel(xml_node_t* Root);
};
/*****************************************************************************/
/* __KERNEL_HPP_CLASSES__ */
#endif
/* __HDR_CLASSES__ */
#endif
}
/* End Classes ***************************************************************/

/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
