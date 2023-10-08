/******************************************************************************
Filename    : chip.hpp
Author      : pry
Date        : 16/07/2019
Licence     : LGPL v3+; see COPYING for details.
Description : The header for the chip configuration class.
******************************************************************************/

/* Defines *******************************************************************/
namespace RVM_GEN
{
#ifdef __HDR_DEFS__
#ifndef __CHIP_HPP_DEFS__
#define __CHIP_HPP_DEFS__
/*****************************************************************************/

/*****************************************************************************/
/* __CHIP_HPP_DEFS__ */
#endif
/* __HDR_DEFS__ */
#endif
/* End Defines ***************************************************************/

/* Classes *******************************************************************/
#ifdef __HDR_CLASSES__
#ifndef __CHIP_HPP_CLASSES__
#define __CHIP_HPP_CLASSES__
/*****************************************************************************/
/* Chip information */
class Chip
{
public:
    /* Platform */
    std::string Platform;
    /* Class */
    std::string Class;
    /* The vendor */
    std::string Name;
    /* Selected coprocessors in this project */
    std::vector<std::string> Coprocessor;
    std::set<std::string> Coprocessor_Set;
    /* The configuration option values */
    std::map<std::string,std::string> Config;

    /* void */ Chip(xml_node_t* Root);

    void Check(void);
};
/*****************************************************************************/
/* __CHIP_HPP_CLASSES__ */
#endif
/* __HDR_CLASSES__ */
#endif
}
/* End Classes ***************************************************************/

/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
