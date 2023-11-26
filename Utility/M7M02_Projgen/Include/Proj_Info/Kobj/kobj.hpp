/******************************************************************************
Filename    : kobj.hpp
Author      : pry
Date        : 16/07/2019
Licence     : LGPL v3+; see COPYING for details.
Description : The header for the kernel object class.
******************************************************************************/

/* Define ********************************************************************/
namespace RVM_GEN
{
#ifdef __HDR_DEF__
#ifndef __KOBJ_DEF__
#define __KOBJ_DEF__
/*****************************************************************************/

/*****************************************************************************/
/* __KOBJ_DEF__ */
#endif
/* __HDR_DEF__ */
#endif
/* End Define ****************************************************************/

/* Classes *******************************************************************/
#ifdef __HDR_CLASS__
#ifndef __KOBJ_CLASS__
#define __KOBJ_CLASS__
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
    ptr_t Cid_Local;
    /* The global linear capid (not the real 2-level cap) */
    ptr_t Cid_Global;
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
/* __KOBJ_CLASS__ */
#endif
/* __HDR_CLASS__ */
#endif
}
/* End Classes ***************************************************************/

/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
