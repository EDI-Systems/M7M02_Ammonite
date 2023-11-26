/******************************************************************************
Filename    : conf_info.hpp
Author      : pry
Date        : 16/07/2019
Licence     : LGPL v3+; see COPYING for details.
Description : The header for the config class.
******************************************************************************/

/* Define ********************************************************************/
namespace RVM_GEN
{
#ifdef __HDR_DEF__
#ifndef __CONF_INFO_DEF__
#define __CONF_INFO_DEF__
/*****************************************************************************/
/* Config types */
#define CONFIG_RANGE                (0)
#define CONFIG_CHOICE               (1)
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
/* The config information */
class Conf_Info
{
public:
    /* Name*/
    std::string Name;
    /* Type of the option, either range or select */
    ptr_t Type;
    /* Macro of the option */
    std::string Macro;
    /* Range of the option */
    std::vector<std::string> Range;

    /* Whether it is configured in the project file */
    ptr_t Is_Configured;

    /* void */ Conf_Info(xml_node_t* Root);

    void Check(void);
    void Project_Config_Mark(const std::string & Name, const std::string& Value);
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
