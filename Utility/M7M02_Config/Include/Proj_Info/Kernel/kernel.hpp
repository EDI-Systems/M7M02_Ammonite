/******************************************************************************
Filename    : kernel.hpp
Author      : pry
Date        : 13/01/2023
License     : Proprietary; confidential.
Description : Monitor class header.
******************************************************************************/

/* Defines *******************************************************************/
#ifdef __HDR_DEFS__
#ifndef __KERNEL_HPP_TYPES__
#define __KERNEL_HPP_TYPES__
/*****************************************************************************/

/*****************************************************************************/
/* __KERNEL_HPP_TYPES__ */
#endif
/* __HDR_DEFS__ */
#endif

namespace RVM_CFG
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
class Kernel
{
public:
    /* Kernel source root folder */
    std::string Kernel_Root;
    /* Code base & size */
    ptr_t Code_Base;
    ptr_t Code_Size;
    /* Data base & size */
    ptr_t Data_Base;
    ptr_t Data_Size;
    /* Stack size */
    ptr_t Stack_Size;
    /* Extra kernel memory */
    ptr_t Extra_Kom;
    /* Kernel memory allocation order of 2 */
    ptr_t Kom_Order;
    /* Number of kernel priorities */
    ptr_t Kern_Prio;
    /* Buildsystem */
    std::string Buildsystem;
    /* Toolchain */
    std::string Toolchain;
    /* Optimization */
    ptr_t Optimization;
    /* Whether to generate a full image */
    ptr_t Full_Image;
    /* Project output & overwrite */
    std::string Project_Output;
    ptr_t Project_Overwrite;
    /* Linker output */
    std::string Linker_Output;
    /* Config header output */
    std::string Config_Header_Output;
    /* Boot header/source output */
    std::string Boot_Header_Output;
    std::string Boot_Source_Output;
    /* Hook source & overwrite */
    std::string Hook_Source_Output;
    ptr_t Hook_Source_Overwrite;
    /* Handler source & overwrite */
    std::string Handler_Source_Output;
    ptr_t Handler_Source_Overwrite;

    /* void */ Kernel(const std::string& Kernel_Root,
                      const class Plat_Info* Plat,
                      const class Plat_Info* Chip);
    /* void */ Kernel(class wxXmlNode* Node);
    /* void */ ~Kernel(void);

    void Save(class wxXmlNode* Parent);
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
