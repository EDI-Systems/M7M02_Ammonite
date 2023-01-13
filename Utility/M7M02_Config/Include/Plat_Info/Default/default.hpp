/******************************************************************************
Filename    : default.hpp
Author      : pry
Date        : 13/01/2023
License     : Proprietary; confidential.
Description : Chip class header.
******************************************************************************/

/* Defines *******************************************************************/
#ifdef __HDR_DEFS__
#ifndef __DEFAULT_HPP_TYPES__
#define __DEFAULT_HPP_TYPES__
/*****************************************************************************/

/*****************************************************************************/
/* __DEFAULT_HPP_TYPES__ */
#endif
/* __HDR_DEFS__ */
#endif

namespace RVM_CFG
{
#ifdef __HDR_DEFS__
#ifndef __DEFAULT_HPP_DEFS__
#define __DEFAULT_HPP_DEFS__
/*****************************************************************************/

/*****************************************************************************/
/* __DEFAULT_HPP_DEFS__ */
#endif
/* __HDR_DEFS__ */
#endif
/* End Defines ***************************************************************/

/* Classes *******************************************************************/
#ifdef __HDR_CLASSES__
#ifndef __DEFAULT_HPP_CLASSES__
#define __DEFAULT_HPP_CLASSES__
/*****************************************************************************/
class Default
{
public:
    /* Default build options */
    std::string Buildsystem;
    std::string Toolchain;
    ptr_t Optimization;

    /* Kernel size options */
    ptr_t Kernel_Code_Size;
    ptr_t Kernel_Data_Size;
    ptr_t Kernel_Stack_Size;

    /* Monitor size options */
    ptr_t Monitor_Code_Size;
    ptr_t Monitor_Data_Size;
    ptr_t Init_Stack_Size;
    ptr_t Sftd_Stack_Size;
    ptr_t Vctd_Stack_Size;
    ptr_t Timd_Stack_Size;
    ptr_t Hypd_Stack_Size;

    /* void */ Default(class wxXmlNode* Node);
    /* void */ ~Default(void);
};
/*****************************************************************************/
/* __DEFAULT_HPP_CLASSES__ */
#endif
/* __HDR_CLASSES__ */
#endif
}
/* End Classes ***************************************************************/

/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
