/******************************************************************************
Filename    : chip.hpp
Author      : pry
Date        : 13/01/2023
License     : Proprietary; confidential.
Description : Chip class header.
******************************************************************************/

/* Define ********************************************************************/
#ifdef __HDR_DEF__
#ifndef __CHIP_TYPE__
#define __CHIP_TYPE__
/*****************************************************************************/

/*****************************************************************************/
/* __CHIP_TYPE__ */
#endif
/* __HDR_DEF__ */
#endif

namespace RVM_CFG
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
/* __CHIP_CLASS__ */
#endif
/* __HDR_CLASS__ */
#endif
}
/* End Classes ***************************************************************/

/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
