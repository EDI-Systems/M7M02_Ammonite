/******************************************************************************
Filename    : default.hpp
Author      : pry
Date        : 13/01/2023
License     : Proprietary; confidential.
Description : Chip class header.
******************************************************************************/

/* Define ********************************************************************/
#ifdef __HDR_DEF__
#ifndef __DEFAULT_TYPE__
#define __DEFAULT_TYPE__
/*****************************************************************************/

/*****************************************************************************/
/* __DEFAULT_TYPE__ */
#endif
/* __HDR_DEF__ */
#endif

namespace RVM_CFG
{
#ifdef __HDR_DEF__
#ifndef __DEFAULT_DEF__
#define __DEFAULT_DEF__
/*****************************************************************************/

/*****************************************************************************/
/* __DEFAULT_DEF__ */
#endif
/* __HDR_DEF__ */
#endif
/* End Define ****************************************************************/

/* Classes *******************************************************************/
#ifdef __HDR_CLASS__
#ifndef __DEFAULT_CLASS__
#define __DEFAULT_CLASS__
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
/* __DEFAULT_CLASS__ */
#endif
/* __HDR_CLASS__ */
#endif
}
/* End Classes ***************************************************************/

/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
