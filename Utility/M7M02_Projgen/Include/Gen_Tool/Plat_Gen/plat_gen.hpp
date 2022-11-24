/******************************************************************************
Filename    : plat_gen.hpp
Author      : pry
Date        : 08/04/2017
Licence     : LGPL v3+; see COPYING for details.
Description : The header of the platform generator.
******************************************************************************/

/* Defines *******************************************************************/
namespace RVM_GEN
{
#ifdef __HDR_DEFS__
#ifndef __PLAT_GEN_HPP_DEFS__
#define __PLAT_GEN_HPP_DEFS__
/*****************************************************************************/

/*****************************************************************************/
/* __PLAT_GEN_HPP_DEFS__ */
#endif
/* __HDR_DEFS__ */
#endif
/* End Defines ***************************************************************/

/* Classes *******************************************************************/
#ifdef __HDR_CLASSES__
#ifndef __PLAT_GEN_HPP_CLASSES__
#define __PLAT_GEN_HPP_CLASSES__
/*****************************************************************************/
/* Platform generator */
class Plat_Gen
{
public:
    /* Platform name */
    std::string Name;

    /* void */ Plat_Gen(const std::string& Name);
    virtual /* void */ ~Plat_Gen(void){};

    virtual void Mem_Align(class Mem_Info* Mem)=0;
};
/*****************************************************************************/
/* __PLAT_GEN_HPP_CLASSES__ */
#endif
/* __HDR_CLASSES__ */
#endif
}
/* End Classes ***************************************************************/

/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
