/******************************************************************************
Filename    : chip_info.hpp
Author      : pry
Date        : 13/01/2023
License     : Proprietary; confidential.
Description : Chip information class header.
******************************************************************************/

/* Define ********************************************************************/
#ifdef __HDR_DEF__
#ifndef __CHIP_INFO_TYPE__
#define __CHIP_INFO_TYPE__
/*****************************************************************************/

/*****************************************************************************/
/* __CHIP_INFO_TYPE__ */
#endif
/* __HDR_DEF__ */
#endif

namespace RVM_CFG
{
#ifdef __HDR_DEF__
#ifndef __CHIP_INFO_DEF__
#define __CHIP_INFO_DEF__
/*****************************************************************************/

/*****************************************************************************/
/* __CHIP_INFO_DEF__ */
#endif
/* __HDR_DEF__ */
#endif
/* End Define ****************************************************************/

/* Classes *******************************************************************/
#ifdef __HDR_CLASS__
#ifndef __CHIP_INFO_CLASS__
#define __CHIP_INFO_CLASS__
/*****************************************************************************/
class Chip_Info
{
public:
    /* Path information */
    std::string Path;
    std::string Filename;
    std::string Fullpath;
    std::string Fulldir;

    /* Chip name */
    std::string Name;
    /* Version */
    std::string Version;
    /* Platform */
    std::string Platform;
    /* Compatible */
    std::vector<std::string> Compatible;
    /* Vendor */
    std::string Vendor;
    /* Vector */
    ptr_t Vector;
    /* Region */
    ptr_t Region;
    /* Iregion */
    ptr_t Iregion;
    /* Dregion */
    ptr_t Dregion;
    /* Coprocessor */
    std::vector<std::string> Coprocessor;
    /* Attribute */
    std::map<std::string,std::string> Attribute;
    /* Memory */
    std::vector<std::unique_ptr<class Mem_Info>> Memory;

    /* void */ Chip_Info(const std::string& Path);
    /* void */ ~Chip_Info(void);
};
/*****************************************************************************/
/* __CHIP_INFO_CLASS__ */
#endif
/* __HDR_CLASS__ */
#endif
}
/* End Classes ***************************************************************/

/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
