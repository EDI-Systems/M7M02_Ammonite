/******************************************************************************
Filename    : compatible.hpp
Author      : pry
Date        : 13/01/2023
License     : Proprietary; confidential.
Description : Compatibility class header.
******************************************************************************/

/* Define ********************************************************************/
#ifdef __HDR_DEF__
#ifndef __COMPATIBLE_TYPE__
#define __COMPATIBLE_TYPE__
/*****************************************************************************/

/*****************************************************************************/
/* __COMPATIBLE_TYPE__ */
#endif
/* __HDR_DEF__ */
#endif

namespace RVM_CFG
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
/* __COMPATIBLE_CLASS__ */
#endif
/* __HDR_CLASS__ */
#endif
}
/* End Classes ***************************************************************/

/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
