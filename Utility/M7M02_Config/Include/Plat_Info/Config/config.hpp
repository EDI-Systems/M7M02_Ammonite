/******************************************************************************
Filename    : conf_info.hpp
Author      : pry
Date        : 13/01/2023
License     : Proprietary; confidential.
Description : Configuration class header.
******************************************************************************/

/* Define ********************************************************************/
#ifdef __HDR_DEF__
#ifndef __CONF_INFO_TYPES__
#define __CONF_INFO_TYPES__
/*****************************************************************************/

/*****************************************************************************/
/* __CONF_INFO_TYPES__ */
#endif
/* __HDR_DEF__ */
#endif

namespace RVM_CFG
{
#ifdef __HDR_DEF__
#ifndef __CONF_INFO_DEF__
#define __CONF_INFO_DEF__
/*****************************************************************************/
#define CONFIG_CHOICE       0
#define CONFIG_RANGE        1
/*****************************************************************************/
/* __CONF_INFO_DEF__ */
#endif
/* __HDR_DEF__ */
#endif
/* End Define ****************************************************************/

/* Classes *******************************************************************/
#ifdef __HDR_CLASS__
#ifndef __CONF_INFO_CLASS__
#define __CONF_INFO_CLASS__
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
/* __CONF_INFO_CLASS__ */
#endif
/* __HDR_CLASS__ */
#endif
}
/* End Classes ***************************************************************/

/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
