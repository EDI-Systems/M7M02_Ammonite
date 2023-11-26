/******************************************************************************
Filename    : compatible.hpp
Author      : pry
Date        : 16/07/2019
Licence     : LGPL v3+; see COPYING for details.
Description : The header for the chip class.
******************************************************************************/

/* Define ********************************************************************/
namespace RVM_GEN
{
#ifdef __HDR_DEF__
#ifndef __COMPATIBLE_DEF__
#define __COMPATIBLE_DEF__
/*****************************************************************************/

/*****************************************************************************/
/* __COMPATIBLE_DEF__ */
#endif
/* __HDR_DEF__ */
#endif
/* End Define ****************************************************************/

/* Classes *******************************************************************/
#ifdef __HDR_CLASS__
#ifndef __COMPATIBLE_CLASS__
#define __COMPATIBLE_CLASS__
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
/* __COMPATIBLE_CLASS__ */
#endif
/* __HDR_CLASS__ */
#endif
}
/* End Classes ***************************************************************/

/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
