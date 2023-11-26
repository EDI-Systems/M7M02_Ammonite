/******************************************************************************
Filename    : chip_info.hpp
Author      : pry
Date        : 16/07/2019
Licence     : LGPL v3+; see COPYING for details.
Description : The header for the chip class.
******************************************************************************/

/* Define ********************************************************************/
namespace RVM_GEN
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
/* Chip information */
class Chip_Info
{
public:
    /* The name of the chip class */
    std::string Name;
    std::string Name_Upper;
    std::string Name_Lower;
    /* Version info */
    std::string Version;
    /* The platform */
    std::string Platform;
    /* Compatible chip list */
    std::vector<std::string> Compatible;
    std::set<std::string> Compatible_Set;
    /* The vendor */
    std::string Vendor;
    /* The number of interrupt vectors */
    ptr_t Vector;
    /* The number of dual-purpose MPU regions */
    ptr_t Region;
    /* The number of instruction-dedicated MPU regions */
    ptr_t Iregion;
    /* The number of data-dedicated MPU regions */
    ptr_t Dregion;
    /* Coprocessor hardwares */
    std::vector<std::string> Coprocessor;
    std::set<std::string> Coprocessor_Set;

    /* The platform-specific attributes to be passed to the platform-specific generator */
    std::map<std::string,std::string> Attribute;
    /* Memory information */
    std::vector<std::unique_ptr<class Mem_Info>> Memory;
    /* Chip-specific configuration information */
    std::vector<std::unique_ptr<class Conf_Info>> Config;
    std::map<std::string,class Conf_Info*> Config_Map;
    std::map<std::string,class Conf_Info*> Config_Macro_Map;

    /* void */ Chip_Info(xml_node_t* Root);

    void Check(void);

    void Project_Config_Mark_Check(void);
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
