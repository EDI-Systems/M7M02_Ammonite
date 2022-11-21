/******************************************************************************
Filename    : config.hpp
Author      : pry
Date        : 16/07/2019
Licence     : LGPL v3+; see COPYING for details.
Description : The header for the config class.
******************************************************************************/

/* Defines *******************************************************************/
namespace RVM_GEN
{
#ifdef __HDR_DEFS__
#ifndef __CONFIG_HPP_DEFS__
#define __CONFIG_HPP_DEFS__
/*****************************************************************************/
/* Config types */
#define CONFIG_RANGE                (0)
#define CONFIG_SELECT               (1)
/*****************************************************************************/
/* __CONFIG_HPP_DEFS__ */
#endif
/* __HDR_DEFS__ */
#endif
/* End Defines ***************************************************************/

/* Classes *******************************************************************/
#ifdef __HDR_CLASSES__
#ifndef __CONFIG_HPP_CLASSES__
#define __CONFIG_HPP_CLASSES__
/*****************************************************************************/
/* The config information */
class Config
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

    /* void */ Config(xml_node_t* Root);
};
/*****************************************************************************/
/* __CONFIG_HPP_CLASSES__ */
#endif
/* __HDR_CLASSES__ */
#endif
}
/* End Classes ***************************************************************/

/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
