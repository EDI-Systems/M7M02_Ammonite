/******************************************************************************
Filename    : captbl.hpp
Author      : pry
Date        : 16/07/2019
Licence     : LGPL v3+; see COPYING for details.
Description : The header for the capability table class.
******************************************************************************/

/* Define ********************************************************************/
namespace RVM_GEN
{
#ifdef __HDR_DEF__
#ifndef __CAPTBL_DEF__
#define __CAPTBL_DEF__
/*****************************************************************************/

/*****************************************************************************/
/* __CAPTBL_DEF__ */
#endif
/* __HDR_DEF__ */
#endif
/* End Define ****************************************************************/

/* Classes *******************************************************************/
#ifdef __HDR_CLASS__
#ifndef __CAPTBL_CLASS__
#define __CAPTBL_CLASS__
/*****************************************************************************/
/* Capability table information */
class Captbl:public Kobj
{
public:
    /* The frontier */
    ptr_t Front;
    /* The ultimate size (number of entries) */
    ptr_t Size;

    /* void */ Captbl(ptr_t Front, ptr_t Size, class Process* Owner);
};
/*****************************************************************************/
/* __CAPTBL_CLASS__ */
#endif
/* __HDR_CLASS__ */
#endif
}
/* End Classes ***************************************************************/

/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
