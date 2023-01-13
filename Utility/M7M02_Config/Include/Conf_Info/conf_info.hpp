/******************************************************************************
Filename    : conf_info.hpp
Author      : pry
Date        : 13/01/2023
License     : Proprietary; confidential.
Description : Configuration class header.
******************************************************************************/

/* Defines *******************************************************************/
#ifdef __HDR_DEFS__
#ifndef __CONF_INFO_TYPES__
#define __CONF_INFO_TYPES__
/*****************************************************************************/

/*****************************************************************************/
/* __CONF_INFO_TYPES__ */
#endif
/* __HDR_DEFS__ */
#endif

namespace RVM_CFG
{
#ifdef __HDR_DEFS__
#ifndef __CONF_INFO_DEFS__
#define __CONF_INFO_DEFS__
/*****************************************************************************/
#define CONFIG_CHOICE       0
#define CONFIG_RANGE        1
/*****************************************************************************/
/* __CONF_INFO_DEFS__ */
#endif
/* __HDR_DEFS__ */
#endif
/* End Defines ***************************************************************/

/* Classes *******************************************************************/
#ifdef __HDR_CLASSES__
#ifndef __CONF_INFO_CLASSES__
#define __CONF_INFO_CLASSES__
/*****************************************************************************/
class Conf_Info
{
public:
    /* Config name */
    std::string Name;
    /* Macro name */
    std::string Macro;
    /* Config type */
    ptr_t Type;
    /* Range values */
    std::vector<std::string> Range;
    /* Default setting */
    std::string Default;

    /* void */ Conf_Info(class wxXmlNode* Node);
    /* void */ ~Conf_Info(void);
};
/*****************************************************************************/
/* __CONF_INFO_CLASSES__ */
#endif
/* __HDR_CLASSES__ */
#endif
}
/* End Classes ***************************************************************/

/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
