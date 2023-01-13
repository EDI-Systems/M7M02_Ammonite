/******************************************************************************
Filename    : plat_info.hpp
Author      : zjx
Date        : 14/01/2023
License     : Proprietary; confidential.
Description : Platform information class header.
******************************************************************************/

/* Defines *******************************************************************/
#ifdef __HDR_DEFS__
#ifndef __PLAT_INFO_HPP_TYPES__
#define __PLAT_INFO_HPP_TYPES__
/*****************************************************************************/

/*****************************************************************************/
/* __PLAT_INFO_HPP_TYPES__ */
#endif
/* __HDR_DEFS__ */
#endif

namespace RVM_CFG
{
#ifdef __HDR_DEFS__
#ifndef __PLAT_INFO_HPP_DEFS__
#define __PLAT_INFO_HPP_DEFS__
/*****************************************************************************/

/*****************************************************************************/
/* __PLAT_INFO_HPP_DEFS__ */
#endif
/* __HDR_DEFS__ */
#endif
/* End Defines ***************************************************************/

/* Classes *******************************************************************/
#ifdef __HDR_CLASSES__
#ifndef __PLAT_INFO_HPP_CLASSES__
#define __PLAT_INFO_HPP_CLASSES__
/*****************************************************************************/
class Plat_Info
{
public:
    /* Path information */
    std::string Path;
    std::string Filename;
    std::string Fullpath;
    std::string Fulldir;

    /* The name of the project */
    std::string Name;
    /* The project version */
    std::string Version;
    /* Whether we assume all asserts to be correct */
    ptr_t Wordlength;
    /* Allowed build systems */
    std::vector<std::string> Buildsystem;
    /* Allowed toolchains */
    std::vector<std::string> Toolchain;
    /* Allowed guest OSes */
    std::vector<std::string> Guest;
    /* List of compatible collections */
    std::vector<std::unique_ptr<class Compatible>> Compatible;
    /* Default settings */
    std::unique_ptr<class Default> Default;
    /* List of configs */
    std::vector<std::unique_ptr<class Conf_Info>> Config;

    /* void */ Plat_Info(const std::string& Path);
    /* void */ ~Plat_Info(void);
};
/*****************************************************************************/
/* __PLAT_INFO_HPP_CLASSES__ */
#endif
/* __HDR_CLASSES__ */
#endif
}
/* End Classes ***************************************************************/

/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
