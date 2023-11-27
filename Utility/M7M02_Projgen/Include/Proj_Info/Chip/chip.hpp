/******************************************************************************
Filename    : chip.hpp
Author      : pry
Date        : 16/07/2019
Licence     : LGPL v3+; see COPYING for details.
Description : The header for the chip configuration class.
******************************************************************************/

/* Define ********************************************************************/
namespace RVM_GEN
{
#ifdef __HDR_DEF__
#ifndef __CHIP_DEF__
#define __CHIP_DEF__
/*****************************************************************************/

/*****************************************************************************/
/* __CHIP_DEF__ */
#endif
/* __HDR_DEF__ */
#endif
/* End Define ****************************************************************/

/* Classes *******************************************************************/
#ifdef __HDR_CLASS__
#ifndef __CHIP_CLASS__
#define __CHIP_CLASS__
/*****************************************************************************/
/* Chip information */
class Chip
{
public:
    /* Platform */
    std::string Platform;
    std::string Platform_Lower;
    /* Class */
    std::string Class;
    std::string Class_Lower;
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
/* __CHIP_CLASS__ */
#endif
/* __HDR_CLASS__ */
#endif
}
/* End Classes ***************************************************************/

/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
