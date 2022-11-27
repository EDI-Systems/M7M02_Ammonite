/******************************************************************************
Filename    : captbl.hpp
Author      : pry
Date        : 16/07/2019
Licence     : LGPL v3+; see COPYING for details.
Description : The header for the capability table class.
******************************************************************************/

/* Defines *******************************************************************/
namespace RVM_GEN
{
#ifdef __HDR_DEFS__
#ifndef __CAPTBL_HPP_DEFS__
#define __CAPTBL_HPP_DEFS__
/*****************************************************************************/

/*****************************************************************************/
/* __CAPTBL_HPP_DEFS__ */
#endif
/* __HDR_DEFS__ */
#endif
/* End Defines ***************************************************************/

/* Classes *******************************************************************/
#ifdef __HDR_CLASSES__
#ifndef __CAPTBL_HPP_CLASSES__
#define __CAPTBL_HPP_CLASSES__
/*****************************************************************************/
/* Capability table information */
class Captbl:public Kobj
{
public:
    /* The frontier */
    ptr_t Front;
    /* The ultimate size */
    ptr_t Size;

    /* void */ Captbl(ptr_t Front, ptr_t Size, class Process* Owner);
};
/*****************************************************************************/
/* __CAPTBL_HPP_CLASSES__ */
#endif
/* __HDR_CLASSES__ */
#endif
}
/* End Classes ***************************************************************/

/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
