/******************************************************************************
Filename    : build_gen.hpp
Author      : pry
Date        : 16/07/2019
Licence     : LGPL v3+; see COPYING for details.
Description : The header for the buildsystem generator class.
******************************************************************************/

/* Defines *******************************************************************/
namespace RVM_GEN
{
#ifdef __HDR_DEFS__
#ifndef __BUILD_GEN_HPP_DEFS__
#define __BUILD_GEN_HPP_DEFS__
/*****************************************************************************/

/*****************************************************************************/
/* __BUILD_GEN_HPP_DEFS__ */
#endif
/* __HDR_DEFS__ */
#endif
/* End Defines ***************************************************************/

/* Classes *******************************************************************/
#ifdef __HDR_CLASSES__
#ifndef __BUILD_GEN_HPP_CLASSES__
#define __BUILD_GEN_HPP_CLASSES__
/*****************************************************************************/
/* Buildsystem generator information */
class Build_Gen
{
public:
    /* Name */
    std::string Name;

    /* void */ Build_Gen(const std::string& Name);
    virtual /* void */ ~Build_Gen(void){};
};
/*****************************************************************************/
/* __BUILD_GEN_HPP_CLASSES__ */
#endif
/* __HDR_CLASSES__ */
#endif
}
/* End Classes ***************************************************************/

/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/