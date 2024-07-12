/******************************************************************************
Filename    : kernel.hpp
Author      : pry
Date        : 08/04/2017
Licence     : LGPL v3+; see COPYING for details.
Description : The header for the process.
******************************************************************************/

/* Define ********************************************************************/
namespace RVM_GEN
{
#ifdef __HDR_DEF__
#ifndef __KERNEL_DEF__
#define __KERNEL_DEF__
/*****************************************************************************/

/*****************************************************************************/
/* __KERNEL_DEF__ */
#endif
/* __HDR_DEF__ */
#endif
/* End Define ****************************************************************/

/* Classes *******************************************************************/
#ifdef __HDR_CLASS__
#ifndef __KERNEL_CLASS__
#define __KERNEL_CLASS__
/*****************************************************************************/
/* Kernel information */
class Kernel
{
public:
    /* RME code section base/size */
    ptr_t Code_Base;
    ptr_t Code_Size;
    std::unique_ptr<class Mem_Info> Code;
    /* RME data section base/size */
    ptr_t Data_Base;
    ptr_t Data_Size;
    std::unique_ptr<class Mem_Info> Data;
    /* RME kernel stack size */
    ptr_t Stack_Size;
    /* Extra amount of kernel memory */
    ptr_t Extra_Kom;
    /* Slot order of kernel memory */
    ptr_t Kom_Order;
    /* Priorities supported */
    ptr_t Kern_Prio;

    /* Build system to use */
    std::string Buildsystem;
    /* Toolchain to use for that build system */
    std::string Toolchain;
    /* Compiler optimization level */
    std::string Optimization;
    /* Whether the kernel project will generate a full image (process partitions included) */
    ptr_t Full_Image;
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
    /* Initialization hook source folder/overwrite */
    std::string Hook_Source_Output;
    ptr_t Hook_Source_Overwrite;
    /* Interrupt handler source folder/overwrite */
    std::string Handler_Source_Output;
    ptr_t Handler_Source_Overwrite;

    /* Kernel memory */
    ptr_t Kom_Base;
    ptr_t Kom_Size;
    /* Kernel stack */
    ptr_t Stack_Base;
    /* Vector flag section */
    ptr_t Vctf_Base;
    ptr_t Vctf_Size;
    /* Event flag section */
    ptr_t Evtf_Base;
    ptr_t Evtf_Size;
    /* Initial state for vector creation */
    ptr_t Vct_Cap_Front;
    ptr_t Vct_Kom_Front;

    /* Linker filename */
    std::string Linker_Filename;
    /* Project filename */
    std::string Project_Filename;

    /* void */ Kernel(xml_node_t* Root);

    void Mem_Alloc(ptr_t Kom_Front, ptr_t Vector_Num, ptr_t Event_Num, ptr_t Wordlength);
};
/*****************************************************************************/
/* __KERNEL_CLASS__ */
#endif
/* __HDR_CLASS__ */
#endif
}
/* End Classes ***************************************************************/

/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
