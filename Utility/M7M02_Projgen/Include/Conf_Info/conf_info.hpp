/******************************************************************************
Filename    : conf_info.hpp
Author      : pry
Date        : 16/07/2019
Licence     : LGPL v3+; see COPYING for details.
Description : The header for the config class.
******************************************************************************/

/* Defines *******************************************************************/
namespace RVM_GEN
{
#ifdef __HDR_DEFS__
#ifndef __CONF_INFO_HPP_DEFS__
#define __CONF_INFO_HPP_DEFS__
/*****************************************************************************/
/* Config types */
#define CONFIG_RANGE                (0)
#define CONFIG_CHOICE               (1)
/*****************************************************************************/
/* __CONF_INFO_HPP_DEFS__ */
#endif
/* __HDR_DEFS__ */
#endif
/* End Defines ***************************************************************/

/* Classes *******************************************************************/
#ifdef __HDR_CLASSES__
#ifndef __CONF_INFO_HPP_CLASSES__
#define __CONF_INFO_HPP_CLASSES__
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
/* __CONF_INFO_HPP_CLASSES__ */
#endif
/* __HDR_CLASSES__ */
#endif
}
/* End Classes ***************************************************************/

/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
