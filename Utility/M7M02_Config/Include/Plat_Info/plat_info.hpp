/******************************************************************************
Filename    : plat_info.hpp
Author      : zjx
Date        : 14/01/2023
License     : Proprietary; confidential.
Description : Platform information class header.
******************************************************************************/

/* Define ********************************************************************/
#ifdef __HDR_DEF__
#ifndef __PLAT_INFO_TYPE__
#define __PLAT_INFO_TYPE__
/*****************************************************************************/

/*****************************************************************************/
/* __PLAT_INFO_TYPE__ */
#endif
/* __HDR_DEF__ */
#endif

namespace RVM_CFG
{
#ifdef __HDR_DEF__
#ifndef __PLAT_INFO_DEF__
#define __PLAT_INFO_DEF__
/*****************************************************************************/

/*****************************************************************************/
/* __PLAT_INFO_DEF__ */
#endif
/* __HDR_DEF__ */
#endif
/* End Define ****************************************************************/

/* Classes *******************************************************************/
#ifdef __HDR_CLASS__
#ifndef __PLAT_INFO_CLASS__
#define __PLAT_INFO_CLASS__
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
    /* List of Coprocessor */
    std::vector<std::string> Coprocessor;
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

    /* Check compatibility */
    ret_t Compat_Check(const std::string& Toolchain,
                       const std::string& Buildsystem);
    ret_t Compat_Check(const std::string& Toolchain,
                       const std::string& Buildsystem,
                       const std::string& Guest);

    /* Find compatibles for buildsystem */
    void Buildsystem_Workspace(std::vector<std::string>& Build_Avail);

    /* Find compatibles for native */
    void Toolchain_Native(std::vector<std::string>& Tool_Avail);
    void Buildsystem_Native(const std::string& Toolchain,
                            std::vector<std::string>& Build_Avail);

    /* Find compatibles for virtual */
    void Toolchain_Virtual(std::vector<std::string>& Tool_Avail);
    void Buildsystem_Virtual(const std::string& Toolchain,
                             std::vector<std::string>& Build_Avail);
    void Guest_Virtual(const std::string& Toolchain,
                       const std::string& Buildsystem,
                       std::vector<std::string>& Guest_Avail);
};
/*****************************************************************************/
/* __PLAT_INFO_CLASS__ */
#endif
/* __HDR_CLASS__ */
#endif
}
/* End Classes ***************************************************************/

/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
