/******************************************************************************
Filename    : chip.hpp
Author      : pry
Date        : 13/01/2023
License     : Proprietary; confidential.
Description : Chip class header.
******************************************************************************/

/* Defines *******************************************************************/
#ifdef __HDR_DEFS__
#ifndef __CHIP_HPP_TYPES__
#define __CHIP_HPP_TYPES__
/*****************************************************************************/

/*****************************************************************************/
/* __CHIP_HPP_TYPES__ */
#endif
/* __HDR_DEFS__ */
#endif

namespace RVM_CFG
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
class Chip
{
public:
    /* Platform name */
    std::string Platform;
    /* Chip class */
    std::string Class;
    /* Exact chip name */
    std::string Name;
    /* The configuration option values */
    std::map<std::string,std::string> Config;

    /* void */ Chip(const std::string& Name, const class Chip_Info* Chip);
    /* void */ Chip(class wxXmlNode* Node);
    /* void */ ~Chip(void);

    void Save(class wxXmlNode* Parent);
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