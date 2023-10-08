/******************************************************************************
Filename    : plat_info.hpp
Author      : pry
Date        : 16/07/2019
Licence     : LGPL v3+; see COPYING for details.
Description : The header for the chip class.
******************************************************************************/

/* Defines *******************************************************************/
namespace RVM_GEN
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
/* Platform information */
class Plat_Info
{
public:
    /* The platform */
    std::string Name;
    std::string Name_Upper;
    std::string Name_Lower;
    /* Version info */
    std::string Version;
    /* Processor word length */
    ptr_t Wordlength;
    /* List of possible coprocessors */
    std::vector<std::string> Coprocessor;
    std::set<std::string> Coprocessor_Set;
    /* Capability table maximum capacity (for a single layer) */
    ptr_t Captbl_Max;
    /* Kernel function maximum number (last one not included) */
    ptr_t Kfunc_Max;
    /* Compatible guest list */
    std::vector<std::string> Guest;
    std::set<std::string> Guest_Set;
    /* Compatible buildsystem list */
    std::vector<std::string> Buildsystem;
    std::set<std::string> Buildsystem_Set;
    /* Compatible buildsystem list */
    std::vector<std::string> Toolchain;
    std::set<std::string> Toolchain_Set;

    /* Compatibility information */
    std::vector<std::unique_ptr<class Compatible>> Compatible;

    /* Platform-specific configuration information */
    std::vector<std::unique_ptr<class Conf_Info>> Config;
    std::map<std::string,class Conf_Info*> Config_Map;
    std::map<std::string,class Conf_Info*> Config_Macro_Map;

    /* void */ Plat_Info(xml_node_t* Root);

    void Check(void);

    void Compatible_Check(const std::string& Name,
                          const std::string& Buildsystem,
                          const std::string& Toolchain,
                          const std::string& Guest);
    void Project_Config_Mark_Check(void);
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
