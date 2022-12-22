/******************************************************************************
Filename    : kobj.hpp
Author      : pry
Date        : 16/07/2019
Licence     : LGPL v3+; see COPYING for details.
Description : The header for the kernel object class.
******************************************************************************/

/* Defines *******************************************************************/
namespace RVM_GEN
{
#ifdef __HDR_DEFS__
#ifndef __KOBJ_HPP_DEFS__
#define __KOBJ_HPP_DEFS__
/*****************************************************************************/

/*****************************************************************************/
/* __KOBJ_HPP_DEFS__ */
#endif
/* __HDR_DEFS__ */
#endif
/* End Defines ***************************************************************/

/* Classes *******************************************************************/
#ifdef __HDR_CLASSES__
#ifndef __KOBJ_HPP_CLASSES__
#define __KOBJ_HPP_CLASSES__
/*****************************************************************************/
/* Kernel object information */
class Kobj
{
public:
    /* Name of the kernel object */
    std::string Name;
    std::string Name_Upper;
    std::string Name_Lower;
    /* The process that it belongs to */
    class Process* Owner;
    /* The local capid */
    ptr_t Capid_Local;
    /* The global linear capid (not the real 2-level cap) */
    ptr_t Capid_Global;
    /* The macro denoting the global capid */
    std::string Macro_Local;
    /* The macro denoting the global capid */
    std::string Macro_Global;
    /* The macro denoting the global capid - for RME */
    std::string Macro_Kernel;

    /* void */ Kobj(class Process* Owner);
    virtual /* void */ ~Kobj(void)=0;
};
/*****************************************************************************/
/* __KOBJ_HPP_CLASSES__ */
#endif
/* __HDR_CLASSES__ */
#endif
}
/* End Classes ***************************************************************/

/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
