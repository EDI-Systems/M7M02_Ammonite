/******************************************************************************
Filename    : compatible.hpp
Author      : pry
Date        : 13/01/2023
License     : Proprietary; confidential.
Description : Compatibility class header.
******************************************************************************/

/* Defines *******************************************************************/
#ifdef __HDR_DEFS__
#ifndef __COMPATIBLE_HPP_TYPES__
#define __COMPATIBLE_HPP_TYPES__
/*****************************************************************************/

/*****************************************************************************/
/* __COMPATIBLE_HPP_TYPES__ */
#endif
/* __HDR_DEFS__ */
#endif

namespace RVM_CFG
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
class Compatible
{
public:
    /* Default build options */
    std::string Buildsystem;
    std::string Toolchain;
    std::string Guest;

    /* void */ Compatible(class wxXmlNode* Node);
    /* void */ ~Compatible(void);
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
