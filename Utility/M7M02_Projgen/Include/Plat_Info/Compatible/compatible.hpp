/******************************************************************************
Filename    : compatible.hpp
Author      : pry
Date        : 16/07/2019
Licence     : LGPL v3+; see COPYING for details.
Description : The header for the chip class.
******************************************************************************/

/* Defines *******************************************************************/
namespace RVM_GEN
{
#ifdef __HDR_DEFS__
#ifndef __COMPATIBLE_HPP_DEFS__
#define __COMPATIBLE_HPP_DEFS__
/*****************************************************************************/

/*****************************************************************************/
/* __COMPATIBLE_HPP_DEFS__ */
#endif
/* __HDR_DEFS__ */
#endif
/* End Defines ***************************************************************/

/* Classes *******************************************************************/
#ifdef __HDR_CLASSES__
#ifndef __COMPATIBLE_HPP_CLASSES__
#define __COMPATIBLE_HPP_CLASSES__
/*****************************************************************************/
/* Compatibility information */
class Compatible
{
public:
    std::string Buildsystem;
    std::string Toolchain;
    std::string Guest;

    /* void */ Compatible(xml_node_t* Root);
};
/*****************************************************************************/
/* __COMPATIBLE_HPP_CLASSES__ */
#endif
/* __HDR_CLASSES__ */
#endif
}
/* End Classes ***************************************************************/

/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
